#include "stdafx.h"
#include "ListaMedios.h"
#include "DStringUtils.h"
#include "resource.h"

ListaMedios::ListaMedios() : Pos(0) {
}


ListaMedios::~ListaMedios() {
}


void ListaMedios::BorrarListaReproduccion(void) {
	App.VLC.Stop();
	App.VLC.CerrarMedio();

	BorrarTodosLosItems();
	Pos = 0;
}

void ListaMedios::AgregarMedio(TablaMedios_Medio *nMedio) {
	ItemMedio *TmpMedio = new ItemMedio;
	TmpMedio->Hash = nMedio->Hash();
	int nIcono = 3;
	if (nMedio->TipoMedio() == Tipo_Medio::Tipo_Medio_Video) nIcono = 4;
	AgregarItem(TmpMedio, nIcono, -1, -1, DWL::DString_ToStr(nMedio->Pista(), 2).c_str(), nMedio->Nombre(), L"00:00");
//	AgregarItem(TmpMedio, 0, -1, -1, DWL::DString_ToStr(Pista, 2).c_str(), Nombre, Disco, Grupo, Genero, Tiempo);

}

ItemMedio *ListaMedios::BuscarHash(sqlite3_int64 bHash) {
	for (size_t i = 0; i < _Items.size(); i++) {
		if (Medio(i)->Hash == bHash) return Medio(i);
	}
	return NULL;
}

/*
ItemMedio *ListaMedios::Medio(const size_t nPos) {
	return static_cast<ItemMedio *>(Item(nPos));
}*/