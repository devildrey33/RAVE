// Generar Actualizador_RAVE.exe.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Generar Actualizador_RAVE.h"
#include <DArchivoBinario.h>

#ifdef _WIN64
	#define DIRECTORIO_RAVE L"Release-x64"
	#define PATH_FINAL		L"Actualizador\\Actualizador_RAVE_x64.exe"
	#ifdef _DEBUG
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x64\\Debug\\Actualizador_RAVE.exe"
	#else
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x64\\Release\\Actualizador_RAVE.exe"
	#endif
#else
	#define DIRECTORIO_RAVE L"Release-x86"
	#define PATH_FINAL		L"Actualizador\\Actualizador_RAVE_x86.exe"
	#ifdef _DEBUG
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x86\\Debug\\Actualizador_RAVE.exe"
	#else
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x86\\Release\\Actualizador_RAVE.exe"
	#endif
#endif


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::wstring DirectorioRAVE;		// Raíz del proyecto RAVE
	std::wstring DirectorioArchivos;	// Path donde está ubicado el reproductor compilado y con todas sus dlls
	std::wstring PathInstaladorVacio;	// Path donde se ubica el instalador vacio
	std::wstring PathInstaladorFinal;	// Path donde se ubica el instalador con los archivos comprimidos
	std::wstring Tmp;
	std::wstring PathActual = App.PathActual();

	App.ConsolaDebug.Crear(L"Generando Actualizador.exe");
	App.ConsolaDebug.EscribirMS(L"Ejecutandose desde : '%s'\n", PathActual.c_str());	
	// Recorto el path del Directorio RAVE
	Tmp = L"Actualizador\\Generar Actualizador_RAVE";
	DirectorioRAVE = PathActual.substr(0, PathActual.size() - Tmp.size());

	// Agrego el directorio de la buld (32 / 64 bits)
	DirectorioArchivos = DirectorioRAVE + DIRECTORIO_RAVE;


	App.ConsolaDebug.EscribirMS(L"Paso 1 : Enumerar archivos a comprimir...");
	App.EscanearDirectorio(DirectorioArchivos.c_str());
	App.ConsolaDebug.Escribir(L" Se han encontrado %d archivos.\n", App.Archivos.size());
	
	App.ConsolaDebug.EscribirMS(L"Paso 2 : Analizar y almacenar el instalador vacio.");

	PathInstaladorVacio = DirectorioRAVE + PATH_BUILD;	
	PathInstaladorFinal = DirectorioRAVE + PATH_FINAL;
	DWL::DArchivoBinario InstaladorVacio, InstaladorFinal;
	// Abro el instalador vacio para lectura
	InstaladorVacio.AbrirArchivoLectura(PathInstaladorVacio.c_str());
	// Obtengo el tamaño del instalador vacio
	DWORD TamInstaladorVacio = InstaladorVacio.Posicion(0, TRUE);
	
	if (InstaladorVacio.EstaAbierto() == FALSE) {
		MessageBox(NULL, L"No se encuentra el instalador vacio", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	// Abro el instalador final para escritura
	InstaladorFinal.AbrirArchivoEscritura(PathInstaladorFinal.c_str());
	char* Datos = new char[TamInstaladorVacio]; // Creo la memória para leer el instalador vacio
	InstaladorVacio.Posicion(0, FALSE);
	InstaladorVacio.Leer(Datos, TamInstaladorVacio);

	// Guardo el instalador vacio dentro del instalador final
	InstaladorFinal.Guardar(Datos, TamInstaladorVacio);

	// Elimino la memória que contiene el instalador vacio
	delete[] Datos;



	int R = MessageBox(NULL, L"El instalador se ha creado correctamente", L"Terminado", MB_OK);

	return 0;
}



// Función que comprueba el nombre de archivo por si es válido o no ('.' y '..' no son válidos)
const BOOL GenerarActualizadorApp::EsNombreValido(const wchar_t* nNombre) {
	if (nNombre[0] == TEXT('.') && nNombre[1] == 0)								return FALSE;
	if (nNombre[0] == TEXT('.') && nNombre[1] == TEXT('.') && nNombre[2] == 0)	return FALSE;
	return TRUE;
}


// Función que Escanea recursivamente un directorio y almacena todos los paths de los archivos en el vector _Archivos
void GenerarActualizadorApp::EscanearDirectorio(const wchar_t *nPath) {
	WIN32_FIND_DATA		FindInfoPoint;
	HANDLE				hFind = NULL;
	std::wstring        Path = nPath;
	if (Path[Path.size() - 1] != TEXT('\\')) Path += TEXT('\\');
	std::wstring		Tmp = Path + L'*';
	hFind = FindFirstFile(Tmp.c_str(), &FindInfoPoint);

	while (FindNextFile(hFind, &FindInfoPoint) != 0) {
		Path = nPath;
		if (Path[Path.size() - 1] != TEXT('\\')) Path += TEXT('\\');
		// Miro que no sea "." o ".."
		if (EsNombreValido(FindInfoPoint.cFileName)) {
			Path += FindInfoPoint.cFileName;
			// Si es un directorio
			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Escaneo recursivo
				EscanearDirectorio(Path.c_str());
			}
			// Si es un archivo
			else {
				// Guardo el path en el vector de archivos
				Archivos.push_back(Path);
			}
		}
	}
	FindClose(hFind);

}
