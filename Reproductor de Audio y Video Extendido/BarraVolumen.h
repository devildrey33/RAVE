#pragma once

#include "DBarraDesplazamientoEx.h"

#define BARRAVOLUMEN_MOSTRARDEBUG		FALSE

class BarraVolumen : public DWL::DBarraDesplazamientoEx {
  public:
						BarraVolumen();
	                   ~BarraVolumen();
	void				CrearBarraVolumen(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo = 0.0f, const float nMaximo = 1.0f, const float nValor = 0.0);
	void				Evento_PintarBarra(HDC DC, RECT &RBarra);
	void				Resaltar(const BOOL Resaltado);
	void				Valor(const float nValor);
	inline const float	Valor(void) { return _Valor; }
};

