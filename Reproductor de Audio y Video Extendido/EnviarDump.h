#pragma once

#include "DBarraProgresoEx.h"
#include "DBarraTareas.h"
#include <winsock2.h>

#define WM_THREAD_ENVIARVALOR  WM_USER + 1000		// Se ha actualizado el progreso
#define WM_THREAD_TERMINADO    WM_USER + 1001		// Se ha terminado el thread

class EnviarDump_Datos {
  public :
					EnviarDump_Datos(std::wstring &nPath, HWND nhWnd) : Path(nPath), hWnd(nhWnd)	{ }
				   ~EnviarDump_Datos(void)											  				{ }
	std::wstring	Path;
	HWND			hWnd;
};

class EnviarDump {
  public:
							EnviarDump(void);
		                   ~EnviarDump(void);
	const BOOL				Enviar(std::wstring &Path, HWND hWnd);
  protected:
	static unsigned long   _ThreadEnviar(void *pThis);

	const BOOL	 			Revisar(int iStatus, char *szFunction);
	static std::string    &_base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	static BOOL	           _Error;

	static const BOOL	   _Enviar(const char *eTxt, const BOOL nRecibir = FALSE);
	static const BOOL	   _EnviarNC(const char *eTxt);
	static const BOOL      _Recibir(void);

	static SOCKET          _Server;

	HANDLE				   _Thread;
};

