#pragma once

#include "VentanaPrincipal.h"
#include "BaseDatos.h"
#include "DConsola.h"
#include "RaveVLC.h"
#include "DImageList.h"
#include "ControlesPantallaCompleta.h"
#include "DMenu.h"
#include "RAVE_Configuracion.h"
#include "VentanaErrorCritico.h"

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
	const BOOL					Iniciar(int nCmdShow);

								// Inicia los menús y la ventana principal
	void						IniciarUI(int nCmdShow);

								// Obtiene la linea de comando y determina que hay que hacer
	const LineaComando			ObtenerLineaComando(void);


	void						Terminar(void);
	VentanaPrincipal			VentanaRave;
	BaseDatos					BD;
	DConsola					ConsolaDebug;
	RaveVLC						VLC;
	ControlesPantallaCompleta	ControlesPC;
	DWL::DImageList				ListaImagenes16;


	DWL::DMenu 					Menu_ArbolBD;
	DWL::DMenu 					Menu_Mezclar;
	DWL::DMenu 					Menu_Repetir;


	BOOL						PlayerInicial;

	VentanaErrorCritico			VentanaErrorCrit;
};

extern RAVE *_APLICACION;

#define App (*_APLICACION) 


void RAVE_Iniciar(); 


#ifdef MOSTRAR_CONSOLA 
	#define Debug_Escribir_Varg(TEXTO, ...)		App.ConsolaDebug.EscribirMS(TEXTO, __VA_ARGS__);
	#define Debug_Escribir(TEXTO)				App.ConsolaDebug.EscribirMS(TEXTO);
	#define Debug_Leer(TEXTO)					App.ConsolaDebug.Leer(TEXTO);
#else
	#define Debug_Escribir_Varg(TEXTO, ...)		
	#define Debug_Escribir(TEXTO)				
	#define Debug_Leer(TEXTO)					
#endif