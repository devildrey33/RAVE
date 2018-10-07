#include "stdafx.h"
#include "ControlesPantallaCompleta.h"
#include "DMouse.h"
#include "DMensajesWnd.h"
#include "Rave_Skin.h"
#include "resource.h"

void ControlesPantallaCompleta::Crear(void) {	
	DVentana::CrearVentana(NULL, L"ControlesPantallaCompleta", L"", 0, 0, 500, 80, WS_POPUP, WS_EX_TOOLWINDOW, NULL, NULL, NULL, NULL);
	
	//AnimateWindow(_hWnd, 100, AW_HOR_POSITIVE | AW_VER_POSITIVE);

	// Asigno la posición del menú y lo hago siempre visible
//	SetWindowPos(_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);

	RECT RC;
	GetClientRect(hWnd(), &RC);

	BotonAtras.CrearBotonEx(this,	 IDI_PREV32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  10, 10, 30, 30, ID_BOTON_ANTERIOR);
	BotonPlay.CrearBotonEx(this,	 IDI_PLAY32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  50, 10, 30, 30, ID_BOTON_PLAY);
//	BotonPausa.CrearBotonEx(this,	 IDI_PAUSA32, 32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  90, 10, 30, 30, ID_BOTON_PAUSA);
	BotonStop.CrearBotonEx(this,	 IDI_STOP32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  90, 10, 30, 30, ID_BOTON_STOP);
	BotonAdelante.CrearBotonEx(this, IDI_NEXT32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 130, 10, 30, 30, ID_BOTON_SIGUIENTE);

	LabelRatio.CrearEtiquetaEx(this, L"x1.0", 170, 16, 40, 30, ID_LABEL_RATIO, DEtiquetaEx_Alineacion_Centrado);

	BotonMezclar.CrearBotonEx(this, L"Mezclar", 220, 10, 80, 30, ID_BOTON_MEZCLAR);
	BotonMezclar.Fuente = Fuente18Negrita;
	if (App.BD.Opciones_Shufle() == TRUE) BotonMezclar.Marcado(TRUE);
	BotonRepetir.CrearBotonEx(this, L"Repetir", 310, 10, 80, 30, ID_BOTON_REPETIR);
	BotonRepetir.Fuente = Fuente18Negrita;
	if (App.BD.Opciones_Repeat() > 0) BotonRepetir.Marcado(TRUE);


	SliderTiempo.CrearBarraDesplazamientoEx(this, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO);
//	SliderTiempo.Crear(hWnd, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO, WS_CHILD | TBS_NOTICKS | WS_VISIBLE, 0, 30000, 0);
//	SliderTiempo.TamPagina(30000 / 50);

	SliderVolumen.CrearBarraVolumen(this, RC.right - 145, 17, 90, 16, ID_SLIDER_VOLUMEN, 0, 200, 100);
	LabelVolumen.CrearEtiquetaEx(this, L"100%", RC.right - 40, 16, 40, 20, ID_LABEL_VOLUMEN, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);

	LabelTiempoActual.CrearEtiquetaEx(this, L"00:00", RC.right - 265, 16, 55, 20, ID_LABEL_TIEMPOACTUAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);
	LabelTiempoSeparador.CrearEtiquetaEx(this, L"/", RC.right - 210, 16, 10, 20, ID_LABEL_TIEMPOSEPARADOR, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);
	LabelTiempoTotal.CrearEtiquetaEx(this, L"00:00", RC.right - 200, 16, 55, 20, ID_LABEL_TIEMPOTOTAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);

	Opacidad(240);
}


void ControlesPantallaCompleta::Mostrar(void) {
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
	SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Opciones_OcultarMouseEnVideo(), NULL);


//	if (_Visible == TRUE) return;
//	_Visible = TRUE;
	RECT RC;
	GetWindowRect(App.VentanaRave.hWnd(), &RC);
	int AnchoTotal = (RC.right - RC.left);
	LONG X = (AnchoTotal - 1000) / 2;

	int Ancho = 1000;
//	MoveWindow(hWnd(), X, RC.bottom - 80, Ancho, 80, TRUE);

	MoveWindow(SliderTiempo.hWnd(),				10, 45, Ancho - 20, 24, TRUE);
	MoveWindow(SliderVolumen.hWnd(),			Ancho - 145, 13, 90, 16, TRUE);
	MoveWindow(LabelVolumen.hWnd(),				Ancho - 50, 12, 40, 20, TRUE);
	MoveWindow(LabelTiempoActual.hWnd(),		Ancho - 285, 12, 55, 20, TRUE);
	MoveWindow(LabelTiempoSeparador.hWnd(),		Ancho - 230, 12, 10, 20, TRUE);
	MoveWindow(LabelTiempoTotal.hWnd(),			Ancho - 220, 12, 55, 20, TRUE);

	SetWindowPos(_hWnd, HWND_TOP, RC.left + X, RC.bottom - 80, Ancho, 80, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	//	ShowWindow(_hWnd, SW_SHOWNOACTIVATE);

	DMouse::ObtenerPosicion(&App.VentanaRave.MousePos);
	DMouse::Visible(TRUE);
//	Debug_Escribir(L"ControlesPantallaCompleta::Mostrar\n");
//	DMouse::Visible(TRUE);
}

