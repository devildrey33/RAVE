#include "stdafx.h"
#include "VentanaErrorCritico.h"
#include "resource.h"

VentanaErrorCritico::VentanaErrorCritico() {
}


VentanaErrorCritico::~VentanaErrorCritico() {
}


HWND VentanaErrorCritico::Crear(void) {
	CrearVentana(L"Rave_ErrorCritico", L"Error crítico", 100, 100, 400, 380, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	TextoError.Crear(hWnd, L"Rave ha sufrido un error crítico y debe cerrase.", 10, 10, 380, 280, ID_VEC_TEXTOERROR, WS_CHILD);
	BotonEnviar.Crear(hWnd, L"Enviar", 85, 300, 100, 30, ID_VEC_ENVIAR);
	BotonSalir.Crear(hWnd, L"Salir", 195, 300, 100, 30, ID_VEC_SALIR);
	return hWnd();
}

LRESULT CALLBACK VentanaErrorCritico::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case WM_COMMAND :
			switch (LOWORD(wParam)) {
				case ID_VEC_ENVIAR:
					break;
				case ID_VEC_SALIR:
					PostMessage(hWnd(), WM_CLOSE, 0, 0);
					break;
			}
			break;
	}
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}