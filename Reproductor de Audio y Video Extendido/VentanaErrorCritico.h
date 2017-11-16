#pragma once

#include "DhWnd.h"
#include "DStaticControl.h"
#include "DButton.h"

class VentanaErrorCritico : public DVentana {
  public:
						VentanaErrorCritico();
		               ~VentanaErrorCritico();
	HWND				Crear(void);

	DStaticControl		TextoError;
	DButton				BotonEnviar;
	DButton				BotonSalir;

	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);
};

