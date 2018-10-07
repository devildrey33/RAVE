#pragma once

/* Control separador para las opciones, viene a ser como un <hr /> */

class ControlSeparador : public DWL::DControlEx {
  public:
						ControlSeparador();
		               ~ControlSeparador();
					    // Función que crea el control separador (el alto debe se impar, o no respondo por la alineación vertical)
	void				Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto = 5);
	void                Pintar(HDC DC);
	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

