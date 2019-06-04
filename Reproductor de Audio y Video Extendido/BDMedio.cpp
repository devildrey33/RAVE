#include "stdafx.h"



BDMedio::BDMedio(const BDMedio& c) :	PistaTag(c.PistaTag), PistaPath(c.PistaPath), PistaEleccion(c.PistaEleccion),
										NombreTag(c.NombreTag), NombrePath(c.NombrePath), NombreEleccion(c.NombreEleccion),
										Hash(c.Hash), Path(c.Path), TipoMedio(c.TipoMedio), Tiempo(c.Tiempo), Extension(c.Extension),
										Longitud(c.Longitud), Reproducido(c.Reproducido), Nota(c.Nota), Id(c.Id), IDDisco(c.IDDisco),
										GrupoTag(c.GrupoTag), GrupoPath(c.GrupoPath), GrupoEleccion(c.GrupoEleccion),
										DiscoTag(c.DiscoTag), DiscoPath(c.DiscoPath), DiscoEleccion(c.DiscoEleccion),
										Subtitulos(c.Subtitulos), Parseado(c.Parseado), Actualizar(c.Actualizar),
										Proporcion(c.Proporcion), Brillo(c.Brillo), Contraste(c.Contraste), Saturacion(c.Saturacion) {
	// Creo una copia del vector de momentos
	for (size_t i = 0; i < c.Momentos.size(); i++) {
		Momentos.push_back(new BDMomento(c.Momentos[i]));
	}
}

// Constructor que obtiene los datos de una fila
BDMedio::BDMedio(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades) : PistaPath(0), PistaTag(0), Hash(0), TipoMedio(Tipo_Medio_INDEFINIDO), Extension(Extension_NOSOPORTADA), Tiempo(0), Longitud(0), Id(0), IDDisco(0), Parseado(FALSE), Actualizar(FALSE), Nota(2.5f), PistaEleccion(0), Reproducido(0), GrupoEleccion(0), DiscoEleccion(0), NombreEleccion(0), Saturacion(1.0f), Brillo(1.0f), Contraste(1.0f) {
	ObtenerFila(SqlQuery, Unidades);
}

BDMedio::~BDMedio(void) { 
	for (size_t i = 0; i < Momentos.size(); i++) {
		delete Momentos[i];
	}
};

void BDMedio::BorrarMomentos(void) {
	for (size_t i = 0; i < Momentos.size(); i++) {
		delete Momentos[i];
	}
	Momentos.resize(0);
}

BDMedio& BDMedio::operator = (const BDMedio& c) {
	PistaTag		= c.PistaTag;
	PistaPath		= c.PistaPath;
	PistaEleccion	= c.PistaEleccion;
	NombreTag		= c.NombreTag;
	NombrePath		= c.NombrePath;
	NombreEleccion	= c.NombreEleccion;
	Hash			= c.Hash;
	Path			= c.Path;
	TipoMedio		= c.TipoMedio;
	Tiempo			= c.Tiempo;
	Extension		= c.Extension;
	Longitud		= c.Longitud;
	Reproducido		= c.Reproducido;
	Nota			= c.Nota;
	Id				= c.Id;
	IDDisco			= c.IDDisco;
	GrupoTag		= c.GrupoTag;
	GrupoPath		= c.GrupoPath;
	GrupoEleccion	= c.GrupoEleccion;
	DiscoTag		= c.DiscoTag;
	DiscoPath		= c.DiscoPath;
	DiscoEleccion	= c.DiscoEleccion;
	Subtitulos		= c.Subtitulos;
	Parseado		= c.Parseado;
	Actualizar		= c.Actualizar;
	Proporcion		= c.Proporcion;
	Brillo			= c.Brillo;
	Contraste		= c.Contraste;
	Saturacion		= c.Saturacion;

	BorrarMomentos();
	// Creo una copia del vector de momentos
	for (size_t i = 0; i < c.Momentos.size(); i++) {
		Momentos.push_back(new BDMomento(c.Momentos[i]));
	}

	return *this;

}

