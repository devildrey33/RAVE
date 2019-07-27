#pragma once

#include <DVentana.h>
#include <DBotonEx.h>
#include <DBarraProgresoEx.h>

class VentanaActualizacion : public DWL::DVentana {
  public:
	void					Crear(void);

	DWL::DBotonEx			BotonCancelar;
	DWL::DBotonEx			BotonNovedades;
	DWL::DEtiquetaEx		Etiqueta;
	DWL::DBarraProgresoEx	Barra;

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);
};

