#include "stdafx.h"
#include "RAVE.h"
#include "resource.h"
#include <Shellapi.h>
#include <versionhelpers.h>

void RAVE_Iniciar(void) {
	_APLICACION = new RAVE;
}


RAVE *_APLICACION = NULL;

RAVE::RAVE(void) : PlayerInicial(FALSE) {
}


RAVE::~RAVE(void) {
}

// Devuelve TRUE para continuar con el bucle de mensajes, FALSE para salir de la aplicación
const BOOL RAVE::Iniciar(int nCmdShow) {
		// Obtengo/Creo un mutex para evitar que 2 o mas reproductores se inicien a la vez.
	// De esta forma cuando se inicia el reproductor, este espera a que el anterior termine completamente de cargarse.
	HANDLE MutexPlayer = OpenMutex(NULL, FALSE, TEXT("Mutex_RAVE"));
	if (MutexPlayer == NULL) {
		PlayerInicial = true;
		MutexPlayer = CreateMutex(NULL, FALSE, TEXT("Mutex_RAVE"));
	}
	WaitForSingleObject(MutexPlayer, INFINITE);

	ObtenerSO();
	// Consola para mensajes de depuración
	#ifdef MOSTRAR_CONSOLA
		ConsolaDebug.Crear(L"Consola de depuración");
		Debug_Escribir_Varg(L"RAVE::Iniciar en %s.\n", SO.c_str());
	#endif

	BOOL Ret = FALSE;

	// Obtengo la linea de comandos y ejecuto el reproductor según los argumentos
	LineaComando LC = ObtenerLineaComando();

	#ifdef MOSTRAR_ERRORCRITICO
		LC = LineaComando_ErrorCritico;
	#endif

	#ifdef SIMULAR_ERRORCRITICO
		// Hay que comprobar que no se vaya a mostrar la ventana de error crítico, o el windows entrará en un bucle infinito abriendo el Rave xd
		if (LC != LineaComando_ErrorCritico) {
			int *SEC = NULL;
			SEC[0] = 2;
		}
	#endif

	switch (LC) {
		// Ejecución normal sin parámetros
		case LineaComando_Nada :
			VLC.Iniciar();
			BD.IniciarBD();

			IniciarUI(nCmdShow);

			Ret = BD.Tabla_Raiz.Argerar_Raiz(TEXT("D:\\MP3"));
			Ret = BD.Tabla_Raiz.Argerar_Raiz(TEXT("D:\\Pelis i Series"));
			BD.ActualizarArbol();

			// Libero el mutex que impide la carga de un supuesto segundo reproductor
			ReleaseMutex(MutexPlayer);
			return TRUE;

		// Ejecución para mostrar la ventana de error crítico
		case LineaComando_ErrorCritico :
			VentanaErrorCrit.Crear();
			ReleaseMutex(MutexPlayer);
			return TRUE;

		default :
			ReleaseMutex(MutexPlayer);
			return FALSE;
	}

	return TRUE;
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
	ListaImagenes16.Crear(16, 16);
	ListaImagenes16.AgregarIconoRecursos(IDI_GENERO);				// 0
	ListaImagenes16.AgregarIconoRecursos(IDI_GRUPO);				// 1
	ListaImagenes16.AgregarIconoRecursos(IDI_DISCO);				// 2
	ListaImagenes16.AgregarIconoRecursos(IDI_CANCION);				// 3
	ListaImagenes16.AgregarIconoRecursos(IDI_VIDEO);				// 4
	ListaImagenes16.AgregarIconoRecursos(IDI_CDAUDIO);	     		// 5
	ListaImagenes16.AgregarIconoRecursos(IDI_LISTACANCIONES);		// 6
	ListaImagenes16.AgregarIconoRecursos(IDI_SUBTITULOS);			// 7
	ListaImagenes16.AgregarIconoRecursos(IDI_DIRECTORIO);			// 8
	ListaImagenes16.AgregarIconoRecursos(IDI_RAIZ);					// 9

	// Ventana principal
	VentanaRave.Crear(nCmdShow);
	// Controles pantalla completa
	ControlesPC.Crear();
}

void RAVE::Terminar(void) {
	BD.TerminarBD();
}

void RAVE::Eventos_Mirar(void) {
	static MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/**/
void RAVE::ObtenerSO() {
	UINT Version = 0;
	if (IsWindowsXPOrGreater()) Version++;
	if (IsWindowsXPSP1OrGreater()) Version++;
	if (IsWindowsXPSP2OrGreater()) Version++;
	if (IsWindowsXPSP3OrGreater()) Version++;
	if (IsWindowsVistaOrGreater()) Version++;
	if (IsWindowsVistaSP1OrGreater()) Version++;
	if (IsWindowsVistaSP2OrGreater()) Version++;
	if (IsWindows7OrGreater()) Version++;
	if (IsWindows7SP1OrGreater()) Version++;
	if (IsWindows8OrGreater()) Version++;
	if (IsWindows8Point1OrGreater()) Version++;
	if (IsWindows10OrGreater()) Version++;

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

const LineaComando RAVE::ObtenerLineaComando(void) {
	int				TotalArgs	= 0;
	TCHAR         **Args		= CommandLineToArgvW(GetCommandLine(), &TotalArgs);
	LineaComando	Ret			= LineaComando_Nada;

	// Si hay parámetros
	if (TotalArgs > 1) {
		// Parámetro para mostrar la ventana de error crítico
		if (wcscmp(Args[1], L"-MostrarErrorCritico") == 0) {
			Ret = LineaComando_ErrorCritico;
		}
	}

	// Elimino memoria que ocupan los argumentos
	if (TotalArgs > 0) {
		LocalFree(Args);
	}
	
	return Ret;
}