#include "stdafx.h"
#include "DListView.h"
//#include "DAplicacion.h"
#include "DImageList.h"
#include "DMouse.h"
#include "DMensajesWnd.h"

#ifndef DWL_MSGRESULT
    #define DWL_MSGRESULT 0
#endif

namespace DWL {


    //! Constructor
    /*! Constructor por defecto.
            \fn			DListView(void);
            \return     No devuelve nada
            \remarks    Este constructor hace una llamada a InitCommonControlsEx para registrar la clase ListView
    */
    DListView::DListView(void) : _hWndEdicion(NULL), _eItem(0), _eSubItem(0), _Editando(DEnum_ListView_TipoEdicion_SinEdicion), _GestorMensajesOriginalEdicion(NULL), _OrdenColumnas(NULL) {		
		static bool Cargado = false;
		if (Cargado == false) {
			INITCOMMONCONTROLSEX Comm;
			Comm.dwSize = sizeof(INITCOMMONCONTROLSEX);
			Comm.dwICC = ICC_LISTVIEW_CLASSES;
			InitCommonControlsEx(&Comm);
			Cargado = true;
		}
	};


    //! Destructor
    /*! Destructor.
            \fn		   ~DListView(void);
            \return     No devuelve nada
            \remarks    Este destructor elimina los items y las columnas de memoria.
    */
	DListView::~DListView(void) {
        CancelarEdicion();
		Destruir();
		size_t i;
		for (i = 0; i < _Columnas.size(); i++) {
			delete _Columnas[i];
		}
		for (i = 0; i < _Items.size(); i++) {
			delete _Items[i];
		}
        if (_OrdenColumnas != NULL) delete [] _OrdenColumnas;
	};

    //! Función para crear el ListView (CreateWindowEx).
    /*! Esta función se usa para crear un ListView dinamicamente.
            \fn         HWND Crear(DBaseWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos = NULL);
            \param[in]  hWndPadre           HWND de la ventana padre para el ListView.
            \param[in]  cX                  Coordenada X relativa a la ventana padre.
            \param[in]  cY                  Coordenada Y relativa a la ventana padre.
            \param[in]  cAncho              Ancho del ListView en pixeles.
            \param[in]  cAlto               Altura del ListView en pixeles
            \param[in]  cID                 ID para poder identificar el ListView en sus eventos.
            \param[in]  nEstilos            Estilos estandar para el ListView.
            \param[in]  nEstilosExtendidos  Estilos extendidos para el ListView. Estos estilos son los que se asignan con la macro ListView_SetExtendedListViewStyleEx.
            \return     Devuelve el HWND del ListView o NULL en caso de error.
    */
	HWND DListView::Crear(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos) {
		if (hWnd()) { Debug_Escribir(L"DListView::Crear() Error : ya se ha creado el listview\n"); return hWnd(); }

		//        if (nPadre == NULL) throw (DError(this, __PROTOTIPO_FUNCION__, DEnum_Error_BaseWndNULL)); 
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
		_hWnd = CreateWindowEx(NULL, WC_LISTVIEW, NULL, nEstilos, cX, cY, cAncho, cAlto, hWndPadre, reinterpret_cast<HMENU>(IntToPtr(cID)), GetModuleHandle(NULL), this); 
		_ConectarControl(cID, nPadre);
        ListView_SetExtendedListViewStyleEx(hWnd(), DWL_ESTILOS_EX_LISTVIEW, nEstilosExtendidos);
        return hWnd();
	};

	                          
    //! Función para asignar esta clase a un ListView de un dialogo.
    /*! Esta función se usa para asignar esta clase a un ListView de un dialogo.
            \fn         HWND Asignar(DBaseWnd *nPadre, const UINT cID);
            \param[in]  nPadre      Clase DBaseWnd que va a hacer de padre para el ListView
            \param[in]  cID         ID para poder identificar el ListView en sus eventos.
            \return     Devuelve el HWND del ListView o NULL en caso de error.
            \remarks    Esta función solo debe utilizarse si tenemos un ListView en un dialogo de los recursos.
    */
	HWND DListView::Asignar(DhWnd *nPadre, const UINT cID) {
//        if (nPadre == NULL) throw (DError(this, __PROTOTIPO_FUNCION__, DEnum_Error_BaseWndNULL)); 
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
		_hWnd = GetDlgItem(hWndPadre, cID);
		_ConectarControl(cID, nPadre); 
//		_Fuente = (HFONT)SendMessage(_hWnd, WM_GETFONT, 0, 0);
		return hWnd();
	};
	                          


    //! Función para agregar un item al ListView
    /*! Esta función se usa para agregar un item al ListView, se pueden añadir textos de los subitems si hay varias columnas.
            \fn DListView_Item *AgregarItem(const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...);
            \param[in]  nIcono : ID del icono dentro del ImageList asignado a este listview (si no hay ImageList puedes especificar cualquier valor)
            \param[in]  nPos   : Posicion donde se añadira el nuevo Item (-1 para añadirlo al final)
            \param[in]  nGrupo : Grupo al que pertenece el Item (-1 si no pertenece a ningun grupo, o no hay grupos)
            \param[in]  nTexto : Cadena de caracteres que contiene el texto para el Item.
            \param[in]  ...    : Parametros opcionales que deben ser tantas cadenas de caracteres como columnas - 1 tenga el ListView, que se refieren a los textos para los subitems.
            \return     Devuelve un puntero al DListView_Item creado.
    */
    DListView_Item *DListView::AgregarItem(const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...) {
		DListView_Item *NDItem = new DListView_Item(this);
        // Posición
        int     Pos;
        if (nPos == -1) Pos = static_cast<int>(_Items.size());
        else            Pos = nPos;
        // Grupo
        int Grupo;
        if (nGrupo == -1 ) Grupo = I_GROUPIDCALLBACK;
        else               Grupo = nGrupo;
        // Creo el item
		LV_ITEM LVItem;
		int     i = 0; 
		TCHAR   Text[256];
		va_list Marker;
		wcscpy_s(Text, 256, nTexto);
		LVItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_GROUPID | LVIF_COLUMNS; 
		LVItem.pszText = Text; 
		LVItem.cchTextMax = static_cast<int>(wcslen(Text));
		LVItem.iSubItem = 0; 
		LVItem.iImage = nIcono; 
		LVItem.iItem = Pos; 
        LVItem.iGroupId = Grupo;
        LVItem.cColumns = static_cast<int>(_Columnas.size()) - 1;

//        UINT *OrdenColumnas = new UINT[_Columnas.size() - 1];
//        for (i = 0; i < _Columnas.size(); i++) OrdenColumnas[i] = i;
        LVItem.puColumns = _OrdenColumnas;

		i = ListView_InsertItem(hWnd(), &LVItem);
        // Añado texto de los subitems
		if (_Columnas.size()) {
			va_start(Marker, nTexto);
			for (unsigned int n = 0; n < _Columnas.size() - 1; n++) {
				wcscpy_s(Text, 256, va_arg(Marker, const TCHAR *));
				ListView_SetItemText(hWnd(), i, n + 1, Text);
			}
			va_end(Marker);
		}
        // Añado el item al vector
        _Items.push_back(NDItem);

//        delete OrdenColumnas;
        return NDItem;
    };

    //! Función para agregar un item al ListView
    /*! Esta función se usa para agregar un item al ListView, se pueden añadir textos de los subitems si hay varias columnas.
            \fn DListView_Item *AgregarItem(DListView_Item *nItem, const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...);
            \param[in]  nItem  : Clase DListView_Item de la que partira este item
            \param[in]  nIcono : ID del icono dentro del ImageList asignado a este listview (si no hay ImageList puedes especificar cualquier valor)
            \param[in]  nPos   : Posicion donde se añadira el nuevo Item (-1 para añadirlo al final)
            \param[in]  nGrupo : Grupo al que pertenece el Item (-1 si no pertenece a ningun grupo, o no hay grupos)
            \param[in]  nTexto : Cadena de caracteres que contiene el texto para el Item.
            \param[in]  ...    : Parametros opcionales que deben ser tantas cadenas de caracteres como columnas - 1 tenga el ListView, que se refieren a los textos para los subitems.
            \return     Devuelve un puntero al DListView_Item creado.
            \remarks    Esta funcion se ha diseñado para que puedas crear una clase que herede de DListView_Item, en la que puedas añadir mas datos.
                        De esta forma al pasarle la clase heredada a esta funcion se añadira al vector como si fuenra una clase DListView_Item normal.
                        Cuando necesites acceder a los datos extras de tu clase heredada bastara con buscar el item deseado y hacerle un casting.
    */
    DListView_Item *DListView::AgregarItem(DListView_Item *nItem, const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...) {
        nItem->_AsignarNuevo(this); 
        // Posición
        int     Pos;
        if (nPos == -1) Pos = static_cast<int>(_Items.size());
        else            Pos = nPos;
        // Grupo
        int Grupo;
        if (nGrupo == -1 ) Grupo = I_GROUPIDCALLBACK;
        else               Grupo = nGrupo;

        // Creo el item
		LV_ITEM LVItem;
		int     i = 0; 
		TCHAR   Text[256];
		va_list Marker;
		wcscpy_s(Text, 256, nTexto);
		LVItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_GROUPID | LVIF_COLUMNS;
		LVItem.pszText = Text; 
		LVItem.cchTextMax = static_cast<int>(wcslen(Text));
		LVItem.iSubItem = 0; 
		LVItem.iImage = nIcono; 
		LVItem.iItem = Pos; 
        LVItem.iGroupId = Grupo;
		LVItem.cColumns = static_cast<int>(_Columnas.size()) - 1;
		LVItem.puColumns = _OrdenColumnas;

		i = ListView_InsertItem(hWnd(), &LVItem);
        // Añado texto de los subitems
		va_start(Marker, nTexto); 
		for (unsigned int n = 0; n < _Columnas.size() -1; n++) {  
			wcscpy_s(Text, 256, va_arg(Marker, const TCHAR *));
			ListView_SetItemText(hWnd(), i, n + 1, Text);
		} 
		va_end(Marker);
        // Añado el item al vector
        _Items.push_back(nItem);
        return nItem;
    };


