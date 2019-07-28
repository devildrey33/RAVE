#pragma once

#pragma comment(lib, "wininet.lib")	// Agrego la libreria wininet.lib al proyecto

#include <mutex>


// Objeto dise�ado para buscar y descargar actualizaciones para este reproductor
class Actualizaciones {
  public:
							// Constructor
							Actualizaciones(void) : _hThreadBuscar(NULL), _hThreadDescargar(NULL) { };
							// Funci�n que busca una nueva actualizaci�n
	const BOOL				Buscar(void);
							// Funci�n que descarga la nueva actualizacion
	const BOOL				Descargar();
							// Funci�n que permite asignar el estado cancelado tanto para la busqueda, como para la descarga
	static void             Cancelar(const BOOL nCancelar);
							// Funci�n que obtiene el estado cancelado para la busqueda y la descarga
	static const BOOL       Cancelar(void);
  protected:
							// Funci�n main para el hilo de buscar una actualizacion
	static unsigned long   _ThreadBuscar(void* pThis);
							// Funci�n main para el hilo de descargar la actualizacion
	static unsigned long   _ThreadDescargar(void* pThis);
							// Variable que determina si hay que cancelar la busqueda o la descarga
	static BOOL            _Cancelar;
							// Handle para el thread buscar
	HANDLE                 _hThreadBuscar;
							// Handle para el thread descargar
	HANDLE                 _hThreadDescargar;
							// Mutex para los threads buscar y descargar
	static std::mutex      _Mutex;
							// HWND de la ventana principal del reproductor
	static HWND            _VentanaRave;
	static std::wstring    _Version;
};

