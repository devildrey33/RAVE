#pragma once

#include "DVentana.h"
#include "DEtiquetaEx.h"
#include "DBotonEx.h"
#include "DBarraProgresoEx.h"

class VentanaErrorCritico : public DVentana {
  public:
						VentanaErrorCritico();
		               ~VentanaErrorCritico();
	HWND				Crear(void);

	void				Evento_BorraFondo(HDC DC);
	void				Evento_BotonEx_Mouse_Click(DEventoMouse &DatosTeclado);
//	void				Evento_Temporizador(void);
	void				Enviar(void);


	DEtiquetaEx			TextoTitulo;
	DEtiquetaEx			TextoError;
	DBotonEx			BotonEnviar;
	DBotonEx			BotonSalir;
	DBarraProgresoEx	BarraProgreso;
	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);
  protected:
//	BOOL               _EstadoError;
};

