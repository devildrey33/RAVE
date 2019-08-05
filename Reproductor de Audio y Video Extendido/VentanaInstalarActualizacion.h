#pragma once

#include <DVentana.h>
#include <DBotonEx.h>
#include <DMarcaEx.h>
#include <DEtiquetaEx.h>

class VentanaInstalarActualizacion : public DWL::DVentana {
 public:
	 void					Crear(void);


	 DWL::DBotonEx			BotonCancelar;
	 DWL::DBotonEx			BotonInstalar;
	 DWL::DEtiquetaEx		Etiqueta;
	 DWL::DMarcaEx			MarcaNoBuscar;

	 void					Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	 void					Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);

	 LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

};

