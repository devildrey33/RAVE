#include "stdafx.h"
#include "RAVE.h"
#include "resource.h"
#include <Shellapi.h>
#include <Shlobj.h>
#include <DIcono.h>
#include <DRegistro.h>
#include <DDirectoriosWindows.h>
#include "Historial_Lista.h"


#include <DEnviarCorreo.h>
//#include <gdiplus.h>



#define MUTEX_RAVE L"Mutex_RAVE"

/*void RAVE_Iniciar(void) {
	// Inicio la aplicación
	_Aplicacion = new RAVE;
}*/


//RAVE *_APLICACION = NULL;

RAVE::RAVE(void) : PlayerInicial(FALSE), MutexPlayer(NULL), MenuVideoPistasDeAudio(NULL), MenuVideoProporcion(NULL), MenuVideoFiltros(NULL), MenuVideoSubtitulos(NULL), MenuVideoMomentos(NULL), _LC(LineaComando_Nada)/*, _gdiplusToken(0) */ {
	// Inicio la semilla para generar números aleatórios
//	srand(GetTickCount());

/*	static DWL::DEnviarCorreo C;
	C.Servidor("smtp.gmail.com", 465);
	
	C.Enviar(
		// Terminado
		[=]() {
			int z = 0;
		},
		// Error
		[=](int nError) {
			int z = 0;
		},
		// Porcentaje
		[=](int nValor) {
			int z = 0;
		}
	);*/
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

	// Ignoro players previos y muestro la ventana igualmente en modo DEBUG
	#ifdef RAVE_IGNORAR_INSTANCIAS_PREVIAS
		PlayerInicial = TRUE;
	#endif


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
	
	Debug_Escribir(ObtenerSO());
	Debug_EscribirSinMS(L"\n");

	// Inicializo la librería COM (para el TaskBarList)
//	HRESULT CIE = CoInitializeEx(0, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);

	BOOL MemRet = FALSE;
	// Si este hilo pertenece al reproductor inicial, creo la memória compartida.
	// Si este hilo no pertenece al reproductor inicial, obtengo la memória compartida (que ya debe haber sido inicializada)
	if (PlayerInicial == TRUE) 	MemRet = MemCompartida.Crear(L"Memoria_Rave");	 // Creo un espacio de memória compartida	
	else						MemRet = MemCompartida.Obtener(L"Memoria_Rave"); // Obtengo el espacio de memória compartida


	BOOL Ret = FALSE;

	// Obtengo la linea de comandos y ejecuto el reproductor según los argumentos
	std::vector<std::wstring> Paths;
	_LC = ObtenerLineaComando(Paths);

	// Simula los argumentos para reproducir un video nada mas iniciar
	#ifdef RAVE_SIMULAR_REPRODUCIR_VIDEO
		LC = LineaComando_Reproducir;
		Paths.resize(0);
		Paths.push_back(RAVE_SIMULAR_REPRODUCIR_VIDEO_PATH);
	#endif

	// Muestra la ventana para alertar de un error crítico
	#ifdef RAVE_MOSTRAR_ERRORCRITICO
		_LC = LineaComando_ErrorCritico;
	#endif

	// Simula un error crítico y luego invoca una nueva instancia que muestra el error crítico
	#ifdef RAVE_SIMULAR_ERRORCRITICO
		// Hay que comprobar que no se vaya a mostrar la ventana de error crítico, o el windows entrará en un bucle infinito abriendo el Rave xd
		if (_LC != LineaComando_ErrorCritico) {
			try {
				int *SEC = NULL;
				SEC[0] = 2;
			} // PETIT TEST PER VEURE SI FUNCIONA EL CATCH AMB EL ACCESS VIOLATION
			catch (const std::exception &ex) {
				Debug_Escribir(L"Simulación Error Crítico");
			}
		}
	#endif

	// Simula que ha recibido un mensaje del actualizador conforme está corrupto
	#ifdef RAVE_SIMULAR_INSTALADOR_CORRUPTO
		LC = LineaComando_ActualizadorCorrupto;
	#endif

	#ifdef RAVE_SIMULAR_ACTUALIZACION_TERMINADA
		LC = LineaComando_ActualizacionTerminada;
	#endif

	// Miro el path del ejecutable
	std::wstring P = App.Path();	
	// Si no está en un directorio Debug o Release (para que no me la lie actualizando en alguno de esos directorios)
	if (P.find(L"Debug") == std::wstring::npos && P.find(L"Release") == std::wstring::npos) {
		// Añado una clave en el registro con el path del reproductor, para que el actualizador sepa donde hay que actualizar
		#ifdef _WIN64
			DWL::DRegistro::AsignarValor_String(HKEY_CURRENT_USER, L"Software\\Rave", L"Path64", P.c_str());
		#else
			DWL::DRegistro::AsignarValor_String(HKEY_CURRENT_USER, L"Software\\Rave", L"Path32", P.c_str());
		#endif
	}


	// Teclas rapidas por defecto
	TeclasRapidas.push_back(TeclaRapida(VK_SPACE	, FALSE, FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_INSERT	, FALSE, FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_ADD		, FALSE, FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_SUBTRACT	, FALSE, FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_RIGHT	, TRUE , FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_LEFT		, TRUE , FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_F1		, FALSE, FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_F2		, FALSE, FALSE, FALSE));
	TeclasRapidas.push_back(TeclaRapida(VK_F3		, FALSE, FALSE, FALSE));

	
	switch (_LC) {
		// El instalador está corrupto, lo borro
		case LineaComando_ActualizadorCorrupto :
			EliminarActualizador();
			Ret = EjecutarReproductor(Paths, hWndPlayer, nCmdShow);
			MostrarToolTipPlayerError(L"La actualización ha fallado...");
			break;
		// La actualización ha finalizado correctamente, la borro
		case LineaComando_ActualizacionTerminada :
			EliminarActualizador();
			Ret = EjecutarReproductor(Paths, hWndPlayer, nCmdShow);
			MostrarToolTipPlayer(L"RAVE actualizado a la versión : " RAVE_VERSIONSTR);
			break;			
		// Ejecución normal sin parámetros
		case LineaComando_Nada :
		case LineaComando_Path :
			Ret = EjecutarReproductor(Paths, hWndPlayer, nCmdShow);
			break;
		// Ejecución desde el explorador (Reproducir)
		case LineaComando_Reproducir:
			VentanaRave.Lista.BorrarListaReproduccion();			
			Ret = EjecutarReproductor(Paths, hWndPlayer, nCmdShow);
			break;
		// Ejecución para mostrar la ventana de error crítico
		case LineaComando_ErrorCritico :
			VentanaErrorCrit.Crear();
			Ret = TRUE;
			break;

		// Crea las asociaciones con todas las extensiones aceptadas, y registra la aplicación RAVE
/*		case LineaComando_AsociarArchivos:
			// Si esta aplicación no tiene privilegios de administración, ejecuto una nueva instancia que pedirá privilegios de administración
			if (IsUserAnAdmin() == FALSE)	{	ShellExecute(NULL, TEXT("RunAs"), AppExe.c_str(), TEXT("-AsociarArchivos"), AppPath.c_str(), SW_SHOWNORMAL);		}
			else							{	AsociarMedios.RegistrarApp();																						}
			Ret = FALSE;
			break;*/

		// Elimina todos los datos del registro referentes a RAVE, y carga las copias de seguridad
/*		case LineaComando_DesasociarArchivos:
			// Si esta aplicación no tiene privilegios de administración, ejecuto una nueva instancia que pedirá privilegios de administración
			if (IsUserAnAdmin() == FALSE)	{	ShellExecute(NULL, TEXT("RunAs"), AppExe.c_str(), TEXT("-DesasociarArchivos"), AppPath.c_str(), SW_SHOWNORMAL);	}
			else							{	AsociarMedios.DesRegistrarApp();																				}
			Ret = FALSE;
			break;*/

		default :
			Ret = FALSE;
			break;
	}

	// Libero el mutex que impide la carga de un supuesto segundo reproductor
	ReleaseMutex(MutexPlayer);
	CloseHandle(MutexPlayer);

	return Ret;
}

