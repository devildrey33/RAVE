#include "stdafx.h"
#include "DControlEx.h"

namespace DWL {

	HWND DControlEx::CrearControlEx(DhWnd &nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase, HBRUSH nColorFondo) {
		if (hWnd()) { Debug_Escribir(L"DControlEx::Crear() Error : ya se ha creado el control extendido\n"); return hWnd(); }
		ATOM CA = hWnd.RegistrarClase(nNombre, _GestorMensajes, nEstilosClase);
		return CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, nPadre(), reinterpret_cast<HMENU>(cID), GetModuleHandle(NULL), this);
	};


	void DControlEx::_ConectarControl(const UINT nID, DhWnd &nPadre) {
		if (hWnd() != NULL) {
			SetWindowLongPtr(hWnd(), GWLP_USERDATA, (LONG_PTR)this);
			SetWindowLongPtr(hWnd(), GWLP_WNDPROC, (LONG_PTR)_GestorMensajes);
			SendMessage(hWnd(), WM_SETFONT, (WPARAM)hWnd._Fuente(), 0);
		}
	};


	LRESULT CALLBACK DControlEx::_GestorMensajes(HWND nhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_CREATE: {
			DControl *PreControlEx = reinterpret_cast<DControl *>(((CREATESTRUCT *)lParam)->lpCreateParams);
			if (PreControlEx == NULL) return FALSE;
			PreControlEx->hWnd = nhWnd;
			SetWindowLongPtr(nhWnd, GWLP_USERDATA, (LONG_PTR)PreControlEx);
			PreControlEx->GestorMensajes(uMsg, wParam, lParam);
			return TRUE;
		}
		default: {
			DControl *ControlEx = reinterpret_cast<DControl *>(GetWindowLongPtr(nhWnd, GWLP_USERDATA));
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
	const BOOL DControlEx::_MouseEntrando(void) {
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
	};

}