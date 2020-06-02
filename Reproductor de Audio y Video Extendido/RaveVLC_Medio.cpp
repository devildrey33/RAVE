#include "stdafx.h"
#include "RaveVLC_Medio.h"
#include "DStringUtils.h"
#include "RAVE_Iconos.h"
//#include "RaveVLC.h"
#include "Rave_MediaPlayer.h"


RaveVLC_Medio::RaveVLC_Medio(libvlc_instance_t *Instancia, ItemMedio *nMedio) : Rave_Medio(nMedio), _Medio(NULL), _Eventos(NULL), _Parseado(FALSE), _Instancia(Instancia) /*, _Volumen(nullptr)*/ {

	App.MenuVideoFiltros->Menu(0)->BarraValor(Medio->BdMedio.Brillo);		// Brillo
	App.MenuVideoFiltros->Menu(1)->BarraValor(Medio->BdMedio.Contraste);	// Contraste
	App.MenuVideoFiltros->Menu(2)->BarraValor(Medio->BdMedio.Saturacion);	// Saturación

//	Medio.Path = L"https://livestartover.atresmedia.com/lasexta/master.m3u8";
//	Medio.Path = L"E:\\mp3\\ViRGiN STeeLe\\The Book Of Burining\\The Book Of Burining - copia.m3u";

	//std::wstring TxtError;
	Ubicacion_Medio Ubicacion = Medio->BdMedio.Ubicacion();
	if (Ubicacion != Ubicacion_Medio_Internet) {
		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(Medio->BdMedio.Path.c_str())) {
			Debug_Escribir_Varg(L"RaveVLC_Medio::RaveVLC_Medio  El archivo '%s' NO EXISTE!\n", Medio->BdMedio.Path.c_str());
			TxtError = L"El archivo '" + Medio->BdMedio.Path + L"' NO EXISTE!";
			App.MostrarToolTipPlayerError(TxtError);
			return;
		}
	}

	std::string PathUTF8;
	// Hay que convertir el path a UTF8 para que funcione en el VLC...
	if (DWL::Strings::WideToUTF8(Medio->BdMedio.Path.c_str(), PathUTF8) == 0) {
		Debug_MostrarUltimoError();
		TxtError = L"Error al convertir el string '" + Medio->BdMedio.Path + L"'";
		App.MostrarToolTipPlayerError(TxtError);
		return;
	}
	
	libvlc_media_t *_Media = NULL;
	if (Ubicacion == Ubicacion_Medio_Internet)	_Media = libvlc_media_new_location(Instancia, PathUTF8.c_str());
	else										_Media = libvlc_media_new_path(Instancia, PathUTF8.c_str());

	if (_Media == NULL) {
		Debug_Escribir_Varg(L"RaveVLC_Medio::RaveVLC_Medio  Error al abrir '%s'\n", Medio->BdMedio.Path.c_str());
		TxtError = L"Error al abrir '" + Medio->BdMedio.Path + L"'";
		App.MostrarToolTipPlayerError(TxtError);
		return;
	}
	_Medio = libvlc_media_player_new_from_media(_Media);

	libvlc_media_release(_Media);

	_Eventos = libvlc_media_player_event_manager(_Medio);
	//	libvlc_event_attach(_Eventos, libvlc_MediaStateChanged, EventosVLC, this);				// no va
	//	libvlc_event_attach(_Eventos, libvlc_MediaDurationChanged, EventosVLC, this);			// no va (igual amb streaming si...)
	libvlc_event_attach(_Eventos, libvlc_MediaPlayerEndReached			, Rave_MediaPlayer::EventosVLC, this);
	libvlc_event_attach(_Eventos, libvlc_MediaPlayerEncounteredError	, Rave_MediaPlayer::EventosVLC, this);	// errors
	libvlc_event_attach(_Eventos, libvlc_MediaParsedChanged				, Rave_MediaPlayer::EventosVLC, this);	// no va
	libvlc_event_attach(_Eventos, libvlc_MediaPlayerPlaying				, Rave_MediaPlayer::EventosVLC, this);	// para detectar cuando empieza a reproducirse (necesario para los m3us iptv)
	libvlc_event_attach(_Eventos, libvlc_MediaSubItemAdded				, Rave_MediaPlayer::EventosVLC, this);	// 
	libvlc_event_attach(_Eventos, libvlc_MediaSubItemTreeAdded			, Rave_MediaPlayer::EventosVLC, this);	// 
	libvlc_event_attach(_Eventos, libvlc_MediaPlayerVout				, Rave_MediaPlayer::EventosVLC, this);	// 
	
	// Desactiva los eventos del mouse y del teclado dentro de la ventana del vlc 
	libvlc_video_set_mouse_input(_Medio, false);
	libvlc_video_set_key_input(_Medio, false);

	Volumen(static_cast<int>(App.VentanaRave.SliderVolumen.Valor()));
	

	// Si es un video o una iptv
	if (Medio->BdMedio.TipoMedio == Tipo_Medio_Video || Medio->BdMedio.Tiempo == Tipo_Medio_IpTv) {
		// Pulso el botón para mostrar el video
		if (App.VentanaRave.PantallaCompleta() == FALSE) {
			App.VentanaRave.MostrarMarco(ID_BOTON_VIDEO);
//			App.VentanaRave.Evento_BotonEx_Mouse_Click(DEventoMouse(0, 0, &App.VentanaRave.BotonVideo, 0));
		}

		// Cargo los valores de brillo, proporción, contraste, y saturación
		if (App.Opciones.GuardarBSCP() == TRUE) {
			if (Medio->BdMedio.Proporcion.size() != 0)	AsignarProporcion(Medio->BdMedio.Proporcion.c_str());
			if (Medio->BdMedio.Brillo != 1.0)			Brillo(Medio->BdMedio.Brillo);
			if (Medio->BdMedio.Contraste != 1.0)		Contraste(Medio->BdMedio.Contraste);
			if (Medio->BdMedio.Saturacion != 1.0)		Saturacion(Medio->BdMedio.Saturacion);
		}

	}
	else {
		libvlc_media_player_set_hwnd(_Medio, NULL);
	}