// Función que elimina el ejecutable de la actualización
void RAVE::EliminarActualizador(void) {
	std::wstring PathActualizador;
	std::wstring PathMD5;
	// Elimino el ejecutable de la actualizacion
	DWL::DDirectoriosWindows::Comun_AppData(PathActualizador);
	PathMD5 = PathActualizador;
	#ifdef _WIN64
		PathActualizador	+= L"\\Rave\\Actualizador_RAVE_x64.exe";
		PathMD5				+= L"\\Rave\\Actualizador_RAVE_x64.md5";
	#else
		PathActualizador	+= L"\\Rave\\Actualizador_RAVE_x86.exe";
		PathMD5				+= L"\\Rave\\Actualizador_RAVE_x86.md5";
	#endif
	BOOL R = DeleteFile(PathActualizador.c_str());
	BOOL R2 = DeleteFile(PathMD5.c_str());
}


const BOOL RAVE::EjecutarReproductor(std::vector<std::wstring> &Paths, HWND hWndPlayer, const int nCmdShow) {
	BOOL Ret = FALSE;

	// Agrego el path a la memoria compartida, y envio un mensaje al reproductor
	if (Paths.size() > 0 && PlayerInicial == FALSE) {
		MemCompartida.AgregarPath(Paths[Paths.size() - 1]);

//		MessageBox(NULL, Paths[0].c_str(), L"test", MB_OK);
		PostMessage(hWndPlayer, (_LC == LineaComando_Reproducir) ? WM_REPRODUCIRMEDIO : WM_AGREGARMEDIO, 0, 0);
	}
	// Si ya existe un reproductor activo, salgo
	if (PlayerInicial == FALSE) {
		return FALSE;
	}


	// Inicio la BD de las opciones para tener los datos de la ventana principal, etc..
	Opciones.Iniciar();
	// Inicio la base de datos 
	BD.Iniciar();

	// Muestro la ventana principal y creo los menús
	IniciarUI(nCmdShow);

	// Compruebo las aosiciaciones de archivo y muestro la ventana si es necesario
/*	if (AsociarMedios.ComprobarAsociaciones() == FALSE) {
		VentanaAsociar.Mostrar();
	}*/

	// No hay paths, creo una lista de inicio
	if (Paths.size() == 0 && PlayerInicial == TRUE) {
		#ifndef  RAVE_IGNORAR_LISTA_INICIO 	// No genera ninguna lista al iniciar (por el debug del VLC que es muy heavy.. y carga mucho al visual studio)
			// Inicia la acción por defecto al empezar
			Debug_Escribir_Varg(L"Rave::Iniciar ->  Acción de inicio : %d\n", Opciones.Inicio());
			switch (Opciones.Inicio()) {
				//					case Tipo_Inicio_NADA			:																		break;
				case Tipo_Inicio_Genero		:	VentanaRave.GenerarListaAleatoria(TLA_Genero);						break;
				case Tipo_Inicio_Grupo		:	VentanaRave.GenerarListaAleatoria(TLA_Grupo);						break;
				case Tipo_Inicio_Disco		:	VentanaRave.GenerarListaAleatoria(TLA_Disco);						break;
				case Tipo_Inicio_50Medios	:	VentanaRave.GenerarListaAleatoria(TLA_50Medios);					break;
				case Tipo_Inicio_LoQueSea	:	VentanaRave.GenerarListaAleatoria(TLA_LoQueSea);					break;
				case Tipo_Inicio_Nota		:	VentanaRave.GenerarListaAleatoria(TLA_Nota);						break;
				case Tipo_Inicio_UltimaLista:	BD.ObtenerUltimaLista(Unidades);									break;
				default                     :   BD.GuardarHistorial_Lista(Historial_Lista(L"Lista"));				break;
			}
		#else
			// Guardo una lista inicial en el historial
			BD.GuardarHistorial_Lista(Historial_Lista(L"Lista"));
		#endif

	}
	// Hay paths, los añado a la lista
	else {
		// Guardo una lista inicial en el historial
		BD.GuardarHistorial_Lista(Historial_Lista(L"Lista"));
		for (size_t i = 0; i < Paths.size(); i++) {
			if (Paths[i] != L"-r") {
				MemCompartida.AgregarPath(Paths[i]);
			}
		}
		VentanaRave.ExploradorAgregarMedio(TRUE);
	}

	// Actualizo el arbol de la base de datos
	VentanaRave.ActualizarArbol();

	return TRUE;

}


