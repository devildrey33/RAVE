#include "stdafx.h"
#include "Rave_Medio.h"
#include "Rave_Iconos.h"
#include "ListaMedios.h"

Rave_Medio::Rave_Medio(ItemMedio *nMedio) : Medio(nMedio), _SalirPantallaCompleta(0) {
};

void Rave_Medio::ComprobarMomento(void) {
	if (Medio->BdMedio.PosMomento != -1) {
		//		float T = (1.0f / static_cast<float>(Medio.Tiempo)) * Medio.Momentos[Medio.PosMomento]->TiempoInicio;
		TiempoActualMs(Medio->BdMedio.Momentos[Medio->BdMedio.PosMomento]->TiempoInicio);
	}

}

/* nTipo :
	0 - Normal
	1 - Play
	2 - Pausa
*/
void Rave_Medio::ActualizarIconos(int nTipo) {
	if (Medio == nullptr) return;
	if (Medio->BdMedio.Hash != 0) {
		int nIcono = RAVE_Iconos::RAVE_Icono_Interrogante;
		switch (nTipo) {
			case 0: // normal
				if (Medio->BdMedio.TipoMedio == Tipo_Medio::Tipo_Medio_Audio) nIcono = RAVE_Iconos::RAVE_Icono_Cancion;	// IDI_CANCION
				if (Medio->BdMedio.TipoMedio == Tipo_Medio::Tipo_Medio_Video) nIcono = RAVE_Iconos::RAVE_Icono_Video;	// IDI_VIDEO
				if (Medio->BdMedio.TipoMedio == Tipo_Medio::Tipo_Medio_IpTv)  nIcono = RAVE_Iconos::RAVE_Icono_IpTv;	// IDI_IPTV
				break;
			case 1:
				nIcono = RAVE_Iconos::RAVE_Icono_Play; // play
				break;
			case 2:
				nIcono = RAVE_Iconos::RAVE_Icono_Pausa; // Pausa
				break;
		}
		// Actualizo los nodos del ArbolBD
		NodoBD *Nodo = App.VentanaRave.Arbol.BuscarHash(Medio->BdMedio.Hash), *NodoS;
		while (Nodo != NULL) {
			Nodo->Icono(nIcono);
			NodoS = static_cast<NodoBD*>(App.VentanaRave.Arbol.BuscarNodoSiguiente(Nodo, FALSE, NULL));
			// Se ha llegado al final del arbol
			if (NodoS == nullptr) break;
			Nodo = App.VentanaRave.Arbol.BuscarHash(Medio->BdMedio.Hash, NodoS);
		}
		App.VentanaRave.Arbol.Repintar();
		// Actualizo el Item del ListaMedios
		ItemMedio *Item = App.VentanaRave.Lista.BuscarHash(Medio->BdMedio.Hash);
		if (Item != NULL) {
			Item->Icono(nIcono);
			App.VentanaRave.Lista.MostrarItem(Item);
		}
		App.VentanaRave.Lista.Repintar();
	}
}

void Rave_Medio::Evento_SalirPantallaCompleta(void) {
	_SalirPantallaCompleta = GetTickCount64();
}