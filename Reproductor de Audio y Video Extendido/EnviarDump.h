#pragma once

#include "DBarraProgresoEx.h"
#include <winsock2.h>

class EnviarDump {
  public:
					EnviarDump();
		           ~EnviarDump();
	const BOOL		Enviar(std::wstring &Path, DWL::DBarraProgresoEx &Barra);
  protected:
	const BOOL	 	Revisar(int iStatus, char *szFunction);
	std::string   &_base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	BOOL	       _Error;

	const BOOL	   _Enviar(const char *eTxt, const BOOL nRecibir = FALSE);
	const BOOL	   _EnviarNC(const char *eTxt);
	const BOOL     _Recibir(void);

	SOCKET         _Server;
};

