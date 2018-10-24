#ifndef DLISTAEX_H
#define DLISTAEX_H

#include "DControlEx.h"
#include "DListaEx_Columna.h"
#include "DListaEx_Item.h"

namespace DWL {

	#define DLISTAEX_MOSTRARDEBUG	FALSE

	#ifdef _WIN64
		#define DLISTAEX_POSICION_INICIO	 0
		#define DLISTAEX_POSICION_FIN		-1
		#define DLISTAEX_POSICION_ORDENADO	-2
	#else
		#define DLISTAEX_POSICION_INICIO	0
		#define DLISTAEX_POSICION_FIN		_UI32_MAX - 1
		#define DLISTAEX_POSICION_ORDENADO	_UI32_MAX - 2
	#endif

	/* Estados del mouse que puede tener un item */
	enum DListaEx_Item_EstadoMouse {
		DListaEx_Item_EstadoMouse_Normal		= 0,
		DListaEx_Item_EstadoMouse_Resaltado		= 1,
		DListaEx_Item_EstadoMouse_Presionado	= 2
	};

	class DDesplegableEx;

	class DListaEx : public virtual DBarraScrollEx {
	  public:
														DListaEx(void);
		                                               ~DListaEx(void);
														// Crea el control ListaEx
		HWND											CrearListaEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const DWORD Estilos = WS_CHILD | WS_VISIBLE, const DWORD EstilosExtendidos = NULL);
		const BOOL										Destruir(void);

		DListaEx_Columna							   *AgregarColumna(const int nAncho = DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion nAlineacion = DListaEx_Columna_Alineacion_Izquierda);

//		void                                            EliminarColumna(const size_t nPosColumna);
		void											EliminarTodasLasColumnas(void);

