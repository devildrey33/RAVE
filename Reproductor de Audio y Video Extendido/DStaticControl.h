#ifndef DSTATICCONTROL_H
#define DSTATICCONTROL_H

#include "DhWnd.h"


namespace DWL {

	class DStaticControl : public DControl {
	public:
							DStaticControl() : DControl() { };
		                   ~DStaticControl() { };
		HWND				Crear(DhWnd &nPadre, const wchar_t *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos = WS_CHILD, BOOL nVisible = TRUE);
		void				Texto(const wchar_t *nTexto);

/*		void				Evento_Pintar(void);

		LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);*/

	};

};
#endif