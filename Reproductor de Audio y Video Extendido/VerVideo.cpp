#include "stdafx.h"
#include "VerVideo.h"
#include "DMouse.h"
#include "Rave_Skin.h"

VerVideo::VerVideo() {
}


VerVideo::~VerVideo() {
}


HWND VerVideo::Crear(DhWnd &Padre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID) {
	return DControlEx::CrearControlEx(Padre, L"VerVideo", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD, NULL, CS_DBLCLKS /*| CS_VREDRAW | CS_HREDRAW */);
}

LRESULT CALLBACK VerVideo::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static POINT nPos = { 0 , 0 };
	switch (uMsg) {
/*		case WM_ERASEBKGND :
			return 1;*/
		case WM_PAINT :
			return Pintar();
		case WM_SIZING:
			return Pintar();
		case WM_MOUSEMOVE :
//			Debug_Escribir(L"VerView::GestorMensajes uMsg = WM_MOUSEMOVE\n");
			DWL::DMouse::ObtenerPosicion(&nPos);
			if (App.VentanaRave.PantallaCompleta() == TRUE) {
				if (App.VentanaRave.MousePos.x != nPos.x || App.VentanaRave.MousePos.y != nPos.y) {
					App.VentanaRave.MousePos = nPos;
					App.ControlesPC.Mostrar();
				}
			}
			break;
		case WM_LBUTTONDBLCLK :
		case WM_RBUTTONDBLCLK :
		case WM_MBUTTONDBLCLK :
			App.VentanaRave.PantallaCompleta(!App.VentanaRave.PantallaCompleta());
			break;
	}
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}


LRESULT VerVideo::Pintar(void) {
	PAINTSTRUCT PS;
	HDC hDC = BeginPaint(hWnd(), &PS);
	
	//HBRUSH Negro = (HBRUSH)GetStockObject(BLACK_BRUSH);
	HBRUSH Fondo = CreateSolidBrush(COLOR_TOOLTIP_FONDO);
	RECT R;
	
	GetClientRect(hWnd(), &R);
	FillRect(hDC, &R, Fondo);

	DeleteObject(Fondo);
	EndPaint(hWnd(), &PS);
	return 0;
}