#pragma once

namespace DWL {
	class DListaIconos;

	class DListaIconos_Icono {
	  public:
								DListaIconos_Icono(void) : _ID(0) { };
								DListaIconos_Icono(HICON nIcono, const INT_PTR nID, const int nAncho, const int nAlto) : _Icono(nIcono), _ID(nID), _Ancho(nAncho), _Alto(nAlto), _IDStrPos(0) { };
								DListaIconos_Icono(HICON nIcono, const INT_PTR nID, const int nAncho, const int nAlto, std::wstring &nIDStr, const int nIDStrPos) : _Icono(nIcono), _ID(nID), _Ancho(nAncho), _Alto(nAlto), _IDStr(nIDStr), _IDStrPos(nIDStrPos) { };
						       ~DListaIconos_Icono(void) { };
		inline HICON			Icono(void)				{ return _Icono; }
		inline const INT_PTR	ID(void)                { return _ID;    }
		inline HICON			operator()(void)		{ return _Icono; }
		inline const int		Ancho(void)             { return _Ancho; }
		inline const int		Alto(void)              { return _Alto;  }

	  protected :
		HICON				   _Icono;
		INT_PTR			       _ID;
		int			           _Ancho;
		int				       _Alto;
		int					   _IDStrPos;
		std::wstring	       _IDStr;

		friend class DListaIconos;
	};
}