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


Rave_MediaPlayer::Rave_MediaPlayer(void) : _InstanciaVLC(NULL)
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
	// On Microsoft Windows, setting the default DLL directories to SYSTEM32 exclusively is strongly recommended for security reasons
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);

	// Ventana de precarga
	VentanaPrecarga Precarga;
	Debug_Escribir(L"Rave_MediaPlayer::Iniciar : Cargando LibVLC...\n");

	ULONGLONG t = GetTickCount64();

	// Para poder cambiar los volumenes de varias canciones (fade in/out) necesito el DirectSound
	char const* Arg[] = { 
		"--aout=directsound"/*,		// Direct sound para controlar el volumen de cada medio independientemente.
		"--audio-filter=normvol",	// Filtro para normalizar el volumen
		"--norm-buff-size=20",		// Tama�o del buffer para normalizar el volumen
		"--norm-max-level=1,500000"	// Nivel m�ximo de volumen*/
	};
	_InstanciaVLC = libvlc_new(sizeof(Arg) / sizeof(*Arg), Arg);
//	_InstanciaVLC = libvlc_new(0, nullptr);

	// Error iniciando la instancia
	if (_InstanciaVLC == NULL) {
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR Cargando la LibVLC.\n");
		return FALSE;
	}

	//	const char *ErrorVLC = libvlc_errmsg();
	const char *Version = libvlc_get_version();
	std::wstring StrVersion;
	DWL::Strings::UTF8ToWide(Version, StrVersion);
	Debug_Escribir_Varg(L"Rave_MediaPlayer::Iniciar : LibVLC cargada en %d MS, Version = '%s' \n", GetTickCount64() - t, StrVersion.c_str());

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
		Debug_Escribir(L"Rave_MediaPlayer::Iniciar : ERROR a�adiendo el DSP al canal master en el FMOD.\n");
		return FALSE;
	}

	//	_CanalMaster->getDSP()

	Debug_Escribir_Varg(L"Rave_MediaPlayer::Iniciar : FMOD cargado '%d'\n", _SistemaFMOD);
#endif

	// Creo los temporizadores
	SetTimer(_hWnd, ID_TEMPORIZADOR_TIEMPO, 100, NULL);
	SetTimer(_hWnd, ID_TEMPORIZADOR_LISTA, 500, NULL);

	return TRUE;
}

//void Rave_MediaPlayer::ObtenerTTF(void) {
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
//}


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
		// Se ha llegado al final de la lista
		if (App.VentanaRave.Lista.PosMedio(App.VentanaRave.Lista.MedioActual) == App.VentanaRave.Lista.TotalItems() - 1) {
			App.VentanaRave.Repeat();
		}
		// Siguiente medio
		else {
//			if (_Actual) _Actual->ActualizarIconos(0);
			App.VentanaRave.Lista_Siguiente();
		}
	}
}


