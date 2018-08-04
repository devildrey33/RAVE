#include "stdafx.h"
#include "RaveVLC.h"
#include "RAVE_Iconos.h"

RaveVLC::RaveVLC() : _Log(NULL), _MediaPlayer(NULL), _Instancia(NULL), MedioActual(), hWndVLC(NULL), _Eventos(NULL) {
}


RaveVLC::~RaveVLC() {
}


const BOOL RaveVLC::Iniciar(void) {
/*	std::wstring AppPath, Tmp;
	App.ObtenerPathApp(AppPath);
	Tmp = TEXT("--plugin - path =") + AppPath + TEXT("\\Plugins");

	ArgumentosVLC ArgsVLC;
	const char * test_defaults_args[] = {
		"-vvv",
		"--ignore-config",
		"-I",
		"dummy",
		"--no-media-library",
		"--vout=dummy",
		"--aout=dummy"
	};*/
/*	int test_defaults_nargs = sizeof (test_defaults_args) / sizeof (test_defaults_args[0]);
	ArgsVLC.AgregarArgumento(AppPath.c_str());			// Path de la aplicacion
	ArgsVLC.AgregarArgumento(Tmp.c_str());				// Path del directorio de plugins
//	ArgsVLC.AgregarArgumento("--no-video-title-show");	// No mostrar el titulo del video */
	//_Instancia = VLC::Instance(0, NULL);

	char OpcionesSMem[1024];
	sprintf_s(OpcionesSMem,	1024, "smem{audio-prerender-callback=%lld,audio-postrender-callback=%lld,audio-data=%lld}", ((long long int)(intptr_t)(void*)&audio_prerendercb), ((long long int)(intptr_t)(void*)&audio_postrendercb), ((long long int)(intptr_t)(void*)this));
	const char * const Argumentos[] = {	
			OpcionesSMem
	};

	Debug_Escribir_Varg(L"RaveVLC::Iniciar Cargando LibVLC...\n", _Instancia);
	DWORD t = GetTickCount();
//	_Instancia = libvlc_new(0, NULL);
	_Instancia = libvlc_new(sizeof(Argumentos) / sizeof(Argumentos[0]), Argumentos);

	const char *ErrorVLC = libvlc_errmsg();
	/*_Log = libvlc_log_open(_Instancia); // PARTE DEL LOG PARA EL VLC
	libvlc_set_log_verbosity(_Instancia, 2);*/

	Debug_Escribir_Varg(L"RaveVLC::Iniciar Cargado en %d MS, Instancia = '%x' \n", GetTickCount() - t, _Instancia);

	return TRUE;
}

void RaveVLC::audio_prerendercb(void* data, unsigned char** buffer, size_t size) {
	int i = 0;
}
void RaveVLC::audio_postrendercb(void* data, unsigned char* buffer, unsigned int channels, unsigned int rate, unsigned int nb_samples, unsigned int bits_per_sample, size_t size, int64_t pts) {
	int i = 0;
}


void RaveVLC::Terminar(void) {
	if (_Log != NULL) {
		libvlc_log_close(_Log);
		_Log = NULL;
	}
	if (_MediaPlayer != NULL) {
		libvlc_media_player_stop(_MediaPlayer);
		libvlc_media_player_release(_MediaPlayer);
		_MediaPlayer = NULL;
	}
	if (_Instancia != NULL) {
		libvlc_release(_Instancia);
		_Instancia = NULL;
	}
	hWndVLC = NULL;
	Debug_Escribir(L"RaveVLC::Terminar\n");
}

/*
void EventosVLC(const libvlc_event_t* event, void* ptr) {
	switch (event->type) {
		case libvlc_MediaStateChanged:			Debug_Escribir_Varg(L"RaveVLC::EventosVLC '%s'\n", L"libvlc_MediaDurationChanged");		break;
		case libvlc_MediaDurationChanged :		Debug_Escribir_Varg(L"RaveVLC::EventosVLC '%s'\n", L"libvlc_MediaDurationChanged");	break;
		case libvlc_MediaPlayerEndReached:			Debug_Escribir_Varg(L"RaveVLC::EventosVLC '%s'\n", L"libvlc_MediaPlayerEndReached");		break;
		case libvlc_MediaPlayerEncounteredError:	Debug_Escribir_Varg(L"RaveVLC::EventosVLC '%s'\n", L"libvlc_MediaPlayerEncounteredError");  break;
		default:									Debug_Escribir_Varg(L"RaveVLC::EventosVLC '%s'\n", L"Evento desconocido...");				break;
	}
}*/


