#pragma once

#include "Historial_Lista.h"
#include "BDMedio.h"

class Historial_Medio {
  public:
						Historial_Medio(const LONG_PTR IDMedio, const LONG_PTR IDLista = -1);
	LONG_PTR			IdLista;
	LONG_PTR			IdMedio;
	Historial_Fecha		Fecha;
};

