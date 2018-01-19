#include "stdafx.h"
#include "ListaMedios.h"
#include "resource.h"

ListaMedios::ListaMedios() : MedioActual(0), Errores(0) {
}


ListaMedios::~ListaMedios() {
}


/*TablaMedios_Medio *nMedio*/
/*ItemMedio *ListaMedios::AgregarMedio(const int nIcono, const TCHAR *nPista, const TCHAR *nNombre, const TCHAR *nTiempo, const sqlite3_int64 nHash) {


	// Busco si existe el hash para no agregar 2 medios iguales a la lista
	for (size_t i = 0; i < _Items.size(); i++) {
		if (static_cast<ItemMedio *>(_Items[i])->Hash == nHash) {
			return Medio(i); // Devuelvo el medio existente
		}
	}


	ItemMedio *nMedio = AgregarItem<ItemMedio>(nIcono, DLISTAEX_POSICIONFILA_FIN, nPista, nNombre, nTiempo);
	nMedio->Hash = nHash;
	return nMedio;
}
*/


void ListaMedios::BorrarListaReproduccion(void) {
	App.VLC.Stop();
	App.VLC.CerrarMedio();

	EliminarTodosLosItems();
	MedioActual = 0;
}

ItemMedio *ListaMedios::AgregarMedio(TablaMedios_Medio *nMedio) {
//	ItemMedio *TmpMedio = new ItemMedio;
	
	// Busco si existe el hash para no agregar 2 medios iguales a la lista
	for (size_t i = 0; i < _Items.size(); i++) {
		if (static_cast<ItemMedio *>(_Items[i])->Hash == nMedio->Hash()) {
			return Medio(i); // Devuelvo el medio existente
		}
	}


	int nIcono = IDI_CANCION;
	switch (nMedio->TipoMedio()) {
		case Tipo_Medio_Audio: nIcono = IDI_CANCION;	break;
		case Tipo_Medio_Video: nIcono = IDI_VIDEO;		break;
	}

	
	if (nMedio->TipoMedio() == Tipo_Medio::Tipo_Medio_Video) nIcono = 4;

	std::wstring Pista = std::to_wstring(nMedio->Pista());
	if (Pista.size() == 1) Pista = L"0" + Pista;

	ItemMedio *TmpMedio = AgregarItem<ItemMedio>(nIcono, DLISTAEX_POSICION_FIN, Pista.c_str(), nMedio->Nombre(), L"00:00");
	TmpMedio->Hash = nMedio->Hash();
//	AgregarItem(TmpMedio, nIcono, -1, -1, DWL::DString_ToStr(nMedio->Pista(), 2).c_str(), nMedio->Nombre(), L"00:00");
	//	AgregarItem(TmpMedio, 0, -1, -1, DWL::DString_ToStr(Pista, 2).c_str(), Nombre, Disco, Grupo, Genero, Tiempo);
	return TmpMedio;
}

ItemMedio *ListaMedios::BuscarHash(sqlite3_int64 bHash) {
	for (size_t i = 0; i < _Items.size(); i++) {
		if (Medio(i)->Hash == bHash) return Medio(i);
	}
	return NULL;
}


void ListaMedios::Evento_MouseDobleClick(const UINT Boton, const int cX, const int cY, const UINT Param) {
	if (Boton == 0) {
		if (_ItemResaltado != -1) {
			MedioActual = _ItemResaltado;
			ItemMedio *Itm = Medio(MedioActual);
			TablaMedios_Medio NCan(App.BD(), Medio(MedioActual)->Hash);
			if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
			App.VLC.Play();
		}
	}
}
