#include "stdafx.h"
#include "RaveBD.h"
#include "DDirectoriosWindows.h"
#include "DStringUtils.h"

RaveBD::RaveBD(void) : _BD(NULL)  {
}


RaveBD::~RaveBD(void) {
}

// Crea / carga la base de datos
const BOOL RaveBD::Iniciar(void) {
	int Ret = 0;
	std::wstring PathBD;
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(PathBD);
	PathBD += L"\\Rave\\";
	// Creo el directorio "\AppData\Rave" si no existe
	if (GetFileAttributes(PathBD.c_str()) == INVALID_FILE_ATTRIBUTES)
		CreateDirectory(PathBD.c_str(), NULL);

	// Las bases de datos para 32 y 64 bits dan hash distintos y pueden duplicar medios, por ello cada configuración tiene su bd
#ifdef _WIN64
	PathBD += L"Rave_x64.BD";
#else
	PathBD += L"Rave_x86.BD";
#endif
	// Creo / Abro la BD
	Ret = sqlite3_open16(PathBD.c_str(), &_BD);
	if (Ret) {
		sqlite3_close(_BD);
		_BD = NULL;
		return FALSE;
	}
	// Creo las tablas para la base de datos (si es necesario)
	_CrearTablas();

	// Obtengo las raices de la base de datos
	ObtenerRaices();
	// Obtengo las opciones de la base de datos
	ObtenerOpciones();
	// Obtengo las etiquetas
	ObtenerEtiquetas();

	return TRUE;
}

// Cierra la Base de datos
void RaveBD::Terminar(void) {
	sqlite3_close_v2(_BD);
	_BD = NULL;
}

/*
const int RaveBD::ConsultaVarg(const wchar_t *TxtConsulta, ...) {
	static wchar_t  Texto[4096];
	va_list			Marker;
	va_start(Marker, TxtConsulta);
	vswprintf_s(Texto, 4096, TxtConsulta, Marker);
	va_end(Marker);
	return Consulta(Texto);
}*/

// Función para realizar consultas simples 
const int RaveBD::Consulta(const wchar_t *TxtConsulta) {
	if (_BD == NULL) return FALSE;

	sqlite3_stmt *SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
//		Debug_Escribir_Varg(L"RaveBD::Consulta %s\n", _UltimoErrorSQL.c_str());
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
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
//		Debug_Escribir_Varg(L"RaveBD::Consulta %s\n", _UltimoErrorSQL.c_str());
	}

	return SqlRet;
}

const BOOL RaveBD::ObtenerMedio(const sqlite3_int64 mHash, BDMedio &OUT_Medio) {
	std::wstring    SqlStr = L"SELECT * FROM Medios WHERE Hash =" + std::to_wstring(mHash);
	return _ConsultaObtenerMedio(SqlStr, OUT_Medio);
}

const BOOL RaveBD::ObtenerMedio(std::wstring &mPath, BDMedio &OUT_Medio) {
	std::wstring TmpPath = mPath;
	TmpPath[0] = L'?'; // Los paths de la BD emnpiezan por ?
	std::wstring SqlStr = L"SELECT * FROM Medios WHERE Path =\"" + TmpPath + L"\" COLLATE NOCASE"; // COLLATE NOCASE = Comparar strings case insensitive
	return _ConsultaObtenerMedio(SqlStr, OUT_Medio);
}

// Función que busca una raíz por el path
// Se ha adaptado para que encuentre raices parciales, es decir :
// - Si tenemos en la BD la raíz "c:\mp3\grupo" y buscamos "c:\mp3" devolvera la raíz "c:\mp3\grupo"
// - Si tenemos en la BD la raíz "c:\mp3" y buscamos "c:\mp3\grupo" devolverá la raíz "c:\mp3"
BDRaiz *RaveBD::BuscarRaiz(std::wstring &Path) {
	int Comp = 0;
	for (size_t i = 0; i < _Raices.size(); i++) {
		if (_CompararRaices(_Raices[i]->Path, Path) == TRUE) {
			return _Raices[i];
		}
/*		// El Path a buscar es de mayor tamaño
		if (_Raices[i]->Path.size() < Path.size())		Comp = _wcsicmp(_Raices[i]->Path.c_str(), Path.substr(0, _Raices[i]->Path.size()).c_str());
		// El Path a buscar es de menor tamaño
		else if (_Raices[i]->Path.size() > Path.size())	Comp = _wcsicmp(_Raices[i]->Path.substr(0, Path.size()).c_str(), Path.c_str());
		// Tienen el mismo tamaño
		else											Comp = _wcsicmp(_Raices[i]->Path.c_str(), Path.c_str());
		// Si son iguales retorno los datos
		if (Comp == 0) return _Raices[i];*/
	}
	return NULL;
}


// Función que compara 2 paths pertenecientes a posibles raices
// Se ha adaptado para que encuentre raices parciales, es decir :
// - Si tenemos en la BD la raíz "c:\mp3\grupo" y buscamos "c:\mp3" devolvera la raíz "c:\mp3\grupo"
// - Si tenemos en la BD la raíz "c:\mp3" y buscamos "c:\mp3\grupo" devolverá la raíz "c:\mp3"
const BOOL RaveBD::_CompararRaices(std::wstring &Path1, std::wstring &Path2) {
	int Comp = 0;
	// El Path a buscar es de mayor tamaño
	if (Path1.size() < Path2.size())		Comp = _wcsicmp(Path1.c_str(), Path2.substr(0, Path1.size()).c_str());
	// El Path a buscar es de menor tamaño
	else if (Path1.size() > Path2.size())	Comp = _wcsicmp(Path1.substr(0, Path2.size()).c_str(), Path2.c_str());
	// Tienen el mismo tamaño
	else									Comp = _wcsicmp(Path1.c_str(), Path2.c_str());
	// Devuelvo TRUE si son similares o iguales
	return (Comp == 0) ? TRUE : FALSE;
}

// Función que busca una raíz por su ID
BDRaiz *RaveBD::BuscarRaiz(const unsigned long bID) {
	for (size_t i = 0; i < _Raices.size(); i++) {
		if (_Raices[i]->Id == bID) return _Raices[i];
	}
	return NULL;
}


const BOOL RaveBD::ObtenerMediosPorParsear(std::vector<std::wstring> &Paths) {
	std::wstring TxtConsulta = L"SELECT * FROM Medios WHERE Parseado = 0";
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	BDMedio         Medio;
	SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			Medio.ObtenerFila(SqlQuery, Unidades);
			Paths.push_back(Medio.Path);
		}

		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);
}


// Devuelve FALSE si no se encuentra el medio o hay algun error
const BOOL RaveBD::_ConsultaObtenerMedio(std::wstring &TxtConsulta, BDMedio &OUT_Medio) {
	int				SqlRet		= 0;
	sqlite3_stmt   *SqlQuery	= NULL;
	BOOL            Ret			= FALSE;
	SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		Debug_Escribir_Varg(L"RaveBD::_ConsultaObtenerMedio %s\n", _UltimoErrorSQL.c_str());
		return FALSE;
	}

	std::wstring TmpPath;
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			OUT_Medio.ObtenerFila(SqlQuery, Unidades);
			Ret = TRUE;
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		Debug_Escribir_Varg(L"RaveBD::_ConsultaObtenerMedio %s\n", _UltimoErrorSQL.c_str());
		return FALSE;
	}

	return Ret;
}

