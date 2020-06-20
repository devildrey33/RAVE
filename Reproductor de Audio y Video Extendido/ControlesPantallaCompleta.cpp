#include "stdafx.h"
#include "ControlesPantallaCompleta.h"
#include "DMouse.h"
#include "DMensajesWnd.h"
//#include "Rave_Skin.h"
#include "resource.h"

// Tamaño del ControlesPantallaCompleta máximo horizontal
#define ANCHO_CPCH	1000
#define ALTO_CPCH	  80
// Tamaño del ControlesPantallaCompleta máximo vertical
#define ANCHO_CPCV	  90
#define ALTO_CPCV	 900

#define ID_BOTON_BD_PC		WM_USER + 500
#define ID_BOTON_LISTA_PC	WM_USER + 501

void ControlesPantallaCompleta::Crear(void) {	
	DVentana::CrearVentana(NULL, L"ControlesPantallaCompleta", L"", 0, 0, 1000, 80, WS_POPUP, WS_EX_TOOLWINDOW, NULL, NULL, NULL, NULL);
	
	Alineacion = static_cast<CPC_Alineacion>(App.Opciones.AlineacionControlesVideo());

	//AnimateWindow(_hWnd, 100, AW_HOR_POSITIVE | AW_VER_POSITIVE);

	// Asigno la posición del menú y lo hago siempre visible
//	SetWindowPos(_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);

	RECT RC;
	GetClientRect(hWnd(), &RC);

	BotonAtras.CrearBotonEx(this,	 IDI_PREV,  24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  10, 10, 30, 30, ID_BOTON_ANTERIOR);
	BotonAtras.TextoToolTip = L"Medio anterior";
	BotonPlay.CrearBotonEx(this,	 IDI_PLAY4,  24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  50, 10, 30, 30, ID_BOTON_PLAY);
	BotonPlay.TextoToolTip = L"Play";
	BotonStop.CrearBotonEx(this,	 IDI_STOP,  24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  90, 10, 30, 30, ID_BOTON_STOP);
	BotonStop.TextoToolTip = L"Stop";
	BotonAdelante.CrearBotonEx(this, IDI_NEXT,  24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 130, 10, 30, 30, ID_BOTON_SIGUIENTE);
	BotonAdelante.TextoToolTip = L"Siguiente medio";

	LabelRatio.CrearEtiquetaEx(this, L"x1.0", 170, 16, 40, 30, ID_LABEL_RATIO, DEtiquetaEx_Alineacion_Centrado);

	BotonMezclar.CrearBotonEx(this, IDI_MEZCLAR, 24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 260, 10, 30, 30, ID_BOTON_MEZCLAR);
	BotonMezclar.TextoToolTip = L"Mezclar lista";
	
	if (App.Opciones.Shufle() == TRUE) BotonMezclar.Marcado(TRUE);
	BotonRepetir.CrearBotonEx(this, IDI_REPETIR, 24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 300, 10, 30, 30, ID_BOTON_REPETIR);
	BotonRepetir.TextoToolTip = L"Repetir lista";
	if (App.Opciones.Repeat() > 0) BotonRepetir.Marcado(TRUE);

	BotonRotar.CrearBotonEx(this, (Alineacion == Arriba || Alineacion == Abajo) ? IDI_ROTARV : IDI_ROTARH, 24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 380, 10, 30, 30, ID_BOTON_ROTAR);
	BotonRotar.TextoToolTip = L"Rotar controles";

	BotonBD.CrearBotonEx(this, IDI_BASEDATOS2, 24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 460, 10, 30, 30, ID_BOTON_BD_PC);
	BotonBD.TextoToolTip = L"Base de datos";

	BotonLista.CrearBotonEx(this, IDI_LISTA, 24, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 500, 10, 30, 30, ID_BOTON_LISTA_PC);
	BotonLista.TextoToolTip = L"Lista de medios";
//	BotonLista.Fuente.CrearFuente(18, BotonMezclar.Skin.FuenteNombre.c_str(), TRUE);

	SliderTiempo.CrearBarraDesplazamientoEx(this, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO);
//	SliderTiempo.Crear(hWnd, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO, WS_CHILD | TBS_NOTICKS | WS_VISIBLE, 0, 30000, 0);
//	SliderTiempo.TamPagina(30000 / 50);

	SliderVolumen.CrearBarraVolumen(this, RC.right - 145, 17, 90, 16, ID_SLIDER_VOLUMEN, 0, 200, 100);
	LabelVolumen.CrearEtiquetaEx(this, L"100%", RC.right - 40, 16, 40, 20, ID_LABEL_VOLUMEN, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);

	LabelTiempoActual.CrearEtiquetaEx(this, L"00:00", RC.right - 265, 12, 55, 20, ID_LABEL_TIEMPOACTUAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);
	LabelTiempoSeparador.CrearEtiquetaEx(this, L"/", RC.right - 210, 12, 10, 20, ID_LABEL_TIEMPOSEPARADOR, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);
	LabelTiempoTotal.CrearEtiquetaEx(this, L"00:00", RC.right - 200, 12, 55, 20, ID_LABEL_TIEMPOTOTAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);

	Opacidad(0);
}



