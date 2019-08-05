#pragma once

#include "resource.h"
#include <DApp.h>
#include "VentanaPrincipal.h"
#include <DArchivoBinario.h>

/*
	Esta aplicaci�n tiene 3 configuraciones : Debug, Release, y DebugGenerado.
		- Las configuraciones Debug y Release ejecutan el instalador vacio.
		- La configuraci�n DebugGenerado ejecuta el GenerarActualizadorRAVE para generar el instalador durante el post-build
			- Esto me permite depurar el instalador funcionando

	Para activar / desactivar el modo DebugGenerado ve al administrador de configuraci�n.
		- Selecciona de configuraci�n activa la Debug (x86 o x64, la que necesites).
		- Establece la configuraci�n Debug o DebugGenerado en Actualizador_RAVE

	NOTA IMPORTANTE : No modificar los directorios de build de la zlib, o no compilar� bien la zlib (ya que tiene post-build con un bat que copia archivos asm)
*/


// Archivo lib de la DWL y de la ZLib seg�n la configuraci�n
#ifdef _DEBUG
	#if _WIN64
		#pragma comment(lib, "..\\..\\Build\\DWL\\x64\\Debug\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x64\\ZlibStatDebug\\zlibstat.lib")
	#else
		#pragma comment(lib, "..\\..\\Build\\DWL\\x86\\Debug\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x86\\ZlibStatDebug\\zlibstat.lib")
	#endif

	#define ELEVAR_PRIVILEGIOS FALSE
#else
	#if _WIN64
		#pragma comment(lib, "..\\..\\Build\\DWL\\x64\\Release\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x64\\ZlibStatRelease\\zlibstat.lib")
	#else
		#pragma comment(lib, "..\\..\\Build\\DWL\\x86\\Release\\DWL3.lib")
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x86\\ZlibStatReleaseWithoutAsm\\zlibstat.lib")
	#endif

	#define ELEVAR_PRIVILEGIOS TRUE
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