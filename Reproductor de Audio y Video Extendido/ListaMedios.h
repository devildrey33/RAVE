#pragma once

#include "ItemMedio.h"
#include "ToolTipInfo.h"



class ListaMedios : public DWL::DListaEx {
  public:
								ListaMedios(void);
				               ~ListaMedios(void);

								// Función que agrega un medio a la lista
	ItemMedio		           *AgregarMedio(BDMedio *nMedio);

								// Función que agrega los medios de un m3u a la lista
	void						AgregarM3u(std::wstring &PathM3u);

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
	ItemMedio                  *MedioAnterior(ItemMedio *nMedio);
//	ItemMedio					*MedioActualOrdenado;
	
	ItemMedio				   *MedioActual;

	inline ItemMedio           *MedioPrimero(void)			{ return (_Items.size() == 0) ? NULL : static_cast<ItemMedio*>(_Items[0]); };
	inline ItemMedio	       *MedioUltimo(void)			{ return (_Items.size() == 0) ? NULL : static_cast<ItemMedio*>(_Items[_Items.size() - 1]); };

	void						Opacidad(const BYTE nNivel);
	const BYTE                  Opacidad(void);


	int							Errores;
								// Posición en la lista del medio especificado (puede ser -1 si no hay medio actual)
	const LONG_PTR              PosMedio(ItemMedio *pMedio);

	
   protected:

								// Función que parsea un M3u previamente cargado en memoria 
	void					   _ParsearM3u(std::wstring &PathM3u, const char* Datos);


//	size_t                     _ItemMarcadoOriginal;
	std::vector<ItemMedio *>   _MediosOrdenados;
	ToolTipInfo_Medio          _ToolTip;
};