void ControlesPantallaCompleta::RotarControles(const BOOL Invertido) {
	// El BotonAtras siempre se queda en la misma posición
	UINT IcoRotar = IDI_ROTARH;
	if (Invertido == false) {
		switch (Alineacion) {
			case Abajo		:	Alineacion = Izquierda;		IcoRotar = IDI_ROTARH;		break;
			case Izquierda	:	Alineacion = Arriba;		IcoRotar = IDI_ROTARV;		break;
			case Arriba		:	Alineacion = Derecha;		IcoRotar = IDI_ROTARH;		break;
			case Derecha	:	Alineacion = Abajo;			IcoRotar = IDI_ROTARV;		break;
		}
	}
	else {
		switch (Alineacion) {
			case Abajo		:	Alineacion = Derecha;		IcoRotar = IDI_ROTARV;		break;
			case Izquierda	:	Alineacion = Abajo;			IcoRotar = IDI_ROTARH;		break;
			case Arriba		:	Alineacion = Izquierda;		IcoRotar = IDI_ROTARV;		break;
			case Derecha	:	Alineacion = Arriba;		IcoRotar = IDI_ROTARH;		break;
		}

	}

	BotonRotar.Icono(IcoRotar, 24);
	Alinear();

	if (BotonBD.Marcado() == TRUE)		MostrarBD(TRUE);
	if (BotonLista.Marcado() == TRUE)	MostrarLista(TRUE);
}

void ControlesPantallaCompleta::Mostrar(void) {
	KillTimer(hWnd(), TIMER_CPC_INACTIVIDAD);
	SetTimer(hWnd(), TIMER_CPC_INACTIVIDAD, App.Opciones.OcultarMouseEnVideo(), NULL);

	// NO USAR IsWindowVisible para comprobar si están visibles los controles
	if (_Visible == TRUE) return;

	_Visible = TRUE;
	Debug_Escribir(L"ControlesPantallaCompleta::Mostrar\n");

	Alinear();

	DMouse::ObtenerPosicion(&_MousePos);
	DMouse::Visible(TRUE);
}

void ControlesPantallaCompleta::Transicion(const CPC_Transicion nTransicion) {
	Debug_Escribir_Varg(L"ControlesPantallaCompleta::Transicion : %d\n", nTransicion);
	DWORD Duracion = DhWnd::TiempoAnimaciones;
	if (_AniMostrar.Animando() == TRUE) {
		Duracion = _AniMostrar.TiempoActual();
		_AniMostrar.Terminar();
//		return;
	}

	static double OpacidadHasta = 0.0f;
	switch (nTransicion) {
		case CPC_Transicion_Mostrar:
			OpacidadHasta = static_cast<double>(App.Opciones.OpacidadControlesVideo());
			break;
		case CPC_Transicion_Ocultar:
			OpacidadHasta = 0.0f;
			break;
		case CPC_Transicion_Resaltado:
			break;
		case CPC_Transicion_Normal:
			break;
	}

	_AniMostrar.Iniciar({ static_cast<double>(Opacidad()) }, { &OpacidadHasta }, Duracion, [=](DAnimacion::Valores& Datos, const BOOL Terminado) {
		Opacidad(static_cast<BYTE>(Datos[0].Decimal()));
		App.VentanaRave.Arbol.Opacidad(static_cast<BYTE>(Datos[0].Decimal()));
		App.VentanaRave.Lista.Opacidad(static_cast<BYTE>(Datos[0].Decimal()));
//		_ColorBorde = Valores[1];
//		_ColorTexto = Valores[2];
		// Ha terminado de ocultarse
		if (Datos[0].Decimal() == 0.0f && Terminado == TRUE) {
			Visible(FALSE);
//			BotonBD.Marcado(FALSE);
//			BotonLista.Marcado(FALSE);
//			App.VentanaRave.Arbol.Visible(FALSE);
//			App.VentanaRave.Lista.Visible(FALSE);
		}
		else {
//			Repintar();
		}
	});

}


