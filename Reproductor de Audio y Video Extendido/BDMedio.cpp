#include "stdafx.h"



BDMedio::BDMedio(const BDMedio& c) :	PistaTag(c.PistaTag), PistaPath(c.PistaPath), PistaEleccion(c.PistaEleccion),
										NombreTag(c.NombreTag), NombrePath(c.NombrePath), NombreEleccion(c.NombreEleccion),
										Hash(c.Hash), Path(c.Path), TipoMedio(c.TipoMedio), Tiempo(c.Tiempo), Extension(c.Extension),
										Longitud(c.Longitud), Reproducido(c.Reproducido), Nota(c.Nota), Id(c.Id), IDDisco(c.IDDisco),
										GrupoTag(c.GrupoTag), GrupoPath(c.GrupoPath), GrupoEleccion(c.GrupoEleccion),
										DiscoTag(c.DiscoTag), DiscoPath(c.DiscoPath), DiscoEleccion(c.DiscoEleccion),
										Subtitulos(c.Subtitulos), Parseado(c.Parseado), Actualizar(c.Actualizar), Genero(c.Genero),
										Proporcion(c.Proporcion), Brillo(c.Brillo), Contraste(c.Contraste), Saturacion(c.Saturacion), PosMomento(c.PosMomento) {
	// Creo una copia del vector de momentos
	for (size_t i = 0; i < c.Momentos.size(); i++) {
		Momentos.push_back(new BDMomento(c.Momentos[i]));
	}
}

// Constructor que obtiene los datos de una fila
BDMedio::BDMedio(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades, RaveSQLite* BD) : IDDisco(0), PistaPath(0), PistaTag(0), Hash(0), TipoMedio(Tipo_Medio_INDEFINIDO), Extension(Extension_NOSOPORTADA), Tiempo(0), Longitud(0), Id(0), /*IDDisco(0),*/ Parseado(FALSE), Actualizar(FALSE), Nota(2.5f), PistaEleccion(0), Reproducido(0), GrupoEleccion(0), DiscoEleccion(0), NombreEleccion(0), Saturacion(1.0f), Brillo(1.0f), Contraste(1.0f) {
	ObtenerFila(SqlQuery, Unidades, BD);
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
	PistaTag		= c.PistaTag;			//  0
	PistaPath		= c.PistaPath;			//  1
	PistaEleccion	= c.PistaEleccion;		//  2
	NombreTag		= c.NombreTag;			//  3
	NombrePath		= c.NombrePath;			//  4
	NombreEleccion	= c.NombreEleccion;		//  5
	Hash			= c.Hash;				//  6
	Path			= c.Path;				//  7
	TipoMedio		= c.TipoMedio;			//  8
	Tiempo			= c.Tiempo;				//  9
	Extension		= c.Extension;			// 10
	Longitud		= c.Longitud;			// 11
	Reproducido		= c.Reproducido;		// 12
	Nota			= c.Nota;				// 13
	Id				= c.Id;					// 14
	IDDisco			= c.IDDisco;			// 15
	GrupoTag		= c.GrupoTag;			// 16
	GrupoPath		= c.GrupoPath;			// 17
	GrupoEleccion	= c.GrupoEleccion;		// 18
	DiscoTag		= c.DiscoTag;			// 19
	DiscoPath		= c.DiscoPath;			// 20
	DiscoEleccion	= c.DiscoEleccion;		// 21
	Subtitulos		= c.Subtitulos;			// 22
	Parseado		= c.Parseado;			// 23
	Actualizar		= c.Actualizar;			// 24
	Proporcion		= c.Proporcion;			// 25
	Brillo			= c.Brillo;				// 26
	Contraste		= c.Contraste;			// 27
	Saturacion		= c.Saturacion;			// 28

	BorrarMomentos();
	// Creo una copia del vector de momentos
	for (size_t i = 0; i < c.Momentos.size(); i++) {
		Momentos.push_back(new BDMomento(c.Momentos[i]));
	}

	return *this;
}

