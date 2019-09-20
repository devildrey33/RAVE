#include "stdafx.h"
#include "VentanaOpcionesRAVE2.h"

#define ID_ARBOLSECCIONES	1000
#define ID_MARCOBD			1001

void VentanaOpcionesRAVE2::Crear(void) {
	DVentana::CrearVentana(NULL, L"RAVE_VentanaOpciones2", L"Opciones", App.Opciones.VentanaOpciones_PosX(), App.Opciones.VentanaOpciones_PosY(), 700, 350, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	ArbolSecciones.CrearArbolEx(this, 10, 10, 190, 330, ID_ARBOLSECCIONES, WS_CHILD | WS_VISIBLE);

	ArbolSecciones.AgregarNodo(L"Base de datos", NULL, 0);
	ArbolSecciones.AgregarNodo(L"General", NULL, 0);
	ArbolSecciones.AgregarNodo(L"Teclado", NULL, 0);
	ArbolSecciones.AgregarNodo(L"Listas", NULL, 0);
	ArbolSecciones.AgregarNodo(L"Canciones", NULL, 0);
	ArbolSecciones.AgregarNodo(L"Video", NULL, 0);


	MarcoBaseDatos.Crear(this, 210, 10, 480, 330, ID_MARCOBD);

	Visible(TRUE);
}

void VentanaOpcionesRAVE2::Evento_Cerrar(void) {
	ArbolSecciones.Destruir();
	Destruir();
	App.OcultarToolTipOpciones();
}

void VentanaOpcionesRAVE2::Evento_ListaEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
}

LRESULT CALLBACK VentanaOpcionesRAVE2::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE:		
			Evento_Cerrar();	
			return 0;
		case DWL_LISTAEX_MOUSESOLTADO:
			Evento_ListaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
	}

	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}