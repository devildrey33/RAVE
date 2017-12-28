//#pragma once
#include "stdafx.h"
#include "DListaIconos.h"
//#include "DArbolEx.h"

#define ARBOLEX_PADDING					2 // Espacio entre el marco de la selección y el texto (tambien se utiliza para espaciar el expansor, el icono y el texto horizontalmente)
#define ARBOLEX_MARGEN_Y_SELECCION		1 // Espacio entre la Y inicial / final y el marco de seleccion
#define ARBOLEX_TAMEXPANSOR				9 // OJO!! tiene que ser impar para quedar bien
#define ARBOLEX_TAMICONO				16 // Tamaño del icono

namespace DWL {

	DArbolEx::DArbolEx(void) : DBarraScrollEx(),
		_NodoMarcado(NULL),																		// Info nodo Marcado
		_NodoPresionado(NULL), _NodoPresionadoParte(DArbolEx_ParteNodo_Nada),					// Info nodo presionado
		_NodoResaltado(NULL), _NodoResaltadoParte(DArbolEx_ParteNodo_Nada),						// Info nodo resaltado
		_NodoUResaltado(NULL), _NodoUResaltadoParte(DArbolEx_ParteNodo_Nada),					// Info ultimo nodo resaltado (para evitar repintados innecesarios en el mousemove)
		_Fuente(NULL), _TotalAnchoVisible(0), _TotalAltoVisible(0),								// Fuente y tamaño
		_NodoPaginaInicio(NULL), _NodoPaginaFin(NULL), _NodoPaginaVDif(0), _NodoPaginaHDif(0),	// Nodo inicial y final de la página visible
		_BufferNodo(NULL), _BufferNodoBmp(NULL), _BufferNodoBmpViejo(NULL) {					// Buffer para pintar un solo nodo
		_Raiz._Expandido = TRUE;
		_Raiz._Arbol = this;
		_ColorFondoScroll = COLOR_ARBOL_FONDO_SCROLL;
		//		_ColorScrollResaltado   = COLOR_ARBOL_FONDO_SCROLL_RESALTADO;
				//_ColorScrollPresionado  = COLOR_ARBOL_FONDO_SCROLL_PRESIONADO;
	};

	DArbolEx::~DArbolEx(void) {
		// Elimino el buffer para pintar el nodo
		if (_BufferNodo != NULL) {
			SelectObject(_BufferNodo, _BufferNodoBmpViejo);
			DeleteObject(_BufferNodoBmp);
			DeleteDC(_BufferNodo);
		}

	}

	HWND DArbolEx::CrearArbolEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
		if (hWnd()) { Debug_Escribir(L"DArbolEx::CrearArbolEx() Error : ya se ha creado el arbol\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DArbolEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
		_Fuente = hWnd._FuenteTest;
		return hWnd();
	}

	DArbolEx_Nodo *DArbolEx::_AgregarNodo(DArbolEx_Nodo *nNodo, const TCHAR *nTexto, DArbolEx_Nodo *nPadre, const int nIcono, DhWnd_Fuente *nFuente, const size_t PosicionNodo) {

		// Asigno el arbol padre
		nNodo->_Arbol = this;

		// Asigno el texto
		nNodo->_Texto = nTexto;
		// Cargo el icono (si existe)
		if (nIcono != NULL) {
			nNodo->_Icono = DListaIconos::AgregarIconoRecursos(nIcono, ARBOLEX_TAMICONO, ARBOLEX_TAMICONO);
		}

		// Compruebo la fuente (si no es NULL asigno la nueva fuente)
		if (nFuente != NULL) nNodo->_Fuente = nFuente;
		else                 nNodo->_Fuente = &_Fuente; // no se ha especificado la fuente, asigno la fuente por defecto del arbol

		// Calculo y asigno el ancho del texto
		nNodo->_AnchoTexto = nNodo->_Fuente->Tam(nNodo->_Texto).cx;

		// Compruebo el padre (si es null nPadre será _Raiz)
		if (nPadre == NULL) {
			nPadre = &_Raiz;
		}
		// Asigno el nodo padre y el total de ancestros para este nodo
		nNodo->_Padre = nPadre;
		nNodo->_Ancestros = nPadre->_Ancestros + 1;

		// Si el padre está expandido, sumo la altura de este nodo al total 
		if (nNodo->_Padre->Expandido() == TRUE) {
			// Cuento los pixeles de todos los nodos visibles
			_TotalAltoVisible += (nNodo->_Fuente->Alto() + (ARBOLEX_PADDING * 2));
		}
		DArbolEx_Nodo *nAnterior = NULL;
		size_t nPos = 0;
		switch (PosicionNodo) {
			// Al final de la lista
		case DARBOLEX_POSICIONNODO_FIN:
			// Asigno el nodo sigüiente al último nodo del padre (si es que existe algún nodo)
			if (nPadre->TotalHijos() > 0) {
				nPadre->Hijo(nPadre->TotalHijos() - 1)->_Siguiente = nNodo;
				nNodo->_Anterior = nPadre->_Hijos[nPadre->TotalHijos() - 1];
			}
			nPadre->_Hijos.push_back(nNodo);
			break;

			// Por orden alfabético
		case DARBOLEX_POSICIONNODO_ORDENADO:
			// Busco la posición que deberia tener ordenado alfabeticamente
			for (nPos = 0; nPos < nPadre->TotalHijos(); nPos++) {
				if (_wcsicmp(nTexto, nPadre->Hijo(nPos)->Texto().c_str()) < 0) break;
			}
			// enlazo el nodo anterior
			if (nPadre->TotalHijos() > 0 && nPos > 0) { nNodo->_Anterior = nPadre->_Hijos[nPos - 1]; }

			// Enlazo el nodo siguiente
			if (nPos > 0) { nPadre->_Hijos[nPos - 1]->_Siguiente = nNodo; }
			if (nPos + 1 > nPadre->TotalHijos()) { nNodo->_Siguiente = nPadre->_Hijos[nPos + 1]; }

			// Agrego el nodo 
			if (nPadre->_Hijos.size() == 0) { nPadre->_Hijos.push_back(nNodo); }
			else { nPadre->_Hijos.insert(nPadre->_Hijos.begin() + nPos, nNodo); }
			break;

			// el default incluye DARBOLEX_POSICIONNODO_INICIO que es 0
		default:
			// enlazo el nodo anterior
			if (nPadre->TotalHijos() > 0 && nPos > 0) { nNodo->_Anterior = nPadre->_Hijos[nPos - 1]; }

			// Enlazo el nodo siguiente
			if (PosicionNodo > 0) { nPadre->_Hijos[PosicionNodo - 1]->_Siguiente = nNodo; }
			if (PosicionNodo + 1 > nPadre->TotalHijos()) { nNodo->_Siguiente = nPadre->_Hijos[PosicionNodo + 1]; }
			// Agrego el nodo 
			if (nPadre->_Hijos.size() == 0) { nPadre->_Hijos.push_back(nNodo); }
			else { nPadre->_Hijos.insert(nPadre->_Hijos.begin() + PosicionNodo, nNodo); }
			break;

		}

		_CalcularScrolls();
		return nNodo;
	}

	void DArbolEx::EliminarNodo(DArbolEx_Nodo *nEliminar) {
		/*size_t nPos = 0;

		_NodoMarcado = nEliminar->_Siguiente;
		// Busco la posición del nodo a eliminar dentro de su padre
		for (nPos = 0; nPos < nEliminar->_Padre->TotalHijos(); nPos++) {
			if (nEliminar == nEliminar->_Padre->Hijo(nPos)) {
				break;
			}
		}

		// Asigno el nodo siguiente para el nodo anterior del que estamos eliminando
		if (nPos > 0) {
			if (nEliminar->_Padre->TotalHijos() > nPos + 1) { nEliminar->_Padre->_Hijos[nPos - 1]->_Siguiente = nEliminar->_Padre->_Hijos[nPos + 1];	}
			else											{ nEliminar->_Padre->_Hijos[nPos - 1]->_Siguiente = NULL;									}
		}
		nEliminar->_Padre->_Hijos.erase(nEliminar->_Padre->_Hijos.begin() + (nPos - 1));
	*/
	}


	void DArbolEx::Pintar(HDC hDC) {
		RECT	RC, RCS;
		ObtenerRectaCliente(&RC, &RCS);

		HDC		Buffer = CreateCompatibleDC(hDC);
		HBITMAP Bmp = CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));



