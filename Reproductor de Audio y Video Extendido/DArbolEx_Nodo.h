#pragma once

#include "DListaIconos_Icono.h"
#include <vector>

namespace DWL {

	#define ARBOLEX_PADDING					2 // Espacio entre el marco de la selección y el texto (tambien se utiliza para espaciar el expansor, el icono y el texto horizontalmente)
	#define ARBOLEX_MARGEN_Y_SELECCION		1 // Espacio entre la Y inicial / final del texto y el marco de seleccion
	#define ARBOLEX_TAMEXPANSOR				9 // OJO!! tiene que ser impar para quedar bien
	#define ARBOLEX_TAMICONO				16 // Tamaño del icono


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

	// Cuaquier cambio efectuado en esta clase, debe ser actualizado con la función Repintar del ArbolEx.
	class DArbolEx_Nodo {
	  public:
										DArbolEx_Nodo(void);
									   ~DArbolEx_Nodo(void);

//		inline std::wstring			   &Texto(void)															{ return _Texto;						}
//		inline const BOOL				Expandido(void)														{ return _Expandido;					}
//		inline void						Expandido(const BOOL nExpandido)									{ _Expandido = nExpandido;				}
		inline void						MostrarExpansor(const DArbolEx_MostrarExpansor nMostrarExpansor)	{ _MostrarExpansor = nMostrarExpansor;	}
		const BOOL						MostrarExpansor(void);
/*		inline const BOOL				Seleccionado(void)													{ return _Seleccionado;					}
		inline void						Seleccionado(const BOOL nSeleccionado)								{ _Seleccionado = nSeleccionado;		}*/

		inline DhWnd_Fuente			   &Fuente(void)														{ return *_Fuente; }
		inline void						Fuente(DhWnd_Fuente &nFuente)                                       { _Fuente = &nFuente;	_AnchoTexto = _Fuente->Tam(Texto).cx;	}

		inline DArbolEx_Nodo           *Padre(void)															{ return _Padre;						}
		inline DArbolEx_Nodo           *Siguiente(void)														{ return _Siguiente;					}
		inline DArbolEx_Nodo	       *Hijo(const size_t Pos)												{ return _Hijos[Pos];					}
		inline const size_t				TotalHijos(void)													{ return _Hijos.size();					}
		inline DArbolEx_Nodo           *UltimoHijo(void)													{ return (_Hijos.size() == 0) ? NULL : _Hijos[_Hijos.size() - 1]; }

		inline const int                IDIcono(void)														{ return _Icono->ID(); }
		void                            Icono(const int nIconoRecursos);
		//const size_t					Posicion(void);

		std::wstring				    Texto;

		BOOL						    Expandido;			// bits necesaris 1	
		BOOL						    Seleccionado;		// bits necesaris 1
	  protected:
		DArbolEx_MostrarExpansor       _MostrarExpansor;	// bits necesaris 4
		BOOL						   _SubSeleccionado;	// bits necesaris 1

		int                            _AnchoTexto;
		DListaIconos_Icono			  *_Icono;


		DArbolEx_Nodo			      *_Padre;
		size_t						   _Ancestros; // Total de padres por encima de este nodo
		DArbolEx_Nodo			      *_Siguiente;
		DArbolEx_Nodo			      *_Anterior;
		std::vector<DArbolEx_Nodo *>   _Hijos;
		DhWnd_Fuente				  *_Fuente;
		DArbolEx                      *_Arbol;
		friend class DArbolEx;
	};

}