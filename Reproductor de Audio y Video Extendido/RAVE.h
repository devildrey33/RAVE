#pragma once

#include "VentanaPrincipal.h"
#include "DConsola.h"
#include "RaveVLC.h"
#include "ControlesPantallaCompleta.h"
#include "RAVE_Configuracion.h"
#include "VentanaErrorCritico.h"
#include "MemoriaCompartida.h"
#include "RaveBD.h"
#include "AsociarReproductor.h"
#include "VentanaAsociarReproductor.h"
#include <stdlib.h>

using namespace DWL;


enum LineaComando {
	LineaComando_Nada,					// Sin parámetros extra
	LineaComando_Path,					// Viene con uno o varios paths
	LineaComando_ErrorCritico,			// Mostrar ventana de error crítico
	LineaComando_AsociarArchivos,		// Ejecuta una instancia de RAVE con permisos de administración para agregar las asociaciones de los medios en el registro de windows.
	LineaComando_DesasociarArchivos,	// Ejecuta una instancia de RAVE con permisos de administración para eliminar las asociaciones del registro de windows.
	LineaComando_Reproducir				// Reproduce el medio especificado en el segundo parámetro
};

enum SOCerrarSistema {
	SOCerrarSistema_Apagar			= EWX_POWEROFF,
	SOCerrarSistema_ReIniciar		= EWX_REBOOT,
	SOCerrarSistema_CerrarUsuario	= EWX_LOGOFF,
	SOCerrarSistema_Hibernar        = EWX_HYBRID_SHUTDOWN
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

	void							CerrarSistema(const SOCerrarSistema Forma = SOCerrarSistema_Apagar, const BOOL Forzar = FALSE);

//	int							Rand()

	void							Terminar(void);

									// Ventana principal del reproductor
	VentanaPrincipal				VentanaRave;
									// Ventana para mostrar errores criticos
	VentanaErrorCritico				VentanaErrorCrit;
									// Ventana que muestra el mensaje para las asociaciones de archivo
	VentanaAsociarReproductor		VentanaAsociar;
									// Ventana que muestra una consola al estilo MS-DOS para depuración
	DConsola						ConsolaDebug;

	RaveBD							BD;
//	BaseDatos						BD;
	RaveVLC							VLC;
	ControlesPantallaCompleta		ControlesPC;



//	DWL::DMenuEx                    Menu_Test;


	BOOL							PlayerInicial;
	HANDLE							MutexPlayer;


	std::wstring					SO;
									// Directorio de la aplicación, Siempre termina con "\"
	std::wstring					AppPath;

	MemoriaCompartida               MemCompartida;

									// Objeto para asociar este reproductor en el registro de windows
	AsociarReproductor				AsociarMedios;

									// Token para el GDI+
	ULONG_PTR						gdiplusToken;


};


// Función para pintar texto con sombra utilizando TextOut
static void PintarTexto(HDC DC, const wchar_t *pTexto, const int PosX, const int PosY, COLORREF ColorTexto = COLOR_TEXTO, COLORREF ColorSombra = COLOR_TEXTO_SOMBRA) {
	SetTextColor(DC, ColorSombra);
	TextOut(DC, PosX + 1, PosY + 1, pTexto, static_cast<int>(wcslen(pTexto)));
	SetTextColor(DC, ColorTexto);
	TextOut(DC, PosX, PosY, pTexto, static_cast<int>(wcslen(pTexto)));
}

static void PintarTexto(HDC DC, std::wstring &sTexto, const int PosX, const int PosY, COLORREF ColorTexto = COLOR_TEXTO, COLORREF ColorSombra = COLOR_TEXTO_SOMBRA) {
	SetTextColor(DC, ColorSombra);
	TextOut(DC, PosX + 1, PosY + 1, sTexto.c_str(), static_cast<int>(sTexto.size()));
	SetTextColor(DC, ColorTexto);
	TextOut(DC, PosX, PosY, sTexto.c_str(), static_cast<int>(sTexto.size()));
}

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