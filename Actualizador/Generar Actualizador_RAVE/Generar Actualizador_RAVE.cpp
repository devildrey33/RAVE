// Generar Actualizador_RAVE.exe.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Generar Actualizador_RAVE.h"
#include "..\zlib-1.2.11\zlib.h"
#include <DStringUtils.h>

//#define CHUNK 16384
#define CHUNK 7680

#ifdef _WIN64
	#define DIRECTORIO_RAVE L"Release-x64"
	#define PATH_FINAL		L"Actualizador\\Actualizador_RAVE_x64.exe"
	#ifdef _DEBUG
		#define TITULO		L"Instalador x64 Debug"
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x64\\Debug\\Actualizador_RAVE.exe"
	#else
		#define TITULO		L"Instalador x64 Release"
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x64\\Release\\Actualizador_RAVE.exe"
	#endif
#else
	#define DIRECTORIO_RAVE L"Release-x86"
	#define PATH_FINAL		L"Actualizador\\Actualizador_RAVE_x86.exe"
	#ifdef _DEBUG
		#define TITULO		L"Instalador x86 Debug"
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x86\\Debug\\Actualizador_RAVE.exe"
	#else
		#define TITULO		L"Instalador x86 Release"
		#define PATH_BUILD	L"Build\\Actualizador_RAVE\\x86\\Release\\Actualizador_RAVE.exe"
	#endif
#endif


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	App.Iniciar();
	return 0;
}


