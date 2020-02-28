#include "stdafx.h"
#include "VentanaObtenerURL.h"
#include "resource.h"
#include <DArchivoInternet.h>

#define ID_ETIQUETA 1000
#define ID_EDITURL	1001
#define ID_ABRIR	1002

void VentanaObtenerURL::Mostrar(void) {
	RECT RC;
	CrearVentana(NULL, L"RAVE_ObtenerURL", L"Abrir URL", App.Opciones.VentanaMomentos_PosX(), App.Opciones.VentanaMomentos_PosY(), 500, 50, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
	GetClientRect(_hWnd, &RC);
	//	Etiqueta.CrearEtiquetaEx(this, , 10, 10, 300, 20, ID_ETIQUETA);
	EditURL.CrearEdicionTextoEx(this, L"", 10, 10, RC.right - 130, 30, ID_EDITURL);
	EditURL.Placeholder = L"Introduce la URL que deseas abrir";
	BotonAbrir.CrearBotonEx(this, L"Abrir", RC.right - 110, 10, 100, 30, ID_ABRIR);
	Visible(TRUE);
	// Asigno el foco del teclado al editbox
	SetFocus(EditURL.hWnd());
}

void VentanaObtenerURL::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	if (DatosMouse.ID() == ID_ABRIR) {
		BotonAbrir.Activado(FALSE);
		BDMedio Medio;
		App.BD.AnalizarMedio(EditURL.Texto(), Medio, App.Unidades);
		App.VentanaRave.Lista.AgregarMedio(&Medio);

/*		DArchivoInternet::Obtener(EditURL.Texto(),
			[=](DWL::DPeticion &P) {
				Destruir();
				BDMedio Medio;
				App.BD.AnalizarMedio(P.)
			},
			[=](const UINT IDError) {
				Destruir();
			}
		);*/
	}
}


LRESULT CALLBACK VentanaObtenerURL::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case WM_EXITSIZEMOVE:
//			App.Opciones.GuardarPosVentanaMomentos();
			return 0;

		case WM_MOVING:
//			App.OcultarToolTipMomentos();
			return 0;

	}

	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}
