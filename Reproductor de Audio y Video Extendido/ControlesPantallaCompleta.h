#pragma once
#include "DVentana.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"
#include "DEtiquetaEx.h"

class ControlesPantallaCompleta :public DVentana {
  public:
							ControlesPantallaCompleta() : DVentana() { };
	                       ~ControlesPantallaCompleta() { };
							// Se está modificando (WM_MOUSEDOWN, WM_MOUSEHOVER)
	void					Evento_SliderTiempo_Cambio(void);
							// Se ha modificado (WM_MOUSEUP)
	void					Evento_SliderTiempo_Cambiado(void);

	void					Evento_SliderVolumen_Cambio(void);
	void					Evento_SliderVolumen_Cambiado(void);

	void					Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse);
	void					Evento_BarraEx_Cambio(DWL::DEventoMouse &DatosMouse);

//	void					Evento_BorraFondo(HDC DC);
	void                    Evento_Pintar(void);
	void					Pintar(HDC DC);

//	LRESULT					Evento_SliderH(WPARAM wParam, LPARAM lParam);

	void					Crear(void);
	void					Mostrar(void);
	void					Ocultar(void);

	DWL::DBotonEx			BotonPlay;
//	DWL::DBotonEx			BotonPausa;
	DWL::DBotonEx			BotonStop;
	DWL::DBotonEx			BotonAdelante;
	DWL::DBotonEx			BotonAtras;

	DWL::DBotonEx			BotonMezclar;
	DWL::DBotonEx			BotonRepetir;

	BarraTiempo				SliderTiempo;
//	DWL::DTrackBar			SliderTiempo;
	BarraVolumen			SliderVolumen;

	DWL::DEtiquetaEx		LabelVolumen;

	DWL::DEtiquetaEx		LabelTiempoActual;
	DWL::DEtiquetaEx		LabelTiempoSeparador;
	DWL::DEtiquetaEx		LabelTiempoTotal;

//	POINT					uPos;


	LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
  protected :
//	BOOL				   _Visible;
};

