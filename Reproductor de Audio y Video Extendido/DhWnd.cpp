#include "stdafx.h"
#include "DhWnd.h"

#include "resource.h"
#include "DMensajesWnd.h"
#include "RAVE_Configuracion.h"

namespace DWL {

	DhWnd_Fuente DhWnd::_Fuente18Normal;
	DhWnd_Fuente DhWnd::_Fuente18Negrita;
	DhWnd_Fuente DhWnd::_Fuente21Normal;
	DhWnd_Fuente DhWnd::_Fuente21Negrita;
	DhWnd_Fuente DhWnd::_FuenteTest;

	bool         DhWnd::_Teclado[256];// = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

	void DhWnd::IniciarMiembrosEstaticos(void) {
		// Creamos las fuentes si no existen, y limpiamos el buffer para el teclado
		if (_Fuente18Normal.Fuente() == NULL) {
			BorrarBufferTeclado();
			_Fuente18Normal		= CreateFont(FUENTE_PEQUE_TAM, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, FUENTE_NORMAL);
			_Fuente18Negrita	= CreateFont(FUENTE_PEQUE_TAM, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, FUENTE_NORMAL);
			_Fuente21Normal		= CreateFont(FUENTE_NORMAL_TAM, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, FUENTE_NORMAL);
			_Fuente21Negrita	= CreateFont(FUENTE_NORMAL_TAM, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, FUENTE_NORMAL);
			_FuenteTest			= CreateFont(26, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, FUENTE_NORMAL);
		}
	}

	void DhWnd::EliminarFuentesEstaticas(void) {
		_Fuente18Normal.Destruir();
		_Fuente18Negrita.Destruir();
		_Fuente21Normal.Destruir();
		_Fuente21Negrita.Destruir();
		_FuenteTest.Destruir();
	}

	ATOM DhWnd::RegistrarClase(const TCHAR *nNombre, WNDPROC WindowProcedureInicial, UINT nEstilos, const int nIconoRecursos, HBRUSH nColorFondo, HINSTANCE hInstance) {
		
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












																																																							        /* NO SE UTILIZA */
	HWND DVentana::CrearVentana(DhWnd *nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase, HMENU nMenu, HBRUSH nColorFondo, const int nIconoRecursos) {
		// La ventana ya no existe pero tenemos el hWnd, por lo que elimino toda la memória
		if (_hWnd != NULL && IsWindow(_hWnd) == 0) {	Destruir();		}

		if (hWnd()) { Debug_Escribir(L"DVentana::Crear() Error : ya se ha creado la ventana\n"); return hWnd(); }
		// hWnd del padre
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : HWND_DESKTOP;

		// Afegit expresament per borrar el fondo amb els colors del RAVE
		static HBRUSH ColFondo = CreateSolidBrush(COLOR_FONDO);

		ATOM RetRgistrarClase = RegistrarClase(nNombre, reinterpret_cast<WNDPROC>(_GestorMensajes), nEstilosClase, nIconoRecursos, ColFondo);
		_hWnd = CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, hWndPadre, nMenu, GetModuleHandle(NULL), this);
		Debug_MostrarUltimoError();
		SendMessage(hWnd(), WM_SETFONT, (WPARAM)_Fuente18Normal(), 0);
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









	HWND DControl::CrearControl(DhWnd *nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, HBRUSH nColorFondo) {
		// La ventana ya no existe pero tenemos el hWnd, por lo que elimino toda la memória
		if (_hWnd != NULL && IsWindow(_hWnd) == 0) { Destruir(); }
		if (hWnd()) { Debug_Escribir(L"DControl::Crear() Error : ya se ha creado el control\n"); return hWnd(); }

		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;

		_hWnd = CreateWindowEx(nEstilosExtendidos, nNombre, nTexto, nEstilos, cX, cY, cAncho, cAlto, hWndPadre, reinterpret_cast<HMENU>(cID), GetModuleHandle(NULL), this);
		return hWnd();
		//	return CreateWindowEx(nEstiloExtendido, nNombre, nTexto, Estilo, cX, cY, cAncho, cAlto, nPadre(), reinterpret_cast<HMENU>(IntToPtr(nID)), GetModuleHandle(NULL), this);
	};


	LRESULT CALLBACK DControl::_GestorMensajes(HWND nhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DControl *Control = reinterpret_cast<DControl *>(GetWindowLongPtr(nhWnd, GWLP_USERDATA));
		if (Control == NULL) return FALSE;
		return Control->GestorMensajes(uMsg, wParam, lParam);
	};

	void DControl::_ConectarControl(const UINT nID, DhWnd *nPadre) {
		if (hWnd() != NULL) {
			SetWindowLongPtr(hWnd(), GWLP_USERDATA, (LONG_PTR)this);
			_GestorMensajesOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd(), GWLP_WNDPROC, (LONG_PTR)_GestorMensajes));

			SendMessage(hWnd(), WM_SETFONT, (WPARAM)_Fuente18Normal(), 0);
		}
	};







};