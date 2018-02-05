#include "stdafx.h"
#include "DTreeView.h"
#include "DMensajesWnd.h"

#define DCX_USESTYLE	0x00010000
#define DCX_NODELETERGN 0x00040000

namespace DWL {

	//const DEnum_Objeto DTreeView::Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_TreeView; };

	DTreeView::DTreeView(void) {
		static bool Cargado = false;
		if (Cargado == false) {
			INITCOMMONCONTROLSEX Comm;
			Comm.dwSize = sizeof(INITCOMMONCONTROLSEX);
			Comm.dwICC = ICC_TREEVIEW_CLASSES;
			InitCommonControlsEx(&Comm);
			Cargado = true;
		}
	};

	DTreeView::~DTreeView(void) {
		BorrarTodo();
	};


	void DTreeView::BorrarTodo(void) {
		TreeView_DeleteAllItems(hWnd());
		for (size_t i = 0; i < _Nodos.size(); i++) {
			delete _Nodos[i];
		}
		_Nodos.resize(0);
	}

	//! Función para crear el TreeView (CreateWindowEx).
	/*! Esta función se usa para crear un TreeView dinamicamente.
	\fn         HWND Crear(DBaseWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos = NULL);
	\param[in]  nPadre               Clase DBaseWnd que va a hacer de padre para el TreeView
	\param[in]  cX                   Coordenada X relativa a la ventana padre.
	\param[in]  cY                   Coordenada Y relativa a la ventana padre.
	\param[in]  cAncho               Ancho del TreeView en pixeles.
	\param[in]  cAlto                Altura del TreeView en pixeles
	\param[in]  cID                  ID para poder identificar el TreeView en sus eventos.
	\param[in]  nEstilos             Estilos estandar para el TreeView.
	\param[in]  nEstilosExtendidos   Estilos extendidos para el TreeView.
	\return     Devuelve el HWND del TreeView o NULL en caso de error.
	*/
	HWND DTreeView::Crear(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos) {
		if (hWnd()) { Debug_Escribir(L"DTreeView::Crear() Error : ya se ha creado el treeview\n");  return hWnd(); }
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
		hWnd = CreateWindowEx(nEstilosExtendidos, WC_TREEVIEW, NULL, nEstilos, cX, cY, cAncho, cAlto, hWndPadre, reinterpret_cast<HMENU>(IntToPtr(cID)), GetModuleHandle(NULL), this);
		_ConectarControl(cID, nPadre);
		return hWnd();
	}


	//! Función para asignar esta clase a un TreeView de un dialogo.
	/*! Esta función se usa para asignar esta clase a un TreeView de un dialogo.
	\fn         HWND Asignar(DBaseWnd *Padre, const UINT cID);
	\param[in]  nPadre      Clase DBaseWnd que va a hacer de padre para el TreeView
	\param[in]  cID         ID para poder identificar el TreeView en sus eventos.
	\return     Devuelve el HWND del TreeView o NULL en caso de error.
	\remarks    Esta función solo debe utilizarse si tenemos un TreeView en un dialogo de los recursos.
	*/
	HWND DTreeView::Asignar(DhWnd *nPadre, const UINT cID) {
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
		hWnd = GetDlgItem(hWndPadre, cID);
		_ConectarControl(cID, nPadre);
		return hWnd();
	}

	DTreeView_Nodo *DTreeView::AgregarNodo(DTreeView_Nodo *nNodo, DTreeView_Nodo *nPadre, const TCHAR *cTexto, const int cIko, DTreeView_Nodo *InsertarDespuesDe, const UINT Estado, const UINT MascaraEstado) {
		TVINSERTSTRUCT ITS;
		TVITEMEX       Item;
		static TCHAR   Tmp[512];
		nNodo->_TV = this;
		wcscpy_s(Tmp, 512, cTexto);
		Item.pszText = Tmp;
		Item.cchTextMax = static_cast<int>(wcslen(Tmp));
		Item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE | TVIF_CHILDREN;
		Item.iImage = cIko;
		Item.iSelectedImage = cIko;
		Item.state = Estado;
		Item.stateMask = MascaraEstado;
		//												Item.cChildren		= 0;
		Item.lParam = static_cast<LPARAM>(PtrToLong(nNodo));
		//												Item.lParam			= (LPARAM)this;
		ITS.hInsertAfter = (InsertarDespuesDe) ? InsertarDespuesDe->_ID : TVI_LAST;
		ITS.itemex = Item;
		ITS.hParent = (nPadre) ? nPadre->_ID : TVI_ROOT;

		//												HTREEITEM nID = TreeView_InsertItem(_hWnd, &ITS);
		static_cast<DTreeView_Nodo *>(nNodo)->_ID = TreeView_InsertItem(hWnd(), &ITS);
		_Nodos.push_back(nNodo);
		return nNodo;
	}


