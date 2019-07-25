#include "stdafx.h"
#include "RaveBD.h"
#include "DDirectoriosWindows.h"
#include "DStringUtils.h"
#include "ListaMedios.h"

RaveBD::RaveBD(void) : _BD(NULL), _Opciones_Volumen(0), _Opciones_PosX(0), _Opciones_PosY(0), _Opciones_VentanaOpciones_PosX(0), _Opciones_VentanaOpciones_PosY(0), _Opciones_DlgDirectorios_PosX(0), _Opciones_DlgDirectorios_PosY(0), 
								  _Opciones_VentanaAnalizar_PosX(0), _Opciones_VentanaAnalizar_PosY(0), _Opciones_Ancho(0), _Opciones_Alto(0), _Opciones_Shufle(FALSE), _Opciones_Repeat(Tipo_Repeat_NADA), _Opciones_Inicio(Tipo_Inicio_NADA),
								  _Opciones_Version(0.0f), _Opciones_OcultarMouseEnVideo(0), _Opciones_MostrarObtenerMetadatos(FALSE), _Opciones_MostrarAsociarArchivos(FALSE), _Opciones_AnalizarMediosPendientes(FALSE), 
								  _Opciones_BuscarActualizacion(FALSE), _Opciones_TiempoAnimaciones(0), _Opciones_TiempoToolTips(0), _Opciones_NoAgregarMedioMenos25(FALSE), _Opciones_NoGenerarListasMenos3(FALSE), 
							 	  _Opciones_Sumar005(FALSE), _Opciones_AlineacionControlesVideo(0), _Opciones_OpacidadControlesVideo(0), _Opciones_EfectoFadeAudioMS(10000), _Opciones_DlgDirectorios_Ancho(400), _Opciones_DlgDirectorios_Alto(600), 
								  _Opciones_VentanaMomentos_PosX(100), _Opciones_VentanaMomentos_PosY(100), _Opciones_OcultarTooltipsMouse(0), _Opciones_MostrarMedioActualTitulo(TRUE), 
								  _Opciones_MezclarListaGenero(0), _Opciones_MezclarListaGrupo(0), _Opciones_MezclarListaDisco(0), _Opciones_MezclarLista50Can(0), _Opciones_MezclarListaNota(0), 
								  _Opciones_GuardarBSCP(1) {
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
	// Modifico las tablas de la BD si tienen una versión antigua
	_ModificarTablas();

	// Obtengo las raices de la base de datos
	ObtenerRaices();
	// Obtengo las opciones de la base de datos
	ObtenerOpciones();
	// Obtengo las etiquetas
	ObtenerEtiquetas();
	// Obtengo las teclas rapidas
	ObtenerTeclasRapidas();

	return TRUE;
}

// Cierra la Base de datos
void RaveBD::Terminar(void) {
	Debug_Escribir(L"RaveBD::Terminar\n");
	sqlite3_close_v2(_BD);
	_BD = NULL;
}


const LONG_PTR RaveBD::UltimaIdInsertada(void) {
	return static_cast<LONG_PTR>(sqlite3_last_insert_rowid(_BD));
}


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

// Obtiene el medio por el hash
const BOOL RaveBD::ObtenerMedio(const sqlite3_int64 mHash, BDMedio &OUT_Medio) {
	std::wstring    SqlStr = L"SELECT * FROM Medios WHERE Hash =" + std::to_wstring(mHash);
	return _ConsultaObtenerMedio(SqlStr, OUT_Medio);
}

// Obtiene el medio por el path
const BOOL RaveBD::ObtenerMedio(std::wstring &mPath, BDMedio &OUT_Medio) {
	std::wstring TmpPath = mPath;
	TmpPath[0] = L'?'; // Evito la letra de unidad (para raices cambiantes)
	std::wstring SqlStr = L"SELECT * FROM Medios WHERE Path like \"" + TmpPath + L"\" COLLATE NOCASE"; // COLLATE NOCASE = Comparar strings case insensitive
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

/*const float RaveBD::ObtenerEtiquetaNota(EtiquetaBD *nEtiqueta) {
	if (nEtiqueta->EsGrupoPath() == TRUE) {

	}
	else if (nEtiqueta->EsDiscoPath() == TRUE) {

	}
	return 0.0f;
}*/

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
				L"Tiempo"			L" BIGINT,"
				L"Longitud"			L" BIGINT"
			")";*/
			std::wstring	Texto	 = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			UINT			Tipo	 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 2));
			UINT			Medios	 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 3));
			float			Nota	 = static_cast<float>(sqlite3_column_double(SqlQuery, 4));
			INT64			Tiempo   = static_cast<INT64>(sqlite3_column_int64(SqlQuery, 5));
			ULONGLONG		Longitud = static_cast<ULONGLONG>(sqlite3_column_int64(SqlQuery, 6));
			_Etiquetas.push_back(EtiquetaBD(Texto, Tipo, Nota, Tiempo, Longitud, Medios));
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
	INT64				Tiempo		       14			INT
	std::wstring		Subtitulos	       15			VARCHAR(260)
	BOOL				Parseado           16			TINYINT(1)
	std::wstring		NombreTag		   17			VARCHAR(128)
	std::wstring		GrupoTag		   18			VARCHAR(128)
	std::wstring		DiscoTag		   19			VARCHAR(128)
	UINT				PistaTag	       20			INT
	BOOL				NombreEleccion     21			TINYINT(1)
	BOOL				GrupoEleccion      22			TINYINT(1)
	BOOL				DiscoEleccion      23			TINYINT(1)
	BOOL				PistaEleccion      24			TINYINT(1)
	std::wstring		Proporcion 		   25           VARCHAR(16)
	float               Brillo 			   26			DOUBLE
	float               Contraste		   27			DOUBLE
	float               Saturacion 		   28			DOUBLE		*/
