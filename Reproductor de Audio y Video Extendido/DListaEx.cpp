#include "stdafx.h"
#include "DListaEx.h"
#include "DListaIconos.h"
#include "DMensajesWnd.h"

namespace DWL {

	DListaEx::DListaEx(void) :	_ItemPaginaInicio(0)	, _ItemPaginaFin(0)			, _ItemPaginaVDif(0)		, _ItemPaginaHDif(0),
								_SubItemResaltado(-1)	, _SubItemUResaltado(-1)	, _SubItemPresionado(-1)	, MostrarSeleccion(TRUE),
								_ItemResaltado(-1)		, _ItemUResaltado(-1)		, _ItemMarcado(0)			,
								_ItemPresionado(-1)		, _ItemShift(0)				, _Repintar(FALSE)			,
								_TotalAnchoVisible(0)	, _TotalAltoVisible(0)		, 
								_BufferItem(NULL)		, _BufferItemBmp(NULL)		, _BufferItemBmpViejo(NULL)	, _BufferItemFuenteViejo(NULL) {

//		ColorFondoScroll = COLOR_LISTA_FONDO_SCROLL;
	}


	DListaEx::~DListaEx(void) {
		EliminarTodasLasColumnas();
		EliminarTodosLosItems();
	}

	
	HWND DListaEx::CrearListaEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
//		if (hWnd()) { Debug_Escribir(L"DListaEx::CrearListaEx() Error : ya se ha creado la lista\n"); return hWnd(); }
		Fuente = Fuente18Normal;
		_hWnd = CrearControlEx(nPadre, L"DListaEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD, NULL, CS_DBLCLKS); // CS_DBLCLKS (el control recibe notificaciones de doble click)
		_Repintar = TRUE;
		return hWnd();
	}
	
	DListaEx_Columna *DListaEx::AgregarColumna(const int nAncho, DListaEx_Columna_Alineacion nAlineacion) {
		DListaEx_Columna *nColumna = new DListaEx_Columna(nAlineacion, nAncho);
		_Columnas.push_back(nColumna);
		_Repintar = TRUE;
		return nColumna;
	}
	
	DListaEx_Item *DListaEx::_AgregarItem(DListaEx_Item *nItem, DListaIconos_Icono *nIcono, const size_t PosicionItem, const TCHAR *nTxt, va_list Marker) {
//		DListaEx_Item		*nItem		= new DListaEx_Item();
		DListaEx_SubItem	*nSubItem 	= new DListaEx_SubItem(nTxt);
		nItem->_SubItems.push_back(nSubItem);

		nItem->_Icono = nIcono;

		// Obtengo los textos de cada columna
		for (size_t i = 1; i < _Columnas.size(); i++) {
			nSubItem = new DListaEx_SubItem(static_cast<const TCHAR *>(va_arg(Marker, const TCHAR *)));
			nItem->_SubItems.push_back(nSubItem);
		}
		_Items.push_back(nItem);
		
		_Repintar = TRUE;
//		_CalcularScrolls();

		return nItem;
	}

	void DListaEx::EliminarTodosLosItems(void) {
		for (size_t i = 0; i < _Items.size(); i++) {
			delete _Items[i];
		}
		_Items.resize(0);
		_ItemResaltado	= -1;
		_ItemMarcado	= -1;
		_ItemPresionado = -1;
		_Repintar = TRUE;
	}

