#pragma once

#include "DhWnd.h"

namespace DWL {

	class DToolTipEx : public DVentana {
	public:
							DToolTipEx();
		                   ~DToolTipEx();
		// Función que crea la ventana del tooltip para futuras interacciones
		HWND				CrearToolTipEx();

		void				MostrarToolTipEx(const int cX, const int cY, std::wstring &Str);
		void				OcultarToolTipEx(void);
		virtual void		PintarToolTipEx(HDC DC);

		LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		std::wstring	   _Str;
	};

}