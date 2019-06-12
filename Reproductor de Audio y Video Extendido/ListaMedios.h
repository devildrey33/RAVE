#pragma once

#include "DListaEx.h"
#include "ToolTipInfo.h"

class ItemMedio : public DWL::DListaEx_Item {
  public:
						ItemMedio(void) : Hash(0), Id(0)  {};
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
	void						ReproducirMedio(BDMedio &nMedio, LONG PosMomento = -1);

	const BOOL					Mezclar(const BOOL nMezclar);

	ItemMedio                  *BuscarHash(sqlite3_int64 Hash);

	inline ItemMedio           *Medio(const LONG_PTR nPos)	{ return static_cast<ItemMedio *>(_Items[static_cast<unsigned int>(nPos)]); 											}
	inline ItemMedio           *MedioMarcado(void)			{ return static_cast<ItemMedio *>(ItemMarcado());																		}

	inline ItemMedio		   *MedioResaltado(void)		{ return static_cast<ItemMedio *>(ItemResaltado());																		}
	inline ItemMedio           *UMedioResaltado(void)		{ return static_cast<ItemMedio *>((_ItemUResaltado == -1) ? NULL : _Items[static_cast<unsigned int>(_ItemUResaltado)]);	}

	/*void                        Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado);
	void                        Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado);
	void						Evento_Tecla(DWL::DEventoTeclado &DatosTeclado);*/

	void						Evento_MouseDobleClick(DWL::DEventoMouse &EventoMouse);
	void						Evento_MouseSoltado(DWL::DEventoMouse &DatosMouse);
	void						Evento_MouseMovimiento(DWL::DEventoMouse &DatosMouse);
	void                        Evento_MouseSaliendo(void);
//	void						Evento_FocoPerdido(HWND hWndNuevoFoco);

	void                        Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado);

	ItemMedio                  *MedioSiguiente(ItemMedio *nMedio);
	ItemMedio                  *MedioAnterior(ItemMedio *nMedio, const BOOL SituarAlFinal = TRUE);
//	ItemMedio					*MedioActualOrdenado;
	ItemMedio				   *MedioActual;

	void						Opacidad(const BYTE nNivel);
	const BYTE                  Opacidad(void);


	int							Errores;
								// Posición en la lista del medio especificado (puede ser -1 si no hay medio actual)
	const LONG_PTR              PosMedio(ItemMedio *pMedio);
   protected:

//	size_t                     _ItemMarcadoOriginal;
	std::vector<ItemMedio *>   _MediosOrdenados;
	ToolTipInfo_Medio          _ToolTip;
};

