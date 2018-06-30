#include "stdafx.h"
#include "DEdicionTextoEx.h"

namespace DWL {

/*     _____  ______    _ _      _          _______        _        ______          _   _            _            
      |  __ \|  ____|  | (_)    (_)        |__   __|      | |      |  ____|        | \ | |          | |           
      | |  | | |__   __| |_  ___ _  ___  _ __ | | _____  _| |_ ___ | |__  __  __   |  \| |_   _  ___| | ___  ___  
      | |  | |  __| / _` | |/ __| |/ _ \| '_ \| |/ _ \ \/ / __/ _ \|  __| \ \/ /   | . ` | | | |/ __| |/ _ \/ _ \ 
      | |__| | |___| (_| | | (__| | (_) | | | | |  __/>  <| || (_) | |____ >  <    | |\  | |_| | (__| |  __/ (_) |
      |_____/|______\__,_|_|\___|_|\___/|_| |_|_|\___/_/\_\\__\___/|______/_/\_\   |_| \_|\__,_|\___|_|\___|\___/                                                                                                                   
*/

	DEdicionTextoEx_Nucleo::DEdicionTextoEx_Nucleo(void) :	TextoEditable(TRUE)										, ColorCursor(COLOR_EDICION_CURSOR),
															ColorTexto(COLOR_EDICION_TEXTO)							, ColorTextoSombra(COLOR_EDICION_TEXTO_SOMBRA),
															ColorTextoSeleccionado(COLOR_EDICION_SELECCION_TEXTO)	, ColorTextoSombraSeleccionado(COLOR_EDICION_SELECCION_TEXTO_SOMBRA),
															ColorFondo(COLOR_EDICION_FONDO)							, ColorFondoSeleccionado(COLOR_EDICION_SELECCION),
															_PosCursor(0)											, _FinSeleccion(-1) {
	}


	DEdicionTextoEx_Nucleo::~DEdicionTextoEx_Nucleo(void) {

	}

	void DEdicionTextoEx_Nucleo::IniciarEdicionTextoEx(RECT &Espacio) {
		_EspacioEdicion = Espacio;
	}


	void DEdicionTextoEx_Nucleo::Texto(std::wstring &nTexto) {
		_Texto		= nTexto;
		_PosCursor	= _Texto.size();
	}

	void DEdicionTextoEx_Nucleo::Texto(const wchar_t *nTexto) {
		_Texto		= nTexto;
		_PosCursor	= _Texto.size();
	}


	void DEdicionTextoEx_Nucleo::EdicionTexto_Pintar(HDC hDC) {

		// Calculo el tamaño del texto
		SIZE TF = Fuente.Tam(_Texto);
		// Si el ancho del texto es mas pequeño que el espacio para la edición de texto, corrijo el ancho
		if (TF.cx < _EspacioEdicion.right - _EspacioEdicion.left) TF.cx = _EspacioEdicion.right - _EspacioEdicion.left;
		// Si la altura del texto es mas pequeña que el espacio para la edición de texto, corrijo la altura
		if (TF.cy < _EspacioEdicion.bottom - _EspacioEdicion.top) TF.cy = _EspacioEdicion.bottom - _EspacioEdicion.top;
		
		// Calculo la posición X del cursor
		SIZE PC = Fuente.Tam(_Texto.substr(0, _PosCursor));
		long PosDifX = 0;
		if (PC.cx > _EspacioEdicion.right - _EspacioEdicion.left) PosDifX = PC.cx - (_EspacioEdicion.right - _EspacioEdicion.left);

		// Creo un buffer con el espacio suficiente para pintar todo el texto
		HDC		Buffer		= CreateCompatibleDC(hDC);
		HBITMAP Bmp			= CreateCompatibleBitmap(hDC, TF.cx, TF.cy);
		HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));


		RECT REspacio = { 0, 0, TF.cx, TF.cy };
		RECT RSombra  = REspacio;

		HBRUSH BrochaFondo = CreateSolidBrush(ColorFondo);
		FillRect(Buffer, &REspacio, BrochaFondo);
		DeleteObject(BrochaFondo);

		RSombra.left++; RSombra.top++; RSombra.right++; RSombra.bottom++;
		SetBkMode(Buffer, TRANSPARENT);
		// Pinto la sombra del texto
		SetTextColor(Buffer, ColorTextoSombra);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RSombra, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);

		// Pinto el texto
		SetTextColor(Buffer, ColorTexto);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &REspacio, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);

		// Pinto el cursor
		if (TextoEditable == TRUE) {
			std::wstring TextoHastaElCursor = _Texto.substr(0, _PosCursor);
			HPEN PlumaCursor = CreatePen(PS_SOLID, 1, ColorCursor);
			HPEN VPluma = static_cast<HPEN>(SelectObject(Buffer, PlumaCursor));
			SIZE TamTxt;
			GetTextExtentPoint(Buffer, TextoHastaElCursor.c_str(), static_cast<int>(TextoHastaElCursor.size()), &TamTxt);
			int PosCursorPx = REspacio.left + TamTxt.cx + 1;
			MoveToEx(Buffer, PosCursorPx, 2, NULL);
			LineTo(Buffer, PosCursorPx, (REspacio.bottom - REspacio.top) - 1);
			SelectObject(Buffer, VPluma);
		}


