#include "stdafx.h"
#include "DBotonEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBotonEx::DBotonEx() : DControlEx_TextoDinamico(), _Estado(DBotonEx_Estado_Normal) {
	}


	DBotonEx::~DBotonEx() {
	}

	HWND DBotonEx::CrearBotonEx(DhWnd &nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		if (hWnd()) { Debug_Escribir(L"DBotonEx::CrearBotonEx() Error : ya se ha creado el botón\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DBotonEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);
		_Fuente = hWnd._Fuente;
		_Texto = nTxt;

		return hWnd();
	}

	void DBotonEx::PintarBotonEx(HDC DC) {
		RECT    RC, RCF, RCT, RCS;
		GetClientRect(hWnd(), &RC);
		RCF = RC; RCF.left++; RCF.top++; RCF.right--; RCF.bottom--;
		RCS = RC; RCS.left++; RCS.top++; RCS.right++; RCS.bottom++;
		RCT = RC; 

		// Creo un buffer en memória para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));
		HFONT   vFuente = static_cast<HFONT>(SelectObject(Buffer, _Fuente));

		COLORREF ColorBorde = 0, ColorTexto = 0, ColorFondo = 0;
		switch (_Estado) {
			case DBotonEx_Estado_Normal:
				ColorFondo = _ColorFondo;
				ColorBorde = _ColorBorde;
				ColorTexto = _ColorTexto;
				break;
			case DBotonEx_Estado_Resaltado:
				ColorFondo = _ColorFondoResaltado;
				ColorBorde = _ColorBordeResaltado;
				ColorTexto = _ColorTextoResaltado;
				break;
			case DBotonEx_Estado_Presionado:
				ColorFondo = _ColorFondoPresionado;
				ColorBorde = _ColorBordePresionado;
				ColorTexto = _ColorTextoPresionado;
				RCT.top += 2;
				RCS.top += 2;
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

		SetBkMode(Buffer, TRANSPARENT);
		// Pinto la sombra del texto
		SetTextColor(Buffer, _ColorTextoSombra);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCS, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		// Pinto el texto
		SetTextColor(Buffer, ColorTexto);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCT, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, vFuente);
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