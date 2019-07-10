#pragma once

#pragma comment(lib, "wininet.lib")	// Agrego la libreria wininet.lib al proyecto

#include <mutex>


// Objeto diseñado para buscar y descargar actualizaciones para este reproductor
class Actualizaciones {
  public:
							// Constructor
							Actualizaciones(void) : _Cancelar(FALSE), _hThreadBuscar(NULL), _hThreadDescargar(NULL) { };
							// Función que busca una nueva actualización
	const BOOL				Buscar(void);
							// Función que descarga la nueva actualizacion
	const BOOL				Descargar(void);
							// Función que permite asignar el estado cancelado tanto para la busqueda, como para la descarga
	void                    Cancelar(const BOOL nCancelar);
							// Función que obtiene el estado cancelado para la busqueda y la descarga
	const BOOL              Cancelar(void);
  protected:
							// Función main para el hilo de buscar una actualizacion
	static unsigned long   _ThreadBuscar(void* pThis);
							// Función main para el hilo de descargar la actualizacion
	static unsigned long   _ThreadDescargar(void* pThis);
	BOOL                   _Cancelar;
	HANDLE                 _hThreadBuscar;
	HANDLE                 _hThreadDescargar;
	std::mutex             _Mutex;
};

