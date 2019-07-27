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
	const BOOL				Descargar();
							// Función que permite asignar el estado cancelado tanto para la busqueda, como para la descarga
	void                    Cancelar(const BOOL nCancelar);
							// Función que obtiene el estado cancelado para la busqueda y la descarga
	const BOOL              Cancelar(void);
  protected:
							// Función main para el hilo de buscar una actualizacion
	static unsigned long   _ThreadBuscar(void* pThis);
							// Función main para el hilo de descargar la actualizacion
	static unsigned long   _ThreadDescargar(void* pThis);
							// Variable que determina si hay que cancelar la busqueda o la descarga
	BOOL                   _Cancelar;
							// Handle para el thread buscar
	HANDLE                 _hThreadBuscar;
							// Handle para el thread descargar
	HANDLE                 _hThreadDescargar;
							// Mutex para los threads buscar y descargar
	std::mutex             _Mutex;
							// HWND de la ventana principal del reproductor
	static HWND            _VentanaRave;
	static std::wstring    _Version;
};

