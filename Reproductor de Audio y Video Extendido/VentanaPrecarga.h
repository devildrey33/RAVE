#pragma once

#include "DVentana.h"
#include "DFuente.h"

// Ventana que advierte de que se está cargando la LibVLC
class VentanaPrecarga : public DWL::DVentana {
public:
						VentanaPrecarga(void);
	                   ~VentanaPrecarga(void);
	void                Pintar(HDC DC);
	DFuente             Fuente;
	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);
};