const BOOL RaveBD::ActualizarMedio(BDMedio *nMedio) {
	std::wstring Q =	L"UPDATE Medios SET "
							L"NombrePath=\""	+ nMedio->NombrePath						+ L"\","	//	1
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
							L"PistaEleccion="	+ std::to_wstring(nMedio->PistaEleccion)	+ L","
							L"Proporcion=\""	+ nMedio->Proporcion						+ L"\","
							L"Brillo="			+ std::to_wstring(nMedio->Brillo)			+ L","
							L"Contraste="		+ std::to_wstring(nMedio->Contraste)		+ L","
							L"Saturacion="		+ std::to_wstring(nMedio->Saturacion)		+ L" "
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

// Actualiza el path del medio (SOLO para medios con extension CRDOWNLOAD y OPDOWNLOAD)
// YA NO SIRVE con el Chrome..
const BOOL RaveBD::ActualizarPathMedio(std::wstring &Path, const UINT mID) {
	size_t				PosNombre	 = Path.find_last_of(TEXT("\\"));																				// Posición donde empieza el nombre
	size_t				PosExtension = Path.find_last_of(TEXT("."));																				// Posición donde empieza la extensión
	Extension_Medio		Extension = ExtensionesValidas::ObtenerExtension(Path.substr(PosExtension + 1, (Path.size() - PosExtension) - 1).c_str());	// Obtengo el tipo de extensión
	std::wstring Q = L"UPDATE Medios SET Path =\"" + Path + L"\", Extension =" + std::to_wstring(Extension) + L" WHERE Id=" + std::to_wstring(mID);
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

// Función que genera una lista aleatória por el tipo especificado, y la guarda en la variable OUT_Medios 
const BOOL RaveBD::GenerarListaAleatoria(std::vector<BDMedio> &OUT_Medios, const TipoListaAleatoria nTipo) {
	// si no hay etiquetas solo se puede generar listas aleatorias de 50 medios sin ninguna base, si el tipo especificado no se puede gemerar. salgo
	if (_Etiquetas.size() == 0 && nTipo != TLA_50Medios && nTipo != TLA_LoQueSea && nTipo != TLA_Nota) {
		App.MostrarToolTipPlayer(L"No se pueden generar listas aleatórias hasta que no se analize la base de datos.");
		return FALSE;
	}

	TipoListaAleatoria Tipo = nTipo;
	if (Tipo == TLA_LoQueSea) {
		Tipo = static_cast<TipoListaAleatoria>(App.Rand(static_cast<int>(TLA_LoQueSea - 1), static_cast<int>(TLA_Genero)));
	}

	// Si no hay etiquetas solo se puede generar una lista por TLA_50Medios
	if (_Etiquetas.size() == 0)		Tipo = TLA_50Medios;

	#define MAX_INTENTOS 20

	std::vector<EtiquetaBD *>	Etiquetas;
	std::wstring				Q;
	int							Intentos	= 0;
	size_t						Rand		= 0;
	int							Mezclar		= 0;

	switch (Tipo) {
		case TLA_Genero:
			for (size_t i = 0; i < _Etiquetas.size(); i++) {
				if (_Etiquetas[i].EsGenero() == TRUE) Etiquetas.push_back(&_Etiquetas[i]);
			}
			if (Etiquetas.size() == 0) {
				App.MostrarToolTipPlayerError(L"No hay etiquetas con generos para generar una lista aleatória.");
				return FALSE;
			}
			// Si la etiqueta tiene menos de 5 medios vuelvo a obtener una etiqueta aleatória			
			if (App.BD.Opciones_NoGenerarListasMenos3() == TRUE) {
				do {
					Rand = App.Rand<size_t>(Etiquetas.size());
				} while (Etiquetas[Rand]->Medios < 5 && Intentos++ < MAX_INTENTOS);
			}
			Q = L"SELECT * FROM Medios WHERE Genero=\"" + Etiquetas[Rand]->Texto + L"\"";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Genero (%s)\n", Etiquetas[Rand]->Texto.c_str());
			Mezclar = _Opciones_MezclarListaGenero;
			break;
		case TLA_Grupo:
			for (size_t i = 0; i < _Etiquetas.size(); i++) {
				if (_Etiquetas[i].EsGrupoPath() == TRUE || _Etiquetas[i].EsGrupoTag() == TRUE) Etiquetas.push_back(&_Etiquetas[i]);
			}
			if (Etiquetas.size() == 0) {
				App.MostrarToolTipPlayerError(L"No hay etiquetas con grupos para generar una lista aleatória.");
				return FALSE;
			}
			// Si la etiqueta tiene menos de 5 medios vuelvo a obtener una etiqueta aleatória			
			if (App.BD.Opciones_NoGenerarListasMenos3() == TRUE) {
				do {
					Rand = App.Rand<size_t>(Etiquetas.size());
				} while (Etiquetas[Rand]->Medios < 5 && Intentos++ < MAX_INTENTOS);
			}
			Q = L"SELECT * FROM Medios WHERE (GrupoPath=\"" + Etiquetas[Rand]->Texto + L"\") OR (GrupoTag=\"" + Etiquetas[Rand]->Texto + L"\")";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Grupo (%s)\n", Etiquetas[Rand]->Texto.c_str());
			Mezclar = _Opciones_MezclarListaGrupo;
			break;
		case TLA_Disco:
			for (size_t i = 0; i < _Etiquetas.size(); i++) {
				if (_Etiquetas[i].EsDiscoPath() == TRUE || _Etiquetas[i].EsDiscoTag() == TRUE) Etiquetas.push_back(&_Etiquetas[i]);
			}
			if (Etiquetas.size() == 0) {
				App.MostrarToolTipPlayerError(L"No hay etiquetas con discos para generar una lista aleatória.");
				return FALSE;
			}
			// Si la etiqueta tiene menos de 5 medios vuelvo a obtener una etiqueta aleatória			
			if (App.BD.Opciones_NoGenerarListasMenos3() == TRUE) {
				do {
					Rand = App.Rand<size_t>(Etiquetas.size());
				} while (Etiquetas[Rand]->Medios < 5 && Intentos++ < MAX_INTENTOS);
			}
			Q = L"SELECT * FROM Medios WHERE (DiscoPath=\"" + Etiquetas[Rand]->Texto + L"\") OR (DiscoTag=\"" + Etiquetas[Rand]->Texto + L"\")";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Disco (%s)\n", Etiquetas[Rand]->Texto.c_str());
			Mezclar = _Opciones_MezclarListaDisco;
			break;
		case TLA_50Medios:
			Q = L"SELECT * FROM Medios WHERE TipoMedio=1 ORDER BY RANDOM() LIMIT 50";
			Debug_Escribir(L"RaveBD::GenerarListaAleatoria Tipo : 50 Canciones.\n");
			Mezclar = _Opciones_MezclarLista50Can;
			break;
		case TLA_Nota:
			Q = L"SELECT * FROM Medios WHERE TipoMedio=1 AND Nota > 2.5 ORDER BY Nota DESC LIMIT 50";
			Debug_Escribir(L"RaveBD::GenerarListaAleatoria Tipo : Nota.\n");
			Mezclar = _Opciones_MezclarListaNota;
			break;
	}

	
	int				    SqlRet = 0;
	sqlite3_stmt       *SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	int TotalAgregados = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDMedio TmpMedio;
			TmpMedio.ObtenerFila(SqlQuery, Unidades);
			if (App.BD.Opciones_NoAgregarMedioMenos25() == TRUE) {
				// si la nota es mas grande o igual que 2.5
				if (TmpMedio.Nota >= 2.5f) {
					if (GetFileAttributes(TmpMedio.Path.c_str()) != INVALID_FILE_ATTRIBUTES) {
						OUT_Medios.push_back(TmpMedio);
						TotalAgregados++;
					}
				}
			}
			else {
				if (GetFileAttributes(TmpMedio.Path.c_str()) != INVALID_FILE_ATTRIBUTES) {
					OUT_Medios.push_back(TmpMedio);
					TotalAgregados++;
				}
			}
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}

	sqlite3_finalize(SqlQuery);

	// Aplico el shufle
	if		(Mezclar == 1)	App.VentanaRave.BotonMezclar.Marcado(TRUE);  // Forzar mezclar
	else if (Mezclar == 2)	App.VentanaRave.BotonMezclar.Marcado(FALSE); // Forzar sin mezclar
	App.ControlesPC.BotonMezclar.Marcado(App.VentanaRave.BotonMezclar.Marcado());
	App.BD.Opciones_Shufle(App.VentanaRave.BotonMezclar.Marcado());

	// Miro si hay algun error en la consulta
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	// Muestro el tooltip en el player
	std::wstring ToolTip;
	switch (Tipo) {
		case TLA_Genero:	ToolTip = L"Lista aleatória por Genero \""	+ Etiquetas[Rand]->Texto + L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_Grupo:		ToolTip = L"Lista aleatória por Grupo \""	+ Etiquetas[Rand]->Texto + L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_Disco:		ToolTip = L"Lista aleatória por Disco \""	+ Etiquetas[Rand]->Texto + L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_50Medios:	ToolTip = L"Lista aleatória con "			+ std::to_wstring(TotalAgregados) + L" canciones generada.";										break;
		case TLA_Nota:		ToolTip = L"Lista aleatória por Nota con "  + std::to_wstring(TotalAgregados) + L" canciones generada.";										break;
	}
	App.MostrarToolTipPlayer(ToolTip);


	return (SqlRet != SQLITE_BUSY);
}


const BOOL RaveBD::_ModificarTablas(void) {
	float        Version = ObtenerVersionBD();
	int          SqlRet  = 0;
	std::wstring Q;

	if (Version < 1.1) {
		// Opciones ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Añado la columna EfectoFadeAudioMS en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN EfectoFadeAudioMS INTEGER AFTER OpacidadControlesVideo");
		// Añado la columna DlgDirectorios_Ancho en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN DlgDirectorios_Ancho INTEGER AFTER EfectoFadeAudioMS");
		// Añado la columna DlgDirectorios_Alto en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN DlgDirectorios_Alto INTEGER AFTER DlgDirectorios_Ancho");
		// Añado la columna VentanaMomentos_PosX en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN VentanaMomentos_PosX INTEGER AFTER DlgDirectorios_Alto");
		// Añado la columna VentanaMomentos_PosX en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN VentanaMomentos_PosY INTEGER AFTER VentanaMomentos_PosX");
		// Añado la columna OcultarTooltipsMouse en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN OcultarTooltipsMouse INTEGER AFTER VentanaMomentos_PosY");
		// Añado la columna MostrarMedioActualTitulo en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN MostrarMedioActualTitulo INTEGER AFTER OcultarTooltipsMouse");
		// Añado la columna MezclarListaGenero en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN MezclarListaGenero INTEGER AFTER MostrarMedioActualTitulo");
		// Añado la columna MezclarListaGrupo en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN MezclarListaGrupo INTEGER AFTER MezclarListaGenero");
		// Añado la columna MezclarListaDisco en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN MezclarListaDisco INTEGER AFTER MezclarListaGrupo");
		// Añado la columna MezclarLista50Can en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN MezclarLista50Can INTEGER AFTER MezclarListaDisco");
		// Añado la columna MezclarListaNota en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN MezclarListaNota INTEGER AFTER MezclarLista50Can");
		// Añado la columna MezclarListaNota en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones	ADD COLUMN GuardarBSCP INTEGER AFTER MezclarListaNota");
		// Modifico el valor de EfectoFadeAudioMS a 10000
		SqlRet = Consulta(L"UPDATE Opciones SET EfectoFadeAudioMS=10000, DlgDirectorios_Ancho=400, DlgDirectorios_Alto=600, VentanaMomentos_PosX=100, VentanaMomentos_PosY=100,"
			                                  " OcultarTooltipsMouse=0, MostrarMedioActualTitulo=1, MezclarListaGenero=0, MezclarListaGrupo=0, MezclarListaDisco=0, MezclarLista50Can=0,"
											  " MezclarListaNota=0, GuardarBSCP=1 WHERE Id=0");

		// Medios /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Añado la columna Proporcion en los medios
		SqlRet = Consulta(L"ALTER TABLE Medios ADD COLUMN Proporcion INTEGER AFTER PistaEleccion");
		// Añado la columna Brillo en los medios
		SqlRet = Consulta(L"ALTER TABLE Medios ADD COLUMN Brillo INTEGER AFTER Proporcion");
		// Añado la columna Contraste en los medios
		SqlRet = Consulta(L"ALTER TABLE Medios ADD COLUMN Contraste INTEGER AFTER Brillo");
		// Añado la columna Saturacion en los medios
		SqlRet = Consulta(L"ALTER TABLE Medios ADD COLUMN Saturacion INTEGER AFTER Contraste");
		// Actualizo los valores de proporcion, brillo, contraste, y saturación
		SqlRet = Consulta(L"UPDATE Medios SET Proporcion='', Brillo=1.0, Contraste=1.0, Saturacion=1.0");

		ActualizarHashs11();

		// TeclasRapidas ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Añado la tecla F1 a la tabla TeclasRapidas
		SqlRet = Consulta(L"INSERT INTO TeclasRapidas (Tecla, Control, Alt, Shift) VALUES(112, 0, 0, 0)");
		// Añado la tecla F2 a la tabla TeclasRapidas
		SqlRet = Consulta(L"INSERT INTO TeclasRapidas (Tecla, Control, Alt, Shift) VALUES(113, 0, 0, 0)");
		// Añado la tecla F3 a la tabla TeclasRapidas
		SqlRet = Consulta(L"INSERT INTO TeclasRapidas (Tecla, Control, Alt, Shift) VALUES(114, 0, 0, 0)");

		// Versión //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Actualizo la versión de la BD (MOVER ESTE UPDATE A LA ULTIMA VERSIÓN)
		Q = L"UPDATE Opciones SET Version=" RAVE_VERSIONBD L" WHERE Id=0";
		SqlRet = Consulta(Q);
		return TRUE;

	}

/*	if (Version < 1.2) {
		// Añado la columna EfectoFadeAudioMS en las opciones
		Q = L"ALTER TABLE Opciones ADD COLUMN ContadorIDSMomentos INTEGER AFTER DlgDirectorios_Alto";
		SqlRet = Consulta(Q);

		// Modifico el valor de EfectoFadeAudioMS a 10000
		Q = L"UPDATE Opciones SET ContadorIDSMomentos=0 WHERE Id=0";
		SqlRet = Consulta(Q);


		// Añado la columna IDMomentos en los medios
		Q = L"ALTER TABLE Medios ADD COLUMN IDMomentos INTEGER AFTER Saturacion";
		SqlRet = Consulta(Q);

		// Modifico el valor de IDMomentos a -1 (que significa que no hay momentos guardados para el medio)
		Q = L"UPDATE Medios SET IDMomentos=-1";
		SqlRet = Consulta(Q);



	}*/

	return FALSE;
}




const BOOL RaveBD::AgregarMomento(BDMedio* nMedio, std::wstring &nNombre, const UINT64 nTiempoInicial, const UINT64 nTiempoFinal, const BOOL EvitarReproduccion) {
	std::wstring Q = L"INSERT INTO Momentos (Nombre, TiempoInicio, TiempoFin, Excluir, IdMedio) VALUES(\"" +
											 nNombre								+ L"\"," + 
											 std::to_wstring(nTiempoInicial)		+ L"," + 
											 std::to_wstring(nTiempoFinal)			+ L"," + 
											 std::to_wstring(EvitarReproduccion)	+ L"," +
											 std::to_wstring(nMedio->Id)			+ L")";
	int SqlRet = Consulta(Q);

	if (SqlRet == SQLITE_DONE) {
		// Agrego el momento al vector de momentos del medio
		nMedio->Momentos.push_back(new BDMomento(App.BD.UltimaIdInsertada(), nMedio->Id, nNombre.c_str(), nTiempoInicial, nTiempoFinal, EvitarReproduccion));
		return TRUE;
	}

	return FALSE;
}


const BOOL RaveBD::ModificarMomento(BDMedio* nMedio, const LONG_PTR mID, std::wstring &nNombre, const UINT64 nTiempoInicial, const UINT64 nTiempoFinal, const BOOL EvitarReproduccion) {
	std::wstring Q = L"UPDATE Momentos SET Nombre=\"" + nNombre + L"\", TiempoInicio=" + std::to_wstring(nTiempoInicial) + L", TiempoFin=" + std::to_wstring(nTiempoFinal) + L", Excluir=" + std::to_wstring(EvitarReproduccion) + L" WHERE Id=" + std::to_wstring(mID);
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_DONE) {
		for (size_t i = 0; i < nMedio->Momentos.size(); i++) {
			if (nMedio->Momentos[i]->Id == mID) {
				nMedio->Momentos[i]->Nombre = nNombre;
				nMedio->Momentos[i]->TiempoInicio = nTiempoInicial;
				nMedio->Momentos[i]->TiempoFinal = nTiempoFinal;
				nMedio->Momentos[i]->Excluir = EvitarReproduccion;
				return TRUE;
			}
		}
		return TRUE;
	}
	return FALSE;
}