/*		Tipo				Nombre			Posici�n		Tipo
		------------------------------------------------------------------------
		UINT				Id				     0			INTEGER PRIMARY KEY
		sqlite_int64		Hash			     1			INT UNIQUE
		std::wstring		Path			     2			VARCHAR(260)
		std::wstring		NombrePath		     3			VARCHAR(128)
		Tipo_Medio			TipoMedio		     4			INT
		Extension_Medio		Extension		     5			INT
		UINT				Reproducido		     6			INT
		ULONG				Longitud		     7			INT
		DWORD				IDDisco			     8			INT						(YA NO SE USA)  (Primera ID de disco encontrada, si hay varios discos con el medio en la misma ubicaci�n, se elegir� la letra de la Unidad con la primera ID de disco encontrada)
		float				Nota			     9			DOUBLE
		std::wstring		Genero		        10			VARCHAR(128)
		std::wstring		GrupoPath	        11			VARCHAR(128)
		std::wstring		DiscoPath	        12			VARCHAR(128)
		UINT				PistaPath	        13			INT
		INT64				Tiempo		        14			INT
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
*/
void BDMedio::ObtenerFila(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades, RaveSQLite *BD) {
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
	Tiempo			= static_cast<INT64>(sqlite3_column_int(SqlQuery, 14));
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




	//			m'haig de refiar de l'ultim path guardat amb lletra inclosa
	//				en cas de no existir l'arxiu, puc buscar en les unitats disponibles aviam si existeix l'arxiu
	//					si no trobo l'arxiu en cap unitat disponible, deixo el path original, per que mostri l'error de l'arxiu.
	//				s'ha de preveure que el path pot ser local, de red o de internet.
	//					local    : es pot detectar si existeix l'arxiu amb el api del sistema d'arxius i es pot obtenir el md5 de l'arxiu
	//									si no existeix, el puc buscar a la resta d''unitats locals disponibles
	//					red		 : es pot detectar si existeix l'arxiu amb el api del sistema d'arxius de windows, per� no es recomenable obtenir el md5 per no saturar la red.
	//					internet : no es pot detectar amb el api del sistema d'arxius, i no es pot obtenir el md5.

	// El medio no existe en el ultimo path conocido y es un medio local
	if (GetFileAttributes(Path.c_str()) == INVALID_FILE_ATTRIBUTES && Ubicacion() == Ubicacion_Medio_Local) {		
		// Miro si el medio est� en otra unidad disponible con la misma ruta
		DUnidadDisco *Unidad = nullptr;
		std::wstring  Q;
		// Recorro todas las unidades disponibles
		for (size_t i = 0; i < Unidades.TotalUnidades(); i++) {
			Unidad = Unidades.Unidad(i);
			// La unidad no existe (NO DEBERIA OCURRIR...)
			if (Unidad != nullptr) {
				Path[0] = Unidad->Letra();
				// El archivo existe en otra unidad con el mismo path (solo cambia la letra de unidad)
				if (GetFileAttributes(Path.c_str()) != INVALID_FILE_ATTRIBUTES) {
					// Actualizo el path en la BD
					Q = L"UPDATE Medios SET Path =\"" + Path + L"\" WHERE Id=" + std::to_wstring(Id);
					Consulta(Q.c_str(), BD->_BD, BD->_UltimoErrorSQL);
					// Salgo del bucle
					break;
				}
			}
		}
	}

/*	Path[0] = L'?';

	// Si el medio pertenece a una unidad local
	// Hay que pensar que el medio puede est�r en varias unidades a la vez.
	//  - Para ello en el primer escaneo se guarda la ID del disco donde se ha encontrado.
	//  - Si se encuentran mas de una unidad con el mismo medio se elegir� siempre el medio con la ID de disco del primer escaneo
	//  - Si no se encuentra la ID de disco original, se elegir� el primer disco por orden alfabetico que contenga el medio, y se guardar� la ID del disco de este nuevo disco.
	BOOL ReasignarIDDisco = TRUE;
	if (Ubicacion() == Ubicacion_Medio_Local) {
		// Busco la letra de unidad del medio
		for (size_t i = 0; i < Unidades.TotalUnidades(); i++) {
			// Obtengo la primera letra de unidad que contiene el medio
			if (Path[0] == L'?') {
				Path[0] = Unidades.Unidad(i)->Letra();
			}
			// Si la letra de unidad coincide, y la unidad es la unidad de disco del primer escaneo
			if (Path[0] == Unidades.Unidad(i)->Letra() && Unidades.Unidad(i)->Numero_Serie() == IDDisco) {
				ReasignarIDDisco = FALSE;
				break;
			}
		}
	}

	// Hay un disco con el medio, y no se ha encontrado un disco con la IDDisco del primer escaneo, se asignara la IDDisco actual 
	if (Path[0] == L'?' && ReasignarIDDisco == TRUE) {
		DWL::DUnidadDisco *Unidad = Unidades.Buscar_Letra(Path[0]);
		// Si la unidad existe
		if (Unidad != nullptr) {
			// Creo la consulta
			std::wstring Q = L"UPDATE Medios SET IDDisco=" + std::to_wstring(Unidad->Numero_Serie()) + L" WHERE Id=" + std::to_wstring(Id);
			// Actualizo el la ID del disco del medio
			Consulta(Q.c_str(), BD->_BD, BD->_UltimoErrorSQL);
		}
	}*/

	ObtenerMomentos(Id);
}