const BOOL RaveVLC::AbrirMedio(BDMedio &Medio) {
	CerrarMedio();
	TiempoTotal = 0;
	MedioActual = Medio;

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(MedioActual.Path.c_str())) {
		Debug_Escribir_Varg(L"RaveVLC::AbrirMedio  El archivo '%s' NO EXISTE!\n", MedioActual.Path.c_str());
		return FALSE;
	}

	// Hay que convertir el path a UTF8 para que funcione en el VLC...
	char	Destino[MAX_PATH];
	size_t  TamnTexto	= wcslen(MedioActual.Path.c_str()) + 1;
	int		TamRes		= WideCharToMultiByte(CP_UTF8, NULL, MedioActual.Path.c_str(), static_cast<int>(TamnTexto), Destino, MAX_PATH, NULL, NULL);
	Destino[TamRes] = 0;	// finalizo el string porque en la versión RELEASE de WideCharToMultiByte no se pone el terminador de cadenas, en la debug si.... (NO TESTEADO DESDE VS2008)

	
	libvlc_media_t *Media = NULL;
	Media = libvlc_media_new_path(_Instancia, Destino);
	_MediaPlayer = libvlc_media_player_new_from_media(Media);
	libvlc_media_release(Media);
	
//	_Eventos = libvlc_media_player_event_manager(_MediaPlayer);
//	libvlc_event_attach(_Eventos, libvlc_MediaStateChanged, EventosVLC, this);				// no va
//	libvlc_event_attach(_Eventos, libvlc_MediaDurationChanged, EventosVLC, this);			// no va (igual amb streaming si...)
//	libvlc_event_attach(_Eventos, libvlc_MediaPlayerEndReached, EventosVLC, this);			
//	libvlc_event_attach(_Eventos, libvlc_MediaPlayerEncounteredError, EventosVLC, this);	// ?

	// Desactiva los eventos del mouse y del teclado dentro de la ventana del vlc 
	libvlc_video_set_mouse_input(_MediaPlayer, false);
	libvlc_video_set_key_input(_MediaPlayer, false);

	Volumen(static_cast<int>(App.VentanaRave.SliderVolumen.Valor()));

	if (Medio.TipoMedio == Tipo_Medio_Video) {
		libvlc_media_player_set_hwnd(_MediaPlayer, App.VentanaRave.Video.hWnd());
		// Pulso el botón para mostrar el video
		App.VentanaRave.Evento_Button_Mouse_Click(ID_BOTON_VIDEO);
	}

	// Escondo los tooltip de las barras de tiempo
	App.VentanaRave.SliderTiempo.OcultarToolTip();
	App.ControlesPC.SliderTiempo.OcultarToolTip();

	// Asigno el titulo de la ventana con el nombre del medio que se acaba de abrir
	std::wstring nTitulo = std::wstring(RAVE_TITULO) + L" - " +  std::wstring(MedioActual.Nombre);
	App.VentanaRave.Titulo(nTitulo);

	Debug_Escribir_Varg(L"RaveVLC::AbrirMedio Path '%s'\n", MedioActual.Path.c_str());

	return TRUE;
}


void RaveVLC::CerrarMedio(void) {
	hWndVLC = NULL;
	if (_MediaPlayer != NULL) {
		Stop();

/*		libvlc_event_detach(_Eventos, libvlc_MediaStateChanged,		EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaDurationChanged,	EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaPlayerEndReached,			EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaPlayerEncounteredError,	EventosVLC, this);*/

		libvlc_media_player_release(_MediaPlayer);
		_MediaPlayer = NULL;
	}
}

/* nTipo :
	0 - Normal
	1 - Play
	2 - Pausa
*/

void RaveVLC::ActualizarIconos(int nTipo) {
	if (MedioActual.Hash != 0) {

		int nIcono = 0;
		switch (nTipo) {
			case 0 : // normal
				if (MedioActual.TipoMedio == Tipo_Medio::Tipo_Medio_Audio) nIcono = RAVE_Iconos::RAVE_Icono_Cancion; // IDI_CANCION
				if (MedioActual.TipoMedio == Tipo_Medio::Tipo_Medio_Video) nIcono = RAVE_Iconos::RAVE_Icono_Video; // IDI_VIDEO
				break;
			case 1 :
				nIcono = RAVE_Iconos::RAVE_Icono_Play; // play
				break;
			case 2 : 
				nIcono = RAVE_Iconos::RAVE_Icono_Pausa; // Pausa
				break;
		}
		// Actualizo el nodo del ArbolBD
		NodoBD *Nodo = App.VentanaRave.Arbol.BuscarHash(MedioActual.Hash);
		if (Nodo != NULL) 
			Nodo->Icono(nIcono);
		App.VentanaRave.Arbol.Repintar();
		// Actualizo el Item del ListaMedios
		ItemMedio *Item = App.VentanaRave.Lista.BuscarHash(MedioActual.Hash);
		if (Item != NULL) {
			Item->Icono(nIcono);
			App.VentanaRave.Lista.MostrarItem(Item);
		}
		App.VentanaRave.Lista.Repintar();

/*		if (nTipo == 1) {
			std::wstring StrTiempo;
			//	Debug_Escribir_Varg(L"Evento_SliderTiempo_Cambiado %d, %.02f\n", App.VLC.TiempoTotalMs(), SliderTiempo.Valor());
			TiempoStr(static_cast<UINT64>(App.VLC.TiempoTotalMs()), StrTiempo);
			Item->Texto(0) = StrTiempo;
		}*/
	}

}

