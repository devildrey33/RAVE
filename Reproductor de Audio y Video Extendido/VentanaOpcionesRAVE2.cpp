#include "stdafx.h"
#include "VentanaOpcionesRAVE2.h"

#define ID_ARBOLSECCIONES	1000
#define ID_MARCOBD			1001
#define ID_MARCOCANCIONES	1002
#define ID_MARCOGENERAL		1003
#define ID_MARCOLISTAS		1004
#define ID_MARCOTECLADO		1005
#define ID_MARCOAUDIO		1006
#define ID_MARCOVIDEO		1007



const wchar_t *VentanaOpcionesRAVE2::OpcionesStr[NUM_OPCIONES] = {
	L"Base de datos",	// 0
	L"Inicio",			// 1
	L"UI",				// 2
	L"Teclado",			// 3
	L"Listas",			// 4
	L"Audio",			// 5
	L"Video"			// 6
};


void VentanaOpcionesRAVE2::Crear(void) {
	DVentana::CrearVentana(NULL, L"RAVE_VentanaOpciones2", L"Opciones", App.Opciones.VentanaOpciones_PosX(), App.Opciones.VentanaOpciones_PosY(), 700, 350, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	ArbolSecciones.CrearArbolEx(this, 10, 10, 190, 330, ID_ARBOLSECCIONES, WS_CHILD | WS_VISIBLE);

	int Iconos[NUM_OPCIONES] = { 
		IDI_BASEDATOS, 
		IDI_NOTA, 
		IDI_PROPORCION, 
		IDI_TECLADO,
		IDI_AGREGARLISTA,	
		IDI_CANCION2,
		IDI_VIDEO 
	};

	for (size_t i = 0; i < NUM_OPCIONES; i++) {
		ArbolSecciones.AgregarNodo(OpcionesStr[i], NULL, Iconos[i]);
	}


	MarcoBaseDatos.Crear(this, 210, 10, 480, 330, ID_MARCOBD);
	MarcoInicio.Crear(this, 210, 10, 480, 330, ID_MARCOCANCIONES);
	MarcoUI.Crear(this, 210, 10, 480, 330, ID_MARCOGENERAL);
	MarcoListas.Crear(this, 210, 10, 480, 330, ID_MARCOLISTAS);
	MarcoTeclado.Crear(this, 210, 10, 480, 330, ID_MARCOTECLADO);
	MarcoAudio.Crear(this, 210, 10, 480, 330, ID_MARCOAUDIO);
	MarcoVideo.Crear(this, 210, 10, 480, 330, ID_MARCOVIDEO);

	MostrarMarco(0);

	Visible(TRUE);
}


void VentanaOpcionesRAVE2::MostrarMarco(const size_t Marco) {
	if (Marco > NUM_OPCIONES) return;

	BOOL Marcos[NUM_OPCIONES];
	for (size_t i = 0; i < NUM_OPCIONES; i++) Marcos[i] = FALSE;

	if (Marco < NUM_OPCIONES) {
		Marcos[Marco] = TRUE;
	}

	MarcoBaseDatos.Visible(Marcos[0]);
	MarcoInicio.Visible(Marcos[1]);
	MarcoUI.Visible(Marcos[2]);
	MarcoTeclado.Visible(Marcos[3]);
	MarcoListas.Visible(Marcos[4]);
	MarcoAudio.Visible(Marcos[5]);
	MarcoVideo.Visible(Marcos[6]);
}

void VentanaOpcionesRAVE2::Evento_Cerrar(void) {
	ArbolSecciones.Destruir();
	Destruir();
	App.OcultarToolTipOpciones();
}

void VentanaOpcionesRAVE2::Evento_ArbolEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	if (ArbolSecciones.NodoMarcado() != nullptr) {
		for (size_t i = 0; i < NUM_OPCIONES; i++) {
			if (ArbolSecciones.NodoMarcado()->Texto.compare(OpcionesStr[i]) == 0) {
				MostrarMarco(i);
				return;
			}
		}
	}
}


void VentanaOpcionesRAVE2::Evento_DlgDirectorios_CambioTamPos(HWND hWndDlg) {
	RECT RW, RC;
	GetWindowRect(hWndDlg, &RW);
	GetClientRect(hWndDlg, &RC);
	App.Opciones.GuardarPosTamDlgDirectorios(RW, RC);
}


LRESULT CALLBACK VentanaOpcionesRAVE2::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE:		
			Evento_Cerrar();	
			return 0;
		case DWL_ARBOLEX_CLICK:
			Evento_ArbolEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case WM_EXITSIZEMOVE:
			App.Opciones.GuardarPosVentanaOpciones();
			return 0;
		case WM_MOVING:
			App.OcultarToolTipOpciones();
			return 0;
		// El DlgDirectorios ha cambiado de tamaño o posición
		case DWL_DLGDIRECTORIOS_POSTAM_CAMBIADO:
			Evento_DlgDirectorios_CambioTamPos(WPARAM_TO_HWND(wParam));
			return 0;

	}

	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}