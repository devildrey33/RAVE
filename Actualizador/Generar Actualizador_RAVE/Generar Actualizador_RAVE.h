#pragma once

#include "resource.h"
#include <DApp.h>
#include <DArchivoBinario.h>



// Archivo lib de la DWL y de la ZLib según la configuración
#ifdef _DEBUG
	#if _WIN64
		#pragma comment(lib, "..\\..\\Build\\DWL\\x64\\Debug\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x64\\ZlibStatDebug\\zlibstat.lib")
	#else
		#pragma comment(lib, "..\\..\\Build\\DWL\\x86\\Debug\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x86\\ZlibStatDebug\\zlibstat.lib")
	#endif
#else
	#if _WIN64
		#pragma comment(lib, "..\\..\\Build\\DWL\\x64\\Release\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x64\\ZlibStatRelease\\zlibstat.lib")
	#else
		#pragma comment(lib, "..\\..\\Build\\DWL\\x86\\Release\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x86\\ZlibStatRelease\\zlibstat.lib")
	#endif
#endif


class GenerarActualizadorApp : public DWL::DApp {
  public:
								GenerarActualizadorApp(void) : MaxBuffer(0), Datos(nullptr), TamDatos(0) { };
	void						Iniciar(void);
	void						EscanearDirectorio(const wchar_t *Path);
	const BOOL		            EsNombreValido(const wchar_t* nNombre);
	const int					Comprimir(DWL::DArchivoBinario &Origen);
	std::vector<std::wstring>	Archivos;
	DWORD                       MaxBuffer;
	char                       *Datos;
	size_t                      TamDatos;
};


// Inicio la DWL con la clase GenerarActualizadorApp
INICIAR_DWL3(GenerarActualizadorApp);