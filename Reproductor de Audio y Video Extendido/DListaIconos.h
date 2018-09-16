#pragma once

#include "DListaIconos_Icono.h"

namespace DWL {

	class DListaIconos {
	  public:
													DListaIconos(void);
												   ~DListaIconos(void);
		static DListaIconos_Icono				   *AgregarIconoRecursos(const INT_PTR IDRecursos, const int nAncho, const int nAlto);
		//static DListaIconos_Icono				   *AgregarIconoCSIDL(const int CSIDL, const int nAncho, const int nAlto); // DEPRECATED
		static DListaIconos_Icono				   *AgregarIconoKnownFolder(const GUID &rfid, const int nAncho, const int nAlto);
		static DListaIconos_Icono				   *AgregarIconoExterno(const wchar_t *Path, const int nAncho, const int nAlto, const int nPosicionIco = 0);
	  protected:
		static DListaIconos_Icono				  *_BuscarIDStr(const wchar_t *nIDStr, const int nPosicionStr);
		static INT_PTR						       _IDNegativa;
		static std::vector <DListaIconos_Icono *>  _Iconos;
	};

}