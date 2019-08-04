#include "VentanaPrincipal.h"
#include "Actualizador_RAVE.h"

#define ID_CANCELAR		1000
#define ID_ETIQUETA		1001
#define ID_ETIQUETA2	1002
#define ID_BARRA 		1003

VentanaPrincipal::VentanaPrincipal(void) {

}


void VentanaPrincipal::Crear(void) {
	CrearVentana(NULL, L"ActualizadorRAVE", L"Actualizando RAVE", 100, 100, 570, 130, WS_OVERLAPPED | WS_CAPTION);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	EtiquetaTextoActualizacion.CrearEtiquetaEx(this, L"Descomprimiendo los archivos...", 10, 10, RC.right - 20, 30, ID_ETIQUETA);
	EtiquetaTextoActualizacion.Fuente.CrearFuente(FUENTE_GRANDE, EtiquetaTextoActualizacion.Skin.FuenteNombre.c_str(), TRUE, FALSE, FALSE);
	EtiquetaNombreArchivo.CrearEtiquetaEx(this, L"RAVE.exe", 10, 40, RC.right - 20, 30, ID_ETIQUETA2);
	
	Barra.CrearBarraProgresoEx(this, 10, RC.bottom - 24, RC.right - 20, 14, ID_BARRA);


	//BotonCancelar.CrearBotonEx(this, L"Cancelar", (RC.right - 100) / 2, RC.bottom - 40, 100, 30, ID_CANCELAR);
	Visible(TRUE);

	App.Actualizar();
}


/*
void VentanaPrincipal::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_CANCELAR:
//			Destruir();
			break;
	}
}*/

/*
LRESULT CALLBACK VentanaPrincipal::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	}
	return DWL::DVentana::GestorMensajes(uMsg, wParam, lParam);
}*/