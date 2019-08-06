#include "stdafx.h"
#include "VentanaInstalarActualizacion.h"
#include "resource.h"
#include <shellapi.h>
#include <DDirectoriosWindows.h>

#define ID_INSTALAR 1000
#define ID_ETIQUETA	1001
#define ID_CANCELAR 1002
#define ID_MARCA	1003

void VentanaInstalarActualizacion::Crear(void) {
	CrearVentana(NULL, L"RAVE_InstalarActualizacion", L"Nueva actualización", 100, 100, 570, 120, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	std::wstring Texto = L"Deseas instalar la actualización? requerirá privilegios de administración.";
	Etiqueta.CrearEtiquetaEx(this, Texto.c_str(), 10, 10, RC.right - 20, 30, ID_ETIQUETA, DWL::DEtiquetaEx_Alineacion_Izquierda);
	Etiqueta.Fuente.CrearFuente(22, Etiqueta.Skin.FuenteNombre.c_str(), TRUE, FALSE, FALSE);

	MarcaNoBuscar.CrearMarcaEx(this, L"Desactivar actualizaciones automáticas.", 10, RC.bottom - 40, 290, 30, ID_MARCA, IDI_CHECK2);
	MarcaNoBuscar.Marcado(FALSE);

	BotonInstalar.CrearBotonEx(this, L"Instalar", RC.right - 220, RC.bottom - 40, 100, 30, ID_INSTALAR);
	BotonCancelar.CrearBotonEx(this, L"Cancelar", RC.right - 110, RC.bottom - 40, 100, 30, ID_CANCELAR);
	Visible(TRUE);
}


void VentanaInstalarActualizacion::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	App.Opciones.BuscarActualizacion(!MarcaNoBuscar.Marcado());
	if (MarcaNoBuscar.Marcado() == FALSE)		App.MostrarToolTipPlayer(L"Se buscarán actualizaciones automáticamente.");
	else 										App.MostrarToolTipPlayer(L"No se buscarán actualizaciones automáticamente.");
}



void VentanaInstalarActualizacion::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_CANCELAR :
			Destruir();
			break;
		case ID_INSTALAR :
			std::wstring Path;
			std::wstring Exe;
			DWL::DDirectoriosWindows::Comun_AppData(Path);
			Path += L"\\Rave\\";
			Exe = Path + NOMBRE_ACTUALIZADOR + L".exe";
			ShellExecute(NULL, TEXT("RunAs"), Exe.c_str(), L"", Path.c_str(), SW_SHOWNORMAL);
			break;
	}
}

LRESULT CALLBACK VentanaInstalarActualizacion::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_BOTONEX_CLICK	:	
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));				
			return 0;
		case DWL_MARCAEX_CLICK:
			Evento_MarcaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case WM_CLOSE			:
			Destruir();							
			return 0;
	}
	return DWL::DVentana::GestorMensajes(uMsg, wParam, lParam);

}
