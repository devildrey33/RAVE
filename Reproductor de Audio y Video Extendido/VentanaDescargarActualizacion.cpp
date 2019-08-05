#include "stdafx.h"
#include "VentanaDescargarActualizacion.h"
#include <Shellapi.h>

#define ID_ETIQUETA		1000
#define ID_CANCELAR		1001
#define ID_BARRA		1002
#define ID_NOVEDADES	1003

void VentanaDescargarActualizacion::Crear(const wchar_t *Version) {
	CrearVentana(NULL, L"RAVE_DescargarActualizacion", L"Actualización", 100, 100, 570, 140, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);

	RECT RC;
	GetClientRect(_hWnd, &RC);
	std::wstring VerStr = Version;
	std::wstring Texto = L"Se ha encontrado la versión " + VerStr + L", que se está descargando en estos momentos.";
	Etiqueta.CrearEtiquetaEx(this, Texto.c_str(), 10, 10, RC.right - 20, RC.bottom - 70, ID_ETIQUETA, DWL::DEtiquetaEx_Alineacion_Centrado);
	Barra.CrearBarraProgresoEx(this, 10, RC.bottom - 60, RC.right - 20, 10, ID_BARRA);
	BotonNovedades.CrearBotonEx(this, L"Novedades", (RC.right - 210) / 2, RC.bottom - 40, 100, 30, ID_NOVEDADES);
	BotonCancelar.CrearBotonEx(this, L"Cancelar", ((RC.right - 210) / 2) + 110, RC.bottom - 40, 100, 30, ID_CANCELAR);
	Visible(TRUE);	
}


void VentanaDescargarActualizacion::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_CANCELAR:
			App.Actualizacion.Cancelar(TRUE);
			Destruir();
			break;
		case ID_NOVEDADES:			
			ShellExecute(_hWnd, L"open", L"https://devildrey33.github.io/RAVE/ChangeLog.html", L"", L"", SW_SHOW);
			break;
	}
}


LRESULT CALLBACK VentanaDescargarActualizacion::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_BOTONEX_CLICK	:	
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));				
			return 0;
		case WM_CLOSE			:				
			App.Actualizacion.Cancelar(TRUE);		
			Destruir();							
			return 0;
	}
	return DWL::DVentana::GestorMensajes(uMsg, wParam, lParam);
}