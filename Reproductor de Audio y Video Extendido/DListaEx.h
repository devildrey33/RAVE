#ifndef DLISTAEX_H
#define DLISTAEX_H

#include "DControlEx.h"
#include "DListaEx_Columna.h"
#include "DListaEx_Item.h"

namespace DWL {

	#define DLISTAEX_MOSTRARDEBUG	FALSE

	#ifdef _WIN64
		#define DLISTAEX_POSICION_INICIO	0
		#define DLISTAEX_POSICION_FIN		_UI64_MAX - 1
		#define DLISTAEX_POSICION_ORDENADO	_UI64_MAX - 2
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
		HWND											CrearListaEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);
		const BOOL										Destruir(void);

		DListaEx_Columna							   *AgregarColumna(const int nAncho = DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion nAlineacion = DListaEx_Columna_Alineacion_Izquierda);

//		void                                            EliminarColumna(const size_t nPosColumna);
		void											EliminarTodasLasColumnas(void);

														// Agrega un item personalizado (por defecto es del tipo DListaEx_Item) para iconos de los recursos
		template <class TItem = DListaEx_Item> TItem   *AgregarItem(const int nIconoRecursos = NULL, const size_t PosicionFila = DLISTAEX_POSICION_FIN, const TCHAR *nTxt = NULL, ...) {
															TItem  *nItem = new TItem();
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO);
															va_list Marker;
															va_start(Marker, nTxt);
															_AgregarItem(nItem, TmpIco, PosicionFila, nTxt, Marker);
															va_end(Marker);
															return nItem;
														};

														// Agrega un item personalizado (por defecto es del tipo DListaEx_Item) para iconos del sistema
		template <class TItem = DListaEx_Item> TItem   *AgregarItem(const GUID nIconoKnownFolder, const size_t PosicionFila = DLISTAEX_POSICION_FIN, const TCHAR *nTxt = NULL, ...) {
															TItem  *nItem = new TItem();
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoKnownFolder(nIconoKnownFolder, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO);
															va_list Marker;
															va_start(Marker, nTxt);
															_AgregarItem(nItem, TmpIco, PosicionFila, nTxt, Marker);
															va_end(Marker);
															return nItem;
														};

														// Agrega un item personalizado (por defecto es del tipo DListaEx_Item) para iconos especificos de un path
		template <class TItem = DListaEx_Item> TItem   *AgregarItem(const wchar_t *nPathIcono, const int nPosIcono = 0, const size_t PosicionFila = DLISTAEX_POSICION_FIN, const TCHAR *nTxt = NULL, ...) {
															TItem  *nItem = new TItem();
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoExterno(nPathIcono, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO, nPosIcono);
															va_list Marker;
															va_start(Marker, nTxt);
															_AgregarItem(nItem, TmpIco, PosicionFila, nTxt, Marker);
															va_end(Marker);
															return nItem;
														};

		void											EliminarItem(const size_t ePosItem);
		void											EliminarItem(DListaEx_Item *eItem);
		const UINT										EliminarItemsSeleccionados(void);
		void											EliminarTodosLosItems(void);
														// Acceso a los DListaEx_Item
		inline DListaEx_Item                           *Item(const size_t iPos) { if (iPos == -1)            { return NULL; }	return _Items[iPos]; }
		inline DListaEx_Item                           *ItemResaltado(void)		{ if (_ItemResaltado == -1)  { return NULL; }	return _Items[_ItemResaltado]; };
		inline DListaEx_Item                           *ItemPresionado(void)	{ if (_ItemPresionado == -1) { return NULL; }	return _Items[_ItemPresionado]; };
		inline DListaEx_Item                           *ItemMarcado(void)		{ if (_ItemMarcado == -1)    { return NULL; }	return _Items[_ItemMarcado]; };
		void											ItemMarcado(DListaEx_Item *NuevoItemMarcado, const BOOL nRepintar = FALSE);
														// Devuelve la posición del DListaEx_Item que está dentro de esta lista
		const size_t                                    ItemPos(DListaEx_Item *pItem);		
		virtual void									Repintar(void);
		void											DesSeleccionarTodo(void);
		void											MostrarItem(const size_t iPos);
		void											MostrarItem(DListaEx_Item *eItem);

		const BOOL                                      ObtenerRectaItem(const size_t iPos, RECT &rRecta);
		inline const BOOL                               ObtenerRectaItem(DListaEx_Item *eItem, RECT &rRecta) { return ObtenerRectaItem(ItemPos(eItem), rRecta); }

		inline const size_t								TotalItems(void) { return _Items.size(); }
		void											TotalItemsSeleccionados(void); // poc util...

		inline const size_t                             TotalColumnas(void) { return _Columnas.size();  }
		inline DListaEx_Columna                        *Columna(const size_t cPos) { return _Columnas[cPos];  }

		void											Pintar(HDC hDC);
		void											PintarItem(HDC hDC, const size_t pPosItem, RECT &Espacio);

		const size_t									HitTest(const int cX, const int cY, size_t *PosSubItem = NULL);

		void											Scrolls_EventoCambioPosicion(void);

														// Eventos virtuales
		virtual void          							Evento_MouseEntrando(void)																	{ };
		virtual void									Evento_MouseSaliendo(void)																	{ };
		virtual void									Evento_MouseMovimiento(DEventoMouse &DatosMouse)											{ };
		virtual void									Evento_MousePresionado(DEventoMouse &DatosMouse)											{ };
		virtual void									Evento_MouseSoltado(DEventoMouse &DatosMouse)												{ };
		virtual void                                    Evento_MouseRueda(DEventoMouseRueda &DatosMouse)											{ };
		virtual void									Evento_MouseDobleClick(DEventoMouse &DatosMouse)											{ };


		virtual void                                    Evento_TeclaPresionada(DEventoTeclado &DatosTeclado)										{ };
		virtual void                                    Evento_TeclaSoltada(DEventoTeclado &DatosTeclado)											{ };
		virtual void									Evento_Tecla(DEventoTeclado &DatosTeclado)													{ };

		virtual void								    Evento_FocoObtenido(HWND hWndUltimoFoco)													{ };
		virtual void								    Evento_FocoPerdido(HWND hWndNuevoFoco)														{ };

		virtual void                                    Evento_PintarSubItem(HDC hDC, const size_t NumItem, const size_t NumSubItem, RECT *Espacio) { };

		BOOL                                            MostrarSeleccion;

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
		void										   _Tecla_CursorArriba(void);
		void										   _Tecla_CursorAbajo(void);
		void										   _Tecla_Inicio(void);
		void										   _Tecla_Fin(void);
		void										   _Tecla_AvPag(void);
		void										   _Tecla_RePag(void);

														// Valor que determina si hay que recalcular los tamaños antes de pintar
		BOOL                                           _Repintar; 		

														// Funciones que calculan el espacio para el listview
		void										   _CalcularTotalEspacioVisible(RECT &RC);
		void										   _CalcularScrolls(void);
		void                                           _CalcularColumnas(void);
		void                                           _CalcularItemsPagina(const size_t TamPagina);

		DListaEx_Item                                 *_AgregarItem(DListaEx_Item *nItem, DListaIconos_Icono *nIcono, const size_t PosicionFila, const TCHAR *nTxt, va_list Marker);
		std::vector<DListaEx_Item *>                   _Items;
		std::vector<DListaEx_Columna *>                _Columnas;

		size_t                                         _ItemResaltado;
		size_t		                                   _ItemUResaltado;
		size_t                                         _SubItemResaltado; // es en número de columa partiendo de la _FilaRsaltada
		size_t										   _SubItemUResaltado;
		size_t				                           _ItemPresionado;
		size_t				                           _SubItemPresionado;
		size_t				                           _ItemMarcado;
		size_t						                   _ItemShift;
														// Diferencia de pixeles inicial entre la _FilaInicioPagina y el inicio de la página visible
		LONG                                           _ItemPaginaVDif;
														// Diferencia de pixeles entre .... 
		LONG                                           _ItemPaginaHDif;

		size_t                                         _ItemPaginaInicio;
		size_t                                         _ItemPaginaFin;

		size_t                                         _TotalAnchoVisible;
		size_t										   _TotalAltoVisible;

		void										   _CrearBufferItem(const int nAncho, const int nAlto);
														// Buffer DC permanente para pintar UNA fila
		HDC                                            _BufferItem;
		HBITMAP                                        _BufferItemBmp;
		HBITMAP                                        _BufferItemBmpViejo;
		HFONT                                          _BufferItemFuenteViejo;

/*		int                                            _MouseX;
		int                                            _MouseY;*/

		friend class DListaEx_Columna;
		friend class DListaEx_Item;
		friend class DListaEx_SubItem;

		friend class DDesplegableEx;

	};

}
#endif