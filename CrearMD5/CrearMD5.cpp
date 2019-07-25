// CrearMD5.cpp : Aplicación para generar un archivo con el MD5 del instalador
//

#include "framework.h"
#include "CrearMD5.h"
#include <DArchivoBinario.h>

#define MAX_LOADSTRING 100

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Colocar código aquí.
	INICIAR_DWL3(DWL::DApp);


	wchar_t CurDir[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, CurDir);

	std::wstring Path = CurDir;
/*	MessageBox(NULL, Path.c_str(), L"CrearMD5", MB_OK);
	
	// Ubicación actual directorio Build
	if (Path.substr(Path.size() - 8, 8).compare(L"CrearMD5") == 0) {
		Path.resize(Path.size() - 8);
		Path += L"Instaladores";
	}
	// Ubicación actual directorio Instaladores
	else {
//		MessageBox(NULL, Path.c_str(), L"CrearMD5", MB_OK);
	}*/

#if _WIN64
	std::wstring PathO = Path + L"\\SetupRave-x64.msi";
	std::wstring PathD = Path + L"\\SetupRave-x64.md5";
#else
	std::wstring PathO = Path + L"\\SetupRave-x86.msi";
	std::wstring PathD = Path + L"\\SetupRave-x86.md5";
#endif

	// Desde el compilador funciona, però un cop creat el exe a \\Instaladores\\ el path ha de ser diferent
/*	#if _WIN64
		PathO = Path + L"\\Instalador x64\\SetupRave-x64.msi";
		PathD = Path + L"\\Instalador x64\\SetupRave-x64.md5";
	#else 
		PathO = Path + L"\\Instalador x86\\SetupRave-x86.msi";
		PathD = Path + L"\\Instalador x86\\SetupRave-x86.md5";
	#endif*/

//	MessageBox(NULL, PathO.c_str(), L"CrearMD5", MB_OK);

	// No se encuentra el archivo, salgo
	if (GetFileAttributes(PathO.c_str()) == INVALID_FILE_ATTRIBUTES) return 0;

	DWL::DArchivoBinario Origen(PathO.c_str());
	DWL::DArchivoBinario Destino(PathD.c_str());


	Destino.Guardar(Origen.MD5(), FALSE);

	/*    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CREARMD5));

    MSG msg;

    // Bucle principal de mensajes:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }*/

	return 0;
}


