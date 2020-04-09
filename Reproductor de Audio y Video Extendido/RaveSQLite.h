#pragma once

#include <string>

class BDMedio;
class ThreadActualizarArbol;



class RaveSQLite_Consulta {
  public    :
								RaveSQLite_Consulta(void) : _SqlQuery(nullptr), _SqlRet(0) { };
								RaveSQLite_Consulta(sqlite3 *nBD, const wchar_t *Consulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila = [](RaveSQLite_Consulta &) { });
								RaveSQLite_Consulta(sqlite3 *nBD, std::wstring  &Consulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila = [](RaveSQLite_Consulta &) { });
								// Ejecuta una consulta con un lambda para cada fila
	const int					Ejecutar(sqlite3 *nBD, const wchar_t *Consulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila = [=](RaveSQLite_Consulta &) { });
	inline const int			Ejecutar(sqlite3 *nBD, std::wstring  &Consulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila = [=](RaveSQLite_Consulta &) { }) { return Ejecutar(nBD, Consulta.c_str(), LambdaFila); }
								// Ejecuta una consulta que devuelve la primera columna de la ultima fila, y devuelve el resultado como string
	const int                   Ejecutar_Str(sqlite3* nBD, const wchar_t *Consulta, std::wstring& OUT_Str);
	const int                   Ejecutar_Str(sqlite3* nBD, std::wstring  &Consulta, std::wstring& OUT_Str);
								// Ejecuta una consulta que devuelve la primera columna de la ultima fila, y devuelve el resultado como int
	const int                   Ejecutar_Int(sqlite3* nBD, const wchar_t *Consulta, int &OUT_Int);
	const int                   Ejecutar_Int(sqlite3* nBD, std::wstring  &Consulta, int &OUT_Int);
								// Ejecuta una consulta que devuelve la primera columna de la ultima fila, y devuelve el resultado como sqlite_int64
	const int                   Ejecutar_Int64(sqlite3* nBD, const wchar_t *Consulta, sqlite_int64 &OUT_Int64);
	const int                   Ejecutar_Int64(sqlite3* nBD, std::wstring  &Consulta, sqlite_int64 &OUT_Int64);
								// Ejecuta una consulta que devuelve la primera columna de la ultima fila, y devuelve el resultado como double
	const int                   Ejecutar_Double(sqlite3* nBD, const wchar_t *Consulta, double &OUT_Double);
	const int                   Ejecutar_Double(sqlite3* nBD, std::wstring  &Consulta, double &OUT_Double);

								// Devuelve el código con el resultado de la consulta
	inline const int			Resultado(void)					{ return _SqlRet; };
								// Equivalente a sqlite3_column_text16
	inline const wchar_t	   *ObtenerStr(const int nPos)		{ return reinterpret_cast<const wchar_t*>(sqlite3_column_text16(_SqlQuery, nPos)); };
								// Equivalente a sqlite3_column_int
	inline const int			ObtenerInt(const int nPos)		{ return sqlite3_column_int(_SqlQuery, nPos); }
								// Equivalente a sqlite3_column_int64
	inline const sqlite_int64	ObtenerInt64(const int nPos)	{ return sqlite3_column_int64(_SqlQuery, nPos); }
								// Equivalente a sqlite3_column_double
	inline const double			ObtenerDouble(const int nPos)	{ return sqlite3_column_double(_SqlQuery, nPos); }
								// Devuelve el ultimo error en formato texto (para obtener el código usa la función Resultado)
	inline std::wstring        &UltimoErrorStr(void)			{ return _TmpStr; }
  protected :
	sqlite3_stmt              *_SqlQuery;
	int                        _SqlRet;
	std::wstring		       _TmpStr;
};


class RaveSQLite {
  public:

								RaveSQLite(void);
							   ~RaveSQLite(void);
								// Consulta básica wchar_t
	const int					Consulta(const wchar_t *TxtConsulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila = [](RaveSQLite_Consulta &) { });
	inline const int			Consulta(std::wstring  &TxtConsulta, std::function<void(RaveSQLite_Consulta &)> LambdaFila = [](RaveSQLite_Consulta &) { }) { return Consulta(TxtConsulta.c_str(), LambdaFila); };
								// Consulta que devuelve la primera columna de la ultima fila en formato string
	const int					Consulta(const wchar_t *TxtConsulta, std::wstring &OUT_String);
	inline const int			Consulta(std::wstring  &TxtConsulta, std::wstring &OUT_String)	{ return Consulta(TxtConsulta.c_str(), OUT_String); };
	const int					Consulta(const wchar_t *TxtConsulta, int &OUT_Int);
	inline const int			Consulta(std::wstring  &TxtConsulta, int &OUT_Int)				{ return Consulta(TxtConsulta.c_str(), OUT_Int); };
	const int					Consulta(const wchar_t *TxtConsulta, sqlite_int64 &OUT_Int64);
	inline const int			Consulta(std::wstring  &TxtConsulta, sqlite_int64 &OUT_Int64)	{ return Consulta(TxtConsulta.c_str(), OUT_Int64); };
	const int					Consulta(const wchar_t *TxtConsulta, double &OUT_Double);
	inline const int			Consulta(std::wstring  &TxtConsulta, double &OUT_Double)	{ return Consulta(TxtConsulta.c_str(), OUT_Double); };
	const BOOL                  IniciarSQLite(const wchar_t *PathBD);
	void						Terminar(void);
								// Devuelve la ultima ID insertada
	const LONG_PTR              UltimaIdInsertada(void);

								// Funciones que devuelven el valor de la columna especificada de la consulta actual
//	std::wstring			   &Consulta_ObtenerStr(const size_t nPos, std::wstring &OUT_String);
//	const int                  &Consulta_ObtenerInt(const size_t nPos, int &OUT_Int);				// Equivalente a sqlite3_column_int

  protected:
	std::wstring		       _UltimoErrorSQL;
	sqlite3				      *_BD;

	friend class BDMedio;
	friend class ThreadActualizarArbol;
};

