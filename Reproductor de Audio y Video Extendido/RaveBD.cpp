#include "stdafx.h"
#include "RaveBD.h"
#include "DDirectoriosWindows.h"
#include "DStringUtils.h"
#include "ListaMedios.h"

RaveBD::RaveBD(void) : RaveSQLite(), Historial_UltimaIDLista(0) {
}


// Crea / carga la base de datos
const BOOL RaveBD::Iniciar(void) {
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

	if (IniciarSQLite(PathBD.c_str()) == FALSE) {
		Debug_Escribir(L"RaveBD::Iniciar ERROR\n");
		return FALSE;
	}
	// Creo las tablas para la base de datos (si es necesario)
	if (_CrearTablas() == FALSE) {
		// Modifico las tablas de la BD si tienen una versión antigua
		_ModificarTablas();
	}

	// Obtengo las etiquetas
	ObtenerEtiquetas();

	Debug_Escribir(L"RaveBD::Iniciar\n");

	return TRUE;
}



// Obtiene el medio por el hash
const BOOL RaveBD::ObtenerMedio(const sqlite3_int64 mHash, BDMedio &OUT_Medio, DWL::DUnidadesDisco& Unidades) {
	std::wstring    SqlStr = L"SELECT * FROM Medios WHERE Hash =" + std::to_wstring(mHash);
	return _ConsultaObtenerMedio(SqlStr, OUT_Medio, Unidades);
}

// Obtiene el medio por el path
const BOOL RaveBD::ObtenerMedio(std::wstring &mPath, BDMedio &OUT_Medio, DWL::DUnidadesDisco& Unidades) {
	std::wstring TmpPath = mPath;
//	TmpPath[0] = L'?'; // Evito la letra de unidad (para raices cambiantes)
	std::wstring SqlStr = L"SELECT * FROM Medios WHERE Path like \"%" + TmpPath.substr(1) + L"\" COLLATE NOCASE"; // COLLATE NOCASE = Comparar strings case insensitive
	return _ConsultaObtenerMedio(SqlStr, OUT_Medio, Unidades);
}



