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


	void DMarcaEx::Marcado(const BOOL nMarcado) {
		_Marcado = nMarcado;
		Repintar();
	}

	void DMarcaEx::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {
		BOOL nRepintar = FALSE;
		DEventoMouse DatosMouse(wParam, lParam, ID(), -1);
		if (_MouseEntrando() == TRUE) {
			// Mouse enter
			if (_Estado != DMarcaEx_Estado_Presionado) {
				_Estado = DMarcaEx_Estado_Resaltado;
				nRepintar = TRUE;
			}
		}
		Evento_MouseMovimiento(DatosMouse);

		if (nRepintar == TRUE)	Repintar();
	}

	void DMarcaEx::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, ID(), Boton);
		SetCapture(hWnd());
		_Estado = DMarcaEx_Estado_Presionado;
		Evento_MousePresionado(DatosMouse);
		Repintar();
	}

	void DMarcaEx::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, ID(), Boton);
		BOOL nRepintar = FALSE;
		if (_Estado == DMarcaEx_Estado_Presionado) {
			ReleaseCapture();

			RECT RC;
			GetClientRect(hWnd(), &RC);

			POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
			if (PtInRect(&RC, Pt) != 0) {
				_Estado = DMarcaEx_Estado_Resaltado;
				_Marcado = !_Marcado;
				SendMessage(GetParent(hWnd()), DWL_MARCAEX_CLICK, reinterpret_cast<WPARAM>(&DatosMouse), 0);
			}
			else {
				_Estado = DMarcaEx_Estado_Normal;
			}
			nRepintar = TRUE;
		}
		Evento_MouseSoltado(DatosMouse);
		if (nRepintar == TRUE)	Repintar();
	}

	void DMarcaEx::_Evento_MouseSaliendo(void) {
		_MouseDentro = FALSE;
		BOOL nRepintar = FALSE;
		if (_Estado != DMarcaEx_Estado_Presionado) {
			_Estado = DMarcaEx_Estado_Normal;
			nRepintar = TRUE;
		}
		Evento_MouseSaliendo();
		if (nRepintar == TRUE)	Repintar();
	}

	LRESULT CALLBACK DMarcaEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT	: 
				_Evento_Pintar();			
				break;
			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(wParam, lParam);																									return 0;
			case WM_MOUSELEAVE:		_Evento_MouseSaliendo();																													return 0;
				// Mouse presionado
			case WM_LBUTTONDOWN:	_Evento_MousePresionado(0, wParam, lParam);																									return 0;
			case WM_RBUTTONDOWN:	_Evento_MousePresionado(1, wParam, lParam);																									return 0;
			case WM_MBUTTONDOWN:	_Evento_MousePresionado(2, wParam, lParam);																									return 0;
				// Mouse soltado
			case WM_LBUTTONUP:		_Evento_MouseSoltado(0, wParam, lParam);																									return 0;
			case WM_RBUTTONUP:		_Evento_MouseSoltado(1, wParam, lParam);																									return 0;
			case WM_MBUTTONUP:		_Evento_MouseSoltado(2, wParam, lParam);																									return 0;

		}
		return DControlEx::GestorMensajes(uMsg, wParam, lParam);
	}

};