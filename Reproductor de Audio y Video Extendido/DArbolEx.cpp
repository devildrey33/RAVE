//#pragma once
#include "stdafx.h"
#include "DListaIconos.h"
#include "DMensajesWnd.h"
#include "DArbolEx.h"


/*		_________ _______  ______   _______																											 _  _ 
		\__   __/(  ___  )(  __  \ (  ___  )	   - Fer el teclat pels caracters, que detecti si es un caracter o una paraula  			        ( )( )
		   ) (   | (   ) || (  \  )| (   ) |   _   - Finiquitar el shift																			| || |
		   | |   | |   | || |   ) || |   | |  (_)  - LabelEdit																						| || |
		   | |   | |   | || |   | || |   | |       - Drag&Drop ?¿?¿   																				| || |
		   | |   | |   | || |   ) || |   | |   _   - Events enviats a la finstrea pare																(_)(_)
		   | |   | (___) || (__/  )| (___) |  (_)	    																							 _  _ 
		   )_(   (_______)(______/ (_______)           																								(_)(_)
*/

namespace DWL {

	DArbolEx::DArbolEx(void) : DBarraScrollEx(),
  	  _NodoMarcado(NULL)		, _NodoShift(NULL)								, _PosShift(0)				,							// Info nodo Marcado y Shift
	  _NodoPresionado(NULL)		, _NodoPresionadoParte(DArbolEx_ParteNodo_Nada)	, _Repintar(FALSE)			,							// Info nodo presionado
	  _NodoResaltado(NULL)		, _NodoResaltadoParte(DArbolEx_ParteNodo_Nada)	, MultiSeleccion(TRUE)		,							// Info nodo resaltado
	  _NodoUResaltado(NULL)		, _NodoUResaltadoParte(DArbolEx_ParteNodo_Nada)	, SubSeleccion(FALSE)		,							// Info ultimo nodo resaltado (para evitar repintados innecesarios en el mousemove)
	  _TotalAnchoVisible(0)		, _TotalAltoVisible(0)							,														// tamaño
	  _NodoPaginaInicio(NULL)	, _NodoPaginaFin(NULL)							, _NodoPaginaVDif(0)		, _NodoPaginaHDif(0),		// Nodo inicial y final de la página visible
	  _BufferNodo(NULL)			, _BufferNodoBmp(NULL)							, _BufferNodoBmpViejo(NULL)							{	// Buffer para pintar un solo nodo

		_Raiz.Expandido = TRUE;
		_Raiz._Arbol = this;
//		ColorFondoScroll = COLOR_ARBOL_FONDO_SCROLL;
	};

	DArbolEx::~DArbolEx(void) {
		BorrarTodo();
		// Elimino el buffer para pintar el nodo
		if (_BufferNodo != NULL) {
			SelectObject(_BufferNodo, _BufferNodoBmpViejo);
			DeleteObject(_BufferNodoBmp);
			DeleteDC(_BufferNodo);
		}
	}

