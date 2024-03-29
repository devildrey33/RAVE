#pragma once

#include "DUnidadesDisco.h"
#include "ExtensionesValidas.h"
#include "vlc.hpp"
#include "RaveSQLite.h"

// Objeto para almacenar un momento
class BDMomento {
  public:
					BDMomento(void) : Id(0), IdPadre(0), TiempoInicio(0), TiempoFinal(0), Excluir(FALSE) { };
					BDMomento(const LONG_PTR nId, const LONG_PTR nIdPadre, const TCHAR *nNombre, const INT64 nTiempoInicio, const INT64 nTiempoFinal, const BOOL nExcluir) : Id(nId), IdPadre(nIdPadre), TiempoInicio(nTiempoInicio), TiempoFinal(nTiempoFinal), Excluir(nExcluir) { };
					BDMomento(const BDMomento &c) : Id(c.Id), IdPadre(c.IdPadre), Nombre(c.Nombre), TiempoInicio(c.TiempoInicio), TiempoFinal(c.TiempoFinal), Excluir(c.Excluir) { };
					BDMomento(const BDMomento *c) : Id(c->Id), IdPadre(c->IdPadre), Nombre(c->Nombre), TiempoInicio(c->TiempoInicio), TiempoFinal(c->TiempoFinal), Excluir(c->Excluir) { };
	LONG_PTR		Id;				// ID dentro de la tabla
	LONG_PTR        IdPadre;		// ID del medio al que pertenece este momento
	std::wstring	Nombre;			// Nombre del momento
	INT64			TiempoInicio;	// Tiempo de inicio del momento
	INT64			TiempoFinal;	// Tiempo final del momento
	BOOL            Excluir;		// Excluir momento de la reproducci�n (para intros, creditos, etc...)
};

// Clase con los datos de un medio
class BDMedio {
  public :
									BDMedio(void) : IDDisco(0), PistaPath(0), PistaTag(0), Hash(0), TipoMedio(Tipo_Medio_INDEFINIDO), Extension(Extension_NOSOPORTADA), Tiempo(0), Longitud(0), Id(0), /*IDDisco(0),*/ Parseado(FALSE), Actualizar(FALSE), Nota(2.5f), PistaEleccion(0), Reproducido(0), GrupoEleccion(0), DiscoEleccion(0), NombreEleccion(0), Brillo(1.0f), Contraste(1.0f), Saturacion(1.0f), PosMomento(-1) { };
//									BDMedio(UINT nId, sqlite3_int64 nHash, const wchar_t *nPath, const wchar_t *nNombre, Tipo_Medio nTipoMedio, Extension_Medio nExtension, UINT nReproducido, ULONG nLongitud, DWORD nIDDisco, UINT nNota, UINT nGenero, UINT nGrupo, UINT nDisco, UINT nPista, INT64 nTiempo, const wchar_t *nSubtitulos) : Id(nId), Hash(nHash), Path(nPath), NombrePath(nNombre), TipoMedio(nTipoMedio), Extension(nExtension), Longitud(nLongitud), IDDisco(nIDDisco), Nota(nNota), Pista(nPista), Tiempo(nTiempo), Subtitulos(nSubtitulos), Parseado(FALSE) { }
									BDMedio(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades, RaveSQLite *BD);
									BDMedio(const BDMedio& c);
						           ~BDMedio(void);

	BDMedio					       &operator = (const BDMedio &c);

	UINT							Pista(void);
	UINT							PistaTag;
	UINT							PistaPath;
	BOOL							PistaEleccion;	// Elecci�n : TRUE Path, FALSE Tag 

	std::wstring				   &Nombre(void);
	std::wstring					NombreTag;
	std::wstring					NombrePath;
	BOOL							NombreEleccion;	// Elecci�n : TRUE Path, FALSE Tag 

	sqlite3_int64					Hash;
	std::wstring					Path;