void ControlesPantallaCompleta::Ocultar(void) {
	// Si el menú del repeat o del video estan visibles no oculto nada
	if (App.VentanaRave.Menu_Repetir.Visible() == TRUE) return;
	if (App.VentanaRave.Menu_Video.Visible() == TRUE) return;

	// Obtengo la recta de esta ventana, y la posición del mouse
	RECT RV;
	GetWindowRect(_hWnd, &RV);
	DMouse::ObtenerPosicion(&App.VentanaRave.MousePos);
	
	// Si el mouse está dentro del control no oculto nada
	if (PtInRect(&RV, App.VentanaRave.MousePos) == TRUE) return;
	

	Debug_Escribir(L"ControlesPantallaCompleta::Ocultar\n");
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);

	Visible(FALSE);
	DMouse::Visible(FALSE);
//	_Visible = FALSE;
}



void ControlesPantallaCompleta::Evento_SliderTiempo_Cambio(void) {
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
}

void ControlesPantallaCompleta::Evento_SliderTiempo_Cambiado(void) {
//	static wchar_t TiempoStr[64];
	std::wstring TiempoStr;
	static float   TmpFloat = 0.0f;
	Debug_Escribir_Varg(L"Evento_SliderH %d, %f\n", App.VLC.TiempoTotalMs(), TmpFloat);
	App.VLC.TiempoStr(static_cast<UINT64>(static_cast<float>(App.VLC.TiempoTotalMs()) * TmpFloat), TiempoStr);
	LabelTiempoActual.Texto(TiempoStr);
	App.VentanaRave.LabelTiempoActual.Texto(TiempoStr);
	App.VLC.TiempoActual(SliderTiempo.Valor());

	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
//	SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Tabla_Opciones.OcultarMouseEnVideo(), NULL);
}

void ControlesPantallaCompleta::Evento_SliderVolumen_Cambiado(void) {
	App.BD.Opciones_Volumen(static_cast<int>(SliderVolumen.Valor()));
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
	SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Opciones_OcultarMouseEnVideo(), NULL);
}

void ControlesPantallaCompleta::Evento_SliderVolumen_Cambio(void) {
	int Volumen = static_cast<int>(SliderVolumen.Valor());
	App.VLC.Volumen(Volumen);
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

	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	HBRUSH BrochaBorde = CreateSolidBrush(COLOR_MENU_BORDE);
	FillRect(DC, &RC, BrochaFondo);
	FrameRect(DC, &RC, BrochaBorde);
	DeleteObject(BrochaFondo);
	DeleteObject(BrochaBorde);
}

void ControlesPantallaCompleta::Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(_hWnd, &PS);
	Pintar(DC);
	EndPaint(_hWnd, &PS);
}


void ControlesPantallaCompleta::Evento_BarraEx_Cambio(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_SLIDER_VOLUMEN: 					Evento_SliderVolumen_Cambio();					break;
	}
}

void ControlesPantallaCompleta::Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_SLIDER_TIEMPO:						Evento_SliderTiempo_Cambiado();					break;
		case ID_SLIDER_VOLUMEN: 					Evento_SliderVolumen_Cambiado();				break;
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
			if (App.VentanaRave.MousePos.x != nPos.x || App.VentanaRave.MousePos.y != nPos.y) {
				KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
				SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Opciones_OcultarMouseEnVideo(), NULL);
				App.VentanaRave.MousePos = nPos;
				Mostrar();
			}
			return 0;
/*		case WM_TIMER :
			if (wParam == TIMER_CPC_INACTIVIDAD) 
				Ocultar();
			break;*/

		case DWL_BOTONEX_CLICK:
			App.VentanaRave.Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BOTONEX_MOUSEDOWN:
			App.VentanaRave.Evento_BotonEx_Mouse_Presionado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BOTONEX_MOUSEUP:
			App.VentanaRave.Evento_BotonEx_Mouse_Soltado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

			// Barra de desplazamiento (barra de tiempo y volumen)
		case DWL_BARRAEX_CAMBIO:	// Se está modificando (mouse down)
			Evento_BarraEx_Cambio(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}