// Elimina las raices de la memória
void RaveBD::_BorrarRaices(void) {
	for (size_t i = 0; i < _Raices.size(); i++) delete _Raices[i];
	_Raices.resize(0);
}

// Función que obtiene todas las raices de la base de datos, y las carga en memória
const BOOL RaveBD::ObtenerRaices(void) {
	_BorrarRaices();
	int				    SqlRet = 0;
	const wchar_t      *SqlStr = L"SELECT * FROM Raiz";
	DWL::DUnidadesDisco nUnidades;
	DWL::DUnidadDisco  *Unidad   = NULL;
	sqlite3_stmt       *SqlQuery = NULL;
	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDRaiz *TmpRaiz		= new BDRaiz;
			TmpRaiz->Id			= static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 0));
			TmpRaiz->Path		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			TmpRaiz->ID_Disco	= static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 2));			
			// Asigno la letra de unidad a la raíz ya que las unidades extraibles pueden cambiar de letra pero siempre tienen el mismo número de serie.
			Unidad = nUnidades.Buscar_Numero_Serie(TmpRaiz->ID_Disco);
			if (Unidad != NULL) {
				TmpRaiz->Path[0] = Unidad->Letra();
				TmpRaiz->Letra = Unidad->Letra();
			}
			_Raices.push_back(TmpRaiz);
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);
}


const BOOL RaveBD::ObtenerEtiquetas(void) {
	_Etiquetas.resize(0);

	const wchar_t      *SqlStr = L"SELECT * FROM Etiquetas ORDER BY Texto";
	int				    SqlRet = 0;
	sqlite3_stmt       *SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {

/*			std::wstring CrearTablaEtiquetas = L"CREATE TABLE Etiquetas ("
				L"Id" 				L" INTEGER PRIMARY KEY,"
				L"Texto"			L" VARCHAR(260),"
				L"Tipo"				L" INTEGER,"
				L"Medios"			L" INTEGER,"
				L"Nota"				L" DOUBLE,"
				L"Tiempo"			L" BIGINT"
			")";*/
			std::wstring	Texto	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			UINT			Tipo	= static_cast<UINT>(sqlite3_column_int(SqlQuery, 2));
			UINT			Medios	= static_cast<UINT>(sqlite3_column_int(SqlQuery, 3));
			float			Nota	= static_cast<float>(sqlite3_column_double(SqlQuery, 4));
			libvlc_time_t	Tiempo  = static_cast<libvlc_time_t>(sqlite3_column_int64(SqlQuery, 5));
			_Etiquetas.push_back(EtiquetaBD(Texto, Tipo, Nota, Tiempo, Medios));
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);

}


// Actualiza los valores del medio en la base de datos
/*	Tipo				Nombre			Posición		Tipo
		---------------------------------------------------------------------- -
		UINT				Id				    0			INTEGER PRIMARY KEY
		sqlite_int64		Hash			    1			INT UNIQUE
		std::wstring		Path			    2			VARCHAR(260)
		std::wstring		NombrePath		    3			VARCHAR(128)
		Tipo_Medio			TipoMedio		    4			INT
		Extension_Medio		Extension		    5			INT
		UINT				Reproducido		    6			INT
		ULONG				Longitud		    7			INT
		DWORD				IDDisco			    8			INT
		float				Nota			    9			DOUBLE
		std::wstring		Genero		       10			VARCHAR(128)
		std::wstring		GrupoPath	       11			VARCHAR(128)
		std::wstring		DiscoPath	       12			VARCHAR(128)
		UINT				PistaPath	       13			INT
		libvlc_time_t		Tiempo		       14			INT
		std::wstring		Subtitulos	       15			VARCHAR(260)
		BOOL				Parseado           16			TINYINT(1)
		std::wstring		NombreTag		   17			VARCHAR(128)
		std::wstring		GrupoTag		   18			VARCHAR(128)
		std::wstring		DiscoTag		   19			VARCHAR(128)
		UINT				PistaTag	       20			INT
		BOOL				NombreEleccion     21			TINYINT(1)
		BOOL				GrupoEleccion      22			TINYINT(1)
		BOOL				DiscoEleccion      23			TINYINT(1)
		BOOL				PistaEleccion      24			TINYINT(1)				*/
