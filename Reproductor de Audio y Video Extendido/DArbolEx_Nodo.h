#pragma once

#include "DListaIconos_Icono.h"

namespace DWL {

	enum DArbolEx_Nodo_Estado {
		DArbolEx_Nodo_Estado_Normal,
		DArbolEx_Nodo_Estado_Resaltado,
		DArbolEx_Nodo_Estado_Presdionado,
	};

	class DArbolEx;

#define DARBOLEX_NODO_EXPANDIDO    1
#define DARBOLEX_NODO_SELECCIONADO 2

/*	class DArbolEx_Nodo_Colores {
	  public:
		DArbolEx_Nodo_Colores() { };
		~DArbolEx_Nodo_Colores() { };
	};*/

	enum DArbolEx_MostrarExpansor {
		DArbolEx_MostrarExpansor_Auto,
		DArbolEx_MostrarExpansor_Mostrar,
		DArbolEx_MostrarExpansor_Ocultar
	};

	class DArbolEx_Nodo {
	  public:
										DArbolEx_Nodo(void);
									   ~DArbolEx_Nodo(void);

		inline std::wstring			   &Texto(void)															{ return _Texto;						}
		inline const BOOL				Expandido(void)														{ return _Expandido;					}
//		inline void						Expandido(const BOOL nExpandido)									{ _Expandido = nExpandido;				}
		inline void						MostrarExpansor(const DArbolEx_MostrarExpansor nMostrarExpansor)	{ _MostrarExpansor = nMostrarExpansor;	}
		const BOOL						MostrarExpansor(void);
		inline const BOOL				Seleccionado(void)													{ return _Seleccionado;					}
		inline void						Seleccionado(const BOOL nSeleccionado)								{ _Seleccionado = nSeleccionado;		}

		inline DhWnd_Fuente			   &Fuente(void)														{ return *_Fuente; }
		inline void						Fuente(DhWnd_Fuente &nFuente)                                       { _Fuente = &nFuente; _AnchoTexto = _Fuente->Tam(_Texto).cx;	}

		inline DArbolEx_Nodo           *Padre(void)															{ return _Padre;						}
		inline DArbolEx_Nodo           *Siguiente(void)														{ return _Siguiente;					}
		inline DArbolEx_Nodo	       *Hijo(const size_t Pos)												{ return _Hijos[Pos];					}
		inline const size_t				TotalHijos(void)													{ return _Hijos.size();					}

		//const size_t					Posicion(void);

	  protected:
		std::wstring				   _Texto;
		int                            _AnchoTexto;
		DListaIconos_Icono			  *_Icono;


		// bitwitzar
		DArbolEx_MostrarExpansor       _MostrarExpansor;	// 4
		BOOL						   _Expandido;			// 1 
		BOOL						   _Seleccionado;		// 1

		DArbolEx_Nodo			      *_Padre;
		size_t						   _Ancestros; // Total de padres por encima de este nodo
		DArbolEx_Nodo			      *_Siguiente;
		std::vector<DArbolEx_Nodo *>   _Hijos;
		DhWnd_Fuente				  *_Fuente;
		DArbolEx                      *_Arbol;
		friend class DArbolEx;
	};

}