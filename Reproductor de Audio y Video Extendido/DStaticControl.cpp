#include "stdafx.h"
#include "DStaticControl.h"

namespace DWL {

	HWND DStaticControl::Crear(DhWnd *nPadre, const wchar_t *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, BOOL nVisible) {
		if (hWnd()) { Debug_Escribir(L"DStaticControl::Crear() Error : ya se ha creado el label\n"); return hWnd(); }
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
		_hWnd = CreateWindowEx(NULL, L"STATIC", nTexto, nEstilos, cX, cY, cAncho, cAlto, hWndPadre, reinterpret_cast<HMENU>(IntToPtr(cID)), GetModuleHandle(NULL), this);
		_ConectarControl(cID, nPadre);
		Visible(nVisible);
		return hWnd();
	}


	void DStaticControl::Texto(const wchar_t *nTexto) {
		
		// Compruebo que el nuevo texto sea distinto al actual para evitar un efecto de flicknes
/*		static wchar_t TmpStr[512];
		GetWindowText(hWnd(), TmpStr, 512);
		if (wcscmp(nTexto, TmpStr) != 0)*/
			SetWindowText(hWnd(), nTexto);
	}
	/*
	void DStaticControl::Evento_Pintar(void) {
		PAINTSTRUCT		PS;
		HDC				hDC		= BeginPaint(hWnd(), &PS);
		LONG_PTR		Estilos = GetWindowLongPtr(hWnd(), GWL_STYLE);		
		HBRUSH			Fondo	= GetSysColorBrush(COLOR_3DFACE);
		RECT			R;
		static wchar_t	TmpStr[512];
		GetWindowText(hWnd(), TmpStr, 512);
		GetClientRect(hWnd(), &R);

		// BackBuffer 
		HDC     TmphDC			= CreateCompatibleDC(NULL);
		HBITMAP TmphDCBmp		= CreateCompatibleBitmap(hDC, R.right, R.bottom);
		HBITMAP VTmphDCBmp		= static_cast<HBITMAP>(SelectObject(TmphDC, TmphDCBmp));
		HFONT	VFont			= static_cast<HFONT>(SelectObject(TmphDC, hWnd._Fuente));
		
		FillRect(TmphDC, &R, Fondo);

		UINT Formato = DT_LEFT;
		if (Estilos & SS_RIGHT)		Formato = DT_RIGHT;
		if (Estilos & SS_CENTER)	Formato = DT_CENTER;
		SetBkMode(TmphDC, TRANSPARENT);
		DrawText(TmphDC, TmpStr, -1, &R, Formato);

		BitBlt(hDC, 0, 0, R.right, R.bottom, TmphDC, 0, 0, SRCCOPY);
		EndPaint(hWnd(), &PS);

		SelectObject(TmphDC, VFont);
		SelectObject(TmphDC, VTmphDCBmp);
		DeleteObject(TmphDCBmp);
		DeleteDC(TmphDC);


	}

	LRESULT CALLBACK DStaticControl::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT :
				Evento_Pintar();
				return 0;
		}
		return DControl::GestorMensajes(uMsg, wParam, lParam);
	}*/

	/*void DStaticControl::Texto() {
		GetWindowText()
	}*/
}