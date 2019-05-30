#pragma once

#include "DBarraDesplazamientoEx.h"

class BarraTiempo : public DWL::DBarraDesplazamientoEx {
  public:
			BarraTiempo(void);
		   ~BarraTiempo(void);
	void    Evento_MostrarToolTip(float nValor, std::wstring &Texto);
};

