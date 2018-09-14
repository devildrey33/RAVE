#include "stdafx.h"
#include "DBotonEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBotonEx::DBotonEx(void) :	_Marcado(FALSE), _PosIconoX(-1), _PosIconoY(-1), _Icono(NULL) {
	}


	DBotonEx::~DBotonEx(void) {
	} 

	// Función que crea un BotonEx con texto
	HWND DBotonEx::CrearBotonEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		_Texto = nTxt;
		return _CrearBotonEx(nPadre, cX, cY, cAncho, cAlto, cID, Estilos);
	}

	// Función que crea un BotonEx con icono
	HWND DBotonEx::CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		HWND h = _CrearBotonEx(nPadre, cX, cY, cAncho, cAlto, cID, Estilos);
		Icono(IDIcono, TamIcono, PosIconoX, PosIconoY);
		return h;
	}

	// Función que crea un BotonEx con icono y texto
	HWND DBotonEx::CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		
		_Texto = nTxt;
		HWND h =_CrearBotonEx(nPadre, cX, cY, cAncho, cAlto, cID, Estilos);
		Icono(IDIcono, TamIcono, PosIconoX, PosIconoY);
		return h;
	}

	// Función que asigna un icono al BotonEx
	void DBotonEx::Icono(const int IDIcono, const int TamIcono, const int PosX, const int PosY) {
		RECT RC;
		GetClientRect(_hWnd, &RC);
		_Icono = DListaIconos::AgregarIconoRecursos(IDIcono, TamIcono, TamIcono);
		_PosIconoX = (PosX == -1) ? (RC.right - TamIcono) / 2 : PosX;
		_PosIconoY = (PosY == -1) ? (RC.bottom - TamIcono) / 2 : PosX;
		Repintar();
	}

	HWND DBotonEx::_CrearBotonEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
