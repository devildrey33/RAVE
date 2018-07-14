#pragma once 

#include "sqlite3.h"
#include <string>
#include "ExtensionesValidas.h"
//#include "..\vlc-2.2.1\Include\vlc\vlc.h"
#include "vlc.hpp"

/*
	Nombre		Posición	   Tipo
	--------------------------------
	Id				0		INTEGER PRIMARY KEY
	Hash			1		INT UNIQUE
	Path			2		VARCHAR(260)
	Nombre			3		VARCHAR(128)
	TipoMedio		4		INT
	Extension		5		INT
	Reproducido		6		INT
	Longitud		7		INT
	Raiz			8		INT
	Nota			9		SMALLINT
	Genero			10		INT
	Grupo			11		INT
	Disco			12		INT
	Pista			13		INT
	Tiempo			14		INT
	Subtitulos		15		VARCHAR(260)
*/


class TablaMedios {
  public :
									TablaMedios(void) { };
	                               ~TablaMedios(void) { };
	 const BOOL						CrearTabla(sqlite3 *BD);
	 static const sqlite3_int64     CrearHash(DWORD NSD, std::wstring &nPath);
};

/* Acceso a una canción de la base de datos */
class TablaMedios_Medio {
public:
									TablaMedios_Medio(void) : _Id(0), _Hash(0), _TipoMedio(Tipo_Medio_INDEFINIDO), _Extension(Extension_NOSOPORTADA), _Longitud(0), _Raiz(0), _Nota(0), _Genero(0), _Grupo(0), _Disco(0), _Pista(0) {};
									TablaMedios_Medio(UINT nId, sqlite3_int64 nHash, const wchar_t *nPath, const wchar_t *nNombre, Tipo_Medio nTipoMedio, Extension_Medio nExtension, UINT nReproducido, ULONG nLongitud, UINT nRaiz, UINT nNota, UINT nGenero, UINT nGrupo, UINT nDisco, UINT nPista, libvlc_time_t nTiempo, const wchar_t *nSubtitulos) : _Id(nId), _Hash(nHash), _Path(nPath), _Nombre(nNombre), _TipoMedio(nTipoMedio), _Extension(nExtension), _Longitud(nLongitud), _Raiz(nRaiz), _Nota(nNota), _Genero(nGenero), _Grupo(nGrupo), _Disco(nDisco), _Pista(nPista), _Tiempo(nTiempo), _Subtitulos(nSubtitulos) { }
									// Constructor que obtiene los datos de un medio partiendo del hash
									TablaMedios_Medio(sqlite3 *BD, const sqlite3_int64 Hash) { Obtener(BD, Hash); }
                                   ~TablaMedios_Medio(void) { }
	
	inline const UINT               Id(void) { return _Id; }
	inline const sqlite3_int64      Hash(void) { return _Hash; }

	inline const wchar_t           *Path(void) { return _Path.c_str(); };
	inline const wchar_t           *Nombre(void) { return _Nombre.c_str(); };

	inline const Tipo_Medio			TipoMedio(void) { return _TipoMedio; };
	inline const Extension_Medio	Extension(void) { return _Extension; };

	inline const UINT               Reproducido(void) { return _Reproducido; }
	inline const DWORD              Longitud(void) { return _Longitud; }
	inline const UINT               Raiz(void) { return _Raiz; }
	inline const UINT               Nota(void) { return _Nota; }
	inline const UINT               Pista(void) { return _Pista; }
	void                            PistaStr(std::wstring &nPistaStr);
									// Tiempo en milisegundos
	inline libvlc_time_t			Tiempo(void) { return _Tiempo;  };
									// Re-asigna el tiempo en la base de datos
	const BOOL                      Tiempo(const libvlc_time_t nTiempo);
									// Funciones para obtener un medio de la BD partiendo del Hash o del Path.
	const BOOL                      Obtener(sqlite3 *BD, const sqlite3_int64 Hash);
	const BOOL                      Obtener(sqlite3 *BD, std::wstring &mPath);
  protected:

	const BOOL                     _Consulta(sqlite3 *BD, std::wstring &StrConsulta);

	UINT                           _Id;
	sqlite3_int64                  _Hash;
	std::wstring                   _Path;
	std::wstring                   _Nombre;
	Tipo_Medio                     _TipoMedio;
	Extension_Medio                _Extension;
	UINT                           _Reproducido;
	ULONG                          _Longitud;
	UINT                           _Raiz;
	UINT                           _Nota;
	UINT						   _Genero;
	UINT                           _Grupo;
	UINT                           _Disco;
	UINT                           _Pista;
	libvlc_time_t                  _Tiempo;
	std::wstring                   _Subtitulos;
};