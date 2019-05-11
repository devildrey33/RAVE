#pragma once

#ifdef RAVE_UTILIZAR_FMOD

#include "..\FMOD\fmod.hpp"
#include "Rave_Medio.h"


#ifdef _WIN64
	#pragma comment(lib, "..\\Libs\\fmod64_vc.lib")	// LibVLC x64
#else 
	#pragma comment(lib, "..\\Libs\\fmod_vc.lib")	// LibVLC x86
#endif


class RaveFMOD_Medio : public Rave_Medio {
  public:
								RaveFMOD_Medio(FMOD::System *SistemaFMOD, BDMedio &nMedio);
	                           ~RaveFMOD_Medio(void);

	void                        Eliminar(void);
							    // Controles básicos de reproducción
	const BOOL					Play(void);
	const BOOL					Pausa(void);
	const BOOL					Stop(void);

								// Volumen del medio
	const int					Volumen(void);
	void						Volumen(int nVolumen, const BOOL ActualizarUI = TRUE);

	const Estados_Medio			ComprobarEstado(void);

								// Tiempo del medio
	const float					TiempoActual(void);
	void						TiempoActual(float nTiempo);
	const UINT64				TiempoTotalMs(void);
	const UINT64				TiempoActualMs(void);

	

	const Rave_Medio_Tipo		Tipo(void) { return Rave_Medio_Tipo_FMOD; };
  protected:
	FMOD::Sound               *_Stream;
	FMOD::Channel             *_Canal;
	FMOD::System              *_Sistema;
	FMOD::ChannelGroup        *_GrupoCanales;

	Estados_Medio              _Estado;
};

#endif