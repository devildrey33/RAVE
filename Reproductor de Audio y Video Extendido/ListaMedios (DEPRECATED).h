#pragma once

#include "DListView.h"
#include "sqlite3.h"
#include "TablaMedios.h"

/*
class ItemMedio : public DWL::DListView_Item {
  public:
				ItemMedio(void) : Hash(0)  {};
		       ~ItemMedio(void) {};

    sqlite3_int64     Hash;
};


class ListaMedios : public DWL::DListView {
  public:
						ListaMedios();
	                   ~ListaMedios();

	void				AgregarMedio(TablaMedios_Medio *nMedio);
	void				BorrarListaReproduccion(void);

	ItemMedio          *BuscarHash(sqlite3_int64 Hash);

	inline ItemMedio   *Medio(const size_t nPos) { return static_cast<ItemMedio *>(Item(nPos)); };
				// Posición de la lista
	int					Pos;
};

*/