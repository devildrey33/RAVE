#pragma once

#include "DBarraDesplazamientoEx.h"

class BarraVolumen : public DWL::DBarraDesplazamientoEx {
  public:
			BarraVolumen();
	       ~BarraVolumen();
	void	Evento_PintarBarra(HDC DC, RECT &RBarra);

};

