#ifndef DLISTAEX_ITEM
#define DLISTAEX_ITEM

#include "DListaEx_SubItem.h"
#include "DListaIconos_Icono.h"

namespace DWL {

	#define DLISTAEX_PADDING				2 // Espacio entre el marco de la selecci�n y el texto (tambien se utiliza para espaciar el expansor, el icono y el texto horizontalmente)
	#define DLISTAEX_MARGEN_Y_SELECCION		1 // Espacio entre la Y inicial / final del texto y el marco de seleccion
	#define DLISTAEX_TAMICONO				16 // Tama�o del icono

	class DListaEx;

	/* Clase que engloba un item de la lista (puede tener varias columnas) 
		NOTA : Cualquier cambio que se realize en un DListaEx_Item necesitara una llamada a la funci�n Repintar() o a la funci�n PintarItem() para actualizar los cambios */
	class DListaEx_Item {
	  public:
											DListaEx_Item(void) : _Icono(NULL), Seleccionado(FALSE)  { };
										   ~DListaEx_Item(void);
										    // Funci�n que devuelve una referencia al std::wstring que contiene el texto del subitem especificado 
		inline std::wstring                &Texto(const size_t nSubItem = 0) {
												return _SubItems[nSubItem]->Texto;
											};		
										    // Funci�n que devuelve una referencia al std::wstring que contiene el texto del subitem especificado 
		inline std::wstring                &Texto(const size_t nSubItem, std::wstring &nTexto) {
												_SubItems[nSubItem]->Texto = nTexto;
												return _SubItems[nSubItem]->Texto;
											};		

		void                                Icono(const int nIconoRecursos);

		BOOL							    Seleccionado;
	  protected : 
		std::vector<DListaEx_SubItem *>    _SubItems;
		DListaIconos_Icono			      *_Icono;

		friend class DListaEx;
	};
	
}
#endif

