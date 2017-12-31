#ifndef DLISTAEX_ITEM
#define DLISTAEX_ITEM

#include "DListaEx_SubItem.h"
#include "DListaIconos_Icono.h"

namespace DWL {
	class DListaEx;

	/* Clase que engloba un item de la lista (puede tener varias columnas) 
		NOTA : Cualquier cambio que se realize en un DListaEx_Item necesitara una llamada a la función Repintar() o a la función PintarItem() para actualizar los cambios */
	class DListaEx_Item	{
	  public:
											DListaEx_Item(void) : _Icono(NULL), Seleccionado(FALSE)  { };
//											DListaEx_Item(const TCHAR *nTexto, DListaIconos_Icono *nIcono);
											//DListaEx_Item(std::wstring &nTexto, DListaIconos_Icono *nIcono);
										   ~DListaEx_Item(void) { 
											   for (size_t i = 0; i < _SubItems.size(); i++) {
												   delete _SubItems[i];
											   }
											   _SubItems.resize(0);
										    };
										   /* Función que devuelve una referencia al std::wstring que contiene el texto del subitem especificado */
		inline std::wstring                &Texto(const size_t nSubItem = 0) {
												return _SubItems[nSubItem]->Texto;
											};

/*		inline DListaEx_SubItem		       *SubItem(const size_t nPos) {
											   return _SubItems[nPos];
										    };*/

		BOOL							    Seleccionado;
	  protected : 
		std::vector<DListaEx_SubItem *>    _SubItems;
		DListaIconos_Icono			      *_Icono;

		friend class DListaEx;
	};
	
}
#endif

