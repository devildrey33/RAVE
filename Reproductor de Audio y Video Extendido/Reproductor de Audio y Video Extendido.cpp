// Reproductor de Audio y Video Extendido.cpp: define el punto de entrada de la aplicación.
//

#include "stdafx.h"
#include "resource.h"
#include <ShellAPI.h>
#include <dbghelp.h>
#include "DDirectoriosWindows.h"

#pragma comment(lib, "dbghelp.lib")		// Libreria para las excepciones graves

// Error crítico
LONG WINAPI FuncionErrorCritico(struct _EXCEPTION_POINTERS* ExceptionInfo) {
	// Path para el archivo dump
	std::wstring PathDump;
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(PathDump);
	PathDump += L"\\Rave\\ErrorCritico.dmp";

	// Creo el archivo para volcar la información del error crítico
	HANDLE hFile = CreateFile(PathDump.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Si no hay ningún error al crear el archivo..
	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE)) {
		// Rellenamos la estructura con la información de la excepción
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = ExceptionInfo;
		mdei.ClientPointers = FALSE;
		// Rellenamos la estructura con la información del callback
		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = NULL;
		mci.CallbackParam = 0;
		// Creamos una variable con las opciones que se usaran para crear el dump // MiniDumpWithFullMemory
		MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpFilterMemory | MiniDumpScanMemory | MiniDumpNormal);
		// Escribimos el dump en el archivo
		BOOL rv;
		rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (ExceptionInfo != 0) ? &mdei : 0, 0, &mci);
		// Cerramos el archivo dump
		CloseHandle(hFile);
	}

	// Obtengo el Path de RAVE.exe y lo ejecuto con el parámetro '-MostrarErrorCritico'
	int		TotalArgs	= 0;
	TCHAR **Args		= CommandLineToArgvW(GetCommandLine(), &TotalArgs);
	ShellExecute(NULL, L"Open", Args[0], L"-MostrarErrorCritico", NULL, SW_SHOW);
	// Libero la memória
	if (TotalArgs > 0) LocalFree(Args);

	return TRUE;
}



// MAIN
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::wstring PathRAVE;
	// Creo el directorio \AppData\Rave (si no existe)
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(PathRAVE);
	PathRAVE += L"\\Rave\\";
	if (GetFileAttributes(PathRAVE.c_str()) == INVALID_FILE_ATTRIBUTES) {
		CreateDirectory(PathRAVE.c_str(), NULL);
	}

	// Filtro para errores críticos
	LPTOP_LEVEL_EXCEPTION_FILTER FiltroErrorCritico;
	FiltroErrorCritico = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)FuncionErrorCritico);

 	// TODO: colocar código aquí.
	MSG		msg;
	HACCEL	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO));	

	if (App.Iniciar(nCmdShow) == TRUE) {
		// Bucle principal de mensajes:
		while (GetMessage(&msg, NULL, 0, 0)) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	App.Terminar();
	// Elimino la clase aplicacion de la DWL
//	delete _Aplicacion;

	return 0;
}