void BDMedio::ObtenerMomentos(const UINT nId) {
	BorrarMomentos();

	std::wstring Q = L"SELECT * FROM Momentos WHERE IdMedio=" + std::to_wstring(nId);

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
			TmpMomento->TiempoInicio = static_cast<INT64>(sqlite3_column_int(SqlQuery, 2));
			TmpMomento->TiempoFinal  = static_cast<INT64>(sqlite3_column_int(SqlQuery, 3));
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

// Pasa la pista num�rica a string y la devuelve en el par�metro nPistaStr
void BDMedio::PistaStr(std::wstring &nPistaStr) {
	if (Pista() < 10)	{ nPistaStr = L"0" + std::to_wstring(Pista());	}
	else				{ nPistaStr = std::to_wstring(Pista());			}
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


const Ubicacion_Medio BDMedio::Ubicacion(std::wstring& nPath) {
	// Si tiene mas de 4 car�cteres se puede identificar
	if (nPath.size() > 4) {
		// Medio local que empieza por "?:\"
		if (nPath[1] == L':' && nPath[2] == L'\\')
			return Ubicacion_Medio_Local;
		// Path que empieza por "\\" que viene de un directorio de la red local (\\<computer name>\<shared directory>\)
		if (nPath[0] == L'\\' && nPath[1] == L'\\')
			return Ubicacion_Medio_Red;
		// URL que empieza por "Http"
		if ((nPath[0] == L'h' || nPath[0] == L'H') && (nPath[1] == L't' || nPath[1] == L'T') && (nPath[2] == L't' || nPath[2] == L'T') && (nPath[3] == L'p' || nPath[3] == L'P'))
			return Ubicacion_Medio_Internet;
		// URL que empieza por "Ftp"
		if ((nPath[0] == L'f' || nPath[0] == L'F') && (nPath[1] == L't' || nPath[1] == L'T') && (nPath[2] == L'p' || nPath[2] == L'P'))
			return Ubicacion_Medio_Internet;
	}

	// No se puede determinar la ubicaci�n...
	return Ubicacion_Medio_INDEFINIDO;
}

// Funci�n que devuelve la ubicaci�n del medio analizando el path
const Ubicacion_Medio BDMedio::Ubicacion(void) {
	return Ubicacion(Path);
}

// Funci�n que devuelve true si el medio se tiene que reproducir con FMOD
const BOOL BDMedio::EsFMOD(void) {
#ifdef RAVE_UTILIZAR_FMOD
	switch (Extension) {
		case Extension_ASF :
		case Extension_IT  :
		case Extension_MOD :
		case Extension_MP2 :
		case Extension_MP3 :
		case Extension_MPC :
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

// Funci�n que recarga los datos desde la BD para este medio (NO USAR EN THREADS)
void BDMedio::Recargar(void) {
	App.BD.ObtenerMedio(Hash, *this, App.Unidades);
}



// Consulta SQL, que requiere : 
//	TxtConsulta    [IN]  : Puntero constante al texto de la consulta.
//  BD             [IN]  : Puntero a la base de datos de donde  queremos hacer la consulta.
//  UltimoErrorSQL [OUT] : std::wstring con el texto del ultimo error generado por la consulta (si no hay error UltimoErrorSQL.size() ser� 0)
const int BDMedio::Consulta(const wchar_t* TxtConsulta, sqlite3 *BD, std::wstring &UltimoErrorSQL) {
	if (BD == NULL) return FALSE;

	sqlite3_stmt* SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(BD, TxtConsulta, -1, &SqlQuery, NULL);
	if (SqlRet) {
		UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(BD));
		Debug_Escribir_Varg(L"BDMedio::Consulta %s\n", UltimoErrorSQL.c_str());
		return SqlRet;
	}

	/*	SQLITE_FULL		: database or disk full
		SQLITE_IOERR	: disk I / O error
		SQLITE_BUSY		: database in use by another process
		SQLITE_NOMEM	:*/
	int VecesBusy = 0;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}
	sqlite3_finalize(SqlQuery);
	// Si hay un error lo apunto
	if (SqlRet == SQLITE_ERROR || SqlRet == SQLITE_CONSTRAINT) {
		UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(BD));
		//		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
	}

	return SqlRet;
}
