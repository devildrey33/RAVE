#pragma once

#include "DArbolEx.h"
#include "sqlite3.h"
#include "ToolTipInfo.h"

enum ArbolBD_TipoNodo {
	ArbolBD_TipoNodo_Indefinido = -1,
	ArbolBD_TipoNodo_Genero,
	ArbolBD_TipoNodo_Grupo,
	ArbolBD_TipoNodo_Disco,
	ArbolBD_TipoNodo_Cancion,
	ArbolBD_TipoNodo_Video,
	ArbolBD_TipoNodo_PistaCDAudio,
	ArbolBD_TipoNodo_ListaCanciones,
	ArbolBD_TipoNodo_Subtitulos,
	ArbolBD_TipoNodo_Directorio,
	ArbolBD_TipoNodo_Raiz,
};



class NodoBD : public DWL::DArbolEx_Nodo {
  public :
							NodoBD() : DArbolEx_Nodo(), Id(0), Hash(0), TipoNodo(ArbolBD_TipoNodo_Indefinido) {};
	                       ~NodoBD() {};

	ArbolBD_TipoNodo		TipoNodo;
	sqlite3_int64			Hash;
	UINT					Id;
  private :
};

class BDMedio;

class ArbolBD : public DWL::DArbolEx {
  public:
							ArbolBD(void);
	                       ~ArbolBD(void);
	NodoBD		           *BuscarHash(sqlite3_int64 bHash);
	NodoBD                 *AgregarBDNodo(const ArbolBD_TipoNodo nTipoNodo, NodoBD *nPadre, const TCHAR *cTexto, const sqlite3_int64 nHash, const UINT Id);
	const BOOL				AgregarNodoALista(NodoBD *nNodo);

							// Busca el primer nodo hijo que tiene el texto Buscar
	NodoBD                 *BuscarHijoTxt(std::wstring &Buscar, NodoBD *Padre = NULL);
	inline NodoBD          *BDNodo(const size_t nPos)	{ return static_cast<NodoBD *>(Nodo(nPos)); }

	void					ObtenerPath(NodoBD *nNodo, std::wstring &rPath);
	void					ExplorarPath(NodoBD *nNodo);
							// Falta fer una funció com la de const BOOL ArbolBD::TreeView_Evento_Nodo_Expandiendo(DTreeView_Nodo *nNodo) adaptada a aquesta..... (ArbolBD DEPRECATED.cpp ArbolBD::TreeView_Evento_Nodo_Expandiendo)
	void					Evento_Nodo_Expandido(DWL::DArbolEx_Nodo *nNodo, const BOOL nExpandido);
	void					Evento_MouseSoltado(DWL::DEventoMouse &DatosMouse);
	void					Evento_MouseMovimiento(DWL::DEventoMouse &DatosMouse);

//	void                    Evento_FocoPerdido(HWND hWndNuevoFoco);

/*	void					Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado);
	void					Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado);
	void					Evento_Tecla(DWL::DEventoTeclado &DatosTeclado);*/
	inline NodoBD          *MedioMarcado(void) { return static_cast<NodoBD *>(_NodoMarcado); }
	inline NodoBD          *MedioResaltado(void) { return static_cast<NodoBD *>(_NodoResaltado); }

	void                    ObtenerPathNodo(NodoBD *pNodo, std::wstring &OUT_Path);

	void					Opacidad(const BYTE nNivel);
	const BYTE              Opacidad(void);

  protected:
    void                   _AgregarMedio(NodoBD *nPadre, BDMedio *nMedio);
	ToolTipInfo_Medio      _ToolTipM;
	ToolTipInfo_Etiqueta   _ToolTipE;
};

