#include "stdafx.h"
#include "ControlSeparador.h"


ControlSeparador::ControlSeparador(void) {
}


ControlSeparador::~ControlSeparador(void) {
}


void ControlSeparador::Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto) {
	CrearControlEx(nPadre, L"RAVE_ControlSeparador", L"", 0, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
}

void ControlSeparador::Pintar(HDC DC) {
	RECT RC;
	GetClientRect(_hWnd, &RC);

	// Creo un buffer en memória para pintar el control
	HDC		Buffer		= CreateCompatibleDC(NULL);
	// Creo un DC compatible para el buffer
	HBITMAP Bmp			= CreateCompatibleBitmap(DC, RC.right, RC.bottom);
	HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(Buffer, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	// Pinto la barra separadora
	HPEN PlumaSeparador = CreatePen(PS_SOLID, 1, COLOR_SEPARADOR);
	HPEN VPen = static_cast<HPEN>(SelectObject(Buffer, PlumaSeparador));
	int t = RC.bottom / 2;
	MoveToEx(Buffer, 10, RC.bottom / 2, NULL);
	LineTo(Buffer, RC.right -10, RC.bottom / 2);
	SelectObject(Buffer, VPen);
	DeleteObject(PlumaSeparador);

	// Copio el buffer al DC
	BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

	// Elimino el buffer de la memória
	SelectObject(Buffer, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(Buffer);

}

LRESULT CALLBACK ControlSeparador::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT :
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(_hWnd, &PS);
			Pintar(DC);
			EndPaint(_hWnd, &PS);
			break;

	}
	return DWL::DControlEx::GestorMensajes(uMsg, wParam, lParam);
}
