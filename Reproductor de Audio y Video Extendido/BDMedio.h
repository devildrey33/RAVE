#pragma once

#include "DUnidadesDisco.h"
#include "ExtensionesValidas.h"
#include "vlc.hpp"

class BDMomento {
  public:
					BDMomento(void) : Id(0), IdPadre(0), TiempoInicio(0), TiempoFinal(0), Excluir(FALSE) { };
					BDMomento(const LONG_PTR nId, const LONG_PTR nIdPadre, const TCHAR *nNombre, const libvlc_time_t nTiempoInicio, const libvlc_time_t nTiempoFinal, const BOOL nExcluir) : Id(nId), IdPadre(nIdPadre), TiempoInicio(nTiempoInicio), TiempoFinal(nTiempoFinal), Excluir(nExcluir) { };
					BDMomento(const BDMomento &c) : Id(c.Id), IdPadre(c.IdPadre), Nombre(c.Nombre), TiempoInicio(c.TiempoInicio), TiempoFinal(c.TiempoFinal), Excluir(c.Excluir) { };
					BDMomento(const BDMomento *c) : Id(c->Id), IdPadre(c->IdPadre), Nombre(c->Nombre), TiempoInicio(c->TiempoInicio), TiempoFinal(c->TiempoFinal), Excluir(c->Excluir) { };
	LONG_PTR		Id;				// ID dentro de la tabla
	LONG_PTR        IdPadre;		// ID del medio al que pertenece este momento
	std::wstring	Nombre;			// Nombre del momento
	libvlc_time_t	TiempoInicio;	// Tiempo de inicio del momento
	libvlc_time_t	TiempoFinal;	// Tiempo final del momento
	BOOL            Excluir;		// Excluir momento de la reproducción (para intros, creditos, etc...)
};

// Clase con los datos de un medio
class BDMedio {
  public :
							BDMedio(void) : PistaPath(0), PistaTag(0), Hash(0), TipoMedio(Tipo_Medio_INDEFINIDO), Extension(Extension_NOSOPORTADA), Tiempo(0), Longitud(0), Id(0), IDDisco(0), Parseado(FALSE), Actualizar(FALSE), Nota(2.5f), PistaEleccion(0), Reproducido(0), GrupoEleccion(0), DiscoEleccion(0), NombreEleccion(0), Brillo(1.0f), Contraste(1.0f), Saturacion(1.0f) { };
//							BDMedio(UINT nId, sqlite3_int64 nHash, const wchar_t *nPath, const wchar_t *nNombre, Tipo_Medio nTipoMedio, Extension_Medio nExtension, UINT nReproducido, ULONG nLongitud, DWORD nIDDisco, UINT nNota, UINT nGenero, UINT nGrupo, UINT nDisco, UINT nPista, libvlc_time_t nTiempo, const wchar_t *nSubtitulos) : Id(nId), Hash(nHash), Path(nPath), NombrePath(nNombre), TipoMedio(nTipoMedio), Extension(nExtension), Longitud(nLongitud), IDDisco(nIDDisco), Nota(nNota), Pista(nPista), Tiempo(nTiempo), Subtitulos(nSubtitulos), Parseado(FALSE) { }
							BDMedio(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades);
							BDMedio(const BDMedio& c);
	                       ~BDMedio(void);
	BDMedio                &operator = (const BDMedio &c);

	UINT					Pista(void);
	UINT					PistaTag;
	UINT					PistaPath;
	BOOL					PistaEleccion;	// Elección : TRUE Path, FALSE Tag 

	std::wstring		   &Nombre(void);
	std::wstring			NombreTag;
	std::wstring			NombrePath;
	BOOL					NombreEleccion;	// Elección : TRUE Path, FALSE Tag 

	sqlite3_int64			Hash;
	std::wstring			Path;

	Tipo_Medio				TipoMedio;
	Extension_Medio			Extension;
	libvlc_time_t			Tiempo;
	ULONG					Longitud;

	UINT					Reproducido;
	float					Nota;

	UINT					Id;					// ID unica que identifica al medio (mucho mkas recomendable que el Hash)
	DWORD					IDDisco;

	std::wstring			Genero;

	std::wstring	       &Grupo(void);
	std::wstring			GrupoTag;
	std::wstring			GrupoPath;
	BOOL					GrupoEleccion;	// Elección : TRUE Path, FALSE Tag 

	std::wstring	       &Disco(void);
	std::wstring			DiscoTag;
	std::wstring			DiscoPath;
	BOOL					DiscoEleccion;	// Elección : TRUE Path, FALSE Tag 

	std::wstring			Subtitulos;

	BOOL					Parseado;	// Determina si se ha parseado/analizado el medio en busca de metadatos
	BOOL                    Actualizar; // NO SE GUARDA EN LA BD, ES SOLO PARA SABER SI HAY QUE ACTUALIZAR EL MEDIO O NO:...
		
	void					PistaStr(std::wstring &nPistaStr);
	void					ObtenerFila(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades);
	void                    ObtenerMomentos(const UINT nId);

	std::wstring            Proporcion;
	float					Brillo;
	float                   Contraste;
	float                   Saturacion;

//	LONG                    IDMomentos;

	const BOOL              EsFMOD(void);

	bool					operator != (const BDMedio &Comp) const { if (Comp.Hash != Hash) return true; return false; }

	std::vector<BDMomento*>	Momentos;
	void                    BorrarMomentos(void);
};
