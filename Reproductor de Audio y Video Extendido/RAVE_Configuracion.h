// Configuración para compilar RAVE

// Versión y titulo
#define RAVE_VERSION			0, 4, 1, 2
#define RAVE_VERSIONF			0.412f
#define RAVE_VERSIONSTR			L"0.412"
#define RAVE_TITULO				L"RAVE " RAVE_VERSIONSTR
#define RAVE_VERSIONBD          L"1.0"

// Mostrar consola de depuración
#ifdef _DEBUG
	#define RAVE_MOSTRAR_CONSOLA
#endif

// Simular el AppPath en "C:\ProgramFiles\RAVE\"
//#define RAVE_SIMULAR_APPPATH

// Muestra la ventana para alertar de un error crítico
//#define RAVE_MOSTRAR_ERRORCRITICO

// Simula un error crítico y luego invoca una nueva instancia que muestra el error crítico
//#define RAVE_SIMULAR_ERRORCRITICO

