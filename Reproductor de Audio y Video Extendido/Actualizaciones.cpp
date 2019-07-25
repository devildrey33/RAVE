#include "stdafx.h"
#include "Actualizaciones.h"
#include <wininet.h>
#include <wincrypt.h>
#include <DStringUtils.h>

HWND			Actualizaciones::_VentanaRave = NULL;
std::wstring	Actualizaciones::_Version;

// Función que busca una nueva actualización
const BOOL Actualizaciones::Buscar(void) {
	// Ya se está buscando o descargando 
	if (_hThreadBuscar != NULL || _hThreadDescargar != NULL) return FALSE;

	// Asigno el estado cancelado a false
	Cancelar(FALSE);

	// Inicio el thread
	_hThreadBuscar = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->_ThreadBuscar, (void*)this, 0, NULL);

	// Si hay un error, salgo
	if (_hThreadBuscar == NULL) {
		Debug_Escribir(L"Actualizaciones::Buscar ERROR creando el thread.\n");
		return FALSE;
	}

	// Guardo el HWND de la ventana principal
	_VentanaRave = App.VentanaRave.hWnd();
	
	// Thread creado correctamente
	Debug_Escribir(L"Actualizaciones::Buscar thread iniciado.\n");
	return TRUE;
}

// Función que descarga la nueva actualizacion
const BOOL Actualizaciones::Descargar(void) {
	// Ya se está descargando 
	if (_hThreadDescargar != NULL) return FALSE;

	// Asigno el estado cancelado a false
	Cancelar(FALSE);

	// Inicio el thread
	_hThreadDescargar = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->_ThreadDescargar, (void*)this, 0, NULL);

	// Si hay un error, salgo
	if (_hThreadDescargar == NULL) {
		Debug_Escribir(L"Actualizaciones::Descargar ERROR creando el thread.\n");
		return FALSE;
	}

	// Thread creado correctamente
	Debug_Escribir(L"Actualizaciones::Descargar thread iniciado.\n");
	return TRUE;
}

// Función que permite asignar el estado cancelado tanto para la busqueda, como para la descarga
void Actualizaciones::Cancelar(const BOOL nCancelar) {
	_Mutex.lock();
	_Cancelar = nCancelar;
	_Mutex.unlock();
}

// Función que obtiene el estado cancelado para la busqueda y la descarga
const BOOL Actualizaciones::Cancelar(void) {
	_Mutex.lock();
	BOOL Ret = _Cancelar;
	_Mutex.unlock();
	return Ret;
}


// Función main para el hilo de buscar una actualizacion
unsigned long Actualizaciones::_ThreadBuscar(void* pThis) {
	HINTERNET		Sesion				= InternetOpen(TEXT("RAVE_Actualizacion"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
	HINTERNET		Peticion			= InternetOpenUrl(Sesion, TEXT("https://devildrey33.github.io/RAVE/ULTIMA_VERSION.txt"), NULL, 0, INTERNET_FLAG_RELOAD, 0);
	char			Txt[32]				= "";
	DWORD			BytesLeidos			= 0;
	DWORD			TotalBytesLeidos	= 0;
	DWORD			MaxBuffer			= sizeof(Txt) -1;
	std::string     ResultadoANSI;

	// Error en la petición (provablemente no va el servidor)
	if (Peticion == NULL) {
		InternetCloseHandle(Sesion);
		return 0;
	}

	// Leo el archivo
	BOOL Leido = InternetReadFile(Peticion, Txt, MaxBuffer, &BytesLeidos);
	if (BytesLeidos != 0) {
		Txt[BytesLeidos] = 0;
		ResultadoANSI = Txt;
	}

	// El archivo ULTIMA_VERSION.txt está vacio?!?!
	if (ResultadoANSI.size() == 0) {
		InternetCloseHandle(Peticion);
		InternetCloseHandle(Sesion);
		return 0;
	}

	// Devuelve un HTML con algún error (404 NOT FOUND, etc...)
	if (ResultadoANSI[0] == '<') {
		InternetCloseHandle(Peticion);
		InternetCloseHandle(Sesion);
		return 0; // no se ha encontrado el documento
	}

	// Paso la versión de ANSI a wchar_t
	DWL::Strings::AnsiToWide(ResultadoANSI.c_str(), _Version);

	// Si la versión no coincide
	if (_Version.compare(RAVE_VERSIONSTR) != 0) {
		// Informo a la ventana del reproductor que hay una nueva actualización
		SendMessage(_VentanaRave, WM_ACTUALIZACION_ENCONTRADA, reinterpret_cast<WPARAM>(_Version.c_str()), 0);
	}

	return 0;
}


// Función main para el hilo de descargar la actualizacion
unsigned long Actualizaciones::_ThreadDescargar(void* pThis) {

	return 0;
}
