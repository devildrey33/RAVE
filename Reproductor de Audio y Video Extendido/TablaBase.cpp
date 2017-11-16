#include "stdafx.h"
#include "TablaBase.h"


TablaBase::TablaBase(void) : _BD(NULL) {
}


TablaBase::~TablaBase(void) {
}

/* Para updates y inserts mayormente, si hay que hacer un select hay que construir un bucle */
const BOOL TablaBase::_Query(const wchar_t *nQuery) {
	if (_BD == NULL) return FALSE;

	sqlite3_stmt *SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(_BD, nQuery, -1, &SqlQuery, NULL);

/*	SQLITE_FULL: database or disk full
	SQLITE_IOERR : disk I / O error
	SQLITE_BUSY : database in use by another process
	SQLITE_NOMEM :*/
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_BUSY) {
		SqlRet = sqlite3_step(SqlQuery);
	}
	sqlite3_finalize(SqlQuery);
	if (SqlRet == SQLITE_ERROR)
		return FALSE;
	return TRUE;
}