const BOOL RaveBD::EliminarMomento(BDMedio* nMedio, const LONG_PTR mID) {
	std::wstring Q = L"DELETE FROM Momentos WHERE Id=" + std::to_wstring(mID);
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_DONE) {
		for (size_t i = 0; i < nMedio->Momentos.size(); i++) {
			if (nMedio->Momentos[i]->Id == mID) {
				delete nMedio->Momentos[i];
				nMedio->Momentos.erase(nMedio->Momentos.begin() + static_cast<unsigned int>(i));
				return TRUE;
			}
		}
		return TRUE;
	}
	return FALSE;
}


/* PASOS / RECOMENDACIONES A TENER EN CUENTA A LA HORA DE AGREGAR NUEVAS TABLAS Y NUEVAS COLUMNAS A TABLAS EXISTENTES

	* Para nuevas tablas simplemente añade el código necesario al principio de la función _CrearTablas

	* Para modificar las columnas de una tabla existente, debes utilizar la función _ModificarTablas, subir la versión de RAVE_VERSIONBD,
	  y crear un if para la nueva versión en el que alteraremos las tablas, y rellenaremos las entradas que ya pudieram tener las tablas alteradas,
	  para asignar los valores por defecto de las nuevas columnas.

	  Por supuesto tambien tendras que añadir esas columnas en la función CrearTablas (por si se crea la base de datos desde 0)
*/
// Si hay que modificar alguna tabla, se tiene que hacer en la función _ModificarTablas, la cual mira la versión de las opciones 
const BOOL RaveBD::_CrearTablas(void) {

	std::wstring Q;
	// Creo la tabla para los Momentos ///////////////////////////////////////////////////
	std::wstring CrearTablaMomentos = L"CREATE TABLE Momentos ("
											L"Id" 				L" INTEGER PRIMARY KEY,"
											L"Nombre"			L" VARCHAR(260),"
											L"TiempoInicio"		L" INTEGER,"
											L"TiempoFin"		L" INTEGER,"
											L"Excluir"			L" INTEGER,"
											L"IdMedio"			L" INTEGER"
										")";
	if (Consulta(CrearTablaMomentos.c_str()) == SQLITE_ERROR) return FALSE;
	///////////////////////////////////////////////////////////////////////////////////////


	// Creo la tabla para las teclas rápidas ///////////////////////////////////////////////////
	std::wstring CrearTablaTeclasRapidas =	L"CREATE TABLE TeclasRapidas ("
												L"Tecla"	L" INTEGER,"
												L"Control"	L" TINYINT(1),"
												L"Alt"		L" TINYINT(1),"
												L"Shift"	L" TINYINT(1)"
											L")";
	if (Consulta(CrearTablaTeclasRapidas.c_str()) == SQLITE_ERROR) return FALSE;
	// Agrego los valores por defecto en la tabla de las teclas rápidas
	for (size_t i = 0; i < App.TeclasRapidas.size(); i++) {
		Q = L"INSERT INTO TeclasRapidas (Tecla, Control, Alt, Shift) "
			L"VALUES(" + std::to_wstring(App.TeclasRapidas[i].Tecla) + L"," + 
										 std::to_wstring(App.TeclasRapidas[i].Control)	+ L"," + 
										 std::to_wstring(App.TeclasRapidas[i].Alt)		+ L"," +
										 std::to_wstring(App.TeclasRapidas[i].Shift)	+ L")";
		if (Consulta(Q.c_str()) == SQLITE_ERROR) return FALSE;
	}
	////////////////////////////////////////////////////////////////////////////////////////////


	// Creo la tabla para guardar la ultima lista reproducida ////////////////////////
	std::wstring CrearTablaUltimaLista = L"CREATE TABLE UltimaLista (Hash BIGINT)";
	if (Consulta(CrearTablaUltimaLista.c_str()) == SQLITE_ERROR) return FALSE;
	////////////////////////////////////////////////////////////////////////////////////////////


	// Creo la tabla para las opciones /////////////////////////////////////////////////////////////
	std::wstring CrearTablaOpciones = L"CREATE TABLE Opciones ("
											L"Id" 						L" INTEGER PRIMARY KEY,"		// 00
											L"Volumen"					L" INTEGER,"					// 01
											L"PathAbrir"				L" VARCHAR(260),"				// 02 NO SE USA	  
											L"PosX"						L" INTEGER,"					// 03
											L"PosY"						L" INTEGER,"					// 04
											L"Ancho"					L" INTEGER,"					// 05
											L"Alto"						L" INTEGER,"					// 06
											L"Shufle"					L" INTEGER,"					// 07
											L"Repeat"					L" INTEGER,"					// 08
											L"Inicio"					L" INTEGER,"					// 09
											L"OcultarMouseEnVideo"		L" INTEGER,"					// 10
											L"Version"					L" DOUBLE,"						// 11
											L"MostrarObtenerMetadatos"	L" INTEGER,"					// 12
											L"MostrarAsociarArchivos"	L" INTEGER,"					// 13 NO SE USA
											L"AnalizarMediosPendientes" L" INTEGER,"					// 14
											L"VentanaOpciones_PosX"		L" INTEGER,"					// 15
											L"VentanaOpciones_PosY"		L" INTEGER,"					// 16
											L"VentanaAsociar_PosX"		L" INTEGER,"					// 17 Ahora es Opciones_DlgDirectorios_PosX
											L"VentanaAsociar_PosY"		L" INTEGER,"					// 18 Ahora es Opciones_DlgDirectorios_PosY
											L"VentanaAnalizar_PosX"		L" INTEGER,"					// 19
											L"VentanaAnalizar_PosY"		L" INTEGER,"					// 20
											L"BuscarActualizacion"		L" INTEGER,"					// 21
											L"TiempoAnimaciones"		L" INTEGER,"					// 22
											L"TiempoToolTips"			L" INTEGER,"					// 23
											L"NoAgregarMedioMenos25"	L" INTEGER,"					// 24
											L"NoGenerarListasMenos3"	L" INTEGER,"					// 25
											L"Sumar005"					L" INTEGER,"					// 26 Sumar 0.05 a la nota una vez finalizado el medio
											L"AlineacionControlesVideo"	L" INTEGER,"					// 27 Alineación para los controles del video
											L"OpacidadControlesVideo"	L" INTEGER,"					// 28 Opacidad másima para los controles del video (ControlesPantallaCompleta)
											L"EfectoFadeAudioMS"     	L" INTEGER,"					// 29 (NO SE USA DE MOMENTO, COMO MUCHO SE USARA SI IMPLEMENTO LA FMOD)
											L"DlgDirectorios_Ancho"   	L" INTEGER,"					// 30
											L"DlgDirectorios_Alto"   	L" INTEGER,"					// 31
											L"VentanaMomentos_PosX"   	L" INTEGER,"					// 32 Posición X de la ventana de los momentos
											L"VentanaMomentos_PosY"   	L" INTEGER,"					// 33 Posición Y de la ventana de los momentos
											L"OcultarTooltipsMouse"     L" INTEGER,"					// 34 Ocultar tooltips al pasar por encima con el mouse
											L"MostrarMedioActualTitulo" L" INTEGER,"					// 35 Mostrar el nombre del medio actual en la barra de titulo
											L"MezclarListaGenero"       L" INTEGER,"					// 36 Mezclar listas por genero
											L"MezclarListaGrupo"        L" INTEGER,"					// 37 Mezclar listas por grupo
											L"MezclarListaDisco"        L" INTEGER,"					// 38 Mezclar listas por disco
											L"MezclarLista50Can"        L" INTEGER,"					// 39 Mezclar listas con 50 canciones
											L"MezclarListaNota"         L" INTEGER,"					// 40 Mezclar listas por nota
											L"GuardarBSCP"              L" INTEGER"						// 41 guardar brillo, contraste, saturación, y proporción
									  L")";
	if (Consulta(CrearTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;
	//////////////////////////////////////////////////////////////////////////////////////////////////


	// Añado los datos por defecto de las opciones ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::wstring ValoresTablaOpciones = L"INSERT INTO Opciones (ID, Volumen, PathAbrir, PosX, PosY, Ancho, Alto, Shufle, Repeat, Inicio, OcultarMouseEnVideo, Version, MostrarObtenerMetadatos, MostrarAsociarArchivos, AnalizarMediosPendientes, VentanaOpciones_PosX, VentanaOpciones_PosY, VentanaAsociar_PosX, VentanaAsociar_PosY, VentanaAnalizar_PosX, VentanaAnalizar_PosY, BuscarActualizacion, TiempoAnimaciones, TiempoToolTips, NoAgregarMedioMenos25, NoGenerarListasMenos3, Sumar005, AlineacionControlesVideo, OpacidadControlesVideo, EfectoFadeAudioMS, DlgDirectorios_Ancho, DlgDirectorios_Alto, VentanaMomentos_PosX, VentanaMomentos_PosY, OcultarTooltipsMouse, MostrarMedioActualTitulo, MezclarListaGenero, MezclarListaGrupo, MezclarListaDisco, MezclarLista50Can, MezclarListaNota, GuardarBSCP) "
										L"VALUES(0, 100, \"C:\\\", 100, 100, 660, 400, 0, 0, 0, 3000," RAVE_VERSIONBD ", 1, 1, 1, 400, 300, 500, 400, 300, 200, 1, 400, 5500, 1, 1, 1, 0, 200, 10000, 350, 400, 100, 100, 0, 1, 0, 0, 0, 0, 0, 1)";
	if (Consulta(ValoresTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// Creo la tabla para las raices
	std::wstring CrearTablaRaiz = L"CREATE TABLE Raiz (Id INTEGER PRIMARY KEY, Path VARCHAR(260), IdDisco INTEGER)";
	if (Consulta(CrearTablaRaiz.c_str()) == SQLITE_ERROR) return FALSE;
	
	/*	Tipo				Nombre			Posición		Tipo
		---------------------------------------------------------------------- -
		UINT				Id				     0			INTEGER PRIMARY KEY
		sqlite_int64		Hash			     1			INT UNIQUE
		std::wstring		Path			     2			VARCHAR(260)
		std::wstring		NombrePath		     3			VARCHAR(128)
		Tipo_Medio			TipoMedio		     4			INT
		Extension_Medio		Extension		     5			INT
		UINT				Reproducido		     6			INT
		ULONG				Longitud		     7			INT
		DWORD				IDDisco			     8			INT						(NO SE USA)
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
										L"PistaEleccion "	L"TINYINT(1), "				// 24
										L"Proporcion "		L"VARCHAR(16), "			// 25
										L"Brillo "			L"DOUBLE, "					// 26
										L"Contraste "		L"DOUBLE, "					// 27
										L"Saturacion "		L"DOUBLE"					// 28
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
											L"Tiempo"			L" BIGINT,"				
											L"Longitud"			L" BIGINT"				
									   L")";

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
	
	// Recorto el path y substituyo la letra de unidad por un interrogante
	std::wstring		PathCortado = nPath;
	PathCortado[0] = L'?';
//	DWL::DUnidadDisco  *UnidadDisco = Unidades.Buscar_Letra(nPath[0]);


	size_t				PosNombre		= nPath.find_last_of(TEXT("\\"));																					// Posición donde empieza el nombre
	size_t				PosExtension	= nPath.find_last_of(TEXT("."));																					// Posición donde empieza la extensión
	Extension_Medio		Extension		= ExtensionesValidas::ObtenerExtension(nPath.substr(PosExtension + 1, (nPath.size() - PosExtension) - 1).c_str());	// Obtengo el tipo de extensión
	Tipo_Medio			Tipo			= ExtensionesValidas::ObtenerTipoMedio(Extension);																	// Obtengo el tipo de medio
	sqlite3_int64		Hash			= CrearHash(PathCortado);																							// Creo un hash partiendo del path y el número de serie del disco
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
//			OUT_Medio.Path[0]		= L'?';
			OUT_Medio.NombrePath	= NombreFinal;
			OUT_Medio.TipoMedio		= Tipo;
			OUT_Medio.Extension		= Extension;
			OUT_Medio.PistaPath		= Pista;
//			OUT_Medio.IDDisco		= UnidadDisco->Numero_Serie();
			OUT_Medio.Longitud      = Longitud;
			OUT_Medio.Nota          = 2;
			OUT_Medio.Tiempo        = 0;
			//SqlStr = L"INSERT INTO Medios (Hash, Path, NombrePath, TipoMedio, Extension, PistaPath, IDDisco, Longitud, Nota, Tiempo, Subtitulos, Parseado, DiscoPath, GrupoPath) VALUES(7931991700765854209,\"G:\\Pelis i Series\\...
			SqlStr = L"INSERT INTO Medios (Hash, Path, NombrePath, TipoMedio, Extension, PistaPath, Longitud, Nota, Tiempo, Subtitulos, Parseado, DiscoPath, GrupoPath, Brillo, Saturacion, Contraste)"
						  L" VALUES(" + std::to_wstring(Hash)							+ L",\"" +				// Hash
										PathCortado										+ L"\",\"" +			// Path
										NombreFinal										+ L"\", " +				// NombrePath
										std::to_wstring(Tipo)							+ L"," +				// Tipo
										std::to_wstring(Extension)						+ L"," +				// Extension
										std::to_wstring(Pista)							+ L"," +				// PistaPath
										//std::to_wstring(UnidadDisco->Numero_Serie())	+ L"," +				// ID Disco Duro
										std::to_wstring(Longitud)						+ L"," +				// Longitud en bytes
										L"2.5," +																// Nota
										L"0," +																	// Tiempo
										L"\"\"," +																// Subtitulos
										L"0,\"" +																// Parseado
										OUT_Medio.DiscoPath								+ L"\",\"" +			// DiscoPath
										OUT_Medio.GrupoPath								+ L"\"," +				// GrupoPath
										L"1.0," +                                                               // Brillo
										L"1.0," +																// Saturación
										L"1.0" +																// Contraste
									L")";
			SqlRet = Consulta(SqlStr);
			if (SqlRet == SQLITE_DONE) {
				return TRUE; // No existe el hash
			}
			else if (SqlRet == SQLITE_CONSTRAINT) {
				return 2; // Ya existe el hash
			}
			else {  /* Error ? */
				_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
				return FALSE;
			}
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


/* Función estatica para crear un hash a partir del path del medio */
/* NOTA se eliminan los dos primeros carácteres del path que son los de la unidad, ya que los medios extraibles pueden cambiar de letra. */
const sqlite3_int64 RaveBD::CrearHash(std::wstring &nPath) {
	//	std::wstring PathCortado = Path.substr(2, Path.size() - 2);
	std::hash<std::wstring> HashFunc;
	return HashFunc(nPath.substr(2, nPath.size() - 2));
}


const BOOL RaveBD::ObtenerTeclasRapidas(void) {
	std::wstring	Q			= L"SELECT * FROM TeclasRapidas";
	wchar_t		   *SqlError	= NULL;
	int				SqlRet		= 0;
	sqlite3_stmt   *SqlQuery	= NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	size_t R = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			App.TeclasRapidas[R].Tecla		= static_cast<int>(sqlite3_column_int(SqlQuery, 0));
			App.TeclasRapidas[R].Control	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 1));
			App.TeclasRapidas[R].Alt		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 2));
			App.TeclasRapidas[R++].Shift	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 3));
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

