#include "stdafx.h"
#include "TablaOpciones.h"
#include "DStringUtils.h"

TablaOpciones::TablaOpciones() {
}


TablaOpciones::~TablaOpciones() {
}


const BOOL TablaOpciones::CrearTabla(sqlite3 *BD) {
	int		    Ret = 0;
	char       *Error = NULL;
	const char *SqlCrearTablaRaiz = "CREATE TABLE Opciones (Id"     			  " INTEGER PRIMARY KEY, "	\
														   "Volumen"			  " INTEGER, "              \
														   "PathAbrir"			  " VARCHAR(260), "			\
														   "PosX"				  " INTEGER, "              \
														   "PosY"				  " INTEGER, "              \
														   "Ancho"				  " INTEGER, "              \
														   "Alto"				  " INTEGER, "              \
														   "Shufle"				  " INTEGER, "              \
														   "Repeat"				  " INTEGER, "              \
														   "Inicio"				  " INTEGER, "              \
														   "OcultarMouseEnVideo"  " INTEGER)";
	Ret = sqlite3_exec(BD, SqlCrearTablaRaiz, NULL, NULL, &Error);
	if (Ret) {
		sqlite3_free(Error);
		return FALSE;
	}


	sqlite3_stmt *SqlQuery = NULL;
	std::wstring SqlStr = L"INSERT INTO Opciones (ID, Volumen, PathAbrir, PosX, PosY, Ancho, Alto, Shufle, Repeat, Inicio, OcultarMouseEnVideo) VALUES(0, 100, \"C:\\\", 100, 100, 645, 400, 0, 0, 0, 3000)";
	int SqlRet = sqlite3_prepare16_v2(BD, SqlStr.c_str(), -1, &SqlQuery, NULL);
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_BUSY) {
		SqlRet = sqlite3_step(SqlQuery);
	}
	sqlite3_finalize(SqlQuery);
	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}



	return TRUE;
}


const UINT TablaOpciones::ObtenerDatos(sqlite3 *BD) {

	const wchar_t  *SqlStr = L"SELECT * FROM Opciones";
	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0; 
	sqlite3_stmt   *SqlQuery = NULL;
	_BD = BD;

	SqlRet = sqlite3_prepare16_v2(BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			_Volumen				= static_cast<int>(sqlite3_column_int(SqlQuery, 1));
			_PathAbrir				= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
			_PosX					= static_cast<int>(sqlite3_column_int(SqlQuery, 3));
			_PosY					= static_cast<int>(sqlite3_column_int(SqlQuery, 4));
			_Ancho					= static_cast<int>(sqlite3_column_int(SqlQuery, 5));
			_Alto					= static_cast<int>(sqlite3_column_int(SqlQuery, 6));
			_Shufle					= static_cast<int>(sqlite3_column_int(SqlQuery, 7));
			_Repeat					= static_cast<int>(sqlite3_column_int(SqlQuery, 8));
			_Inicio					= static_cast<int>(sqlite3_column_int(SqlQuery, 9));
			_OcultarMouseEnVideo	= static_cast<int>(sqlite3_column_int(SqlQuery, 10));
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}

	return TRUE;

	/*
	//	return Consulta(BD, L"SELECT * FROM Raiz");
	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	const wchar_t  *SqlStr = L"SELECT * FROM Raiz";
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
			TmpRaiz->Id = static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 0));
			TmpRaiz->Path = reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			TmpRaiz->ID_Disco = static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 2));
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


	return TRUE;*/
}

/* NOTA es mejor tener 2 selects para las opciones, uno para el tamaño y posición de la ventana, y otro para el resto de valores (shufle, repeat, volumen, etc...) 
   Y no viene de 15 milisegundos mas a la hora de cerrar el reproductor */
const BOOL TablaOpciones::GuardarOpciones(void) {
	GuardarPosTamVentana();
	std::wstring Q = L"UPDATE Opciones SET" 
						L" Volumen=" + std::to_wstring(_Volumen) +
//						L", Volumen=" + std::to_wstring(_Volumen) +
						L", Shufle=" + std::to_wstring(_Shufle) +
						L", Repeat=" + std::to_wstring(_Repeat) +
				     L" WHERE Id=0";
	return _Query(Q.c_str());
}

const BOOL TablaOpciones::GuardarPosTamVentana(void) {
	if (App.VentanaRave.Maximizada() == FALSE) {
		RECT RC;
		GetWindowRect(App.VentanaRave.hWnd(), &RC);
		_PosX = RC.left;
		_PosY = RC.top;
		_Ancho = abs(RC.right - RC.left);
		_Alto = abs(RC.bottom - RC.top);

		std::wstring Q = L"UPDATE Opciones SET PosX=" + DWL::DString_ToStr(RC.left) + L", PosY=" + DWL::DString_ToStr(RC.top) + L", Ancho=" + DWL::DString_ToStr(_Ancho) + L", Alto=" + DWL::DString_ToStr(_Alto) + L" WHERE Id=0";
		return _Query(Q.c_str());
	}
	return FALSE;
}

// UPDATE nTabla SET nParam="nInt" WHERE nDonde="nEs"
/*int TablaOpciones::_Update(const wchar_t *nTabla, const wchar_t *nParam, const wchar_t *nDonde, const wchar_t *nEs, int nInt) {
	std::wstring  wTabla  = nTabla;
	std::wstring  wParam  = nParam;
	std::wstring  wDonde  = nDonde;
	std::wstring  wEs     = nEs;
	std::wstring  SqlUpdateStr = L"UPDATE " + wTabla + L" SET " + wParam + L"=\"" + DWL::DString_ToStr(nInt) + L"\" WHERE " + wDonde + L"=\"" + wEs + L"\"";
	sqlite3_stmt *SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(_BD, SqlUpdateStr.c_str(), -1, &SqlQuery, NULL);
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
	}
	sqlite3_finalize(SqlQuery);
	if (SqlRet == SQLITE_ERROR)
		return NULL;

}*/

void TablaOpciones::Volumen(const int nVolumen) {
	_Volumen = nVolumen;
	std::wstring Q = L"Update Opciones SET Volumen=" + DWL::DString_ToStr(nVolumen) + L" WHERE Id=0";
	_Query(Q.c_str());
}

void TablaOpciones::Shufle(const int nShufle) {
	_Shufle = nShufle;
	std::wstring Q = L"Update Opciones SET Volumen=" + DWL::DString_ToStr(nShufle) + L" WHERE Id=0";
	_Query(Q.c_str());
}

void TablaOpciones::Repeat(const int nRepeat) {
	_Repeat = nRepeat;
	std::wstring Q = L"Update Opciones SET Volumen=" + DWL::DString_ToStr(nRepeat) + L" WHERE Id=0";
	_Query(Q.c_str());

}


void TablaOpciones::Inicio(const int nInicio) {
	_Inicio = nInicio;
	std::wstring Q = L"Update Opciones SET Volumen=" + DWL::DString_ToStr(nInicio) + L" WHERE Id=0";
	_Query(Q.c_str());
}


void TablaOpciones::OcultarMouseEnVideo(const int nOcultarMouseEnVideo) {
	_OcultarMouseEnVideo = nOcultarMouseEnVideo;
	std::wstring Q = L"Update Opciones SET Volumen=" + DWL::DString_ToStr(nOcultarMouseEnVideo) + L" WHERE Id=0";
	_Query(Q.c_str());
}

