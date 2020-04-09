#include "stdafx.h"
#include "RaveSQLite.h"


RaveSQLite::RaveSQLite(void) : _BD(NULL) {

}

RaveSQLite::~RaveSQLite(void) {
	Terminar();
}


const BOOL RaveSQLite::IniciarSQLite(const wchar_t *Path) {

	// Creo / Abro la BD
	int Ret = sqlite3_open16(Path, &_BD);
	if (Ret) {
		sqlite3_close(_BD);
		_BD = NULL;

		Debug_Escribir(L"RaveSQLite::Iniciar ERROR\n");

		return FALSE;
	}

	// Activo las relaciones para las foregin keys
	int SqlRet = Consulta(L"PRAGMA foreign_keys = ON");

	// Debug_Escribir(L"RaveSQLite::Iniciar\n");

	return TRUE;
}


void RaveSQLite::Terminar(void) {
	// Debug_Escribir(L"RaveSQLite::Terminar\n");
	if (_BD != NULL) sqlite3_close_v2(_BD);
	_BD = NULL;
}

const int RaveSQLite::Consulta(const wchar_t *TxtConsulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila) {
	if (_BD == NULL) return FALSE;
	RaveSQLite_Consulta Q(_BD, TxtConsulta, LambdaFila);
	// Si hay un error en formato texto, lo guardo
	if (Q.UltimoErrorStr().size() != 0) 
		_UltimoErrorSQL = Q.UltimoErrorStr();
	return Q.Resultado();
}


const int RaveSQLite::Consulta(const wchar_t *TxtConsulta, std::wstring &OUT_String) {
	if (_BD == NULL) return FALSE;
	RaveSQLite_Consulta Q;
	Q.Ejecutar_Str(_BD, TxtConsulta, OUT_String);
	// Si hay un error en formato texto, lo guardo
	if (Q.UltimoErrorStr().size() != 0) 
		_UltimoErrorSQL = Q.UltimoErrorStr();
	return Q.Resultado();
}

const int RaveSQLite::Consulta(const wchar_t *TxtConsulta, int &OUT_Int) {
	if (_BD == NULL) return FALSE;
	RaveSQLite_Consulta Q;
	Q.Ejecutar_Int(_BD, TxtConsulta, OUT_Int);
	// Si hay un error en formato texto, lo guardo
	if (Q.UltimoErrorStr().size() != 0) 
		_UltimoErrorSQL = Q.UltimoErrorStr();
	return Q.Resultado();
}

const int RaveSQLite::Consulta(const wchar_t *TxtConsulta, sqlite_int64 &OUT_Int64) {
	if (_BD == NULL) return FALSE;
	RaveSQLite_Consulta Q;
	Q.Ejecutar_Int64(_BD, TxtConsulta, OUT_Int64);
	// Si hay un error en formato texto, lo guardo
	if (Q.UltimoErrorStr().size() != 0)
		_UltimoErrorSQL = Q.UltimoErrorStr();
	return Q.Resultado();
}

const int RaveSQLite::Consulta(const wchar_t *TxtConsulta, double &OUT_Double) {
	if (_BD == NULL) return FALSE;
	RaveSQLite_Consulta Q;
	Q.Ejecutar_Double(_BD, TxtConsulta, OUT_Double);
	// Si hay un error en formato texto, lo guardo
	if (Q.UltimoErrorStr().size() != 0) 
		_UltimoErrorSQL = Q.UltimoErrorStr();
	return Q.Resultado();
}


/*
// Consulta básica wchar_t
const int RaveSQLite::Consulta(const wchar_t* TxtConsulta) {
	if (_BD == NULL) return FALSE;

	sqlite3_stmt* SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
		return SqlRet;
	}

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
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		//		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
	}

	return SqlRet;
}

// Consulta que retorna un valor
const int RaveSQLite::ConsultaInt(const wchar_t* TxtConsulta, int& OUT_Int) {
	if (_BD == NULL) return FALSE;

	sqlite3_stmt* SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
		return SqlRet;
	}

	int VecesBusy = 0;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			OUT_Int = sqlite3_column_int(SqlQuery, 0);
		}

		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}
	sqlite3_finalize(SqlQuery);
	// Si hay un error lo apunto
	if (SqlRet == SQLITE_ERROR || SqlRet == SQLITE_CONSTRAINT) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		//		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
	}

	return SqlRet;
}

// Consulta que retorna un valor string
const int RaveSQLite::ConsultaStr(const wchar_t* TxtConsulta, std::wstring &OUT_String) {
	if (_BD == NULL) return FALSE;

	sqlite3_stmt* SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
		return SqlRet;
	}

	int VecesBusy = 0;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			OUT_String = sqlite3_column_int(SqlQuery, 0);
		}

		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}
	sqlite3_finalize(SqlQuery);
	// Si hay un error lo apunto
	if (SqlRet == SQLITE_ERROR || SqlRet == SQLITE_CONSTRAINT) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		//		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
	}

	return SqlRet;
}
*/

