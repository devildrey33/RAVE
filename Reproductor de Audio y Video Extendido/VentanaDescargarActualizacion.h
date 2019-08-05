#pragma once

#include <DVentana.h>
#include <DBotonEx.h>
#include <DBarraProgresoEx.h>
#include <DEtiquetaEx.h>

class VentanaDescargarActualizacion : public DWL::DVentana {
  public:
	void					Crear(const wchar_t* Version);

//	void					Cancelar(void);

	DWL::DBotonEx			BotonCancelar;
	DWL::DBotonEx			BotonNovedades;
	DWL::DEtiquetaEx		Etiqueta;
	DWL::DBarraProgresoEx	Barra;

	void					Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);

	LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

