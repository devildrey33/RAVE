#pragma once

#include "StrTp.h"

//class NodoArbolBaseDatos;

class ArbolBD_Nodo;

// clase que engloba un tipo
class TRaiz {
public:
						TRaiz(void)				: ID_Disco(0), Nodo(NULL)							{ };
						TRaiz(const TRaiz &c)	: ID_Disco(c.ID_Disco), Path(c.Path), Nodo(NULL)	{ };
					   ~TRaiz(void)																	{ };
	std::wstring        Path;
	unsigned long		ID_Disco;	// ID del disco duro padre
	ArbolBD_Nodo       *Nodo;

	//	NodoArbolBaseDatos *Nodo;
};
