#pragma once

#include "Historial_Fecha.h"

class Historial_Lista {
  public:
						Historial_Lista(void) { };
						Historial_Lista(std::wstring &nNombre) : Nombre(nNombre) { };
						Historial_Lista(const wchar_t *nNombre) : Nombre(nNombre) { };
	std::wstring		Nombre;
	Historial_Fecha		Fecha;
};