const BOOL RaveBD::ActualizarMedio(BDMedio *nMedio) {
	std::wstring Q =	L"UPDATE Medios SET "
							L"NombrePath=\""	+ nMedio->NombrePath						+ L"\","
							L"Reproducido="		+ std::to_wstring(nMedio->Reproducido)		+ L","
							L"Nota="			+ DWL::Strings::ToStrF(nMedio->Nota, 1)		+ L","
							L"Genero=\""		+ nMedio->Genero							+ L"\","
							L"GrupoPath=\""		+ nMedio->GrupoPath							+ L"\","
							L"DiscoPath=\""		+ nMedio->DiscoPath							+ L"\","
							L"PistaPath="		+ std::to_wstring(nMedio->PistaPath)		+ L","
							L"Tiempo="			+ std::to_wstring(nMedio->Tiempo)			+ L","
							L"Subtitulos=\""	+ nMedio->Subtitulos						+ L"\","
							L"Parseado="		+ std::to_wstring(nMedio->Parseado)			+ L","
							L"NombreTag=\""		+ nMedio->NombreTag							+ L"\","
							L"GrupoTag=\""		+ nMedio->GrupoTag							+ L"\","
							L"DiscoTag=\""		+ nMedio->DiscoTag							+ L"\","
							L"PistaTag="		+ std::to_wstring(nMedio->PistaTag)			+ L","
							L"NombreEleccion="	+ std::to_wstring(nMedio->NombreEleccion)	+ L","
							L"GrupoEleccion="	+ std::to_wstring(nMedio->GrupoEleccion)	+ L","
							L"DiscoEleccion="	+ std::to_wstring(nMedio->DiscoEleccion)	+ L","
							L"PistaEleccion="	+ std::to_wstring(nMedio->PistaEleccion)	+ L" "
						L"WHERE Id=" + std::to_wstring(nMedio->Id);

/*	int SqlRet = ConsultaVarg(L"UPDATE Medios SET "
												L"NombrePath=\"%s\","		L"Reproducido=%d,"			L"Nota=%f,"					L"Genero=\"%s\","				L"GrupoPath=\"%s\","
												L"DiscoPath=\"%s\","		L"PistaPath=%d,"			L"Tiempo=%d,"				L"Subtitulos=\"%s\","			L"Parseado=%d,"
												L"NombreTag=\"%s\","		L"GrupoTag=\"%s\","			L"DiscoTag=\"%s\","			L"PistaTag=%d,"
												L"NombreEleccion=%d,"		L"GrupoEleccion=%d,"		L"DiscoEleccion=%d,"		L"PistaEleccion=%d"

							   L" WHERE Id=%d", nMedio->NombrePath.c_str(), nMedio->Reproducido,		nMedio->Nota,				nMedio->Genero.c_str(),			nMedio->GrupoPath.c_str(), 
												nMedio->DiscoPath.c_str(),	nMedio->PistaPath,			nMedio->Tiempo,				nMedio->Subtitulos.c_str(),		nMedio->Parseado, 
												nMedio->NombreTag.c_str(),	nMedio->GrupoTag.c_str(),	nMedio->DiscoTag.c_str(),	nMedio->PistaTag,
												nMedio->NombreEleccion,		nMedio->GrupoEleccion,		nMedio->DiscoEleccion,		nMedio->PistaEleccion,			
								nMedio->Id);*/
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

// Función que actualiza SOLO los miembros : Genero, GrupoPath, GrupoTag, DiscoPath, DiscoTag
const BOOL RaveBD::ActualizarMedioAnalisis(BDMedio *nMedio) {
	std::wstring Q =	L"UPDATE Medios SET " 
							L"Genero=\""	+ nMedio->Genero	+ L"\","	
							L"GrupoPath=\"" + nMedio->GrupoPath	+ L"\","	
							L"GrupoTag=\""  + nMedio->GrupoTag  + L"\","		
							L"DiscoPath=\""	+ nMedio->DiscoPath + L"\","
							L"DiscoTag=\""	+ nMedio->DiscoTag	+ L"\" "		
						"WHERE Id=" + std::to_wstring(nMedio->Id);
/*	int SqlRet = ConsultaVarg(L"UPDATE Medios SET "
										L"Genero=\"%s\","		L"GrupoPath=\"%s\","		L"GrupoTag=\"%s\","			L"DiscoTag=\"%s\","			L"DiscoPath=\"%s\""
								" WHERE Id=%d", 
										nMedio->Genero.c_str(), nMedio->GrupoPath.c_str(),	nMedio->GrupoTag.c_str(),	nMedio->DiscoTag.c_str(),	nMedio->DiscoPath.c_str(),
								nMedio->Id);*/
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveBD::GenerarListaAleatoria(std::vector<BDMedio> &OUT_Medios, const TipoListaAleatoria nTipo) {
	TipoListaAleatoria Tipo = nTipo;
	if (Tipo == TLA_LoQueSea) {
		Tipo = static_cast<TipoListaAleatoria>(App.Rand(static_cast<int>(TLA_LoQueSea - 1), static_cast<int>(TLA_Genero)));
	}

	std::vector<EtiquetaBD *> Etiquetas;
	std::wstring Q;
	std::wstring ToolTip;
	size_t Rand;
	switch (Tipo) {
		case TLA_Genero:
			for (size_t i = 0; i < _Etiquetas.size(); i++) {
				if (_Etiquetas[i].EsGenero() == TRUE) Etiquetas.push_back(&_Etiquetas[i]);
			}
			Rand = App.Rand<size_t>(Etiquetas.size());
			Q = L"SELECT * FROM Medios WHERE Genero=\"" + Etiquetas[Rand]->Texto + L"\"";
			ToolTip = L"Lista aleatória por Genero :\"" + Etiquetas[Rand]->Texto + L"\" generada.";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Genero (%s)\n", Etiquetas[Rand]->Texto.c_str());
			break;
		case TLA_Grupo:
			for (size_t i = 0; i < _Etiquetas.size(); i++) {
				if (_Etiquetas[i].EsGrupoPath() == TRUE || _Etiquetas[i].EsGrupoTag() == TRUE) Etiquetas.push_back(&_Etiquetas[i]);
			}
			Rand = App.Rand<size_t>(Etiquetas.size());
			Q = L"SELECT * FROM Medios WHERE (GrupoPath=\"" + Etiquetas[Rand]->Texto + L"\") OR (GrupoTag=\"" + Etiquetas[Rand]->Texto + L"\")";
			ToolTip = L"Lista aleatória por Grupo :\"" + Etiquetas[Rand]->Texto + L"\" generada.";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Grupo (%s)\n", Etiquetas[Rand]->Texto.c_str());
			break;
		case TLA_Disco:
			for (size_t i = 0; i < _Etiquetas.size(); i++) {
				if (_Etiquetas[i].EsDiscoPath() == TRUE || _Etiquetas[i].EsDiscoTag() == TRUE) Etiquetas.push_back(&_Etiquetas[i]);
			}
			Rand = App.Rand<size_t>(Etiquetas.size());
			Q = L"SELECT * FROM Medios WHERE (DiscoPath=\"" + Etiquetas[Rand]->Texto + L"\") OR (DiscoTag=\"" + Etiquetas[Rand]->Texto + L"\")";
			ToolTip = L"Lista aleatória por Disco :\"" + Etiquetas[Rand]->Texto + L"\" generada.";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Disco (%s)\n", Etiquetas[Rand]->Texto.c_str());
			break;
		case TLA_50Medios:
			Q = L"SELECT * FROM Medios WHERE TipoMedio=1 ORDER BY RANDOM() LIMIT 50";
			Debug_Escribir(L"RaveBD::GenerarListaAleatoria Tipo : 50 Canciones.\n");
			ToolTip = L"Lista aleatória con 50 canciones generada.";
			break;
	}

	App.MostrarToolTip(App.VentanaRave, ToolTip);
	
	int				    SqlRet = 0;
	sqlite3_stmt       *SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDMedio TmpMedio;
			TmpMedio.ObtenerFila(SqlQuery, Unidades);
			OUT_Medios.push_back(TmpMedio);
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);
}

const BOOL RaveBD::_CrearTablas(void) {

	// Creo la tabla para guardar la ultima lista reproducida
	std::wstring CrearTablaUltimaLista = L"CREATE TABLE UltimaLista (Hash BIGINT)";
	if (Consulta(CrearTablaUltimaLista.c_str()) == SQLITE_ERROR) return FALSE;

	// Creo la tabla para las opciones
	std::wstring CrearTablaOpciones = L"CREATE TABLE Opciones ("
											L"Id" 						L" INTEGER PRIMARY KEY," 
											L"Volumen"					L" INTEGER,"             
											L"PathAbrir"				L" VARCHAR(260),"	 	  
											L"PosX"						L" INTEGER,"             
											L"PosY"						L" INTEGER,"             
											L"Ancho"					L" INTEGER,"             
											L"Alto"						L" INTEGER,"             
											L"Shufle"					L" INTEGER,"             
											L"Repeat"					L" INTEGER,"             
											L"Inicio"					L" INTEGER,"             
											L"OcultarMouseEnVideo"		L" INTEGER,"             
											L"Version"					L" DOUBLE,"				  
											L"MostrarObtenerMetadatos"	L" INTEGER,"
											L"MostrarAsociarArchivos"	L" INTEGER,"
											L"AnalizarMediosPendientes" L" INTEGER,"
											L"VentanaOpciones_PosX"		L" INTEGER,"             
											L"VentanaOpciones_PosY"		L" INTEGER,"             
											L"VentanaAsociar_PosX"		L" INTEGER,"             
											L"VentanaAsociar_PosY"		L" INTEGER," 
											L"VentanaAnalizar_PosX"		L" INTEGER,"             
											L"VentanaAnalizar_PosY"		L" INTEGER" 
										")";
	if (Consulta(CrearTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;

	// Añado los datos por defecto de las opciones
	std::wstring ValoresTablaOpciones = L"INSERT INTO Opciones (ID, Volumen, PathAbrir, PosX, PosY, Ancho, Alto, Shufle, Repeat, Inicio, OcultarMouseEnVideo, Version, MostrarObtenerMetadatos, MostrarAsociarArchivos, AnalizarMediosPendientes, VentanaOpciones_PosX, VentanaOpciones_PosY, VentanaAsociar_PosX, VentanaAsociar_PosY, VentanaAnalizar_PosX, VentanaAnalizar_PosY) "
										L"VALUES(0, 100, \"C:\\\", 100, 100, 660, 400, 0, 0, 0, 3000," RAVE_VERSIONBD ", 1, 1, 1, 400, 300, 500, 400, 300, 200)";
	if (Consulta(ValoresTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;

	// Creo la tabla para las raices
	std::wstring CrearTablaRaiz = L"CREATE TABLE Raiz (Id INTEGER PRIMARY KEY, Path VARCHAR(260), IdDisco INTEGER)";
	if (Consulta(CrearTablaRaiz.c_str()) == SQLITE_ERROR) return FALSE;
	
	/*	Tipo				Nombre			Posición		Tipo
		---------------------------------------------------------------------- -
		UINT				Id				    0			INTEGER PRIMARY KEY
		sqlite_int64		Hash			    1			INT UNIQUE
		std::wstring		Path			    2			VARCHAR(260)
		std::wstring		NombrePath		    3			VARCHAR(128)
		Tipo_Medio			TipoMedio		    4			INT
		Extension_Medio		Extension		    5			INT
		UINT				Reproducido		    6			INT
		ULONG				Longitud		    7			INT
		DWORD				IDDisco			    8			INT
		float				Nota			    9			DOUBLE
		std::wstring		Genero		       10			VARCHAR(128)
		std::wstring		GrupoPath	       11			VARCHAR(128)
		std::wstring		DiscoPath	       12			VARCHAR(128)
		UINT				PistaPath	       13			INT
		libvlc_time_t		Tiempo		       14			INT
		std::wstring		Subtitulos	       15			VARCHAR(260)
		BOOL				Parseado           16			TINYINT(1)
		std::wstring		NombreTag		   17			VARCHAR(128)
		std::wstring		GrupoTag		   18			VARCHAR(128)
		std::wstring		DiscoTag		   19			VARCHAR(128)
		UINT				PistaTag	       20			INT
		BOOL				NombreEleccion     21			TINYINT(1)
		BOOL				GrupoEleccion      22			TINYINT(1)
		BOOL				DiscoEleccion      23			TINYINT(1)
		BOOL				PistaEleccion      24			TINYINT(1)				*/
	// Creo la tabla para los medios
	std::wstring CrearTablaMedios =	L"CREATE TABLE Medios ("							
										L"Id "				L"INTEGER PRIMARY KEY, "	//  0
 										L"Hash "			L"BIGINT UNIQUE, "			//  1
										L"Path "			L"VARCHAR(260), "			//  2
										L"NombrePath "		L"VARCHAR(128), "			//  2
										L"TipoMedio "		L"INT, "					//  4
										L"Extension "		L"INT, "					//  5
										L"Reproducido "		L"INT, "					//  6
										L"Longitud "		L"INT, "					//  7
										L"IDDisco "			L"INT, "					//  8
										L"Nota "			L"DOUBLE, "					//  9
										L"Genero "			L"VARCHAR(128), "			// 10
										L"GrupoPath "		L"VARCHAR(128), "			// 11
										L"DiscoPath "		L"VARCHAR(128), "			// 12
										L"PistaPath "		L"INT, "					// 13
										L"Tiempo "			L"INT, "					// 14
										L"Subtitulos "		L"VARCHAR(260), "			// 15
										L"Parseado "		L"TINYINT(1), "				// 16
										L"NombreTag "		L"VARCHAR(128), "			// 17
										L"GrupoTag "		L"VARCHAR(128), "			// 18
										L"DiscoTag "		L"VARCHAR(128), "			// 19
										L"PistaTag "		L"INT, "					// 20
										L"NombreEleccion "	L"TINYINT(1), "				// 21
										L"GrupoEleccion	"	L"TINYINT(1), "				// 22
										L"DiscoEleccion "	L"TINYINT(1), "				// 23
										L"PistaEleccion "	L"TINYINT(1)"				// 24
									L")";
	if (Consulta(CrearTablaMedios.c_str()) == SQLITE_ERROR)
		return FALSE;


	// Creo la tabla para las etiquetas
	std::wstring CrearTablaEtiquetas = L"CREATE TABLE Etiquetas ("
											L"Id" 				L" INTEGER PRIMARY KEY," 
											L"Texto"			L" VARCHAR(260),"	 	  
											L"Tipo"				L" INTEGER,"
											L"Medios"			L" INTEGER,"
											L"Nota"				L" DOUBLE,"				
											L"Tiempo"			L" BIGINT"				
										")";

	if (Consulta(CrearTablaEtiquetas.c_str()) == SQLITE_ERROR) return FALSE;



	return TRUE;
}



/* Devuelve 0 si ya existe una raiz igual, devuelve 1 si se ha agregado una nueva raiz, devuelve 2 si se ha agregado una nueva raiz, pero han desaparecido otras raices parientes */

/*	Se puede dar el caso en que dada una raíz existente se quiera agregar otra raíz que es pariente de esta.
	En este no se añadirá ninguna raíz y la raíz existente pasara a tener el path que ocupe menos caracteres. */
const int RaveBD::AgregarRaiz(std::wstring &nPath) {
	int				Ret				= 1;
	UINT			Total			= 0;
	int				SqlRet			= 0;
	size_t          i				= 0;
	sqlite3_stmt   *SqlQuery		= NULL;
	std::wstring	PathFinal		= nPath;
	if (PathFinal[PathFinal.size() - 1] != TEXT('\\')) PathFinal += TEXT('\\');

	std::wstring    RaizFinal		= PathFinal;		// Path final mas corto
	size_t			PathMasCorto	= RaizFinal.size();

	// Busco raices similares
	std::vector<BDRaiz *> RaicesSimilares;
	for (i = 0; i < _Raices.size(); i++) {
		// Si las raices se parecen y no tienen el mismo número de caracteres
		if (_CompararRaices(_Raices[i]->Path, PathFinal) == TRUE) {
			if (_Raices[i]->Path.size() != PathFinal.size()) {
				RaicesSimilares.push_back(_Raices[i]);
				// Busco el path mas corto que será el definitivo
				if (PathMasCorto > _Raices[i]->Path.size()) {
					PathMasCorto = _Raices[i]->Path.size();
					RaizFinal = _Raices[i]->Path;
				}
			}
			// La raiz es igual
			else {
				return 0;
			}
		}
	}

	// Borro todas las raices similares
	for (i = 0; i < RaicesSimilares.size(); i++) {
		std::wstring Q = L"DELETE FROM Raiz WHERE Path=\"" + RaicesSimilares[i]->Path + L"\"";
		SqlRet = Consulta(Q);
		Ret = 2;
	}

	DWL::DUnidadDisco  *Unidad = Unidades.Buscar_Letra(RaizFinal[0]);
	if (Unidad == NULL)
		return NULL;

	std::wstring SqlStr = L"INSERT INTO Raiz (Path, IDDisco) VALUES(\"" + RaizFinal + L"\", " + std::to_wstring(Unidad->Numero_Serie()) + L")";
	SqlRet = Consulta(SqlStr);

	ObtenerRaices();

	return Ret;
}


const BOOL RaveBD::EliminarRaiz(std::wstring &nPath) {
	std::wstring Q = L"DELETE FROM Raiz WHERE Path=\"" + nPath + L"\"";
	int SqlRet = Consulta(Q);
//	BOOL Ret = ConsultaVarg(L"DELETE FROM Raiz WHERE Path ='%s'", nPath.c_str());
	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}
	ObtenerRaices();
	return TRUE;
}

// Analiza e inserta el medio en la BD
// Devuelve FALSE si ahy un error
// Devuelve TRUE si ya se ha añadido el medio
// Devuelve 2 si el medio ya existia en la BD
const BOOL RaveBD::AnalizarMedio(std::wstring &nPath, BDMedio &OUT_Medio, const ULONG Longitud) {
	// Compruebo que existe fisicamente en el disco (comprobar velocidades usando CreateFile)
	if (GetFileAttributes(nPath.c_str()) == INVALID_FILE_ATTRIBUTES)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Obtenir temps d'un mp3 en milisegons																							//
	// http://stackoverflow.com/questions/3505575/how-can-i-get-the-duration-of-an-mp3-file-cbr-or-vbr-with-a-very-small-library	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Recorto el path y elimino los dos primeros caracteres que representan la unidad
	std::wstring		PathCortado = nPath;
	PathCortado[0] = L'?';
	DWL::DUnidadDisco  *UnidadDisco = Unidades.Buscar_Letra(nPath[0]);


	size_t				PosNombre		= nPath.find_last_of(TEXT("\\"));																					// Posición donde empieza el nombre
	size_t				PosExtension	= nPath.find_last_of(TEXT("."));																					// Posición donde empieza la extensión
	Extension_Medio		Extension		= ExtensionesValidas::ObtenerExtension(nPath.substr(PosExtension + 1, (nPath.size() - PosExtension) - 1).c_str());	// Obtengo el tipo de extensión
	Tipo_Medio			Tipo			= ExtensionesValidas::ObtenerTipoMedio(Extension);																	// Obtengo el tipo de medio
	sqlite3_int64		Hash			= CrearHash(UnidadDisco->Numero_Serie(), PathCortado);																// Creo un hash partiendo del path y el número de serie del disco
	UINT				Pista			= 0;																												// Número de pista
	std::wstring		TmpNombre;
	std::wstring		NombreFinal;
	std::wstring        SqlStr;
	int                 SqlRet = 0;
//	ULONG               Longitud = 0;

	Strings::Split PathSeparado(nPath, L'\\');

	switch (Tipo) {
		case Tipo_Medio_INDEFINIDO:
			return FALSE;
		case Tipo_Medio_Audio:
			if (PathSeparado.Total() > 1) {
//				OUT_Medio.DiscoPath = PathSeparado[PathSeparado.Total() - 2];
				FiltroNombre(PathSeparado[PathSeparado.Total() - 2], OUT_Medio.DiscoPath);
			}
			if (PathSeparado.Total() > 2) {
//				OUT_Medio.GrupoPath = PathSeparado[PathSeparado.Total() - 3];
				FiltroNombre(PathSeparado[PathSeparado.Total() - 3], OUT_Medio.GrupoPath);
			}
		case Tipo_Medio_Video:
			AnalizarNombre(nPath.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, Pista);
			FiltroNombre(TmpNombre, NombreFinal);
			OUT_Medio.Hash			= Hash;
			OUT_Medio.Path			= PathCortado;
			OUT_Medio.NombrePath	= NombreFinal;
			OUT_Medio.TipoMedio		= Tipo;
			OUT_Medio.Extension		= Extension;
			OUT_Medio.PistaPath		= Pista;
			OUT_Medio.IDDisco		= UnidadDisco->Numero_Serie();
			OUT_Medio.Longitud      = Longitud;
			OUT_Medio.Nota          = 2;
			OUT_Medio.Tiempo        = 0;
			SqlStr = L"INSERT INTO Medios (Hash, Path, NombrePath, TipoMedio, Extension, PistaPath, IDDisco, Longitud, Nota, Tiempo, Subtitulos, Parseado, DiscoPath, GrupoPath)"
								  L" VALUES(" + std::to_wstring(Hash)							+ L",\"" +				// Hash
												PathCortado										+ L"\",\"" +			// Path
												NombreFinal										+ L"\", " +				// NombrePath
												std::to_wstring(Tipo)							+ L"," +				// Tipo
												std::to_wstring(Extension)						+ L"," +				// Extension
												std::to_wstring(Pista)							+ L"," +				// PistaPath
												std::to_wstring(UnidadDisco->Numero_Serie())	+ L"," +				// ID Disco Duro
												std::to_wstring(Longitud)						+ L"," +				// Longitud en bytes
												L"2.5," +															// Nota
												L"0," +																// Tiempo
												L"\"\"," +															// Subtitulos
												L"0,\"" +															// Parseado
												OUT_Medio.DiscoPath								+ L"\",\"" +			// DiscoPath
												OUT_Medio.GrupoPath								+ L"\")";				// GrupoPath
			SqlRet = Consulta(SqlStr);
			if (SqlRet == SQLITE_DONE)
				return TRUE; // No existe el hash
			else if (SqlRet == SQLITE_CONSTRAINT)
				return 2; // Ya existe el hash
			else  /* Error ? */
				_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
				return FALSE;

			break;
		//			_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, static_cast<TMedioVideo *>(Medio)->Pista);
//		break;
		case Tipo_Medio_CDAudio:
		//		_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, static_cast<TMedioCDAudio *>(Medio)->Pista);
			break;
		case Tipo_Medio_Lista:
		//		_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, TmpPista);
			break;
	}
	return FALSE;
}


/* Función estatica para crear un hash a partir de un numero de serie del disco y el path de la canción */
/* NOTA se eliminan los dos primeros carácteres del path que son los de la unidad, ya que los medios extraibles pueden cambiar de letra. */
const sqlite3_int64 RaveBD::CrearHash(DWORD NumeroSerieDisco, std::wstring &nPath) {
	//	std::wstring PathCortado = Path.substr(2, Path.size() - 2);
	std::hash<std::wstring> HashFunc;
	return HashFunc(std::to_wstring(NumeroSerieDisco) + nPath.substr(2, nPath.size() - 2));
}


const BOOL RaveBD::ObtenerUltimaLista(void) {

	App.VentanaRave.Lista.BorrarListaReproduccion();

	const wchar_t  *SqlStr = L"SELECT * FROM UltimaLista";
	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDMedio Medio;
			ObtenerMedio(sqlite3_column_int64(SqlQuery, 0), Medio);
			App.VentanaRave.Lista.AgregarMedio(&Medio);
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}

	sqlite3_finalize(SqlQuery);
	App.VentanaRave.Lista_Play();

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}


	return (SqlRet != SQLITE_BUSY);

}

const BOOL RaveBD::GuardarUltimaLista(void) {
	// Botto todos los datos de la tabla UltimaLista sin borrar la tabla
	Consulta(L"DELETE FROM UltimaLista");
	std::wstring Q;
	int          SqlRet;
	for (size_t i = 0; i < App.VentanaRave.Lista.TotalItems(); i++) {
		Q = L"INSERT INTO UltimaLista (Hash) VALUES(" + std::to_wstring(App.VentanaRave.Lista.Medio(i)->Hash) + L")";
		SqlRet = Consulta(Q);
		if (SqlRet != SQLITE_DONE) return FALSE;
	}
	return TRUE;
}


/*
	IN  -> Analisis
	OUT -> nNombre
	OUT -> nPista
	Nombres de pista validos : 1, 01, 1x01, 1-12, 20x21
	Nunca se hace caso a 4 digitos seguidos o mas (ya que son o el año o otra cosa.. pero la pista seguro que no)
	Se analiza de izquierda a derecha (lo mas normal es que la pista salga a la izquieda nada mas empezar)
*/
const BOOL RaveBD::AnalizarNombre(std::wstring &Analisis, std::wstring &nNombre, UINT &nPista) {
	bool PistaEncontrada = false;
	BOOL C1, C2, C3, C4, C5;
	for (size_t i = 0; i < Analisis.size(); i++) {
		if (_EsNumero(Analisis[i]) == TRUE) {
			C1 = FALSE;	C2 = FALSE;	C3 = FALSE;	C4 = FALSE;	C5 = FALSE;
			if (Analisis.size() > i + 1) C1 = _EsNumero(Analisis[i + 1]);
			if (Analisis.size() > i + 2) C2 = _EsNumero(Analisis[i + 2]);
			if (Analisis.size() > i + 3) C3 = _EsNumero(Analisis[i + 3]);
			if (Analisis.size() > i + 4) C4 = _EsNumero(Analisis[i + 4]);
			if (Analisis.size() > i + 5) C5 = _EsNumero(Analisis[i + 5]);
			// 01?01? (si el separador es un espacio no se tomara como pista doble)
			if (C1 == TRUE && C2 == FALSE && Analisis[i + 2] != TEXT(' ') && C3 == TRUE && C4 == TRUE && C5 == FALSE) { // Pista doble, se multiplicara por 100 la primera parte de la pista
				nPista = (_wtoi(Analisis.substr(i, 2).c_str()) * 100) + _wtoi(Analisis.substr(i + 3, 2).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 5, Analisis.size() - (i + 5));
				return TRUE;
			}
			// 1?01? (si el separador es un espacio no se tomara como pista doble)
			if (C1 == FALSE && Analisis[i + 1] != TEXT(' ') && C2 == TRUE && C3 == TRUE && C4 == FALSE) { // Pista doble, se multiplicara por 100 la primera parte de la pista
				nPista = (_wtoi(Analisis.substr(i, 1).c_str()) * 100) + _wtoi(Analisis.substr(i + 2, 2).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 4, Analisis.size() - (i + 4));
				return TRUE;
			}
			// 001?
			if (C1 == TRUE && C2 == TRUE && C3 == FALSE) { // Pista unica tres digitos
				nPista = _wtoi(Analisis.substr(i, 3).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 3, Analisis.size() - (i + 3));
				return TRUE;
			}
			// 01?
			if (C1 == TRUE && C2 == FALSE) { // Pista unica dos digitos
				nPista = _wtoi(Analisis.substr(i, 2).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 2, Analisis.size() - (i + 2));
				return TRUE;
			}
			// 1?
			if (C1 == FALSE) { // Pista unica un digito
				nPista = _wtoi(Analisis.substr(i, 1).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 1, Analisis.size() - (i + 1));
				return TRUE;
			}
		}
	}

	nPista = 0;
	nNombre = Analisis;
	return FALSE;
}


const BOOL RaveBD::_EsNumero(const wchar_t Caracter) {
	if (Caracter >= TEXT('0') && Caracter <= TEXT('9'))	return TRUE;
	return FALSE;
}


/* Para filtrar caracteres molestos en nombres de genero grupo disco y canciones */
void RaveBD::FiltroNombre(std::wstring &In, std::wstring &Out) {
	Out.resize(0);
	Out.reserve(In.size());
	bool Espacio = false;
	for (size_t i = 0; i < In.size(); i++) {
		switch (In[i]) {
			case 65279: // caracter invisible inutil del unicode
				break;
			case TEXT(':'): case TEXT('_'): case TEXT(','): case TEXT('.'): case 96  /*'*/: case TEXT('('): case TEXT(')'):
			case TEXT('!'): case TEXT('?'): case TEXT('¿'): case TEXT('^'): case TEXT('"'): case TEXT('#'): case TEXT('$'):
			case TEXT('%'): case TEXT('/'): case TEXT('\\'): case TEXT('-'): case TEXT('['): case TEXT(']'): case TEXT('~'):
			case ' ':
				Espacio = false;
				if (Out.size() != 0) {
					if (In[Out.size() - 1] != L' ') {
						Espacio = true;
					}
				}
				else {
					Espacio = true;
				}

				// Si el caracter anterior no es un espacio y no es el primer caracter, pongo un espacio. 
				if (Espacio == true && Out.size() != 0) Out += TEXT(' ');
				break;
			default:
				Out += static_cast<TCHAR>(tolower(In[i]));
				break;
		}
	}

	// Quito los espacios del principio
	while (Out[0] == L' ') {
		Out = Out.substr(1);
	}
	// Quito los espacios del final
	if (Out.size() > 0) {
		while (Out[Out.size() - 1] == L' ') {
			Out.resize(Out.size() - 1);
			if (Out.size() == 0) break;
		}
	}

	// Primer caracter en mayusculas
	Out[0] = toupper(Out[0]);
}

void RaveBD::FiltroPath(std::wstring &In, std::wstring &Out) {
	Out.reserve(In.size());
	for (size_t i = 0; i < In.size(); i++) {
		if (i == 0) Out = static_cast<wchar_t>(toupper(In[0]));
		else        Out += static_cast<wchar_t>(tolower(In[i]));
	}

}





void RaveBD::Opciones_Volumen(const int nVolumen) {
	_Opciones_Volumen = nVolumen;
	std::wstring Q = L"Update Opciones SET Volumen=" + std::to_wstring(nVolumen) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveBD::Opciones_Shufle(const BOOL nShufle) {
	_Opciones_Shufle = nShufle;
	std::wstring Q = L"Update Opciones SET Shufle=" + std::to_wstring(nShufle) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveBD::Opciones_Repeat(const Tipo_Repeat nRepeat) {
	_Opciones_Repeat = nRepeat;

	// Guardo el valor del repeat en la BD, siempre que el repeat no implique apagar el reproductor o el windows
	if (nRepeat != Tipo_Repeat_ApagarReproductor && nRepeat != Tipo_Repeat_ApagarOrdenador /*&& nRepeat != Tipo_Repeat_HibernarOrdenador*/) {
		std::wstring Q = L"Update Opciones SET Repeat=" + std::to_wstring(nRepeat) + L" WHERE Id=0";
		Consulta(Q.c_str());
	}
}

void RaveBD::Opciones_Inicio(const Tipo_Inicio nInicio) {
	_Opciones_Inicio = nInicio;
	std::wstring Q = L"Update Opciones SET Inicio=" + std::to_wstring(nInicio) + L" WHERE Id=0";
	Consulta(Q.c_str());
}


void RaveBD::Opciones_OcultarMouseEnVideo(const int nOcultarMouseEnVideo) {
	_Opciones_OcultarMouseEnVideo = nOcultarMouseEnVideo;
	std::wstring Q = L"Update Opciones SET OcultarMouseEnVideo=" + std::to_wstring(nOcultarMouseEnVideo) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveBD::Opciones_MostrarObtenerMetadatos(const BOOL nMostrarObtenerMetadatos) {
	_Opciones_MostrarObtenerMetadatos = nMostrarObtenerMetadatos;
	std::wstring Q = L"Update Opciones SET MostrarObtenerMetadatos=" + std::to_wstring(nMostrarObtenerMetadatos) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
	Ret = Ret;
}

void RaveBD::Opciones_MostrarAsociarArchivos(const BOOL nMostrarAsociarArchivos) {
	_Opciones_MostrarObtenerMetadatos = nMostrarAsociarArchivos;
	std::wstring Q = L"Update Opciones SET MostrarAsociarArchivos=" + std::to_wstring(nMostrarAsociarArchivos) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
	Ret = Ret;
}

void RaveBD::Opciones_AnalizarMediosPendientes(const BOOL nAnalizarMediosPendientes) {
	_Opciones_AnalizarMediosPendientes = nAnalizarMediosPendientes;
	std::wstring Q = L"Update Opciones SET AnalizarMediosPendientes=" + std::to_wstring(nAnalizarMediosPendientes) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
	Ret = Ret;
}

const BOOL RaveBD::ObtenerOpciones(void) {

	const wchar_t  *SqlStr = L"SELECT * FROM Opciones";
	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			_Opciones_Volumen					= static_cast<int>(sqlite3_column_int(SqlQuery, 1));
			_Opciones_PathAbrir					= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
			_Opciones_PosX						= static_cast<int>(sqlite3_column_int(SqlQuery, 3));
			_Opciones_PosY						= static_cast<int>(sqlite3_column_int(SqlQuery, 4));
			_Opciones_Ancho						= static_cast<int>(sqlite3_column_int(SqlQuery, 5));
			_Opciones_Alto						= static_cast<int>(sqlite3_column_int(SqlQuery, 6));
			_Opciones_Shufle					= static_cast<int>(sqlite3_column_int(SqlQuery, 7));
			_Opciones_Repeat					= static_cast<Tipo_Repeat>(sqlite3_column_int(SqlQuery, 8));
			_Opciones_Inicio					= static_cast<Tipo_Inicio>(sqlite3_column_int(SqlQuery, 9));
			_Opciones_OcultarMouseEnVideo		= static_cast<int>(sqlite3_column_int(SqlQuery, 10));
			_Opciones_Version					= static_cast<float>(sqlite3_column_double(SqlQuery, 11));
			_Opciones_MostrarObtenerMetadatos	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 12));
			_Opciones_MostrarAsociarArchivos	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 13));
			_Opciones_AnalizarMediosPendientes  = static_cast<BOOL>(sqlite3_column_int(SqlQuery, 14));
			_Opciones_VentanaOpciones_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 15));
			_Opciones_VentanaOpciones_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 16));
			_Opciones_VentanaAsociar_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 17));
			_Opciones_VentanaAsociar_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 18));
			_Opciones_VentanaAnalizar_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 19));
			_Opciones_VentanaAnalizar_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 20));
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);
}
/*
const BOOL RaveBD::AsignarTiempoMedio(const libvlc_time_t nTiempo, const sqlite3_int64 mHash) {
	int SqlRet = ConsultaVarg(L"UPDATE Medios SET Tiempo=%d WHERE Hash=%d", nTiempo, mHash);
	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}
	return TRUE;
}*/


