#pragma once

#include "resource.h"
#include <DApp.h>
#include "VentanaPrincipal.h"
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


class ActualizadorApp : public DWL::DApp {
  public:
	VentanaPrincipal			Ventana;
	void						Actualizar(void);
	int							Descomprimir(char *Datos, DWORD TamDatos, DWL::DArchivoBinario& Destino);
	void						BorrarDirectorio(const wchar_t* Directorio);
	const BOOL					EsNombreValido(const wchar_t* nNombre);
	void						CrearDirectorios(std::wstring &Path);
	std::vector<std::wstring>	Paths;
};

// Inicio la DWL con la clase ActualizadorApp
INICIAR_DWL3(ActualizadorApp);