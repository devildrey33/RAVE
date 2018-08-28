#include "stdafx.h"
#include "ThreadObtenerMetadatos.h"
#include "DStringUtils.h"
#include "DMensajesWnd.h"
#include "resource.h"

#define ID_BARRAPROGRESO		1000
#define ID_BOTONCANCELAR		1001
#define ID_MARCANOMOSTRARMAS	1002

ThreadObtenerMetadatos::ThreadObtenerMetadatos(void) {
}


ThreadObtenerMetadatos::~ThreadObtenerMetadatos(void) {
}



const BOOL ThreadObtenerMetadatos::Iniciar(HWND nhWndDest) {
	// Se está ejecutando 
	if (_Thread != NULL) return FALSE;

	// Asigno Ventana para los mensajes
	_VentanaPlayer = nhWndDest;

	if (App.BD.Opciones_MostrarObtenerMetadatos() == TRUE) {
		// Creo la ventana que mostrará el progreso
		CrearVentana(NULL, L"RAVE_ObtenerMetadatos", L"Obteniendo metadatos", 300, 200, 600, 200, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE);
		RECT RC;
		GetClientRect(_hWnd, &RC);
		_BarraProgreso.CrearBarraProgresoEx(this, 10, 80, RC.right - 20, 20, ID_BARRAPROGRESO);
		_BotonCancelar.CrearBotonEx(this, L"Cancelar", 280, 110, 100, 30, ID_BOTONCANCELAR);
		_MarcaNoMostrarMas.CrearMarcaEx(this, L"No volver a mostrar esta ventana.", 10, 116, 250, 20, ID_MARCANOMOSTRARMAS, IDI_CHECK2);
	}

	// Iniciamos el Thread
	_Thread = CreateThread(NULL, 0, (unsigned long(__stdcall *)(void *))this->_ThreadObtenerMetadatos, (void *)this, 0, NULL);

	Cancelar(FALSE);

	if (_Thread == NULL)
		return FALSE;

	SetThreadPriority(_Thread, 0);

	Debug_Escribir(L"ThreadObtenerMetadatos::Iniciado\n");
	return TRUE;
}


void ThreadObtenerMetadatos::Terminar(void) {
	_BarraProgreso.Destruir();
	_BotonCancelar.Destruir();
	_MarcaNoMostrarMas.Destruir();
	Destruir();
	CloseHandle(_Thread);
	_Thread = NULL;
}

void ThreadObtenerMetadatos::Cancelar(const BOOL nCancelar) {
	_Mutex.lock();
	_Cancelar = nCancelar;
	_Mutex.unlock();
}

const BOOL ThreadObtenerMetadatos::Cancelar(void) {
	_Mutex.lock();
	BOOL Ret = _Cancelar;
	_Mutex.unlock();
	return Ret;
}

unsigned long ThreadObtenerMetadatos::_ThreadObtenerMetadatos(void *pThis) {
	ThreadObtenerMetadatos *This = reinterpret_cast<ThreadObtenerMetadatos *>(pThis);
	// Fase 1 : Obtener una lista de medios que no se han parseado
	This->_BD.Iniciar();
	This->_PorParsear.resize(0);
	This->_BD.ObtenerMediosPorParsear(This->_PorParsear);

	// Fase 2 : Iniciar la VLC en este thread
	const char * const Argumentos[] = { "-v", "--vout=vdummy" };
	libvlc_instance_t *VLC = libvlc_new(2, Argumentos);

	int Contador = 0;

	// Mando un me4nsaje con el total de medios por analizar
	SendMessage(This->_hWnd, WM_TOM_INICIADO, static_cast<WPARAM>(This->_PorParsear.size()), 0);

	// Inicio la transaccion por segunda vez
	This->_BD.Consulta(L"BEGIN TRANSACTION");
	// Fase 3 : analizar nuevos medios añadidos a la BD
	for (size_t i = 0; i < This->_PorParsear.size(); i++) {
		if (This->Cancelar() == TRUE) {
			break;
		}

		// Guardo los datos en la BD cada 50 iteraciones
		if (Contador < 50) { Contador++; }
		else {
			This->_BD.Consulta(L"COMMIT TRANSACTION");
			This->_BD.Consulta(L"BEGIN TRANSACTION");
			Contador = 0;
		}

		This->_Parsear(VLC, This->_PorParsear[i]);
		SendMessage(This->_VentanaPlayer, WM_TOM_TOTALMEDIOS, i, static_cast<LPARAM>(This->_PorParsear.size()));
		if (This->_hWnd != NULL) SendMessage(This->_hWnd, WM_TOM_TOTALMEDIOS, i, static_cast<LPARAM>(This->_PorParsear.size()));
	}

	// Terminado
	libvlc_release(VLC);
	This->_BD.Consulta(L"COMMIT TRANSACTION");
	This->_BD.Terminar();

	SendMessage(This->_VentanaPlayer, WM_TOM_TERMINADO, 0, static_cast<LPARAM>(This->_PorParsear.size()));

	return 0;
}