	HWND DArbolEx::CrearArbolEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, DWORD nEstilos) {
		if (hWnd()) {
			#if DARBOLEX_MOSTRARDEBUG == TRUE
				Debug_Escribir(L"DArbolEx::CrearArbolEx() Error : ya se ha creado el arbol\n");
			#endif
			return hWnd();
		}
		
		_hWnd = CrearControlEx(nPadre, L"DArbolEx", L"", cID, cX, cY, cAncho, cAlto, nEstilos, NULL, CS_DBLCLKS);  // CS_DBLCLKS (el control recibe notificaciones de doble click)
		Fuente = Fuente18Normal;
		return hWnd();
	}

	void DArbolEx::BorrarTodo(void) {
		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DArbolEx::BorrarTodo()\n");
		#endif

		for (size_t i = 0; i < _Raiz._Hijos.size(); i++) {
			delete _Raiz._Hijos[i];
		}
		_Raiz._Hijos.resize(0);

		_NodoPaginaInicio		= NULL;
		_NodoPaginaFin			= NULL;
		_NodoMarcado			= NULL;
		_NodoPresionado			= NULL;
		_NodoResaltado			= NULL;
		_NodoUResaltado			= NULL;
		_NodoShift				= NULL;
		_PosShift				= 0;
		_NodoPaginaVDif			= 0;
		_NodoPaginaHDif			= 0;
		_NodoPresionadoParte	= DArbolEx_ParteNodo_Nada;
		_NodoResaltadoParte		= DArbolEx_ParteNodo_Nada;
		_NodoUResaltadoParte	= DArbolEx_ParteNodo_Nada;

		_Repintar = TRUE;
	}

	DArbolEx_Nodo *DArbolEx::_AgregarNodo(DArbolEx_Nodo *nNodo, const TCHAR *nTexto, DArbolEx_Nodo *nPadre, DListaIconos_Icono *nIcono, DhWnd_Fuente *nFuente, const size_t PosicionNodo) {
		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DArbolEx::_AgregarNodo(%s)\n", nTexto);
		#endif

		// Asigno el arbol padre
		nNodo->_Arbol = this;

		// Asigno el texto
		nNodo->Texto = nTexto;
		// Cargo el icono (si existe)
/*		if (nIcono == NULL) {
			nNodo->_Icono = NULL;
		}*/
		nNodo->_Icono = nIcono;

		// Compruebo la fuente (si no es NULL asigno la nueva fuente)
		if (nFuente != NULL) nNodo->_Fuente = nFuente;
		else                 nNodo->_Fuente = &Fuente; // no se ha especificado la fuente, asigno la fuente por defecto del arbol

														// Calculo y asigno el ancho del texto
		nNodo->_AnchoTexto = nNodo->_Fuente->Tam(nNodo->Texto).cx;

		// Compruebo el padre (si es null nPadre será _Raiz)
		if (nPadre == NULL) {
			nPadre = &_Raiz;
		}
		// Asigno el nodo padre y el total de ancestros para este nodo
		nNodo->_Padre = nPadre;
		nNodo->_Ancestros = nPadre->_Ancestros + 1;

		// Si el padre está expandido, sumo la altura de este nodo al total 
		if (nNodo->_Padre->Expandido == TRUE) {
			// Cuento los pixeles de todos los nodos visibles
			_TotalAltoVisible += (nNodo->_Fuente->Alto() + (DARBOLEX_PADDING * 2));
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
					if (_wcsicmp(nTexto, nPadre->Hijo(nPos)->Texto.c_str()) < 0) break;
				}
				// enlazo el nodo anterior
				if (nPadre->TotalHijos() > 0 && nPos > 0) { nNodo->_Anterior = nPadre->_Hijos[nPos - 1]; }

				// Enlazo el nodo siguiente
				if (nPos > 0) { nPadre->_Hijos[nPos - 1]->_Siguiente = nNodo; }
				if (nPos < nPadre->_Hijos.size()) { nNodo->_Siguiente = nPadre->_Hijos[nPos]; }

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

		// Recalcular valores del arbol en el próximo repintado
		_Repintar = TRUE;
		return nNodo;
	}

	void DArbolEx::EliminarNodo(DArbolEx_Nodo *nEliminar) {
		// Obtengo el padre del nodo
		DArbolEx_Nodo *ePadre = nEliminar->_Padre;
		// El nodo especificado es la raíz..
		if (ePadre == NULL) return;
		// Busco el nodo a eliminar dentro del vector de hijos de su padre
		for (size_t i = 0; i < ePadre->_Hijos.size(); i++) {
			if (nEliminar == ePadre->_Hijos[i]) {
				ePadre->_Hijos.erase(ePadre->_Hijos.begin() + i);
				break;
			}
		}
		// Si existe un nodo anterior, reasigno el siguiente nodo del anterior
		if (nEliminar->_Anterior != NULL) nEliminar->_Anterior->_Siguiente = nEliminar->_Siguiente;
		// Si existe un nodo siguiente, reasigno el anterior nodo del siguiente
		if (nEliminar->_Siguiente != NULL) nEliminar->_Siguiente->_Anterior = nEliminar->_Anterior;
		// Borro el nodo de la memória
		delete nEliminar;
		// En el próximo repintado se recalcularan los tamaños del arbol	
		_Repintar = TRUE;
	}

	// Devuelve el primer nodo visible, o NULL si no hay nodos
	DArbolEx_Nodo *DArbolEx::PrimerNodoVisible(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return NULL;

		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DArbolEx::PrimerNodoVisible = %s\n", _Raiz._Hijos[0]->Texto.c_str());
		#endif

		// Devuelvo el primer nodo
		return _Raiz._Hijos[0];
	}

	// Devuelve el último nodo visible, o NULL si no hay nodos
	DArbolEx_Nodo *DArbolEx::UltimoNodoVisible(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return NULL;
		// Busco el ultimo nodo visible
		DArbolEx_Nodo *Tmp = _Raiz._Hijos[_Raiz._Hijos.size() - 1];
		// Si está expandido y tiene hijos, ubico el puntero Tmp al primer hijo
		while (Tmp->Expandido == TRUE && Tmp->_Hijos.size() > 0) {
			Tmp = Tmp->_Hijos[Tmp->_Hijos.size() -1];
		}
		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DArbolEx::PrimerNodoVisible = %s\n", Tmp->Texto.c_str());
		#endif
		return Tmp;
	}


	void DArbolEx::Pintar(HDC hDC) {
		// Compruebo si hay que recalcular los valores del arbol antes de pintar
		if (_Repintar == TRUE) {			
			_CalcularScrolls();
		}

		RECT	RC, RCS;
		ObtenerRectaCliente(&RC, &RCS);

		HDC		Buffer			= CreateCompatibleDC(hDC);
		HBITMAP Bmp				= CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
		HBITMAP BmpViejo		= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		DArbolEx_Nodo *nActual	= _NodoPaginaInicio;

		LONG PixelInicio		= static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion);
		LONG PixelFin			= PixelInicio + RCS.bottom;
		LONG nAlto				= 0;
		// Diferencia de pixeles Vertical (puede ser negativa si el primer nodo es parcialmente visible, o 0 si el primer nodo está justo al principio del área visible)
		LONG DifInicioV			= _NodoPaginaVDif;
		// Ultimo nodo visible (puede ser NULL si el espacio del arbol es mas grande que todos los nodos visibles)
		DArbolEx_Nodo *NodoFin = _NodoPaginaFin;
		// El ultimo nodo a pintar debe ser el siguiente al ultimo nodo visible (si es posible)s
		if (NodoFin != NULL) NodoFin = _NodoPaginaFin->_Siguiente;

		RECT RectaItem; // = { 0, DifInicio, RCS.right, DifInicio + nActual->_Fuente->Alto() };
		while (nActual != NodoFin && nActual != NULL) {
			nAlto = nActual->_Fuente->Alto() + (DARBOLEX_PADDING * 2);
			RectaItem = { 0, DifInicioV, RCS.right, DifInicioV + nAlto };
			PintarNodo(Buffer, &RectaItem, nActual, -_NodoPaginaHDif);
			DifInicioV += nAlto;							// Sumo la altura del nodo a la diferencia inicial
			nActual = BuscarNodoSiguiente(nActual, TRUE);
		}

		// Pinto el resto del fondo
		if (DifInicioV < RCS.bottom) {
			RECT RFondo = RCS; RFondo.top = DifInicioV;
			HBRUSH BFondo = CreateSolidBrush(COLOR_ARBOL_FONDO);
			FillRect(Buffer, &RFondo, BFondo);
			DeleteObject(BFondo);
		}

		// Pinto las barras de scroll en el buffer
		Scrolls_Pintar(Buffer, RC);

		// Pinto el buffer en el DC
		BitBlt(hDC, RC.left, RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino objetos gdi de la memoria
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}

	void DArbolEx::PintarNodo(HDC hDC, RECT *Espacio, DArbolEx_Nodo *nNodo, const int PosH) {
		// Determino el estado del nodo (0 normal, 1 presionado, 2 resaltado, 3 seleccionado, 4 presionado, 5 resaltado, 6 sub-seleccionado, 7 presionado, 8 resaltado)
		int Estado = 0;

		// Normal +0, Presionado +1, Resaltado +2
		if			(nNodo == _NodoPresionado) 			Estado = 1;
		else if		(nNodo == _NodoResaltado)			Estado = 2;

		// Activado +0, Desactivado +9
		if			(nNodo->Activado == FALSE)			Estado += 9;
		else {
			// Seleccionado +3, SubSeleccionado +6
			if		(nNodo->Seleccionado == TRUE)		Estado += 3;
			else if (nNodo->_SubSeleccionado == TRUE)   Estado += 6;
		}

		COLORREF ColFondo = NULL, ColTexto = NULL, ColSombra = NULL;
		switch (Estado) {
			case 0: // Activado Normal
				ColTexto  = COLOR_ARBOL_TEXTO;
				ColSombra = COLOR_ARBOL_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_FONDO;
				break;
			case 1: // Activado Presionado
				ColTexto  = COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO;
				ColSombra = COLOR_ARBOL_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_FONDO_PRESIONADO;
				break;
			case 2: // Activado Resaltado
				ColTexto  = COLOR_ARBOL_TEXTO_RESALTADO;
				ColSombra = COLOR_ARBOL_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_FONDO_RESALTADO;
				break;
			case 3: // Activado Seleccionado
				ColTexto  = COLOR_ARBOL_SELECCION_TEXTO;
				ColSombra = COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_SELECCION;
				break;
			case 4: // Activado Seleccionado presionado
				ColTexto  = COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO;
				ColSombra = COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_SELECCION_PRESIONADO;
				break;
			case 5: // Activado Seleccionado resaltado
				ColTexto  = COLOR_ARBOL_SELECCION_TEXTO_RESALTADO;
				ColSombra = COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_SELECCION_RESALTADO;
				break;
			case 6: // Activado Sub-Seleccionado
				ColTexto  = COLOR_ARBOL_SUBSELECCION_TEXTO;
				ColSombra = COLOR_ARBOL_SUBSELECCION_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_SUBSELECCION;
				break;
			case 7: // Activado Sub-Seleccionado presionado
				ColTexto  = COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO;
				ColSombra = COLOR_ARBOL_SUBSELECCION_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_SELECCION_PRESIONADO;
				break;
			case 8: // Activado Sub-Seleccionado resaltado
				ColTexto  = COLOR_ARBOL_SUBSELECCION_TEXTO_RESALTADO;
				ColSombra = COLOR_ARBOL_SUBSELECCION_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_SUBSELECCION_RESALTADO;
				break;
			case 9: // Desactivado Normal
				ColTexto  = COLOR_ARBOL_TEXTO_DESACTIVADO;
				ColSombra = COLOR_ARBOL_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_FONDO;
				break;
			case 10: // Desactivado Presionado
			case 11: // Desactivado Resaltado
				ColTexto  = COLOR_ARBOL_TEXTO_DESACTIVADO;
				ColSombra = COLOR_ARBOL_TEXTO_SOMBRA;
				ColFondo  = COLOR_ARBOL_FONDO_RESALTADO;
				break;
		}

		// Pinto el fondo del buffer
		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_ARBOL_FONDO);
		RECT EspacioLocal = { 0 , 0, (static_cast<LONG>(_TotalAnchoVisible) > Espacio->right) ? static_cast<LONG>(_TotalAnchoVisible) : Espacio->right, (2 * DARBOLEX_PADDING) + nNodo->_Fuente->Alto() };
		FillRect(_BufferNodo, &EspacioLocal, BrochaFondo);
		DeleteObject(BrochaFondo);

		int		AnchoOcupado = static_cast<int>((nNodo->_Ancestros - 1) * DARBOLEX_TAMICONO) + DARBOLEX_PADDING;  // ARBOLEX_PADDING + (static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO));
		// Determino si hay que mostrar el expansor
		if (nNodo->MostrarExpansor() == TRUE) {
			HBRUSH	BrochaBordeExpansor = NULL; // color del borde del expansor
			HPEN	PlumaExpansor		= NULL; // color de los simbolos '+' y '-'

			int PosExpansorY = ((Espacio->bottom - Espacio->top) - DARBOLEX_TAMEXPANSOR) / 2;
			int EPresionado = 0; // Pixel de mas (si el expansor está presionado)
			if (_NodoPresionado == nNodo && _NodoResaltadoParte == DArbolEx_ParteNodo_Expansor) { EPresionado = 1; }
			RECT MarcoExpansor = {	EPresionado + AnchoOcupado,
									EPresionado + PosExpansorY,
									EPresionado + AnchoOcupado + DARBOLEX_TAMEXPANSOR,
									EPresionado + PosExpansorY + DARBOLEX_TAMEXPANSOR };
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
			FrameRect(_BufferNodo, &MarcoExpansor, BrochaBordeExpansor);

			HPEN VPluma = static_cast<HPEN>(SelectObject(_BufferNodo, PlumaExpansor));
			// Pinto una linea horizontal para el expansor
			MoveToEx(_BufferNodo, EPresionado + AnchoOcupado + DARBOLEX_PADDING, EPresionado + PosExpansorY + DARBOLEX_TAMEXPANSOR / 2, NULL);
			LineTo(_BufferNodo, EPresionado + AnchoOcupado + DARBOLEX_TAMEXPANSOR - DARBOLEX_PADDING, EPresionado + PosExpansorY + DARBOLEX_TAMEXPANSOR / 2);
			if (nNodo->Expandido == FALSE) {
				// Pinto una linea vertical para el expansor
				MoveToEx(_BufferNodo, EPresionado + AnchoOcupado + (DARBOLEX_TAMEXPANSOR / 2), EPresionado + PosExpansorY + 2, NULL);
				LineTo(_BufferNodo, EPresionado + AnchoOcupado + (DARBOLEX_TAMEXPANSOR / 2), EPresionado + PosExpansorY + DARBOLEX_TAMEXPANSOR - 2);
			}

			DeleteObject(BrochaBordeExpansor);
			SelectObject(_BufferNodo, VPluma);
			DeleteObject(PlumaExpansor);
		}
		AnchoOcupado += DARBOLEX_PADDING + DARBOLEX_TAMEXPANSOR;

		int SPresionado = 0; // pixel extra a sumar en la 'x' y 'y' iniciales de los rectangulos del Icono, FondoTexto y Texto
		if (_NodoPresionado == nNodo && _NodoPresionadoParte > DArbolEx_ParteNodo_Expansor) { SPresionado = 1; }

		// Pinto el icono
		int PosIcoY = SPresionado + (((Espacio->bottom - Espacio->top) - DARBOLEX_TAMICONO) / 2);
		int PosIcoX = SPresionado + (AnchoOcupado + DARBOLEX_PADDING);
		DrawIconEx(_BufferNodo, PosIcoX, PosIcoY, nNodo->_Icono->Icono(), DARBOLEX_TAMICONO, DARBOLEX_TAMICONO, 0, 0, DI_NORMAL);
		AnchoOcupado += DARBOLEX_TAMICONO + DARBOLEX_PADDING + 2;

		// Pinto el fondo del texto del nodo (tanto si está seleccionado como si no)
		RECT RFondo = { SPresionado + (AnchoOcupado - DARBOLEX_PADDING),
						SPresionado + (DARBOLEX_MARGEN_Y_SELECCION * 2),
						SPresionado + (AnchoOcupado + nNodo->_AnchoTexto) + DARBOLEX_PADDING,
						SPresionado + ((DARBOLEX_PADDING * 2) + nNodo->_Fuente->Alto()) - DARBOLEX_MARGEN_Y_SELECCION };

		HBRUSH BrochaFondoTexto = CreateSolidBrush(ColFondo);
		FillRect(_BufferNodo, &RFondo, BrochaFondoTexto);
		DeleteObject(BrochaFondoTexto);

		// Seleciono la fuente del nodo
		HFONT FuenteVieja = static_cast<HFONT>(SelectObject(_BufferNodo, nNodo->_Fuente->Fuente()));

		// Pinto la sombra del texto
		SetTextColor(_BufferNodo, ColSombra);
		RECT RTexto = { SPresionado + AnchoOcupado								+1,
						SPresionado + DARBOLEX_PADDING							+1,
						SPresionado + AnchoOcupado + nNodo->_AnchoTexto			+1,
						SPresionado + DARBOLEX_PADDING + nNodo->_Fuente->Alto()	+1 };
		DrawText(_BufferNodo, nNodo->Texto.c_str(), static_cast<int>(nNodo->Texto.size()), &RTexto, DT_LEFT | DT_NOPREFIX);

		// Pinto el texto del nodo
		SetTextColor(_BufferNodo, ColTexto);
		RTexto = {	SPresionado + AnchoOcupado,
					SPresionado + DARBOLEX_PADDING,
					SPresionado + AnchoOcupado + nNodo->_AnchoTexto,
					SPresionado + DARBOLEX_PADDING + nNodo->_Fuente->Alto() };
		DrawText(_BufferNodo, nNodo->Texto.c_str(), static_cast<int>(nNodo->Texto.size()), &RTexto, DT_LEFT | DT_NOPREFIX);

		// Vuelvo a seleccionar la fuente inicial del DC
		SelectObject(_BufferNodo, FuenteVieja);

		// Pinto la marca del nodo (si es el _NodoMarcado) 
		if (_NodoMarcado == nNodo) {
			//int AnchoExpansorIcono = ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + ARBOLEX_PADDING;
			HBRUSH BrochaMarcaNodo = CreateSolidBrush(COLOR_ARBOL_NODO_MARCA);
//			HBRUSH BrochaMarcaNodo = CreateSolidBrush((_NodoMarcado->Seleccionado == TRUE) ? COLOR_ARBOL_NODO_MARCA_SELECCIONADO : COLOR_ARBOL_NODO_MARCA);
			FrameRect(_BufferNodo, &RFondo, BrochaMarcaNodo);
			DeleteObject(BrochaMarcaNodo);
		}

		// Pinto el resultado del buffer al DC de la función pintar
		BitBlt(hDC, Espacio->left, Espacio->top, Espacio->right, (DARBOLEX_PADDING * 2) + nNodo->_Fuente->Alto(), _BufferNodo, PosH, 0, SRCCOPY);
	}


	/* Función especial para eventos del mouse.
		- Obtiene el nodo en modo DArbolEx_HitTest_Todo.
		- El parámetro Parte devuelve la parte del nodo que se ha presionado
	*/	
	DArbolEx_Nodo *DArbolEx::HitTest(const int cX, const int cY, DArbolEx_ParteNodo &Parte) {
		DArbolEx_Nodo *Tmp = _NodoPaginaInicio;
		int PixelesContados = _NodoPaginaVDif; // Pixeles de altura contados hasta el nodo
		int AltoNodo = 0;
		// Busco el nodo que está en la misma altura que cY
		while (Tmp != _NodoPaginaFin) {
			AltoNodo = Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2);
			if (PixelesContados <= cY && PixelesContados + AltoNodo >= cY) {
				break;
			}
			PixelesContados += AltoNodo;
			Tmp = BuscarNodoSiguiente(Tmp, TRUE);
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
				int inicioexpansorX = static_cast<int>((Tmp->_Ancestros - 1) * DARBOLEX_TAMICONO) + DARBOLEX_PADDING + _NodoPaginaHDif;
				int inicioexpansorY = ((Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2)) - DARBOLEX_TAMEXPANSOR) / 2;							// ((Altura del nodo + (padding * 2)) - altura del expansor) / 2
				iniciotodoX = static_cast<int>((Tmp->_Ancestros - 1) * DARBOLEX_TAMICONO) + DARBOLEX_PADDING + _NodoPaginaHDif;
				tamtodoX = DARBOLEX_TAMEXPANSOR + DARBOLEX_PADDING + DARBOLEX_TAMICONO + 2 + DARBOLEX_PADDING + Tmp->_AnchoTexto + DARBOLEX_PADDING;
				RECT RectaExpansor = {  inicioexpansorX,							PixelesContados + inicioexpansorY,
										inicioexpansorX + DARBOLEX_TAMEXPANSOR,		PixelesContados + inicioexpansorY + DARBOLEX_TAMEXPANSOR };
				if (PtInRect(&RectaExpansor, Pt) != 0) {
					Parte = DArbolEx_ParteNodo_Expansor;
					#if DARBOLEX_MOSTRARDEBUG == TRUE
						Debug_Escribir_Varg(L"DArbolEx::HitTest(%d, %d) = Expansor de '%s'\n", cX, cY, Tmp->Texto.c_str());
					#endif
					return Tmp;
				}
			}
			else {
				iniciotodoX = static_cast<int>((Tmp->_Ancestros - 1) * DARBOLEX_TAMICONO) + DARBOLEX_PADDING + DARBOLEX_TAMEXPANSOR + DARBOLEX_PADDING + _NodoPaginaHDif;
				tamtodoX = DARBOLEX_TAMICONO + 2 + DARBOLEX_PADDING + Tmp->_AnchoTexto + DARBOLEX_PADDING;
			}
			// Compruebo si el mouse está en el icono del nodo
			int inicioiconoX = static_cast<int>((Tmp->_Ancestros - 1) * DARBOLEX_TAMICONO) + DARBOLEX_PADDING + DARBOLEX_TAMEXPANSOR + DARBOLEX_PADDING + _NodoPaginaHDif;
			int inicioiconoY = ((Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2)) - DARBOLEX_TAMICONO) / 2;
			RECT RectaIcono = {  inicioiconoX,										PixelesContados + inicioiconoY,
								 inicioiconoX + DARBOLEX_TAMICONO,					PixelesContados + inicioiconoY + DARBOLEX_TAMICONO };
			if (PtInRect(&RectaIcono, Pt) != 0) {
				Parte = DArbolEx_ParteNodo_Icono;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx::HitTest(%d, %d) = Icono de '%s'\n", cX, cY, Tmp->Texto.c_str());
				#endif
				return Tmp;
			}
			int iniciotexto = static_cast<int>((Tmp->_Ancestros - 1) * DARBOLEX_TAMICONO) + DARBOLEX_PADDING + DARBOLEX_TAMEXPANSOR + DARBOLEX_PADDING + DARBOLEX_TAMICONO + 2 + _NodoPaginaHDif;
			// Compruebo si el mouse está en el texto del nodo
			RECT RectaTexto = { iniciotexto,										PixelesContados + DARBOLEX_PADDING,
								iniciotexto + Tmp->_AnchoTexto + DARBOLEX_PADDING,	PixelesContados + (DARBOLEX_PADDING * 2) + Tmp->_Fuente->Alto() };
			if (PtInRect(&RectaTexto, Pt) != 0) {
				Parte = DArbolEx_ParteNodo_Texto;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx::HitTest(%d, %d) = Texto de '%s'\n", cX, cY, Tmp->Texto.c_str());
				#endif
				return Tmp;
			}
			// Compruebo si el mouse está dentro del nodo (hay partes del nodo que no pertenecen ni al texto ni al icono ni al expansor)
			RECT RectaNodo = { iniciotodoX,											PixelesContados + DARBOLEX_PADDING,
								 iniciotodoX + tamtodoX,							PixelesContados + (DARBOLEX_PADDING * 2) + Tmp->_Fuente->Alto() };
			if (PtInRect(&RectaNodo, Pt) != 0) {
				Parte = DArbolEx_ParteNodo_Nodo;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx::HitTest(%d, %d) = Marco de '%s'\n", cX, cY, Tmp->Texto.c_str());
				#endif
				return Tmp;
			}

		}

		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DArbolEx::HitTest(%d, %d) = Nada\n", cX, cY);
		#endif
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
	DArbolEx_Nodo *DArbolEx::BuscarNodoAnterior(DArbolEx_Nodo *nActual, const BOOL nVisible) {
		if (nActual == NULL) return NULL;

		if (nActual->_Anterior == NULL) {
			if (nActual->_Padre == &_Raiz)	return NULL;
			else							return nActual->_Padre;
		}
		else { // El nodo anterior existe (busco en sus hijos el ultimo nodo)
			if (nActual->_Anterior->_Hijos.size() > 0) {
				DArbolEx_Nodo *Tmp = nActual->_Anterior;
				// Buscamos nodos visibles y el nodo anterior está expandido
				if (nVisible == TRUE && nActual->_Anterior->Expandido == TRUE) {
					while (Tmp->TotalHijos() > 0 && Tmp->Expandido == TRUE) {
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
		DentroDe        : Si especificas un nodo la busqueda finalizará cuando se salga de él
	*/
	DArbolEx_Nodo *DArbolEx::BuscarNodoSiguiente(DArbolEx_Nodo *nActual, const BOOL nVisible, DArbolEx_Nodo *DentroDe) {
		if (nActual == NULL) return NULL;

		// El nodo actual tiene hijos visibles
		if (nActual->TotalHijos() > 0) {
			if		(nVisible == TRUE && nActual->Expandido == TRUE)	return nActual->_Hijos[0];
			else if (nVisible == FALSE)									return nActual->_Hijos[0];
		}

		// El nodo actual no tiene hijos visibles
		DArbolEx_Nodo *Tmp = nActual;
		while (Tmp->_Siguiente == NULL) {
			if (Tmp->_Padre == NULL || Tmp->_Padre == DentroDe) { // Es el ultimo nodo del arbol
				return NULL;
			}
			Tmp = Tmp->_Padre;
		}
		return Tmp->_Siguiente;
	}


	/* Evento que se recibe al cambiar el scroll de posición */
	void DArbolEx::Scrolls_EventoCambioPosicion(void) {
		RECT RC, RCS;
		//		GetClientRect(hWnd(), &RC);
		ObtenerRectaCliente(&RC, &RCS);
		_CalcularNodosPagina(RCS.bottom);
	}


	void DArbolEx::Expandir(DArbolEx_Nodo *nNodo, const BOOL nExpandir) {
		if (nNodo == NULL) return;

		nNodo->Expandido = nExpandir;
		// Calculo el nuevo espacio total para los nodos visibles
		_CalcularTotalEspacioVisible();
		// Calculo las barras de scroll
		_CalcularScrolls();

		Evento_Nodo_Expandido(nNodo, nExpandir);

		MostrarNodo(nNodo);

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
				rRecta.left		= static_cast<LONG>((Tmp->_Ancestros - 1) * DARBOLEX_TAMICONO);
				rRecta.right    = rRecta.left + DARBOLEX_PADDING + DARBOLEX_TAMEXPANSOR + DARBOLEX_PADDING + DARBOLEX_TAMICONO + 2 + DARBOLEX_PADDING + Tmp->_AnchoTexto + DARBOLEX_PADDING + 2,
				rRecta.bottom	= rRecta.top + Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2);
				return TRUE;
			}
			rRecta.top += Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2);
			Tmp = BuscarNodoSiguiente(Tmp, TRUE);
		}
		return FALSE;
	}


	void DArbolEx::MostrarNodo(DArbolEx_Nodo *vNodo) {
		if (vNodo == NULL) return;

		BOOL nRecalcular = FALSE;
		// En primer lugar aseguro que todos los ancestros del nodo están expandidos
		DArbolEx_Nodo *Tmp = vNodo;
		while (Tmp != NULL) {
			if (Tmp->_Padre != NULL) {
				if (Tmp->_Padre->Expandido == FALSE) nRecalcular = TRUE; // Si no estaba expandido, marco que hay que recalcular 
				Tmp->_Padre->Expandido = TRUE;
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

		if (RNodo.left < RAV.left) {			// Hay una parte a la izquierda del nodo que no es visible (lateral)
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
			AltoNodo = (Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2));
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

			Tmp = BuscarNodoSiguiente(Tmp, TRUE);
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
		while (nNodo != NULL) { //                                 Tamaño ancestros   + Padding          + Expansor             + Padding          + Icono             + Padding extra + Padding          + Ancho texto        + Padding           + 2 pixels de separación                      
			TmpAncho = (static_cast<int>((nNodo->_Ancestros - 1) * DARBOLEX_TAMICONO) + DARBOLEX_PADDING + DARBOLEX_TAMEXPANSOR + DARBOLEX_PADDING + DARBOLEX_TAMICONO + 2             + DARBOLEX_PADDING + nNodo->_AnchoTexto + DARBOLEX_PADDING) + 2;
			if (_TotalAnchoVisible < TmpAncho) _TotalAnchoVisible = TmpAncho;

			TmpAlto = (nNodo->_Fuente->Alto() + (DARBOLEX_PADDING * 2));
			if (_MaxAltoNodo < TmpAlto) _MaxAltoNodo = TmpAlto;

			_TotalAltoVisible += TmpAlto;
			nNodo = BuscarNodoSiguiente(nNodo, TRUE);
		}
		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"ArbolEx::_CalcularTotalEspacioVisible an:%dpx / al:%dpx\n", _TotalAnchoVisible, _TotalAltoVisible);
		#endif
	}


	void DArbolEx::_CalcularScrolls(void) {
		// Desactivo la comprobación para recalcular los valores
		_Repintar = FALSE;

		RECT RC;
		GetClientRect(hWnd(), &RC);
		_CalcularTotalEspacioVisible();

		BOOL SV = FALSE, SH = FALSE;
		// Determino si se necesita scroll horizontal
		if (RC.right > static_cast<LONG>(_TotalAnchoVisible)) { SH = FALSE; }
		else { SH = TRUE;	RC.bottom -= _ScrollH_Alto; }
		// Determino si se necesita scroll vertical
		if (RC.bottom > static_cast<LONG>(_TotalAltoVisible)) { SV = FALSE; }
		else { SV = TRUE;	RC.right -= _ScrollV_Ancho; }

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
		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"ArbolEx::_CalcularScrolls %dpx / %dpx %.02f%%\n", _TotalAltoVisible, RC.bottom, _ScrollV_Pagina);
		#endif	
	}

	void DArbolEx::_CrearBufferNodo(const int nAncho, const int nAlto) {
		if (_BufferNodo != NULL) {
			SelectObject(_BufferNodo, _BufferNodoBmpViejo);
			DeleteObject(_BufferNodoBmp);
			DeleteDC(_BufferNodo);
		}
		HDC hDC = GetDC(NULL);
		_BufferNodo			= CreateCompatibleDC(NULL);
		_BufferNodoBmp		= CreateCompatibleBitmap(hDC, nAncho, nAlto);
		_BufferNodoBmpViejo = static_cast<HBITMAP>(SelectObject(_BufferNodo, _BufferNodoBmp));
		ReleaseDC(NULL, hDC);
		SetBkMode(_BufferNodo, TRANSPARENT);
	}

	void DArbolEx::SeleccionarNodo(DArbolEx_Nodo *sNodo, const BOOL nSeleccionado) {
		if (sNodo == NULL) return;
		if (sNodo->Activado == FALSE) return;

		sNodo->Seleccionado = nSeleccionado;
		if (SubSeleccion == TRUE && sNodo->_Hijos.size() > 0) {
			DArbolEx_Nodo *Tmp = sNodo->_Hijos[0];
			while (Tmp != NULL) {
				if (Tmp->Activado == TRUE)	Tmp->_SubSeleccionado = nSeleccionado;
				Tmp = BuscarNodoSiguiente(Tmp, FALSE, sNodo);
			}
		}
	}

	void DArbolEx::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {

		BOOL bME = _MouseEntrando();

		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)));
		int cX		= DatosMouse.X(),
			cY		= DatosMouse.Y();


		if (Scrolls_MouseMovimiento(DatosMouse) == TRUE) { return; } // las coordenadas pertenecen al scroll (salgo del evento)
		_NodoResaltado = HitTest(cX, cY, _NodoResaltadoParte);

		// Eventos virtuales
		Evento_MouseMovimiento(DatosMouse);
		SendMessage(GetParent(hWnd()), DWL_ARBOLEX_MOUSEPRESIONADO, reinterpret_cast<WPARAM>(&DatosMouse), 0);
		if (bME == TRUE)	Evento_MouseEntrando();

		// Comprueba si el nodo resaltado es el mismo que la ultima vez, y si no lo és repinta el control
		if (_NodoUResaltado != _NodoResaltado || _NodoResaltadoParte != _NodoUResaltadoParte) {
			_NodoUResaltado = _NodoResaltado;
			_NodoUResaltadoParte = _NodoResaltadoParte;
			Repintar(); // TODO : substituir per un RepintarNodo(nNodo, nUNodo)
		}


	}

	void DArbolEx::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {
		
		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)), Boton);
		int cX		= DatosMouse.X(),
			cY		= DatosMouse.Y();

		SetFocus(hWnd());
		if (Scrolls_MousePresionado(DatosMouse) == TRUE) { return; }
		
		_NodoMarcado = HitTest(cX, cY, _NodoResaltadoParte);
		if (_NodoMarcado != NULL) {
			if (_NodoMarcado->Activado == TRUE) {
				_NodoPresionadoParte	= _NodoResaltadoParte;
				_NodoPresionado			= _NodoMarcado;
			}
		}
		if (MultiSeleccion == TRUE && (DhWnd::_Teclado[VK_CONTROL] == true || DhWnd::_Teclado[VK_SHIFT] == true)) {
			if (DhWnd::_Teclado[VK_CONTROL] == true) {
				if (_NodoPresionado != NULL) SeleccionarNodo(_NodoPresionado, !_NodoPresionado->Seleccionado);
			}
			else if (DhWnd::_Teclado[VK_SHIFT] == true) {
				if (_NodoPresionado != NULL && _NodoShift != NULL) {
					// Determino el orden (Shift, Pres) o (Pres, Shift)
					bool ShiftPrimero = false;
					DArbolEx_Nodo *Tmp = _NodoPaginaInicio, *nFin = BuscarNodoSiguiente(_NodoPaginaFin, TRUE);
					while (Tmp != nFin) {
						if (_NodoShift == Tmp) {
							ShiftPrimero = true;
							break;
						}
						if (_NodoMarcado == Tmp) {
							ShiftPrimero = false;
							break;
						}
						Tmp = BuscarNodoSiguiente(Tmp, TRUE);
					}
					DesSeleccionarTodo();
					// Ahora que tenemos el orden seleccionamos los nodos
					if (ShiftPrimero == false) {
						Tmp = _NodoMarcado;
//						nFin = BuscarNodoSiguiente(_NodoShift, TRUE);
						while (Tmp != _NodoShift) {
							SeleccionarNodo(Tmp, TRUE);							
							Tmp = BuscarNodoSiguiente(Tmp, TRUE);
						}
					}
					else {
						Tmp = _NodoShift;
//						nFin = BuscarNodoSiguiente(_NodoMarcado, TRUE);
						while (Tmp != _NodoMarcado) {
							SeleccionarNodo(Tmp, TRUE);
							Tmp = BuscarNodoSiguiente(Tmp, TRUE);
						}
					}

				}
			}
		}
		else {
			DesSeleccionarTodo();
			SeleccionarNodo(_NodoPresionado, TRUE);
		}
		
		if (_NodoMarcado != NULL) SeleccionarNodo(_NodoMarcado, TRUE);
		if (_NodoShift != NULL)   SeleccionarNodo(_NodoShift, TRUE);

		Evento_MousePresionado(DatosMouse);
		Repintar();

		// Envio el evento mousedown a la ventana padre
		