void Rave_MediaPlayer::Temporizador_Tiempo(void) {
	Estados_Medio Estado	= ComprobarEstado();
	INT64         TTotalMS	= TiempoTotalMs();
	INT64         TActualMS = TiempoActualMs();
	std::wstring  StrMomento;

	// MOMENTOS
	if (_Actual) {
		// Compruebo si el medio actual es un momento
		if (_Actual->Medio->BdMedio.PosMomento != -1) {
			// Compruebo si ha llegado al final del momento para parar el medio
			if (TActualMS > _Actual->Medio->BdMedio.Momentos[_Actual->Medio->BdMedio.PosMomento]->TiempoFinal) {
				Stop();
				CerrarMedio();				
			}
		}
		// Compruebo si el medio actual tiene algun momento de exclusi�n
		else {
			for (size_t i = 0; i < _Actual->Medio->BdMedio.Momentos.size(); i++) {
				if (_Actual->Medio->BdMedio.Momentos[i]->Excluir == TRUE) {
					// Si el tiempo actual esta entre el inicio y el final del momento
					if (TActualMS > _Actual->Medio->BdMedio.Momentos[i]->TiempoInicio && TActualMS < _Actual->Medio->BdMedio.Momentos[i]->TiempoFinal) {
						StrMomento = L"Momento '" + _Actual->Medio->BdMedio.Momentos[i]->Nombre + L"' excluido";
						App.MostrarToolTipPlayer(StrMomento);
						TiempoActualMs(_Actual->Medio->BdMedio.Momentos[i]->TiempoFinal);
					}
					// Si el tiempo actual esta entre el inicio y el inicio + 500 ms
/*					if (TActualMS > _Actual->Medio->BdMedio.Momentos[i]->TiempoInicio && TActualMS < _Actual->Medio->BdMedio.Momentos[i]->TiempoInicio + 500) {
						StrMomento = L"Momento '" + _Actual->Medio->BdMedio.Momentos[i]->Nombre + L"' excluido";
						App.MostrarToolTipPlayer(StrMomento);
						TiempoActualMs(_Actual->Medio->BdMedio.Momentos[i]->TiempoFinal);
					}*/
				}
			}
		}		
	}


	// Visualizaci�n del tiempo (barra y etiquetas)
	ObtenerDatosParsing();
	if (!App.VentanaRave.Minimizado()) {
		std::wstring TmpStr;
		if (Estado == EnPlay || Estado == EnPausa) { // EnPlay y EnPausa
			float TActual = TiempoActual();
			// Asigno el tiempo del medio anterior, que ahora ya no es controlable
			if (_Anterior != nullptr) {
				// TODO : hi ha un bug chungo... si executes l'ultima can�o de la llista, i despres tires enrera, acabes la can�o i tornes a tirar enrera... peta...
				// EN PRINCIPI EST� SOLUCIONAT
				float TAnterior = _Anterior->TiempoActual();
				App.VentanaRave.SliderTiempo.Valor2(TAnterior);
				App.ControlesPC.SliderTiempo.Valor2(TAnterior);
			}

			// Si el slider del tiempo tiene la captura, es porque se esta modificando el tiempo, por lo que no hay que actualizar la posici�n en ese momento.
			if (App.VentanaRave.SliderTiempo.Estado() != DBarraEx_Estado_Presionado && App.ControlesPC.SliderTiempo.Estado() != DBarraEx_Estado_Presionado) {
				App.VentanaRave.SliderTiempo.Valor(TActual);
				App.ControlesPC.SliderTiempo.Valor(TActual);
				// Lo mismo pasa con el tiempo actual
				TiempoStr(TActualMS, TmpStr);
				App.VentanaRave.LabelTiempoActual.Texto(TmpStr);
				App.ControlesPC.LabelTiempoActual.Texto(TmpStr);
			}

			// Actualizo el tiempo total
			TmpStr = L"";
			TiempoStr(TTotalMS, TmpStr);
			App.VentanaRave.LabelTiempoTotal.Texto(TmpStr);
			App.ControlesPC.LabelTiempoTotal.Texto(TmpStr);
		}
		else { //if (Estado == EnStop || Estado == SinCargar || Estado == Abriendo || Estado == Terminada || Estado == EnError || Estado == Nada) {
			std::wstring Tiempo(L"00:00");
			App.VentanaRave.LabelTiempoActual.Texto(Tiempo);
			App.VentanaRave.LabelTiempoTotal.Texto(Tiempo);
			App.ControlesPC.LabelTiempoActual.Texto(Tiempo);
			App.ControlesPC.LabelTiempoTotal.Texto(Tiempo);
		}
	}



	// Tiempo para el fade in/out
	if (Estado == EnPlay && App.Opciones.EfectoFadeAudio() == TRUE) {
//		UINT64        TTotalMS  = TiempoTotalMs();
//		UINT64        TActualMS = TiempoActualMs();

//		if (TTotalMS < App.Opciones.EfectoFadeAudioMS() || _Actual == nullptr) return;
		if (TTotalMS < App.Opciones.EfectoFadeAudioMS() || _Actual == nullptr || _Siguiente == nullptr) return;

		if (TTotalMS - App.Opciones.EfectoFadeAudioMS() < TActualMS && _Actual->Tipo() == Tipo_Medio_Audio && _Siguiente->Tipo() == Tipo_Medio_Audio) {
			// Si el medio actual no es el ultimo, o el repeat es Tipo_Repeat_RepetirLista (en cualquier otro repeat no es posible calcular el primer medio ya que aun no se ha generado la lista)
			if (App.Opciones.Repeat() == Tipo_Repeat_RepetirLista || App.VentanaRave.Lista.MedioActual != App.VentanaRave.Lista.MedioUltimo()) {

				// cambio el medio actual
				App.VentanaRave.Lista.MedioActual = App.VentanaRave.Lista.MedioSiguiente(App.VentanaRave.Lista.MedioActual);

				// Asigno el volumen del medio siguiente a 0, y empiezo a reproducir-lo
				_Siguiente->Volumen(0, FALSE);				
				BOOL B = _Siguiente->Play();
				if (B == TRUE) {
					// Actualizo los iconos de la lista y la BD
					_Siguiente->ActualizarIconos(1);
					// Muestro el tooltip con los datos
					App.MostrarToolTipPlayer(_Siguiente->Medio->BdMedio);
					// Compruebo si es un momento
					//if (ComprobarMomento == TRUE) _Actual->ComprobarMomento();
				}

				// Si el nuevo medio actual es NULL y el repeat es Tipo_Repeat_RepetirLista, asigno el primer item de la lista como actual
//				if (App.VentanaRave.Lista.MedioActual == nullptr && App.Opciones.Repeat() == Tipo_Repeat_RepetirLista) 
//					App.VentanaRave.Lista.MedioActual = App.VentanaRave.Lista.MedioPrimero();

				// Abro el nuevo medio actual
				if (AbrirMedio(App.VentanaRave.Lista.MedioActual) == FALSE) App.VentanaRave.Lista.Errores++; 
				
				// efecto fade in out
				if (_Anterior != NULL && _Actual != NULL) {
					Debug_Escribir(L"Rave_MediaPlayer::Temporizador_Lista Fade in out\n");
					if (_Anterior->Medio->BdMedio.TipoMedio == Tipo_Medio_Audio && _Actual->Medio->BdMedio.TipoMedio == Tipo_Medio_Audio) {
						_Anterior->FadeOut();
						_Actual->FadeIn();
					}
				}
			}
		}

	}
}


