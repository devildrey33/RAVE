#pragma once

#include "RaveBD.h"
#include "DAnimacion.h"

class RaveVLC_Medio {
  public :
								RaveVLC_Medio(libvlc_instance_t	*Instancia, BDMedio &Medio);
							   ~RaveVLC_Medio(void);

	void                        Eliminar(void);
							    
							    // Controles básicos de reproducción
	const BOOL					Play(void);
	const BOOL					Pausa(void);
	const BOOL					Stop(void);
								// Aumenta o disminuye el ratio de reproducción (POR DEFECTO ES 1.0)
	void                        Ratio(const float R);

								// Volumen del medio
	const int					Volumen(void);
	void						Volumen(int nVolumen, const BOOL ActualizarUI = TRUE);

	Estados_Medio				ComprobarEstado(void);

								// Tiempo del medio
	const float					TiempoActual(void);
	void						TiempoActual(float nTiempo);
	const UINT64				TiempoTotalMs(void);
	const UINT64				TiempoActualMs(void);
	
								// Proporción para el video
	std::wstring               &ObtenerProporcion(void);
	void                        AsignarProporcion(const char *Prop);

								// Filtros para el vídeo
	void                        Brillo(const float nBrillo);
	void                        Contraste(const float nContraste);
	void                        Gamma(const float nGamma);
	void                        Hue(const int nHue);
	void                        Saturacion(const float nSaturacion);
								// Pista de audio para el video
	void						AsignarPistaAudio(int nPista);

//	UINT64                      TiempoTotal;
	BDMedio                     Medio;		// Datos del medio
//	HWND                        hWndVLC;

								// Actualiza los iconos de la BD i la lista
	void						ActualizarIconos(int nIcono);
	const BOOL                  ObtenerDatosParsing(void);

	std::wstring                TxtError;

	void                        FadeIn(void);
	void                        FadeOut(void);
  protected :
	libvlc_media_player_t	  *_Medio;
	libvlc_event_manager_t    *_Eventos;
	BOOL                       _Parseado;

	DWL::DAnimacion            _AniVolumen;
};