const BOOL RaveBD::ObtenerMediosPorParsear(std::vector<std::wstring> &Paths, DUnidadesDisco &Unidades) {
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
			Medio.ObtenerFila(SqlQuery, Unidades, this);
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
const BOOL RaveBD::_ConsultaObtenerMedio(std::wstring &TxtConsulta, BDMedio &OUT_Medio, DWL::DUnidadesDisco& Unidades) {
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
			OUT_Medio.ObtenerFila(SqlQuery, Unidades, this);
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


// Obtiene la lista del historial según la Id especificada
Historial_Lista RaveBD::ObtenerHistorial_Lista(const INT64 Id) {
	Historial_Lista Ret;
	std::wstring Q = L"SELECT * FROM Historial_Lista WHERE Id = " + std::to_wstring(Id);

	int			    SqlRet = 0; 
	sqlite3_stmt   *SqlQuery = NULL;

	Ret.Id = Id;

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			Ret.Fecha  = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(SqlQuery, 1));
			Ret.Nombre = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(SqlQuery, 2));
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return Ret;
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

// Genera una lista partiendo de la sugerencia donde se ha hecho click
const BOOL RaveBD::GenerarListaDesdeSugerencias(DWL::DMenuEx &Menu, const TipoListaAleatoria nTipo) {
	std::wstring	Q;
	BOOL			Mezclar = FALSE;
	std::wstring    MT = Menu.Texto();
	size_t          Pos = std::wstring::npos;

	// Si texto del menú viene con parentesis (es un disco al que le he añadido el grupo al principio entre parentesis)
	Pos = MT.find_first_of(L')');
	if (Pos != std::wstring::npos && Pos + 2 < MT.size()) {
		MT = Menu.Texto().substr(Pos + 2);
	}

	// Genero la consulta según el tipo especificado
	switch (nTipo) {
		case TLA_Genero :
			Q = L"SELECT * FROM Medios WHERE Genero=\"" + MT + L"\" ORDER BY GrupoPath, DiscoPath, PistaPath, PistaTag ASC";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaDesdeSugerencias Tipo : Genero (%s)\n", MT.c_str());
			Mezclar = App.Opciones.MezclarListaGenero();
			break;
		case TLA_Grupo :
			Q = L"SELECT * FROM Medios WHERE (GrupoPath=\"" + MT + L"\") OR (GrupoTag=\"" + MT + L"\") ORDER BY DiscoPath, PistaPath, PistaTag ASC";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaDesdeSugerencias Tipo : Grupo (%s)\n", MT);
			Mezclar = App.Opciones.MezclarListaGrupo();
			break;
		case TLA_Disco :			
			Q = L"SELECT * FROM Medios WHERE (DiscoPath=\"" + MT + L"\") OR (DiscoTag=\"" + MT + L"\") ORDER BY PistaPath, PistaTag ASC";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaDesdeSugerencias Tipo : Disco (%s)\n", MT);
			Mezclar = App.Opciones.MezclarListaDisco();
			break;
	}

	// Ejecuto la consulta 
	int						SqlRet = 0;
	sqlite3_stmt           *SqlQuery = NULL;
	int						VecesBusy = 0;
	int						TotalAgregados = 0;
	std::vector<BDMedio>	OUT_Medios;

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDMedio TmpMedio;
			TmpMedio.ObtenerFila(SqlQuery, App.Unidades, this);
			if (App.Opciones.NoAgregarMedioMenos25() == TRUE) {
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

	// Asigno el estado de los botones de shufle
	if		(Mezclar == 1)	App.VentanaRave.BotonMezclar.Marcado(TRUE);  // Forzar mezclar
	else if (Mezclar == 2)	App.VentanaRave.BotonMezclar.Marcado(FALSE); // Forzar sin mezclar
	App.ControlesPC.BotonMezclar.Marcado(App.VentanaRave.BotonMezclar.Marcado());
	App.Opciones.Shufle(App.VentanaRave.BotonMezclar.Marcado());

	// Agrego los medios a la lista
	App.VentanaRave.Lista.BorrarListaReproduccion();
	for (size_t i = 0; i < OUT_Medios.size(); i++) {
		App.VentanaRave.Lista.AgregarMedio(&OUT_Medios[i]);
	}
	if (App.VentanaRave.BotonMezclar.Marcado() == TRUE) {
		App.VentanaRave.Lista.Mezclar(TRUE);
		App.VentanaRave.Lista.MedioActual = NULL;
	}
	App.VentanaRave.Lista.Repintar();



	// Miro si hay algun error en la consulta
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	// Muestro el tooltip en el player
	std::wstring ToolTip, ToolTip2, ToolTip3;
	switch (nTipo) {
		case TLA_Genero:	ToolTip3 = L"Sugerencia por "; ToolTip = L"Genero \"" + Menu.Texto() + L"\""; ToolTip2 = L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_Grupo:		ToolTip3 = L"Sugerencia por "; ToolTip = L"Grupo \"" + Menu.Texto() + L"\""; ToolTip2 = L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_Disco:		ToolTip3 = L"Sugerencia por "; ToolTip = L"Disco \"" + Menu.Texto() + L"\""; ToolTip2 = L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
	}
	// Asigno el nombre a la lista
	App.VentanaRave.Lista.Nombre = ToolTip;
	// Agrego la lista al historial
	Historial_Lista Historial(ToolTip);
	App.BD.GuardarHistorial_Lista(Historial);
	// Agrego la entrada del historial a la BD
	App.VentanaRave.Arbol_AgregarHistorial_Lista(Historial, FALSE);

	// Muestro el tooltip
	App.MostrarToolTipPlayer(ToolTip3 + ToolTip + ToolTip2);

	// Empiezo a reproducir la lista
	App.VentanaRave.Lista_Play();

	return (VecesBusy != 100) ? TRUE : FALSE;

}


// Genera una sugerencia del tipo especificado de etiqueta con 5 etiquetas aleatórias (SOLO SE PUEDE USAR CON LOS TIPOS GENERO, GRUPO, y DISCO)
const BOOL RaveBD::GenerarSugerenciasMenu(DWL::DMenuEx &Menu, const TipoListaAleatoria nTipo) {
	std::wstring Q;
	INT_PTR		 IDMenu  = 0;
	UINT         IDIcono = 0;
	std::wstring TDP = std::to_wstring(TIPO_DISCOPATH);
	switch (nTipo) {
		case TLA_Genero:
			Q = L"SELECT Texto FROM Etiquetas WHERE Tipo & " + std::to_wstring(TIPO_GENERO) + L" != 0 ORDER BY RANDOM() LIMIT 5";
			IDMenu = ID_MENUBOTONLISTA_GENERAR_GENERO_RND;
			IDIcono = IDI_GENERO;
			break;
		case TLA_Grupo:
			Q = L"SELECT Texto FROM Etiquetas WHERE Tipo & " + std::to_wstring(TIPO_GRUPOPATH) + L" != 0 OR Tipo & " + std::to_wstring(TIPO_GRUPOTAG) + L" != 0 ORDER BY RANDOM() LIMIT 5";
			IDMenu = ID_MENUBOTONLISTA_GENERAR_GRUPO_RND;
			IDIcono = IDI_GRUPO;
			break;
		case TLA_Disco:
			
			/* El tema es que vull afegir a :
			----------------------------------------------------			
				
				SELECT Etiquetas.Texto AS aTexto FROM Etiquetas
					WHERE (
						Etiquetas.Tipo & 4 != 0
						OR
						Etiquetas.Tipo & 10 != 0
					)
					ORDER BY RANDOM()
					LIMIT 5
				
			------------------------------------------------------
				els noms de grup de la primera coincindencia d'un medio amb els membres DiscoPath i DiscoTag (segons el DiscoEleccion
			*/
			// GrupoEleccion : 1 path, 0 tag
			Q = L"SELECT Texto FROM Etiquetas WHERE Tipo & " + std::to_wstring(TIPO_DISCOPATH) + L" != 0 AND Tipo & " + std::to_wstring(TIPO_DISCOTAG) + L" != 0 ORDER BY RANDOM() LIMIT 5";
			IDMenu = ID_MENUBOTONLISTA_GENERAR_DISCO_RND;
			IDIcono = IDI_DISCO;
			break;
		default : // Si el tipo no es válido salgo
			return FALSE;
	}

//	Menu.EliminarTodosLosMenus();

//	Debug_Escribir_Varg(L"RaveBD::GenerarSugerenciasMenu %d\n", nTipo);

	sqlite3 *BD = _BD;
	int TotalAgregados = 0;
	RaveSQLite_Consulta C(_BD, Q, [&BD, &nTipo, &Menu, &IDMenu, &IDIcono, &TotalAgregados](RaveSQLite_Consulta &This) {
		std::wstring Texto;
		std::wstring Etiqueta = This.ObtenerStr(0);
		// Si es un disco, ejecuto una segunda consulta para determinar su grupo
		if (nTipo == TLA_Disco) {
			std::wstring Q2 = L"SELECT GrupoPath, GrupoTag, GrupoEleccion FROM Medios WHERE (DiscoPath=\"" + Etiqueta + L"\" OR DiscoTag=\"" + Etiqueta + L"\") LIMIT 1";
			std::wstring GP, GT;
			int          El = 0;
			RaveSQLite_Consulta C2(BD, Q2, [&GP, &GT, &El, &Texto](RaveSQLite_Consulta& This) {
				GP = This.ObtenerStr(0);
				GT = This.ObtenerStr(1);
				El = This.ObtenerInt(2);
			});
			Texto = L"(";
			if (El == 0)	{ Texto += (GT.size() != 0) ? GT : GP; }	// Tag
			else			{ Texto += (GP.size() != 0) ? GP : GT; }	// Path
			Texto += L") ";
		}
		Texto += Etiqueta;
		Menu[TotalAgregados++].Texto(Texto, (TotalAgregados == 5) ? TRUE : FALSE);
//		Menu.AgregarMenu(IDMenu + TotalAgregados, Texto, IDIcono);
//		TotalAgregados++;
//		Debug_Escribir_Varg(L"%s\n", Texto.c_str());

	});
	return C.Resultado();
}

// Función que genera una lista aleatória por el tipo especificado, y la guarda en la variable OUT_Medios (DEBE EJECUTARSE DESDE EL THREAD PRINCIPAL)
const BOOL RaveBD::GenerarListaAleatoria(std::vector<BDMedio> &OUT_Medios, DWL::DUnidadesDisco& Unidades, const TipoListaAleatoria nTipo) {
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
			if (App.Opciones.NoGenerarListasMenos3() == TRUE) {
				do {
					Rand = App.Rand<size_t>(Etiquetas.size());
				} while (Etiquetas[Rand]->Medios < 5 && Intentos++ < MAX_INTENTOS);
			}
			Q = L"SELECT * FROM Medios WHERE Genero=\"" + Etiquetas[Rand]->Texto + L"\" ORDER BY GrupoPath, DiscoPath, PistaPath, PistaTag ASC";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Genero (%s)\n", Etiquetas[Rand]->Texto.c_str());
			Mezclar = App.Opciones.MezclarListaGenero();
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
			if (App.Opciones.NoGenerarListasMenos3() == TRUE) {
				do {
					Rand = App.Rand<size_t>(Etiquetas.size());
				} while (Etiquetas[Rand]->Medios < 5 && Intentos++ < MAX_INTENTOS);
			}
			Q = L"SELECT * FROM Medios WHERE (GrupoPath=\"" + Etiquetas[Rand]->Texto + L"\") OR (GrupoTag=\"" + Etiquetas[Rand]->Texto + L"\") ORDER BY DiscoPath, PistaPath, PistaTag ASC";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Grupo (%s)\n", Etiquetas[Rand]->Texto.c_str());
			Mezclar = App.Opciones.MezclarListaGrupo();
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
			if (App.Opciones.NoGenerarListasMenos3() == TRUE) {
				do {
					Rand = App.Rand<size_t>(Etiquetas.size());
				} while (Etiquetas[Rand]->Medios < 5 && Intentos++ < MAX_INTENTOS);
			}
			Q = L"SELECT * FROM Medios WHERE (DiscoPath=\"" + Etiquetas[Rand]->Texto + L"\") OR (DiscoTag=\"" + Etiquetas[Rand]->Texto + L"\") ORDER BY PistaPath, PistaTag ASC";
			Debug_Escribir_Varg(L"RaveBD::GenerarListaAleatoria Tipo : Disco (%s)\n", Etiquetas[Rand]->Texto.c_str());
			Mezclar = App.Opciones.MezclarListaDisco();
			break;
		case TLA_50Medios:
			Q = L"SELECT * FROM Medios WHERE TipoMedio=1 ORDER BY RANDOM() LIMIT 50";
			Debug_Escribir(L"RaveBD::GenerarListaAleatoria Tipo : 50 Canciones.\n");
			Mezclar = App.Opciones.MezclarLista50Can();
			break;
		case TLA_Nota:
			Q = L"SELECT * FROM Medios WHERE TipoMedio=1 AND Nota > 2.5 ORDER BY Nota DESC LIMIT 50";
			Debug_Escribir(L"RaveBD::GenerarListaAleatoria Tipo : Nota.\n");
			Mezclar = App.Opciones.MezclarListaNota();
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
			TmpMedio.ObtenerFila(SqlQuery, Unidades, this);
			if (App.Opciones.NoAgregarMedioMenos25() == TRUE) {
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
	App.Opciones.Shufle(App.VentanaRave.BotonMezclar.Marcado());

	// Miro si hay algun error en la consulta
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	// Muestro el tooltip en el player
	std::wstring ToolTip, ToolTip2, ToolTip3;
	switch (Tipo) {
		case TLA_Genero:	ToolTip3 = L"Lista aleatória por "; ToolTip = L"Genero \"" + Etiquetas[Rand]->Texto + L"\""; ToolTip2 = L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_Grupo:		ToolTip3 = L"Lista aleatória por "; ToolTip = L"Grupo \""  + Etiquetas[Rand]->Texto + L"\""; ToolTip2 = L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_Disco:		ToolTip3 = L"Lista aleatória por "; ToolTip = L"Disco \""  + Etiquetas[Rand]->Texto + L"\""; ToolTip2 = L"\" generada con " + std::to_wstring(TotalAgregados) + L" canciones.";	break;
		case TLA_50Medios:	ToolTip3 = L""; ToolTip = L"Lista aleatória"; ToolTip2 =L" con " + std::to_wstring(TotalAgregados) + L" canciones generada.";													break;
		case TLA_Nota:		ToolTip3 = L""; ToolTip = L"Lista por Nota";  ToolTip2 = L" con " + std::to_wstring(TotalAgregados) + L" canciones generada.";													break;
	}
	// Asigno el nombre a la lista
	App.VentanaRave.Lista.Nombre = ToolTip;
	// Agrego la lista al historial
	Historial_Lista Historial(ToolTip);
	App.BD.GuardarHistorial_Lista(Historial);
	// Agrego la entrada del historial a la BD
	App.VentanaRave.Arbol_AgregarHistorial_Lista(Historial, FALSE);

	// Muestro el tooltip
	App.MostrarToolTipPlayer(ToolTip3 + ToolTip + ToolTip2);

	return (SqlRet != SQLITE_BUSY);
}


const BOOL RaveBD::_ModificarTablas(void) {
	int          Version = App.Opciones.ObtenerVersionBD();
	int          SqlRet  = 0;
	BOOL         Ret     = FALSE;
	std::wstring Q;

	if (Version < 11) {
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

		Ret = TRUE;
	}
	// En la versión 12 se modifica el tamaño para los paths, y hay que crear una tabla nueva, copiar la vieja, borrar la vieja y renombrar la nueva
	if (Version < 12) {
		// Creo una nueva tabla para los medios con el tamaño del path corregido
		if (_CrearTablaMedios(L"Medios2") == TRUE) {
			// Establezco una transacción en memória
			SqlRet = Consulta(L"BEGIN TRANSACTION");
			// Copio los datos de la tabla Medios a la tabla Medios2
			SqlRet = Consulta(L"INSERT INTO Medios2 SELECT * FROM Medios");
			// Borro la tabla medios
			SqlRet = Consulta(L"DROP TABLE Medios");
			// Renombro la tabla Medios2 a Medios
			SqlRet = Consulta(L"ALTER TABLE Medios2 RENAME TO Medios");
			// Termino la transacción en memória
			SqlRet = Consulta(L"COMMIT TRANSACTION");

			// Versión //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Actualizo la versión de la BD (MOVER ESTE UPDATE A LA ULTIMA VERSIÓN)			
			SqlRet = App.Opciones.Consulta(L"UPDATE Opciones SET Version=" RAVE_VERSIONBD L" WHERE Id=0");

			Ret = TRUE;
		}		
	}

	return Ret;
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
//	std::wstring Q;
	// Creo la tabla para guardar las listas del historial
	std::wstring CrearTablaHistorialLista =	L"CREATE TABLE Historial_Lista ("
											L"Id"				L" INTEGER PRIMARY KEY,"
											L"Fecha"			L" TEXT,"
											L"Nombre"			L" TEXT"		
										L")";
	if (Consulta(CrearTablaHistorialLista.c_str()) == SQLITE_ERROR)
		return FALSE;

	// Creo la tabla para guardar los medios del historial
	std::wstring CrearTablaHistorial =	L"CREATE TABLE Historial_Medio ("
											//L"Id"				L" INTEGER PRIMARY KEY,"
											L"IdMedio"			L" INTEGER,"
											L"IdLista"			L" INTEGER,"
											L"Fecha"			L" TEXT,"
											L"FOREIGN KEY(IdMedio) REFERENCES Medios(Id),"
											L"FOREIGN KEY(IdLista) REFERENCES Historial_Lista(Id)"
										L")";
	if (Consulta(CrearTablaHistorial.c_str()) == SQLITE_ERROR) 
		return FALSE;

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

	

	// Creo la tabla para guardar la ultima lista reproducida ////////////////////////
//	std::wstring CrearTablaUltimaLista = L"CREATE TABLE UltimaLista (Hash BIGINT)";
//	if (Consulta(CrearTablaUltimaLista.c_str()) == SQLITE_ERROR) return FALSE;
	////////////////////////////////////////////////////////////////////////////////////////////



	// Creo la tabla para las raices
//	std::wstring CrearTablaRaiz = L"CREATE TABLE Raiz (Id INTEGER PRIMARY KEY, Path VARCHAR(260), IdDisco INTEGER)";
//	if (Consulta(CrearTablaRaiz.c_str()) == SQLITE_ERROR) return FALSE;
	
	// Creo la tabla para los medios
	_CrearTablaMedios(L"Medios");

	// Creo la tabla para guardar la ultima lista reproducida con referencias //////////////////
	std::wstring CrearTablaUltimaLista = L"CREATE TABLE UltimaLista2 (Id INTEGER, FOREIGN KEY(Id) REFERENCES Medios(Id))";
	if (Consulta(CrearTablaUltimaLista.c_str()) == SQLITE_ERROR)
		return FALSE;
	////////////////////////////////////////////////////////////////////////////////////////////


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


const BOOL RaveBD::_CrearTablaMedios(const wchar_t *Nombre) {
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
	std::wstring StrNombre = Nombre;
	// Creo la tabla para los medios
	std::wstring CrearTablaMedios =	L"CREATE TABLE " + StrNombre +  L" ("
										L"Id "				L"INTEGER PRIMARY KEY, "	//  0
 										L"Hash "			L"BIGINT UNIQUE, "			//  1
										L"Path "			L"VARCHAR(1024), "			//  2
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

	return TRUE;
}


// Pre-Analiza el medio y lo inserta en la BD
// Devuelve FALSE si ahy un error
// Devuelve TRUE si se ha añadido el medio (no existia anteriormente)
// Devuelve 2 si el medio ya existia en la BD
const BOOL RaveBD::AnalizarMedio(std::wstring &nPath, BDMedio &OUT_Medio, DWL::DUnidadesDisco& Unidades, const ULONG Longitud, const UINT TiempoEnSecs, const wchar_t* NombreDesdeM3u) {
	Ubicacion_Medio		Ubicacion = BDMedio::Ubicacion(nPath);

	// Asigno el path, por si no se encuentra el archivo, que el tooltip pueda decir el path que no encuentra
	OUT_Medio.Path = nPath;

	// Compruebo que existe fisicamente en el disco (comprobar velocidades usando CreateFile)
	if (Ubicacion != Ubicacion_Medio_Internet) {
		if (GetFileAttributes(nPath.c_str()) == INVALID_FILE_ATTRIBUTES)
			return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Obtenir temps d'un mp3 en milisegons																							//
	// http://stackoverflow.com/questions/3505575/how-can-i-get-the-duration-of-an-mp3-file-cbr-or-vbr-with-a-very-small-library	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::wstring		PathCortado = nPath;
	wchar_t             UltimaLetra = PathCortado[0];
	// Si es un medio local
/*	if (Ubicacion == Ubicacion_Medio_Local) {
		// Substituyo la letra de unidad por un interrogante
		PathCortado[0] = L'?';
	}*/

	// Busco el caracter ?, y lo elimino junto a todo el texto que tenga detrás
//	size_t PosDelimitadorPHP = PathCortado.find(L'?');
//	if ( ==)

	// Asigno el delimitador del nombre (por defecto es '\\', pero si es un medio de internet es '/')
	wchar_t				DelimitadorNombre	= (Ubicacion != Ubicacion_Medio_Internet)  ? L'\\' : L'/';

//	wchar_t             DelimitadorPHP      = L'?';

	size_t				PosNombre			= nPath.find_last_of(DelimitadorNombre);																			// Posición donde empieza el nombre
	size_t				PosExtension		= nPath.find_last_of(L'.');																							// Posición donde empieza la extensión
	Extension_Medio		Extension			= ExtensionesValidas::ObtenerExtension(nPath.substr(PosExtension + 1, (nPath.size() - PosExtension) - 1).c_str());	// Obtengo el tipo de extensión
	Tipo_Medio			Tipo				= ExtensionesValidas::ObtenerTipoMedio(Extension);																	// Obtengo el tipo de medio
	sqlite3_int64		Hash				= CrearHash(PathCortado);																							// Creo un hash partiendo del path y el número de serie del disco
	UINT				Pista				= 0;																												// Número de pista
	int                 SqlRet = 0;
	std::wstring		TmpNombre;
	std::wstring		NombreFinal;
	std::wstring        SqlStr;

	Strings::Split PathSeparado(nPath, DelimitadorNombre);

	switch (Tipo) {
		case Tipo_Medio_INDEFINIDO:
			// Los tipos de medio indefinido locales se ignoran
			if (Ubicacion != Ubicacion_Medio_Internet)	return FALSE;
			break;
		case Tipo_Medio_Audio:
			// Obtengo el nombre del disco
			if (PathSeparado.Total() > 1) {
				FiltroNombre(PathSeparado[PathSeparado.Total() - 2], OUT_Medio.DiscoPath);
			}
			// Obtengo el nombre del grupo
			if (PathSeparado.Total() > 2) {
				FiltroNombre(PathSeparado[PathSeparado.Total() - 3], OUT_Medio.GrupoPath);
			}
			break;
//		case Tipo_Medio_Video:
//			break;
//		case Tipo_Medio_CDAudio:
//			break;
		case Tipo_Medio_Lista:
			// Excluyo la lista m3u del análisis
			OUT_Medio.Parseado = TRUE;
			// Es una URL a un m3u (provablemente a una IPTV)
			if (Ubicacion == Ubicacion_Medio_Internet) {
				Tipo = Tipo_Medio_IpTv;
			}
			break;
	}


	// Analizo el nombre para obtener la pista (si existe)
	AnalizarNombre(nPath.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, Pista);
	
	// Filtro de carácteres para el nombre
	FiltroNombre(TmpNombre, NombreFinal);

	// Relleno los valores del medio
	OUT_Medio.Hash			= Hash;
	OUT_Medio.Path			= PathCortado;
	OUT_Medio.NombrePath	= (NombreDesdeM3u == nullptr) ? NombreFinal : NombreDesdeM3u;	// Si se ha especificado un nombre desde el M3u pongo el nombre del M3u
	OUT_Medio.TipoMedio		= Tipo;
	OUT_Medio.Extension		= Extension;
	OUT_Medio.PistaPath		= Pista;
	OUT_Medio.Longitud      = Longitud;
	OUT_Medio.Tiempo		= (INT64)1000 * (INT64)TiempoEnSecs;

	// Si es un medio de internet
	if (OUT_Medio.Ubicacion() == Ubicacion_Medio_Internet) {
		// Excluyo el medio del análisis
		OUT_Medio.Parseado = TRUE;
	}

	
	DWORD IDDisco = 0;
	// Busco la unidad por la letra especificada en el path
	DWL::DUnidadDisco *Unidad = Unidades.Buscar_Letra(nPath[0]);
	if (Unidad != nullptr) {
		// Guardo el numero de serie del disco, para tener una referéncia en caso de multiples ubicaciones del mismo medio.
		IDDisco = Unidad->Numero_Serie();
	}

	// Creo el string para insertar el medio
	SqlStr = L"INSERT INTO Medios (Hash, Path, NombrePath, TipoMedio, Extension, PistaPath, Longitud, Nota, Tiempo, Subtitulos, Parseado, DiscoPath, GrupoPath, Brillo, Saturacion, Contraste)"
				L" VALUES(" +	std::to_wstring(OUT_Medio.Hash)					+ L",\"" +				// Hash
								OUT_Medio.Path									+ L"\",\"" +			// Path
								OUT_Medio.NombrePath							+ L"\", " +				// NombrePath
								std::to_wstring(OUT_Medio.TipoMedio)			+ L"," +				// Tipo
								std::to_wstring(OUT_Medio.Extension)			+ L"," +				// Extension
								std::to_wstring(OUT_Medio.PistaPath)			+ L"," +				// PistaPath
//								std::to_wstring(IDDisco)						+ L"," +				// ID Disco Duro (a malas puede ser 0)
								std::to_wstring(OUT_Medio.Longitud)				+ L"," +				// Longitud en bytes
								L"2.5," +																// Nota
								std::to_wstring(OUT_Medio.Tiempo)				+ L"," +				// Tiempo
								L"\"\"," +																// Subtitulos
								std::to_wstring(OUT_Medio.Parseado)				+ L",\"" +				// Parseado
								OUT_Medio.DiscoPath								+ L"\",\"" +			// DiscoPath
								OUT_Medio.GrupoPath								+ L"\"," +				// GrupoPath
								L"1.0," +                                                               // Brillo
								L"1.0," +																// Saturación
								L"1.0" +																// Contraste
							L")";
	// Ejecuto la consulta
	SqlRet = Consulta(SqlStr);

	// Restauro la letra de unidad con la que venia
	OUT_Medio.Path[0] = UltimaLetra;


	// Se ha agregado
	if (SqlRet == SQLITE_DONE) {
		return TRUE; // No existe el hash
	}
	// No se ha agregado por que ya existe
	else if (SqlRet == SQLITE_CONSTRAINT) {
		return 2; // Ya existe el hash
	}
	// Error al agregar
	else {  
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}


	return TRUE;
}


/* Función estatica para crear un hash a partir del path del medio */
/* NOTA se eliminan los dos primeros carácteres del path que son los de la unidad, ya que los medios extraibles pueden cambiar de letra. */
const sqlite3_int64 RaveBD::CrearHash(std::wstring &nPath) {
	//	std::wstring PathCortado = Path.substr(2, Path.size() - 2);
	std::hash<std::wstring> HashFunc;
	return HashFunc(nPath.substr(2, nPath.size() - 2));
}


const BOOL RaveBD::ObtenerUltimaLista(DWL::DUnidadesDisco &Unidades) {

	// Elimino los medios de la lista
	App.VentanaRave.Lista.BorrarListaReproduccion();

	const wchar_t  *SqlStr		= L"SELECT * FROM Medios, UltimaLista2 WHERE Medios.Id == UltimaLista2.Id";
	wchar_t*		SqlError	= NULL;
	int				SqlRet		= 0;
	sqlite3_stmt   *SqlQuery	= NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDMedio Medio;
			// Obtengo los datos del medio
			Medio.ObtenerFila(SqlQuery, Unidades, this);
//			ObtenerMedio(sqlite3_column_int64(SqlQuery, 0), Medio, Unidades);
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
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}


	return (SqlRet != SQLITE_BUSY);




}

const BOOL RaveBD::GuardarUltimaLista(void) {
	// Botto todos los datos de la tabla UltimaLista sin borrar la tabla
/*	Consulta(L"DELETE FROM UltimaLista");
	std::wstring Q;
	int          SqlRet;
	for (LONG_PTR i = 0; i < App.VentanaRave.Lista.TotalItems(); i++) {
		Q = L"INSERT INTO UltimaLista (Id) VALUES(" + std::to_wstring(App.VentanaRave.Lista.Medio(i)->BdMedio.Hash) + L")";
		SqlRet = Consulta(Q);
		if (SqlRet != SQLITE_DONE) return FALSE;
	}
	return TRUE;*/

	// Botto todos los datos de la tabla UltimaLista sin borrar la tabla
	Consulta(L"DELETE FROM UltimaLista2");
	std::wstring Q;
	int          SqlRet;
	for (LONG_PTR i = 0; i < App.VentanaRave.Lista.TotalItems(); i++) {
		Q = L"INSERT INTO UltimaLista2 (Id) VALUES(" + std::to_wstring(App.VentanaRave.Lista.Medio(i)->BdMedio.Id) + L")";
		SqlRet = Consulta(Q);
		if (SqlRet != SQLITE_DONE)
			return FALSE;
	}
	return TRUE;

}

// Guarda los datos de una lista en el historial
const BOOL RaveBD::GuardarHistorial_Lista(Historial_Lista &HL) {
	std::wstring Fecha;
	int          SqlRet = 0;
	HL.Fecha.Texto(Fecha);
	std::wstring Q = L"INSERT INTO Historial_Lista (Nombre, Fecha) VALUES('" + HL.Nombre + L"', '" + Fecha + L"')";
	SqlRet = Consulta(Q);
	if (SqlRet != SQLITE_DONE)
		return FALSE;
	Historial_UltimaIDLista = UltimaIdInsertada();
	HL.Id = Historial_UltimaIDLista;
	return TRUE;
}

// Guarda los datos de un medio en el historial
const BOOL RaveBD::GuardarHistorial_Medio(Historial_Medio &HM) {
	std::wstring Fecha;
	int          SqlRet = 0;
	HM.Fecha.Texto(Fecha);
	std::wstring Q = L"INSERT INTO Historial_Medio (IdLista, IdMedio, Fecha) VALUES(" + std::to_wstring(HM.IdLista) + L", " + std::to_wstring(HM.IdMedio) + L", '" + Fecha + L"')";
	SqlRet = Consulta(Q);
	if (SqlRet != SQLITE_DONE)
		return FALSE;
	return TRUE;
}

/* Función que analiza el string para buscar posibles numeros de pista, para ello separa el string por carácteres y analiza cada sub-string por separado
	Una vez identificados los sub-strings mira si hay alguno de ellos que pueda ser la pista, y en caso de haber mas de uno se usará el que sea más explicito.
	Pos ejemplo podemos encontrar : "1999 Grupo X - Disco Z - 01x01 Cancion"
	- En este caso hay 2 substrings que podrian ser una pista, pero por defecto las pistas de 4 digitos suelen indicar el año y no el número de pista, y por esa razón se elegirá 01x01 como pista, ya que es mucho mas explicita.
 */
const BOOL RaveBD::AnalizarNombre(std::wstring &Analisis, std::wstring &nNombre, UINT &nPista) {
	// Separo el string por espacios, simbolos '-' y barras inferiores '_'
	// No incluyo el punto '.' como separador, ya que es importante para detectar URLS en los nombres
	DWL::Strings::Split Sp(Analisis, L" _-");
	// Creo un vector de tipos para cada parte del string
	std::vector<Tipo_ParteNombre> Tipos;
	for (size_t i = 0; i < Sp.Total(); i++) Tipos.push_back(Tipo_ParteNombre_Texto);

	// Analizo cada trozo del string, lo primero que busco son sub-strings con 5 carácteres de máximo, ya que estos pueden ser la pista.
	// Los sub-strings con más de 5 carácteres quedan descartados i solo son analizados por si contienen una URL (para su eliminación)
	UINT TmpNum = 0;
	for (size_t i = 0; i < Sp.Total(); i++) {
		if (Sp[i].size() < 6) {
			switch (Sp[i].size()) {
				case 1: // Pista de un caracter (MUY POCO FIABLE)
					if		(_EsNumero(Sp[i][0]) == TRUE)																						Tipos[i] = Tipo_ParteNombre_Pista1;
					break;
				case 2 : // Pista con dos carácteres, puede ser 2 digitos o un digito seguido de un punto
					if		(_EsNumero(Sp[i][0]) == TRUE && _EsNumero(Sp[i][1]) == TRUE)														Tipos[i] = Tipo_ParteNombre_Pista2;
					else if	(_EsNumero(Sp[i][0]) == TRUE && Sp[i][1] == L'.')																	Tipos[i] = Tipo_ParteNombre_Pista1p;
					break;
				case 3: // Pista con tres carácteres puede ser 3 digitos, o 1x1, 0 01.
					if		(_EsNumero(Sp[i][0]) == TRUE && _EsNumero(Sp[i][1]) == TRUE && _EsNumero(Sp[i][2]) == TRUE)							Tipos[i] = Tipo_ParteNombre_Pista3;
					else if (_EsNumero(Sp[i][0]) == TRUE && Sp[i][1] == L'x' && _EsNumero(Sp[i][2]) == TRUE)									Tipos[i] = Tipo_ParteNombre_Pista1x1;
					else if (_EsNumero(Sp[i][0]) == TRUE && _EsNumero(Sp[i][1]) == TRUE && Sp[i][2] == L'.' )									Tipos[i] = Tipo_ParteNombre_Pista2p;
					break;
				case 4: // Pista con cuatro carácteres hay 3 posibilidades : 1x01, 1-01, y 1999 (si es un número de 4 digitos es MUY provable que sea el año, solo elegir como pista si el medio es una canción)
					TmpNum = _wtoi(Sp[i].c_str());
					// Los 4 carácteres son digitos
					if (_EsNumero(Sp[i][0]) == TRUE && _EsNumero(Sp[i][1]) == TRUE && _EsNumero(Sp[i][2]) == TRUE && _EsNumero(Sp[i][3]) == TRUE) {
						if (TmpNum > 1900 && TmpNum < 2050) { Tipos[i] = Tipo_ParteNombre_Texto;  } // Si va de 1900 a 2050, es casi seguro que es un año 
						else								{ Tipos[i] = Tipo_ParteNombre_Pista4; } // No parece ser un año cercano...
					}
					// El segundo carácter es una 'x'
					else if (_EsNumero(Sp[i][0]) == TRUE && Sp[i][1] == L'x' && _EsNumero(Sp[i][2]) == TRUE && _EsNumero(Sp[i][3]) == TRUE)		Tipos[i] = Tipo_ParteNombre_Pista1x2; // Pista doble con un digito delante y dos detras separada por una 'x'
					break;
				case 5: // Pista con cinco carácteres hay 2 posibilidades : 10x01 y 10-01
					if	(_EsNumero(Sp[i][0]) == TRUE && _EsNumero(Sp[i][1]) == TRUE && Sp[i][2] == L'x' && _EsNumero(Sp[i][3]) == TRUE && _EsNumero(Sp[i][4]) == TRUE)		Tipos[i] = Tipo_ParteNombre_Pista2x2; // Pista doble con dos digitos delante y dos detras  separada por una 'x'
					break;
			}
			
		}
		else {
			// Es una parte de texto, solo debo mirar si es una url para descartar-lo
			if (Sp[i].find(L"http") != std::wstring::npos)		Tipos[i] = Tipo_ParteNombre_Mierda;
			else if (Sp[i].find(L"www.") != std::wstring::npos) Tipos[i] = Tipo_ParteNombre_Mierda;
			else if (Sp[i].find(L".com") != std::wstring::npos) Tipos[i] = Tipo_ParteNombre_Mierda;
		}
	}

	int PosPista         = -1;
	Tipo_ParteNombre UPN = Tipo_ParteNombre_Texto;
	// Una vez identificados los sub-strings elijo el que tiene mas posibilidades de ser la pista
	// Los tipos están ordenados de forma que el tipo que mas provabilidades tiene que ser una pista, es el que tiene mas valor.
	for (size_t i = 0; i < Sp.Total(); i++) {
		if (Tipos[i] > UPN) {
			UPN = Tipos[i];
			PosPista = static_cast<int>(i);
		}
	}

	// Si hay algun sub-string que puede ser la pista
	if (PosPista != -1) {
		// Asigno la pista elegida
		switch (Tipos[PosPista]) {
			// Pistas en la que todos los carácteres son digitos
			case Tipo_ParteNombre_Pista1 :
			case Tipo_ParteNombre_Pista2 :
			case Tipo_ParteNombre_Pista3 :
			case Tipo_ParteNombre_Pista4 :
				nPista = _wtoi(Sp[PosPista].c_str());
				break;
			case Tipo_ParteNombre_Pista1p:
				nPista = _wtoi(Sp[PosPista].substr(0, 1).c_str());
				break;
			case Tipo_ParteNombre_Pista2p:
				nPista = _wtoi(Sp[PosPista].substr(0, 2).c_str());
				break;
				// Pistas que el primer carácter es un digito, y el segundo un separador 'x' o '-' (el ultimo caracter es un digito)
			case Tipo_ParteNombre_Pista1x1 :
				nPista = (_wtoi(Sp[PosPista].substr(0, 1).c_str()) * 100) + _wtoi(Sp[PosPista].substr(2, 1).c_str());
				break;
			// Pistas que el primer carácter es un digito, y el segundo un separador 'x' o '-' (los dos ultimos caracteres son un digito)
			case Tipo_ParteNombre_Pista1x2:
				nPista = (_wtoi(Sp[PosPista].substr(0, 1).c_str()) * 100) + _wtoi(Sp[PosPista].substr(2, 2).c_str());
				break;
			// Pistas que el primer y el segundo carácter son digitos, y el tercero un separador 'x' o '-' (los dos ultimos caracteres son un digito)
			case Tipo_ParteNombre_Pista2x2:
				nPista = (_wtoi(Sp[PosPista].substr(0, 2).c_str()) * 100) + _wtoi(Sp[PosPista].substr(3, 2).c_str());
				break;
		}
	}

	// Por ultimo construyo el string con el nombre al que le quito la pista y posibles urls
	for (size_t i = 0; i < Sp.Total(); i++) {
		// Si el substring NO ES la pista elegida, o NO ES un sub-string marcado como mierda
		if (i != PosPista && Tipos[i] != Tipo_ParteNombre_Mierda) {
			nNombre += Sp[i];
		}
		// Si empieza una url salgo del bucle, y no añado nada mas al nombre (siempre que sea el tercer substring o mas grande
/*		if (Tipos[i] == Tipo_ParteNombre_Mierda && i > 3) {
			break;
		}*/

		// Si la posición de la pista, es el ultimo sub-string hay que mirar que no estamos en el penultimo sub-string para agregar el carácter separador.
		// En caso de que la posición de la pista no sea el ultimo sub-string, hay que mirar que no estemos en el ultimo sub-string para añadir el espacio separador.
		// Agrego un espacio para separar el substring
		if (i > Sp.Total() - (PosPista == Sp.Total() - 1) ? -2 : -1) nNombre += L" ";
	}

//	Debug_Escribir_Varg(L"RaveBD::AnalizarNombre %d %s\n", nPista, nNombre.c_str());

	// Devuelvo TRUE si se ha encontrado una pista, FALSE en caso contrario
	return (PosPista != -1) ? TRUE : FALSE;
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




// Suma 1 a las reproducciones del BDMedio, y Suma 0.05 a la Nota (si la opción está activada)
const BOOL RaveBD::MedioReproducido(BDMedio *rMedio) {
	if (rMedio == NULL) return FALSE;
	std::wstring Q;

	if (App.Opciones.Sumar005() == TRUE) {	
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
	std::wstring Q = L"UPDATE Medios SET Nota=" + DWL::Strings::ToStrF(nNota, 2) + L" WHERE Id=" + std::to_wstring(nMedio->BdMedio.Id);
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

const BOOL RaveBD::ObtenerMediosPorRevisar(std::vector<BDMedio> &Medios, DWL::DUnidadesDisco& Unidades) {
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



// Función que compara la distancia entre 2 strings (levenshtein)
const int RaveBD::Distancia(std::wstring &Origen, std::wstring &Destino) {
	// Step 1
	const LONGLONG n = static_cast<int>(Origen.size());
	const LONGLONG m = static_cast<int>(Destino.size());

	if (n < 5) return static_cast<int>(n + m); // Si origen es mas pequeño que 4 caracteres
	if (m < 5) return static_cast<int>(m + n); // Si destino es mas pequeño que 4 caracteres
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
		for (LONGLONG i = 1; i <= n; i++) {
			const TCHAR s_i = Origen[i - 1];
			// Step 4
			for (LONGLONG j = 1; j <= m; j++) {
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


















