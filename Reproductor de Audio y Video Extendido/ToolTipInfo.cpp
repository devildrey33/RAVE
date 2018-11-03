#include "stdafx.h"
#include "ToolTipInfo.h"
#include <dwmapi.h>
#include "DStringUtils.h"
#include "resource.h"

#define TOOLTIPINFO_PADDING			   8
#define ID_TEMPORIZADOR_OCULTAR		1000
#define ID_TEMPORIZADOR_ANIMACION	1001
#define ID_ANIMACION_TTI            1002
#define DESPLAZAMIENTOY              10.0f

//#define TIEMPO_ANIMACION             400

/*
  _______          _ _______ _      _____        __       
 |__   __|        | |__   __(_)    |_   _|      / _|      
	| | ___   ___ | |  | |   _ _ __  | |  _ __ | |_ ___   
	| |/ _ \ / _ \| |  | |  | | '_ \ | | | '_ \|  _/ _ \  
	| | (_) | (_) | |  | |  | | |_) || |_| | | | || (_) | 
	|_|\___/ \___/|_|  |_|  |_| .__/_____|_| |_|_| \___/  
							  | |                         
							  |_|						*/


void ToolTipInfo::Mostrar(const int cX, const int cY, const int cAncho, const int cAlto, ToolTipsInfo *nPadre, std::function<void(void)> CallbackOcultarTerminado) {
	_Padre = nPadre;
	_CallbackOcultarTerminado = CallbackOcultarTerminado;
	Y = cY;

	DhWnd *DWndPadre = NULL;
	if (_Padre != NULL) DWndPadre = _Padre->DPadre();
	else                DWndPadre = &App.VentanaRave; // Esto solo pasara si es un tooltip_medio o tooltip_Etiqueta utilizado desde el arbol / lista

	if (_hWnd == NULL) {
		_Ocultando = FALSE;
		_hWnd = DVentana::CrearVentana(DWndPadre, L"RAVE_ToolTipInfo", L"", cX, cY, cAncho, cAlto, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		MARGINS Margen = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(_hWnd, &Margen);
	}
	Opacidad(0);
	// Asigno la posición del tooltip detras de la ventana con el foco
	SetWindowPos(_hWnd, DWndPadre->hWnd(), cX, cY, cAncho, cAlto, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	// Situo el padre justo detras de este tooltip
	SetWindowPos(DWndPadre->hWnd(), _hWnd, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	// Inicio el temporizador para ocultar el tooltip (si está bajo el gestor de tooltips ToolTipsInfo)
	if (_Padre != NULL) {
		SetTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR, App.BD.Opciones_TiempoToolTips(), NULL);
	}
	// Inicio la animación de la opacidad
	Ani.Iniciar(0.0f, 255.0f, DESPLAZAMIENTOY, 0.0f, App.BD.Opciones_TiempoAnimaciones(), [=](std::vector<float> &Valores, const BOOL Terminado) {
		Opacidad(static_cast<BYTE>(Valores[0]));
		RECT RV;
		GetWindowRect(_hWnd, &RV);
		Mover(RV.left, Y + static_cast<int>(Valores[1]));
	});	
}


void ToolTipInfo::Mover(const int PosX, const int PosY) {
	POINT Punto = { PosX, PosY };
	if (PosX == -1 || PosY == -1) {
		GetCursorPos(&Punto);
		Punto.x += 10;
		Punto.y += 20;
	}
	SetWindowPos(_hWnd, HWND_TOP, Punto.x, Punto.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}


void ToolTipInfo::Ocultar(const BOOL Rapido) {
	if (_Ocultando == TRUE && Rapido == FALSE) return;
	_Ocultando = TRUE;

	KillTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR);
	//	KillTimer(_hWnd, ID_TEMPORIZADOR_ANIMACION);
	if (Rapido == FALSE) {
		if (_hWnd != NULL) { 
			Ani.Terminar();
			Ani.Iniciar(255.0f, 0.0f, 0.0f, DESPLAZAMIENTOY, App.BD.Opciones_TiempoAnimaciones(), [=](std::vector<float> &Valores, const BOOL Terminado) {
				Opacidad(static_cast<BYTE>(Valores[0]));
				RECT RV;
				GetWindowRect(_hWnd, &RV);
				Mover(RV.left, Y + static_cast<int>(Valores[1]));

				if (Terminado == TRUE) {
					ShowWindow(_hWnd, SW_HIDE);
					PostMessage(_hWnd, WM_CLOSE, 0, 0);
					_CallbackOcultarTerminado();
				}
			});
		}
	}
	else {
		Ani.Terminar();
		ShowWindow(_hWnd, SW_HIDE);
		Destruir();
	}
}


void ToolTipInfo::_Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(hWnd(), &PS);
	Pintar(DC);
	EndPaint(hWnd(), &PS);
}

void ToolTipInfo::_Evento_Temporizador(INT_PTR tID) {
	switch (tID) {
		case ID_TEMPORIZADOR_OCULTAR:
			Ocultar();
			break;
	}
}

LRESULT CALLBACK ToolTipInfo::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_TIMER:
			_Evento_Temporizador(static_cast<INT_PTR>(wParam));
			return 0;
		case WM_PAINT:	
			_Evento_Pintar();	
			return 0;
		// Al activar via mouse
		case WM_MOUSEACTIVATE:
			return MA_NOACTIVATEANDEAT;
		// Mouse 
		case WM_LBUTTONDOWN:	case WM_RBUTTONDOWN:	case WM_MBUTTONDOWN:// Mouse presionado
		case WM_LBUTTONUP:		case WM_RBUTTONUP:		case WM_MBUTTONUP:	// Mouse soltado
//		case WM_MOUSEMOVE:
			Ocultar();
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
			return TRUE;
	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}