void RAVE::IniciarUI(int nCmdShow) {
//	Fuente13 = CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Tahoma");

	// Menu Arbol BD	
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_AGREGARALISTA		, L"Añadir a lista"								, IDI_AGREGARLISTA);
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_AGREGARANUEVALISTA	, L"Añadir a una nueva lista"					, IDI_AGREGARNUEVALISTA);
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_ABRIRCARPETA			, L"Abrir carpeta"								, IDI_DIRECTORIO);
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_MOMENTOS				, L"Momentos"									, IDI_MOMENTOS);
	VentanaRave.Menu_ArbolBD.AgregarBarra(ID_MENUBD_NOTA				, L"Nota"										, IDI_NOTA, 0.0f, 5.0f, 2.5f, DBarraEx_MostrarValor_ValorMaximo2Decimales);
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_INFORMACION			, L"Información"								, IDI_INTERROGANTE);
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_PROPIEDADES			, L"Propiedades"								, IDI_MOMENTO_EXCLUIR2);
	VentanaRave.Menu_ArbolBD.AgregarSeparador();
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_ACTUALIZAR			, L"Actualizar"									, IDI_ACTUALIZAR);
	VentanaRave.Menu_ArbolBD.AgregarMenu(ID_MENUBD_ANALIZAR				, L"Analizar"									, IDI_ANALIZAR);

	// Menu Boton Arbol BD	
	VentanaRave.Menu_BotonArbolBD.AgregarMenu(ID_MENUBD_ACTUALIZAR		, L"Actualizar"									, IDI_ACTUALIZAR);
	VentanaRave.Menu_BotonArbolBD.AgregarMenu(ID_MENUBD_ANALIZAR		, L"Analizar"									, IDI_ANALIZAR);
	VentanaRave.Menu_BotonArbolBD.AgregarMenu(ID_MENUBD_AGREGAR_URL		, L"Agregar URL"								, IDI_URL);
	
	// Menu Lista
	VentanaRave.Menu_Lista.AgregarMenu(ID_MENULISTA_ABRIRCARPETA		, L"Abrir carpeta"								, IDI_DIRECTORIO);
	VentanaRave.Menu_Lista.AgregarMenu(ID_MENULISTA_ELIMINAR			, L"Eliminar de la lista"						, IDI_ELIMINAR);
	VentanaRave.Menu_Lista.AgregarMenu(ID_MENULISTA_MOMENTOS			, L"Momentos"									, IDI_MOMENTOS);
	VentanaRave.Menu_Lista.AgregarMenu(ID_MENULISTA_MOSTRARBD			, L"Mostrar en la BD"							, IDI_BASEDATOS);
	VentanaRave.Menu_Lista.AgregarBarra(ID_MENULISTA_NOTA				, L"Nota"										, IDI_NOTA , 0.0f, 5.0f, 2.5f, DBarraEx_MostrarValor_ValorMaximo2Decimales);
	VentanaRave.Menu_Lista.AgregarMenu(ID_MENULISTA_INFORMACION			, L"Información"								, IDI_INTERROGANTE);
	VentanaRave.Menu_Lista.AgregarMenu(ID_MENULISTA_PROPIEDADES			, L"Propiedades"								, IDI_MOMENTO_EXCLUIR2);

	// Menu Boton Lista
	VentanaRave.Menu_BotonLista.AgregarMenu(ID_MENUBOTONLISTA_BORRAR					, L"Borrar Lista"					, IDI_ELIMINAR);
	DMenuEx *Menu = VentanaRave.Menu_BotonLista.AgregarMenu(ID_MENUBOTONLISTA_GENERAR	, L"Generar Lista"					, IDI_LISTAALEATORIA);
	DMenuEx *MenuRnd = Menu->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_GENERO					, L"Aleatória por Genero"		, IDI_GENERO);
						MenuRnd->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_GENERO_RND,				L"Rnd"						, IDI_GENERO);
						MenuRnd->EventoMostrarMenu = [=](DWL::DMenuEx& nMenu) {	BD.GenerarSugerenciasMenu(nMenu, TLA_Genero); };
			 MenuRnd = Menu->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_GRUPO					, L"Aleatória por Grupo"		, IDI_GRUPO);
 						MenuRnd->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_GRUPO_RND,				L"Rnd"						, IDI_GRUPO);
						MenuRnd->EventoMostrarMenu = [=](DWL::DMenuEx& nMenu) { BD.GenerarSugerenciasMenu(nMenu, TLA_Grupo); };
			 MenuRnd = Menu->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_DISCO					, L"Aleatória por Disco"		, IDI_DISCO);
						MenuRnd->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_DISCO_RND,				L"Rnd"						, IDI_DISCO);
						MenuRnd->EventoMostrarMenu = [=](DWL::DMenuEx& nMenu) { BD.GenerarSugerenciasMenu(nMenu, TLA_Disco); };
			 MenuRnd = Menu->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_50MEDIOS					, L"Aleatória con 50 Medios"	, IDI_TERMINADO);
			 MenuRnd = Menu->AgregarSeparador();
			 MenuRnd = Menu->AgregarMenu(ID_MENUBOTONLISTA_GENERAR_NOTA						, L"Por Nota"					, IDI_NOTA);
	VentanaRave.Menu_BotonLista.AgregarMenu(ID_MENUBOTONLISTA_AGREGAR_URL				, L"Agregar URL"					, IDI_URL);

	// Menu Repetir
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_NO					, L"Desactivado"		, IDI_NOCHECK);
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI					, L"Repetir"			, IDI_NOCHECK);
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI_MEZCLAR			, L"Repetir y mezclar"	, IDI_NOCHECK);
	Menu = VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_GENERAR		, L"Generar lista"		, IDI_NOCHECK);
	Menu->AgregarMenu(ID_REPETIR_GENERO										, L"Genero"			, IDI_NOCHECK);
	Menu->AgregarMenu(ID_REPETIR_GRUPO										, L"Grupo"			, IDI_NOCHECK);
	Menu->AgregarMenu(ID_REPETIR_DISCO										, L"Disco"			, IDI_NOCHECK);
	Menu->AgregarMenu(ID_REPETIR_50CANCIONES								, L"60 Canciones"	, IDI_NOCHECK);
	Menu->AgregarSeparador();
	Menu->AgregarMenu(ID_REPETIR_LOQUESEA									, L"Lo que sea"		, IDI_NOCHECK);

	VentanaRave.Menu_Repetir.AgregarSeparador();
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI_APAGAR_REP		, L"Apagar RAVE"		, IDI_NOCHECK);
	VentanaRave.Menu_Repetir.AgregarMenu(ID_REPETIR_SI_APAGAR_WIN		, L"Apagar Windows"		, IDI_NOCHECK);

	switch (Opciones.Repeat()) {
		case Tipo_Repeat_NADA				:		VentanaRave.Menu_Repetir.Menu(0)->Icono(IDI_CHECK);		break;
		case Tipo_Repeat_RepetirLista		:		VentanaRave.Menu_Repetir.Menu(1)->Icono(IDI_CHECK);		break;
		case Tipo_Repeat_RepetirListaShufle	:		VentanaRave.Menu_Repetir.Menu(2)->Icono(IDI_CHECK);		break;
		case Tipo_Repeat_Generar50Canciones :       VentanaRave.Menu_Repetir.Menu(3)->Icono(IDI_CHECK);		VentanaRave.Menu_Repetir.Menu(3)->Menu(3)->Icono(IDI_CHECK);	break;
		case Tipo_Repeat_GenerarGenero      :       VentanaRave.Menu_Repetir.Menu(3)->Icono(IDI_CHECK);		VentanaRave.Menu_Repetir.Menu(3)->Menu(0)->Icono(IDI_CHECK);	break;
		case Tipo_Repeat_GenerarGrupo		:       VentanaRave.Menu_Repetir.Menu(3)->Icono(IDI_CHECK);		VentanaRave.Menu_Repetir.Menu(3)->Menu(1)->Icono(IDI_CHECK);	break;
		case Tipo_Repeat_GenerarDisco		:       VentanaRave.Menu_Repetir.Menu(3)->Icono(IDI_CHECK);		VentanaRave.Menu_Repetir.Menu(3)->Menu(2)->Icono(IDI_CHECK);	break;
		case Tipo_Repeat_GenerarLoQueSea    :       VentanaRave.Menu_Repetir.Menu(3)->Icono(IDI_CHECK);		VentanaRave.Menu_Repetir.Menu(3)->Menu(5)->Icono(IDI_CHECK);	break;
		default                             : 		VentanaRave.Menu_Repetir.Menu(0)->Icono(IDI_CHECK);		break;
//		case Tipo_Repeat_ApagarReproductor	:		Menu_Repetir.Menu(3)->Icono(IDI_CHECK);		break;
//		case Tipo_Repeat_ApagarOrdenador	:		Menu_Repetir.Menu(4)->Icono(IDI_CHECK);		break;
	}

