#include "stdafx.h"
#include "Rave_MediaPlayer.h"
#include "VentanaPrecarga.h"
#include "DStringUtils.h"

#define ID_TEMPORIZADOR_TIEMPO 11
#define ID_TEMPORIZADOR_LISTA  12

HWND		Rave_MediaPlayer::_hWndMensajes = NULL;
HWND		Rave_MediaPlayer::hWndVLC       = NULL;

Rave_Medio *Rave_MediaPlayer::_Anterior		= NULL;
Rave_Medio *Rave_MediaPlayer::_Actual		= NULL;
Rave_Medio *Rave_MediaPlayer::_Siguiente	= NULL;


Rave_MediaPlayer::Rave_MediaPlayer(void) :  _InstanciaVLC(NULL)
#ifdef RAVE_UTILIZAR_FMOD
											, _SistemaFMOD(NULL), _CanalMaster(NULL), _DSP(NULL) 
#endif
																									{
}


Rave_MediaPlayer::~Rave_MediaPlayer(void) {
}


const BOOL Rave_MediaPlayer::Iniciar(void) {
	// Inicio la ventana para los mensajes
	Debug_Escribir(L"Rave_MediaPlayer::Iniciar : Creando ventana para los mensajes\n");
	CrearVentana(NULL, L"Rave_MediaPlayer_Mensajes", L"", 0, 0, 1, 1, WS_POPUP);
	if (_hWnd == NULL) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR Creando la ventana para los mensajes.\n");
		Debug_MostrarUltimoError();
		return FALSE;
	}

	_hWndMensajes = _hWnd;

	// Inicio la VLC
	// On Microsoft Windows, setting the default DLL directories to SYSTEM32 exclusively is strongly recommended for security reasons:
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);
	// Ventana de precarga
	VentanaPrecarga Precarga;
	Debug_Escribir(L"Rave_MediaPlayer::Iniciar : Cargando LibVLC...\n");

	DWORD t = GetTickCount();
	_InstanciaVLC = libvlc_new(0, NULL);
	if (_InstanciaVLC == NULL) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR Cargando la LibVLC.\n");
		return FALSE;
	}
	const char *ErrorVLC = libvlc_errmsg();
	const char *Version = libvlc_get_version();
	std::wstring StrVersion;
	DWL::Strings::AnsiToWide(Version, StrVersion);
	Debug_Escribir_Varg(L"Rave_MediaPlayer::Iniciar : LibVLC cargada en %d MS, Version = '%s' \n", GetTickCount() - t, StrVersion.c_str());

	Precarga.Destruir();

#ifdef RAVE_UTILIZAR_FMOD
	// Cargo el FMOD
	FMOD_RESULT Ret = FMOD::System_Create(&_SistemaFMOD);
	if (Ret != FMOD_OK) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR Cargando el FMOD.\n");
		return FALSE;
		// Error fmod
	}
	// Inicio el Sistema FMOD
	Ret = _SistemaFMOD->init(16, FMOD_INIT_NORMAL, NULL);
	if (Ret != FMOD_OK) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR iniciando el FMOD.\n");
		return FALSE;
	}

	Ret = _SistemaFMOD->getMasterChannelGroup(&_CanalMaster);
	if (Ret != FMOD_OK) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR obteniendo el canal master en el FMOD.\n");
		return FALSE;
	}
	
	Ret = _SistemaFMOD->createDSPByType(FMOD_DSP_TYPE_FFT, &_DSP);
	if (Ret != FMOD_OK) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR creando el DSP con el FFT en el FMOD.\n");
		return FALSE;
	}

	Ret = _CanalMaster->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL, _DSP);
	if (Ret != FMOD_OK) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR añadiendo el DSP al canal master en el FMOD.\n");
		return FALSE;
	}

	//	_CanalMaster->getDSP()

	Debug_Escribir_Varg(L"Rave_MediaPlayer::Iniciar : FMOD cargado '%d'\n", _SistemaFMOD);
#endif

	// Creo los temporizadores
	SetTimer(_hWnd, ID_TEMPORIZADOR_TIEMPO, 250, NULL);
	SetTimer(_hWnd, ID_TEMPORIZADOR_LISTA, 500, NULL);

	return TRUE;
}

