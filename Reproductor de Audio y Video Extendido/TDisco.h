#pragma once
#include "TGrupo.h"


// clase que engloba un disco
class TDisco {
public:
							TDisco(void) : Nombre(NULL), StrTipo(NULL), StrGenero(NULL), StrGrupo(NULL), Tipo(NULL), Genero(NULL), Grupo(NULL), Canciones(0), Longitud(0), Tiempo(0)/*, Nodo(NULL)*/ {  }; 
						   ~TDisco(void) { };
	StrTp                  *Nombre;
	StrTp                  *StrTipo;
	StrTp                  *StrGenero;
	StrTp                  *StrGrupo;
	TTipo                  *Tipo;
	TGenero                *Genero;
	TGrupo                 *Grupo;
	unsigned int			Canciones;
	unsigned long			Longitud;
	UINT64					Tiempo;
//	NodoArbolBaseDatos     *Nodo;
};