//		if (hWnd()) { Debug_Escribir(L"DBotonEx::CrearBotonEx() Error : ya se ha creado el botón\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DBotonEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);
		Fuente = Fuente21Negrita;
		_Estado = DBotonEx_Estado_Normal;
		_MouseDentro = FALSE;
		return hWnd();
	}

	void DBotonEx::Pintar(HDC DC) {
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
		int bPresionado = 0;
		switch (_Estado) {
			case DBotonEx_Estado_Normal:
				nColorFondo = COLOR_BOTON;
				nColorBorde = COLOR_BORDE;
				nColorTexto = COLOR_TEXTO;
				break;
			case DBotonEx_Estado_Resaltado:
				nColorFondo = COLOR_BOTON_RESALTADO;
				nColorBorde = COLOR_BORDE_RESALTADO;
				nColorTexto = COLOR_TEXTO_RESALTADO;
				break;
			case DBotonEx_Estado_Presionado:
				nColorFondo = COLOR_BOTON_PRESIONADO;
				nColorBorde = COLOR_BORDE_PRESIONADO;
				nColorTexto = COLOR_TEXTO_PRESIONADO;
				RCT.top += 2;
				RCS.top += 2;
				break;
		}
		// Si el control no está activado modifico el color final del texto
		if (Activado() == FALSE) {
			nColorTexto = COLOR_TEXTO_DESACTIVADO;
		}

		if (_Marcado == TRUE) {
			nColorBorde = COLOR_ROJO_MARCADO;
		}

		// Pinto el borde
		HBRUSH BrochaBorde = CreateSolidBrush(nColorBorde);
		FrameRect(Buffer, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		// Pinto el fondo
		HBRUSH BrochaFondo = CreateSolidBrush(nColorFondo);
		FillRect(Buffer, &RCF, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Si está marcado, pinto un borde rojo oscuro a la derecha
		if (_Marcado == TRUE) {
			HBRUSH BrochaMarcado = CreateSolidBrush(COLOR_ROJO_MARCADO);
			RECT RM = { RC.right - 5, 0, RC.right, RC.bottom };
			FillRect(Buffer, &RM, BrochaMarcado);
			DeleteObject(BrochaMarcado);			
		}


		if (_Texto.size() > 0) {
			SetBkMode(Buffer, TRANSPARENT);
			// Pinto la sombra del texto
			SetTextColor(Buffer, COLOR_TEXTO_SOMBRA);
			DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCS, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			// Pinto el texto
			SetTextColor(Buffer, nColorTexto);
			DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCT, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		if (_Icono != NULL) {
			DrawIconEx(Buffer, bPresionado + _PosIconoX, bPresionado + _PosIconoY, _Icono->Icono(), _Icono->Ancho(), _Icono->Alto(), 0, 0, DI_NORMAL);
		}

		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, vFuente);
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	void DBotonEx::Activado(const BOOL nActivar) {
		BOOL Ret = FALSE;
		Ret = EnableWindow(_hWnd, nActivar);
		Repintar();
	}

	

	void DBotonEx::_Evento_MouseMovimiento(const WPARAM wParam, const LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, ID());
		Evento_MouseMovimiento(DatosMouse);
	}


	void DBotonEx::_Evento_MousePresionado(const WPARAM wParam, const LPARAM lParam, const int Boton) {
		DEventoMouse DatosMouse(wParam, lParam, ID(), Boton);
		SetCapture(hWnd());
		_Estado = DBotonEx_Estado_Presionado;
		SendMessage(GetParent(hWnd()), DWL_BOTONEX_MOUSEDOWN, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
		Evento_MousePresionado(DatosMouse);
		Repintar();
	}

	void DBotonEx::_Evento_MouseSoltado(const WPARAM wParam, const LPARAM lParam, const int Boton) {
		DEventoMouse DatosMouse(wParam, lParam, ID(), Boton);
		if (_Estado == DBotonEx_Estado_Presionado) {
			ReleaseCapture();

			RECT RC;
			GetClientRect(hWnd(), &RC);

			POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
			if (PtInRect(&RC, Pt) != 0) {
				_Estado = DBotonEx_Estado_Resaltado;
				Evento_MouseSoltado(DatosMouse);
				SendMessage(GetParent(hWnd()), DWL_BOTONEX_CLICK, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
			}
			else {
				_Estado = DBotonEx_Estado_Normal;
			}
			Repintar();
		}
	}

	void DBotonEx::Marcado(const BOOL nMarcar) {
		_Marcado = nMarcar;
		Repintar();
	}

	void DBotonEx::_Evento_Pintar(void) {
		HDC         DC;
		PAINTSTRUCT PS;
		DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}


	LRESULT CALLBACK DBotonEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT		:		_Evento_Pintar();				return 0;
			case WM_MOUSEMOVE	:
				if (_MouseEntrando() == TRUE) {
					// Mouse enter
					if (_Estado != DBotonEx_Estado_Presionado) {
						_Estado = DBotonEx_Estado_Resaltado;
						Repintar();
					}
				}				
				_Evento_MouseMovimiento(wParam, lParam);
				break;
			case WM_MOUSELEAVE:
				_MouseDentro = FALSE;
				if (_Estado != DBotonEx_Estado_Presionado) {
					_Estado = DBotonEx_Estado_Normal;
					Repintar();
				}
				break;
			case WM_LBUTTONDOWN:
				_Evento_MousePresionado(wParam, lParam, 0);
				return 0;
			case WM_RBUTTONDOWN:
				_Evento_MousePresionado(wParam, lParam, 1);
				return 0;
			case WM_MBUTTONDOWN:
				_Evento_MousePresionado(wParam, lParam, 2);
				return 0;
			case WM_LBUTTONUP:
				_Evento_MouseSoltado(wParam, lParam, 0);
				return 0;
			case WM_RBUTTONUP:
				_Evento_MouseSoltado(wParam, lParam, 1);
				return 0;
			case WM_MBUTTONUP:
				_Evento_MouseSoltado(wParam, lParam, 2);
				return 0;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}
}