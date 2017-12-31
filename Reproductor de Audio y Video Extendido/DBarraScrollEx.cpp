#include "stdafx.h"
#include "DBarraScrollEx.h"

namespace DWL {

	DBarraScrollEx::DBarraScrollEx(void) :  DControlEx_FondoEstatico(), _Scroll_PosPresionado({ 0 ,0 }), _Scroll_PosInicio(0.0f),
											_ColorScroll(COLOR_SCROLL_BARRA),		_ColorScrollResaltado(COLOR_SCROLL_BARRA_RESALTADO),
											_ColorFondoScroll(COLOR_SCROLL_FONDO),  _ColorScrollPresionado(COLOR_SCROLL_BARRA_PRESIONADO),
											_ScrollV_Estado(DBarraScrollEx_Estado_Invisible), _ScrollV_Pagina(100.0f), _ScrollV_Posicion(0.0f),
											_ScrollH_Estado(DBarraScrollEx_Estado_Invisible), _ScrollH_Pagina(100.0f), _ScrollH_Posicion(0.0f) {
	}


	DBarraScrollEx::~DBarraScrollEx(void) {
	}
	

	void DBarraScrollEx::Scrolls_Pintar(HDC hDC, RECT &RC) {
		if (_ScrollV_Estado == DBarraScrollEx_Estado_Invisible && _ScrollH_Estado == DBarraScrollEx_Estado_Invisible) return;
		BOOL PintarRecuadro = TRUE;
		RECT RCV, RCH, RCBV, RCBH;
		ObtenerRectasScroll(RC, RCH, RCV);
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible)	ObtenerRectaBarraScrollV(RCV, RCBV);
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible)	ObtenerRectaBarraScrollH(RCH, RCBH);
		switch (_ScrollV_Estado) {
			case DBarraScrollEx_Estado_Normal		:	_PintarBarraScrollEx(hDC, RCV, RCBV, _ColorScroll);					break;
			case DBarraScrollEx_Estado_Resaltado	:	_PintarBarraScrollEx(hDC, RCV, RCBV, _ColorScrollResaltado);		break;
			case DBarraScrollEx_Estado_Presionado	:	_PintarBarraScrollEx(hDC, RCV, RCBV, _ColorScrollPresionado);		break;
			case DBarraScrollEx_Estado_Invisible	:   PintarRecuadro = FALSE;												break;
		}
		switch (_ScrollH_Estado) {
			case DBarraScrollEx_Estado_Normal		:	_PintarBarraScrollEx(hDC, RCH, RCBH, _ColorScroll);					break;
			case DBarraScrollEx_Estado_Resaltado	:	_PintarBarraScrollEx(hDC, RCH, RCBH, _ColorScrollResaltado);		break;
			case DBarraScrollEx_Estado_Presionado	:	_PintarBarraScrollEx(hDC, RCH, RCBH, _ColorScrollPresionado);		break;
			case DBarraScrollEx_Estado_Invisible	:   PintarRecuadro = FALSE;												break;
		}
		// Si las dos barras son visibles hay que pintar un recuadro en la parte inferior derecha del control 
		if (PintarRecuadro == TRUE) {
			RECT Recuadro = { RCV.left, RCH.top, RCV.right, RCH.bottom };
			HBRUSH BrochaRecuadro = CreateSolidBrush(_ColorFondoScroll);
			FillRect(hDC, &Recuadro, BrochaRecuadro);
			DeleteObject(BrochaRecuadro);
		}
	}

	/* Obtiene el área que pertenece al control (RectaCliente es el resultado de GetClientRect, y RectaClienteSinScroll es el área del control excluyendo las barras de scroll) */
	void DBarraScrollEx::ObtenerRectaCliente(RECT *RectaCliente, RECT *RectaClienteSinScroll) {
		GetClientRect(hWnd(), RectaCliente);
		*RectaClienteSinScroll = *RectaCliente;
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) RectaClienteSinScroll->right	-= TAM_BARRA_SCROLL;
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) RectaClienteSinScroll->bottom	-= TAM_BARRA_SCROLL;
	}


	/* Obtiene el área que pertenece los scrolls Horizontal y Vertical */
	void DBarraScrollEx::ObtenerRectasScroll(RECT &RC, RECT &RectaH, RECT &RectaV) {
		RectaH = RC; RectaH.top = RectaH.bottom - TAM_BARRA_SCROLL;
		RectaV = RC; RectaV.left = RectaV.right - TAM_BARRA_SCROLL;
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible && _ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			RectaH.right -= TAM_BARRA_SCROLL;
			RectaV.bottom -= TAM_BARRA_SCROLL;
		}
	}

	/* Obtiene el área que pertenece los scrolls Horizontal y Vertical */
	void DBarraScrollEx::ObtenerRectasScroll(RECT &RectaH, RECT &RectaV) {
		RECT Recta;
		GetClientRect(hWnd(), &Recta);
		RectaH = Recta; RectaH.top = RectaH.bottom - TAM_BARRA_SCROLL;
		RectaV = Recta; RectaV.left = RectaV.right - TAM_BARRA_SCROLL;
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible && _ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			RectaH.right -= TAM_BARRA_SCROLL;
			RectaV.bottom -= TAM_BARRA_SCROLL;
		}
	}

	/* Obtiene el área de la barra de scroll vertical (sin el fondo) */
	void DBarraScrollEx::ObtenerRectaBarraScrollV(RECT &RectaScroll, RECT &RectaBarra) {
		float Parte	= static_cast<float>(RectaScroll.bottom / 100.0f);
		RectaBarra  = {	RectaScroll.left + 2,																// left
						static_cast<int>(_ScrollV_Posicion * Parte) + 2,									// top
						RectaScroll.right - 2,																// right
						static_cast<int>((_ScrollV_Posicion * Parte) + (_ScrollV_Pagina * Parte)) - 2   };	// bottom
	}

	/* Obtiene el área de la barra de scroll horizontal (sin el fondo) */
	void DBarraScrollEx::ObtenerRectaBarraScrollH(RECT &RectaScroll, RECT &RectaBarra) {
		float Parte	= static_cast<float>(RectaScroll.right / 100.0f);
		RectaBarra  = { static_cast<int>(_ScrollH_Posicion * Parte) + 2,									// left
						RectaScroll.top + 2,																// top
						static_cast<int>((_ScrollH_Posicion * Parte) + (_ScrollH_Pagina * Parte)) - 2,		// right
						RectaScroll.bottom - 2															};  // bottom
	}


	const float DBarraScrollEx::_CalcularPosScrollH(const UINT nTam, const int nPos) {		
		float NuevaPos = _Scroll_PosInicio + (100.0f / static_cast<float>(nTam)) * static_cast<float>(nPos - _Scroll_PosPresionado.x);
		if (NuevaPos > 100.0f - _ScrollH_Pagina)	NuevaPos = (100.0f - _ScrollH_Pagina);
		if (NuevaPos < 0)							NuevaPos = 0;
		return NuevaPos;
	}

	const float DBarraScrollEx::_CalcularPosScrollV(const UINT nTam, const int nPos) {
		float NuevaPos = _Scroll_PosInicio + (100.0f / static_cast<float>(nTam)) * static_cast<float>(nPos - _Scroll_PosPresionado.y);
		if (NuevaPos > 100.0f - _ScrollV_Pagina)	NuevaPos = (100.0f - _ScrollV_Pagina);
		if (NuevaPos < 0)							NuevaPos = 0;
		return NuevaPos;
	}

	// Devuelve TRUE si el mouse está dentro de alguna barra de scroll
	const BOOL DBarraScrollEx::Scrolls_MouseMovimiento(const int cX, const int cY, const UINT Param) {
		RECT RCV, RCH;
		ObtenerRectasScroll(RCH, RCV);
		POINT Pt = { cX, cY };
		BOOL  RetV = FALSE, RetH = FALSE;
		// Compruebo el estado vertical
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			if (_ScrollV_Estado != DBarraScrollEx_Estado_Presionado) {
				RetV = PtInRect(&RCV, Pt);
				if (RetV == TRUE && _ScrollV_Estado != DBarraScrollEx_Estado_Resaltado) {
					_ScrollV_Estado = DBarraScrollEx_Estado_Resaltado;
					Repintar();
					return TRUE;
				}
				if (RetV == FALSE && _ScrollV_Estado != DBarraScrollEx_Estado_Normal) {
					_ScrollV_Estado = DBarraScrollEx_Estado_Normal;
					Repintar();
					return TRUE;
				}
			}
			else { // ScrollV presionado
				_ScrollV_Posicion = _CalcularPosScrollV(RCH.bottom, cY);
				Debug_Escribir_Varg(L"Scrolls_MouseMovimiento V:%.02f\n", _ScrollV_Posicion);
				Scrolls_EventoCambioPosicion();
				Repintar();
				return TRUE;
			}
		}
		// Compruebo el estado horizontal
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) {
			if (_ScrollH_Estado != DBarraScrollEx_Estado_Presionado) {
				RetH = PtInRect(&RCH, Pt);
				if (RetH == TRUE && _ScrollH_Estado != DBarraScrollEx_Estado_Resaltado) {
					_ScrollH_Estado = DBarraScrollEx_Estado_Resaltado;
					Repintar();
					return TRUE;
				}
				if (RetH == FALSE && _ScrollH_Estado != DBarraScrollEx_Estado_Normal) {
					_ScrollH_Estado = DBarraScrollEx_Estado_Normal;
					Repintar();
					return TRUE;
				}
			}
			else { // ScrollH presionado
				_ScrollH_Posicion = _CalcularPosScrollH(RCH.right, cX);
				Debug_Escribir_Varg(L"Scrolls_MouseMovimiento H:%.02f\n", _ScrollH_Posicion);
				Scrolls_EventoCambioPosicion();
				Repintar();
				return TRUE;
			}
		}
		
		return FALSE;
	}

	// Devuelve TRUE si el mouse está dentro de alguna barra de scroll
	const BOOL DBarraScrollEx::Scrolls_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param) {
		if (Boton != 0) return FALSE;
		RECT RCV, RCH, RCBV, RCBH;
		ObtenerRectasScroll(RCH, RCV);
		POINT Pt = { cX, cY };
		BOOL  RetV = FALSE, RetH = FALSE, RetBV = FALSE, RetBH = FALSE;
		float NuevaPos = 0.0f;
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			RetV = PtInRect(&RCV, Pt);
			if (RetV == TRUE) { // Está dentro del área del scroll vertical
				ObtenerRectaBarraScrollV(RCV, RCBV);
				RetBV = PtInRect(&RCBV, Pt);
				if (RetBV == FALSE) { // Está en la parte del fondo (recalculo la posición de la barra antes de empezar el drag)
					NuevaPos = (100.0f / static_cast<float>(RCV.bottom)) * static_cast<float>(cY);
					if (NuevaPos > _ScrollV_Posicion) _ScrollV_Posicion = NuevaPos - _ScrollV_Pagina;
					else                              _ScrollV_Posicion = NuevaPos;
				}
				SetCapture(hWnd());
				_Scroll_PosPresionado = { cX, cY };			// Posición del mouse desde donde se inicia el drag de la barra
				_Scroll_PosInicio     = _ScrollV_Posicion;	// Posición del scroll al iniciar el drag
				_ScrollV_Estado		  = DBarraScrollEx_Estado_Presionado;
				Debug_Escribir_Varg(L"Scrolls_MousePresionado V:%.02f\n", _ScrollV_Posicion);
				Scrolls_EventoCambioPosicion();
				Repintar();
				return TRUE;
			}
		}
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) {
			RetH = PtInRect(&RCH, Pt);
			if (RetH == TRUE) { // Está dentro del área del scroll horizontal
				ObtenerRectaBarraScrollH(RCH, RCBH);
				RetBH = PtInRect(&RCBH, Pt);
				if (RetBH == FALSE) { // Está en la parte del fondo (recalculo la posición de la barra antes de empezar el drag)
					NuevaPos = (100.0f / static_cast<float>(RCH.right)) * static_cast<float>(cX);
					if (NuevaPos > _ScrollH_Posicion) _ScrollH_Posicion = NuevaPos - _ScrollH_Pagina;
					else                              _ScrollH_Posicion = NuevaPos;
				}
				SetCapture(hWnd());
				_Scroll_PosPresionado = { cX, cY };			// Posición desde donde se inicia el drag de la barra
				_Scroll_PosInicio	  = _ScrollH_Posicion;	// Posición del scroll al iniciar el drag
				_ScrollH_Estado		  = DBarraScrollEx_Estado_Presionado;
				Debug_Escribir_Varg(L"Scrolls_MousePresionado H:%.02f\n", _ScrollH_Posicion);
				Scrolls_EventoCambioPosicion();
				Repintar();
				return TRUE;
			}
		}
		
		return FALSE;		
	}

	// Devuelve TRUE si el mouse está dentro de alguna barra de scroll
	const BOOL DBarraScrollEx::Scrolls_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param) {
		if (Boton != 0) return FALSE;
		RECT RCV, RCH;
		ObtenerRectasScroll(RCH, RCV);
		POINT Pt = { cX, cY };
		BOOL  RetV = FALSE, RetH = FALSE;
		ReleaseCapture();
		if (_ScrollV_Estado == DBarraScrollEx_Estado_Presionado) {
			RetV = PtInRect(&RCV, Pt);
			if (RetV == TRUE) _ScrollV_Estado = DBarraScrollEx_Estado_Resaltado;
			else              _ScrollV_Estado = DBarraScrollEx_Estado_Normal;
			_ScrollV_Posicion = _CalcularPosScrollV(RCH.bottom, cY);
			Debug_Escribir_Varg(L"Scrolls_MouseSoltado V:%.02f\n", _ScrollV_Posicion);
			Scrolls_EventoCambioPosicion();
			Repintar();
			return TRUE;
		}
		if (_ScrollH_Estado == DBarraScrollEx_Estado_Presionado) {
			RetH = PtInRect(&RCH, Pt);
			if (RetH == TRUE) _ScrollH_Estado = DBarraScrollEx_Estado_Resaltado;
			else              _ScrollH_Estado = DBarraScrollEx_Estado_Normal;
			_ScrollH_Posicion = _CalcularPosScrollH(RCH.right, cX);
			Debug_Escribir_Varg(L"Scrolls_MouseSoltado H:%.02f\n", _ScrollH_Posicion);
			Scrolls_EventoCambioPosicion();
			Repintar();
			return TRUE;
		}

		return FALSE;		
	}

	void DBarraScrollEx::_PintarBarraScrollEx(HDC hDC, RECT &RectaScroll, RECT &RectaBarra, const COLORREF pColorBarra) {
		// Creo las brochas 
		HBRUSH ColorFondo = CreateSolidBrush(_ColorFondoScroll);
		HBRUSH ColorBarra = CreateSolidBrush(pColorBarra);

		// Pinto el scrollbar
		FillRect(hDC, &RectaScroll, ColorFondo);
		FillRect(hDC, &RectaBarra, ColorBarra);

		// Borro las brochas de la memória
		DeleteObject(ColorFondo);
		DeleteObject(ColorBarra);
	}


	void DBarraScrollEx::ScrollH_Pagina(const float nPagina) {
		_ScrollH_Pagina = nPagina;
		Repintar();
	}

	void DBarraScrollEx::ScrollH_Posicion(const float nPosicion) {
		_ScrollH_Posicion = nPosicion;
		Repintar();
	}

	const float DBarraScrollEx::ScrollH_Pagina(void) {
		return _ScrollH_Pagina;
	}

	const float DBarraScrollEx::ScrollH_Posicion(void) {
		return _ScrollH_Posicion;
	}

	void DBarraScrollEx::ScrollH_Visible(const BOOL nVisible) {
		if (nVisible == TRUE)	_ScrollH_Estado = DBarraScrollEx_Estado_Normal;
		else                    _ScrollH_Estado = DBarraScrollEx_Estado_Invisible;
	}

	void DBarraScrollEx::ScrollV_Pagina(const float nPagina) {
		_ScrollV_Pagina = nPagina;
		Repintar();
	}

	void DBarraScrollEx::ScrollV_Posicion(const float nPosicion) {
		_ScrollV_Posicion = nPosicion;
		Repintar();
	}

	const float DBarraScrollEx::ScrollV_Pagina(void) {
		return _ScrollV_Pagina;
	}

	const float DBarraScrollEx::ScrollV_Posicion(void) {
		return _ScrollV_Posicion;
	}

	void DBarraScrollEx::ScrollV_Visible(const BOOL nVisible) {
		if (nVisible == TRUE)	_ScrollV_Estado = DBarraScrollEx_Estado_Normal;
		else                    _ScrollV_Estado = DBarraScrollEx_Estado_Invisible;

	}

	// Devuelve TRUE si es necesario repintar, FALSE en caso contrario
	const BOOL DBarraScrollEx::Scrolls_MouseEntrando(void) {
		return FALSE;
	}

	// Devuelve TRUE si es necesario repintar, FALSE en caso contrario
	const BOOL DBarraScrollEx::Scrolls_MouseSaliendo(void) {
		BOOL nRepintar = FALSE;
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Presionado && _ScrollH_Estado != DBarraScrollEx_Estado_Presionado) {
			if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
				_ScrollV_Estado = DBarraScrollEx_Estado_Normal;
				nRepintar = TRUE;
			}
			if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) {
				_ScrollH_Estado = DBarraScrollEx_Estado_Normal;
				nRepintar = TRUE;
			}			
		}
		return nRepintar;
	}

	/*
	LRESULT CALLBACK DBarraScrollEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}*/

}