#include "stdafx.h"
#include "DControlDesplegableEx.h"

#define TAM_ICONO     16

#define ID_BOTON	1000
#define ID_EDICION	1001

DControlDesplegableEx::DControlDesplegableEx(void) {
}


DControlDesplegableEx::~DControlDesplegableEx(void) {
}


void DControlDesplegableEx::CrearControlDesplegable(DhWnd *nPadre, const wchar_t *nTexto, const INT_PTR nID, const INT_PTR nIDIcono, const int cX, const int cY, const int cAncho, const int cAlto) {
	CrearControlEx(nPadre, L"DControlDesplegable", nTexto, nID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
	_Boton.CrearBotonEx(this, L"\\/", cAncho - cAlto, 0, cAlto, cAlto, ID_BOTON);
	_Icono = DListaIconos::AgregarIconoRecursos(nIDIcono, TAM_ICONO, TAM_ICONO);
	_Edicion.CrearEdicionTextoEx(this, nTexto, 0, 0, cAncho - cAlto, cAlto, ID_EDICION, _Icono);
}

LRESULT CALLBACK DControlDesplegableEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_BOTONEX_CLICK:
			Evento_Desplegar();
			return 0;
	}
	return DControlEx::GestorMensajes(uMsg, wParam, lParam);
}