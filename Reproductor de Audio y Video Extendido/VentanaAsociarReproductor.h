#pragma once

#include "DhWnd.h"
#include "DBotonEx.h"
#include "DMarcaEx.h"

class VentanaAsociarReproductor : public DWL::DVentana {
  public:
								VentanaAsociarReproductor(void);
	                           ~VentanaAsociarReproductor(void);
	void                        Mostrar(void);
	void						Pintar(HDC DC);
	void						Evento_BotonEx_Mouse_Click(const UINT cID);
	LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
  protected:
	void					   _Evento_Pintar(void);
	DWL::DBotonEx              _BotonCancelar;
	DWL::DBotonEx              _BotonAceptar;
	DWL::DMarcaEx              _MarcaNoMostrarMas;
};

