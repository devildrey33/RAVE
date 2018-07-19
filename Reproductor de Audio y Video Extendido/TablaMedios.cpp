#include "stdafx.h"
//#include "sqlite3.c"
#include "TablaMedios.h"
#include "TablaRaiz.h"
//#include <string>
#include "DStringUtils.h"

//#include <tag.h>
//#include <fileref.h>


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
	TmpPath[0] = L'?'; // Los paths de la BD emnpiezan por ?
	std::wstring SqlStr = L"SELECT * FROM Medios WHERE Path =\"" + TmpPath + L"\" COLLATE NOCASE"; // COLLATE NOCASE = Comparar strings case insensitive
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

// Función que re-asigna el tiempo del medio en la BD
const BOOL TablaMedios_Medio::Tiempo(const libvlc_time_t nTiempo) {
	// NO FUNCIONA
	BOOL Ret = App.BD.SqlQuery(L"UPDATE Medios SET Tiempo=%d WHERE Id=%d", nTiempo, _Id);
	Obtener(App.BD(), _Hash);
	return Ret;


}




const BOOL TablaMedios_Medio::_Consulta(sqlite3 *SqlBD, std::wstring &SqlStr) {
	DWORD Tick = GetTickCount();
	Debug_Escribir_Varg(L"TablaMedios_Medio::_Consulta TxtQuery = '%s'\n", SqlStr.c_str());

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


const BOOL TablaMedios_Medio::AgregarMedio(sqlite3 *BD, std::wstring &Path, TablaRaiz &Raiz) {
	// Compruebo si el medio existe en la BD
	if (Obtener(BD, Path) == FALSE) {
		// Si el medio no existe 
		//	1 - Busco la Raíz (que tiene que ser el path menos el nombre de archivo)
		//      - Si no existe la raíz creo una nueva basada en el ultimo directorio del medio
		//	2 - Añado el medio a la BD

		std::wstring PathRaiz;
		size_t i = 0;
		// Busco la ultima antibarra
		for (i = Path.size(); i > 0; i--) {
			if (Path[i] == L'\\') 
				break;
		}
		PathRaiz = Path.substr(0, i);
		// Agrego la raiz o busco una existente
		CeRaiz *nRaiz = NULL;
		Raiz.Argerar_Raiz(PathRaiz.c_str(), nRaiz);
		
//		TagLib::FileRef(Path.c_str());

		return AnalizarMedio(BD, Path, Raiz);
	}
	return FALSE;
}


/*
	NOTA :	Si el path existe en la BD retorna hash.
	Si el path no existe en la BD, lo añade a la BD y retorna el hash.
	Si el path es falso y no conduce a ningún archivo válido retorna NULL.
*/
// Analiza el medio, y si no existe lo agrega a la BD
// TODO : eliminar param longitud i obtenirla al fer el CreateFile per obrir l'arxiu
const BOOL TablaMedios_Medio::AnalizarMedio(sqlite3 *SqlBD, std::wstring &nPath, TablaRaiz &Raiz, DWORD Longitud) {
	//	TCHAR			TmpPath[MAX_PATH];
//	std::wstring	Path = nPath;
	// Compruebo que existe fisicamente en el disco (comprobar velocidades usando CreateFile)
	if (GetFileAttributes(nPath.c_str()) == INVALID_FILE_ATTRIBUTES)
		//	if (PathFileExists(nPath) == FALSE) 
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Obtenir temps d'un mp3 en milisegons																							//
	// http://stackoverflow.com/questions/3505575/how-can-i-get-the-duration-of-an-mp3-file-cbr-or-vbr-with-a-very-small-library	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Recorto el path y elimino los dos primeros caracteres que representan la unidad
	std::wstring    PathCortado = nPath;
	PathCortado[0] = L'?';
	sqlite3_stmt   *SqlQuery = NULL;

	// https://www.mail-archive.com/sqlite-users@sqlite.org/msg63109.html
	// http://www.sqlite.org/c3ref/stmt.html
	// http://www.sqlite.org/c3ref/prepare.html
	// http://sqlite.1065341.n5.nabble.com/How-to-use-sqlite3-exec-to-execute-SQL-command-with-Unicode-text-td53339.html



	size_t			PosNombre = nPath.find_last_of(TEXT("\\"));
	size_t			PosExtension = nPath.find_last_of(TEXT("."));
	Extension_Medio	Extension = ExtensionesValidas::ObtenerExtension(nPath.substr(PosExtension + 1, (nPath.size() - PosExtension) - 1).c_str());
	Tipo_Medio		Tipo = ExtensionesValidas::OtenerTipoMedio(Extension);
	//	TMedio         *Medio = NULL;
	UINT			Pista = 0;
	std::wstring	TmpNombre;
	std::wstring    NombreFinal;
	int				SqlRet = 0;
	std::wstring	SqlStr;
	//	wchar_t        *SqlError = NULL;
	const char     *Error;

	// Obtengo / Creo la raiz para el medio
	CeRaiz *nRaiz = NULL;
	Raiz.Argerar_Raiz(PathCortado.c_str(), nRaiz);

	sqlite3_int64   Hash = TablaMedios::CrearHash(nRaiz->ID_Disco, nPath);
	//	std::hash<std::wstring> HashFunc;
	//	size_t					Hash = HashFunc(DString_ToStr(Raiz->Id) + PathCortado);

	switch (Tipo) {
		case Tipo_Medio_INDEFINIDO:
			return FALSE;
		case Tipo_Medio_Audio:
		case Tipo_Medio_Video:
			_AnalizarNombre(nPath.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, Pista);
			BaseDatos_Filtros::FiltroNombre(TmpNombre, NombreFinal);
			SqlStr = L"INSERT INTO Medios (Hash, Path, Nombre, TipoMedio, Extension, Pista, Raiz, Longitud, Nota, Tiempo, Subtitulos) VALUES(" + DString_ToStr(Hash)		+ L", \"" +
																																				 PathCortado				+ L"\", \"" +
																																				 NombreFinal				+ L"\", " +
																																				 DString_ToStr(Tipo)		+ L", " +
																																				 DString_ToStr(Extension)	+ L", " +
																																				 DString_ToStr(Pista)		+ L", " +
																																				 DString_ToStr(nRaiz->Id)	+ L", " +
																																				 DString_ToStr(Longitud)	+ L", " +
																													/* Nota */					 +L"2," +
																													/* Tiempo*/					 +L"0," +
																																				 +L"\"\")";

			SqlRet = sqlite3_prepare16_v2(SqlBD, SqlStr.c_str(), -1, &SqlQuery, NULL);
			if (SqlRet) {
				Error = sqlite3_errmsg(SqlBD);
				return FALSE;
			}

			while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
				SqlRet = sqlite3_step(SqlQuery);
			}
			sqlite3_finalize(SqlQuery);
			if (SqlRet == SQLITE_DONE)
				return TRUE; // No existe el hash
			else if (SqlRet == SQLITE_CONSTRAINT)
				return TRUE; // Ya existe el hash
			else  /* Error ? */
				Error = sqlite3_errmsg(SqlBD);

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
	/*	Medio->Extension = Extension;
	//	Medio->Raiz = nRaiz; // LA RAIZ SE TIENE QUE ASIGNAR UNA VEZ MANDADO EL MENSAJE CON LOS DATOS AL THREAD PRINCIPAL
	Medio->Path = Path;
	Medio->TipoMedio = Tipo;
	Medio->Disponible = true;
	Medios.push_back(Medio);

	return Medio;*/
	return FALSE;
}


/*
IN  -> Analisis
OUT -> nNombre
OUT -> nPista
Nombres de pista validos : 1, 01, 1x01, 1-12, 20x21
Nunca se hace caso a 4 digitos seguidos o mas (ya que son o el año o otra cosa.. pero la pista seguro que no)
Se analiza de izquierda a derecha (lo mas normal es que la pista salga a la izquieda nada mas empezar)
*/
const BOOL TablaMedios_Medio::_AnalizarNombre(std::wstring &Analisis, std::wstring &nNombre, UINT &nPista) {
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


const BOOL TablaMedios_Medio::_EsNumero(const wchar_t Caracter) {
	if (Caracter >= TEXT('0') && Caracter <= TEXT('9'))	return TRUE;
	return FALSE;
}