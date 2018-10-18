#include "stdafx.h"
#include "RaveVLC.h"
#include "RAVE_Iconos.h"
#include "VentanaPrecarga.h"
#include "DStringUtils.h"

RaveVLC::RaveVLC() : _Log(NULL), _MediaPlayer(NULL), _Instancia(NULL), MedioActual(), hWndVLC(NULL), _Eventos(NULL), _Parseado(FALSE) {
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
	
	// Ventana de precarga
	VentanaPrecarga Precarga;


	char OpcionesSMem[1024];
	sprintf_s(OpcionesSMem,	1024, "smem{audio-prerender-callback=\"%lld\",audio-postrender-callback=\"%lld\",audio-data=\"%lld\"}", ((long long int)(intptr_t)(void*)&audio_prerendercb), ((long long int)(intptr_t)(void*)&audio_postrendercb), ((long long int)(intptr_t)(void*)this));
	const char * const Argumentos[] = {	
		OpcionesSMem,
//		"--verbose=3"
	};

	Debug_Escribir_Varg(L"RaveVLC::Iniciar Cargando LibVLC...\n", _Instancia);
	DWORD t = GetTickCount();
//	_Instancia = libvlc_new(0, NULL);
	_Instancia = libvlc_new(sizeof(Argumentos) / sizeof(Argumentos[0]), Argumentos);

//	libvlc_set_log_verbosity(_Instancia, 3);

	const char *ErrorVLC = libvlc_errmsg();
	/*_Log = libvlc_log_open(_Instancia); // PARTE DEL LOG PARA EL VLC
	libvlc_set_log_verbosity(_Instancia, 2);*/

	const char *Version = libvlc_get_version();
	std::wstring StrVersion;
	DWL::Strings::AnsiToWide(Version, StrVersion);
	Debug_Escribir_Varg(L"RaveVLC::Iniciar Cargado en %d MS, Version = '%s' \n", GetTickCount() - t, StrVersion.c_str());

	Precarga.Destruir();



	libvlc_module_description_t *LA = libvlc_audio_filter_list_get(_Instancia);
	libvlc_module_description_t *LV = libvlc_video_filter_list_get(_Instancia);
	
	libvlc_module_description_list_release(LA);
	libvlc_module_description_list_release(LV);


	return TRUE;
}

void RaveVLC::audio_prerendercb(void* data, unsigned char** buffer, size_t size) {
	int i = 0;
}
void RaveVLC::audio_postrendercb(void* data, unsigned char* buffer, unsigned int channels, unsigned int rate, unsigned int nb_samples, unsigned int bits_per_sample, size_t size, int64_t pts) {
	int i = 0;
}


void RaveVLC::Terminar(void) {
	Debug_Escribir(L"RaveVLC::Terminar\n");
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
	Debug_Escribir(L"RaveVLC::Terminar TERMINADO\n");
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
	_Parseado = FALSE;

	App.MenuVideoFiltros->Menu(0)->BarraValor(1.0f);	// Brillo
	App.MenuVideoFiltros->Menu(1)->BarraValor(1.0f);	// Contraste
	App.MenuVideoFiltros->Menu(2)->BarraValor(1.0f);	// Saturación
	

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(MedioActual.Path.c_str())) {
		Debug_Escribir_Varg(L"RaveVLC::AbrirMedio  El archivo '%s' NO EXISTE!\n", MedioActual.Path.c_str());
		return FALSE;
	}

	std::string AnsiStr;
	// Hay que convertir el path a UTF8 para que funcione en el VLC...
	if (DWL::Strings::WideToAnsi(MedioActual.Path.c_str(), AnsiStr) == 0) {
		Debug_MostrarUltimoError();
		return FALSE;
	}

