#pragma once

#include "TGenero.h"


// clase que engloba un grupo
class TGrupo {
public:
						  TGrupo(void) : Nombre(NULL), StrTipo(NULL), StrGenero(NULL), Tipo(NULL), Genero(NULL), Canciones(0), Discos(0), Longitud(0), Tiempo(0)/*, Nodo(NULL)*/ {  };
					     ~TGrupo(void) { };
	StrTp                *Nombre;
	StrTp                *StrTipo;
	StrTp                *StrGenero;
	TTipo                *Tipo;
	TGenero              *Genero;
	unsigned int          Canciones;
	unsigned int          Discos;
	unsigned long         Longitud;
	UINT64                Tiempo;
//	NodoArbolBaseDatos   *Nodo;
};
