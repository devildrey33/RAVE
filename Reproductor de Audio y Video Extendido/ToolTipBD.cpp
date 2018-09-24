#include "stdafx.h"
#include "ToolTipBD.h"
#include <dwmapi.h>
#include "DStringUtils.h"
#include "resource.h"


#define ANCHOCOL1	70
//#define PADDING      4
#define MARGEN      10

ToolTipBD::ToolTipBD(void) {
}


ToolTipBD::~ToolTipBD(void) {
}

void ToolTipBD::Mostrar(BDMedio &nMedio, const int PosX, const int PosY) {
	_Titulo = nMedio.Nombre();
	POINT Tam = _CalcularMedio(nMedio);
	_Mostrar(Tam, PosX, PosY);
}

void ToolTipBD::Mostrar(EtiquetaBD &nEtiqueta, const int PosX, const int PosY) {
	_Titulo = nEtiqueta.Texto;
	POINT Tam = _CalcularEtiqueta(nEtiqueta);
	_Mostrar(Tam, PosX, PosY);
}


void ToolTipBD::_Mostrar(POINT &Tam, const int PosX, const int PosY) {
	_Tam = Tam;
	POINT Punto = { PosX, PosY };
	if (PosX == -1 || PosY == -1) {
		GetCursorPos(&Punto);
		Punto.x += 10;
		Punto.y += 20;
	}

	if (_hWnd == NULL) {
		_hWnd = DVentana::CrearVentana(&App.VentanaRave, L"RAVE_ToolTipBD", L"", Punto.x, Punto.y, Tam.x, Tam.y, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		MARGINS Margen = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(_hWnd, &Margen);
	}

	// Asigno la posición del menú y lo hago siempre visible
	SetWindowPos(_hWnd, HWND_TOP, Punto.x, Punto.y, Tam.x, Tam.y, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	//	AnimateWindow(_hWnd, 200, AW_BLEND);
//	SetFocus(App.VentanaRave.hWnd());

	// Envio el mensaje WM_NCACTIVATE a la ventana principal para que no se vea como pierde el foco, y parezca que el desplegable es un hijo de la ventana principal
//	SendMessage(App.VentanaRave.hWnd(), WM_NCACTIVATE, TRUE, 0);
}

void ToolTipBD::Mover(const int PosX, const int PosY) {
	POINT Punto = { PosX, PosY };
	if (PosX == -1 || PosY == -1) {
		GetCursorPos(&Punto);
		Punto.x += 10;
		Punto.y += 20;
	}

	SetWindowPos(_hWnd, HWND_TOP, Punto.x, Punto.y, _Tam.x, _Tam.y, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}

const POINT ToolTipBD::_CalcularEtiqueta(EtiquetaBD &nEtiqueta) {

	_Icono = DListaIconos::AgregarIconoRecursos(IDI_DIRECTORIO, 16, 16);

	HDC   hDC   = GetDC(NULL);
	HFONT VFont = static_cast<HFONT>(SelectObject(hDC, Fuente21Negrita.Fuente()));
	POINT Ret   = { 0, (MARGEN * 2) + 25 };

	_Col1.resize(0);
	_Col2.resize(0);

	// Construyo las columnas con sus datos
	const size_t	NumFilas = 3;
	const wchar_t  *Col1[NumFilas] = { L"Medios", L"Nota", L"Tiempo" };
	std::wstring    Col2[NumFilas];

	Col2[0] = std::to_wstring(nEtiqueta.Medios);
	Col2[1] = DWL::Strings::ToStrF(static_cast<float>(nEtiqueta.Nota / static_cast<float>(nEtiqueta.Medios)), 2);
	RaveVLC::TiempoStr(nEtiqueta.Tiempo, Col2[2]);

	// Miro el ancho del titulo
	SIZE Tam = { 0, 0 };
	GetTextExtentPoint32(hDC, _Titulo.c_str(), static_cast<int>(_Titulo.size()), &Tam);
	int TamIcono = (_Icono == NULL) ? 0 : 20;
	Ret.x = TamIcono + (MARGEN * 2) + Tam.cx;

	int Ancho1 = 0, Ancho2 = 0;
	// Miro el ancho de cada columna y guardo las columnas
	for (size_t i = 0; i < NumFilas; i++) {
		SelectObject(hDC, Fuente18Negrita.Fuente());
		GetTextExtentPoint32(hDC, Col1[i], static_cast<int>(wcslen(Col1[i])), &Tam);
		if (Tam.cx > Ancho1) Ancho1 = Tam.cx;
		_Col1.push_back(ToolTipBD_Celda(Col1[i], Tam.cx));

		SelectObject(hDC, Fuente18Normal.Fuente());
		GetTextExtentPoint32(hDC, Col2[i].c_str(), static_cast<int>(Col2[i].size()), &Tam);
		if (Tam.cx > Ancho2) Ancho2 = Tam.cx;
		_Col2.push_back(ToolTipBD_Celda(Col2[i], Tam.cx));
		Ret.y += 20;
	}
	// Miro si el ancho del titulo es mas grande que el ancho de las dos columnas sumadas
	int AnchoTmp = (MARGEN * 2) + Ancho1 + 20 + Ancho2;	// el 20 es el espacio para los ':'
	_AnchoCol1 = MARGEN + Ancho1 + 20;

	if (Ret.x < AnchoTmp) Ret.x = AnchoTmp;

	// Des-selecciono la fuente y libero el DC
	SelectObject(hDC, VFont);
	ReleaseDC(NULL, hDC);

	return Ret;
}


const POINT ToolTipBD::_CalcularMedio(BDMedio &nMedio) {
	if (nMedio.TipoMedio == Tipo_Medio_Video)	{	_Icono = DListaIconos::AgregarIconoRecursos(IDI_VIDEO3, 16, 16);	}
	else										{	_Icono = DListaIconos::AgregarIconoRecursos(IDI_CANCION2, 16, 16);	}
	HDC   hDC = GetDC(NULL);
	HFONT VFont = static_cast<HFONT>(SelectObject(hDC, Fuente21Negrita.Fuente()));
	POINT Ret = { 0, (MARGEN * 2) + 25 };

	_Col1.resize(0);
	_Col2.resize(0);

	// Construyo las columnas con sus datos
	const size_t	NumFilas = 7;
	const wchar_t  *Col1[NumFilas] = { L"Pista", L"Genero", L"Grupo", L"Disco", L"Tiempo", L"Tamaño", L"Nota" };
	std::wstring    Col2[NumFilas];

	nMedio.PistaStr(Col2[0]);	//	Col2[0] = std::to_wstring(nMedio.Pista());
	Col2[1] = nMedio.Genero;
	Col2[2] = nMedio.Grupo();
	Col2[3] = nMedio.Disco();
	RaveVLC::TiempoStr(nMedio.Tiempo, Col2[4]);
	Strings::FormatoBytes(nMedio.Longitud, Col2[5]);
	Col2[6] = DWL::Strings::ToStrF(nMedio.Nota, 2);


	// Miro el ancho del titulo
	SIZE Tam = { 0, 0 };
	GetTextExtentPoint32(hDC, _Titulo.c_str(), static_cast<int>(_Titulo.size()), &Tam);
	int TamIcono = (_Icono == NULL) ? 0 : 20;
	Ret.x = TamIcono + (MARGEN * 2) + Tam.cx;

	int Ancho1 = 0, Ancho2 = 0;
	// Miro el ancho de cada columna y guardo las columnas
	for (size_t i = 0; i < NumFilas; i++) {
		// Si la segunda columna tiene resultado
		// Por ejemplo un video no tiene ni grupo ni disco, y estos tendran un tamaño de 0, por lo que no hace falta añadirlos a la lista final
		if (Col2[i].size() > 0) { 
			SelectObject(hDC, Fuente18Negrita.Fuente());
			GetTextExtentPoint32(hDC, Col1[i], static_cast<int>(wcslen(Col1[i])), &Tam);
			if (Tam.cx > Ancho1) Ancho1 = Tam.cx;
			_Col1.push_back(ToolTipBD_Celda(Col1[i], Tam.cx));

			SelectObject(hDC, Fuente18Normal.Fuente());
			GetTextExtentPoint32(hDC, Col2[i].c_str(), static_cast<int>(Col2[i].size()), &Tam);
			if (Tam.cx > Ancho2) Ancho2 = Tam.cx;
			_Col2.push_back(ToolTipBD_Celda(Col2[i], Tam.cx));
			Ret.y += 20;
		}
	}
	// Miro si el ancho del titulo es mas grande que el ancho de las dos columnas sumadas
	int AnchoTmp = (MARGEN * 2) + Ancho1 + 20 + Ancho2;	// el 20 es el espacio para los ':'
	_AnchoCol1 = MARGEN + Ancho1 + 20;

	if (Ret.x < AnchoTmp) Ret.x = AnchoTmp;

	// Des-selecciono la fuente y libero el DC
	SelectObject(hDC, VFont);
	ReleaseDC(NULL, hDC);

	return Ret;
}



void ToolTipBD::Pintar(HDC hDC) {
	// Obtengo el tamaño de la ventana
	RECT RC;
	GetClientRect(_hWnd, &RC);

	// Creo un DC compatible para utilizar-lo de back buffer
	HDC		DC			= CreateCompatibleDC(NULL);
	HBITMAP Bmp			= CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
	HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(DC, Bmp));
	HFONT	VFont		= static_cast<HFONT>(SelectObject(DC, Fuente21Negrita.Fuente()));

	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	// Pinto el borde
	HBRUSH BrochaBorde = CreateSolidBrush(COLOR_MENU_BORDE);
	FrameRect(DC, &RC, BrochaBorde);
	DeleteObject(BrochaBorde);

	SetBkMode(DC, TRANSPARENT);

	int TamIcono = 0;
	if (_Icono != NULL) {
		TamIcono = 20;
		DrawIconEx(DC, 10, 12, _Icono->Icono(), _Icono->Ancho(), _Icono->Alto(), 0, 0, DI_NORMAL);
	}

	// Pinto el titulo
	PintarTexto(DC, _Titulo, 10 + TamIcono, 10);

	// Pinto la columna con los nombres de los datos, y tambien pinto los 2 puntos
	SelectObject(DC, Fuente18Negrita.Fuente());
	for (int i = 0; i < static_cast<int>(_Col1.size()); i++) {
		PintarTexto(DC, _Col1[i].Texto, 10, 35 + (i * 20));
		PintarTexto(DC, L":", _AnchoCol1 - 10, 35 + (i * 20));
	}
	
	// Pinto los datos
	SelectObject(DC, Fuente18Normal.Fuente());
	for (int i = 0; i < static_cast<int>(_Col2.size()); i++) {
		PintarTexto(DC, _Col2[i].Texto, RC.right - MARGEN - _Col2[i].Ancho, 35 + (i * 20));
	}

	// Pinto el BackBuffer en el DC de la ventana
	BitBlt(hDC, 0, 0, RC.right, RC.bottom, DC, 0, 0, SRCCOPY);

	// Elimino objetos gdi de la memoria
	SelectObject(DC, VFont);
	SelectObject(DC, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(DC);
}


void ToolTipBD::_Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(hWnd(), &PS);
	Pintar(DC);
	EndPaint(hWnd(), &PS);
}

void ToolTipBD::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {
	Destruir();
}

void ToolTipBD::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {
	Destruir();
}

void ToolTipBD::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
	Destruir();
}


LRESULT CALLBACK ToolTipBD::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}