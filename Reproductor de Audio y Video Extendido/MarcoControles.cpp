#include "stdafx.h"
#include "MarcoControles.h"
#include "DMensajesWnd.h"
//#include "Rave_Skin.h"

HWND MarcoControles::Crear(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID) {
//	HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
	return DControlEx::CrearControlEx(nPadre, L"MarcoControles", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
}



void MarcoControles::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}


LRESULT CALLBACK MarcoControles::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_ERASEBKGND:
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;

		case WM_HSCROLL :
		case DWL_MENUEX_CLICK:
		case DWL_MENUEX_BARRA_CAMBIANDO:
		case DWL_MENUEX_BARRA_CAMBIADO:
		case DWL_BARRAEX_CAMBIANDO:
		case DWL_BARRAEX_CAMBIADO:
		case DWL_BOTONEX_CLICK:
		case DWL_BOTONEX_MOUSEDOWN:
		case DWL_BOTONEX_MOUSEUP:
		case DWL_MARCAEX_CLICK :
		case DWL_DESPLEGABLEEX_CAMBIO :
		case DWL_LISTAEX_MOUSESOLTADO:
			return SendMessage(hWndPadre(), uMsg, wParam, lParam);
			break;
	}
	return DControlEx::GestorMensajes(uMsg, wParam, lParam);
}