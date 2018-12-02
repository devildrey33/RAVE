#include "stdafx.h"
#include "DToolTipEx.h"
//#include "Rave_Skin.h"
#include <dwmapi.h>

#define ID_TEMPORIZADOR_OCULTAR	1000

namespace DWL {

	COLORREF     DToolTipEx_Skin::Fondo				= COLOR_TOOLTIP_FONDO;
	COLORREF     DToolTipEx_Skin::Texto				= COLOR_TOOLTIP_TEXTO;
	COLORREF     DToolTipEx_Skin::TextoSombra		= COLOR_TOOLTIP_TEXTO_SOMBRA;
	COLORREF     DToolTipEx_Skin::Borde				= COLOR_TOOLTIP_BORDE;
	// Fuente
	int			 DToolTipEx_Skin::FuenteTam			= FUENTE_NORMAL;
	std::wstring DToolTipEx_Skin::FuenteNombre		= FUENTE_NOMBRE;
	BOOL         DToolTipEx_Skin::FuenteNegrita		= FALSE;
	BOOL         DToolTipEx_Skin::FuenteCursiva		= FALSE;
	BOOL         DToolTipEx_Skin::FuenteSubrayado	= FALSE;
	BOOL		 DToolTipEx_Skin::FuenteSombraTexto	= FALSE;



	DToolTipEx::DToolTipEx(void) : Padre(NULL) {
	}


	DToolTipEx::~DToolTipEx(void) {
	}


	HWND DToolTipEx::CrearToolTipEx(DhWnd *nPadre) {
		_hWnd = DVentana::CrearVentana(NULL, L"DToolTipEx", L"", 0, 0, 0, 0, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		Padre = nPadre->hWnd();
		while (GetParent(Padre) != NULL) {
			Padre = GetParent(Padre);
		}
		MARGINS Margen = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(_hWnd, &Margen);
		_Fuente.CrearFuente(DToolTipEx_Skin::FuenteTam, DToolTipEx_Skin::FuenteNombre.c_str(), DToolTipEx_Skin::FuenteNegrita, DToolTipEx_Skin::FuenteCursiva, DToolTipEx_Skin::FuenteSubrayado);
		return hWnd();
	}


	SIZE DToolTipEx::CalcularTam(std::wstring &Str) {
		// Calculo el tama�o del texto (una sola l�nea de texto)
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

		RedrawWindow(_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);

	}

	void DToolTipEx::Mostrar(const int cX, const int cY, std::wstring &Str, const int cAncho, const int cAlto) {
		_Str = Str;

		SetWindowPos(_hWnd, HWND_TOP, cX, cY, cAncho, cAlto, SWP_SHOWWINDOW | SWP_NOACTIVATE);
		
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
		RECT RCT = { RC.left, RC.top + DTOOLTIPEX_PADDING, RC.right, RC.bottom + (DTOOLTIPEX_PADDING * 2) };
		
		
		// Creo un buffer en mem�ria para pintar el control
		HDC		Buffer		= CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp			= CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		HBRUSH BrochaBorde = CreateSolidBrush(DToolTipEx_Skin::Borde);
		FrameRect(Buffer, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		HBRUSH BrochaFondo = CreateSolidBrush(DToolTipEx_Skin::Fondo);
		FillRect(Buffer, &RCF, BrochaFondo);
		DeleteObject(BrochaFondo);

		SetBkMode(Buffer, TRANSPARENT);

		HFONT vFuente = static_cast<HFONT>(SelectObject(Buffer, _Fuente()));
		
		if (DToolTipEx_Skin::FuenteSombraTexto == TRUE) {
			RECT RCTS = { RCT.left + 1, RCT.top + 1, RCT.right + 1, RCT.bottom + 1 };
			SetTextColor(Buffer, DToolTipEx_Skin::TextoSombra);
			DrawText(Buffer, _Str.c_str(), static_cast<int>(_Str.size()), &RCTS, DT_CENTER);
		}

		SetTextColor(Buffer, DToolTipEx_Skin::Texto);
		DrawText(Buffer, _Str.c_str(), static_cast<int>(_Str.size()), &RCT, DT_CENTER);

		//TextOut(Buffer, DTOOLTIPEX_PADDING, DTOOLTIPEX_PADDING, _Str.c_str(), static_cast<int>(_Str.size()));
		SelectObject(Buffer, vFuente);

		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la mem�ria
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
			case WM_MOUSEACTIVATE:
				return MA_NOACTIVATEANDEAT;

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
			case WM_NCACTIVATE:																																					return TRUE;
		}
		return DVentana::GestorMensajes(uMsg, wParam, lParam);
	}
}