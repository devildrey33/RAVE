#pragma once

#include "Rave_Medio.h"
#include "DAnimacion.h"


#ifdef _WIN64
	#pragma comment(lib, "..\\Libs\\libvlc64.lib")	// LibVLC x64
#else 
	#pragma comment(lib, "..\\Libs\\libvlc.lib")	// LibVLC x86
#endif


class Rave_MediaPlayer;

class RaveVLC_Medio : public Rave_Medio {
  public :
								// Constructor para un medio
								RaveVLC_Medio(libvlc_instance_t	*Instancia, const size_t nInstanciaNum, BDMedio &Medio);
							   ~RaveVLC_Medio(void);

	void                        Eliminar(void);
							    
							    // Controles básicos de reproducción
	const BOOL					Play(void);
	const BOOL					Pausa(void);
	const BOOL					Stop(void);
								// Aumenta o disminuye el ratio de reproducción (POR DEFECTO ES 1.0)
	const BOOL                  Ratio(const float R);

								// Volumen del medio
	const int					Volumen(void);
	void						Volumen(int nVolumen, const BOOL ActualizarUI = TRUE);

	const Estados_Medio			ComprobarEstado(void);

								// Tiempo del medio
	const float					TiempoActual(void);
	void						TiempoActual(float nTiempo);
	const UINT64				TiempoTotalMs(void);
	const UINT64				TiempoActualMs(void);
	void						TiempoActualMs(UINT64 nTiempo);

								// Proporción para el video
	std::wstring               &ObtenerProporcion(void);
	void                        AsignarProporcion(const char* Prop);
	void                        AsignarProporcion(const wchar_t* Prop);

								// Filtros para el vídeo
	void                        Brillo(const float nBrillo);
	void                        Contraste(const float nContraste);
	void                        Gamma(const float nGamma);
	void                        Hue(const int nHue);
	void                        Saturacion(const float nSaturacion);
								// Pista de audio para el video
	void						AsignarPistaAudio(int nPista);
	const int                   AsignarSubtitulos(const wchar_t* Path);
	const int                   EnumerarSubtitulos(void);

//	UINT64                      TiempoTotal;
//	BDMedio                     Medio;		// Datos del medio
//	HWND                        hWndVLC;

								// Actualiza los iconos de la BD i la lista
//	void						ActualizarIconos(int nIcono);
	const BOOL                  ObtenerDatosParsing(void);

//	std::wstring                TxtError;

	void                        FadeIn(void);
	void                        FadeOut(void);

	inline const size_t			InstanciaNum(void) { return _InstanciaVLC; };

	const Rave_Medio_Tipo		Tipo(void) { return Rave_Medio_Tipo_VLC; };
  protected :
	libvlc_media_player_t	  *_Medio;
	libvlc_event_manager_t    *_Eventos;
	BOOL                       _Parseado;

	size_t					   _InstanciaVLC;

	DWL::DAnimacion            _AniVolumen;

	friend class Rave_MediaPlayer;
};