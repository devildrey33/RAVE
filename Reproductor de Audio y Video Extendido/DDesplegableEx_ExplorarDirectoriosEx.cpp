#include "stdafx.h"
#include "DDesplegableEx.h"
#include "DDesplegableEx_ExplorarDirectoriosEx.h"
#include "RAVE.h"

namespace DWL {

	DDesplegableEx_ExplorarDirectoriosEx::DDesplegableEx_ExplorarDirectoriosEx(DDesplegableEx *nPadre) : DExplorarDirectoriosEx(), _Mostrando(FALSE) {
		_Padre = nPadre;
		
		RECT RC;
		GetWindowRect(nPadre->hWnd(), &RC);
		Debug_UltimoError(0);		// Reseteo el ultimo error del WINAPI
		_hWnd = CrearControlEx(NULL, L"DArbolEx", L"", NULL, RC.left, RC.bottom, RC.right - RC.left, 200, WS_POPUP | WS_BORDER, WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, NULL);  // CS_DBLCLKS (el control recibe notificaciones de doble click)
		Debug_MostrarUltimoError(); // Muestra el error de CreateWindowEx por la consola (si ha sucedido uno)
		_Fuente = _Fuente18Normal;
//		SetLayeredWindowAttributes(_hWnd, 0, 240, LWA_ALPHA);
		ActualizarDirectorios();


		//SetForegroundWindow(App.VentanaRave.hWnd());
	}


	DDesplegableEx_ExplorarDirectoriosEx::~DDesplegableEx_ExplorarDirectoriosEx(void) {
	}


	void DDesplegableEx_ExplorarDirectoriosEx::Mostrar(void) {
		SetWindowPos(hWnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		_Mostrando = TRUE;
		SetFocus(hWnd());
		// Envio el mensaje WM_NCACTIVATE a la ventana principal para que no se vea como pierde el foco, i parezca que el desplegable es un hijo de la ventana principal
		SendMessage(GetAncestor(_Padre->hWnd(), GA_ROOT), WM_NCACTIVATE, TRUE, 0);

	}

	void DDesplegableEx_ExplorarDirectoriosEx::Evento_FocoPerdido(HWND hWndNuevoFoco) {
		if (_Mostrando == TRUE) {
			_Mostrando = FALSE;
		}
		else {
			Visible(FALSE);
		}
		Debug_Escribir_Varg(L"Foco perdido : %d\n", hWndNuevoFoco);
	}

	void DDesplegableEx_ExplorarDirectoriosEx::Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param) { 
//		SetFocus(_Padre->hWnd()); 
	};

	void DDesplegableEx_ExplorarDirectoriosEx::Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param) { 
//		SetFocus(_Padre->hWnd());
	};

}