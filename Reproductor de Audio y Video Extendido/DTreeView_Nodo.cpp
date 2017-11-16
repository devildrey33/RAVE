#include "stdafx.h"
#include "DTreeView.h"

namespace DWL {

/*	const DEnum_Objeto DTreeView_Nodo::Objeto_Tipo(void) { 
		return DEnum_Objeto_TreeView_Nodo; 
	};*/

	DTreeView_Nodo::DTreeView_Nodo(void) : _ID(NULL), _TV(NULL) {		
	}

	DTreeView_Nodo::~DTreeView_Nodo(void) {	
	}

	const BOOL DTreeView_Nodo::AsignarTexto(std::wstring nTexto) {
		return AsignarTexto(nTexto.c_str(), nTexto.size());
	};

	const BOOL DTreeView_Nodo::AsignarTexto(const wchar_t *nTexto, size_t nTotalCaracteres) {
		if (nTotalCaracteres == 0) nTotalCaracteres = wcslen(nTexto);
		TCHAR *Tmp = new TCHAR[nTotalCaracteres + 1];
		wcscpy_s(Tmp, nTotalCaracteres + 1, nTexto);
		TVITEMEX Nodo;
		Nodo.mask = TVIF_HANDLE | TVIF_TEXT;
		Nodo.pszText = Tmp;
		Nodo.cchTextMax = static_cast<int>(nTotalCaracteres);
		Nodo.hItem = this->_ID;
		BOOL R = TreeView_SetItem(_TV->hWnd(), &Nodo);
		delete[] Tmp;
		return R;
	};

	const BOOL DTreeView_Nodo::ObtenerTexto(std::wstring &nTexto) {
		static TCHAR	Tmp[256];
		TVITEMEX		TvItem;
		TvItem.mask = TVIF_HANDLE | TVIF_TEXT;
		TvItem.pszText = Tmp;
		TvItem.cchTextMax = 256;
		TvItem.hItem = _ID;
		BOOL R = TreeView_GetItem(_TV->hWnd(), &TvItem);
		nTexto = Tmp;
		return R;
	}

	const BOOL DTreeView_Nodo::Icono(const int nIcono) {
		TVITEMEX Nodo;
		Nodo.mask = TVIF_HANDLE | TVIF_IMAGE;
		Nodo.hItem = this->_ID;
		Nodo.iImage = nIcono;
		BOOL R = TreeView_SetItem(_TV->hWnd(), &Nodo);
		return R;
	};

	const int DTreeView_Nodo::Icono(void) {
		TVITEMEX		TvItem;
		TvItem.mask = TVIF_HANDLE | TVIF_IMAGE;
		TvItem.hItem = _ID;
		BOOL R = TreeView_GetItem(_TV->hWnd(), &TvItem);
		return TvItem.iImage;
	}


	DTreeView_Nodo *DTreeView_Nodo::Padre(void) {
		HTREEITEM Padre = TreeView_GetParent(_TV->hWnd(), _ID);
		return _ObtenerLPARAM(Padre);
	}

	DTreeView_Nodo *DTreeView_Nodo::Hijo(void) {
		HTREEITEM Hijo = TreeView_GetChild(_TV->hWnd(), _ID);
		return _ObtenerLPARAM(Hijo);
	};

	DTreeView_Nodo *DTreeView_Nodo::Siguiente(void) {
		HTREEITEM Siguiente = TreeView_GetNextItem(_TV->hWnd(), _ID, TVGN_NEXT);
		return _ObtenerLPARAM(Siguiente);
	};

	DTreeView_Nodo *DTreeView_Nodo::Anterior(void) {
		HTREEITEM Anterior = TreeView_GetNextItem(_TV->hWnd(), _ID, TVGN_PREVIOUS);
		return _ObtenerLPARAM(Anterior);
	};

	DTreeView_Nodo *DTreeView_Nodo::_ObtenerLPARAM(HTREEITEM nItem) {
		if (nItem == NULL) return NULL;
		TVITEMEX Nodo;
		Nodo.mask = TVIF_HANDLE | TVIF_PARAM;
		Nodo.hItem = nItem;
		if (TreeView_GetItem(_TV->hWnd(), &Nodo) == FALSE) return NULL;
		return static_cast<DTreeView_Nodo *>(reinterpret_cast<void *>(Nodo.lParam));
	}


};