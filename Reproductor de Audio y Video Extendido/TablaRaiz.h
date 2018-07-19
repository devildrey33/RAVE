#pragma once

//#include "ConsultaSQLite.h"
#include <vector>
#include <string>
#include "sqlite3.h"

// clase que engloba un tipo
class CeRaiz {
public:
						CeRaiz(void)			: ID_Disco(0), Id(0), Letra('C')								{ };
						CeRaiz(const CeRaiz &c) : ID_Disco(c.ID_Disco), Path(c.Path), Id(c.Id), Letra(c.Letra)	{ };
					   ~CeRaiz(void)																			{ };
	std::wstring        Path;
	unsigned long		ID_Disco;	// Numero de serie de la unidad
	unsigned long		Id;			// Id dentro de la base de datos
	wchar_t             Letra;
};


class TablaRaiz {
  public:
							TablaRaiz(void) : _BD(NULL) { };
							TablaRaiz(sqlite3 * BD);
						   ~TablaRaiz(void);
	const UINT			    ObtenerDatos(sqlite3 *BD);
//	void					CallbackResultado(sqlite3_stmt *SqlQuery, const UINT Pos);
	const BOOL              CrearTabla(sqlite3 *BD);
	std::vector<CeRaiz *>	Datos;

	CeRaiz				   *Buscar_Raiz(const TCHAR *nPath);
	CeRaiz				   *Buscar_RaizPorID(const unsigned long bID);
	const BOOL              Argerar_Raiz(const TCHAR *nPath, CeRaiz *Raiz);
	const BOOL              Eliminar_Raiz(std::wstring &nPath);

  protected:
	void				   _BorrarMemoria(void);
	sqlite3               *_BD;
//	static int			   _Callback_ObtenerDatos(void *pArg, int nCol, char **azVals, char **azCols);
};