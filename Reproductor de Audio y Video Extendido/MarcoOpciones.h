#pragma once

#include "DBarraScrollEx.h"

class MarcoOpciones : public DWL::DBarraScrollEx {
  public:
						MarcoOpciones();
					   ~MarcoOpciones();
	HWND				Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID);
	
	void				Pintar(HDC hDC);

	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