//	VentanaRave.Menu_Video.AgregarMenu(ID_MENUVIDEO_AUDIO									, L"Audio");
	MenuVideoPistasDeAudio = VentanaRave.Menu_Video.AgregarMenu(ID_MENUVIDEO_PISTA_AUDIO	, L"Pistas de audio"		, IDI_PISTAAUDIO, -1, FALSE);
//	MenuPistasDeAudio->Activado(FALSE);
																						// ID_MENUVIDEO_AUDIO_PISTAS_AUDIO <-> ID_MENUVIDEO_AUDIO_PISTAS_AUDIO_FIN (Espacio para 20 pistas de audio para no hacer corto...)
	MenuVideoProporcion = VentanaRave.Menu_Video.AgregarMenu(ID_MENUVIDEO_PROPORCION		, L"Proporción"				, IDI_PROPORCION, -1, FALSE);
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_PREDETERMINADO					, L"Predeterminado", IDI_CHECK2);
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_16A9							, L"16:9");
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_4A3							, L"4:3");
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_1A1							, L"1:!");
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_16A10							, L"16:10");
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_2P21A1							, L"2.21:1");
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_2P35A1							, L"2.35:1");
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_2P39A1							, L"2.39:1");
		MenuVideoProporcion->AgregarMenu(ID_MENUVIDEO_PROPORCION_5A4							, L"5:4");