//		DEventoMouse ParamMouse(cX, cY, GetWindowLongPtr(_hWnd, GWL_ID), Boton);
		SendMessage(GetParent(hWnd()), DWL_ARBOLEX_MOUSEPRESIONADO, reinterpret_cast<WPARAM>(&DatosMouse), 0);

	}

	void DArbolEx::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)), Boton);
		int cX		= DatosMouse.X(),
			cY		= DatosMouse.Y();

		if (Scrolls_MouseSoltado(DatosMouse) == TRUE) { return; }

		DArbolEx_ParteNodo nParte;
		DArbolEx_Nodo *NodoSoltado = HitTest(cX, cY, nParte);

		if (NodoSoltado != NULL) {
			// expansor
			if (NodoSoltado == _NodoPresionado && nParte == DArbolEx_ParteNodo_Expansor) {
				Expandir(_NodoPresionado, !_NodoPresionado->Expandido);
			}
			// el resto del nodo
			else if (NodoSoltado == _NodoPresionado && static_cast<int>(nParte) > static_cast<int>(DArbolEx_ParteNodo_Expansor)) {				
				//NodoSoltado->_Seleccionado = TRUE; 
			}
		}

		// Llamo al evento virtual
		Evento_MouseSoltado(DatosMouse);
		// Envio el evento mouseup a la ventana padre
		SendMessage(GetParent(hWnd()), DWL_ARBOLEX_MOUSESOLTADO, reinterpret_cast<WPARAM>(&DatosMouse), 0);
		// reasigno el estado presionado y Repinto
		_NodoPresionadoParte = DArbolEx_ParteNodo_Nada;
		_NodoPresionado = NULL;
		Repintar();

	}

	void DArbolEx::_Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey) {		
		RECT RW;
		GetWindowRect(hWnd(), &RW);

		if (Delta > 0) { // hacia arriba
			_ScrollV_Posicion -= _ScrollV_Pagina / 10.0f;
			if (_ScrollV_Posicion < 0.0f) _ScrollV_Posicion = 0.0f;
		}
		else { // hacia abajo
			_ScrollV_Posicion += _ScrollV_Pagina / 10.0f;
			if (_ScrollV_Posicion + _ScrollV_Pagina > 100.0f) _ScrollV_Posicion = 100.0f - _ScrollV_Pagina;
		}
		
		_CalcularScrolls();
		RECT RC, RCC;
		ObtenerRectaCliente(&RC, &RCC);		
		_CalcularNodosPagina(RCC.bottom);

		// Las coordenadas X e Y son relativas a la pantalla...
		LONG ncX = RW.left - cX;
		LONG ncY = RW.top - cY;
		_NodoResaltado	 = HitTest(ncX, ncY, _NodoResaltadoParte);
		_NodoUResaltado	 = _NodoResaltado;

		Evento_MouseRueda(Delta, ncX, ncY, VirtKey);
		Repintar();

		// Envio el evento click a la ventana padre
//		DEventoMouse ParamMouse(cX, cY, GetWindowLongPtr(_hWnd, GWL_ID), Boton);
//		SendMessage(GetParent(hWnd()), DWL_ARBOLEX_MOUSESOLTADO, reinterpret_cast<WPARAM>(&ParamMouse), 0);

	}

	/* Función que elimina la selección y la sub-selección de todos los nodos 
	     Requiere Repintar(); después de utilizar esta función		 */
	void DArbolEx::DesSeleccionarTodo(void) {
		// Si no hay nodos salgo
		if (_Raiz._Hijos.size() == 0) return;
		DArbolEx_Nodo *dNodo = _Raiz._Hijos[0];

		while (dNodo != NULL) {
			dNodo->Seleccionado = FALSE;
			dNodo->_SubSeleccionado = FALSE;
			dNodo = BuscarNodoSiguiente(dNodo, FALSE);
		}
	}



	void DArbolEx::_AplicarShift(const LONG nPosShift) {
		
		DArbolEx_Nodo *nTmp = NULL;
		LONG Sum = 0;
		// Des-selecciono los nodos que quedan dentro del shift (antes de presionar el cursor)
/*		if (_PosShift != 0) {
			Sum  = (_PosShift > 0) ? -1 : 1;
			nTmp = _NodoShift;

			while (nTmp != NULL) {
				nTmp->_Seleccionado = FALSE;
				if (Sum == -1)
					nTmp = BuscarNodoSiguiente(nTmp, TRUE);
				else
					nTmp = BuscarNodoAnterior(nTmp, TRUE);
				if (nTmp == _NodoMarcado) break;
			}
		}*/
		DesSeleccionarTodo();
		
		_PosShift = nPosShift;
		// Selecciono los nodos que quedan dentro del shift (una vez sumado el desplazamiento del shift)
		if (_PosShift != 0) {
			LONG Sum = (_PosShift > 0) ? -1 : 1;
			nTmp = _NodoShift;
			while (nTmp != NULL) {
				SeleccionarNodo(nTmp, TRUE);
				if (Sum == -1)	
					nTmp = BuscarNodoSiguiente(nTmp, TRUE);
				else
					nTmp = BuscarNodoAnterior(nTmp, TRUE);
				if (nTmp == _NodoMarcado) break;
			}
		}
		#if DARBOLEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"ArbolEx::_AplicarShift %d\n", _PosShift);
		#endif
	}

	void DArbolEx::_Tecla_CursorAbajo(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return;

		// Si no hay ningun nodo marcado, marcamos el primero del arbol
		if (_NodoMarcado == NULL) {
			_NodoMarcado = _NodoPaginaInicio;
			SeleccionarNodo(_NodoMarcado, TRUE);
			MostrarNodo(_NodoMarcado);
			return;
		}

		DArbolEx_Nodo *nTmp = BuscarNodoSiguiente(_NodoMarcado, TRUE); 
		if (nTmp != NULL) _NodoMarcado = nTmp;
		else              return;

		if (DhWnd::_Teclado[VK_SHIFT] == TRUE && MultiSeleccion == TRUE) {
			_AplicarShift(_PosShift + 1);
		}
		else {
			DesSeleccionarTodo();
		}

		if (nTmp != NULL) {			
			SeleccionarNodo(_NodoMarcado, TRUE);
			MostrarNodo(nTmp);
		}		
	}

	void DArbolEx::_Tecla_CursorArriba(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return;

		// Si no hay ningun nodo marcado, marcamos el primero del arbol
		if (_NodoMarcado == NULL) {
			_NodoMarcado = _NodoPaginaInicio;
			SeleccionarNodo(_NodoMarcado, TRUE);
			MostrarNodo(_NodoMarcado);
			return;
		}		 

		DArbolEx_Nodo *nTmp = BuscarNodoAnterior(_NodoMarcado, TRUE);
		if (nTmp != NULL) _NodoMarcado = nTmp;
		else              return;

		if (DhWnd::_Teclado[VK_SHIFT] == TRUE && MultiSeleccion == TRUE) {
			_AplicarShift(_PosShift - 1);
		}
		else {
			DesSeleccionarTodo();
		}

		if (nTmp != NULL) {
			SeleccionarNodo(_NodoMarcado, TRUE);
			MostrarNodo(nTmp);
		}


	}

	void DArbolEx::_Tecla_Inicio(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return;

		if (DhWnd::_Teclado[VK_SHIFT] == TRUE && MultiSeleccion == TRUE) {
/*			DArbolEx_Nodo *Tmp = _NodoShift;
			LONG nPosShift = 0;
			while (Tmp != NULL) {
				Tmp = BuscarNodoAnterior(Tmp, TRUE);
				nPosShift--;
			}
			_AplicarShift(_PosShift + nPosShift);*/

		}
		else {
			DesSeleccionarTodo();
		}
		_NodoMarcado = _Raiz._Hijos[0];
		SeleccionarNodo(_NodoMarcado, TRUE);
		MostrarNodo(_NodoMarcado);
	}

	void DArbolEx::_Tecla_Fin(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return;

		if (DhWnd::_Teclado[VK_SHIFT] == TRUE && MultiSeleccion == TRUE) {
/*			DArbolEx_Nodo *Tmp = _NodoShift;
			LONG nPosShift = 0;
			while (Tmp != NULL) {
				Tmp = BuscarNodoSiguiente(Tmp, TRUE);
				nPosShift++;
			}
			_AplicarShift(_PosShift + nPosShift);*/
		}
		else {
			DesSeleccionarTodo();
		}
		_NodoMarcado = UltimoNodoVisible();
		SeleccionarNodo(_NodoMarcado, TRUE);
		MostrarNodo(_NodoMarcado);
	}

	void DArbolEx::_Tecla_AvPag(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return;

		RECT RC, RA;
		ObtenerRectaCliente(&RC, &RA);

//		DesSeleccionarTodo();
		LONG PixelesContados = 0;
		// Si no hay nodo marcado, marco el primero que se ve en la pagina
		if (_NodoMarcado == NULL) _NodoMarcado = _NodoPaginaInicio;
		// Buscon el nodo que está 1 pagina mas atrás (OJO no tiene por que ser el _NodoPaginaFin)
		DArbolEx_Nodo *Tmp = _NodoMarcado, *UTmp = NULL;
		while (Tmp != NULL && PixelesContados < RA.bottom) {
			if (Tmp != NULL) {
				UTmp = Tmp;
				PixelesContados += Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2);
				if (DhWnd::_Teclado[VK_SHIFT] == TRUE && MultiSeleccion == TRUE) {
					SeleccionarNodo(Tmp, !Tmp->Seleccionado);
				}
				Tmp = BuscarNodoSiguiente(Tmp, TRUE);
			}
		}

		if (DhWnd::_Teclado[VK_SHIFT] == false || MultiSeleccion == FALSE) DesSeleccionarTodo();
		_NodoMarcado = (Tmp == NULL) ? UTmp : Tmp;
		SeleccionarNodo(_NodoMarcado, TRUE);
		if (_NodoShift != NULL) SeleccionarNodo(_NodoShift, TRUE);
		MostrarNodo(_NodoMarcado);
	}

	void DArbolEx::_Tecla_RePag(void) {
		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) return;

		RECT RC, RA;
		ObtenerRectaCliente(&RC, &RA);

