// RAVE_LoaderActualizador.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "RAVE_LoaderActualizador.h"
#include "RAVE_LA.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,  _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Creo la clase aplicación para la DWL
	_Aplicacion = new RAVE_LA;

	App.Iniciar();

	/*
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAVELOADERACTUALIZADOR));
    MSG    msg;

    // Bucle principal de mensajes:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))  {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	delete _Aplicacion;*/

//    return (int) msg.wParam;
	return 0;
}


