#include "stdafx.h"
#include "DMenuEx.h"
#include "DMouse.h"

namespace DWL {

	DMenuEx::DMenuEx(void) : _Padre(NULL), _Tipo(DMenuEx_Tipo_Raiz), _hWndDest(NULL), _ID(0), _Activado(TRUE), _MenuResaltado(NULL), _MenuPresionado(NULL) {
		_Recta = { 0, 0, 0, 0 };
	}

	DMenuEx::DMenuEx(DMenuEx *nPadre, DMenuEx_Tipo nTipo, HWND nhWndPadre, const UINT nID) : _Padre(nPadre), _Tipo(DMenuEx_Tipo_Separador), _hWndDest(nhWndPadre), _ID(nID), _MenuResaltado(NULL), _MenuPresionado(NULL) {
		_Recta = { 0, 0, 0, 0 };
	}

	DMenuEx::DMenuEx(DMenuEx *nPadre, DMenuEx_Tipo nTipo, HWND nhWndPadre, const UINT nID, const wchar_t *nTexto, const int nIconoRecursos, const BOOL nActivado) : _Padre(nPadre), _Tipo(DMenuEx_Tipo_Texto), _hWndDest(nhWndPadre), _ID(nID), _Texto(nTexto), _Activado(nActivado), _MenuResaltado(NULL), _MenuPresionado(NULL) {
		_Recta = { 0, 0, 0, 0 };
		_Icono = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DMENUEX_TAMICONO, DMENUEX_TAMICONO);
	}

	DMenuEx::~DMenuEx(void) {
		Terminar();
	}

	const int DMenuEx::MostrarModal(DhWnd *nPadre) {
		Mostrar(nPadre);
		while (_hWnd != NULL) {
			MSG msg;
			if (TRUE == GetMessage(&msg, 0, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return 0;
	}


	void DMenuEx::Mostrar(DhWnd *nPadre) {
		POINT Punto;
		GetCursorPos(&Punto);

		// Calculo el tamaño que necesitará el menú
		POINT Tam = _CalcularMedidas();

		_hWndDest = nPadre->hWnd();

		if (_hWnd == NULL) {
			CrearVentana(nPadre, L"DMenuEx", L"", Punto.x, Punto.y, Tam.x, Tam.y, WS_POPUP);

			DMouse::CambiarCursor();
			Opacidad(230);

			AnimateWindow(_hWnd, 100, AW_HOR_POSITIVE | AW_VER_POSITIVE);

			// Asigno la posición del menú y lo hago siempre visible
			SetWindowPos(_hWnd, HWND_TOPMOST, Punto.x, Punto.y, Tam.x, Tam.y, SWP_NOACTIVATE);
			// Asigno la captura del mouse a este menú
			SetCapture(_hWnd);

			// Envio el mensaje WM_NCACTIVATE a la ventana principal para que no se vea como pierde el foco, y parezca que el desplegable es un hijo de la ventana principal
			SendMessage(nPadre->hWnd(), WM_NCACTIVATE, TRUE, 0);
		}
	}


	void DMenuEx::Ocultar(void) {
		AnimateWindow(_hWnd, 100, AW_HOR_NEGATIVE | AW_VER_NEGATIVE | AW_HIDE);
		DestroyWindow(_hWnd);
		_hWnd = NULL;
	}


	DMenuEx *DMenuEx::AgregarMenu(const UINT nID, const wchar_t *nTexto, const int nIconoRecursos, const int nPosicion, const BOOL nActivado) {
		int Pos = nPosicion;
		if (nPosicion == -1) {
			Pos = static_cast<int>(_Menus.size());
		}

//		_Icono = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DMENUEX_TAMICONO, DMENUEX_TAMICONO);

		DMenuEx *TmpMenu = new DMenuEx(this, DMenuEx_Tipo_Texto, _hWndDest, nID, nTexto, nIconoRecursos, nActivado);
		_Menus.insert(_Menus.begin() + Pos, TmpMenu);
		return TmpMenu;
	}


	DMenuEx *DMenuEx::AgregarSeparador(const int uID, const int nPosicion) {
		int Pos = nPosicion;
		if (nPosicion == -1) {
			Pos = static_cast<int>(_Menus.size());
		}

		DMenuEx *TmpMenu = new DMenuEx(this, DMenuEx_Tipo_Texto, _hWndDest, uID);
		_Menus.insert(_Menus.begin() + Pos, TmpMenu);
		return TmpMenu;
	}

	const BOOL DMenuEx::Activar(const BOOL nActivar) {
		_Activado = nActivar;
		return TRUE;
	}

	// Función que termina el menú y elimina la memória
	void DMenuEx::Terminar(void) {
		Ocultar();
		for (size_t i = 0; i < _Menus.size(); i++) {
			delete _Menus[i];
		}
		_Menus.resize(0);
		_hWndDest	= NULL;
		_Padre		= NULL;
		_Tipo		= DMenuEx_Tipo_Raiz;
		_ID			= 0;
	}



	void DMenuEx::Pintar(HDC hDC) {
		// Obtengo el tamaño de la ventana
		RECT RC;
		GetClientRect(_hWnd, &RC);

		// Creo un DC compatible para utilizar-lo de back buffer
		HDC		DC		 = CreateCompatibleDC(NULL);
		HBITMAP Bmp		 = CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(DC, Bmp));


		// Pinto el borde
		HBRUSH BrochaBorde = CreateSolidBrush(COLOR_MENU_BORDE);
		FillRect(DC, &RC, BrochaBorde);
		DeleteObject(BrochaBorde);

		// El texto se pintará sin fondo
		SetBkMode(DC, TRANSPARENT);

		for (size_t i = 0; i < _Menus.size(); i++) {
			if (_Menus[i]->_Tipo == DMenuEx_Tipo_Texto)		_PintarMenu(DC, _Menus[i]);
			else                                            _PintarSeparador(DC, _Menus[i]);
		}

		BitBlt(hDC, 0, 0, RC.right, RC.bottom, DC, 0, 0, SRCCOPY);

		// Elimino objetos gdi de la memoria
		SelectObject(DC, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(DC);
	}

	void DMenuEx::_PintarSeparador(HDC DC, DMenuEx *pMenu) {
		RECT RectaSeparador = { pMenu->_Recta.left + 10 , pMenu->_Recta.top + DMENUEX_MARGEN_Y,  pMenu->_Recta.right - 10 , pMenu->_Recta.bottom - DMENUEX_MARGEN_Y };

		// Pinto el fondo
		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_MENU_FONDO);
		FillRect(DC, &pMenu->_Recta, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Pinto el separador
		HBRUSH BrochaSeparador = CreateSolidBrush(COLOR_MENU_BORDE);
		FillRect(DC, &RectaSeparador, BrochaSeparador);
		DeleteObject(BrochaSeparador);
	}

	void DMenuEx::_PintarMenu(HDC DC, DMenuEx *pMenu) {
		int bPresionado = 0;
		
		// Obtengo el estado del menú
		DMenuEx_Estado Estado = DMenuEx_Estado_Normal;
		if (pMenu == _MenuResaltado) Estado = DMenuEx_Estado_Resaltado;
		// Si está presionado
		if (pMenu == _MenuPresionado) {
			bPresionado = 1;
			Estado = DMenuEx_Estado_Presionado;
		}
		// Si no está activado
		if (_Activado == FALSE) Estado = DMenuEx_Estado_Desactivado;

		// Elijo los colores del menú según su estado
		COLORREF ColFondo;
		COLORREF ColTexto;
		switch (Estado) {
			case DMenuEx_Estado_Normal:
				ColFondo = COLOR_MENU_FONDO;
				ColTexto = COLOR_MENU_TEXTO;
				break;
			case DMenuEx_Estado_Resaltado:
				ColFondo = COLOR_MENU_FONDO_RESALTADO;
				ColTexto = COLOR_MENU_TEXTO_RESALTADO;
				break;
			case DMenuEx_Estado_Presionado:
				ColFondo = COLOR_MENU_FONDO_PRESIONADO;
				ColTexto = COLOR_MENU_TEXTO_PRESIONADO;
				break;
			case DMenuEx_Estado_Desactivado:
				ColFondo = COLOR_MENU_FONDO;
				ColTexto = COLOR_MENU_TEXTO_DESACTIVADO;
				break;
		}

		// Pinto el fondo
		HBRUSH BrochaFondo = CreateSolidBrush(ColFondo);
		FillRect(DC, &pMenu->_Recta, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Pinto el icono
		int YIcono = static_cast<int>(static_cast<float>((pMenu->_Recta.bottom - pMenu->_Recta.top) - DMENUEX_TAMICONO) / 2.0f);
		DrawIconEx(DC, bPresionado + DMENUEX_MARGEN_X, bPresionado + pMenu->_Recta.top + YIcono, pMenu->_Icono->Icono(), DMENUEX_TAMICONO, DMENUEX_TAMICONO, 0, 0, DI_NORMAL);				

		// Pinto la sombra del texto
		SetTextColor(DC, COLOR_MENU_TEXTO_SOMBRA);
		TextOut(DC, 1 + bPresionado + DMENUEX_MARGEN_X + DMENUEX_TAMICONO + DMENUEX_MARGEN_X, 1 + pMenu->_Recta.top + DMENUEX_MARGEN_Y, pMenu->_Texto.c_str(), static_cast<int>(pMenu->_Texto.size()));
		// Pinto el texto
		SetTextColor(DC, ColTexto);
		TextOut(DC, bPresionado + DMENUEX_MARGEN_X + DMENUEX_TAMICONO + DMENUEX_MARGEN_X, pMenu->_Recta.top + DMENUEX_MARGEN_Y, pMenu->_Texto.c_str(), static_cast<int>(pMenu->_Texto.size()));
	}



	const POINT DMenuEx::_CalcularMedidas(void) {
		LONG  TmpAncho  = 0;
		POINT Ret		= { 0, 0 };
		SIZE  Tam		= { 0, 0 };
		HDC   hDC		= GetDC(NULL);
		HFONT VFont		= static_cast<HFONT>(SelectObject(hDC, _Fuente21Normal.Fuente()));

		for (size_t i = 0; i < _Menus.size(); i++) {
			switch (_Menus[i]->_Tipo) {
				case DMenuEx_Tipo_Texto :
					// Obtengo el tamaño del texto
					GetTextExtentPoint32(hDC, _Menus[i]->_Texto.c_str(), static_cast<int>(_Menus[i]->_Texto.size()), &Tam);

					// Calculo el ancho necesario
					// Margen + Ancho icono + Margen + Ancho texto + Margen + Acho icono + Margen
					TmpAncho = Tam.cx + (DMENUEX_TAMICONO * 2) + (DMENUEX_MARGEN_Y * 4);
					// Si el ancho temporal es mas grande que el ancho final, asigno el ancho temporal al ancho final
					if (TmpAncho > Ret.x) Ret.x = TmpAncho;

					// Calculo la altura necesaria
					// Si la altura del texto es menor que la altura del icono, asigno la altura del icono
					if (Tam.cy < DMENUEX_TAMICONO) Tam.cy = DMENUEX_TAMICONO;
					// Asigno la posición inicial del menú
					_Menus[i]->_Recta.top = Ret.y + DMENUEX_BORDE;
					// Sumo el margen superior, margen inferior, y altura del texto
					Ret.y += (DMENUEX_MARGEN_Y * 2) + Tam.cy;
					_Menus[i]->_Recta.bottom = Ret.y + DMENUEX_BORDE;
					break;
				case DMenuEx_Tipo_Separador :
					_Menus[i]->_Recta.top = Ret.y + DMENUEX_BORDE;
					Ret.y += (DMENUEX_MARGEN_Y * 2) + 1; // Espacio adicional para el separador
					_Menus[i]->_Recta.bottom = Ret.y + DMENUEX_BORDE;
					break;
			}
		}

		// Añado el borde
		Ret.x += (DMENUEX_BORDE * 2);
		Ret.y += (DMENUEX_BORDE * 2);

		// Des-selecciono la fuente y libero el DC
		SelectObject(hDC, VFont);
		ReleaseDC(NULL, hDC);

		// Asigno el ancho a todos los submenus, una vez se ha calculado
		for (size_t i = 0; i < _Menus.size(); i++) {
			_Menus[i]->_Recta.left = DMENUEX_BORDE;
			_Menus[i]->_Recta.right = Ret.x - DMENUEX_BORDE;
		}

		// Devuelve el tamaño total con bordes incluidos
		return Ret;
	}


	void DMenuEx::_MostrarSubMenu(HWND hWndDestMsg, DMenuEx *nPadre, const int cX, const int cY) {
		POINT Punto = { cX, cY };
//		GetCursorPos(&Punto);

		// Calculo el tamaño que necesitará el menú
		POINT Tam = _CalcularMedidas();

		if (_hWnd == NULL) {
			CrearVentana(nPadre, L"DMenuEx", L"", Punto.x, Punto.y, Tam.x, Tam.y, WS_POPUP);

			DMouse::CambiarCursor();
			Opacidad(230);

			AnimateWindow(_hWnd, 100, AW_HOR_POSITIVE | AW_VER_POSITIVE);

			// Asigno la posición del menú y lo hago siempre visible
			SetWindowPos(_hWnd, HWND_TOPMOST, Punto.x, Punto.y, Tam.x, Tam.y, SWP_NOACTIVATE);
			// Asigno la captura del mouse a este menú
			SetCapture(_hWnd);

			// Envio el mensaje WM_NCACTIVATE a la ventana principal para que no se vea como pierde el foco, y parezca que el desplegable es un hijo de la ventana principal
			SendMessage(hWndDestMsg, WM_NCACTIVATE, TRUE, 0);
		}

	}

	void DMenuEx::_Evento_Pintar(void) {
		PAINTSTRUCT PS;
		HDC DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}


	void DMenuEx::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {
		DEventoMouse	DatosMouse(wParam, lParam, _ID);
		POINT			Pt = { DatosMouse.X(), DatosMouse.Y() };
		RECT            RC;
		GetClientRect(_hWnd, &RC);
		//´El mouse está dentro de esta ventana
		if (PtInRect(&RC, Pt) == TRUE) {
			// Busco cual és el menú resaltado
			for (size_t i = 0; i < _Menus.size(); i++) {
				if (PtInRect(&_Menus[i]->_Recta, Pt) != FALSE) {
					// Si el menu resaltado no es el mismo (para no repintar siempre)
					if (_MenuResaltado != _Menus[i]) {
						_MenuResaltado = _Menus[i];
						// Si tiene Sub-menus
						if (_MenuResaltado->TotalMenus() > 0) {
							_MenuResaltado->_MostrarSubMenu(_hWndDest, this, RC.right, _MenuResaltado->_Recta.top);
						}
						Repintar();
					}
					break;
				}
			}
		}
		// El mouse está en otra ventana
		else {
			HWND hWndVentana = WindowFromPoint(Pt);
			// Si la ventana debajo del mouse pertenece a un MenuEx
			if (SendMessage(hWndVentana, WM_ESMENUEX, 0, 0) == WM_ESMENUEX) {
				// Paso la captura a la ventana del submenú
				SetCapture(hWndVentana);
			}
			// No es un SubMenu
			else {
				// Si hay algun menú resaltado
				if (_MenuResaltado != NULL) {
					_MenuResaltado = NULL;
					Repintar();
				}
			}
		}

	}

	void DMenuEx::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse	DatosMouse(wParam, lParam, _ID);
		POINT			Pt = { DatosMouse.X(), DatosMouse.Y() };
		for (size_t i = 0; i < _Menus.size(); i++) {
			if (PtInRect(&_Menus[i]->_Recta, Pt) != FALSE) {
				_MenuPresionado = _Menus[i];
				Repintar();
				return;
			}
		}
	}

	void DMenuEx::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
		_MenuPresionado = NULL;
		Ocultar();
		ReleaseCapture();
	}

	void DMenuEx::_Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) {

	}

	void DMenuEx::_Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {

	}

	void DMenuEx::_Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param) {

	}



	LRESULT CALLBACK DMenuEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT:			_Evento_Pintar();																															return 0;
			// Mouse movimiento
			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(wParam, lParam);																									return 0;
			// Mouse presionado
			case WM_LBUTTONDOWN:	_Evento_MousePresionado(0, wParam, lParam);																									return 0;
			case WM_RBUTTONDOWN:	_Evento_MousePresionado(1, wParam, lParam);																									return 0;
			case WM_MBUTTONDOWN:	_Evento_MousePresionado(2, wParam, lParam);																									return 0;
			// Mouse soltado
			case WM_LBUTTONUP:		_Evento_MouseSoltado(0, wParam, lParam);																									return 0;
			case WM_RBUTTONUP:		_Evento_MouseSoltado(1, wParam, lParam);																									return 0;
			case WM_MBUTTONUP:		_Evento_MouseSoltado(2, wParam, lParam);																									return 0;
			// Teclado
			case WM_KEYDOWN:		_Evento_TeclaPresionada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
			case WM_KEYUP:			_Evento_TeclaSoltada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
			case WM_CHAR:           _Evento_Tecla(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));																	return 0;
			
			// Print y Print Client (para AnimateWindow)
			case WM_PRINTCLIENT:    Pintar(reinterpret_cast<HDC>(wParam));																										return 0;

			case WM_ESMENUEX:																																					return WM_ESMENUEX;

		}
		return DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}

}