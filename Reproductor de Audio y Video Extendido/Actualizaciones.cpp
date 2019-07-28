#include "stdafx.h"
#include "Actualizaciones.h"
#include <wininet.h>
#include <wincrypt.h>
#include <DStringUtils.h>
#include <DDirectoriosWindows.h>
#include <DArchivoBinario.h>

HWND			Actualizaciones::_VentanaRave = NULL;
std::wstring	Actualizaciones::_Version;
BOOL			Actualizaciones::_Cancelar = FALSE;
std::mutex		Actualizaciones::_Mutex;

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

	// Creo la ventana
	App.VentanaAct.Crear(_Version.c_str());


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

#ifdef _WIN64
	#define URL_INSTALADOR		L"https://devildrey33.github.io/RAVE/Instaladores/Instalador%20x64/SetupRave-x64."
	#define NOMBRE_INSTALADOR	L"SetupRave-x64.msi"
#else
	#define URL_INSTALADOR		L"https://devildrey33.github.io/RAVE/Instaladores/Instalador%20x86/SetupRave-x86."
	#define NOMBRE_INSTALADOR	L"SetupRave-x86.msi"
#endif


// Función main para el hilo de descargar la actualizacion
unsigned long Actualizaciones::_ThreadDescargar(void* pThis) {

	std::wstring	UrlInstalador		= URL_INSTALADOR L"msi";
	std::wstring	UrlMD5				= URL_INSTALADOR L"md5";

	TCHAR			szHead[]			= L"Accept: */*\r\n\r\n";
	HINTERNET		Sesion				= InternetOpen(L"RAVE_Actualizacion", INTERNET_OPEN_TYPE_PRECONFIG, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
	HINTERNET		Peticion			= InternetOpenUrl(Sesion, UrlInstalador.c_str(), szHead, 0, INTERNET_FLAG_RELOAD, 0);
//	DWORD			Longitud			= 0;
	DWORD			Descargado			= 64;
	DWORD           TotalDescargado		= 0;
	char		    Datos[4097];
	DWORD			TotalDatos			= 0;
	TCHAR			TotalDatosStr[64];
	BOOL			Ret					= HttpQueryInfo(Peticion, HTTP_QUERY_CONTENT_LENGTH, (LPVOID)TotalDatosStr, &Descargado, (LPDWORD)0);
	
	if (Ret == TRUE) TotalDatos = _wtol(TotalDatosStr);
	
	if (TotalDatos == 0) {
		InternetCloseHandle(Peticion);
		InternetCloseHandle(Sesion);
		return -1;
	}

	float Valor = 0.0f;
	SendMessage(_VentanaRave, WM_ACTUALIZACION_BARRA, reinterpret_cast<WPARAM&>(Valor), NULL);

	Descargado = 0;

	std::wstring PathFinal;  
	DWL::DDirectoriosWindows::Comun_AppData(PathFinal);	
	PathFinal += L"\\Rave\\" NOMBRE_INSTALADOR;

	DWL::DArchivoBinario Archivo(PathFinal, TRUE);
	
	BOOL P = FALSE;
	while (TRUE) {
		Valor = static_cast<float>(TotalDescargado) / static_cast<float>(TotalDatos);
		SendMessage(_VentanaRave, WM_ACTUALIZACION_BARRA, reinterpret_cast<WPARAM&>(Valor), NULL);
		P = InternetReadFile(Peticion, (LPVOID)Datos, 4096, &Descargado);
        if (P == FALSE) {
			Debug_MostrarUltimoError();
//            ReleaseMutex(Mutex);
			break;
		}

		if (Cancelar() == TRUE) {
			break;
		}
//        ReleaseMutex(Mutex);
		Datos[Descargado] = '\0';
		TotalDescargado += Descargado;
		if (Descargado == 0)	
			break;
		else					
			Archivo.Guardar(Datos, Descargado);
	}
    
	
	InternetCloseHandle(Peticion);
	// Leo el hash que tiene la web
	Peticion = InternetOpenUrl(Sesion, UrlMD5.c_str(), szHead, 0, INTERNET_FLAG_RELOAD, 0);
	char  TxtHash[33]	= "";
	DWORD BytesLeidos	= 0;
	BOOL  Leido			= InternetReadFile(Peticion, TxtHash, 32, &BytesLeidos);


	InternetCloseHandle(Peticion);
	InternetCloseHandle(Sesion);

	// Tiene el mismo md5
	if (Archivo.MD5_char().compare(TxtHash) == 0) {
		// Actualizacion descargada
		SendMessage(_VentanaRave, WM_ACTUALIZACION_DESCARGADA, reinterpret_cast<WPARAM>(_Version.c_str()), 0);
	}
	// Cancelado o error
	else {
		if (Cancelar() == false) {
			// Error
			SendMessage(_VentanaRave, WM_ACTUALIZACION_ERROR, reinterpret_cast<WPARAM>(_Version.c_str()), 0);
		}
		else {
			SendMessage(_VentanaRave, WM_ACTUALIZACION_CANCELADA, reinterpret_cast<WPARAM>(_Version.c_str()), 0);
		}
	}


	return 0;
}