		//		LONG DifInicio = 0; // altura total pintada
		//		_CalcularNodosPagina(RCS.bottom);
		DArbolEx_Nodo *nActual = _NodoPaginaInicio;

		LONG PixelInicio = static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion);
		LONG PixelFin = PixelInicio + RCS.bottom;
		LONG nAlto = 0;
		int  DifInicioV = _NodoPaginaVDif;
		int  DifInicioH = _NodoPaginaHDif;

		DArbolEx_Nodo *NodoFin = _NodoPaginaFin;
		// Determino el ultimo nodo a pintar
		if (NodoFin != NULL) NodoFin = _NodoPaginaFin->_Siguiente;

		RECT RectaItem; // = { 0, DifInicio, RCS.right, DifInicio + nActual->_Fuente->Alto() };
		while (nActual != NodoFin && nActual != NULL) {
			nAlto = nActual->_Fuente->Alto() + (ARBOLEX_PADDING * 2);
			RectaItem = { 0, DifInicioV, RCS.right, DifInicioV + nAlto };
			PintarNodo(Buffer, &RectaItem, nActual, -_NodoPaginaHDif);
			DifInicioV += nAlto;
			nActual = _BuscarNodoSiguiente(nActual, TRUE);

		}

		// Pinto el resto del fondo
		if (DifInicioV < RCS.bottom) {
			RECT RFondo = RCS; RFondo.top = DifInicioV;
			HBRUSH BFondo = CreateSolidBrush(COLOR_ARBOL_FONDO);
			FillRect(Buffer, &RFondo, BFondo);
			DeleteObject(BFondo);
		}

		Scrolls_Pintar(Buffer, RC);

		BitBlt(hDC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino objetos gdi de la memoria
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	void DArbolEx::PintarNodo(HDC hDC, RECT *Espacio, DArbolEx_Nodo *nNodo, const int PosH) {
		// Determino el estado del nodo (0 normal, 1 resaltado, 2 presionado)
		int Estado = 0;
		if		(nNodo == _NodoPresionado)		Estado = 1;
		else if (nNodo == _NodoResaltado)		Estado = 2;

		COLORREF ColFondo = NULL, ColTexto = NULL;
		switch (Estado) {
			case 0: // Normal
				ColTexto = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_TEXTO : COLOR_ARBOL_SELECCION_TEXTO;
				ColFondo = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_FONDO : COLOR_ARBOL_SELECCION;
				break;
			case 1: // Presionado
				ColTexto = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_TEXTO_PRESIONADO : COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO;
				ColFondo = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_FONDO_PRESIONADO : COLOR_ARBOL_SELECCION_PRESIONADO;
				break;
			case 2: // Resaltado
				ColTexto = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_TEXTO_RESALTADO : COLOR_ARBOL_SELECCION_TEXTO_RESALTADO;
				ColFondo = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_FONDO_RESALTADO : COLOR_ARBOL_SELECCION_RESALTADO; // RGB(0, 0, 255) : RGB(0, 255, 0);
				break;
		}

		// Pinto el fondo del buffer
		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_ARBOL_FONDO);
		RECT EspacioLocal = { 0 , 0, (static_cast<LONG>(_TotalAnchoVisible) > Espacio->right) ? static_cast<LONG>(_TotalAnchoVisible) : Espacio->right, (2 * ARBOLEX_PADDING) + nNodo->_Fuente->Alto() };
		FillRect(_BufferNodo, &EspacioLocal, BrochaFondo);
		DeleteObject(BrochaFondo);

		int		AnchoOcupado = static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING;  // ARBOLEX_PADDING + (static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO));
		// Determino si hay que mostrar el expansor
		if (nNodo->MostrarExpansor() == TRUE) {
			HBRUSH	BrochaBordeExpansor = NULL; // color del borde del expansor
			HPEN	PlumaExpansor		= NULL; // color de los simbolos '+' y '-'

			int PosExpansorY = ((Espacio->bottom - Espacio->top) - ARBOLEX_TAMEXPANSOR) / 2;
			int EPresionado = 0; // Pixel de mas (si el expansor está presionado)
			if (_NodoPresionado == nNodo && _NodoResaltadoParte == DArbolEx_ParteNodo_Expansor) { EPresionado = 1; }
			RECT MarcoExpansor = {	EPresionado + AnchoOcupado,
									EPresionado + PosExpansorY,
									EPresionado + AnchoOcupado + ARBOLEX_TAMEXPANSOR,
									EPresionado + PosExpansorY + ARBOLEX_TAMEXPANSOR };
			// Expansor presionado
			if (_NodoPresionado == nNodo && _NodoResaltadoParte == DArbolEx_ParteNodo_Expansor) {
				HBRUSH	BrochaExpansorPresionado = CreateSolidBrush(COLOR_ARBOL_SELECCION_PRESIONADO);
				FillRect(_BufferNodo, &MarcoExpansor, BrochaExpansorPresionado);
				DeleteObject(BrochaExpansorPresionado);
				BrochaBordeExpansor = CreateSolidBrush(COLOR_ARBOL_EXPANSOR_BORDE_PRESIONADO);
				PlumaExpansor = CreatePen(PS_SOLID, 1, COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO);
			}
			// Expansor resaltado
			else if (_NodoResaltado == nNodo && _NodoResaltadoParte == DArbolEx_ParteNodo_Expansor) {
				HBRUSH	BrochaExpansorResaltado = CreateSolidBrush(COLOR_ARBOL_SELECCION_RESALTADO);
				FillRect(_BufferNodo, &MarcoExpansor, BrochaExpansorResaltado);
				DeleteObject(BrochaExpansorResaltado);
				BrochaBordeExpansor = CreateSolidBrush(COLOR_ARBOL_EXPANSOR_BORDE_RESALTADO);
				PlumaExpansor = CreatePen(PS_SOLID, 1, COLOR_ARBOL_SELECCION_TEXTO_RESALTADO);
			}
			// Expansor normal
			else {
				BrochaBordeExpansor = CreateSolidBrush(COLOR_ARBOL_EXPANSOR_BORDE);
				PlumaExpansor = CreatePen(PS_SOLID, 1, COLOR_ARBOL_TEXTO);
			}
			//			Debug_Escribir_Varg(L"ArbolEx::PintarNodo left:%d top:%d right:%d bottom:%d\n", MarcoExpansor.left, MarcoExpansor.top, MarcoExpansor.right, MarcoExpansor.bottom);
			FrameRect(_BufferNodo, &MarcoExpansor, BrochaBordeExpansor);

			HPEN VPluma = static_cast<HPEN>(SelectObject(_BufferNodo, PlumaExpansor));
			// Pinto una linea horizontal para el expansor
			MoveToEx(_BufferNodo, EPresionado + AnchoOcupado + ARBOLEX_PADDING, EPresionado + PosExpansorY + ARBOLEX_TAMEXPANSOR / 2, NULL);
			LineTo(_BufferNodo, EPresionado + AnchoOcupado + ARBOLEX_TAMEXPANSOR - ARBOLEX_PADDING, EPresionado + PosExpansorY + ARBOLEX_TAMEXPANSOR / 2);
			if (nNodo->_Expandido == FALSE) {
				// Pinto una linea vertical para el expansor
				MoveToEx(_BufferNodo, EPresionado + AnchoOcupado + (ARBOLEX_TAMEXPANSOR / 2), EPresionado + PosExpansorY + 2, NULL);
				LineTo(_BufferNodo, EPresionado + AnchoOcupado + (ARBOLEX_TAMEXPANSOR / 2), EPresionado + PosExpansorY + ARBOLEX_TAMEXPANSOR - 2);
			}

			DeleteObject(BrochaBordeExpansor);
			SelectObject(_BufferNodo, VPluma);
			DeleteObject(PlumaExpansor);
		}
		AnchoOcupado += ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR;

		int SPresionado = 0; // pixel extra a sumar en la 'x' y 'y' iniciales de los rectangulos del Icono, FondoTexto y Texto
		if (_NodoPresionado == nNodo && _NodoPresionadoParte > DArbolEx_ParteNodo_Expansor) { SPresionado = 1; }

		// Pinto el icono
		int PosIcoY = SPresionado + (((Espacio->bottom - Espacio->top) - ARBOLEX_TAMICONO) / 2);
		int PosIcoX = SPresionado + (AnchoOcupado + ARBOLEX_PADDING);
		DrawIconEx(_BufferNodo, PosIcoX, PosIcoY, nNodo->_Icono->Icono(), ARBOLEX_TAMICONO, ARBOLEX_TAMICONO, 0, 0, DI_NORMAL);
		AnchoOcupado += ARBOLEX_TAMICONO + ARBOLEX_PADDING + 2;

		// Pinto el fondo del texto del nodo (tanto si está seleccionado como si no)
		RECT RFondo = { SPresionado + (AnchoOcupado - ARBOLEX_PADDING),
						SPresionado + (ARBOLEX_MARGEN_Y_SELECCION * 2),
						SPresionado + (AnchoOcupado + nNodo->_AnchoTexto) + ARBOLEX_PADDING,
						SPresionado + ((ARBOLEX_PADDING * 2) + nNodo->_Fuente->Alto()) - ARBOLEX_MARGEN_Y_SELECCION };

		HBRUSH BrochaFondoTexto = CreateSolidBrush(ColFondo);
		FillRect(_BufferNodo, &RFondo, BrochaFondoTexto);
		DeleteObject(BrochaFondoTexto);

		// Pinto el texto del nodo
		SetTextColor(_BufferNodo, ColTexto);
		HFONT FuenteVieja = static_cast<HFONT>(SelectObject(_BufferNodo, nNodo->_Fuente->Fuente()));
		RECT RTexto = { SPresionado + AnchoOcupado,
						SPresionado + ARBOLEX_PADDING,
						SPresionado + AnchoOcupado + nNodo->_AnchoTexto,
						SPresionado + ARBOLEX_PADDING + nNodo->_Fuente->Alto() };
		DrawText(_BufferNodo, nNodo->_Texto.c_str(), static_cast<int>(nNodo->_Texto.size()), &RTexto, DT_LEFT | DT_NOPREFIX);
		SelectObject(_BufferNodo, FuenteVieja);

		// Pinto la marca del nodo (si es el _NodoMarcado) 
		if (_NodoMarcado == nNodo) {
			//int AnchoExpansorIcono = ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + ARBOLEX_PADDING;
			HBRUSH BrochaMarcaNodo = CreateSolidBrush(COLOR_ARBOL_EXPANSOR_BORDE_RESALTADO);
			FrameRect(_BufferNodo, &RFondo, BrochaMarcaNodo);
			DeleteObject(BrochaMarcaNodo);
		}

		// Pinto el resultado del buffer al DC de la función pintar
		BitBlt(hDC, Espacio->left, Espacio->top, Espacio->right, (ARBOLEX_PADDING * 2) + nNodo->_Fuente->Alto(), _BufferNodo, PosH, 0, SRCCOPY);
	}


	/* Función especial para eventos del mouse.
		- Obtiene el nodo en modo DArbolEx_HitTest_Todo.
		- El parámetro Parte devuelve la parte del nodo que se ha presionado
	*/
	// TODO : El expansor no es calcula correctament... falta o sobra un padding a la 'x' diria...
	DArbolEx_Nodo *DArbolEx::HitTest(const int cX, const int cY, DArbolEx_ParteNodo &Parte) {
		DArbolEx_Nodo *Tmp = _NodoPaginaInicio;
		int PixelesContados = _NodoPaginaVDif; // Pixeles de altura contados hasta el nodo
		int AltoNodo = 0;
		// Busco el nodo que está en la misma altura que cY
		while (Tmp != _NodoPaginaFin) {
			AltoNodo = Tmp->_Fuente->Alto();
			if (PixelesContados <= cY && PixelesContados + AltoNodo >= cY) {
				break;
			}
			PixelesContados += AltoNodo + (ARBOLEX_PADDING * 2);
			Tmp = _BuscarNodoSiguiente(Tmp, TRUE);
		}

		// Se ha encontrado un nodo a la altura del mouse
		if (Tmp != NULL) {

			// Ahora hay que crear varias rectas para determinar si el mouse está encima de una parte del nodo o no
			//			  					  Total ancestros      * Tamaño ancestro   + Padding         + Expansor            + Padding         + Icono            + Padding extra + Padding         + Ancho texto        + Padding          + 2 pixels de separación                      
			// AnchoNodo = (static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2			    + ARBOLEX_PADDING + nNodo->_AnchoTexto + ARBOLEX_PADDING) + 2;
			POINT Pt = { cX, cY };
			int iniciotodoX = 0, tamtodoX = 0;
			// Si el nodo tiene el expansor visible
			if (Tmp->MostrarExpansor() == TRUE) {
				// Compruebo si el mouse está en el expansor del nodo
				int inicioexpansorX = static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + _NodoPaginaHDif;
				int inicioexpansorY = ((Tmp->_Fuente->Alto() + (ARBOLEX_PADDING * 2)) - ARBOLEX_TAMEXPANSOR) / 2;							// ((Altura del nodo + (padding * 2)) - altura del expansor) / 2
				iniciotodoX = static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + _NodoPaginaHDif;
				tamtodoX = ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2 + ARBOLEX_PADDING + Tmp->_AnchoTexto + ARBOLEX_PADDING;
				RECT RectaExpansor = { inicioexpansorX,							PixelesContados + inicioexpansorY,
										inicioexpansorX + ARBOLEX_TAMEXPANSOR,		PixelesContados + inicioexpansorY + ARBOLEX_TAMEXPANSOR };
				//				Debug_Escribir_Varg(L"ArbolEx::HitTest left:%d top:%d right:%d bottom:%d\n", RectaExpansor.left, RectaExpansor.top, RectaExpansor.right, RectaExpansor.bottom);
				if (PtInRect(&RectaExpansor, Pt) != 0) {
					Parte = DArbolEx_ParteNodo_Expansor;
					return Tmp;
				}
			}
			else {
				iniciotodoX = static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + _NodoPaginaHDif;
				tamtodoX = ARBOLEX_TAMICONO + 2 + ARBOLEX_PADDING + Tmp->_AnchoTexto + ARBOLEX_PADDING;
			}
			// Compruebo si el mouse está en el icono del nodo
			int inicioiconoX = static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + _NodoPaginaHDif;
			int inicioiconoY = ((Tmp->_Fuente->Alto() + (ARBOLEX_PADDING * 2)) - ARBOLEX_TAMICONO) / 2;
			RECT RectaIcono = { inicioiconoX,										PixelesContados + inicioiconoY,
								 inicioiconoX + ARBOLEX_TAMICONO,					PixelesContados + inicioiconoY + ARBOLEX_TAMICONO };
			if (PtInRect(&RectaIcono, Pt) != 0) {
				Parte = DArbolEx_ParteNodo_Icono;
				return Tmp;
			}
			int iniciotexto = static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2 + _NodoPaginaHDif;
			// Compruebo si el mouse está en el texto del nodo
			RECT RectaTexto = { iniciotexto,										PixelesContados + ARBOLEX_PADDING,
								iniciotexto + Tmp->_AnchoTexto + ARBOLEX_PADDING,	PixelesContados + (ARBOLEX_PADDING * 2) + Tmp->_Fuente->Alto() };
			if (PtInRect(&RectaTexto, Pt) != 0) {
				Parte = DArbolEx_ParteNodo_Texto;
				return Tmp;
			}
			// Compruebo si el mouse está dentro del nodo (hay partes del nodo que no pertenecen ni al texto ni al icono ni al expansor)
			RECT RectaNodo = { iniciotodoX,										PixelesContados + ARBOLEX_PADDING,
								 iniciotodoX + tamtodoX,							PixelesContados + (ARBOLEX_PADDING * 2) + Tmp->_Fuente->Alto() };
			if (PtInRect(&RectaNodo, Pt) != 0) {
				Parte = DArbolEx_ParteNodo_Nodo;
				return Tmp;
			}

		}
		Parte = DArbolEx_ParteNodo_Nada;
		return NULL;
	}

	/*
		+ RAIZ
			+ N1
				+N1.1
				+N1.2
					+N1.2.1
					+N1.2.2
			+ N2
				+N2.1
				+N2.2
			+ N3
				+N3.1
				+N3.2
	*/

	/* Busca el nodo anterior del nodo especificado, devuelve NULL si se ha llegado al principio
			nActual			: Nodo desde el que se busca
			nVisible		: Determina si el nodo a buscar debe ser visible (un nodo es visible cuando su padre tiene el expansor marcado)
							  Si se especifica FALSE, encuentra tanto nodos visibles como invisibles
	*/
	DArbolEx_Nodo *DArbolEx::_BuscarNodoAnterior(DArbolEx_Nodo *nActual, const BOOL nVisible) {
		if (nActual->_Anterior == NULL) {
			if (nActual->_Padre == &_Raiz)	return NULL;
			else							return nActual->_Padre;
		}
		else { // El nodo anterior existe (busco en sus hijos el ultimo nodo)
			if (nActual->_Anterior->_Hijos.size() > 0) {
				DArbolEx_Nodo *Tmp = nActual->_Anterior;
				// Buscamos nodos visibles y el nodo anterior está expandido
				if (nVisible == TRUE && nActual->_Anterior->_Expandido == TRUE) {
					while (Tmp->TotalHijos() > 0 && Tmp->_Expandido == TRUE) {
						Tmp = Tmp->UltimoHijo();
					}
				}
				// Buscamos cualquier nodo (expandido o no)
				else if (nVisible == FALSE) {
					while (Tmp->TotalHijos() > 0) {
						Tmp = Tmp->UltimoHijo();
					}
				}
				return Tmp;
			}
		}
		return nActual->_Anterior;
	}


	/* Busca el nodo siguiente del nodo especificado, devuelve NULL si se ha llegado al final
		nActual			: Nodo desde el que se busca
		nVisible		: Determina si el nodo a buscar debe ser visible (un nodo es visible cuando su padre tiene el expansor marcado)
						  Si se especifica FALSE, encuentra tanto nodos visibles como invisibles
	*/
	DArbolEx_Nodo *DArbolEx::_BuscarNodoSiguiente(DArbolEx_Nodo *nActual, const BOOL nVisible) {
		// El nodo actual tiene hijos visibles
		if (nActual->TotalHijos() > 0) {
			if (nVisible == TRUE && nActual->_Expandido == TRUE)	return nActual->_Hijos[0];
			else if (nVisible == FALSE)									return nActual->_Hijos[0];
		}

		// El nodo actual no tiene hijos visibles
		DArbolEx_Nodo *Tmp = nActual;
		while (Tmp->_Siguiente == NULL) {
			if (Tmp->_Padre == NULL) { // Es el ultimo nodo del arbol
				return NULL;
			}
			Tmp = Tmp->_Padre;
		}
		return Tmp->_Siguiente;
	}

	void DArbolEx::Scrolls_EventoCambioPosicion(void) {
		RECT RC, RCS;
		//		GetClientRect(hWnd(), &RC);
		ObtenerRectaCliente(&RC, &RCS);
		_CalcularNodosPagina(RCS.bottom);
	}


	void DArbolEx::Expandir(DArbolEx_Nodo *nNodo, const BOOL nExpandir) {
		if (nNodo == NULL) return;

		nNodo->_Expandido = nExpandir;
		// Calculo el nuevo espacio total para los nodos visibles
		_CalcularTotalEspacioVisible();
		// Calculo las barras de scroll
		_CalcularScrolls();
		Repintar();
	}

	/* Función que obtiene la recta absoluta para el nodo 
		Si los ancestros del nodo no están expandidos la función devuelve FALSE	

		Estructura del nodo :
			[A] + P + [E] + P + [I] + P2 + [TXT] + P2
				 [A]   : Ancho para los ancestros
				 P     : Padding (si es P2 es padding + 2)
				 [E]   : Expansor
				 [I]   : Icono
				 [TXT] : Texto

		Estructura del nodo en código :
		                                            Tamaño ancestros  + Padding         + Expansor            + Padding         + Icono            + Padding extra + Padding         + Ancho texto        + Padding          + 2 pixels de separación
		(static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2             + ARBOLEX_PADDING + nNodo->_AnchoTexto + ARBOLEX_PADDING) + 2;                		*/
	const BOOL DArbolEx::ObtenerRectaNodo(DArbolEx_Nodo *rNodo, RECT &rRecta) {
		if (rNodo == NULL || _Raiz._Hijos.size() == 0) return FALSE;

		rRecta = { 0, 0, 0, 0 };
		DArbolEx_Nodo *Tmp = _Raiz._Hijos[0];
		while (Tmp != NULL) {
			if (Tmp == rNodo) {
				rRecta.left		= static_cast<LONG>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO);


				rRecta.right    = rRecta.left + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2 + ARBOLEX_PADDING + Tmp->_AnchoTexto + ARBOLEX_PADDING + 2,
				rRecta.bottom	= rRecta.top + Tmp->_Fuente->Alto() + (ARBOLEX_PADDING * 2);
				return TRUE;
			}
			rRecta.top += Tmp->_Fuente->Alto() + (ARBOLEX_PADDING * 2);
			Tmp = _BuscarNodoSiguiente(Tmp, TRUE);
		}
		return FALSE;
	}


	void DArbolEx::HacerVisible(DArbolEx_Nodo *vNodo) {
		if (vNodo == NULL) return;

		BOOL nRecalcular = FALSE;
		// En primer lugar aseguro que todos los ancestros del nodo están expandidos
		DArbolEx_Nodo *Tmp = vNodo;
		while (Tmp != NULL) {
			if (Tmp->_Padre != NULL) {
				if (Tmp->_Padre->_Expandido == FALSE) nRecalcular = TRUE; // Si no estaba expandido, marco que hay que recalcular 
				Tmp->_Padre->_Expandido = TRUE;
			}
			Tmp = Tmp->_Padre;
		}

		// Compruebo si hay que recalcular el espacio total y los scrolls
		if (nRecalcular == TRUE) {
			// Calculo el nuevo espacio total para los nodos visibles
			_CalcularTotalEspacioVisible();
			// Calculo las barras de scroll
			_CalcularScrolls();
		}

		// Obtengo la recta absoluta del nodo
		RECT RNodo = { 0, 0, 0, 0 };
		if (ObtenerRectaNodo(vNodo, RNodo) == FALSE) 
			return;										// Si no se encuentra el nodo, salgo del la función		

		// Obtengo la recta absoluta visible
		RECT RC, RAV;
		ObtenerRectaCliente(&RC, &RAV);
		// Sumo a la recta RAV las posiciones de los Scrolls V y H
		LONG YInicio = static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion);
		LONG XInicio = static_cast<LONG>((static_cast<float>(_TotalAnchoVisible) / 100.0f) * _ScrollH_Posicion);
		OffsetRect(&RAV, XInicio, YInicio);

		if (RNodo.left < RAV.left) {			// Hay una parte a la izquierda del nodo que no es visible
			_ScrollH_Posicion = (100.0f / static_cast<float>(_TotalAnchoVisible)) * static_cast<float>(RNodo.left);
		}

		if (RNodo.top < RAV.top) {				// Hay una parte del nodo que no es visible (por arriba)
			_ScrollV_Posicion = (100.0f / static_cast<float>(_TotalAltoVisible)) * static_cast<float>(RNodo.top);
		}
		else if (RNodo.bottom > RAV.bottom) {	// Hay una parte del nodo que no es visible (por abajo)
			// Sumo la diferencia de RNodo.bottom + RAV.bottom a la posición del ScrollV
			_ScrollV_Posicion += (100.0f / static_cast<float>(_TotalAltoVisible)) * static_cast<float>(RNodo.bottom - RAV.bottom);			
		}

		// Calculo los nodos InicioPagina y FinPagina
		_CalcularNodosPagina(RAV.bottom - RAV.top);