/*	char	Destino[MAX_PATH];
	size_t  TamnTexto	= wcslen(MedioActual.Path.c_str()) + 1;
	int		TamRes		= WideCharToMultiByte(CP_UTF8, NULL, MedioActual.Path.c_str(), static_cast<int>(TamnTexto), Destino, MAX_PATH, NULL, NULL);
	Destino[TamRes] = 0;	// finalizo el string porque en la versión RELEASE de WideCharToMultiByte no se pone el terminador de cadenas, en la debug si.... (NO TESTEADO DESDE VS2008)*/

	libvlc_media_t *_Media = NULL;
	_Media = libvlc_media_new_path(_Instancia, AnsiStr.c_str());
	_MediaPlayer = libvlc_media_player_new_from_media(_Media);
	libvlc_media_release(_Media);
	
//	_Eventos = libvlc_media_player_event_manager(_MediaPlayer);
//	libvlc_event_attach(_Eventos, libvlc_MediaStateChanged, EventosVLC, this);				// no va
//	libvlc_event_attach(_Eventos, libvlc_MediaDurationChanged, EventosVLC, this);			// no va (igual amb streaming si...)
//	libvlc_event_attach(_Eventos, libvlc_MediaPlayerEndReached, EventosVLC, this);			
//	libvlc_event_attach(_Eventos, libvlc_MediaPlayerEncounteredError, EventosVLC, this);	// ?

	// Desactiva los eventos del mouse y del teclado dentro de la ventana del vlc 
	libvlc_video_set_mouse_input(_MediaPlayer, false);
	libvlc_video_set_key_input(_MediaPlayer, false);

//	libvlc_add_option();

	Volumen(static_cast<int>(App.VentanaRave.SliderVolumen.Valor()));

	if (Medio.TipoMedio == Tipo_Medio_Video) {
		libvlc_media_player_set_hwnd(_MediaPlayer, App.VentanaRave.Video.hWnd());
		// Pulso el botón para mostrar el video
		App.VentanaRave.Evento_BotonEx_Mouse_Click(DEventoMouse(0, 0, &App.VentanaRave.BotonVideo, 0));
	}

	// Escondo los tooltip de las barras de tiempo
	App.VentanaRave.SliderTiempo.OcultarToolTip();
	App.ControlesPC.SliderTiempo.OcultarToolTip();

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
		Debug_Escribir(L"RaveVLC::CerrarMedio\n");
		libvlc_media_player_release(_MediaPlayer);
		_MediaPlayer = NULL;

		BOOL R = App.VentanaRave.BarraTareas.Clip(NULL);
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
	if (_MediaPlayer != NULL) {
		Debug_Escribir_Varg(L"RaveVLC::Stop %d\n", _MediaPlayer);

		ActualizarIconos(0);

		// Escondo los tooltip de las barras de tiempo
		App.VentanaRave.SliderTiempo.OcultarToolTip();
		App.ControlesPC.SliderTiempo.OcultarToolTip();
		// Evito mostrar el tooltip si se pasa el mouse por encima del slider tiempo
		App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
		App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);

		
		// Deadlock just despres de mostrar un DMenuEx en un video i utilitzar el stop.... SOLUCIONAT, pero s'ha d'anar amb cuidado al utilitzar SetFocus...
		if (libvlc_media_player_has_vout(_MediaPlayer) > 0) {
			// Para evitar un deadlock si se está reproduciendo un video y el foco está en otra parte
			HWND Foco = SetFocus(App.VentanaRave.hWnd()); // REVISADO
		}
		// Desactivo los filtros que pueda haber activados para evitar un crash en la versión x86
		libvlc_video_set_adjust_int(_MediaPlayer, libvlc_adjust_Enable, 0);
		libvlc_media_player_stop(_MediaPlayer);

		hWndVLC = NULL;

		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, TRUE); // Activo protector de pantalla
		
		// Asigno el titulo de la ventana con el nombre del medio que se acaba de abrir
