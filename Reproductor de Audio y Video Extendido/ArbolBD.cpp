#include "stdafx.h"
#include "ArbolBD.h"
#include "DStringUtils.h"

ArbolBD::ArbolBD() {
}


ArbolBD::~ArbolBD() {
}
/*
LRESULT ArbolBD::Evento_Mouse_BotonSoltado(const UINT Boton, const int cX, const int cY, const UINT Param) {
	if (Boton == 1) 
		App.Menu_ArbolBD.Mostrar(_hWnd);
	return 0;
}*/


ArbolBD_Nodo *ArbolBD::AgregarBDNodo(const ArbolBD_TipoNodo nTipoNodo, ArbolBD_Nodo *nPadre, const TCHAR *cTexto, const sqlite3_int64 nHash) {

	// Busco si existe el hash
	if (nTipoNodo == ArbolBD_TipoNodo_Cancion || nTipoNodo == ArbolBD_TipoNodo_Video) {
		for (size_t i = 0; i < _Nodos.size(); i++) {
			if (BDNodo(i)->Hash == nHash) {
				return BDNodo(i);
			}
		}
	}

	
	TVINSERTSTRUCT ITS;
	TVITEMEX       Item;
	static TCHAR   Tmp[512];
	ArbolBD_Nodo *nNodo = new ArbolBD_Nodo;
	nNodo->_TV = this;

	wcscpy_s(Tmp, 512, cTexto);
	Item.pszText = Tmp;
	Item.cchTextMax = static_cast<int>(wcslen(Tmp));
	Item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE | TVIF_CHILDREN;

	Item.iImage = static_cast<int>(nTipoNodo);
	Item.iSelectedImage = static_cast<int>(nTipoNodo);

	switch (nTipoNodo) {
		case  ArbolBD_TipoNodo_Raiz:
			Item.state = TVIS_BOLD | TVIS_EXPANDED;
			Item.stateMask = TVIS_BOLD | TVIS_EXPANDED;
			Item.cChildren = 1;
			break;
		case ArbolBD_TipoNodo_Directorio:
			Item.state = 0;
			Item.stateMask = 0;
			Item.cChildren = 1;
			break;
		default:
			Item.state = 0;
			Item.stateMask = 0;
			Item.cChildren = 0;
			break;
	}
/*	Item.iImage = cIko;
	Item.iSelectedImage = cIko;
	Item.state = Estado;
	Item.stateMask = MascaraEstado;*/
	//												Item.cChildren		= 0;
	Item.lParam = static_cast<LPARAM>(PtrToLong(nNodo));
	//												Item.lParam			= (LPARAM)this;
	ITS.hInsertAfter = TVI_LAST;
	ITS.itemex = Item;
	ITS.hParent = (nPadre) ? nPadre->_ID : TVI_ROOT;

	//												HTREEITEM nID = TreeView_InsertItem(_hWnd, &ITS);
	nNodo->TipoNodo = nTipoNodo;
	nNodo->Hash = nHash;
	nNodo->_ID = TreeView_InsertItem(hWnd(), &ITS);
	_Nodos.push_back(nNodo);
	return nNodo;
}


/* TODO : No muestra los medios de las carpetas que contienen subcarpetas con mas medios */
const BOOL ArbolBD::TreeView_Evento_Nodo_Expandiendo(DTreeView_Nodo *nNodo) {
	if (nNodo == NULL) return FALSE;
	ArbolBD_Nodo *BDNodo = static_cast<ArbolBD_Nodo *>(nNodo);
	ArbolBD_Nodo *TmpNodo = BDNodo->BDHijo();

	// Si no hay hijos cargo las canciones correspondientes.
//	if (TmpNodo == NULL) {
	std::wstring Path, TmpStr, TmpPath, TmpRaiz;
	TmpNodo = BDNodo;
	do {
		TmpNodo->ObtenerTexto(TmpStr);
		TmpNodo = TmpNodo->BDPadre();
		if (TmpNodo)	Path = TmpStr + L"\\" + Path;
	} while (TmpNodo != NULL);

	size_t TAP = 0; // Total antibarras path
	size_t TAR = 0; // Total antibarras raiz
	size_t TAM = 0; // Total antibarras medios

	for (size_t i = 0; i < Path.size(); i++) {
		if (Path[i] == L'\\') TAP++;
	}

	std::wstring SqlStr = L"SELECT * FROM Medios WHERE Path LIKE \"%" + Path + L"%\" COLLATE NOCASE";
	sqlite3_stmt   *SqlQuery = NULL;
	int SqlRet = sqlite3_prepare16_v2(App.BD(), SqlStr.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		const char *Error = sqlite3_errmsg(App.BD());
		return FALSE;
	}


	sqlite3_int64 Hash = 0;
	int           PosRaiz = -1;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			TmpStr = DWL::DString_ToStr(static_cast<UINT>(sqlite3_column_int(SqlQuery, 13)), 2) + L" " + reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
			//				std::wstring TmpHash = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			Hash = sqlite3_column_int64(SqlQuery, 1);
			if (PosRaiz == -1) {
				PosRaiz = static_cast<UINT>(sqlite3_column_int(SqlQuery, 8));
				TmpRaiz = App.BD.Tabla_Raiz.Buscar_RaizPorID(PosRaiz)->Path;
				for (size_t i2 = 0; i2 < TmpRaiz.size(); i2++) {
					if (TmpRaiz[i2] == L'\\') TAR++;
				}
			}

			TmpPath = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
			TAM = 0;
			for (size_t i3 = 0; i3 < TmpPath.size(); i3++) {
				if (TmpPath[i3] == L'\\') TAM++;
			}

			if ((TAP + TAR) == TAM)
				AgregarBDNodo(ArbolBD_TipoNodo_Cancion, BDNodo, TmpStr.c_str(), Hash);
		}
	}
	sqlite3_finalize(SqlQuery);
	//}
	return FALSE;
};