    //! Función para agregar un grupo dentro del ListView.
    /*!	Esta función se usa para agregar un grupo dentro del ListView.
		    \fn			void AgregarGrupo(const TCHAR *nTexto, const int nID, const int nPos = -1);
            \param[in]  nTexto : Texto que se mostrara en el grupo.
            \param[in]  nID    : ID para el grupo que deberas especificar al agregar un item si lo quieres introducir dentro.
            \param[in]  nPos   : Posición donde se agregara el grupo (-1 para agregarlo al final)
		    \return		No devuelve nada.
    */
    void DListView::AgregarGrupo(const TCHAR *nTexto, const int nID, const int nPos) {
        LVGROUP Grupo;
        TCHAR Text[256];
		wcscpy_s(Text, 256, nTexto);
        Grupo.cbSize = sizeof(LVGROUP);
        Grupo.iGroupId = nID;
        Grupo.pszHeader = Text;
		Grupo.cchHeader = static_cast<int>(wcslen(Text));
        Grupo.mask = LVGF_HEADER | LVGF_GROUPID;
		ListView_InsertGroup(hWnd(), nPos, &Grupo);
    };                       


    //! Función para borrar un item del ListView especificando su posición.
    /*!	Esta función se usa para borrar un item del ListView especificando su posición.
		    \fn			void BorrarItem(const int nItem);
		    \param[in]	nItem : Posición del item que queremos eliminar dentro de este ListView.
		    \return		No devuelve nada.
            \remarks    Al borrar un item del ListView recibiremos una llamada en la función Evento_ListView_Item_Borrar de su ventana padre.
    */
	void DListView::BorrarItem(const int nItem) { 
		SendMessage(hWnd(), LVM_DELETEITEM, nItem, 0);
		delete _Items[nItem]; 
		_Items.erase(_Items.begin() + nItem);
	};
	                          

    //! Función para borrar todos los items seleccionados dentro del ListView.
    /*!	Esta función se usa para borrar todos los items seleccionados dentro del ListView.
		    \fn			void BorrarItemsSeleccionados(void);
		    \return		No devuelve nada.
            \remarks    Al borrar los items seleccionados del ListView recibiremos una llamada en la función Evento_ListView_Item_Borrar de su ventana padre, por cada item borrado.
    */
    void DListView::BorrarItemsSeleccionados(void) {
		for (int i = static_cast<int>(_Items.size()) - 1; i > -1; i--) {
            if (_Items[i]->Seleccionado() == TRUE) BorrarItem(i);
        }
    }


    //! Función para borrar todos los items dentro del ListView.
    /*!	Esta función se usa para borrar todos los items dentro del ListView.
		    \fn			void BorrarTodosLosItems(void);
		    \return		No devuelve nada.
            \remarks    Al borrar los todos items del ListView recibiremos una llamada en la función Evento_ListView_Item_Borrar de su ventana padre, por cada item borrado. Ademas tambien recibiremos una llamada a la función Evento_ListView_Item_BorrarTodos.
    */
	void DListView::BorrarTodosLosItems(void) { 
		SendMessage(hWnd(), LVM_DELETEALLITEMS, 0, 0);
		for (unsigned int i = 0; i < _Items.size(); i++) {
			delete _Items[i]; 
		} 
		_Items.resize(0); 
	};
	                          

    //! Función para agregar una columna al ListView.
    /*!	Esta función se usa para agregar una columna al ListView.
		    \fn			void AgregarColumna(const TCHAR *nTexto, const int cAncho, const DEnum_ListView_AlineacionTexto nAlineacion = DEnum_ListView_AlineacionTexto_Izquierda, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);
            \param[in]  nTexto       : Texto que se mostrara en la columna.
            \param[in]  cAncho       : Ancho en pixeles para la columna.
            \param[in]  nAlineacion  : Alineacion para los subitems de esta columna que puede ser : DEnum_ListView_AlineacionTexto_Izquierda, DEnum_ListView_AlineacionTexto_Derecha y DEnum_ListView_AlineacionTexto_Centrado
            \param[in]  nTipoEdicion : Tipo de edición automatica para los subitems de esta columna que puede ser : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, y DEnum_ListView_TipoEdicion_ComboStatic.
		    \return		No devuelve nada.
            \remarks    La primera columna siempre se alineara a la izquierda por mucho que especifiquemos lo contrario. En la MSDN lei que hay un truco que es agregar una columna dummy como primera columna, y luego borrarla. De esta forma en teoria si que se podria alinear la primera columna de otra forma, pero no lo he probado.
    */
	void DListView::AgregarColumna(const TCHAR *nTexto, const int cAncho, const DEnum_ListView_AlineacionTexto nAlineacion, const DEnum_ListView_TipoEdicion nTipoEdicion) { 
		TCHAR    Tmp[256];
		LVCOLUMN Columna; 
		wcscpy_s(Tmp, 256, nTexto);
		Columna.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT; 
		Columna.cx = cAncho; 
		Columna.pszText = Tmp;
        Columna.fmt = static_cast<int>(nAlineacion);
		Columna.cchTextMax = static_cast<int>(wcslen(Tmp));
		SendMessage(hWnd(), LVM_INSERTCOLUMN, _Columnas.size(), (LPARAM)&Columna);
		DListView_Columna *Col = new DListView_Columna(this, nTipoEdicion); 
		_Columnas.push_back(Col);
        // Establezco el orden para mostrar las columnas del listview en vista mosaico
        if (_OrdenColumnas != NULL) delete [] _OrdenColumnas;
        _OrdenColumnas = new UINT[_Columnas.size() + 1];
        for (UINT i = 0; i < _Columnas.size() - 1; i++) _OrdenColumnas[i] = i + 1;
	};


    //! Función para borrar una columna del ListView especificando su posición.
    /*!	Esta función se usa para borrar una columna del ListView especificando su posición.
		    \fn			void BorrarColumna(const int bColumna);
		    \param[in]	bColumna : Posición de la columna que queremos eliminar dentro de este ListView.
		    \return		No devuelve nada.
    */
	void DListView::BorrarColumna(const int bColumna) { 
		SendMessage(hWnd(), LVM_DELETECOLUMN, bColumna, 0);
		delete _Columnas[bColumna]; 
		_Columnas.erase(_Columnas.begin() + bColumna); 
		for (unsigned int i = 0; i < _Items.size(); i++) { 
			_Items[i]->_SubItems.erase(_Items[i]->_SubItems.begin() + bColumna); 
		} 
        // Establezco el orden para mostrar las columnas del listview en vista mosaico
        if (_OrdenColumnas != NULL) delete [] _OrdenColumnas;
        _OrdenColumnas = new UINT[_Columnas.size() + 1];
        for (UINT i = 0; i < _Columnas.size() - 1; i++) _OrdenColumnas[i] = i + 1;
	};


    //! Función para obtener el item que esta seleccionado con el foco del teclado.
    /*!	Esta función se usa para obtener el item que esta seleccionado con el foco del teclado.
		    \fn			const int Seleccion(void);
		    \return		Devuelve la posición del item que esta seleccionado y con el foco del teclado. Devuelve -1 si no hay item marcado con el foco.
            \remarks    Si deseas saber que items estan seleccionados deberas recorrer todos los items utilizando la funciones DListView::Item() y DListView::TotalItems(), y luego consultar la función DListView_Item::Seleccionado() para saber si el item esta seleccionado.
    */
	const int DListView::Seleccion(void) {
		unsigned int S = 0; 
		for (S = 0; S < _Items.size(); S++) { 
			if (ListView_GetItemState(hWnd(), S, LVIS_SELECTED) == LVIS_SELECTED) {
				return ListView_GetSelectionMark(hWnd());
			} 
		}
		return -1;
	};


    //! Función para eliminar el estado de seleccion a todos los items del ListView.
    /*!	Esta función se usa para eliminar el estado de seleccion a todos los items del ListView
		    \fn			void DesSeleccionarTodo(void);
		    \return		No devuelve nada.
    */
	void DListView::DesSeleccionarTodo(void) { 
		for (unsigned int i = 0; i < _Items.size(); i++) {  
			ListView_SetItemState(hWnd(), i, LVIS_SELECTED, 0);
		} 
	};
	                          

    //! Función para obtener acceso al item de la posición especificada.
    /*  Esta función se utiliza para obtener un puntero a la clase DListView_Item referente al item de la posición especificada.
            \fn         DListView_Item *Item(const UINT Pos);
            \param[in]  Pos : Posición del item del que queremos obtener.
            \return     Esta función devuelve un puntero a la clase DListView_Item que hace referencia a la posición especificada.
            \remarks    Cuidate mucho de especificar posiciones correctas o provocaras una violación de acceso.
    */
	DListView_Item *DListView::Item(const size_t Pos) {
		return _Items[Pos];
	};
	                          

