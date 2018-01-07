#pragma once

namespace DWL {
	class DListaIconos;

	class DListaIconos_Icono {
	  public:
							DListaIconos_Icono() : _ID(0) { };
							DListaIconos_Icono(HICON nIcono, const int nID, const int nAncho, const int nAlto) : _Icono(nIcono), _ID(nID), _Ancho(nAncho), _Alto(nAlto), _IDStrPos(0) { };
							DListaIconos_Icono(HICON nIcono, const int nID, const int nAncho, const int nAlto, std::wstring &nIDStr, const int nIDStrPos) : _Icono(nIcono), _ID(nID), _Ancho(nAncho), _Alto(nAlto), _IDStr(nIDStr), _IDStrPos(nIDStrPos) { };
						   ~DListaIconos_Icono()	{ };
		inline HICON		Icono(void)				{ return _Icono; }
		inline const int	ID(void)                { return _ID; }
		inline HICON		operator()(void)		{ return _Icono; }
	  protected :
		HICON			   _Icono;
		int			       _ID;
		int		           _Ancho;
		int			       _Alto;
		int				   _IDStrPos;
		std::wstring	   _IDStr;

		friend class DListaIconos;
	};
}