/*
  _______          _ _______ _      _____        __                 ___   _____      _
 |__   __|        | |__   __(_)    |_   _|      / _|               |__ \ / ____|    | |
	| | ___   ___ | |  | |   _ _ __  | |  _ __ | |_ ___               ) | |     ___ | |_   _ _ __ ___  _ __   __ _ ___
	| |/ _ \ / _ \| |  | |  | | '_ \ | | | '_ \|  _/ _ \             / /| |    / _ \| | | | | '_ ` _ \| '_ \ / _` / __|
	| | (_) | (_) | |  | |  | | |_) || |_| | | | || (_) |           / /_| |___| (_) | | |_| | | | | | | | | | (_| \__ \
	|_|\___/ \___/|_|  |_|  |_| .__/_____|_| |_|_| \___/   ______  |____|\_____\___/|_|\__,_|_| |_| |_|_| |_|\__,_|___/
							  | |                         |______|
							  |_|                         
*/


void ToolTipInfo_2Columnas::Pintar(HDC hDC) {
	// Obtengo el tamaño de la ventana
	RECT RC;
	GetClientRect(_hWnd, &RC);

	// Creo un DC compatible para utilizar-lo de back buffer
	HDC		DC = CreateCompatibleDC(NULL);
	HBITMAP Bmp = CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
	HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(DC, Bmp));
	HFONT	VFont = static_cast<HFONT>(SelectObject(DC, Fuente21Negrita.Fuente()));

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
		if (_Col1[i].Texto != L"Nota") 	PintarTexto(DC, _Col2[i].Texto, _AnchoCol1, 35 + (i * 20));		
		else							PintarNota(DC, _AnchoCol1, 35 + (i * 20));		
//		PintarTexto(DC, _Col2[i].Texto, RC.right - TOOLTIPINFO_PADDING - _Col2[i].Ancho, 35 + (i * 20));
	}

	// Pinto el BackBuffer en el DC de la ventana
	BitBlt(hDC, 0, 0, RC.right, RC.bottom, DC, 0, 0, SRCCOPY);

	// Elimino objetos gdi de la memoria
	SelectObject(DC, VFont);
	SelectObject(DC, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(DC);
}

