//#pragma once
#include "stdafx.h"
#include "DListaIconos.h"
//#include "DArbolEx.h"

#define ARBOLEX_PADDING		 2 // Espacio entre el marco de la selección y el texto (tambien se utiliza para espaciar el expansor, el icono y el texto horizontalmente)
#define ARBOLEX_TAMEXPANSOR  9 // OJO!! tiene que ser impar para quedar bien
#define ARBOLEX_TAMICONO	16 // Tamaño del icono

namespace DWL {

	DArbolEx::DArbolEx(void) :	DBarraScrollEx(),
								_NodoMarcado(NULL), _NodoPresionado(NULL), _NodoResaltado(NULL), _NodoUResaltado(NULL), _NodoResaltadoParte(DArbolEx_ParteNodo_Nada), _NodoUResaltadoParte(DArbolEx_ParteNodo_Nada),
								_Fuente(NULL), _TotalAnchoVisible(0), _TotalAltoVisible(0), 
								_NodoPaginaInicio(NULL), _NodoPaginaFin(NULL), _NodoPaginaVDif(0), _NodoPaginaHDif(0),
								_BufferNodo(NULL), _BufferNodoBmp(NULL), _BufferNodoBmpViejo(NULL) {
		_Raiz._Expandido		= TRUE;		 
		_Raiz._Arbol			= this;
		_ColorFondoScroll		= COLOR_ARBOL_FONDO_SCROLL;
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

	void DArbolEx::Expandir(DArbolEx_Nodo *nNodo, const BOOL nExpandir) {
		nNodo->_Expandido = nExpandir;
		_CalcularTotalEspacioVisible();
		Repintar();
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

		size_t nPos = 0;
		switch (PosicionNodo) {
			// Al final de la lista
			case DARBOLEX_POSICIONNODO_FIN :
				// Asigno el nodo sigüiente al último nodo del padre (si es que existe algún nodo)
				if (nPadre->TotalHijos() > 0)	{	
					nPadre->Hijo(nPadre->TotalHijos() - 1)->_Siguiente = nNodo;		
				}	
				nPadre->_Hijos.push_back(nNodo);
				break;

			// Por orden alfabético
			case DARBOLEX_POSICIONNODO_ORDENADO :
				// Busco la posición que deberia tener ordenado alfabeticamente
				for (nPos = 0; nPos < nPadre->TotalHijos(); nPos++) {
					if (_wcsicmp(nTexto, nPadre->Hijo(nPos)->Texto().c_str()) < 0) break;
				}
				// Enlazo el nodo siguiente
				if (nPos > 0)									{ nPadre->Hijo(nPos - 1)->_Siguiente = nNodo; }
				if (nPos + 1 > nPadre->TotalHijos())			{ nNodo->_Siguiente = nPadre->Hijo(nPos + 1); }
				// Agrego el nodo 
				if (nPadre->_Hijos.size() == 0)					{ nPadre->_Hijos.push_back(nNodo); }
				else											{ nPadre->_Hijos.insert(nPadre->_Hijos.begin() + nPos, nNodo); }
				break;

			// el default incluye DARBOLEX_POSICIONNODO_INICIO que es 0
			default :
				// Enlazo el nodo siguiente
				if (PosicionNodo > 0)							{ nPadre->Hijo(PosicionNodo - 1)->_Siguiente = nNodo; }
				if (PosicionNodo + 1 > nPadre->TotalHijos())	{ nNodo->_Siguiente = nPadre->Hijo(PosicionNodo + 1); }
				// Agrego el nodo 
				if (nPadre->_Hijos.size() == 0)					{ nPadre->_Hijos.push_back(nNodo); }
				else											{ nPadre->_Hijos.insert(nPadre->_Hijos.begin() + PosicionNodo, nNodo); }
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

		HDC		Buffer		= CreateCompatibleDC(hDC);
		HBITMAP Bmp			= CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
		HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));		



//		LONG DifInicio = 0; // altura total pintada
//		_CalcularNodosPagina(RCS.bottom);
		DArbolEx_Nodo *nActual = _NodoPaginaInicio;

		LONG PixelInicio	= static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion);
		LONG PixelFin		= PixelInicio + RCS.bottom;
		LONG nAlto			= 0;
		int  DifInicioV		= _NodoPaginaVDif;
		int  DifInicioH		= _NodoPaginaHDif;

