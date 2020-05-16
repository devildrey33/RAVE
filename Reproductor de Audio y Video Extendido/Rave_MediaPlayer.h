#pragma once

#include "RaveVLC_Medio.h"
#ifdef RAVE_UTILIZAR_FMOD
	#include "RaveFMOD_Medio.h"
#endif
#include "DVentana.h"

class Rave_MediaPlayer : public DWL::DVentana {
  public:
									Rave_MediaPlayer(void);
							       ~Rave_MediaPlayer(void);
								    // Inicia la VLC y el FMOD
	const BOOL						Iniciar(void);
	void							Terminar(void);
									// Abre el medio de la lista especificado
	const BOOL						AbrirMedio(ItemMedio *Medio);
									// Función que comprueba si el medio siguiente de la lista sigue siendo el medio siguiente del MediaPlayer, y en caso contrario, lo carga.
	void							AsegurarMedioSiguiente(void);
									// Abre el medio especificado y el medio siguiente (necesario para el efecto FADE IN OUT)
//	const BOOL						AbrirMedio(BDMedio &Medio, BDMedio *MedioSiguiente);
									// Cierra todos los medios (Anterior, Actual, y Siguiente)
	void							CerrarMedio(void);

	const BOOL						Play(const BOOL ComprobarMomento = FALSE);
	const BOOL						Pausa(void);
	const BOOL						Stop(void);
	const BOOL						StopTODO(void);

	const int						Volumen(void);
	void							Volumen(int nVolumen);
		
	Estados_Medio					ComprobarEstado(void);

	void							AsignarPistaAudio(int nPista);
	const int				        AsignarSubtitulos(const wchar_t* Path);
	const int                       EnumerarSubtitulos(void);

	const float						TiempoActual(void);
	void							TiempoActual(float nTiempo);
	const UINT64					TiempoTotalMs(void);
	const UINT64					TiempoActualMs(void);
	void							TiempoActualMs(UINT64 nTiempo);

	void						    Ratio(const float R);

	void                            AsignarTitulo(void);

									// CALLBACK para los eventos de la VLC
	static void						EventosVLC(const libvlc_event_t* event, void* ptr);
									// CALLBACK para los eventos del FMOD
#ifdef RAVE_UTILIZAR_FMOD
	static FMOD_RESULT F_CALLBACK   EventosFMOD(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2);
#endif
									// Formatea el tiempo especificado en el string especificado
	static void						TiempoStr(UINT64 TMS, std::wstring &StrTiempo);
									// Función que devuelve un string con un tiempo en milisegundos
	static const UINT64             TiempoStr_Ms(std::wstring& StrTiempo);

									// Temporizadores
	void							Temporizador_Lista(void);
	void							Temporizador_Tiempo(void);
	void							Temporizador_ObtenerVentanaVLC(void);

	std::wstring				   &ObtenerProporcion(void);
	void							AsignarProporcion(const int Prop);

	void				            Brillo(const float nBrillo);
	void				            Contraste(const float nContraste);
	void					        Gamma(const float nGamma);
	void				            Hue(const int nHue);
	void					        Saturacion(const float nSaturacion);

	BDMedio					       &MedioActual(void);
	void                            MedioActual(BDMedio& nMedio);

	const BOOL						ObtenerDatosParsing(void);
	
									// Asigna el tick en el que se ha salido de pantalla completa
	void                            AsignarTick_SalirPantallaCompleta(void);

	static std::wstring            &UltimoErrorVLC(void);

	LRESULT CALLBACK				GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

									// Necesito encontrar la ventana del VLC que se crea dentro del VerVideo para repintar el fondo negro
	static BOOL CALLBACK			EnumeracionVLC(HWND hWndWnd, LPARAM lParam);
	static HWND						hWndVLC;

//	void                            ObtenerTTF(void);
//	float                           TTF[2048];
  protected:
	void                           _TerminarMedio(Rave_Medio *tMedio);

	static void                    _EliminarRaveMedio(Rave_Medio *eMedio, const BOOL ActualizarIcono = TRUE);

	static HWND                    _hWndMensajes;

	static Rave_Medio             *_Anterior;
	static Rave_Medio             *_Actual;
	static Rave_Medio             *_Siguiente;

						    		// VLC
	libvlc_instance_t	    	  *_InstanciaVLC;

									// FMOD
#ifdef RAVE_UTILIZAR_FMOD
	FMOD::System                  *_SistemaFMOD;
	FMOD::DSP                     *_DSP;

	FMOD::ChannelGroup            *_CanalMaster;

	friend class RaveFMOD_Medio;
#endif 

	friend class RaveVLC_Medio;
};

