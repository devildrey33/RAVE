#include "stdafx.h"
#include "DBotonEx.h"
#include "RAVE_Colores.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBotonEx::DBotonEx() : DControlEx(), _Estado(DBotonEx_Estado_Normal) {
	}


	DBotonEx::~DBotonEx() {
	}

	HWND DBotonEx::CreadBotonEx(DhWnd &nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		if (hWnd()) { Debug_Escribir(L"DBotonEx::CreadBotonEx() Error : ya se ha creado el botón\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DBotonEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);

		_Texto = nTxt;

		return hWnd();
	}

	void DBotonEx::PintarBotonEx(HDC DC) {
		RECT    RC, RCF, RCT;
		GetClientRect(hWnd(), &RC);
		RCF = RC; RCF.left++; RCF.top++; RCF.right--; RCF.bottom--;
		RCT = RC; 

		// Creo un buffer en memória para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		COLORREF ColorBorde = 0, ColorTexto = 0, ColorFondo = 0;
		switch (_Estado) {
			case DBotonEx_Estado_Normal:
				ColorFondo = COLOR_BOTON;
				ColorBorde = COLOR_BORDE;
				ColorTexto = COLOR_BOTON_TEXTO;
				break;
			case DBotonEx_Estado_Resaltado:
				ColorFondo = COLOR_BOTON_RESALTADO;
				ColorBorde = COLOR_BORDE_RESALTADO;
				ColorTexto = COLOR_BOTON_TEXTO_RESALTADO;
				break;
			case DBotonEx_Estado_Presionado:
				ColorFondo = COLOR_BOTON_PRESIONADO;
				ColorBorde = COLOR_BORDE_PRESIONADO;
				ColorTexto = COLOR_BOTON_TEXTO_PRESIONADO;
				RCT.top += 2;
				break;
		}
		// Pinto el borde
		HBRUSH BrochaBorde = CreateSolidBrush(ColorBorde);
		FrameRect(Buffer, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		// Pinto el fondo
		HBRUSH BrochaFondo = CreateSolidBrush(ColorFondo);
		FillRect(Buffer, &RCF, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Pinto el texto
		SetBkMode(Buffer, TRANSPARENT);
		SetTextColor(Buffer, ColorTexto);
		HFONT vFuente = static_cast<HFONT>(SelectObject(Buffer, hWnd._Fuente));
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCT, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		SelectObject(Buffer, vFuente);

		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	
	void DBotonEx::Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam) {
	}

	void DBotonEx::Evento_MousePresionado(const int cX, const int cY, const UINT wParam) {
		SetCapture(hWnd());
		_Estado = DBotonEx_Estado_Presionado;
		Repintar();
	}

	void DBotonEx::Evento_MouseSoltado(const int cX, const int cY, const UINT wParam) {
		if (_Estado == DBotonEx_Estado_Presionado) {
			ReleaseCapture();

			RECT RC;
			GetClientRect(hWnd(), &RC);

			POINT Pt = { cX, cY };
			if (PtInRect(&RC, Pt) != 0) {
				_Estado = DBotonEx_Estado_Resaltado;
				PostMessage(GetParent(hWnd()), DWL_BOTONEX_CLICK, static_cast<WPARAM>(GetWindowLongPtr(hWnd(), GWL_ID)), MAKELPARAM(cX, cY));
			}
			else {
				_Estado = DBotonEx_Estado_Normal;
			}
			Repintar();

			
		}
	}

	LRESULT CALLBACK DBotonEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				PintarBotonEx(DC);
				EndPaint(hWnd(), &PS);
				return 0;
			case WM_MOUSEMOVE:
				if (_MouseEntrando() == TRUE) {
					// Mouse enter
					if (_Estado != DBotonEx_Estado_Presionado) {
						_Estado = DBotonEx_Estado_Resaltado;
						Repintar();
					}
				}
				Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
			case WM_MOUSELEAVE:
				_MouseDentro = FALSE;
				if (_Estado != DBotonEx_Estado_Presionado) {
					_Estado = DBotonEx_Estado_Normal;
					Repintar();
				}
				break;
			case WM_LBUTTONDOWN:
				Evento_MousePresionado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
			case WM_LBUTTONUP:
				Evento_MouseSoltado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
				return DefWindowProc(hWnd(), uMsg, wParam, lParam);
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}
}