#pragma once

#define TIPO_GENERO			0x01 
#define TIPO_GRUPOPATH		0x02 
#define TIPO_DISCOPATH		0x04 
#define TIPO_NOMBREPATH		0x08 
#define TIPO_GRUPOTAG		0x10 
#define TIPO_DISCOTAG		0x20 
#define TIPO_NOMBRETAG		0x40 


enum EtiquetaBD_Tipo {
	EtiquetaBD_Tipo_Genero		= TIPO_GENERO,
	EtiquetaBD_Tipo_GrupoPath	= TIPO_GRUPOPATH,
	EtiquetaBD_Tipo_DiscoPath	= TIPO_DISCOPATH,
	EtiquetaBD_Tipo_NombrePath	= TIPO_NOMBREPATH,
	EtiquetaBD_Tipo_GrupoTag	= TIPO_GRUPOTAG,
	EtiquetaBD_Tipo_DiscoTag	= TIPO_DISCOTAG,
	EtiquetaBD_Tipo_NombreTag	= TIPO_NOMBRETAG
};


class EtiquetaBD {
  public:
						EtiquetaBD(void) : Tipo(0), Nota(0.0f), Tiempo(0), Medios(0)						{ };
						EtiquetaBD(std::wstring &nTexto, const DWORD nTipo, const float nNota, const libvlc_time_t nTiempo) : Tipo(nTipo), Texto(nTexto), Nota(nNota), Tiempo(nTiempo), Medios(1)	{ };
					   ~EtiquetaBD(void)																	{ };

	inline const DWORD	AgregarTipo(const EtiquetaBD_Tipo nTipo)											{ Tipo |= nTipo;	return Tipo;			}
	inline const DWORD	AgregarTipo(const DWORD nTipo)														{ Tipo |= nTipo;	return Tipo;			}
	inline const BOOL   EsGenero(void)																		{ return ((Tipo & TIPO_GENERO) > 0);		}
	inline const BOOL   EsGrupoPath(void)																	{ return ((Tipo & TIPO_GRUPOPATH) > 0);		}
	inline const BOOL   EsDiscoPath(void)																	{ return ((Tipo & TIPO_DISCOPATH) > 0);		}
	inline const BOOL   EsNombrePath(void)																	{ return ((Tipo & TIPO_NOMBREPATH) > 0);	}
	inline const BOOL   EsGrupoTag(void)																	{ return ((Tipo & TIPO_GRUPOTAG) > 0);		}
	inline const BOOL   EsDiscoTag(void)																	{ return ((Tipo & TIPO_DISCOTAG) > 0);		}
	inline const BOOL   EsNombreTag(void)																	{ return ((Tipo & TIPO_NOMBRETAG) > 0);		}
	DWORD				Tipo;

//	std::wstring       &SqlEtiquetas(void);

	std::wstring		Texto;

	UINT                Medios;
	libvlc_time_t       Tiempo;
	float               Nota;		// Suma de todas las notas, (para extraer la media hay que dividir la Nota por Medios.size())
};
