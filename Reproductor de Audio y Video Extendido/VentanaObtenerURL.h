#pragma once

#include <DVentana.h>
#include <DEdicionTextoEx.h>
#include <DBotonEx.h>

class VentanaObtenerURL : public DWL::DVentana {
  public:
							VentanaObtenerURL(void) { };

	void					Mostrar(void);

	void					Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	DWL::DEtiquetaEx		Etiqueta;
	DWL::DEdicionTextoEx	EditURL;
	DWL::DBotonEx			BotonAbrir;
	DWL::DBotonEx			BotonCancelar;
};