//	MenuProporcion->Activado(FALSE);
	MenuVideoFiltros = VentanaRave.Menu_Video.AgregarMenu(ID_MENUVIDEO_FILTROS				, L"Filtros"				, IDI_FILTROS, -1, FALSE);
	MenuVideoFiltros->AgregarBarra(ID_MENUVIDEO_BRILLO											, L"Brillo"				, NULL, 0.0f, 2.0f, 1.0f, DBarraEx_MostrarValor_Valor2Decimales);
	MenuVideoFiltros->AgregarBarra(ID_MENUVIDEO_CONTRASTE										, L"Contraste"			, NULL, 0.0f, 2.0f, 1.0f, DBarraEx_MostrarValor_Valor2Decimales);
	MenuVideoFiltros->AgregarBarra(ID_MENUVIDEO_SATURACION										, L"Saturación"			, NULL, 0.0f, 2.0f, 1.0f, DBarraEx_MostrarValor_Valor2Decimales);
	MenuVideoFiltros->AgregarSeparador();
	MenuVideoFiltros->AgregarMenu(ID_MENUVIDEO_PORDEFECTO										, L"Restaurar valores iniciales");
	MenuVideoMomentos   = VentanaRave.Menu_Video.AgregarMenu(ID_MENUVIDEO_MOMENTOS			, L"Momentos"				, IDI_MOMENTOS);
	MenuVideoSubtitulos = VentanaRave.Menu_Video.AgregarMenu(ID_MENUVIDEO_SUBTITULOS		, L"Subtitulos"				, IDI_SUBTITULOS, -1, FALSE);


	// Ventana principal
	VentanaRave.Crear(nCmdShow);
	// Controles pantalla completa
	ControlesPC.Crear();

