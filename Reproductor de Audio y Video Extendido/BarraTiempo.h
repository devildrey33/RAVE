#pragma once

#include "DBarraDesplazamientoEx.h"

class BarraTiempo : public DWL::DBarraDesplazamientoEx {
  public:
			BarraTiempo();
		   ~BarraTiempo();
	void    Evento_MostrarToolTip(float nValor, std::wstring &Texto);
};