	void DListaEx::Repintar(const BOOL nForzar) {
		if (_Repintar == FALSE)	_Repintar = nForzar;
		if (IsWindowVisible(_hWnd) == FALSE) return;
		RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT); 
		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::Repintar\n", _Repintar);
		#endif
	};

	void DListaEx::EliminarTodasLasColumnas(void) {
		for (size_t i = 0; i < _Columnas.size(); i++) {
			delete _Columnas[i];
		}
		_Columnas.resize(0);
		_Repintar = TRUE;
	}

	void DListaEx::Pintar(HDC hDC) {
		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::Pintar -> _Repintar = %d\n", _Repintar);
		#endif
		if (_Repintar == TRUE) {			
			_CalcularScrolls();
		}

		RECT	RC, RCS;
		ObtenerRectaCliente(&RC, &RCS);

		HDC		Buffer		= CreateCompatibleDC(hDC);
		HBITMAP Bmp			= CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
		HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));


		//		LONG PixelInicio = static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion);
				//LONG PixelFin = PixelInicio + RCS.bottom;

				// Diferencia de pixeles Vertical (puede ser negativo si el primer item es parcialmente visible, o 0 si el primer item está justo al principio del área visible)
		LONG DifInicioV = _ItemPaginaVDif;

		size_t nItemFin = _ItemPaginaFin;
		if (nItemFin == -1) nItemFin = _Items.size() - 1;

		RECT RectaItem;
		LONG nAlto = Fuente.Alto() + (DLISTAEX_PADDING * 2);
		// _ItemPaginaInicio es unsigned -1 se refiere al valor máximo en una variable del tipo size_t
		if (_ItemPaginaInicio != -1) {
			for (size_t i = _ItemPaginaInicio; i < nItemFin + 1; i++) {
				RectaItem = { 0, DifInicioV, RCS.right, DifInicioV + nAlto };
				PintarItem(Buffer, i, RectaItem);
				DifInicioV += nAlto;							// Sumo la altura del nodo a la diferencia inicial
			}
		}

		// Pinto el resto del fondo
		if (DifInicioV < RCS.bottom) {
			RECT RFondo = RCS; RFondo.top = DifInicioV;
			HBRUSH BFondo = CreateSolidBrush(COLOR_LISTA_FONDO);
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

	void DListaEx::PintarItem(HDC hDC, const size_t nPosItem, RECT &Espacio) {
		if (_Items.size() == 0) return;

		BOOL bResaltado		= (nPosItem == _ItemResaltado) ? TRUE : FALSE;
		BOOL bPresionado	= (nPosItem == _ItemPresionado) ? TRUE : FALSE;
		//BOOL bMarcado		= (nPosItem == _ItemMarcado) ? TRUE : FALSE;

		COLORREF ColTexto, ColSombra, ColFondo;
		// Presionado ////////////////////////////////////
		if (bPresionado == TRUE) { 
			if (MostrarSeleccion == TRUE) {
				ColTexto  = COLOR_LISTA_SELECCION_TEXTO_PRESIONADO;
				ColSombra = COLOR_LISTA_SELECCION_TEXTO_SOMBRA;
				ColFondo  = COLOR_LISTA_SELECCION_PRESIONADO;
			}
			else {
				ColTexto  = COLOR_LISTA_TEXTO;
				ColSombra = COLOR_LISTA_TEXTO_SOMBRA;
				ColFondo  = COLOR_LISTA_FONDO_PRESIONADO;
			}
		}
		// Seleccionado //////////////////////////////////
		else if (_Items[nPosItem]->Seleccionado == TRUE && MostrarSeleccion == TRUE) { 
			if (bResaltado == FALSE) {	////////////////// Normal
				ColTexto = COLOR_LISTA_SELECCION_TEXTO;
				ColFondo = COLOR_LISTA_SELECCION;
			}
			else {	////////////////////////////////////// Resaltado
				ColTexto = COLOR_LISTA_SELECCION_TEXTO_RESALTADO;
				ColFondo = COLOR_LISTA_SELECCION_RESALTADO;
			}
			ColSombra = COLOR_LISTA_SELECCION_TEXTO_SOMBRA;
		}
		// Sin selección ni presionado ///////////////////
		else { 
			if (bResaltado == FALSE) {	////////////////// Normal
				ColTexto = COLOR_LISTA_TEXTO;
				ColFondo = COLOR_LISTA_FONDO;
			}
			else {  ////////////////////////////////////// Resaltado
				ColTexto = COLOR_LISTA_TEXTO_RESALTADO;
				ColFondo = COLOR_LISTA_FONDO_RESALTADO;
			}
			ColSombra = COLOR_LISTA_TEXTO_SOMBRA;
		}  ///////////////////////////////////////////////

		// Pinto el fondo del buffer
		HBRUSH BrochaFondo = CreateSolidBrush(ColFondo);
		RECT EspacioLocal = { 0 , 0, (static_cast<LONG>(_TotalAnchoVisible) > Espacio.right) ? static_cast<LONG>(_TotalAnchoVisible) : Espacio.right, (2 * DLISTAEX_PADDING) + Fuente.Alto() };
		FillRect(_BufferItem, &EspacioLocal, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Pinto el icono
		int PosYIco = ((Espacio.bottom - Espacio.top) - DLISTAEX_TAMICONO) / 2;
		DrawIconEx(_BufferItem, bPresionado + DLISTAEX_PADDING, bPresionado + PosYIco, _Items[nPosItem]->_Icono->Icono(), DLISTAEX_TAMICONO, DLISTAEX_TAMICONO, 0, 0, DI_NORMAL);

		RECT RCelda;
//		LONG AnchoPintado = (DLISTAEX_PADDING * 2) + DLISTAEX_TAMICONO;
		LONG AnchoPintado = 0;
		//		SetTextColor(_BufferItem, RGB(0, 0 ,0));
		for (size_t i = 0; i < _Columnas.size(); i++) {
			RCelda = {
				1 + bPresionado + AnchoPintado + DLISTAEX_PADDING, 
				1 + bPresionado + DLISTAEX_PADDING,
				1 + bPresionado + AnchoPintado + _Columnas[i]->_AnchoCalculado - (DLISTAEX_PADDING * 2),
				1 + bPresionado + Fuente.Alto() + DLISTAEX_PADDING
			};
			if (i == 0) { // La primera columna contiene el icono (que ya se ha pintado)
				RCelda.left += (DLISTAEX_PADDING * 2) + DLISTAEX_TAMICONO;
			}
			// Si hay texto lo pinto
			if (_Items[nPosItem]->Texto(i).size() > 0) {
				// Pinto la sombra
				SetTextColor(_BufferItem, ColSombra);
				DrawText(_BufferItem, _Items[nPosItem]->Texto(i).c_str(), static_cast<int>(_Items[nPosItem]->Texto(i).size()), &RCelda, _Columnas[i]->Alineacion | DT_NOPREFIX);

				// Pinto el texto
				SetTextColor(_BufferItem, ColTexto);
				OffsetRect(&RCelda, -1, -1);
				DrawText(_BufferItem, _Items[nPosItem]->Texto(i).c_str(), static_cast<int>(_Items[nPosItem]->Texto(i).size()), &RCelda, _Columnas[i]->Alineacion | DT_NOPREFIX);
			}
			// Llamo al evento virtual para pintar el subitem (para extender el control y pintar iconos por ejemplo)
			Evento_PintarSubItem(_BufferItem, nPosItem, i, &RCelda);

			AnchoPintado += _Columnas[i]->_AnchoCalculado;
		}

		if (nPosItem == _ItemMarcado) {
			// Pinto la marca del foco del teclado
			HBRUSH BrochaMarcaItem = CreateSolidBrush(COLOR_LISTA_NODO_MARCA);
			RECT RFondo = { 0, 0, Espacio.right - Espacio.left, Espacio.bottom - Espacio.top };
			FrameRect(_BufferItem, &RFondo, BrochaMarcaItem);
			DeleteObject(BrochaMarcaItem);
		}

		// Pinto el resultado del buffer al DC de la función pintar
		BitBlt(hDC, Espacio.left, Espacio.top, Espacio.right, Espacio.bottom - Espacio.top, _BufferItem, -_ItemPaginaHDif, 0, SRCCOPY);

	}

	/* Evento que se recibe al cambiar el scroll de posición */
	void DListaEx::Scrolls_EventoCambioPosicion(void) {
		RECT RC, RCS;
		//		GetClientRect(hWnd(), &RC);
		ObtenerRectaCliente(&RC, &RCS);
		_CalcularItemsPagina(RCS.bottom);
	}


	/* Función que determina el item que hay debajo de las coordenadas especificadas 
		[in]  cX			: Coordenada X
		[in]  cY			: Coordenada Y
		[out] nPosSubItem	: Si no es NULL devolverá la posición del SubItem
	*/
	const size_t DListaEx::HitTest(const int cX, const int cY, size_t *nPosSubItem) {
		if (_ItemPaginaInicio == -1) {
			#if DLISTAEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DListaEx::HitTest X:%d Y:%d I:-1 SI:-1\n", cX, cY);
			#endif
			return -1;
		}

		int PixelesContados		= _ItemPaginaVDif; // Pixeles de altura contados hasta el nodo
		int AltoItem			= Fuente.Alto() + (DLISTAEX_PADDING * 2);
		int PixelesContadosH	= _ItemPaginaHDif;
		size_t PosInicio		= _ItemPaginaInicio;
		size_t PosFin			= _ItemPaginaFin + 1;
		
		if (PosFin == 0) PosFin = _Items.size();
		for (size_t i = PosInicio; i < PosFin; i++) {
			// El item está en las coordenadas del mouse
			if (PixelesContados <= cY && PixelesContados + AltoItem >= cY) {				
				// Si PosSubItem no es NULL necesitamos determinar la posición del SubItem
				if (nPosSubItem != NULL) {
					for (size_t si = 0; si < _Columnas.size(); si++) {

//						if (si == 0) { PixelesContadosH += (DLISTAEX_PADDING * 2) + DLISTAEX_TAMICONO; } // Si es el primer subitem, incluyo el tamaño del icono al ancho contado.

						if (PixelesContadosH <= cX && PixelesContadosH + _Columnas[si]->_AnchoCalculado >= cX) {
							*nPosSubItem = si;
							#if DLISTAEX_MOSTRARDEBUG == TRUE
								Debug_Escribir_Varg(L"DListaEx::HitTest X:%d Y:%d I:%d SI:%d\n", cX, cY, i, si);
							#endif
							return i;
						}
						PixelesContadosH += _Columnas[si]->_AnchoCalculado;
					}
				}
				else { // Solo se busca el item
					#if DLISTAEX_MOSTRARDEBUG == TRUE
						Debug_Escribir_Varg(L"DListaEx::HitTest X:%d Y:%d I:%d\n", cX, cY, i);
					#endif
					return i;
				}
			}
			PixelesContados += AltoItem;
		}

		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::HitTest X:%d Y:%d I:-1\n", cX, cY);
		#endif
		return -1;
	}


	void DListaEx::EliminarItem(DListaEx_Item *eItem) {
		for (size_t i = 0; i < _Items.size(); i++) {
			if (_Items[i] == eItem) {
				EliminarItem(i);
				return;
			}
		}
	}

	void DListaEx::EliminarItem(const size_t ePos) {
		if (ePos < _Items.size()) {
			delete _Items[ePos];
			_Items.erase(_Items.begin() + ePos);

			_ItemMarcado        = -1;
			_ItemPresionado		= -1;
			_SubItemPresionado	= -1;
			_ItemResaltado		= -1;
			_SubItemResaltado	= -1;
			_ItemShift			= -1;
			// Recalculo todos los valores de la lista antes de repintar
			_Repintar = TRUE;
		}
	}

	void DListaEx::DesSeleccionarTodo(void) {
		for (size_t i = 0; i < _Items.size(); i++) {
			_Items[i]->Seleccionado = FALSE;
		}
	}

	void DListaEx::MostrarItem(DListaEx_Item *mItem) {
		for (size_t i = 0; i < _Items.size(); i++) {
			if (_Items[i] == mItem) {
				MostrarItem(i);
				return;
			}
		}
	}

	void DListaEx::MostrarItem(const size_t iPos) {
		if (iPos >= _Items.size() || _Items.size() == 0) return;

		RECT RItem;
		if (ObtenerRectaItem(iPos, RItem) == FALSE) return;

//		int nAltoItem = _Fuente.Alto() + (LISTAEX_PADDING * 2);
		
		// Obtengo la recta absoluta visible
		RECT RC, RAV;
		ObtenerRectaCliente(&RC, &RAV);

		// Sumo a la recta RAV las posiciones de los Scrolls V y H
		LONG YInicio = static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion);
		LONG XInicio = static_cast<LONG>((static_cast<float>(_TotalAnchoVisible) / 100.0f) * _ScrollH_Posicion);
		OffsetRect(&RAV, XInicio, YInicio);

		if (RItem.left < RAV.left) {			// Hay una parte a la izquierda del item que no es visible (lateral)
			_ScrollH_Posicion = (100.0f / static_cast<float>(_TotalAnchoVisible)) * static_cast<float>(RItem.left);
		}

		if (RItem.top < RAV.top) {				// Hay una parte del item que no es visible (por arriba)
			_ScrollV_Posicion = (100.0f / static_cast<float>(_TotalAltoVisible)) * static_cast<float>(RItem.top);
		}
		else if (RItem.bottom > RAV.bottom) {	// Hay una parte del item que no es visible (por abajo)
												// Sumo la diferencia de RItem.bottom + RAV.bottom a la posición del ScrollV
			_ScrollV_Posicion += (100.0f / static_cast<float>(_TotalAltoVisible)) * static_cast<float>(RItem.bottom - RAV.bottom);
		}

		// Calculo los nodos InicioPagina y FinPagina
		_CalcularScrolls();
//		_CalcularItemsPagina(RAV.bottom - RAV.top);

		// Repinto el control
		Repintar();
	}

	const size_t DListaEx::ItemPos(DListaEx_Item *pItem) {
		return std::find(_Items.begin(), _Items.end(), pItem) - _Items.begin();
	}

	void DListaEx::ItemMarcado(DListaEx_Item *NuevoItemMarcado, const BOOL nRepintar) {
		for (size_t i = 0; i < _Items.size(); i++) {
			if (NuevoItemMarcado == _Items[i]) {
				_ItemMarcado = i;
				break;
			}
		}
		if (nRepintar == TRUE) Repintar();
	}


	const BOOL DListaEx::Destruir(void) {
		EliminarTodasLasColumnas();
		EliminarTodosLosItems();
		return DControlEx::Destruir();
	}

	
	// Obtiene la recta absoluta del item
	const BOOL DListaEx::ObtenerRectaItem(const size_t iPos, RECT &rRecta) {
		if (_Items.size() == 0 || iPos >= _Items.size()) return FALSE;
		LONG nAncho = 0;
		for (size_t i = 0; i < _Columnas.size(); i++) nAncho += _Columnas[i]->_AnchoCalculado;		

		const int nAltoItem = Fuente.Alto() + (DLISTAEX_PADDING * 2);
		rRecta.left   = 0;
		rRecta.top    = nAltoItem * static_cast<LONG>(iPos);
		rRecta.right  = nAncho;
		rRecta.bottom = nAltoItem * (static_cast<LONG>(iPos) + 1);
		return TRUE;
	}

	void DListaEx::_CalcularItemsPagina(const size_t TamPagina) {
		_ItemPaginaInicio = -1;
		_ItemPaginaFin = -1;
		if (_Items.size() == 0 || _Columnas.size() == 0) {
			#if DLISTAEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DListaEx::_CalcularItemsPagina I:%d F:%d\n", static_cast<__int64>(_ItemPaginaInicio), static_cast<__int64>(_ItemPaginaFin));
			#endif
			return;
		}

		LONG   PixelInicio			= 0; 
		LONG   PixelFin				= static_cast<LONG>(TamPagina);
		LONG   TotalPixelesContados = -static_cast<LONG>((static_cast<float>(_TotalAltoVisible) / 100.0f) * _ScrollV_Posicion);

		_ItemPaginaHDif = -static_cast<LONG>((static_cast<float>(_TotalAnchoVisible) / 100.0f) * _ScrollH_Posicion);

		LONG   nAltoItem = Fuente.Alto() + (DLISTAEX_PADDING * 2);

		for (size_t i = 0; i < _Items.size(); i++) {
			if (_ItemPaginaInicio == -1) { // Item inicial
				if (TotalPixelesContados <= PixelInicio && TotalPixelesContados + nAltoItem >= PixelInicio) {
					_ItemPaginaVDif = static_cast<LONG>(TotalPixelesContados - PixelInicio);
					_ItemPaginaInicio = i;
				}
			}
			else {							// Item final
				if (TotalPixelesContados >= PixelFin - nAltoItem) {
					_ItemPaginaFin = i;

					#if DLISTAEX_MOSTRARDEBUG == TRUE
						Debug_Escribir_Varg(L"DListaEx::_CalcularItemsPagina I:%d F:%d\n", static_cast<__int64>(_ItemPaginaInicio), static_cast<__int64>(_ItemPaginaFin));
					#endif

					return;
				}
			}
			TotalPixelesContados += nAltoItem;
		}

		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::_CalcularItemsPagina I:%d F:%d\n", static_cast<__int64>(_ItemPaginaInicio), static_cast<__int64>(_ItemPaginaFin));
		#endif
	}

	// Calcula el total de espacio requerido para mostrar toda la lista en pixeles (además también calcula el tamaño de las columnas con ancho automático)
	void DListaEx::_CalcularTotalEspacioVisible(RECT &RC) {
		_TotalAltoVisible	= 0;
		_TotalAnchoVisible	= 0;
		if (_Items.size() == 0 || _Columnas.size() == 0) return;
		// Calculo el ancho de las columnas fijas, y cuento las columnas automáticas
		UINT ColumnasAuto = 0;
		INT  nAnchoVisible	= (DLISTAEX_PADDING *2) + DLISTAEX_TAMICONO;
		for (size_t i = 0; i < _Columnas.size(); i++) {
			if (_Columnas[i]->Ancho != DLISTAEX_COLUMNA_ANCHO_AUTO) {	nAnchoVisible += _Columnas[i]->Ancho;		}
			else													{	ColumnasAuto++;								}
		}
		// Hay una o mas columnas con el ancho automático
		if (ColumnasAuto > 0) {
			_TotalAnchoVisible = RC.right;
			// Calculo el ancho para las columnas que tiene ancho automático
			//  En principio está pensado para una sola columna con ancho automático, pero si hay mas de una se dividirá ese espacio equitativamente entre las columnas automáticas
/*			for (UINT c = 0; c < _Columnas.size(); c++) {
				if (_Columnas[c]->Ancho != DLISTAEX_COLUMNA_ANCHO_AUTO) {	_Columnas[c]->_AnchoCalculado = _Columnas[c]->Ancho;														} // Ancho sobrante dividido por el número de columnas automáticas
				else													{	_Columnas[c]->_AnchoCalculado = static_cast<LONG>((_TotalAnchoVisible - nAnchoVisible) / ColumnasAuto);		} // Ancho en pixeles
			}*/
		}
		else {
			if (nAnchoVisible >= RC.right)	_TotalAnchoVisible = nAnchoVisible;
			else                            _TotalAnchoVisible = RC.right;
		}

		// Altura (total de items * altura de la fuente)
		_TotalAltoVisible = (Fuente.Alto() + (DLISTAEX_PADDING * 2)) * _Items.size();
	}


	void DListaEx::_CalcularColumnas(void) {
		INT		nAnchoFijo		= (DLISTAEX_PADDING * 2) + DLISTAEX_TAMICONO;
		UINT	ColumnasAuto	= 0;
		size_t  i               = 0;
		// Cuento el ancho fijo y las columnas con ancho automático
		for (i = 0; i < _Columnas.size(); i++) {
			if (_Columnas[i]->Ancho != DLISTAEX_COLUMNA_ANCHO_AUTO) {	nAnchoFijo += _Columnas[i]->Ancho;		}
			else													{	ColumnasAuto++;							}
		}
		RECT RC, RCSS;
		ObtenerRectaCliente(&RC, &RCSS);
		// Asigno el AnchoCalculado a todas las columnas
		for (i = 0; i < _Columnas.size(); i++) {
			if (_Columnas[i]->Ancho != DLISTAEX_COLUMNA_ANCHO_AUTO) {	
				_Columnas[i]->_AnchoCalculado = _Columnas[i]->Ancho;	// Ancho en pixeles
			} 
			else													{	
				_Columnas[i]->_AnchoCalculado = static_cast<LONG>((RCSS.right - nAnchoFijo) / ColumnasAuto);	// Ancho sobrante dividido por el número de columnas automáticas
				if (i == 0) { _Columnas[i]->_AnchoCalculado += (DLISTAEX_PADDING * 2) + DLISTAEX_TAMICONO; }	// Si es la primera columna, añado el ancho del icono al tamaño auto
			} 
		}
	}


	// Calcula si hay que mostrar los scrolls V y H, sus tamaños de página y posiciones relativas, y además crea el buffer para pintar un item una vez determinados los tamaños de scroll
	void DListaEx::_CalcularScrolls(void) {
		_Repintar = FALSE;

		RECT RC;
		GetClientRect(hWnd(), &RC);
		_CalcularTotalEspacioVisible(RC);

		BOOL SV = FALSE, SH = FALSE;
		// Determino si se necesita scroll horizontal
		if (RC.right >= static_cast<LONG>(_TotalAnchoVisible))	{ SH = FALSE; }
		else													{ SH = TRUE;	RC.bottom -= _ScrollH_Alto; }
		// Determino si se necesita scroll vertical
		if (RC.bottom >= static_cast<LONG>(_TotalAltoVisible))	{ SV = FALSE; }
		else													{ SV = TRUE;	RC.right -= _ScrollV_Ancho; }

		// Borro y vuelvo a crear el buffer DC para pintar los nodos
		_CrearBufferItem((static_cast<LONG>(_TotalAnchoVisible) > RC.right) ? static_cast<int>(_TotalAnchoVisible) : static_cast<int>(RC.right), Fuente.Alto() + (DLISTAEX_PADDING *2) );

		// Calculo el ancho máximo
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

		// Calculo la altura máxima
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

		_CalcularColumnas();
		_CalcularItemsPagina(RC.bottom);
	}




	void DListaEx::_CrearBufferItem(const int nAncho, const int nAlto) {
		if (_BufferItem != NULL) {
			SelectObject(_BufferItem, _BufferItemFuenteViejo);
			SelectObject(_BufferItem, _BufferItemBmpViejo);
			DeleteObject(_BufferItemBmp);
			DeleteDC(_BufferItem);
		}
		HDC hDC = GetDC(NULL);
		_BufferItem				= CreateCompatibleDC(NULL);
		_BufferItemBmp			= CreateCompatibleBitmap(hDC, nAncho, nAlto);
		_BufferItemBmpViejo		= static_cast<HBITMAP>(SelectObject(_BufferItem, _BufferItemBmp));
		_BufferItemFuenteViejo	= static_cast<HFONT>(SelectObject(_BufferItem, Fuente()));
		ReleaseDC(NULL, hDC);
		SetBkMode(_BufferItem, TRANSPARENT);
	}

	void DListaEx::_Evento_Pintar(void) {		
		PAINTSTRUCT PS;
		HDC DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}

	void DListaEx::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)));