// Funci�n que abre el medio especificado de la lista de medios
// El medio que hasta ahora era el actual se posicionara en el anterior
// El nuevo medio pasa a ser el medio actual
// Se busca si el medio siguiente es una cancion para tener-lo cargado para un efecto fade in/out
const BOOL Rave_MediaPlayer::AbrirMedio(ItemMedio *Medio) {

	Debug_Escribir(L"Rave_MediaPlayer::AbrirMedio_Lista\n");
	// Si el item es null, salgo.
	if (Medio == nullptr) 
		return FALSE;

	// Guardo el medio en el historial
	App.BD.GuardarHistorial_Medio(Historial_Medio(Medio->BdMedio.Id));
	
	

	// Elimino el medio anterior, si aun existe
	if (_Anterior != nullptr) {
		// Elimino el medio anterior (si el medio anterior es el mismo que el actual, no actualizo los iconos)
		_EliminarRaveMedio(_Anterior, (Medio == _Anterior->Medio) ? FALSE : TRUE);
		_Anterior = nullptr;
	}

	// Busco el pr�ximo medio siguiente
	ItemMedio* NuevoMedioSiguiente = App.VentanaRave.Lista.MedioSiguiente(Medio);

	// Si el nuevo medio siguiente es NULL, hay mas de un item en la lista, y el repeat est� en repetir lista :
	// - Asigno el primer medio de la lista al NuevoMedioSiguiente
//	if (App.Opciones.Repeat() == Tipo_Repeat_RepetirLista && NuevoMedioSiguiente == nullptr && App.VentanaRave.Lista.TotalItems() > 1) NuevoMedioSiguiente = App.VentanaRave.Lista.MedioPrimero();

	// Si el medio actual es una cancion, pasa a ser el anterior
	if (Medio->BdMedio.TipoMedio == Tipo_Medio_Audio) {
		_Anterior	= _Actual;
		_Actual		= nullptr;
	}
	// Es un video u otra cosa
	else {
		if (_Actual != nullptr) {
			_EliminarRaveMedio(_Actual);
			_Actual = nullptr;
		}
	}

	// Existe un medio siguiente cargado y tiene la misma id que el medio actual
	if (_Siguiente != nullptr) {
		if (_Siguiente->Medio->BdMedio.Id == Medio->BdMedio.Id) {
			_Actual = _Siguiente;
			_Siguiente = nullptr; // Hay que asignar null al medio siguiente para no tenerlo duplicado y que al borrar el actual me quede el puntero al siguiente inv�lido
		}
		// El medio siguiente no se corresponde con el medio a cargar
		else {
			_EliminarRaveMedio(_Siguiente);
			_Siguiente = nullptr;
			_Actual = new RaveVLC_Medio(_InstanciaVLC, Medio);
		}
	}
	// No hay un medio siguiente 
	else {
		_Actual = new RaveVLC_Medio(_InstanciaVLC, Medio);
	}

	// Recargo la informaci�n del medio en la BD
	_Actual->Medio->BdMedio.Recargar();


	// Si existe un posible nuevo medio siguiente
	if (NuevoMedioSiguiente != nullptr) {
		// Si el nuevo medio actual y el nuevo medio siguiente son canciones
		if (Medio->BdMedio.TipoMedio == Tipo_Medio_Audio && NuevoMedioSiguiente->BdMedio.TipoMedio == Tipo_Medio_Audio) {
			// Cargo el nuevo medio siguiente
			_Siguiente = new RaveVLC_Medio(_InstanciaVLC, NuevoMedioSiguiente);
		}
	}

	// Si existe el medio actual
	if (_Actual != nullptr) {
		// Si no hay ning�n texto de error devuelvo TRUE
		if (_Actual->TxtError.size() == 0) return TRUE;
	}

	// Parece que hay un error
	return FALSE;
}


