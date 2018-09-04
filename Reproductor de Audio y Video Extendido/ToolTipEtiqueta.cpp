#include "stdafx.h"
#include "ToolTipEtiqueta.h"
#include <dwmapi.h>
#include "DStringUtils.h"

#define ANCHOCOL1	70
#define PADDING      4
#define MARGEN      10

ToolTipEtiqueta::ToolTipEtiqueta(void) {
}


ToolTipEtiqueta::~ToolTipEtiqueta(void) {
}


void ToolTipEtiqueta::Mostrar(EtiquetaBD &nEtiqueta, const int PosX, const int PosY) {
	_Etiqueta = nEtiqueta;
	POINT Punto = { PosX, PosY };
	if (PosX == -1 || PosY == -1) {
		GetCursorPos(&Punto);
		Punto.x += 10;
		Punto.y += 20;
	}

	int Ancho = _CalcularAncho();
	if (_hWnd == NULL) {
		_hWnd = DVentana::CrearVentana(&App.VentanaRave, L"RAVE_ToolTipEtiqueta", L"", Punto.x, Punto.y, Ancho, 105, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		MARGINS Margen = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(_hWnd, &Margen);
	}
	else {
		PostMessage(_hWnd, WM_PAINT, 0, 0);
	}

	// Asigno la posición del menú y lo hago siempre visible
	SetWindowPos(_hWnd, HWND_TOPMOST, Punto.x, Punto.y, Ancho, 105, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	SetFocus(App.VentanaRave.hWnd());

	// Envio el mensaje WM_NCACTIVATE a la ventana principal para que no se vea como pierde el foco, y parezca que el desplegable es un hijo de la ventana principal
	SendMessage(App.VentanaRave.hWnd(), WM_NCACTIVATE, TRUE, 0);


//	AnimateWindow(_hWnd, 200, AW_BLEND);
}

const int ToolTipEtiqueta::_CalcularAncho(void) {
	HDC   hDC	= GetDC(NULL);
	HFONT VFont = static_cast<HFONT>(SelectObject(hDC, _Fuente21Negrita.Fuente()));

	std::wstring TmpTxt[3];
	TmpTxt[0] = std::to_wstring(_Etiqueta.Medios);
	TmpTxt[1] = DWL::DString_ToStrF(static_cast<float>(_Etiqueta.Nota / static_cast<float>(_Etiqueta.Medios)));
	RaveVLC::TiempoStr(_Etiqueta.Tiempo, TmpTxt[2]);
	// Mito el titulo
	SIZE Tam = { 0, 0 };
	GetTextExtentPoint32(hDC, _Etiqueta.Texto.c_str(), static_cast<int>(_Etiqueta.Texto.size()), &Tam);
	int nAncho = 20 + Tam.cx;
	// Mido los datos
	SelectObject(hDC, _Fuente18Normal.Fuente());
	for (size_t i = 0; i < 3; i++) {
		GetTextExtentPoint32(hDC, TmpTxt[i].c_str(), static_cast<int>(TmpTxt[i].size()), &Tam);
		if (ANCHOCOL1 + (MARGEN * 2) + Tam.cx > nAncho) nAncho = ANCHOCOL1 + (MARGEN * 2) + Tam.cx;
		_Tam[i] = Tam.cx;
	}

	// Des-selecciono la fuente y libero el DC
	SelectObject(hDC, VFont);
	ReleaseDC(NULL, hDC);

	return nAncho;
}


void ToolTipEtiqueta::Pintar(HDC hDC) {
	// Obtengo el tamaño de la ventana
	RECT RC;
	GetClientRect(_hWnd, &RC);

	// Creo un DC compatible para utilizar-lo de back buffer
	HDC		DC			= CreateCompatibleDC(NULL);
	HBITMAP Bmp			= CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
	HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(DC, Bmp));
	HFONT	VFont		= static_cast<HFONT>(SelectObject(DC, _Fuente21Negrita.Fuente()));

	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	// Pinto el borde
	HBRUSH BrochaBorde = CreateSolidBrush(COLOR_MENU_BORDE);
	FrameRect(DC, &RC, BrochaBorde);
	DeleteObject(BrochaBorde);

	SetBkMode(DC, TRANSPARENT);
	// Pinto el titulo
	PintarTexto(DC, _Etiqueta.Texto, 10, 10);

	// Pinto la columna con los nombres de los datos
	SelectObject(DC, _Fuente18Negrita.Fuente());
	PintarTexto(DC, L"Medios", 10, 35);
	PintarTexto(DC, L"Nota", 10, 55);
	PintarTexto(DC, L"Tiempo", 10, 75);

	// Pinto los : que separan los nombres de los datos
	PintarTexto(DC, L":", ANCHOCOL1, 35);
	PintarTexto(DC, L":", ANCHOCOL1, 55);
	PintarTexto(DC, L":", ANCHOCOL1, 75);
	
	// Pinto los datos
	SelectObject(DC, _Fuente18Normal.Fuente());
	std::wstring Medios = std::to_wstring(_Etiqueta.Medios);
	std::wstring Nota = DWL::DString_ToStrF(static_cast<float>(_Etiqueta.Nota / static_cast<float>(_Etiqueta.Medios)), 1);
	std::wstring Tiempo;
	App.VLC.TiempoStr(_Etiqueta.Tiempo, Tiempo);
	PintarTexto(DC, Medios,	RC.right - MARGEN - _Tam[0], 35);
	PintarTexto(DC, Nota,	RC.right - MARGEN - _Tam[1], 55);
	PintarTexto(DC, Tiempo,	RC.right - MARGEN - _Tam[2], 75);

	// Pinto el BackBuffer en el DC de la ventana
	BitBlt(hDC, 0, 0, RC.right, RC.bottom, DC, 0, 0, SRCCOPY);

	// Elimino objetos gdi de la memoria
	SelectObject(DC, VFont);
	SelectObject(DC, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(DC);
}


void ToolTipEtiqueta::_Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(hWnd(), &PS);
	Pintar(DC);
	EndPaint(hWnd(), &PS);
}

void ToolTipEtiqueta::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {
	Destruir();
}

void ToolTipEtiqueta::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {
	Destruir();
}

void ToolTipEtiqueta::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
	Destruir();
}


