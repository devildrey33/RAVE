#include "stdafx.h"
#include "VentanaMomento.h"

#define ID_LISTAMOMENTOS	1000
#define ID_CREARMOMENTO		1001
#define ID_ELIMINARMOMENTO	1002
#define ID_TXTNOMBRE		1003


VentanaMomento::VentanaMomento(void) {
}


VentanaMomento::~VentanaMomento(void) {
}


void VentanaMomento::Mostrar(BDMedio& nMedio) {
	CrearVentana(NULL, L"RAVE_Momentos", L"Momentos", 100, 100, 800, 240, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	ListaMomentos.CrearListaEx(this, 10, 10, 290, 140, ID_LISTAMOMENTOS, WS_CHILD | WS_VISIBLE);

	CrearMomento.CrearBotonEx(this, L"Crear momento", 10, 160, 130, 30, ID_CREARMOMENTO, WS_CHILD | WS_VISIBLE);
	EliminarMomento.CrearBotonEx(this, L"Eliminar momento", 150, 160, 150, 30, ID_ELIMINARMOMENTO, WS_CHILD | WS_VISIBLE);

	Nombre.CrearEdicionTextoEx(this, L"Nombre del momento", 310, 10, 300, 30, ID_TXTNOMBRE);

	Visible(TRUE);
}