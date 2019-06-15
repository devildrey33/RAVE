#include "RAVE_LA.h"
#include <DDirectoriosWindows.h>

#define MUTEX_RAVE_LA L"Mutex_RAVE_LA"

void RAVE_LA::Iniciar(void) {
	// Obtengo/Creo un mutex para evitar que 2 o mas reproductores se inicien a la vez.
	// De esta forma cuando se inicia el reproductor, este espera a que el anterior termine completamente de cargarse.
	MutexLA = OpenMutex(NULL, FALSE, MUTEX_RAVE_LA);
	if (MutexLA == NULL) {
		// Creo un mutex para ejecutar ordenadamente otras instancias de este mismo reproductor
		MutexLA = CreateMutex(NULL, FALSE, MUTEX_RAVE_LA);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Prevengo que otros reproductores se inicien durante la inicialización del reproductor principal. //
	DWORD r = WaitForSingleObject(MutexLA, INFINITE);                                                   //
	//////////////////////////////////////////////////////////////////////////////////////////////////////


	// Creo la consola para mensajes de depuración 
	#ifdef RAVE_MOSTRAR_CONSOLA
		ConsolaDebug.Crear(L"Consola de depuración");
		Debug_Escribir(L"RAVE_LA::Iniciar\n");
	#endif

	// Obtengo el directorio y el path completo del reproductor en ProgramData
	std::wstring AppPath, ExePath;
	DWL::DDirectoriosWindows::Comun_AppData(AppPath);
	AppPath += L"\\RAVE";
	ExePath = L"\"" + AppPath + L"\\Reproductor de Audio y Video Extendido.exe\"";



	// Compruebo si hay alguna actualización por instalar
	// ...



	// Aseguro que el directorio actual sea el del player en el ProgramData del SO
	SetCurrentDirectory(AppPath.c_str());

	// Obtengo la línea de comandos para poder pasarla al reproductor
	int		     TotalArgs = 0;
	TCHAR      **Args      = CommandLineToArgvW(GetCommandLine(), &TotalArgs);
	std::wstring Params;
	for (int i = 1; i < TotalArgs; i++) {
		Params += Args[i];
		Params += L" ";
	}
	// Libero la memória de CommandLineToArgvW
	if (TotalArgs > 0) LocalFree(Args);

	ShellExecute(NULL, L"Open", ExePath.c_str(), Params.c_str(), AppPath.c_str(), SW_SHOW);
	



	







	// Libero el mutex que impide la carga de un supuesto segundo reproductor
	ReleaseMutex(MutexLA);
	CloseHandle(MutexLA);
}