// Funci�n que comprueba si el medio siguiente de la lista sigue siendo el medio siguiente del MediaPlayer, y en caso contrario, lo carga.
void Rave_MediaPlayer::AsegurarMedioSiguiente(void) {
	// Si el medio actual no es v�lido, salgo de la funci�n
	if (_Actual			== nullptr)	 return;
	if (_Actual->Medio	== nullptr)  return;

	// Si el medio actual es una cancion
	if (_Actual->Medio->BdMedio.TipoMedio == Tipo_Medio_Audio) {
		// Busco el pr�ximo medio siguiente
		ItemMedio *NuevoMedioSiguiente = App.VentanaRave.Lista.MedioSiguiente(_Actual->Medio);
		// Si existe un nuevo medio siguiente
		if (NuevoMedioSiguiente != nullptr) {
			// Si el nuevo medio siguiente es una cancion
			if (NuevoMedioSiguiente->BdMedio.TipoMedio == Tipo_Medio_Audio) {
				// Si no hay medio siguiente asignado
				if (_Siguiente == nullptr) {
					// Cargo el nuevo medio siguiente
					_Siguiente = new RaveVLC_Medio(_InstanciaVLC, NuevoMedioSiguiente);
				}
				else {
					// Si el medio siguiente no tiene el mismo path que el nuevo medio siguiente y no se est� reproduciendo
					if (_Siguiente->Medio->BdMedio.Path != NuevoMedioSiguiente->BdMedio.Path && _Siguiente->ComprobarEstado() != EnPlay) {
						// Elimino el medio siguiente desfasado
						_EliminarRaveMedio(_Siguiente);
						// Cargo el nuevo medio siguiente
						_Siguiente = new RaveVLC_Medio(_InstanciaVLC, NuevoMedioSiguiente);
					}
				}
			}
		}
	}
}




void Rave_MediaPlayer::CerrarMedio(void) {
	if (_Anterior  != nullptr)	_EliminarRaveMedio(_Anterior);
	if (_Actual    != nullptr)	_EliminarRaveMedio(_Actual);
	if (_Siguiente != nullptr)  _EliminarRaveMedio(_Siguiente);

	_Anterior  = nullptr;
	_Actual    = nullptr;
	_Siguiente = nullptr;

	AsignarTitulo();
}


