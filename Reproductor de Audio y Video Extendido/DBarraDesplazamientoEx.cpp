#include "stdafx.h"
#include "DBarraDesplazamientoEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBarraDesplazamientoEx::DBarraDesplazamientoEx(void) :	DBarraProgresoEx(), _MostrarToolTip(DBarraDesplazamientoEx_ToolTip_SinToolTip) {
	}

	DBarraDesplazamientoEx::~DBarraDesplazamientoEx(void) {
	}


	HWND DBarraDesplazamientoEx::CrearBarraDesplazamientoEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo, const float nMaximo, const float nValor) {
//		if (hWnd()) { Debug_Escribir(L"DBarraDesplazamientoEx::CrearBarraEx() Error : ya se ha creado la barra\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DBarraDesplazamientoEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL, CS_HREDRAW | CS_VREDRAW);
		_Minimo = nMinimo;
		_Maximo = nMaximo;
		_Valor = nValor;
		_ColorBarra = COLOR_BARRA;
		_ColorFondo = COLOR_BARRA_FONDO;
		_ColorBorde = COLOR_BORDE;

		_ToolTip.CrearToolTipEx(Fuente18Normal, nPadre);
		return hWnd();
	}

	void DBarraDesplazamientoEx::ToolTip(DBarraDesplazamientoEx_ToolTip nValor) {
		_MostrarToolTip = nValor;
	}

	void DBarraDesplazamientoEx::OcultarToolTip(void) {
		_ToolTip.Visible(FALSE);
	}

	void DBarraDesplazamientoEx::Evento_PintarBarra(HDC DC, RECT &RBarra) {
		COLORREF Color = (_Estado == DBarraEx_Estado_Presionado) ? COLOR_BARRA_PRESIONADO : _ColorBarra;
//		switch (_Estado) {
//			case DBarraDesplazamientoEx_Estado_Normal		: Color = COLOR_BARRA;				break;
//			case DBarraDesplazamientoEx_Estado_Resaltado	: Color = COLOR_BARRA_RESALTADO;	break;
//			case DBarraDesplazamientoEx_Estado_Presionado	: Color = COLOR_BARRA_PRESIONADO;	break;
//		}
		HBRUSH BrochaBarra = CreateSolidBrush(Color);
		FillRect(DC, &RBarra, BrochaBarra);
		DeleteObject(BrochaBarra);
	}

	void DBarraDesplazamientoEx::Evento_PintarFondo(HDC DC, RECT &RFondo) {
		COLORREF Color = (_Estado == DBarraEx_Estado_Presionado) ? COLOR_BARRA_FONDO_PRESIONADO : _ColorFondo;
//		switch (_Estado) {
//			case DBarraDesplazamientoEx_Estado_Normal		: Color = COLOR_BARRA_FONDO;				break;
//			case DBarraDesplazamientoEx_Estado_Resaltado	: Color = COLOR_BARRA_FONDO_RESALTADO;		break;
//			case DBarraDesplazamientoEx_Estado_Presionado	: Color = COLOR_BARRA_FONDO_PRESIONADO;		break;
//		}
		HBRUSH BrochaFondo = CreateSolidBrush(Color);
		FillRect(DC, &RFondo, BrochaFondo);
		DeleteObject(BrochaFondo);
	}

	void DBarraDesplazamientoEx::Evento_PintarBorde(HDC DC, RECT &RBorde) {
		COLORREF Color = (_Estado == DBarraEx_Estado_Presionado) ? COLOR_ROJO_PRESIONADO : _ColorBorde;
//		switch (_Estado) {
//			case DBarraDesplazamientoEx_Estado_Normal		: Color = COLOR_BORDE;				break;
//			case DBarraDesplazamientoEx_Estado_Resaltado	: Color = COLOR_ROJO;				break;
//			case DBarraDesplazamientoEx_Estado_Presionado	: Color = COLOR_ROJO_PRESIONADO;	break;
//		}
		HBRUSH BrochaBorde = CreateSolidBrush(Color);
		FrameRect(DC, &RBorde, BrochaBorde);
		DeleteObject(BrochaBorde);
	}


	void DBarraDesplazamientoEx::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {

		if (_MouseEntrando() == TRUE) {
			// Mouse enter
			if (_Estado != DBarraEx_Estado_Presionado) {
				//_Estado = DBarraDesplazamientoEx_Estado_Resaltado;
				//Repintar();
				Resaltar(TRUE);
			}
		}

		DEventoMouse DatosMouse(wParam, lParam, this);
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);
		int cX = DatosMouse.X(); //  , cY = DatosMouse.Y();
		float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
		float valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;

		RECT RW = { 0, 0, 0, 0 };
		GetWindowRect(hWnd(), &RW);

		std::wstring TextoToolTip;
		Evento_MostrarToolTip(valor, TextoToolTip);

		switch (_MostrarToolTip) {
			case DBarraDesplazamientoEx_ToolTip_Superior:		_ToolTip.Mostrar(RW.left + cX, RW.top - 35, TextoToolTip);		break;
			case DBarraDesplazamientoEx_ToolTip_Inferior:		_ToolTip.Mostrar(RW.left + cX, RW.bottom + 10, TextoToolTip);	break;
		}
		if (_Estado == DBarraEx_Estado_Presionado) {
			float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
			_Valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;
			if (_Valor > _Maximo) { _Valor = _Maximo; }
			if (_Valor < _Minimo) { _Valor = _Minimo; }
			Repintar();
			SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIO, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
			Evento_MouseMovimiento(DatosMouse);
		}
	}

	void DBarraDesplazamientoEx::Evento_MostrarToolTip(float nValor, std::wstring &Texto) {
		Texto = std::to_wstring(nValor);
	}

	void DBarraDesplazamientoEx::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, this, Boton);
		POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);
		if (PtInRect(&RC, Pt) == TRUE) {
			SetCapture(hWnd());
			_Estado = DBarraEx_Estado_Presionado;
			float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
			_Valor = (static_cast<float>(Pt.x - RC.left) * Parte) - _Minimo;
			Repintar();
			SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIO, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
			Evento_MousePresionado(DatosMouse);
		}
	}

	void DBarraDesplazamientoEx::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, this, Boton);
		POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);
		ReleaseCapture();
		if (PtInRect(&RC, Pt) == TRUE) { _Estado = DBarraEx_Estado_Resaltado; }
		else                           { _Estado = DBarraEx_Estado_Normal; }

		float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
		_Valor = (static_cast<float>(Pt.x - RC.left) * Parte) - _Minimo;
		if (_Valor > _Maximo) { _Valor = _Maximo; }
		if (_Valor < _Minimo) { _Valor = _Minimo; }
		SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIADO, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
		Repintar();
	}

	void DBarraDesplazamientoEx::_Evento_MouseSaliendo(void) {
		_ToolTip.Ocultar();
		_MouseDentro = FALSE;
		if (_Estado != DBarraEx_Estado_Presionado) {
			//_Estado = DBarraDesplazamientoEx_Estado_Normal;
			//Repintar();
			Resaltar(FALSE);
		}
	}


	void DBarraDesplazamientoEx::Resaltar(const BOOL Resaltado) {
		if (_AniResaltado.Animando() == TRUE) {
			_AniResaltado.Invertir();
			return;
		}

		COLORREF BordeDesde, BordeHasta, FondoDesde, FondoHasta, BarraDesde, BarraHasta;
		if (Resaltado == TRUE) {
			BordeDesde = COLOR_BORDE;
			BordeHasta = COLOR_BORDE_RESALTADO;
			FondoDesde = COLOR_BARRA_FONDO;
			FondoHasta = COLOR_BARRA_FONDO_RESALTADO;
			BarraDesde = COLOR_BARRA;
			BarraHasta = COLOR_BARRA_RESALTADO;
			_Estado = DBarraEx_Estado_Resaltado;
		}
		else {
			BordeDesde = COLOR_BORDE_RESALTADO;
			BordeHasta = COLOR_BORDE;
			FondoDesde = COLOR_BARRA_FONDO_RESALTADO;
			FondoHasta = COLOR_BARRA_FONDO;
			BarraDesde = COLOR_BARRA_RESALTADO;
			BarraHasta = COLOR_BARRA;
			_Estado = DBarraEx_Estado_Normal;
		}
		_AniResaltado.Iniciar(FondoDesde, FondoHasta, BordeDesde, BordeHasta, BarraDesde, BarraHasta, 400, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
			_ColorFondo = Valores[0];
			_ColorBorde = Valores[1];
			_ColorBarra = Valores[2];
			Repintar();
		});

	}

	LRESULT CALLBACK DBarraDesplazamientoEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_ERASEBKGND:
				PintarBarraEx(reinterpret_cast<HDC>(wParam));
				return TRUE;
			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				PintarBarraEx(DC);
				EndPaint(hWnd(), &PS);
				return 0;
			case WM_MOUSEMOVE:
				_Evento_MouseMovimiento(wParam, lParam);
				return 0;
			case WM_MOUSELEAVE:
				_Evento_MouseSaliendo();
				return 0;
			case WM_LBUTTONDOWN:
				_Evento_MousePresionado(0, wParam, lParam);
				return 0;
			case WM_RBUTTONDOWN:
				_Evento_MousePresionado(1, wParam, lParam);
				return 0;
			case WM_MBUTTONDOWN:
				_Evento_MousePresionado(2, wParam, lParam);
				return 0;
			case WM_LBUTTONUP:
				_Evento_MouseSoltado(0, wParam, lParam);
				return 0;
			case WM_RBUTTONUP:
				_Evento_MouseSoltado(1, wParam, lParam);
				return 0;
			case WM_MBUTTONUP:
				_Evento_MouseSoltado(2, wParam, lParam);
				return 0;
		}
		return DControlEx::GestorMensajes(uMsg, wParam, lParam);
	}

}