    //! Función para obtener acceso a la columna de la posición especificada.
    /*  Esta función se utiliza para obtener un puntero a la clase DListView_Columna referente a la columna de la posición especificada.
            \fn         DListView_Columna *Columna(const UINT Pos);
            \param[in]  Pos : Posición de la columna del que queremos obtener.
            \return     Esta función devuelve un puntero a la clase DListView_Columna que hace referencia a la posición especificada.
            \remarks    Cuidate mucho de especificar posiciones correctas o provocaras una violación de acceso.
    */
	DListView_Columna *DListView::Columna(const size_t Pos) {
		return _Columnas[Pos];
	};


    //! Función para obtener el total de items que hay en el ListView.
    /*  Esta función se utiliza para obtener el total de items se han creado dentro de este ListView.
            \fn         const UINT TotalItems(void);
            \return     Devuelve el total de items del ListView.
    */
	const UINT DListView::TotalItems(void) { 
		return static_cast<UINT>(_Items.size());
	};


    //! Función para obtener el total de columnas que hay en el ListView.
    /*  Esta función se utiliza para obtener el total de columnas se han creado dentro de este ListView.
            \fn         const UINT TotalColumnas(void);
            \return     Devuelve el total de columnas del ListView.
    */
	const UINT DListView::TotalColumnas(void) { 
		return static_cast<UINT>(_Columnas.size()); 
	};
	                          

    //! Función para obtener el rectangulo donde se ubica el texto de un item / subitem dentro del ListView.
    /*  Esta función se utiliza para obtener el rectangulo que contiene el texto de un item/subitem. Esta función se ha diseñado especificamente para la función DListView::EmpezarEdicion(), ya que dependiendo de la vista del ListView me interesa tener un determinado tamaño.
            \fn         void ObtenerRectaTexto(const UINT eItem, const UINT eSubItem, RECT &RectaDestino, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);
            \param[in]  eItem        : Posición del item del que queremos obtener el rectangulo.
            \param[in]  eSubItem     : Posición del subitem del que queremos obtener el rectangulo.
            \param[out] RectaDestino : RECT donde se almacenaran los valores del rectangulo.
            \param[in]  nTipoEdicion : Parametro que indica el tipo de edicion que ha requerido esta función. Especifica DEnum_ListView_TipoEdicion_SinEdicion si vas a utilizar esta funcion, de lo contrario puede darte un tamaño algo mas grande dependiendo de la edición que especifiques.
            \return     Esta función no devuelve nada.
            \remarks    Las macros ListView_GetItemRect y ListView_GetSubItemRect no me devolvian los tamaños deseados a la hora de crear los controles de edicion, por ello he tenido que crear esta funcion que se basa en una mezcla de las dos macros, dependiendo de la vista y del tipo de edición.
    */
    void DListView::ObtenerRectaTexto(const UINT eItem, const UINT eSubItem, RECT &RectaDestino, const DEnum_ListView_TipoEdicion nTipoEdicion) {
        UINT TamExtra = 0;
        UINT TotalSubItemsMosaico = 0;
        RECT RectaMarco;
        RECT RectaSubItem;
        if (nTipoEdicion == DEnum_ListView_TipoEdicion_ComboEditTexto || nTipoEdicion == DEnum_ListView_TipoEdicion_ComboEditEntero || nTipoEdicion == DEnum_ListView_TipoEdicion_ComboEditDecimal || nTipoEdicion == DEnum_ListView_TipoEdicion_ComboStatic) {
            TamExtra = 10;
        }
		switch (static_cast<DEnum_ListView_Vista>(SendMessage(hWnd(), LVM_GETVIEW, 0, 0))) { // Tipo de vista
            case DEnum_ListView_Vista_Detalles :
				ListView_GetSubItemRect(hWnd(), eItem, eSubItem, LVIR_LABEL, &RectaDestino);
                if (eSubItem == 0) RectaDestino.left -=4; 
                break;
            case DEnum_ListView_Vista_Lista :
				ListView_GetSubItemRect(hWnd(), eItem, eSubItem, LVIR_LABEL, &RectaDestino);
                RectaDestino.left -=4; 
                RectaDestino.right += TamExtra; 
                break;
            case DEnum_ListView_Vista_IconosPeque :
				ListView_GetSubItemRect(hWnd(), eItem, eSubItem, LVIR_LABEL, &RectaDestino);
                RectaDestino.left -=4; 
                RectaDestino.right += (TamExtra + 8); 
                break;
            case DEnum_ListView_Vista_Iconos :
                // En esta vista no quiero hacer mas grande la ComboBox porque suele salir el texto centrado, y al hacer mas grande la parte derecha, no acaba de quedar bien.....
				ListView_GetItemRect(hWnd(), eItem, &RectaDestino, LVIR_LABEL);
                RectaDestino.top -= 2;
                RectaDestino.bottom -= 2;
                RectaDestino.left -=4; 
                RectaDestino.right += 4; 
                break;
            case DEnum_ListView_Vista_Mosaico :
                TotalSubItemsMosaico = MostrarSubItemsMosaico();
				ListView_GetItemRect(hWnd(), eItem, &RectaMarco, LVIR_LABEL);
                RectaMarco.top += 2;
                RectaMarco.bottom += 2;
                RectaDestino.left = RectaMarco.left - 6;
                RectaDestino.right = RectaMarco.right - 4;
                if (TotalSubItemsMosaico == 0) { // solo se muestra el item
                    // Hay que obtener la aultura de la fuente seleccionada para el item, pero no se si se podra cambiar con el customdraw.... de momento la dejaremos a 13
                    RectaDestino.top = (((RectaMarco.bottom - RectaMarco.top - 13) / 2) + RectaMarco.top) - 4;
                    RectaDestino.bottom = RectaDestino.top + 13;
                }
                else {
                    if (eSubItem == 0) {
						ListView_GetSubItemRect(hWnd(), eItem, 1, LVIR_LABEL, &RectaSubItem);
                        TamExtra = RectaSubItem.bottom - RectaSubItem.top;
                        RectaDestino.top = (RectaSubItem.top - TamExtra) - 2;
                        RectaDestino.bottom = (RectaSubItem.bottom - TamExtra);
                    }
                    else {
						ListView_GetSubItemRect(hWnd(), eItem, eSubItem, LVIR_LABEL, &RectaSubItem);
                        RectaDestino.top = RectaSubItem.top - 2;
                        RectaDestino.bottom = RectaSubItem.bottom;
                    }
                }
                break;
        }
    }