void ToolTipInfo_2Columnas::_PintarNota(HDC hDC, const int cX, const int cY, const float nNota) {
	float Nota = nNota;
	int X = 0;
//	int Ancho = 16;
	DListaIconos_Icono *IcoNota = DWL::DListaIconos::AgregarIconoRecursos(IDI_NOTA, 16, 16);
	
	HDC		DC = CreateCompatibleDC(NULL);
	HBITMAP Bmp = CreateCompatibleBitmap(hDC, static_cast<int>(Nota * 16.0f), 16);
	HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(DC, Bmp));

	RECT RC = { 0, 0, static_cast<int>(Nota * 16.0f), 16 };
	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	while (Nota > 0.0f) {
		DrawIconEx(DC, X, 0, IcoNota->Icono(), 16 , 16, 0, 0, DI_NORMAL);
		X += 16;
		Nota -= 1.0f;
	}

	BitBlt(hDC, cX, cY, RC.right, RC.bottom, DC, 0, 0, SRCCOPY);

	SelectObject(DC, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(DC);

}



/*
  _______          _ _______ _      _____        __                 __  __          _ _
 |__   __|        | |__   __(_)    |_   _|      / _|               |  \/  |        | (_)
	| | ___   ___ | |  | |   _ _ __  | |  _ __ | |_ ___            | \  / | ___  __| |_  ___
	| |/ _ \ / _ \| |  | |  | | '_ \ | | | '_ \|  _/ _ \           | |\/| |/ _ \/ _` | |/ _ \
	| | (_) | (_) | |  | |  | | |_) || |_| | | | || (_) |          | |  | |  __/ (_| | | (_) |
	|_|\___/ \___/|_|  |_|  |_| .__/_____|_| |_|_| \___/   ______  |_|  |_|\___|\__,_|_|\___/
							  | |                         |______|
							  |_|																*/

void ToolTipInfo_Medio::MostrarFijo(BDMedio &nMedio) {
	Medio = nMedio;
	SIZE Tam = CalcularTam();
	POINT Punto;
	GetCursorPos(&Punto);
	Punto.x += 10;
	Punto.y += 20;
	Mostrar(Punto.x, Punto.y, Tam.cx, Tam.cy, NULL, [=] {});
}