//	ComprobarMomento();

	Debug_Escribir_Varg(L"RaveVLC_Medio::RaveVLC_Medio Path '%s'\n", Medio->BdMedio.Path.c_str());
}


RaveVLC_Medio::~RaveVLC_Medio(void) {
	Eliminar();
}

// OJU s'ha d'utilitzar sempre desde el thread principal o pot fer un deadlock
void RaveVLC_Medio::Eliminar(void) {
	//	hWndVLC = NULL;
	_AniVolumen.Terminar();

	if (_Medio != NULL) {
//		Debug_Escribir_Varg(L"RaveVLC_Medio::~RaveVLC_Medio '%s' \n", Medio->BdMedio.Path.c_str());
		libvlc_event_detach(_Eventos, libvlc_MediaPlayerEndReached		, Rave_MediaPlayer::EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaPlayerEncounteredError, Rave_MediaPlayer::EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaParsedChanged			, Rave_MediaPlayer::EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaPlayerPlaying			, Rave_MediaPlayer::EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaSubItemAdded			, Rave_MediaPlayer::EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaSubItemTreeAdded		, Rave_MediaPlayer::EventosVLC, this);
		libvlc_event_detach(_Eventos, libvlc_MediaPlayerVout			, Rave_MediaPlayer::EventosVLC, this);

		Stop();
		libvlc_media_player_release(_Medio);

		// Renombro archivos CRDOWNLOAD y OPDOWNLOAD una vez cerrados
/*		if (Medio->BdMedio.Extension == Extension_CRDOWNLOAD || Medio->BdMedio.Extension == Extension_OPDOWNLOAD) {
			size_t PosExtension = Medio->BdMedio.Path.find_last_of(TEXT("."));																				// Posición donde empieza la extensión
			std::wstring NuevoPath = Medio->BdMedio.Path.substr(0, PosExtension);
			if (MoveFile(Medio->BdMedio.Path.c_str(), NuevoPath.c_str()) != FALSE) {
				App.BD.ActualizarPathMedio(NuevoPath, Medio->BdMedio.Id);
			}
		}*/

		_Medio = NULL;
	}
}

