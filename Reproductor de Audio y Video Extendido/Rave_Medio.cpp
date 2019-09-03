#include "stdafx.h"
#include "Rave_Medio.h"
#include "Rave_Iconos.h"

void Rave_Medio::ComprobarMomento(void) {
	if (Medio.PosMomento != -1) {
		//		float T = (1.0f / static_cast<float>(Medio.Tiempo)) * Medio.Momentos[Medio.PosMomento]->TiempoInicio;
		TiempoActualMs(Medio.Momentos[Medio.PosMomento]->TiempoInicio);
	}

}

/* nTipo :
	0 - Normal
	1 - Play
	2 - Pausa
*/
void Rave_Medio::ActualizarIconos(int nTipo) {
	if (Medio.Hash != 0) {
		int nIcono = RAVE_Iconos::RAVE_Icono_Interrogante;
		switch (nTipo) {
			case 0: // normal
				if (Medio.TipoMedio == Tipo_Medio::Tipo_Medio_Audio) nIcono = RAVE_Iconos::RAVE_Icono_Cancion;	// IDI_CANCION
				if (Medio.TipoMedio == Tipo_Medio::Tipo_Medio_Video) nIcono = RAVE_Iconos::RAVE_Icono_Video;	// IDI_VIDEO
				if (Medio.TipoMedio == Tipo_Medio::Tipo_Medio_IpTv)  nIcono = RAVE_Iconos::RAVE_Icono_IpTv;		// IDI_IPTV
				break;
			case 1:
				nIcono = RAVE_Iconos::RAVE_Icono_Play; // play
				break;
			case 2:
				nIcono = RAVE_Iconos::RAVE_Icono_Pausa; // Pausa
				break;
		}
		// Actualizo el nodo del ArbolBD
		NodoBD *Nodo = App.VentanaRave.Arbol.BuscarHash(Medio.Hash);
		if (Nodo != NULL)
			Nodo->Icono(nIcono);
		App.VentanaRave.Arbol.Repintar();
		// Actualizo el Item del ListaMedios
		ItemMedio *Item = App.VentanaRave.Lista.BuscarHash(Medio.Hash);
		if (Item != NULL) {
			Item->Icono(nIcono);
			App.VentanaRave.Lista.MostrarItem(Item);
		}
		App.VentanaRave.Lista.Repintar();
	}
}