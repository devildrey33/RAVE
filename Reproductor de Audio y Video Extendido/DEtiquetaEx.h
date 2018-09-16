#pragma once

#include "DControlEx.h"

namespace DWL {

	class DEtiquetaEx : public DControlEx {
	  public:
								DEtiquetaEx(void);
				               ~DEtiquetaEx(void);
		HWND					CrearEtiquetaEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const BOOL nCentrado = FALSE, const long Estilos = WS_CHILD | WS_VISIBLE);
		void					Pintar(HDC DC);
		void					Texto(std::wstring &nTexto);
		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

		COLORREF                ColorFondo;
		COLORREF                ColorTexto;
		COLORREF                ColorTextoSombra;
	
		DhWnd_Fuente            Fuente;
	  protected:
		UINT				   _Formato;
		std::wstring           _Texto;
	};

}