#pragma once

#include "DhWnd.h"

class VentanaOpcionesRAVE : public DWL::DVentana {
  public:
						VentanaOpcionesRAVE();
	                   ~VentanaOpcionesRAVE();
	void				Crear();

	void				Evento_BorraFondo(HDC DC);

	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

};

