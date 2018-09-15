#pragma once

#include "DhWnd.h"

class VerVideo : public DWL::DControlEx {
public:
						VerVideo();
	                   ~VerVideo();
	HWND				Crear(DWL::DhWnd *Padre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID);
	void				Pintar(HDC hDC);
	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