/* NOTA es mejor tener 2 selects para las opciones, uno para el tamaño y posición de la ventana, y otro para el resto de valores (shufle, repeat, volumen, etc...)
Y no viene de 15 milisegundos mas a la hora de cerrar el reproductor */
const BOOL RaveBD::Opciones_GuardarOpciones(void) {
	Opciones_GuardarPosTamVentana();
	std::wstring Q = L"UPDATE Opciones SET"
		L" Volumen=" + std::to_wstring(_Opciones_Volumen) +
		//						L", Volumen=" + std::to_wstring(_Volumen) +
		L", Shufle=" + std::to_wstring(_Opciones_Shufle) +
		L", Repeat=" + std::to_wstring(_Opciones_Repeat) +
		L" WHERE Id=0";
	int SqlRet = Consulta(Q);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveBD::Opciones_GuardarPosTamVentana(void) {
	if (App.VentanaRave.Maximizada() == FALSE) {
		RECT RC;
		GetWindowRect(App.VentanaRave.hWnd(), &RC);
		_Opciones_PosX = RC.left;
		_Opciones_PosY = RC.top;
		_Opciones_Ancho = abs(RC.right - RC.left);
		_Opciones_Alto = abs(RC.bottom - RC.top);

		std::wstring Q = L"UPDATE Opciones SET PosX=" + std::to_wstring(RC.left) + L", PosY=" + std::to_wstring(RC.top) + L", Ancho=" + std::to_wstring(_Opciones_Ancho) + L", Alto=" + std::to_wstring(_Opciones_Alto) + L" WHERE Id=0";
		int SqlRet = Consulta(Q);
		if (SqlRet == SQLITE_ERROR) {
			_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

const BOOL RaveBD::Opciones_GuardarPosVentanaOpciones(void) {
	RECT RV;
	GetWindowRect(App.VentanaOpciones.hWnd(), &RV);
	_Opciones_VentanaOpciones_PosX = RV.left;
	_Opciones_VentanaOpciones_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaOpciones_PosX=" + std::to_wstring(RV.left) + L", VentanaOpciones_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveBD::Opciones_GuardarPosVentanaAsociar(void) {
	RECT RV;
	GetWindowRect(App.VentanaAsociar.hWnd(), &RV);
	_Opciones_VentanaAsociar_PosX = RV.left;
	_Opciones_VentanaAsociar_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaAsociar_PosX=" + std::to_wstring(RV.left) + L", VentanaAsociar_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveBD::Opciones_GuardarPosVentanaAnalizar(void) {
	RECT RV;
	GetWindowRect(App.VentanaRave.ThreadAnalizar.hWnd(), &RV);
	_Opciones_VentanaAnalizar_PosX = RV.left;
	_Opciones_VentanaAnalizar_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaAnalizar_PosX=" + std::to_wstring(RV.left) + L", VentanaAnalizar_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}



const BOOL RaveBD::ObtenerMediosPorRevisar(std::vector<BDMedio> &Medios) {
	const wchar_t  *Q = L"SELECT Id, NombrePath, Genero, GrupoPath, DiscoPath, NombreTag, GrupoTag, DiscoTag, Path, PistaTag, PistaPath, Nota, Tiempo FROM Medios WHERE TipoMedio=1"; // Tipo_Medio_Audio
	
	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	BDMedio         TmpMedio;

	SqlRet = sqlite3_prepare16_v2(_BD, Q, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {

			TmpMedio.Id = static_cast<UINT>(sqlite3_column_int(SqlQuery, 0));
			const wchar_t *nNombrePath	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			if (nNombrePath != NULL)	TmpMedio.NombrePath = nNombrePath;
			const wchar_t *nGenero		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
			const wchar_t *nGrupoPath	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
			const wchar_t *nDiscoPath	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 4));
			if (nGenero != NULL)		TmpMedio.Genero = nGenero;
			if (nGrupoPath != NULL)		TmpMedio.GrupoPath = nGrupoPath;
			if (nDiscoPath != NULL)		TmpMedio.DiscoPath = nDiscoPath;
			const wchar_t *nNombreTag	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 5));
			const wchar_t *nGrupoTag	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 6));
			const wchar_t *nDiscoTag	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 7));
			if (nNombreTag != NULL)		TmpMedio.NombreTag = nNombreTag;
			if (nGrupoTag != NULL)		TmpMedio.GrupoTag = nGrupoTag;
			if (nDiscoTag != NULL)		TmpMedio.DiscoTag = nDiscoTag;
			TmpMedio.Path               = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 8));
			TmpMedio.PistaTag			= static_cast<UINT>(sqlite3_column_int(SqlQuery, 9));
			TmpMedio.PistaPath			= static_cast<UINT>(sqlite3_column_int(SqlQuery, 10));
			TmpMedio.Nota				= static_cast<float>(sqlite3_column_double(SqlQuery, 11));
			TmpMedio.Tiempo				= static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 12));
			Medios.push_back(TmpMedio);
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);
}



