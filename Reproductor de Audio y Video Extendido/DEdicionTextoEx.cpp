#include "stdafx.h"
#include "DEdicionTextoEx.h"

#define DEDICIONTEXTOEX_TAMICONO	16
#define DEDICIONTEXTOEX_MARGEN_X	4

#define ID_TIMER_CURSOR				1000

namespace DWL {
/*
	  _____  ______    _ _      _          _______        _        ______      
	 |  __ \|  ____|  | (_)    (_)        |__   __|      | |      |  ____|     
	 | |  | | |__   __| |_  ___ _  ___  _ __ | | _____  _| |_ ___ | |__  __  __
	 | |  | |  __| / _` | |/ __| |/ _ \| '_ \| |/ _ \ \/ / __/ _ \|  __| \ \/ /
	 | |__| | |___| (_| | | (__| | (_) | | | | |  __/>  <| || (_) | |____ >  < 
	 |_____/|______\__,_|_|\___|_|\___/|_| |_|_|\___/_/\_\\__\___/|______/_/\_\
                                                                          																		  
*/

	DEdicionTextoEx::DEdicionTextoEx(void) : DControlEx(), _Icono(NULL), TextoEditable(TRUE), Alineacion(DEdicionTextoEx_Alineacion_Izquierda), _Presionado(FALSE) {
	}


	DEdicionTextoEx::~DEdicionTextoEx(void) {
	}