const BOOL RaveVLC::Stop(void) {
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, TRUE); // Activo protector de pantalla

	if (_MediaPlayer != NULL) {
		ActualizarIconos(0);

		// Escondo los tooltip de las barras de tiempo
		App.VentanaRave.SliderTiempo.OcultarToolTip();
		App.ControlesPC.SliderTiempo.OcultarToolTip();
		// Evito mostrar el tooltip si se pasa el mouse por encima del slider tiempo
		App.VentanaRave.SliderTiempo.ToolTip(DBarraDesplazamientoEx_ToolTip_SinToolTip);
		App.ControlesPC.SliderTiempo.ToolTip(DBarraDesplazamientoEx_ToolTip_SinToolTip);

//		Sistema.App.MediaPlayer.HackVLCWNDPROC(false);
		libvlc_media_player_stop(_MediaPlayer);
		hWndVLC = NULL;
		return TRUE;
	}
	return FALSE;
}

const BOOL RaveVLC::Pausa(void) {

	if (_MediaPlayer != NULL) {
		if (ComprobarEstado() == EnPlay) {
			ActualizarIconos(2);
			libvlc_media_player_pause(_MediaPlayer);
		}
		return TRUE;
	}
	return FALSE;
}

const BOOL RaveVLC::Play(void) {
	if (_MediaPlayer != NULL) {
		if (libvlc_media_player_play(_MediaPlayer) == 0)	{
			App.VentanaRave.SliderTiempo.ToolTip(DBarraDesplazamientoEx_ToolTip_Inferior);
			App.ControlesPC.SliderTiempo.ToolTip(DBarraDesplazamientoEx_ToolTip_Superior);
			hWndVLC = NULL;
			ActualizarIconos(1);
			SetTimer(App.VentanaRave.hWnd(), TIMER_OBTENERVLCWND, 100, NULL);
			SetTimer(App.VentanaRave.hWnd(), TIMER_OBTENER_TIEMPO_TOTAL, 250, NULL);
			if (MedioActual.TipoMedio == Tipo_Medio_Video) { // Desactivo el protector de pantalla si es un video
				SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, TRUE);
			}
			return TRUE;
		}
	}
	return FALSE;	
}

Estados_Medio RaveVLC::ComprobarEstado(void) {
	if (_MediaPlayer == NULL) return SinCargar;
	libvlc_state_t Estado = libvlc_media_player_get_state(_MediaPlayer);
	switch (Estado) {
		case libvlc_NothingSpecial:
			return Nada;
		case libvlc_Opening:
			return Abriendo;
		case libvlc_Buffering:
			return SinCargar;
		case libvlc_Playing:
	/*		if (_Comprobacion == 1) {
				libvlc_media_player_pause(_MediaPlayer);
				libvlc_media_player_play(_MediaPlayer);
				_Comprobacion = 2;
				return SinCargar;
			}
			// Miramos el log para ver si hay errores
			else if (_Comprobacion < 10) {
				_Comprobacion++;
				if (ComprobarError() != 0)
					return EnError;
			} */
			return EnPlay;
		case libvlc_Stopped:
			return EnStop;
		case libvlc_Paused:
			return EnPausa;
			//	#if (LIBVLC_NUM_VERSION == 100)
		case libvlc_Ended:
			return Terminada;
			//	#endif
		case libvlc_Error:
			return EnError;
	}
	return SinCargar;
}

