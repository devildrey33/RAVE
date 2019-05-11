#include "stdafx.h"
#include "RaveFMOD_Medio.h"

#ifdef RAVE_UTILIZAR_FMOD

#include "DStringUtils.h"
#include "Rave_MediaPlayer.h"


RaveFMOD_Medio::RaveFMOD_Medio(FMOD::System *SistemaFMOD, BDMedio &nMedio) : _Stream(NULL), _Canal(NULL), _Sistema(NULL), _GrupoCanales(NULL), _Estado(SinCargar) {
	Medio				= nMedio;
	_Sistema			= SistemaFMOD;
	FMOD_RESULT	Error	= FMOD_OK;

	std::string AnsiStr;
	// Hay que convertir el path a UTF8 para que funcione en el VLC...
	if (DWL::Strings::WideToAnsi(Medio.Path.c_str(), AnsiStr) == 0) {
		Debug_MostrarUltimoError();
		TxtError = L"Error al convertir el string '" + Medio.Path + L"'";
		App.MostrarToolTipPlayerError(TxtError);
		return;
	}

	//FMOD_CREATESOUNDEXINFO SoundInfo;
	Error = _Sistema->createStream(AnsiStr.c_str(), FMOD_2D | FMOD_ACCURATETIME | FMOD_LOOP_OFF, NULL, &_Stream);
	if (Error != FMOD_OK) {
		TxtError = L"Error al cargar el medio '" + Medio.Path + L"'";
	}
	Error = _Sistema->createChannelGroup("", &_GrupoCanales);
	if (Error != FMOD_OK) {
		TxtError = L"Error al crear el grupo de canales.";
	}	
}


RaveFMOD_Medio::~RaveFMOD_Medio(void) {
	Eliminar();
}

void RaveFMOD_Medio::Eliminar(void) {
	if (_Canal != NULL) {
		_Canal->setUserData(NULL);
		_Canal = NULL;
	}

	Stop();

	if (_Stream != NULL) {
		_Stream->release();
		_Stream = NULL;
	}

	if (_GrupoCanales != NULL) {
		_GrupoCanales->release();
	}

}

const BOOL RaveFMOD_Medio::Play(void) {
	FMOD_RESULT	Error = FMOD_OK, Error2 = FMOD_OK;
	if (_Canal != NULL) {		
		Error = _Canal->setPaused(false);
	}
	else {		
		Error = _Sistema->playSound(_Stream, _GrupoCanales, false, &_Canal);
		Volumen(static_cast<int>(App.VentanaRave.SliderVolumen.Valor()));
		// Callback para los eventos
		_Canal->setUserData(this);
//#ifdef RAVE_UTILIZAR_FMOD
		Error2 = _Canal->setCallback(Rave_MediaPlayer::EventosFMOD);		
//#endif
	}

	if (Error == FMOD_OK && Error2 == FMOD_OK) {
		_Estado = EnPlay;
		ActualizarIconos(1);
		// Muestro el tooltip con los datos
		App.MostrarToolTipPlayer(Medio);

		return TRUE;
	}
	else {		
		_Estado = EnError;
		return FALSE;
	}
}

const BOOL RaveFMOD_Medio::Pausa(void) {
	FMOD_RESULT	Error = FMOD_OK;
	if (_Canal != NULL) {
		Error = _Canal->setPaused(true);		
	}

	if (Error == FMOD_OK) {
		ActualizarIconos(2);
		_Estado = EnPausa;
		return TRUE;
	}
	else {
		_Estado = EnError;
		return FALSE;
	}
}

const BOOL RaveFMOD_Medio::Stop(void) {
	FMOD_RESULT	Error = FMOD_OK, Error2 = FMOD_OK;
	if (_Canal != NULL) {
		Error = _Canal->setPaused(true);
		//Error2 = _Canal->stop();
		Error2 = _Canal->setPosition(0, FMOD_TIMEUNIT_MS);
	}

	if (Error == FMOD_OK && Error2 == FMOD_OK) {
		ActualizarIconos(0);
		_Estado = EnStop;
		return TRUE;
	}
	else {
		_Estado = EnError;
		return FALSE;
	}

}


// 0 - 200
void RaveFMOD_Medio::Volumen(int nVolumen, const BOOL ActualizarUI) {
	// Topes
	if (nVolumen > 200) nVolumen = 200;
	if (nVolumen < 0)	nVolumen = 0;

	Debug_Escribir_Varg(L"RaveFMOD_Medio::Volumen  %d\n", nVolumen);

	if (ActualizarUI == TRUE) {
		App.VentanaRave.SliderVolumen.Valor(static_cast<float>(nVolumen));
		App.ControlesPC.SliderVolumen.Valor(static_cast<float>(nVolumen));
		std::wstring StrVol = std::to_wstring(nVolumen) + L"%";
		App.VentanaRave.LabelVolumen.Texto(StrVol);
		App.ControlesPC.LabelVolumen.Texto(StrVol);
	}

	if (_Canal != NULL) {
		_Canal->setVolume(static_cast<float>(nVolumen) / 100.0f);
	}
}

const int RaveFMOD_Medio::Volumen(void) {
	if (_Stream != NULL) {
		float nVol = 0.0f;
		_Canal->getVolume(&nVol);
		return static_cast<int>(nVol * 100.0f);
	}
	return 0;
}

const Estados_Medio RaveFMOD_Medio::ComprobarEstado(void) {
	return _Estado;
}

// Tiempo del medio
const float RaveFMOD_Medio::TiempoActual(void) {
	if (_Stream != NULL) {
		unsigned int tTotal = 0, tActual = 0;
		_Stream->getLength(&tTotal, FMOD_TIMEUNIT_MS);
		_Canal->getPosition(&tActual, FMOD_TIMEUNIT_MS);
		return (1.0f / static_cast<float>(tTotal)) * static_cast<float>(tActual);
	}
	return 0.0f;
}

void RaveFMOD_Medio::TiempoActual(float nTiempo) {
	if (_Stream != NULL) {
		unsigned int tTotal = 0;
		_Stream->getLength(&tTotal, FMOD_TIMEUNIT_MS);
		_Canal->setPosition(static_cast<int>(static_cast<float>(tTotal) * nTiempo), FMOD_TIMEUNIT_MS);
	}
}

const UINT64 RaveFMOD_Medio::TiempoTotalMs(void) {
	unsigned int Tmp = 0;
	if (_Stream != NULL)	_Stream->getLength(&Tmp, FMOD_TIMEUNIT_MS);	
	return static_cast<UINT64>(Tmp);
}

const UINT64 RaveFMOD_Medio::TiempoActualMs(void) {
	unsigned int Tmp = 0;
	if (_Canal != NULL) {
		_Canal->getPosition(&Tmp, FMOD_TIMEUNIT_MS);
	}
	return static_cast<UINT64>(Tmp);
}

#endif