/*		if (_FinSeleccion != -1) {
			std::wstring Str1 = Texto.substr(0, _PosCursor);
			std::wstring Str2 = (_FinSeleccion > _PosCursor) ? Texto.substr(_PosCursor, _FinSeleccion - _PosCursor) : Texto.substr(_FinSeleccion, _PosCursor - _FinSeleccion);
			std::wstring Str3 = (_FinSeleccion > _PosCursor) ? Texto.substr(_FinSeleccion)							: Texto.substr(_PosCursor);
			

		}*/

		// Pinto el buffer en el DC
		BitBlt(hDC, _EspacioEdicion.left, _EspacioEdicion.top, _EspacioEdicion.right - _EspacioEdicion.left, _EspacioEdicion.bottom - _EspacioEdicion.top, Buffer, PosDifX, 0, SRCCOPY);

		// Elimino objetos gdi de la memoria
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);


	}


	const BOOL DEdicionTextoEx_Nucleo::EdicionTexto_Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		switch (Caracter) {
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
		return TRUE;
	}

	const BOOL DEdicionTextoEx_Nucleo::EdicionTexto_Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		switch (Caracter) {
			case VK_SHIFT:
				break;
		}
		return FALSE;
	}

	const BOOL DEdicionTextoEx_Nucleo::EdicionTexto_Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param) {
		switch (Caracter) {
			case VK_RETURN:
				if (TextoEditable == FALSE) return FALSE;
				break;
			case VK_BACK: // Borrar
				if (TextoEditable == FALSE) return FALSE;
				if (_PosCursor > 0) _Texto.erase(--_PosCursor, 1);				
				break;
			case VK_ESCAPE: // Desselecciono todo
				_FinSeleccion = -1;
				break;
			default:
				if (_PosCursor == _Texto.size()) {
					_Texto += static_cast<wchar_t>(Caracter);
				}
				else {
					_Texto.insert(_PosCursor, 1, static_cast<wchar_t>(Caracter));
				}
				_PosCursor++;
				break;
		}		
		return TRUE;
	}


