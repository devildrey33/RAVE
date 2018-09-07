#pragma once

#include "DListaEx.h"
#include "ToolTipBD.h"

class ItemMedio : public DWL::DListaEx_Item {
  public:
						ItemMedio(void) : Hash(0)  {};
		               ~ItemMedio(void) {};

    sqlite3_int64		Hash;
	UINT                Id;
};

class BDMedio;

class ListaMedios : public DWL::DListaEx {
  public:
								ListaMedios(void);
				               ~ListaMedios(void);

	ItemMedio		           *AgregarMedio(BDMedio *nMedio);
	void						BorrarListaReproduccion(void);

	const BOOL					Mezclar(const BOOL nMezclar);

	ItemMedio                  *BuscarHash(sqlite3_int64 Hash);

	inline ItemMedio           *Medio(const size_t nPos) { return static_cast<ItemMedio *>(_Items[nPos]); 						}

	inline ItemMedio		   *MedioResaltado(void) { return static_cast<ItemMedio *>(ItemResaltado()); }
	inline ItemMedio           *UMedioResaltado(void) { return static_cast<ItemMedio *>((_ItemUResaltado == -1) ? NULL : _Items[_ItemUResaltado]); }

	void						Evento_MouseDobleClick(DWL::DEventoMouse &EventoMouse);
	void						Evento_MouseSoltado(DWL::DEventoMouse &DatosMouse);
	void						Evento_MouseMovimiento(DWL::DEventoMouse &DatosMouse);

	size_t						MedioActual;
	size_t						MedioActualOrdenado;

	int							Errores;

   protected:

	size_t                    _ItemMarcadoOriginal;
	std::vector<ItemMedio *>  _MediosOrdenados;
	ToolTipBD                 _ToolTip;
};