LRESULT CALLBACK ToolTipEtiqueta::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT		:	_Evento_Pintar();																															return 0;
		// Print y Print Client (para AnimateWindow)
		case WM_PRINTCLIENT	:    Pintar(reinterpret_cast<HDC>(wParam));																										return 0;
		case WM_MOUSEMOVE:		_Evento_MouseMovimiento(wParam, lParam);																									return 0;
		// Mouse presionado
		case WM_LBUTTONDOWN:	_Evento_MousePresionado(0, wParam, lParam);																									return 0;
		case WM_RBUTTONDOWN:	_Evento_MousePresionado(1, wParam, lParam);																									return 0;
		case WM_MBUTTONDOWN:	_Evento_MousePresionado(2, wParam, lParam);																									return 0;
		// Mouse soltado
		case WM_LBUTTONUP:		_Evento_MouseSoltado(0, wParam, lParam);																									return 0;
		case WM_RBUTTONUP:		_Evento_MouseSoltado(1, wParam, lParam);																									return 0;
		case WM_MBUTTONUP:		_Evento_MouseSoltado(2, wParam, lParam);																									return 0;
			// Sombra de la ventana
			// https://stackoverflow.com/questions/43818022/borderless-window-with-drop-shadow
		case WM_NCCALCSIZE	:
			if (wParam == TRUE) {
				// DWL_MSGRESULT (no esta definit)
				SetWindowLongPtr(_hWnd, 0, 0);
				return TRUE;
			}
			return 0;
			// Evita que al obtener el foco cambie la parte del caption
		case WM_NCACTIVATE	:																																				return FALSE;
	}
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}