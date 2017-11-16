#include "stdafx.h"
#include "DListView.h"

namespace DWL {


    //! Constructor
    /*! Constructor por defecto.
            \fn			DListView_Item(void);
            \return     No devuelve nada
    */
    DListView_Item::DListView_Item(void) : _LV(NULL) { 
	}


    //! Constructor
    /*! Constructor que asigna el padre.
            \fn			DListView_Item(DListView *LV);
            \return     No devuelve nada
    */
	DListView_Item::DListView_Item(DListView *LV) {
        _AsignarNuevo(LV);
    }

    //! Función que asigna el ListView padre para este item.
    /*! Esta función se usa para asignar el ListView padre para este item.
            \fn         void HacerVisible(void);
            \return     No devuelve nada.
            \remarks    Esta funcion es de uso interno.
    */
	void DListView_Item::_AsignarNuevo(DListView *LV) {
		_LV = LV;
        if (_LV->_Columnas.size() == 0) {
            _SubItems.push_back(DListView_SubItem());
//            _SubItems.push_back(DListView_SubItem(_LV->ColorTexto(), _LV->ColorFondo(), _LV->Fuente()));
        }
        else {
            for (size_t i = 0; i < _LV->_Columnas.size(); i++) {
//                _SubItems.push_back(DListView_SubItem(_LV->ColorTexto(), _LV->ColorFondo(), _LV->Fuente(), _LV->_Columnas[i]->_TipoEdicion));
                _SubItems.push_back(DListView_SubItem(_LV->_Columnas[i]->_TipoEdicion));
    		}
        }
	}

    //! Función para obtener si el checkbox de un item esta marcado o no.
    /*! Esta función se usa para saber si el checkbox de un item esta marcado o no.
            \fn         void Marcar(const BOOL nMarcar);
            \return     Devuelve TRUE si el CheckBox esta marcado, FALSE en caso contrario.
            \remarks    Solo funciona con el estilo extendido LVS_EX_CHECKBOXES.
    */
    const BOOL DListView_Item::Marcado(void) {
        UINT i = 0;
		for (i = 0; i < _LV->_Items.size(); i++) { 
			if (_LV->_Items[i] == this) break; 
		}
        return ListView_GetCheckState(_LV->hWnd(), i);
    }

    //! Función para marcar/desmarcar el checkbox de un item.
    /*! Esta función se usa para marcar/desmarcar el checkbox de un item.
            \fn         void Marcar(const BOOL nMarcar);
            \param[in]  nMarcar : TRUE para marcar el CheckBox, FALSE para desmarcarlo.
            \return     No devuelve nada.
            \remarks    Solo funciona con el estilo extendido LVS_EX_CHECKBOXES.
    */
    void DListView_Item::Marcar(const BOOL nMarcar) {
        UINT i = 0;
		for (i = 0; i < _LV->_Items.size(); i++) { 
			if (_LV->_Items[i] == this) break; 
		}
        ListView_SetCheckState(_LV->hWnd(), i, nMarcar);
    }

    //! Función para asignar el texto a un item / subitem.
    /*! Esta función se usa para modificar el texto de un item o de un subitem.
            \fn         void AsignarTexto(const UINT nSubItem, const TCHAR *nTexto);
            \param[in]  nSubItem : Posición del subitem a editar, 0 para editar el texto del item.
            \param[in]  nTexto   : Nuevo texto que adoptara el Item/SubItem.
            \return     No devuelve nada.
            \remarks    El texto asignado esta limitado a 256 caracteres.
    */
	void DListView_Item::AsignarTexto(const UINT nSubItem, const TCHAR *nTexto) {
		TCHAR   Text[256];
		LV_ITEM LVItem; 
		unsigned int i;
		wcscpy_s(Text, 256, nTexto);
		LVItem.mask = LVIF_TEXT;
		LVItem.pszText = Text;
		LVItem.cchTextMax = static_cast<int>(wcslen(Text)); 
		LVItem.iSubItem = nSubItem; 
		for (i = 0; i < _LV->_Items.size(); i++) { 
			if (_LV->_Items[i] == this) {
				break; 
			} 
		}
		LVItem.iItem = static_cast<int>(i);
		SendMessage(_LV->hWnd(), LVM_SETITEM, 0, (LPARAM)&LVItem);
	}