// 0 - 200
void RaveVLC::Volumen(int nVolumen) {
	App.VentanaRave.SliderVolumen.Valor(static_cast<float>(nVolumen));
	App.ControlesPC.SliderVolumen.Valor(static_cast<float>(nVolumen));
	std::wstring StrVol = std::to_wstring(nVolumen) + L"%";
	App.VentanaRave.LabelVolumen.Texto(StrVol);
	App.ControlesPC.LabelVolumen.Texto(StrVol);

	if (_MediaPlayer != NULL) {
		libvlc_audio_set_volume(_MediaPlayer, nVolumen);
	}
}

const int RaveVLC::Volumen(void) {
	if (_MediaPlayer != NULL) {
		return libvlc_audio_get_volume(_MediaPlayer);
	}
	return 0;
}


void RaveVLC::TiempoStr(UINT64 TMS, std::wstring &StrTiempo) {            
	StrTiempo.resize(0);
	unsigned int Horas = static_cast<int>(((TMS / 1000) / 60) / 60);
	unsigned int Minutos = static_cast<int>((TMS / 1000) / 60) - (Horas * 60);
	unsigned int Segundos = static_cast<int>(TMS / 1000) - (Minutos * 60) - ((Horas * 60) * 60);
	if (Horas > 0)    StrTiempo = std::to_wstring(Horas) + L":";
	if (Minutos > 9)  StrTiempo += std::to_wstring(Minutos) + L":";
	else              StrTiempo += L"0" + std::to_wstring(Minutos) + L":";
	if (Segundos > 9) StrTiempo += std::to_wstring(Segundos);
	else              StrTiempo += L"0" + std::to_wstring(Segundos);
}

void RaveVLC::TiempoStr(UINT64 TMS, wchar_t *StrTiempo) {                                      //
	//tm temp;
//	temp.tm_hour = static_cast<int>(((TMS / 1000) / 60) / 60);
	int min = static_cast<int>(((TMS / 1000) / 60)); // -(temp.tm_hour * 60));
	int sec = static_cast<int>((TMS / 1000) - (min * 60)); // -((temp.tm_hour * 60) * 60));
	swprintf(StrTiempo, 256, TEXT("%.2d:%.2d"), min, sec);

	
}
/*
void RaveVLC::TiempoStr(UINT64 TMS, wchar_t *StrTiempo) {                                      //
	tm temp;
	temp.tm_hour = static_cast<int>(((TMS / 1000) / 60) / 60);
	temp.tm_min = static_cast<int>(((TMS / 1000) / 60) - (temp.tm_hour * 60));
	temp.tm_sec = static_cast<int>((TMS / 1000) - (temp.tm_min * 60) - ((temp.tm_hour * 60) * 60));
	swprintf(StrTiempo, 256, TEXT("%.2d:%.2d"), temp.tm_min, temp.tm_sec);
}                                                                        //
*/


const UINT64 RaveVLC::TiempoTotalMs(void) {
	if (_MediaPlayer != NULL) {
		return static_cast<UINT64>(libvlc_media_player_get_length(_MediaPlayer));
	}
	return 0;
}

const UINT64 RaveVLC::TiempoActualMs(void) {
	if (_MediaPlayer != NULL) {
		return static_cast<UINT64>(libvlc_media_player_get_time(_MediaPlayer));
	}
	return 0;
}


const float RaveVLC::TiempoActual(void) {
	if (_MediaPlayer != NULL) {
		return libvlc_media_player_get_position(_MediaPlayer);
	}
	return 0.0f;
}

void RaveVLC::TiempoActual(float nTiempo) {
	if (_MediaPlayer != NULL) {
		libvlc_media_player_set_position(_MediaPlayer, nTiempo);
	}
}

void RaveVLC::RepintarVLC(void) {
	if (hWndVLC != NULL) {
/*		Debug_Escribir_Varg(L"RepintarVLC %d\n", hWndVLC);
		RECT RC;
		GetClientRect(hWndVLC, &RC);
		InvalidateRect(hWndVLC, &RC, FALSE);
		UpdateWindow(hWndVLC);*/
		Debug_Escribir_Varg(L"RepintarVLC %d\n", hWndVLC);
		HDC hDC = GetDC(hWndVLC);
		HBRUSH Brocha = (HBRUSH)GetStockObject(BLACK_BRUSH);
		RECT RC;
		GetClientRect(hWndVLC, &RC);
		FillRect(hDC, &RC, Brocha);
		ReleaseDC(hWndVLC, hDC);
	}
}


// Necesito encontrar la ventana del VLC que se crea dentro del VerVideo para repintar el fondo negro
BOOL CALLBACK RaveVLC::EnumeracionVLC(HWND hWndWnd, LPARAM lParam) {
	App.VLC.hWndVLC = hWndWnd;
//	EnableWindow(hWndWnd, FALSE);
	return FALSE;
}