		RECT RectaItem; // = { 0, DifInicio, RCS.right, DifInicio + nActual->_Fuente->Alto() };
		while (nActual != _NodoPaginaFin && nActual != NULL) {
			nAlto = nActual->_Fuente->Alto() + (ARBOLEX_PADDING * 2);
			RectaItem = { 0, DifInicioV, RCS.right, DifInicioV + nAlto };
			PintarNodo(Buffer, &RectaItem, nActual, -_NodoPaginaHDif);
			DifInicioV += nAlto;
			nActual = _BuscarSiguienteNodoVisible(nActual);			
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
		if		(nNodo == _NodoResaltado)	
			Estado = 1;
		else if (nNodo == _NodoPresionado)	
			Estado = 2;
		COLORREF ColFondo = NULL, ColTexto = NULL;		
		switch (Estado) {
			case 0 : // Normal
				ColTexto = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_TEXTO : COLOR_ARBOL_SELECCION_TEXTO;
				ColFondo = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_FONDO : COLOR_ARBOL_SELECCION;
				break;
			case 1 : // Resaltado
				ColTexto = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_TEXTO_RESALTADO : COLOR_ARBOL_SELECCION_TEXTO_RESALTADO;
				ColFondo = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_FONDO_RESALTADO : COLOR_ARBOL_SELECCION_RESALTADO; // RGB(0, 0, 255) : RGB(0, 255, 0);
				break;
			case 2 : // Presionado
				ColTexto = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_TEXTO_PRESIONADO : COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO;
				ColFondo = (nNodo->_Seleccionado == FALSE) ? COLOR_ARBOL_FONDO_PRESIONADO : COLOR_ARBOL_SELECCION_PRESIONADO;
				break;
		}

		// Pinto el fondo del buffer
		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_ARBOL_FONDO);
		RECT EspacioLocal = { 0 , 0, (_TotalAnchoVisible > Espacio->right) ? static_cast<LONG>(_TotalAnchoVisible) : Espacio->right, (2 * ARBOLEX_PADDING) + nNodo->_Fuente->Alto() };
		FillRect(_BufferNodo, &EspacioLocal, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Determino si hay que mostrar el expansor
		BOOL MostrarExpansor = nNodo->MostrarExpansor();
/*		switch (nNodo->_MostrarExpansor) {
			case DArbolEx_MostrarExpansor_Auto		: MostrarExpansor = (nNodo->_Hijos.size() > 0) ? TRUE : FALSE;		break;
			case DArbolEx_MostrarExpansor_Mostrar	: MostrarExpansor = TRUE;											break;
			case DArbolEx_MostrarExpansor_Ocultar	: MostrarExpansor = FALSE;											break;
		}*/

		int		AnchoOcupado		= static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING;  // ARBOLEX_PADDING + (static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO));
		HBRUSH	BrochaBordeExpansor	= CreateSolidBrush(COLOR_ARBOL_EXPANSOR_BORDE);
		HPEN	PlumaExpansor		= CreatePen(PS_SOLID, 1, ColTexto);
		if (MostrarExpansor == TRUE) {
			int PosExpansorY = ((Espacio->bottom - Espacio->top) - ARBOLEX_TAMEXPANSOR) / 2;
			RECT MarcoExpansor = { AnchoOcupado, PosExpansorY, AnchoOcupado + ARBOLEX_TAMEXPANSOR, PosExpansorY + ARBOLEX_TAMEXPANSOR };
//			Debug_Escribir_Varg(L"ArbolEx::PintarNodo left:%d top:%d right:%d bottom:%d\n", MarcoExpansor.left, MarcoExpansor.top, MarcoExpansor.right, MarcoExpansor.bottom);
			FrameRect(_BufferNodo, &MarcoExpansor, BrochaBordeExpansor);
			// Pinto una linea horizontal para el expansor
			MoveToEx(_BufferNodo, AnchoOcupado + 2, PosExpansorY + ARBOLEX_TAMEXPANSOR / 2, NULL);
			LineTo(_BufferNodo, AnchoOcupado + ARBOLEX_TAMEXPANSOR - 2, PosExpansorY + ARBOLEX_TAMEXPANSOR / 2);
			if (nNodo->_Expandido == FALSE) {
				// Pinto una linea vertical para el expansor
				MoveToEx(_BufferNodo, AnchoOcupado + ARBOLEX_PADDING + (ARBOLEX_TAMEXPANSOR / 2), PosExpansorY + 2, NULL);
				LineTo(_BufferNodo, AnchoOcupado + ARBOLEX_PADDING + (ARBOLEX_TAMEXPANSOR / 2), PosExpansorY + ARBOLEX_TAMEXPANSOR -2);
			}
		}
		AnchoOcupado += ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR;
		DeleteObject(BrochaBordeExpansor);
		DeleteObject(PlumaExpansor);

		// Pinto el icono
		int PosIcoY = ((Espacio->bottom - Espacio->top) - ARBOLEX_TAMICONO) / 2;
		int PosIcoX = AnchoOcupado + ARBOLEX_PADDING;
		DrawIconEx(_BufferNodo, PosIcoX, PosIcoY, nNodo->_Icono->Icono(), ARBOLEX_TAMICONO, ARBOLEX_TAMICONO, 0, 0, DI_NORMAL);
		AnchoOcupado += ARBOLEX_TAMICONO + ARBOLEX_PADDING + 2;

		// Pinto el fondo del texto del nodo (tanto si está seleccionado como si no)
		RECT RFondo = { (AnchoOcupado - ARBOLEX_PADDING), 2, ((AnchoOcupado + nNodo->_AnchoTexto) + ARBOLEX_PADDING), ((ARBOLEX_PADDING * 2) + nNodo->_Fuente->Alto()) -1 };
		HBRUSH BrochaFondoTexto = CreateSolidBrush(ColFondo);
		FillRect(_BufferNodo, &RFondo, BrochaFondoTexto);
		DeleteObject(BrochaFondoTexto);

		// Pinto el texto del nodo
		SetTextColor(_BufferNodo, ColTexto);
		HFONT FuenteVieja = static_cast<HFONT>(SelectObject(_BufferNodo, nNodo->_Fuente->Fuente()));
		RECT RTexto = { AnchoOcupado, ARBOLEX_PADDING, AnchoOcupado + nNodo->_AnchoTexto, ARBOLEX_PADDING + nNodo->_Fuente->Alto() };
		DrawText(_BufferNodo, nNodo->_Texto.c_str(), static_cast<int>(nNodo->_Texto.size()), &RTexto, DT_LEFT | DT_NOPREFIX);
		SelectObject(_BufferNodo, FuenteVieja);


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
			Tmp = _BuscarSiguienteNodoVisible(Tmp);
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
				iniciotodoX			= static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + _NodoPaginaHDif;
				tamtodoX			= ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2 + ARBOLEX_PADDING + Tmp->_AnchoTexto + ARBOLEX_PADDING;
				RECT RectaExpansor	= {	inicioexpansorX,							PixelesContados + inicioexpansorY,
										inicioexpansorX + ARBOLEX_TAMEXPANSOR,		PixelesContados + inicioexpansorY + ARBOLEX_TAMEXPANSOR };
				Debug_Escribir_Varg(L"ArbolEx::_HitTestEventoMouse left:%d top:%d right:%d bottom:%d\n", RectaExpansor.left, RectaExpansor.top, RectaExpansor.right, RectaExpansor.bottom);
				if (PtInRect(&RectaExpansor, Pt) != 0) {
					Parte = DArbolEx_ParteNodo_Expansor;
					return Tmp;
				}
			}
			else {
				iniciotodoX = static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING +  _NodoPaginaHDif;
				tamtodoX	= ARBOLEX_TAMICONO + 2 + ARBOLEX_PADDING + Tmp->_AnchoTexto + ARBOLEX_PADDING;
			}
			// Compruebo si el mouse está en el icono del nodo
			int inicioiconoX = static_cast<int>((Tmp->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + _NodoPaginaHDif;
			int inicioiconoY = ((Tmp->_Fuente->Alto() + (ARBOLEX_PADDING * 2)) - ARBOLEX_TAMICONO) / 2;
			RECT RectaIcono	 = { inicioiconoX,										PixelesContados + inicioiconoY,
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
			RECT RectaNodo	 = { iniciotodoX,										PixelesContados + ARBOLEX_PADDING,
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

	DArbolEx_Nodo *DArbolEx::_BuscarSiguienteNodo(DArbolEx_Nodo *nActual) {
		// El nodo actual tiene hijos
		if (nActual->TotalHijos() > 0) {
			return nActual->_Hijos[0];
		}
		// El nodo actual no tiene hijos visibles
		else {
			DArbolEx_Nodo *Tmp = nActual;
			while (Tmp->_Siguiente == NULL) {
				if (Tmp->_Padre == NULL) { // Es el ultimo nodo del arbol
					return NULL;
				}
				Tmp = Tmp->_Padre;
			}
			return Tmp->_Siguiente;
		}
	}

	DArbolEx_Nodo *DArbolEx::_BuscarSiguienteNodoVisible(DArbolEx_Nodo *nActual) {
		// El nodo actual tiene hijos visibles
		if (nActual->TotalHijos() > 0 && nActual->_Expandido == TRUE) {
			return nActual->_Hijos[0];
		}
		// El nodo actual no tiene hijos visibles
		else {
			DArbolEx_Nodo *Tmp = nActual;
			while (Tmp->_Siguiente == NULL) {
				if (Tmp->_Padre == NULL) { // Es el ultimo nodo del arbol
					return NULL;
				}
				Tmp = Tmp->_Padre;
			}
			return Tmp->_Siguiente;
		}
	}

	void DArbolEx::Scrolls_EventoCambioPosicion(void) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		_CalcularNodosPagina(RC.bottom);
	}

	/* Busca el primer nodo visible, y el ultimo nodo visible (aunque solo sean una parte de ellos) */	
	void DArbolEx::_CalcularNodosPagina(const size_t TamPagina) {
		_NodoPaginaInicio = NULL;
		_NodoPaginaFin = NULL;
		if (_Raiz.TotalHijos() == 0) {	return;	}
		LONG   PixelInicio			= static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion); // Calculo de forma temporal los pixeles que hay que contar hasta empezar a pintar
		LONG   PixelFin				= PixelInicio + static_cast<LONG>(TamPagina);
		LONG   TotalPixelesContados = -static_cast<LONG>(PixelInicio);
		LONG   AltoNodo				= 0;
		PixelInicio = 0; // El pixel inicial se cuenta desde 0
		
		_NodoPaginaHDif = - static_cast<LONG>((static_cast<float>(_TotalAnchoVisible) / 100.0f) * _ScrollH_Posicion);

		//_NodosPagina(nInicio, nFin, &_Raiz, PixelInicio, PixelFin, TotalPixelesContados, DifInicio);
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
				if (TotalPixelesContados >= PixelFin) {
					_NodoPaginaFin = Tmp;
					return;
				}
			}
			TotalPixelesContados += AltoNodo;

			Tmp = _BuscarSiguienteNodoVisible(Tmp);
		}
	}