//	_ToolTip.CrearToolTipEx(DWL::DhWnd::Fuente18Normal, &VentanaRave);
//	_ToolTip2.CrearToolTipEx(DWL::DhWnd::Fuente18Normal, &VentanaOpciones);
	
	_ToolTipPlayer.Iniciar(&VentanaRave);
	_ToolTipOpciones.Iniciar(&VentanaOpciones2);
	_ToolTipMomentos.Iniciar(&VentanaRave.Momentos);
}

void RAVE::Terminar(void) {
	Opciones.Terminar();
	BD.Terminar();
	MP.Terminar();
	VentanaRave.Destruir();

	if (PlayerInicial == TRUE) {
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, TRUE); // activo el protector de pantalla
	}
	
	CoUninitialize();

	DFuente::EliminarFuentes();
	DIcono::EliminarIconos();

//	DFuenteP::EliminarFuentes();
//	DIconoP::EliminarIconos();
//	Gdiplus::GdiplusShutdown(_gdiplusToken);
	Debug_Escribir(L"Rave::Terminar\n");
}


const LineaComando RAVE::ObtenerLineaComando(std::vector<std::wstring> &Paths) {
	size_t			TotalArgs	= TotalLineaComandos();
//	TCHAR         **Args		= CommandLineToArgvW(GetCommandLine(), &TotalArgs);
	LineaComando	Ret			= LineaComando_Nada;		

	// Si hay parámetros
	if (TotalArgs > 1) {
		Ret = LineaComando_Path;
		// Guardo los argumentos en el vector Paths
		for (size_t i = 1; i < TotalArgs; i++) {
			// Si es un comando
			if (LineaComandos(i)[0] == L'-') {
				// Parámetro para mostrar la ventana de error crítico
				if (LineaComandos(i).compare(L"-MostrarErrorCritico") == 0) {
					Ret = LineaComando_ErrorCritico;
				}
				else if (LineaComandos(i).compare(L"-ActualizacionTerminada") == 0) {
					Ret = LineaComando_ActualizacionTerminada;
				}				
				else if (LineaComandos(i).compare(L"-ActualizadorCorrupto") == 0) {		
					Ret = LineaComando_ActualizadorCorrupto;
				}
				else if (LineaComandos(i).compare(L"-AsociarArchivos") == 0) {		// NO SE USA
					Ret = LineaComando_AsociarArchivos;
				}
				else if (LineaComandos(i).compare(L"-DesAsociarArchivos") == 0) {	// NO SE USA
					Ret = LineaComando_DesasociarArchivos;
				}
				else if (LineaComandos(i).compare(L"-r") == 0) {
					Ret = LineaComando_Reproducir;
				}
			}
			// Es un path, lo añado
			else {
				Paths.push_back(LineaComandos(i));
			}
		}
		/*
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
		}*/
	}

	// Libero la memória 
//	if (TotalArgs > 0)
//		LocalFree(Args);
	
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


// TECLADO GENERAL PARA TODAS LAS VENTANAS DE ESTE HILO EXCEPTO LA DEL VIDEO DEL VLC
void RAVE::Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado) {
//	DhWnd::Teclado[DatosTeclado.TeclaVirtual()] = true;
//	Debug_Escribir_Varg(L"RAVE::Evento_TeclaPresionada Tecla : %d, Id : %d\n", DatosTeclado.TeclaVirtual(), DatosTeclado.ID());
}

