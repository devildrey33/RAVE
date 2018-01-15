#include "stdafx.h"
#include "DListaEx.h"
#include "DListaIconos.h"

namespace DWL {

	DListaEx::DListaEx(void) :	_ItemPaginaInicio(-1)	, _ItemPaginaFin(-1)	, _ItemPaginaVDif(0)		, _ItemPaginaHDif(0),
								_ItemResaltado(-1)		, _ItemUResaltado(-1)	, _ItemMarcado(-1)			, /*_SubItemResaltado(-1)		,*/
								_ItemPresionado(-1)		, _ItemShift(-1)		, _CalcularValores(FALSE)	,
								_TotalAnchoVisible(0)	, _TotalAltoVisible(0)	,
								_BufferItem(NULL)		, _BufferItemBmp(NULL)	, _BufferItemBmpViejo(NULL)	, _BufferItemFuenteViejo(NULL) {

		_ColorFondoScroll = COLOR_LISTA_FONDO_SCROLL;
	}


	DListaEx::~DListaEx(void) {
		EliminarTodasLasColumnas();
		EliminarTodosLosItems();
	}

	
	HWND DListaEx::CrearListaEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
		if (hWnd()) { Debug_Escribir(L"DListaEx::CrearListaEx() Error : ya se ha creado la lista\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DListaEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD, NULL, CS_DBLCLKS); // CS_DBLCLKS (el control recibe notificaciones de doble click)
		_Fuente = hWnd._Fuente;
		return hWnd();
	}
	
	DListaEx_Columna *DListaEx::AgregarColumna(const int nAncho, DListaEx_Columna_Alineacion nAlineacion) {
		DListaEx_Columna *nColumna = new DListaEx_Columna(nAlineacion, nAncho);
		_Columnas.push_back(nColumna);
		_CalcularValores = TRUE;
		return nColumna;
	}
	
	DListaEx_Item *DListaEx::_AgregarItem(const int nIcono, const size_t PosicionItem, const TCHAR *nTxt, va_list Marker) {
		DListaEx_Item		*nItem		= new DListaEx_Item();
		DListaEx_SubItem	*nSubItem 	= new DListaEx_SubItem(nTxt);
		nItem->_SubItems.push_back(nSubItem);

		nItem->_Icono = DListaIconos::AgregarIconoRecursos(nIcono, LISTAEX_TAMICONO, LISTAEX_TAMICONO);

		// Obtengo los textos de cada columna
		for (size_t i = 1; i < _Columnas.size(); i++) {
			nSubItem = new DListaEx_SubItem(static_cast<const TCHAR *>(va_arg(Marker, const TCHAR *)));
			nItem->_SubItems.push_back(nSubItem);
		}
		_Items.push_back(nItem);
		
		_CalcularValores = TRUE;
//		_CalcularScrolls();

		return nItem;
	}

	void DListaEx::EliminarTodosLosItems(void) {
		for (size_t i = 0; i < _Items.size(); i++) {
			delete _Items[i];
		}
		_Items.resize(0);
	}

	void DListaEx::EliminarTodasLasColumnas(void) {
		for (size_t i = 0; i < _Columnas.size(); i++) {
			delete _Columnas[i];
		}
		_Columnas.resize(0);
	}

	void DListaEx::Pintar(HDC hDC) {

		if (_CalcularValores == TRUE) {
			_CalcularValores = FALSE;
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
		LONG nAlto = _Fuente.Alto() + (LISTAEX_PADDING * 2);
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

		BOOL bResaltado		= (nPosItem == _ItemResaltado) ? TRUE : FALSE;
		BOOL bPresionado	= (nPosItem == _ItemPresionado) ? TRUE : FALSE;
		//BOOL bMarcado		= (nPosItem == _ItemMarcado) ? TRUE : FALSE;

		COLORREF ColTexto, ColSombra, ColFondo;
		// Presionado ////////////////////////////////////
		if (bPresionado == TRUE) { 
			ColTexto  = COLOR_LISTA_SELECCION_TEXTO_PRESIONADO;
			ColSombra = COLOR_LISTA_SELECCION_TEXTO_SOMBRA;
			ColFondo  = COLOR_LISTA_SELECCION_PRESIONADO;
		}
		// Seleccionado //////////////////////////////////
		else if (_Items[nPosItem]->Seleccionado == TRUE) { 
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
		RECT EspacioLocal = { 0 , 0, (static_cast<LONG>(_TotalAnchoVisible) > Espacio.right) ? static_cast<LONG>(_TotalAnchoVisible) : Espacio.right, (2 * LISTAEX_PADDING) + _Fuente.Alto() };
		FillRect(_BufferItem, &EspacioLocal, BrochaFondo);
		DeleteObject(BrochaFondo);

		// Pinto el icono
		int PosYIco = ((Espacio.bottom - Espacio.top) - LISTAEX_TAMICONO) / 2;
		DrawIconEx(_BufferItem, bPresionado + LISTAEX_PADDING, bPresionado + PosYIco, _Items[nPosItem]->_Icono->Icono(), LISTAEX_TAMICONO, LISTAEX_TAMICONO, 0, 0, DI_NORMAL);

		RECT RCelda;
		LONG AnchoPintado = (LISTAEX_PADDING * 2) + LISTAEX_TAMICONO;
		SetTextColor(_BufferItem, RGB(0, 0 ,0));
		for (size_t i = 0; i < _Columnas.size(); i++) {
			RCelda = {
				1 + bPresionado + AnchoPintado + LISTAEX_PADDING, 
				1 + bPresionado + LISTAEX_PADDING,
				1 + bPresionado + AnchoPintado + _Columnas[i]->_AnchoCalculado - (LISTAEX_PADDING * 2),
				1 + bPresionado + _Fuente.Alto() + LISTAEX_PADDING
			};
			
			// Pinto la sombra
			SetTextColor(_BufferItem, ColSombra);
			DrawText(_BufferItem, _Items[nPosItem]->Texto(i).c_str(), static_cast<int>(_Items[nPosItem]->Texto(i).size()), &RCelda, _Columnas[i]->Alineacion | DT_NOPREFIX);

			// Pinto el texto
			SetTextColor(_BufferItem, ColTexto);
			OffsetRect(&RCelda, -1, -1);
			DrawText(_BufferItem, _Items[nPosItem]->Texto(i).c_str(), static_cast<int>(_Items[nPosItem]->Texto(i).size()), &RCelda, _Columnas[i]->Alineacion | DT_NOPREFIX);

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



	const size_t DListaEx::HitTest(const int cX, const int cY) {
		if (_ItemPaginaInicio == -1) {
			#if DLISTAEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DListaEx::HitTest X:%d Y:%d P:-1\n", cX, cY);
			#endif
			return -1;
		}

//		_TotalAltoVisible





		int PixelesContados = _ItemPaginaVDif; // Pixeles de altura contados hasta el nodo
		int AltoItem = _Fuente.Alto() + (LISTAEX_PADDING * 2);
		int PixelesContadosH = _ItemPaginaHDif;
		for (size_t i = _ItemPaginaInicio; i < _ItemPaginaFin + 1; i++) {			
			// El item está en las coordenadas del mouse
			if (PixelesContados <= cY && PixelesContados + AltoItem >= cY) {				
				#if DLISTAEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DListaEx::HitTest X:%d Y:%d P:%d\n", cX, cY, i);
				#endif
				return i;
			}
			PixelesContados += AltoItem;
		}

		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::HitTest X:%d Y:%d P:-1\n", cX, cY);
		#endif
		return -1;
	}


	void DListaEx::DesSeleccionarTodo(void) {
		for (size_t i = 0; i < _Items.size(); i++) {
			_Items[i]->Seleccionado = FALSE;
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

	const size_t DListaEx::PosItem(DListaEx_Item *pItem) {
		return std::find(_Items.begin(), _Items.end(), pItem) - _Items.begin();
	}
	
	// Obtiene la recta absoluta del item
	const BOOL DListaEx::ObtenerRectaItem(const size_t iPos, RECT &rRecta) {
		if (_Items.size() == 0 || iPos >= _Items.size()) return FALSE;
		LONG nAncho = 0;
		for (size_t i = 0; i < _Columnas.size(); i++) nAncho += _Columnas[i]->_AnchoCalculado;		

		const int nAltoItem = _Fuente.Alto() + (LISTAEX_PADDING * 2);
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

		LONG   nAltoItem = _Fuente.Alto() + (LISTAEX_PADDING * 2);

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
		INT  nAnchoVisible	= (LISTAEX_PADDING *2) + LISTAEX_TAMICONO;
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
		_TotalAltoVisible = (_Fuente.Alto() + (LISTAEX_PADDING * 2)) * _Items.size();
	}


	void DListaEx::_CalcularColumnas(void) {
		INT		nAnchoFijo		= (LISTAEX_PADDING * 2) + LISTAEX_TAMICONO;
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
				if (_Columnas[i]->Ancho != DLISTAEX_COLUMNA_ANCHO_AUTO) {	_Columnas[i]->_AnchoCalculado = _Columnas[i]->Ancho;											} // Ancho sobrante dividido por el número de columnas automáticas
				else													{	_Columnas[i]->_AnchoCalculado = static_cast<LONG>((RCSS.right - nAnchoFijo) / ColumnasAuto);	} // Ancho en pixeles
		}
	}


	// Calcula si hay que mostrar los scrolls V y H, sus tamaños de página y posiciones relativas, y además crea el buffer para pintar un item una vez determinados los tamaños de scroll
	void DListaEx::_CalcularScrolls(void) {
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
		_CrearBufferItem((static_cast<LONG>(_TotalAnchoVisible) > RC.right) ? static_cast<int>(_TotalAnchoVisible) : static_cast<int>(RC.right), _Fuente.Alto() + (LISTAEX_PADDING *2) );

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
		_BufferItemFuenteViejo	= static_cast<HFONT>(SelectObject(_BufferItem, _Fuente()));
		ReleaseDC(NULL, hDC);
		SetBkMode(_BufferItem, TRANSPARENT);
	}

	void DListaEx::_Evento_Pintar(void) {		
		PAINTSTRUCT PS;
		HDC DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}

	void DListaEx::_Evento_MouseMovimiento(const int cX, const int cY, const UINT Param) {
//		_mX = cX; _mY = cY;
		// Utilizo la función _MouseEntrando() para poder recibir los mensajes WM_MOUSELEAVE
		BOOL bME = _MouseEntrando();
//		if (bME == TRUE)	Scrolls_MouseEntrando();		

		if (Scrolls_MouseMovimiento(cX, cY, Param) == TRUE) { return; } // las coordenadas pertenecen al scroll (salgo del evento)

		_ItemResaltado = HitTest(cX, cY);
		// Comprueba si el item resaltado es el mismo que la ultima vez, y si no lo és repinta el control
		if (_ItemResaltado != _ItemUResaltado) {
			_ItemUResaltado = _ItemResaltado;
			Repintar();
		}
		
		Evento_MouseMovimiento(cX, cY, Param);

		if (bME == TRUE)	Evento_MouseEntrando();
	}

	void DListaEx::_Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param) {
		SetFocus(hWnd());
		if (Scrolls_MousePresionado(Boton, cX, cY, Param) == TRUE) { return; }

		_ItemPresionado = HitTest(cX, cY);
		_ItemMarcado	= _ItemPresionado;

		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::_Evento_MousePresionado IP:%d X:%d Y:%d\n", _ItemMarcado, cX, cY);
		#endif

		if (_ItemPresionado != -1) {
			DesSeleccionarTodo();
			_Items[_ItemPresionado]->Seleccionado = TRUE;
		}

		Evento_MousePresionado(Boton, cX, cY, Param);
		Repintar();
	}

	void DListaEx::_Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param) {
		if (Scrolls_MouseSoltado(Boton, cX, cY, Param) == TRUE) { return; }

		if (_ItemPresionado != -1) {
			_Items[_ItemPresionado]->Seleccionado = TRUE;
			_ItemPresionado = -1;
		}

		Evento_MouseSoltado(Boton, cX, cY, Param);
		Repintar();
	}

	void DListaEx::_Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey) {
		RECT RW;
		GetWindowRect(hWnd(), &RW);

		#if DLISTAEX_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DListaEx::_Evento_MouseRueda cX:%d cY:%d mX:%d mY:%d\n", RW.left - cX , RW.top - cY);
		#endif


		if (Delta > 0) { // hacia arriba
			_ScrollV_Posicion -= _ScrollV_Pagina / 10.0f;
			if (_ScrollV_Posicion < 0.0f) _ScrollV_Posicion = 0.0f;
		}
		else { // hacia abajo
			_ScrollV_Posicion += _ScrollV_Pagina / 10.0f;
			if (_ScrollV_Posicion + _ScrollV_Pagina > 100.0f) _ScrollV_Posicion = 100.0f - _ScrollV_Pagina;
		}

		_CalcularScrolls();
		// Las coordenadas X e Y son relativas a la pantalla...
		LONG ncX = RW.left - cX;
		LONG ncY = RW.top - cY;
		_ItemResaltado = HitTest(ncX, ncY);
		_ItemUResaltado = _ItemResaltado;

		Evento_MouseRueda(Delta, ncX, ncY, VirtKey);
		Repintar();
	}


	void DListaEx::_Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) { 
		// Marco la tecla como presionada
		DhWnd::_Teclado[Caracter] = true;

		// Si no hay nodos, salgo de la función
		if (_Items.size() == 0) {
			Evento_TeclaPresionada(Caracter, Repeticion, Params);
			return;
		}

		// Me guardo el item marcado para indicar desde donde empieza el shift
		if (Caracter == VK_SHIFT) {
			if (_ItemShift == -1) {
				_ItemShift = (_ItemMarcado == NULL) ? _ItemPaginaInicio : _ItemMarcado;
			}
		}

		switch (Caracter) {
			case VK_HOME		: _Tecla_Inicio();								break;
			case VK_END			: _Tecla_Fin();									break;
			case VK_UP			: _Tecla_CursorArriba();						break;
			case VK_DOWN		: _Tecla_CursorAbajo();							break;
			case VK_PRIOR		: _Tecla_RePag();								break; // RePag
			case VK_NEXT		: _Tecla_AvPag();								break; // AvPag
			default				: Evento_Tecla(Caracter, Repeticion, Params);	break; // if (Caracter >= 0x30 && Caracter <= 0x5A) // Cualquier tecla valida
		}

		Evento_TeclaPresionada(Caracter, Repeticion, Params);

	}

	void DListaEx::_Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
	}

	void DListaEx::_Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param) {
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
		Evento_MouseSaliendo();
		if (nRepintar == TRUE) Repintar();
	}

