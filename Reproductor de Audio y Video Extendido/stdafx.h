// stdafx.h: archivo de inclusi�n de los archivos de inclusi�n est�ndar del sistema
// o archivos de inclusi�n espec�ficos de un proyecto utilizados frecuentemente,
// pero rara vez modificados
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Excluir material rara vez utilizado de encabezados de Windows
// Archivos de encabezado de Windows:
#include <windows.h>

// Archivos de encabezado en tiempo de ejecuci�n de C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: mencionar aqu� los encabezados adicionales que el programa necesita
// GDI +
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")


#pragma comment(lib, "Shlwapi.lib")		// Libreria ShellWinApi.dll (para probar la funci�n PathFileExists)
#pragma comment(lib, "Comctl32.lib")	// Libreria para InitCommonControlEx

#pragma comment(lib, "UxTheme.lib")		// Libreria para los temas extendidos de windows (a partir de windows XP)
#include <Uxtheme.h>

// Estilo visual XP+
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "dwmapi")			// Librer�a para DwmExtendFrameIntoClientArea 

#define VLC_API
#define VLC_USED
#include "RAVE.h"


// Controles de VentanaPrincipal
#define ID_OPCIONES								1998
#define ID_VERVIDEO								1999
#define ID_LISTAMEDIOS							2000
#define ID_ARBOLBD								2001
#define ID_BOTON_PLAY							2002
#define ID_BOTON_PAUSA							2003
#define ID_BOTON_STOP							2004
#define ID_BOTON_SIGUIENTE						2005
#define ID_BOTON_ANTERIOR						2006

#define ID_BOTON_MEZCLAR						2007
#define ID_BOTON_REPETIR						2008

#define ID_BOTON_OPCIONES						2009
#define ID_BOTON_LISTA							2010
#define ID_BOTON_BD								2011
#define ID_BOTON_VIDEO							2012

#define ID_SLIDER_TIEMPO						2013
#define ID_SLIDER_VOLUMEN						2014

#define ID_LABEL_TIEMPOACTUAL					2015
#define ID_LABEL_TIEMPOSEPARADOR				2016
#define ID_LABEL_TIEMPOTOTAL					2017
#define ID_LABEL_VOLUMEN						2018

#define ID_MARCOSD								2020
#define ID_MARCOSI								2021
#define ID_MARCOII								2022
	
#define ID_LISTARAIZ						    2023
//#define ID_COMBORAIZ							2024
#define ID_BOTON_AGREGARRAIZ					2024



#define ID_MENUBD_AGREGARALISTA					2100
#define ID_MENUBD_AGREGARANUEVALISTA			2101
#define ID_MENUBD_ACTUALIZAR					2102
#define ID_MENUBD_ANALIZAR					    2103

/*#define ID_MENUMEZCLAR_SI						2110
#define ID_MENUMEZCLAR_NO						2111*/

#define ID_REPETIR_NO							2120
#define ID_REPETIR_SI							2121
#define ID_REPETIR_SI_MEZCLAR					2122
#define ID_REPETIR_SI_APAGAR_REP				2123
#define ID_REPETIR_SI_APAGAR_WIN				2124
#define ID_REPETIR_SI_HIBERNAR_WIN				2125
#define ID_REPETIR_SEPARADOR			 	    2126

#define ID_MENULISTA_ABRIRCARPETA				2130
#define ID_MENULISTA_ELIMINAR					2131
#define ID_MENULISTA_MOSTRARBD					2132
#define ID_MENULISTA_NOTA						2133
#define ID_MENULISTA_PROPIEDADES				2134

// Menu boton lista
#define	ID_MENUBOTONLISTA_BORRAR				2140
#define ID_MENUBOTONLISTA_GENERAR				2141
#define ID_MENUBOTONLISTA_GENERAR_GENERO		2142
#define ID_MENUBOTONLISTA_GENERAR_GRUPO			2143
#define ID_MENUBOTONLISTA_GENERAR_DISCO			2144
#define ID_MENUBOTONLISTA_GENERAR_50MEDIOS		2145

#define ID_MENUVIDEO_AUDIO						2150
#define ID_MENUVIDEO_VIDEO						2151
#define ID_MENUVIDEO_SUBTITULOS					2152
#define ID_MENUVIDEO_AUDIO_PISTA_AUDIO			2153
#define ID_MENUVIDEO_AUDIO_PISTAS_AUDIO			2160 // Inicio de las pistas de audio del VLC 
													 // (DEJAR COMO M�NIMO 10)
#define ID_MENUVIDEO_AUDIO_PISTAS_AUDIO_FIN		2180

#define TIMER_LISTA								2200
#define TIMER_TIEMPO							2201
#define TIMER_REPINTARVLC						2202
#define TIMER_OBTENERVLCWND						2203
#define TIMER_CPC_INACTIVIDAD					2204
//#define TIMER_OBTENER_TIEMPO_TOTAL      2205

// Controles Ventana error cr�tico
#define ID_VEC_ENVIAR							2300
#define ID_VEC_SALIR							2301
#define ID_VEC_TEXTOERROR						2302
#define ID_VEC_PROGRESO							2303