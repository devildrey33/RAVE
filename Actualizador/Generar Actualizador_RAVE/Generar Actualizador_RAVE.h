#pragma once

#include "resource.h"
#include <DApp.h>
#include <DArchivoBinario.h>


/*
	Esta aplicación tiene por objetivo fusionar un ejecutable con varios archivos comprimidos, (el ejecutable fusionado deberia saber como tratar esos archivos comprimidos)
	Puede funcionar de 3 formas :
		- Desde el directorio "RAVE\Actualizador\Generar Actualizador_RAVE\"		cuando se ejecuta desde el visual studio, crea el instalador en la carpeta "RAVE\Actualizador\"
		- Desde el directorio "RAVE\Actualizador\"									cuando se ejecuta desde el explorador, crea el instalador en la misma carpeta.
		- Desde el directorio "RAVE\Actualizador\Actualizador_RAVE\"				cuando se ejecuta en el post-build de la configuración DebugGenerado del Actualizador_RAVE
			- Este modo guarda el ejecutable final en la carpeta build debug para que se pueda depurar

	NOTA IMPORTANTE : No modificar los directorios de build de la zlib, o no compilará bien la zlib (ya que tiene post-build con un bat que copia archivos asm)
*/


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
		#pragma comment(lib, "..\\zlib-1.2.11\\contrib\\vstudio\\vc14\\x86\\ZlibStatReleaseWithoutAsm\\zlibstat.lib")
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