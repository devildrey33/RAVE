#pragma once

#include "DControlEx.h"

namespace DWL {

	class DLabelEx : public DControlEx {
	  public:
								DLabelEx(void);
				               ~DLabelEx(void);
		HWND					CrearLabelEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const BOOL nCentrado = FALSE, const long Estilos = WS_CHILD | WS_VISIBLE);
		void					PintarLabelEx(HDC DC);
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