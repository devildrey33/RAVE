// Configuración para compilar RAVE

// Versión y titulo
#define RAVE_VERSION			0, 5, 0, 7
#define RAVE_VERSIONF			0.507
#define RAVE_VERSIONSTR			L"0.507c"
#define RAVE_TITULO				L"RAVE " RAVE_VERSIONSTR
#define RAVE_VERSIONBD          L"1.0"

#ifdef _DEBUG
	// Mostrar consola de depuración
	#define RAVE_MOSTRAR_CONSOLA

	// Simular el AppPath en "C:\ProgramFiles\RAVE\" (OJO PORQUE LA LIBVLC LA CARGA DE LA CARPETA "...\RAVE\VLCx64")
	#define RAVE_SIMULAR_APPPATH

	// Si ya existe un reproductor encendido, se arranca igual
	#define RAVE_IGNORAR_INSTANCIAS_PREVIAS

	// No genera ninguna lista al iniciar (por el debug del VLC que es muy heavy.. y carga mucho al visual studio)
	#define RAVE_IGNORAR_LISTA_INICIO

	// Muestra la ventana para alertar de un error crítico (OJO si quieres enviar el dump, requiere tener un dump creado)
	//#define RAVE_MOSTRAR_ERRORCRITICO

	// Simula un error crítico y luego invoca una nueva instancia que muestra el error crítico
	//#define RAVE_SIMULAR_ERRORCRITICO

#endif

// Simula los argumentos para reproducir un video nada mas iniciar
//#define RAVE_SIMULAR_REPRODUCIR_VIDEO

// Utiliza un segundo media player para obtener el oscyloscopio (EXPERIMENTAL)
// No acaba de funcionar muy bien, y hay problemas al ejecutar un video con la doble reproducción...
//#define RAVE_VLC_DOBLE_MEDIO_FFT