SIZE ToolTipInfo_Medio::CalcularTam(void) {	
	if (Medio.TipoMedio == Tipo_Medio_Video)	{	_Icono = DListaIconos::AgregarIconoRecursos(IDI_VIDEO3, 16, 16);	}
	else										{	_Icono = DListaIconos::AgregarIconoRecursos(IDI_CANCION2, 16, 16);	}

	HDC   hDC = GetDC(NULL);
	HFONT VFont = static_cast<HFONT>(SelectObject(hDC, Fuente21Negrita.Fuente()));
	SIZE  Ret = { 0, (TOOLTIPINFO_PADDING * 2) + 25 };

	_Col1.resize(0);
	_Col2.resize(0);

	// Construyo las columnas con sus datos
	const size_t	NumFilas = 8;
	const wchar_t  *Col1[NumFilas] = { L"Pista", L"Genero", L"Grupo", L"Disco", L"Tiempo", L"Tamaño", L"Nota", L"Acabado" };
	std::wstring    Col2[NumFilas];

	_Titulo = Medio.Nombre();

	Medio.PistaStr(Col2[0]);	//	Col2[0] = std::to_wstring(nMedio.Pista());
	Col2[1] = Medio.Genero;
	Col2[2] = Medio.Grupo();
	Col2[3] = Medio.Disco();
	RaveVLC::TiempoStr(Medio.Tiempo, Col2[4]);
	Strings::FormatoBytes(Medio.Longitud, Col2[5]);
	Col2[6] = DWL::Strings::ToStrF(Medio.Nota, 2);
	Col2[7] = std::to_wstring(Medio.Reproducido);
	if (Medio.Reproducido == 1) Col2[7] += +L" vez";
	else                        Col2[7] += +L" veces";

	// Miro el ancho del titulo
	SIZE Tam = { 0, 0 };
	GetTextExtentPoint32(hDC, Medio.Nombre().c_str(), static_cast<int>(Medio.Nombre().size()), &Tam);
	int TamIcono = (_Icono == NULL) ? 0 : 20;
	Ret.cx = TamIcono + (TOOLTIPINFO_PADDING * 2) + Tam.cx;

	int Ancho1 = 0, Ancho2 = 0;
	// Miro el ancho de cada columna y guardo las columnas
	for (size_t i = 0; i < NumFilas; i++) {
		// Si la segunda columna tiene resultado
		// Por ejemplo un video no tiene ni grupo ni disco, y estos tendran un tamaño de 0, por lo que no hace falta añadirlos a la lista final
		if (Col2[i].size() > 0) {
			SelectObject(hDC, Fuente18Negrita.Fuente());
			GetTextExtentPoint32(hDC, Col1[i], static_cast<int>(wcslen(Col1[i])), &Tam);
			if (Tam.cx > Ancho1) Ancho1 = Tam.cx;
			_Col1.push_back(ToolTipInfo_Celda(Col1[i], Tam.cx));

			SelectObject(hDC, Fuente18Normal.Fuente());
			GetTextExtentPoint32(hDC, Col2[i].c_str(), static_cast<int>(Col2[i].size()), &Tam);
			if (Tam.cx > Ancho2) Ancho2 = Tam.cx;
			_Col2.push_back(ToolTipInfo_Celda(Col2[i], Tam.cx));
			Ret.cy += 20;
		}
	}
	// Miro si el ancho del titulo es mas grande que el ancho de las dos columnas sumadas
	int AnchoTmp = (TOOLTIPINFO_PADDING * 2) + Ancho1 + 20 + Ancho2;	// el 20 es el espacio para los ':'
	_AnchoCol1 = TOOLTIPINFO_PADDING + Ancho1 + 20;

	if (Ret.cx < AnchoTmp) Ret.cx = AnchoTmp;

	// Des-selecciono la fuente y libero el DC
	SelectObject(hDC, VFont);
	ReleaseDC(NULL, hDC);

	return Ret;
}

/*
  _______          _ _______ _      _____        __                 ______ _   _                  _
 |__   __|        | |__   __(_)    |_   _|      / _|               |  ____| | (_)                | |
	| | ___   ___ | |  | |   _ _ __  | |  _ __ | |_ ___            | |__  | |_ _  __ _ _   _  ___| |_ __ _
	| |/ _ \ / _ \| |  | |  | | '_ \ | | | '_ \|  _/ _ \           |  __| | __| |/ _` | | | |/ _ \ __/ _` |
	| | (_) | (_) | |  | |  | | |_) || |_| | | | || (_) |          | |____| |_| | (_| | |_| |  __/ || (_| |
	|_|\___/ \___/|_|  |_|  |_| .__/_____|_| |_|_| \___/   ______  |______|\__|_|\__, |\__,_|\___|\__\__,_|
							  | |                         |______|                  | |
							  |_|                                                   |_|
*/

void ToolTipInfo_Etiqueta::MostrarFijo(EtiquetaBD *nEtiqueta) {
	Etiqueta = nEtiqueta;
	SIZE Tam = CalcularTam();
	POINT Punto;
	GetCursorPos(&Punto);
	Punto.x += 10;
	Punto.y += 20;
	Mostrar(Punto.x, Punto.y, Tam.cx, Tam.cy, NULL, [=] {});
}


