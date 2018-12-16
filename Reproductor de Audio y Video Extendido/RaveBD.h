#pragma once

#include "DUnidadesDisco.h"
#include "vlc.hpp"
#include "ExtensionesValidas.h"
#include "EtiquetaBD.h"

// Thread actualizar BD
#define WM_TBA_AGREGARDIR			WM_USER + 2000
#define WM_TBA_AGREGARRAIZ			WM_USER + 2001
#define WM_TBA_TERMINADO			WM_USER + 2002
#define WM_TBA_CANCELADO			WM_USER + 2003
// Agregar un medio desde el explorador (dobleclick o menu)
#define WM_AGREGARMEDIO				WM_USER + 2004
#define WM_REPRODUCIRMEDIO			WM_USER + 2005
// Thread agregar medios externos
#define WM_TAAL_AGREGARMEDIO		WM_USER + 2006
#define WM_TAAL_TERMINADO			WM_USER + 2007
// Thread obtener metadatos
#define WM_TOM_INICIADO1	        WM_USER + 2008
#define WM_TOM_INICIADO2	        WM_USER + 2009
#define WM_TOM_INICIADO3	        WM_USER + 2010
#define WM_TOM_TOTALMEDIOS1         WM_USER + 2011
#define WM_TOM_TOTALMEDIOS2         WM_USER + 2012
#define WM_TOM_TOTALMEDIOS3         WM_USER + 2013
#define WM_TOM_TERMINADO            WM_USER + 2014
#define WM_TOM_CANCELADO            WM_USER + 2015
#define WM_TOM_MOSTRARVENTANA		WM_USER + 2016


enum Estados_Medio {
	Nada      = 0,
	SinCargar = 1,
	Abriendo  = 2,
	EnStop    = 3,
	EnPlay    = 4,
	EnPausa   = 5,
	Terminada = 6,
	EnError   = 7
};


enum Tipo_Repeat {
	Tipo_Repeat_NADA				= 0,
	Tipo_Repeat_RepetirLista		= 1,
	Tipo_Repeat_RepetirListaShufle  = 2,
	Tipo_Repeat_GenerarGenero       = 3,
	Tipo_Repeat_GenerarGrupo		= 4,
	Tipo_Repeat_GenerarDisco		= 5,
	Tipo_Repeat_Generar50Canciones  = 6,
	Tipo_Repeat_GenerarLoQueSea		= 7,

	Tipo_Repeat_ApagarReproductor	= 8,
	Tipo_Repeat_ApagarOrdenador		= 9
//	Tipo_Repeat_HibernarOrdenador	= 5
};

enum Tipo_Inicio {
	Tipo_Inicio_NADA		= 0,
	Tipo_Inicio_Genero		= 1,
	Tipo_Inicio_Grupo		= 2,
	Tipo_Inicio_Disco		= 3,
	Tipo_Inicio_50Medios	= 4,
	Tipo_Inicio_LoQueSea    = 5,
	Tipo_Inicio_UltimaLista = 6
};

class EtiquetaBD;

// clase que engloba una raíz
class BDRaiz {
  public:
						BDRaiz(void)				: ID_Disco(0), Id(0), Letra('C') { };
						BDRaiz(std::wstring &nPath) : ID_Disco(0), Id(0), Letra('C'), Path(nPath) { };
						BDRaiz(const BDRaiz &c)		: ID_Disco(c.ID_Disco), Path(c.Path), Id(c.Id), Letra(c.Letra) { };
	                   ~BDRaiz(void) { };
	std::wstring        Path;
	unsigned long		ID_Disco;	// Numero de serie de la unidad
	unsigned long		Id;			// Id dentro de la base de datos
	wchar_t             Letra;		// Letra de la unidad actual
};

// Clase con los datos de un medio
class BDMedio {
  public :
							BDMedio(void) : PistaPath(0), PistaTag(0), Hash(0), TipoMedio(Tipo_Medio_INDEFINIDO), Extension(Extension_NOSOPORTADA), Tiempo(0), Longitud(0), Id(0), IDDisco(0), Parseado(FALSE), Actualizar(FALSE), Nota(2.5f), PistaEleccion(0), Reproducido(0), GrupoEleccion(0), DiscoEleccion(0), NombreEleccion(0) { };
//							BDMedio(UINT nId, sqlite3_int64 nHash, const wchar_t *nPath, const wchar_t *nNombre, Tipo_Medio nTipoMedio, Extension_Medio nExtension, UINT nReproducido, ULONG nLongitud, DWORD nIDDisco, UINT nNota, UINT nGenero, UINT nGrupo, UINT nDisco, UINT nPista, libvlc_time_t nTiempo, const wchar_t *nSubtitulos) : Id(nId), Hash(nHash), Path(nPath), NombrePath(nNombre), TipoMedio(nTipoMedio), Extension(nExtension), Longitud(nLongitud), IDDisco(nIDDisco), Nota(nNota), Pista(nPista), Tiempo(nTiempo), Subtitulos(nSubtitulos), Parseado(FALSE) { }
							BDMedio(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades);
	                       ~BDMedio(void) { };

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

