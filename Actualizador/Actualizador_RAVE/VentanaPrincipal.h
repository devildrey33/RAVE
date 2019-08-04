#pragma once

#include <DVentana.h>
#include <DBarraProgresoEx.h>
#include <DEtiquetaEx.h>
#include <DBotonEx.h>

class VentanaPrincipal : public DWL::DVentana {
  public:
							VentanaPrincipal(void);
	void					Crear(void);

//	void					Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
//	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	DWL::DBarraProgresoEx	Barra;
	DWL::DEtiquetaEx		EtiquetaNombreArchivo;
	DWL::DEtiquetaEx		EtiquetaTextoActualizacion;
//	DWL::DBotonEx			BotonCancelar;
};

