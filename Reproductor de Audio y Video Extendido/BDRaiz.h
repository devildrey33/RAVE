#pragma once

#include <string>

// clase que engloba una raíz
class BDRaiz {
  public:
						BDRaiz(void)				: ID_Disco(0), Id(0), Letra('C') { };
						BDRaiz(std::wstring &nPath) : ID_Disco(0), Id(0), Letra('C'), Path(nPath) { };
						BDRaiz(const BDRaiz &c)		: ID_Disco(c.ID_Disco), Path(c.Path), Id(c.Id), Letra(c.Letra) { };
	                   ~BDRaiz(void) { };
	std::wstring        Path;
	unsigned long		ID_Disco;	// Numero de serie de la unidad
	unsigned long		Id;			// Id dentro de la base de datos
	wchar_t             Letra;		// Letra de la unidad actual
};