SIZE ToolTipInfo_Etiqueta::CalcularTam(void) {
	_Icono = DListaIconos::AgregarIconoRecursos(IDI_DIRECTORIO, 16, 16);
	_Titulo = Etiqueta->Texto;

	HDC   hDC = GetDC(NULL);
	HFONT VFont = static_cast<HFONT>(SelectObject(hDC, Fuente21Negrita.Fuente()));
	SIZE  Ret = { 0, (TOOLTIPINFO_PADDING * 2) + 25 };

	_Col1.resize(0);
	_Col2.resize(0);

	// Construyo las columnas con sus datos
	const size_t	NumFilas = 3;
	const wchar_t  *Col1[NumFilas] = { L"Medios", /*L"Nota",*/ L"Tiempo", L"Longitud" };
	std::wstring    Col2[NumFilas];

	Col2[0] = std::to_wstring(Etiqueta->Medios);
	//	Col2[1] = DWL::Strings::ToStrF(static_cast<float>(nEtiqueta.Nota / static_cast<float>(nEtiqueta.Medios)), 2);
	RaveVLC::TiempoStr(Etiqueta->Tiempo, Col2[1]);
	DWL::Strings::FormatoBytes(Etiqueta->Longitud, Col2[2]);


	// Miro el ancho del titulo
	SIZE Tam = { 0, 0 };
	GetTextExtentPoint32(hDC, _Titulo.c_str(), static_cast<int>(_Titulo.size()), &Tam);
	int TamIcono = (_Icono == NULL) ? 0 : 20;
	Ret.cx = TamIcono + (TOOLTIPINFO_PADDING * 2) + Tam.cx;

	int Ancho1 = 0, Ancho2 = 0;
	// Miro el ancho de cada columna y guardo las columnas
	for (size_t i = 0; i < NumFilas; i++) {
		SelectObject(hDC, Fuente18Negrita.Fuente());
		GetTextExtentPoint32(hDC, Col1[i], static_cast<int>(wcslen(Col1[i])), &Tam);
		if (Tam.cx > Ancho1) Ancho1 = Tam.cx;
		_Col1.push_back(ToolTipInfo_Celda(Col1[i], Tam.cx));

		SelectObject(hDC, Fuente18Normal.Fuente());
		GetTextExtentPoint32(hDC, Col2[i].c_str(), static_cast<int>(Col2[i].size()), &Tam);
		if (Tam.cx > Ancho2) Ancho2 = Tam.cx;
		_Col2.push_back(ToolTipInfo_Celda(Col2[i], Tam.cx));
		Ret.cy += 20;
	}
	// Miro si el ancho del titulo es mas grande que el ancho de las dos columnas sumadas
	int AnchoTmp = (TOOLTIPINFO_PADDING * 2) + Ancho1 + 20 + Ancho2;	// el 20 es el espacio para los ':'
	_AnchoCol1 = TOOLTIPINFO_PADDING + Ancho1 + 20;

	if (Ret.cx < AnchoTmp) Ret.cx = AnchoTmp;

	// Des-selecciono la fuente y libero el DC
	SelectObject(hDC, VFont);
	ReleaseDC(NULL, hDC);

	return Ret;
}

/*
  _______          _ _______ _      _____        __               _______        _        
 |__   __|        | |__   __(_)    |_   _|      / _|             |__   __|      | |       
    | | ___   ___ | |  | |   _ _ __  | |  _ __ | |_ ___             | | _____  _| |_ ___  
    | |/ _ \ / _ \| |  | |  | | '_ \ | | | '_ \|  _/ _ \            | |/ _ \ \/ / __/ _ \ 
    | | (_) | (_) | |  | |  | | |_) || |_| | | | || (_) |           | |  __/>  <| || (_) |
    |_|\___/ \___/|_|  |_|  |_| .__/_____|_| |_|_| \___/  _______   |_|\___/_/\_\\__\___/ 
                              | |                        |_______|                    
                              |_|                                                          */