std::wstring ThreadObtenerMetadatos::_ObtenerMeta(libvlc_media_t *Media, libvlc_meta_t Tipo) {
	std::wstring Ret;
	char *Txt = libvlc_media_get_meta(Media, Tipo);
	if (Txt != NULL) {
		std::wstring TmpTxt; // = DString_ToStr(Txt);
		DString_AnsiToWide(Txt, TmpTxt);
		RaveBD::FiltroNombre(TmpTxt, Ret);
	}
	libvlc_free(Txt);
	return Ret;
}


void ThreadObtenerMetadatos::_Parsear(libvlc_instance_t *VLC, std::wstring &Path) {
	// Compruebo que existe el archivo	
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(Path.c_str())) {
//		Debug_Escribir_Varg(L"ThreadObtenerMetadatos::_Parsear  El archivo '%s' NO EXISTE!\n", .Path.c_str());
		return;
	}

	// Hay que convertir el path a UTF8 para que funcione en el VLC...
	char	Destino[MAX_PATH];
	size_t  TamnTexto = wcslen(Path.c_str()) + 1;
	int		TamRes = WideCharToMultiByte(CP_UTF8, NULL, Path.c_str(), static_cast<int>(TamnTexto), Destino, MAX_PATH, NULL, NULL);
	Destino[TamRes] = 0;	// finalizo el string porque en la versión RELEASE de WideCharToMultiByte no se pone el terminador de cadenas, en la debug si.... (NO TESTEADO DESDE VS2008)

	libvlc_media_t *Media = NULL;
	Media = libvlc_media_new_path(VLC, Destino);

	HANDLE Sem = CreateSemaphore(NULL, 0, 1, L"RAVE_ThreadObtenerMetadatos");
	//	vlc_sem_t sem;
	//	vlc_sem_init(&sem, 0);

	// Check to see if we are properly receiving the event.
	libvlc_event_manager_t *em = libvlc_media_event_manager(Media);
	libvlc_event_attach(em, libvlc_MediaParsedChanged, _ParsearTerminado, Sem);

	// Parse the media. This is synchronous.
	int i_ret = libvlc_media_parse_with_options(Media, libvlc_media_parse_local, -1);
	// Espero a que termine el parsing o 10 segundos máximo
	WaitForSingleObject(Sem, 10000);

	// Si se ha realizado el parsing correctamente
	if (libvlc_media_get_parsed_status(Media) == libvlc_media_parsed_status_done) {		
		BDMedio Medio;
		_BD.ObtenerMedio(Path, Medio);

		std::wstring TmpTxt, TmpPath, Filtrado;
//		UINT nPista;

		char *Txt = libvlc_media_get_meta(Media, libvlc_meta_Title);
		DString_AnsiToWide(Txt, TmpTxt);
		libvlc_free(Txt);

//		TmpTxt			= _ObtenerMeta(Media, libvlc_meta_Title);

		size_t PosNombre = Medio.Path.find_last_of(L"\\");
		
		// No tiene el mismo nombre de archivo
		TmpPath = Medio.Path.substr(PosNombre + 1);
		if (Medio.Path.substr(PosNombre + 1) != TmpTxt) {
//			RaveBD::_AnalizarNombre(TmpTxt, Filtrado, nPista);
			Medio.NombreTag = Filtrado;
		}

		Medio.GrupoTag	= _ObtenerMeta(Media, libvlc_meta_Artist);
		Medio.DiscoTag	= _ObtenerMeta(Media, libvlc_meta_Album);
		Medio.Genero	= _ObtenerMeta(Media, libvlc_meta_Genre);
		TmpTxt			= _ObtenerMeta(Media, libvlc_meta_TrackNumber);
		Medio.PistaTag	= DString_StrTo(TmpTxt, Medio.PistaTag);
		Medio.Parseado	= TRUE;


		Medio.Tiempo = libvlc_media_get_duration(Media);

/*		libvlc_media_track_t **pp_tracks;
		unsigned int i_count = libvlc_media_tracks_get(Media, &pp_tracks);
//		if (i_count > 0) {
			const char *Codec = (const char *)&pp_tracks[0]->i_codec;
			const char *FourCC = (const char *)&pp_tracks[0]->i_original_fourcc;
	//	}
		libvlc_media_tracks_release(pp_tracks, i_count);*/

		_BD.ActualizarMedio(&Medio);
	}
	// Ha pasado el tiempo..
	else {
		libvlc_media_parse_stop(Media);
	}

	libvlc_media_release(Media);
}