/*
  _____  ______    _ _      _          _______        _        ______      
 |  __ \|  ____|  | (_)    (_)        |__   __|      | |      |  ____|     
 | |  | | |__   __| |_  ___ _  ___  _ __ | | _____  _| |_ ___ | |__  __  __
 | |  | |  __| / _` | |/ __| |/ _ \| '_ \| |/ _ \ \/ / __/ _ \|  __| \ \/ /
 | |__| | |___| (_| | | (__| | (_) | | | | |  __/>  <| || (_) | |____ >  < 
 |_____/|______\__,_|_|\___|_|\___/|_| |_|_|\___/_/\_\\__\___/|______/_/\_\
                                                                          																		  
*/
#define ESPACIO_ICONO 20

	DEdicionTextoEx::DEdicionTextoEx() : DControlEx(), DEdicionTextoEx_Nucleo(), _Icono(NULL) {
	}


	DEdicionTextoEx::~DEdicionTextoEx() {
	}

	HWND DEdicionTextoEx::CrearEdicionTextoEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, DListaIconos_Icono *nIcono, const long Estilos) {
		if (hWnd()) { Debug_Escribir(L"DEdicionTextoEx::CrearEdicionTextoEx() Error : ya se ha creado el control...\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DEdicionTextoEx", L"", cID, cX, cY, cAncho, cAlto, Estilos, NULL);
		_Texto = nTxt;
		Fuente = _Fuente18Normal;

		Icono(nIcono, FALSE);

		return hWnd();
	}

	void DEdicionTextoEx::Icono(DListaIconos_Icono *nIcono, const BOOL nRepintar) {
		_Icono = nIcono;

		RECT RC;
		GetClientRect(_hWnd, &RC);
		if (_Icono != NULL) RC.left += ESPACIO_ICONO;
		IniciarEdicionTextoEx(RC);

		if (nRepintar != FALSE) Repintar();
	}

	void DEdicionTextoEx::Pintar(HDC DC) {
		RECT RC;
		GetClientRect(hWnd(), &RC);

		// Creo un buffer en memória para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		// Pinto el texto
		EdicionTexto_Pintar(Buffer);

		// Pinto el icono (si hay icono)
		if (_Icono != NULL) {
			// Pinto el fondo del icono
			RECT RFondoIcono = { 0 , 0, ESPACIO_ICONO, RC.bottom };
			HBRUSH BrochaFondo = CreateSolidBrush(ColorFondo);
			FillRect(Buffer, &RFondoIcono, BrochaFondo);
			DeleteObject(BrochaFondo);
			// Pinto el icono
			DrawIconEx(Buffer, 2, 2, _Icono->Icono(), DARBOLEX_TAMICONO, DARBOLEX_TAMICONO, 0, 0, DI_NORMAL);
		}

		// Copio el buffer al DC
		BitBlt(DC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	void DEdicionTextoEx::_Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		if (EdicionTexto_Evento_TeclaPresionada(Caracter, Repeticion, Params) == TRUE) Repintar();
	}

	void DEdicionTextoEx::_Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		if (EdicionTexto_Evento_TeclaSoltada(Caracter, Repeticion, Params) == TRUE) Repintar();
	}

	void DEdicionTextoEx::_Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param) {
		if (EdicionTexto_Evento_Tecla(Caracter, Repeticion, Param) == TRUE) Repintar();
	}

	void DEdicionTextoEx::_Evento_Pintar(void) {
		HDC         DC;
		PAINTSTRUCT PS;
		DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}

	void DEdicionTextoEx::_Evento_Size(void) {
		RECT RC;
		GetClientRect(_hWnd, &RC);
		if (_Icono != NULL) RC.left += ESPACIO_ICONO;
		IniciarEdicionTextoEx(RC);
		Repintar();
	}

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


	LRESULT CALLBACK DEdicionTextoEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_SETFOCUS:		BorrarBufferTeclado();																														return 0;
			case WM_KILLFOCUS:		BorrarBufferTeclado();																														return 0;

			case WM_KEYDOWN:		_Evento_TeclaPresionada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
			case WM_KEYUP:			_Evento_TeclaSoltada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
			case WM_CHAR: 			_Evento_Tecla(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));																	return 0;

			case WM_SIZE:			Repintar();																																	return 0;

			case WM_PAINT:			_Evento_Pintar();																															return 0;

//			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;

//			case WM_MOUSELEAVE:		_Evento_MouseSaliendo();																													return 0;

		}
		return DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}


}