void Rave_MediaPlayer::ObtenerTTF(void) {
/*	int NumParams = 0;
	
	_DSP->getNumParameters(&NumParams);
	for (int i = 0; i < NumParams; i++) {
		_DSP->getParameterFloat(i, &TTF[i], NULL, 0);
	}		*/
	/*
	_SistemaFMOD->update();

	FMOD_RESULT Ret = FMOD_OK;
	FMOD_DSP_PARAMETER_FFT *fft = NULL;
	Ret = _DSP->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fft, 0, 0, 0);
	if (fft == NULL) 
		return;

	int pos = 0;
	for (int channel = 0; channel < fft->numchannels; channel++) {
		for (int bin = 0; bin < fft->length; bin++) {
			TTF[pos++] = fft->spectrum[channel][bin];
		}
	}*/
}


void Rave_MediaPlayer::Terminar(void) {
	// Elimino temporizadores
	KillTimer(_hWnd, ID_TEMPORIZADOR_TIEMPO);
	KillTimer(_hWnd, ID_TEMPORIZADOR_LISTA);

	// Elimino el VLC
	if (_InstanciaVLC != NULL) {
		libvlc_release(_InstanciaVLC);
		_InstanciaVLC = NULL;
	}

#ifdef RAVE_UTILIZAR_FMOD
	// Elimino el FMOD
	if (_SistemaFMOD != NULL) {
		_CanalMaster->removeDSP(_DSP);
		_DSP->release();
		_DSP = NULL;

		_SistemaFMOD->close();
		_SistemaFMOD->release();
		_SistemaFMOD = NULL;
	}
#endif
}

void Rave_MediaPlayer::Temporizador_Lista(void) {
	Estados_Medio Estado = ComprobarEstado();
	// Si no hay items salgo
	if (App.VentanaRave.Lista.TotalItems() == 0) return;

	// Si hay 10 errores en la lista, paro
	if (App.VentanaRave.Lista.Errores > 10) {
		App.VentanaRave.Lista_Stop();
		//App.VentanaRave.Lista.Errores = 0; // ya lo hace el stop
		return;
	}

#ifdef RAVE_UTILIZAR_FMOD
	_SistemaFMOD->update();
#endif
	// Un medio de la lista ha terminado
	if (Estado == SinCargar) {
		App.BD.MedioReproducido(&App.MP.MedioActual());
		// Se ha llegado al final de la lista
		if (App.VentanaRave.Lista.PosMedio(App.VentanaRave.Lista.MedioActual) == App.VentanaRave.Lista.TotalItems() - 1) {
			App.VentanaRave.Repeat();
		}
		// Siguiente medio
		else {
			App.VentanaRave.Lista_Siguiente();
		}
	}
}