//		Debug_Escribir_Varg(L"ArbolEx::HacerVisible H:%f V:%f\n", _ScrollH_Posicion, _ScrollV_Posicion);
//		Debug_Escribir_Varg(L"ArbolEx::HacerVisible l:%d t:%d r:%d, b:%d\n", RNodo.left, RNodo.top, RNodo.right, RNodo.bottom);

		// Repinto el control
		Repintar();
	}

	/* Busca el primer nodo visible, y el ultimo nodo visible (aunque solo sean una parte de ellos) */
	void DArbolEx::_CalcularNodosPagina(const size_t TamPagina) {
		_NodoPaginaInicio = NULL;
		_NodoPaginaFin = NULL;
		if (_Raiz.TotalHijos() == 0) { return; }
		LONG   PixelInicio = static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion); // Calculo de forma temporal los pixeles que hay que contar hasta empezar a pintar
		LONG   PixelFin = static_cast<LONG>(TamPagina);
		LONG   TotalPixelesContados = -PixelInicio;
		LONG   AltoNodo = 0;
		PixelInicio = 0; // El pixel inicial se cuenta desde 0

		_NodoPaginaHDif = -static_cast<LONG>((static_cast<float>(_TotalAnchoVisible) / 100.0f) * _ScrollH_Posicion);


		//		std::wstring Ini, Fini;

		DArbolEx_Nodo *Tmp = _Raiz._Hijos[0];
		while (Tmp != NULL) {
			AltoNodo = (Tmp->_Fuente->Alto() + (ARBOLEX_PADDING * 2));
			if (_NodoPaginaInicio == NULL) {
				if (TotalPixelesContados <= PixelInicio && TotalPixelesContados + AltoNodo >= PixelInicio) {
					_NodoPaginaVDif = static_cast<LONG>(TotalPixelesContados - PixelInicio);
					_NodoPaginaInicio = Tmp;
				}
			}
			else {
				if (TotalPixelesContados >= PixelFin - AltoNodo) {
					_NodoPaginaFin = Tmp;

					/*
					if (_NodoPaginaFin != NULL) Fini = _NodoPaginaFin->Texto();
					else                        Fini = L"NULL";
					Debug_Escribir_Varg(L"ArbolEx::_CalcularNodosPagina i:%s f:%s\n", _NodoPaginaInicio->Texto().c_str(), Fini.c_str());*/


					return;
				}
			}
			TotalPixelesContados += AltoNodo;

			Tmp = _BuscarNodoSiguiente(Tmp, TRUE);
		}

		/*		if (_NodoPaginaInicio != NULL)	Ini = _NodoPaginaInicio->Texto();
				else							Ini = L"NULL";

				if (_NodoPaginaFin != NULL) Fini = _NodoPaginaFin->Texto();
				else                        Fini = L"NULL";
				Debug_Escribir_Varg(L"ArbolEx::_CalcularNodosPagina i:%s f:%s\n", Ini.c_str(), Fini.c_str());*/
	}


	// Obtiene la altura total que necesitan todos los nodos visibles (en pixeles)
	void DArbolEx::_CalcularTotalEspacioVisible(void) {
		_TotalAltoVisible = 0;
		_TotalAnchoVisible = 0;
		if (_Raiz._Hijos.size() == 0) return;

		DArbolEx_Nodo *nNodo = _Raiz._Hijos[0];
		size_t TmpAncho = 0;
		size_t TmpAlto = 0;
		while (nNodo != NULL) { //                                 Tamaño ancestros  + Padding         + Expansor            + Padding         + Icono            + Padding extra + Padding         + Ancho texto        + Padding          + 2 pixels de separación                      
			TmpAncho = (static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2             + ARBOLEX_PADDING + nNodo->_AnchoTexto + ARBOLEX_PADDING) + 2;
			if (_TotalAnchoVisible < TmpAncho) _TotalAnchoVisible = TmpAncho;

			TmpAlto = (nNodo->_Fuente->Alto() + (ARBOLEX_PADDING * 2));
			if (_MaxAltoNodo < TmpAlto) _MaxAltoNodo = TmpAlto;

			_TotalAltoVisible += TmpAlto;
			nNodo = _BuscarNodoSiguiente(nNodo, TRUE);
		}
		Debug_Escribir_Varg(L"ArbolEx::_CalcularTotalEspacioVisible an:%dpx / al:%dpx\n", _TotalAnchoVisible, _TotalAltoVisible);
	}


	void DArbolEx::_CalcularScrolls(void) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		_CalcularTotalEspacioVisible();

		BOOL SV = FALSE, SH = FALSE;
		// Determino si se necesita scroll horizontal
		if (RC.right > static_cast<LONG>(_TotalAnchoVisible)) { SH = FALSE; }
		else { SH = TRUE;	RC.bottom -= TAM_BARRA_SCROLL; }
		// Determino si se necesita scroll vertical
		if (RC.bottom > static_cast<LONG>(_TotalAltoVisible)) { SV = FALSE; }
		else { SV = TRUE;	RC.right -= TAM_BARRA_SCROLL; }

		// Borro y vuelvo a crear el buffer DC para pintar los nodos
		_CrearBufferNodo((static_cast<LONG>(_TotalAnchoVisible) > RC.right) ? static_cast<int>(_TotalAnchoVisible) : static_cast<int>(RC.right), static_cast<int>(_MaxAltoNodo));

		// calculo el ancho máximo
		if (SH == FALSE) {
			_ScrollH_Pagina = 100.0f;
		}
		else {
			_ScrollH_Pagina = ((static_cast<float>(RC.right) / static_cast<float>(_TotalAnchoVisible)) * 100.0f);
			// Si la suma de la posición del scroll y la pagina son más grandes que 100% 
			if (_ScrollH_Posicion + _ScrollH_Pagina > 100.0f) {
				_ScrollH_Posicion = 100.0f - _ScrollH_Pagina; // Ajusto la posición del scroll para que sumada con la página sea el 100%
			}
		}
		ScrollH_Visible(SH);
		// calculo la altura máxima

		if (SV == FALSE) {
			_ScrollV_Pagina = 100.0f;
		}
		else {
			_ScrollV_Pagina = ((static_cast<float>(RC.bottom) / static_cast<float>(_TotalAltoVisible)) * 100.0f);
			// Si la suma de la posición del scroll y la pagina son más grandes que 100% 
			if (_ScrollV_Posicion + _ScrollV_Pagina > 100.0f) {
				_ScrollV_Posicion = 100.0f - _ScrollV_Pagina; // Ajusto la posición del scroll para que sumada con la página sea el 100%
			}
		}
		ScrollV_Visible(SV);

		_CalcularNodosPagina(RC.bottom);

		Debug_Escribir_Varg(L"ArbolEx::_CalcularScrolls %dpx / %dpx %.02f%%\n", _TotalAltoVisible, RC.bottom, _ScrollV_Pagina);
	}

	void DArbolEx::_CrearBufferNodo(const int nAncho, const int nAlto) {
		if (_BufferNodo != NULL) {
			SelectObject(_BufferNodo, _BufferNodoBmpViejo);
			DeleteObject(_BufferNodoBmp);
			DeleteDC(_BufferNodo);
		}
		_BufferNodo = CreateCompatibleDC(NULL);
		_BufferNodoBmp = CreateCompatibleBitmap(GetDC(NULL), nAncho, nAlto);
		_BufferNodoBmpViejo = static_cast<HBITMAP>(SelectObject(_BufferNodo, _BufferNodoBmp));

		SetBkMode(_BufferNodo, TRANSPARENT);
	}

	void DArbolEx::SeleccionarNodo(DArbolEx_Nodo *sNodo) {
		if (sNodo == NULL) return;

		sNodo->_Seleccionado = TRUE;
		if (Comportamiento.SubSeleccion == TRUE) {

		}
	}

	void DArbolEx::Evento_MouseMovimiento(const int cX, const int cY, const UINT Param) {
		if (Scrolls_MouseMovimiento(cX, cY, Param) == TRUE) { return; } // las coordenadas pertenecen al scroll (salgo del evento)
		_NodoResaltado = HitTest(cX, cY, _NodoResaltadoParte);

		//		Debug_Escribir_Varg(L"ArbolEx::Evento_MouseMovimiento x:%d y:%d p:%d\n", cX, cY, Parte);		
		if (_NodoUResaltado != _NodoResaltado || _NodoResaltadoParte != _NodoUResaltadoParte) {
			_NodoUResaltado = _NodoResaltado;
			_NodoUResaltadoParte = _NodoResaltadoParte;
			Repintar(); // TODO : substituir per un RepintarNodo(nNodo, nUNodo)
		}

		//		if (nNodo != NULL) { Debug_Escribir_Varg(L"DArbolEx::Evento_MouseMovimiento %s\n", nNodo->Texto().c_str()); }
				//else               { Debug_Escribir_Varg(L"DArbolEx::Evento_MouseMovimiento NULL\n"); }
	}

	void DArbolEx::Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param) {
		SetFocus(hWnd());
		if (Scrolls_MousePresionado(Boton, cX, cY, Param) == TRUE) { return; }


		_NodoPresionado = HitTest(cX, cY, _NodoResaltadoParte);
		_NodoPresionadoParte = _NodoResaltadoParte;
		_NodoMarcado = _NodoPresionado;

		_DesSeleccionarTodo();
		SeleccionarNodo(_NodoPresionado);
		//_NodoPresionado->_Seleccionado = TRUE;

		Repintar();
	}

	void DArbolEx::Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param) {
		if (Scrolls_MouseSoltado(Boton, cX, cY, Param) == TRUE) { return; }

		DArbolEx_ParteNodo nParte;
		DArbolEx_Nodo *NodoSoltado = HitTest(cX, cY, nParte);
		if (NodoSoltado != NULL) {
			// expansor
			if (NodoSoltado == _NodoPresionado && nParte == DArbolEx_ParteNodo_Expansor) {
				Expandir(_NodoPresionado, !_NodoPresionado->Expandido());
			}
			// el resto del nodo
			else if (NodoSoltado == _NodoPresionado && static_cast<int>(nParte) > static_cast<int>(DArbolEx_ParteNodo_Expansor)) {
				NodoSoltado->_Seleccionado = TRUE;
			}
		}
		_NodoPresionadoParte = DArbolEx_ParteNodo_Nada;
		_NodoPresionado = NULL;
		Repintar();
	}

	void DArbolEx::_DesSeleccionarTodo(void) {
		// Si no hay nodos salgo
		if (_Raiz._Hijos.size() == 0) return;
		DArbolEx_Nodo *dNodo = _Raiz._Hijos[0];

		while (dNodo != NULL) {
			dNodo->_Seleccionado = FALSE;
			dNodo->_SubSeleccionado = FALSE;
			dNodo = _BuscarNodoSiguiente(dNodo, FALSE);
		}
	}

	void DArbolEx::_Tecla_CursorAbajo(void) {
		if (_Raiz._Hijos.size() == 0) return;

		// Si no hay ningun nodo marcado, marcamos el primero del arbol
		if (_NodoMarcado == NULL) {
			_NodoMarcado = _NodoPaginaInicio;
			_NodoMarcado->_Seleccionado = TRUE;
			HacerVisible(_NodoMarcado);
			return;
		}

		DArbolEx_Nodo *nTmp;
		nTmp = _BuscarNodoSiguiente(_NodoMarcado, TRUE);
		if (nTmp != NULL) {
			_DesSeleccionarTodo();
			_NodoMarcado = nTmp;
			nTmp->_Seleccionado = TRUE;
			HacerVisible(nTmp);
		}
	}

	void DArbolEx::_Tecla_CursorArriba(void) {
		if (_Raiz._Hijos.size() == 0) return;

		// Si no hay ningun nodo marcado, marcamos el primero del arbol
		if (_NodoMarcado == NULL) {
			_NodoMarcado = _NodoPaginaInicio;
			_NodoMarcado->_Seleccionado = TRUE;
			HacerVisible(_NodoMarcado);
			return;
		}

		DArbolEx_Nodo *nTmp;
		nTmp = _BuscarNodoAnterior(_NodoMarcado, TRUE);
		if (nTmp != NULL) {
			_DesSeleccionarTodo();
			_NodoMarcado = nTmp;
			nTmp->_Seleccionado = TRUE;
			HacerVisible(nTmp);
		}
	}

	void DArbolEx::Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		if (_Raiz._Hijos.size() == 0) return;
		// Si no existe un nodo marcado lo asignamos al inicio de la página
