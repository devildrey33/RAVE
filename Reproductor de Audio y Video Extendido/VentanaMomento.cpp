#include "stdafx.h"
#include "VentanaMomento.h"

#define ID_LISTAMOMENTOS	1000
#define ID_CREARMOMENTO		1001
#define ID_EDITARMOMENTO    1002
#define ID_ELIMINARMOMENTO	1003
#define ID_TXTNOMBRE		1004
#define ID_TXTINICIO		1005
#define ID_TXTFINAL			1006
#define ID_BARRAINICIO      1007
#define ID_BARRAFINAL       1008
#define ID_EXCLUIR          1009
#define ID_GUARDAR          1010


VentanaMomento::VentanaMomento(void) {
}


VentanaMomento::~VentanaMomento(void) {
}


void VentanaMomento::Mostrar(BDMedio& nMedio) {
	CrearVentana(NULL, L"RAVE_Momentos", L"Momentos", 100, 100, 746, 240, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	ListaMomentos.CrearListaEx(this, 10, 10, 290, 140, ID_LISTAMOMENTOS, WS_CHILD | WS_VISIBLE);
	CrearMomento.CrearBotonEx(this, L"Crear", 10, 160, 70, 30, ID_CREARMOMENTO, WS_CHILD | WS_VISIBLE);
	EditarMomento.CrearBotonEx(this, L"Editar", 100, 160, 70, 30, ID_EDITARMOMENTO, WS_CHILD | WS_VISIBLE);
	EditarMomento.Activado(FALSE);
	EliminarMomento.CrearBotonEx(this, L"Eliminar", 190, 160, 70, 30, ID_ELIMINARMOMENTO, WS_CHILD | WS_VISIBLE);
	EliminarMomento.Activado(FALSE);

	Nombre.CrearEdicionTextoEx(this, L"Nombre del momento", 310, 10, 410, 30, ID_TXTNOMBRE);
	Nombre.Activado(FALSE);
	Txt_TiempoInicio.CrearEdicionTextoEx(this, L"0:00", 310, 50, 50, 20, ID_TXTINICIO);
	Txt_TiempoInicio.Entrada    = DEdicionTextoEx_Entrada_ValoresTiempo;
	Txt_TiempoInicio.Alineacion = DEdicionTextoEx_Alineacion_Derecha;
	Txt_TiempoInicio.Activado(FALSE);
	Txt_TiempoFinal.CrearEdicionTextoEx(this, L"0:00", 310, 80, 50, 20, ID_TXTFINAL);
	Txt_TiempoFinal.Entrada     = DEdicionTextoEx_Entrada_ValoresTiempo;
	Txt_TiempoFinal.Alineacion  = DEdicionTextoEx_Alineacion_Derecha;
	Txt_TiempoFinal.Activado(FALSE);
	Barra_TiempoInicio.CrearBarraDesplazamientoEx(this, 370, 50, 350, 20, ID_BARRAINICIO);
	Barra_TiempoInicio.Activado(FALSE);
	Barra_TiempoFinal.CrearBarraDesplazamientoEx(this, 370, 80, 350, 20, ID_BARRAFINAL, 0.0f, 1.0f, 1.0f);
	Barra_TiempoFinal.Activado(FALSE);
	Excluir.CrearMarcaEx(this, L"Momento a excluir de la reproducción (intro, creditos, etc...)", 310, 110, 413, 30, ID_EXCLUIR, IDI_CHECK2, WS_CHILD | WS_VISIBLE);
	Excluir.Activado(FALSE);

	Guardar.CrearBotonEx(this, L"Guardar", 470, 150, 100, 30, ID_GUARDAR, WS_CHILD | WS_VISIBLE);
	Guardar.Activado(FALSE);

	Visible(TRUE);
}