void Rave_MediaPlayer::_EliminarRaveMedio(Rave_Medio *eMedio, const BOOL ActualizarIcono) {
	if (eMedio != nullptr) {
		Debug_Escribir_Varg(L"Rave_MediaPlayer::_EliminarRaveMedio %02d %s\n", eMedio->Medio->BdMedio.Pista(), eMedio->Medio->BdMedio.Nombre().c_str());
	}
	else {
		Debug_Escribir(L"Rave_MediaPlayer::_EliminarRaveMedio nullptr\n");
	}
	

	if (eMedio) {
		if (ActualizarIcono == TRUE)	eMedio->ActualizarIconos(0, eMedio == _Actual);
	}
	

	switch (eMedio->Tipo()) {
		case Rave_Medio_Tipo_VLC  : delete static_cast<RaveVLC_Medio *>(eMedio);	break;
#ifdef RAVE_UTILIZAR_FMOD
		case Rave_Medio_Tipo_FMOD : delete static_cast<RaveFMOD_Medio *>(eMedio);	break;
#endif
		default                   : delete eMedio;									break;
	}	
}

// Formatea el tiempo especificado en el string especificado
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

// Funci�n que devuelve un string con un tiempo en milisegundos
const UINT64 Rave_MediaPlayer::TiempoStr_Ms(std::wstring& StrTiempo) {
	DWL::Strings::Split Sp(StrTiempo, L':');
	UINT64 S = 0, M = 0, H = 0;
	UINT64 Ret = 0;
	switch (Sp.Total()) {
		case 1 : // solo son segundos
			Ret = DWL::Strings::StrTo(Sp[0], S) * 1000;
			break;
		case 2: // minutos y segundos
			Ret = ((DWL::Strings::StrTo(Sp[0], M) * 60) +  DWL::Strings::StrTo(Sp[1], S)) * 1000;
			break;
		case 3: // horas, minutos, y segundos
			Ret = ((DWL::Strings::StrTo(Sp[0], H) * 60) + (DWL::Strings::StrTo(Sp[1], M) * 60) + DWL::Strings::StrTo(Sp[2], S)) * 1000;
			break;
	}
	return Ret;	
}


std::wstring &Rave_MediaPlayer::UltimoErrorVLC(void) {
	static std::wstring TxtError;
	DWL::Strings::UTF8ToWide(libvlc_errmsg(), TxtError);
	return TxtError;
}