void ThreadObtenerMetadatos::_ParsearTerminado(const libvlc_event_t *event, void *user_data) {
	(void)event;
	HANDLE Sem = (HANDLE)user_data;
	ReleaseSemaphore(Sem, 1, NULL);
	//vlc_sem_post(sem);
}


void ThreadObtenerMetadatos::Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(_hWnd, &PS);
	Pintar(DC);
	EndPaint(_hWnd, &PS);
}

void ThreadObtenerMetadatos::Pintar(HDC DC) {
	static const wchar_t pTexto[] = L"Analizando los metadatos de todos los medios en segundo plano.\n"
								    L"Este proceso recolecta datos tales como el tiempo, genero, grupo, disco, etc... y los\n"
									L"añade a la base de datos, lo que permite entre otras cosas generar listas aleatorias.";
	RECT RC;
	GetClientRect(_hWnd, &RC);
	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);

	HFONT VFuente = static_cast<HFONT>(SelectObject(DC, DhWnd::_Fuente18Normal()));

	SetBkMode(DC, TRANSPARENT);
	RECT RTS = { 11, 11, RC.right - 9, 91 };
	RECT RT  = { 10, 10, RC.right - 10, 90 };
	SetTextColor(DC, COLOR_TEXTO_SOMBRA);
	DrawText(DC, pTexto, -1, &RTS, DT_LEFT);
	SetTextColor(DC, COLOR_TEXTO);
	DrawText(DC, pTexto, -1, &RT, DT_LEFT);

	SelectObject(DC, VFuente);
	DeleteObject(BrochaFondo);
}


LRESULT CALLBACK ThreadObtenerMetadatos::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT			:	Evento_Pintar();														break;
		case WM_TOM_TOTALMEDIOS :	_BarraProgreso.Valor(static_cast<float>(wParam));						break;
		case WM_TOM_INICIADO    :   _BarraProgreso.Maximo(static_cast<float>(wParam));						break;
		case WM_CLOSE			:   Cancelar(TRUE);															break;
		case DWL_BOTONEX_CLICK  :   Cancelar(TRUE);															break;
		case DWL_MARCAEX_CLICK  :   App.BD.Opciones_MostrarObtenerMetadatos(!_MarcaNoMostrarMas.Marcado());	break;
	}
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}