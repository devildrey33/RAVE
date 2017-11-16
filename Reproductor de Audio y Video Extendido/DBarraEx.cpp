#include "stdafx.h"
#include "DBarraEx.h"
#include "RAVE_Colores.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBarraEx::DBarraEx(void) : _Minimo(0), _Maximo(0), _Valor(0), _Estado(DBarraEx_Estado_Normal), _MostrarToolTip(DBarraEx_ToolTip_SinToolTip) {

	}


	DBarraEx::~DBarraEx(void) {
	}


	HWND DBarraEx::CrearBarraEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo, const float nMaximo, const float nValor) {
		if (hWnd()) { Debug_Escribir(L"DBarraEx::CrearBarraEx() Error : ya se ha creado la barra\n"); return hWnd(); }
		hWnd			= CrearControlEx(nPadre, L"DBarraEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
		_Minimo		    = nMinimo;
		_Maximo			= nMaximo;
		_Valor			= nValor;
		_ToolTip.CrearToolTipEx();

		return hWnd();
	}

	void DBarraEx::Minimo(const float nMinimo) {
		_Minimo = nMinimo;
		Repintar();
	}

	void DBarraEx::Maximo(const float nMaximo) {
		_Maximo = nMaximo;
		Repintar();
	}

	void DBarraEx::Valor(const float nValor) {
		_Valor = nValor;
		Repintar();
	}

	void DBarraEx::ToolTip(DBarraEx_ToolTip nValor) {
		_MostrarToolTip = nValor;
	}

	void DBarraEx::OcultarToolTip(void) {
		_ToolTip.hWnd.Visible(FALSE);
	}

	void DBarraEx::PintarBarraEx(HDC DC) {
		RECT    RC, RBarra, RFondo;
		GetClientRect(hWnd(), &RC);
		// Creo un buffer en memória para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		// Calculo el tamaño de la barra para el valor actual
		RBarra = RC; RFondo = RC;
		RBarra.left++; RBarra.top++; RBarra.bottom--;  // Evito los bordes
		RFondo.top++; RFondo.bottom--; RFondo.right--; // Evito los bordes
		float Parte = static_cast<float>((RC.right - RC.left) - 2) / (_Maximo - _Minimo); 
		RBarra.right = 1 + static_cast<int>(Parte * (_Valor - _Minimo));
		RFondo.left = RBarra.right;

		// Pinto el borde
		Evento_PintarBorde(Buffer, RC);

		// Pinto la barra
		Evento_PintarBarra(Buffer, RBarra);

		// Pinto el fondo
		Evento_PintarFondo(Buffer, RFondo);



		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}


	void DBarraEx::Evento_PintarBarra(HDC DC, RECT &RBarra) {
		COLORREF Color = 0;
		switch (_Estado) {
			case DBarraEx_Estado_Normal     : Color = COLOR_BARRA;				break;
			case DBarraEx_Estado_Resaltado  : Color = COLOR_BARRA_RESALTADO;	break;
			case DBarraEx_Estado_Presionado : Color = COLOR_BARRA_PRESIONADO;	break;
		}
		HBRUSH BrochaBarra = CreateSolidBrush(Color);
		FillRect(DC, &RBarra, BrochaBarra);
		DeleteObject(BrochaBarra);
	}

	void DBarraEx::Evento_PintarFondo(HDC DC, RECT &RFondo) {
		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_BARRA_FONDO);
		FillRect(DC, &RFondo, BrochaFondo);
		DeleteObject(BrochaFondo);
	}

	void DBarraEx::Evento_PintarBorde(HDC DC, RECT &RBorde) {
		COLORREF Color = 0;
		switch (_Estado) {
			case DBarraEx_Estado_Normal     : Color = COLOR_BORDE;				break;
			case DBarraEx_Estado_Resaltado  : Color = COLOR_BORDE_RESALTADO;	break;
			case DBarraEx_Estado_Presionado : Color = COLOR_BORDE_PRESIONADO;	break;
		}
		HBRUSH BrochaBorde = CreateSolidBrush(Color);
		FrameRect(DC, &RBorde, BrochaBorde);
		DeleteObject(BrochaBorde);
	}

	void DBarraEx::Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam) {
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);

		float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
		float valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;

		RECT RW = { 0, 0, 0, 0 };
		GetWindowRect(hWnd(), &RW);

		std::wstring TextoToolTip;		
		Evento_MostrarToolTip(valor, TextoToolTip);

		switch (_MostrarToolTip) {
			case DBarraEx_ToolTip_Superior:		_ToolTip.MostrarToolTipEx(RW.left + cX, RW.top - 35, TextoToolTip);		break;
			case DBarraEx_ToolTip_Inferior:		_ToolTip.MostrarToolTipEx(RW.left + cX, RW.bottom + 10, TextoToolTip);	break;
		}
		if (_Estado == DBarraEx_Estado_Presionado) {
			float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
			_Valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;
			if (_Valor > _Maximo) { _Valor = _Maximo; }
			if (_Valor < _Minimo) { _Valor = _Minimo; }
			Repintar();
			SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIO, (WPARAM)GetWindowLongPtr(hWnd(), GWL_ID), 0);
		}
	}

	void DBarraEx::Evento_MostrarToolTip(float nValor, std::wstring &Texto) {
		Texto = std::to_wstring(nValor);
	}

	void DBarraEx::Evento_MousePresionado(const int cX, const int cY, const UINT wParam) {
		POINT Pt = { cX, cY };
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);
		if (PtInRect(&RC, Pt) == TRUE) {
			SetCapture(hWnd());
			_Estado = DBarraEx_Estado_Presionado;
			float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
			_Valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;
			Repintar();
			SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIO, (WPARAM)GetWindowLongPtr(hWnd(), GWL_ID), 0);
		}
	}

	void DBarraEx::Evento_MouseSoltado(const int cX, const int cY, const UINT wParam) {
		POINT Pt = { cX, cY };
		RECT  RC = { 0, 0, 0, 0 };
		GetClientRect(hWnd(), &RC);
		ReleaseCapture();
		if (PtInRect(&RC, Pt) == TRUE)	{	_Estado = DBarraEx_Estado_Resaltado;		}
		else							{	_Estado = DBarraEx_Estado_Normal;   		}

		float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
		_Valor = (static_cast<float>(cX - RC.left) * Parte) - _Minimo;
		if (_Valor > _Maximo) { _Valor = _Maximo; }
		if (_Valor < _Minimo) { _Valor = _Minimo; }
		SendMessage(GetParent(hWnd()), DWL_BARRAEX_CAMBIADO, (WPARAM)GetWindowLongPtr(hWnd(), GWL_ID), 0);
		Repintar();
	}

	LRESULT CALLBACK DBarraEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_ERASEBKGND :
				PintarBarraEx(reinterpret_cast<HDC>(wParam));
				return TRUE;
			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				PintarBarraEx(DC);
				EndPaint(hWnd(), &PS);
				return 0;
			case WM_MOUSEMOVE :
				if (_MouseEntrando() == TRUE) {
					// Mouse enter
					if (_Estado != DBarraEx_Estado_Presionado) {
						_Estado = DBarraEx_Estado_Resaltado;
						Repintar();
					}
				}
				Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
			case WM_MOUSELEAVE :
				_ToolTip.OcultarToolTipEx();
				_MouseDentro = FALSE;
				if (_Estado != DBarraEx_Estado_Presionado) {
					_Estado = DBarraEx_Estado_Normal;
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