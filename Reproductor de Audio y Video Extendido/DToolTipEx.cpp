#include "stdafx.h"
#include "DToolTipEx.h"
#include "Rave_Skin.h"
#include <dwmapi.h>

#define ID_TEMPORIZADOR_OCULTAR	1000

namespace DWL {


	DToolTipEx::DToolTipEx() : _Fuente(NULL), Padre(NULL) {
	}


	DToolTipEx::~DToolTipEx() {
	}


	HWND DToolTipEx::CrearToolTipEx(DhWnd_Fuente Fuente, DhWnd *nPadre) {
		_hWnd = DVentana::CrearVentana(nPadre, L"DToolTipEx", L"", 0, 0, 0, 0, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		Padre = nPadre->hWnd();
		while (GetParent(Padre) != NULL) {
			Padre = GetParent(Padre);
		}
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

//		GetTextExtentPoint32(hDC, Str.c_str(), static_cast<int>(Str.size()), &Ret);
		
		RECT Tam = { 0, 0, 0, 0 };
		int Al = DrawText(hDC, Str.c_str(), static_cast<int>(Str.size()), &Tam, DT_CALCRECT);

		SelectObject(hDC, vFuente);
		SelectObject(hDC, Viejo);
		DeleteObject(Bmp);
		DeleteDC(hDC);

		Ret.cx = Tam.right + (DTOOLTIPEX_PADDING * 2);
		Ret.cy = Tam.bottom + (DTOOLTIPEX_PADDING * 2);
		return Ret;
	}

	// Muestra el tooltip centrado horizontalmente en las coordenadas especificadas
	void DToolTipEx::Mostrar(const int cX, const int cY, std::wstring &Str) {
		SIZE Ret = CalcularTam(Str);
		_Str = Str;
//		MoveWindow(hWnd(), cX - static_cast<int>(Ret.cx / 2), cY, Ret.cx, Ret.cy, FALSE);

//		HWND Before = GetWindow(Padre, GW_HWNDNEXT);
		//if (Before == NULL) Before = HWND_TOP;
		SetWindowPos(_hWnd, HWND_TOP, cX - static_cast<int>(Ret.cx / 2), cY, Ret.cx, Ret.cy, SWP_SHOWWINDOW | SWP_NOACTIVATE);

		RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);

	}

	void DToolTipEx::Mostrar(const int cX, const int cY, std::wstring &Str, const int cAncho, const int cAlto) {
		_Str = Str;
//		MoveWindow(hWnd(), cX, cY, cAncho, cAlto, FALSE);
		HWND Before = GetWindow(Padre, GW_HWNDPREV);
		if (Before == NULL) {
			Before = HWND_TOP;
		}
		else {
			while (IsWindowVisible(Before) == FALSE) {
				if (GetWindow(Before, GW_HWNDPREV) != NULL) {
					Before = GetWindow(Before, GW_HWNDPREV);
				}
				else {
					break;
				}
			}
		}
		
/*		HWND After = GetWindow(Padre, GW_HWNDNEXT);
		wchar_t Txt[128], Txt2[128];
		wchar_t Txt3[128], Txt4[128];
		GetWindowText(Before, Txt, 128);
		GetClassName(Before, Txt2, 128);
		GetWindowText(After, Txt3, 128);
		GetClassName(After, Txt4, 128);
		Debug_Escribir_Varg(L"%s | %s ||,|| %s | %s\n", Txt, Txt2, Txt3, Txt4);*/
//		if (Before == NULL) Before = HWND_TOP;
		SetWindowPos(_hWnd, Before, cX, cY, cAncho, cAlto, SWP_SHOWWINDOW | SWP_NOACTIVATE);
		
//		ShowWindow(hWnd(), SW_SHOWNOACTIVATE);
//		AnimateWindow(_hWnd, 100, AW_BLEND);
		RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);

		SetTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR, 5000, NULL);
	}

	void DToolTipEx::Ocultar(void) {
		ShowWindow(hWnd(), SW_HIDE);
	}

	void DToolTipEx::OcultarAnimado(void) {
		AnimateWindow(_hWnd, 100, AW_HIDE | AW_BLEND);
//		ShowWindow(hWnd(), SW_HIDE);
	}

	void DToolTipEx::Pintar(HDC DC) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		RECT RCF = { RC.left + 1, RC.top + 1, RC.right - 1, RC.bottom - 1 };
		RECT RCT = { RC.left + DTOOLTIPEX_PADDING, RC.top + DTOOLTIPEX_PADDING, RC.right + (DTOOLTIPEX_PADDING * 2), RC.bottom + (DTOOLTIPEX_PADDING * 2) };
		
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
		
		DrawText(Buffer, _Str.c_str(), static_cast<int>(_Str.size()), &RCT, DT_LEFT);

		//TextOut(Buffer, DTOOLTIPEX_PADDING, DTOOLTIPEX_PADDING, _Str.c_str(), static_cast<int>(_Str.size()));
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
		return DVentana::GestorMensajes(uMsg, wParam, lParam);
	}
}