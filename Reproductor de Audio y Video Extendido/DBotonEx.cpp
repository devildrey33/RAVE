#include "stdafx.h"
#include "DBotonEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBotonEx::DBotonEx() :	ColorTexto(COLOR_TEXTO), ColorTextoResaltado(COLOR_TEXTO_RESALTADO), ColorTextoPresionado(COLOR_TEXTO_PRESIONADO), ColorTextoSombra(COLOR_TEXTO_SOMBRA), _Estado(DBotonEx_Estado_Normal),
							ColorBorde(COLOR_BORDE), ColorBordeResaltado(COLOR_BORDE_RESALTADO), ColorBordePresionado(COLOR_BORDE_PRESIONADO), ColorTextoDesactivado(COLOR_TEXTO_DESACTIVADO),
							ColorFondo(COLOR_BOTON), ColorFondoResaltado(COLOR_BOTON_RESALTADO), ColorFondoPresionado(COLOR_BOTON_PRESIONADO) {
	}


	DBotonEx::~DBotonEx() {
	} 

	HWND DBotonEx::CrearBotonEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		if (hWnd()) { Debug_Escribir(L"DBotonEx::CrearBotonEx() Error : ya se ha creado el botón\n"); return hWnd(); }
		_hWnd			= CrearControlEx(nPadre, L"DBotonEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);
		_Texto			= nTxt;
		Fuente			= _Fuente21Negrita;
		_Estado			= DBotonEx_Estado_Normal;
		_MouseDentro	= FALSE;
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
		HFONT   vFuente = static_cast<HFONT>(SelectObject(Buffer, Fuente()));

		COLORREF nColorBorde = 0, nColorTexto = 0, nColorFondo = 0;
		switch (_Estado) {
			case DBotonEx_Estado_Normal:
				nColorFondo = ColorFondo;
				nColorBorde = ColorBorde;
				nColorTexto = ColorTexto;
				break;
			case DBotonEx_Estado_Resaltado:
				nColorFondo = ColorFondoResaltado;
				nColorBorde = ColorBordeResaltado;
				nColorTexto = ColorTextoResaltado;
				break;
			case DBotonEx_Estado_Presionado:
				nColorFondo = ColorFondoPresionado;
				nColorBorde = ColorBordePresionado;
				nColorTexto = ColorTextoPresionado;
				RCT.top += 2;
				RCS.top += 2;
				break;
		}
		// Si el control no está activado modifico el color final del texto
		if (Activado() == FALSE) {
			nColorTexto = ColorTextoDesactivado;
		}

		// Pinto el borde
		HBRUSH BrochaBorde = CreateSolidBrush(nColorBorde);
		FrameRect(Buffer, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		// Pinto el fondo
		HBRUSH BrochaFondo = CreateSolidBrush(nColorFondo);
		FillRect(Buffer, &RCF, BrochaFondo);
		DeleteObject(BrochaFondo);

		SetBkMode(Buffer, TRANSPARENT);
		// Pinto la sombra del texto
		SetTextColor(Buffer, ColorTextoSombra);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCS, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		// Pinto el texto
		SetTextColor(Buffer, nColorTexto);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCT, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, vFuente);
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	const BOOL DBotonEx::Activar(const BOOL nActivar) {
		BOOL Ret = FALSE;
		Ret = EnableWindow(_hWnd, nActivar);
		Repintar();
		return Ret;
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