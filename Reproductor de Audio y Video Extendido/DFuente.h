#ifndef DFUENTE_H
#define DFUENTE_H

#include <vector>
#include <string>

namespace DWL {

	class DFuente_Datos {
      public :
							DFuente_Datos(void) : Tam(0), Negrita(FALSE), Cursiva(FALSE), Subrayado(FALSE), Tachado(FALSE), Fuente(NULL), Alto(0) { }
							DFuente_Datos(const int nTam, std::wstring &nNombre, const BOOL nNegrita, const BOOL nCursiva, const BOOL nSubrayado, const BOOL nTachado, HFONT nFuente) : Tam(nTam), Nombre(nNombre), Negrita(nNegrita), Cursiva(nCursiva), Subrayado(nSubrayado), Tachado(nTachado), Fuente(nFuente), Alto(0) {
								Alto = ObtenerTamTexto(L"W").cy;
							}
							// Obtiene el tamaño del texto especificado, utilizando la fuente de esta clase
		const SIZE			ObtenerTamTexto(const TCHAR *nTexto);
							// Obtiene el tamaño del string especificado, utilizando la fuente de esta clase
		const SIZE			ObtenerTamTexto(std::wstring &nTexto);
		
		int					Tam;
		std::wstring		Nombre;
		BOOL				Negrita;
		BOOL				Cursiva;
		BOOL				Subrayado;
		BOOL                Tachado;
		HFONT				Fuente;
		int                 Alto;
	};

	// Objeto para controlar las fuentes que se usan en la aplicación
	// Para no tener mil fuentes iguales cargadas para distintos controles, este objeto guarda una lista con todas las fuentes creadas, y si se intenta crear una fuente que ya existia, se devuelve la existente.
	class DFuente {
	   public:	
											DFuente(void);										
		                                   ~DFuente(void);
											// Función que crea la fuente especificada, si no se ha creado una igual antes. Si ya existe una fuente con estas caracteristicas adopta el HFONT de ella.
		void								CrearFuente(const int nTam, const wchar_t *nNombre, const BOOL nNegrita = FALSE, const BOOL nCursiva = FALSE, const BOOL nSubrayado = FALSE, const BOOL nTachado = FALSE);
											// Funciones para obtener los valores de la fuente
		inline const int					Tam(void)			{ return _Fuentes[_FuentePos].Tam;				};
		inline const wchar_t	           *Nombre(void)		{ return _Fuentes[_FuentePos].Nombre.c_str();	};
		inline const BOOL					Negrita(void)		{ return _Fuentes[_FuentePos].Negrita;			};
		inline const BOOL					Cursiva(void)		{ return _Fuentes[_FuentePos].Cursiva;			};
		inline const BOOL					Subrayado(void)		{ return _Fuentes[_FuentePos].Subrayado;		};
		inline const BOOL					Tachado(void)		{ return _Fuentes[_FuentePos].Tachado;			};
											// Funciones para modificar la fuente (realmente se crea una nueva fuente)
		void								Tam(const int nTam);
		void								Nombre(const wchar_t *nNombre);
		void								Negrita(const BOOL nNegrita);
		void								Cursiva(const BOOL nCursiva);
		void								Subrayado(const BOOL nSubrayado);
		void								Tachado(const BOOL nTachado);
											// Devuelve la altura en pixeles de la fuente
		inline const int					Alto(void)			{ return _Fuentes[_FuentePos].Alto; }
											// Fuente para utilizar al pintar las ventanas / controles
		inline HFONT						operator() (void)	{ return _Fuentes[_FuentePos].Fuente; }
		inline HFONT						Fuente(void)        { return _Fuentes[_FuentePos].Fuente; }

											// Obtiene el tamaño del texto especificado, utilizando la fuente de esta clase
		const SIZE							ObtenerTamTexto(const TCHAR *nTexto);
											// Obtiene el tamaño del string especificado, utilizando la fuente de esta clase
		const SIZE							ObtenerTamTexto(std::wstring &nTexto);

											// Elimina todas las fuentes estaticas de la memoria
		static void							EliminarFuentes(void);

	  protected:								
		/* TODO : pasar el LONG a DFuente_Datos * per evitar el operador [] del vector (pero ja aviso que peta.. i no se per que...) */
		LONG                               _FuentePos;
//		DFuente_Datos                     *_Fuente;

		static LONG                        _BuscarFuente(const int nTam, std::wstring &nNombre, const BOOL nNegrita, const BOOL nCursiva, const BOOL nSubrayado, const BOOL nTachado);
//		static DFuente_Datos              *_BuscarFuente(const int nTam, std::wstring &nNombre, const BOOL nNegrita, const BOOL nCursiva, const BOOL nSubrayado, const BOOL nTachado);
		static std::vector<DFuente_Datos>  _Fuentes;
	};
}

#endif