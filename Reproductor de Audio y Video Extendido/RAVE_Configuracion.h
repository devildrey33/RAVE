// Configuraci�n para compilar RAVE

// Versi�n y titulo
#define RAVE_VERSION			0, 4, 1, 3
#define RAVE_VERSIONF			0.413f
#define RAVE_VERSIONSTR			L"0.413"
#define RAVE_TITULO				L"RAVE " RAVE_VERSIONSTR
#define RAVE_VERSIONBD          L"1.0"

#ifdef _DEBUG
	// Mostrar consola de depuraci�n
	#define RAVE_MOSTRAR_CONSOLA

	// Simular el AppPath en "C:\ProgramFiles\RAVE\"
	#define RAVE_SIMULAR_APPPATH

	// Muestra la ventana para alertar de un error cr�tico
	//#define RAVE_MOSTRAR_ERRORCRITICO

	// Simula un error cr�tico y luego invoca una nueva instancia que muestra el error cr�tico
	//#define RAVE_SIMULAR_ERRORCRITICO

#endif