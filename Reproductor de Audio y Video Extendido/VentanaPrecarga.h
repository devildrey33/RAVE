#pragma once

#include "DhWnd.h"

// Ventana que advierte de que se est� cargando la LibVLC
class VentanaPrecarga : public DWL::DVentana {
public:
						VentanaPrecarga();
	                   ~VentanaPrecarga();
	void                Pintar(HDC DC);
	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);
};

