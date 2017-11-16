#pragma once

#include "DTreeView.h"
#include "sqlite3.h"

using namespace DWL;

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

class ArbolBD;

class ArbolBD_Nodo : public DTreeView_Nodo {
public:
							ArbolBD_Nodo() : TipoNodo(ArbolBD_TipoNodo_Indefinido), Hash(0) {};
					       ~ArbolBD_Nodo() { };
	
	inline ArbolBD_Nodo	   *BDPadre(void)		{ return static_cast<ArbolBD_Nodo *>(Padre());		};
	inline ArbolBD_Nodo	   *BDHijo(void)		{ return static_cast<ArbolBD_Nodo *>(Hijo());		};
	inline ArbolBD_Nodo	   *BDSiguiente(void)	{ return static_cast<ArbolBD_Nodo *>(Siguiente());	};
	inline ArbolBD_Nodo	   *BDAnterior(void)	{ return static_cast<ArbolBD_Nodo *>(Anterior());	};

	ArbolBD_TipoNodo		TipoNodo;
	sqlite3_int64			Hash;
	friend class ArbolBD;
	
};

class ArbolBD : public DTreeView {
  public:
							ArbolBD(void);
	                       ~ArbolBD(void);
	const BOOL				TreeView_Evento_Nodo_Expandiendo(DTreeView_Nodo *nNodo);

	inline ArbolBD_Nodo    *BDNodo(const size_t Pos) { return static_cast<ArbolBD_Nodo *>(_Nodos[Pos]); };

	ArbolBD_Nodo           *BuscarHash(sqlite3_int64 bHash);
//	LRESULT				Evento_Mouse_BotonSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);
//	LRESULT             Evento_Mouse_Click(DTreeView_Nodo *cNodo, const UINT nBoton, const int cX, const int cY);

//	void				AgregarNodo();
	ArbolBD_Nodo           *AgregarBDNodo(const ArbolBD_TipoNodo nTipoNodo, ArbolBD_Nodo *nPadre, const TCHAR *cTexto, const sqlite3_int64 nHash = 0);

	const BOOL				AgregarNodoALista(DTreeView_Nodo *nNodo);
};

