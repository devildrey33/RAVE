#pragma once

#include "DhWnd.h"

namespace DWL {

	class DLabelEx : public DControlEx_TextoEstatico, public DControlEx_FondoEstatico {
	  public:
								DLabelEx(void);
				               ~DLabelEx(void);
		HWND					CrearLabelEx(DhWnd &nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const BOOL nCentrado = FALSE, const long Estilos = WS_CHILD | WS_VISIBLE);
		void					PintarLabelEx(HDC DC);
		void					Texto(std::wstring &nTexto);
		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		UINT				   _Formato;
	};

}