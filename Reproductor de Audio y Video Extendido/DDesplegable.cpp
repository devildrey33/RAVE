#include "stdafx.h"
#include "DDesplegable.h"


DDesplegable::DDesplegable() {
}


DDesplegable::~DDesplegable() {
}


LRESULT CALLBACK DDesplegable::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}