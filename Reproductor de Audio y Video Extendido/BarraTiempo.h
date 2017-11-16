#pragma once

#include "DBarraEx.h"

class BarraTiempo : public DBarraEx {
  public:
			BarraTiempo();
		   ~BarraTiempo();
	void    Evento_MostrarToolTip(float nValor, std::wstring &Texto);
};