const LONG_PTR RaveSQLite::UltimaIdInsertada(void) {
	return static_cast<LONG_PTR>(sqlite3_last_insert_rowid(_BD));
}







RaveSQLite_Consulta::RaveSQLite_Consulta(sqlite3 *nBD, std::wstring &Consulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila) : 
	_SqlQuery(nullptr), 
	_SqlRet(0) {
		Ejecutar(nBD, Consulta, LambdaFila);
}

RaveSQLite_Consulta::RaveSQLite_Consulta(sqlite3 *nBD, const wchar_t *Consulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila) : 
	_SqlQuery(nullptr), 
	_SqlRet(0) {
		Ejecutar(nBD, Consulta, LambdaFila);
}

const int RaveSQLite_Consulta::Ejecutar(sqlite3 *nBD, const wchar_t *Consulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila) {
	// La BD no es válida
	if (nBD == NULL) return FALSE;

//	sqlite3_stmt *SqlQuery = NULL;
	_SqlRet   = sqlite3_prepare16_v2(nBD, Consulta, -1, &_SqlQuery, NULL);

	if (_SqlRet) {
		_TmpStr = static_cast<const wchar_t*>(sqlite3_errmsg16(nBD));
		Debug_Escribir_Varg(L"RaveSQLite_Consulta::Ejecutar %s\n", _TmpStr.c_str());
		return _SqlRet;
	}

	/*	SQLITE_FULL		: database or disk full
		SQLITE_IOERR	: disk I / O error
		SQLITE_BUSY		: database in use by another process
		SQLITE_NOMEM	:*/
	int VecesBusy = 0;

	while (_SqlRet != SQLITE_DONE && _SqlRet != SQLITE_ERROR && _SqlRet != SQLITE_CONSTRAINT) {
		_SqlRet = sqlite3_step(_SqlQuery);
		if (_SqlRet == SQLITE_ROW) {
			LambdaFila(*this);
		}

		if (_SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) {
//				_TmpStr = L"Base de datos ocupada por otro proceso";
				break;
			}
		}

	}
	sqlite3_finalize(_SqlQuery);
	// Si hay un error lo apunto
	if (_SqlRet == SQLITE_ERROR || _SqlRet == SQLITE_CONSTRAINT) {
		_TmpStr = static_cast<const wchar_t*>(sqlite3_errmsg16(nBD));
//		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _TmpStr.c_str());
	}

	return _SqlRet;
}

const int RaveSQLite_Consulta::Ejecutar_Str(sqlite3 *nBD, const wchar_t *Consulta, std::wstring &OUT_Str) {
	return Ejecutar(nBD, Consulta, [&OUT_Str](RaveSQLite_Consulta &This) {
		OUT_Str = This.ObtenerStr(0);
	});
}

const int RaveSQLite_Consulta::Ejecutar_Str(sqlite3 *nBD, std::wstring &Consulta, std::wstring &OUT_Str) {
	return Ejecutar(nBD, Consulta, [&OUT_Str](RaveSQLite_Consulta &This) {
		OUT_Str = This.ObtenerStr(0);
	});
}

const int RaveSQLite_Consulta::Ejecutar_Int(sqlite3 *nBD, const wchar_t *Consulta, int &OUT_Int) {
	return Ejecutar(nBD, Consulta, [&OUT_Int](RaveSQLite_Consulta &This) {
		OUT_Int = This.ObtenerInt(0);
	});
}

const int RaveSQLite_Consulta::Ejecutar_Int(sqlite3 *nBD, std::wstring &Consulta, int &OUT_Int) {
	return Ejecutar(nBD, Consulta, [&OUT_Int](RaveSQLite_Consulta &This) {
		OUT_Int = This.ObtenerInt(0);
	});
}

const int RaveSQLite_Consulta::Ejecutar_Int64(sqlite3 *nBD, const wchar_t *Consulta, sqlite_int64 &OUT_Int64) {
	return Ejecutar(nBD, Consulta, [&OUT_Int64](RaveSQLite_Consulta &This) {
		OUT_Int64 = This.ObtenerInt64(0);
	});
}

const int RaveSQLite_Consulta::Ejecutar_Int64(sqlite3 *nBD, std::wstring &Consulta, sqlite_int64 &OUT_Int64) {
	return Ejecutar(nBD, Consulta, [&OUT_Int64](RaveSQLite_Consulta &This) {
		OUT_Int64 = This.ObtenerInt64(0);
	});
}

const int RaveSQLite_Consulta::Ejecutar_Double(sqlite3 *nBD, const wchar_t *Consulta, double &OUT_Double) {
	return Ejecutar(nBD, Consulta, [&OUT_Double](RaveSQLite_Consulta &This) {
		OUT_Double = This.ObtenerDouble(0);
	});
}

const int RaveSQLite_Consulta::Ejecutar_Double(sqlite3 *nBD, std::wstring &Consulta, double &OUT_Double) {
	return Ejecutar(nBD, Consulta, [&OUT_Double](RaveSQLite_Consulta &This) {
		OUT_Double = This.ObtenerDouble(0);
	});
}
