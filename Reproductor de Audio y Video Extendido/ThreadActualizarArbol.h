#pragma once

#include "RaveBD.h"
#include <mutex>

class ThreadActualizarArbol {
  public:
							ThreadActualizarArbol(void);
				           ~ThreadActualizarArbol(void);
							// Inicia la actualización del arbol de la base de datos en un thread nuevo
    const BOOL	           	Iniciar(HWND nhWndDest);
	void                    Terminar(void);
	void                    Cancelar(void);
  protected:
	static unsigned long   _ThreadActualizar(void *pThis);
	const UINT             _EscanearDirectorio(std::wstring &nPath, BDRaiz *Raiz);
	const BOOL             _EsNombreValido(const wchar_t *nNombre);
	std::mutex             _Mutex;
	BOOL			       _Cancelar;
	HWND				   _VentanaPlayer;
	HANDLE				   _Thread;
	RaveBD                 _BD;
};

