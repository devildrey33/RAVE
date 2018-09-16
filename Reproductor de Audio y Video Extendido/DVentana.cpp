#include "stdafx.h"
#include "DVentana.h"

namespace DWL {

	
	HWND DVentana::CrearVentana(DhWnd *nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase, HMENU nMenu, HBRUSH nColorFondo, const int nIconoRecursos) {
		// La ventana ya no existe pero tenemos el hWnd, por lo que elimino toda la mem�ria
		if (_hWnd != NULL && IsWindow(_hWnd) == 0) { Destruir(); }

		if (hWnd()) { Debug_Escribir(L"DVentana::Crear() Error : ya se ha creado la ventana\n"); return hWnd(); }
		// hWnd del padre
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : HWND_DESKTOP;

		// Afegit expresament per borrar el fondo amb els colors del RAVE
		static HBRUSH ColFondo = CreateSolidBrush(COLOR_FONDO);

		ATOM RetRgistrarClase = RegistrarClase(nNombre, reinterpret_cast<WNDPROC>(_GestorMensajes), nEstilosClase, nIconoRecursos, ColFondo);
		_hWnd = CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, hWndPadre, nMenu, GetModuleHandle(NULL), this);
		Debug_MostrarUltimoError();
		SendMessage(hWnd(), WM_SETFONT, (WPARAM)Fuente18Normal(), 0);
		BarraTareas._Iniciar(_hWnd);
		return hWnd();
	}

	void DVentana::Titulo(std::wstring & nTitulo) {
		SetWindowText(hWnd(), nTitulo.c_str());
	}

	LRESULT CALLBACK  DVentana::_GestorMensajes(HWND nhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_CREATE: {
			DVentana *PreVentana = reinterpret_cast<DVentana *>(((CREATESTRUCT *)lParam)->lpCreateParams);
			if (PreVentana == NULL) return FALSE;
			PreVentana->_hWnd = nhWnd;
			SetWindowLongPtr(nhWnd, GWLP_USERDATA, (LONG_PTR)PreVentana);
			PreVentana->GestorMensajes(uMsg, wParam, lParam);
			return TRUE;
		}
		default: {
			DVentana *Ventana = reinterpret_cast<DVentana *>(::GetWindowLongPtr(nhWnd, GWLP_USERDATA));
			if (Ventana != NULL) {
				return Ventana->GestorMensajes(uMsg, wParam, lParam);
				//					if (Ret != DWL_USAR_GESTOR_POR_DEFECTO) return 0;
			}
		}
		}
		return DefWindowProc(nhWnd, uMsg, wParam, lParam);
	}

	// Establece la opacidad de la ventana (0 transparente, 255 solido)
	void DVentana::Opacidad(const BYTE nNivel) {
		SetWindowLongPtr(_hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		SetLayeredWindowAttributes(_hWnd, 0, nNivel, LWA_ALPHA);
	};


	LRESULT CALLBACK DVentana::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}






}