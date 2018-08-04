#include "stdafx.h"
#include "ThreadActualizarArbol.h"
#include "DStringUtils.h"


#define MUTEX_ACTUALIZARARBOL L"Mutex_ActualizarArbol"

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
	_Thread = CreateThread(NULL, 0, (unsigned long(__stdcall *)(void *))this->_ThreadActualizar, (void *)this, 0, NULL);
	_Mutex.lock();
	_Cancelar = FALSE;
	_Mutex.unlock();
	if (_Thread == NULL)
		return FALSE;
	SetThreadPriority(_Thread, 0);

	Debug_Escribir(L"ThreadActualizarArbol::Iniciado\n");
	return TRUE;
}

void ThreadActualizarArbol::Terminar(void) {
	CloseHandle(_Thread);
	_Thread = NULL;
	_BD.Terminar();

}

void ThreadActualizarArbol::Cancelar(void) {
	_Mutex.lock();
	_Cancelar = TRUE;
	_Mutex.unlock();
}


unsigned long ThreadActualizarArbol::_ThreadActualizar(void *pThis) {
	ThreadActualizarArbol *This = reinterpret_cast<ThreadActualizarArbol *>(pThis);
	// Inicio la BD en este thread
	This->_BD.Iniciar();

	UINT TotalArchivos = 0;
	// Fase 1 : enumerar arxius i directoris
	This->_BD.Consulta(L"BEGIN TRANSACTION");
	BDRaiz *R = NULL;
	for (size_t i = 0; i < This->_BD.TotalRaices(); i++) {
		R = This->_BD.Raiz(i);
		TotalArchivos += This->_EscanearDirectorio(R->Path, R);
	}


	This->_BD.Consulta(L"COMMIT TRANSACTION");
	This->_BD.Terminar();

	PostMessage(This->_VentanaPlayer, WM_TBA_TERMINADO, 0, static_cast<LPARAM>(TotalArchivos));

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
	BOOL Salir = FALSE;
	_Mutex.lock();
	if (_Cancelar == TRUE) Salir = TRUE;
	_Mutex.unlock();

	if (Salir == TRUE) return 0;

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
		SendMessage(_VentanaPlayer, WM_TBA_AGREGARRAIZ, 0, reinterpret_cast<LPARAM>(StringDirectorio));
	}
	else {
		// SOLO Si es un directorio hijo de la raiz (evito el resto de subdirectorios)
		if ((DString_ContarCaracter(Raiz->Path, L'\\') + 1) == DString_ContarCaracter(Path, L'\\')) { 
			StringDirectorio = new std::wstring(nPath);
			// Informo al hilo principal que se ha agregado un directorio
			SendMessage(_VentanaPlayer, WM_TBA_AGREGARDIR, 0, reinterpret_cast<LPARAM>(StringDirectorio));
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
				_BD.AnalizarMedio(Path, Medio, FindInfoPoint.nFileSizeLow);
			}
		}
	}
	FindClose(hFind);

	return TotalArchivosEscaneados;
}


const BOOL ThreadActualizarArbol::_EsNombreValido(const wchar_t *nNombre) {
	if (nNombre[0] == TEXT('.') && nNombre[1] == 0)								return FALSE;
	if (nNombre[0] == TEXT('.') && nNombre[1] == TEXT('.') && nNombre[2] == 0)	return FALSE;
	return TRUE;
}