														// Agrega un item personalizado (por defecto es del tipo DListaEx_Item) para iconos de los recursos
		template <class TItem = DListaEx_Item> TItem   *AgregarItem(const INT_PTR nIconoRecursos = NULL, const INT_PTR PosicionFila = DLISTAEX_POSICION_FIN, const TCHAR *nTxt = NULL, ...) {
															TItem  *nItem = new TItem();
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO);
															va_list Marker;
															va_start(Marker, nTxt);
															_AgregarItem(nItem, TmpIco, PosicionFila, nTxt, Marker);
															va_end(Marker);
															return nItem;
														};

														// Agrega un item personalizado (por defecto es del tipo DListaEx_Item) para iconos del sistema
		template <class TItem = DListaEx_Item> TItem   *AgregarItem(const GUID nIconoKnownFolder, const INT_PTR PosicionFila = DLISTAEX_POSICION_FIN, const TCHAR *nTxt = NULL, ...) {
															TItem  *nItem = new TItem();
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoKnownFolder(nIconoKnownFolder, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO);
															va_list Marker;
															va_start(Marker, nTxt);
															_AgregarItem(nItem, TmpIco, PosicionFila, nTxt, Marker);
															va_end(Marker);
															return nItem;
														};

														// Agrega un item personalizado (por defecto es del tipo DListaEx_Item) para iconos especificos de un path
		template <class TItem = DListaEx_Item> TItem   *AgregarItem(const wchar_t *nPathIcono, const int nPosIcono = 0, const INT_PTR PosicionFila = DLISTAEX_POSICION_FIN, const TCHAR *nTxt = NULL, ...) {
															TItem  *nItem = new TItem();
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoExterno(nPathIcono, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO, nPosIcono);
															va_list Marker;
															va_start(Marker, nTxt);
															_AgregarItem(nItem, TmpIco, PosicionFila, nTxt, Marker);
															va_end(Marker);
															return nItem;
														};

		void											EliminarItem(const LONGLONG ePosItem);
		void											EliminarItem(DListaEx_Item *eItem);
		const UINT										EliminarItemsSeleccionados(void);
		void											EliminarTodosLosItems(void);
														// Acceso a los DListaEx_Item
		inline DListaEx_Item                           *Item(const LONGLONG iPos) { if (iPos == -1)            { return NULL; }	return _Items[static_cast<unsigned int>(iPos)]; }
		inline DListaEx_Item                           *ItemResaltado(void)	  	  { if (_ItemResaltado == -1)  { return NULL; }	return _Items[static_cast<unsigned int>(_ItemResaltado)]; };
		inline DListaEx_Item                           *ItemPresionado(void)	  { if (_ItemPresionado == -1) { return NULL; }	return _Items[static_cast<unsigned int>(_ItemPresionado)]; };
		inline DListaEx_Item                           *ItemMarcado(void)		  { if (_ItemMarcado == -1)    { return NULL; }	return _Items[static_cast<unsigned int>(_ItemMarcado)]; };
		void											ItemMarcado(DListaEx_Item *NuevoItemMarcado, const BOOL nRepintar = FALSE);
														// Devuelve la posici�n del DListaEx_Item que est� dentro de esta lista
		const LONGLONG                                  ItemPos(DListaEx_Item *pItem);
		virtual void									Repintar(const BOOL nForzar = FALSE);
		void											DesSeleccionarTodo(void);
		void											MostrarItem(const LONGLONG iPos, const BOOL nRepintar = TRUE);
		void											MostrarItem(DListaEx_Item *eItem);

		const BOOL                                      ObtenerRectaItem(const LONGLONG iPos, RECT &rRecta);
		inline const BOOL                               ObtenerRectaItem(DListaEx_Item *eItem, RECT &rRecta) { return ObtenerRectaItem(ItemPos(eItem), rRecta); }

		inline const size_t								TotalItems(void) { return _Items.size(); }
		void											TotalItemsSeleccionados(void); // poc util...

		inline const size_t                             TotalColumnas(void) { return _Columnas.size();  }
		inline DListaEx_Columna                        *Columna(const LONGLONG cPos) { return _Columnas[static_cast<unsigned int>(cPos)];  }

		void											Pintar(HDC hDC);
		void											PintarItem(HDC hDC, const LONGLONG pPosItem, RECT &Espacio);

		const LONGLONG									HitTest(const int cX, const int cY, LONGLONG *PosSubItem = NULL);

		void											Scrolls_EventoCambioPosicion(void);

														// Eventos virtuales
		virtual void          							Evento_MouseEntrando(void)																		{ };
		virtual void									Evento_MouseSaliendo(void)																		{ };
		virtual void									Evento_MouseMovimiento(DEventoMouse &DatosMouse)												{ };
		virtual void									Evento_MousePresionado(DEventoMouse &DatosMouse)												{ };
		virtual void									Evento_MouseSoltado(DEventoMouse &DatosMouse)													{ };
		virtual void                                    Evento_MouseRueda(DEventoMouseRueda &DatosMouse)												{ };
		virtual void									Evento_MouseDobleClick(DEventoMouse &DatosMouse)												{ };


		virtual void                                    Evento_TeclaPresionada(DEventoTeclado &DatosTeclado)											{ };
		virtual void                                    Evento_TeclaSoltada(DEventoTeclado &DatosTeclado)												{ };
		virtual void									Evento_Tecla(DEventoTeclado &DatosTeclado)														{ };

		virtual void								    Evento_FocoObtenido(HWND hWndUltimoFoco)														{ };
		virtual void								    Evento_FocoPerdido(HWND hWndNuevoFoco)															{ };

		virtual void                                    Evento_PintarSubItem(HDC hDC, const LONGLONG NumItem, const LONGLONG NumSubItem, RECT *Espacio) { };

		BOOL                                            MostrarSeleccion;
		BOOL                                            MultiSeleccion;

		LRESULT CALLBACK								GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

		DhWnd_Fuente                                    Fuente;

	  protected:
														// Eventos internos
		inline void                                    _Evento_Pintar(void);

		void										   _Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam);
		void										   _Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam);
		void										   _Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam);
		void                                           _Evento_MouseRueda(WPARAM wParam, LPARAM lParam);
		void										   _Evento_MouseSaliendo(void);
		void										   _Evento_MouseDobleClick(const int Boton, WPARAM wParam, LPARAM lParam);

		void                                           _Evento_TeclaPresionada(WPARAM wParam, LPARAM lParam);
		void                                           _Evento_TeclaSoltada(WPARAM wParam, LPARAM lParam);
		void										   _Evento_Tecla(WPARAM wParam, LPARAM lParam);

		void										   _Evento_FocoObtenido(HWND hWndUltimoFoco);
		void										   _Evento_FocoPerdido(HWND hWndNuevoFoco);


		// Teclas especiales
		void										   _Tecla_CursorArriba(DEventoTeclado &DatosTeclado);
		void										   _Tecla_CursorAbajo(DEventoTeclado &DatosTeclado);
		void										   _Tecla_Inicio(DEventoTeclado &DatosTeclado);
		void										   _Tecla_Fin(DEventoTeclado &DatosTeclado);
		void										   _Tecla_AvPag(DEventoTeclado &DatosTeclado);
		void										   _Tecla_RePag(DEventoTeclado &DatosTeclado);

														// Valor que determina si hay que recalcular los tama�os antes de pintar
		BOOL                                           _Repintar; 		

														// Funciones que calculan el espacio para el listview
		void										   _CalcularTotalEspacioVisible(RECT &RC);
		void										   _CalcularScrolls(void);
		void                                           _CalcularColumnas(void);
		void                                           _CalcularItemsPagina(const size_t TamPagina);
		void                                           _CalcularPintarIconos(void);

		DListaEx_Item                                 *_AgregarItem(DListaEx_Item *nItem, DListaIconos_Icono *nIcono, const INT_PTR PosicionFila, const TCHAR *nTxt, va_list Marker);
		std::vector<DListaEx_Item *>                   _Items;
		std::vector<DListaEx_Columna *>                _Columnas;

		LONGLONG                                       _ItemResaltado;
		LONGLONG	                                   _ItemUResaltado;
		LONGLONG                                       _SubItemResaltado; // es en n�mero de columa partiendo de la _FilaRsaltada
		LONGLONG									   _SubItemUResaltado;
		LONGLONG			                           _ItemPresionado;
		LONGLONG			                           _SubItemPresionado;
		LONGLONG			                           _ItemMarcado;
		LONGLONG					                   _ItemShift;
														// Diferencia de pixeles inicial entre la _FilaInicioPagina y el inicio de la p�gina visible
		LONG                                           _ItemPaginaVDif;
														// Diferencia de pixeles entre .... 
		LONG                                           _ItemPaginaHDif;

		LONGLONG                                       _ItemPaginaInicio;
		LONGLONG                                       _ItemPaginaFin;

		size_t                                         _TotalAnchoVisible;
		size_t										   _TotalAltoVisible;

		void										   _CrearBufferItem(const int nAncho, const int nAlto);
														// Buffer DC permanente para pintar UNA fila
		HDC                                            _BufferItem;
		HBITMAP                                        _BufferItemBmp;
		HBITMAP                                        _BufferItemBmpViejo;
		HFONT                                          _BufferItemFuenteViejo;

		BOOL                                           _PintarIconos;
/*		int                                            _MouseX;
		int                                            _MouseY;*/

		friend class DListaEx_Columna;
		friend class DListaEx_Item;
		friend class DListaEx_SubItem;

		friend class DDesplegableEx;

	};

}
#endif