const int RaveBD::Distancia(std::wstring &Origen, std::wstring &Destino) {
	// Step 1
	const int n = static_cast<int>(Origen.size());
	const int m = static_cast<int>(Destino.size());

	if (n < 5) return n + m; // Si origen es mas pequeño que 4 caracteres
	if (m < 5) return m + n; // Si destino es mas pequeño que 4 caracteres
							 //	if (n < 3) return n; // Si origen es mas pequeño que 2 caracteres
							 //	if (m < 3) return m; // Si destino es mas pequeño que 2 caracteres

	typedef std::vector< std::vector<int> > Tmatrix;
	Tmatrix matrix(n + 1);

	// Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
	// allow for allocation on declaration of 2.nd dimension of vec of vec
	for (int i = 0; i <= n; i++) matrix[i].resize(m + 1);

	// Step 2
	for (int i = 0; i <= n; i++) matrix[i][0] = i;
	for (int j = 0; j <= m; j++) matrix[0][j] = j;
	// Si se llevan mas de dos caracteres de tamaño no hace falta comprobar nada
	if (n - m < 2 && n - m > -2) {
		// Step 3
		for (int i = 1; i <= n; i++) {
			const TCHAR s_i = Origen[i - 1];
			// Step 4
			for (int j = 1; j <= m; j++) {
				const TCHAR t_j = Destino[j - 1];

				// Step 5
				int cost;
				if (s_i == t_j)  cost = 0;
				else		     cost = 1;

				// Step 6
				const int above = matrix[i - 1][j];
				const int left = matrix[i][j - 1];
				const int diag = matrix[i - 1][j - 1];
				int       cell = min(above + 1, min(left + 1, diag + cost));

				// Step 6A: Cover transposition, in addition to deletion,
				// insertion and substitution. This step is taken from:
				// Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
				// Enhanced Dynamic Programming ASM Algorithm"
				// (http://www.acm.org/~hlb/publications/asm/asm.html)

				if (i > 2 && j > 2) {
					int trans = matrix[i - 2][j - 2] + 1;
					if (Origen[i - 2] != t_j)	trans++;
					if (s_i != Destino[j - 2])	trans++;
					if (cell > trans)			cell = trans;
				}
				matrix[i][j] = cell;
			}
		}
	}

	if (n - m < 2 && n - m > -2)	return matrix[n][m];
	else							return 100;
}



