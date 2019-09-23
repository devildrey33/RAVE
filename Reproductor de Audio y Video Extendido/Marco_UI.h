#pragma once

#include <DMarcoScrollEx.h>

class Marco_UI : public DWL::DMarcoScrollEx {
  public:
							Marco_UI(void) : DMarcoScrollEx() { }
	void					Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);


	void					Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	void					Evento_BarraEx_Cambiando(DWL::DEventoMouse& DatosMouse);
	void					Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse);
	void					Evento_ListaDesplegable_Cambio(INT_PTR nID);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	ControlSeparador        SeparadorUI;

	DEtiquetaEx			    EtiquetaTiempoAnimaciones;
	DEtiquetaEx             EtiquetaTiempoAnimacionesTA;
	DBarraDesplazamientoEx	BarraTiempoAnimaciones;
	DEtiquetaEx			    EtiquetaTiempoOcultarToolTips;
	DBarraDesplazamientoEx	BarraTiempoToolTips;
	DEtiquetaEx			    EtiquetaTiempoOcultarToolTipsTA;
	DWL::DMarcaEx			MarcaOcultarTooltipsMouse;
	DWL::DMarcaEx			MarcaMostrarNombreTitulo;

	ControlSeparador        SeparadorUI2;

	DEtiquetaEx			    EtiquetaPantallaCompleta;

	DEtiquetaEx             EtiquetaVideoTiempo;
	DBarraDesplazamientoEx	BarraVideoTiempo;
	DEtiquetaEx             EtiquetaVideoTiempoTA;
	DEtiquetaEx             EtiquetaVideoOpacidad;
	DBarraDesplazamientoEx	BarraVideoOpacidad;
	DEtiquetaEx             EtiquetaVideoOpacidadTA;
	DEtiquetaEx             EtiquetaVideoPosicion;
	DListaDesplegableEx     DesplegableVideoPosicion;

};

