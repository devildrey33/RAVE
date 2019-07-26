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
	// Creo las tablas para la base de datos (si es necesario)
	Debug_Escribir(L"RaveSQLite::Iniciar\n");

	return TRUE;
}


void RaveSQLite::Terminar(void) {
	Debug_Escribir(L"RaveSQLite::Terminar\n");
	if (_BD != NULL) sqlite3_close_v2(_BD);
	_BD = NULL;
}


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
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
//		Debug_Escribir_Varg(L"RaveSQLite::Consulta %s\n", _UltimoErrorSQL.c_str());
	}

	return SqlRet;
}


const LONG_PTR RaveSQLite::UltimaIdInsertada(void) {
	return static_cast<LONG_PTR>(sqlite3_last_insert_rowid(_BD));
}
