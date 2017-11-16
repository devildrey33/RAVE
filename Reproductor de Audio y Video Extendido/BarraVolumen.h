#pragma once

#include "DBarraEx.h"

class BarraVolumen : public DWL::DBarraEx {
  public:
			BarraVolumen();
	       ~BarraVolumen();
	void	Evento_PintarBarra(HDC DC, RECT &RBarra);

};

