#include "stdafx.h"
#include "DMensajesWnd.h"
#include "DDlgDirectorios.h"
#include "resource.h"

namespace DWL {

	#define ID_ARBOLDIRECTORIOS		WM_USER + 1000
	#define ID_EDICIONDIRECTORIO	WM_USER + 1001
	#define ID_BOTONACEPTAR			WM_USER + 1002
	#define ID_BOTONCANCELAR		WM_USER + 1003

	DDlgDirectorios::DDlgDirectorios() {
	}


	DDlgDirectorios::~DDlgDirectorios() {
	}


	const BOOL DDlgDirectorios::Mostrar(DhWnd *nPadre, std::wstring &rPath) {
		_Terminado = FALSE;
		// Creo la ventana
		CrearVentana(nPadre, L"DDlgDirectorio", L"Selecciona un Directorio", CW_USEDEFAULT, 0, 400, 600, WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
		// Obtengo el tamaño de la recta cliente de la ventana.
		RECT RC;
		GetClientRect(_hWnd, &RC);
		// Creo los controles
		ArbolDirectorios.CrearExplorarDirectoriosEx(this, 10, 10, RC.right - 20, RC.bottom - 100, ID_ARBOLDIRECTORIOS, WS_CHILD | WS_VISIBLE | WS_BORDER);
		ArbolDirectorios.MultiSeleccion = FALSE;
		ArbolDirectorios.SubSeleccion   = FALSE;
		EdicionSeleccion.CrearEdicionTextoEx(this, L"Txt", 10, RC.bottom - 80, RC.right - 20, 24, ID_EDICIONDIRECTORIO, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER);
		BotonAceptar.CrearBotonEx(this, L"Aceptar", (RC.right / 2) - 110, RC.bottom - 40, 100, 30, ID_BOTONACEPTAR);
		BotonCancelar.CrearBotonEx(this, L"Cancelar", (RC.right / 2) + 10, RC.bottom - 40, 100, 30, ID_BOTONCANCELAR);
		
		// Muestro los controles
		ArbolDirectorios.Visible(TRUE);
//		BotonAceptar.Visible(TRUE);
		BotonAceptar.Activar(FALSE);
//		BotonCancelar.Visible(TRUE);

		// Muestro la ventana modal
		Visible(TRUE);
		// Des-activo la ventana padre
		nPadre->Activar(FALSE);

		// Bucle de mensajes que espera la respuesta del usuario hasta devolver el control a la ventana principal
		MSG		msg;
		HACCEL	hAccelTable = LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO));
		while (GetMessage(&msg, NULL, 0, 0) && _Terminado == FALSE) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		Visible(FALSE);


		rPath  = EdicionSeleccion.Texto();
//		rIcono = EdicionSeleccion.Icono();

		Destruir();

		// Re-activo la ventana padre
		nPadre->Activar(TRUE);
		nPadre->AsignarFoco();

		// Si se ha pulsado cancelar devuleve FALSE
		return (_Terminado == 2) ? FALSE : TRUE;
	}

	void DDlgDirectorios::Evento_Cerrar(void) {
		_Terminado = 2; // cancelado
	}

	void DDlgDirectorios::Evento_BotonEx_Mouse_Click(const UINT cID) {
		switch (cID) {
			case ID_BOTONACEPTAR :
				_Terminado = 1;
				break;
			case ID_BOTONCANCELAR :
				_Terminado = 2;
				break;
		}
		
	}

	void DDlgDirectorios::Evento_BorraFondo(HDC DC) {
		RECT RC;
		GetClientRect(hWnd(), &RC);
		HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
		FillRect(DC, &RC, BrochaFondo);
		DeleteObject(BrochaFondo);
	}


	void DDlgDirectorios::_ActualizarAceptar(void) {
		DExplorarDirectoriosEx_Nodo *Nodo = ArbolDirectorios.PathResaltado();
		BOOL nActivar = FALSE;
		// SI hay un nodo
		if (Nodo != NULL) {
			// Si el nodo está activado
			if (Nodo->Activado == TRUE) {
				// Si no es un nodo de la raíz
				if (Nodo != ArbolDirectorios.PathEscritorio() &&
					Nodo != ArbolDirectorios.PathMiPC() &&
					Nodo != ArbolDirectorios.PathRed() &&
					Nodo != ArbolDirectorios.PathMisDocumentos()) {

					nActivar = TRUE;
					EdicionSeleccion.Texto(Nodo->Path, FALSE);
					EdicionSeleccion.Icono(Nodo->Icono(), TRUE);
				}
			}
		}

		BotonAceptar.Activar(nActivar);
	}


	LRESULT CALLBACK DDlgDirectorios::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_ERASEBKGND:
				Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
				return TRUE;
			case WM_CLOSE:
				Evento_Cerrar();
				return 0;
			case DWL_BOTONEX_CLICK:
				Evento_BotonEx_Mouse_Click(static_cast<UINT>(wParam));
				return 0;
			case DWL_ARBOLEX_CLICK: 
				_ActualizarAceptar();
				return 0;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);		
	}
};