// header.h: archivo de inclusión para archivos de inclusión estándar del sistema,
// o archivos de inclusión específicos de un proyecto
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Excluir material rara vez utilizado de encabezados de Windows
// Archivos de encabezado de Windows
#include <windows.h>
// Archivos de encabezado en tiempo de ejecución de C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#ifdef _DEBUG
	#if _WIN32 || _WIN64
		#if _WIN64
			#pragma comment(lib, "..\\Build\\DWL\\x64\\Debug\\DWL3.lib")
		#else
			#pragma comment(lib, "..\\Build\\DWL\\x86\\Debug\\DWL3.lib")
		#endif
	#endif
#else
	#if _WIN32 || _WIN64
		#if _WIN64
			#pragma comment(lib, "..\\Build\\DWL\\x64\\Release\\DWL3.lib")
		#else
			#pragma comment(lib, "..\\Build\\DWL\\x86\\Release\\DWL3.lib")
		#endif
	#endif
#endif