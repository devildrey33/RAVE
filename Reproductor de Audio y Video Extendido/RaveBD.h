#pragma once

// Base de datos para los medios
#include "Historial_Medio.h"
#include "EtiquetaBD.h"
//#include "BDRaiz.h"
#include "RAVE_Mensajes.h"
#include "RaveSQLite.h"
#include <DMenuEx.h>

enum Estados_Medio {
	Nada      = 0,
	SinCargar = 1,
	Cargando  = 2,
	Abriendo  = 3,
	EnStop    = 4,
	EnPlay    = 5,
	EnPausa   = 6,
	Terminada = 7,
	EnError   = 8
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
	Tipo_Inicio_Nota		= 5,
	Tipo_Inicio_LoQueSea    = 6,
	Tipo_Inicio_UltimaLista = 7
};

class EtiquetaBD;



enum TipoListaAleatoria {
	TLA_Genero		= 0,
	TLA_Grupo		= 1,
	TLA_Disco		= 2,
	TLA_50Medios	= 3,
	TLA_Nota        = 4,
	TLA_LoQueSea	= 5
};

class ThreadAnalisis;
class ItemMedio;
class NodoBD;
class RaveVLC_Medio;



enum Tipo_ParteNombre {
	Tipo_ParteNombre_Mierda     = -1,	// www. (Urls incluidas en el mismo nombre del archivo)
	Tipo_ParteNombre_Texto		=  0,	// Qualquier texto
	Tipo_ParteNombre_Pista4		=  1,	//  2019   (Posible / provable any) 
	Tipo_ParteNombre_Pista1		=  2,	//     1		
	Tipo_ParteNombre_Pista2		=  3,	//    03
	Tipo_ParteNombre_Pista3		=  4,	//   102
	Tipo_ParteNombre_Pista1p	=  5,	//    1.
	Tipo_ParteNombre_Pista2p	=  6,	//   01.
	Tipo_ParteNombre_Pista1x1	=  7,	//   1x4
	Tipo_ParteNombre_Pista1x2	=  8,	//  1x04
	Tipo_ParteNombre_Pista2x2	=  9,   // 01x05
};

class RaveBD : public RaveSQLite {
  public:
								RaveBD(void);

								// Inicia la base de datos
	const BOOL					Iniciar(void);

								// Función que agrega un momento para un medio
	const BOOL                  AgregarMomento(BDMedio* nMedio, std::wstring& nNombre, const UINT64 nTiempoInicial, const UINT64 nTiempoFinal, const BOOL EvitarReproduccion);
								// Función que modifica el momento especificado de un medio
	const BOOL                  ModificarMomento(BDMedio* nMedio, const LONG_PTR mID, std::wstring &nNombre, const UINT64 nTiempoInicial, const UINT64 nTiempoFinal, const BOOL EvitarReproduccion);
								// Función que elimina el momento especificado de un medio
	const BOOL                  EliminarMomento(BDMedio* nMedio, const LONG_PTR mID);

								// Función que extrae los datos del medio que nos da el path
	const BOOL					AnalizarMedio(std::wstring &mPath, BDMedio &OUT_Medio, DWL::DUnidadesDisco& Unidades, const ULONG Longitud = 0, const UINT TiempoEnSecs = 0, const wchar_t *Nombre = nullptr);

								// Obtiene un puntero con los datos del medio en la BD (Si el medio no existe devuelve FALSE)
	const BOOL					ObtenerMedio(const sqlite3_int64 mHash, BDMedio &OUT_Medio, DWL::DUnidadesDisco &Unidades);
	const BOOL					ObtenerMedio(std::wstring &mPath, BDMedio &OUT_Medio, DWL::DUnidadesDisco &Unidades);

								// Obtiene una lista de paths que pertenecen a medios por parsear (extraer metadatos, id3, etc...)
	const BOOL                  ObtenerMediosPorParsear(std::vector<std::wstring> &Paths, DWL::DUnidadesDisco &Unidades);
	const BOOL                  ObtenerMediosPorRevisar(std::vector<BDMedio> &Medios, DWL::DUnidadesDisco &Unidades);

