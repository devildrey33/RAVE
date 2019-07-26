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

	// Obtengo el path de la aplicación, que debería ser el path donde está ubicado el instalador
	wchar_t CurDir[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, CurDir);
	std::wstring Path = CurDir;

	#if _WIN64
		std::wstring PathO = Path + L"\\SetupRave-x64.msi";
		std::wstring PathD = Path + L"\\SetupRave-x64.md5";
	#else
		std::wstring PathO = Path + L"\\SetupRave-x86.msi";
		std::wstring PathD = Path + L"\\SetupRave-x86.md5";
	#endif


	// No se encuentra el archivo SetupRave-x??.msi, salgo
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


