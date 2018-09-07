#include "stdafx.h"
#include "RAVE.h"
#include "resource.h"
#include <Shellapi.h>
#include <versionhelpers.h>
#include <Shlobj.h>

#define MUTEX_RAVE L"Mutex_RAVE"

void RAVE_Iniciar(void) {
	// Inicio la aplicación
	_APLICACION = new RAVE;
}


RAVE *_APLICACION = NULL;

RAVE::RAVE(void) : PlayerInicial(FALSE), MutexPlayer(NULL) {
	// Inicio la semilla para generar números aleatórios
	srand(GetTickCount());
}


RAVE::~RAVE(void) {
}

// Devuelve TRUE para continuar con el bucle de mensajes, FALSE para salir de la aplicación
const BOOL RAVE::Iniciar(int nCmdShow) {

	// Obtengo/Creo un mutex para evitar que 2 o mas reproductores se inicien a la vez.
	// De esta forma cuando se inicia el reproductor, este espera a que el anterior termine completamente de cargarse.
	MutexPlayer = OpenMutex(NULL, FALSE, MUTEX_RAVE);
	if (MutexPlayer == NULL) {
		// Creo un mutex para ejecutar ordenadamente otras instancias de este mismo reproductor
		MutexPlayer = CreateMutex(NULL, FALSE, MUTEX_RAVE);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Prevengo que otros reproductores se inicien durante la inicialización del reproductor principal. //
	DWORD r = WaitForSingleObject(MutexPlayer, INFINITE);                                               //
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// Compruebo si no existe una ventana del reproductor, para determinar si este será el reproductor principal
	HWND hWndPlayer = FindWindow(L"RAVE_VentanaPrincipal", NULL);
	if (hWndPlayer == NULL) {
		PlayerInicial = TRUE;
	}


	// Obtengo el directorio actual de la aplicación, para ello debo obtener el path del ejecutable, y recortarlo
	TCHAR PathApp[1024];
	DWORD Size = GetModuleFileName(NULL, PathApp, 1024);
	std::wstring AppExe = PathApp;
	for (Size; Size > 0; Size--) {
		if (PathApp[Size] == TCHAR('\\')) {
			PathApp[Size + 1] = 0;
			AppPath = PathApp;
			break;
		}
	}
		
	// Consola para mensajes de depuración
	#ifdef RAVE_MOSTRAR_CONSOLA
		ConsolaDebug.Crear(L"Consola de depuración");
		Debug_Escribir(L"RAVE::Iniciar\n");
	#endif

	// Aseguro que el directorio actual sea el del player
	SetCurrentDirectory(AppPath.c_str());
	Debug_Escribir_Varg(L"Path del ejecutable : %s\n", AppPath.c_str());

	// Simulo que el path del reproductor está en C:\ProgramFiles\Rave
	#ifdef RAVE_SIMULAR_APPPATH
		AppPath = L"C:\\ProgramFiles\\RAVE\\";
		Debug_Escribir(L"Simulando AppPath en : C:\\ProgramFiles\\Rave\\\n");
	#endif

	// Obtengo el sistema operativo (NO FUNCIONA BIEN A PARTIR DE WIN 8)
	ObtenerSO();


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

	Gdiplus::GdiplusStartupInput	gdiplusStartupInput;
	

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	switch (LC) {
		// Ejecución normal sin parámetros
		case LineaComando_Nada :
		case LineaComando_Path :
		case LineaComando_Reproducir :
			// Si ya existe un reproductor activo, salgo
			if (PlayerInicial == FALSE) {				
				if (Paths.size() > 0) {
					MemCompartida.Escribir(Paths[Paths.size() - 1]);					
					SendMessage(hWndPlayer, (Paths[0] == L"-r") ? WM_REPRODUCIRMEDIO : WM_AGREGARMEDIO, 0, 0);
				}
				Ret = FALSE;
				break;
			}
			// Inicio la base de datos y cargo las opciones antes de mostrar la ventana
			BD.Iniciar();
			// Muestro la ventana principal y creo los menús
			IniciarUI(nCmdShow);
			// Cargo la LibVLC
			VLC.Iniciar();

			// Compruebo las aosiciaciones de archivo y muestro la ventana si es necesario
			if (AsociarMedios.ComprobarAsociaciones() == FALSE) {
				VentanaAsociar.Mostrar();
			}


			// Hay uno o mas paths por agregar a la lista
			if (Paths.size() > 0) {
				// Agrega los paths a la lista de medios
				VentanaRave.ThreadArchivosLista.Iniciar(VentanaRave.hWnd(), Paths);
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

		// Crea las asociaciones con todas las extensiones aceptadas, y registra la aplicación RAVE
		case LineaComando_AsociarArchivos:
			// Si esta aplicación no tiene privilegios de administración, ejecuto una nueva instancia que pedirá privilegios de administración
			if (IsUserAnAdmin() == FALSE)	{	ShellExecute(NULL, TEXT("RunAs"), AppExe.c_str(), TEXT("-AsociarArchivos"), AppPath.c_str(), SW_SHOWNORMAL);		}
			else							{	AsociarMedios.RegistrarApp();																						}
			Ret = FALSE;
			break;

		// Elimina todos los datos del registro referentes a RAVE, y carga las copias de seguridad
		case LineaComando_DesasociarArchivos:
			// Si esta aplicación no tiene privilegios de administración, ejecuto una nueva instancia que pedirá privilegios de administración
			if (IsUserAnAdmin() == FALSE)	{	ShellExecute(NULL, TEXT("RunAs"), AppExe.c_str(), TEXT("-DesasociarArchivos"), AppPath.c_str(), SW_SHOWNORMAL);	}
			else							{	AsociarMedios.DesRegistrarApp();																					}
			Ret = FALSE;
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
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_AGREGARALISTA		, L"Añadir a lista");
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_AGREGARANUEVALISTA	, L"Añadir a una nueva lista");
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_ACTUALIZAR			, L"Actualizar");

	// Menu Mezclar (shufle)
//	VentanaRave.Menu_Mezclar.AgregarMenu(ID_MENUMEZCLAR_NO			, L"NO Mezclar");
//	VentanaRave.Menu_Mezclar.AgregarMenu(ID_MENUMEZCLAR_SI			, L"Mezclar");

	// Menu Repetir
	//Menu_Repetir.CrearMenu();
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_NO					, L"Desactivado");
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI					, L"Repetir");
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI_MEZCLAR			, L"Repetir y mezclar");
	VentanaRave.Menu_Repetir.AgregarSeparador();
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI_APAGAR_REP		, L"Apagar RAVE");
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI_APAGAR_WIN		, L"Apagar Windows");
	//Menu_Repetir.AgregarMenu(ID_REPETIR_SI_HIBERNAR_WIN	, L"Hibernar Windows (Estilo Win8+)");

	switch (BD.Opciones_Repeat()) {
		case Tipo_Repeat_NADA				:		VentanaRave.Menu_Repetir.Menu(0)->Icono(IDI_CHECK);		break;
		case Tipo_Repeat_RepetirLista		:		VentanaRave.Menu_Repetir.Menu(1)->Icono(IDI_CHECK);		break;
		case Tipo_Repeat_RepetirListaShufle	:		VentanaRave.Menu_Repetir.Menu(2)->Icono(IDI_CHECK);		break;
		default                             : 		VentanaRave.Menu_Repetir.Menu(0)->Icono(IDI_CHECK);		break;
//		case Tipo_Repeat_ApagarReproductor	:		Menu_Repetir.Menu(3)->Icono(IDI_CHECK);		break;
//		case Tipo_Repeat_ApagarOrdenador	:		Menu_Repetir.Menu(4)->Icono(IDI_CHECK);		break;
	}

	/*
	Menu_Test.AgregarMenu(ID_REPETIR_NO, L"Desactivado", IDI_CHECK);
	Menu_Test.AgregarMenu(ID_REPETIR_SI, L"Repetir");
	Menu_Test.AgregarMenu(ID_REPETIR_SI_MEZCLAR, L"Repetir y mezclar");
	Menu_Test.AgregarSeparador();
	Menu_Test.AgregarMenu(ID_REPETIR_SI_APAGAR_REP, L"Apagar RAVE");
	Menu_Test.AgregarMenu(ID_REPETIR_SI_APAGAR_WIN, L"Apagar Windows");
	DMenuEx *SM = Menu_Test.AgregarMenu(1, L"Test");
	SM->AgregarMenu(2, L"Test");
	SM->AgregarMenu(3, L"Test");
	SM->AgregarMenu(4, L"Test");
	SM->AgregarMenu(5, L"Test");
	SM->AgregarMenu(6, L"Test");
	SM = Menu_Test.AgregarMenu(1, L"Test2");
	SM->AgregarMenu(2, L"Test2");
	SM->AgregarMenu(3, L"Test2");
	SM->AgregarMenu(4, L"Test2");
	SM->AgregarMenu(5, L"Test2");
	SM = SM->AgregarMenu(6, L"Test2");
	SM->AgregarMenu(2, L"Test2.1");
	SM->AgregarMenu(3, L"Test2.1");
	SM->AgregarMenu(4, L"Test2.1");
	SM->AgregarMenu(5, L"Test2.1");*/
	//	Menu_Test.AgregarMenu(ID_REPETIR_SI_HIBERNAR_WIN, L"Hibernar Windows (Estilo Win8+)");

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
	BD.Terminar();

	if (PlayerInicial == TRUE) {
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, TRUE); // activo el protector de pantalla
	}
	
	CoUninitialize();

	Gdiplus::GdiplusShutdown(gdiplusToken);

	DhWnd::EliminarFuentesEstaticas();

}


