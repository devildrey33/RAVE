#include "stdafx.h"
#include "TablaRaiz.h"
#include "DStringUtils.h"
#include "DUnidadesDisco.h"

TablaRaiz::TablaRaiz(sqlite3 *BD) {
	ObtenerDatos(BD);
}

TablaRaiz::~TablaRaiz(void) {
	_BorrarMemoria();
}

const BOOL TablaRaiz::CrearTabla(sqlite3 *BD) {
	int		    Ret = 0;
	char       *Error = NULL;
	const char *SqlCrearTablaRaiz = "CREATE TABLE Raiz (Id INTEGER PRIMARY KEY, "	\
													   "Path VARCHAR(260), "	\
													   "IdDisco INTEGER)";
	Ret = sqlite3_exec(BD, SqlCrearTablaRaiz, NULL, NULL, &Error);
	if (Ret) {
		sqlite3_free(Error);
		return FALSE;
	}
	return TRUE;
}

void TablaRaiz::_BorrarMemoria(void) {
	for (size_t i = 0; i < Datos.size(); i++) delete Datos[i];
	Datos.resize(0);
	_BD = NULL;
}

const UINT TablaRaiz::ObtenerDatos(sqlite3 *BD) {

//	return Consulta(BD, L"SELECT * FROM Raiz");
	wchar_t		   *SqlError	= NULL;
	int				SqlRet		= 0;
	const wchar_t  *SqlStr		= L"SELECT * FROM Raiz";
	_BorrarMemoria();
	_BD = BD;

	DWL::DUnidadesDisco nUnidades;
	DWL::DUnidadDisco  *Unidad;
	sqlite3_stmt   *SqlQuery = NULL;
	SqlRet = sqlite3_prepare16_v2(BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			CeRaiz *TmpRaiz = NULL;
			TmpRaiz = new CeRaiz;
			TmpRaiz->Id			= static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 0));
			TmpRaiz->Path		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			TmpRaiz->ID_Disco	= static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 2));
			// Asigno la letra de unidad a la raíz ya que las unidades extraibles pueden cambiar de letra pero siempre tienen el mismo número de serie.
			Unidad = nUnidades.Buscar_Numero_Serie(TmpRaiz->ID_Disco);
			if (Unidad != NULL) {
				TmpRaiz->Path[0] = Unidad->Letra();
				TmpRaiz->Letra = Unidad->Letra();
			}
			Datos.push_back(TmpRaiz);
		}
	} 

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}

	return TRUE;
}


CeRaiz *TablaRaiz::Buscar_RaizPorID(const unsigned long bID) {
	for (size_t i = 0; i < Datos.size(); i++) {
		if (Datos[i]->Id == bID) return Datos[i];
	}
	return NULL;
}
/* Aseguramos que esta en la BD y buscamos la clase CeRaiz con sus datos */
CeRaiz *TablaRaiz::Buscar_Raiz(const TCHAR *nPath) {
	std::wstring Path = nPath;
	int Comp = 0;
	for (size_t i = 0; i < Datos.size(); i++) {
		// El Path a buscar es de mayor tamaño
		if (Datos[i]->Path.size() < Path.size())		Comp = _wcsicmp(Datos[i]->Path.c_str(), Path.substr(0, Datos[i]->Path.size()).c_str());
		// El Path a buscar es de menor tamaño
		else if (Datos[i]->Path.size() > Path.size())	Comp = _wcsicmp(Datos[i]->Path.substr(0, Path.size()).c_str(), Path.c_str());
		// Tienen el mismo tamaño
		else											Comp = _wcsicmp(Datos[i]->Path.c_str(), Path.c_str());
		// Si son iguales retorno los datos
		if (Comp == 0) return Datos[i];
	}

	return NULL;
}

/*	FALSE Ya esta agregada o hay un error
	 > 0  Nueva raíz agregada, el resultado es la ID de la raiz dentro de la base de datos  */

/*	Se puede dar el caso en que dada una raíz existente se quiera agregar otra raíz que es pariente de esta.
		En este no se añadirá ninguna raíz y la raíz existente pasara a tener el path que ocupe menos caracteres. */
const BOOL TablaRaiz::Argerar_Raiz(const TCHAR *nPath) {
	BOOL Ret = 0;
	UINT Total = 0;
	int  SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	std::wstring PathFinal = nPath;
	if (PathFinal[PathFinal.size() - 1] != TEXT('\\')) PathFinal += TEXT('\\');

	CeRaiz *BuscarRaiz = Buscar_Raiz(PathFinal.c_str());
	if (BuscarRaiz != NULL) {
		// La nueva raíz es el padre/abuelo/etc.. de una raíz definida anteriormente 
		if (BuscarRaiz->Path.size() > PathFinal.size()) {
			std::wstring SqlUpdateStr = L"UPDATE Raiz SET Path=\"" + PathFinal + L"\"WHERE Id=" + DWL::DString_ToStr(BuscarRaiz->Id);
			SqlRet = sqlite3_prepare16_v2(_BD, SqlUpdateStr.c_str(), -1, &SqlQuery, NULL);
			while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_BUSY) {
				SqlRet = sqlite3_step(SqlQuery);
			}
			sqlite3_finalize(SqlQuery);
			if (SqlRet == SQLITE_ERROR) 
				return NULL;
			ObtenerDatos(_BD);
		}
		return FALSE;
	}


	DWL::DUnidadesDisco nUnidades;
	DWL::DUnidadDisco  *Unidad = nUnidades.Buscar_Letra(PathFinal[0]);
	if (Unidad == NULL)
		return FALSE;


//	assert(Unidad != NULL);

	std::wstring SqlStr = L"INSERT INTO Raiz (Path, IDDisco) VALUES(\"" + PathFinal + L"\", " + DWL::DString_ToStr(Unidad->Numero_Serie()) + L")";
	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr.c_str(), -1, &SqlQuery, NULL);


	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_BUSY) {
		SqlRet = sqlite3_step(SqlQuery);
/*		if (SqlRet == SQLITE_DONE) {
//			CallbackResultado(SqlQuery, Total++);
			CeRaiz *TmpRaiz = NULL;
			TmpRaiz = new CeRaiz;
			TmpRaiz->Id = static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 0));
			TmpRaiz->Path = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			TmpRaiz->ID_Disco = static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 2));
			Datos.push_back(TmpRaiz);
			Ret = TmpRaiz->Id;
		}*/
	} 
	sqlite3_finalize(SqlQuery);
	if (SqlRet == SQLITE_ERROR) {
		return NULL;
	}

	ObtenerDatos(_BD);

	/*
	wchar_t *SqlError = NULL;
	int SqlRet = sqlite3_exec16(_BD, SqlStr.c_str(), NULL, NULL, &SqlError);
	if (SqlRet) {
		sqlite3_free(SqlError);
		return FALSE;
	}*/

	return Ret;
}