/*
ArbolBD_Nodo *ArbolBD::BDNodo(const size_t Pos) {
	return static_cast<ArbolBD_Nodo *>(_Nodos[Pos]);
}*/


const BOOL ArbolBD::AgregarNodoALista(DTreeView_Nodo *nNodo) {
	if (nNodo == NULL) return FALSE;
	ArbolBD_Nodo *TmpNodo = static_cast<ArbolBD_Nodo *>(nNodo);
	sqlite3_int64 Hash = TmpNodo->Hash;
	//	ArbolBD_Nodo *TmpNodo = aNodo->BDHijo();
	// Si no hay hijos cargo las canciones correspondientes.
	std::wstring Path, TmpStr;

	if (Hash == 0) { // Si no hay hash es que el nodo contiene uno o mas hijos
		do {
			TmpNodo->ObtenerTexto(TmpStr);
			TmpNodo = TmpNodo->BDPadre();
			if (TmpNodo) {
				Path = TmpStr + L"\\" + Path;
			}
		} while (TmpNodo != NULL);
	}

	std::wstring SqlStr;
	int i = nNodo->Icono();
	switch (nNodo->Icono()) {
		
		case 3 : // Canción
		case 4 : // Video
			SqlStr = L"SELECT * FROM Medios WHERE Hash =\"" + DString_ToStr(Hash) + L"\"";
//			SqlStr = L"SELECT * FROM Medios WHERE Path LIKE\"" + Path + L"\"";
			break;
		case 9 : // Directorio principal (no val... ha de ser nomes l'arrel de MP3 o Pelis i Series)
		case 8 : // Directorio
			SqlStr = L"SELECT * FROM Medios WHERE Path LIKE \"%" + Path + L"%\" COLLATE NOCASE";
			break;
	}

	sqlite3_stmt   *SqlQuery = NULL;
	int SqlRet = sqlite3_prepare16_v2(App.BD(), SqlStr.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		const char *Error = sqlite3_errmsg(App.BD());
		return FALSE;
	}

	/*
		Nombre		Posición	   Tipo
		--------------------------------
		Id				0		INTEGER PRIMARY KEY
		Hash			1		INT UNIQUE
		Path			2		VARCHAR(260)
		Nombre			3		VARCHAR(128)
		TipoMedio		4		INT
		Extension		5		INT
		Reproducido		6		INT
		Longitud		7		INT
		Raiz			8		INT
		Nota			9		SMALLINT
		Genero		   10		INT
		Grupo		   11		INT
		Disco		   12		INT
		Pista		   13		INT
		Tiempo		   14		INT
		Subtitulos	   15		VARCHAR(260)
	*/

	sqlite3_int64	nHash;
	std::wstring	nNombre;
	UINT			nPista;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_MISUSE) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			TablaMedios_Medio *Medio = new TablaMedios_Medio(
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 0)),
				sqlite3_column_int64(SqlQuery, 1),
				reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2)),
				reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3)),
				static_cast<Tipo_Medio>(sqlite3_column_int(SqlQuery, 4)),
				static_cast<Extension_Medio>(sqlite3_column_int(SqlQuery, 5)),
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 6)),
				static_cast<ULONG>(sqlite3_column_int(SqlQuery, 7)),
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 8)),
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 9)),
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 10)),
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 11)),
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 12)),
				static_cast<UINT>(sqlite3_column_int(SqlQuery, 13)),
				static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 14)),
				reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 15))
			);

			nHash   = sqlite3_column_int64(SqlQuery, 1);
			nNombre	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
			nPista	= static_cast<UINT>(sqlite3_column_int(SqlQuery, 13));
			App.VentanaRave.Lista.AgregarMedio(Medio);

			delete Medio;

			/*TmpStr = DWL::DString_ToStr(static_cast<UINT>(sqlite3_column_int(SqlQuery, 13)), 2) + L" " + reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
			AgregarBDNodo(ArbolBD_TipoNodo_Cancion, BDNodo, TmpStr.c_str(), static_cast<size_t>(sqlite3_column_int(SqlQuery, 1)));*/
		}
	}
	sqlite3_finalize(SqlQuery);
	return TRUE;
}



ArbolBD_Nodo *ArbolBD::BuscarHash(sqlite3_int64 bHash) {
	for (size_t i = 0; i < _Nodos.size(); i++) {
		if (BDNodo(i)->Hash == bHash) return BDNodo(i);
	}
	return NULL;
}



/*
LRESULT ArbolBD::Evento_Mouse_Click(DTreeView_Nodo *cNodo, const UINT nBoton, const int cX, const int cY) {
	if (nBoton == 1) {
		const BOOL IdMenu = App.Menu_ArbolBD.Mostrar(hWnd());
		switch (IdMenu) {
			case ID_MENUBD_AGREGARANUEVALISTA:
				App.Lista.BorrarListaReproduccion();
				AgregarNodoALista(cNodo);
				App.VentanaRave.Lista_Play();
				break;
			case ID_MENUBD_AGREGARALISTA:
				AgregarNodoALista(cNodo);
				break;
			case ID_MENUBD_ACTUALIZAR:
				App.BD.ActualizarArbol();
				break;
		}
	}
	return 0;
}*/