    //! Función para obtener el texto a un item / subitem.
    /*! Esta función se usa para obtener el texto de un item o de un subitem.
            \fn         void ObtenerTexto(const UINT nSubItem, DString &Texto);
            \param[in]  nSubItem : Posición del subitem a obtener, 0 para obtener el texto del item.
            \param[out] nTexto   : Texto actual del item/subitem especificado
            \return     No devuelve nada.
            \remarks    El texto obtenido esta limitado a 256 caracteres.
    */
	void DListView_Item::ObtenerTexto(const UINT nSubItem, std::wstring &nTexto) {
		TCHAR    Text[256];
		unsigned int i;
		LV_ITEM LVItem; 
		LVItem.mask = LVIF_TEXT;
		LVItem.pszText = Text; 
		LVItem.cchTextMax = 256; 
		LVItem.iSubItem = nSubItem;
		for (i = 0; i < _LV->_Items.size(); i++) {
			if (_LV->_Items[i] == this) { 
				break; 
			} 
		}
		LVItem.iItem = static_cast<int>(i);
		SendMessage(_LV->hWnd(), LVM_GETITEM, 0, (LPARAM)&LVItem); 
        nTexto = Text;
	}


    //! Función para asignar el icono para el item.
    /*! Esta función se usa para asignar un icono del imagelist a este item.
            \fn         void Icono(const int nIcono);
            \param[in]  nIcono : Posicion del icono dentro del imagelist.
            \return     No devuelve nada.
            \remarks    Solo funciona si se ha asignado un imagelist con el tamaño adecuado al ListView.
    */
	void DListView_Item::Icono(const int nIcono) { 
		LVITEM LVItem; 
		LVItem.mask = LVIF_IMAGE;
		unsigned int i;
		for (i = 0; i < _LV->_Items.size(); i++) {
			if (_LV->_Items[i] == this) { break; } 
		} 
		LVItem.iSubItem = 0; 
		LVItem.iItem = static_cast<int>(i);
		LVItem.iImage = nIcono;
		ListView_SetItem(_LV->hWnd(), &LVItem);
	}


    //! Función para obtener el icono para el item.
    /*! Esta función se usa para obtener el icono del imagelist que lleva este item.
            \fn         const int Icono(void);
            \return     Devuelve la posición del icono dentro del imagelist.
            \remarks    Solo funciona si se ha asignado un imagelist con el tamaño adecuado al ListView.
    */
	const int DListView_Item::Icono(void) { 
		LV_ITEM LVItem; 
		unsigned int i;
		LVItem.mask = LVIF_IMAGE;
		for (i = 0; i < _LV->_Items.size(); i++) {
			if (_LV->_Items[i] == this) { break; } 
		} 
		LVItem.iItem = static_cast<int>(i);
		ListView_GetItem(_LV->hWnd(), &LVItem);
		return LVItem.iImage; 
	}


    //! Función para asignar una fuente al item y todos sus subitems.
    /*! Esta función se usa para asignar una fuente a este item y todos sus subitems.
            \fn         void Fuente(HFONT nFuente);
            \return     No devuelve nada
            \remarks    Si quieres especificar la fuente de un SubItem en concreto puedes utilizar la función FuenteSubItem(). No borres la fuente hasta que no sea utilizada por el ListView.
    */
	void DListView_Item::Fuente(HFONT nFuente) { 
		for (size_t i = 0; i < _SubItems.size(); i++) _SubItems[i].Fuente = nFuente; 
        _LV->RepintarItem(this);
	}

    //! Función para asignar una fuente al item/subitem especificado.
    /*! Esta función se usa para asignar una fuente a este item y todos sus subitems.
            \fn         void FuenteSubItem(const UINT SubItem, HFONT nFuente);
            \param[in]  nSubItem : Posición del subitem en el que se quiere asignar la fuente. 0 para asignar la fuente del item.
            \param[in]  nFuente  : Fuente que utilizara el item/subitem especificado.
            \return     No devuelve nada
            \remarks    No borres la fuente hasta que no sea utilizada por el ListView.
    */
	void DListView_Item::FuenteSubItem(const UINT nSubItem, HFONT nFuente) { 
		_SubItems[nSubItem].Fuente = nFuente; 
	}