    //! Función para empezar la edición de un item/subitem.
    /*! Esta función se utiliza para permitir al usuario que edite el valor de un item/subitem.
            \fn         HWND EmpezarEdicion(const UINT eItem, const UINT eSubItem, const DEnum_ListView_TipoEdicion nTipoEdicion);
            \param[in]  eItem           Posición del item que se va a editar.
            \param[in]  eSubItem        Posicion del subitem que se va a editar.
            \param[in]  nTipoEdicion    Tipo de edicion que se va a aplicar al subitem, que puede ser : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, y DEnum_ListView_TipoEdicion_ComboStatic.
            \return     Devuelve el HWND del control que se ha creado para la edición.
            \remarks    Si empiezas una edición manualmente utilizando esta funcion, debes terminarla con la función TerminarEdicion para guardar los datos, o con la función CancelarEdicion para descartar los datos introducidos.
                        Al utilizar esta función la ventana padre del ListView recibira una llamada a su funcion virtual DEventosPadre::Evento_ListView_Edicion_Empezar().
                        Si el usuario presiona INTRO, la edicion terminara y se guardaran los datos. Si el usuario presiona ESC, la edición se cancelara y se descartaran los datos.
            \sa         TerminarEdicion(), CancelarEdicion(), DEventosPadre::Evento_ListView_Edicion_Empezar().
    */
    HWND DListView::EmpezarEdicion(const UINT eItem, const UINT eSubItem, const DEnum_ListView_TipoEdicion nTipoEdicion) {
        CancelarEdicion();
		if (eItem > _Items.size() || eItem < 0 || eSubItem > _Columnas.size() || eSubItem < 0) return NULL; 
        HFONT FuenteEdicion = _Items[eItem]->FuenteSubItem(eSubItem);
		if (FuenteEdicion == NULL) FuenteEdicion = (HFONT)SendMessage(hWnd(), WM_GETFONT, 0, 0);
		ListView_SetItemState(hWnd(), -1, 0, LVIS_SELECTED);  // Elimino la selección de todos los items
		_eItem = eItem; 
		_eSubItem = eSubItem; 
        _Editando = nTipoEdicion;
        // Obtengo el tamaño del subitem
        RECT Dim;
        ObtenerRectaTexto(eItem, eSubItem, Dim, nTipoEdicion);

//        DWL_IMPRIMIR_DEBUG(TEXT("RectaItem -> Left : %d, Top %d, Right %d, Bottom %d\n"), Dim.left, Dim.top, Dim.right, Dim.bottom);
        // Obtengo el texto del subitem 
		std::wstring TmpStr;
        _Items[eItem]->ObtenerTexto(eSubItem, TmpStr);
        // Creo la clase con los datos de la edición
        DListView_DatosEdicion DatosEdicion(_eItem, _eSubItem, _hWndEdicion, nTipoEdicion);
        LRESULT Ret = 0;
        switch (nTipoEdicion) {
            case DEnum_ListView_TipoEdicion_EditTexto :
            case DEnum_ListView_TipoEdicion_EditEntero :
            case DEnum_ListView_TipoEdicion_EditDecimal :
				_hWndEdicion = CreateWindow(TEXT("EDIT"), TmpStr.c_str(), WS_CHILD | ES_AUTOHSCROLL, Dim.left + 3, Dim.top + 2, Dim.right - (Dim.left + 2), Dim.bottom - (Dim.top + 2), hWnd(), (HMENU)555, GetModuleHandle(NULL), NULL);
        		SendMessage(_hWndEdicion, EM_LIMITTEXT, 0, 0); // Buffer al maximo para el texto
        		SendMessage(_hWndEdicion, EM_SETSEL, 0, TmpStr.size()); // Selecciono todo el texto

           		SetWindowLongPtr(_hWndEdicion, GWLP_USERDATA, (LONG_PTR)this); // Userdata contiene esta clase
                SetWindowTheme(_hWndEdicion, TEXT("Explorer"), NULL);
        		SendMessage(_hWndEdicion, WM_SETFONT, (WPARAM)FuenteEdicion, 0); // Asigno la fuente
                DatosEdicion._hWndEdicion = _hWndEdicion;
				Ret = SendMessage(hWndPadre(), DWL_LISTVIEW_EMPEZAR_EDICION, static_cast<WPARAM>(ID()), reinterpret_cast<LPARAM>(&DatosEdicion));
                if (Ret == 0) {
		            _GestorMensajesOriginalEdicion = (WNDPROC)SetWindowLongPtr(_hWndEdicion, GWLP_WNDPROC, (LONG_PTR)_GestorMensajesEdicion); 

		            ShowWindow (_hWndEdicion, SW_SHOW); 
		            UpdateWindow(_hWndEdicion);  
		            SetFocus(_hWndEdicion); 
                }
                else {
                    if (_hWndEdicion != NULL) DestroyWindow(_hWndEdicion);
                    _hWndEdicion = NULL;
                    _Editando = DEnum_ListView_TipoEdicion_SinEdicion;
                }
                break;

            case DEnum_ListView_TipoEdicion_ComboEditTexto :
            case DEnum_ListView_TipoEdicion_ComboEditEntero :
            case DEnum_ListView_TipoEdicion_ComboEditDecimal :
				_hWndEdicion = CreateWindowEx(NULL, TEXT("COMBOBOX"), NULL, WS_CHILD | CBS_DROPDOWN, Dim.left, Dim.top - 1, Dim.right - Dim.left, 60, hWnd(), (HMENU)556, GetModuleHandle(NULL), NULL);
                SendMessage(_hWndEdicion, WM_SETFONT, (WPARAM)FuenteEdicion , 0); // Fuente del control
                SetWindowTheme(_hWndEdicion, TEXT("Explorer"), NULL);
                DatosEdicion._hWndEdicion = _hWndEdicion;

				Ret = SendMessage(hWndPadre(), DWL_LISTVIEW_EMPEZAR_EDICION, static_cast<WPARAM>(ID()), reinterpret_cast<LPARAM>(&DatosEdicion));
                if (Ret == 0) {
                    // Busco el EditBox dentro del ComboBox para re-emplazar su WindowProcedure
                    EnumChildWindows(_hWndEdicion, _BuscarEditEnCombo, (LONG_PTR)this);
                    // Asigno el texto al editbox
                    SendMessage(_hWndEdicion, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(TmpStr.c_str()));
                    ShowWindow(_hWndEdicion, SW_SHOW);
                }
                else {
                    if (_hWndEdicion != NULL) DestroyWindow(_hWndEdicion);
                    _hWndEdicion = NULL;
                    _Editando = DEnum_ListView_TipoEdicion_SinEdicion;
                }

                break;

            case DEnum_ListView_TipoEdicion_ComboStatic :
				_hWndEdicion = CreateWindowEx(NULL, TEXT("COMBOBOX"), NULL, WS_CHILD | CBS_DROPDOWNLIST, Dim.left + 2, Dim.top - 2, Dim.right - Dim.left - 2, 60, hWnd(), (HMENU)556, GetModuleHandle(NULL), NULL);
                SetWindowTheme(_hWndEdicion, TEXT("Explorer"), NULL);
                SendMessage(_hWndEdicion, WM_SETFONT, (WPARAM)FuenteEdicion, 0); // Fuente del control
                DatosEdicion._hWndEdicion = _hWndEdicion;

				Ret = SendMessage(hWndPadre(), DWL_LISTVIEW_EMPEZAR_EDICION, static_cast<WPARAM>(ID()), reinterpret_cast<LPARAM>(&DatosEdicion));
                if (Ret == 0) {
                    SendMessage(_hWndEdicion, CB_SELECTSTRING, -1, (LPARAM)TmpStr.c_str());
                    ShowWindow(_hWndEdicion, SW_SHOW);
                }
                else {
                    if (_hWndEdicion != NULL) DestroyWindow(_hWndEdicion);
                    _hWndEdicion = NULL;
                    _Editando = DEnum_ListView_TipoEdicion_SinEdicion;
                }
                break;
        }
        return _hWndEdicion;
    }


    //! Función que busca el HWND del EditBox que hay dentro del ComboBox.
    /*! Esta función de uso interno unicamente tiene la finalidad de obtener el HWND del EditBox que hay dentro de una ComboBox.
            \fn         static BOOL CALLBACK _BuscarEditEnCombo(HWND hWndEdit, LPARAM lParam);
            \param[in]  hWndEdit    HWND que deberia ser el del EditBox
            \param[in]  lParam      Clase DWlistView que pide la busqueda.
            \return     No devuelve nada.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
    BOOL CALLBACK DListView::_BuscarEditEnCombo(HWND hWndEdit, LPARAM lParam) {
        DListView *This = reinterpret_cast<DListView *>(lParam);
   		SetWindowLongPtr(hWndEdit, GWLP_USERDATA, (LONG_PTR)This); // Userdata contiene esta clase
        This->_GestorMensajesOriginalEdicion = (WNDPROC)SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (LONG_PTR)This->_GestorMensajesEdicion); 
        return FALSE; // detenemos la enumeración, el EditBox es la unica ventana dentro del ComboBox
    }

    //! Función para terminar la edición de un item/subitem.
    /*! Esta función se utiliza para terminar la edición de un item/subitem y guardar los datos introducidos.
            \fn         void TerminarEdicion(void);
            \return     No devuelve nada.
            \remarks    Al utilizar esta función la ventana padre del ListView recibira una llamada a su funcion virtual DEventosPadre::Evento_ListView_Edicion_Terminar().
            \sa         EmpezarEdicion(), CancelarEdicion(), DEventosPadre::Evento_ListView_Edicion_Terminar().
    */
    void DListView::TerminarEdicion(void) {
        if (_hWndEdicion != NULL && _Editando != DEnum_ListView_TipoEdicion_SinEdicion) {
            // Creo la clase con los datos de la edición
            DListView_DatosEdicion DatosEdicion(_eItem, _eSubItem, _hWndEdicion, _Editando);
            LRESULT Ret = 0;
			std::wstring TmpTxt;

			Ret = SendMessage(hWndPadre(), DWL_LISTVIEW_TERMINAR_EDICION, static_cast<WPARAM>(ID()), reinterpret_cast<LPARAM>(&DatosEdicion));
            switch (_Editando) {
                case DEnum_ListView_TipoEdicion_EditTexto :
                case DEnum_ListView_TipoEdicion_EditEntero :
                case DEnum_ListView_TipoEdicion_EditDecimal :
//			        GetDlgItemText(GetParent(_hWndEdicion), GetWindowLongPtr(_hWndEdicion, GWL_ID), Txt, 512);
                    if (Ret == 0) {
                        DatosEdicion.Edit_ObtenerTexto(TmpTxt);
                        _Items[_eItem]->AsignarTexto(_eSubItem, TmpTxt.c_str()); 	 
                    }
                    break;
                case DEnum_ListView_TipoEdicion_ComboEditTexto :
                case DEnum_ListView_TipoEdicion_ComboEditEntero :
                case DEnum_ListView_TipoEdicion_ComboEditDecimal :
                    if (Ret == 0) {
                        DatosEdicion.ComboEdit_ObtenerTexto(TmpTxt);
						_Items[_eItem]->AsignarTexto(_eSubItem, TmpTxt.c_str());
                    }
                    break;
                case DEnum_ListView_TipoEdicion_ComboStatic :
                    if (Ret == 0) {
                        DatosEdicion.ComboStatic_ObtenerSeleccion(TmpTxt);
						_Items[_eItem]->AsignarTexto(_eSubItem, TmpTxt.c_str());
                    }
                    break;
            }

			RECT Dim = { 0, 0, 0, 0 }; 
			ListView_GetSubItemRect(hWnd(), _eItem, _eSubItem, LVIR_LABEL, &Dim);
			InvalidateRect(hWnd(), &Dim, false);  
			ListView_SetItemState(hWnd(), _eItem, 0, LVIS_SELECTED);
        }
		if (_hWndEdicion != NULL) DestroyWindow(_hWndEdicion);	
        _Editando = DEnum_ListView_TipoEdicion_SinEdicion;
        _hWndEdicion = NULL;
    }

