#pragma once

#include <DVentana.h>
#include "Marco_BaseDatos.h"
#include <DArbolEx.h>

class VentanaOpcionesRAVE2 : public DWL::DVentana {
  public:
						VentanaOpcionesRAVE2(void) { };


	void				Crear(void);

	void				Evento_Cerrar(void);
	void				Evento_ListaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);

	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);


	DWL::DArbolEx       ArbolSecciones;

	Marco_BaseDatos		MarcoBaseDatos;


};

