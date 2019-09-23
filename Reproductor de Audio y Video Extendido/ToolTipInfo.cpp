#include "stdafx.h"
#include "ToolTipInfo.h"
#include <dwmapi.h>
#include "DStringUtils.h"
#include "resource.h"
#include "DToolTipEx.h"

#define TOOLTIPINFO_PADDING			   8
#define ID_TEMPORIZADOR_OCULTAR		1000
#define ID_TEMPORIZADOR_ANIMACION	1001
#define ID_ANIMACION_TTI            1002
#define DESPLAZAMIENTOY              15.0f
#define MAX_OPACIDAD                220.0f

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
	X = cX;

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


	// Corrijo el z-order de las ventanas que puedan tener el foco en este momento
	HWND Foco = GetForegroundWindow();
	if (DWndPadre->hWnd() != Foco) {
		// Si la ventana del thread analizar tiene el foco
		if (Foco == App.VentanaRave.ThreadAnalizar.hWnd()) {
			// Si la ventana opciones es visible
			if (App.VentanaOpciones2.Visible() == TRUE) {
				// Muevo la ventana de las opciones arriba del todo
				SetWindowPos(App.VentanaOpciones2.hWnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			}
			// Muevo la ventana del thread arriba del todo
//			SetWindowPos(Foco, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
		}
		// Si la ventana opciones tiene el foco
//		if (Foco == App.VentanaOpciones.hWnd()) {
		// Muevo la ventana de las opciones arriba del todo
		SetWindowPos(Foco, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE); 
		// NOTA : De moment ho deixo general, si amb els warcos no dona problemes, es quedará aixi.
//		}
	}




	// Inicio el temporizador para ocultar el tooltip (si está bajo el gestor de tooltips ToolTipsInfo)
	if (_Padre != NULL) {
		SetTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR, App.Opciones.TiempoToolTips(), NULL);
	}

	static double Cero = 0.0f;
	static double MaxOpacidad = MAX_OPACIDAD;

	// Inicio la animación de la opacidad
	Ani.Iniciar({ 0.0f, DESPLAZAMIENTOY }, { &MaxOpacidad, &Cero }, App.Opciones.TiempoAnimaciones(), [=](DAnimacion::Valores& Datos, const BOOL Terminado) {
		double Op = Datos[0].Decimal();
		if (Op > 0.0f && Op < 255.1f) {
			Opacidad(static_cast<BYTE>(Op));
		}
		Mover(X, Y + static_cast<int>(Datos[1].Decimal()));
//		Debug_Escribir_Varg(L"Mostrar %d, %d\n", static_cast<int>(Datos[0].Decimal()), static_cast<int>(Datos[1].Decimal()))
	});	
	#if DEBUG_TOOLTIPINFO == TRUE
		Debug_Escribir(L"ToolTipInfo::Mostrar\n");
	#endif
}


void ToolTipInfo::Mover(const int PosX, const int PosY) {
	// Si no es visible, o se está ocultando SALGO
	if (_hWnd == NULL)						return;
	if (IsWindowVisible(_hWnd) == FALSE)	return;
//	if (_Ocultando == TRUE)					return;
	#if DEBUG_TOOLTIPINFO == TRUE
		Debug_Escribir_Varg(L"ToolTipInfo::Mover (X=%d, Y=%d)\n", PosX, PosY);
	#endif	
	// Determino la posición (manual, o debajo del mouse)
	POINT Punto = { PosX, PosY };
	if (PosX == -1 || PosY == -1) {
		GetCursorPos(&Punto);
		Punto.x += 10;
		Punto.y += 20;
	}
	SetWindowPos(_hWnd, NULL, Punto.x, Punto.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER);
}