EtiquetaBD *RaveBD::ObtenerEtiqueta(std::wstring &eTexto) {
	for (size_t i = 0; i < _Etiquetas.size(); i++) {
		if (_Etiquetas[i].Texto == eTexto) {
			return &_Etiquetas[i];
		}
	}
	return NULL;
}
/*
const BOOL RaveBD::ObtenerEtiqueta(std::wstring &eTexto, EtiquetaBD &Etiqueta) {
	std::wstring Q = L"SELECT * FROM Etiquetas WHERE Texto=\"";	Q += eTexto.c_str(); Q += L"\""; 

	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	BDMedio         TmpMedio;

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) { // La iD es el 0, pero no la necesito para nada.
			Etiqueta.Texto	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			Etiqueta.Tipo	= static_cast<UINT>(sqlite3_column_int(SqlQuery, 2));
			Etiqueta.Medios	= static_cast<UINT>(sqlite3_column_int(SqlQuery, 3));
			Etiqueta.Nota	= static_cast<float>(sqlite3_column_double(SqlQuery, 4));
			Etiqueta.Tiempo = static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 5));
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (Etiqueta.Texto.size() > 0);

}
*/






















// Constructor que obtiene los datos de una fila
BDMedio::BDMedio(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades) : Actualizar(FALSE) {
	ObtenerFila(SqlQuery, Unidades);
}

