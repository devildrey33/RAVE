#include "stdafx.h"
#include "ListaMedios2.h"


ListaMedios2::ListaMedios2() {
}


ListaMedios2::~ListaMedios2() {
}


/*TablaMedios_Medio *nMedio*/
ItemMedio2 *ListaMedios2::AgregarMedio(const int nIcono, const TCHAR *nPista, const TCHAR *nNombre, const TCHAR *nTiempo, const sqlite3_int64 nHash) {


	// Busco si existe el hash para no agregar 2 medios iguales a la lista
	for (size_t i = 0; i < _Items.size(); i++) {
		if (static_cast<ItemMedio2 *>(_Items[i])->Hash == nHash) {
			return Medio(i); // Devuelvo el medio existente
		}
	}


	ItemMedio2 *nMedio = AgregarItem<ItemMedio2>(nIcono, DLISTAEX_POSICIONFILA_FIN, nPista, nNombre, nTiempo);
	nMedio->Hash = nHash;
	return nMedio;
}