	// Obtiene la altura total que necesitan todos los nodos visibles (en pixeles)
	void DArbolEx::_CalcularTotalEspacioVisible(void) {
		_TotalAltoVisible	= 0;
		_TotalAnchoVisible	= 0;
		if (_Raiz._Hijos.size() == 0) return;

		DArbolEx_Nodo *nNodo = _Raiz._Hijos[0];
		size_t TmpAncho = 0;
		size_t TmpAlto = 0;
		while (nNodo != NULL) { //                                 Tamaño ancestros  + Padding         + Expansor            + Padding         + Icono            + Padding extra   + Padding         + Ancho texto        + Padding          + 2 pixels de separación                      
			TmpAncho = (static_cast<int>((nNodo->_Ancestros - 1) * ARBOLEX_TAMICONO) + ARBOLEX_PADDING + ARBOLEX_TAMEXPANSOR + ARBOLEX_PADDING + ARBOLEX_TAMICONO + 2				+ ARBOLEX_PADDING + nNodo->_AnchoTexto + ARBOLEX_PADDING) + 2;
			if (_TotalAnchoVisible < TmpAncho) _TotalAnchoVisible = TmpAncho;

			TmpAlto = (nNodo->_Fuente->Alto() + (ARBOLEX_PADDING * 2));
			if (_MaxAltoNodo < TmpAlto) _MaxAltoNodo = TmpAlto;

			_TotalAltoVisible += TmpAlto;
			nNodo = _BuscarSiguienteNodoVisible(nNodo);
		}
		
	}