void Rave_MediaPlayer::Temporizador_Tiempo(void) {
	Estados_Medio Estado	= ComprobarEstado();
	UINT64        TTotalMS	= TiempoTotalMs();
	UINT64        TActualMS = TiempoActualMs();
	ObtenerDatosParsing();
	if (!App.VentanaRave.Minimizado()) {
		std::wstring TmpStr;
		if (Estado == EnPlay || Estado == EnPausa) { // EnPlay y EnPausa
			float TActual = TiempoActual();
			
			// Si el slider del tiempo tiene la captura, es porque se esta modificando el tiempo, por lo que no hay que actualizar la posición en ese momento.
			if (App.VentanaRave.SliderTiempo.Estado() != DBarraEx_Estado_Presionado && App.ControlesPC.SliderTiempo.Estado() != DBarraEx_Estado_Presionado) {
				App.VentanaRave.SliderTiempo.Valor(TActual);
				//					SliderTiempo.Posicion(static_cast<UINT64>(App.VLC.TiempoActual() * 30000));
				App.ControlesPC.SliderTiempo.Valor(TActual);
				// Lo mismo pasa con el tiempo actual
				TiempoStr(TActualMS, TmpStr);
				App.VentanaRave.LabelTiempoActual.Texto(TmpStr);
				App.ControlesPC.LabelTiempoActual.Texto(TmpStr);
			}
			// El tiempo total solo se actualiza cuando es distinto al anterior			
			static UINT64 nTiempoTotal = 0;
			if (TTotalMS != nTiempoTotal) {
				TmpStr = L"";
				nTiempoTotal = TTotalMS;
				TiempoStr(TTotalMS, TmpStr);
				App.VentanaRave.LabelTiempoTotal.Texto(TmpStr);
				App.ControlesPC.LabelTiempoTotal.Texto(TmpStr);
			}
		}
		else { //if (Estado == EnStop || Estado == SinCargar || Estado == Abriendo || Estado == Terminada || Estado == EnError || Estado == Nada) {
			std::wstring Tiempo(L"00:00");
			App.VentanaRave.LabelTiempoActual.Texto(Tiempo);
			App.VentanaRave.LabelTiempoTotal.Texto(Tiempo);
			App.ControlesPC.LabelTiempoActual.Texto(Tiempo);
			App.ControlesPC.LabelTiempoTotal.Texto(Tiempo);
			// Si se ha terminado el medio asigno las barras del tiempo al máximo
			if (Estado == Terminada) {
				// Asigno el valor de las barras del tiempo a su máximo
				App.VentanaRave.SliderTiempo.Valor(App.VentanaRave.SliderTiempo.Maximo());
				App.ControlesPC.SliderTiempo.Valor(App.ControlesPC.SliderTiempo.Maximo());
			}
		}
	}



	// Tiempo para el fade in out
/*	if (Estado == EnPlay) {
		UINT64        TTotalMS  = TiempoTotalMs();
		UINT64        TActualMS = TiempoActualMs();

		if (TTotalMS < App.BD.Opciones_EfectoFadeAudioMS() || _Actual == NULL || _Siguiente == NULL) return;

		if (TTotalMS - App.BD.Opciones_EfectoFadeAudioMS() < TActualMS) {
			// cambio el medio actual
			App.VentanaRave.Lista.MedioActual = App.VentanaRave.Lista.MedioSiguiente(App.VentanaRave.Lista.MedioActual);


			_Siguiente->Volumen(0, FALSE);
			_Siguiente->Play();

			BDMedio NCan, NCan2;
			App.BD.ObtenerMedio(App.VentanaRave.Lista.MedioActual->Hash, NCan);
			ItemMedio *IMS = App.VentanaRave.Lista.MedioSiguiente(App.VentanaRave.Lista.MedioActual);
			if (IMS == NULL) {
				if (AbrirMedio(NCan, NULL) == FALSE) App.VentanaRave.Lista.Errores++;
			}
			else {
				App.BD.ObtenerMedio(IMS->Hash, NCan2);
				if (AbrirMedio(NCan, &NCan2) == FALSE) App.VentanaRave.Lista.Errores++;
			}
			// efecto fade in out
			if (_Anterior != NULL && _Actual != NULL) {
				Debug_Escribir(L"RaveVLC::Temporizador_Lista Fade in out\n");
				if (_Anterior->Medio.TipoMedio != Tipo_Medio_Video && _Actual->Medio.TipoMedio != Tipo_Medio_Video) {
					_Anterior->FadeOut();
					_Actual->FadeIn();
				}
			}

		}

	}*/
}



