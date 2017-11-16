#pragma once

#include <vector>
#include <Windows.h>

namespace DWL {
	class DMenu {
	  public:
										DMenu(void);
										DMenu(HMENU nMenu, const int nID, DMenu *nPadre, const bool Raiz);
									   ~DMenu(void);

		HMENU							CrearMenu(void);
		void							BorrarTodo(void);

		DMenu                          *CrearMenuItem(const int nID, const wchar_t *Texto, const int nPos = -1);

		DMenu                          *CrearSubMenu(const int nID, const wchar_t *Texto, const int nPos = -1);
		DMenu						   *CrearSeparador(const int nPos = -1);

		const BOOL						Mostrar(HWND hWndDest);

		const int						ID(void);
		HMENU							hMenu(void);

		void							Activado(const BOOL Activar);

		inline DMenu                   *operator[] (const size_t nPos)	{ return &_Items[nPos]; }
		inline HMENU			        operator() (void)				{ return _hMenu; }
   	  protected:
		HMENU                          _hMenu;
		int                            _ID;
		//			DMenu                         *_MenuPadre;
		DMenu                         *_Padre;
		bool						   _Raiz;

		std::vector<DMenu>             _Items;

	};

};