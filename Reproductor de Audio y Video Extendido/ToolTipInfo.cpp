#include "stdafx.h"
#include "ToolTipInfo.h"
#include <dwmapi.h>

#define TOOLTIPINFO_PADDING			   4
#define ID_TEMPORIZADOR_OCULTAR		1000
#define ID_TEMPORIZADOR_ANIMACION	1001
#define ID_ANIMACION_TTI            1002

//#define TIEMPO_ANIMACION             400

/*
  _______          _ _______ _      _____        __      
 |__   __|        | |__   __(_)    |_   _|      / _|     
    | | ___   ___ | |  | |   _ _ __  | |  _ __ | |_ ___  
    | |/ _ \ / _ \| |  | |  | | '_ \ | | | '_ \|  _/ _ \ 
    | | (_) | (_) | |  | |  | | |_) || |_| | | | || (_) |
    |_|\___/ \___/|_|  |_|  |_| .__/_____|_| |_|_| \___/ 
                              | |                        
                              |_|							 */

void ToolTipInfo::Mostrar(const int cX, const int cY, const int cAncho, const int cAlto, std::wstring &Str, ToolTipsInfo *nPadre, std::function<void(void)> CallbackOcultarTerminado) {
	_Str						= Str;
	_Padre						= nPadre;
	_CallbackOcultarTerminado	= CallbackOcultarTerminado;

	if (_hWnd == NULL) {
		_hWnd = DVentana::CrearVentana(NULL, L"RAVE_ToolTipInfo", L"", cX, cY, cAncho, cAlto, WS_POPUP | WS_CAPTION, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		MARGINS Margen = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(_hWnd, &Margen);
	}
	Opacidad(0);
	// Asigno la posición del menú y lo hago siempre visible
	SetWindowPos(_hWnd, _Padre->Padre(), cX, cY, cAncho, cAlto, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	// Situo el padre justo detras de este tooltip
	SetWindowPos(_Padre->Padre(), _hWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	SetTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR, App.BD.Opciones_TiempoToolTips(), NULL);

	Ani.Iniciar(0, 255, App.BD.Opciones_TiempoAnimaciones(), [=](std::vector<float> &Valores, const BOOL Terminado) {
		Opacidad(static_cast<BYTE>(Valores[0]));
	});
//	SetTimer(_hWnd, ID_TEMPORIZADOR_ANIMACION, 16, NULL); // 16 ms para llegar a 60fps (1000 / 16 = 62.5)
}


SIZE ToolTipInfo::CalcularTam(std::wstring &Str) {
	// Calculo el tamaño del texto (una sola línea de texto)
	HDC	   		hDC = CreateCompatibleDC(NULL);
	HBITMAP		Bmp = CreateCompatibleBitmap(hDC, 1, 1);
	HBITMAP		Viejo = static_cast<HBITMAP>(SelectObject(hDC, Bmp));
	HFONT       vFuente = static_cast<HFONT>(SelectObject(hDC, Fuente18Normal()));
	SIZE		Ret = { 0, 0 };

	RECT Tam = { 0, 0, 0, 0 };
	int Al = DrawText(hDC, Str.c_str(), static_cast<int>(Str.size()), &Tam, DT_CALCRECT);

	SelectObject(hDC, vFuente);
	SelectObject(hDC, Viejo);
	DeleteObject(Bmp);
	DeleteDC(hDC);

	Ret.cx = Tam.right + (TOOLTIPINFO_PADDING * 2);
	Ret.cy = Tam.bottom + (TOOLTIPINFO_PADDING * 2);
	return Ret;
}


void ToolTipInfo::Ocultar(const BOOL Rapido) {
	if (_Ocultando == TRUE && Rapido == FALSE) return;
	_Ocultando = TRUE;

	KillTimer(_hWnd, ID_TEMPORIZADOR_OCULTAR);
//	KillTimer(_hWnd, ID_TEMPORIZADOR_ANIMACION);
	if (Rapido == FALSE) {
		//AnimateWindow(_hWnd, 100, AW_HIDE | AW_BLEND);
		Ani.Terminar();
		Ani.Iniciar(255, 0, App.BD.Opciones_TiempoAnimaciones(), [=](std::vector<float> &Valores, const BOOL Terminado) {
			Opacidad(static_cast<BYTE>(Valores[0]));
			if (Terminado == TRUE) {
				ShowWindow(hWnd(), SW_HIDE);
				PostMessage(hWnd(), WM_CLOSE, 0, 0);
				_CallbackOcultarTerminado();
			}
		});
	}
	else {
		Ani.Terminar();
		ShowWindow(hWnd(), SW_HIDE);
		Destruir();
	}
}


void ToolTipInfo::Pintar(HDC DC) {
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


void ToolTipInfo::_Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(hWnd(), &PS);
	Pintar(DC);
	EndPaint(hWnd(), &PS);	
}

void ToolTipInfo::_Evento_Temporizador(INT_PTR tID) {
	switch (tID) {
		case ID_TEMPORIZADOR_OCULTAR :
			Ocultar();
			break;
	}
}

LRESULT CALLBACK ToolTipInfo::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_TIMER :
			_Evento_Temporizador(static_cast<INT_PTR>(wParam));
//			if (wParam == ID_TEMPORIZADOR_OCULTAR) Ocultar();
			return 0;

		case WM_PAINT		:	_Evento_Pintar();																															return 0;
		// Print y Print Client (para AnimateWindow)
		case WM_PRINTCLIENT	:    Pintar(reinterpret_cast<HDC>(wParam));																										return 0;
		// Mouse 
		case WM_MOUSEMOVE:		
		case WM_LBUTTONDOWN:	case WM_RBUTTONDOWN:	case WM_MBUTTONDOWN:// Mouse presionado
		case WM_LBUTTONUP:		case WM_RBUTTONUP:		case WM_MBUTTONUP:	// Mouse soltado
			Ocultar();
			return 0;
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
	if (_Padre->hWnd() == NULL)	return;
	if (IsWindowVisible(_Padre->hWnd()) == FALSE) return;
	
	// Creo el tooltip y calculo su tamaño
	ToolTipInfo *TT = new ToolTipInfo();
	RECT RV;
	GetWindowRect(_Padre->hWnd(), &RV);
	SIZE Tam = TT->CalcularTam(Texto);

	// Elimino tooltips que ya se han ocultado
	for (long t = static_cast<long>(_ToolTips.size()) - 1; t > -1; t--) {
		if (_ToolTips[t]->hWnd() == NULL) {
			_ToolTips[t]->Ani.Terminar();
			delete _ToolTips[t];
			_ToolTips.erase(_ToolTips.begin() + t);
		}
	}

	// Muevo los otros tooltips creados anteriormente hacia arriba
	RECT Tmp;
	for (size_t i = 0; i < _ToolTips.size(); i++) {
		GetWindowRect(_ToolTips[i]->hWnd(), &Tmp);
		// Si el tooltip sobresale por encima de la parte superior de la ventana padre, lo oculto
		if (RV.top > Tmp.top - (Tam.cy + TOOLTIPINFO_PADDING)) {
			_ToolTips[i]->Ocultar(TRUE);
		}
		else {
			SetWindowPos(_ToolTips[i]->hWnd(), NULL, Tmp.left, Tmp.top - (Tam.cy + TOOLTIPINFO_PADDING), 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE);
		}
	}

	// Muestro el nuevo tooltip alineado a la parte inferior izquierda
	if (RV.right - RV.left > Tam.cx) {
		TT->Mostrar(RV.right - (Tam.cx + 20), RV.bottom - (Tam.cy + 20), Tam.cx, Tam.cy, Texto, this, [=]{ RecolocarToolTips();	});
	}
	// Muestro el nuevo tooltip centrado en la parte inferior
	else {
		int x = (Tam.cx - (RV.right - RV.left)) / 2;
		TT->Mostrar(RV.left - x, RV.bottom - (Tam.cy + 20), Tam.cx, Tam.cy, Texto, this, [=] { RecolocarToolTips(); });
	}

	_ToolTips.push_back(TT);
}


void ToolTipsInfo::RecolocarToolTips(void) {
	RECT RT, RV;
	GetWindowRect(_Padre->hWnd(), &RV);
	long Y = RV.bottom - 16;
	for (long i = static_cast<long>(_ToolTips.size()) - 1; i > -1; i--) {
		if (_ToolTips[i]->hWnd() != NULL) {
			if (_ToolTips[i]->Visible() == TRUE) {
				GetWindowRect(_ToolTips[i]->hWnd(), &RT);
				Y -= (RT.bottom - RT.top) + TOOLTIPINFO_PADDING;
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