#pragma once

#include "TRaiz.h"

//class NodoArbolBaseDatos;

// clase que engloba un tipo
class TTipo {
public:
						TTipo(void) : Nombre(NULL), Canciones(0), Generos(0), Grupos(0), Discos(0), Longitud(0), Tiempo(0)/*, Nodo(NULL)*/ {  };
					   ~TTipo(void) { };
	StrTp              *Nombre;
	UINT64              Tiempo;
	unsigned long       Longitud;
	unsigned int		Canciones;
	unsigned int	    Generos;
	unsigned int        Grupos;
	unsigned int		Discos;
//	NodoArbolBaseDatos *Nodo;
};
