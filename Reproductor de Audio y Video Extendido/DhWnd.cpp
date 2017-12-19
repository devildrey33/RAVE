#include "stdafx.h"
#include "DhWnd.h"

#include "resource.h"
#include "DMensajesWnd.h"
#include "RAVE_Configuracion.h"

namespace DWL {

	DhWnd_Fuente DhWnd::_Fuente;
	DhWnd_Fuente DhWnd::_FuenteB;

	ATOM DhWnd::RegistrarClase(const TCHAR *nNombre, WNDPROC WindowProcedureInicial, UINT nEstilos, const int nIconoRecursos, HBRUSH nColorFondo, HINSTANCE hInstance) {
		// Creamos las fuentes si no existen
		if (_Fuente.Fuente() == NULL) {
			_Fuente  = CreateFont(FUENTE_NORMAL_TAM, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, FUENTE_NORMAL);
			_FuenteB = CreateFont(FUENTE_NORMAL_TAM, 0, 0, 0, FW_BOLD  , false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, FUENTE_NORMAL);
		}
		
		WNDCLASSEX WndClass;
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.style = nEstilos; // CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		WndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(WindowProcedureInicial);
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		if (hInstance == NULL) hInstance = GetModuleHandle(NULL);
		WndClass.hInstance = hInstance;
		if (nIconoRecursos != 0) {
			WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(nIconoRecursos));
			WndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(nIconoRecursos));
		}
		else {
			WndClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
			WndClass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
		}
		WndClass.hCursor = LoadCursor(0, IDC_ARROW);
		// Probar a utilitzar un CreateSolidBrush(COLOR_FONDO) y comentar els WM_ERASEBKGRND aviam si ho fa igual
		if (nColorFondo != NULL) {
			WndClass.hbrBackground = nColorFondo;
		}
		else {			
			WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			//			WndClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
		}
		WndClass.lpszMenuName = 0;
		WndClass.lpszClassName = nNombre;
		return RegisterClassEx(&WndClass);
	}


	const BOOL DhWnd::Maximizada(void) {
		WINDOWPLACEMENT WndPl;
		GetWindowPlacement(_hWnd, &WndPl);
		if (WndPl.showCmd == SW_MAXIMIZE) return TRUE;
		return FALSE;
	};













	HWND DVentana::CrearVentana(const TCHAR *nNombre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase, HMENU nMenu, HBRUSH nColorFondo, const int nIconoRecursos) {
//		if (hWnd() != NULL) hWnd.Destruir();
		if (hWnd()) { Debug_Escribir(L"DVentana::Crear() Error : ya se ha creado la ventana\n"); return hWnd(); }

		ATOM RetRgistrarClase = hWnd.RegistrarClase(nNombre, reinterpret_cast<WNDPROC>(_GestorMensajes), nEstilosClase, nIconoRecursos, nColorFondo);
		hWnd = CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, HWND_DESKTOP, nMenu, GetModuleHandle(NULL), this);
		SendMessage(hWnd(), WM_SETFONT, (WPARAM)hWnd._Fuente(), 0);
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
				PreVentana->hWnd = nhWnd;
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
















	HWND DControl::CrearControl(DhWnd &nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, HBRUSH nColorFondo) {
		//	ATOM CA = hWnd.RegistrarClase(nNombre, _GestorMensajes, 0, nColorFondo);
		if (hWnd()) { Debug_Escribir(L"DControl::Crear() Error : ya se ha creado el control\n"); return hWnd(); }
		hWnd = CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, nPadre(), reinterpret_cast<HMENU>(cID), GetModuleHandle(NULL), this);
		return hWnd();
		//	return CreateWindowEx(nEstiloExtendido, nNombre, nTexto, Estilo, cX, cY, cAncho, cAlto, nPadre(), reinterpret_cast<HMENU>(IntToPtr(nID)), GetModuleHandle(NULL), this);
	};


	LRESULT CALLBACK DControl::_GestorMensajes(HWND nhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DControl *Control = reinterpret_cast<DControl *>(GetWindowLongPtr(nhWnd, GWLP_USERDATA));
		if (Control == NULL) return FALSE;
		return Control->GestorMensajes(uMsg, wParam, lParam);
	};

	void DControl::_ConectarControl(const UINT nID, DhWnd &nPadre) {
		if (hWnd() != NULL) {
			SetWindowLongPtr(hWnd(), GWLP_USERDATA, (LONG_PTR)this);
			_GestorMensajesOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd(), GWLP_WNDPROC, (LONG_PTR)_GestorMensajes));

			SendMessage(hWnd(), WM_SETFONT, (WPARAM)hWnd._Fuente(), 0);
		}
	};











	HWND DControlEx::CrearControlEx(DhWnd &nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase, HBRUSH nColorFondo) {
		if (hWnd()) { Debug_Escribir(L"DControlEx::Crear() Error : ya se ha creado el control extendido\n"); return hWnd(); }
		ATOM CA = hWnd.RegistrarClase(nNombre, _GestorMensajes, nEstilosClase);
		return CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, nPadre(), reinterpret_cast<HMENU>(cID), GetModuleHandle(NULL), this);
	};


	void DControlEx::_ConectarControl(const UINT nID, DhWnd &nPadre) {
		if (hWnd() != NULL) {
			SetWindowLongPtr(hWnd(), GWLP_USERDATA, (LONG_PTR)this);
			SetWindowLongPtr(hWnd(), GWLP_WNDPROC,  (LONG_PTR)_GestorMensajes);
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

	/* Devuleve TRUE si el mouse acaba de entrar en el área del control, FALSE si está fuera o ya estaba dentro del área del control */
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


};