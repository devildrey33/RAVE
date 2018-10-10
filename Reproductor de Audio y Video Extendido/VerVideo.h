#pragma once

#include "DControlEx.h"

class VerVideo : public DWL::DControlEx {
public:
						VerVideo();
	                   ~VerVideo();
	HWND				Crear(DWL::DhWnd *Padre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID);
	void				Pintar(HDC hDC);
//	void				Evento_MenuEx_Click(const UINT cID);
	void			   _Evento_MouseRueda(WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

