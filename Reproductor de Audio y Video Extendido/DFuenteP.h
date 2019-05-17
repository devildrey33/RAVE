#ifndef DFUENTEP_H
#define DFUENTEP_H

#include <vector>
#include <string>

namespace DWL {
	// Versi�n para GDI +

	// Objeto para controlar las fuentes que se usan en la aplicaci�n
	// Para no tener mil fuentes iguales cargadas para distintos controles, este objeto guarda una lista con todas las fuentes creadas, y si se intenta crear una fuente que ya existia, se devuelve la existente.
	class DFuenteP {
	   protected:
		    // Datos para una fuente
			class DFuenteP_Datos {
			  public :
									DFuenteP_Datos(void) : Tam(0), Negrita(FALSE), Cursiva(FALSE), Subrayado(FALSE), Tachado(FALSE), Fuente(NULL), Alto(0) { }
									DFuenteP_Datos(const int nTam, std::wstring &nNombre, const BOOL nNegrita, const BOOL nCursiva, const BOOL nSubrayado, const BOOL nTachado, Gdiplus::Font *nFuente) : Tam(nTam), Nombre(nNombre), Negrita(nNegrita), Cursiva(nCursiva), Subrayado(nSubrayado), Tachado(nTachado), Fuente(nFuente), Alto(0) {
										Alto = ObtenerTamTexto(L"W").cy;
									}
								   ~DFuenteP_Datos(void) { if (Fuente != NULL) delete Fuente;  }
									// Obtiene el tama�o del texto especificado, utilizando la fuente de esta clase y su propio DC
				const SIZE			ObtenerTamTexto(const TCHAR *nTexto);
									// Obtiene el tama�o del string especificado, utilizando la fuente de esta clase y su propio DC
				const SIZE			ObtenerTamTexto(std::wstring &nTexto);
									// Obtiene el tama�o del texto especificado. El DC especificado debe tener seleccionada esta fuente.
				const SIZE			ObtenerTamTexto(HDC DC, const TCHAR *nTexto);
									// Obtiene el tama�o del string especificado. El DC especificado debe tener seleccionada esta fuente.
				const SIZE			ObtenerTamTexto(HDC DC, std::wstring &nTexto);
		
				int					Tam;
				std::wstring		Nombre;
				BOOL				Negrita;
				BOOL				Cursiva;
				BOOL				Subrayado;
				BOOL                Tachado;
				Gdiplus::Font	   *Fuente;
				int                 Alto;
			};
	   public:	
												DFuenteP(void);										
												DFuenteP(const int nTam, const wchar_t *nNombre, const BOOL nNegrita = FALSE, const BOOL nCursiva = FALSE, const BOOL nSubrayado = FALSE, const BOOL nTachado = FALSE);
		                                       ~DFuenteP(void);
												// Funci�n que crea la fuente especificada, si no se ha creado una igual antes. Si ya existe una fuente con estas caracteristicas adopta el HFONT de ella.
		void									CrearFuente(const int nTam, const wchar_t *nNombre, const BOOL nNegrita = FALSE, const BOOL nCursiva = FALSE, const BOOL nSubrayado = FALSE, const BOOL nTachado = FALSE);
												// Funciones para obtener los valores de la fuente
		inline const int						Tam(void)			{ return (_Fuente == NULL) ? 0 : _Fuente->Tam;					};
		inline const wchar_t	               *Nombre(void)		{ return (_Fuente == NULL) ? L"" : _Fuente->Nombre.c_str();		};
		inline const BOOL						Negrita(void)		{ return (_Fuente == NULL) ? FALSE : _Fuente->Negrita;			};
		inline const BOOL						Cursiva(void)		{ return (_Fuente == NULL) ? FALSE : _Fuente->Cursiva;			};
		inline const BOOL						Subrayado(void)		{ return (_Fuente == NULL) ? FALSE : _Fuente->Subrayado;		};
		inline const BOOL						Tachado(void)		{ return (_Fuente == NULL) ? FALSE : _Fuente->Tachado;			};
												// Funciones para modificar la fuente (realmente se crea una nueva fuente)
		void									Tam(const int nTam);
		void									Nombre(const wchar_t *nNombre);
		void									Negrita(const BOOL nNegrita);
		void									Cursiva(const BOOL nCursiva);
		void									Subrayado(const BOOL nSubrayado);
		void									Tachado(const BOOL nTachado);
												// Devuelve la altura en pixeles de la fuente
		inline const int						Alto(void)			{ return (_Fuente == NULL) ? 0 : _Fuente->Alto; }
												// Fuente para utilizar al pintar las ventanas / controles
		inline Gdiplus::Font				   *operator() (void)	{ return (_Fuente == NULL) ? NULL : _Fuente->Fuente; }
		inline Gdiplus::Font				   *Fuente(void)        { return (_Fuente == NULL) ? NULL : _Fuente->Fuente; }

												// Obtiene el tama�o del texto especificado, utilizando la fuente de esta clase
		inline const SIZE						ObtenerTamTexto(const TCHAR *nTexto)			{ static SIZE Z = { 0, 0 };  return (_Fuente == NULL) ? Z : _Fuente->ObtenerTamTexto(nTexto); };
												// Obtiene el tama�o del string especificado, utilizando la fuente de esta clase
		inline const SIZE						ObtenerTamTexto(std::wstring &nTexto)			{ static SIZE Z = { 0, 0 };  return (_Fuente == NULL) ? Z : _Fuente->ObtenerTamTexto(nTexto); };
												// Obtiene el tama�o del texto especificado. El DC especificado debe tener seleccionada esta fuente.
		inline const SIZE						ObtenerTamTexto(HDC DC, const TCHAR *nTexto)	{ static SIZE Z = { 0, 0 };  return (_Fuente == NULL) ? Z : _Fuente->ObtenerTamTexto(DC, nTexto); };
												// Obtiene el tama�o del string especificado. El DC especificado debe tener seleccionada esta fuente.
		inline const SIZE						ObtenerTamTexto(HDC DC, std::wstring &nTexto)	{ static SIZE Z = { 0, 0 };  return (_Fuente == NULL) ? Z : _Fuente->ObtenerTamTexto(DC, nTexto); };


												// Elimina todas las fuentes estaticas de la memoria
		static void								EliminarFuentes(void);

	  protected:								
		DFuenteP_Datos                        *_Fuente;

		static DFuenteP_Datos                 *_BuscarFuente(const int nTam, std::wstring &nNombre, const BOOL nNegrita, const BOOL nCursiva, const BOOL nSubrayado, const BOOL nTachado);
		static std::vector<DFuenteP_Datos *>   _Fuentes;
	};
}

#endif