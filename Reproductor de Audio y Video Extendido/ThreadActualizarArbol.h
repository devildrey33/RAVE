#pragma once

#include "RaveBD.h"
#include <mutex>
#include "RaveOpciones.h"

class ThreadActualizarArbol {
  public:
								ThreadActualizarArbol(void);
							   ~ThreadActualizarArbol(void);
								// Inicia la actualización del arbol de la base de datos en un thread nuevo
    const BOOL	           		Iniciar(HWND nhWndDest);
	void						Terminar(void);
	void						Cancelar(const BOOL nCancelar);
	const BOOL                  Cancelar(void);
	HANDLE                      Thread(void);
  protected:
/*	void                       _Parsear(libvlc_instance_t *VLC, std::wstring &Path);
	std::wstring               _ObtenerMeta(libvlc_media_t *Media, libvlc_meta_t Tipo);
	static void                _ParsearTerminado(const libvlc_event_t *event, void *user_data);*/

	static unsigned long       _ThreadActualizar(void *pThis);
	const UINT                 _EscanearDirectorio(std::wstring &nPath, BDRaiz *Raiz);
	const BOOL                 _EsNombreValido(const wchar_t *nNombre);
	std::mutex                 _Mutex;	
	BOOL			           _Cancelar;
	HWND				       _VentanaPlayer;
	HANDLE				       _Thread;
	RaveBD                     _BD;
	RaveOpciones               _Opciones;
	DWL::DUnidadesDisco        _Unidades;
	//std::vector<std::wstring>  _PorParsear;
};