    //! Función que devuelve la fuente que utiliza el item/subitem especificado.
    /*! Esta función se usa para obtener la fuente que utiliza el item/subitem especificado.
            \fn         HFONT FuenteSubItem(const UINT nSubItem);
            \param[in]  nSubItem : Posición del subitem del que se quiere obtener la fuente. 0 para obtener la fuente del item.
            \return     Devuelve la fuente especificada.
            \remarks    Ni se te ocurra borrar la fuente mientras se esta utilizando por el ListView.....
    */
    HFONT DListView_Item::FuenteSubItem(const UINT nSubItem) { 
		return _SubItems[nSubItem].Fuente;
	}

    //! Función para asignar el color del texto al item y todos sus subitems.
    /*! Esta función se usa para asignar el color del texto a este item y todos sus subitems.
            \fn         void ColorTexto(COLORREF nColor);
            \param[in]  nColor : Color que se utilizara para mostrar este item y sus subitems
            \return     No devuelve nada
            \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color para un subitem especifico puedes utilizar la función ColorTextoSubItem().
    */
	void DListView_Item::ColorTexto(COLORREF nColor) {
        for (size_t i = 0; i < _SubItems.size(); i++) {
            if (_SubItems[i].ColorTexto == NULL) _SubItems[i].ColorTexto = new COLORREF;
            *_SubItems[i].ColorTexto = nColor; 
        }
        _LV->RepintarItem(this);
	}

    //! Función para asignar el color del texto al item/subitem especificado.
    /*! Esta función se usa para asignar el color del texto al item/subitem especificado.
            \fn         void ColorTextoSubItem(const UINT nSubItem, COLORREF nColor);
            \param[in]  nSubItem : Posición del item / subitem al que le quieres cambiar el color del texto.
            \param[in]  nColor   : Color que se utilizara para mostrar el item/subitem especificado.
            \return     No devuelve nada
            \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color todos los subitems puedes utilizar la función ColorTexto().
    */
    void DListView_Item::ColorTextoSubItem(const UINT nSubItem, COLORREF nColor) {
        if (_SubItems[nSubItem].ColorTexto == NULL) _SubItems[nSubItem].ColorTexto = new COLORREF;
		*_SubItems[nSubItem].ColorTexto = nColor; 
	}


    //! Función que devuelve el color del texto para el item/subitem especificado.
    /*! Esta función se usa para obtener el color del texto que utiliza el item/subitem especificado.
            \fn         HFONT ColorTextoSubItem(const UINT nSubItem);
            \param[in]  nSubItem : Posición del subitem del que se quiere obtener el color. 0 para obtener el color del item.
            \return     Devuelve el color especificado.
            \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles.
    */
	COLORREF DListView_Item::ColorTextoSubItem(const UINT nSubItem) { 
		return *_SubItems[nSubItem].ColorTexto;
	}

    //! Función para asignar el color del texto seleccionado al item y todos sus subitems.
    /*! Esta función se usa para asignar el color del texto seleccionado a este item y todos sus subitems.
            \fn         void ColorTextoSeleccionado(COLORREF nColor);
            \param[in]  nColor : Color que se utilizara para mostrar el texto seleccionado de este item y sus subitems
            \return     No devuelve nada
            \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color para un subitem especifico puedes utilizar la función ColorFondoSubItem().
    */
	void DListView_Item::ColorTextoSeleccionado(COLORREF nColor) {
        for (size_t i = 0; i < _SubItems.size(); i++) {
            if (_SubItems[i].ColorTextoSeleccionado == NULL) _SubItems[i].ColorTextoSeleccionado = new COLORREF;
            *_SubItems[i].ColorTextoSeleccionado = nColor; 
        }
        _LV->RepintarItem(this);
	}

    //! Función para asignar el color del texto seleccionado al item/subitem especificado.
    /*! Esta función se usa para asignar el texto seleccionado del fondo al item/subitem especificado.
            \fn         void ColorTextoSeleccionadoSubItem(const UINT nSubItem, COLORREF nColor);
            \param[in]  nSubItem : Posición del item / subitem al que le quieres cambiar el color del texto seleccionado.
            \param[in]  nColor   : Color que se utilizara para mostrar el item/subitem especificado.
            \return     No devuelve nada
            \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color todos los subitems puedes utilizar la función ColorFondo().
    */
	void DListView_Item::ColorTextoSeleccionadoSubItem(const UINT nSubItem, COLORREF nColor) {
        if (_SubItems[nSubItem].ColorTextoSeleccionado == NULL) _SubItems[nSubItem].ColorTextoSeleccionado = new COLORREF;
		*_SubItems[nSubItem].ColorTextoSeleccionado = nColor; 
	}


