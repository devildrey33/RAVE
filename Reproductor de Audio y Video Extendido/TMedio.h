#pragma once

#include "ExtensionesValidas.h"


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


class TMedioAudio;
class TMedioVideo;
class TMedioListaM3u;
class TMedioSubtitulos;
class TMedioCDAudio;

class NodoBD;

// Datos basicos de un medio (tanto audio como video)
class TMedio {
  public:
								TMedio(void) : Reproducido(0), Tiempo(0), Longitud(0), Nota(3), Extension(Extension_NOSOPORTADA), Raiz(NULL), Nodo(NULL), Disponible(false) {};
	std::wstring				Path;		// UNICO
	std::wstring				Nombre;		// UNICO
	Tipo_Medio					TipoMedio;	// TIPO
	Extension_Medio				Extension;	// 
	unsigned long				Reproducido;// Veces reproducido
	UINT64						Tiempo;		// Tiempo
	unsigned long				Longitud;	// Longitud en bytes
//	unsigned long				ID_Disco;	// ID del disco duro padre
	TRaiz					   *Raiz;		// Raíz que contiene el path principal y la id del disco duro
	NodoBD       			   *Nodo;
//	ItemListaMedios            *Item;
	unsigned char               Nota;		// NOTA
	bool						Disponible;

/*	TMedioAudio                *TMAudio(void)	{
									return static_cast<TMedioAudio *>(this);
								};
	TMedioVideo                *TMVideo(void)	{
									return static_cast<TMedioVideo *>(this);
								};
	TMedioListaM3u             *TMListaM3u(void)	{
									return static_cast<TMedioListaM3u *>(this);
								};
	TMedioSubtitulos           *TMSubtitulos(void)	{
									return static_cast<TMedioSubtitulos *>(this);
								};
	TMedioCDAudio              *TMCDAudio(void)	{
									return static_cast<TMedioCDAudio *>(this);
								};*/
};

/*
// Datos temporales para un medio con audio
class TMedioPreAudio : public TMedio {
public:
	TMedioPreAudio(void) { TipoMedio = Tipo_Medio_PreAudio; Pista = 0; };
	int                     Pista;
	std::wstring			NombreTipo;
	std::wstring			NombreGenero;
	std::wstring			NombreGrupo;
	std::wstring	        NombreDisco;
};*/

// Datos para un medio con audio
class TMedioAudio : public TMedio {
public:
							TMedioAudio(void) : Pista(0), Tipo(NULL), Genero(NULL), Grupo(NULL), Disco(NULL), StrTipo(NULL), StrGenero(NULL), StrGrupo(NULL), StrDisco(NULL) { TipoMedio = Tipo_Medio_Audio; };
	UINT                    Pista;
	float                   EQ[11]; // de momento solo audio
	TTipo                  *Tipo;
	TGenero                *Genero;
	TGrupo                 *Grupo;
	TDisco                 *Disco;
	StrTp                  *StrTipo;
	StrTp                  *StrGenero;
	StrTp                  *StrGrupo;
	StrTp                  *StrDisco;
};

// Datos para un medio con video
class TMedioVideo : public TMedio {
public:
					TMedioVideo(void) : Pista(0), Ancho(0), Alto(0) { TipoMedio = Tipo_Medio_Video; };
	unsigned int	Ancho;
	unsigned int	Alto;
	UINT            Pista;
	std::wstring	ArchivoSubtitulos;
};

// Datos para una lista de medios M3u
class TMedioListaM3u : public TMedio {
public:
	TMedioListaM3u(void) { TipoMedio = Tipo_Medio_Lista; };
	//std::vector<DWL::DWLString> Paths;
};

// Datos para la ruta de un archivo de subtitulos
class TMedioSubtitulos : public TMedio {
public:
	TMedioSubtitulos(void) { TipoMedio = Tipo_Medio_Subtitulos; };
	//std::vector<DWL::DWLString> Paths;
};

// Datos para una pista de un CD de audio
class TMedioCDAudio : public TMedio {
public:
	TMedioCDAudio(void) : Pista(0) { TipoMedio = Tipo_Medio_CDAudio; };
	UINT		Pista;
};
