#pragma once

#include "RaveBD.h"
#include <mutex>
#include "DhWnd.h"
#include "DBarraProgresoEx.h"
#include "DBotonEx.h"
#include "DMarcaEx.h"

class ThreadObtenerMetadatos :	public DWL::DVentana {
  public:
							    ThreadObtenerMetadatos();
	                           ~ThreadObtenerMetadatos();
    const BOOL	           		Iniciar(HWND nhWndDest);
	void						Terminar(void);
	void						Cancelar(const BOOL nCancelar);
	const BOOL                  Cancelar(void);

	void						Pintar(HDC DC);
	LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
  protected:
	void					   _Evento_Pintar(void);
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


	DWL::DBarraProgresoEx      _BarraProgreso;
	DWL::DBotonEx              _BotonCancelar;
	DWL::DMarcaEx              _MarcaNoMostrarMas;
};

