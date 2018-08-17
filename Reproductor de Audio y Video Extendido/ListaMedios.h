#pragma once

#include "DListaEx.h"
//#include "TablaMedios.h"

class ItemMedio : public DWL::DListaEx_Item {
  public:
						ItemMedio(void) : Hash(0)  {};
		               ~ItemMedio(void) {};

    sqlite3_int64		Hash;
};

class BDMedio;

class ListaMedios : public DWL::DListaEx {
  public:
						ListaMedios(void);
	                   ~ListaMedios(void);
//	ItemMedio          *AgregarMedio(const int nIcono, const TCHAR *nPista, const TCHAR *nNombre, const TCHAR *nTiempo, const sqlite3_int64 nHash);

	ItemMedio		   *AgregarMedio(BDMedio *nMedio);
	void				BorrarListaReproduccion(void);

	const BOOL			Mezclar(const BOOL nMezclar);

	ItemMedio          *BuscarHash(sqlite3_int64 Hash);

	inline ItemMedio   *Medio(const size_t nPos) { 
							return static_cast<ItemMedio *>(_Items[nPos]); 
						}

	void				Evento_MouseDobleClick(DWL::DEventoMouse &EventoMouse);

	size_t              MedioActual;
	size_t              MedioActualOrdenado;
	size_t             _ItemMarcadoOriginal;

	int                 Errores;

	std::vector<ItemMedio *> _MediosOrdenados;
};