const BOOL Rave_MediaPlayer::AbrirMedio(BDMedio &Medio, BDMedio *MedioSiguiente) {
	if (_Anterior != NULL) _EliminarRaveMedio(_Anterior);
	// Si no es un video el medio actual pasa a ser el anterior
	if (Medio.TipoMedio != Tipo_Medio_Video) {
		_Anterior = _Actual;
	}
	// Si es un video, elimino el medio actual
	else {
		_Anterior = NULL;
		if (_Actual != NULL) _EliminarRaveMedio(_Actual);
	}

	// Existe un medio siguiente cargado y tiene la misma id que el medio actual
	if (_Siguiente != NULL) {
		if (_Siguiente->Medio.Id == Medio.Id) {
			_Actual = _Siguiente;
		}
		// El medio siguiente no se corresponde con el medio a cargar
		else {
			_EliminarRaveMedio(_Siguiente);
			_Siguiente = NULL;
			if (Medio.EsFMOD() == FALSE)	_Actual = new RaveVLC_Medio(_InstanciaVLC, Medio);
#ifdef RAVE_UTILIZAR_FMOD
			else                            _Actual = new RaveFMOD_Medio(_SistemaFMOD, Medio);
#endif
		}
	}
	// No hay un medio siguiente
	else {
		if (Medio.EsFMOD() == FALSE)	_Actual = new RaveVLC_Medio(_InstanciaVLC, Medio);
#ifdef RAVE_UTILIZAR_FMOD
		else                            _Actual = new RaveFMOD_Medio(_SistemaFMOD, Medio);
#endif
	}

	// Si el medio actual no es un video
	if (Medio.TipoMedio != Tipo_Medio_Video) {
		// Si el medio siguiente existe
		if (MedioSiguiente != NULL) {
			// Si el medio siguiente no es un video, cargo el medio siguiente.
			if (MedioSiguiente->TipoMedio != Tipo_Medio_Video) {
				if (Medio.EsFMOD() == FALSE)	_Siguiente = new RaveVLC_Medio(_InstanciaVLC, *MedioSiguiente);
#ifdef RAVE_UTILIZAR_FMOD
				else                            _Siguiente = new RaveFMOD_Medio(_SistemaFMOD, *MedioSiguiente);
#endif
			}
		}
	}

	if (_Actual->TxtError.size() == 0) return TRUE;
	else                               return FALSE;
}

void Rave_MediaPlayer::CerrarMedio(void) {
	if (_Anterior  != NULL)	_EliminarRaveMedio(_Anterior);
	if (_Actual    != NULL)	_EliminarRaveMedio(_Actual);
	if (_Siguiente != NULL) _EliminarRaveMedio(_Siguiente);

	_Anterior  = NULL;
	_Actual    = NULL;
	_Siguiente = NULL;
}

void Rave_MediaPlayer::_EliminarRaveMedio(Rave_Medio *eMedio) {
	switch (eMedio->Tipo()) {
		case Rave_Medio_Tipo_VLC  : delete static_cast<RaveVLC_Medio *>(eMedio);	break;
#ifdef RAVE_UTILIZAR_FMOD
		case Rave_Medio_Tipo_FMOD : delete static_cast<RaveFMOD_Medio *>(eMedio);	break;
#endif
		default                   : delete eMedio;									break;
	}
}


void Rave_MediaPlayer::TiempoStr(UINT64 TMS, std::wstring &StrTiempo) {
	StrTiempo.resize(0);
	unsigned int Horas    = static_cast<int>(((TMS / 1000) / 60) / 60);
	unsigned int Minutos  = static_cast<int>((TMS / 1000) / 60) - (Horas * 60);
	unsigned int Segundos = static_cast<int>(TMS / 1000) - (Minutos * 60) - ((Horas * 60) * 60);
	if (Horas > 0)    StrTiempo = std::to_wstring(Horas) + L":";
	if (Minutos > 9)  StrTiempo += std::to_wstring(Minutos) + L":";
	else              StrTiempo += L"0" + std::to_wstring(Minutos) + L":";
	if (Segundos > 9) StrTiempo += std::to_wstring(Segundos);
	else              StrTiempo += L"0" + std::to_wstring(Segundos);
}



std::wstring &Rave_MediaPlayer::UltimoErrorVLC(void) {
	static std::wstring TxtError;
	DWL::Strings::AnsiToWide(libvlc_errmsg(), TxtError);
	return TxtError;
}

void Rave_MediaPlayer::EventosVLC(const libvlc_event_t *event, void *ptr) {
	RaveVLC_Medio *MedioEvento = static_cast<RaveVLC_Medio *>(ptr);

	// Error
	if (event->type == libvlc_MediaPlayerEncounteredError) {
		PostMessage(_hWndMensajes, WM_MEDIO_TERMINADO, reinterpret_cast<WPARAM>(MedioEvento), 0);
		App.MostrarToolTipPlayerError(UltimoErrorVLC());
	}
	// Se ha terminado el medio
	else if (event->type == libvlc_MediaPlayerEndReached) {
		PostMessage(_hWndMensajes, WM_MEDIO_TERMINADO, reinterpret_cast<WPARAM>(MedioEvento), 0);
	}
	// Se ha parseado el medio (NO VA)
	else if (event->type == libvlc_MediaParsedChanged) {
		MedioEvento->ObtenerDatosParsing();
	}
}

