#include "stdafx.h"
#include "DMarcaEx.h"
#include "DMensajesWnd.h"

namespace DWL {

	#define DMARCAEX_TAMICONO 16

	DMarcaEx::DMarcaEx(void) {
	}


	DMarcaEx::~DMarcaEx(void) {
	}

	// Alto mínimo 18
	HWND DMarcaEx::CrearMarcaEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const int IDIconoMarca, const long Estilos) {
//		if (hWnd())		 { Debug_Escribir(L"DMarcaEx::CrearBotonEx() Error : ya se ha creado la marca\n");				return hWnd(); }
		if (cAlto <= 18) { Debug_Escribir(L"DMarcaEx::CrearBotonEx() Error : la altura es inferior a 18 pixeles\n");	return NULL; }
	
		_hWnd = CrearControlEx(nPadre, L"DMarcaEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);
		_Texto = nTxt;
		_Icono = DListaIconos::AgregarIconoRecursos(IDIconoMarca, DMARCAEX_TAMICONO, DMARCAEX_TAMICONO);
		_MouseDentro = FALSE;
		return hWnd();
	}


	void DMarcaEx::Pintar(HDC DC) {
		RECT RC;
		GetClientRect(_hWnd, &RC);
		// Creo un buffer en memória para pintar el control
		HDC		Buffer		= CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp			= CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));
		HFONT   vFuente		= static_cast<HFONT>(SelectObject(Buffer, DhWnd::Fuente18Normal()));

		// Pinto el fondo
		HBRUSH ColorFondo = CreateSolidBrush(COLOR_FONDO);
		FillRect(Buffer, &RC, ColorFondo);
		DeleteObject(ColorFondo);

		// Determino que colores hay que utilizar según el estado
		COLORREF ColTexto, ColMarcoBorde;
		int bPresionado = 0;
		switch (_Estado) {
			case DMarcaEx_Estado_Normal		:	ColTexto = COLOR_TEXTO;					ColMarcoBorde =	COLOR_BORDE;						break;
			case DMarcaEx_Estado_Resaltado	:	ColTexto = COLOR_TEXTO_RESALTADO;		ColMarcoBorde = COLOR_ROJO;							break;
			case DMarcaEx_Estado_Presionado :	ColTexto = COLOR_TEXTO_PRESIONADO;		ColMarcoBorde = COLOR_BORDE;	bPresionado = 1;	break;
		}



		// Pinto el marco para el check
		int  YMarco = (RC.bottom - (DMARCAEX_TAMICONO + 2)) / 2;
		RECT Marco  = { 0, YMarco, DMARCAEX_TAMICONO + 2, (DMARCAEX_TAMICONO + 2) + YMarco };
		
		HBRUSH	ColorFondoMarco = CreateSolidBrush(COLOR_FONDO_CLARO);
		HPEN	ColorBordeMarco = CreatePen(PS_SOLID, 1, ColMarcoBorde);
		HBRUSH  BrochaViejo		= static_cast<HBRUSH>(SelectObject(Buffer, ColorFondoMarco));
		HPEN    PlumaViejo		= static_cast<HPEN>(SelectObject(Buffer, ColorBordeMarco));
		Rectangle(Buffer, Marco.left, Marco.top, Marco.right, Marco.bottom);

		SelectObject(Buffer, BrochaViejo);
		SelectObject(Buffer, PlumaViejo);
		DeleteObject(ColorFondoMarco);
		DeleteObject(ColorBordeMarco);

		// Pinto la marca si es necesario
		if (_Marcado == TRUE) {
			DrawIconEx(Buffer, 1, YMarco + 1, _Icono->Icono(), DMARCAEX_TAMICONO, DMARCAEX_TAMICONO, 0, 0, DI_NORMAL);			
		}

		// Pinto el texto
		SetBkMode(Buffer, TRANSPARENT);
		SetTextColor(Buffer, COLOR_TEXTO_SOMBRA);
		TextOut(Buffer, DMARCAEX_TAMICONO + 7, YMarco + 1, _Texto.c_str(), static_cast<int>(_Texto.size()));
		SetTextColor(Buffer, ColTexto);
		TextOut(Buffer, DMARCAEX_TAMICONO + 6, YMarco, _Texto.c_str(), static_cast<int>(_Texto.size()));


		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, vFuente);
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}


	void DMarcaEx::_Evento_Pintar(void) {
		HDC         DC;
		PAINTSTRUCT PS;
		DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}




	void DMarcaEx::Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam) {
	}

	void DMarcaEx::Evento_MousePresionado(const int cX, const int cY, const UINT wParam) {
		SetCapture(hWnd());
		_Estado = DMarcaEx_Estado_Presionado;
		Repintar();
	}

	void DMarcaEx::Evento_MouseSoltado(const int cX, const int cY, const UINT wParam) {
		if (_Estado == DMarcaEx_Estado_Presionado) {
			ReleaseCapture();

			RECT RC;
			GetClientRect(hWnd(), &RC);

			POINT Pt = { cX, cY };
			if (PtInRect(&RC, Pt) != 0) {
				_Estado = DMarcaEx_Estado_Resaltado;
				_Marcado = !_Marcado;
				PostMessage(GetParent(hWnd()), DWL_MARCAEX_CLICK, static_cast<WPARAM>(GetWindowLongPtr(hWnd(), GWL_ID)), MAKELPARAM(cX, cY));
			}
			else {
				_Estado = DMarcaEx_Estado_Normal;
			}
			Repintar();
		}
	}

	LRESULT CALLBACK DMarcaEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT	: 
				_Evento_Pintar();			
				break;
			case WM_MOUSEMOVE:
				if (_MouseEntrando() == TRUE) {
					// Mouse enter
					if (_Estado != DMarcaEx_Estado_Presionado) {
						_Estado = DMarcaEx_Estado_Resaltado;
						Repintar();
					}
				}
				Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
			case WM_MOUSELEAVE:
				_MouseDentro = FALSE;
				if (_Estado != DMarcaEx_Estado_Presionado) {
					_Estado = DMarcaEx_Estado_Normal;
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
		return DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}

};