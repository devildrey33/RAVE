#pragma once
#include "TTipo.h"

class NodoArbolBaseDatos;


// clase que engloba un genero
class TGenero {
public:
							TGenero(void) : Nombre(NULL), StrTipo(NULL), Tipo(NULL), Canciones(0), Grupos(0), Discos(0), Longitud(0), Tiempo(0)/*, Nodo(NULL)*/ {  };
						   ~TGenero(void) { };
	StrTp                  *Nombre;
	StrTp                  *StrTipo;
	TTipo                  *Tipo;
	UINT64                  Tiempo;
	unsigned long           Longitud;
	unsigned int			Canciones;
	unsigned int		    Grupos;
	unsigned int            Discos;
//	NodoArbolBaseDatos     *Nodo;
};