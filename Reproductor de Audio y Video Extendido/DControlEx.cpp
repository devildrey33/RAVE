#include "stdafx.h"
#include "DControlEx.h"

namespace DWL {

	HWND DControlEx::CrearControlEx(DhWnd *nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase, HBRUSH nColorFondo) {
		BOOL Is = IsWindow(_hWnd);
		// Si existe el hWnd pero la ventana / control ya no existe
		if (_hWnd != NULL && IsWindow(_hWnd) == 0) { Destruir(); }
		if (hWnd()) { Debug_Escribir(L"DControlEx::Crear() Error : ya se ha creado el control extendido\n"); return hWnd(); }
		ATOM CA = RegistrarClase(nNombre, _GestorMensajes, nEstilosClase);
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
		_MouseDentro = FALSE;
		return CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, hWndPadre, reinterpret_cast<HMENU>(cID), GetModuleHandle(NULL), this);
	};


	void DControlEx::_ConectarControl(const UINT nID, DhWnd *nPadre) {
		if (hWnd() != NULL) {
			SetWindowLongPtr(hWnd(), GWLP_USERDATA, (LONG_PTR)this);
			SetWindowLongPtr(hWnd(), GWLP_WNDPROC, (LONG_PTR)_GestorMensajes);
			SendMessage(hWnd(), WM_SETFONT, (WPARAM)Fuente18Normal(), 0);
		}
	};


	LRESULT CALLBACK DControlEx::_GestorMensajes(HWND nhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_CREATE: {
				DControlEx *PreControlEx = reinterpret_cast<DControlEx *>(((CREATESTRUCT *)lParam)->lpCreateParams);
				if (PreControlEx == NULL) return FALSE;
				PreControlEx->_hWnd = nhWnd;
				SetWindowLongPtr(nhWnd, GWLP_USERDATA, (LONG_PTR)PreControlEx);
				PreControlEx->GestorMensajes(uMsg, wParam, lParam);
				return TRUE;
			}
			default: {
				DControlEx *ControlEx = reinterpret_cast<DControlEx *>(GetWindowLongPtr(nhWnd, GWLP_USERDATA));
				if (ControlEx != NULL) {
					return ControlEx->GestorMensajes(uMsg, wParam, lParam);
					//					if (Ret != DWL_USAR_GESTOR_POR_DEFECTO) return 0;
				}
			}
		}
		return DefWindowProc(nhWnd, uMsg, wParam, lParam);
	};

	/* Devuleve TRUE si el mouse acaba de entrar en el �rea del control, FALSE si est� fuera o ya estaba dentro del �rea del control */
	/* De esta forma se simula el mensaje WM_MOUSEENTER */
/*	const BOOL DControlEx::_MouseEntrando(void) {
		if (_MouseDentro == FALSE) {
			TRACKMOUSEEVENT Trk;
			Trk.cbSize = sizeof(TRACKMOUSEEVENT);
			Trk.dwFlags = TME_LEAVE;
			Trk.hwndTrack = hWnd();
			TrackMouseEvent(&Trk);
			_MouseDentro = TRUE;
			return TRUE;
		}
		return FALSE;
	};*/

	LRESULT CALLBACK DControlEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_KEYDOWN: 
				App.Evento_TeclaPresionada(DWL::DEventoTeclado(wParam, lParam, this));
				return 0;
			case WM_KEYUP:
				App.Evento_TeclaSoltada(DWL::DEventoTeclado(wParam, lParam, this));
				return 0;
		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}

}
