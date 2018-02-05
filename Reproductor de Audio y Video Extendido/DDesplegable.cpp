#include "stdafx.h"
#include "DDesplegable.h"

namespace DWL {

	DDesplegable::DDesplegable() {
	}


	DDesplegable::~DDesplegable() {
	}


	HWND DDesplegable::CrearDesplegable(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const DDesplegable_Tipo nTipo) {
		if (hWnd()) { Debug_Escribir(L"DDesplegable::CrearDesplegable() Error : ya se ha creado el control desplegable\n"); return hWnd(); }
		hWnd = CrearControlEx(nPadre, L"DDesplegable", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD, NULL, CS_DBLCLKS);  // CS_DBLCLKS (el control recibe notificaciones de doble click)
		_Fuente = hWnd._Fuente;
		return hWnd();

	}


	void DDesplegable::MostrarDesplegable(void) {

	}

	LRESULT CALLBACK DDesplegable::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}

}