//		std::wstring nTitulo = std::wstring(RAVE_TITULO) + L" - " +  MedioActual.Nombre();
		App.VentanaRave.Titulo(std::wstring(RAVE_TITULO));

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
			App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_Abajo);
			// Establezco la norma del tooltip según la alineación de los controles pantalla completa
			switch (App.ControlesPC.Alineacion) {
				case Abajo		:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Arriba);		break;
				case Arriba		:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Abajo);		break;
				case Izquierda	:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Derecha);		break;
				case Derecha	:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Izquierda);	break;
			}
			
			
			hWndVLC = NULL;
			ActualizarIconos(1);
			SetTimer(App.VentanaRave.hWnd(), TIMER_OBTENERVLCWND, 100, NULL);
//			SetTimer(App.VentanaRave.hWnd(), TIMER_OBTENER_TIEMPO_TOTAL, 250, NULL);
			if (MedioActual.TipoMedio == Tipo_Medio_Video) { // Desactivo el protector de pantalla si es un video
				SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, TRUE);
			}

			// Asigno el titulo de la ventana con el nombre del medio que se acaba de abrir
			std::wstring nTitulo = std::wstring(RAVE_TITULO) + L" - " + MedioActual.Nombre();
			App.VentanaRave.Titulo(nTitulo);

			// Muestro el tooltip con los datos
			std::wstring TT = DWL::Strings::ToStr(MedioActual.Pista(), 2) + L" " + MedioActual.Nombre();

			if (MedioActual.Disco().size() > 0)
				TT += L"\n" + MedioActual.Disco(); 
			if (MedioActual.Grupo().size() > 0) 
				TT += L"\n" + MedioActual.Grupo();
			if (MedioActual.Genero.size() > 0)
				TT += L"\n" + MedioActual.Genero;
			
			App.MostrarToolTipPlayer(TT);
			return TRUE;
		}
	}
	return FALSE;	
}

// Obtiene los datos del medio una vez parseado (despues del play)
void RaveVLC::ObtenerDatosParsing(void) {
	if (_MediaPlayer == NULL) return;
	libvlc_state_t nEstado = libvlc_media_player_get_state(_MediaPlayer);
	// Si no se ha parseado
	if (_Parseado == FALSE && nEstado == libvlc_Playing) {
		_Parseado = TRUE;
		// Enumero las pistas de audio
		int TotalPîstas = libvlc_audio_get_track_count(_MediaPlayer);
		if (TotalPîstas > 2) { // si hay mas de una pista, las enumero
			App.MenuVideoPistasDeAudio->Activado(TRUE);

			libvlc_track_description_t *Desc = libvlc_audio_get_track_description(_MediaPlayer);

			std::wstring Texto;
			App.MenuVideoPistasDeAudio->EliminarTodosLosMenus();
			for (int i = 0; i < TotalPîstas; i++) {
				if (i != 0) {
					DWL::Strings::AnsiToWide(Desc->psz_name, Texto);
					App.MenuVideoPistasDeAudio->AgregarMenu(ID_MENUVIDEO_AUDIO_PISTAS_AUDIO + i, Texto);
				}
				Desc = Desc->p_next;
			}
			int PistaActual = libvlc_audio_get_track(_MediaPlayer);
			DMenuEx *MenuPistaActual = App.MenuVideoPistasDeAudio->BuscarMenu(ID_MENUVIDEO_AUDIO_PISTAS_AUDIO + PistaActual);
			if (MenuPistaActual != NULL) MenuPistaActual->Icono(IDI_CHECK2);
			libvlc_track_description_list_release(Desc);
		}
		else {	// Si solo hay una pista de audio desactivo el menú
			App.MenuVideoPistasDeAudio->Activado(FALSE);
		}

		BOOL EsVideo = (MedioActual.TipoMedio == Tipo_Medio_Video);

		// Activo / desactivo el menú de la proporción según el tipo de medio (audio / video)
		App.MenuVideoProporcion->Activado(EsVideo);
		App.MenuVideoFiltros->Activado(EsVideo);
		App.MenuVideoSubtitulos->Activado(EsVideo);

		// Obtengo el ratio de aspecto
		std::wstring Proporcion = ObtenerProporcion();
		for (size_t i = 0; i < App.MenuVideoProporcion->TotalMenus(); i++) {
			if (App.MenuVideoProporcion->Menu(i)->Texto() == Proporcion)	App.MenuVideoProporcion->Menu(i)->Icono(IDI_CHECK2);
			else															App.MenuVideoProporcion->Menu(i)->Icono(0);
		}

		// Modifico el tiempo
//		App.VentanaRave.Lista.BuscarHash(MedioActual.Hash);
	}
}

