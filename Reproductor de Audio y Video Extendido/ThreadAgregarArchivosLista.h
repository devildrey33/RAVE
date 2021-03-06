#pragma once

#include "RaveBD.h"
#include <mutex>


class ThreadAgregarArchivosLista {
  public:
								ThreadAgregarArchivosLista(void);
	                           ~ThreadAgregarArchivosLista(void);
								// Inicia la actualización del arbol de la base de datos en un thread nuevo
    const BOOL	           		Iniciar(HWND nhWndDest, std::vector<std::wstring> &Paths);
	void						Terminar(void);
	void						Cancelar(const BOOL nCancelar = TRUE);
	HANDLE                      Thread(void);
  protected:
	static unsigned long       _ThreadAgregarArchivosLista(void *pThis);
	const UINT                 _EscanearDirectorio(std::wstring &nPath);
	const BOOL                 _EsNombreValido(const wchar_t *nNombre);

	std::mutex                 _Mutex;
	BOOL			           _Cancelar;
	HWND				       _VentanaPlayer;
	HANDLE				       _Thread;
	RaveBD                     _BD;
	DWL::DUnidadesDisco        _Unidades;
	std::vector<std::wstring>  _Paths;
};