	DTreeView_Nodo *DTreeView::AgregarNodo(DTreeView_Nodo *nPadre, const TCHAR *cTexto, const int cIko, DTreeView_Nodo *InsertarDespuesDe, const UINT Estado, const UINT MascaraEstado) {
		DTreeView_Nodo *nNodo = new DTreeView_Nodo;
		return AgregarNodo(nNodo, nPadre, cTexto, cIko, InsertarDespuesDe, Estado, MascaraEstado);
	}

	void DTreeView::BorrarNodo(DTreeView_Nodo *eNodo) {
		TreeView_DeleteItem(hWnd(), eNodo->_ID);
		for (size_t i = 0; i < _Nodos.size(); i++) {
			if (eNodo == _Nodos[i]) {
				delete _Nodos[i];
				_Nodos.erase(_Nodos.begin() + i);
				break;
			}
		}
	}


	//
	//! Evento que es llamado cuando se expande / contrae un nodo del treeview
	//  virtual void TreeView_Evento_Nodo_Expandiendo(DTreeView_Nodo *nNodo);
	const BOOL DTreeView::TreeView_Evento_Nodo_Expandiendo(DTreeView_Nodo *nNodo) { 
		return FALSE; 
	};

	void DTreeView::AsignarImageList(DImageList *nListaImagenes) {
		HIMAGELIST nImg = NULL;
		if (nListaImagenes != NULL) nImg = nListaImagenes->operator ()();
		TreeView_SetImageList(hWnd(), nImg, TVSIL_NORMAL);
	}


