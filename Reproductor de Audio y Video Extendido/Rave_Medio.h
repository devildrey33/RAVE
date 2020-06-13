#pragma once

#include "RaveBD.h"
#include "ItemMedio.h"

enum Rave_Medio_Tipo {
	Rave_Medio_Tipo_NADA = 0,
	Rave_Medio_Tipo_VLC  = 1,
	Rave_Medio_Tipo_FMOD = 2
};



// Clase base para los medios del VLC o del FMOD
class Rave_Medio {
  public :
									Rave_Medio(ItemMedio *nMedio);

									/////////////////////////
									// Funciones virtuales //
									/////////////////////////
	  virtual void                  Eliminar(void)										            { };

									// Controles
	  virtual const BOOL			Play(void)														{ return FALSE; };
	  virtual const BOOL			Pausa(void)														{ return FALSE; };
	  virtual const BOOL			Stop(void)														{ return FALSE; };

	  virtual const BOOL			Ratio(const float R)											{ return FALSE; };

	  virtual const int				Volumen(void)													{ return 0; };
	  virtual void					Volumen(const long nVolumen, const BOOL ActualizarUI = TRUE)	{ };

									// Estado
	  virtual const Estados_Medio	ComprobarEstado(void)											{ return Estados_Medio::Nada; };

									// Tiempo del medio
	  virtual const float			TiempoActual(void)												{ return 0.0f; };
	  virtual void					TiempoActual(float nTiempo)										{ };
	  virtual const UINT64			TiempoTotalMs(void)												{ return 0; };
	  virtual const UINT64			TiempoActualMs(void)											{ return 0; };
	  virtual void					TiempoActualMs(UINT64 nTiempo)									{ };

									// Proporción para el video
	  virtual std::wstring         &ObtenerProporcion(void)											{ static std::wstring Nada; return Nada; };
	  virtual void                  AsignarProporcion(const char *Prop)								{ };

									// Filtros para el vídeo
	  virtual void					Brillo(const float nBrillo)										{ };
	  virtual void					Contraste(const float nContraste)								{ };
	  virtual void					Gamma(const float nGamma)										{ };
	  virtual void					Hue(const int nHue)												{ };
	  virtual void					Saturacion(const float nSaturacion)								{ };
									// Pista de audio para el video
	  virtual void					AsignarPistaAudio(int nPista)									{ };
	  virtual const int             AsignarSubtitulos(const wchar_t* Path)							{ return 0; };
	  virtual const int             EnumerarSubtitulos(void)										{ return 0; };
	  virtual const Rave_Medio_Tipo Tipo(void)														{ return Rave_Medio_Tipo_NADA; };

	  virtual void                  FadeIn(void)													{ };
	  virtual void                  FadeOut(void)													{ };


	  virtual const BOOL			ObtenerDatosParsing(void)										{ return FALSE; };

									// Obtiene el numero de instancia que se esta usando del VLC
	  virtual const size_t			InstanciaNum(void)												{ return 0; };
			

									///////////////////////
									// Funciones propias //
									///////////////////////

									// Asigna el tick en el que se ha salido de pantalla completa
	  void                          Evento_SalirPantallaCompleta(void);

									// Actualiza los iconos de la BD i la lista
	  void							ActualizarIconos(int nIcono, const BOOL ActualizarBotones = TRUE);
	  void                          ComprobarMomento(void);
	  
									///////////////
									// Variables //
									///////////////

	  std::wstring                  TxtError;
	  ItemMedio					   *Medio;		// Datos del medio

    protected:
									// Para el fade in / out
	  DWL::DAnimacion              _AniVolumen;


  								    // Tick en el que se ha pasado de pantalla completa a pantalla normal
								    // Al cambiar de pantalla completa a normal con doble click, puede pasar que el ultimo click suceda encima de la barra de tiempo, y este sea modificado
								    // Por eso, no se debe cambiar el tiempo del medio durante _SalirPantallaCompleta + 100 
	  ULONGLONG                    _SalirPantallaCompleta;


	  friend class Rave_MediaPlayer;
};
