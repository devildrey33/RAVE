#include "stdafx.h"
#include "DBarraScrollEx.h"

namespace DWL {
#define BARRA_MIN_TAM 10

	DBarraScrollEx::DBarraScrollEx(void) :	DControlEx(),
											_Scroll_PosPresionado({ 0 ,0 })					, _Scroll_PosInicio(0.0f)			,
											_ColorFondoV(COLOR_SCROLL_FONDO)				, _ColorFondoH(COLOR_SCROLL_FONDO)	,
											_ColorBarraV(COLOR_SCROLL_BARRA)				, _ColorBarraH(COLOR_SCROLL_BARRA)	,
											_ScrollV_Estado(DBarraScrollEx_Estado_Invisible), _ScrollV_Pagina(1.0f)				, _ScrollV_Posicion(0.0f),
											_ScrollH_Estado(DBarraScrollEx_Estado_Invisible), _ScrollH_Pagina(1.0f)				, _ScrollH_Posicion(0.0f) {
		_ScrollH_Alto = GetSystemMetrics(SM_CYHSCROLL);
		_ScrollV_Ancho = GetSystemMetrics(SM_CXVSCROLL);
	}


	DBarraScrollEx::~DBarraScrollEx(void) {
	}


	void DBarraScrollEx::Scrolls_Pintar(HDC hDC, RECT &RC) {
		if (_ScrollV_Estado == DBarraScrollEx_Estado_Invisible && _ScrollH_Estado == DBarraScrollEx_Estado_Invisible) return;
		/*		#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DBarraScrollEx::Scrolls_Pintar  _ScrollV_Estado = %d\n", _ScrollV_Estado);
				#endif*/

		BOOL PintarRecuadro = TRUE;
		RECT RCV, RCH, RCBV, RCBH;
		ObtenerRectasScroll(RC, RCH, RCV);
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible)	ObtenerRectaBarraScrollV(RCV, RCBV);
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible)	ObtenerRectaBarraScrollH(RCH, RCBH);
		//		Debug_Escribir_Varg(L"_ScrollV_Estado : %d, _ScrollH_Estado : %d, ColorFondoScroll %d\n", _ScrollV_Estado, _ScrollH_Estado, ColorFondoScroll);
		switch (_ScrollV_Estado) {
			case DBarraScrollEx_Estado_Normal			:	
			case DBarraScrollEx_Estado_BarraResaltada	:	
			case DBarraScrollEx_Estado_FondoResaltado	:	
			case DBarraScrollEx_Estado_Presionado		:	
				_PintarBarraScrollEx(hDC, RCV, RCBV, _ColorBarraV, _ColorFondoV);
				break;
			case DBarraScrollEx_Estado_Invisible		:   PintarRecuadro = FALSE;																					break;
		}
		switch (_ScrollH_Estado) {
			case DBarraScrollEx_Estado_Normal			:	
			case DBarraScrollEx_Estado_BarraResaltada	:	
			case DBarraScrollEx_Estado_FondoResaltado	:	
			case DBarraScrollEx_Estado_Presionado		:	
				_PintarBarraScrollEx(hDC, RCH, RCBH, _ColorBarraH, _ColorFondoH);
				break;
			case DBarraScrollEx_Estado_Invisible		:   PintarRecuadro = FALSE;																					break;
		}
		// Si las dos barras son visibles hay que pintar un recuadro en la parte inferior derecha del control 
		if (PintarRecuadro == TRUE) {
			RECT Recuadro = { RCV.left, RCH.top, RCV.right, RCH.bottom };
			HBRUSH BrochaRecuadro = CreateSolidBrush(COLOR_SCROLL_FONDO);
			FillRect(hDC, &Recuadro, BrochaRecuadro);
			DeleteObject(BrochaRecuadro);
		}
	}

	/* Obtiene el área que pertenece al control (RectaCliente es el resultado de GetClientRect, y RectaClienteSinScroll es el área del control excluyendo las barras de scroll) */
	void DBarraScrollEx::ObtenerRectaCliente(RECT *RectaCliente, RECT *RectaClienteSinScroll) {
		GetClientRect(hWnd(), RectaCliente);
		*RectaClienteSinScroll = *RectaCliente;
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) RectaClienteSinScroll->right -= _ScrollV_Ancho;
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) RectaClienteSinScroll->bottom -= _ScrollH_Alto;
	}


	/* Obtiene el área que pertenece los scrolls Horizontal y Vertical */
	void DBarraScrollEx::ObtenerRectasScroll(RECT &RC, RECT &RectaH, RECT &RectaV) {
		RectaH = RC; RectaH.top = RectaH.bottom - _ScrollH_Alto;
		RectaV = RC; RectaV.left = RectaV.right - _ScrollV_Ancho;
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible && _ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			RectaH.right -= _ScrollH_Alto;
			RectaV.bottom -= _ScrollV_Ancho;
		}
	}

	/* Obtiene el área que pertenece los scrolls Horizontal y Vertical */
	void DBarraScrollEx::ObtenerRectasScroll(RECT &RectaH, RECT &RectaV) {
		RECT Recta;
		GetClientRect(hWnd(), &Recta);
		RectaH = Recta; RectaH.top = RectaH.bottom - _ScrollH_Alto;
		RectaV = Recta; RectaV.left = RectaV.right - _ScrollV_Ancho;
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible && _ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			RectaH.right -= _ScrollH_Alto;
			RectaV.bottom -= _ScrollV_Ancho;
		}
	}

	/* Obtiene el área de la barra de scroll vertical (sin el fondo) */
	void DBarraScrollEx::ObtenerRectaBarraScrollV(RECT &RectaScroll, RECT &RectaBarra) {
		int TamBarra = static_cast<int>(static_cast<float>(RectaScroll.bottom) * _ScrollV_Pagina);
		if (TamBarra < BARRA_MIN_TAM) TamBarra = BARRA_MIN_TAM;

		RectaBarra = {  2 + RectaScroll.left,																													// left
						2 + static_cast<int>(_ScrollV_Posicion * (RectaScroll.bottom - TamBarra)),																// top
					   -2 + RectaScroll.right,																													// right
					   -2 + static_cast<int>((_ScrollV_Posicion * (RectaScroll.bottom - TamBarra)) + TamBarra) };	// bottom
	}

	/* Obtiene el área de la barra de scroll horizontal (sin el fondo) */
	void DBarraScrollEx::ObtenerRectaBarraScrollH(RECT &RectaScroll, RECT &RectaBarra) {
		int TamBarra = static_cast<int>(static_cast<float>(RectaScroll.right) * _ScrollH_Pagina);
		if (TamBarra < BARRA_MIN_TAM) TamBarra = BARRA_MIN_TAM;

		//		float Parte	= static_cast<float>(1.0f / RectaScroll.right);
		RectaBarra = {  2 + static_cast<int>(_ScrollH_Posicion * (RectaScroll.right - TamBarra)),															// left
						2 + RectaScroll.top,																												// top
					   -2 + static_cast<int>((_ScrollH_Posicion * (RectaScroll.right - TamBarra)) + TamBarra),	// right
					   -2 + RectaScroll.bottom }; // bottom
	}

	// S'ha de recalcular de forma que el maxim sigui 1, i li haig de restar al tamany total, el tamany d'una pagina
	const float DBarraScrollEx::_CalcularPosScrollH(const UINT nTam, const int nPos) {
		float ancho = (static_cast<float>(nTam) - (static_cast<float>(nTam) * _ScrollH_Pagina)); // ancho resatandole la página

		int TamBarra = static_cast<int>(static_cast<float>(ancho) * _ScrollH_Pagina);
		if (TamBarra < BARRA_MIN_TAM) TamBarra = BARRA_MIN_TAM;


		float tam = static_cast<float>(ancho) - TamBarra;
		//		float tam = (static_cast<float>(nTam) - (static_cast<float>(nTam) * _ScrollH_Pagina)); // altura resatandole la página
		float NuevaPos = _Scroll_PosInicio + (1.0f / tam) * static_cast<float>(nPos - _Scroll_PosPresionado.x);
		//		float NuevaPos = _Scroll_PosInicio + (1.0f / static_cast<float>(nTam)) * nPos;
		if (NuevaPos > 1.0f)	NuevaPos = 1.0f;
		if (NuevaPos < 0.0f)	NuevaPos = 0.0f;

		#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DBarraScrollEx::_CalcularPosScrollH  NuevaPos = %0.2f\n", NuevaPos);
		#endif

		return NuevaPos;
	}

	const float DBarraScrollEx::_CalcularPosScrollV(const UINT nTam, const int nPos) {
		
		float alto = (static_cast<float>(nTam) - (static_cast<float>(nTam) * _ScrollV_Pagina)); // alto resatandole la página
//		float Pos = (static_cast<float>(nPos) - (static_cast<float>(nTam) * _ScrollV_Pagina)); // alto resatandole la página
//		if (Pos < 0.0f) Pos = 0.0f;

		int TamBarra = static_cast<int>(static_cast<float>(alto) * _ScrollV_Pagina);
		if (TamBarra < BARRA_MIN_TAM) TamBarra = BARRA_MIN_TAM;

		float tam = static_cast<float>(alto) - TamBarra;
		//		float tam = (static_cast<float>(nTam) - (static_cast<float>(nTam) * _ScrollV_Pagina)); // altura resatandole la página
		float NuevaPos = _Scroll_PosInicio + (1.0f / tam) * static_cast<float>(nPos - _Scroll_PosPresionado.y);
		if (NuevaPos > 1.0f)	NuevaPos = 1.0f;
		if (NuevaPos < 0.0f)	NuevaPos = 0.0f;

		#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DBarraScrollEx::_CalcularPosScrollV  NuevaPos = %0.2f\n", NuevaPos);
		#endif

		return NuevaPos;
	}

	// Devuelve TRUE si el mouse está dentro de alguna barra de scroll
	const BOOL DBarraScrollEx::Scrolls_MouseMovimiento(DEventoMouse &DatosMouse) {
		int cX = DatosMouse.X(),
			cY = DatosMouse.Y();

		RECT RCV, RCH, RectaBarra;
		ObtenerRectasScroll(RCH, RCV);
		POINT Pt = { cX, cY };
		BOOL  RetV = FALSE, RetH = FALSE, RetB = FALSE;
		// Compruebo el estado vertical
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			if (_ScrollV_Estado != DBarraScrollEx_Estado_Presionado) {
				ObtenerRectaBarraScrollV(RCV, RectaBarra);
				RetV = PtInRect(&RCV, Pt);			// Espacio del fondo del scroll vertical
				RetB = PtInRect(&RectaBarra, Pt);	// Espacio de la barra del scroll vertical
				// Dentro del scroll vertical 
				if (RetV == TRUE && RetB == TRUE && _ScrollV_Estado != DBarraScrollEx_Estado_BarraResaltada) {
					_ScrollV_Estado = DBarraScrollEx_Estado_BarraResaltada;
					#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
						Debug_Escribir(L"DBarraScrollEx::Scrolls_MouseMovimiento  _ScrollV_Estado = DBarraScrollEx_Estado_BarraResaltada\n");
					#endif
					ScrollV_Transicion(DBarraScrollEx_Transicion_BarraResaltada);
					//Repintar();
					return TRUE;
				}
				else if (RetV == TRUE && RetB == FALSE && _ScrollV_Estado != DBarraScrollEx_Estado_FondoResaltado) {
					_ScrollV_Estado = DBarraScrollEx_Estado_FondoResaltado;
					#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
						Debug_Escribir(L"DBarraScrollEx::Scrolls_MouseMovimiento  _ScrollV_Estado = DBarraScrollEx_Estado_FondoResaltado\n");
					#endif
					//Repintar();
					ScrollV_Transicion(DBarraScrollEx_Transicion_FondoResaltado);
					return TRUE;
				}
				// Fuera del scroll vertical
				if (RetV == FALSE && _ScrollV_Estado != DBarraScrollEx_Estado_Normal) {
					_ScrollV_Estado = DBarraScrollEx_Estado_Normal;
					#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
						Debug_Escribir(L"DBarraScrollEx::Scrolls_MouseMovimiento  _ScrollV_Estado = DBarraScrollEx_Estado_Normal\n");
					#endif
					//Repintar();
					ScrollV_Transicion(DBarraScrollEx_Transicion_Normal);
					return TRUE;
				}
			}
			else { // ScrollV presionado
				_ScrollV_Posicion = _CalcularPosScrollV(RCH.bottom, cY);
/*				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DBarraScrollEx::Scrolls_MouseMovimiento _ScrollV_Estado = DBarraScrollEx_Estado_Presionado  V:%.02f\n", _ScrollV_Posicion);
				#endif*/
				Scrolls_EventoCambioPosicion();
				Repintar();
				//ScrollV_Transicion(DBarraScrollEx_Transicion_Presionado);
				return TRUE;
			}
		}
		// Compruebo el estado horizontal
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) {
			if (_ScrollH_Estado != DBarraScrollEx_Estado_Presionado) {
				ObtenerRectaBarraScrollH(RCH, RectaBarra);
				RetB = PtInRect(&RectaBarra, Pt);	// Espacio de la barra del scroll vertical
				RetH = PtInRect(&RCH, Pt);
				if (RetH == TRUE && RetB == TRUE && _ScrollH_Estado != DBarraScrollEx_Estado_BarraResaltada) {
					_ScrollH_Estado = DBarraScrollEx_Estado_BarraResaltada;
					ScrollH_Transicion(DBarraScrollEx_Transicion_BarraResaltada);
					//Repintar();
					return TRUE;
				}
				else if (RetH == TRUE && RetB == FALSE && _ScrollH_Estado != DBarraScrollEx_Estado_FondoResaltado) {
					_ScrollH_Estado = DBarraScrollEx_Estado_FondoResaltado;
					ScrollH_Transicion(DBarraScrollEx_Transicion_FondoResaltado);
					//Repintar();
					return TRUE;
				}
				if (RetH == FALSE && _ScrollH_Estado != DBarraScrollEx_Estado_Normal) {
					_ScrollH_Estado = DBarraScrollEx_Estado_Normal;
					//Repintar();
					ScrollH_Transicion(DBarraScrollEx_Transicion_Normal);
					return TRUE;
				}
			}
			else { // ScrollH presionado
				_ScrollH_Posicion = _CalcularPosScrollH(RCH.right, cX);
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"Scrolls_MouseMovimiento H:%.02f\n", _ScrollH_Posicion);
				#endif
				Scrolls_EventoCambioPosicion();
				Repintar();
				//ScrollH_Transicion(DBarraScrollEx_Transicion_Presionado);
				return TRUE;
			}
		}

		return FALSE;
	}


	void DBarraScrollEx::ScrollV_Transicion(const DBarraScrollEx_Transicion nTransicion) {
		DWORD Duracion = DhWnd::TiempoAnimaciones;
		if (_ScrollV_AniTransicion.Animando() == TRUE) {
//			Duracion = _ScrollV_AniTransicion.TiempoActual();
			_ScrollV_AniTransicion.Terminar();
		}

		COLORREF FondoHasta, BarraHasta;
		switch (nTransicion) {
			case DBarraScrollEx_Transicion_Normal:
				FondoHasta = COLOR_SCROLL_FONDO;
				BarraHasta = COLOR_SCROLL_BARRA;
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DBarraScrollEx::ScrollV_Transicion DBarraScrollEx_Transicion_Normal\n");
				#endif
				break;
			case DBarraScrollEx_Transicion_BarraResaltada:
				FondoHasta = COLOR_SCROLL_FONDO;
				BarraHasta = COLOR_SCROLL_BARRA_RESALTADO;
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DBarraScrollEx::ScrollV_Transicion DBarraScrollEx_Transicion_BarraResaltada\n");
				#endif
				break;
			case DBarraScrollEx_Transicion_FondoResaltado:
				FondoHasta = COLOR_SCROLL_FONDO_RESALTADO;
				BarraHasta = COLOR_SCROLL_BARRA;
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DBarraScrollEx::ScrollV_Transicion DBarraScrollEx_Transicion_FondoResaltado\n");
				#endif
				break;
			case DBarraScrollEx_Transicion_Presionado:
				FondoHasta = COLOR_SCROLL_FONDO_PRESIONADO;
				BarraHasta = COLOR_SCROLL_BARRA_PRESIONADO;
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DBarraScrollEx::ScrollV_Transicion DBarraScrollEx_Transicion_Presionado\n");
				#endif
				break;
		}

		_ScrollV_AniTransicion.Iniciar(_ColorFondoV, FondoHasta, _ColorBarraV, BarraHasta, Duracion, [=](DAnimacion::Valores &Datos, const BOOL Terminado) {
			_ColorFondoV = Datos[0].Color();
			_ColorBarraV = Datos[1].Color();
			RepintarAni();
		});
	}

	void DBarraScrollEx::ScrollH_Transicion(const DBarraScrollEx_Transicion nTransicion) {
		DWORD Duracion = DhWnd::TiempoAnimaciones;
		if (_ScrollH_AniTransicion.Animando() == TRUE) {
//			Duracion = _ScrollH_AniTransicion.TiempoActual();
			_ScrollH_AniTransicion.Terminar();
		}

		COLORREF FondoHasta, BarraHasta;
		switch (nTransicion) {
			case DBarraScrollEx_Transicion_Normal:
				FondoHasta = COLOR_SCROLL_FONDO;
				BarraHasta = COLOR_SCROLL_BARRA;
				break;
			case DBarraScrollEx_Transicion_BarraResaltada:
				FondoHasta = COLOR_SCROLL_FONDO;
				BarraHasta = COLOR_SCROLL_BARRA_RESALTADO;
				break;
			case DBarraScrollEx_Transicion_FondoResaltado:
				FondoHasta = COLOR_SCROLL_FONDO_RESALTADO;
				BarraHasta = COLOR_SCROLL_BARRA;
				break;
			case DBarraScrollEx_Transicion_Presionado:
				FondoHasta = COLOR_SCROLL_FONDO_PRESIONADO;
				BarraHasta = COLOR_SCROLL_BARRA_PRESIONADO;
				break;
		}

		_ScrollH_AniTransicion.Iniciar(_ColorFondoH, FondoHasta, _ColorBarraH, BarraHasta, Duracion, [=](DAnimacion::Valores &Datos, const BOOL Terminado) {
			_ColorFondoH = Datos[0].Color();
			_ColorBarraH = Datos[1].Color();
			RepintarAni();
		});

	}


	// Devuelve TRUE si el mouse está dentro de alguna barra de scroll
	const BOOL DBarraScrollEx::Scrolls_MousePresionado(DEventoMouse &DatosMouse) {
		int cX		= DatosMouse.X(),
			cY		= DatosMouse.Y();

		if (DatosMouse.Boton != 0) return FALSE;
		RECT RCV, RCH, RCBV, RCBH;
		ObtenerRectasScroll(RCH, RCV);
		POINT Pt = { cX, cY };
		BOOL  RetV = FALSE, RetH = FALSE, RetBV = FALSE, RetBH = FALSE;
//		float NuevaPos = 0.0f;
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) {
			RetV = PtInRect(&RCV, Pt);
			if (RetV == TRUE) { // Está dentro del área del scroll vertical
				ObtenerRectaBarraScrollV(RCV, RCBV);
				RetBV = PtInRect(&RCBV, Pt);
				if (RetBV == FALSE) { // Está en la parte del fondo (recalculo la posición de la barra antes de empezar el drag)
//					float alto = (static_cast<float>(RCV.bottom) - (static_cast<float>(RCV.bottom) * _ScrollV_Pagina)); // altura resatandole la página
					_ScrollV_Posicion = _CalcularPosScrollV(RCH.bottom, cY);
//					_ScrollV_Posicion = (1.0f / RCV.bottom) * static_cast<float>(cY);
				}
				SetCapture(hWnd());
				_Scroll_PosPresionado = { cX, cY };			// Posición del mouse desde donde se inicia el drag de la barra
				_Scroll_PosInicio     = _ScrollV_Posicion;	// Posición del scroll al iniciar el drag
				_ScrollV_Estado		  = DBarraScrollEx_Estado_Presionado;
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"Scrolls_MousePresionado V:%.02f\n", _ScrollV_Posicion);
				#endif
				Scrolls_EventoCambioPosicion();
				ScrollV_Transicion(DBarraScrollEx_Transicion_Presionado);
				//Repintar();
				return TRUE;
			}
		}
		if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) {
			RetH = PtInRect(&RCH, Pt);
			if (RetH == TRUE) { // Está dentro del área del scroll horizontal
				ObtenerRectaBarraScrollH(RCH, RCBH);
				RetBH = PtInRect(&RCBH, Pt);
				if (RetBH == FALSE) { // Está en la parte del fondo (recalculo la posición de la barra antes de empezar el drag)
					//float ancho = (static_cast<float>(RCH.right) - (static_cast<float>(RCH.right) * _ScrollH_Pagina)); // ancho resatandole la página
					//_ScrollH_Posicion = (1.0f / static_cast<float>(RCH.right)) * static_cast<float>(cX);
					_ScrollH_Posicion = _CalcularPosScrollH(RCH.right, cX);
				}
				SetCapture(hWnd());
				_Scroll_PosPresionado = { cX, cY };			// Posición desde donde se inicia el drag de la barra
				_Scroll_PosInicio	  = _ScrollH_Posicion;	// Posición del scroll al iniciar el drag
				_ScrollH_Estado		  = DBarraScrollEx_Estado_Presionado;
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"Scrolls_MousePresionado H:%.02f\n", _ScrollH_Posicion);
				#endif
				Scrolls_EventoCambioPosicion();
				ScrollH_Transicion(DBarraScrollEx_Transicion_Presionado);
				//Repintar();
				return TRUE;
			}
		}
		
		return FALSE;		
	}

	// Devuelve TRUE si el mouse está dentro de alguna barra de scroll
	const BOOL DBarraScrollEx::Scrolls_MouseSoltado(DEventoMouse &DatosMouse) {
		int cX		= DatosMouse.X(),
			cY		= DatosMouse.Y();

		if (DatosMouse.Boton != 0) return FALSE;
		RECT RCV, RCH, RectaBarra;
		ObtenerRectasScroll(RCH, RCV);
		POINT Pt = { cX, cY };
		BOOL  RetV = FALSE, RetH = FALSE, RetB = FALSE;
		ReleaseCapture();
		DBarraScrollEx_Transicion Trans = DBarraScrollEx_Transicion_Normal;
		if (_ScrollV_Estado == DBarraScrollEx_Estado_Presionado) {
			ObtenerRectaBarraScrollV(RCV, RectaBarra);
			RetV = PtInRect(&RCV, Pt);
			RetB = PtInRect(&RectaBarra, Pt);
			if (RetV == TRUE && RetB == TRUE)		{ _ScrollV_Estado = DBarraScrollEx_Estado_BarraResaltada;	Trans = DBarraScrollEx_Transicion_BarraResaltada;	}
			else if (RetV == TRUE && RetB == FALSE) { _ScrollV_Estado = DBarraScrollEx_Estado_FondoResaltado;	Trans = DBarraScrollEx_Transicion_FondoResaltado;	}
			else									{ _ScrollV_Estado = DBarraScrollEx_Estado_Normal;			Trans = DBarraScrollEx_Transicion_Normal;			}
			#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DBarraScrollEx::Scrolls_MouseSoltado  _ScrollV_Estado = %d\n", _ScrollV_Estado);
			#endif

			_ScrollV_Posicion = _CalcularPosScrollV(RCH.bottom, cY);
			#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"Scrolls_MouseSoltado V:%.02f\n", _ScrollV_Posicion);
			#endif
			Scrolls_EventoCambioPosicion();
			ScrollV_Transicion(Trans);
			//Repintar();
			return TRUE;
		}
		if (_ScrollH_Estado == DBarraScrollEx_Estado_Presionado) {
			ObtenerRectaBarraScrollH(RCH, RectaBarra);
			RetH = PtInRect(&RCH, Pt);
			RetB = PtInRect(&RectaBarra, Pt);
			if (RetH == TRUE && RetB == TRUE)		{ _ScrollH_Estado = DBarraScrollEx_Estado_BarraResaltada;	Trans = DBarraScrollEx_Transicion_BarraResaltada;	}
			else if (RetH == TRUE && RetB == FALSE) { _ScrollH_Estado = DBarraScrollEx_Estado_FondoResaltado;	Trans = DBarraScrollEx_Transicion_FondoResaltado;	}
			else									{ _ScrollH_Estado = DBarraScrollEx_Estado_Normal;			Trans = DBarraScrollEx_Transicion_Normal;			}
			_ScrollH_Posicion = _CalcularPosScrollH(RCH.right, cX);
			#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"Scrolls_MouseSoltado H:%.02f\n", _ScrollH_Posicion);
			#endif
			Scrolls_EventoCambioPosicion();
			ScrollH_Transicion(Trans);
			//Repintar();
			return TRUE;
		}

		return FALSE;		
	}

	void DBarraScrollEx::_PintarBarraScrollEx(HDC hDC, RECT &RectaScroll, RECT &RectaBarra, const COLORREF pColorBarra, const COLORREF pColorFondo) {
		// Creo las brochas 
		HBRUSH BrochaColorFondo = CreateSolidBrush(pColorFondo);
		HBRUSH BrochaColorBarra = CreateSolidBrush(pColorBarra);

		// Pinto el scrollbar
		FillRect(hDC, &RectaScroll, BrochaColorFondo);
		FillRect(hDC, &RectaBarra, BrochaColorBarra);

		// Borro las brochas de la memória
		DeleteObject(BrochaColorFondo);
		DeleteObject(BrochaColorBarra);
	}


	void DBarraScrollEx::ScrollH_Pagina(const float nPagina) {
		_ScrollH_Pagina = nPagina;
		Repintar();
	}

	void DBarraScrollEx::ScrollH_Posicion(const float nPosicion) {
		_ScrollH_Posicion = nPosicion;
		Repintar();
	}

	// Función para avanzar una página 
	void DBarraScrollEx::AvPag(void) {
		_ScrollV_Posicion += _ScrollV_Pagina;
		if (_ScrollV_Posicion > 1.0f) _ScrollV_Posicion = 1.0f;
		Scrolls_EventoCambioPosicion();
	}

	// Función para retroceder una página 
	void DBarraScrollEx::RePag(void) {
		_ScrollV_Posicion -= _ScrollV_Pagina;
		if (_ScrollV_Posicion < 0.0f) _ScrollV_Posicion = 0.0f;
		Scrolls_EventoCambioPosicion();
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
		#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DBarraScrollEx::ScrollV_Visible  _ScrollV_Estado = %d\n", _ScrollV_Estado);
		#endif

	}

	// Devuelve TRUE si es necesario repintar, FALSE en caso contrario