	Tipo_Medio						TipoMedio;
	Extension_Medio					Extension;
	INT64							Tiempo;
	ULONG							Longitud;

	UINT							Reproducido;
	float							Nota;

	UINT							Id;					// ID unica que identifica al medio (mucho mas recomendable que el Hash para buscar en la BD)

	DWORD							IDDisco;			// (NO SE USA, Solo se conserva para evitar retocar la tabla de Medios)
														// Ultima ID de disco conocida, (si se encuentran 2 o mas unidades con el medio disponible en su directorio, deber� escoger siempre el disco donde se encontro la ultima vez)
														//	Esto evitara un bug RARO que tengo por la distribuci�n de mis unidades... tengo los medios en la unidad E, pero tengo un disco externo de backup que siempre es la D.
														//  Al escanear los medios diponibles encuentra primero los del disco externo, y si lo apago... se lia gorda, y si adem�s mientras salen mensajes de error por que no encuentra los medios, creo una lista aleat�ria... rebienta y ni muestra la ventana del dump....

	std::wstring					Genero;

	std::wstring	               &Grupo(void);
	std::wstring					GrupoTag;
	std::wstring					GrupoPath;
	BOOL							GrupoEleccion;		// Elecci�n : TRUE Path, FALSE Tag 

	std::wstring	               &Disco(void);
	std::wstring					DiscoTag;
	std::wstring					DiscoPath;
	BOOL							DiscoEleccion;		// Elecci�n : TRUE Path, FALSE Tag 

	std::wstring					Subtitulos;

	BOOL							Parseado;			// Determina si se ha parseado/analizado el medio en busca de metadatos
	BOOL							Actualizar;			// NO SE GUARDA EN LA BD, ES SOLO PARA SABER SI HAY QUE ACTUALIZAR EL MEDIO O NO:...

									// Convierte la pista de UINT a std::wstring y la devuelve en el par�metro nPistaStr
	void							PistaStr(std::wstring &nPistaStr);

									// Funci�n virtual que obtiene todos los datos del medio en la BD
	virtual void					ObtenerFila(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades, RaveSQLite *BD);
	void							ObtenerMomentos(const UINT nId);

	std::wstring					Proporcion;
	float							Brillo;
	float							Contraste;
	float							Saturacion;

	LONG							PosMomento;			// Momento a reproducir (si es -1 es que no hay momento a reproducir)

	const BOOL						EsFMOD(void);

	const Ubicacion_Medio			Ubicacion(void);
	static const Ubicacion_Medio	Ubicacion(std::wstring &nPath);

	bool							operator != (const BDMedio &Comp) const { if (Comp.Hash != Hash) return true; return false; }

	std::vector<BDMomento*>			Momentos;
	void							BorrarMomentos(void);

									// Recarga los datos desde la BD para este medio
	void							Recargar(void);

									// Consulta SQL, que requiere : 
									//	TxtConsulta    [IN]  : Puntero constante al texto de la consulta.
									//  BD             [IN]  : Puntero a la base de datos de donde  queremos hacer la consulta.
									//  UltimoErrorSQL [OUT] : std::wstring con el texto del ultimo error generado por la consulta (si no hay error UltimoErrorSQL.size() ser� 0)
	const int						Consulta(const wchar_t *TxtConsulta, sqlite3 *BD, std::wstring &UltimoErrorSQL);
};




/*
// Objeto para los elementos del historial
class BDMedio_Historial : public BDMedio {
public:
	BDMedio_Historial(void) : BDMedio(), Id(0) {	}
	BDMedio_Historial(sqlite3_stmt* SqlQuery, DWL::DUnidadesDisco& Unidades, RaveSQLite* BD);
	BDMedio_Historial(const BDMedio& c);

	virtual void	ObtenerFila(sqlite3_stmt* SqlQuery, DWL::DUnidadesDisco& Unidades, RaveSQLite* BD);
	UINT			Id;
	std::wstring	FechaInicio;
	std::wstring	FechaFin;
};*/