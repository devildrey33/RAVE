#pragma once

#include <DMarcoScrollEx.h>
#include <DListaDesplegableEx.h>

class Marco_Inicio : public DWL::DMarcoScrollEx {
  public:
							Marco_Inicio(void) : DMarcoScrollEx() { }
	void					Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

	void					Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	void					Evento_ListaDesplegable_Cambio(INT_PTR nID);


	void					ActualizarListaInicio(void);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	DWL::DMarcaEx			MarcaBuscarActualizaciones;

	ControlSeparador        SeparadorInicio;

	DEtiquetaEx             EtiquetaListaInicio;
	DListaDesplegableEx     DesplegableListaInicio;

};