    //! Función para cancelar la edición de un item/subitem.
    /*! Esta función se utiliza para cancelar la edición de un item/subitem y descartar los datos introducidos.
            \fn         void CancelarEdicion(void);
            \return     No devuelve nada.
            \remarks    Al utilizar esta función la ventana padre del ListView recibira una llamada a su funcion virtual DEventosPadre::Evento_ListView_Edicion_Cancelar().
            \sa         EmpezarEdicion(), TerminarEdicion(), DEventosPadre::Evento_ListView_Edicion_Cancelar().
    */
    void DListView::CancelarEdicion(void) {
        if (_hWndEdicion != NULL && _Editando != DEnum_ListView_TipoEdicion_SinEdicion) {
			ListView_SetItemState(hWnd(), _eItem, 0, LVIS_SELECTED);
            DListView_DatosEdicion DatosEdicion(_eItem, _eSubItem, _hWndEdicion, _Editando);
			SendMessage(hWndPadre(), DWL_LISTVIEW_CANCELAR_EDICION, static_cast<WPARAM>(ID()), reinterpret_cast<LPARAM>(&DatosEdicion));
        }
		if (_hWndEdicion != NULL) DestroyWindow(_hWndEdicion);	
        _Editando = DEnum_ListView_TipoEdicion_SinEdicion;
        _hWndEdicion = NULL;
    }


    //! Función que indica que item esta en unas determinadas coordenadas relativas al ListView.
    /*! Esta función se utiliza obtener que item esta en unas determinadas coordenadas relativas a los items que se ven en el ListView.
            \fn         const int ItemEnCoordenadas(const int cX, const int cY);
            \param[in]  cX     Coordenada X.
            \param[in]  cY     Coordenada Y.
            \return     Devuelve la posición del item que esta debajo de esas coordenadas o -1 en caso de no haber ningun item debajo.
    */
	const int DListView::ItemEnCoordenadas(const int cX, const int cY) {
		int				HT = -1;
		RECT			R;
		LVHITTESTINFO   LInfo;
		LInfo.pt.x = cX;
		LInfo.pt.y = cY;
		LInfo.flags = LVHT_ABOVE | LVHT_TOLEFT;
		HT = ListView_HitTest(hWnd(), &LInfo);
		ListView_GetItemRect(hWnd(), HT, &R, LVIR_BOUNDS);
		if (PtInRect(&R, LInfo.pt) != 0) { return HT; }
		else                       { return -1; } 
	};


    //! Función que indica que subitem esta en unas determinadas coordenadas relativas al ListView.
    /*! Esta función se utiliza obtener que subitem esta en unas determinadas coordenadas relativas a los items que se ven en el ListView.
            \fn         const int SubItemEnCoordenadas(const int cX, const int cY, int &cItem, int &cSubItem);
            \param[in]  cX          Coordenada X.
            \param[in]  cY          Coordenada Y.
            \param[out] cItem       Variable donde se almacenara la posición del item debajo de las coordenadas, o -1 si no hay item en esas coordenadas.
            \param[out] cSubItem    Variable donde se almacenara la posición del item debajo de las coordenadas, o -1 si no hay item en esas coordenadas.
            \return     Esta función no devuelve nada.
    */
	void DListView::SubItemEnCoordenadas(const int cX, const int cY, int &cItem, int &cSubItem) {
		RECT			R;
		LVHITTESTINFO   LInfo;
        cItem = -1;
        cSubItem = -1;
		LInfo.pt.x = cX;
		LInfo.pt.y = cY;
		LInfo.flags = LVHT_ABOVE | LVHT_TOLEFT;
		cItem = ListView_SubItemHitTest(hWnd(), &LInfo);
		ListView_GetSubItemRect(hWnd(), cItem, LInfo.iSubItem, LVIR_BOUNDS, &R);
		if (PtInRect(&R, LInfo.pt) != 0) { cSubItem = LInfo.iSubItem; }
	};