void GenerarActualizadorApp::Iniciar(void) {
	std::wstring DirectorioRAVE;		// Raíz del proyecto RAVE
	std::wstring DirectorioArchivos;	// Path donde está ubicado el reproductor compilado y con todas sus dlls
	std::wstring PathInstaladorVacio;	// Path donde se ubica el instalador vacio
	std::wstring PathInstaladorFinal;	// Path donde se ubica el instalador con los archivos comprimidos
	std::wstring Tmp;
	std::wstring nPathActual = PathActual();

	BOOL PostCompilacion = FALSE;

	DWL::Strings::Split Sp(nPathActual, L'\\');
	// Ejecutandose desde el compilador
	if (Sp[Sp.Total() - 1].compare(L"Generar Actualizador_RAVE") == 0) {
		Tmp = L"Actualizador\\Generar Actualizador_RAVE";
	}
	// Ejecutandose para evento de post-compilación
	else if (Sp[Sp.Total() - 1].compare(L"Actualizador_RAVE") == 0) {
		Tmp = L"Actualizador\\Actualizador_RAVE";
		PostCompilacion = TRUE;
	}
	// Ejecutandose desde el directorio Actualizador
	else if (Sp[Sp.Total() - 1].compare(L"Actualizador") == 0) {
		Tmp = L"Actualizador";
	}
	// Ni idea de donde estamos, salgo
	else {
		MessageBox(NULL, nPathActual.c_str(), L"Error", MB_OK | MB_ICONERROR);
		return;
	}
	ConsolaDebug.Crear(L"Generando Actualizador.exe");
	ConsolaDebug.EscribirMS(L"Ejecutandose desde : '%s'\n", nPathActual.c_str());
	// Recorto el path del Directorio RAVE para que quede con la barra al final
	DirectorioRAVE = nPathActual.substr(0, nPathActual.size() - Tmp.size());

	// Agrego el directorio de la buld (32 / 64 bits)
	DirectorioArchivos = DirectorioRAVE + DIRECTORIO_RAVE;


	ConsolaDebug.EscribirMS(L"Paso 1 : Enumerar archivos a comprimir...");
	EscanearDirectorio(DirectorioArchivos.c_str());
	ConsolaDebug.Escribir(L" Se han encontrado %d archivos.\n", Archivos.size());

	ConsolaDebug.EscribirMS(L"Paso 2 : Analizar y almacenar el instalador vacio.");

	PathInstaladorVacio = DirectorioRAVE + PATH_BUILD;
	PathInstaladorFinal = DirectorioRAVE + PATH_FINAL;
	DWL::DArchivoBinario InstaladorVacio, InstaladorFinal, Archivo;

	// Abro el instalador vacio para lectura
	InstaladorVacio.AbrirArchivoLectura(PathInstaladorVacio.c_str());
	// Obtengo el tamaño del instalador vacio
	size_t TamInstaladorVacio = InstaladorVacio.PosicionLectura(0, TRUE);

	if (InstaladorVacio.EstaAbierto() == FALSE) {
		MessageBox(NULL, L"No se encuentra el instalador vacio", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Abro el instalador final para escritura
	InstaladorFinal.AbrirArchivoEscritura(PathInstaladorFinal.c_str());
	Datos = new char[MaxBuffer]; // Creo memoria temporal para leer los archivos (el tamaño de la memoria equivale al archivo mas grande escaneado)
	InstaladorVacio.PosicionLectura(0, FALSE);
	InstaladorVacio.Leer(Datos, TamInstaladorVacio);
	InstaladorVacio.Cerrar();
	// Guardo el instalador vacio dentro del instalador final
	InstaladorFinal.Guardar(Datos, TamInstaladorVacio);
	ConsolaDebug.Escribir(L" $d bytes guardados.\n", TamInstaladorVacio);

	ConsolaDebug.EscribirMS(L"Paso 3 : Comprimir los archivos.\n");
	// Guardo el total de archivos en un size_t
	size_t T = Archivos.size();
	InstaladorFinal.Guardar<size_t>(&T);

	std::string		MD5;
	std::wstring	PathArchivo;
	
	for (size_t i = 0; i < Archivos.size(); i++) {
		// Abro el archivo a comprimir
		Archivo.AbrirArchivoLectura(Archivos[i].c_str());
		// Recorto el path del archivo para guardarlo dentro del instalador
		PathArchivo = Archivos[i].substr(DirectorioArchivos.size() + 1, Archivos[i].size() - (DirectorioArchivos.size() + 1));
		// Guardo el path relativo del archivo
		InstaladorFinal.GuardarString(PathArchivo);
		// Guardo el MD5 del archivo
		MD5 = Archivo.MD5_char();
		InstaladorFinal.GuardarString(MD5);
		// Comprimo el archivo en memória
		Comprimir(Archivo);
		// Guardo el tamaño del archivo
		InstaladorFinal.Guardar<size_t>(&TamDatos);
		// Guardo el archivo
		InstaladorFinal.Guardar(Datos, TamDatos);
		// Cierro el archivo de lectura
		Archivo.Cerrar();
		ConsolaDebug.EscribirMS(L"%s\n", Archivos[i].c_str());
	}

	// Guardo el tamaño del instalador al final del archivo, para saber donde posicionarme nada mas empezar
	InstaladorFinal.Guardar<size_t>(&TamInstaladorVacio);
	// Guardo la firma del instalador (si no se encuentra la firma, el instalador no es válido)
	std::string Firma("ActualizadorRAVE");
	InstaladorFinal.GuardarString(Firma, FALSE);
	InstaladorFinal.Cerrar();

	// Elimino la memória temporal para leer los archivos
	delete[] Datos;

	// Evento de post-compilación de Actualizador_RAVE para poder depurar un instalador con archivos comprimidos
	if (PostCompilacion == TRUE) {
//		MessageBox(NULL, PathInstaladorFinal.c_str(), L"", MB_OK);
//		MessageBox(NULL, PathInstaladorVacio.c_str(), L"", MB_OK);
		CopyFile(PathInstaladorFinal.c_str(), PathInstaladorVacio.c_str(), FALSE);
	}
	else {
		int R = MessageBox(NULL, L"El instalador se ha creado correctamente", TITULO, MB_OK);
	}
	
}

// Genera el archivo comprimido en memória
const int GenerarActualizadorApp::Comprimir(DWL::DArchivoBinario &Origen) {
	int ret, flush;
	unsigned have;
	z_stream strm;
	char in[CHUNK];
	char out[CHUNK];

	// allocate deflate state 
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, 5);
	if (ret != Z_OK) return ret;

	Origen.PosicionLectura(0, false);
	TamDatos = 0;

	// compress until end of file 
	do {
		Origen.Leer(in, CHUNK * sizeof(char));
		strm.avail_in = (uInt)Origen.BytesLeidos();
		/*		if (Origen.Error() == true) {
					(void)deflateEnd(&strm);
					return Z_ERRNO;
				}*/
		flush = Origen.FinalDelArchivo() ? Z_FINISH : Z_NO_FLUSH;
		//        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = reinterpret_cast<Bytef*>(&in[0]);

		// run deflate() on input until output buffer not full, finish
		//   compression if all of source has been read in 
		do {
			strm.avail_out = CHUNK;
			strm.next_out = reinterpret_cast<Bytef*>(&out[0]);
			ret = deflate(&strm, flush);    // no bad return value 
			have = CHUNK - strm.avail_out;
			CopyMemory(&Datos[TamDatos], out, have);
			TamDatos += have;
//			Destino.AgregarParte(out, have);
			/*if (Destino.Error() == true) {
				(void)deflateEnd(&strm);
				return Z_ERRNO;
			}*/
		} while (strm.avail_out == 0);

	} while (flush != Z_FINISH);

	// clean up and return 
	(void)deflateEnd(&strm);
	return Z_OK;

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
				if (MaxBuffer < FindInfoPoint.nFileSizeLow) {
					MaxBuffer = FindInfoPoint.nFileSizeLow;
				}
				// Guardo el path en el vector de archivos
				Archivos.push_back(Path);
			}
		}
	}
	FindClose(hFind);

}