const BOOL RaveBD::GuardarTeclasRapidas(void) {
	// Botto todos los datos de la tabla TeclasRapidas sin borrar la tabla
	Consulta(L"DELETE FROM TeclasRapidas");
	std::wstring Q;
	int          SqlRet;
	for (size_t i = 0; i <  App.TeclasRapidas.size(); i++) {
		Q = L"INSERT INTO TeclasRapidas (Tecla, Control, Alt, Shift) VALUES(" + 
				std::to_wstring(App.TeclasRapidas[i].Tecla)		+ L"," +
				std::to_wstring(App.TeclasRapidas[i].Control)	+ L"," + 
				std::to_wstring(App.TeclasRapidas[i].Alt)		+ L"," +
				std::to_wstring(App.TeclasRapidas[i].Shift)		+
			L")";
		SqlRet = Consulta(Q);
		if (SqlRet != SQLITE_DONE) return FALSE;
	}
	return TRUE;
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
	for (LONG_PTR i = 0; i < App.VentanaRave.Lista.TotalItems(); i++) {
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
	if (Caracter >= L'0' && Caracter <= L'9')	return TRUE;
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
			// Espacio o caracteres por filtrar
			case L':': case L'_': case L',':  case L'.': case L'(': case L')': case L'~': case L'`':
			case L'!': case L'?': case L'¿':  case L'^': case L'"': case L'#': case L'$': case L'´':
			case L'%': case L'/': case L'\\': case L'-': case L'[': case L']': case L'*': case L' ':
				// Solo añado un espacio si no estamos al principio, o el ultimo caracter NO es un espacio
				if (Out.size() > 0) {
					if (Out[Out.size() - 1] != L' ') {
						Out += L' ';
					}
				}
				break;
			default:
				Out += static_cast<TCHAR>(tolower(In[i]));
				break;
		}
	}

	// Quito el ultimo espacio si existe
	if (Out.size() > 0) {
		if (Out[Out.size() - 1] == L' ') {
			Out.resize(Out.size() - 1);
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
}

void RaveBD::Opciones_MostrarAsociarArchivos(const BOOL nMostrarAsociarArchivos) {
	_Opciones_MostrarObtenerMetadatos = nMostrarAsociarArchivos;
	std::wstring Q = L"Update Opciones SET MostrarAsociarArchivos=" + std::to_wstring(nMostrarAsociarArchivos) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_AnalizarMediosPendientes(const BOOL nAnalizarMediosPendientes) {
	_Opciones_AnalizarMediosPendientes = nAnalizarMediosPendientes;
	std::wstring Q = L"Update Opciones SET AnalizarMediosPendientes=" + std::to_wstring(nAnalizarMediosPendientes) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_BuscarActualizacion(const BOOL nBuscarActualizacion) {
	_Opciones_BuscarActualizacion = nBuscarActualizacion;
	std::wstring Q = L"Update Opciones SET BuscarActualizacion=" + std::to_wstring(nBuscarActualizacion) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_TiempoAnimaciones(const UINT nTiempoAnimaciones) {
	_Opciones_TiempoAnimaciones = nTiempoAnimaciones;
	std::wstring Q = L"Update Opciones SET TiempoAnimaciones=" + std::to_wstring(nTiempoAnimaciones) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_TiempoToolTips(const UINT nTiempoToolTips) {
	_Opciones_TiempoToolTips = nTiempoToolTips;
	std::wstring Q = L"Update Opciones SET ToolTips=" + std::to_wstring(nTiempoToolTips) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
	Ret = Ret;
}

void RaveBD::Opciones_NoAgregarMedioMenos25(const BOOL nNoAgregarMedioMenos25) {
	_Opciones_NoAgregarMedioMenos25 = nNoAgregarMedioMenos25;
	std::wstring Q = L"Update Opciones SET NoAgregarMedioMenos25=" + std::to_wstring(nNoAgregarMedioMenos25) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_NoGenerarListasMenos3(const BOOL nNoGenerarListasMenos3) {
	_Opciones_NoGenerarListasMenos3 = nNoGenerarListasMenos3;
	std::wstring Q = L"Update Opciones SET NoGenerarListasMenos3=" + std::to_wstring(nNoGenerarListasMenos3) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_Sumar005(const BOOL nSumar005) {
	_Opciones_Sumar005 = nSumar005;
	std::wstring Q = L"Update Opciones SET Sumar005=" + std::to_wstring(nSumar005) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_AlineacionControlesVideo(const int nOpciones_AlineacionControlesVideo) {
	_Opciones_AlineacionControlesVideo = nOpciones_AlineacionControlesVideo;
	std::wstring Q = L"Update Opciones SET AlineacionControlesVideo=" + std::to_wstring(nOpciones_AlineacionControlesVideo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_OpacidadControlesVideo(const int nOpciones_OpacidadControlesVideo) {
	_Opciones_OpacidadControlesVideo = nOpciones_OpacidadControlesVideo;
	std::wstring Q = L"Update Opciones SET OpacidadControlesVideo=" + std::to_wstring(nOpciones_OpacidadControlesVideo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_EfectoFadeAudioMS(const UINT nOpciones_EfectoFadeAudioMS) {
	_Opciones_EfectoFadeAudioMS = nOpciones_EfectoFadeAudioMS;
	std::wstring Q = L"Update Opciones SET EfectoFadeAudioMS=" + std::to_wstring(nOpciones_EfectoFadeAudioMS) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_OcultarTooltipsMouse(const BOOL nOpciones_OcultarTooltipsMouse) {
	_Opciones_OcultarTooltipsMouse = nOpciones_OcultarTooltipsMouse;
	std::wstring Q = L"Update Opciones SET OcultarTooltipsMouse=" + std::to_wstring(nOpciones_OcultarTooltipsMouse) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_MostrarMedioActualTitulo(const BOOL nOpciones_MostrarMedioActualTitulo) {
	_Opciones_MostrarMedioActualTitulo = nOpciones_MostrarMedioActualTitulo;
	std::wstring Q = L"Update Opciones SET MostrarMedioActualTitulo=" + std::to_wstring(nOpciones_MostrarMedioActualTitulo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_MezclarListaGenero(const int nOpciones_MezclarListaGenero) {
	_Opciones_MezclarListaGenero = nOpciones_MezclarListaGenero;
	std::wstring Q = L"Update Opciones SET MezclarListaGenero=" + std::to_wstring(nOpciones_MezclarListaGenero) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_MezclarListaGrupo(const int nOpciones_MezclarListaGrupo) {
	_Opciones_MezclarListaGrupo = nOpciones_MezclarListaGrupo;
	std::wstring Q = L"Update Opciones SET MezclarListaGrupo=" + std::to_wstring(nOpciones_MezclarListaGrupo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_MezclarListaDisco(const int nOpciones_MezclarListaDisco) {
	_Opciones_MezclarListaDisco = nOpciones_MezclarListaDisco;
	std::wstring Q = L"Update Opciones SET MezclarListaDisco=" + std::to_wstring(nOpciones_MezclarListaDisco) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_MezclarLista50Can(const int nOpciones_MezclarLista50Can) {
	_Opciones_MezclarLista50Can = nOpciones_MezclarLista50Can;
	std::wstring Q = L"Update Opciones SET MezclarLista50Can=" + std::to_wstring(nOpciones_MezclarLista50Can) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_MezclarListaNota(const int nOpciones_MezclarListaNota) {
	_Opciones_MezclarListaNota = nOpciones_MezclarListaNota;
	std::wstring Q = L"Update Opciones SET MezclarListaNota=" + std::to_wstring(nOpciones_MezclarListaNota) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveBD::Opciones_GuardarBSCP(const BOOL nOpciones_GuardarBSCP) {
	_Opciones_GuardarBSCP = nOpciones_GuardarBSCP;
	std::wstring Q = L"Update Opciones SET GuardarBSCP=" + std::to_wstring(nOpciones_GuardarBSCP) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}



// Función que devuelve la versión de la BD
const float RaveBD::ObtenerVersionBD(void) {
	float			RetVersion = 0.0f;
	const wchar_t*	SqlStr = L"SELECT * FROM Opciones";
	wchar_t*		SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt*	SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			RetVersion = static_cast<float>(sqlite3_column_double(SqlQuery, 11));
		}


		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return 0.0f;
	}

	return (SqlRet != SQLITE_BUSY) ? RetVersion : 0.0f;
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
			_Opciones_DlgDirectorios_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 17));
			_Opciones_DlgDirectorios_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 18));
			_Opciones_VentanaAnalizar_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 19));
			_Opciones_VentanaAnalizar_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 20));
			_Opciones_BuscarActualizacion       = static_cast<BOOL>(sqlite3_column_int(SqlQuery, 21));
			_Opciones_TiempoAnimaciones         = static_cast<UINT>(sqlite3_column_int(SqlQuery, 22));
			DhWnd::TiempoAnimaciones = _Opciones_TiempoAnimaciones;
			_Opciones_TiempoToolTips            = static_cast<UINT>(sqlite3_column_int(SqlQuery, 23));
			_Opciones_NoAgregarMedioMenos25		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 24));
			_Opciones_NoGenerarListasMenos3		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 25));
			_Opciones_Sumar005					= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 26));
			_Opciones_AlineacionControlesVideo  = static_cast<int>(sqlite3_column_int(SqlQuery, 27));
			_Opciones_OpacidadControlesVideo	= static_cast<int>(sqlite3_column_int(SqlQuery, 28));
			_Opciones_EfectoFadeAudioMS			= static_cast<int>(sqlite3_column_int(SqlQuery, 29));
			_Opciones_DlgDirectorios_Ancho		= static_cast<int>(sqlite3_column_int(SqlQuery, 30));
			_Opciones_DlgDirectorios_Alto		= static_cast<int>(sqlite3_column_int(SqlQuery, 31));
			_Opciones_VentanaMomentos_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 32));
			_Opciones_VentanaMomentos_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 33));
			_Opciones_OcultarTooltipsMouse		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 34));
			_Opciones_MostrarMedioActualTitulo  = static_cast<BOOL>(sqlite3_column_int(SqlQuery, 35));
			_Opciones_MezclarListaGenero		= static_cast<int>(sqlite3_column_int(SqlQuery, 36));
			_Opciones_MezclarListaGrupo			= static_cast<int>(sqlite3_column_int(SqlQuery, 37));
			_Opciones_MezclarListaDisco			= static_cast<int>(sqlite3_column_int(SqlQuery, 38));
			_Opciones_MezclarLista50Can			= static_cast<int>(sqlite3_column_int(SqlQuery, 39));
			_Opciones_MezclarListaNota			= static_cast<int>(sqlite3_column_int(SqlQuery, 40));
			_Opciones_GuardarBSCP				= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 41));

			// Si hay alguna opcion de mezclar lista que no sea cero, establezco el shufle inicial a FALSE.
			if (_Opciones_MezclarListaGenero != 0 || _Opciones_MezclarListaGrupo != 0 || _Opciones_MezclarListaDisco != 0 || _Opciones_MezclarLista50Can != 0 || _Opciones_MezclarListaNota != 0) {				
				_Opciones_Shufle = FALSE;
			}
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
const BOOL RaveBD::AsignarTiempoMedio(const INT64 nTiempo, const sqlite3_int64 mHash) {
	int SqlRet = ConsultaVarg(L"UPDATE Medios SET Tiempo=%d WHERE Hash=%d", nTiempo, mHash);
	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}
	return TRUE;
}*/