// OJU s'ha d'utilitzar sempre desde el thread principal o pot fer un deadlock
const BOOL RaveVLC_Medio::Stop(void) {
	if (_Medio != NULL) {
		BOOL Video = FALSE;
		Debug_Escribir_Varg(L"RaveVLC_Medio::Stop %d\n", _Medio);

		_AniVolumen.Terminar();

//		ActualizarIconos(0);

		// Escondo los tooltip de las barras de tiempo
/*		App.VentanaRave.SliderTiempo.OcultarToolTip();
		App.ControlesPC.SliderTiempo.OcultarToolTip();
		// Evito mostrar el tooltip si se pasa el mouse por encima del slider tiempo
		App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
		App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);*/

		
		// Deadlock just despres de mostrar un DMenuEx en un video i utilitzar el stop.... SOLUCIONAT, pero s'ha d'anar amb cuidado al utilitzar SetFocus...
		if (libvlc_media_player_has_vout(_Medio) > 0) {
			// Para evitar un deadlock si se está reproduciendo un video y el foco está en otra parte
			HWND Foco = SetFocus(App.VentanaRave.hWnd()); // REVISADO
			Video = TRUE;
		}
		// Desactivo los filtros que pueda haber activados para evitar un crash en la versión x86
//		libvlc_video_set_adjust_int(_Medio, libvlc_adjust_Enable, 0);
		libvlc_media_player_stop(_Medio);

		//hWndVLC = NULL;

		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, TRUE); // Activo protector de pantalla

		// Elimino la región del clip
		if (Video == TRUE) {
			RECT RC;
			GetClientRect(App.VentanaRave.hWnd(), &RC);
			BOOL R = App.VentanaRave.BarraTareas.Clip(&RC);
			Debug_Escribir_Varg(L"BarraTareasClip : T%d, L%d, B%d, R%d, BOOL:%d\n", RC.top, RC.left, RC.bottom, RC.right, R);
		}
		
		// Asigno el titulo de la ventana con el nombre del medio que se acaba de abrir
//		std::wstring nTitulo = std::wstring(RAVE_TITULO) + L" - " +  MedioActual.Nombre();
//		App.VentanaRave.Titulo(std::wstring(RAVE_TITULO));

		return TRUE;
	}
	return FALSE;
}

const BOOL RaveVLC_Medio::Pausa(void) {

	if (_Medio != NULL) {
		if (ComprobarEstado() == EnPlay) {
//			ActualizarIconos(2);
			libvlc_media_player_pause(_Medio);
		}
		return TRUE;
	}
	return FALSE;
}

const BOOL RaveVLC_Medio::Play(void) {
	if (_Medio != NULL) {
		if (libvlc_media_player_play(_Medio) == 0)	{
//			ActualizarIconos(1);


			if (Medio->BdMedio.TipoMedio == Tipo_Medio_Video || Medio->BdMedio.TipoMedio == Tipo_Medio_IpTv) { // Desactivo el protector de pantalla si es un video
				SetTimer(Rave_MediaPlayer::_hWndMensajes, TIMER_OBTENERVLCWND, 100, NULL);
				SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, TRUE);
			}

			// Muestro el tooltip con los datos
//			App.MostrarToolTipPlayer(Medio);

			
//			App.BD.AgregarMomento(&App.MP.MedioActual(), std::wstring(L"MomentoTest"), 0, 10000, FALSE); // TEST INICIAL
			return TRUE;
		}
	}
	return FALSE;	
}



// 0 - 200
void RaveVLC_Medio::Volumen(const long rVolumen, const BOOL ActualizarUI) {
	long nVolumen = rVolumen;
	// Topes
	if (nVolumen > 200) nVolumen = 200;
	if (nVolumen < 0)	nVolumen = 0;

	Debug_Escribir_Varg(L"RaveVLC_Medio::Volumen Vol : %d\n",  nVolumen);

	if (ActualizarUI == TRUE) {
		App.VentanaRave.SliderVolumen.Valor(static_cast<float>(nVolumen));
		App.ControlesPC.SliderVolumen.Valor(static_cast<float>(nVolumen));
		std::wstring StrVol = std::to_wstring(nVolumen) + L"%";
		App.VentanaRave.LabelVolumen.Texto(StrVol);
		App.ControlesPC.LabelVolumen.Texto(StrVol);
	}

	if (_Medio != NULL) {
		libvlc_audio_set_volume(_Medio, nVolumen);
	}
}

const int RaveVLC_Medio::Volumen(void) {
	if (_Medio != NULL) {
		return libvlc_audio_get_volume(_Medio);
	}
	return 0;
}


