#pragma once

#include "DListaIconos_Icono.h"

namespace DWL {

	class DListaIconos {
	  public:
													DListaIconos(void);
												   ~DListaIconos(void);
		static DListaIconos_Icono				   *AgregarIconoRecursos(const int IDRecursos, const int nAncho, const int nAlto);
		static DListaIconos_Icono				   *AgregarIconoCSIDL(const int CSIDL, const int nAncho, const int nAlto);
		static DListaIconos_Icono				   *AgregarIconoExterno(const TCHAR *Path, const int nAncho, const int nAlto, const int nPosicionIco);
	  protected:
		static DListaIconos_Icono				  *_BuscarIDStr(const TCHAR *nIDStr, const int nPosicionStr);
		static int							       _IDNegativa;
		static std::vector <DListaIconos_Icono *>  _Iconos;
	};

}