    //! Función que devuelve el color del texto seleccionado para el item/subitem especificado.
    /*! Esta función se usa para obtener el color del texto seleccionado que utiliza el item/subitem especificado.
            \fn         HFONT ColorTextoSeleccionadoSubItem(const UINT nSubItem);
            \param[in]  nSubItem : Posición del subitem del que se quiere obtener el color. 0 para obtener el color del item.
            \return     Devuelve el color especificado.
            \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles.
    */
	COLORREF DListView_Item::ColorTextoSeleccionadoSubItem(const UINT nSubItem) { 
		return *_SubItems[nSubItem].ColorTextoSeleccionado;
	}


    //! Función que asigna el estado de selección para este item.
    /*! Esta función se usa para asignar el estado de selección para este item y todos sus subitems.
            \fn         void Seleccionado(const BOOL Sel);
            \param[in]  Sel : TRUE para seleccionar el item, FALSE para des-seleccionarlo.
            \return     No devuelve nada.
    */
	void DListView_Item::Seleccionado(const BOOL Sel) { 
		unsigned int i;
		for (i = 0; i < _LV->_Items.size(); i++) {
			if (_LV->_Items[i] == this) {   
				if (Sel == TRUE) { ListView_SetItemState(_LV->hWnd(), i, LVIS_SELECTED, LVIS_SELECTED); }
				else             { ListView_SetItemState(_LV->hWnd(), i, LVIS_SELECTED, 0); }
                return;
            }
		}
	}


    //! Función que obtiene el estado de selección para este item.
    /*! Esta función se usa para obtener el estado de selección para este item y todos sus subitems.
            \fn         const BOOL Seleccionado(void);
            \return     Devuelve TRUE si el item esta seleccionado, FALSE en caso contrario.
    */
	const BOOL DListView_Item::Seleccionado(void) { 
		unsigned int i;
		for (i = 0; i < _LV->_Items.size(); i++) { 
			if (_LV->_Items[i] == this) {   
				if ((ListView_GetItemState(_LV->hWnd(), i, LVIS_SELECTED) == LVIS_SELECTED)) { return TRUE; }
		        else                                                                        { return FALSE; }
            } 
		} 
        return FALSE;
	}


    //! Función que hace visible dentro del control este item.
    /*! Esta función se usa para mostrar este item en el rectangulo visible del control.
            \fn         void HacerVisible(void);
            \return     No devuelve nada.
    */
	void DListView_Item::HacerVisible(void) { 
		unsigned int i;
		for (i = 0; i < _LV->_Items.size(); i++) {
			if (_LV->_Items[i] == this) {   break;  } 
		} 
		ListView_EnsureVisible(_LV->hWnd(), i, false);
	}

    //! Función para asignar el tipo de edición que se utilizara en el item/subitem especificado.
    /*! Esta función se usa para asignar el tipo de edición que se utilizara en el item/subitem especificado.
            \fn         void TipoEdicion(const UINT nSubItem, const DEnum_ListView_TipoEdicion nTipoEdicion);
            \param[in]  nSubItem     : Posición del item/subitem al que queremos asignar el tipo de edición.
            \param[in]  nTipoEdicion : Tipo de edición que utilizara el item/subitem, que puede ser : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, DEnum_ListView_TipoEdicion_ComboStatic.
            \return     No devuelve nada.
    */
    void DListView_Item::TipoEdicion(const UINT SubItem, const DEnum_ListView_TipoEdicion nTipoEdicion) {
        _SubItems[SubItem].TipoEdicion = nTipoEdicion;
    }

    //! Función para obtener el tipo de edición que se utilizara en el item/subitem especificado.
    /*! Esta función se usa para obtener el tipo de edición que se utilizara en el item/subitem especificado.
            \fn         const DEnum_ListView_TipoEdicion    TipoEdicion(const UINT nSubItem);
            \param[in]  nSubItem     : Posición del item/subitem al que queremos asignar el tipo de edición.
            \return     Puede devolver uno de los siguientes valores : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, DEnum_ListView_TipoEdicion_ComboStatic.
    */
    const DEnum_ListView_TipoEdicion DListView_Item::TipoEdicion(const UINT SubItem) {
        return _SubItems[SubItem].TipoEdicion;
    }

};