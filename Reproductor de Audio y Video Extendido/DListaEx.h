#ifndef DLISTAEX_H
#define DLISTAEX_H

#include "DControlEx.h"
#include "DListaEx_Columna.h"
#include "DListaEx_Item.h"

namespace DWL {

	#ifdef _WIN64
		#define DLISTAEX_POSICIONNODO_INICIO	0
		#define DLISTAEX_POSICIONNODO_FIN		_UI64_MAX - 1
		#define DLISTAEX_POSICIONNODO_ORDENADO	_UI64_MAX - 2
	#else
		#define DLISTAEX_POSICIONNODO_INICIO	0
		#define DLISTAEX_POSICIONNODO_FIN		_UI32_MAX - 1
		#define DLISTAEX_POSICIONNODO_ORDENADO	_UI32_MAX - 2
	#endif




	class DListaEx : public virtual DBarraScrollEx {
	  public:
		DListaEx();
		~DListaEx();
														// Crea el control ListaEx
		HWND											CrearListaEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

		void											AgregarColumna(const int nAncho = DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion nAlineacion = DListaEx_Columna_Alineacion_Centrado);
		void                                            EliminarColumna(const size_t nPosColumna);
														// El control DListaEx debe tener mínimo siempre una columna (se eliminaran todos los datos de las otras columnas, pero siempre se mantendrá la primera)
		void											EliminarTodasLasColumnas(void);

		// Agrega un nodo personalizado (por defecto es del tipo DArbolEx_Nodo)
		template <class TItem = DListaEx_Item> TItem   *AgregarItem(const int nIcono = NULL, DhWnd_Fuente *nFuente = NULL, const size_t PosicionItem = DLISTAEX_POSICIONNODO_FIN, const TCHAR *nTxt = NULL, ...) {
															TItem *nItem = new TItem();
															va_list Marker;
															va_start(Marker, Txt);
															_AgregarItem(nIcono, nFuente, PosicionItem, nTxt, Marker);
															va_end(Marker);
															return nItem;
														};

		void											EliminarItem(const size_t PosItem);
		void											EliminarItem(DListaEx_Item *eItem);
		const UINT										EliminarItemsSeleccionados(void);
		void											EliminarTodosLosItems(void);
		inline DListaEx_Item                           *Item(const size_t iPos) { return _Items[iPos]; }

		void											DesSeleccionarTodo(void);
		void											MostrarItem(const size_t iPos);
		void											MostrarItem(DListaEx_Item *eItem);

		inline const size_t								TotalItems(void) { return _Items.size(); }
		void											TotalItemsSeleccionados(void); // poc util...

		inline const size_t                             TotalColumnas(void) { return _Columnas.size();  }
		inline DListaEx_Columna                        *Columna(const size_t cPos) { return _Columnas[cPos];  }

		void											Pintar(HDC hDC);
		void											PintarItem(HDC hDC, DListaEx_Item *nItem, RECT &Espacio);

		LRESULT CALLBACK								GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		DListaEx_Item                                 *_AgregarItem(const int nIcono, DhWnd_Fuente *nFuente, const size_t PosicionItem, const TCHAR *nTxt, va_list Marker);
		std::vector<DListaEx_Item *>                   _Items;
		std::vector<DListaEx_Columna *>                _Columnas;

		DListaEx_Item                                 *_ItemResaltado;
		size_t                                         _SubItemResaltado;
		DListaEx_Item                                 *_UItemResaltado;
		DListaEx_Item                                 *_ItemPresionado;
		DListaEx_Item                                 *_ItemShift;

		DhWnd_Fuente			                       _Fuente;
		bool										   _Teclado[256];

		friend class DListaEx_Columna;
		friend class DListaEx_Item;
		friend class DListaEx_SubItem;
	};

}
#endif