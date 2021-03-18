// Configuraci�n para compilar RAVE

// Versi�n y titulo
#define RAVE_VERSION			0, 6, 0, 7
#define RAVE_VERSIONSTR			L"0.607"
#define RAVE_TITULO				L"RAVE " RAVE_VERSIONSTR
#define RAVE_VERSIONBD          L"1.2"
#define RAVE_VERSIONOPCIONES    L"1.2"

// Skin del reproductor (0 = normal, 1 = visual studio)
#define RAVE_SKIN               RAVE_SKIN_NORMAL				// Se tiene que modificar DhWnd.h para a�adir mas skins de prueba
//#define RAVE_SKIN               RAVE_SKIN_VISUALSTUDIO		// Se tiene que modificar DhWnd.h para a�adir mas skins de prueba
#define RAVE_SKIN_NORMAL        0
#define RAVE_SKIN_VISUALSTUDIO  1

#define RAVE_LOG_NADA			DWL::DTipoLog::Nada
#define RAVE_LOG_ARCHIVO		DWL::DTipoLog::Archivo
#define RAVE_LOG_CONSOLA		DWL::DTipoLog::Consola

#ifdef _DEBUG
	// Tipo de log (puede ser : Nada, Archivo, y Consola)
	#define RAVE_LOG				RAVE_LOG_CONSOLA
//	#define RAVE_LOG				RAVE_LOG_ARCHIVO

	// Simular el AppPath en "C:\ProgramFiles\RAVE\" (OJO PORQUE LA LIBVLC LA CARGA DE LA CARPETA "...\RAVE\VLCx64")
	#define RAVE_SIMULAR_APPPATH

	// Si ya existe un reproductor encendido, se arranca igual
	#define RAVE_IGNORAR_INSTANCIAS_PREVIAS

	// No genera ninguna lista al iniciar (por el debug del VLC que es muy heavy.. y carga mucho al visual studio)
	#define RAVE_IGNORAR_LISTA_INICIO

	// Muestra la ventana para alertar de un error cr�tico (OJO si quieres enviar el dump, requiere tener un dump creado)
	//#define RAVE_MOSTRAR_ERRORCRITICO	

	// Simula un error cr�tico y luego invoca una nueva instancia que muestra el error cr�tico
	//#define RAVE_SIMULAR_ERRORCRITICO

	// Simula que ha recibido un mensaje del actualizador que le dice que est� corrupto
	//#define RAVE_SIMULAR_INSTALADOR_CORRUPTO

	// Simula que ha recibido un mensaje del actualizador conforme ha terminado correctamente
	//#define RAVE_SIMULAR_ACTUALIZACION_TERMINADA

	// Simula que hay una nueva versi�n disponible (9.99)
	//#define RAVE_SIMULAR_ACTUALIZACION_NUEVA

#else // RELEASE
	// Tipo de log (puede ser : Nada, Archivo, y Consola)
	#define RAVE_LOG				RAVE_LOG_ARCHIVO
//	#define RAVE_LOG				RAVE_LOG_NADA

#endif

// Uso de la librer�a FmodEx (descomentar para utilizar FMOD [EXPERIMENTAL])
//#define RAVE_UTILIZAR_FMOD

// Simula los argumentos para reproducir un video nada mas iniciar
//#define RAVE_SIMULAR_REPRODUCIR_VIDEO
//#define RAVE_SIMULAR_REPRODUCIR_VIDEO_PATH L"C:\\Users\\devildrey33\\Videos\\World of Warships\\555.mp4"
//#define RAVE_SIMULAR_REPRODUCIR_VIDEO_PATH L"G:\\Pelis i Series\\Futurama\\Futurama dvdrip,.avi"

// Utiliza un segundo media player para obtener el oscyloscopio (EXPERIMENTAL)
// No acaba de funcionar muy bien, y hay problemas al ejecutar un video con la doble reproducci�n...
//#define RAVE_VLC_DOBLE_MEDIO_FFT
