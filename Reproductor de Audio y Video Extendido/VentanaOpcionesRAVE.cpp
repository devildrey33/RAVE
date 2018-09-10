#include "stdafx.h"
#include "VentanaOpcionesRAVE.h"
#include "resource.h"

VentanaOpcionesRAVE::VentanaOpcionesRAVE(void) {
}

VentanaOpcionesRAVE::~VentanaOpcionesRAVE(void) {
}

void VentanaOpcionesRAVE::Crear(void) {
	HWND rhWnd = DVentana::CrearVentana(NULL, L"RAVE_VentanaOpciones", L"Opciones", 100, 100, 600, 400, WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	// Muestro la ventana principal
	ShowWindow(hWnd(), SW_SHOW);
}

void VentanaOpcionesRAVE::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}


LRESULT CALLBACK VentanaOpcionesRAVE::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
/*		case WM_PAINT:
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(hWnd(), &PS);
			Pintar(DC);
			EndPaint(hWnd(), &PS);
			return 0;*/
		case WM_ERASEBKGND:
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;

		// Sombra de la ventana
		// https://stackoverflow.com/questions/43818022/borderless-window-with-drop-shadow
		case WM_NCCALCSIZE:
			if (wParam == TRUE) {
				// DWL_MSGRESULT (no esta definit)
				SetWindowLongPtr(_hWnd, 0, 0);
				return TRUE;
			}
			return 0;
		// Evita que al obtener el foco cambie la parte del caption
		case WM_NCACTIVATE:
			return 0;
	}
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}
