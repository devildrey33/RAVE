#include "stdafx.h"
#include "ListaMedios.h"
#include "RAVE_Iconos.h"


ListaMedios::ListaMedios() : MedioActual(0), Errores(0) {
}


ListaMedios::~ListaMedios() {
}

/*
void ListaMedios::Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_TeclaPresionada(DatosTeclado);
}

void ListaMedios::Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_TeclaSoltada(DatosTeclado);
}

void ListaMedios::Evento_Tecla(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_Tecla(DatosTeclado);
}*/



void ListaMedios::BorrarListaReproduccion(void) {
//	App.VLC.Stop();
	App.VLC.CerrarMedio();

	EliminarTodosLosItems();
	MedioActual = 0;

	Repintar();
}

ItemMedio *ListaMedios::AgregarMedio(BDMedio *nMedio) {
	
	// Busco si existe el hash para no agregar 2 medios iguales a la lista
	for (size_t i = 0; i < _Items.size(); i++) {
		if (static_cast<ItemMedio *>(_Items[i])->Hash == nMedio->Hash) {
			return Medio(i); // Devuelvo el medio existente
		}
	}

	// Miro el icono que necesita el medio
	int nIcono = RAVE_Iconos::RAVE_Icono_Cancion;
	switch (nMedio->TipoMedio) {
		case Tipo_Medio_Audio: nIcono = RAVE_Iconos::RAVE_Icono_Cancion;	break;
		case Tipo_Medio_Video: nIcono = RAVE_Iconos::RAVE_Icono_Video;		break;
	}

	// Paso la pista a string formateada a 2 digitos
	std::wstring Pista = std::to_wstring(nMedio->Pista());
	if (Pista.size() == 1) Pista = L"0" + Pista;

	// Paso el tiempo a string formateado en mm:ss
	std::wstring StrTiempo;
	App.VLC.TiempoStr(nMedio->Tiempo, StrTiempo);

	// Agrego el item
	ItemMedio *TmpMedio = AgregarItem<ItemMedio>(nIcono, DLISTAEX_POSICION_FIN, Pista.c_str(), nMedio->Nombre().c_str(), StrTiempo.c_str());
	// Agrego el item también en el vector MediosOrdenados (por si el shufle está activado)
	_MediosOrdenados.push_back(TmpMedio);
	// Asigno el hash y la Id al item
	TmpMedio->Hash	= nMedio->Hash;
	TmpMedio->Id	= nMedio->Id;

	return TmpMedio;
}

ItemMedio *ListaMedios::BuscarHash(sqlite3_int64 bHash) {
	for (size_t i = 0; i < _Items.size(); i++) {
		if (Medio(i)->Hash == bHash) return Medio(i);
	}
	return NULL;
}

// Reproduce el medio especificado con el ratón
void ListaMedios::Evento_MouseDobleClick(DWL::DEventoMouse &EventoMouse) {
	if (EventoMouse.Boton == 0) {
		if (_ItemResaltado != -1) {
			MedioActual = static_cast<long>(_ItemResaltado);
			ItemMedio *Itm = Medio(MedioActual);
			
			BDMedio NCan;
			App.BD.ObtenerMedio(Medio(MedioActual)->Hash, NCan);

			if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
			App.VLC.Play();
		}
	}
}

void ListaMedios::Evento_MouseSoltado(DWL::DEventoMouse &DatosMouse) {
	if (DatosMouse.Boton == 2) {		
		if (_ItemResaltado != -1) {
			BDMedio nMedio;
			App.BD.ObtenerMedio(Medio(_ItemResaltado)->Hash, nMedio);

			_ToolTip.Destruir();
			_ToolTip.Mostrar(nMedio);
		}
	}

	// Oculto el tooltip por que no hay Nodo resaltado, o por que se ha presionado un boton que no es el del medio
	if (DatosMouse.Boton != 2 || _ItemResaltado == -1) {
		_ToolTip.Destruir();
	}

	// Mostrar el menú
	if (DatosMouse.Boton == 1) {
		BOOL nActivar = (_ItemResaltado == -1) ? FALSE : TRUE;
		for (size_t i = 0; i < App.VentanaRave.Menu_Lista.TotalMenus(); i++) {
			App.VentanaRave.Menu_Lista.Menu(i)->Activado(nActivar);
		}
		App.VentanaRave.Menu_Lista.Mostrar(&App.VentanaRave);
	}
}

void ListaMedios::Evento_MouseMovimiento(DWL::DEventoMouse &DatosMouse) {
	if (_ItemResaltado == -1) {
		_ToolTip.Destruir();
	}
	else {
		if (_ItemResaltado != _ItemUResaltado) {
			_ToolTip.Destruir();
		}
		else {
			_ToolTip.Mover();
		}
	}
}


void ListaMedios::Evento_MouseSaliendo(void) {
	_ToolTip.Destruir();
}

// 	Función que mezcla las canciones de la lista (TRUE), o restaura el orden original en que se añadieron (FALSE)
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
				MedioActual = static_cast<long>(i);
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
				MedioActual = static_cast<long>(i);
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