/* nTipo :
	0 - Normal
	1 - Play
	2 - Pausa
*//*
void RaveVLC_Medio::ActualizarIconos(int nTipo) {
	if (Medio.Hash != 0) {
		int nIcono = 0;
		switch (nTipo) {
			case 0: // normal
				if (Medio.TipoMedio == Tipo_Medio::Tipo_Medio_Audio) nIcono = RAVE_Iconos::RAVE_Icono_Cancion; // IDI_CANCION
				if (Medio.TipoMedio == Tipo_Medio::Tipo_Medio_Video) nIcono = RAVE_Iconos::RAVE_Icono_Video; // IDI_VIDEO
				break;
			case 1:
				nIcono = RAVE_Iconos::RAVE_Icono_Play; // play
				break;
			case 2:
				nIcono = RAVE_Iconos::RAVE_Icono_Pausa; // Pausa
				break;
		}
		// Actualizo el nodo del ArbolBD
		NodoBD *Nodo = App.VentanaRave.Arbol.BuscarHash(Medio.Hash);
		if (Nodo != NULL)
			Nodo->Icono(nIcono);
		App.VentanaRave.Arbol.Repintar();
		// Actualizo el Item del ListaMedios
		ItemMedio *Item = App.VentanaRave.Lista.BuscarHash(Medio.Hash);
		if (Item != NULL) {
			Item->Icono(nIcono);
			App.VentanaRave.Lista.MostrarItem(Item);
		}
		App.VentanaRave.Lista.Repintar();
	}
}
*/


std::wstring &RaveVLC_Medio::ObtenerProporcion(void) {
	static std::wstring Ret = L"Predeterminado";
	char *Tmp = libvlc_video_get_aspect_ratio(_Medio);
	if (Tmp != NULL) {
		DWL::Strings::UTF8ToWide(Tmp, Ret);
		libvlc_free(Tmp);
	}
	return Ret;
}

void RaveVLC_Medio::AsignarProporcion(const char* Prop) {
	libvlc_video_set_aspect_ratio(_Medio, Prop);
	if (Prop == NULL) Medio->BdMedio.Proporcion.resize(0);
	else              DWL::Strings::UTF8ToWide(Prop, Medio->BdMedio.Proporcion);
}

void RaveVLC_Medio::AsignarProporcion(const wchar_t* Prop) {
	std::string Ansi;
	DWL::Strings::WideToAnsi(Prop, Ansi);
	libvlc_video_set_aspect_ratio(_Medio, Ansi.c_str());
	Medio->BdMedio.Proporcion = Prop;
}


/*void RaveVLC_Medio::AsignarPistaAudio(const int nPista) {
	libvlc_audio_set_track(_MediaPlayer, nPista);
}*/

const Estados_Medio RaveVLC_Medio::ComprobarEstado(void) {
	if (_Medio == NULL) 
		return SinCargar;
	libvlc_state_t Estado = libvlc_media_player_get_state(_Medio);
	switch (Estado) {
		case libvlc_NothingSpecial:	return Nada;
		case libvlc_Opening:		return Abriendo;
		case libvlc_Buffering:		return Cargando;
		case libvlc_Playing:		return EnPlay;
		case libvlc_Stopped:		return EnStop;
		case libvlc_Paused:			return EnPausa;
		case libvlc_Ended:			return Terminada;
		case libvlc_Error:			return EnError;
	}
	return SinCargar;
}



const UINT64 RaveVLC_Medio::TiempoTotalMs(void) {
	if (_Medio != NULL) {
		return static_cast<UINT64>(libvlc_media_player_get_length(_Medio));
	}
	return 0;
}

const UINT64 RaveVLC_Medio::TiempoActualMs(void) {
	if (_Medio != NULL) {
		return static_cast<UINT64>(libvlc_media_player_get_time(_Medio));
	}
	return 0;
}


const float RaveVLC_Medio::TiempoActual(void) {
	if (_Medio != NULL) {
		return libvlc_media_player_get_position(_Medio);
	}
	return 0.0f;
}

void RaveVLC_Medio::TiempoActual(float nTiempo) {
	if (_Medio != NULL) {
		// Si el tick de la ultima vez que se salio de pantalla completa + 100 es menor que el ultimo tick
		if (_SalirPantallaCompleta + 500 < GetTickCount64())	libvlc_media_player_set_position(_Medio, nTiempo);
	}
}

