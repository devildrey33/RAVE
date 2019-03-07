#pragma once

#include "VentanaPrincipal.h"
#include "DConsola.h"
#include "Rave_MediaPlayer.h"
#include "ControlesPantallaCompleta.h"
#include "RAVE_Configuracion.h"
#include "VentanaErrorCritico.h"
#include "MemoriaCompartida.h"
#include "RaveBD.h"
//#include "AsociarReproductor.h"
//#include "VentanaAsociarReproductor.h"
#include "VentanaOpcionesRAVE.h"
#include "DToolTipEx.h"
#include "AsignarTeclaRapida.h"
#include <stdlib.h>
#include <random>
#include "ToolTipInfo.h"

using namespace DWL;


enum LineaComando {
	LineaComando_Nada,					// Sin par�metros extra
	LineaComando_Path,					// Viene con uno o varios paths
	LineaComando_ErrorCritico,			// Mostrar ventana de error cr�tico
	LineaComando_AsociarArchivos,		// Ejecuta una instancia de RAVE con permisos de administraci�n para agregar las asociaciones de los medios en el registro de windows.
	LineaComando_DesasociarArchivos,	// Ejecuta una instancia de RAVE con permisos de administraci�n para eliminar las asociaciones del registro de windows.
	LineaComando_Reproducir				// Reproduce el medio especificado en el segundo par�metro
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
									// Al devolver FALSE se salta el bucle de mensajes y termina la aplicaci�n
	const BOOL						Iniciar(int nCmdShow);

									// Inicia los men�s y la ventana principal
	void							IniciarUI(int nCmdShow);

	template <typename T = int>  T	Rand(T Max, T Min = 0) {
										std::mt19937 mt(_rd());
										std::uniform_real_distribution<double> dist(static_cast<double>(Min), static_cast<double>(Max));
										return static_cast<T>(dist(mt));

//										T Rnd = static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
//										return Rnd * (Max - Min) + Min;
									}
									// Obtiene la linea de comando y determina que hay que hacer
	const LineaComando				ObtenerLineaComando(std::vector<std::wstring> &Paths);

	void							Eventos_Mirar(void);

	const TCHAR 				   *ObtenerSO(void);

									// Evento Tecla soltada general de todas las ventanas excepto el video del VLC
	void                            Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado);
									// Evento Tecla soltada general de todas las ventanas excepto el video del VLC
	void                            Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado);

	void							CerrarSistema(const SOCerrarSistema Forma = SOCerrarSistema_Apagar, const BOOL Forzar = FALSE);

	inline void                     MostrarToolTipPlayer(const wchar_t *Texto)			{ _ToolTipPlayer.MostrarToolTip(Texto); };
	inline void                     MostrarToolTipPlayer(std::wstring &Texto)			{ _ToolTipPlayer.MostrarToolTip(Texto); };
	inline void                     MostrarToolTipPlayer(BDMedio &Medio)				{ _ToolTipPlayer.MostrarToolTip(Medio); };
	inline void                     MostrarToolTipPlayerError(const wchar_t *Texto)		{ _ToolTipPlayer.MostrarToolTipError(Texto); };
	inline void                     MostrarToolTipPlayerError(std::wstring &Texto)		{ _ToolTipPlayer.MostrarToolTipError(Texto); };

	inline void                     MostrarToolTipOpciones(const wchar_t *Texto)		{ _ToolTipOpciones.MostrarToolTip(Texto); };
	inline void                     MostrarToolTipOpciones(std::wstring &Texto)			{ _ToolTipOpciones.MostrarToolTip(Texto); };
	inline void                     MostrarToolTipOpcionesError(const wchar_t *Texto)	{ _ToolTipOpciones.MostrarToolTipError(Texto); };
	inline void                     MostrarToolTipOpcionesError(std::wstring &Texto)	{ _ToolTipOpciones.MostrarToolTipError(Texto); };

	inline void                     OcultarToolTipPlayer(void)							{ _ToolTipPlayer.Ocultar(); }
	inline void                     OcultarToolTipOpciones(void)						{ _ToolTipOpciones.Ocultar(); }
	inline void                     OcultarToolTipPlayerRapido(void)					{ _ToolTipPlayer.OcultarRapido(); }
	inline void                     OcultarToolTipOpcionesRapido(void)					{ _ToolTipOpciones.OcultarRapido(); }
	//	int							Rand()

	void							Terminar(void);

									// Ventana principal del reproductor
	VentanaPrincipal				VentanaRave;
									// Ventana para mostrar errores criticos
	VentanaErrorCritico				VentanaErrorCrit;
									// Ventana que muestra el mensaje para las asociaciones de archivo
//	VentanaAsociarReproductor		VentanaAsociar;
									// Ventana para mostrar las opciones
	VentanaOpcionesRAVE             VentanaOpciones;
									// Ventana que muestra una consola al estilo MS-DOS para depuraci�n
	DConsola						ConsolaDebug;

	RaveBD							BD;