/*	Tipo				Nombre			Posici�n		Tipo
		---------------------------------------------------------------------- -
		UINT				Id				     0			INTEGER PRIMARY KEY
		sqlite_int64		Hash			     1			INT UNIQUE
		std::wstring		Path			     2			VARCHAR(260)
		std::wstring		NombrePath		     3			VARCHAR(128)
		Tipo_Medio			TipoMedio		     4			INT
		Extension_Medio		Extension		     5			INT
		UINT				Reproducido		     6			INT
		ULONG				Longitud		     7			INT
		DWORD				IDDisco			     8			INT
		float				Nota			     9			DOUBLE
		std::wstring		Genero		        10			VARCHAR(128)
		std::wstring		GrupoPath	        11			VARCHAR(128)
		std::wstring		DiscoPath	        12			VARCHAR(128)
		UINT				PistaPath	        13			INT
		libvlc_time_t		Tiempo		        14			INT
		std::wstring		Subtitulos	        15			VARCHAR(260)
		BOOL				Parseado            16			TINYINT(1)
		std::wstring		NombreTag		    17			VARCHAR(128)
		std::wstring		GrupoTag		    18			VARCHAR(128)
		std::wstring		DiscoTag		    19			VARCHAR(128)
		UINT				PistaTag	        20			INT
		BOOL				NombreEleccion      21			TINYINT(1)
		BOOL				GrupoEleccion       22			TINYINT(1)
		BOOL				DiscoEleccion       23			TINYINT(1)
		BOOL				PistaEleccion       24			TINYINT(1)
		std::wstring		Proporcion 		    25           VARCHAR(16)
		float               Brillo 			    26			DOUBLE
		float               Contraste			27			DOUBLE
		float               Saturacion 			28			DOUBLE
		LONG                IDMomentos			29			INT
*/
void BDMedio::ObtenerFila(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades) {
	Id				= static_cast<UINT>(sqlite3_column_int(SqlQuery, 0));
	Hash			= sqlite3_column_int64(SqlQuery, 1);
	Path			= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
	const wchar_t *nNombrePath	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
	if (nNombrePath != NULL) NombrePath = nNombrePath;
	TipoMedio		= static_cast<Tipo_Medio>(sqlite3_column_int(SqlQuery, 4));
	Extension		= static_cast<Extension_Medio>(sqlite3_column_int(SqlQuery, 5));
	Reproducido		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 6));
	Longitud		= static_cast<DWORD>(sqlite3_column_int(SqlQuery, 7));
	IDDisco			= static_cast<DWORD>(sqlite3_column_int(SqlQuery, 8));
	Nota			= static_cast<float>(sqlite3_column_double(SqlQuery, 9));
	const wchar_t *nGenero		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 10));
	const wchar_t *nGrupoPath	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 11));
	const wchar_t *nDiscoPath	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 12));
	if (nGenero != NULL)		Genero = nGenero;
	if (nGrupoPath != NULL)		GrupoPath = nGrupoPath;
	if (nDiscoPath != NULL)		DiscoPath = nDiscoPath;
	PistaPath		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 13));
	Tiempo			= static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 14));
	Subtitulos		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 15));	
	Parseado		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 16));
	const wchar_t *nNombreTag	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 17));
	const wchar_t *nGrupoTag	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 18));
	const wchar_t *nDiscoTag	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 19));
	if (nNombreTag != NULL)		NombreTag = nNombreTag;
	if (nGrupoTag != NULL)		GrupoTag = nGrupoTag;
	if (nDiscoTag != NULL)		DiscoTag = nDiscoTag;
	PistaTag		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 20));
	NombreEleccion	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 21));
	GrupoEleccion	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 22));
	DiscoEleccion	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 23));
	PistaEleccion	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 24));
	const wchar_t* nProporcion = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(SqlQuery, 25));
	if (nProporcion != NULL) Proporcion = nProporcion;
	Brillo			= static_cast<float>(sqlite3_column_double(SqlQuery, 26));
	Contraste		= static_cast<float>(sqlite3_column_double(SqlQuery, 27));
	Saturacion		= static_cast<float>(sqlite3_column_double(SqlQuery, 28));
