#pragma once
#include "DVentana.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"
#include "DEtiquetaEx.h"
#include "DAnimacion.h"

enum CPC_Alineacion {
	Abajo,
	Izquierda,
	Arriba,
	Derecha
};

class ControlesPantallaCompleta :public DVentana {
  public:
							ControlesPantallaCompleta() : DVentana(), Alineacion(Abajo) { };
	                       ~ControlesPantallaCompleta() { };
							// Se est� modificando (WM_MOUSEDOWN, WM_MOUSEHOVER)
	void					Evento_SliderTiempo_Cambio(void);
							// Se ha modificado (WM_MOUSEUP)
	void					Evento_SliderTiempo_Cambiado(void);

	void					Evento_SliderVolumen_Cambio(void);
	void					Evento_SliderVolumen_Cambiado(void);

	void					Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse);
	void					Evento_BarraEx_Cambio(DWL::DEventoMouse &DatosMouse);

	void					Evento_BotonEx_Click(DWL::DEventoMouse &DatosMouse);

//	void					Evento_BorraFondo(HDC DC);
	void                    Evento_Pintar(void);
	void					Pintar(HDC DC);

	void                    Alinear(void);

//	LRESULT					Evento_SliderH(WPARAM wParam, LPARAM lParam);

	void					Crear(void);
	void					Mostrar(void);
	void					Ocultar(void);

	DWL::DBotonEx			BotonPlay;
//	DWL::DBotonEx			BotonPausa;
	DWL::DBotonEx			BotonStop;
	DWL::DBotonEx			BotonAdelante;
	DWL::DBotonEx			BotonAtras;

	DWL::DEtiquetaEx		LabelRatio;

	DWL::DBotonEx			BotonMezclar;
	DWL::DBotonEx			BotonRepetir;

	DWL::DBotonEx           BotonRotar;

	BarraTiempo				SliderTiempo;
//	DWL::DTrackBar			SliderTiempo;
	BarraVolumen			SliderVolumen;

	DWL::DEtiquetaEx		LabelVolumen;

	DWL::DEtiquetaEx		LabelTiempoActual;
	DWL::DEtiquetaEx		LabelTiempoSeparador;
	DWL::DEtiquetaEx		LabelTiempoTotal;

	CPC_Alineacion		    Alineacion;

	void					RotarControles(void);

	LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
  protected :
//	BOOL				   _Visible;


};

