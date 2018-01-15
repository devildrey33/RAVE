#pragma once

#include "DBarraDesplazamientoEx.h"

#define BARRAVOLUMEN_MOSTRARDEBUG		FALSE

class BarraVolumen : public DWL::DBarraDesplazamientoEx {
  public:
			BarraVolumen();
	       ~BarraVolumen();
	void	Evento_PintarBarra(HDC DC, RECT &RBarra);

};

