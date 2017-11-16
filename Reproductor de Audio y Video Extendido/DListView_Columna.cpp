#include "stdafx.h"
#include "DListView.h"

namespace DWL {
    
    //! Constructor
    /*! Constructor por defecto.
            \fn			DListView_Columna(void);
            \return     No devuelve nada
    */
    DListView_Columna::DListView_Columna(void) : _LV(NULL), _TipoEdicion(DEnum_ListView_TipoEdicion_SinEdicion) {
    };

    //! Constructor
    /*! Constructor asignador.
            \fn			DListView_Columna(DListView *LV, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);
            \param[in]  LV           : Puntero de la clase DListView que hace de padre para esta columna
            \param[in]  nTipoEdicion : Tipo de edición que se utilizara por defecto en todos los items/subitems pertenecientes a esta columna.
            \return     No devuelve nada
    */
    DListView_Columna::DListView_Columna(DListView *LV, const DEnum_ListView_TipoEdicion nTipoEdicion) : _LV(LV), _TipoEdicion(nTipoEdicion) {
	}


    //! Función que asigna el texto para esta columna.
    /*! Esta función se usa para asignar el texto de esta columna.
            \fn         void AsignarTexto(const TCHAR *nTexto);
            \param[in]  nTexto : Nuevo texto para la columna.
            \return     No devuelve nada.
            \remarks    Esta función soporta strings con un máximo de 256 caracteres.
    */
	void DListView_Columna::AsignarTexto(const TCHAR *nTexto) {
		LVCOLUMN Columna;
		TCHAR    Tmp[256];
		wcscpy_s(Tmp, 256, nTexto);
		Columna.mask = LVCF_TEXT;
		Columna.pszText = Tmp; 
		Columna.cchTextMax = 256; 
		unsigned int i;
		for (i = 0; i < _LV->_Columnas.size(); i++) { 
			if (_LV->_Columnas[i] == this) {   break;  }
		} 
		Columna.iSubItem = i; 
		SendMessage(_LV->hWnd() ,LVM_SETCOLUMN ,0 ,(LPARAM)&Columna); 
	}


    //! Función que obtiene el texto para esta columna.
    /*! Esta función se usa para obtener el texto de esta columna.
            \fn         void ObtenerTexto(DString &nTexto);
            \param[out] nTexto : Clase DString donde se almacenara el texto de esta columna.
            \return     No devuelve nada.
            \remarks    Esta función soporta strings con un máximo de 256 caracteres.
    */
	void DListView_Columna::ObtenerTexto(std::wstring &nTexto) { 
		LVCOLUMN Columna; 
		TCHAR    Tmp[256]; 
		Columna.mask = LVCF_TEXT;
		Columna.pszText = Tmp;
		Columna.cchTextMax = 256;
		unsigned int i;
		for (i = 0; i < _LV->_Columnas.size(); i++) { 
			if (_LV->_Columnas[i] == this) {   break;  }
		} 
		Columna.iSubItem = i; 
		SendMessage(_LV->hWnd(), LVM_GETCOLUMN, 0, (LPARAM)&Columna);
        nTexto = Columna.pszText; 
	}


    //! Función que asigna el ancho en pixeles para esta columna.
    /*! Esta función se usa para asignar el ancho en pixeles de esta columna.
            \fn         void Ancho(const int nAncho);
            \param[in]  nAncho : Nuevo ancho en pixeles para la columna.
            \return     No devuelve nada.
    */
	void DListView_Columna::Ancho(const int nAncho) {
		LVCOLUMN Columna; 
		Columna.mask = LVCF_WIDTH; 
		unsigned int i;
		for (i = 0; i < _LV->_Columnas.size(); i++) {
			if (_LV->_Columnas[i] == this) {   break;  } 
		} 
		Columna.iSubItem = i; 
		Columna.cx = nAncho; 
		SendMessage(_LV->hWnd(), LVM_SETCOLUMN, i, (LPARAM)&Columna);
	}

    //! Función que obtiene el ancho en pixeles para esta columna.
    /*! Esta función se usa para obtener el ancho en pixeles de esta columna.
            \fn         const int Ancho(void);
            \return     Devuelve el ancho en pixeles de esta columna.
    */
	const int DListView_Columna::Ancho(void) { 
		LVCOLUMN Columna; 
		Columna.mask = LVCF_WIDTH; 
		unsigned int i;
		for (i = 0; i < _LV->_Columnas.size(); i++) { 
			if (_LV->_Columnas[i] == this) {   break;  }
		} 
		Columna.iSubItem = i;
		SendMessage(_LV->hWnd(), LVM_GETCOLUMN, 0, (LPARAM)&Columna);
		return Columna.cx; 
	}

    //! Función que asigna la alineación del texto para los subitems de esta columna.
    /*! Esta función se usa para asignar la alineación del texto para los subitems de esta columna.
            \fn         void AlineacionTexto(const DEnum_ListView_AlineacionTexto nAlineacion);
            \param[in]  nAlineacion : Nueva alineación del texto que puede ser : DEnum_ListView_AlineacionTexto_Izquierda, DEnum_ListView_AlineacionTexto_Derecha, DEnum_ListView_AlineacionTexto_Centrado.
            \return     No devuelve nada.
    */
    void DListView_Columna::AlineacionTexto(const DWL::DEnum_ListView_AlineacionTexto nAlineacion) {
		LVCOLUMN Columna; 
		Columna.mask = LVCF_FMT; 
		unsigned int i;
		for (i = 0; i < _LV->_Columnas.size(); i++) { 
			if (_LV->_Columnas[i] == this) {   break;  }
		} 
		Columna.iSubItem = i;
        Columna.fmt = static_cast<int>(nAlineacion);
		SendMessage(_LV->hWnd(), LVM_SETCOLUMN, 0, (LPARAM)&Columna);
    }

    //! Función que obtiene la alineación del texto para los subitems de esta columna.
    /*! Esta función se usa para obtener la alineación del texto para los subitems de esta columna.
            \fn         const DEnum_ListView_AlineacionTexto AlineacionTexto(void);
            \return     Devuelve la alineacion del texto que puede ser : DEnum_ListView_AlineacionTexto_Izquierda, DEnum_ListView_AlineacionTexto_Derecha, DEnum_ListView_AlineacionTexto_Centrado.
    */
    const DEnum_ListView_AlineacionTexto DListView_Columna::AlineacionTexto(void) {
		LVCOLUMN Columna; 
		Columna.mask = LVCF_FMT; 
		unsigned int i;
		for (i = 0; i < _LV->_Columnas.size(); i++) { 
			if (_LV->_Columnas[i] == this) {   break;  }
		} 
		Columna.iSubItem = i;
		SendMessage(_LV->hWnd(), LVM_GETCOLUMN, 0, (LPARAM)&Columna);
		return static_cast<DEnum_ListView_AlineacionTexto>(Columna.fmt);
    };

};
