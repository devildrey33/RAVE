#include "stdafx.h"
#include "VerVideo.h"
#include "DMouse.h"
#include "Rave_Skin.h"
#include "resource.h"

VerVideo::VerVideo() {
}


VerVideo::~VerVideo() {
}


HWND VerVideo::Crear(DhWnd *Padre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID) {
	return DControlEx::CrearControlEx(Padre, L"VerVideo", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD, NULL, CS_DBLCLKS /*| CS_VREDRAW | CS_HREDRAW */);
}

void VerVideo::Evento_MenuEx_Click(const UINT cID) {
/*	switch (cID) {
		case ID_MENUVIDEO_AUDIO:
			break;
		case ID_MENUVIDEO_VIDEO:
			break;
		case ID_MENUVIDEO_SUBTITULOS:
			break;
	}*/
	// Des-marco todas las pistas de audio
	if (cID >= ID_MENUVIDEO_AUDIO_PISTAS_AUDIO && cID < ID_MENUVIDEO_AUDIO_PISTAS_AUDIO_FIN) {
		for (size_t i = 0; i < App.MenuPistasDeAudio->TotalMenus(); i++) {
			App.MenuPistasDeAudio->Menu(i)->Icono(0);
		}
	}
	// Marco la pista de audio actual (si existe, ... que debería)
	DMenuEx *MenuClick = App.MenuPistasDeAudio->BuscarMenu(cID);
	if (MenuClick != NULL) MenuClick->Icono(IDI_CHECK2);

	App.VLC.AsignarPistaAudio(cID - ID_MENUVIDEO_AUDIO_PISTAS_AUDIO);
}

LRESULT CALLBACK VerVideo::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static POINT nPos = { 0 , 0 };
	switch (uMsg) {
		case WM_COMMAND:
			Evento_MenuEx_Click(LOWORD(wParam));
			return 0;

/*		case WM_ERASEBKGND :
			return 1;*/
		case WM_PAINT :
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(hWnd(), &PS);
			Pintar(DC);
			EndPaint(hWnd(), &PS);
			return 0;
		case WM_SIZING:
			Repintar();
			return 0;
		case WM_MOUSEMOVE :
//			Debug_Escribir(L"VerView::GestorMensajes uMsg = WM_MOUSEMOVE\n");
			DWL::DMouse::ObtenerPosicion(&nPos);
			if (App.VentanaRave.PantallaCompleta() == TRUE) {
				if (App.VentanaRave.MousePos.x != nPos.x || App.VentanaRave.MousePos.y != nPos.y) {
					App.VentanaRave.MousePos = nPos;
					App.ControlesPC.Mostrar();
				}
			}
			return 0;
		case WM_LBUTTONDBLCLK :
		case WM_RBUTTONDBLCLK :
		case WM_MBUTTONDBLCLK :
			App.VentanaRave.PantallaCompleta(!App.VentanaRave.PantallaCompleta());
			return 0;
		case WM_RBUTTONUP:
			App.VentanaRave.Menu_Video.Mostrar(this);
			return 0;
/*		case WM_KEYUP:
			if (wParam == VK_ESCAPE) {
				App.VentanaRave.PantallaCompleta(FALSE);
			}
			return 0;*/

	}
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}


void VerVideo::Pintar(HDC hDC) {
	RECT R;
	GetClientRect(hWnd(), &R);

//	HBRUSH Fondo = CreateSolidBrush(COLOR_TOOLTIP_FONDO);
	FillRect(hDC, &R, (HBRUSH)GetStockObject(BLACK_BRUSH));
//	DeleteObject(Fondo);
}