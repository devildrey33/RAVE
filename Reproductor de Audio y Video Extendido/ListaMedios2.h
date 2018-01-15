#pragma once

#include "DListaEx.h"


class ItemMedio2 : public DWL::DListaEx_Item {
  public:
						ItemMedio2(void) : Hash(0)  {};
		               ~ItemMedio2(void) {};

    sqlite3_int64		Hash;
};



class ListaMedios2 : public DWL::DListaEx {
  public:
					ListaMedios2();
	               ~ListaMedios2();
	ItemMedio2     *AgregarMedio(const int nIcono, const TCHAR *nPista, const TCHAR *nNombre, const TCHAR *nTiempo, const sqlite3_int64 nHash);

	inline ItemMedio2 *Medio(const size_t nPos) { return static_cast<ItemMedio2 *>(_Items[nPos]); }
};

