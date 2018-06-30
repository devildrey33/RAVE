#include "stdafx.h"
#include "ControlesPantallaCompleta.h"
#include "DMouse.h"
#include "DMensajesWnd.h"
#include "Rave_Skin.h"

void ControlesPantallaCompleta::Crear(void) {	
	DVentana::CrearVentana(NULL, L"ControlesPantallaCompleta", L"", 0, 0, 500, 80, WS_POPUP, WS_EX_TOOLWINDOW, NULL, NULL, NULL, NULL);
	RECT RC;
	GetClientRect(hWnd(), &RC);
	BotonAtras.Crear(this, L"<", 10, 10, 30, 30, ID_BOTON_ANTERIOR);
	BotonPlay.Crear(this, L"P", 50, 10, 30, 30, ID_BOTON_PLAY);
	BotonPausa.Crear(this, L"||", 90, 10, 30, 30, ID_BOTON_PAUSA);
	BotonStop.Crear(this, L"S", 130, 10, 30, 30, ID_BOTON_STOP);
	BotonAdelante.Crear(this, L">", 170, 10, 30, 30, ID_BOTON_SIGUIENTE);

	SliderTiempo.CrearBarraDesplazamientoEx(this, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO);
//	SliderTiempo.Crear(hWnd, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO, WS_CHILD | TBS_NOTICKS | WS_VISIBLE, 0, 30000, 0);
//	SliderTiempo.TamPagina(30000 / 50);

	SliderVolumen.CrearBarraDesplazamientoEx(this, RC.right - 145, 13, 90, 16, ID_SLIDER_VOLUMEN, 0, 200, 100);
	LabelVolumen.CrearLabelEx(this, L"100%", RC.right - 40, 12, 40, 20, ID_LABEL_VOLUMEN, WS_CHILD | WS_VISIBLE);

	LabelTiempoActual.CrearLabelEx(this, L"00:00", RC.right - 265, 12, 55, 20, ID_LABEL_TIEMPOACTUAL, TRUE, WS_CHILD | WS_VISIBLE);
	LabelTiempoSeparador.CrearLabelEx(this, L"/", RC.right - 210, 12, 10, 20, ID_LABEL_TIEMPOSEPARADOR, TRUE, WS_CHILD | WS_VISIBLE);
	LabelTiempoTotal.CrearLabelEx(this, L"00:00", RC.right - 200, 12, 55, 20, ID_LABEL_TIEMPOTOTAL, TRUE, WS_CHILD | WS_VISIBLE);
}


void ControlesPantallaCompleta::Mostrar(void) {
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
	SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Tabla_Opciones.OcultarMouseEnVideo(), NULL);


//	if (_Visible == TRUE) return;
//	_Visible = TRUE;
	RECT RC;
	GetWindowRect(App.VentanaRave.hWnd(), &RC);
	RC.right = RC.right - 200;

/*	WINDOWPLACEMENT WP;
	WP.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(App.VentanaRave.hWnd(), &WP);*/

	int Ancho = abs(RC.right - (RC.left));
	MoveWindow(hWnd(), RC.left + 100, RC.bottom - 80, Ancho, 80, TRUE);

	MoveWindow(SliderTiempo.hWnd(),				10, 45, Ancho - 20, 24, TRUE);
	MoveWindow(SliderVolumen.hWnd(),			Ancho - 145, 13, 90, 16, TRUE);
	MoveWindow(LabelVolumen.hWnd(),				Ancho - 50, 12, 40, 20, TRUE);
	MoveWindow(LabelTiempoActual.hWnd(),		Ancho - 265, 12, 55, 20, TRUE);
	MoveWindow(LabelTiempoSeparador.hWnd(),		Ancho - 210, 12, 10, 20, TRUE);
	MoveWindow(LabelTiempoTotal.hWnd(),			Ancho - 200, 12, 55, 20, TRUE);

	Visible(TRUE);

	DMouse::ObtenerPosicion(&App.VentanaRave.MousePos);
	DMouse::Visible(TRUE);
	Debug_Escribir(L"ControlesPantallaCompleta::Mostrar\n");
//	DMouse::Visible(TRUE);
}

void ControlesPantallaCompleta::Ocultar(void) {
	Debug_Escribir(L"ControlesPantallaCompleta::Ocultar\n");
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
	DMouse::ObtenerPosicion(&App.VentanaRave.MousePos);
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
	App.BD.Tabla_Opciones.Volumen(static_cast<int>(SliderVolumen.Valor()));
	KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
	SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Tabla_Opciones.OcultarMouseEnVideo(), NULL);
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


void ControlesPantallaCompleta::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}


LRESULT CALLBACK ControlesPantallaCompleta::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static POINT nPos = { 0, 0 };
	switch (uMsg) {
		case WM_ERASEBKGND:
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;


		case WM_MOUSEMOVE :
			DWL::DMouse::ObtenerPosicion(&nPos);
			if (App.VentanaRave.MousePos.x != nPos.x || App.VentanaRave.MousePos.y != nPos.y) {
				KillTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD);
				SetTimer(App.VentanaRave.hWnd(), TIMER_CPC_INACTIVIDAD, App.BD.Tabla_Opciones.OcultarMouseEnVideo(), NULL);
				App.VentanaRave.MousePos = nPos;
				Mostrar();
			}
			break;
/*		case WM_TIMER :
			if (wParam == TIMER_CPC_INACTIVIDAD) 
				Ocultar();
			break;*/

		case WM_COMMAND :
			if (HIWORD(wParam) == BN_CLICKED) {
				App.VentanaRave.Evento_Button_Mouse_Click(LOWORD(wParam));
				return 0;
			}
			break;

		case DWL_BARRAEX_CAMBIO :
			switch (wParam) {
				case ID_SLIDER_TIEMPO: 					Evento_SliderTiempo_Cambio();					break;
				case ID_SLIDER_VOLUMEN: 				Evento_SliderVolumen_Cambio();					break;
			}
			break;

		case DWL_BARRAEX_CAMBIADO :
			switch (wParam) {
				case ID_SLIDER_TIEMPO: 					Evento_SliderTiempo_Cambiado();					break;
				case ID_SLIDER_VOLUMEN: 				Evento_SliderVolumen_Cambiado();				break;
			}
			break;

	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}