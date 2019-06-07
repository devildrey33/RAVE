#pragma once

#include "BDMedio.h"
#include "EtiquetaBD.h"
#include "BDRaiz.h"

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
// Temporizadores ejecutados con post message
#define WM_TIMER_LISTA				WM_USER + 2017
#define WM_TIMER_TIEMPO				WM_USER + 2018
#define WM_MEDIO_TERMINADO			WM_USER + 2019
#define WM_TIMER_OBTENERHWNDVLC		WM_USER + 2020

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
								// Devuelve la ultima ID insertada
	const LONG_PTR              UltimaIdInsertada(void);

								// Función que agrega un momento para un medio
	const BOOL                  AgregarMomento(BDMedio* nMedio, std::wstring& nNombre, const UINT64 nTiempoInicial, const UINT64 nTiempoFinal, const BOOL EvitarReproduccion);
								// Función que modifica el momento especificado de un medio
	const BOOL                  ModificarMomento(BDMedio* nMedio, const LONG_PTR mID, std::wstring &nNombre, const UINT64 nTiempoInicial, const UINT64 nTiempoFinal, const BOOL EvitarReproduccion);
								// Función que elimina el momento especificado de un medio
	const BOOL                  EliminarMomento(BDMedio* nMedio, const LONG_PTR mID);

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
								// Función que devuelve la versión de la BD
	const float                 ObtenerVersionBD(void);

								// NOTA Esta función no guarda la posición en la BD
	inline void					Opciones_AsignarPosVentana(const int nX, const int nY) { _Opciones_PosX = nX; _Opciones_PosY = nY; };
								// Opciones que se guardan en la BD
//	const BOOL					Opciones_GuardarOpciones(void);
	const BOOL					Opciones_GuardarPosTamVentana(void);
	const BOOL					Opciones_GuardarPosVentanaOpciones(void);
//	const BOOL					Opciones_GuardarPosVentanaAsociar(void);
	const BOOL					Opciones_GuardarPosVentanaAnalizar(void);
	const BOOL					Opciones_GuardarPosTamDlgDirectorios(RECT &RC);
	const BOOL					Opciones_GuardarPosVentanaMomentos(void);

	inline const int			Opciones_Volumen(void) { return _Opciones_Volumen; }
	void						Opciones_Volumen(const int nVolumen);

	inline const int			Opciones_PosX(void)                 { return _Opciones_PosX; }
	inline const int			Opciones_PosY(void)                 { return _Opciones_PosY; }
	inline const int			Opciones_VentanaOpciones_PosX(void) { return _Opciones_VentanaOpciones_PosX; }
	inline const int			Opciones_VentanaOpciones_PosY(void) { return _Opciones_VentanaOpciones_PosY; }
	inline const int			Opciones_DlgDirectorios_PosX(void)  { return _Opciones_DlgDirectorios_PosX;  }
	inline const int			Opciones_DlgDirectorios_PosY(void)  { return _Opciones_DlgDirectorios_PosY;  }
	inline const int			Opciones_DlgDirectorios_Ancho(void) { return _Opciones_DlgDirectorios_Ancho; }
	inline const int			Opciones_DlgDirectorios_Alto(void)  { return _Opciones_DlgDirectorios_Alto;  }
	inline const int			Opciones_VentanaAnalizar_PosX(void) { return _Opciones_VentanaAnalizar_PosX; }
	inline const int			Opciones_VentanaAnalizar_PosY(void) { return _Opciones_VentanaAnalizar_PosY; }
	inline const int			Opciones_VentanaMomentos_PosX(void) { return _Opciones_VentanaMomentos_PosX; }
	inline const int			Opciones_VentanaMomentos_PosY(void) { return _Opciones_VentanaMomentos_PosY; }

	inline const int			Opciones_Ancho(void) { return _Opciones_Ancho; }
	inline const int			Opciones_Alto(void)  { return _Opciones_Alto;  }

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

	inline const UINT			Opciones_EfectoFadeAudioMS(void) { return _Opciones_EfectoFadeAudioMS; }
	void						Opciones_EfectoFadeAudioMS(const UINT nOpciones_EfectoFadeAudioMS);

/*	inline const LONG			Opciones_ContadorIDSMomentos(void) { return _Opciones_ContadorIDSMomentos; }
	void						Opciones_ContadorIDSMomentos(const LONG nOpciones_ContadorIDSMomentos);*/

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
	const BOOL                 _ModificarTablas(void);
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
	int                        _Opciones_DlgDirectorios_Ancho;
	int                        _Opciones_DlgDirectorios_Alto;
	int                        _Opciones_VentanaAnalizar_PosX;
	int                        _Opciones_VentanaAnalizar_PosY;
	int                        _Opciones_VentanaMomentos_PosX;
	int                        _Opciones_VentanaMomentos_PosY;
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
//	LONG                       _Opciones_ContadorIDSMomentos;		// Contador global de las IDS para las tablas de momentos

	UINT                       _Opciones_EfectoFadeAudioMS;			// Milisegundos para el efecto fade audio
	friend class ThreadAnalisis;
	friend class BDMedio;
};