	//! Nuevo WindowProcedure que se sobrepondra al WindowProcedure original del TreeView.
	/*! Esta función es la que re-emplaza el WindowProcedure del ListView para poder hacer cosas como la edicion, y el pintado personalizado.
	\fn         virtual LRESULT CALLBACK GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam)
	\param[in]  uMsg        Mensaje
	\param[in]  wParam      Parametro wide
	\param[in]  lParam      Parametro long
	\return     Dependiendo del mensaje procesado devolvera una cosa u otra...
	\remarks    Hacer un SendMessage al HWND de este ListView o utilizar esta función es exactamente lo mismo.
	*/
	LRESULT CALLBACK DTreeView::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		HTREEITEM hID;
		DTreeView_Nodo *NodoTmp = NULL;
		switch (uMsg) {
			case DWL_NOTIFICACION :
				switch (((LPNMHDR)lParam)->code) {
					case NM_CLICK: // nItem, nSubItem, ptAction
					case NM_DBLCLK: // nItem, nSubItem, ptAction
					case NM_RCLICK: // nItem, nSubItem, ptAction
					case NM_RDBLCLK: // nItem, nSubItem, ptAction
						return _NotificacionClick(lParam);
					//                    case NM_HOVER : // (devolver 0 para normal, 1 para cancelar)
					//                        return SendMessage(GetParent(_hWnd), DWL_LISTVIEW_RESALTAR_ITEM, static_cast<WPARAM>(ID()), 0);
				case NM_KILLFOCUS: // nada
					return SendMessage(hWnd.Padre(), DWL_TREEVIEW_FOCO_PERDIDO, static_cast<WPARAM>(hWnd.ID()), 0);
				case NM_RELEASEDCAPTURE: // nada
					return SendMessage(hWnd.Padre(), DWL_TREEVIEW_TERMINAR_CAPTURA, static_cast<WPARAM>(hWnd.ID()), 0);
				case NM_RETURN: // nada
					return SendMessage(hWnd.Padre(), DWL_TREEVIEW_TECLADO_INTRO, static_cast<WPARAM>(hWnd.ID()), 0);
				case NM_SETFOCUS: // nada
					return SendMessage(hWnd.Padre(), DWL_TREEVIEW_FOCO_OBTENIDO, static_cast<WPARAM>(hWnd.ID()), 0);
				}
				break;
			case DWL_TREEVIEW_NODO_EXPANDIENDO :
				hID = ((LPNMTREEVIEW)lParam)->itemNew.hItem;
				for (size_t i2 = 0; i2 < _Nodos.size(); i2++) {
					if (_Nodos[i2]->_ID == hID) {
						NodoTmp = _Nodos[i2];
						break;
					}
				}
				if (this->TreeView_Evento_Nodo_Expandiendo(NodoTmp) == TRUE) return TRUE;
				return SendMessage(hWnd.Padre(), DWL_TREEVIEW_NODO_EXPANDIENDO, (WPARAM)NodoTmp, (LPARAM)hWnd.ID());

				// Este mensaje se recibe una vez borrado el item y se usa para borrar la clase con el nodo de la memoria.
				// El LPARAM es un puntero a la clase DTreeView_Nodo que contiene la información adicional para el nodo, y que debe ser eliminada tanto del vector de nodos como de memória.
			case DWL_TREEVIEW_BORRARNODO :
				NodoTmp = static_cast<DTreeView_Nodo *>(reinterpret_cast<void *>(lParam));
				for (size_t i = 0; i < _Nodos.size(); i++) {
					if (_Nodos[i] == NodoTmp) {
						_Nodos.erase(_Nodos.begin() + i);
						break;
					}
				}
				delete NodoTmp;
				return 0;
		}
		return DControl::_GestorMensajesOriginal(hWnd(), uMsg, wParam, lParam);
	}

	//! Función que procesa las notificaciones NM_CLICK, NM_DBLCLK, NM_RCLICK, NM_RDBLCLK.
	/*! Esta función de uso interno devuelve las notificaciones del tipo click a su ventana padre.
	\fn         LRESULT _NotificacionClick(LPARAM lParam);
	\param[in]  lParam   Datos para la notificación.
	\return     Siempre devuelve 0.
	\remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
	*/
	LRESULT DTreeView::_NotificacionClick(LPARAM lParam) {
		int Boton = 0;
		bool DobleClick = false;
		if (((LPNMHDR)lParam)->code == NM_RCLICK || ((LPNMHDR)lParam)->code == NM_RDBLCLK) Boton = 1;
		if (((LPNMHDR)lParam)->code == NM_DBLCLK || ((LPNMHDR)lParam)->code == NM_RDBLCLK) DobleClick = true;
//		_Evento_Mouse_BotonSoltado(Boton, ((LPNMITEMACTIVATE)lParam)->ptAction.x, ((LPNMITEMACTIVATE)lParam)->ptAction.y, 0);

		TVHITTESTINFO ht;
		RECT          Rect;
		POINT         CoordMouse;
		GetCursorPos(&CoordMouse);
		RECT RV;
		GetWindowRect(hWnd(), &RV);
		CoordMouse.x -= RV.left;
		CoordMouse.y -= RV.top;
		ht.pt = CoordMouse;
		SendMessage(hWnd(), TVM_HITTEST, 0, (LPARAM)(LPTVHITTESTINFO)&ht);
		TreeView_GetItemRect(hWnd(), ht.hItem, &Rect, TRUE);

		DTreeView_Nodo *TmpNodo = _BuscarID(ht.hItem);
		Evento_Mouse_Click(TmpNodo, Boton, CoordMouse.x, CoordMouse.y);

		DTreeView_DatosClick DatosClick(TmpNodo, Boton, CoordMouse.x, CoordMouse.y);
		if (DobleClick == false) return SendMessage(hWnd.Padre(), DWL_TREEVIEW_CLICK, static_cast<WPARAM>(hWnd.ID()), reinterpret_cast<LPARAM>(&DatosClick));
		else                     return SendMessage(hWnd.Padre(), DWL_TREEVIEW_DOBLECLICK, static_cast<WPARAM>(hWnd.ID()), reinterpret_cast<LPARAM>(&DatosClick));
	}


	/*
	DTreeView_Nodo *DTreeView::_HitTest(void) {
		TVHITTESTINFO ht;
		RECT          Rect;
		POINT         hitPt;
		GetCursorPos(&hitPt);
		RECT RV;
		GetWindowRect(_hWnd, &RV);
		hitPt.x -= RV.left;
		hitPt.y -= RV.top;
		ht.pt = hitPt;
		SendMessage(_hWnd, TVM_HITTEST, 0, (LPARAM)(LPTVHITTESTINFO)&ht);
		TreeView_GetItemRect(_hWnd, ht.hItem, &Rect, TRUE);
		if (PtInRect(&Rect, hitPt) != 0) {
			return _BuscarID(ht.hItem);
		}
		return NULL;

	}*/


	DTreeView_Nodo *DTreeView::_BuscarID(HTREEITEM bID) {
		for (size_t i = 0; i < _Nodos.size(); i++) {
			if (_Nodos[i]->_ID == bID) return _Nodos[i];
		}
		return NULL;
	}

	const size_t DTreeView::TotalNodos(void) {
		return _Nodos.size();
	}

	DTreeView_Nodo *DTreeView::Nodo(size_t Pos) {
		return _Nodos[Pos];
	}

	DTreeView_Nodo *DTreeView::PrimerNodo(void) {
		HTREEITEM TvRoot = TreeView_GetRoot(hWnd());
		if (TvRoot == NULL) return NULL;
		TVITEMEX tvItem;
		tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
		tvItem.hItem = TvRoot;
		TreeView_GetItem(hWnd(), &tvItem);
		DTreeView_Nodo *NodoPadre = static_cast<DTreeView_Nodo *>(reinterpret_cast<void *>(tvItem.lParam));
		return NodoPadre;
	}

	/* Función que devuelve el primer texto encontrado en el arbol igual que el parámetro buscar, o null si no lo encuentra
	NOTA : la comparación se hace INSENSITIVE
	Parámetros :
	- Buscar    : Cadena a buscar
	- Padre     : Nodo padre por donde empieza la busqueda, NULL si se busca desde la raíz (valor por defecto).
	- Recursivo : Busqueda recursiva por los hijos (por defecto desactivada)
	*/

	DTreeView_Nodo *DTreeView::BuscarTexto(std::wstring &Buscar, DTreeView_Nodo *Padre, const BOOL Recursivo) {
		DTreeView_Nodo *Ret = NULL;
		//			HTREEITEM		Hijo;
		TVITEMEX		TvItem, TvItemH;
		TCHAR			Tmp[256];
		BOOL			R = FALSE;
		//			if (bNodo != NULL) BuscarDesde = bNodo->_ID;
		TvItem.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
		TvItem.pszText = Tmp;
		TvItem.cchTextMax = 256;
		TvItem.hItem = (Padre) ? TreeView_GetChild(hWnd(), Padre->_ID) : TreeView_GetRoot(hWnd());
		TvItemH.mask = TVIF_HANDLE | TVIF_PARAM;
		TvItemH.hItem = TVI_ROOT;
		R = TreeView_GetItem(hWnd(), &TvItem);
		if (R == FALSE) return NULL;

		do {
			// Tiene el mismo texto
			if (_wcsicmp(Buscar.c_str(), TvItem.pszText) == 0) {
				return static_cast<DTreeView_Nodo *>(reinterpret_cast<void *>(TvItem.lParam));
			}
			// Si tiene hijos busco en ellos
			if (TvItem.cChildren && Recursivo == TRUE) {
				TvItemH.hItem = TreeView_GetChild(hWnd(), TvItem.hItem);
				R = TreeView_GetItem(hWnd(), &TvItemH);
				Ret = BuscarTexto(Buscar, static_cast<DTreeView_Nodo *>(reinterpret_cast<void *>(TvItemH.lParam)));
				if (Ret) return Ret;
			}
			// Obtengo el sigiente nodo
			TvItem.hItem = TreeView_GetNextItem(hWnd(), TvItem.hItem, TVGN_NEXT);
			R = TreeView_GetItem(hWnd(), &TvItem);
		} while (TvItem.hItem != NULL);

		return NULL;
	};


};