#include "stdafx.h"
#include "DLabelEx.h"
#include "RAVE_Colores.h"

namespace DWL {
	DLabelEx::DLabelEx() {
	}


	DLabelEx::~DLabelEx() {
	}

	HWND DLabelEx::CrearLabelEx(DhWnd &nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		if (hWnd()) { Debug_Escribir(L"DLabelEx::CreadLabelEx() Error : ya se ha creado el label\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DLabelEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);

		_Texto = nTxt;
		return hWnd();
	}

	void DLabelEx::PintarLabelEx(HDC DC) {
/*		LONG_PTR		Estilos = GetWindowLongPtr(hWnd(), GWL_STYLE);
		HBRUSH			Fondo = GetSysColorBrush(COLOR_3DFACE);*/
		RECT			RC;
//		static wchar_t	TmpStr[512];
//		GetWindowText(hWnd(), TmpStr, 512);
		GetClientRect(hWnd(), &RC);

		// BackBuffer 
		HDC     TmphDC = CreateCompatibleDC(NULL);
		HBITMAP TmphDCBmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP VTmphDCBmp = static_cast<HBITMAP>(SelectObject(TmphDC, TmphDCBmp));
		HFONT	VFont = static_cast<HFONT>(SelectObject(TmphDC, hWnd._Fuente));

		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
		FillRect(TmphDC, &RC, BrochaFondo);
		DeleteObject(BrochaFondo);

		SetBkMode(TmphDC, TRANSPARENT);
		SetTextColor(TmphDC, COLOR_TEXTO);
		DrawText(TmphDC, _Texto.c_str(), -1, &RC, DT_CENTER);

		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, TmphDC, 0, 0, SRCCOPY);
//		EndPaint(hWnd(), &PS);

		SelectObject(TmphDC, VFont);
		SelectObject(TmphDC, VTmphDCBmp);
		DeleteObject(TmphDCBmp);
		DeleteDC(TmphDC);
	}

	void DLabelEx::Texto(std::wstring &nTexto) {
		_Texto = nTexto;
		Repintar();
	}

	LRESULT CALLBACK DLabelEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT :
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				PintarLabelEx(DC);
				EndPaint(hWnd(), &PS);
				return 0;
				break;
		}

		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}
}