void RaveVLC_Medio::TiempoActualMs(UINT64 nTiempo) {
	if (_Medio != NULL) {
		// Si el tick de la ultima vez que se salio de pantalla completa + 100 es menor que el ultimo tick
		if (_SalirPantallaCompleta + 500 < GetTickCount64())	libvlc_media_player_set_time(_Medio, nTiempo);
	}
}

const BOOL RaveVLC_Medio::Ratio(const float R) {
	int r;
	if (_Medio != NULL) {
		r = libvlc_media_player_set_rate(_Medio, R);
	}	
	std::wstring Tmp = L"x" + DWL::Strings::ToStrF(R, 1);
	App.VentanaRave.LabelRatio.Texto(Tmp);
	App.ControlesPC.LabelRatio.Texto(Tmp);
	return TRUE;
}


void RaveVLC_Medio::Brillo(const float nBrillo) {
	if (_Medio == NULL) return;
	//	Debug_Escribir_Varg(L"RaveVLC::Brillo %02f\n", nBrillo);
	libvlc_video_set_adjust_int(_Medio, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_Medio, libvlc_adjust_Brightness, nBrillo);
	Medio->BdMedio.Brillo = nBrillo;
	App.MenuVideoFiltros->Menu(0)->BarraValor(nBrillo);
}

void RaveVLC_Medio::Contraste(const float nContraste) {
	if (_Medio == NULL) return;
	libvlc_video_set_adjust_int(_Medio, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_Medio, libvlc_adjust_Contrast, nContraste);
	Medio->BdMedio.Contraste = nContraste;
	App.MenuVideoFiltros->Menu(1)->BarraValor(nContraste);
}

void RaveVLC_Medio::Gamma(const float nGamma) {
	if (_Medio == NULL) return;
	libvlc_video_set_adjust_int(_Medio, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_Medio, libvlc_adjust_Gamma, nGamma);
}