#ifdef RAVE_UTILIZAR_FMOD
FMOD_RESULT F_CALLBACK Rave_MediaPlayer::EventosFMOD(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2) {
	if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END) {
		FMOD::Channel  *Canal       = (FMOD::Channel *)chanControl;
		RaveFMOD_Medio *MedioEvento = NULL;
		FMOD_RESULT     Error       = FMOD_OK;
		Error = Canal->getUserData((void **)&MedioEvento);
		if (Error == FMOD_OK && MedioEvento != NULL) {
			PostMessage(_hWndMensajes, WM_MEDIO_TERMINADO, reinterpret_cast<WPARAM>(MedioEvento), 0);
		}
	}
	return FMOD_OK;
}
#endif

void Rave_MediaPlayer::_TerminarMedio(Rave_Medio *MedioEvento) {
	if (_Anterior  == MedioEvento) _Anterior  = NULL;
	if (_Actual    == MedioEvento) _Actual	  = NULL; 
	if (_Siguiente == MedioEvento) _Siguiente = NULL; 
	MedioEvento->Eliminar();
	_EliminarRaveMedio(MedioEvento);
}



const BOOL Rave_MediaPlayer::Play(void) {
	if (_Actual == NULL) return FALSE;

	App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_Abajo);
	// Establezco la norma del tooltip según la alineación de los controles pantalla completa
	switch (App.ControlesPC.Alineacion) {
		case Abajo		:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Arriba);		break;
		case Arriba		:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Abajo);		break;
		case Izquierda	:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Derecha);		break;
		case Derecha	:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Izquierda);	break;
	}

	// Asigno el titulo de la ventana con el nombre del medio que se acaba de abrir
	std::wstring nTitulo = std::wstring(RAVE_TITULO) + L" [" + DWL::Strings::ToStr(_Actual->Medio.Pista(), 2) + L" " + _Actual->Medio.Nombre() + L"]";
	App.VentanaRave.Titulo(nTitulo);

	return _Actual->Play();
}

const BOOL Rave_MediaPlayer::Pausa(void) {
	if (_Actual == NULL) return FALSE;
	return _Actual->Pausa();
}

const BOOL Rave_MediaPlayer::Stop(void) {
	if (_Actual == NULL) return FALSE;

	// Escondo los tooltip de las barras de tiempo
	App.VentanaRave.SliderTiempo.OcultarToolTip();
	App.ControlesPC.SliderTiempo.OcultarToolTip();
	// Evito mostrar el tooltip si se pasa el mouse por encima del slider tiempo
	App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
	// Elimino el nombre del medio en el titulo
	App.VentanaRave.Titulo(std::wstring(RAVE_TITULO));

	return _Actual->Stop();
}

const BOOL Rave_MediaPlayer::StopTODO(void) {
	if (_Anterior  != NULL) _Anterior->Stop();
	if (_Actual    != NULL) _Actual->Stop();
	if (_Siguiente != NULL) _Siguiente->Stop();
	return TRUE;
}

const int Rave_MediaPlayer::Volumen(void) {
	if (_Actual == NULL) return App.BD.Opciones_Volumen();
	return _Actual->Volumen();
}

void Rave_MediaPlayer::Volumen(int nVolumen) {
	if (_Actual == NULL) return;
	_Actual->Volumen(nVolumen);
}

Estados_Medio Rave_MediaPlayer::ComprobarEstado(void) {
	if (_Actual == NULL) return SinCargar;
	return _Actual->ComprobarEstado();
}


const float Rave_MediaPlayer::TiempoActual(void) {
	if (_Actual == NULL) return 0.0f;
	return _Actual->TiempoActual();
}

