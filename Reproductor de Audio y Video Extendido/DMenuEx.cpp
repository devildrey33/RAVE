#include "stdafx.h"
#include "DMenuEx.h"
#include "DMouse.h"
#include "dwmapi.h"


namespace DWL {

	DMenuEx *DMenuEx::_ResultadoModal = NULL;


	DMenuEx::DMenuEx(void) : _Padre(NULL), _Tipo(DMenuEx_Tipo_Raiz), _hWndDest(NULL), _ID(0), _Activado(TRUE), _MenuResaltado(NULL), _MenuPresionado(NULL), _MenuDesplegado(NULL), _AnularMouseMove(NULL) {
		_Recta = { 0, 0, 0, 0 };
	}

	DMenuEx::DMenuEx(DMenuEx *nPadre, DMenuEx_Tipo nTipo, HWND nhWndPadre, const UINT nID) : _Padre(nPadre), _Tipo(DMenuEx_Tipo_Separador), _hWndDest(nhWndPadre), _ID(nID), _MenuResaltado(NULL), _MenuPresionado(NULL), _Activado(TRUE), _MenuDesplegado(NULL), _AnularMouseMove(NULL) {
		_Recta = { 0, 0, 0, 0 };
	}

	DMenuEx::DMenuEx(DMenuEx *nPadre, DMenuEx_Tipo nTipo, HWND nhWndPadre, const UINT nID, const wchar_t *nTexto, const int nIconoRecursos, const BOOL nActivado) : _Padre(nPadre), _Tipo(DMenuEx_Tipo_Texto), _hWndDest(nhWndPadre), _ID(nID), _Texto(nTexto), _Activado(nActivado), _MenuResaltado(NULL), _MenuPresionado(NULL), _MenuDesplegado(NULL), _AnularMouseMove(NULL) {
		_Recta = { 0, 0, 0, 0 };
		_Icono = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DMENUEX_TAMICONO, DMENUEX_TAMICONO);
	}

	DMenuEx::~DMenuEx(void) {
		Terminar();
	}

	// Función que muestra el menú, y espera a una respuesta por parte del usuario
	// Devuelve NULL si no se ha seleccionado ningún menú, o devuelve el menú seleccionado.
	DMenuEx *DMenuEx::MostrarModal(DhWnd *nPadre, const int PosX, const int PosY) {
		_ResultadoModal = NULL;
		Mostrar(nPadre, PosX, PosY);
		while (_hWnd != NULL) {
			MSG msg;
			if (TRUE == GetMessage(&msg, 0, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return _ResultadoModal;
	}

	DMenuEx *DMenuEx::MostrarModal(DhWnd *nPadre) {
		POINT Punto;
		GetCursorPos(&Punto);
		return MostrarModal(nPadre, Punto.x, Punto.y);
	}

	void DMenuEx::Mostrar(DhWnd *nPadre) {
		POINT Punto;
		GetCursorPos(&Punto);
		Mostrar(nPadre, Punto.x, Punto.y);
	}


	void DMenuEx::Mostrar(DhWnd *nPadre, const int PosX, const int PosY) {

		// Calculo el tamaño que necesitará el menú
		POINT Tam = _CalcularMedidas();

		_hWndDest = nPadre->hWnd();

		if (_hWnd == NULL) {
			CrearVentana(nPadre, L"DMenuEx", L"", PosX, PosY, Tam.x, Tam.y, WS_POPUP | WS_CAPTION);

			DMouse::CambiarCursor();
			Opacidad(230);
			MARGINS Margen = { 0, 0, 0, 1 };
			DwmExtendFrameIntoClientArea(_hWnd, &Margen);
			//AnimateWindow(_hWnd, 100, AW_HOR_POSITIVE | AW_VER_POSITIVE);

			// Asigno la posición del menú y lo hago siempre visible
			SetWindowPos(_hWnd, HWND_TOPMOST, PosX, PosY, Tam.x, Tam.y, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

			// Asigno la captura del mouse a este menú
			#if DMENUEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DMenuEx::Mostrar SetCapture : %d\n", _hWnd);
			#endif	
			SetCapture(_hWnd);
			SetFocus(_hWnd);

			// Envio el mensaje WM_NCACTIVATE a la ventana principal para que no se vea como pierde el foco, y parezca que el desplegable es un hijo de la ventana principal
			SendMessage(nPadre->hWnd(), WM_NCACTIVATE, TRUE, 0);
		}
	}

	// Función que oculta este menú 
	//	Si OcultarTodos es TRUE, se ocultaran todos los menus hijos de este menú, y todos los ancestros de este menú (padre, abuelo, etc..)
	//  Si OcultarTodos es FALSE, se ocultaran todos los menus hijos de este menú, y este menú.
	void DMenuEx::Ocultar(const BOOL OcultarTodos) {
//		AnimateWindow(_hWnd, 100, AW_HOR_NEGATIVE | AW_VER_NEGATIVE | AW_HIDE);
		DestroyWindow(_hWnd);
		_hWnd			= NULL;
		_MenuResaltado	= NULL;

		// Oculto todos los menús hijos de este
		_OcultarRecursivo(this);

		if (OcultarTodos == TRUE) {
			// Oculto los menus ancestros
			DMenuEx *pPadre = _Padre;
			while (pPadre != NULL) {
				pPadre->Ocultar(FALSE);
				pPadre = pPadre->_Padre;
			}
		}
		// Si no hay que ocultar todos los menus (si lo hacemos en el menú raíz, es como si ocultaramos todo, por lo que hay que comprobar si tiene padre)
		else {
			// Miro si tiene padre, y en caso afirmativo, asigno el foco al padre.
			if (_Padre != NULL) {
				SetFocus(_Padre->_hWnd);
			}
		}
	}

	// Oculta todos los menus hijos recursivamente
	void DMenuEx::_OcultarRecursivo(DMenuEx *oMenu) {
		for (size_t i = 0; i < oMenu->_Menus.size(); i++) {
			if (oMenu->_Menus[i]->_Menus.size() > 0) {
				oMenu->_Menus[i]->Ocultar(FALSE);
			}
		}
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

	void DMenuEx::Activado(const BOOL nActivar) {
		_Activado = nActivar;
	}

	// Función que termina el menú y elimina la memória
	void DMenuEx::Terminar(void) {
		#if DMENUEX_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DMenuEx::Terminar Ocultar menu\n");
		#endif
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


	void DMenuEx::Icono(const int nIconoRecursos) {
		_Icono = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DMENUEX_TAMICONO, DMENUEX_TAMICONO);
	}


	void DMenuEx::Pintar(HDC hDC) {
		// Obtengo el tamaño de la ventana
		RECT RC;
		GetClientRect(_hWnd, &RC);

		// Creo un DC compatible para utilizar-lo de back buffer
		HDC		DC		 = CreateCompatibleDC(NULL);
		HBITMAP Bmp		 = CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(DC, Bmp));
		HFONT	VFont	 = static_cast<HFONT>(SelectObject(DC, _Fuente21Normal.Fuente()));

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
		SelectObject(DC, VFont);
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
		HBRUSH BrochaSeparador = CreateSolidBrush(COLOR_MENU_SEPARADOR);
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
		if (pMenu->_Activado == FALSE) Estado = DMenuEx_Estado_Desactivado;

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
		int YIcono = static_cast<int>((static_cast<float>(pMenu->_Recta.bottom - pMenu->_Recta.top) - static_cast<float>(DMENUEX_TAMICONO)) / 2.0f);
		DrawIconEx(DC, bPresionado + DMENUEX_MARGEN_X, bPresionado + pMenu->_Recta.top + YIcono, pMenu->_Icono->Icono(), DMENUEX_TAMICONO, DMENUEX_TAMICONO, 0, 0, DI_NORMAL);				

		// Pinto la sombra del texto
		SetTextColor(DC, COLOR_MENU_TEXTO_SOMBRA);
		TextOut(DC, 1 + bPresionado + DMENUEX_MARGEN_X + DMENUEX_TAMICONO + DMENUEX_MARGEN_X, 1 + pMenu->_Recta.top + DMENUEX_MARGEN_Y, pMenu->_Texto.c_str(), static_cast<int>(pMenu->_Texto.size()));
		// Pinto el texto
		SetTextColor(DC, ColTexto);
		TextOut(DC, bPresionado + DMENUEX_MARGEN_X + DMENUEX_TAMICONO + DMENUEX_MARGEN_X, pMenu->_Recta.top + DMENUEX_MARGEN_Y, pMenu->_Texto.c_str(), static_cast<int>(pMenu->_Texto.size()));
		
		// Si el menú tiene submenus
		if (pMenu->_Menus.size() > 0) {
			// Pinto el expansor
			_PintarExpansor(DC, pMenu->_Recta.right - (DMENUEX_TAMICONO + DMENUEX_MARGEN_X), pMenu->_Recta.top + YIcono);
		}
	}


	void DMenuEx::_PintarExpansor(HDC DC, const int eX, const int eY) {
		HPEN Pluma  = CreatePen(PS_SOLID, 1, COLOR_MENU_TEXTO);
		HPEN VPluma = static_cast<HPEN>(SelectObject(DC, Pluma));
		int MedioIcono = (DMENUEX_TAMICONO / 2);
		MoveToEx(DC, eX + MedioIcono, eY, NULL);
		LineTo(DC, eX + DMENUEX_TAMICONO, eY + MedioIcono);
		LineTo(DC, eX + MedioIcono, eY + DMENUEX_TAMICONO);
		SelectObject(DC, VPluma);
		DeleteObject(Pluma);
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
					TmpAncho = Tam.cx + (DMENUEX_TAMICONO * 2) + (DMENUEX_MARGEN_X * 4);
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


	void DMenuEx::_MostrarSubMenu(HWND hWndDestMsg, DMenuEx *nPadre, const int cX, const int cY, const BOOL AsignarFoco) {

		if (_hWnd != NULL) return;
		#if DMENUEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DMenuEx::_MostrarSubMenu AsignarFoco = %d\n", AsignarFoco);
		#endif
		POINT Punto = { cX, cY };
		RECT WR;
		GetWindowRect(nPadre->hWnd(), &WR);

		// Calculo el tamaño que necesitará el menú
		POINT Tam = _CalcularMedidas();

		CrearVentana(nPadre, L"DMenuEx", L"", -4 + WR.left + Punto.x, -1 + WR.top + Punto.y, Tam.x, Tam.y, WS_POPUP | WS_CAPTION);

		DMouse::CambiarCursor();
		Opacidad(230);

		MARGINS Margen = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(_hWnd, &Margen);
		//AnimateWindow(_hWnd, 100, AW_HOR_POSITIVE | AW_VER_POSITIVE);

		// Asigno la posición del menú y lo hago siempre visible
		SetWindowPos(_hWnd, HWND_TOPMOST, -4 + WR.left + Punto.x, -1 + WR.top + Punto.y, Tam.x, Tam.y, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		// Asigno el foco a este menú
		if (AsignarFoco == TRUE) SetFocus(_hWnd);

		// Envio el mensaje WM_NCACTIVATE a la ventana principal para que no se vea como pierde el foco, y parezca que el desplegable es un hijo de la ventana principal
		SendMessage(hWndDestMsg, WM_NCACTIVATE, TRUE, 0);

	}

	void DMenuEx::_Evento_Pintar(void) {
		PAINTSTRUCT PS;
		HDC DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}


	void DMenuEx::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {
		if (_AnularMouseMove == TRUE) {
			_AnularMouseMove = FALSE;
			return;
		}

		DEventoMouse	DatosMouse(wParam, lParam, _ID);
		POINT			Pt = { DatosMouse.X(), DatosMouse.Y() };
		RECT            RC;
		GetClientRect(_hWnd, &RC);		
		//´El mouse está dentro de esta ventana
		if (PtInRect(&RC, Pt) == TRUE) {
			#if DMENUEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DMenuEx::Evento_MouseMovimiento hWnd : %d\n", _hWnd);
			#endif
			// Busco cual és el menú resaltado
			for (size_t i = 0; i < _Menus.size(); i++) {
				// Si el mouse está dentro de este menú
				if (PtInRect(&_Menus[i]->_Recta, Pt) != FALSE) {
					// Si el submenu resaltado no es el mismo (para no repintar siempre)
					if (_MenuResaltado != _Menus[i]) {
						// Oculto el ultimo menú desplegado
						if (_MenuDesplegado != _Menus[i] && _MenuDesplegado != NULL) {
							#if DMENUEX_MOSTRARDEBUG == TRUE
								Debug_Escribir(L"DMenuEx::Evento_MouseMovimiento Ocultar menu desplegado\n");
							#endif
							_MenuDesplegado->Ocultar();
							_MenuDesplegado = NULL;
						}

						SetFocus(_hWnd);
						// Asigno el nuevo menú resaltado
						_MenuResaltado = _Menus[i];

						// Si tiene Sub-menus
						if (_MenuResaltado->TotalMenus() > 0) {
							_MenuResaltado->_MostrarSubMenu(_hWndDest, this, RC.right, _MenuResaltado->_Recta.top);
							_MenuDesplegado = _MenuResaltado;
						}
						Repintar();
					}
					break;
				}
			}
		}
		// El mouse está en otra ventana
		else {
			RECT RW;
			GetWindowRect(_hWnd, &RW);
			POINT Pt2 = { DatosMouse.X() + RW.left, DatosMouse.Y() + RW.top };
			HWND hWndVentana = WindowFromPoint(Pt2);

			// Si la ventana debajo del mouse pertenece a un MenuEx
			if (SendMessage(hWndVentana, WM_ESMENUEX, 0, 0) == WM_ESMENUEX) {
				// Paso la captura a la ventana del submenú
				SetCapture(hWndVentana);
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DMenuEx::Evento_MouseMovimiento SetCapture hWnd : %d\n", hWndVentana);
				#endif
			}
			// No es un SubMenu
			else {
//				#if DMENUEX_MOSTRARDEBUG == TRUE
//					Debug_Escribir(L"DMenuEx::Evento_MouseMovimiento NO ES UN SUBMENU\n");
//				#endif
				// Si hay algún sub-menu desplegado
/*				if (_MenuDesplegado != NULL) {
					_MenuDesplegado->Ocultar();
					_MenuDesplegado = NULL;
				}*/

				// Si hay algun menú resaltado
				if (_MenuResaltado != NULL) {
					if (_MenuResaltado->TotalMenus() == 0) {
						_MenuResaltado = NULL;
						Repintar();
					}
				}

			}
		}

	}

	void DMenuEx::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {

		POINT Pt;
		DWL::DMouse::ObtenerPosicion(&Pt);
		HWND WFP = WindowFromPoint(Pt);
		// Si la ventana debajo del mouse no es un MenuEx
		if (SendMessage(WFP, WM_ESMENUEX, 0, 0) != WM_ESMENUEX) {
			SetFocus(WFP);
		}


		DEventoMouse	DatosMouse(wParam, lParam, _ID);
		Pt = { DatosMouse.X(), DatosMouse.Y() };
		for (size_t i = 0; i < _Menus.size(); i++) {
			if (PtInRect(&_Menus[i]->_Recta, Pt) != FALSE) {
				_MenuPresionado = _Menus[i];
				Repintar();
				return;
			}
		}
	}

	void DMenuEx::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
		ReleaseCapture();
		_ResultadoModal = _MenuPresionado;
		_MenuPresionado = NULL;
		#if DMENUEX_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DMenuEx::_Evento_MouseSoltado Ocultar menu\n");
		#endif
		Ocultar(TRUE);

		POINT Pt;
		DWL::DMouse::ObtenerPosicion(&Pt);
		HWND WFP = WindowFromPoint(Pt);
		// Si la ventana debajo del mouse no es un MenuEx
		if (SendMessage(WFP, WM_ESMENUEX, 0, 0) != WM_ESMENUEX) {			
			SetFocus(WFP);
		}
	}

	void DMenuEx::_Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		DhWnd::_Teclado[Caracter] = true;
		
		// Obtengo la posición del menu resaltado dentro del vector
		int MenuPos = -1;
		if (_MenuResaltado != NULL) {
			for (MenuPos = 0; MenuPos < _Menus.size(); MenuPos++) {
				if (_Menus[MenuPos] == _MenuResaltado) {
					break;
				}
			}
		}

		// Elijo el menu resaltado según la tecla presionada
		switch (Caracter) {
			case VK_HOME : 
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada VK_HOME\n");
				#endif
				_MenuResaltado = _Menus[0];
				break;
			case VK_END:
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada VK_END\n");
				#endif
				_MenuResaltado = _Menus[_Menus.size() - 1];
				break;
			case VK_DOWN :
				if (MenuPos < static_cast<int>(_Menus.size() - 1))	MenuPos ++;
				// Si el menu resaltado tiene un siguiente menú
				if (MenuPos < static_cast<int>(_Menus.size() - 1)) {
					// Si es un separador lo salto
					if (_Menus[MenuPos]->_Tipo == DMenuEx_Tipo_Separador) MenuPos++;
				}
				// Si el menu resaltado no tiene un siguiente menu
				else {
					// Si es un separador, vuelvo atras
					if (_Menus[MenuPos]->_Tipo == DMenuEx_Tipo_Separador) MenuPos--;
				}
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada VK_DOWN\n");
				#endif
				if (MenuPos == -1)	_MenuResaltado = NULL;
				else				_MenuResaltado = _Menus[MenuPos];
				_AnularMouseMove = TRUE;
				break;
			case VK_UP:
				if (MenuPos > 0)	MenuPos--;
				// Si el menu resaltado tiene un menú anterior
				if (MenuPos > 0) {
					// Si es un separador lo salto
					if (_Menus[MenuPos]->_Tipo == DMenuEx_Tipo_Separador) MenuPos--;
				}
				// Si el menu resaltado no tiene un menu anterior
				else {
					// Si es un separador, vuelvo atras
					if (MenuPos != -1) {
						if (_Menus[MenuPos]->_Tipo == DMenuEx_Tipo_Separador) MenuPos++;
					}
				}
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada VK_UP\n");
				#endif
				if (MenuPos == -1)	_MenuResaltado = NULL;
				else				_MenuResaltado = _Menus[MenuPos];
				_AnularMouseMove = TRUE;
				break;
			case VK_LEFT :
				if (_Padre != NULL) {
					_MenuResaltado = NULL;
					SetFocus(_Padre->_hWnd);
					Repintar();
					SetCapture(_Padre->_hWnd);
				}
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada VK_LEFT\n");
				#endif
				return;
			case VK_RIGHT :
				if (_MenuResaltado != NULL) {
					if (_MenuResaltado->_Menus.size() > 0) {
						_MenuResaltado->_MenuResaltado = _MenuResaltado->_Menus[0];
						SetFocus(_MenuResaltado->_hWnd);
						_MenuResaltado->Repintar();
						SetCapture(_MenuResaltado->_hWnd);
					}
				}
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada VK_RIGHT\n");
				#endif
				return;
			case VK_ESCAPE :
				Ocultar(TRUE);
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada VK_ESCAPE\n");
				#endif
				return;
		}

		// Si hay un sub.menu desplegado
		if (_MenuDesplegado != NULL) {
			// Si el menú desplegado no es el mismo que el menú resaltado
			if (_MenuDesplegado != _MenuResaltado) {
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada Ocultar menu desplegado\n");
				#endif
				_MenuDesplegado->Ocultar();
				_MenuDesplegado = NULL;
			}
		}

		RECT RC;
		GetClientRect(_hWnd, &RC);
		SetFocus(_hWnd);

		if (_MenuResaltado != NULL) {
			// Si tiene Sub-menus
			if (_MenuResaltado->TotalMenus() > 0) {
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DMenuEx::_Evento_TeclaPresionada MostrarSubmenu sin foco\n");
				#endif
				_MenuResaltado->_MostrarSubMenu(_hWndDest, this, RC.right, _MenuResaltado->_Recta.top, FALSE);
				_MenuDesplegado = _MenuResaltado;				
			}
		}
		Repintar();

	}

	void DMenuEx::_Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		DhWnd::_Teclado[Caracter] = false;

	}

	void DMenuEx::_Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param) {

	}

	void DMenuEx::_Evento_FocoPerdido(HWND UltimoFoco) {
		// El nuevo foco no es parte de un MenuEx
		if (SendMessage(UltimoFoco, WM_ESMENUEX, 0, 0) != WM_ESMENUEX) {
			POINT Pt;
			DWL::DMouse::ObtenerPosicion(&Pt);
			HWND VentanaDebajoMouse = WindowFromPoint(Pt);
			// La ventana que hay debajo del ratón no es un MenuEx
			if (SendMessage(VentanaDebajoMouse, WM_ESMENUEX, 0, 0) != WM_ESMENUEX) {
				#if DMENUEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DMenuEx::GestorMensajes WM_KILLFOCUS Ocultar menu hWnd %d %d\n", UltimoFoco, VentanaDebajoMouse);
				#endif
				Ocultar(TRUE);
			}
			else {
				SetFocus(VentanaDebajoMouse);
			}
		}
	}


	void DMenuEx::_Evento_PintarNC(HRGN Region) {
		HDC    DC		= GetDCEx(_hWnd, Region, DCX_WINDOW | DCX_INTERSECTRGN);
		HBRUSH Brocha	= CreateSolidBrush(COLOR_MENU_FONDO);
		RECT   RV;
		GetWindowRect(_hWnd, &RV);
		RECT   RC = { 0, 0, 1, RV.right - RV.left };

		FillRect(DC, &RC, Brocha);

		DeleteObject(Brocha);
		ReleaseDC(_hWnd, DC);
	}


	LRESULT CALLBACK DMenuEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		

			case WM_PAINT:			_Evento_Pintar();																															return 0;
//			case WM_NCPAINT:		_Evento_PintarNC((HRGN)wParam);																												return 0;
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
			// Si pierde el foco, compruebo que el nuevo foco vaya a otro MenuEx, o en caso contrario oculto todo.
			case WM_KILLFOCUS:		_Evento_FocoPerdido(reinterpret_cast<HWND>(wParam));																						return 0;
			// Consulta para determinar si la ventana es parte de un MenuEx
			case WM_ESMENUEX:																																					return WM_ESMENUEX;
			
			// Sombra de la ventana
			// https://stackoverflow.com/questions/43818022/borderless-window-with-drop-shadow
			case WM_NCCALCSIZE:
				if (wParam == TRUE)	{
					// DWL_MSGRESULT (no esta definit)
					SetWindowLongPtr(_hWnd, 0, 0);
					return TRUE;
				}				
				return 0;
			// Evita que al obtener el foco cambie la parte del caption
			case WM_NCACTIVATE:																																					return FALSE;
		}
		return DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}

}