	bool					operator != (const BDMedio &Comp) const { if (Comp.Hash != Hash) return true; return false; }
};


enum TipoListaAleatoria {
	TLA_Genero		= 0,
	TLA_Grupo		= 1,
	TLA_Disco		= 2,
	TLA_50Medios	= 3,
	TLA_LoQueSea	= 4
};

class ThreadAnalisis;
class ItemMedio;
class NodoBD;

class RaveBD {
  public:
								RaveBD(void);
	                           ~RaveBD(void);

								// Inicia la base de datos
	const BOOL					Iniciar(void);
								// Termina la conexión a la base de datos
	void						Terminar(void);
								// Consulta básica wchar_t
	const int					Consulta(const wchar_t *TxtConsulta);
								// Consulta al estilo printf
//	const int					ConsultaVarg(const wchar_t *TxtConsulta, ...);
								// Consulta desde un std::wstring
	inline const int			Consulta(std::wstring &TxtConsulta) { return Consulta(TxtConsulta.c_str()); };

								// Función que extrae los datos del medio que nos da el path
	const BOOL					AnalizarMedio(std::wstring &mPath, BDMedio &OUT_Medio, const ULONG Longitud = 0);

								// Obtiene un puntero con los datos del medio en la BD (Si el medio no existe devuelve FALSE)
	const BOOL					ObtenerMedio(const sqlite3_int64 mHash, BDMedio &OUT_Medio);
	const BOOL					ObtenerMedio(std::wstring &mPath, BDMedio &OUT_Medio);

								// Obtiene una lista de paths que pertenecen a medios por parsear (extraer metadatos, id3, etc...)
	const BOOL                  ObtenerMediosPorParsear(std::vector<std::wstring> &Paths);
	const BOOL                  ObtenerMediosPorRevisar(std::vector<BDMedio> &Medios);

								// Suma 1 a las veces que se ha reproducido el medio
	const BOOL                  MedioReproducido(BDMedio *rMedio);
								// Asigna la nota de un medio de la lista o el arbol
	const BOOL                  MedioNota(ItemMedio *nMedio, const float nNota);
	const BOOL                  MedioNota(NodoBD *nMedio, const float nNota);

								// Obtiene la etiqueta con el texto especificado
	EtiquetaBD                 *ObtenerEtiqueta(std::wstring &eTexto);
								// Obtiene la lista completa de etiquetas
	const BOOL                  ObtenerEtiquetas(void);

								// Calcula el total de medios, el total de bytes, el total de tiempo, y la nota media de una etiqueta
	void						CalcularDatosEtiqueta(EtiquetaBD *Etiqueta);
								// Asigna la nota especificada a los medios que pertenecen a la etiqueta especificada.
	void                        AsignarNotaEtiqueta(const float nNota, EtiquetaBD *Etiqueta);

//	const float                 ObtenerEtiquetaNota(EtiquetaBD *nEtiqueta);

								// Guarda los datos del medio especificado en la BD
	const BOOL                  ActualizarMedio(BDMedio *nMedio);
								// Guarda el path del medio (especial para renombrar archivos CRDOWNLOAD y OPDOWNLOAD)
	const BOOL                  ActualizarPathMedio(std::wstring &Path, const UINT mID);

								// Genera una lista aleatoria por el tipo especificado
	const BOOL                  GenerarListaAleatoria(std::vector<BDMedio> &OUT_Medios, const TipoListaAleatoria nTipo = TLA_LoQueSea);

								// Funciones para buscar una raíz por su path o por su id
	BDRaiz                     *BuscarRaiz(std::wstring &nPath);
	BDRaiz                     *BuscarRaiz(const unsigned long bID);
								// Función para agregar una raíz a la base de datos
	const int                   AgregarRaiz(std::wstring &nPath);
								// Función para eliminar una raíz de la base de datos
	const BOOL					EliminarRaiz(std::wstring &nPath);
								// Función que obtiene las raices de la base de datos
	const BOOL					ObtenerRaices(void);
								// Funciones para obtener los datos de las raices en memória
	inline const size_t			TotalRaices(void)		{ return _Raices.size(); }
	inline BDRaiz              *Raiz(const size_t Pos)  { return _Raices[Pos];   }

								// Función que crea un Hash partiendo de la ID del disco y el path del medio
	const sqlite3_int64			CrearHash(DWORD NSD, std::wstring &nPath);