void ControlesPantallaCompleta::Alinear(void) {	
	int Ancho = 0, Alto = 0;
	RECT RC;
	GetWindowRect(App.VentanaRave.hWnd(), &RC);
	int X = 0, Y = 0;

	// Colocación de los controles (vertical / horizontal)
	switch (Alineacion) {
		case Abajo :
		case Arriba:
			if (RC.right - RC.left < ANCHO_CPCH)	Ancho = (RC.right - RC.left) - 100;
			else									Ancho = ANCHO_CPCH;
			Alto = ALTO_CPCH;
			X = (RC.right + RC.left - Ancho) / 2;

			SetWindowPos(BotonAtras.hWnd()			, HWND_TOP, 10, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonAdelante.hWnd()		, HWND_TOP, 130, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonPlay.hWnd()			, HWND_TOP, 50, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonStop.hWnd()			, HWND_TOP, 90, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(LabelRatio.hWnd()			, HWND_TOP, 170, 16, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonMezclar.hWnd()		, HWND_TOP,	260, 10, 30, 30						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(BotonRepetir.hWnd()		, HWND_TOP,	300, 10, 30, 30						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(BotonRotar.hWnd()			, HWND_TOP, 380, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonBD.hWnd()				, HWND_TOP, 460, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonLista.hWnd()			, HWND_TOP, 500, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(LabelTiempoActual.hWnd()	, HWND_TOP, Ancho - 285, 12, 0, 0				, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(LabelTiempoSeparador.hWnd(), HWND_TOP, Ancho - 230, 12, 0, 0				, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(LabelTiempoTotal.hWnd()	, HWND_TOP,	Ancho - 220, 12, 0, 0				, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(SliderVolumen.hWnd()		, HWND_TOP, Ancho - 145, 15, 90, 16				, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(LabelVolumen.hWnd()		, HWND_TOP,	Ancho - 50, 12, 0, 0				, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(SliderTiempo.hWnd()		, HWND_TOP, 10, 47, Ancho - 20, 20				, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
			SliderTiempo.Alineacion(IzquierdaDerecha);
			break;
		case Izquierda:
		case Derecha:
			if (RC.bottom - RC.top < ALTO_CPCV)		Alto = (RC.bottom - RC.top) - 100;
			else									Alto = ALTO_CPCV;
			Ancho = ANCHO_CPCV;
			Y = (RC.bottom + RC.top - Alto) / 2;

			SetWindowPos(BotonAtras.hWnd()			, HWND_TOP, 10, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonAdelante.hWnd()		, HWND_TOP, 50, 10, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonPlay.hWnd()			, HWND_TOP, 10, 50, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonStop.hWnd()			, HWND_TOP, 50, 50, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(LabelRatio.hWnd()			, HWND_TOP, 25, 90, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonMezclar.hWnd()		, HWND_TOP,	10, 140, 30, 30						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(BotonRepetir.hWnd()		, HWND_TOP,	50, 140, 30, 30						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(BotonRotar.hWnd()			, HWND_TOP, 30, 190, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonBD.hWnd()				, HWND_TOP, 10, 240, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(BotonLista.hWnd()			, HWND_TOP, 50, 240, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			
			
			SetWindowPos(SliderVolumen.hWnd()		, HWND_TOP, 10, 290, 70, 20						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
//			SetWindowPos(LabelVolumen.hWnd()		, HWND_TOP,	950, 12, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | NO_MOVE);
			SetWindowPos(SliderTiempo.hWnd()		, HWND_TOP, 30, 320, 30, Alto - 390				, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
			SliderTiempo.Alineacion(AbajoArriba);
			SetWindowPos(LabelTiempoActual.hWnd()	, HWND_TOP, 17, Alto - 60, 0, 0					, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
//			SetWindowPos(LabelTiempoSeparador.hWnd(), HWND_TOP, 770, 12, 0, 0						, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			SetWindowPos(LabelTiempoTotal.hWnd()	, HWND_TOP,	17, Alto - 30, 0, 0					, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			break;
	}

//	RECT RV;
//	GetWindowRect(App.VentanaRave.hWnd(), &RV);
	// Colocación del control (Arriba, Abajo, Izquierda, Derecha)
	int Top = 0;
	switch (Alineacion) {
		case Abajo		:	
			Y = (RC.top >= 0) ? RC.bottom + RC.top - Alto : RC.bottom - Ancho;
			SetWindowPos(_hWnd, HWND_TOPMOST	, X, Y, Ancho, Alto		, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_FRAMECHANGED);	
			SliderTiempo.ToolTip(DBarraEx_ToolTip_Arriba);
			break;
		case Izquierda	:	
			X = RC.left;
			SetWindowPos(_hWnd, HWND_TOPMOST	, X, Y, Ancho, Alto		, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			SliderTiempo.ToolTip(DBarraEx_ToolTip_Derecha);
			break;
		case Arriba		:	
			Y = RC.top;
			SetWindowPos(_hWnd, HWND_TOPMOST	, X, Y, Ancho, Alto		, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			SliderTiempo.ToolTip(DBarraEx_ToolTip_Abajo);
			break;
		case Derecha	:	
			X = (RC.left >= 0) ? RC.right + RC.left - Ancho : RC.right - Ancho;
			SetWindowPos(_hWnd, HWND_TOPMOST	, X , Y, Ancho, Alto	, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			SliderTiempo.ToolTip(DBarraEx_ToolTip_Izquierda);
			break;
	}
	
	Repintar();

	Transicion(CPC_Transicion_Mostrar);
}

void ControlesPantallaCompleta::Ocultar(void) {
	// Si el menú del repeat o del video estan visibles no oculto nada
	if (App.VentanaRave.Menu_Repetir.Visible() == TRUE) return;
	if (App.VentanaRave.Menu_Video.Visible() == TRUE) return;
	// Scroll presionado en el arbol, no oculto nada
	if (App.VentanaRave.Arbol.ScrollH_Estado() == DBarraScrollEx_Estado_Presionado || App.VentanaRave.Arbol.ScrollV_Estado() == DBarraScrollEx_Estado_Presionado) return;
	// Scroll presionado en la lista, no oculto nada
	if (App.VentanaRave.Lista.ScrollH_Estado() == DBarraScrollEx_Estado_Presionado || App.VentanaRave.Lista.ScrollV_Estado() == DBarraScrollEx_Estado_Presionado) return;


	// Obtengo la recta de esta ventana, y la posición del mouse
	RECT RV, RL, RBD;
	GetWindowRect(_hWnd, &RV);
	DMouse::ObtenerPosicion(&_MousePos);

	// Si el mouse está dentro de este control. No oculto nada
	if (PtInRect(&RV, _MousePos) == TRUE) 
		return;

	// Si el mouse está dentro de la base de datos, y la base de datos está visible. No oculto nada
	GetWindowRect(App.VentanaRave.Arbol.hWnd(), &RBD);
	if (PtInRect(&RBD, _MousePos) == TRUE && App.VentanaRave.Arbol.Visible() == TRUE)
		return;

	// Si el mouse está dentro de la lista, y la lista está visible. No oculto nada
	GetWindowRect(App.VentanaRave.Lista.hWnd(), &RL);
	if (PtInRect(&RL, _MousePos) == TRUE && App.VentanaRave.Lista.Visible() == TRUE) 
		return;


	Debug_Escribir(L"ControlesPantallaCompleta::Ocultar\n");
	KillTimer(hWnd(), TIMER_CPC_INACTIVIDAD);

//	Visible(FALSE);
	Transicion(CPC_Transicion_Ocultar);

	DMouse::Visible(FALSE);
	_Visible = FALSE;
}



void ControlesPantallaCompleta::Evento_SliderTiempo_Cambio(void) {
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
}

void ControlesPantallaCompleta::Evento_SliderTiempo_Cambiado(void) {
//	static wchar_t TiempoStr[64];
	std::wstring	TiempoStr;
	static double	TmpFloat = 0.0f;
	Debug_Escribir_Varg(L"Evento_SliderH %d, %f\n", App.MP.TiempoTotalMs(), TmpFloat);
	App.MP.TiempoStr(static_cast<UINT64>(App.MP.TiempoTotalMs() * TmpFloat), TiempoStr);
	LabelTiempoActual.Texto(TiempoStr);
	App.VentanaRave.LabelTiempoActual.Texto(TiempoStr);
	App.MP.TiempoActual(SliderTiempo.Valor());

	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
//	SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Tabla_Opciones.OcultarMouseEnVideo(), NULL);
}

void ControlesPantallaCompleta::Evento_SliderVolumen_Cambiado(void) {
	App.Opciones.Volumen(static_cast<int>(SliderVolumen.Valor()));
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
	SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.Opciones.OcultarMouseEnVideo(), NULL);
}

void ControlesPantallaCompleta::Evento_SliderVolumen_Cambio(void) {
	int Volumen = static_cast<int>(SliderVolumen.Valor());
	App.MP.Volumen(Volumen);
	//swprintf(TiempoStr, 64, L"%d%%", HIWORD(wParam));
/*	std::wstring StrVol = std::to_wstring(Volumen) + L"%";
	LabelVolumen.Texto(StrVol);
	App.VentanaRave.LabelVolumen.Texto(StrVol);*/
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);

}

void ControlesPantallaCompleta::Pintar(HDC DC) {
	RECT RC;
	GetClientRect(_hWnd, &RC);


	/*	Gdiplus::Graphics	Graficos(DC);
		Gdiplus::Color		ColorFondo(COLOR_FONDO);
		Gdiplus::Color		ColorBorde(COLOR_MENU_BORDE);
		Gdiplus::SolidBrush BrochaFondo(ColorFondo);
		Gdiplus::Pen        PlumaBorde(ColorBorde, 1);
		Graficos.FillRectangle(&BrochaFondo, 0, 0, RC.right, RC.bottom);
		Graficos.DrawRectangle(&PlumaBorde, 1, 1, RC.right -1, RC.bottom -1);*/


	// BackBuffer 
	HDC     TmphDC		= CreateCompatibleDC(NULL);
	HBITMAP TmphDCBmp	= CreateCompatibleBitmap(DC, RC.right, RC.bottom);
	HBITMAP VTmphDCBmp	= static_cast<HBITMAP>(SelectObject(TmphDC, TmphDCBmp));

	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	HBRUSH BrochaBorde = CreateSolidBrush(COLOR_MENU_BORDE);
	FillRect(TmphDC, &RC, BrochaFondo);
	FrameRect(TmphDC, &RC, BrochaBorde);
	DeleteObject(BrochaFondo);
	DeleteObject(BrochaBorde);

	BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, TmphDC, 0, 0, SRCCOPY);

	SelectObject(TmphDC, VTmphDCBmp);
	DeleteObject(TmphDCBmp);
	DeleteDC(TmphDC);

}

void ControlesPantallaCompleta::Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(_hWnd, &PS);
	Pintar(DC);
	EndPaint(_hWnd, &PS);
}


void ControlesPantallaCompleta::Evento_BarraEx_Cambiando(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_SLIDER_VOLUMEN : Evento_SliderVolumen_Cambio();								break;
	}
}

void ControlesPantallaCompleta::Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {		
		case ID_SLIDER_TIEMPO  : Evento_SliderTiempo_Cambiado();							break;
		case ID_SLIDER_VOLUMEN : Evento_SliderVolumen_Cambiado();							break;
	}
}

void ControlesPantallaCompleta::Evento_BotonEx_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BOTON_ROTAR    : RotarControles((DatosMouse.Boton == 0) ? FALSE : TRUE);	break;
		case ID_BOTON_BD_PC    : MostrarBD();												break;
		case ID_BOTON_LISTA_PC : MostrarLista();											break;
		default                : App.VentanaRave.Evento_BotonEx_Mouse_Click(DatosMouse);	break;
	}
}

void ControlesPantallaCompleta::MostrarBD(const BOOL Forzar) {
	App.VentanaRave.Lista.Visible(FALSE);
	BotonLista.Marcado(FALSE);
	if (App.VentanaRave.Arbol.Visible() == TRUE && Forzar == FALSE) {
		App.VentanaRave.Arbol.Visible(FALSE);
		BotonBD.Marcado(FALSE);
	}
	else {
		BotonBD.Marcado(TRUE);
		RECT RW;
		GetWindowRect(BotonBD.hWnd(), &RW);
		switch (Alineacion) {
			case Abajo:		SetWindowPos(App.VentanaRave.Arbol.hWnd(), HWND_TOPMOST, RW.left, RW.top - 320, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
			case Arriba:	SetWindowPos(App.VentanaRave.Arbol.hWnd(), HWND_TOPMOST, RW.left, RW.bottom + 50, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
			case Izquierda:	SetWindowPos(App.VentanaRave.Arbol.hWnd(), HWND_TOPMOST, RW.right + 60, RW.top, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
			case Derecha:	SetWindowPos(App.VentanaRave.Arbol.hWnd(), HWND_TOPMOST, RW.left - 420, RW.top, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
		}

		ShowWindow(App.VentanaRave.Arbol.hWnd(), SW_SHOW);
	}
//	SetFocus(App.VentanaRave.Arbol.hWnd());
//	SetCapture(App.VentanaRave.Arbol.hWnd());
}

void ControlesPantallaCompleta::MostrarLista(const BOOL Forzar) {
	App.VentanaRave.Arbol.Visible(FALSE);
	BotonBD.Marcado(FALSE);
	if (App.VentanaRave.Lista.Visible() == TRUE && Forzar == FALSE) {
		App.VentanaRave.Lista.Visible(FALSE);
		BotonLista.Marcado(FALSE);
	}
	else {
		BotonLista.Marcado(TRUE);
		RECT RW;
		GetWindowRect(BotonLista.hWnd(), &RW);
		switch (Alineacion) {
			case Abajo:		SetWindowPos(App.VentanaRave.Lista.hWnd(), HWND_TOPMOST, RW.left, RW.top - 320, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
			case Arriba:	SetWindowPos(App.VentanaRave.Lista.hWnd(), HWND_TOPMOST, RW.left, RW.bottom + 50, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
			case Izquierda:	SetWindowPos(App.VentanaRave.Lista.hWnd(), HWND_TOPMOST, RW.right + 20, RW.top, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
			case Derecha:	SetWindowPos(App.VentanaRave.Lista.hWnd(), HWND_TOPMOST, RW.left - 460, RW.top, 400, 300, SWP_SHOWWINDOW | SWP_NOACTIVATE); 	break;
		}
		ShowWindow(App.VentanaRave.Lista.hWnd(), SW_SHOW);
	}
//	SetFocus(App.VentanaRave.Lista.hWnd());
//	SetCapture(App.VentanaRave.Lista.hWnd());
}

void ControlesPantallaCompleta::Evento_Temporizador(const UINT cID) {
	switch (cID) {
		// Controles PantallaCompleta Inactividad
		case TIMER_CPC_INACTIVIDAD:
			Ocultar();
			break;
	}
}

LRESULT CALLBACK ControlesPantallaCompleta::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static POINT nPos = { 0, 0 };
	switch (uMsg) {
		case WM_ERASEBKGND:
			Pintar(reinterpret_cast<HDC>(wParam));
			return TRUE;

		case WM_PAINT :
			Evento_Pintar();
			return 0;

		case WM_MOUSEMOVE :
			DWL::DMouse::ObtenerPosicion(&nPos);
			if (_MousePos.x != nPos.x || _MousePos.y != nPos.y) {
				KillTimer(_hWnd, TIMER_CPC_INACTIVIDAD);
				SetTimer(_hWnd, TIMER_CPC_INACTIVIDAD, App.Opciones.OcultarMouseEnVideo(), NULL);
				_MousePos = nPos;
				Mostrar();
			}
			return 0;
/*		case WM_TIMER :
			if (wParam == TIMER_CPC_INACTIVIDAD) 
				Ocultar();
			break;*/

		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Click(WPARAM_TO_DEVENTOMOUSE(wParam));			
			return 0;
		case DWL_BOTONEX_MOUSEDOWN:
			App.VentanaRave.Evento_BotonEx_Mouse_Presionado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BOTONEX_MOUSEUP:
			App.VentanaRave.Evento_BotonEx_Mouse_Soltado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

			// Barra de desplazamiento (barra de tiempo y volumen)
		case DWL_BARRAEX_CAMBIANDO:	// Se está modificando (mouse down)
			Evento_BarraEx_Cambiando(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case WM_TIMER :
			Evento_Temporizador(static_cast<UINT>(wParam));
			return 0;
	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}