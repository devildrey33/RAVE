#pragma once

// Definici�n de los mensajes especiales para el reproductor

// Thread actualizar BD
#define WM_TBA_AGREGARDIR				WM_USER + 2000
#define WM_TBA_AGREGARRAIZ				WM_USER + 2001
#define WM_TBA_TERMINADO				WM_USER + 2002
#define WM_TBA_CANCELADO				WM_USER + 2003
// Agregar un medio desde el explorador (dobleclick o menu)
#define WM_AGREGARMEDIO					WM_USER + 2004
#define WM_REPRODUCIRMEDIO				WM_USER + 2005
// Thread agregar medios externos
#define WM_TAAL_AGREGARMEDIO			WM_USER + 2006
#define WM_TAAL_TERMINADO				WM_USER + 2007
// Thread obtener metadatos
#define WM_TOM_INICIADO1				WM_USER + 2008
#define WM_TOM_INICIADO2				WM_USER + 2009
#define WM_TOM_INICIADO3				WM_USER + 2010
#define WM_TOM_TOTALMEDIOS1				WM_USER + 2011
#define WM_TOM_TOTALMEDIOS2				WM_USER + 2012
#define WM_TOM_TOTALMEDIOS3				WM_USER + 2013
#define WM_TOM_TERMINADO				WM_USER + 2014
#define WM_TOM_CANCELADO				WM_USER + 2015
#define WM_TOM_MOSTRARVENTANA			WM_USER + 2016
// Temporizadores ejecutados con post message
#define WM_TIMER_LISTA					WM_USER + 2017
#define WM_TIMER_TIEMPO					WM_USER + 2018
#define WM_MEDIO_TERMINADO				WM_USER + 2019
#define WM_TIMER_OBTENERHWNDVLC			WM_USER + 2020
// Actualizaci�n encontrada
#define WM_ACTUALIZACION_ENCONTRADA		WM_USER + 2021
// Actualizaci�n descargada
#define WM_ACTUALIZACION_DESCARGADA		WM_USER + 2022