#include "stdafx.h"
#include "DBarraDesplazamientoEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBarraDesplazamientoEx::DBarraDesplazamientoEx(void) : DBarraProgresoEx(), _Estado(DBarraDesplazamientoEx_Estado_Normal), _MostrarToolTip(DBarraDesplazamientoEx_ToolTip_SinToolTip) {
	}

	DBarraDesplazamientoEx::~DBarraDesplazamientoEx(void) {
	}


	HWND DBarraDesplazamientoEx::CrearBarraDesplazamientoEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo, const float nMaximo, const float nValor) {
		if (hWnd()) { Debug_Escribir(L"DBarraDesplazamientoEx::CrearBarraEx() Error : ya se ha creado la barra\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DBarraDesplazamientoEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL, CS_HREDRAW | CS_VREDRAW);
		_Minimo = nMinimo;
		_Maximo = nMaximo;
		_Valor = nValor;
		_ToolTip.CrearToolTipEx();
		_ColorBarra = COLOR_BARRA;
		_ColorBarraResaltado = COLOR_BARRA_RESALTADO;
		_ColorBarraPresionado = COLOR_BARRA_PRESIONADO;
		_ColorFondo = COLOR_BARRA_FONDO;
		_ColorFondoResaltado = COLOR_BARRA_FONDO_RESALTADO;
		_ColorFondoPresionado = COLOR_BARRA_FONDO_PRESIONADO;

		return hWnd();
	}

	void DBarraDesplazamientoEx::ToolTip(DBarraDesplazamientoEx_ToolTip nValor) {
		_MostrarToolTip = nValor;
	}

	void DBarraDesplazamientoEx::OcultarToolTip(void) {
		_ToolTip.Visible(FALSE);
	}

	void DBarraDesplazamientoEx::Evento_PintarBarra(HDC DC, RECT &RBarra) {
		COLORREF Color = 0;
		switch (_Estado) {
			case DBarraDesplazamientoEx_Estado_Normal		: Color = _ColorBarra;				break;
			case DBarraDesplazamientoEx_Estado_Resaltado	: Color = _ColorBarraResaltado;		break;
			case DBarraDesplazamientoEx_Estado_Presionado	: Color = _ColorBarraPresionado;	break;
		}
		HBRUSH BrochaBarra = CreateSolidBrush(Color);
		FillRect(DC, &RBarra, BrochaBarra);
		DeleteObject(BrochaBarra);
	}

	void DBarraDesplazamientoEx::Evento_PintarFondo(HDC DC, RECT &RFondo) {
		COLORREF Color = 0;
		switch (_Estado) {
			case DBarraDesplazamientoEx_Estado_Normal		: Color = _ColorFondo;				break;
			case DBarraDesplazamientoEx_Estado_Resaltado	: Color = _ColorFondoResaltado;		break;
			case DBarraDesplazamientoEx_Estado_Presionado	: Color = _ColorFondoPresionado;	break;
		}
		HBRUSH BrochaFondo = CreateSolidBrush(Color);
		FillRect(DC, &RFondo, BrochaFondo);
		DeleteObject(BrochaFondo);
	}

	void DBarraDesplazamientoEx::Evento_PintarBorde(HDC DC, RECT &RBorde) {
		COLORREF Color = 0;
		switch (_Estado) {
			case DBarraDesplazamientoEx_Estado_Normal		: Color = _ColorBorde;				break;
			case DBarraDesplazamientoEx_Estado_Resaltado	: Color = _ColorBordeResaltado;		break;
			case DBarraDesplazamientoEx_Estado_Presionado	: Color = _ColorBordePresionado;	break;
		}
		HBRUSH BrochaBorde = CreateSolidBrush(Color);
		FrameRect(DC, &RBorde, BrochaBorde);
		DeleteObject(BrochaBorde);
	}

	void DBarraDesplazamientoEx::Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam) {
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);

		float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
		float valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;

		RECT RW = { 0, 0, 0, 0 };
		GetWindowRect(hWnd(), &RW);

		std::wstring TextoToolTip;
		Evento_MostrarToolTip(valor, TextoToolTip);

		switch (_MostrarToolTip) {
			case DBarraDesplazamientoEx_ToolTip_Superior:		_ToolTip.MostrarToolTipEx(RW.left + cX, RW.top - 35, TextoToolTip);		break;
			case DBarraDesplazamientoEx_ToolTip_Inferior:		_ToolTip.MostrarToolTipEx(RW.left + cX, RW.bottom + 10, TextoToolTip);	break;
		}
		if (_Estado == DBarraDesplazamientoEx_Estado_Presionado) {
			float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
			_Valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;
			if (_Valor > _Maximo) { _Valor = _Maximo; }
			if (_Valor < _Minimo) { _Valor = _Minimo; }
			Repintar();
			SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIO, (WPARAM)GetWindowLongPtr(hWnd(), GWL_ID), 0);
		}
	}

	void DBarraDesplazamientoEx::Evento_MostrarToolTip(float nValor, std::wstring &Texto) {
		Texto = std::to_wstring(nValor);
	}

	void DBarraDesplazamientoEx::Evento_MousePresionado(const int cX, const int cY, const UINT wParam) {
		POINT Pt = { cX, cY };
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);
		if (PtInRect(&RC, Pt) == TRUE) {
			SetCapture(hWnd());
			_Estado = DBarraDesplazamientoEx_Estado_Presionado;
			float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
			_Valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;
			Repintar();
			SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIO, (WPARAM)GetWindowLongPtr(hWnd(), GWL_ID), 0);
		}
	}

	void DBarraDesplazamientoEx::Evento_MouseSoltado(const int cX, const int cY, const UINT wParam) {
		POINT Pt = { cX, cY };
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);
		ReleaseCapture();
		if (PtInRect(&RC, Pt) == TRUE) { _Estado = DBarraDesplazamientoEx_Estado_Resaltado; }
		else { _Estado = DBarraDesplazamientoEx_Estado_Normal; }

		float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
		_Valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;
		if (_Valor > _Maximo) { _Valor = _Maximo; }
		if (_Valor < _Minimo) { _Valor = _Minimo; }
		SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIADO, (WPARAM)GetWindowLongPtr(hWnd(), GWL_ID), 0);
		Repintar();
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
				if (_MouseEntrando() == TRUE) {
					// Mouse enter
					if (_Estado != DBarraDesplazamientoEx_Estado_Presionado) {
						_Estado = DBarraDesplazamientoEx_Estado_Resaltado;
						Repintar();
					}
				}
				Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
			case WM_MOUSELEAVE:
				_ToolTip.OcultarToolTipEx();
				_MouseDentro = FALSE;
				if (_Estado != DBarraDesplazamientoEx_Estado_Presionado) {
					_Estado = DBarraDesplazamientoEx_Estado_Normal;
					Repintar();
				}
				break;
			case WM_LBUTTONDOWN:
				Evento_MousePresionado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
			case WM_LBUTTONUP:
				Evento_MouseSoltado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}

}