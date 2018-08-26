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
	_Thread = CreateThread(NULL, 0, (unsigned long(__stdcall *)(void *))this->_ThreadActualizar, (void *)this, 0, NULL);
	
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
	_BD.Terminar();

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
		TotalArchivos += This->_EscanearDirectorio(R->Path, R);
	}
	// Termino la transaction para actualizar los datos básicos
	This->_BD.Consulta(L"COMMIT TRANSACTION");

	// Fase 2 : Iniciar la VLC en este thread
	const char * const Argumentos[] = {	"-v", "--vout=vdummy" };
	libvlc_instance_t *VLC = libvlc_new(2, Argumentos);

	// Inicio la transaccion por segunda vez
	This->_BD.Consulta(L"BEGIN TRANSACTION");
	// Fase 3 : analizar nuevos medios añadidos a la BD
	for (i = 0; i < This->_PorParsear.size(); i++) {
		if (This->Cancelar() == TRUE) {
			break;
		}
		
		This->_Parsear(VLC, This->_PorParsear[i]);
		SendMessage(This->_VentanaPlayer, WM_TBA_TOTALMEDIOS, i, static_cast<LPARAM>(This->_PorParsear.size()));
	}

	// Terminado
	libvlc_release(VLC);
	This->_BD.Consulta(L"COMMIT TRANSACTION");
	This->_BD.Terminar();

	PostMessage(This->_VentanaPlayer, WM_TBA_TERMINADO, 0, static_cast<LPARAM>(TotalArchivos));

	return 0;
}


std::wstring ThreadActualizarArbol::_ObtenerMeta(libvlc_media_t *Media, libvlc_meta_t Tipo) {
	std::wstring Ret;
	char *Txt = libvlc_media_get_meta(Media, Tipo);
	if (Txt != NULL) {
		std::wstring TmpTxt = DString_ToStr(Txt);
		RaveBD::FiltroNombre(TmpTxt, Ret);
	}
	return Ret;
}

void ThreadActualizarArbol::_Parsear(libvlc_instance_t *VLC, std::wstring &Path) {

	// Hay que convertir el path a UTF8 para que funcione en el VLC...
	char	Destino[MAX_PATH];
	size_t  TamnTexto = wcslen(Path.c_str()) + 1;
	int		TamRes = WideCharToMultiByte(CP_UTF8, NULL, Path.c_str(), static_cast<int>(TamnTexto), Destino, MAX_PATH, NULL, NULL);
	Destino[TamRes] = 0;	// finalizo el string porque en la versión RELEASE de WideCharToMultiByte no se pone el terminador de cadenas, en la debug si.... (NO TESTEADO DESDE VS2008)


	libvlc_media_t *Media = NULL;
	Media = libvlc_media_new_path(VLC, Destino);

	HANDLE Sem = CreateSemaphore(NULL, 0, 1, L"RAVE_PARSING");
//	vlc_sem_t sem;
//	vlc_sem_init(&sem, 0);

	// Check to see if we are properly receiving the event.
	libvlc_event_manager_t *em = libvlc_media_event_manager(Media);
	libvlc_event_attach(em, libvlc_MediaParsedChanged, _ParsearTerminado, Sem);

	// Parse the media. This is synchronous.
	int i_ret = libvlc_media_parse_with_options(Media, libvlc_media_parse_local, -1);

	WaitForSingleObject(Sem, 10000);
	// Wait for preparsed event
	//vlc_sem_wait(&sem);
	//vlc_sem_destroy(&sem);

	if (libvlc_media_get_parsed_status(Media) == libvlc_media_parsed_status_done) {
		// Obtengo los datos en ANSI... el VLC tiene muy mal soporte para wchar_t...
/*		char *Nombre	= libvlc_media_get_meta(Media, libvlc_meta_Title);
		char *Grupo		= libvlc_media_get_meta(Media, libvlc_meta_Artist);
		char *Disco		= libvlc_media_get_meta(Media, libvlc_meta_Album);
		char *Genero	= libvlc_media_get_meta(Media, libvlc_meta_Genre);
		char *Pista		= libvlc_media_get_meta(Media, libvlc_meta_TrackNumber);*/

		BDMedio Medio;
		_BD.ObtenerMedio(Path, Medio);

		std::wstring TmpTxt, Filtrado;
		UINT nPista;
		TmpTxt			= _ObtenerMeta(Media, libvlc_meta_Title);
		RaveBD::_AnalizarNombre(TmpTxt, Filtrado, nPista);
		Medio.Nombre	= Filtrado;
		Medio.Grupo		= _ObtenerMeta(Media, libvlc_meta_Artist);
		Medio.Disco		= _ObtenerMeta(Media, libvlc_meta_Album);
		Medio.Genero	= _ObtenerMeta(Media, libvlc_meta_Genre);
		TmpTxt			= _ObtenerMeta(Media, libvlc_meta_TrackNumber);
		Medio.Pista		= DString_StrTo(TmpTxt, Medio.Pista);

/*		if (Nombre != NULL) {
			TmpTxt = DString_ToStr(Nombre);
			RaveBD::FiltroNombre(TmpTxt, TxtFiltrado);
			Medio.Nombre = TxtFiltrado;
		}
		if (Genero != NULL) {
			TmpTxt = DString_ToStr(Genero);
			RaveBD::FiltroNombre(TmpTxt, TxtFiltrado);
			Medio.Genero = TxtFiltrado;
		}
		if (Grupo != NULL) {
			TmpTxt = DString_ToStr(Grupo);
			RaveBD::FiltroNombre(TmpTxt, TxtFiltrado);
			Medio.Grupo = TxtFiltrado;
		}
		if (Disco != NULL) {
			TmpTxt = DString_ToStr(Disco);
			RaveBD::FiltroNombre(TmpTxt, TxtFiltrado);
			Medio.Disco = TxtFiltrado;
		}
		if (Pista != NULL) {
			TmpTxt = DString_ToStr(Pista);
			Medio.Pista = DString_StrTo(TmpTxt, Medio.Pista);
		}*/

		_BD.ActualizarMedio(&Medio);
	}
	// Ha pasado el tiempo..
	else {
		libvlc_media_parse_stop(Media);
	}

	libvlc_media_release(Media);	
}

void ThreadActualizarArbol::_ParsearTerminado(const libvlc_event_t *event, void *user_data) {
	(void)event;
	HANDLE Sem = (HANDLE)user_data;
	ReleaseSemaphore(Sem, 1, NULL);
	//vlc_sem_post(sem);
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
				// Agrego el medio a la BD
				if (_BD.AnalizarMedio(Path, Medio, FindInfoPoint.nFileSizeLow) == TRUE) {
					// Si no existia el medio agregado, añado el medio a la lista de medios a parsear
					_PorParsear.push_back(Path);
				}
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
