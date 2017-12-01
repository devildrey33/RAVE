#include "stdafx.h"
#include "DLabelEx.h"
#include "Rave_Skin.h"

namespace DWL {
	DLabelEx::DLabelEx(void) : DControlEx_TextoEstatico(), _Formato(DT_LEFT) {
	}


	DLabelEx::~DLabelEx(void) {
	}

	HWND DLabelEx::CrearLabelEx(DhWnd &nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const BOOL nCentrado, const long Estilos) {
		if (hWnd()) { Debug_Escribir(L"DLabelEx::CreadLabelEx() Error : ya se ha creado el label\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DLabelEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);

		_Formato = (nCentrado == TRUE) ? DT_CENTER : DT_LEFT;
		_Fuente = hWnd._Fuente;

		_Texto = nTxt;
		return hWnd();
	}


	void DLabelEx::PintarLabelEx(HDC DC) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		RECT RC2 = RC;
		RC2.left++; RC2.top++; RC2.right++; RC2.bottom++;

		// BackBuffer 
		HDC     TmphDC = CreateCompatibleDC(NULL);
		HBITMAP TmphDCBmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP VTmphDCBmp = static_cast<HBITMAP>(SelectObject(TmphDC, TmphDCBmp));
		HFONT	VFont = static_cast<HFONT>(SelectObject(TmphDC, _Fuente));

		HBRUSH BrochaFondo = CreateSolidBrush(_ColorFondo);
		FillRect(TmphDC, &RC, BrochaFondo);
		DeleteObject(BrochaFondo);

		SetBkMode(TmphDC, TRANSPARENT);
		// Pinto la sombra del texto
		SetTextColor(TmphDC, _ColorTextoSombra);
		DrawText(TmphDC, _Texto.c_str(), static_cast<int>(_Texto.size()), &RC2, _Formato);
		// Pinto el texto
		SetTextColor(TmphDC, _ColorTexto);
		DrawText(TmphDC, _Texto.c_str(), static_cast<int>(_Texto.size()), &RC, _Formato);

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