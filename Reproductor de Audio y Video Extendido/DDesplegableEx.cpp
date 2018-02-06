#include "stdafx.h"
#include "DDesplegableEx.h"

namespace DWL {

	DDesplegableEx::DDesplegableEx() {
	}


	DDesplegableEx::~DDesplegableEx() {
	}


	HWND DDesplegableEx::CrearDesplegable(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const DDesplegableEx_TipoEdicion nTipoEdicion, const DDesplegableEx_TipoDesplegable nTipoDesplegable) {
		if (hWnd()) { Debug_Escribir(L"DDesplegableEx::CrearDesplegable() Error : ya se ha creado el control desplegable\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DDesplegableEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD, NULL, NULL);  // CS_DBLCLKS (el control recibe notificaciones de doble click)
		_TipoEdicion		= nTipoEdicion;
		_TipoDesplegable	= nTipoDesplegable;
		_Fuente				= _Fuente18Normal;

		// Creo el tipo de desplegable especificado
		switch (_TipoDesplegable) {
			case DDesplegableEx_TipoDesplegable_Lista :
				break;
			case DDesplegableEx_TipoDesplegable_Arbol :
				break;
			case DDesplegableEx_TipoDesplegable_ExplorarDirectorios :
				break;
		}

		// Creo un editbox o un LabelEx según el tipo de edición
		switch (_TipoEdicion) {
			case DDesplegableEx_TipoEdicion_SinEdicion :
				break;
			case DDesplegableEx_TipoEdicion_Texto :
				break;
			case DDesplegableEx_TipoEdicion_NumericoEntero :
				break;
			case DDesplegableEx_TipoEdicion_NumericoDecimal :
				break;
		}

		return hWnd();

	}


	void DDesplegableEx::MostrarDesplegable(void) {

	}

	void DDesplegableEx::Pintar(HDC hDC) {

	}

	void DDesplegableEx::_Evento_Pintar(void) {
		HDC         DC;
		PAINTSTRUCT PS;
		DC = BeginPaint(hWnd(), &PS);
		Pintar(DC);
		EndPaint(hWnd(), &PS);
	}

	void DDesplegableEx::_Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
	}

	void DDesplegableEx::_Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params) {
	}

	void DDesplegableEx::_Evento_MouseMovimiento(const int cX, const int cY, const UINT Param) {
	}

	void DDesplegableEx::_Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param) {
	}

	void DDesplegableEx::_Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param) {
	}

	void DDesplegableEx::_Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey) {
	}

	void DDesplegableEx::_Evento_MouseSaliendo(void) {
	}

	LRESULT CALLBACK DDesplegableEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_SETFOCUS:		BorrarBufferTeclado();																														return 0;
			case WM_KILLFOCUS:		BorrarBufferTeclado();																														return 0;

			case WM_KEYDOWN:		_Evento_TeclaPresionada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;
			case WM_KEYUP:			_Evento_TeclaSoltada(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));															return 0;		

			case WM_SIZE:			Repintar();																																	return 0;

			case WM_PAINT:			_Evento_Pintar();																															return 0;

			case WM_MOUSEMOVE:		_Evento_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;

			case WM_MOUSELEAVE:		_Evento_MouseSaliendo();																													return 0;

			case WM_MOUSEWHEEL:		_Evento_MouseRueda(static_cast<short>(HIWORD(wParam)), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(LOWORD(wParam)));		return 0;

			case WM_LBUTTONDOWN:	_Evento_MousePresionado(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_RBUTTONDOWN:	_Evento_MousePresionado(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_MBUTTONDOWN:	_Evento_MousePresionado(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;

			case WM_LBUTTONUP:		_Evento_MouseSoltado(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;
			case WM_RBUTTONUP:		_Evento_MouseSoltado(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;
			case WM_MBUTTONUP:		_Evento_MouseSoltado(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));												return 0;

/*			case WM_LBUTTONDBLCLK:	_Evento_MouseDobleClick(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_RBUTTONDBLCLK:	_Evento_MouseDobleClick(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;
			case WM_MBUTTONDBLCLK:	_Evento_MouseDobleClick(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));											return 0;*/

		}
		return DefWindowProc(hWnd(), uMsg, wParam, lParam);
	}

}