#include "stdafx.h"
#include "MarcoOpciones.h"


MarcoOpciones::MarcoOpciones() {
}


MarcoOpciones::~MarcoOpciones() {
}

HWND MarcoOpciones::Crear(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID) {
	DControlEx::CrearControlEx(nPadre, L"MarcoOpciones", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);

	ScrollV_Pagina(25.0f);
	ScrollV_Visible(TRUE);
	
	ScrollH_Pagina(50.0f);
	ScrollH_Visible(TRUE);

	return hWnd();
}

void MarcoOpciones::Pintar(HDC hDC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HDC     Buffer = CreateCompatibleDC(NULL);
	HBITMAP Bmp = CreateCompatibleBitmap(hDC, RC.right, RC.bottom);
	HBITMAP Viejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

	RECT RCC;
	ObtenerRectaCliente(&RCC);
	HBRUSH Brocha = CreateSolidBrush(_ColorFondo);
	FillRect(Buffer, &RCC, Brocha);
	DeleteObject(Brocha);
	Scrolls_Pintar(Buffer);

	BitBlt(hDC, 0, 0, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

	SelectObject(Buffer, Viejo);
	DeleteObject(Bmp);
	DeleteDC(Buffer);

}



LRESULT CALLBACK MarcoOpciones::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND:
			return SendMessage(hWnd.Padre(), uMsg, wParam, lParam);

		case WM_SIZE:
			Repintar();
			return 0;


		case WM_PAINT:
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(hWnd(), &PS);
			Pintar(DC);
			EndPaint(hWnd(), &PS);
			return 0;
		case WM_MOUSEMOVE:
			if (_MouseEntrando() == TRUE) {
				// MouseEntrando
			}

			Scrolls_MouseMovimiento(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
			break;
		case WM_MOUSELEAVE:
			_MouseDentro = FALSE;
			if (_ScrollV_Estado != DBarraScrollEx_Estado_Presionado && _ScrollH_Estado != DBarraScrollEx_Estado_Presionado) {
				if (_ScrollV_Estado != DBarraScrollEx_Estado_Invisible) _ScrollV_Estado = DBarraScrollEx_Estado_Normal;
				if (_ScrollH_Estado != DBarraScrollEx_Estado_Invisible) _ScrollH_Estado = DBarraScrollEx_Estado_Normal;
				Repintar();
			}
			break;
		case WM_LBUTTONDOWN:
			Scrolls_MousePresionado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
			break;
		case WM_LBUTTONUP:
			Scrolls_MouseSoltado(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<UINT>(wParam));
			break;

	}
	return DControlEx::GestorMensajes(uMsg, wParam, lParam);
}