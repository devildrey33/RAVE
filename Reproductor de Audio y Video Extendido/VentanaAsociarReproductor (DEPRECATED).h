#pragma once

#include "DVentana.h"
#include "DBotonEx.h"
#include "DMarcaEx.h"

class VentanaAsociarReproductor : public DWL::DVentana {
  public:
								VentanaAsociarReproductor(void);
	                           ~VentanaAsociarReproductor(void);
	void                        Mostrar(void);
	void						Pintar(HDC DC);
	void						Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
	void                        Boton_Click(const INT_PTR IdBoton);
	LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
  protected:
	void					   _Evento_Pintar(void);
	DWL::DBotonEx              _BotonCancelar;
	DWL::DBotonEx              _BotonAceptar;
	DWL::DMarcaEx              _MarcaNoMostrarMas;
};

