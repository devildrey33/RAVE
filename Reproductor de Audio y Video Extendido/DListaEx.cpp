#include "stdafx.h"
#include "DListaEx.h"

namespace DWL {

	DListaEx::DListaEx(void) {
	}


	DListaEx::~DListaEx(void) {
		EliminarTodasLasColumnas();
		EliminarTodosLosItems();
	}

	/* Al crear el control DListaEx se crea una columna con un ancho automático alineada a la derecha por defecto, si deseas cambiar sus propiedades utiliza la función Columna(Pos)->Porpiedad */
	HWND DListaEx::CrearListaEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
		if (hWnd()) { Debug_Escribir(L"DListaEx::CrearListaEx() Error : ya se ha creado la lista\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DListaEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL, CS_DBLCLKS); // CS_DBLCLKS (el control recibe notificaciones de doble click)
		_Fuente = hWnd._Fuente;
		AgregarColumna(DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion_Derecha);
		return hWnd();
	}

	void DListaEx::AgregarColumna(const int nAncho, DListaEx_Columna_Alineacion nAlineacion) {
		DListaEx_Columna *nColumna = new DListaEx_Columna(nAlineacion, nAncho);
		_Columnas.push_back(nColumna);
	}

	DListaEx_Item *DListaEx::_AgregarItem(const int nIcono, DhWnd_Fuente *nFuente, const size_t PosicionItem, const TCHAR *nTxt, va_list Marker) {
		DListaEx_Item		*nItem		= new DListaEx_Item();
		DListaEx_SubItem	*nSubItem	= NULL;
		for (size_t i = 0; i < _Columnas.size(); i++) {
			nSubItem = new DListaEx_SubItem(static_cast<const TCHAR *>(va_arg(Marker, const TCHAR *)));
			nItem->_SubItems.push_back(nSubItem);
		}

		_Items.push_back(nItem);
		//_Items.resize(0);
		return nItem;
	}

	void DListaEx::EliminarTodosLosItems(void) {
		for (size_t i = 0; i < _Items.size(); i++) {
			delete _Items[i];
		}
	}

	void DListaEx::EliminarTodasLasColumnas(void) {
		for (size_t i = 0; i < _Columnas.size(); i++) {
			delete _Columnas[i];
		}
	}


	void DListaEx::Pintar(HDC hDC) {

	}

	void DListaEx::PintarItem(HDC hDC, DListaEx_Item *nItem, RECT &Espacio) {

	}

	LRESULT CALLBACK DListaEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				Pintar(DC);
				EndPaint(hWnd(), &PS);
				return 0;

		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}


}