//		DesSeleccionarTodo();
		LONG PixelesContados = 0;
		// Si no hay nodo marcado, marco el primero que se ve en la pagina
		if (_NodoMarcado == NULL) _NodoMarcado = _NodoPaginaInicio;
		// Buscon el nodo que está 1 pagina mas atrás (OJO no tiene por que ser el _NodoPaginaInicio)
		DArbolEx_Nodo *Tmp = _NodoMarcado, *UTmp = NULL;
		while (Tmp != NULL && PixelesContados < RA.bottom) {
			if (Tmp != NULL) {
				UTmp = Tmp;
				PixelesContados += Tmp->_Fuente->Alto() + (DARBOLEX_PADDING * 2);
				if (DhWnd::_Teclado[VK_SHIFT] == TRUE && MultiSeleccion == TRUE) {
					SeleccionarNodo(Tmp, !Tmp->Seleccionado);					
				}
				Tmp = BuscarNodoAnterior(Tmp, TRUE);
			}
		}

		if (DhWnd::_Teclado[VK_SHIFT] == false || MultiSeleccion == FALSE) DesSeleccionarTodo();
		_NodoMarcado = (Tmp == NULL) ? UTmp : Tmp;
		SeleccionarNodo(_NodoMarcado, TRUE);
		if (_NodoShift != NULL) 		SeleccionarNodo(_NodoShift, TRUE);
		MostrarNodo(_NodoMarcado);
	}

	void DArbolEx::_Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		// Marco la tecla como presionada
		DhWnd::_Teclado[Caracter] = true;

		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) {
			Evento_TeclaPresionada(Caracter, Repeticion, Params);
			return;
		}
		// Me guardo el nodo marcado para indicar desde donde empieza el shift
		if (Caracter == VK_SHIFT) {
			if (_NodoShift == NULL) {
				_NodoShift = (_NodoMarcado == NULL) ? _NodoPaginaInicio : _NodoMarcado;
			}
		}

		switch (Caracter) {
			case VK_HOME		: _Tecla_Inicio();								break;
			case VK_END			: _Tecla_Fin();									break;
			case VK_UP			: _Tecla_CursorArriba();						break;
			case VK_DOWN		: _Tecla_CursorAbajo();							break;
			case VK_LEFT		: Expandir(_NodoMarcado, FALSE);				break;
			case VK_RIGHT		: Expandir(_NodoMarcado, TRUE);					break;
			case VK_ADD			: Expandir(_NodoMarcado, TRUE);                 break; // +
			case VK_SUBTRACT	: Expandir(_NodoMarcado, FALSE);                break; // -
			case VK_PRIOR		: _Tecla_RePag();								break; // RePag
			case VK_NEXT		: _Tecla_AvPag();								break; // AvPag
			default				: Evento_Tecla(Caracter, Repeticion, Params);	break; // if (Caracter >= 0x30 && Caracter <= 0x5A) // Cualquier tecla valida
		}

		Evento_TeclaPresionada(Caracter, Repeticion, Params);
	}

	void DArbolEx::_Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
		// Marco la tecla como no presionada
		DhWnd::_Teclado[Caracter] = false;

		// Si no hay nodos, salgo de la función
		if (_Raiz._Hijos.size() == 0) {
			Evento_TeclaSoltada(Caracter, Repeticion, Params);
			return;
		}

		if (Caracter == VK_SHIFT) {
			_NodoShift = NULL;
			_PosShift = 0;
		}

		Evento_TeclaSoltada(Caracter, Repeticion, Params);
	}

	/* TODO */
	void DArbolEx::_Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param) {
		if (Caracter >= 0x30 && Caracter <= 0x5A) {

		}

		Evento_Tecla(Caracter, Repeticion, Param);
	}

	void DArbolEx::_Evento_Pintar(void) {
		HDC         DC;
		PAINTSTRUCT PS;
		DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}

	void DArbolEx::_Evento_MouseSaliendo(void) {
		BOOL nRepintar = Scrolls_MouseSaliendo();
		_MouseDentro = FALSE;
		_NodoResaltado = NULL;
		Evento_MouseSaliendo();
		if (nRepintar == TRUE) Repintar();
	}

	void DArbolEx::_Evento_MouseDobleClick(const int Boton, WPARAM wParam, LPARAM lParam) {
		if (_NodoMarcado != NULL) {
			Expandir(_NodoMarcado, !_NodoMarcado->Expandido);
			SeleccionarNodo(_NodoMarcado, TRUE);
		}
		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)), Boton);
		Evento_MouseDobleClick(DatosMouse);
	}

	void DArbolEx::_Evento_FocoObtenido(HWND hWndUltimoFoco) {
		BorrarBufferTeclado();
		Evento_FocoObtenido(hWndUltimoFoco);
	}

	void DArbolEx::_Evento_FocoPerdido(HWND hWndNuevoFoco) {
		BorrarBufferTeclado();
		Evento_FocoPerdido(hWndNuevoFoco);
	}

	LRESULT CALLBACK DArbolEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_SETFOCUS:		_Evento_FocoObtenido((HWND)wParam);																											return 0;
			case WM_KILLFOCUS:		_Evento_FocoPerdido((HWND)wParam);																											return 0;

			case WM_KEYDOWN:		_Evento_TeclaPresionada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
			case WM_KEYUP:			_Evento_TeclaSoltada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;		
			case WM_CHAR:           _Evento_Tecla(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));																	return 0;

			case WM_SIZE:			_CalcularScrolls();		Repintar();																											return 0;

			case WM_PAINT:			_Evento_Pintar();																															return 0;

			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(wParam, lParam);																									return 0;
