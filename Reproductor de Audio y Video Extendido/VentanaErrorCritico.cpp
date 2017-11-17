#include "stdafx.h"
#include "VentanaErrorCritico.h"
#include "resource.h"
#include "RAVE_Colores.h"
#include "DMensajesWnd.h"

VentanaErrorCritico::VentanaErrorCritico() {
}


VentanaErrorCritico::~VentanaErrorCritico() {
}


HWND VentanaErrorCritico::Crear(void) {
	CrearVentana(L"Rave_ErrorCritico", L"Error crítico", 100, 100, 400, 280, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	
	RECT RC;
	GetClientRect(hWnd(), &RC);

	TextoTitulo.CrearLabelEx(hWnd, L"Error crítico!", 10, 10, RC.right - 20, 20, ID_VEC_TEXTOERROR, TRUE, WS_CHILD);
	TextoTitulo.Fuente(hWnd._FuenteB);
	TextoTitulo.ColorTexto(COLOR_BOTON_RESALTADO);
	TextoTitulo.hWnd.Visible(TRUE);

	std::wstring TE =	std::wstring(L"Se ha detectado un error, y el Reproductor de Audio y Video\nExtendido debe cerrarse.\n\n") + 
						std::wstring(L"Si lo deseas puedes mandar el informe de errores para que \n") +
						std::wstring(L"sea revisado pulsando el boton Enviar. De esta forma estaras \n") +
						std::wstring(L"ayudando al desarrollo de este reproductor.") +
						std::wstring(L"\n\nEn caso contrario pula el boton Salir. ");
	TextoError.CrearLabelEx(hWnd, TE.c_str(), 10, 40, 380, 130, ID_VEC_TEXTOERROR, FALSE, WS_CHILD | WS_VISIBLE);

	BarraProgreso.CrearBarraProgresoEx(hWnd, 10, 180, RC.right - 20, 10, ID_VEC_PROGRESO, 0.0f, 100.0f, 0.0f);
	
	BotonEnviar.CrearBotonEx(hWnd, L"Enviar", 85, 200, 100, 30, ID_VEC_ENVIAR);
	BotonSalir.CrearBotonEx(hWnd, L"Salir", 195, 200, 100, 30, ID_VEC_SALIR);
	return hWnd();
}

void VentanaErrorCritico::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}

void VentanaErrorCritico::Evento_BotonEx_Mouse_Click(const UINT nID) {
	switch (nID) {
		case ID_VEC_ENVIAR:
			break;
		case ID_VEC_SALIR:
			PostMessage(hWnd(), WM_CLOSE, 0, 0);
			break;
	}
}

LRESULT CALLBACK VentanaErrorCritico::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_ERASEBKGND:
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Mouse_Click(static_cast<UINT>(wParam));
			break;
	}
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}