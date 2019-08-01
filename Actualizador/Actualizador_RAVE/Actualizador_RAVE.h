#pragma once

#include "resource.h"
#include <DApp.h>
#include "VentanaPrincipal.h"

// Archivo lib de la DWL y de la ZLib según la configuración
#ifdef _DEBUG
	#if _WIN64
		#pragma comment(lib, "..\\..\\Build\\DWL\\x64\\Debug\\DWL3.lib")
		#pragma comment(lib, "..\\..\\Build\\ZLib\\x64\\Debug\\zlib.lib")
	#else
		#pragma comment(lib, "..\\..\\Build\\DWL\\x86\\Debug\\DWL3.lib")
		#pragma comment(lib, "..\\..\\Build\\ZLib\\x86\\Debug\\zlib.lib")
	#endif
#else
	#if _WIN64
		#pragma comment(lib, "..\\..\\Build\\DWL\\x64\\Release\\DWL3.lib")
		#pragma comment(lib, "..\\..\\Build\\ZLib\\x64\\Release\\zlib.lib")
	#else
		#pragma comment(lib, "..\\..\\Build\\DWL\\x86\\Release\\DWL3.lib")
		#pragma comment(lib, "..\\..\\Build\\ZLib\\x86\\Release\\zlib.lib")
	#endif
#endif

class ActualizadorApp : public DWL::DApp {
  public:
	VentanaPrincipal	Ventana;
};

// Inicio la DWL con la clase ActualizadorApp
INICIAR_DWL3(ActualizadorApp);