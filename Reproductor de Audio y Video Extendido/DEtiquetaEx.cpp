#include "stdafx.h"
#include "DEtiquetaEx.h"
#include "Rave_Skin.h"

namespace DWL {
	DEtiquetaEx::DEtiquetaEx(void) : _Formato(DT_LEFT), ColorTexto(COLOR_TEXTO), ColorTextoSombra(COLOR_TEXTO_SOMBRA), ColorFondo(COLOR_FONDO) {
	}


	DEtiquetaEx::~DEtiquetaEx(void) {
	}

	HWND DEtiquetaEx::CrearEtiquetaEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const BOOL nCentrado, const long Estilos) {
		_hWnd = CrearControlEx(nPadre, L"DEtiquetaEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);

		_Formato = (nCentrado == TRUE) ? DT_CENTER : DT_LEFT;
		Fuente = Fuente18Normal;
		_Texto = nTxt;
		return hWnd();
	}


	void DEtiquetaEx::Pintar(HDC DC) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		RECT RC2 = RC;
		RC2.left++; RC2.top++; RC2.right++; RC2.bottom++;

		// BackBuffer 
		HDC     TmphDC = CreateCompatibleDC(NULL);
		HBITMAP TmphDCBmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP VTmphDCBmp = static_cast<HBITMAP>(SelectObject(TmphDC, TmphDCBmp));
		HFONT	VFont = static_cast<HFONT>(SelectObject(TmphDC, Fuente()));

		HBRUSH BrochaFondo = CreateSolidBrush(ColorFondo);
		FillRect(TmphDC, &RC, BrochaFondo);
		DeleteObject(BrochaFondo);

		SetBkMode(TmphDC, TRANSPARENT);
		// Pinto la sombra del texto
		SetTextColor(TmphDC, ColorTextoSombra);
		DrawText(TmphDC, _Texto.c_str(), static_cast<int>(_Texto.size()), &RC2, _Formato);
		// Pinto el texto
		SetTextColor(TmphDC, ColorTexto);
		DrawText(TmphDC, _Texto.c_str(), static_cast<int>(_Texto.size()), &RC, _Formato);

		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, TmphDC, 0, 0, SRCCOPY);
//		EndPaint(hWnd(), &PS);

		SelectObject(TmphDC, VFont);
		SelectObject(TmphDC, VTmphDCBmp);
		DeleteObject(TmphDCBmp);
		DeleteDC(TmphDC);
	}

	void DEtiquetaEx::Texto(std::wstring &nTexto) {
		_Texto = nTexto;
		Repintar();
	}

	LRESULT CALLBACK DEtiquetaEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT :
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				Pintar(DC);
				EndPaint(hWnd(), &PS);
				return 0;
				break;
		}

		return DControlEx::GestorMensajes(uMsg, wParam, lParam);
	}
}