#include "stdafx.h"
#include "ThreadActualizarArbol.h"
#include "DStringUtils.h"

//#define MUTEX_ACTUALIZARARBOL L"Mutex_ActualizarArbol"

ThreadActualizarArbol::ThreadActualizarArbol() : _Cancelar(FALSE), _VentanaPlayer(NULL), _Thread(NULL) {
}


ThreadActualizarArbol::~ThreadActualizarArbol() {
}


const BOOL ThreadActualizarArbol::Iniciar(HWND nhWndDest) {

	// Se está ejecutando 
	if (_Thread != NULL) return FALSE;

	// Asigno Ventana para los mensajes
	_VentanaPlayer = nhWndDest;

	// Iniciamos el Thread
	_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->_ThreadActualizar, (void *)this, 0, NULL);
	
	Cancelar(FALSE);

	if (_Thread == NULL)
		return FALSE;
	SetThreadPriority(_Thread, 0);

	Debug_Escribir(L"ThreadActualizarArbol::Iniciado\n");
	return TRUE;
}

void ThreadActualizarArbol::Terminar(void) {
	CloseHandle(_Thread);
	_Thread = NULL;
}

void ThreadActualizarArbol::Cancelar(const BOOL nCancelar) {
	_Mutex.lock();
	_Cancelar = nCancelar;
	_Mutex.unlock();
}

const BOOL ThreadActualizarArbol::Cancelar(void) {
	_Mutex.lock();
	BOOL Ret = _Cancelar;
	_Mutex.unlock();
	return Ret;
}

HANDLE ThreadActualizarArbol::Thread(void) {
	return _Thread;
}



unsigned long ThreadActualizarArbol::_ThreadActualizar(void *pThis) {
	ThreadActualizarArbol *This = reinterpret_cast<ThreadActualizarArbol *>(pThis);
	// Inicio la BD en este thread
	This->_BD.Iniciar();

	UINT	TotalArchivos = 0;
	size_t	i             = 0;
	// Fase 1 : enumerar archivos y directorios
	This->_BD.Consulta(L"BEGIN TRANSACTION");
	BDRaiz *R = NULL;
	for (i = 0; i < This->_BD.TotalRaices(); i++) {
		R = This->_BD.Raiz(i);
		if (GetFileAttributes(R->Path.c_str()) != INVALID_FILE_ATTRIBUTES) {
			TotalArchivos += This->_EscanearDirectorio(R->Path, R);
		}
	}
	// Termino la transaction para actualizar los datos básicos
	This->_BD.Consulta(L"COMMIT TRANSACTION");
	This->_BD.Terminar();

	if (This->Cancelar() == TRUE)	PostMessage(This->_VentanaPlayer, WM_TBA_CANCELADO, 0, static_cast<LPARAM>(TotalArchivos));
	else							PostMessage(This->_VentanaPlayer, WM_TBA_TERMINADO, 0, static_cast<LPARAM>(TotalArchivos));
	

	return 0;
}



const UINT ThreadActualizarArbol::_EscanearDirectorio(std::wstring &nPath, BDRaiz *Raiz) {
	WIN32_FIND_DATA		FindInfoPoint;
	HANDLE				hFind = NULL;
	UINT        		TotalArchivosEscaneados = 0;
	static TCHAR		TmpPath[MAX_PATH + 1];
	BOOL                HayQueCancelar = FALSE;

	// Compruebo que existe fisicamente en el disco
	if (GetFileAttributes(nPath.c_str()) == INVALID_FILE_ATTRIBUTES)
		return 0;

	// Compruebo si se ha cancelado la actualización
	if (Cancelar() == TRUE) return 0;

	std::wstring		Path;
	Path.reserve(MAX_PATH);
	Path = nPath;
	//	DWL::DSplit Split(Path, TEXT('\\'));
	if (Path[Path.size() - 1] != TEXT('\\'))	Path += TEXT("\\*.*");
	else                                        Path += TEXT("*.*");

	// Puntero que pasaremos de este thread al thread principal
	std::wstring *StringDirectorio;
	if (Raiz->Path == nPath) {
		StringDirectorio = new std::wstring(nPath);
		// Informo al hilo principal que se ha agregado una raíz
		PostMessage(_VentanaPlayer, WM_TBA_AGREGARRAIZ, 0, reinterpret_cast<LPARAM>(StringDirectorio));
	}
	else {
		// SOLO Si es un directorio hijo de la raiz (evito el resto de subdirectorios)
		if ((Strings::ContarCaracter(Raiz->Path, L'\\') + 1) == Strings::ContarCaracter(Path, L'\\')) { 
			StringDirectorio = new std::wstring(nPath);
			// Informo al hilo principal que se ha agregado un directorio
			PostMessage(_VentanaPlayer, WM_TBA_AGREGARDIR, 0, reinterpret_cast<LPARAM>(StringDirectorio));
		}
	}

	BDMedio Medio;
	hFind = FindFirstFile(Path.c_str(), &FindInfoPoint);


	while (FindNextFile(hFind, &FindInfoPoint) != 0) {
		Path = nPath;
		if (Path[Path.size() - 1] != TEXT('\\')) Path += TEXT('\\');

		if (_EsNombreValido(FindInfoPoint.cFileName)) {
			Path += FindInfoPoint.cFileName;
			// Si es un directorio y no empieza por ".", llamo a esta función recursiva de nuevo
			//			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && FindInfoPoint.cFileName[0] != TEXT('.')) {
			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				TotalArchivosEscaneados += _EscanearDirectorio(Path, Raiz);
			}
			else {
				TotalArchivosEscaneados++;
				// Agrego el medio a la BD
				if (_BD.AnalizarMedio(Path, Medio, FindInfoPoint.nFileSizeLow) == TRUE) {
					// Si no existia el medio agregado, añado el medio a la lista de medios a parsear
//					_PorParsear.push_back(Path);
				}
			}
		}
	}
	FindClose(hFind);

	return TotalArchivosEscaneados;
}

// Función que determina si el nombre de archivo es válido. (Basicamente mira que el archivo no sea "." o "..")
const BOOL ThreadActualizarArbol::_EsNombreValido(const wchar_t *nNombre) {
	if (nNombre[0] == TEXT('.') && nNombre[1] == 0)								return FALSE;
	if (nNombre[0] == TEXT('.') && nNombre[1] == TEXT('.') && nNombre[2] == 0)	return FALSE;
	return TRUE;
}
