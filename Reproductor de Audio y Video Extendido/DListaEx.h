#ifndef DLISTAEX_H
#define DLISTAEX_H

#include "DControlEx.h"
#include "DListaEx_Columna.h"
#include "DListaEx_Item.h"

namespace DWL {

	#define DLISTAEX_MOSTRARDEBUG	TRUE

	#ifdef _WIN64
		#define DLISTAEX_POSICIONFILA_INICIO	0
		#define DLISTAEX_POSICIONFILA_FIN		_UI64_MAX - 1
		#define DLISTAEX_POSICIONFILA_ORDENADO	_UI64_MAX - 2
	#else
		#define DLISTAEX_POSICIONFILA_INICIO	0
		#define DLISTAEX_POSICIONFILA_FIN		_UI32_MAX - 1
		#define DLISTAEX_POSICIONFILA_ORDENADO	_UI32_MAX - 2
	#endif

	/* Estados del mouse que puede tener un item */
	enum DListaEx_Item_EstadoMouse {
		DListaEx_Item_EstadoMouse_Normal		= 0,
		DListaEx_Item_EstadoMouse_Resaltado		= 1,
		DListaEx_Item_EstadoMouse_Presionado	= 2
	};


	class DListaEx : public virtual DBarraScrollEx {
	  public:
														DListaEx(void);
		                                               ~DListaEx(void);
														// Crea el control ListaEx
		HWND											CrearListaEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

		DListaEx_Columna							   *AgregarColumna(const int nAncho = DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion nAlineacion = DListaEx_Columna_Alineacion_Izquierda);

//		void                                            EliminarColumna(const size_t nPosColumna);
		void											EliminarTodasLasColumnas(void);

														// Agrega una fila personalizada (por defecto es del tipo DListaEx_Item)
		template <class TFila = DListaEx_Item> TFila   *AgregarItem(const int nIcono = NULL, const size_t PosicionFila = DLISTAEX_POSICIONFILA_FIN, const TCHAR *nTxt = NULL, ...) {
															TFila  *nFila = new TFila();
															va_list Marker;
															va_start(Marker, nTxt);
															_AgregarItem(nIcono, PosicionFila, nTxt, Marker);
															va_end(Marker);
															return nFila;
														};

		void											EliminarItem(const size_t ePosItem);
		void											EliminarItem(DListaEx_Item *eItem);
		const UINT										EliminarItemsSeleccionados(void);
		void											EliminarTodosLosItems(void);
		inline DListaEx_Item                           *Item(const size_t iPos) { return _Items[iPos]; }
														// Devuelve la posición del DListaEx_Item que está dentro de esta lista
		const size_t                                    PosItem(DListaEx_Item *pItem);


		void											DesSeleccionarTodo(void);
		void											MostrarItem(const size_t iPos);
		void											MostrarItem(DListaEx_Item *eItem);

		const BOOL                                      ObtenerRectaItem(const size_t iPos, RECT &rRecta);
		inline const BOOL                               ObtenerRectaItem(DListaEx_Item *eItem, RECT &rRecta) { return ObtenerRectaItem(PosItem(eItem), rRecta); }

		inline const size_t								TotalItems(void) { return _Items.size(); }
		void											TotalItemsSeleccionados(void); // poc util...

		inline const size_t                             TotalColumnas(void) { return _Columnas.size();  }
		inline DListaEx_Columna                        *Columna(const size_t cPos) { return _Columnas[cPos];  }

		void											Pintar(HDC hDC);
		void											PintarItem(HDC hDC, const size_t pPosItem, RECT &Espacio);

		const size_t									HitTest(const int cX, const int cY);

		void											Scrolls_EventoCambioPosicion(void);

		// Eventos virtuales
		virtual void          							Evento_MouseEntrando(void)																	{ };
		virtual void									Evento_MouseSaliendo(void)																	{ };
		virtual void									Evento_MouseMovimiento(const int cX, const int cY, const UINT Param)						{ };
		virtual void									Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param)		{ };
		virtual void									Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param)			{ };
		virtual void                                    Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey)		{ };
		virtual void									Evento_MouseDobleClick(const UINT Boton, const int cX, const int cY, const UINT Param)		{ };


		virtual void                                    Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params)		{ };
		virtual void                                    Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params)			{ };
		virtual void									Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param)					{ };


		LRESULT CALLBACK								GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
														// Eventos internos
		inline void                                    _Evento_Pintar(void);

		void										   _Evento_MouseMovimiento(const int cX, const int cY, const UINT Param);
		void										   _Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void										   _Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void                                           _Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey);
		void										   _Evento_MouseSaliendo(void);
		void										   _Evento_MouseDobleClick(const UINT Boton, const int cX, const int cY, const UINT Param);

		void                                           _Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void                                           _Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void										   _Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param);

		// Teclas especiales
		void										   _Tecla_CursorArriba(void);
		void										   _Tecla_CursorAbajo(void);
		void										   _Tecla_Inicio(void);
		void										   _Tecla_Fin(void);
		void										   _Tecla_AvPag(void);
		void										   _Tecla_RePag(void);

														// Valor que determina si hay que recalcular los tamaños antes de pintar
		BOOL                                           _CalcularValores; 		

														// Funciones que calculan el espacio para el listview
		void										   _CalcularTotalEspacioVisible(RECT &RC);
		void										   _CalcularScrolls(void);
		void                                           _CalcularColumnas(void);
		void                                           _CalcularItemsPagina(const size_t TamPagina);

		DListaEx_Item                                 *_AgregarItem(const int nIcono, const size_t PosicionFila, const TCHAR *nTxt, va_list Marker);
		std::vector<DListaEx_Item *>                   _Items;
		std::vector<DListaEx_Columna *>                _Columnas;

		size_t                                         _ItemResaltado;
//		size_t                                         _SubItemResaltado; // es en número de columa partiendo de la _FilaRsaltada
		size_t		                                   _ItemUResaltado;
		size_t				                           _ItemPresionado;
		size_t				                           _ItemMarcado;
		size_t						                   _ItemShift;
														// Diferencia de pixeles inicial entre la _FilaInicioPagina y el inicio de la página visible
		LONG                                           _ItemPaginaVDif;
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

		DhWnd_Fuente			                       _Fuente;
//		bool										   _Teclado[256];

		friend class DListaEx_Columna;
		friend class DListaEx_Item;
		friend class DListaEx_SubItem;
	};

}
#endif