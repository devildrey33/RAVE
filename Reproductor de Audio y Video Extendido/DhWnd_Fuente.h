#pragma once

//#include "DhWnd.h"

namespace DWL {
	class DhWnd_Fuente {
	  public :
							DhWnd_Fuente(void) : _Fuente(NULL), _Alto(0)							{ }
							DhWnd_Fuente(HFONT nFuente) : _Fuente(nFuente)							{ _Alto = Tam(L"W").cy; }
							DhWnd_Fuente(const DhWnd_Fuente &nFuente) : _Fuente(nFuente._Fuente)	{ _Alto = nFuente._Alto; }
		                   ~DhWnd_Fuente()															{ }

	    inline HFONT		Fuente(void)															{ return _Fuente; }
							// Devuelve la altura máxima que puede tener un caracter utilizando la fuente de esta clase
	    inline const int	Alto(void)																{ return _Alto; }

							// Operadores sobrecargados
		inline void	        operator = (HFONT nFuente)												{ _Fuente = nFuente; _Alto = Tam(L"W").cy; }
		inline void	        operator = (const DhWnd_Fuente &nFuente)								{ _Fuente = nFuente._Fuente; _Alto = nFuente._Alto; }
		inline HFONT        operator() (void)														{ return _Fuente; }

							// Obtiene el tamaño del texto especificado, utilizando la fuente de esta clase
		inline const SIZE   Tam(const TCHAR *nTexto) {
								HDC		Buffer = GetDC(NULL);
								SIZE	nTam = { 0, 0 };
								HFONT	VFuente = static_cast<HFONT>(SelectObject(Buffer, _Fuente));
								GetTextExtentPoint(Buffer, nTexto, static_cast<int>(wcslen(nTexto)), &nTam);
								SelectObject(Buffer, VFuente);
								ReleaseDC(NULL, Buffer);
								return nTam;
							}

							// Obtiene el tamaño del string especificado, utilizando la fuente de esta clase
		inline const SIZE   Tam(std::wstring &nTexto) {
								HDC		Buffer = GetDC(NULL);
								SIZE	nTam = { 0, 0 };
								HFONT	VFuente = static_cast<HFONT>(SelectObject(Buffer, _Fuente));
								GetTextExtentPoint(Buffer, nTexto.c_str(), static_cast<int>(nTexto.size()), &nTam);
								SelectObject(Buffer, VFuente);
								ReleaseDC(NULL, Buffer);
								return nTam;
							}
      protected :
		
		HFONT			   _Fuente;
		int				   _Alto;
	};

}