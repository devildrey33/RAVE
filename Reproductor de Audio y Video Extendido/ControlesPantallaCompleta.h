#pragma once
#include "DhWnd.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"

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

//	void					Evento_BorraFondo(HDC DC);
	void                    Evento_Pintar(void);
	void					Pintar(HDC DC);

//	LRESULT					Evento_SliderH(WPARAM wParam, LPARAM lParam);

	void					Crear(void);
	void					Mostrar(void);
	void					Ocultar(void);

	DWL::DButton			BotonPlay;
	DWL::DButton			BotonPausa;
	DWL::DButton			BotonStop;
	DWL::DButton			BotonAdelante;
	DWL::DButton			BotonAtras;

	BarraTiempo				SliderTiempo;
//	DWL::DTrackBar			SliderTiempo;
	BarraVolumen			SliderVolumen;

	DWL::DLabelEx			LabelVolumen;

	DWL::DLabelEx			LabelTiempoActual;
	DWL::DLabelEx			LabelTiempoSeparador;
	DWL::DLabelEx			LabelTiempoTotal;

//	POINT					uPos;


	LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
  protected :
//	BOOL				   _Visible;
};