std::wstring &RaveVLC::ObtenerProporcion(void) {
	static std::wstring Ret = L"Predeterminado";
	char *Tmp = libvlc_video_get_aspect_ratio(_MediaPlayer);
	if (Tmp != NULL) {
		DWL::Strings::AnsiToWide(Tmp, Ret);
		libvlc_free(Tmp);
	}
	return Ret;
}

void RaveVLC::AsignarProporcion(const char *Prop) {
	libvlc_video_set_aspect_ratio(_MediaPlayer, Prop);
}


void RaveVLC::AsignarPistaAudio(const int nPista) {
	libvlc_audio_set_track(_MediaPlayer, nPista);
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
	// Topes
	if (nVolumen > 200) nVolumen = 200;
	if (nVolumen < 0)	nVolumen = 0;

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

void RaveVLC::Ratio(const float R) {
	int r;
	if (_MediaPlayer != NULL) {
		r = libvlc_media_player_set_rate(_MediaPlayer, R);
	}	
	std::wstring Tmp = L"x" + DWL::Strings::ToStrF(R, 1);
	App.VentanaRave.LabelRatio.Texto(Tmp);
	App.ControlesPC.LabelRatio.Texto(Tmp);
}

/*
void RaveVLC::RepintarVLC(void) {
	if (hWndVLC != NULL) {
		Debug_Escribir_Varg(L"RepintarVLC %d\n", hWndVLC);
		HDC hDC = GetDC(hWndVLC);
		HBRUSH Brocha = (HBRUSH)GetStockObject(BLACK_BRUSH);
		RECT RC;
		GetClientRect(hWndVLC, &RC);
		FillRect(hDC, &RC, Brocha);
		ReleaseDC(hWndVLC, hDC);
	}
}*/


// Necesito encontrar la ventana del VLC que se crea dentro del VerVideo para repintar el fondo negro
BOOL CALLBACK RaveVLC::EnumeracionVLC(HWND hWndWnd, LPARAM lParam) {
	App.VLC.hWndVLC = hWndWnd;
//	EnableWindow(hWndWnd, FALSE);
	return FALSE;
}


	 
void RaveVLC::Brillo(const float nBrillo) {
	if (_MediaPlayer == NULL) return;
//	Debug_Escribir_Varg(L"RaveVLC::Brillo %02f\n", nBrillo);
	libvlc_video_set_adjust_int(_MediaPlayer, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_MediaPlayer, libvlc_adjust_Brightness, nBrillo);
}

void RaveVLC::Contraste(const float nContraste) {
	if (_MediaPlayer == NULL) return;
	libvlc_video_set_adjust_int(_MediaPlayer, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_MediaPlayer, libvlc_adjust_Contrast, nContraste);
}

void RaveVLC::Gamma(const float nGamma) {
	if (_MediaPlayer == NULL) return;
	libvlc_video_set_adjust_int(_MediaPlayer, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_MediaPlayer, libvlc_adjust_Gamma, nGamma);
}

void RaveVLC::Hue(const int nHue) {
	if (_MediaPlayer == NULL) return;
	libvlc_video_set_adjust_int(_MediaPlayer, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_int(_MediaPlayer, libvlc_adjust_Hue, nHue);
}

void RaveVLC::Saturacion(const float nSaturacion) {
	if (_MediaPlayer == NULL) return;
	libvlc_video_set_adjust_int(_MediaPlayer, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_MediaPlayer, libvlc_adjust_Saturation, nSaturacion);
}