//			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;

			case WM_MOUSELEAVE:		_Evento_MouseSaliendo();																													return 0;

			case WM_MOUSEWHEEL:		_Evento_MouseRueda(static_cast<short>(HIWORD(wParam)), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(LOWORD(wParam)));		return 0;
			// Mouse presionado
			case WM_LBUTTONDOWN:	_Evento_MousePresionado(0, wParam, lParam);																									return 0;
			case WM_RBUTTONDOWN:	_Evento_MousePresionado(1, wParam, lParam);																									return 0;
			case WM_MBUTTONDOWN:	_Evento_MousePresionado(2, wParam, lParam);																									return 0;
			// Mouse soltado
			case WM_LBUTTONUP:		_Evento_MouseSoltado(0, wParam, lParam);																									return 0;
			case WM_RBUTTONUP:		_Evento_MouseSoltado(1, wParam, lParam);																									return 0;
			case WM_MBUTTONUP:		_Evento_MouseSoltado(2, wParam, lParam);																									return 0;
			// Mouse doble click
			case WM_LBUTTONDBLCLK:	_Evento_MouseDobleClick(0, wParam, lParam);																									return 0;
			case WM_RBUTTONDBLCLK:	_Evento_MouseDobleClick(1, wParam, lParam);																									return 0;
			case WM_MBUTTONDBLCLK:	_Evento_MouseDobleClick(2, wParam, lParam);																									return 0;

		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	};

}