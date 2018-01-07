#pragma once

#include "DhWnd.h"

class MarcoControles : public DWL::DControlEx { 
  public:
						MarcoControles() : DControlEx() { };
					   ~MarcoControles() { };
	HWND				Crear(DWL::DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID);
	void				Evento_BorraFondo(HDC DC);
	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

