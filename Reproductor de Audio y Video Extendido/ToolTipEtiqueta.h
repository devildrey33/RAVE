#pragma once

#include "EtiquetaBD.h"
#define POS_MOUSE -1

class ToolTipEtiqueta : public DWL::DVentana {
  public:
							ToolTipEtiqueta(void);
							ToolTipEtiqueta(EtiquetaBD &nEtiqueta, const int PosX = POS_MOUSE, const int PosY = POS_MOUSE);
			               ~ToolTipEtiqueta(void);

	void					Mostrar(EtiquetaBD &nEtiqueta, const int PosX = POS_MOUSE, const int PosY = POS_MOUSE);
	void					Pintar(HDC DC);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	inline std::wstring    &Texto(void) { return _Etiqueta.Texto; }
  protected:

	void			       _Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam);
	void			       _Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam);
	void		    	   _Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam);
	int                    _Tam[3];
	const int              _CalcularAncho(void);
	void                   _Evento_Pintar(void);
	EtiquetaBD             _Etiqueta;
};