void Rave_MediaPlayer::EventosVLC(const libvlc_event_t *evento, void *ptr) {
	RaveVLC_Medio *MedioEvento = static_cast<RaveVLC_Medio *>(ptr);

	switch (evento->type) {
		case libvlc_MediaPlayerEncounteredError :
			// Muestro el error
			App.MostrarToolTipPlayerError(UltimoErrorVLC());
			// Cierro el medio y libero la mem�ria
			PostMessage(_hWndMensajes, WM_MEDIO_TERMINADO, 0, reinterpret_cast<LPARAM>(MedioEvento));
			break;
		case libvlc_MediaPlayerEndReached :
			// Sumo 1 a las veces reproducidas del medio
			App.BD.MedioReproducido(&MedioEvento->Medio->BdMedio);
			// Cierro el medio y libero la mem�ria
			PostMessage(_hWndMensajes, WM_MEDIO_TERMINADO, 0, reinterpret_cast<LPARAM>(MedioEvento));
			break;
		case libvlc_MediaParsedChanged :
			MedioEvento->ObtenerDatosParsing();
			break;
		case libvlc_MediaPlayerPlaying :
			libvlc_media_player_set_hwnd(static_cast<RaveVLC_Medio*>(_Actual)->_Medio, App.VentanaRave.Video.hWnd());
			break;
/*		case libvlc_MediaSubItemAdded:
			break;
		case libvlc_MediaSubItemTreeAdded:
			break;*/
		case libvlc_MediaPlayerVout:
			// Muestra el marco del video
			App.VentanaRave.MostrarMarco(ID_BOTON_VIDEO);
			break;
/*		case libvlc_MediaListItemAdded:
			break;
		case libvlc_MediaListEndReached:
			PostMessage(_hWndMensajes, WM_MEDIO_TERMINADO, 0, reinterpret_cast<LPARAM>(MedioEvento));
			break;*/
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

// Funci�n que se llama al terminar un medio
void Rave_MediaPlayer::_TerminarMedio(Rave_Medio *MedioEvento) {

	if (MedioEvento == _Actual) {
		// Escondo los tooltip de las barras de tiempo
		App.VentanaRave.SliderTiempo.OcultarToolTip();
		App.ControlesPC.SliderTiempo.OcultarToolTip();
		// Evito mostrar el tooltip si se pasa el mouse por encima del slider tiempo
		App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
		App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
		// Asigno el valor de las barras del tiempo a su m�ximo
		App.VentanaRave.SliderTiempo.Valor(App.VentanaRave.SliderTiempo.Maximo());
		App.ControlesPC.SliderTiempo.Valor(App.ControlesPC.SliderTiempo.Maximo());

		// Compruebo si el medio actual se est� reproduciendo
/*		BOOL RestaurarIconos = FALSE;
		if (_Actual != NULL) {
			if (_Actual->ComprobarEstado() != EnPlay) 	RestaurarIconos = TRUE;
		}
		else {
			RestaurarIconos = TRUE;
		}

		if (RestaurarIconos == TRUE) {
			// Asigno las imagenes de los botones play / pausa a la del play
			App.VentanaRave.BotonPlay.Icono(IDI_PLAY4, 24);
			App.ControlesPC.BotonPlay.Icono(IDI_PLAY4, 24);
		}*/
	}

//	App.VentanaRave.SliderTiempo.Valor2(0.0f);
//	App.ControlesPC.SliderTiempo.Valor2(0.0f);
	App.VentanaRave.SliderTiempo.DesAnimar();
	App.ControlesPC.SliderTiempo.DesAnimar();

	MedioEvento->Eliminar();
	_EliminarRaveMedio(MedioEvento);

	if (_Anterior  == MedioEvento) _Anterior  = nullptr;
	if (_Actual    == MedioEvento) _Actual	  = nullptr;
	if (_Siguiente == MedioEvento) _Siguiente = nullptr;

	AsignarTitulo();
}



const BOOL Rave_MediaPlayer::Play(const BOOL ComprobarMomento) {
	if (_Actual == NULL) return FALSE;

	App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_Abajo);
	// Establezco la norma del tooltip seg�n la alineaci�n de los controles pantalla completa
	switch (App.ControlesPC.Alineacion) {
		case Abajo		:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Arriba);		break;
		case Arriba		:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Abajo);		break;
		case Izquierda	:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Derecha);		break;
		case Derecha	:	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_Izquierda);	break;
	}


	BOOL B = _Actual->Play();
	if (B == TRUE) {
		// Actualizo los iconos de la lista y la BD
		_Actual->ActualizarIconos(1);
		// Muestro el tooltip con los datos
		App.MostrarToolTipPlayer(_Actual->Medio->BdMedio);
		// Compruebo si es un momento
		if (ComprobarMomento == TRUE) _Actual->ComprobarMomento();
	}

	AsignarTitulo();


	return B;
}

void Rave_MediaPlayer::AsignarTitulo(void) {
	std::wstring nTitulo;
	if (App.Opciones.MostrarMedioActualTitulo() == TRUE && _Actual != NULL && ComprobarEstado() != EnStop) {
		// Asigno el titulo de la ventana con el nombre del medio que se acaba de abrir
		if (_Actual->Medio->BdMedio.Pista() == 0)	nTitulo = std::wstring(RAVE_TITULO) + L" [" + _Actual->Medio->BdMedio.Nombre() + L"]";																		// Sin pista
		else										nTitulo = std::wstring(RAVE_TITULO) + L" [" + DWL::Strings::ToStr(_Actual->Medio->BdMedio.Pista(), 2) + L" " + _Actual->Medio->BdMedio.Nombre() + L"]";		// Con pista
	}
	else {
		nTitulo = std::wstring(RAVE_TITULO);
	}
	App.VentanaRave.Titulo(nTitulo);
}



const BOOL Rave_MediaPlayer::Pausa(void) {
	if (_Actual == NULL) return FALSE;
	BOOL B = _Actual->Pausa();
	if (B == TRUE) {
		_Actual->ActualizarIconos(2);
	}
	return B;
}