//		if (_NodoMarcado == NULL) _NodoMarcado = _NodoPaginaInicio;

		switch (Caracter) {
		case VK_HOME: break;
		case VK_END: break;
		case VK_UP:
			_Tecla_CursorArriba();
			break;
		case VK_DOWN:
			_Tecla_CursorAbajo();
			break;
		case VK_LEFT:
			Expandir(_NodoMarcado, FALSE);
			break;
		case VK_RIGHT:
			Expandir(_NodoMarcado, TRUE);
			break;
		case VK_PRIOR: break; // RePag
		case VK_NEXT: break;  // AvPag
		default: break; // if (Caracter >= 0x30 && Caracter <= 0x5A) // Cualquier tecla valida
		}
	}

	void DArbolEx::Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		if (_Raiz._Hijos.size() == 0) return;
	}

	void DArbolEx::Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param) {

	}

	// Devuelve TRUE si hay que repintar, FAALSE en caso contrario
	const BOOL DArbolEx::Evento_MouseEntrando(void) { // WM_MOUSEENTER
		return FALSE;
	}

	// Devuelve TRUE si hay que repintar, FAALSE en caso contrario
	const BOOL DArbolEx::Evento_MouseSaliendo(void) { // WM_MOUSELEAVE
		return FALSE;
	}

	LRESULT CALLBACK DArbolEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		BOOL nRepintar = FALSE;
		switch (uMsg) {
			case WM_SETFOCUS:
				Debug_Escribir(L"DArbolEx::SetFocus()\n");
				break;
			case WM_KILLFOCUS :
				Debug_Escribir(L"DArbolEx::KillFocus()\n");
				break;
			case WM_KEYDOWN:
				Evento_TeclaPresionada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
				return 0;
			case WM_KEYUP:
				Evento_TeclaSoltada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
				return 0;		
			case WM_CHAR:
				Evento_Tecla(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
//				Debug_Escribir(L"DArbolEx::CrearArbolEx() Error : ya se ha creado el arbol\n");
				return 0;
			case WM_SIZE:
				_CalcularScrolls();
				Repintar();
				return 0;

			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				Pintar(DC);
				EndPaint(hWnd(), &PS);
				return 0;

			case WM_MOUSEMOVE:
				if (_MouseEntrando() == TRUE) {
					Scrolls_MouseEntrando();
					Evento_MouseEntrando();
				}

				Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				return 0;

			case WM_MOUSELEAVE: // Enviado gracias a WM_MOUSEMOVE -> _MouseEntrando()
				nRepintar = Scrolls_MouseSaliendo();
				_MouseDentro = FALSE;
				Evento_MouseSaliendo();
				if (nRepintar == TRUE) Repintar();
				return 0;

			case WM_LBUTTONDOWN:	Evento_MousePresionado(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));		return 0;
			case WM_RBUTTONDOWN:	Evento_MousePresionado(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));		return 0;
			case WM_MBUTTONDOWN:	Evento_MousePresionado(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));		return 0;

			case WM_LBUTTONUP:		Evento_MouseSoltado(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));			return 0;
			case WM_RBUTTONUP:		Evento_MouseSoltado(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));			return 0;
			case WM_MBUTTONUP:		Evento_MouseSoltado(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));			return 0;

		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	};

}