								// Devuelve el puntero de la base de datos sqlite 3
	inline sqlite3			   *operator()(void) { return _BD; }

	const BOOL                  ObtenerTeclasRapidas(void);
	const BOOL                  GuardarTeclasRapidas(void);

	const BOOL                  ObtenerUltimaLista(void);
	const BOOL                  GuardarUltimaLista(void);

								// Función que obtiene las opciones de la base de datos
	const BOOL					ObtenerOpciones(void);
								// NOTA Esta función no guarda la posición en la BD
	inline void					Opciones_AsignarPosVentana(const int nX, const int nY) { _Opciones_PosX = nX; _Opciones_PosY = nY; };
								// Opciones que se guardan en la BD
//	const BOOL					Opciones_GuardarOpciones(void);
	const BOOL					Opciones_GuardarPosTamVentana(void);
	const BOOL					Opciones_GuardarPosVentanaOpciones(void);
//	const BOOL					Opciones_GuardarPosVentanaAsociar(void);
	const BOOL					Opciones_GuardarPosVentanaAnalizar(void);

	inline const int			Opciones_Volumen(void) { return _Opciones_Volumen; }
	void						Opciones_Volumen(const int nVolumen);

	inline const int			Opciones_PosX(void) { return _Opciones_PosX; }
	inline const int			Opciones_PosY(void) { return _Opciones_PosY; }
	inline const int			Opciones_VentanaOpciones_PosX(void) { return _Opciones_VentanaOpciones_PosX; }
	inline const int			Opciones_VentanaOpciones_PosY(void) { return _Opciones_VentanaOpciones_PosY; }
	inline const int			Opciones_DlgDirectorios_PosX(void)  { return _Opciones_DlgDirectorios_PosX; }
	inline const int			Opciones_DlgDirectorios_PosY(void)  { return _Opciones_DlgDirectorios_PosY; }
	inline const int			Opciones_VentanaAnalizar_PosX(void) { return _Opciones_VentanaAnalizar_PosX; }
	inline const int			Opciones_VentanaAnalizar_PosY(void) { return _Opciones_VentanaAnalizar_PosY; }

	inline const int			Opciones_Ancho(void) { return _Opciones_Ancho; }
	inline const int			Opciones_Alto(void) { return _Opciones_Alto; }

	inline const BOOL			Opciones_Shufle(void) { return _Opciones_Shufle; }
	void						Opciones_Shufle(const BOOL nShufle);

	inline const Tipo_Repeat	Opciones_Repeat(void) { return _Opciones_Repeat; }
	void						Opciones_Repeat(const Tipo_Repeat nRepeat);

	inline const Tipo_Inicio	Opciones_Inicio(void) { return _Opciones_Inicio; }
	void						Opciones_Inicio(const Tipo_Inicio nInicio);
								// Tiempo para ocultar los controles pantalla completa
	inline const int			Opciones_OcultarMouseEnVideo(void) { return _Opciones_OcultarMouseEnVideo; }
	void						Opciones_OcultarMouseEnVideo(const int nOcultarMouseEnVideo);

	inline const BOOL			Opciones_MostrarObtenerMetadatos(void) { return _Opciones_MostrarObtenerMetadatos; }
	void						Opciones_MostrarObtenerMetadatos(const BOOL nMostrarObtenerMetadatos);

	inline const BOOL			Opciones_MostrarAsociarArchivos(void) { return _Opciones_MostrarAsociarArchivos; }
	void						Opciones_MostrarAsociarArchivos(const BOOL nMostrarAsociarArchivos);

	inline const BOOL			Opciones_AnalizarMediosPendientes(void) { return _Opciones_AnalizarMediosPendientes; }
	void						Opciones_AnalizarMediosPendientes(const BOOL nAnalizarMediosPendientes);

	inline const BOOL			Opciones_BuscarActualizacion(void) { return _Opciones_BuscarActualizacion; }
	void						Opciones_BuscarActualizacion(const BOOL nOpciones_BuscarActualizacion);

	inline const UINT			Opciones_TiempoAnimaciones(void) { return _Opciones_TiempoAnimaciones; }
	void						Opciones_TiempoAnimaciones(const UINT nOpciones_TiempoAnimaciones);

	inline const UINT			Opciones_TiempoToolTips(void) { return _Opciones_TiempoToolTips; }
	void						Opciones_TiempoToolTips(const UINT nOpciones_TiempoToolTips);

	inline const BOOL			Opciones_NoAgregarMedioMenos25(void) { return _Opciones_NoAgregarMedioMenos25; }
	void						Opciones_NoAgregarMedioMenos25(const BOOL nOpciones_NoAgregarMedioMenos25);