const BOOL Rave_MediaPlayer::Stop(void) {
	if (_Actual == NULL) return FALSE;

	// Escondo los tooltip de las barras de tiempo
	App.VentanaRave.SliderTiempo.OcultarToolTip();
	App.ControlesPC.SliderTiempo.OcultarToolTip();
	// Evito mostrar el tooltip si se pasa el mouse por encima del slider tiempo
	App.VentanaRave.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
	App.ControlesPC.SliderTiempo.ToolTip(DBarraEx_ToolTip_SinToolTip);
	// Asigno el valor de las barras del tiempo a su m�ximo
	App.VentanaRave.SliderTiempo.Valor(0.0f);
	App.ControlesPC.SliderTiempo.Valor(0.0f);

	// Elimino el nombre del medio en el titulo
	App.VentanaRave.Titulo(std::wstring(RAVE_TITULO));

	BOOL B = _Actual->Stop();
	if (B == TRUE) {
		_Actual->ActualizarIconos(0);
	}
	AsignarTitulo();

	return B;
}

const BOOL Rave_MediaPlayer::StopTODO(void) {
	if (_Anterior  != nullptr) _Anterior->Stop();
	if (_Actual    != nullptr) _Actual->Stop();
	if (_Siguiente != nullptr) _Siguiente->Stop();
	return TRUE;
}

const int Rave_MediaPlayer::Volumen(void) {
	if (_Actual == nullptr) return App.Opciones.Volumen();
	return _Actual->Volumen();
}

void Rave_MediaPlayer::Volumen(int nVolumen) {
	if (_Actual == nullptr) return;
	// Si existe un medio anterior
	if (_Anterior != nullptr) {
		// Termino el fade out
		_Anterior->_AniVolumen.Terminar();
		// Asigno el volumen del medio anterior a 0
		_Anterior->Volumen(0, FALSE);
	}
	// Termino el fade in
	_Actual->_AniVolumen.Terminar();
	// Asigno el volumen del medio actual
	_Actual->Volumen(nVolumen);
}

Estados_Medio Rave_MediaPlayer::ComprobarEstado(void) {
	if (_Actual == nullptr) return SinCargar;
	return _Actual->ComprobarEstado();
}


const float Rave_MediaPlayer::TiempoActual(void) {
	if (_Actual == nullptr) return 0.0f;
	return _Actual->TiempoActual();
}

void Rave_MediaPlayer::TiempoActual(float nTiempo) {
	if (_Actual == nullptr) return;
	_Actual->TiempoActual(nTiempo);
}

void Rave_MediaPlayer::TiempoActualMs(UINT64 nTiempo) {
	if (_Actual == nullptr) return;
	_Actual->TiempoActualMs(nTiempo);
}

const UINT64 Rave_MediaPlayer::TiempoTotalMs(void) {
	if (_Actual == nullptr) return 0;
	return _Actual->TiempoTotalMs();
}

const UINT64 Rave_MediaPlayer::TiempoActualMs(void) {
	if (_Actual == nullptr) return 0;
	return _Actual->TiempoActualMs();
}

void Rave_MediaPlayer::Ratio(const float R) {
	if (_Actual == nullptr) return;
	_Actual->Ratio(R);
}

std::wstring &Rave_MediaPlayer::ObtenerProporcion(void) {
	static std::wstring Ret = L"Predeterminado";
	if (_Actual == nullptr) return Ret;
	return _Actual->ObtenerProporcion();
}

void Rave_MediaPlayer::AsignarProporcion(const int Prop) {
	if (_Actual == nullptr) return;

	for (size_t i = 0; i < App.MenuVideoProporcion->TotalMenus(); i++) {
		App.MenuVideoProporcion->Menu(i)->Icono(0);
	}

	App.MenuVideoProporcion->Menu(static_cast<size_t>(Prop) - ID_MENUVIDEO_PROPORCION_PREDETERMINADO)->Icono(IDI_CHECK2);
	
	switch (Prop) {
		case ID_MENUVIDEO_PROPORCION_PREDETERMINADO : _Actual->AsignarProporcion(nullptr);		break;
		case ID_MENUVIDEO_PROPORCION_16A9			: _Actual->AsignarProporcion("16:9");		break;
		case ID_MENUVIDEO_PROPORCION_4A3			: _Actual->AsignarProporcion("4:3");		break;
		case ID_MENUVIDEO_PROPORCION_1A1			: _Actual->AsignarProporcion("1:1");		break;
		case ID_MENUVIDEO_PROPORCION_16A10			: _Actual->AsignarProporcion("16:10");		break;
		case ID_MENUVIDEO_PROPORCION_2P21A1			: _Actual->AsignarProporcion("2.21:1");		break;
		case ID_MENUVIDEO_PROPORCION_2P35A1			: _Actual->AsignarProporcion("2.35:1");		break;
		case ID_MENUVIDEO_PROPORCION_2P39A1			: _Actual->AsignarProporcion("2.39:1");		break;
		case ID_MENUVIDEO_PROPORCION_5A4			: _Actual->AsignarProporcion("5:4");		break;
	}
}