//	IDMomentos		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 29));

	DWL::DUnidadDisco *Unidad = Unidades.Buscar_Numero_Serie(IDDisco);
	if (Unidad != NULL) Path[0] = Unidad->Letra();

	ObtenerMomentos(Id);
}


void BDMedio::ObtenerMomentos(const UINT nId) {
	BorrarMomentos();

	std::wstring Q = L"SELECT * FROM Momentos" + std::to_wstring(nId);

	BDMomento    *TmpMomento = NULL;
	int			  SqlRet 	 = 0;
	sqlite3_stmt* SqlQuery   = NULL;

	SqlRet = sqlite3_prepare16_v2(App.BD._BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		App.BD._UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(App.BD._BD));
		return;
	}

	int VecesBusy = 0;
	const wchar_t* TmpNombre = NULL;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			TmpMomento = new BDMomento();
			TmpMomento->Id			 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 0));
			TmpMomento->IdPadre		 = nId;			
			TmpNombre				 = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(SqlQuery, 1));
			if (TmpNombre != NULL) TmpMomento->Nombre = TmpNombre;
			TmpMomento->TiempoInicio = static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 2));
			TmpMomento->TiempoFinal  = static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 3));
			TmpMomento->Excluir      = static_cast<BOOL>(sqlite3_column_int(SqlQuery, 4));
			Momentos.push_back(TmpMomento);
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		App.BD._UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(App.BD._BD));
		return;
	}
}

void BDMedio::PistaStr(std::wstring &nPistaStr) {
	if (Pista() < 10) { nPistaStr = L"0" + std::to_wstring(Pista()); }
	else { nPistaStr = std::to_wstring(Pista()); }
}

UINT BDMedio::Pista(void) { 
	if (PistaEleccion == TRUE) {
		if (PistaPath == 0) return PistaTag;
		else                return PistaPath;
	}
	else {
		if (PistaTag == 0)  return PistaPath;
		else                return PistaTag;
	}
}

std::wstring &BDMedio::Nombre(void) {
	if (NombreEleccion == TRUE) {
		if (NombrePath.size() == 0) return NombreTag;
		else                 		return NombrePath;
	}
	else {
		if (NombreTag.size() == 0)	return NombrePath;
		else                 		return NombreTag;
	}
};

std::wstring &BDMedio::Grupo(void) {
	if (GrupoEleccion == TRUE) {
		if (GrupoPath.size() == 0)  return GrupoTag;
		else                 		return GrupoPath;
	}
	else {
		if (GrupoTag.size() == 0)	return GrupoPath;
		else                 		return GrupoTag;
	}
};

std::wstring &BDMedio::Disco(void) {
	if (DiscoEleccion == TRUE) {
		if (DiscoPath.size() == 0)  return DiscoTag;
		else                 		return DiscoPath;
	}
	else {
		if (DiscoTag.size() == 0)	return DiscoPath;
		else                 		return DiscoTag;
	}
};

// Funci�n que devuelve true si el medio se tiene que reproducir con FMOD
const BOOL BDMedio::EsFMOD(void) {
#ifdef RAVE_UTILIZAR_FMOD
	switch (Extension) {
		case Extension_ASF :
		case Extension_IT  :
		case Extension_MOD :
		case Extension_MP2 :
		case Extension_MP3 :
		case Extension_OGG :
		case Extension_S3M :
		case Extension_WAV :
		case Extension_WMA :
		case Extension_XM  :
			return TRUE;
		default :
			return FALSE;
	}
#endif
	return FALSE;
}