	inline const BOOL			Opciones_NoGenerarListasMenos3(void) { return _Opciones_NoGenerarListasMenos3; }
	void						Opciones_NoGenerarListasMenos3(const BOOL nOpciones_NoGenerarListasMenos3);

	inline const BOOL			Opciones_Sumar005(void) { return _Opciones_Sumar005; }
	void						Opciones_Sumar005(const BOOL n_pciones_Sumar005);

	inline const int			Opciones_AlineacionControlesVideo(void) { return _Opciones_AlineacionControlesVideo; }
	void						Opciones_AlineacionControlesVideo(const int nOpciones_AlineacionControlesVideo);

	inline const int			Opciones_OpacidadControlesVideo(void) { return _Opciones_OpacidadControlesVideo; }
	void						Opciones_OpacidadControlesVideo(const int nOpciones_OpacidadControlesVideo);

	DWL::DUnidadesDisco			Unidades;


								// Filtros estaticos
								// Filtros para los strings que sean paths o nombres de medio
	static void					FiltroPath(std::wstring &In, std::wstring &Out);
	static void					FiltroNombre(std::wstring &In, std::wstring &Out);
								// Filtro que anializa path y devuelve el nombre del medio y la pista
	static const BOOL           AnalizarNombre(std::wstring &Analisis, std::wstring &nNombre, UINT &nPista);

								// Distancia entre 2 strings
	static const int            Distancia(std::wstring &Origen, std::wstring &Destino);
protected:
	const BOOL                  ActualizarMedioAnalisis(BDMedio *nMedio);

								// Devuelve TRUE si es un número del 0 al 9, no acepta comas para decimales
    static const BOOL		   _EsNumero(const wchar_t Caracter);

	const BOOL			       _CompararRaices(std::wstring &Path1, std::wstring &Path2);
	void                       _BorrarRaices(void);
	std::vector<BDRaiz *>      _Raices;
	std::vector<EtiquetaBD>    _Etiquetas;
	const BOOL                 _ConsultaObtenerMedio(std::wstring &TxtConsulta, BDMedio &OUT_Medio);

    const BOOL                 _CrearTablas(void);
	std::wstring               _UltimoErrorSQL;
	sqlite3                   *_BD;

								// Opciones en memória
	std::wstring               _Opciones_PathAbrir;					// Ultimo path donde se ha abierto un archivo externo
	int                        _Opciones_Volumen;
	int                        _Opciones_PosX;
	int                        _Opciones_PosY;
	int                        _Opciones_VentanaOpciones_PosX;
	int                        _Opciones_VentanaOpciones_PosY;
	int                        _Opciones_DlgDirectorios_PosX;
	int                        _Opciones_DlgDirectorios_PosY;
	int                        _Opciones_VentanaAnalizar_PosX;
	int                        _Opciones_VentanaAnalizar_PosY;
	int                        _Opciones_Ancho;
	int                        _Opciones_Alto;
	BOOL                       _Opciones_Shufle;
	Tipo_Repeat                _Opciones_Repeat;
	Tipo_Inicio                _Opciones_Inicio;					// Como se inicia el reproductor (lista de inicio)
	float                      _Opciones_Version;					// Versión de la base de datos
	int                        _Opciones_OcultarMouseEnVideo;		// Tiempo en MS que tarda en ocultarse el mouse y los controles de un video
	BOOL                       _Opciones_MostrarObtenerMetadatos;	// Mostrar la ventana del thread obtener metadatos
	BOOL                       _Opciones_MostrarAsociarArchivos;	// Mostrar la ventana para asociar este reproductor con todas las extensiones de medios conocidas.
	BOOL					   _Opciones_AnalizarMediosPendientes;	// Analizar medios pendientes al actualizar el arbol de la base de datos
	BOOL					   _Opciones_BuscarActualizacion;		// Buscar actualizaciones del reproductor al iniciar
	UINT                       _Opciones_TiempoAnimaciones;			// Duración de las animaciones
	UINT                       _Opciones_TiempoToolTips;			// Duración de los tooltips informativos de la parte inferior derecha

	BOOL                       _Opciones_NoAgregarMedioMenos25;		// No Agregar medios con menos de 2.5 de nota a las listas aleatórias
	BOOL                       _Opciones_NoGenerarListasMenos3;		// No generar listas aleatórias con menos de 3 medios
	BOOL                       _Opciones_Sumar005;                  // Sumar 0.05 a la nota al reproducir completamente un medio
	int				           _Opciones_AlineacionControlesVideo;	// Alineación para los controles pantalla completa
	int						   _Opciones_OpacidadControlesVideo;    // Opacidad máxima para los controles pantalla completa
	friend class ThreadAnalisis;
};