	void DListaEx::_Evento_MouseDobleClick(const UINT Boton, const int cX, const int cY, const UINT Param) {
		Evento_MouseDobleClick(Boton, cX, cY, Param);
	}

	LRESULT CALLBACK DListaEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			// Foco
			case WM_SETFOCUS:		hWnd.BorrarBufferTeclado();																													return 0;
			case WM_KILLFOCUS:		hWnd.BorrarBufferTeclado();																													return 0;
			// Pintado
			case WM_PAINT:			_Evento_Pintar();																															return 0;
			// Cambio de tamaño
			case WM_SIZE:			_CalcularScrolls();		Repintar();																											return 0;
			// Mouse
			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;
			case WM_MOUSELEAVE:		_Evento_MouseSaliendo();																													return 0;
			// Mouse presionado
			case WM_LBUTTONDOWN:	_Evento_MousePresionado(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_RBUTTONDOWN:	_Evento_MousePresionado(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_MBUTTONDOWN:	_Evento_MousePresionado(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			// Mouse soltado
			case WM_LBUTTONUP:		_Evento_MouseSoltado(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;
			case WM_RBUTTONUP:		_Evento_MouseSoltado(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;
			case WM_MBUTTONUP:		_Evento_MouseSoltado(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;
			// Mouse doble click
			case WM_LBUTTONDBLCLK:	_Evento_MouseDobleClick(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_RBUTTONDBLCLK:	_Evento_MouseDobleClick(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_MBUTTONDBLCLK:	_Evento_MouseDobleClick(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			// Mouse rueda
			case WM_MOUSEWHEEL:		_Evento_MouseRueda(static_cast<short>(HIWORD(wParam)), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(LOWORD(wParam)));		return 0;
			// Teclado
			case WM_KEYDOWN:		_Evento_TeclaPresionada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
			case WM_KEYUP:			_Evento_TeclaSoltada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}

}