void Rave_MediaPlayer::Brillo(const float nBrillo) {
	if (_Actual == nullptr) return;
	_Actual->Brillo(nBrillo);
}

void Rave_MediaPlayer::Contraste(const float nContraste) {
	if (_Actual == nullptr) return;
	_Actual->Contraste(nContraste);
}

void Rave_MediaPlayer::Gamma(const float nGamma) {
	if (_Actual == nullptr) return;
	_Actual->Gamma(nGamma);
}

void Rave_MediaPlayer::Hue(const int nHue) {
	if (_Actual == nullptr) return;
	_Actual->Hue(nHue);
}

void Rave_MediaPlayer::Saturacion(const float nSaturacion) {
	if (_Actual == nullptr) return;
	_Actual->Saturacion(nSaturacion);
}

BDMedio &Rave_MediaPlayer::MedioActual(void) {
	static BDMedio Vacio;
	if (_Actual == nullptr) return Vacio;
	return _Actual->Medio->BdMedio;
}

// Funci�n que re-asigna los datos medio actual 
void Rave_MediaPlayer::MedioActual(BDMedio &nMedio) {
	_Actual->Medio->BdMedio = nMedio;
}

const BOOL Rave_MediaPlayer::ObtenerDatosParsing(void) {
	if (_Actual == nullptr) return FALSE;
	return _Actual->ObtenerDatosParsing();
}

void Rave_MediaPlayer::AsignarPistaAudio(const int nPista) {
	if (_Actual == nullptr) return;
	_Actual->AsignarPistaAudio(nPista);
}

const int Rave_MediaPlayer::AsignarSubtitulos(const wchar_t* Path) {
	if (_Actual == nullptr) return 0;
	return _Actual->AsignarSubtitulos(Path);
}

const int Rave_MediaPlayer::AsignarSubtituloInterno(const int nPos) {
	if (_Actual == nullptr) return 0;
	return _Actual->AsignarSubtituloInterno(nPos);
}


const int Rave_MediaPlayer::EnumerarSubtitulos(void) {
	if (_Actual == nullptr) return 0;
	return _Actual->EnumerarSubtitulos();
}

void Rave_MediaPlayer::AsignarTick_SalirPantallaCompleta(void) {
	if (_Actual != nullptr) _Actual->Evento_SalirPantallaCompleta();
//	if (_Siguiente != nullptr) _Actual->Evento_SalirPantallaCompleta();
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
	//UINT64 T = 0;
	switch (uMsg) {
		case WM_TIMER :
/*			switch (static_cast<UINT>(wParam)) {
				case TIMER_OBTENERVLCWND    : PostMessage(_hWnd, WM_TIMER_OBTENERHWNDVLC, 0, 0);	return 0;
				case ID_TEMPORIZADOR_TIEMPO : PostMessage(_hWnd, WM_TIMER_TIEMPO, 0, 0);			return 0;
				case ID_TEMPORIZADOR_LISTA  : PostMessage(_hWnd, WM_TIMER_LISTA, 0, 0);				return 0;
			}*/
			switch (static_cast<UINT>(wParam)) {
				case TIMER_OBTENERVLCWND    : Temporizador_ObtenerVentanaVLC();	return 0;
				case ID_TEMPORIZADOR_TIEMPO : Temporizador_Tiempo();			return 0;
				case ID_TEMPORIZADOR_LISTA  : Temporizador_Lista();				return 0;
			}
			break;
		// Mensaje para eliminar el medio de la mem�ria
		case WM_MEDIO_TERMINADO :
			_TerminarMedio(reinterpret_cast<Rave_Medio*>(lParam));
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