	HWND DEdicionTextoEx::CrearEdicionTextoEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, DListaIconos_Icono *nIcono, const long Estilos) {
//		if (hWnd()) { Debug_Escribir(L"DEdicionTextoEx::CrearEdicionTextoEx() Error : ya se ha creado el control...\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DEdicionTextoEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);
		Fuente			= Fuente18Normal;
		_Texto			= nTxt;
		_PosCursor		= _Texto.size();
		_ColorTexto		= COLOR_EDICION_TEXTO;
		_ColorFondo		= COLOR_EDICION_FONDO;
		_ColorBorde		= COLOR_EDICION_BORDE;
		_ColorCursor	= COLOR_EDICION_FONDO; // Inicialmente el color del cursor es el del fondo, y tiene que llegar con una animación a COLOR_EDICION_CURSOR;
		Icono(nIcono, FALSE);
		return hWnd();
	}

	void DEdicionTextoEx::Icono(DListaIconos_Icono *nIcono, const BOOL nRepintar) {
		_Icono = nIcono;
		
		if (nRepintar != FALSE) Repintar();
	}

	void DEdicionTextoEx::Pintar(HDC DC, const int cX, const int cY) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		
		HDC		Buffer		= CreateCompatibleDC(NULL);							// Creo un buffer en memória para pintar el control
		HBITMAP Bmp			= CreateCompatibleBitmap(DC, RC.right, RC.bottom);	// Creo un DC compatible para el buffer
		HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		// Pinto el fondo
		HBRUSH BrochaFondo = CreateSolidBrush(_ColorFondo);
		FillRect(Buffer, &RC, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Pinto el borde
		HBRUSH BrochaBorde = CreateSolidBrush(_ColorBorde);
		FrameRect(Buffer, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		// Recta para el espacio del texto
		RECT RTexto = { DEDICIONTEXTOEX_MARGEN_X, 0, RC.right - DEDICIONTEXTOEX_MARGEN_X, RC.bottom };

		// Pinto el icono (si hay icono)
		if (_Icono != NULL) {
			RTexto.left = DEDICIONTEXTOEX_TAMICONO + DEDICIONTEXTOEX_MARGEN_X;
			// Pinto el icono
			DrawIconEx(Buffer, 2, 2, _Icono->Icono(), DEDICIONTEXTOEX_TAMICONO, DEDICIONTEXTOEX_TAMICONO, 0, 0, DI_NORMAL);
		}

		// Calculo la posición X del cursor
		SIZE PC = Fuente.Tam(_Texto.substr(0, _PosCursor));
		long PosDifX = 0;
		if (PC.cx > RTexto.right - RTexto.left) PosDifX = PC.cx - (RTexto.right - RTexto.left);

		RECT RSombra = { RTexto.left + 1, RTexto.top + 1, RTexto.right + 1, RTexto.bottom + 1 };

		SetBkMode(Buffer, TRANSPARENT);
		// Pinto la sombra del texto
		SetTextColor(Buffer, COLOR_EDICION_TEXTO_SOMBRA);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RSombra, static_cast<DWORD>(Alineacion) | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);

		// Pinto el texto
		SetTextColor(Buffer, _ColorTexto);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RTexto, static_cast<DWORD>(Alineacion) | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);

		// Pinto el cursor
		if (TextoEditable == TRUE && GetFocus() == _hWnd) {
			std::wstring TextoHastaElCursor = _Texto.substr(0, _PosCursor);
			HPEN PlumaCursor = CreatePen(PS_SOLID, 1, _ColorCursor);
			HPEN VPluma = static_cast<HPEN>(SelectObject(Buffer, PlumaCursor));
			SIZE TamTxt;
			GetTextExtentPoint(Buffer, TextoHastaElCursor.c_str(), static_cast<int>(TextoHastaElCursor.size()), &TamTxt);
			int PosCursorPx = RTexto.left + TamTxt.cx + 1;
			MoveToEx(Buffer, PosCursorPx, 2, NULL);
			LineTo(Buffer, PosCursorPx, (RTexto.bottom - RTexto.top) - 2);
			SelectObject(Buffer, VPluma);
		}

		// Copio el buffer al DC
		BitBlt(DC, cX + RC.left, cY + RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	void DEdicionTextoEx::_Evento_TeclaPresionada(WPARAM wParam, LPARAM lParam) {
		DEventoTeclado DatosTeclado(wParam, lParam, this);
		switch (DatosTeclado.Caracter()) {
			case VK_SHIFT:
				break;
			case VK_HOME:
				_PosCursor = 0;
				break;
			case VK_END:
				_PosCursor = _Texto.size();
				break;
			case VK_LEFT:
				if (_PosCursor > 0) _PosCursor--;
				break;
			case VK_RIGHT:
				if (_PosCursor < _Texto.size()) _PosCursor++;
				break;
			case VK_DELETE: // Suprimir
				if (_Texto.size() > 0 && _PosCursor < _Texto.size()) _Texto.erase(_PosCursor, 1);
				break;
		}
		Repintar();
	}

	void DEdicionTextoEx::_Evento_TeclaSoltada(WPARAM wParam, LPARAM lParam) {
		DEventoTeclado DatosTeclado(wParam, lParam, this);
		switch (DatosTeclado.TeclaVirtual()) {
			case VK_SHIFT:
				break;
		}
	}

	void DEdicionTextoEx::_Evento_Tecla(WPARAM wParam, LPARAM lParam) {
		DEventoTeclado DatosTeclado(wParam, lParam, this);
		switch (DatosTeclado.TeclaVirtual()) {
			case VK_RETURN:
				if (TextoEditable == FALSE) return;
				break;
			case VK_BACK: // Borrar
				if (TextoEditable == FALSE) return;
				if (_PosCursor > 0) _Texto.erase(--_PosCursor, 1);				
				break;
			case VK_ESCAPE: // Desselecciono todo
				//_FinSeleccion = -1;
				break;
			default:
				if (_PosCursor == _Texto.size()) {
					_Texto += static_cast<wchar_t>(DatosTeclado.TeclaVirtual());
				}
				else {
					_Texto.insert(_PosCursor, 1, static_cast<wchar_t>(DatosTeclado.TeclaVirtual()));
				}
				_PosCursor++;
				break;
		}		
		Repintar();
		SendMessage(GetParent(hWnd()), DWL_EDICIONTEXTOEX_CAMBIO, static_cast<WPARAM>(ID()), 0);
	}

	void DEdicionTextoEx::_Evento_Pintar(void) {
		HDC         DC;
		PAINTSTRUCT PS;
		DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}

	void DEdicionTextoEx::Activado(const BOOL nActivar) {
		BOOL Ret = FALSE;
		Ret = EnableWindow(_hWnd, nActivar);
		Transicion((nActivar == TRUE) ? DEdicionTextoEx_Transicion_Normal : DEdicionTextoEx_Transicion_Desactivado);
	}


/*	void DEdicionTextoEx::_Evento_Size(void) {
		RECT RC;
		GetClientRect(_hWnd, &RC);
		if (_Icono != NULL) RC.left += ESPACIO_ICONO;
		IniciarEdicionTextoEx(RC);
		Repintar();
	}*/

	void DEdicionTextoEx::Texto(std::wstring &nTexto, const BOOL nRepintar) {
		_Texto = nTexto;
		_PosCursor = _Texto.size();

		if (nRepintar != FALSE) Repintar();
	}

	void DEdicionTextoEx::Texto(const wchar_t *nTexto, const BOOL nRepintar) {
		_Texto = nTexto;
		_PosCursor = _Texto.size();

		if (nRepintar != FALSE) Repintar();
	}

	void DEdicionTextoEx::_Evento_MouseMovimiento(const WPARAM wParam, const LPARAM lParam) {
		if (_MouseEntrando() == TRUE) {
			// Mouse enter
			Transicion(DEdicionTextoEx_Transicion_Resaltado);
		}
	}

	void DEdicionTextoEx::_Evento_MousePresionado(const WPARAM wParam, const LPARAM lParam, const int Boton) {
		if (TextoEditable == TRUE) SetFocus(_hWnd);	// REVISADO
		SetCapture(_hWnd);
		Transicion(DEdicionTextoEx_Transicion_Presionado);
		_Presionado = TRUE;
	}

	void DEdicionTextoEx::_Evento_MouseSoltado(const WPARAM wParam, const LPARAM lParam, const int Boton) {
		ReleaseCapture();	
		DEventoMouse DatosMouse(wParam, lParam, this, Boton);
		RECT RC;
		GetClientRect(hWnd(), &RC);
		POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
		if (PtInRect(&RC, Pt) != 0) {
			Transicion(DEdicionTextoEx_Transicion_Resaltado);
			SendMessage(GetParent(hWnd()), DWL_EDICIONTEXTOEX_CLICK, DEVENTOMOUSE_TO_WPARAM(DatosMouse), 0);
		}
//		else {
//			Transicion(DEdicionTextoEx_Transicion_Normal);
//		}
		_Presionado = FALSE;

		//DWL_EDICIONTEXTOEX_CLICK
	}

	void DEdicionTextoEx::_Evento_MouseSaliendo(void) {
		if (_Presionado == FALSE)	Transicion(DEdicionTextoEx_Transicion_Normal);
		_MouseDentro = FALSE;
	}

	void DEdicionTextoEx::_Evento_FocoAsignado(void) {
		if (TextoEditable == TRUE) {
			SetTimer(_hWnd, ID_TIMER_CURSOR, 1000, NULL);
			_Evento_Temporizador(ID_TIMER_CURSOR);
		}
	}

	void DEdicionTextoEx::_Evento_FocoPerdido(void) {
		KillTimer(_hWnd, ID_TIMER_CURSOR);
		// Si se piede el foco, hay que retirar el cursor
		_AniCursor.Terminar();
		_AniCursor.Iniciar(_ColorCursor, _ColorFondo, 400, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
			_ColorCursor = Valores[0];
			Repintar();
		});
	}

	void DEdicionTextoEx::_Evento_Temporizador(const INT_PTR tID) {
		// Si es el temporizador del cursor
		if (tID == ID_TIMER_CURSOR) {			
			static BOOL CursorVisible = FALSE;
			COLORREF	Hasta;
			// Asigno el color final del cursor
			if (CursorVisible == FALSE) Hasta = COLOR_EDICION_CURSOR;
			else						Hasta = _ColorFondo;
			_AniCursor.Terminar();
			_AniCursor.Iniciar(_ColorCursor, Hasta, 400, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
				_ColorCursor = Valores[0];
				Repintar();
			});
			CursorVisible = !CursorVisible;
		}
	}

	void DEdicionTextoEx::Transicion(const DEdicionTextoEx_Transicion nTransicion) {
		Debug_Escribir_Varg(L"DEdicionTextoEx::Transicion %d\n", nTransicion);

		DWORD Duracion = DhWnd::TiempoAnimaciones;
		if (_AniTransicion.Animando() == TRUE) {
			Duracion = _AniTransicion.TiempoActual();
			_AniTransicion.Terminar();
		}

		COLORREF FondoHasta, BordeHasta, TextoHasta;
		switch (nTransicion) {
			case DMarcaEx_Transicion_Normal:
				FondoHasta = COLOR_EDICION_FONDO;
				BordeHasta = COLOR_EDICION_BORDE;
				TextoHasta = COLOR_EDICION_TEXTO;
				break;
			case DMarcaEx_Transicion_Resaltado:
				FondoHasta = COLOR_EDICION_FONDO_RESALTADO;
				BordeHasta = COLOR_EDICION_BORDE_RESALTADO;
				TextoHasta = COLOR_EDICION_TEXTO_RESALTADO;
				break;
			case DMarcaEx_Transicion_Presionado:
				FondoHasta = COLOR_EDICION_FONDO_PRESIONADO;
				BordeHasta = COLOR_EDICION_BORDE_PRESIONADO;
				TextoHasta = COLOR_EDICION_TEXTO_PRESIONADO;
				break;
			case DMarcaEx_Transicion_Desactivado:
				FondoHasta = COLOR_EDICION_FONDO_DESACTIVADO;
				BordeHasta = COLOR_EDICION_BORDE;
				TextoHasta = COLOR_EDICION_TEXTO_DESACTIVADO;
				break;
		}

		_AniTransicion.Iniciar(_ColorFondo, FondoHasta, _ColorBorde, BordeHasta, _ColorTexto, TextoHasta, Duracion, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
			_ColorFondo = Valores[0];
			_ColorBorde = Valores[1];
			_ColorTexto = Valores[2];
			Repintar();
		});
	}


	LRESULT CALLBACK DEdicionTextoEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_SETFOCUS	:		_Evento_FocoAsignado();									return 0;
			case WM_KILLFOCUS	:		_Evento_FocoPerdido();									return 0;

			case WM_KEYDOWN		:		_Evento_TeclaPresionada(wParam, lParam);				return 0;
			case WM_KEYUP		:		_Evento_TeclaSoltada(wParam, lParam);					return 0;
			case WM_CHAR		: 		_Evento_Tecla(wParam, lParam);							return 0;

//			case WM_SIZE		:		Repintar();																																	return 0;

			case WM_PAINT		:		_Evento_Pintar();										return 0;
					
			case WM_MOUSEMOVE	:		_Evento_MouseMovimiento(wParam, lParam);				return 0;
			case WM_MOUSELEAVE	:		_Evento_MouseSaliendo();								return 0;
			case WM_LBUTTONDOWN	:		_Evento_MousePresionado(wParam, lParam, 0);				return 0;
			case WM_RBUTTONDOWN	:		_Evento_MousePresionado(wParam, lParam, 1);				return 0;
			case WM_MBUTTONDOWN	:		_Evento_MousePresionado(wParam, lParam, 2);				return 0;
			case WM_LBUTTONUP	:		_Evento_MouseSoltado(wParam, lParam, 0);				return 0;
			case WM_RBUTTONUP	:		_Evento_MouseSoltado(wParam, lParam, 1);				return 0;
			case WM_MBUTTONUP	:		_Evento_MouseSoltado(wParam, lParam, 2);				return 0;

			case WM_TIMER		:		_Evento_Temporizador(static_cast<UINT_PTR>(wParam));	return 0;

		}
		return DControlEx::GestorMensajes(uMsg, wParam, lParam);
	}


}