//	BaseDatos						BD;
	Rave_MediaPlayer				MP;
	ControlesPantallaCompleta		ControlesPC;



//	DWL::DMenuEx                    Menu_Test;


	BOOL							PlayerInicial;
	HANDLE							MutexPlayer;


//	std::wstring					SO;
									// Directorio de la aplicaci�n, Siempre termina con "\"
	std::wstring					AppPath;

	MemoriaCompartida               MemCompartida;

									// Objeto para asociar este reproductor en el registro de windows
//	AsociarReproductor				AsociarMedios;
									// ID_MENUVIDEO_AUDIO_PISTA_AUDIO
	DMenuEx                        *MenuVideoPistasDeAudio;
	DMenuEx                        *MenuVideoProporcion;
	DMenuEx                        *MenuVideoFiltros;
	DMenuEx                        *MenuVideoSubtitulos;
/*	DMenuEx                        *MenuVideoBrillo;
	DMenuEx                        *MenuVideoContraste;
	DMenuEx                        *MenuVideoSaturacion;*/

	std::vector<TeclaRapida>       TeclasRapidas;
  protected:

	ToolTipsInfo                  _ToolTipPlayer;
	ToolTipsInfo                  _ToolTipOpciones;
//	DToolTipEx                     _ToolTip;
	//DToolTipEx                     _ToolTip2;

	std::random_device			   _rd;
	
	// Token para el GDI+
	//ULONG_PTR					   _gdiplusToken;
};


// Funci�n para pintar texto con sombra utilizando TextOut
static void PintarTexto(HDC DC, const wchar_t *pTexto, const int PosX, const int PosY, COLORREF ColorTexto = COLOR_TOOLTIP_TEXTO, COLORREF ColorSombra = COLOR_TOOLTIP_TEXTO_SOMBRA) {
	SetTextColor(DC, ColorSombra);
	TextOut(DC, PosX + 1, PosY + 1, pTexto, static_cast<int>(wcslen(pTexto)));
	SetTextColor(DC, ColorTexto);
	TextOut(DC, PosX, PosY, pTexto, static_cast<int>(wcslen(pTexto)));
}

static void PintarTexto(HDC DC, std::wstring &sTexto, const int PosX, const int PosY, COLORREF ColorTexto = COLOR_TOOLTIP_TEXTO, COLORREF ColorSombra = COLOR_TOOLTIP_TEXTO_SOMBRA) {
	SetTextColor(DC, ColorSombra);
	TextOut(DC, PosX + 1, PosY + 1, sTexto.c_str(), static_cast<int>(sTexto.size()));
	SetTextColor(DC, ColorTexto);
	TextOut(DC, PosX, PosY, sTexto.c_str(), static_cast<int>(sTexto.size()));
}

extern RAVE *_APLICACION;

#define App (*_APLICACION) 


void RAVE_Iniciar(); 


#ifdef RAVE_MOSTRAR_CONSOLA 
	// Macros para escribir datos en la consola de depuraci�n 
	// Macro que escribe datos con argumentos variables (al estilo printf), sin mostrar milisegundos (OJO! REQUIERE TERMINAR LA L�NEA CON '\n')
	#define Debug_EscribirSinMS_Varg(TEXTO, ...)	App.ConsolaDebug.Escribir(TEXTO, __VA_ARGS__);
	// Macro que escribe datos con argumentos variables (al estilo printf), mostrando los milisegundos desde el �ltimo Debug_Escribir (OJO! REQUIERE TERMINAR LA L�NEA CON '\n')
	#define Debug_Escribir_Varg(TEXTO, ...)			App.ConsolaDebug.EscribirMS(TEXTO, __VA_ARGS__);
	// Macro que escribe datos, sin mostrar milisegundos (OJO! REQUIERE TERMINAR LA L�NEA CON '\n')
	#define Debug_EscribirSinMS(TEXTO)				App.ConsolaDebug.Escribir(TEXTO);
	// Macro que escribe datos, sin mostrar milisegundos, mostrando los milisegundos desde el �ltimo Debug_Escribir (OJO! REQUIERE TERMINAR LA L�NEA CON '\n')
	#define Debug_Escribir(TEXTO)					App.ConsolaDebug.EscribirMS(TEXTO);
	// WUT??
	#define Debug_Leer(TEXTO)						App.ConsolaDebug.Leer(TEXTO);
	// Marco que muestra si la funci�n WINAPI tiene un error por la consola (GetLastError)
	#define Debug_MostrarUltimoError()				App.ConsolaDebug.MostrarUltimoError()
	#define Debug_UltimoError(NUM)					SetLastError(NUM)
#else
	#define Debug_EscribirSinMS_Varg(TEXTO, ...)	
	#define Debug_Escribir_Varg(TEXTO, ...)		
	#define Debug_EscribirSinMS(TEXTO)				
	#define Debug_Escribir(TEXTO)				
	#define Debug_Leer(TEXTO)					
	// Marco que muestra si la funci�n WINAPI tiene un error por la consola (GetLastError)
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