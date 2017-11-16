#include "stdafx.h"
#include "DMenu.h"

namespace DWL {

	DMenu::DMenu(void) : _Padre(NULL), _hMenu(NULL), _ID(0), _Raiz(false) 																{				}
	DMenu::DMenu(HMENU nMenu, const int nID, DMenu *nPadre, const bool nRaiz) : _Padre(nPadre), _hMenu(nMenu), _ID(nID), _Raiz(nRaiz)	{				}
	DMenu::~DMenu(void)																													{ BorrarTodo(); }
		
	HMENU DMenu::CrearMenu(void) {
		if (_hMenu != NULL) BorrarTodo();
		_hMenu = CreatePopupMenu();
		_Padre = NULL;
		return _hMenu;
	}
	void DMenu::BorrarTodo(void) {
		if (_Raiz == true) {
			if (_hMenu != NULL) DestroyMenu(_hMenu);
			_hMenu = NULL;
		}
		_Raiz = false;
		_ID = 0;
	}

	DMenu *DMenu::CrearMenuItem(const int nID, const wchar_t *Texto, const int nPos) {
		int           Posicion = nPos;
		wchar_t       NTexto[256];
		MENUITEMINFO  MenuInfo;
		DMenu         MenuItem;
		wcscpy_s(NTexto, 256, Texto);
		if (Posicion == -1 || Posicion > static_cast<int>(_Items.size()) - 1) Posicion = static_cast<int>(_Items.size());

		MenuItem._ID = nID;
		MenuItem._Padre = this;

		//		for (unsigned int i = Posicion; i < _Items.size(); i++) { _Items[i]->_Pos++; }

		MenuInfo.cbSize		= sizeof(MENUITEMINFO);
		MenuInfo.fMask		= MIIM_DATA | MIIM_ID | MIIM_SUBMENU | MIIM_TYPE;
		MenuInfo.wID		= nID;
		MenuInfo.hSubMenu	= NULL;
		MenuInfo.fType		= MFT_STRING;
		MenuInfo.cch		= static_cast<UINT>(wcslen(NTexto));
		MenuInfo.dwTypeData = NTexto;
		BOOL R = InsertMenuItem(_hMenu, Posicion, TRUE, &MenuInfo);
		/*		if (IconoMarcado != -1)   { _Check = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IconoMarcado)); }
		if (IconoDesMarcado != -1) { _UnCheck = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IconoDesMarcado)); }
		SetMenuItemBitmaps(MenuItem->_Parent, Posicion, MF_BYPOSITION, _Check, _UnCheck);*/

		_Items.insert(_Items.begin() + Posicion, MenuItem);
		return &_Items[Posicion];
	}

	DMenu *DMenu::CrearSubMenu(const int nID, const wchar_t *Texto, const int nPos) {
		return NULL;
	}

	DMenu *DMenu::CrearSeparador(const int nPos) {
		return NULL;
	}

	const BOOL DMenu::Mostrar(HWND hWndDest) {
		POINT Point;
		GetCursorPos(&Point);
		return TrackPopupMenu(_hMenu, TPM_LEFTALIGN | TPM_RETURNCMD, Point.x, Point.y, NULL, hWndDest, NULL);
	}

	const int DMenu::ID(void) { 
		return _ID; 
	};
	
	HMENU DMenu::hMenu(void) { 
		return _hMenu; 
	};

	void DMenu::Activado(const BOOL Activar) {
		MENUITEMINFO  MenuInfo;
		MenuInfo.cbSize = sizeof(MENUITEMINFO);
		MenuInfo.fMask = MIIM_STATE;
		MenuInfo.fState = (Activar) ? MFS_ENABLED : MFS_DISABLED;
		SetMenuItemInfo(_Padre->operator()(), _ID, FALSE, &MenuInfo);
	}
};