//		_mX = cX; _mY = cY;
		// Utilizo la función _MouseEntrando() para poder recibir los mensajes WM_MOUSELEAVE
		BOOL bME = _MouseEntrando();
//		if (bME == TRUE)	Scrolls_MouseEntrando();		

		if (Scrolls_MouseMovimiento(DatosMouse) == TRUE) { return; } // las coordenadas pertenecen al scroll (salgo del evento)
		
		_ItemResaltado = HitTest(DatosMouse.X(), DatosMouse.Y(), &_SubItemResaltado);
		
		Evento_MouseMovimiento(DatosMouse);

		if (bME == TRUE)	Evento_MouseEntrando();

		// Envio el evento mouseup a la ventana padre
		SendMessage(GetParent(hWnd()), DWL_LISTAEX_MOUSEMOVIMIENTO, reinterpret_cast<WPARAM>(&DatosMouse), 0);

		// Comprueba si el item resaltado es el mismo que la ultima vez, y si no lo és repinta el control
		if (_ItemResaltado != _ItemUResaltado || _SubItemResaltado != _SubItemUResaltado) {
			_ItemUResaltado = _ItemResaltado;
			_SubItemUResaltado = _SubItemResaltado;
			Repintar();
		}
	}

	void DListaEx::_Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)), Boton);
		SetFocus(_hWnd);
		if (Scrolls_MousePresionado(DatosMouse) == TRUE) { return; }

		_ItemPresionado = HitTest(DatosMouse.X(), DatosMouse.Y(), &_SubItemPresionado);
		_ItemMarcado	= _ItemPresionado;

		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::_Evento_MousePresionado IP:%d X:%d Y:%d\n", _ItemMarcado, DatosMouse.X(), DatosMouse.Y());
		#endif

		if (_ItemPresionado != -1) {
			DesSeleccionarTodo();
			_Items[_ItemPresionado]->Seleccionado = TRUE;
		}

		Evento_MousePresionado(DatosMouse);
		Repintar();
		// Envio el evento mouseup a la ventana padre
		SendMessage(GetParent(hWnd()), DWL_LISTAEX_MOUSEPRESIONADO, reinterpret_cast<WPARAM>(&DatosMouse), 0);

	}

	void DListaEx::_Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)), Boton);

		if (Scrolls_MouseSoltado(DatosMouse) == TRUE) { return; }

		if (_ItemPresionado != -1) {
			_Items[_ItemPresionado]->Seleccionado = TRUE;
		}

		// Evento virtual
		Evento_MouseSoltado(DatosMouse);
		// Envio el evento mouseup a la ventana padre
		SendMessage(GetParent(hWnd()), DWL_LISTAEX_MOUSESOLTADO, reinterpret_cast<WPARAM>(&DatosMouse), 0);
		// Establezco que no hay ningún item presionado, y repinto
		// TODO : El -1 está malament lo millor seria fer servir el objecte DListaEx_Item * directament com a l'arbre
		_ItemPresionado = -1;
		_SubItemPresionado = -1;
		Repintar();

	}

	void DListaEx::_Evento_MouseRueda(WPARAM wParam, LPARAM lParam) {
		DEventoMouseRueda DatosMouse(wParam, lParam, ID());

		RECT RW;
		GetWindowRect(hWnd(), &RW);

		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::_Evento_MouseRueda cX:%d cY:%d mX:%d mY:%d\n", RW.left - DatosMouse.X() , RW.top - DatosMouse.Y());
		#endif


		if (DatosMouse.Delta() > 0) { // hacia arriba
			_ScrollV_Posicion -= _ScrollV_Pagina / 10.0f;
			if (_ScrollV_Posicion < 0.0f) _ScrollV_Posicion = 0.0f;
		}
		else { // hacia abajo
			_ScrollV_Posicion += _ScrollV_Pagina / 10.0f;
			if (_ScrollV_Posicion + _ScrollV_Pagina > 100.0f) _ScrollV_Posicion = 100.0f - _ScrollV_Pagina;
		}

		_CalcularScrolls();
		// Las coordenadas X e Y son relativas a la pantalla...
		LONG ncX = RW.left - DatosMouse.X();
		LONG ncY = RW.top - DatosMouse.Y();
		_ItemResaltado = HitTest(ncX, ncY);
		_ItemUResaltado = _ItemResaltado;

		Evento_MouseRueda(DatosMouse);
		Repintar();
	}


	void DListaEx::_Evento_TeclaPresionada(WPARAM wParam, LPARAM lParam) {
		DEventoTeclado DatosTeclado(wParam, lParam, ID());
		// Marco la tecla como presionada
		DhWnd::Teclado[DatosTeclado.TeclaVirtual()] = true;

		// Si hay nodos ...
		if (_Items.size() > 0) {
			// Me guardo el item marcado para indicar desde donde empieza el shift
			if (DatosTeclado.TeclaVirtual() == VK_SHIFT) {
				if (_ItemShift == -1) {
					_ItemShift = (_ItemMarcado == NULL) ? _ItemPaginaInicio : _ItemMarcado;
				}
			}

			switch (DatosTeclado.TeclaVirtual()) {
				case VK_HOME	: _Tecla_Inicio();				break;
				case VK_END		: _Tecla_Fin();					break;
				case VK_UP		: _Tecla_CursorArriba();		break;
				case VK_DOWN	: _Tecla_CursorAbajo();			break;
				case VK_PRIOR	: _Tecla_RePag();				break; // RePag
				case VK_NEXT	: _Tecla_AvPag();				break; // AvPag
				default			: Evento_Tecla(DatosTeclado);	break; // if (Caracter >= 0x30 && Caracter <= 0x5A) // Cualquier tecla valida
			}
		}
		Evento_TeclaPresionada(DatosTeclado);

	}

	void DListaEx::_Evento_TeclaSoltada(WPARAM wParam, LPARAM lParam) {
		DEventoTeclado DatosTeclado(wParam, lParam, ID());
		Evento_TeclaSoltada(DatosTeclado);
	}

	void DListaEx::_Evento_Tecla(WPARAM wParam, LPARAM lParam) {
		DEventoTeclado DatosTeclado(wParam, lParam, ID());
		Evento_Tecla(DatosTeclado);
	}


	// Teclas especiales
	void DListaEx::_Tecla_CursorArriba(void) {
		// Si no hay nodos, salgo de la función
		if (_Items.size() == 0) return;

		if (_ItemMarcado == -1) {	
			_ItemMarcado = _ItemPaginaInicio;
			_Items[_ItemMarcado]->Seleccionado = TRUE;
			MostrarItem(_ItemMarcado);
		}

		if (_ItemMarcado - 1 != -1) {
			_ItemMarcado--;
			DesSeleccionarTodo();
			_Items[_ItemMarcado]->Seleccionado = TRUE;
			MostrarItem(_ItemMarcado);
		}
	}

	void DListaEx::_Tecla_CursorAbajo(void) {
		// Si no hay nodos, salgo de la función
		if (_Items.size() == 0) return;

		if (_ItemMarcado == -1) {
			_ItemMarcado = _ItemPaginaInicio;
			_Items[_ItemMarcado]->Seleccionado = TRUE;
			MostrarItem(_ItemMarcado);
		}
		if (_ItemMarcado + 1 < _Items.size()) {
			_ItemMarcado++;
			DesSeleccionarTodo();
			_Items[_ItemMarcado]->Seleccionado = TRUE;
			MostrarItem(_ItemMarcado);
		}
	}

	void DListaEx::_Tecla_Inicio(void) {
		// Si no hay nodos, salgo de la función
		if (_Items.size() == 0) return;

		DesSeleccionarTodo();
		_ItemMarcado = 0;
		_Items[_ItemMarcado]->Seleccionado = TRUE;
		MostrarItem(_ItemMarcado);
	}

	void DListaEx::_Tecla_Fin(void) {
		// Si no hay nodos, salgo de la función
		if (_Items.size() == 0) return;

		DesSeleccionarTodo();
		_ItemMarcado = _Items.size() - 1;
		_Items[_ItemMarcado]->Seleccionado = TRUE;
		MostrarItem(_ItemMarcado);
	}

	void DListaEx::_Tecla_AvPag(void) {
	}

	void DListaEx::_Tecla_RePag(void) {
	}


	void DListaEx::_Evento_MouseSaliendo(void) {
		BOOL nRepintar = Scrolls_MouseSaliendo();
		_MouseDentro = FALSE;
		_ItemResaltado = -1;
		Evento_MouseSaliendo();
		if (nRepintar == TRUE) Repintar();
	}

	void DListaEx::_Evento_MouseDobleClick(const int Boton, WPARAM wParam, LPARAM lParam) {
		DEventoMouse DatosMouse(wParam, lParam, static_cast<int>(GetWindowLongPtr(_hWnd, GWL_ID)), Boton);
		Evento_MouseDobleClick(DatosMouse);

//		Evento_MouseDobleClick(Boton, cX, cY, Param);
	}

	void DListaEx::_Evento_FocoObtenido(HWND hWndUltimoFoco) {
		BorrarBufferTeclado();
		Evento_FocoObtenido(hWndUltimoFoco);
	}

	void DListaEx::_Evento_FocoPerdido(HWND hWndNuevoFoco) {
		BorrarBufferTeclado();
		Evento_FocoPerdido(hWndNuevoFoco);
	}


	LRESULT CALLBACK DListaEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			// Foco
			case WM_SETFOCUS:		_Evento_FocoObtenido((HWND)wParam);																											return 0;
			case WM_KILLFOCUS:		_Evento_FocoPerdido((HWND)wParam);																											return 0;
			// Pintado
			case WM_PAINT:			_Evento_Pintar();																															return 0;
			// Cambio de tamaño
			case WM_SIZE:			_CalcularScrolls();		Repintar();																											return 0;
			// Mouse
			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(wParam, lParam);																									return 0;
//			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;
			case WM_MOUSELEAVE:		_Evento_MouseSaliendo();																													return 0;
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

			// Mouse rueda
			case WM_MOUSEWHEEL:		_Evento_MouseRueda(wParam, lParam);																											return 0;
			// Teclado
			case WM_KEYDOWN:		_Evento_TeclaPresionada(wParam, lParam);																									break;
			case WM_KEYUP:			_Evento_TeclaSoltada(wParam, lParam);																										break;
			case WM_CHAR:           _Evento_Tecla(wParam, lParam);																												break;
		}	
		return DControlEx::GestorMensajes(uMsg, wParam, lParam);
	}

}