/* Vacia la cola de mensajes para este hilo */
void RAVE::Eventos_Mirar(void) {
	static MSG msg;	
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
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
		for (int i = 1; i < TotalArgs; i++) {
			Paths.push_back(Args[i]);
		}

		if (Paths[0][0] == L'-') {
			// Parámetro para mostrar la ventana de error crítico
			if (Paths[0] == L"-MostrarErrorCritico") {
				Ret = LineaComando_ErrorCritico;
			}
			else if (Paths[0] == L"-AsociarArchivos") {
				Ret = LineaComando_AsociarArchivos;
			}
			else if (Paths[0] == L"-DesAsociarArchivos") {
				Ret = LineaComando_DesasociarArchivos;
			}
			else if (Paths[0] == L"-r") {
				Ret = LineaComando_Reproducir;
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


//
// Función para cerrar el sistema de varias formas (SOCerrarSistema [CS_Apagar, CS_Reiniciar, CS_CerrarUsuario])
// void CerrarSistema(const SOCerrarSistema Forma = CS_Apagar, const bool Forzar = false);
void RAVE::CerrarSistema(const SOCerrarSistema Forma, const BOOL Forzar) {
	HANDLE           hToken;
	TOKEN_PRIVILEGES tkp;
	UINT             Flags = NULL;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, false, &tkp, NULL, (PTOKEN_PRIVILEGES)NULL, 0);
	switch (Forma) {
		case SOCerrarSistema_Apagar			:	Flags = EWX_POWEROFF;			break;
		case SOCerrarSistema_ReIniciar		:	Flags = EWX_REBOOT;				break;
		case SOCerrarSistema_CerrarUsuario	:	Flags = EWX_LOGOFF;				break;
		case SOCerrarSistema_Hibernar       :   Flags = EWX_HYBRID_SHUTDOWN;	break;
	}
	if (Forzar == TRUE) Flags = (Flags | EWX_FORCE);
	ExitWindowsEx(Flags, 0);
};










/*
void PintarTexto(HDC DC, const wchar_t *pTexto, const int PosX, const int PosY, COLORREF ColorTexto, COLORREF ColorSombra) {
	SetTextColor(DC, ColorSombra);
	TextOut(DC, PosX + 1, PosY + 1, pTexto, static_cast<int>(wcslen(pTexto)));
	SetTextColor(DC, ColorTexto);
	TextOut(DC, PosX, PosY, pTexto, static_cast<int>(wcslen(pTexto)));
}

void PintarTexto(HDC DC, std::wstring &sTexto, const int PosX, const int PosY, COLORREF ColorTexto, COLORREF ColorSombra) {
	SetTextColor(DC, ColorSombra);
	TextOut(DC, PosX + 1, PosY + 1, sTexto.c_str(), static_cast<int>(sTexto.size()));
	SetTextColor(DC, ColorTexto);
	TextOut(DC, PosX, PosY, sTexto.c_str(), static_cast<int>(sTexto.size()));
}
*/