/* Función que obtiene los datos de una fila :

	Tipo				Nombre			Posición		Tipo
	-----------------------------------------------------------------------
	UINT				Id				    0			INTEGER PRIMARY KEY
	sqlite_int64		Hash			    1			INT UNIQUE
	std::wstring		Path			    2			VARCHAR(260)
	std::wstring		NombrePath		    3			VARCHAR(128)
	Tipo_Medio			TipoMedio		    4			INT
	Extension_Medio		Extension		    5			INT
	UINT				Reproducido		    6			INT
	ULONG				Longitud		    7			INT
	DWORD				IDDisco			    8			INT
	UINT				Nota			    9			SMALLINT
	std::wstring		Genero		       10			VARCHAR(128)
	std::wstring		GrupoPath	       11			VARCHAR(128)
	std::wstring		DiscoPath	       12			VARCHAR(128)
	UINT				PistaPath	       13			INT
	libvlc_time_t		Tiempo		       14			INT
	std::wstring		Subtitulos	       15			VARCHAR(260)
	BOOL				Parseado           16			TINYINT(1)				
	std::wstring		NombrePath		   17			VARCHAR(128)
	std::wstring		GrupoTag		   18			VARCHAR(128)
	std::wstring		DiscoTag		   19			VARCHAR(128)
	UINT				PistaTag	       20			INT
	BOOL				NombreEleccion     21			TINYINT(1)
	BOOL				GrupoEleccion      22			TINYINT(1)
	BOOL				DiscoEleccion      23			TINYINT(1)
	BOOL				PistaEleccion      24			TINYINT(1)

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

	DWL::DUnidadDisco *Unidad = Unidades.Buscar_Numero_Serie(IDDisco);
	if (Unidad != NULL) Path[0] = Unidad->Letra();
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

