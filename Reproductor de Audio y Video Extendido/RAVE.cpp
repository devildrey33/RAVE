#include "stdafx.h"
#include "RAVE.h"
#include "resource.h"
#include <Shellapi.h>
#include <versionhelpers.h>

#define MUTEX_RAVE L"Mutex_RAVE"

void RAVE_Iniciar(void) {
	_APLICACION = new RAVE;
	// Inicio la semilla para generar números aleatórios
	srand(GetTickCount());
}


RAVE *_APLICACION = NULL;

RAVE::RAVE(void) : PlayerInicial(FALSE), MutexPlayer(NULL) {
}


RAVE::~RAVE(void) {
}

// Devuelve TRUE para continuar con el bucle de mensajes, FALSE para salir de la aplicación
const BOOL RAVE::Iniciar(int nCmdShow) {

	// Obtengo/Creo un mutex para evitar que 2 o mas reproductores se inicien a la vez.
	// De esta forma cuando se inicia el reproductor, este espera a que el anterior termine completamente de cargarse.
	MutexPlayer = OpenMutex(NULL, FALSE, MUTEX_RAVE);
	if (MutexPlayer == NULL) {
		// Compruebo si existe una ventana del reproductor, para determinar si este será el reproductor principal
		if (FindWindow(L"RAVE_VentanaPrincipal", NULL) == NULL) {
			PlayerInicial = TRUE;
		}
		// Creo un mutex para ejecutar ordenadamente otras instancias de este mismo reproductor
		MutexPlayer = CreateMutex(NULL, FALSE, MUTEX_RAVE);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Prevengo que otros reproductores se inicien durante la inicialización del reproductor principal. //
	DWORD r = WaitForSingleObject(MutexPlayer, INFINITE);                                               //
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// Obtengo el directorio actual de la aplicación, para ello debo obtener el path del ejecutable, y recortarlo
	TCHAR PathApp[1024];
	DWORD Size = GetModuleFileName(NULL, PathApp, 1024);
	for (Size; Size > 0; Size--) {
		if (PathApp[Size] == TCHAR('\\')) {
			PathApp[Size + 1] = 0;
			AppPath = PathApp;
			break;
		}
	}
		
	// Aseguro que el directorio actual sea el del player
	SetCurrentDirectory(AppPath.c_str());
	Debug_Escribir_Varg(L"Path del ejecutable : %s.\n", AppPath.c_str());

	// Obtengo el sistema operativo (NO FUNCIONA BIEN A PARTIR DE WIN 8)
	ObtenerSO();

	// Consola para mensajes de depuración
#ifdef RAVE_MOSTRAR_CONSOLA
	ConsolaDebug.Crear(L"Consola de depuración");
	Debug_Escribir_Varg(L"RAVE::Iniciar en %s\n", AppPath.c_str());
#endif

	// Inicializo la librería COM (para el TaskBarList)
	CoInitializeEx(0, COINIT_MULTITHREADED);

	BOOL MemRet = FALSE;
	// Si este hilo pertenece al reproductor inicial, creo la memória compartida.
	// Si este hilo no pertenece al reproductor inicial, obtengo la memória compartida (que ya debe haber sido inicializada)
	if (PlayerInicial == TRUE) 	MemRet = MemCompartida.Crear(L"Memoria_Rave");	 // Creo un espacio de memória compartida	
	else						MemRet = MemCompartida.Obtener(L"Memoria_Rave"); // Obtengo el espacio de memória compartida


	BOOL Ret = FALSE;

	std::vector<std::wstring> Paths;
	// Obtengo la linea de comandos y ejecuto el reproductor según los argumentos
	LineaComando LC = ObtenerLineaComando(Paths);

	#ifdef RAVE_MOSTRAR_ERRORCRITICO
		LC = LineaComando_ErrorCritico;
	#endif

	#ifdef RAVE_SIMULAR_ERRORCRITICO
		// Hay que comprobar que no se vaya a mostrar la ventana de error crítico, o el windows entrará en un bucle infinito abriendo el Rave xd
		if (LC != LineaComando_ErrorCritico) {
			int *SEC = NULL;
			SEC[0] = 2;
		}
	#endif

	switch (LC) {
		// Ejecución normal sin parámetros
		case LineaComando_Nada :
		case LineaComando_Path :
			// Si ya existe un reproductor activo, salgo
			if (PlayerInicial == FALSE) {				
				Ret = FALSE;
				break;
			}

			VLC.Iniciar();
			BD.IniciarBD();

			IniciarUI(nCmdShow);

//			Ret = BD.Tabla_Raiz.Argerar_Raiz(TEXT("D:\\MP3"));
//			Ret = BD.Tabla_Raiz.Argerar_Raiz(TEXT("D:\\Pelis i Series"));
			BD.ActualizarArbol();

			if (Paths.size() > 0) {
				// Agrega los paths a la lista de medios
				//VentanaRave.Lista.AgregarMedio();
			}
			else {
				// Inicia la acción por defecto al empezar
			}
			
			Ret = TRUE;
			break;

		// Ejecución para mostrar la ventana de error crítico
		case LineaComando_ErrorCritico :
			VentanaErrorCrit.Crear();
			ReleaseMutex(MutexPlayer);
			Ret = TRUE;
			break;

		default :
			Ret = FALSE;
			break;
	}

	// Libero el mutex que impide la carga de un supuesto segundo reproductor
	ReleaseMutex(MutexPlayer);
	CloseHandle(MutexPlayer);

	return Ret;
}


void RAVE::IniciarUI(int nCmdShow) {

//	Fuente13 = CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Tahoma");

	// Menu Arbol BD
	Menu_ArbolBD.CrearMenu();
	Menu_ArbolBD.CrearMenuItem(ID_MENUBD_AGREGARALISTA, L"Añadir a lista");
	Menu_ArbolBD.CrearMenuItem(ID_MENUBD_AGREGARANUEVALISTA, L"Añadir a una nueva lista");
	Menu_ArbolBD.CrearMenuItem(ID_MENUBD_ACTUALIZAR, L"Actualizar");

	// Menu Mezclar (shufle)
	Menu_Mezclar.CrearMenu();
	Menu_Mezclar.CrearMenuItem(ID_MENUMEZCLAR_NO, L"NO Mezclar");
	Menu_Mezclar.CrearMenuItem(ID_MENUMEZCLAR_SI, L"Mezclar");

	// Menu Repetir
	Menu_Repetir.CrearMenu();
	Menu_Mezclar.CrearMenuItem(ID_REPETIR_NO, L"NO Repetir");
	Menu_Mezclar.CrearMenuItem(ID_REPETIR_SI, L"Repetir");
	Menu_Mezclar.CrearMenuItem(ID_REPETIR_SI_MEZCLAR, L"Repetir y mezclar");
	Menu_Mezclar.CrearMenuItem(ID_REPETIR_SI_APAGAR_REP, L"Apagar RAVE");
	Menu_Mezclar.CrearMenuItem(ID_REPETIR_SI_APAGAR_WIN, L"Apagar Windows");

	// Lista de imagenes de 16 pixeles
/*	ListaImagenes16.Crear(16, 16);
	ListaImagenes16.AgregarIconoRecursos(IDI_GENERO);				// 0
	ListaImagenes16.AgregarIconoRecursos(IDI_GRUPO);				// 1
	ListaImagenes16.AgregarIconoRecursos(IDI_DISCO);				// 2
	ListaImagenes16.AgregarIconoRecursos(IDI_CANCION);				// 3
	ListaImagenes16.AgregarIconoRecursos(IDI_VIDEO);				// 4
	ListaImagenes16.AgregarIconoRecursos(IDI_CDAUDIO);	     		// 5
	ListaImagenes16.AgregarIconoRecursos(IDI_LISTACANCIONES);		// 6
	ListaImagenes16.AgregarIconoRecursos(IDI_SUBTITULOS);			// 7
	ListaImagenes16.AgregarIconoRecursos(IDI_DIRECTORIO);			// 8
	ListaImagenes16.AgregarIconoRecursos(IDI_RAIZ);					// 9*/

	// Ventana principal
	VentanaRave.Crear(nCmdShow);
	// Controles pantalla completa
	ControlesPC.Crear();
}

void RAVE::Terminar(void) {
	BD.TerminarBD();
	
	CoUninitialize();
}

void RAVE::Eventos_Mirar(void) {
	static MSG msg;
	for (int i = 0; i < 10; i++) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

/* NO ES CORRECTE.. Obte el SO minim pel SDK jo diria.. pk amb win 10 diu que es win 8 */
void RAVE::ObtenerSO() {
	//RtlGetVersion()
/*	DWORD dwVersion = GetVersion();

	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));;*/

	UINT Version = 0;
	if (IsWindowsXPOrGreater())			Version++;
	if (IsWindowsXPSP1OrGreater())		Version++;
	if (IsWindowsXPSP2OrGreater())		Version++;
	if (IsWindowsXPSP3OrGreater())		Version++;
	if (IsWindowsVistaOrGreater())		Version++;
	if (IsWindowsVistaSP1OrGreater())	Version++;
	if (IsWindowsVistaSP2OrGreater())	Version++;
	if (IsWindows7OrGreater())			Version++;
	if (IsWindows7SP1OrGreater())		Version++;
	if (IsWindows8OrGreater())			Version++;
	if (IsWindows8Point1OrGreater())	Version++;
	if (IsWindows10OrGreater())			Version++;

	switch (Version) {
		case  0: SO = L"";						break;
		case  1: SO = L"Windows XP";			break;
		case  2: SO = L"Windows XP SP1";		break;
		case  3: SO = L"Windows XP SP2";		break;
		case  4: SO = L"Windows XP SP3";		break;
		case  5: SO = L"Windows Vista";			break;
		case  6: SO = L"Windows Vista SP1";		break;
		case  7: SO = L"Windows Vista SP2";		break;
		case  8: SO = L"Windows 7";				break;
		case  9: SO = L"Windows 7 SP1";			break;
		case 10: SO = L"Windows 8";				break;
		case 11: SO = L"Windows 8.1";			break;
		case 12: SO = L"Windows 10";			break;
		default: SO = L"Windows 10 o superior";	break;
	}
}

const LineaComando RAVE::ObtenerLineaComando(std::vector<std::wstring> &Paths) {
	int				TotalArgs	= 0;
	TCHAR         **Args		= CommandLineToArgvW(GetCommandLine(), &TotalArgs);
	LineaComando	Ret			= LineaComando_Nada;

	// Si hay parámetros
	if (TotalArgs > 1) {
		// Guardo los argumentos en el vector Paths
		for (int i = 0; i < TotalArgs -1; i++) {
			Paths.push_back(Args[i + 1]);
		}

		if (Paths[0][0] == L'-') {
			// Parámetro para mostrar la ventana de error crítico
			if (Paths[0] == L"-MostrarErrorCritico") {
				Ret = LineaComando_ErrorCritico;
			}

			else if (Paths[0] == L"-AsociarArchivos") {

			}

			else if (Paths[0] == L"-DesAsociarArchivos") {

			}
		}
		else {
			Ret = LineaComando_Path;
		}
	}

	// Libero la memória 
	if (TotalArgs > 0)
		LocalFree(Args);
	
	return Ret;
}