#include "stdafx.h"
#include "VentanaActualizacion.h"

#define ID_ETIQUETA		1000
#define ID_CANCELAR		1001
#define ID_BARRA		1002
#define ID_NOVEDADES	1003

void VentanaActualizacion::Crear(void) {
	CrearVentana(NULL, L"RAVE_Actualizacion", L"Actualización", 100, 100, 400, 160, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	Etiqueta.CrearEtiquetaEx(this, L"Descargando nueva actualización", 10, 10, RC.right - 20, RC.bottom - 70, ID_ETIQUETA);
	Barra.CrearBarraProgresoEx(this, 10, RC.bottom - 60, RC.right - 20, 10, ID_BARRA);
	BotonNovedades.CrearBotonEx(this, L"Novedades", (RC.right - 210) / 2, RC.bottom - 40, 100, 30, ID_NOVEDADES);
	BotonCancelar.CrearBotonEx(this, L"Cancelar", ((RC.right - 210) / 2) + 110, RC.bottom - 40, 100, 30, ID_CANCELAR);
	Visible(TRUE);
}


LRESULT CALLBACK VentanaActualizacion::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	}
	return DWL::DVentana::GestorMensajes(uMsg, wParam, lParam);
}