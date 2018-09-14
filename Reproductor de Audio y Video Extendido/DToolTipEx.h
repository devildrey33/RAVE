#pragma once

#include "DhWnd.h"

namespace DWL {

	#define DTOOLTIPEX_PADDING 4

	class DToolTipEx : public DVentana {
	  public:
							DToolTipEx(void);
		                   ~DToolTipEx(void);
							// Función que crea la ventana del tooltip para futuras interacciones
		HWND				CrearToolTipEx(DhWnd_Fuente Fuente = DhWnd::Fuente18Normal);
		SIZE                CalcularTam(std::wstring &Str);
		void				Mostrar(const int cX, const int cY, std::wstring &Str);
		void				Mostrar(const int cX, const int cY, std::wstring &Str, const int cAncho, const int cAlto);
		void				Ocultar(void);
		void				OcultarAnimado(void);
		virtual void		Pintar(HDC DC);

		LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		std::wstring	   _Str;
		DhWnd_Fuente       _Fuente;
	};

}