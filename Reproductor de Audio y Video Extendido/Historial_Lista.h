#pragma once

#include "Historial_Fecha.h"

class Historial_Lista {
  public:
						//Historial_Lista(void) { };
						// Constructores para asignar el nombre y la fecha actual					(usados para guardar la lista)
						Historial_Lista(std::wstring &nNombre)  : Nombre(nNombre), Id(0) { };
						Historial_Lista(const wchar_t *nNombre) : Nombre(nNombre), Id(0) { };
						// Constructores para asignar el nombre, la fecha especificada, y la Id		(usados para obtener la lista)
						Historial_Lista(std::wstring &nNombre, std::wstring &nFecha, const UINT64 nId)   : Nombre(nNombre), Fecha(nFecha), Id(nId) { };
						Historial_Lista(const wchar_t *nNombre, const wchar_t *nFecha, const UINT64 nId) : Nombre(nNombre), Fecha(nFecha), Id(nId) { };

	std::wstring		Nombre;
	Historial_Fecha		Fecha;
	UINT64              Id;
};

