#pragma once

#include <string>

class RaveSQLite {
  public:
								RaveSQLite(void);
							   ~RaveSQLite(void);
								// Consulta básica wchar_t
	const int					Consulta(const wchar_t *TxtConsulta);
	inline const int			Consulta(std::wstring& TxtConsulta) { return Consulta(TxtConsulta.c_str()); };
	const BOOL                  IniciarSQLite(const wchar_t* PathBD);
	void						Terminar(void);
  protected:
	std::wstring		       _UltimoErrorSQL;
	sqlite3				      *_BD;
};