SIZE ToolTipInfo_Texto::CalcularTam(void) {
	// Calculo el tamaño del texto (una sola línea de texto)
	HDC	   		hDC = CreateCompatibleDC(NULL);
	HBITMAP		Bmp = CreateCompatibleBitmap(hDC, 1, 1);
	HBITMAP		Viejo = static_cast<HBITMAP>(SelectObject(hDC, Bmp));
	HFONT       vFuente = static_cast<HFONT>(SelectObject(hDC, Fuente18Normal()));
	SIZE		Ret = { 0, 0 };

	RECT Tam = { 0, 0, 0, 0 };
	int Al = DrawText(hDC, _Str.c_str(), static_cast<int>(_Str.size()), &Tam, DT_CALCRECT | DT_NOPREFIX);

	SelectObject(hDC, vFuente);
	SelectObject(hDC, Viejo);
	DeleteObject(Bmp);
	DeleteDC(hDC);

	Ret.cx = Tam.right + (TOOLTIPINFO_PADDING * 2);
	Ret.cy = Tam.bottom + (TOOLTIPINFO_PADDING * 2);
	return Ret;
}



void ToolTipInfo_Texto::Pintar(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	RECT RCF = { RC.left + 1, RC.top + 1, RC.right - 1, RC.bottom - 1 };
	RECT RCT = { RC.left, RC.top + TOOLTIPINFO_PADDING, RC.right, RC.bottom + (TOOLTIPINFO_PADDING * 2) };

	// Creo un buffer en memória para pintar el control
	HDC		Buffer = CreateCompatibleDC(NULL);
	// Creo un DC compatible para el buffer
	HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
	HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

	// Pinto el borde
	HBRUSH BrochaBorde = CreateSolidBrush(COLOR_TOOLTIP_BORDE);
	FrameRect(Buffer, &RC, BrochaBorde);
	DeleteObject(BrochaBorde);

	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_TOOLTIP_FONDO);
	FillRect(Buffer, &RCF, BrochaFondo);
	DeleteObject(BrochaFondo);

	// Pinto el texto
	SetBkMode(Buffer, TRANSPARENT);
	SetTextColor(Buffer, COLOR_TOOLTIP_TEXTO);
	HFONT vFuente = static_cast<HFONT>(SelectObject(Buffer, Fuente18Normal()));
	DrawText(Buffer, _Str.c_str(), static_cast<int>(_Str.size()), &RCT, DT_CENTER);
	SelectObject(Buffer, vFuente);

	// Copio el buffer al DC
	BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

	// Elimino el buffer de la memória
	SelectObject(Buffer, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(Buffer);
}






















/*
  _______          _ _______ _           _____        __      
 |__   __|        | |__   __(_)         |_   _|      / _|     
    | | ___   ___ | |  | |   _ _ __  ___  | |  _ __ | |_ ___  
    | |/ _ \ / _ \| |  | |  | | '_ \/ __| | | | '_ \|  _/ _ \ 
    | | (_) | (_) | |  | |  | | |_) \__ \_| |_| | | | || (_) |
    |_|\___/ \___/|_|  |_|  |_| .__/|___/_____|_| |_|_| \___/ 
                              | |                             
                              |_|                            */


void ToolTipsInfo::Iniciar(DhWnd *Padre) {
	_Padre = Padre;
}

void ToolTipsInfo::MostrarToolTip(const wchar_t *Texto) {
	MostrarToolTip(std::wstring(Texto));
}

void ToolTipsInfo::MostrarToolTip(std::wstring &Texto) {
	ToolTipInfo *TT = new ToolTipInfo_Texto(Texto);
	_MostrarToolTip(TT);
}

void ToolTipsInfo::MostrarToolTip(BDMedio &Medio) {
	ToolTipInfo *TT = new ToolTipInfo_Medio(Medio);
	_MostrarToolTip(TT);
}