void ToolTipInfo::Ocultar(const BOOL Rapido) {
	if (_Ocultando == TRUE && Rapido == FALSE) return;
	_Ocultando = TRUE;
	#if DEBUG_TOOLTIPINFO == TRUE
		Debug_Escribir_Varg(L"ToolTipInfo::Ocultar (Rapido=%d)\n", Rapido);
	#endif

	RECT RW;
	GetWindowRect(_hWnd, &RW);
	X = RW.left;
	Y = RW.top;

	static double Cero		 = 0.0f;
	static double Doscientos = 200.0f;

	KillTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR);
	//	KillTimer(_hWnd, ID_TEMPORIZADOR_ANIMACION);
	if (Rapido == FALSE) {
		if (_hWnd != NULL) { 
			Ani.Terminar();
			Ani.Iniciar({ MAX_OPACIDAD, 0.0f }, { &Cero, &Doscientos }, App.Opciones.TiempoAnimaciones(), [=](DAnimacion::Valores& Datos, const BOOL Terminado) {
				double Op = Datos[0].Decimal();				
				if (Op > 0.0f && Op < 255.1f) {
					Opacidad(static_cast<BYTE>(Op));
				}
//				Debug_Escribir_Varg(L"Ocultar %d, %d\n", static_cast<int>(Datos[0].Decimal()), static_cast<int>(Datos[1].Decimal()))
				Mover(X + static_cast<int>(Datos[1].Decimal()), Y);
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
		case WM_MOUSEMOVE :
			if (App.Opciones.OcultarTooltipsMouse() == TRUE) Ocultar();
			break;
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
	HDC		DC			= CreateCompatibleDC(NULL);
	HBITMAP Bmp			= CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
	HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(DC, Bmp));
	HFONT	VFont		= static_cast<HFONT>(SelectObject(DC, _FuenteTitulo()));

	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(Skin.Fondo);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	// Pinto el borde
	HBRUSH BrochaBorde = CreateSolidBrush(Skin.Borde);
	FrameRect(DC, &RC, BrochaBorde);
	DeleteObject(BrochaBorde);

	SetBkMode(DC, TRANSPARENT);

	int TamIcono = 0;
	if (_Icono() != NULL) {
		TamIcono = 20;
		DrawIconEx(DC, 10, 12, _Icono(), _Icono.Ancho(), _Icono.Alto(), 0, 0, DI_NORMAL);
	}

	// Pinto el titulo
	PintarTexto(DC, _Titulo, 10 + TamIcono, 10);

	// Pinto la columna con los nombres de los datos, y tambien pinto los 2 puntos
	SelectObject(DC, _FuenteNombreValor());
	for (int i = 0; i < static_cast<int>(_Col1.size()); i++) {
		PintarTexto(DC, _Col1[i].Texto, 10, 35 + (i * 20));
		PintarTexto(DC, L":", _AnchoCol1 - 10, 35 + (i * 20));
	}

	// Pinto los datos
	SelectObject(DC, _FuenteValor());
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


// Para recortar las estrellas en decimales, no me queda otra que crear un segundo backbuffer del tamaño que debería ocupar la nota (16.0f * Nota)
// DrawIconEx no permite pintar una porción del icono, si le pngo menos ancho o alto redimensiona el icono (COSA QUE NO ME INTERESA), por lo que me obliga a utilizar este segundo backbuffer.
// Realmente pinto estrellas completas aunque solo se requiera un 0.1f, y al tener el backbuffer ajustado a la nota, se recorta la ultima estrella decimal.
void ToolTipInfo_2Columnas::_PintarNota(HDC hDC, const int cX, const int cY, const float nNota) {
	float Nota = nNota;
	int X = 0;
//	int Ancho = 16;
//	DIcono  IcoNota;
	//IcoNota.CrearIconoRecursos(IDI_NOTA, 16, 16);
	
	HDC		DC = CreateCompatibleDC(NULL);
	HBITMAP Bmp = CreateCompatibleBitmap(hDC, static_cast<int>(Nota * 16.0f), 16);
	HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(DC, Bmp));

	RECT RC = { 0, 0, static_cast<int>(Nota * 16.0f), 16 };
	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(Skin.Fondo);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	while (Nota > 0.0f) {
		DrawIconEx(DC, X, 0, _IconoNota(), 16 , 16, 0, 0, DI_NORMAL);
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
	if (Medio.TipoMedio == Tipo_Medio_Video)	{	_Icono.CrearIconoRecursos(IDI_VIDEO, 16, 16);	}
	else										{	_Icono.CrearIconoRecursos(IDI_CANCION2, 16, 16);	}

	HDC   hDC = GetDC(NULL);
	HFONT VFont = static_cast<HFONT>(SelectObject(hDC, _FuenteTitulo()));
	SIZE  Ret = { 0, (TOOLTIPINFO_PADDING * 2) + 25 };

	_Col1.resize(0);
	_Col2.resize(0);

	// Construyo las columnas con sus datos
	const size_t	NumFilas = 7;
	const wchar_t  *Col1[NumFilas] = { L"Genero", L"Grupo", L"Disco", L"Tiempo", L"Tamaño", L"Nota", L"Acabado" };
	std::wstring    Col2[NumFilas];
	
	// Creo el titulo con la pista y el nombre
	Medio.PistaStr(_Titulo);
	_Titulo += L" " + Medio.Nombre();

//	Medio.PistaStr(Col2[0]);	//	Col2[0] = std::to_wstring(nMedio.Pista());
	Col2[0] = Medio.Genero;
	Col2[1] = Medio.Grupo();
	Col2[2] = Medio.Disco();
	Rave_MediaPlayer::TiempoStr(Medio.Tiempo, Col2[3]);
	Strings::FormatoBytes(Medio.Longitud, Col2[4]);
	Col2[5] = DWL::Strings::ToStrF(Medio.Nota, 2);
	Col2[6] = std::to_wstring(Medio.Reproducido);
	if (Medio.Reproducido == 1) Col2[6] += +L" vez";
	else                        Col2[6] += +L" veces";

	// Miro el ancho del titulo
	SIZE Tam = { 0, 0 };
	GetTextExtentPoint32(hDC, _Titulo.c_str(), static_cast<int>(_Titulo.size()), &Tam);
	int TamIcono = (_Icono() == NULL) ? 0 : 20;
	Ret.cx = TamIcono + (TOOLTIPINFO_PADDING * 2) + Tam.cx;

	int Ancho1 = 0;
	int Ancho2 = static_cast<int>(16.0f * Medio.Nota) + TOOLTIPINFO_PADDING; // El ancho mínimo es el tamaño que ocupan las estrellas de la nbota
	// Miro el ancho de cada columna y guardo las columnas
	for (size_t i = 0; i < NumFilas; i++) {
		// Si la segunda columna tiene resultado
		// Por ejemplo un video no tiene ni grupo ni disco, y estos tendran un tamaño de 0, por lo que no hace falta añadirlos a la lista final
		if (Col2[i].size() > 0) {
			SelectObject(hDC, _FuenteNombreValor());
			GetTextExtentPoint32(hDC, Col1[i], static_cast<int>(wcslen(Col1[i])), &Tam);
			if (Tam.cx > Ancho1) Ancho1 = Tam.cx;
			_Col1.push_back(ToolTipInfo_Celda(Col1[i], Tam.cx));

			SelectObject(hDC, _FuenteValor());
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

	/*   _______ ____  _____   ____
		|__   __/ __ \|  __ \ / __ \
		   | | | |  | | |  | | |  | |  O
		   | | | |  | | |  | | |  | |
		   | | | |__| | |__| | |__| |  O
		   |_|  \____/|_____/ \____/	 */
	// Hay que mirar si el titulo supera el 140& del ancho de las columnas, y en ese caso reajustar-lo para varias líneas
	// Subgerencia crear un RECT dentro del objeto para el titulo, y pintar-lo con DrawText DT_MULTILINE añadiendo los '\n' que sean necesarios
	// Por ultimo recalcular el ancho máximo de las lineas del titulo, y la altura máxima


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
	_Icono.CrearIconoRecursos(IDI_DIRECTORIO, 16, 16);
	_Titulo = Etiqueta->Texto;

	HDC   hDC = GetDC(NULL);
	HFONT VFont = static_cast<HFONT>(SelectObject(hDC, _FuenteTitulo()));
	SIZE  Ret = { 0, (TOOLTIPINFO_PADDING * 2) + 25 };

	_Col1.resize(0);
	_Col2.resize(0);

	// Construyo las columnas con sus datos
	const size_t	NumFilas = 4;
	const wchar_t  *Col1[NumFilas] = { L"Medios", L"Nota", L"Tiempo", L"Longitud" };
	std::wstring    Col2[NumFilas];

	Col2[0] = std::to_wstring(Etiqueta->Medios);
	Col2[1] = DWL::Strings::ToStrF(Etiqueta->Nota, 2);
	Rave_MediaPlayer::TiempoStr(Etiqueta->Tiempo, Col2[2]);
	DWL::Strings::FormatoBytes(Etiqueta->Longitud, Col2[3]);


	// Miro el ancho del titulo
	SIZE Tam = { 0, 0 };
	GetTextExtentPoint32(hDC, _Titulo.c_str(), static_cast<int>(_Titulo.size()), &Tam);
	int TamIcono = (_Icono() == NULL) ? 0 : 20;
	Ret.cx = TamIcono + (TOOLTIPINFO_PADDING * 2) + Tam.cx;

	int Ancho1 = 0, Ancho2 = static_cast<int>(Etiqueta->Nota * 16.0f);
	// Miro el ancho de cada columna y guardo las columnas
	for (size_t i = 0; i < NumFilas; i++) {
		SelectObject(hDC, _FuenteNombreValor());
		GetTextExtentPoint32(hDC, Col1[i], static_cast<int>(wcslen(Col1[i])), &Tam);
		if (Tam.cx > Ancho1) Ancho1 = Tam.cx;
		_Col1.push_back(ToolTipInfo_Celda(Col1[i], Tam.cx));

		SelectObject(hDC, _FuenteValor());
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
	HFONT       vFuente = static_cast<HFONT>(SelectObject(hDC, _Fuente()));
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
	
	// Pinto el fondo
	_PintarFondo(Buffer, &RC);

	// Pinto el borde
	HBRUSH BrochaBorde = CreateSolidBrush(Skin.Borde);
	FrameRect(Buffer, &RC, BrochaBorde);
	DeleteObject(BrochaBorde);

	// Pinto el texto
	SetBkMode(Buffer, TRANSPARENT);
	SetTextColor(Buffer, Skin.Texto);
	HFONT vFuente = static_cast<HFONT>(SelectObject(Buffer, _Fuente()));
	DrawText(Buffer, _Str.c_str(), static_cast<int>(_Str.size()), &RCT, DT_CENTER);
	SelectObject(Buffer, vFuente);

	// Copio el buffer al DC
	BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

	// Elimino el buffer de la memória
	SelectObject(Buffer, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(Buffer);
}

void ToolTipInfo_Texto::_PintarFondo(HDC Buffer, RECT *RC) {
	HBRUSH BrochaFondo = CreateSolidBrush(Skin.Fondo);
	FillRect(Buffer, RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}







/*
  _______          _ _______ _      _____        __              _______        _        ______
 |__   __|        | |__   __(_)    |_   _|      / _|            |__   __|      | |      |  ____|
	| | ___   ___ | |  | |   _ _ __  | |  _ __ | |_ ___            | | _____  _| |_ ___ | |__   _ __ _ __ ___  _ __
	| |/ _ \ / _ \| |  | |  | | '_ \ | | | '_ \|  _/ _ \           | |/ _ \ \/ / __/ _ \|  __| | '__| '__/ _ \| '__|
	| | (_) | (_) | |  | |  | | |_) || |_| | | | || (_) |          | |  __/>  <| || (_) | |____| |  | | | (_) | |
	|_|\___/ \___/|_|  |_|  |_| .__/_____|_| |_|_| \___/  ______   |_|\___/_/\_\\__\___/|______|_|  |_|  \___/|_|
							  | |                        |______|
							  |_|																					  */

void ToolTipInfo_TextoError::_PintarFondo(HDC Buffer, RECT *RC) {
	HBRUSH BrochaFondo = CreateSolidBrush(Skin.FondoError);
	FillRect(Buffer, RC, BrochaFondo);
	DeleteObject(BrochaFondo);
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

void ToolTipsInfo::MostrarToolTipError(const wchar_t *Texto) {
	MostrarToolTipError(std::wstring(Texto));
}

void ToolTipsInfo::MostrarToolTipError(std::wstring &Texto) {
	ToolTipInfo *TT = new ToolTipInfo_TextoError(Texto);
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
			_ToolTips[i]->Ocultar(FALSE);
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
		_ToolTips[i]->Ocultar(FALSE);
		//		delete _ToolTips[i];
	}
	//	_ToolTips.resize(0);
}

void ToolTipsInfo::OcultarRapido(void) {
	for (size_t i = 0; i < _ToolTips.size(); i++) {
		_ToolTips[i]->Ocultar(TRUE);
		//		delete _ToolTips[i];
	}
	//	_ToolTips.resize(0);
}

HWND ToolTipsInfo::Padre(void) {
	if (_Padre != NULL) return _Padre->hWnd();
	return NULL;
}