#include "stdafx.h"
#include "ListaMedios.h"
#include "RAVE_Iconos.h"


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


	int nIcono = RAVE_Iconos::RAVE_Icono_Cancion;
	switch (nMedio->TipoMedio()) {
		case Tipo_Medio_Audio: nIcono = RAVE_Iconos::RAVE_Icono_Cancion;	break;
		case Tipo_Medio_Video: nIcono = RAVE_Iconos::RAVE_Icono_Video;		break;
	}

	std::wstring Pista = std::to_wstring(nMedio->Pista());
	if (Pista.size() == 1) Pista = L"0" + Pista;

	std::wstring StrTiempo;
	App.VLC.TiempoStr(nMedio->Tiempo(), StrTiempo);

	ItemMedio *TmpMedio = AgregarItem<ItemMedio>(nIcono, DLISTAEX_POSICION_FIN, Pista.c_str(), nMedio->Nombre(), StrTiempo.c_str());
	// Agrego el item también en el vector MediosOrdenados (por si el shufle está activado)
	_MediosOrdenados.push_back(TmpMedio);

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


void ListaMedios::Evento_MouseDobleClick(DWL::DControlEx_EventoMouse &EventoMouse) {
	if (EventoMouse.Boton == 0) {
		if (_ItemResaltado != -1) {
			MedioActual = _ItemResaltado;
			ItemMedio *Itm = Medio(MedioActual);
			TablaMedios_Medio NCan(App.BD(), Medio(MedioActual)->Hash);
			if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
			App.VLC.Play();
		}
	}
}


const BOOL ListaMedios::Mezclar(const BOOL nMezclar) {
	size_t	i	= 0;
	BOOL	Ret = FALSE;
	// Mezclar
	if (nMezclar == TRUE) {
		// Guardo el orden original en el vector MediosOrdenados
		_MediosOrdenados.clear();
		for (i = 0; i < _Items.size(); i++) {	_MediosOrdenados.push_back(Medio(i));	}
		// Limpio el vector de la lista para empezar a meclar sus items
		for (i = 0; i < _Items.size(); i++) {	_Items[i] = NULL;						}
		// Mezclo los items
		size_t R = 0;
		for (i = 0; i < _Items.size(); i++) {
			R = App.Rand<size_t>(_Items.size(), 0);
			// Si el item en la pos R no es NULL pasamos al siguiente
			while (_Items[R] != NULL) {
				// Si R llega al tope de items, la reasignamos al principio
				if (++R == _Items.size()) R = 0;
			}
			_Items[R] = _MediosOrdenados[i];
		}
		// Guardo la posición del medio actual en la lista ordenada
		MedioActualOrdenado = MedioActual;
		// Busco la posición del medio actual en la lista mezclada
		for (i = 0; i < _Items.size(); i++) {
			if (_Items[i] == _MediosOrdenados[MedioActual]) {
				MedioActual = i;
				MostrarItem(i);
				break;
			}
		}
		Ret = TRUE;
	}
	// Restaurar medios ordenados
	else {
		// Busco la posición del medio actual en la lista original
		for (i = 0; i < _Items.size(); i++) {
			if (_Items[MedioActual] == _MediosOrdenados[i]) {
				MedioActual = i;
				MostrarItem(i);
				break;
			}
		}
		// Restauro los items desde la lista original
		for (i = 0; i < _Items.size(); i++) {
			_Items[i] = _MediosOrdenados[i];
		}
	}

	// Repinto el control y devuelvo si la lista está mezclada o no
	Repintar();
	return Ret;
}