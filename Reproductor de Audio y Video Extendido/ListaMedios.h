#pragma once

#include "DListaEx.h"
#include "TablaMedios.h"

class ItemMedio : public DWL::DListaEx_Item {
  public:
						ItemMedio(void) : Hash(0)  {};
		               ~ItemMedio(void) {};

    sqlite3_int64		Hash;
};



class ListaMedios : public DWL::DListaEx {
  public:
						ListaMedios();
	                   ~ListaMedios();
//	ItemMedio          *AgregarMedio(const int nIcono, const TCHAR *nPista, const TCHAR *nNombre, const TCHAR *nTiempo, const sqlite3_int64 nHash);

	void				AgregarMedio(TablaMedios_Medio *nMedio);
	void				BorrarListaReproduccion(void);

	ItemMedio          *BuscarHash(sqlite3_int64 Hash);

	inline ItemMedio   *Medio(const size_t nPos) { return static_cast<ItemMedio *>(_Items[nPos]); }

	size_t              MedioActual;
};

