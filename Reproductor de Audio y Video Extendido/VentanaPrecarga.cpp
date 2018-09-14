#include "stdafx.h"
#include "VentanaPrecarga.h"
#include <dwmapi.h>
#include "resource.h"

#define VENTANAPRECARGA_ANCHO 200
#define VENTANAPRECARGA_ALTO  175

VentanaPrecarga::VentanaPrecarga(void) {

	int PosX = App.BD.Opciones_PosX() + (App.BD.Opciones_Ancho() - VENTANAPRECARGA_ANCHO) / 2;
	int PosY = App.BD.Opciones_PosY() + (App.BD.Opciones_Alto() - VENTANAPRECARGA_ALTO) / 2;

	_hWnd = DVentana::CrearVentana(NULL, L"RAVE_VentanaPrecarga", L"", PosX, PosY, VENTANAPRECARGA_ANCHO, VENTANAPRECARGA_ALTO, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
	MARGINS Margen = { 0, 0, 0, 1 };
	DwmExtendFrameIntoClientArea(_hWnd, &Margen);
	// Asigno la posición del menú y lo hago siempre visible
	SetWindowPos(_hWnd, HWND_TOPMOST, PosX, PosY, VENTANAPRECARGA_ANCHO, VENTANAPRECARGA_ALTO, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	App.Eventos_Mirar();
}


VentanaPrecarga::~VentanaPrecarga(void) {
}

void VentanaPrecarga::Pintar(HDC hDC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);

	// Creo un buffer en memória para pintar el control
	HDC		DC			= CreateCompatibleDC(NULL);
	// Creo un DC compatible para el buffer
	HBITMAP Bmp			= CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
	HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(DC, Bmp));

	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO_CLARO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	// Pinto el borde
	HBRUSH BrochaBorde = CreateSolidBrush(COLOR_MENU_BORDE);
	FrameRect(DC, &RC, BrochaBorde);
	DeleteObject(BrochaBorde);

	// Pinto el texto
	SetBkMode(DC, TRANSPARENT);
	SetTextColor(DC, COLOR_TEXTO_SOMBRA);
	HFONT vFuente = static_cast<HFONT>(SelectObject(DC, Fuente21Negrita()));
	const wchar_t Cargando[] = L"Cargando LibVLC...";
	TextOut(DC, 20, 140, Cargando, static_cast<int>(wcslen(Cargando)));
	SelectObject(DC, vFuente);

	// Pinto el icono
	HICON hIcono = static_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_VLC), IMAGE_ICON, 128, 128, LR_DEFAULTCOLOR));

	DrawIconEx(DC, (RC.right - 128) / 2, 10, hIcono, 128, 128, 0, 0, DI_NORMAL);
	DeleteObject(hIcono);

	// Copio el buffer al DC
	BitBlt(hDC, RC.left, RC.top, RC.right, RC.bottom, DC, 0, 0, SRCCOPY);

	// Elimino el buffer de la memória
	SelectObject(DC, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(DC);
}

LRESULT CALLBACK VentanaPrecarga::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT :
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(hWnd(), &PS);
			Pintar(DC);
			EndPaint(hWnd(), &PS);
			return 0;
		// Sombra de la ventana
		// https://stackoverflow.com/questions/43818022/borderless-window-with-drop-shadow
		case WM_NCCALCSIZE:
			if (wParam == TRUE) {
				// DWL_MSGRESULT (no esta definit)
				SetWindowLongPtr(_hWnd, 0, 0);
				return TRUE;
			}
			return 0;
		// Evita que al obtener el foco cambie la parte del caption
		case WM_NCACTIVATE:																					
			return 0;
	}
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}