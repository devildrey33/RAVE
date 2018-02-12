// stdafx.h: archivo de inclusión de los archivos de inclusión estándar del sistema
// o archivos de inclusión específicos de un proyecto utilizados frecuentemente,
// pero rara vez modificados
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Excluir material rara vez utilizado de encabezados de Windows
// Archivos de encabezado de Windows:
#include <windows.h>

// Archivos de encabezado en tiempo de ejecución de C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: mencionar aquí los encabezados adicionales que el programa necesita

#pragma comment(lib, "Shlwapi.lib")		// Libreria ShellWinApi.dll (para probar la función PathFileExists)
#pragma comment(lib, "Comctl32.lib")	// Libreria para InitCommonControlEx

#pragma comment(lib, "UxTheme.lib")		// Libreria para los temas extendidos de windows (a partir de windows XP)
#include <Uxtheme.h>

// Estilo visual XP+
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "RAVE.h"


// Controles de VentanaPrincipal
#define ID_OPCIONES			1998
#define ID_VERVIDEO         1999
#define ID_LISTAMEDIOS		2000
#define ID_ARBOLBD			2001
#define ID_BOTON_PLAY		2002
#define ID_BOTON_PAUSA		2003
#define ID_BOTON_STOP		2004
#define ID_BOTON_SIGUIENTE  2005
#define ID_BOTON_ANTERIOR   2006

#define ID_BOTON_MEZCLAR    2007
#define ID_BOTON_REPETIR    2008

#define ID_BOTON_OPCIONES   2009
#define ID_BOTON_LISTA		2010
#define ID_BOTON_BD			2011
#define ID_BOTON_VIDEO      2012

#define ID_SLIDER_TIEMPO			2013
#define ID_SLIDER_VOLUMEN           2014

#define ID_LABEL_TIEMPOACTUAL		2015
#define ID_LABEL_TIEMPOSEPARADOR	2016
#define ID_LABEL_TIEMPOTOTAL		2017
#define ID_LABEL_VOLUMEN			2018

#define ID_MARCOSD                  2020
#define ID_MARCOSI                  2021
#define ID_MARCOII                  2022

#define ID_LISTARAIZ                2023
#define ID_COMBORAIZ                2024


#define ID_MENUBD_AGREGARALISTA			2100
#define ID_MENUBD_AGREGARANUEVALISTA	2101
#define ID_MENUBD_ACTUALIZAR			2102

#define ID_MENUMEZCLAR_SI				2110
#define ID_MENUMEZCLAR_NO				2111

#define ID_REPETIR_NO					2120
#define ID_REPETIR_SI					2121
#define ID_REPETIR_SI_MEZCLAR			2122
#define ID_REPETIR_SI_APAGAR_REP		2123
#define ID_REPETIR_SI_APAGAR_WIN		2124

#define TIMER_LISTA						2200
#define TIMER_TIEMPO					2201
#define TIMER_REPINTARVLC				2202
#define TIMER_OBTENERVLCWND				2203
#define TIMER_CPC_INACTIVIDAD			2204

// Controles Ventana error crítico
#define ID_VEC_ENVIAR					2300
#define ID_VEC_SALIR					2301
#define ID_VEC_TEXTOERROR				2302
#define ID_VEC_PROGRESO					2303