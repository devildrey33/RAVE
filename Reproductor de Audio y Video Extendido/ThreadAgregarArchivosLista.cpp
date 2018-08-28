#include "stdafx.h"
#include "ThreadAgregarArchivosLista.h"


ThreadAgregarArchivosLista::ThreadAgregarArchivosLista() : _Cancelar(FALSE), _VentanaPlayer(NULL), _Thread(NULL) {

}


ThreadAgregarArchivosLista::~ThreadAgregarArchivosLista() {
}



const BOOL ThreadAgregarArchivosLista::Iniciar(HWND nhWndDest, std::vector<std::wstring> &Paths) {

	// Se está ejecutando 
	if (_Thread != NULL) return FALSE;

	_Paths = Paths;

	// Asigno Ventana para los mensajes
	_VentanaPlayer = nhWndDest;

	// Iniciamos el Thread
	_Thread = CreateThread(NULL, 0, (unsigned long(__stdcall *)(void *))this->_ThreadAgregarArchivosLista, (void *)this, 0, NULL);
	
	Cancelar(FALSE);

	if (_Thread == NULL)
		return FALSE;
	SetThreadPriority(_Thread, 0);

	Debug_Escribir(L"ThreadAgregarArchivosLista::Iniciado\n");
	return TRUE;
}

void ThreadAgregarArchivosLista::Terminar(void) {
	CloseHandle(_Thread);
	_BD.Terminar();
	_Thread = NULL;
}

void ThreadAgregarArchivosLista::Cancelar(const BOOL nCancelar) {
	_Mutex.lock();
	_Cancelar = nCancelar;
	_Mutex.unlock();
}


unsigned long ThreadAgregarArchivosLista::_ThreadAgregarArchivosLista(void *pThis) {
	
	ThreadAgregarArchivosLista *This = reinterpret_cast<ThreadAgregarArchivosLista *>(pThis);
	// Inicio la BD en este thread
	This->_BD.Iniciar();

	UINT TotalArchivos = 0;
	// Fase 1 : enumerar arxius i directoris
	This->_BD.Consulta(L"BEGIN TRANSACTION");
	DWORD Attr = NULL;
	BDMedio *Medio = NULL;
	for (size_t i = 0; i < This->_Paths.size(); i++) {
		Attr = GetFileAttributes(This->_Paths[i].c_str());
		// Directorio
		if (Attr & FILE_ATTRIBUTE_DIRECTORY) {
			TotalArchivos += This->_EscanearDirectorio(This->_Paths[i]);
		}
		// Archivo
		else {
			TotalArchivos++;
			Medio = new BDMedio;
			This->_BD.AnalizarMedio(This->_Paths[i], *Medio);
			SendMessage(This->_VentanaPlayer, WM_TAAL_AGREGARMEDIO, reinterpret_cast<LPARAM>(Medio), 0);
		}
	}

	This->_BD.Consulta(L"COMMIT TRANSACTION");
	This->_BD.Terminar();

	SendMessage(This->_VentanaPlayer, WM_TAAL_TERMINADO, static_cast<WPARAM>(TotalArchivos), 0);

	return 0;
}



const UINT ThreadAgregarArchivosLista::_EscanearDirectorio(std::wstring &nPath) {
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
	/*std::wstring *StringDirectorio;
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
	}*/

	BDMedio *Medio = NULL;
	hFind = FindFirstFile(Path.c_str(), &FindInfoPoint);


	while (FindNextFile(hFind, &FindInfoPoint) != 0) {
		Path = nPath;
		if (Path[Path.size() - 1] != TEXT('\\')) Path += TEXT('\\');

		if (_EsNombreValido(FindInfoPoint.cFileName)) {
			Path += FindInfoPoint.cFileName;
			// Si es un directorio y no empieza por ".", llamo a esta función recursiva de nuevo
			//			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && FindInfoPoint.cFileName[0] != TEXT('.')) {
			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				TotalArchivosEscaneados += _EscanearDirectorio(Path);
			}
			else {
				TotalArchivosEscaneados++;
				Medio = new BDMedio;
				if (_BD.AnalizarMedio(Path, *Medio, FindInfoPoint.nFileSizeLow) != FALSE) {
					SendMessage(_VentanaPlayer, WM_TAAL_AGREGARMEDIO, reinterpret_cast<WPARAM>(Medio), 0);
				}
			}
		}
	}
	FindClose(hFind);

	return TotalArchivosEscaneados;
}


const BOOL ThreadAgregarArchivosLista::_EsNombreValido(const wchar_t *nNombre) {
	if (nNombre[0] == TEXT('.') && nNombre[1] == 0)								return FALSE;
	if (nNombre[0] == TEXT('.') && nNombre[1] == TEXT('.') && nNombre[2] == 0)	return FALSE;
	return TRUE;
}
