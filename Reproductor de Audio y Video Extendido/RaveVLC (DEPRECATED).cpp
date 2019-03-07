#include "stdafx.h"
#include "RaveVLC (DEPRECATED).h"
#include "VentanaPrecarga.h"
#include "DStringUtils.h"

RaveVLC_Medio *RaveVLC::_Anterior  = NULL;
RaveVLC_Medio *RaveVLC::_Actual    = NULL;
RaveVLC_Medio *RaveVLC::_Siguiente = NULL;
HWND           RaveVLC::hWndVLC    = 0;

#define ID_TEMPORIZADOR_TIEMPO 11
#define ID_TEMPORIZADOR_LISTA  12

RaveVLC::RaveVLC(void) :_Instancia(NULL) {
}


RaveVLC::~RaveVLC(void) {
}

const BOOL RaveVLC::Iniciar(void) {
	// Ventana de precarga
	VentanaPrecarga Precarga;
	Debug_Escribir_Varg(L"RaveVLC::Iniciar Cargando LibVLC...\n", _Instancia);
	
	DWORD t = GetTickCount();
	_Instancia = libvlc_new(0, NULL);
	const char *ErrorVLC = libvlc_errmsg();
	const char *Version  = libvlc_get_version();
	std::wstring StrVersion;
	DWL::Strings::AnsiToWide(Version, StrVersion);
	Debug_Escribir_Varg(L"RaveVLC::Iniciar Cargado en %d MS, Version = '%s' \n", GetTickCount() - t, StrVersion.c_str());
	
	Precarga.Destruir();	

	static HWND hWndRave = App.VentanaRave.hWnd();

	CrearTemporizador(250, FALSE, [=]() { 	Temporizador_Tiempo();	});
	CrearTemporizador(500, FALSE, [=]() { 	PostMessage(hWndRave, WM_TIMER_LISTA, 0, 0); 	});

	

	return TRUE;
}

void RaveVLC::Terminar(void) {
	Debug_Escribir(L"RaveVLC::Terminar\n");

	TerminarTemporizadores();

	CerrarMedio();

	if (_Instancia != NULL) {
		libvlc_release(_Instancia);
		_Instancia = NULL;
	}

//	hWndVLC = NULL;
	Debug_Escribir(L"RaveVLC::Terminar TERMINADO\n");
}

std::wstring &RaveVLC::UltimoError(void) {
	static std::wstring TxtError;
	DWL::Strings::AnsiToWide(libvlc_errmsg(), TxtError);
	return TxtError;
}

const BOOL RaveVLC::AbrirMedio(BDMedio &Medio, BDMedio *MedioSiguiente) {
	if (_Anterior != NULL) delete _Anterior;
	// Si no es un video el medio actual pasa a ser el anterior
	if (Medio.TipoMedio != Tipo_Medio_Video) {
		_Anterior = _Actual;
	}
	// Si es un video, elimino el medio actual
	else {
		_Anterior = NULL;
		if (_Actual != NULL) delete _Actual;
	}

	// Existe un medio siguiente cargado y tiene la misma id que el medio actual
	if (_Siguiente != NULL) {
		if (_Siguiente->Medio.Id == Medio.Id) {
			_Actual = _Siguiente;
		}
		// El medio siguiente no se corresponde con el medio a cargar
		else {
			delete _Siguiente;
			_Siguiente = NULL;
			_Actual = new RaveVLC_Medio(_Instancia, Medio);
		}
	}
	// No hay un medio siguiente
	else {		
		_Actual = new RaveVLC_Medio(_Instancia, Medio);
	}

	// Si el medio actual no es un video
	if (Medio.TipoMedio != Tipo_Medio_Video) {
		// Si el medio siguiente existe
		if (MedioSiguiente != NULL) {
			// Si el medio siguiente no es un video, cargo el medio siguiente.
			if (MedioSiguiente->TipoMedio != Tipo_Medio_Video)
				_Siguiente = new RaveVLC_Medio(_Instancia, *MedioSiguiente);
		}
	}

	if (_Actual->TxtError.size() == 0) return TRUE;
	else                               return FALSE;
}

void RaveVLC::CerrarMedio(void) {
	if (_Anterior  != NULL)	delete _Anterior;
	if (_Actual    != NULL)	delete _Actual;
	if (_Siguiente != NULL) delete _Siguiente;

	_Anterior  = NULL;
	_Actual    = NULL;
	_Siguiente = NULL;
}

void RaveVLC::Evento_Medio_Terminado(RaveVLC_Medio *MedioEvento) {
	if (_Anterior  == MedioEvento) _Anterior  = NULL;
	if (_Actual    == MedioEvento) _Actual	  = NULL; 
	if (_Siguiente == MedioEvento) _Siguiente = NULL; 
	MedioEvento->Eliminar();
	delete MedioEvento;
}

void RaveVLC::Eventos(const libvlc_event_t *event, void *ptr) {
	RaveVLC_Medio *MedioEvento = static_cast<RaveVLC_Medio *>(ptr);

	// Error
	if (event->type == libvlc_MediaPlayerEncounteredError) {
		PostMessage(App.VentanaRave.hWnd(), WM_MEDIO_TERMINADO, reinterpret_cast<WPARAM>(MedioEvento), 0);
		App.MostrarToolTipPlayerError(UltimoError());
	}
	// Se ha terminado el medio
	else if (event->type == libvlc_MediaPlayerEndReached) {
		PostMessage(App.VentanaRave.hWnd(), WM_MEDIO_TERMINADO, reinterpret_cast<WPARAM>(MedioEvento), 0);
	}
	// Se ha parseado el medio
	else if (event->type == libvlc_MediaParsedChanged) {
		MedioEvento->ObtenerDatosParsing();
	}
}

