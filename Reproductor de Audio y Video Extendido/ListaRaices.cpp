#include "stdafx.h"
#include "ListaRaices.h"
#include "resource.h"

ListaRaices::ListaRaices(void) : DListaEx(), _IconoPapelera(NULL), _IconoPapeleraResaltado(NULL), _ItemRaizPresionado(-1), _SubItemRaizPresionado(-1) {

}


ListaRaices::~ListaRaices(void) {
}


void ListaRaices::CrearListaRaiz(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearListaEx(nPadre, cX, cY, cAncho, cAlto, cID);
	/* TODO : Afegir columna per mostrar el percentatge de escaneig (2 fases : enumeració de medis, obtenció de dades de cada medi) */
	AgregarColumna(DLISTAEX_COLUMNA_ANCHO_AUTO);										// Path
	AgregarColumna(16 + (DLISTAEX_PADDING * 2), DListaEx_Columna_Alineacion_Centrado);	// Eliminar
	// Icono de la papelera para borrar la raíz
	_IconoPapeleraResaltado	= DListaIconos::AgregarIconoRecursos(IDI_BORRAR, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO);
	_IconoPapelera			= DListaIconos::AgregarIconoRecursos(IDI_BORRARRESALTADO, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO);

	MostrarSeleccion = FALSE;
}


DListaEx_Item *ListaRaices::AgregarRaiz(const wchar_t *nPath, const BOOL nRepintar) {
	DListaEx_Item *Ret = AgregarItem(nPath, 0, DLISTAEX_POSICION_FIN, nPath, L"");
	if (nRepintar != FALSE) Repintar();
	return Ret;
}


void ListaRaices::Evento_PintarSubItem(HDC hDC, const size_t NumItem, const size_t NumSubItem, RECT *Espacio) {
	if (NumSubItem == 1 && NumItem == _ItemResaltado) {
		int PosY_Ico = 1 + static_cast<int>(((Espacio->bottom - Espacio->top) - DLISTAEX_TAMICONO) / 2);
		int b_Presionado = 0;
		HICON IPapelera = (_SubItemResaltado == 1) ? _IconoPapeleraResaltado->Icono() : _IconoPapelera->Icono();

		if (_ItemPresionado == NumItem) b_Presionado = 1;
		DrawIconEx(_BufferItem, b_Presionado + Espacio->left, b_Presionado + PosY_Ico, IPapelera, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO, 0, 0, DI_NORMAL);
	}
}


void ListaRaices::Evento_MousePresionado(DWL::DEventoMouse &DatosMouse) {
};


void ListaRaices::Evento_MouseSoltado(DWL::DEventoMouse &DatosMouse) {
	DListaEx_Item *nItemPresionado = ItemPresionado();
	if (nItemPresionado == NULL) return;

	if (nItemPresionado == ItemResaltado() && _SubItemResaltado == 1) {
		App.VentanaOpciones.EliminarRaiz(nItemPresionado->Texto());
		EliminarItem(nItemPresionado);
//		Repintar();
	}
};