	void DArbolEx::_CalcularScrolls(void) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		_CalcularTotalEspacioVisible();
		
		BOOL SV = FALSE, SH = FALSE;
		// Determino si se necesita scroll horizontal
		if (RC.right > _TotalAnchoVisible)	{ SH = FALSE;									}
		else								{ SH = TRUE;	RC.bottom -= TAM_BARRA_SCROLL;	}
		// Determino si se necesita scroll vertical
		if (RC.bottom > _TotalAltoVisible)	{ SV = FALSE;									}
		else								{ SV = TRUE;	RC.right -= TAM_BARRA_SCROLL;	}

		// Borro y vuelvo a crear el buffer DC para pintar los nodos
		_CrearBufferNodo((_TotalAnchoVisible > RC.right) ? static_cast<int>(_TotalAnchoVisible) : static_cast<int>(RC.right), static_cast<int>(_MaxAltoNodo));

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
		_BufferNodo			= CreateCompatibleDC(NULL);
		_BufferNodoBmp		= CreateCompatibleBitmap(GetDC(NULL), nAncho, nAlto);
		_BufferNodoBmpViejo = static_cast<HBITMAP>(SelectObject(_BufferNodo, _BufferNodoBmp));

		SetBkMode(_BufferNodo, TRANSPARENT);
	}

	void DArbolEx::Evento_MouseMovimiento(const int cX, const int cY, const UINT Param) {
		if (Scrolls_MouseMovimiento(cX, cY, Param) == TRUE) { return; } // las coordenadas pertenecen al scroll (salgo del evento)
/*	enum DArbolEx_ParteNodo {
		DArbolEx_ParteNodo_Nada		= 0,
		DArbolEx_ParteNodo_Expansor = 1,
		DArbolEx_ParteNodo_Nodo		= 2,
		DArbolEx_ParteNodo_Icono	= 3,
		DArbolEx_ParteNodo_Texto	= 4
	};*/
		_NodoResaltado = HitTest(cX, cY, _NodoResaltadoParte);

//		Debug_Escribir_Varg(L"ArbolEx::Evento_MouseMovimiento x:%d y:%d p:%d\n", cX, cY, Parte);		
		if (_NodoUResaltado != _NodoResaltado || _NodoResaltadoParte != _NodoUResaltadoParte) {
			_NodoUResaltado		 = _NodoResaltado;
			_NodoUResaltadoParte = _NodoResaltadoParte;
			Repintar(); // TODO : substituir per un RepintarNodo(nNodo, nUNodo)
		}

//		if (nNodo != NULL) { Debug_Escribir_Varg(L"DArbolEx::Evento_MouseMovimiento %s\n", nNodo->Texto().c_str()); }
		//else               { Debug_Escribir_Varg(L"DArbolEx::Evento_MouseMovimiento NULL\n"); }
	}

	LRESULT CALLBACK DArbolEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
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
					// MouseEntrando
				}
				
				Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;

			case WM_MOUSELEAVE: // Enviado gracias a WM_MOUSEMOVE -> _MouseEntrando()
				_MouseDentro = FALSE;
				if (_ScrollV_Estado != DBarraScrollEx_Estado_Presionado && _ScrollH_Estado != DBarraScrollEx_Estado_Presionado) {
					if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) _ScrollV_Estado = DBarraScrollEx_Estado_Normal;
					if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) _ScrollH_Estado = DBarraScrollEx_Estado_Normal;
					Repintar();
				}
				break;

			case WM_LBUTTONDOWN:
				Scrolls_MousePresionado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;

			case WM_LBUTTONUP:
				Scrolls_MouseSoltado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
				break;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	};

}