/* NOTA es mejor tener 2 selects para las opciones, uno para el tamaño y posición de la ventana, y otro para el resto de valores (shufle, repeat, volumen, etc...)
Y no viene de 15 milisegundos mas a la hora de cerrar el reproductor */
/*const BOOL RaveBD::Opciones_GuardarOpciones(void) {
	// Compruebo si el repeat no es apagar windows o apagar el reproductor
	Tipo_Repeat Repeat = Tipo_Repeat_NADA;
	if (_Opciones_Repeat != Tipo_Repeat_ApagarReproductor && _Opciones_Repeat != Tipo_Repeat_ApagarOrdenador) {
		Repeat = _Opciones_Repeat;
	}
	Opciones_GuardarPosTamVentana();
	std::wstring Q = L"UPDATE Opciones SET "
		L"Volumen=" + std::to_wstring(_Opciones_Volumen) + L","
		L"Shufle=" + std::to_wstring(_Opciones_Shufle) + L","
		L"Repeat=" + std::to_wstring(Repeat) +
		L" WHERE Id=0";
	int SqlRet = Consulta(Q);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}*/

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


const BOOL RaveBD::Opciones_GuardarPosVentanaMomentos(void) {
	RECT RV;
	GetWindowRect(App.VentanaRave.Momentos.hWnd(), &RV);
	_Opciones_VentanaMomentos_PosX = RV.left;
	_Opciones_VentanaMomentos_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaMomentos_PosX=" + std::to_wstring(RV.left) + L", VentanaMomentos_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

/*const BOOL RaveBD::Opciones_GuardarPosVentanaAsociar(void) {
	RECT RV;
	GetWindowRect(App.VentanaAsociar.hWnd(), &RV);
	_Opciones_DlgDirectorios_PosX = RV.left;
	_Opciones_DlgDirectorios_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaAsociar_PosX=" + std::to_wstring(RV.left) + L", VentanaAsociar_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}*/

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

const BOOL RaveBD::Opciones_GuardarPosTamDlgDirectorios(RECT &RW) {
	_Opciones_DlgDirectorios_PosX  = RW.left;
	_Opciones_DlgDirectorios_PosY  = RW.top;
	_Opciones_DlgDirectorios_Ancho = abs(RW.right - RW.left);
	_Opciones_DlgDirectorios_Alto  = abs(RW.bottom - RW.top);

	std::wstring Q = L"UPDATE Opciones SET VentanaAsociar_PosX=" + std::to_wstring(RW.left) + L", VentanaAsociar_PosY=" + std::to_wstring(RW.top) + L", DlgDirectorios_Ancho=" + std::to_wstring(_Opciones_DlgDirectorios_Ancho) + L", DlgDirectorios_Ancho=" + std::to_wstring(_Opciones_DlgDirectorios_Alto) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}


// Suma 1 a las reproducciones del BDMedio, y Suma 0.05 a la Nota (si la opción está activada)
const BOOL RaveBD::MedioReproducido(BDMedio *rMedio) {
	if (rMedio == NULL) return FALSE;
	std::wstring Q;

	if (Opciones_Sumar005() == TRUE) {	
		float Nota = rMedio->Nota + 0.05f;
		// La nota no puede pasar de 5.0
		if (Nota > 5.0f) Nota = 5.0f;		
		Q = L"UPDATE Medios SET Reproducido=" + std::to_wstring(rMedio->Reproducido + 1) + L", Nota=" + DWL::Strings::ToStrF(Nota, 2) + L" WHERE Id=" + std::to_wstring(rMedio->Id);
	}
	else                             {
		Q = L"UPDATE Medios SET Reproducido=" + std::to_wstring(rMedio->Reproducido + 1) + L" WHERE Id=" + std::to_wstring(rMedio->Id);																
	}
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

// Actualiza la nota del ItemMedio especificado
const BOOL RaveBD::MedioNota(ItemMedio *nMedio, const float nNota) {
	std::wstring Q = L"UPDATE Medios SET Nota=" + DWL::Strings::ToStrF(nNota, 2) + L" WHERE Id=" + std::to_wstring(nMedio->Id);
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

// Actualiza la nota del ItemMedio especificado
const BOOL RaveBD::MedioNota(NodoBD *nMedio, const float nNota) {
	std::wstring Q = L"UPDATE Medios SET Nota=" + DWL::Strings::ToStrF(nNota, 2) + L" WHERE Id=" + std::to_wstring(nMedio->Id);
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveBD::ObtenerMediosPorRevisar(std::vector<BDMedio> &Medios) {
	const wchar_t  *Q = L"SELECT Id, NombrePath, Genero, GrupoPath, DiscoPath, NombreTag, GrupoTag, DiscoTag, Path, PistaTag, PistaPath, Nota, Tiempo, Longitud FROM Medios WHERE TipoMedio=1"; // Tipo_Medio_Audio
	
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	BDMedio         TmpMedio;

	SqlRet = sqlite3_prepare16_v2(_BD, Q, -1, &SqlQuery, NULL);
	if (SqlRet) {
		sqlite3_finalize(SqlQuery);
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
			TmpMedio.Tiempo				= static_cast<INT64>(sqlite3_column_int64(SqlQuery, 12));
			TmpMedio.Longitud			= static_cast<ULONG>(sqlite3_column_int64(SqlQuery, 13));
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

// Funcion que actualiza los hash de los medios a la versión 1.1 de la BD
// En la versión 1.0 los hash se basaban en el numero de serie del disco + el path del archivo
// Ahora en la versión 1.1 se prescinde de la id del disco y solo se utiliza el path del archivo para crear el hash
const BOOL RaveBD::ActualizarHashs11(void) {
	Debug_Escribir(L"RaveBD::ActualizarHashs11 Actualizando los hash de todos los medios a la versión 1.1 ...\n");

	std::wstring	Q			= L"SELECT Id, Path FROM Medios";
	int				SqlRet		= 0;
	int				SqlRetC		= 0;
	sqlite3_stmt   *SqlQuery	= NULL;
	sqlite3_int64	Hash		= 0;
	UINT            Id			= 0;
	int				VecesBusy	= 0;
	std::wstring    Path;

	// Realizar todo el trabajo en memória (es mucho mas rápido)
	Consulta(L"BEGIN TRANSACTION");

	// Ejecuto la consulta
	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	// Compruebo errores
	if (SqlRet) {
		sqlite3_finalize(SqlQuery);
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		Consulta(L"COMMIT TRANSACTION");
		return FALSE;
	}
	// Recorro la consulta
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			Path = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(SqlQuery, 1));
			Id   = static_cast<UINT>(sqlite3_column_int(SqlQuery, 0));
			SqlRetC = Consulta(L"UPDATE Medios SET Hash = " + std::to_wstring(CrearHash(Path)) + L" WHERE Id = " + std::to_wstring(Id));
		}
		// La base de datos parece estár bloqueada por otro proceso
		if (SqlRet == SQLITE_BUSY) {			
			if (++VecesBusy == 100) break;
		}
	}
	// Termino la consulta
	sqlite3_finalize(SqlQuery);

	// Vuelco el resultado al disco
	Consulta(L"COMMIT TRANSACTION");

	// Compruebo errores
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);
}

// Función que actualiza un hash especificado en el medio que corresponda con la Id especificada
/*const BOOL RaveBD::ActualizarHash(const UINT IdMedio, const sqlite3_int64 nHash) {
	
}*/


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


void RaveBD::CalcularDatosEtiqueta(EtiquetaBD *Etiqueta) {
	std::vector<std::wstring> Condiciones;

	if (Etiqueta->EsGenero() == TRUE) {
		Condiciones.push_back(L"(Genero =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsGrupoPath() == TRUE) {
		Condiciones.push_back(L"(GrupoPath =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsGrupoTag() == TRUE) {
		Condiciones.push_back(L"(GrupoTag =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsDiscoPath() == TRUE) {
		Condiciones.push_back(L"(DiscoPath =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsDiscoTag() == TRUE) {
		Condiciones.push_back(L"(DiscoTag =\"" + Etiqueta->Texto + L"\")");
	}


	std::wstring Q = L"SELECT Longitud, Tiempo, Nota FROM Medios WHERE ";

	for (size_t i = 0; i < Condiciones.size(); i++) {
		Q += Condiciones[i];
		if (i + 1 < Condiciones.size()) Q += L" OR ";
	}

	
	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	BDMedio         TmpMedio;

	Etiqueta->Longitud	= 0;
	Etiqueta->Medios	= 0;
	Etiqueta->Tiempo	= 0;
	Etiqueta->Nota		= 0.0f; // nota total de todos los medios, que dividida por los medios nos da la nota media

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return;
	}

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) { // La iD es el 0, pero no la necesito para nada.
			Etiqueta->Longitud	+= static_cast<DWORD>(sqlite3_column_int(SqlQuery, 0));
			Etiqueta->Tiempo	+= static_cast<INT64>(sqlite3_column_int(SqlQuery, 1));
			Etiqueta->Nota		+= static_cast<float>(sqlite3_column_double(SqlQuery, 2));
			Etiqueta->Medios	++;
		}
	}

	Etiqueta->Nota = Etiqueta->Nota / static_cast<float>(Etiqueta->Medios);

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return;
	}

}

void RaveBD::AsignarNotaEtiqueta(const float nNota, EtiquetaBD *Etiqueta) {

	std::vector<std::wstring> Condiciones;

	if (Etiqueta->EsGenero() == TRUE) {
		Condiciones.push_back(L"(Genero =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsGrupoPath() == TRUE) {
		Condiciones.push_back(L"(GrupoPath =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsGrupoTag() == TRUE) {
		Condiciones.push_back(L"(GrupoTag =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsDiscoPath() == TRUE) {
		Condiciones.push_back(L"(DiscoPath =\"" + Etiqueta->Texto + L"\")");
	}

	if (Etiqueta->EsDiscoTag() == TRUE) {
		Condiciones.push_back(L"(DiscoTag =\"" + Etiqueta->Texto + L"\")");
	}

//	std::wstring Q = L"UPDATE Medios SET Nota=" + DWL::Strings::ToStrF(nNota, 2) + L" WHERE Id=" + std::to_wstring(nMedio->Id);
	std::wstring Q = L"UPDATE Medios SET Nota=" + DWL::Strings::ToStrF(nNota, 2) + L" WHERE ";

	for (size_t i = 0; i < Condiciones.size(); i++) {
		Q += Condiciones[i];
		if (i + 1 < Condiciones.size()) Q += L" OR ";
	}

	int R = Consulta(Q);
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
			Etiqueta.Tiempo = static_cast<INT64>(sqlite3_column_int(SqlQuery, 5));
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


















