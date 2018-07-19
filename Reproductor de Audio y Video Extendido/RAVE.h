#pragma once

#include "VentanaPrincipal.h"
#include "BaseDatos.h"
#include "DConsola.h"
#include "RaveVLC.h"
//#include "DImageList.h"
#include "ControlesPantallaCompleta.h"
#include "DMenu.h"
#include "RAVE_Configuracion.h"
#include "VentanaErrorCritico.h"
#include <stdlib.h>
#include "MemoriaCompartida.h"

using namespace DWL;

enum LineaComando {
	LineaComando_Nada,			// Sin parámetros extra
	LineaComando_Path,			// Viene con uno o varios paths
	LineaComando_ErrorCritico	// Mostrar ventana de error crítico
};

class RAVE {
  public:
									RAVE(void);
							       ~RAVE(void);
									// Devuelve TRUE si es la primera instancia, FALSE si ya existe una instancia ejecutada
									// Al devolver FALSE se salta el bucle de mensajes y termina la aplicación
	const BOOL						Iniciar(int nCmdShow);

									// Inicia los menús y la ventana principal
	void							IniciarUI(int nCmdShow);

	template <typename T = int> T	Rand(const T Max, const T Min = 0) {
										return static_cast<T>((double)rand() / (RAND_MAX + 1) * (Max - Min) + Min);
									}
									// Obtiene la linea de comando y determina que hay que hacer
	const LineaComando				ObtenerLineaComando(std::vector<std::wstring> &Paths);

	void							Eventos_Mirar();

	void							ObtenerSO();

//	int							Rand()

	void							Terminar(void);
	VentanaPrincipal				VentanaRave;
	BaseDatos						BD;
	DConsola						ConsolaDebug;
	RaveVLC							VLC;
	ControlesPantallaCompleta		ControlesPC;


	DWL::DMenu 						Menu_ArbolBD;
	DWL::DMenu 						Menu_Mezclar;
	DWL::DMenu 						Menu_Repetir;


	BOOL							PlayerInicial;
	HANDLE							MutexPlayer;

	VentanaErrorCritico				VentanaErrorCrit;

	std::wstring					SO;
	std::wstring					AppPath;

	MemoriaCompartida               MemCompartida;
};

extern RAVE *_APLICACION;

#define App (*_APLICACION) 


void RAVE_Iniciar(); 


#ifdef RAVE_MOSTRAR_CONSOLA 
	#define Debug_EscribirSinMS_Varg(TEXTO, ...)	App.ConsolaDebug.Escribir(TEXTO, __VA_ARGS__);
	#define Debug_Escribir_Varg(TEXTO, ...)			App.ConsolaDebug.EscribirMS(TEXTO, __VA_ARGS__);
	#define Debug_EscribirSinMS(TEXTO)				App.ConsolaDebug.Escribir(TEXTO);
	#define Debug_Escribir(TEXTO)					App.ConsolaDebug.EscribirMS(TEXTO);
	#define Debug_Leer(TEXTO)						App.ConsolaDebug.Leer(TEXTO);
	// Marco que muestra si la función WINAPI tiene un error por la consola (GetLastError)
	#define Debug_MostrarUltimoError()				App.ConsolaDebug.MostrarUltimoError()
	#define Debug_UltimoError(NUM)					SetLastError(NUM)
#else
	#define Debug_EscribirSinMS_Varg(TEXTO, ...)	
	#define Debug_Escribir_Varg(TEXTO, ...)		
	#define Debug_EscribirSinMS(TEXTO)				
	#define Debug_Escribir(TEXTO)				
	#define Debug_Leer(TEXTO)					
	// Marco que muestra si la función WINAPI tiene un error por la consola (GetLastError)
	#define Debug_MostrarUltimoError()				
	#define Debug_UltimoError(NUM)					SetLastError(NUM);	
#endif


/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)   (((i) & 0x80ll) ? '1' : '0'), \
										(((i) & 0x40ll) ? '1' : '0'), \
										(((i) & 0x20ll) ? '1' : '0'), \
										(((i) & 0x10ll) ? '1' : '0'), \
										(((i) & 0x08ll) ? '1' : '0'), \
										(((i) & 0x04ll) ? '1' : '0'), \
										(((i) & 0x02ll) ? '1' : '0'), \
										(((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16			PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i)		PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32			PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i)		PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64			PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i)		PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end macros --- */