// TECLADO GENERAL PARA TODAS LAS VENTANAS DE ESTE HILO EXCEPTO LA DEL VIDEO DEL VLC
void RAVE::Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado) {
//	DhWnd::Teclado[DatosTeclado.TeclaVirtual()] = false;

	// Teclas extendidas para teclados con teclas para Play / Pausa, Stop, Next, Prev
	switch (DatosTeclado.TeclaVirtual()) {
		case VK_MEDIA_PLAY_PAUSE:
			App.VentanaRave.Lista_Play();
			return;
		case VK_MEDIA_STOP:
			App.VentanaRave.Lista_Stop();
			return;
		case VK_MEDIA_NEXT_TRACK:
			App.VentanaRave.Lista_Siguiente();
			return;
		case VK_MEDIA_PREV_TRACK:
			App.VentanaRave.Lista_Anterior();
			return;
	}
	
	BOOL Control	= ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
	BOOL Alt		= ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
	BOOL Shift		= ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);

	// Compruebo si es una tecla rápida
	size_t Pos = 0;
	int Volumen = 0;
	for (Pos = 0; Pos < TeclasRapidas.size(); Pos++) {
		if (TeclasRapidas[Pos].Tecla == DatosTeclado.TeclaVirtual() && TeclasRapidas[Pos].Control == Control && TeclasRapidas[Pos].Alt == Alt && TeclasRapidas[Pos].Shift == Shift) {
			switch (Pos) { // La posición de la tecla dentro del vector, indica su acción
				case 0: // play / pausa
					App.VentanaRave.Lista_Play();
					break;
				case 1: // generar lista
					App.VentanaRave.GenerarListaAleatoria();
					break;
				case 2: // Subir volumen
					if (App.Opciones.Volumen() + 10 > 200)		Volumen = 200;
					else										Volumen = App.Opciones.Volumen() + 10;
					App.MP.Volumen(Volumen);
					App.Opciones.Volumen(Volumen);
					break;
				case 3: // Bajar volumen
					if (App.Opciones.Volumen() - 10 < 0)		Volumen = 0;
					else										Volumen = App.Opciones.Volumen() - 10;
					App.MP.Volumen(Volumen);
					App.Opciones.Volumen(Volumen);
					break;
				case 4: // Avanzar
					App.VentanaRave.Lista_Siguiente();
					break;
				case 5: // Retroceder
					App.VentanaRave.Lista_Anterior();
					break;
				case 6: // Mostrar info medio
					if (MP.ComprobarEstado() == EnPlay || MP.ComprobarEstado() == EnPausa) {
						MostrarToolTipPlayer(MP.MedioActual());
					}
					break;
				case 7: // Mostrar medio en la bd
					if (MP.ComprobarEstado() == EnPlay || MP.ComprobarEstado() == EnPausa) {
						if (VentanaRave.PantallaCompleta() == FALSE) {
							VentanaRave.MostrarMarco(ID_BOTON_BD);
						}
						else {
							ControlesPC.Mostrar();
							ControlesPC.MostrarBD(TRUE);
						}
						VentanaRave.Arbol_MostrarMedio(MP.MedioActual().Hash);
					}
					break;
				case 8: // Mostrar medio en la lista
					if (MP.ComprobarEstado() == EnPlay || MP.ComprobarEstado() == EnPausa) {
						if (VentanaRave.PantallaCompleta() == FALSE) {
							VentanaRave.MostrarMarco(ID_BOTON_LISTA);
						}
						else {
							ControlesPC.Mostrar();
							ControlesPC.MostrarLista(TRUE);
						}
						VentanaRave.Lista.MostrarItem(VentanaRave.Lista.MedioActual);
						VentanaRave.Lista.DesSeleccionarTodo();
						VentanaRave.Lista.MedioActual->Seleccionado = TRUE;
						VentanaRave.Lista.ItemMarcado(VentanaRave.Lista.MedioActual, TRUE);
					}
					break;
			}
		}
	}

	Debug_Escribir_Varg(L"RAVE::Evento_TeclaSoltada Tecla : %d, Id : %d\n", DatosTeclado.TeclaVirtual(), DatosTeclado.ID());
}


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