const BOOL RaveVLC::Play(void) {
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

const BOOL RaveVLC::Pausa(void) {
	if (_Actual == NULL) return FALSE;
	return _Actual->Pausa();
}

const BOOL RaveVLC::Stop(void) {
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

const BOOL RaveVLC::StopTODO(void) {
	if (_Anterior  != NULL) _Anterior->Stop();
	if (_Actual    != NULL) _Actual->Stop();
	if (_Siguiente != NULL) _Siguiente->Stop();
	return TRUE;
}

const int RaveVLC::Volumen(void) {
	if (_Actual == NULL) return App.BD.Opciones_Volumen();
	return _Actual->Volumen();
}

void RaveVLC::Volumen(int nVolumen) {
	if (_Actual == NULL) return;
	_Actual->Volumen(nVolumen);
}

Estados_Medio RaveVLC::ComprobarEstado(void) {
	if (_Actual == NULL) return SinCargar;
	return _Actual->ComprobarEstado();
}


const float RaveVLC::TiempoActual(void) {
	if (_Actual == NULL) return 0.0f;
	return _Actual->TiempoActual();
}

void RaveVLC::TiempoActual(float nTiempo) {
	if (_Actual == NULL) return;
	_Actual->TiempoActual(nTiempo);
}

const UINT64 RaveVLC::TiempoTotalMs(void) {
	if (_Actual == NULL) return 0;
	return _Actual->TiempoTotalMs();
}

const UINT64 RaveVLC::TiempoActualMs(void) {
	if (_Actual == NULL) return 0;
	return _Actual->TiempoActualMs();
}

void RaveVLC::Ratio(const float R) {
	if (_Actual == NULL) return;
	_Actual->Ratio(R);
}

std::wstring &RaveVLC::ObtenerProporcion(void) {
	static std::wstring Ret = L"Predeterminado";
	if (_Actual == NULL) return Ret;
	return _Actual->ObtenerProporcion();
}

void RaveVLC::AsignarProporcion(const char *Prop) {
	if (_Actual == NULL) return;
	_Actual->AsignarProporcion(Prop);
}

void RaveVLC::Brillo(const float nBrillo) {
	if (_Actual == NULL) return;
	_Actual->Brillo(nBrillo);
}

void RaveVLC::Contraste(const float nContraste) {
	if (_Actual == NULL) return;
	_Actual->Contraste(nContraste);
}

void RaveVLC::Gamma(const float nGamma) {
	if (_Actual == NULL) return;
	_Actual->Gamma(nGamma);
}

void RaveVLC::Hue(const int nHue) {
	if (_Actual == NULL) return;
	_Actual->Hue(nHue);
}

void RaveVLC::Saturacion(const float nSaturacion) {
	if (_Actual == NULL) return;
	_Actual->Saturacion(nSaturacion);
}

BDMedio &RaveVLC::MedioActual(void) {
	static BDMedio Vacio;
	if (_Actual == NULL) return Vacio;
	return _Actual->Medio;
}

const BOOL RaveVLC::ObtenerDatosParsing(void) {
	if (_Actual == NULL) return FALSE;
	return _Actual->ObtenerDatosParsing();
}

void RaveVLC::AsignarPistaAudio(const int nPista) {
	if (_Actual == NULL) return;
	_Actual->AsignarPistaAudio(nPista);
}

void RaveVLC::TiempoStr(UINT64 TMS, std::wstring &StrTiempo) {
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

/*
void RaveVLC2::TiempoStr(UINT64 TMS, wchar_t *StrTiempo) {                                      //
	int min = static_cast<int>(((TMS / 1000) / 60)); // -(temp.tm_hour * 60));
	int sec = static_cast<int>((TMS / 1000) - (min * 60)); // -((temp.tm_hour * 60) * 60));
	swprintf(StrTiempo, 256, TEXT("%.2d:%.2d"), min, sec);
}*/

// Necesito encontrar la ventana del VLC que se crea dentro del VerVideo para repintar el fondo negro
BOOL CALLBACK RaveVLC::EnumeracionVLC(HWND hWndWnd, LPARAM lParam) {
	hWndVLC = hWndWnd;
	//	EnableWindow(hWndWnd, FALSE);
	return FALSE;
}

/*
void RaveVLC::Evento_Temporizador(const UINT_PTR cID) {
	switch (cID) {
		case ID_TEMPORIZADOR_TIEMPO : Temporizador_Tiempo();		break;
		case ID_TEMPORIZADOR_LISTA  : Temporizador_Lista();			break;
	}
}*/

// Temporizador que controla la reproducción de la lista
void RaveVLC::Temporizador_Lista(void) {
	Estados_Medio Estado = ComprobarEstado();
	// Si no hay items salgo
	if (App.VentanaRave.Lista.TotalItems() == 0) return;

	// Si hay 10 errores en la lista, paro
	if (App.VentanaRave.Lista.Errores > 10) {
		App.VentanaRave.Lista_Stop();
		//App.VentanaRave.Lista.Errores = 0; // ya lo hace el stop
		return;
	}

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


	// Tiempo para el fade in out
/*	else if (Estado == EnPlay) {
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

// Temporizador que refresca el tiempo en las barras y labels
void RaveVLC::Temporizador_Tiempo(void) {
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
		else { //if (App.VLC.ComprobarEstado == EnStop || App.VLC.ComprobarEstado == SinCargar || App.VLC.ComprobarEstado == Abriendo || App.VLC.ComprobarEstado == Terminada || App.VLC.ComprobarEstado == EnError || App.VLC.ComprobarEstado == Nada) {
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


}