/*	const BOOL DBarraScrollEx::Scrolls_MouseEntrando(void) {
		return FALSE;
	}**/

	// Devuelve TRUE si es necesario repintar, FALSE en caso contrario
	const BOOL DBarraScrollEx::Scrolls_MouseSaliendo(void) {
		BOOL nRepintar = FALSE;
		if (_ScrollV_Estado != DBarraScrollEx_Estado_Presionado && _ScrollH_Estado != DBarraScrollEx_Estado_Presionado) {
			if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible && _ScrollV_Estado != DBarraScrollEx_Estado_Normal) {
				#if DBARRASCROLLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir(L"DBarraScrollEx::Scrolls_MouseSaliendo  _ScrollV_Estado = 1\n");
				#endif
				_ScrollV_Estado = DBarraScrollEx_Estado_Normal;
				ScrollV_Transicion(DBarraScrollEx_Transicion_Normal);
				//nRepintar = TRUE;
			}
			if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible && _ScrollH_Estado != DBarraScrollEx_Estado_Normal) {
				_ScrollH_Estado = DBarraScrollEx_Estado_Normal;
				ScrollH_Transicion(DBarraScrollEx_Transicion_Normal);
				//nRepintar = TRUE;
			}			
		}
		return nRepintar;
	}

	/*
	LRESULT CALLBACK DBarraScrollEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}*/

}