#include "stdafx.h"
//#include "sqlite3.c"
#include "TablaMedios.h"
#include "TablaRaiz.h"
//#include <string>
#include "DStringUtils.h"


const BOOL TablaMedios::CrearTabla(sqlite3 *BD) {
	int		Ret = 0;
	char   *Error = NULL;

	const wchar_t *SqlCrearTablaCanciones = L"CREATE TABLE Medios (Id INTEGER PRIMARY KEY, "	\
																	 L"Hash BIGINT UNIQUE,"		\
																	 L"Path VARCHAR(260), "		\
																	 L"Nombre VARCHAR(128), "	\
																	 L"TipoMedio INT, "			\
																	 L"Extension INT, "			\
																	 L"Reproducido INT, "		\
																	 L"Longitud INT, "			\
																	 L"Raiz INT, "				\
																	 L"Nota SMALLINT, "			\
																	 L"Genero INT, " 			\
																	 L"Grupo INT, "	    		\
																	 L"Disco INT, "  			\
																	 L"Pista INT, "             \
																	 L"Tiempo INT, "            \
																	 L"Subtitulos VARCHAR(260))";
	/*	Ret = sqlite3_exec(BD, SqlCrearTablaCanciones, NULL, NULL, &Error);
	if (Ret) {
		sqlite3_free(Error);
		return FALSE;
	}*/


	sqlite3_stmt   *SqlQuery = NULL;
	int SqlRet = sqlite3_prepare16_v2(BD, SqlCrearTablaCanciones, -1, &SqlQuery, NULL);
	if (SqlRet)
		return FALSE;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_BUSY) {
		SqlRet = sqlite3_step(SqlQuery);
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}

	return TRUE;
}



/* Función estatica para crear un hash a partir de un numero de serie del disco y el path de la canción */
/* NOTA se eliminan los dos primeros carácteres del path que son los de la unidad, ya que los medios extraibles pueden cambiar de letra. */
const sqlite3_int64 TablaMedios::CrearHash(DWORD NSD, std::wstring &nPath) {
//	std::wstring PathCortado = Path.substr(2, Path.size() - 2);
	std::hash<std::wstring> HashFunc;
	return HashFunc(DWL::DString_ToStr(NSD) + nPath.substr(2, nPath.size() - 2));
}














// Obtiene los datos de un medio buscando por su path
const BOOL TablaMedios_Medio::Obtener(sqlite3 *SqlBD, std::wstring &mPath) {
	std::wstring TmpPath = mPath;
	TmpPath[0] = L'?'; //                                                         Comparar strings case insensitive
	std::wstring SqlStr = L"SELECT * FROM Medios WHERE Path =\"" + TmpPath + L"\" COLLATE NOCASE";
	return _Consulta(SqlBD, SqlStr);
}

// Obtiene los datos de un medio buscando por su hash
const BOOL TablaMedios_Medio::Obtener(sqlite3 *SqlBD, const sqlite3_int64 Hash) {
	std::wstring    SqlStr = L"SELECT * FROM Medios WHERE Hash =" + DWL::DString_ToStr(Hash);
	return _Consulta(SqlBD, SqlStr);
}

// Función que devuelve el número de pista en formato string con un mínimo de 2 digitos
void TablaMedios_Medio::PistaStr(std::wstring &nPistaStr) {	
	if (_Pista < 10) { nPistaStr = L"0" + std::to_wstring(_Pista);	}
	else             { nPistaStr = std::to_wstring(_Pista);         }
}


const BOOL TablaMedios_Medio::_Consulta(sqlite3 *SqlBD, std::wstring &SqlStr) {
	DWORD Tick = GetTickCount();
	Debug_Escribir_Varg(L"TablaMedios_Medio::_Consulta  TxtQuery = '%s'\n", SqlStr.c_str());

//	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	SqlRet = sqlite3_prepare16_v2(SqlBD, SqlStr.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		Debug_Escribir_Varg(L"TablaMedios_Medio::_Consulta terminada con error\n");
		return FALSE;
	}

	std::wstring TmpPath;

	CeRaiz *TmpRaiz = NULL;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			_Id = static_cast<UINT>(sqlite3_column_int(SqlQuery, 0));
			_Hash		 = sqlite3_column_int64(SqlQuery, 1);
			_Path		 = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
			_Nombre		 = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
			_TipoMedio	 = static_cast<Tipo_Medio>(sqlite3_column_int(SqlQuery, 4));
			_Extension	 = static_cast<Extension_Medio>(sqlite3_column_int(SqlQuery, 5));
			_Reproducido = static_cast<UINT>(sqlite3_column_int(SqlQuery, 6));
			_Longitud	 = static_cast<DWORD>(sqlite3_column_int(SqlQuery, 7));
			_Raiz		 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 8));
			_Nota		 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 9));
			_Genero		 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 10));
			_Grupo		 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 11));
			_Disco		 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 12));
			_Pista		 = static_cast<UINT>(sqlite3_column_int(SqlQuery, 13));
			_Tiempo		 = static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 14));
			_Subtitulos  = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 15));

			TmpRaiz = App.BD.Tabla_Raiz.Buscar_RaizPorID(_Raiz);
			if (TmpRaiz) _Path[0] = TmpRaiz->Letra;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		Debug_Escribir_Varg(L"TablaMedios_Medio::_Consulta terminada con error : %dMS\n", GetTickCount() - Tick);

		return FALSE;
	}

	Debug_Escribir_Varg(L"TablaMedios_Medio::_Consulta terminada : %dMS\n", GetTickCount() - Tick);
	return TRUE;

}