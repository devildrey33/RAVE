#include "stdafx.h"
#include "VentanaMomento.h"

#define ID_LISTAMOMENTOS	WM_USER + 1000
#define ID_CREARMOMENTO		WM_USER + 1001
#define ID_EDITARMOMENTO    WM_USER + 1002
#define ID_ELIMINARMOMENTO	WM_USER + 1003
#define ID_TXTNOMBRE		WM_USER + 1004
#define ID_TXTINICIO		WM_USER + 1005
#define ID_TXTFINAL			WM_USER + 1006
#define ID_BARRAINICIO      WM_USER + 1007
#define ID_BARRAFINAL       WM_USER + 1008
#define ID_EXCLUIR          WM_USER + 1009
#define ID_GUARDAR          WM_USER + 1010
#define ID_CANCELAR         WM_USER + 1011

VentanaMomento::VentanaMomento(void) {
}


VentanaMomento::~VentanaMomento(void) {
}


void VentanaMomento::Mostrar(BDMedio& nMedio) {
	Medio = nMedio;

	CrearVentana(NULL, L"RAVE_Momentos", L"Momentos", 100, 100, 746, 240, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	ListaMomentos.CrearListaEx(this, 10, 10, 290, 140, ID_LISTAMOMENTOS, WS_CHILD | WS_VISIBLE);
	CrearMomento.CrearBotonEx(this, L"Crear", 10, 160, 70, 30, ID_CREARMOMENTO, WS_CHILD | WS_VISIBLE);
	EditarMomento.CrearBotonEx(this, L"Editar", 100, 160, 70, 30, ID_EDITARMOMENTO, WS_CHILD | WS_VISIBLE);
	EditarMomento.Activado(FALSE);
	EliminarMomento.CrearBotonEx(this, L"Eliminar", 190, 160, 70, 30, ID_ELIMINARMOMENTO, WS_CHILD | WS_VISIBLE);
	EliminarMomento.Activado(FALSE);

	

	Nombre.CrearEdicionTextoEx(this, L"Nombre del momento", 310, 10, 410, 30, ID_TXTNOMBRE);
	Nombre.Activado(FALSE);
	Txt_TiempoInicio.CrearEdicionTextoEx(this, L"0", 310, 50, 90, 20, ID_TXTINICIO);
	Txt_TiempoInicio.Entrada    = DEdicionTextoEx_Entrada_ValoresEnteros;
	Txt_TiempoInicio.Alineacion = DEdicionTextoEx_Alineacion_Derecha;
	Txt_TiempoInicio.Activado(FALSE);
	Txt_TiempoFinal.CrearEdicionTextoEx(this, std::to_wstring(nMedio.Tiempo).c_str(), 310, 80, 90, 20, ID_TXTFINAL);
	Txt_TiempoFinal.Entrada     = DEdicionTextoEx_Entrada_ValoresEnteros;
	Txt_TiempoFinal.Alineacion  = DEdicionTextoEx_Alineacion_Derecha;
	Txt_TiempoFinal.Activado(FALSE);
	Barra_TiempoInicio.CrearBarraDesplazamientoEx(this, 410, 50, 310, 20, ID_BARRAINICIO);
	Barra_TiempoInicio.Activado(FALSE);
	Barra_TiempoFinal.CrearBarraDesplazamientoEx(this, 410, 80, 310, 20, ID_BARRAFINAL, 0.0f, 1.0f, 1.0f);
	Barra_TiempoFinal.Activado(FALSE);
	Excluir.CrearMarcaEx(this, L"Momento a excluir de la reproducción (intro, creditos, etc...)", 310, 100, 413, 30, ID_EXCLUIR, IDI_CHECK2, WS_CHILD | WS_VISIBLE);
	Excluir.Activado(FALSE);

	Guardar.CrearBotonEx(this, L"Guardar", 410, 135, 100, 30, ID_GUARDAR, WS_CHILD | WS_VISIBLE);
	Guardar.Activado(FALSE);

	Cancelar.CrearBotonEx(this, L"Cancelar", 520, 135, 100, 30, ID_CANCELAR, WS_CHILD | WS_VISIBLE);
	Cancelar.Activado(FALSE);

	Visible(TRUE);
}



void VentanaMomento::Evento_BarraEx_Cambiando(DWL::DEventoMouse& DatosMouse) {

}

void VentanaMomento::Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse) {

}

void VentanaMomento::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) { 
	switch (DatosMouse.ID()) {
		case ID_CREARMOMENTO :
			ActivarControles(TRUE);
			break;
		case ID_CANCELAR:
			ActivarControles(FALSE);
			break;
	}
}

void VentanaMomento::ActivarControles(const BOOL nActivar) {
	Nombre.Activado(nActivar);
	Txt_TiempoInicio.Activado(nActivar);
	Txt_TiempoFinal.Activado(nActivar);
	Barra_TiempoInicio.Activado(nActivar);
	Barra_TiempoFinal.Activado(nActivar);
	Excluir.Activado(nActivar);
	Guardar.Activado(nActivar);
	Cancelar.Activado(nActivar);
}

LRESULT CALLBACK VentanaMomento::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		
		case DWL_BARRAEX_CAMBIANDO:	// Se está modificando (mouse down)
			Evento_BarraEx_Cambiando(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}