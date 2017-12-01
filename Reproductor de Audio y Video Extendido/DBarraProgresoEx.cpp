#include "stdafx.h"
#include "DBarraProgresoEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBarraProgresoEx::DBarraProgresoEx(void) : DControlEx_FondoDinamico(), _Minimo(0), _Maximo(0), _Valor(0) {
	}


	DBarraProgresoEx::~DBarraProgresoEx(void) {
	}


	HWND DBarraProgresoEx::CrearBarraProgresoEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo, const float nMaximo, const float nValor) {
		if (hWnd()) { Debug_Escribir(L"DBarraProgresoEx::CrearBarraProgresoEx() Error : ya se ha creado la barra\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DBarraProgresoEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
		_ColorFondo = COLOR_BARRA_FONDO;
		_ColorBarra = COLOR_BARRA;
		_Minimo = nMinimo;
		_Maximo = nMaximo;
		_Valor = nValor;
		return hWnd();
	}

	void DBarraProgresoEx::Minimo(const float nMinimo) {
		_Minimo = nMinimo;
		Repintar();
	}

	void DBarraProgresoEx::Maximo(const float nMaximo) {
		_Maximo = nMaximo;
		Repintar();
	}

	void DBarraProgresoEx::Valor(const float nValor) {
		_Valor = nValor;
		Repintar();
	}

	void DBarraProgresoEx::PintarBarraEx(HDC DC) {
		RECT    RC, RBarra, RFondo;
		GetClientRect(hWnd(), &RC);
		// Creo un buffer en memória para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		// Calculo el tamaño de la barra para el valor actual
		RBarra = RC; RFondo = RC;
		RBarra.left++; RBarra.top++; RBarra.bottom--;  // Evito los bordes
		RFondo.top++; RFondo.bottom--; RFondo.right--; // Evito los bordes
		float Parte = static_cast<float>((RC.right - RC.left) - 2) / (_Maximo - _Minimo);
		RBarra.right = 1 + static_cast<int>(Parte * (_Valor - _Minimo));
		RFondo.left = RBarra.right;

		// Pinto el borde
		Evento_PintarBorde(Buffer, RC);

		// Pinto la barra
		Evento_PintarBarra(Buffer, RBarra);

		// Pinto el fondo
		Evento_PintarFondo(Buffer, RFondo);

		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}


	void DBarraProgresoEx::Evento_PintarBarra(HDC DC, RECT &RBarra) {
		HBRUSH BrochaBarra = CreateSolidBrush(_ColorBarra);
		FillRect(DC, &RBarra, BrochaBarra);
		DeleteObject(BrochaBarra);
	}

	void DBarraProgresoEx::Evento_PintarFondo(HDC DC, RECT &RFondo) {
		HBRUSH BrochaFondo = CreateSolidBrush(_ColorFondo);
		FillRect(DC, &RFondo, BrochaFondo);
		DeleteObject(BrochaFondo);
	}

	void DBarraProgresoEx::Evento_PintarBorde(HDC DC, RECT &RBorde) {
		HBRUSH BrochaBorde = CreateSolidBrush(_ColorBorde);
		FrameRect(DC, &RBorde, BrochaBorde);
		DeleteObject(BrochaBorde);
	}

	LRESULT CALLBACK DBarraProgresoEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_ERASEBKGND:
				PintarBarraEx(reinterpret_cast<HDC>(wParam));
				return TRUE;
			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				PintarBarraEx(DC);
				EndPaint(hWnd(), &PS);
				return 0;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}
}