void ToolTipsInfo::_MostrarToolTip(ToolTipInfo *TT) {
	if (_Padre->hWnd() == NULL) {
		delete TT;
		return;
	}
	if (IsWindowVisible(_Padre->hWnd()) == FALSE) {
		delete TT;
		return;
	}

	// Calculo el tamaño del tooltip
	SIZE Tam = TT->CalcularTam();

	// Elimino tooltips que ya se han ocultado
	for (long t = static_cast<long>(_ToolTips.size()) - 1; t > -1; t--) {
		if (_ToolTips[t]->hWnd() == NULL) {
			_ToolTips[t]->Ani.Terminar();
			delete _ToolTips[t];
			_ToolTips.erase(_ToolTips.begin() + t);
		}
	}

	RECT RV;
	GetWindowRect(_Padre->hWnd(), &RV);
	// Muevo los otros tooltips creados anteriormente hacia arriba
	RECT Tmp;
	for (size_t i = 0; i < _ToolTips.size(); i++) {
		GetWindowRect(_ToolTips[i]->hWnd(), &Tmp);
		_ToolTips[i]->Y = Tmp.top - (Tam.cy + TOOLTIPINFO_PADDING);
		// Si el tooltip sobresale por encima de la parte superior de la ventana padre, lo oculto
		if (RV.top > Tmp.top - (Tam.cy + TOOLTIPINFO_PADDING)) {
			_ToolTips[i]->Ocultar(TRUE);
		}
		else {
//			SetWindowPos(_ToolTips[i]->hWnd(), HWND_TOP, Tmp.left, Tmp.top - (Tam.cy + TOOLTIPINFO_PADDING), 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE);
		}
	}


	/*for (LONGLONG i = static_cast<LONGLONG>(_ToolTips.size()) - 1; i > -1; i--) {
		GetWindowRect(_ToolTips[i]->hWnd(), &Tmp);
		if (RV.top > Tmp.top - Tam.cy) {
			_ToolTips[i]->Ocultar(TRUE);
		}
		else {
			SetWindowPos(_ToolTips[i]->hWnd(), HWND_TOP, Tmp.left, Tmp.top - Tam.cy , 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE);
		}
	}*/

	// Muestro el nuevo tooltip alineado a la parte inferior izquierda
	if (RV.right - RV.left > Tam.cx) {
		TT->Mostrar(RV.right - (Tam.cx + 20), RV.bottom - (Tam.cy + 20), Tam.cx, Tam.cy, this, [=] { RecolocarToolTips();	});
	}
	// Muestro el nuevo tooltip centrado en la parte inferior
	else {
		int x = (Tam.cx - (RV.right - RV.left)) / 2;
		TT->Mostrar(RV.left - x, RV.bottom - (Tam.cy + 20), Tam.cx, Tam.cy, this, [=] { RecolocarToolTips(); });
	}

	_ToolTips.push_back(TT);

	RecolocarToolTips();

}


void ToolTipsInfo::RecolocarToolTips(void) {
	RECT RT, RV;
	GetWindowRect(_Padre->hWnd(), &RV);
	long Y = RV.bottom - 12;
	for (long i = static_cast<long>(_ToolTips.size()) - 1; i > -1; i--) {
		if (_ToolTips[i]->hWnd() != NULL) {
			if (_ToolTips[i]->Visible() == TRUE) {
				GetWindowRect(_ToolTips[i]->hWnd(), &RT);
				Y -= (RT.bottom - RT.top) + TOOLTIPINFO_PADDING;
				_ToolTips[i]->Y = Y;
				SetWindowPos(_ToolTips[i]->hWnd(), NULL, RT.left, Y, 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE);
			}
		}
	}
}


void ToolTipsInfo::Ocultar(void) {
	for (size_t i = 0; i < _ToolTips.size(); i++) {
		_ToolTips[i]->Ocultar(TRUE);		
		delete _ToolTips[i];
	}
	_ToolTips.resize(0);
}

HWND ToolTipsInfo::Padre(void) {
	if (_Padre != NULL) return _Padre->hWnd();
	return NULL;
}