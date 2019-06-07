#pragma once

#include "RaveBD.h"

enum Rave_Medio_Tipo {
	Rave_Medio_Tipo_NADA = 0,
	Rave_Medio_Tipo_VLC  = 1,
	Rave_Medio_Tipo_FMOD = 2
};

// Clase base para los medios del VLC o del FMOD
class Rave_Medio {
  public :
									Rave_Medio(BDMedio &nMedio)	: Medio(nMedio)				{ };

									/////////////////////////
									// Funciones virtuales //
									/////////////////////////
	  virtual void                  Eliminar(void)                                          { };

									// Controles
	  virtual const BOOL			Play(void)												{ return FALSE; };
	  virtual const BOOL			Pausa(void)												{ return FALSE; };
	  virtual const BOOL			Stop(void)												{ return FALSE; };

	  virtual const BOOL			Ratio(const float R)									{ return FALSE; };

	  virtual const int				Volumen(void)											{ return 0; };
	  virtual void					Volumen(int nVolumen, const BOOL ActualizarUI = TRUE)	{ };

									// Estado
	  virtual const Estados_Medio	ComprobarEstado(void)									{ return Estados_Medio::Nada; };

									// Tiempo del medio
	  virtual const float			TiempoActual(void)										{ return 0.0f; };
	  virtual void					TiempoActual(float nTiempo)								{ };
	  virtual const UINT64			TiempoTotalMs(void)										{ return 0; };
	  virtual const UINT64			TiempoActualMs(void)									{ return 0; };
	  virtual void					TiempoActualMs(UINT64 nTiempo)							{ };

									// Proporción para el video
	  virtual std::wstring         &ObtenerProporcion(void)									{ static std::wstring Nada; return Nada; };
	  virtual void                  AsignarProporcion(const char *Prop)						{ };

									// Filtros para el vídeo
	  virtual void					Brillo(const float nBrillo)								{ };
	  virtual void					Contraste(const float nContraste)						{ };
	  virtual void					Gamma(const float nGamma)								{ };
	  virtual void					Hue(const int nHue)										{ };
	  virtual void					Saturacion(const float nSaturacion)						{ };
									// Pista de audio para el video
	  virtual void					AsignarPistaAudio(int nPista)							{ };
	  virtual const Rave_Medio_Tipo Tipo(void)												{ return Rave_Medio_Tipo_NADA; };

	  virtual const BOOL			ObtenerDatosParsing(void) { return FALSE; };

									///////////////////////
									// Funciones propias //
									///////////////////////

									// Actualiza los iconos de la BD i la lista
	  void							ActualizarIconos(int nIcono);
	  void                          ComprobarMomento(void);

									///////////////
									// Variables //
									///////////////

	  std::wstring                  TxtError;
	  BDMedio						Medio;		// Datos del medio

};