void Rave_MediaPlayer::TiempoActual(float nTiempo) {
	if (_Actual == NULL) return;
	_Actual->TiempoActual(nTiempo);
}

const UINT64 Rave_MediaPlayer::TiempoTotalMs(void) {
	if (_Actual == NULL) return 0;
	return _Actual->TiempoTotalMs();
}

const UINT64 Rave_MediaPlayer::TiempoActualMs(void) {
	if (_Actual == NULL) return 0;
	return _Actual->TiempoActualMs();
}

void Rave_MediaPlayer::Ratio(const float R) {
	if (_Actual == NULL) return;
	_Actual->Ratio(R);
}

std::wstring &Rave_MediaPlayer::ObtenerProporcion(void) {
	static std::wstring Ret = L"Predeterminado";
	if (_Actual == NULL) return Ret;
	return _Actual->ObtenerProporcion();
}

void Rave_MediaPlayer::AsignarProporcion(const char *Prop) {
	if (_Actual == NULL) return;
	_Actual->AsignarProporcion(Prop);
}

void Rave_MediaPlayer::Brillo(const float nBrillo) {
	if (_Actual == NULL) return;
	_Actual->Brillo(nBrillo);
}

void Rave_MediaPlayer::Contraste(const float nContraste) {
	if (_Actual == NULL) return;
	_Actual->Contraste(nContraste);
}

void Rave_MediaPlayer::Gamma(const float nGamma) {
	if (_Actual == NULL) return;
	_Actual->Gamma(nGamma);
}

void Rave_MediaPlayer::Hue(const int nHue) {
	if (_Actual == NULL) return;
	_Actual->Hue(nHue);
}

void Rave_MediaPlayer::Saturacion(const float nSaturacion) {
	if (_Actual == NULL) return;
	_Actual->Saturacion(nSaturacion);
}

BDMedio &Rave_MediaPlayer::MedioActual(void) {
	static BDMedio Vacio;
	if (_Actual == NULL) return Vacio;
	return _Actual->Medio;
}

const BOOL Rave_MediaPlayer::ObtenerDatosParsing(void) {
	if (_Actual == NULL) return FALSE;
	return _Actual->ObtenerDatosParsing();
}

void Rave_MediaPlayer::AsignarPistaAudio(const int nPista) {
	if (_Actual == NULL) return;
	_Actual->AsignarPistaAudio(nPista);
}

void Rave_MediaPlayer::Temporizador_ObtenerVentanaVLC(void) {
	EnumChildWindows(App.VentanaRave.Video.hWnd(), &Rave_MediaPlayer::EnumeracionVLC, NULL);
	if (App.MP.hWndVLC != NULL) KillTimer(hWnd(), TIMER_OBTENERVLCWND);
}

// Necesito encontrar la ventana del VLC que se crea dentro del VerVideo para repintar el fondo negro
BOOL CALLBACK Rave_MediaPlayer::EnumeracionVLC(HWND hWndWnd, LPARAM lParam) {
	hWndVLC = hWndWnd;
	return FALSE;
}


LRESULT CALLBACK Rave_MediaPlayer::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_TIMER :
			switch (static_cast<UINT>(wParam)) {
				case TIMER_OBTENERVLCWND    : PostMessage(_hWnd, WM_TIMER_OBTENERHWNDVLC, 0, 0);	return 0;
				case ID_TEMPORIZADOR_TIEMPO : PostMessage(_hWnd, WM_TIMER_TIEMPO, 0, 0);			return 0;
				case ID_TEMPORIZADOR_LISTA  : PostMessage(_hWnd, WM_TIMER_LISTA, 0, 0);				return 0;
			}
			break;
		case WM_MEDIO_TERMINADO :
			_TerminarMedio(reinterpret_cast<Rave_Medio *>(wParam));
			return 0;
		case WM_TIMER_LISTA :
			Temporizador_Lista();
			return 0;
		case WM_TIMER_TIEMPO :
			Temporizador_Tiempo();
			return 0;
		case WM_TIMER_OBTENERHWNDVLC :
			Temporizador_ObtenerVentanaVLC();
			return 0;
	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}