#ifndef DICONOP_H
#define DICONOP_H

#include <vector>
#include <string>

namespace DWL {
	// Versión para GDI+

	// Se ha diseñado para cargar no cargar 20 veces el mismo icono de forma que guarda todos los iconos en memória, y si vamos a crear un icono que ya existe nos devuelve el existente si necesidad de crear uno nuevo.
	class DIconoP {
	  protected:
		// Datos de un icono
		class DIconoP_Datos {
		  public:
									DIconoP_Datos(void) : hIcono(NULL), Icono(NULL), ID(0), Ancho(0), Alto(0), IDStrPos(0) { };
									DIconoP_Datos(HICON nhIcono, Gdiplus::Bitmap *nIcono, const INT_PTR nID, const int nAncho, const int nAlto) : hIcono(nhIcono), Icono(nIcono), ID(nID), Ancho(nAncho), Alto(nAlto), IDStrPos(0) { };
									DIconoP_Datos(HICON nhIcono, Gdiplus::Bitmap *nIcono, const INT_PTR nID, const int nAncho, const int nAlto, std::wstring &nIDStr, const int nIDStrPos) : hIcono(nhIcono), Icono(nIcono), ID(nID), Ancho(nAncho), Alto(nAlto), IDStrPos(nIDStrPos), IDStr(nIDStr) { };
								   ~DIconoP_Datos(void) { };
			//HICON				    Icono;
			Gdiplus::Bitmap		   *Icono;
			HICON                   hIcono;
			INT_PTR			        ID;			// ID del icono (positiva si es un icono de los recursos, negativa si es un icono externo o un icono KnownFolder
			int			            Ancho;
			int				        Alto;
			int					    IDStrPos;	// Posición del icono externo (un archivo de iconos externo puede tener varios iconos dentro)
			std::wstring	        IDStr;		// Si es un icono externo ademas de la id tiene el path
		};

	  public:
												DIconoP(void);
												DIconoP(const INT_PTR IDRecursos, const int nAncho, const int nAlto);
												DIconoP(const GUID &rfid, const int nAncho, const int nAlto);
												DIconoP(const wchar_t *Path, const int nAncho, const int nAlto, const int nPosicionIco = 0);
											   ~DIconoP(void);
		void									CrearIconoRecursos(const INT_PTR IDRecursos, const int nAncho, const int nAlto);
		void									CrearIconoKnownFolder(const GUID &rfid, const int nAncho, const int nAlto);
		void									CrearIconoExterno(const wchar_t *Path, const int nAncho, const int nAlto, const int nPosicionIco = 0);

		inline Gdiplus::Bitmap				   *Icono(void)				{ return (_Icono == NULL) ? NULL	: _Icono->Icono; }
		inline const INT_PTR					ID(void)                { return (_Icono == NULL) ? 0		: _Icono->ID;    }
		inline Gdiplus::Bitmap				   *operator()(void)		{ return (_Icono == NULL) ? 0		: _Icono->Icono; }
		inline const int						Ancho(void)             { return (_Icono == NULL) ? 0		: _Icono->Ancho; }
		inline const int						Alto(void)              { return (_Icono == NULL) ? 0		: _Icono->Alto;  }

		static void                             EliminarIconos(void);
	  protected:
		Gdiplus::Bitmap                       *_CrearBitmap(HICON nIcono);
		DIconoP_Datos                         *_Icono;
												// Busca un icono KnownFolder o un icono externo
		static DIconoP_Datos	     		  *_BuscarIDStr(const wchar_t *nIDStr, const int nPosicionStr);
		static std::vector <DIconoP_Datos *>   _Iconos;
		static INT_PTR                         _IDNegativa; // ID negativa para los iconos KnowFolder o externos, que se va incrementando por cada uno de ellos
	};
};
#endif