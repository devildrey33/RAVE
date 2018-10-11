#include "stdafx.h"
#include "DBotonEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"

namespace DWL {

	DBotonEx::DBotonEx(void) : DControlEx(), _Marcado(FALSE), _PosIconoX(-1), _PosIconoY(-1), _Icono(NULL), _ColorFondo(COLOR_BOTON), _ColorBorde(COLOR_BORDE), _ColorTexto(COLOR_TEXTO) {
	}


	DBotonEx::~DBotonEx(void) {
	} 

	// Funci�n que crea un BotonEx con texto
	HWND DBotonEx::CrearBotonEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		_Texto = nTxt;
		return _CrearBotonEx(nPadre, cX, cY, cAncho, cAlto, cID, Estilos);
	}

	// Funci�n que crea un BotonEx con icono
	HWND DBotonEx::CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		HWND h = _CrearBotonEx(nPadre, cX, cY, cAncho, cAlto, cID, Estilos);
		Icono(IDIcono, TamIcono, PosIconoX, PosIconoY);
		return h;
	}

	// Funci�n que crea un BotonEx con icono y texto
	HWND DBotonEx::CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {		
		_Texto = nTxt;
		HWND h =_CrearBotonEx(nPadre, cX, cY, cAncho, cAlto, cID, Estilos);
		Icono(IDIcono, TamIcono, PosIconoX, PosIconoY);
		return h;
	}

	// Funci�n que asigna un icono al BotonEx
	void DBotonEx::Icono(const int IDIcono, const int TamIcono, const int PosX, const int PosY) {
		RECT RC;
		GetClientRect(_hWnd, &RC);
		_Icono = DListaIconos::AgregarIconoRecursos(IDIcono, TamIcono, TamIcono);
		_PosIconoX = (PosX == -1) ? (RC.right - TamIcono) / 2 : PosX;
		_PosIconoY = (PosY == -1) ? (RC.bottom - TamIcono) / 2 : PosX;
		Repintar();
	}

	HWND DBotonEx::_CrearBotonEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos) {
		_ColorFondo = COLOR_BOTON;
		_ColorBorde = COLOR_BORDE;
		_ColorTexto = COLOR_TEXTO;
//		if (hWnd()) { Debug_Escribir(L"DBotonEx::CrearBotonEx() Error : ya se ha creado el bot�n\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DBotonEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);
		Fuente = Fuente21Negrita;
		_Estado = DBotonEx_Estado_Normal;
		_MouseDentro = FALSE;
		return hWnd();
	}

	void DBotonEx::Pintar(HDC DC, const int nX, const int nY) {
		RECT    RC, RCF, RCT, RCS;
		GetClientRect(hWnd(), &RC);
		RCF = RC; RCF.left++; RCF.top++; RCF.right--; RCF.bottom--;
		RCS = RC; RCS.left++; RCS.top++; RCS.right++; RCS.bottom++;
		RCT = RC; 

		// Creo un buffer en mem�ria para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));
		HFONT   vFuente = static_cast<HFONT>(SelectObject(Buffer, Fuente()));

		int bPresionado = 0;
		// Si el control est� presionado retoco las posiciones del texto i los iconos
		if (_Estado == DBotonEx_Estado_Presionado) {
			RCT.top += 2;		// Recta texto
			RCS.top += 2;		// Recta sombra
			bPresionado = 1;	// x e y iniciales para el icono
		}

		// Pinto el borde
		HBRUSH BrochaBorde = CreateSolidBrush(_ColorBorde);
		FrameRect(Buffer, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		// Pinto el fondo
		HBRUSH BrochaFondo = CreateSolidBrush(_ColorFondo);
		FillRect(Buffer, &RCF, BrochaFondo);
		DeleteObject(BrochaFondo);

		
		if (_Texto.size() > 0) {
			SetBkMode(Buffer, TRANSPARENT);
			// Pinto la sombra del texto
			SetTextColor(Buffer, COLOR_TEXTO_SOMBRA);
			DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCS, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			// Pinto el texto
			SetTextColor(Buffer, _ColorTexto);
			DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RCT, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		if (_Icono != NULL) {
			DrawIconEx(Buffer, bPresionado + _PosIconoX, bPresionado + _PosIconoY, _Icono->Icono(), _Icono->Ancho(), _Icono->Alto(), 0, 0, DI_NORMAL);
		}

		// Copio el buffer al DC
		BitBlt(DC, nX + RC.left, nY + RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la mem�ria
		SelectObject(Buffer, vFuente);
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	void DBotonEx::Activado(const BOOL nActivar) {
		BOOL Ret = FALSE;
		Ret = EnableWindow(_hWnd, nActivar);
		Transicion((nActivar == TRUE) ? DBotonEx_Transicion_Normal : DBotonEx_Transicion_Desactivado);
		//Repintar();
	}

	

	void DBotonEx::_Evento_MouseMovimiento(const WPARAM wParam, const LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, this);
		if (_MouseEntrando() == TRUE) {
			// Mouse enter
			if (_Estado != DBotonEx_Estado_Presionado) {
				_Estado = DBotonEx_Estado_Resaltado;
				Transicion(DBotonEx_Transicion_Resaltado);
			}
		}

		Evento_MouseMovimiento(DatosMouse);
	}


	void DBotonEx::Transicion(const DBotonEx_Transicion nTransicion) {
		DWORD Duracion = DhWnd::TiempoAnimaciones;
		if (_AniTransicion.Animando() == TRUE) {
			Duracion = _AniTransicion.TiempoActual();
			_AniTransicion.Terminar();
		}

		COLORREF FondoHasta, BordeHasta, TextoHasta;
		switch (nTransicion) {
			case DBotonEx_Transicion_Normal:
				FondoHasta = COLOR_BOTON;
				BordeHasta = COLOR_BORDE;
				TextoHasta = COLOR_TEXTO;
				break;
			case DBotonEx_Transicion_Resaltado:
				FondoHasta = COLOR_BOTON_RESALTADO;
				BordeHasta = COLOR_BORDE_RESALTADO;
				TextoHasta = COLOR_TEXTO_RESALTADO;
				break;
			case DBotonEx_Transicion_Presionado:
				FondoHasta = COLOR_BOTON_PRESIONADO;
				BordeHasta = COLOR_BORDE_PRESIONADO;
				TextoHasta = COLOR_TEXTO_PRESIONADO;
				break;
			case DBotonEx_Transicion_Marcado:
				FondoHasta = COLOR_ROJO_MARCADO;
				BordeHasta = COLOR_BORDE;
				TextoHasta = COLOR_TEXTO;
				break;
			case DBotonEx_Transicion_Desactivado:
				FondoHasta = COLOR_BOTON;
				BordeHasta = COLOR_BORDE;
				TextoHasta = COLOR_TEXTO_DESACTIVADO;
				break;
		}
		_AniTransicion.Iniciar(_ColorFondo, FondoHasta, _ColorBorde, BordeHasta, _ColorTexto, TextoHasta, Duracion, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
			_ColorFondo = Valores[0];
			_ColorBorde = Valores[1];
			_ColorTexto = Valores[2];
			Repintar();
		});
	}

	void DBotonEx::_Evento_MousePresionado(const WPARAM wParam, const LPARAM lParam, const int Boton) {
		#if DBOTONEX_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DBotonEx::_Evento_MousePresionado\n");
		#endif	
		DEventoMouse DatosMouse(wParam, lParam, this, Boton);
		SetCapture(hWnd());
		//_Estado = DBotonEx_Estado_Presionado;
		SendMessage(GetParent(hWnd()), DWL_BOTONEX_MOUSEDOWN, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
		Evento_MousePresionado(DatosMouse);
		//Repintar();
		Transicion(DBotonEx_Transicion_Presionado);
		_Estado = DBotonEx_Estado_Presionado;
	}

	void DBotonEx::_Evento_MouseSoltado(const WPARAM wParam, const LPARAM lParam, const int Boton) {		
		#if DBOTONEX_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DBotonEx::_Evento_MouseSoltado\n");
		#endif	
		ReleaseCapture();

		if (_Estado == DBotonEx_Estado_Presionado) {
			DEventoMouse DatosMouse(wParam, lParam, this, Boton);
			

			RECT RC;
			GetClientRect(hWnd(), &RC);

			Evento_MouseSoltado(DatosMouse);
			SendMessage(GetParent(hWnd()), DWL_BOTONEX_MOUSEUP, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);

			POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
			if (PtInRect(&RC, Pt) != 0) {
				_Estado = DBotonEx_Estado_Resaltado;
				Transicion(DBotonEx_Transicion_Resaltado);
				Evento_MouseClick(DatosMouse);
				SendMessage(GetParent(hWnd()), DWL_BOTONEX_CLICK, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
			}
			else {
				_Estado = DBotonEx_Estado_Normal;
//				Transicion(DBotonEx_Transicion_Normal); (no hace falta salta el mouse leave)
			}
//			Repintar();
		}
	}

	void DBotonEx::Marcado(const BOOL nMarcar) {
		_Marcado = nMarcar;
		POINT P;
		DWL::DMouse::ObtenerPosicion(&P);
		ScreenToClient(_hWnd, &P);
		RECT RC;
		GetClientRect(_hWnd, &RC);
		// Miro si el mouse est� encima del control, y asigno el color rojo resaltado
		if (PtInRect(&RC, P) == TRUE) {
			_ColorFondo = COLOR_BOTON_RESALTADO;
//			Transicion(DBotonEx_Transicion_Resaltado); // no se necesita una transici�n porque el mouse est� encima y sigue siendo resaltado
			_Estado = DBotonEx_Estado_Resaltado;
		}
		else { // Si no est� encima del control asigno el color seg�n si est� marcado o no
			_Estado = DBotonEx_Estado_Normal;
			Transicion((nMarcar == TRUE) ? DBotonEx_Transicion_Marcado : DBotonEx_Transicion_Normal);
			
		}	
	}

	void DBotonEx::_Evento_Pintar(void) {
		HDC         DC;
		PAINTSTRUCT PS;
		DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}

	void DBotonEx::_Evento_MouseSaliendo(void) {
		#if DBOTONEX_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DBotonEx::_Evento_MouseSaliendo\n");
		#endif	
		_MouseDentro = FALSE;
		if (_Estado != DBotonEx_Estado_Presionado) {
			Transicion((_Marcado == FALSE) ? DBotonEx_Transicion_Normal : DBotonEx_Transicion_Marcado);
			_Estado = DBotonEx_Estado_Normal;
		}
	}

	LRESULT CALLBACK DBotonEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT		:		_Evento_Pintar();									return 0;
			case WM_MOUSEMOVE	:		_Evento_MouseMovimiento(wParam, lParam);			return 0;
			case WM_MOUSELEAVE	:		_Evento_MouseSaliendo();							return 0;
			case WM_LBUTTONDOWN	:		_Evento_MousePresionado(wParam, lParam, 0);			return 0;
			case WM_RBUTTONDOWN	:		_Evento_MousePresionado(wParam, lParam, 1);			return 0;
			case WM_MBUTTONDOWN	:		_Evento_MousePresionado(wParam, lParam, 2);			return 0;
			case WM_LBUTTONUP	:		_Evento_MouseSoltado(wParam, lParam, 0);			return 0;
			case WM_RBUTTONUP	:		_Evento_MouseSoltado(wParam, lParam, 1);			return 0;
			case WM_MBUTTONUP	:		_Evento_MouseSoltado(wParam, lParam, 2);			return 0;
		}
		return DControlEx::GestorMensajes(uMsg, wParam, lParam);
	}
}