#pragma once

#include "resource.h"
#include <DApp.h>


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


class GenerarActualizadorApp : public DWL::DApp {
  public:
	void						EscanearDirectorio(const wchar_t *Path);
	const BOOL		            EsNombreValido(const wchar_t* nNombre);
	std::vector<std::wstring>	Archivos;
};


// Inicio la DWL con la clase GenerarActualizadorApp
INICIAR_DWL3(GenerarActualizadorApp);