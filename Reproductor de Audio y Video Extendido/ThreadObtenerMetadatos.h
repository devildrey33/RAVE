#pragma once

#include "RaveBD.h"
#include <mutex>

class ThreadObtenerMetadatos {
  public:
							    ThreadObtenerMetadatos();
	                           ~ThreadObtenerMetadatos();
    const BOOL	           		Iniciar(HWND nhWndDest);
	void						Terminar(void);
	void						Cancelar(const BOOL nCancelar);
	const BOOL                  Cancelar(void);
  protected:
	static unsigned long       _ThreadObtenerMetadatos(void *pThis);

	void                       _Parsear(libvlc_instance_t *VLC, std::wstring &Path);
	std::wstring               _ObtenerMeta(libvlc_media_t *Media, libvlc_meta_t Tipo);
	static void                _ParsearTerminado(const libvlc_event_t *event, void *user_data);

	std::mutex                 _Mutex;
	BOOL			           _Cancelar;
	HWND				       _VentanaPlayer;
	HANDLE				       _Thread;
	RaveBD                     _BD;
	std::vector<std::wstring>  _PorParsear;
};