void RaveVLC_Medio::Hue(const int nHue) {
	if (_Medio == NULL) return;
	libvlc_video_set_adjust_int(_Medio, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_int(_Medio, libvlc_adjust_Hue, nHue);
}

void RaveVLC_Medio::Saturacion(const float nSaturacion) {
	if (_Medio == NULL) return;
	libvlc_video_set_adjust_int(_Medio, libvlc_adjust_Enable, 1);
	libvlc_video_set_adjust_float(_Medio, libvlc_adjust_Saturation, nSaturacion);
	Medio->BdMedio.Saturacion = nSaturacion;
	App.MenuVideoFiltros->Menu(2)->BarraValor(nSaturacion);
}


// Obtiene los datos del medio una vez parseado (despues del play)
const BOOL RaveVLC_Medio::ObtenerDatosParsing(void) {
	if (_Medio == NULL) return FALSE;
	libvlc_state_t nEstado = libvlc_media_player_get_state(_Medio);
	std::wstring   Texto;

	// Si no se ha parseado
	if (_Parseado == FALSE && nEstado == libvlc_Playing) {
		_Parseado = TRUE;
		// Enumero las pistas de audio
		int TotalPîstas = libvlc_audio_get_track_count(_Medio);
		if (TotalPîstas > 2) { // si hay mas de una pista, las enumero
			App.MenuVideoPistasDeAudio->Activado(TRUE);

			libvlc_track_description_t *Desc = libvlc_audio_get_track_description(_Medio);

			App.MenuVideoPistasDeAudio->EliminarTodosLosMenus();
			for (int i = 0; i < TotalPîstas; i++) {
				if (i != 0) {
					DWL::Strings::UTF8ToWide(Desc->psz_name, Texto);
					App.MenuVideoPistasDeAudio->AgregarMenu(static_cast<INT_PTR>(ID_MENUVIDEO_AUDIO_PISTAS_AUDIO) + i, Texto);
				}
				Desc = Desc->p_next;
			}
			int PistaActual = libvlc_audio_get_track(_Medio);
			DMenuEx *MenuPistaActual = App.MenuVideoPistasDeAudio->BuscarMenu(static_cast<INT_PTR>(ID_MENUVIDEO_AUDIO_PISTAS_AUDIO) + PistaActual);
			if (MenuPistaActual != NULL) MenuPistaActual->Icono(IDI_CHECK2);
			libvlc_track_description_list_release(Desc);
		}
		else {	// Si solo hay una pista de audio desactivo el menú
			App.MenuVideoPistasDeAudio->Activado(FALSE);
		}

		// Enumero los subtitulos
		EnumerarSubtitulos();

		BOOL EsVideo = (Medio->BdMedio.TipoMedio == Tipo_Medio_Video || Medio->BdMedio.TipoMedio == Tipo_Medio_IpTv);

		// Activo / desactivo el menú de la proporción según el tipo de medio (audio / video)
		App.MenuVideoProporcion->Activado(EsVideo);
		App.MenuVideoFiltros->Activado(EsVideo);
		App.MenuVideoSubtitulos->Activado(EsVideo);

		// Obtengo el ratio de aspecto
/*		std::wstring Proporcion = ObtenerProporcion();
		for (size_t i = 0; i < App.MenuVideoProporcion->TotalMenus(); i++) {
			if (App.MenuVideoProporcion->Menu(i)->Texto() == Proporcion)	App.MenuVideoProporcion->Menu(i)->Icono(IDI_CHECK2);
			else															App.MenuVideoProporcion->Menu(i)->Icono(0);
		}*/
		return TRUE;
	}
	return FALSE;
}

const int RaveVLC_Medio::EnumerarSubtitulos(void) {
	// Enumero los subtitulos
	int          TotalSubtitulos = libvlc_video_get_spu_count(_Medio);
	std::wstring Texto;
	DMenuEx     *TmpMenu = NULL;
	libvlc_track_description_t* Desc2 = libvlc_video_get_spu_description(_Medio);
	for (int i = 0; i < TotalSubtitulos; i++) {
		DWL::Strings::UTF8ToWide(Desc2->psz_name, Texto);
		TmpMenu = App.MenuVideoSubtitulos->AgregarMenu(static_cast<INT_PTR>(ID_MENUVIDEO_SUBTITULOS_SUBS) + i, Texto);
		TmpMenu->Parametro = Desc2->i_id;
		Desc2 = Desc2->p_next;
	}
	libvlc_track_description_list_release(Desc2);
	return TotalSubtitulos;
};

void RaveVLC_Medio::AsignarPistaAudio(const int nPista) {
	libvlc_audio_set_track(_Medio, nPista);
}

const int RaveVLC_Medio::AsignarSubtitulos(const wchar_t* Path) {
	char	Destino[2048];
	size_t  TamnTexto = wcslen(Path);
	int		TamRes = WideCharToMultiByte(CP_UTF8, NULL, Path, static_cast<int>(TamnTexto), Destino, 2048, NULL, NULL);
	Destino[TamRes] = 0;
	// Segons doc de la VLC libvlc_video_set_subtitle_file está DEPRECATED i s'ha d'utilitzar libvlc_media_player_add_slave, PERO a mi no em funciona...
	int R = libvlc_media_player_add_slave(_Medio, libvlc_media_slave_type_subtitle , Destino, false);

//	EnumerarSubtitulos();
	return R;
//	return libvlc_video_set_spu(_Medio, 0);		
//	return libvlc_video_set_subtitle_file(_Medio, Destino);
}





///////////////////////////////////////////////////////
// TODO : moure fade in i fade out a Rave_MediaPlayer
///////////////////////////////////////////////////////

// de 0 al volumen actual
void RaveVLC_Medio::FadeIn(void) {
	_AniVolumen.Terminar();
//	static double *Vol = (double *)&App.Opciones._Volumen;
//	static long *vol = (long *)& App.Opciones._Volumen;

//	_Volumen = (long *)&App.Opciones._Volumen;

	_AniVolumen.Iniciar({ 0 }, { App.Opciones.Volumen() }, App.Opciones.EfectoFadeAudioMS(), [=](DWL::DAnimacion::Valores& Valores, const BOOL Terminado) {
		Volumen(Valores[0].Entero(), FALSE);
	}, { DWL::DAnimacion::FuncionesTiempo::Linear }, 100);
}

// del volumen actual a 0
void RaveVLC_Medio::FadeOut(void) {
	static long NuevoVolumen = 0;
	_AniVolumen.Terminar();
	_AniVolumen.Iniciar({ App.Opciones.Volumen() }, { NuevoVolumen }, App.Opciones.EfectoFadeAudioMS(), [=](DWL::DAnimacion::Valores& Valores, const BOOL Terminado) {
		Volumen(Valores[0].Entero(), FALSE);
	}, { DWL::DAnimacion::FuncionesTiempo::Linear }, 100);

}
