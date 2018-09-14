#include "stdafx.h"
#include "DToolTipEx.h"
#include "Rave_Skin.h"
#include <dwmapi.h>

#define ID_TEMPORIZADOR_OCULTAR	1000

namespace DWL {


	DToolTipEx::DToolTipEx() : _Fuente(NULL) {
	}


	DToolTipEx::~DToolTipEx() {
	}


	HWND DToolTipEx::CrearToolTipEx(DhWnd_Fuente Fuente) {
		_hWnd = DVentana::CrearVentana(NULL, L"DToolTipEx", L"", 0, 0, 0, 0, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		MARGINS Margen = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(_hWnd, &Margen);
		_Fuente = Fuente; // _Fuente18Normal;
		return hWnd();
	}


	SIZE DToolTipEx::CalcularTam(std::wstring &Str) {
		// Calculo el tamaño del texto (una sola línea de texto)
		HDC	   		hDC		= CreateCompatibleDC(NULL);
		HBITMAP		Bmp		= CreateCompatibleBitmap(hDC, 1, 1);
		HBITMAP		Viejo	= static_cast<HBITMAP>(SelectObject(hDC, Bmp));
		HFONT       vFuente = static_cast<HFONT>(SelectObject(hDC, _Fuente()));
		//	HFONT		FViejo = static_cast<HFONT>(SelectObject(hDC, Estilos.Fuentes.Normal()));
		SIZE		Ret		= { 0, 0 };
		//	int			Padding = 8; // 8 pixeles de margen

		GetTextExtentPoint32(hDC, Str.c_str(), static_cast<int>(Str.size()), &Ret);

		SelectObject(hDC, vFuente);
		SelectObject(hDC, Viejo);
		DeleteObject(Bmp);
		DeleteDC(hDC);

		Ret.cx += (DTOOLTIPEX_PADDING * 2);
		Ret.cy += (DTOOLTIPEX_PADDING * 2);
		return Ret;
	}

	// Muestra el tooltip centrado horizontalmente en las coordenadas especificadas
	void DToolTipEx::Mostrar(const int cX, const int cY, std::wstring &Str) {
		SIZE Ret = CalcularTam(Str);
		_Str = Str;
		MoveWindow(hWnd(), cX - static_cast<int>(Ret.cx / 2), cY, Ret.cx, Ret.cy, FALSE);
		ShowWindow(hWnd(), SW_SHOWNOACTIVATE);
		RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
	}

	void DToolTipEx::Mostrar(const int cX, const int cY, std::wstring &Str, const int cAncho, const int cAlto) {
		_Str = Str;
		MoveWindow(hWnd(), cX, cY, cAncho, cAlto, FALSE);
		
		//ShowWindow(hWnd(), SW_SHOWNOACTIVATE);
		AnimateWindow(_hWnd, 200, AW_BLEND);
//		RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);

		SetTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR, 5000, NULL);
	}

	void DToolTipEx::Ocultar(void) {
		ShowWindow(hWnd(), SW_HIDE);
	}

	void DToolTipEx::OcultarAnimado(void) {
		AnimateWindow(_hWnd, 200, AW_HIDE | AW_BLEND);
//		ShowWindow(hWnd(), SW_HIDE);
	}

	void DToolTipEx::Pintar(HDC DC) {
		RECT RC, RCF;
		GetClientRect(hWnd(), &RC);
		RCF = RC; RCF.left++; RCF.top++; RCF.right--; RCF.bottom--;
		// Creo un buffer en memória para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		HBRUSH BrochaBorde = CreateSolidBrush(COLOR_TOOLTIP_BORDE);
		FrameRect(Buffer, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_TOOLTIP_FONDO);
		FillRect(Buffer, &RCF, BrochaFondo);
		DeleteObject(BrochaFondo);

		SetBkMode(Buffer, TRANSPARENT);
		SetTextColor(Buffer, COLOR_TOOLTIP_TEXTO);
		HFONT vFuente = static_cast<HFONT>(SelectObject(Buffer, _Fuente()));
		TextOut(Buffer, DTOOLTIPEX_PADDING, DTOOLTIPEX_PADDING, _Str.c_str(), static_cast<int>(_Str.size()));
		SelectObject(Buffer, vFuente);

		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);

	}

	LRESULT CALLBACK DToolTipEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_TIMER :
				if (wParam == ID_TEMPORIZADOR_OCULTAR) OcultarAnimado();
				break;
			case WM_MOUSEMOVE:
				OcultarAnimado();
				break;
			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				Pintar(DC);
				EndPaint(hWnd(), &PS);
				return 0;
			// Print y Print Client (para AnimateWindow)
			case WM_PRINTCLIENT:   
			case WM_PRINT:
				Pintar(reinterpret_cast<HDC>(wParam));																										return 0;

			// Sombra de la ventana
			// https://stackoverflow.com/questions/43818022/borderless-window-with-drop-shadow
			case WM_NCCALCSIZE:
				if (wParam == TRUE)	{
					// DWL_MSGRESULT (no esta definit)
					SetWindowLongPtr(_hWnd, 0, 0);
					return TRUE;
				}				
				return 0;
			// Evita que al obtener el foco cambie la parte del caption
			case WM_NCACTIVATE:																																					return FALSE;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}
}