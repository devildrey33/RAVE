#pragma once

#include "DBarraDesplazamientoEx.h"

class BarraTiempo : public DWL::DBarraDesplazamientoEx {
  public:
						BarraTiempo(void);
					   ~BarraTiempo(void);
	void				Evento_MostrarToolTip(float nValor, std::wstring &Texto);
						// Función de post pintado para pintar una segunda barra
	void				Evento_PintarPersonalizado(HDC DC, RECT& RFondo);
						// segundo valor
	void				Valor2(const float nValor2);
  protected:
	float              _Valor2;

	DWL::DAnimacion    _AniBarra2;

	COLORREF           _ColorBarra2;
};