    //! Función que procesa las notificaciones NM_CLICK, NM_DBLCLK, NM_RCLICK, NM_RDBLCLK.
    /*! Esta función de uso interno devuelve las notificaciones del tipo click a su ventana padre.
            \fn         LRESULT _NotificacionClick(LPARAM lParam);
            \param[in]  lParam   Datos para la notificación.
            \return     Siempre devuelve 0.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
    LRESULT DListView::_NotificacionClick(LPARAM lParam) {
        int Boton = 0;
        bool DobleClick = false;
        if (((LPNMHDR)lParam)->code == NM_RCLICK || ((LPNMHDR)lParam)->code == NM_RDBLCLK) Boton = 1;
        if (((LPNMHDR)lParam)->code == NM_DBLCLK || ((LPNMHDR)lParam)->code == NM_RDBLCLK) DobleClick = true;
        _Evento_Mouse_BotonSoltado(Boton, ((LPNMITEMACTIVATE)lParam)->ptAction.x, ((LPNMITEMACTIVATE)lParam)->ptAction.y, 0);
        DListView_DatosClick DatosClick(((LPNMITEMACTIVATE)lParam)->iItem, ((LPNMITEMACTIVATE)lParam)->iSubItem, Boton, ((LPNMITEMACTIVATE)lParam)->ptAction.x, ((LPNMITEMACTIVATE)lParam)->ptAction.y);
		if (DobleClick == false) return SendMessage(hWndPadre(), DWL_LISTVIEW_CLICK, static_cast<WPARAM>(ID()), reinterpret_cast<LPARAM>(&DatosClick));
		else                     return SendMessage(hWndPadre(), DWL_LISTVIEW_DOBLECLICK, static_cast<WPARAM>(ID()), reinterpret_cast<LPARAM>(&DatosClick));
    }


    //! Nuevo WindowProcedure que se sobrepondra al WindowProcedure original del ListView.
    /*! Esta función es la que re-emplaza el WindowProcedure del ListView para poder hacer cosas como la edicion, y el pintado personalizado.
            \fn         virtual LRESULT CALLBACK GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam)
            \param[in]  uMsg        Mensaje
            \param[in]  wParam      Parametro wide
            \param[in]  lParam      Parametro long
            \return     Dependiendo del mensaje procesado devolvera una cosa u otra...
            \remarks    Hacer un SendMessage al HWND de este ListView o utilizar esta función es exactamente lo mismo.
    */
	LRESULT CALLBACK DListView::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
            case DWL_NOTIFICACION :
                switch (((LPNMHDR)lParam)->code) {
                    case NM_CLICK : // nItem, nSubItem, ptAction
                    case NM_DBLCLK : // nItem, nSubItem, ptAction
                    case NM_RCLICK : // nItem, nSubItem, ptAction
                    case NM_RDBLCLK : // nItem, nSubItem, ptAction
                        return _NotificacionClick(lParam);
//                    case NM_HOVER : // (devolver 0 para normal, 1 para cancelar)
//                        return SendMessage(GetParent(_hWnd), DWL_LISTVIEW_RESALTAR_ITEM, static_cast<WPARAM>(ID()), 0);
                    case NM_KILLFOCUS : // nada
						return SendMessage(hWndPadre(), DWL_LISTVIEW_FOCO_PERDIDO, static_cast<WPARAM>(ID()), 0);
                    case NM_RELEASEDCAPTURE : // nada
						return SendMessage(hWndPadre(), DWL_LISTVIEW_TERMINAR_CAPTURA, static_cast<WPARAM>(ID()), 0);
                    case NM_RETURN : // nada
						return SendMessage(hWndPadre(), DWL_LISTVIEW_TECLADO_INTRO, static_cast<WPARAM>(ID()), 0);
                    case NM_SETFOCUS : // nada
						return SendMessage(hWndPadre(), DWL_LISTVIEW_FOCO_OBTENIDO, static_cast<WPARAM>(ID()), 0);
                }
                break;
            case WM_MOUSELEAVE :
                _Evento_Mouse_Saliendo();
                break;
            case WM_MOUSEMOVE :
                _Evento_Mouse_Movimiento(LOWORD(lParam), HIWORD(lParam), static_cast<UINT>(wParam));
                break;
            case WM_HSCROLL  :
            case WM_VSCROLL  :
                CancelarEdicion();
                break;
			case WM_SETFOCUS :
//			case WM_KILLFOCUS : NO UTILIZAR EL WM_KILLFOCUS PARA CANCELAR LA EDICIÓN... UNO DE LOS PROBLEMAS ES QUE AL CREAR UN NUEVO CONTROL SE PIERDE EL FOCO, Y LA LIAMOS....
                CancelarEdicion();  
				break;
			case DWL_CUSTOM_DRAWDLG :
                // Para dialogos necesito hacer el SendMessage DWL_MSGRESULT, pero para ventanas debo retornar directamente _CustomDraw(lParam);
				SetWindowLongPtr(hWndPadre(), DWL_MSGRESULT, (LONG)_CustomDraw(lParam));
				return TRUE; 
			case DWL_CUSTOM_DRAWWND :
                // Para ventanas se enlaza directamente con la funcion _CustomDraw(lParam);
				return _CustomDraw(lParam); 
			// Paso el trabajo de cambiar los colores al WndProc del EditBox.
            case WM_CTLCOLOREDIT : 
				return SendMessage((HWND)lParam, uMsg, wParam, 0);  
			// Mensaje que re-emplaza el label edit convencional
            case LVM_EDITLABEL :
                return (LRESULT)EmpezarEdicion((int)wParam, (int)lParam, DEnum_ListView_TipoEdicion_EditTexto);
            // Notificacion que se mandara a traves del combostatic cuando se cambie la seleccion
            case WM_COMMAND :
                if (HIWORD(wParam) == CBN_SELCHANGE && _Editando == DEnum_ListView_TipoEdicion_ComboStatic) TerminarEdicion();
                break;
            // Notificaciones que mandaran los controles de edición
			case WM_NOTIFY : 
				switch (((LPNMHDR)lParam)->code) { 
					case NM_KEYDOWN : 
						switch(((LPNMKEY)lParam)->nVKey) { 
							case VK_RETURN : 
                                TerminarEdicion();
								break; 
							case VK_ESCAPE : 
                                CancelarEdicion();
								break; 
						}	
						break;
//					case NM_KILLFOCUS : // ESTO NO SE RECIBE NUNCA.....
//                        CancelarEdicion();
//						break; 
				}  
				break;
		} 
		return DControl::GestorMensajes(uMsg, wParam, lParam);
	};


    //! Función virtual que permite insertar gráficos en la fase de pintado de un item/subitem.
    /*! El objetivo de esta función virtual es poder insertar gráficos extras en la fase de pintado de un item/subitem.
            \fn         virtual LRESULT Evento_PintarItem(HDC hDC, const UINT nItem, const UINT nSubItem, DWORD nTipoItem);
            \param[in]  hDC             HDC donde pintar los graficos del ListView.
            \param[in]  nItem           Posición del item que se va a pintar.
            \param[in]  nSubItem        Posición del subitem que se va a pintar.
            \param[in]  nTipoItem       Indica que tipo de item se va a pintar, puede ser : LVCDI_ITEM, LVCDI_GROUP, o LVCDI_ITEMSLIST. 
            \return     Debes devolver CDRF_SKIPDEFAULT si vas a insertar gráficos extras, o CDRF_DODEFAULT si no vas a pintar nada.
            \remarks    En el modo de vista mosaico, no podras pintar graficos extras para el nSubItem 0, para los demas modos de vista si que puedes.\n
            \sa         NM_CUSTOMDRAW ListView en la MSDN.
    */
    LRESULT DListView::Evento_PintarItem(HDC hDC, const UINT nItem, const UINT nSubItem, DWORD nTipoItem) {
		return CDRF_DODEFAULT; 
    }

    //! Función que recibe el mensaje NM_CUSTOMDRAW.
    /*! Esta función de uso interno es la base para pintar items con colores y fuentes distintas.
            \fn         LRESULT _CustomDraw(LPARAM Datos);
            \param[in]  Datos      Datos para el pintado.
            \return     No devuelve nada.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
	LRESULT DListView::_CustomDraw(LPARAM Datos) { 
        LRESULT Ret = NULL;
		LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)Datos; 
        switch (lplvcd->nmcd.dwDrawStage) { 
            case CDDS_PREPAINT  : return (CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW); 
            case CDDS_POSTPAINT : return CDRF_NOTIFYPOSTPAINT;
            case CDDS_ITEMPREPAINT : 
                // NOTA : En todos los modos menos el report cuando se va a pintar el primer item pasa por "case (CDDS_ITEMPREPAINT | CDDS_SUBITEM)", pero en el modo mosaico el subitem 0 no pasa por allí,
                //        por lo que hay que mirar la fuente, y los colores aquí tambien....
                if ((LVIS_SELECTED ^ ListView_GetItemState(hWnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED)) != false) {
                    // Color texto normal
                    if (_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTexto != NULL) lplvcd->clrText = *_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTexto; 
                }
                else {
                    // Color texto seleccionado
                    if (_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTextoSeleccionado != NULL) lplvcd->clrText = *_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTextoSeleccionado; 
                }
                if (_Items[lplvcd->nmcd.dwItemSpec]->FuenteSubItem(lplvcd->iSubItem) != NULL) {
                    SelectObject(lplvcd->nmcd.hdc, _Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].Fuente);
                    return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW | CDRF_NEWFONT); 
                }
                return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW); 
            case (CDDS_ITEMPREPAINT | CDDS_SUBITEM) : 
                Ret = Evento_PintarItem(lplvcd->nmcd.hdc, static_cast<UINT>(lplvcd->nmcd.dwItemSpec), static_cast<UINT>(lplvcd->iSubItem), lplvcd->dwItemType);
                // Si Evento_PintarItem devuelve CDRF_SKIPDEFAULT es que se ha pintado algo alli, y no se necesitan ni los colores ni la fuente para el texto de este subitem.
                if (Ret != CDRF_SKIPDEFAULT) {
                    Ret = CDRF_DODEFAULT;
					if ((LVIS_SELECTED ^ ListView_GetItemState(hWnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED)) != false) {
                        // Color texto normal
                        if (_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTexto != NULL) lplvcd->clrText = *_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTexto; 
                    }
                    else {
                        // Color texto seleccionado
                        if (_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTextoSeleccionado != NULL) lplvcd->clrText = *_Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].ColorTextoSeleccionado; 
                    }
                    if (_Items[lplvcd->nmcd.dwItemSpec]->FuenteSubItem(lplvcd->iSubItem) != NULL) {
                        SelectObject(lplvcd->nmcd.hdc, _Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].Fuente);
                        Ret = CDRF_NEWFONT;
                    }
                }
                return Ret;
        }
		return CDRF_DODEFAULT;
    }

    //! Función que recibe el mensaje NM_CUSTOMDRAW.
    /*! Esta función de uso interno es la base para pintar items con colores y fuentes distintas.
            \fn         LRESULT _CustomDraw(LPARAM Datos);
            \param[in]  Datos      Datos para el pintado.
            \return     No devuelve nada.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
	/*LRESULT DListView::_CustomDraw(LPARAM Datos) { 
		LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)Datos; 
//        DWL_IMPRIMIR_DEBUG(TEXT("dwDrawStage %d\n"), lplvcd->nmcd.dwDrawStage);
        switch (lplvcd->nmcd.dwDrawStage) { 
			 case CDDS_PREPAINT :  
//                 DWL_IMPRIMIR_DEBUG(TEXT("CDDS_PREPAINT\n"));
				 return (CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW); 
			 case CDDS_POSTPAINT : 
                 DWL_IMPRIMIR_DEBUG(TEXT("CDDS_POSTPAINT\n"));
                 return CDRF_NOTIFYPOSTPAINT;
			 case CDDS_ITEMPREPAINT : 
				 if (_Items[lplvcd->nmcd.dwItemSpec]->FuenteSubItem(lplvcd->iSubItem) != NULL) 
					 SelectObject(lplvcd->nmcd.hdc, _Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].Fuente);
				 if ((LVIS_SELECTED ^ ListView_GetItemState(_hWnd, lplvcd->nmcd.dwItemSpec, LVIS_SELECTED)) != false)
					 return Evento_Pintar_InicioItem(lplvcd);  
				 else 
					 return Evento_Pintar_InicioItemSeleccionado(lplvcd); 
//                 DWL_IMPRIMIR_DEBUG(TEXT("CDDS_ITEMPREPAINT\n"));
				 return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW); 
			 case (CDDS_ITEMPREPAINT | CDDS_SUBITEM) :
//                 DWL_IMPRIMIR_DEBUG(TEXT("CDDS_SUBITEM CDDS_ITEMPREPAINT\n"));
				 if (_Items[lplvcd->nmcd.dwItemSpec]->FuenteSubItem(lplvcd->iSubItem) != NULL) 
					 SelectObject(lplvcd->nmcd.hdc, _Items[lplvcd->nmcd.dwItemSpec]->_SubItems[lplvcd->iSubItem].Fuente);
				 if ((LVIS_SELECTED ^ ListView_GetItemState(_hWnd, lplvcd->nmcd.dwItemSpec, LVIS_SELECTED)) != false)
					 return Evento_Pintar_InicioItem(lplvcd);  
				 else 
					 return Evento_Pintar_InicioItemSeleccionado(lplvcd); 
			 case (CDDS_ITEMPOSTPAINT | CDDS_SUBITEM) :
                 DWL_IMPRIMIR_DEBUG(TEXT("CDDS_ITEMPOSTPAINT\n"));
				 if ((LVIS_SELECTED ^ ListView_GetItemState(_hWnd, lplvcd->nmcd.dwItemSpec, LVIS_SELECTED)) != false)
					 return Evento_Pintar_FinItem(lplvcd);  
				 else
					 return Evento_Pintar_FinItemSeleccionado(lplvcd);   
		} 
		return CDRF_DODEFAULT;
	}; */




    //! Gestor de mensajes para los editbox de las ediciones.
    /*! Esta función es el gestor de mensajes para los editbox que se crean durante las ediciones de items/subitems.
            \fn         static LRESULT CALLBACK _GestorMensajesEdicion(HWND dhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
            \param[in]  dhWnd       HWND del EditBox.
            \param[in]  uMsg        Mensaje.
            \param[in]  wParam      WPARAM.
            \param[in]  lParam      LPARAM.
            \return     No devuelve nada.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.\n
                        Utilizada en los EditBox y los ComboBox. 
    */
	LRESULT CALLBACK DListView::_GestorMensajesEdicion(HWND dhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
		DListView *This = reinterpret_cast<DListView *>(GetWindowLongPtr(dhWnd, GWLP_USERDATA));  
		NMKEY Key;	
		NMHDR T; 
		T.hwndFrom = dhWnd;
		T.idFrom = GetWindowLongPtr(dhWnd, GWL_ID); 
		switch (uMsg) {  
/*			case WM_CTLCOLOREDIT : 
			case WM_CTLCOLORSTATIC : 
				//This->Item(This->_eItem)->ColorFondo(This->_eSubItem);
				SetTextColor((HDC)wParam, This->Item(This->_eItem)->ColorTextoSubItem(This->_eSubItem));
//				SetBkColor((HDC)wParam, This->Item(This->_eItem)->ColorFondoSubItem(This->_eSubItem));
				SetBkMode((HDC)wParam, TRANSPARENT);	
//				return (LRESULT)This->_BrochaFondoEdicion; 
                return (LRESULT)GetSysColorBrush(COLOR_BACKGROUND);*/
            case WM_KEYUP :
				T.code = NM_KEYDOWN;
				Key.hdr = T;	
				Key.nVKey = (unsigned int)wParam;
				Key.uFlags = (unsigned int)lParam; 
                This->GestorMensajes(WM_NOTIFY, 0, reinterpret_cast<WPARAM>(&Key));
                break;
			case WM_CHAR : 
 				T.code = NM_KEYDOWN;
				Key.hdr = T;	
				Key.nVKey = (unsigned int)wParam;
				Key.uFlags = (unsigned int)lParam; 
                switch (This->_Editando) {
                    case DEnum_ListView_TipoEdicion_EditEntero :
                    case DEnum_ListView_TipoEdicion_ComboEditEntero :
						switch (static_cast<TCHAR>(wParam)) {
							case TEXT('0') :	case TEXT('1') :	case TEXT('2') :	case TEXT('3') :
							case TEXT('4') :	case TEXT('5') :	case TEXT('6') :	case TEXT('7') :
							case TEXT('8') :	case TEXT('9') :	case 8	:
							case VK_TAB :
                                break;
                            default :
                                return 0;
                        }
                        break;
                    case DEnum_ListView_TipoEdicion_EditDecimal :
                    case DEnum_ListView_TipoEdicion_ComboEditDecimal :
						switch (static_cast<TCHAR>(wParam)) {
							case TEXT('0') :	case TEXT('1') :	case TEXT('2') :	case TEXT('3') :
							case TEXT('4') :	case TEXT('5') :	case TEXT('6') :	case TEXT('7') :
							case TEXT('8') :	case TEXT('9') :	case TEXT('.') :    case 8	:
							case VK_TAB :
                                break;
                            default :
                                return 0;
                        }
                        break;
                    default :
                        break;
                }
				This->GestorMensajes(WM_NOTIFY, 0, reinterpret_cast<WPARAM>(&Key));
				break;
		} 
		return CallWindowProc(This->_GestorMensajesOriginalEdicion, dhWnd, uMsg, wParam, lParam);
	};


    //! Función virtual que permite controlar cuando se hace click en el ListView.
    /*! El objetivo de esta función virtual es poder captar cuando se hace click en el ListView al heredar la clase.
            \fn         virtual LRESULT Evento_Mouse_Click(const int nItem, const int nSubItem, const UINT Boton, const int cX, const int cY);
            \param[in]  nItem       Item en el que se ha hecho click (puede ser -1 si no hay item).
            \param[in]  nSubItem    SubItem en el que se ha hecho click (puede ser -1 si no hay item/subitem).
            \param[in]  nBoton      Boton del mouse presionado (0 izquierdo, 1 derecho).
            \param[in]  cX          Coordenada X del mouse.
            \param[in]  cY          Coordenada Y del mouse.
            \return     Debes devolver 0.
    */
    LRESULT DListView::Evento_Mouse_Click(const int nItem, const int nSubItem, const UINT nBoton, const int cX, const int cY) {
        return 0;
    }

    //! Función virtual que permite controlar cuando se moeve el mouse en el ListView.
    /*! El objetivo de esta función virtual es poder captar cuando se moeve el mouse en el ListView al heredar la clase.
            \fn         virtual LRESULT Evento_Mouse_Movimiento(const int nItem, const int nSubItem, const int cX, const int cY);
            \param[in]  nItem       Item en el que se ha hecho click (puede ser -1 si no hay item).
            \param[in]  nSubItem    SubItem en el que se ha hecho click (puede ser -1 si no hay item/subitem).
            \param[in]  cX          Coordenada X del mouse.
            \param[in]  cY          Coordenada Y del mouse.
            \return     Debes devolver 0.
    */
    LRESULT DListView::Evento_Mouse_Movimiento(const int nItem, const int nSubItem, const int cX, const int cY) {
        return 0;
    }
    
    //! Función virtual que permite controlar cuando sale el mouse del ListView.
    /*! El objetivo de esta función virtual es poder captar cuando sale el mouse en el ListView al heredar la clase.
            \fn         virtual LRESULT Evento_Mouse_Saliendo(void);
            \return     Debes devolver 0.
    */
    LRESULT DListView::Evento_Mouse_Saliendo(void) {
        return 0;
    }


    //! Función que es llamada al recibir el mensaje WM_MOUSEMOVE en el ListView.
    /*! Esta función de uso interno en esencia comprueba si debe cambiar el mouse cuando se para por un item/subitem que tiene la edición activada.
            \fn         LRESULT _Evento_Mouse_Movimiento(const int cX, const int cY, const UINT Param);
            \param[in]  cX      Coordenada X del mouse.
            \param[in]  cY      Coordenada Y del mouse.
            \param[in]  Param   WPARAM.
            \return     Siempre devuelve 0.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
    LRESULT DListView::_Evento_Mouse_Movimiento(const int cX, const int cY, const UINT Param) {
        // Detección del item y del subitem bajo el mouse
		int				HT = -1;
		LVHITTESTINFO   LInfo;
		LInfo.pt.x = cX;
		LInfo.pt.y = cY;
		LInfo.flags = LVHT_ABOVE | LVHT_TOLEFT;
		HT = ListView_SubItemHitTest(hWnd(), &LInfo);

        if (HT == -1 || LInfo.iSubItem == -1) 
            return Evento_Mouse_Movimiento(HT, LInfo.iSubItem, cX, cY);

        if (_Items[HT]->_SubItems[LInfo.iSubItem].TipoEdicion != DEnum_ListView_TipoEdicion_SinEdicion) 
            DMouse::CambiarCursor(DMouse_Cursor_Mano);
        else 
			DMouse::CambiarCursor();

        return Evento_Mouse_Movimiento(HT, LInfo.iSubItem, cX, cY);
    }

    //! Función que es llamada al recibir el mensaje WM_MOUSELEAVE en el ListView.
    /*! Esta función de uso interno en esencia devuelve el mouse a su estado original cuando sale del control.
            \fn         LRESULT _Evento_Mouse_Saliendo(void);
            \return     Siempre devuelve 0.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
    LRESULT DListView::_Evento_Mouse_Saliendo(void) {
		DMouse::CambiarCursor();
        return Evento_Mouse_Saliendo();
    }

    //! Función que es llamada cuando el ListView notifica de un click a su padre.
    /*! Esta función de uso interno comprueba si se ha pichado encima de un item/subitem que tenga un tipo de edición asignada, para empezar la edición.
            \fn         LRESULT _Evento_Mouse_BotonSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);
            \param[in]  Boton   Boton del mouse (puede ser 0 o 1. L/R)
            \param[in]  cX      Coordenada X del mouse.
            \param[in]  cY      Coordenada Y del mouse.
            \param[in]  Param   WPARAM.
            \return     Siempre devuelve 0.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
    LRESULT DListView::_Evento_Mouse_BotonSoltado(const UINT Boton, const int cX, const int cY, const UINT Param) {
        int				HT = -1;
	    LVHITTESTINFO   LInfo;
	    LInfo.pt.x = cX;
	    LInfo.pt.y = cY;
	    LInfo.flags = LVHT_ABOVE | LVHT_TOLEFT;
		HT = ListView_SubItemHitTest(hWnd(), &LInfo);

        CancelarEdicion();
        if (HT == -1 || LInfo.iSubItem == -1) 
            return Evento_Mouse_Click(HT, LInfo.iSubItem, Boton, cX, cY);

        // Si se ha hecho click en el icono no hacemos nada.
        RECT RectaIcono;
		ListView_GetItemRect(hWnd(), HT, &RectaIcono, LVIR_ICON);
        if (PtInRect(&RectaIcono, LInfo.pt) == TRUE) 
            return Evento_Mouse_Click(HT, LInfo.iSubItem, Boton, cX, cY);

		if (SendMessage(hWnd(), LVM_GETVIEW, 0, 0) == LV_VIEW_TILE && MostrarSubItemsMosaico() > 0) { // Si es la vista mosaico y se muestra algun subitem
            // Obtengo la X donde empezaria el primer subitem y vuelvo a pasarle el HitTest pero con la X relativa al inicio del primer subitem
            // De esta forma si hacemos click a la misma altura del subitem pero no dentro del texto tambien lo detectara como un subitem
            RECT RectaSubItem;
			ListView_GetSubItemRect(hWnd(), HT, 1, LVIR_LABEL, &RectaSubItem);
            LInfo.pt.x = RectaSubItem.left + 3;

			HT = ListView_SubItemHitTest(hWnd(), &LInfo);
        }

        if (_Items[HT]->_SubItems[LInfo.iSubItem].TipoEdicion != DEnum_ListView_TipoEdicion_SinEdicion && Boton == 0) {
            EmpezarEdicion(HT, LInfo.iSubItem, _Items[HT]->_SubItems[LInfo.iSubItem].TipoEdicion);
        }
        
        return Evento_Mouse_Click(HT, LInfo.iSubItem, Boton, cX, cY);
    }

    //! Función para activar / desactivar el ListView.
    /*!	Esta función activa / desactiva el ListView.
		    \fn			void Activar(const bool nActivar);
		    \param[in]	nActivar	: Parametro en el que especificaremos si queremos activar o desactivar el ListView
		    \return		No devuelve nada.
    */
    const BOOL DListView::Activar(const BOOL nActivar) {
        CancelarEdicion();
        return DhWnd::Activar(nActivar);
    }

    //! Función para buscar la posición de una clase DListView_Item dentro del vector de items de este ListView.
    /*!	Esta función se usa para buscar la posición de una clase DListView_Item dentro del vector de items de este ListView.
		    \fn			void BuscarItem(DListView_Item *bItem);
		    \param[in]	bItem : Clase DListView_Item de la cual queremos saber la posición dentro del vector de este ListView.
		    \return		Devuelve la posicion que ocupa la clase DListView_Item especificada, o -1 si no existe.
    */
    const int DListView::BuscarItem(DListView_Item *bItem) {
        for (int i = 0; i < static_cast<int>(_Items.size()); i++) {
            if (_Items[i] == bItem) return i;
        }
        return -1;
    }


    //! Función que indica al listview que debe repintar un item con sus subitems.
    /*! Esta función de uso interno utiliza ListView_RedrawItems para forzar su repintado.
            \fn         void RepintarItem(DListView_Item *nItem);
            \param[in]  nItem       Clase DListView_Item que identifica al item que queremos repintar.
            \return     No devuelve nada.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
    void DListView::RepintarItem(DListView_Item *nItem) {
        int bItem = BuscarItem(nItem);
		ListView_RedrawItems(hWnd(), bItem, bItem);
    };

    //! Función que indica al listview que debe repintar un item con sus subitems.
    /*! Esta función de uso interno utiliza ListView_RedrawItems para forzar su repintado.
            \fn         void RepintarItem(const int nPos);
            \param[in]  nPos       Posicion del item que queremos repintar.
            \return     No devuelve nada.
            \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
    */
    void DListView::RepintarItem(const int nPos) {
		ListView_RedrawItems(hWnd(), nPos, nPos);
    };



    //! Función que retorna los estilos del ListView.
    /*  Esta función retorna todos los estilos del ListView.
            \fn         DWORD ObtenerEstilos(void);
            \return     Devuelve los estilos del ListView.
    */
    DWORD DListView::ObtenerEstilos(void) {
		return static_cast<DWORD>(GetWindowLongPtr(hWnd(), GWL_STYLE));
    }


    //! Función para asignar los estilos del ListView.
    /*  Esta función permite asignar uno o mas estilos mediante el parametro nEstilos previamente preparado
            \fn         void AsignarEstilos(DWORD nEstilos)
            \param[in]  nEstilos : DWORD que contiene los nuevos estilos a asignar.
            \return     No devuelve nada.
            \remarks    Esta función esta pensada para modificar todos los estilos de golpe, por lo que si quieres modificar un solo estilo conservando los valores de los demas,
                        primero deberas obtener los estilos mediante la función ObtenerEstilos(), para luego modificar lo que te interese, y finalmente pasar los estilos a esta función
    */
    void DListView::AsignarEstilos(DWORD nEstilos) { 
		SetWindowLongPtr(hWnd(), GWL_STYLE, nEstilos);
    };


    //! Función que retorna los estilos extendidos del ListView.
    /*  Esta función retorna todos los estilos extendidos del ListView.
            \fn         DWORD ObtenerEstilosEx(void);
            \return     Devuelve los estilos del ListView.
    */
    DWORD DListView::ObtenerEstilosEx(void) {
		return ListView_GetExtendedListViewStyle(hWnd());
    }


    //! Función para asignar los estilos extendidos del ListView
    /*  Esta función permite asignar uno o mas estilos extendidos mediante el parametro nEstilos previamente preparado
            \fn         void AsignarEstilosEx(DWORD nEstilosEx)
            \param[in]  nEstilosEx : DWORD que contiene los nuevos estilos a asignar.
            \return     No devuelve nada.
            \remarks    Esta funcion esta pensada para modificar todos los estilos extendidos de golpe, por lo que si quieres modificar un solo estilo conservando los valores de los demas,
                        primero deberas obtener los estilos mediante la función ObtenerEstilos(), para luego modificar lo que te interese, y finalmente pasar los estilos a esta función
    */
    void DListView::AsignarEstilosEx(DWORD nEstilosEx) { 
		ListView_SetExtendedListViewStyleEx(hWnd(), DWL_ESTILOS_EX_LISTVIEW, nEstilosEx);
    };


    //! Función para cambiar el tipo de vista del ListView.
    /*!	Esta función se usa para cambiar la visualizacion de los items dentro del ListView.
		    \fn			void CambiarVista(const DEnum_ListView_Vista nVista);
		    \param[in]	nVista : Nueva vista que utilizara el ListView, que puede ser : DEnum_ListView_Vista_Icono, DEnum_ListView_Vista_IconoPeque, DEnum_ListView_Vista_Detalles, DEnum_ListView_Vista_Lista, y DEnum_ListView_Vista_Mosaico.
		    \return		No devuelve nada.
            \remarks    Para los tipos de vista DEnum_ListView_Vista_Icono y DEnum_ListView_Vista_Mosaico se utiliza el imagelist del tipo DEnum_ListView_ImageList_Normal, y para los demas tipos de vista se utiliza el imagelist del tipo DEnum_ListView_ImageList_Peque
    */
    void DListView::CambiarVista(const DEnum_ListView_Vista nVista) {
        CancelarEdicion();
		SendMessage(hWnd(), LVM_SETVIEW, static_cast<WPARAM>(nVista), 0);
    };

    //! Función para obtener cuantos subitems se mostraran en la vista mosaico.
    /*! Esta función se utiliza para obtener cuantos subitems se mostraran en la vista mosaico.
            \fn         const UINT MostrarSubItemsMosaico(void);
            \return     Devuelve el numero de subitems que se mostraran. Si devuelve 0 solo se mostrara el item.
            \remarks    Por defecto la vista mosaico no mostrara subitems.
            \sa         CambiarVista().
    */
    const UINT DListView::MostrarSubItemsMosaico(void) {
        LVTILEVIEWINFO Info;        
        Info.cbSize = sizeof(LVTILEVIEWINFO);
        Info.dwMask = LVTVIM_COLUMNS;
        Info.cLines = 0;
		ListView_GetTileViewInfo(hWnd(), &Info);
        return Info.cLines;
    }

    //! Función para indicar al ListView cuantos subitems debe mostrar en la vista mosaico.
    /*! Esta función se utiliza para indicar al ListView cuantos subitems debe mostrar en la vista mosaico.
            \fn         void MostrarSubItemsMosaico(const UINT NumSubItems);
			\param[in]  NumSubItems     Numero de subitems que se mostraran en la vista mosaico.
			\param[in]  AnchoEtiqueta   Ancho en pixeles para la etiqueta del mosaico.
			\param[in]  AltoEtiqueta    Altura en pixeles para la etiqueta del mosaico.
			\return     No devuelve nada.
            \remarks    Por defecto la vista mosaico no mostrara subitems.
            \sa         CambiarVista().
    */
    void DListView::MostrarSubItemsMosaico(const UINT NumSubItems, const LONG AnchoEtiqueta, const LONG AltoEtiqueta) {
        LVTILEVIEWINFO Info;
        Info.cbSize = sizeof(LVTILEVIEWINFO);
		Info.dwMask = LVTVIM_COLUMNS | LVTVIM_TILESIZE;
        Info.cLines = NumSubItems;
		Info.sizeTile.cx = AnchoEtiqueta;
		Info.sizeTile.cy = AltoEtiqueta;
		Info.dwFlags = ((Info.sizeTile.cx > 0) ? LVTVIF_FIXEDWIDTH : 0) + ((Info.sizeTile.cy > 0) ? LVTVIF_FIXEDHEIGHT : 0);
		ListView_SetTileViewInfo(hWnd(), &Info);
    }

    //! Función para asignar un ImageList al ListView.
    /*  Esta función se utiliza para asignar un ImageList al ListView.
            \fn         void AsignarImageList(DImageList *nListaImagenes, const DEnum_ListView_ImageList nTipo);
            \param[in]  nListaImagenes : Puntero a una clase DImageList con la lista de imagenes que queremos asignar. Si especificas NULL des-asignaras el ultimo imagelist enlazado del mismo tipo
            \param[in]  nTipo          : Tipo de ImageList a asginar, que puede ser : DEnum_ListView_ImageList_Normal, DEnum_ListView_ImageList_Peque, DEnum_ListView_ImageList_Estado, DEnum_ListView_ImageList_GrupoCabecera
            \return     Esta función no devuelve nada.
            \remarks    Si no utilizas el estilo LVS_SHAREIMAGELISTS el imagelist sera borrado al destruir el ListView, por lo que si lo estas utilizando en otro control podrias ocasionar algun error.
    */
    void DListView::AsignarImageList(DImageList *nListaImagenes, const DEnum_ListView_ImageList nTipo) {
        HIMAGELIST nImg = NULL;
        if (nListaImagenes != NULL) nImg = nListaImagenes->operator ()();
		ListView_SetImageList(hWnd(), nImg, static_cast<int>(nTipo));
    }

};

