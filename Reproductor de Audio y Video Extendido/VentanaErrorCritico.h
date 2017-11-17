#pragma once

#include "DhWnd.h"
#include "DLabelEx.h"
#include "DBotonEx.h"
#include "DBarraProgresoEx.h"

class VentanaErrorCritico : public DVentana {
  public:
						VentanaErrorCritico();
		               ~VentanaErrorCritico();
	HWND				Crear(void);

	void				Evento_BorraFondo(HDC DC);
	void				Evento_BotonEx_Mouse_Click(const UINT nID);

	DLabelEx			TextoTitulo;
	DLabelEx			TextoError;
	DBotonEx			BotonEnviar;
	DBotonEx			BotonSalir;
	DBarraProgresoEx	BarraProgreso;
	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);
};