								// Funcion que actualiza los hash de los medios a la versión 1.1 de la BD
	const BOOL                  ActualizarHashs11(void);
								// Función que actualiza un hash especificado en el medio que corresponda con la Id especificada
//	const BOOL                  ActualizarHash(const UINT IdMedio, const sqlite3_int64 nHash);

								// Suma 1 a las veces que se ha reproducido el medio
	const BOOL                  MedioReproducido(BDMedio *rMedio);
								// Asigna la nota de un medio de la lista o el arbol
	const BOOL                  MedioNota(ItemMedio *nMedio, const float nNota);
	const BOOL                  MedioNota(NodoBD *nMedio, const float nNota);

								// Obtiene la etiqueta con el texto especificado
	EtiquetaBD                 *ObtenerEtiqueta(std::wstring &eTexto);
								// Obtiene la lista completa de etiquetas
	const BOOL                  ObtenerEtiquetas(void);

	Historial_Lista             ObtenerHistorial_Lista(const INT64 Id);

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
	const BOOL                  GenerarListaAleatoria(std::vector<BDMedio> &OUT_Medios, DWL::DUnidadesDisco &Unidades, const TipoListaAleatoria nTipo = TLA_LoQueSea);
								// Genera una sugerencia del tipo especificado de etiqueta con 5 etiquetas aleatórias (SOLO SE PUEDE USAR CON LOS TIPOS GENERO, GRUPO, y DISCO)
	const BOOL                  GenerarSugerenciasMenu(DWL::DMenuEx &Menu, const TipoListaAleatoria nTipo);
								// Genera una lista partiendo de la sugerencia donde se ha hecho click
	const BOOL                  GenerarListaDesdeSugerencias(DWL::DMenuEx& Menu, const TipoListaAleatoria nTipo);

								// Función que crea un Hash partiendo de la ID del disco y el path del medio
	const sqlite3_int64			CrearHash(std::wstring &nPath);

								// Devuelve el puntero de la base de datos sqlite 3
	inline sqlite3			   *operator()(void) { return _BD; }


	const BOOL                  ObtenerUltimaLista(DWL::DUnidadesDisco &Unidades);
	const BOOL                  GuardarUltimaLista(void);


	const BOOL                  GuardarHistorial_Lista(Historial_Lista &HL);
	LONG_PTR                    Historial_UltimaIDLista;						// Ultima ID insertada en la tabla Historial_Lista
	const BOOL                  GuardarHistorial_Medio(Historial_Medio &HM);
	//DWL::DUnidadesDisco			Unidades;


								// Filtros estaticos
								// Filtros para los strings que sean paths o nombres de medio
	static void					FiltroPath(std::wstring &In, std::wstring &Out);
	static void					FiltroNombre(std::wstring &In, std::wstring &Out);
								// Filtro que anializa path y devuelve el nombre del medio y la pista
	static const BOOL           AnalizarNombre(std::wstring& Analisis, std::wstring& nNombre, UINT& nPista);
//	static const BOOL           AnalizarNombre2(std::wstring& Analisis, std::wstring& nNombre, UINT& nPista);

								// Distancia entre 2 strings
	static const int            Distancia(std::wstring &Origen, std::wstring &Destino);
  protected:
	const BOOL                  ActualizarMedioAnalisis(BDMedio *nMedio);

								// Devuelve TRUE si es un número del 0 al 9, no acepta comas para decimales
    static const BOOL		   _EsNumero(const wchar_t Caracter);

/*	const BOOL			       _CompararRaices(std::wstring &Path1, std::wstring &Path2);
	void                       _BorrarRaices(void);
	std::vector<BDRaiz *>      _Raices;*/
	std::vector<EtiquetaBD>    _Etiquetas;
	const BOOL                 _ConsultaObtenerMedio(std::wstring &TxtConsulta, BDMedio &OUT_Medio, DWL::DUnidadesDisco& Unidades);
								// Función que crea todas las tablas necesarias
    const BOOL                 _CrearTablas(void);
								// Función que crea una tabla para medios con el nombre especificado
	const BOOL                 _CrearTablaMedios(const wchar_t* Nombre);
								// Función que modifica las tablas si son de una versión anterior
	const BOOL                 _ModificarTablas(void);

	friend class ThreadAnalisis;
	friend class BDMedio;
	friend class RaveVLC_Medio;
};

