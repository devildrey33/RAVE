#pragma once
/*
#include "RaveVLC_Medio.h"
#include "DTemporizador.h"



class RaveVLC : public DWL::DTemporizador {
  public:
								RaveVLC(void);
							   ~RaveVLC(void);

	const BOOL					Iniciar(void);
	void						Terminar(void);
	
	static std::wstring        &UltimoError(void);

	const BOOL                  AbrirMedio(BDMedio &Medio, BDMedio *MedioSiguiente);
	void                        CerrarMedio(void);

	static void                 Eventos(const libvlc_event_t* event, void* ptr);

	const BOOL					Play(void);
	const BOOL					Pausa(void);
	const BOOL					Stop(void);
	const BOOL					StopTODO(void);

	const int					Volumen(void);
	void						Volumen(int nVolumen);

	Estados_Medio				ComprobarEstado(void);

	void						AsignarPistaAudio(int nPista);

	const float					TiempoActual(void);
	void						TiempoActual(float nTiempo);
	const UINT64				TiempoTotalMs(void);
	const UINT64				TiempoActualMs(void);

	void                        Ratio(const float R);

	//static void					TiempoStr(UINT64 TMS, wchar_t *StrTiempo);
	static void					TiempoStr(UINT64 TMS, std::wstring &StrTiempo);

	std::wstring			   &ObtenerProporcion(void);
	void						AsignarProporcion(const char *Prop);

	void                        Brillo(const float nBrillo);
	void                        Contraste(const float nContraste);
	void                        Gamma(const float nGamma);
	void                        Hue(const int nHue);
	void                        Saturacion(const float nSaturacion);

	BDMedio                    &MedioActual(void);

	const BOOL					ObtenerDatosParsing(void);	

//	void						Evento_Temporizador(const UINT_PTR cID);
	void                        Temporizador_Lista(void);
	void                        Temporizador_Tiempo(void);

	void                        Evento_Medio_Terminado(RaveVLC_Medio *MedioEvento);

								// Necesito encontrar la ventana del VLC que se crea dentro del VerVideo para repintar el fondo negro
	static BOOL CALLBACK		EnumeracionVLC(HWND hWndWnd, LPARAM lParam);
	static HWND                 hWndVLC;
  protected:	
	static RaveVLC_Medio      *_Anterior;
	static RaveVLC_Medio      *_Actual;
	static RaveVLC_Medio      *_Siguiente;

	libvlc_instance_t		  *_Instancia;
};


*/