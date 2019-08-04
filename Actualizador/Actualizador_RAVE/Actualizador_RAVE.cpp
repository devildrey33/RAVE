// Actualizador_RAVE.exe.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Actualizador_RAVE.h"
#include <DArchivoBinario.h>
#include "..\zlib-1.2.11\zlib.h"
#include <DDirectoriosWindows.h>
#include <DStringUtils.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Colocar código aquí.

	App.Ventana.Crear();

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ACTUALIZADORRAVEEXE));

	MSG msg;

	// Bucle principal de mensajes:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


// Función que inicia la actualización
void ActualizadorApp::Actualizar(void) {
	std::wstring			PathTmp;
	std::wstring            PathInstalador = App.Path(FALSE);
	DWL::DArchivoBinario	Instalador;
	ULONG					ErrorMD5 = 0;
	std::wstring			TxtError;

	// Creo el path Temporal
	DWL::DDirectoriosWindows::Comun_AppData(PathTmp);
	PathTmp += L"\\Rave\\Tmp\\";
	// Borro el directorio Tmp para asegurar que no hay nada antiguo
	BorrarDirectorio(PathTmp.c_str());
	// Creo nuevamente el directorio Tmp
	CreateDirectory(PathTmp.c_str(), NULL);


	if (Instalador.AbrirArchivoLectura(PathInstalador.c_str()) == FALSE) {
		MessageBox(NULL, L"No se puede abrir el instalador", L"Error", MB_OK | MB_ICONERROR);
		#ifndef _DEBUG
			PostQuitMessage(0);
		#endif
		return;
	}

	// Leo el tamaño del instalador vacio y la firma
	char	Firma[32];
	size_t	TamInstaladorVacio = 0;
	ZeroMemory(Firma, sizeof(char) * 32);
	long    Pos = sizeof(size_t) + 16;
	Instalador.PosicionLectura(-Pos, TRUE);
	Instalador.Leer<size_t>(&TamInstaladorVacio);
	Instalador.PosicionLectura(-16, TRUE);
	Instalador.Leer(Firma, 16);

	std::string FirmaStr = "ActualizadorRAVE";
	// La firma no coincide termino la aplicación (solo en la configuración RELEASE)
	if (FirmaStr.compare(Firma) != 0) {
		std::wstring TmpFirma;
		DWL::Strings::AnsiToWide(Firma, TmpFirma);
		TxtError = L"La firma no coincide : " + TmpFirma;
		MessageBox(NULL, TxtError.c_str(), L"Error", MB_OK | MB_ICONERROR);
		#ifndef _DEBUG
			PostQuitMessage(0);
		#endif
		return;
	}

	size_t TotalArchivos = 0;
	// Leo el total de archivos
	Instalador.PosicionLectura(TamInstaladorVacio, FALSE);
	Instalador.Leer<size_t>(&TotalArchivos);
	App.Ventana.Barra.Maximo(static_cast<float>(TotalArchivos));

	// Leo archivo por archivo para guardar los datos descomprimidos en un directorio temporal
	std::wstring			PathArchivoRelativo;
	std::wstring			PathArchivo;
	std::string				MD5;
	size_t					TamDatos = 0;
	char                   *Datos = nullptr;
	DWL::DArchivoBinario	ArchivoDestino;

//	TxtError = L"TotalArchivos : " + std::to_wstring(TotalArchivos);
//	MessageBox(NULL, TxtError.c_str(), L"Error", MB_OK | MB_ICONERROR);

	for (size_t i = 0; i < TotalArchivos; i++) {
		// Leo el path destino
		Instalador.LeerString(PathArchivoRelativo);
		// Leo el MD5 previamente calculado antes de comprimir
		Instalador.LeerString(MD5);
		// Leo el tamaño del archivo comprimido en caracteres
		Instalador.Leer<size_t>(&TamDatos);
		Datos = new char[TamDatos];
		// Leo los datos comprimidos en memória
		Instalador.Leer(Datos, TamDatos);
		// Asigno el path completo del archivo
		PathArchivo = PathTmp + PathArchivoRelativo;
		// Creo los directorios que hagan falta para el archivo
		CrearDirectorios(PathArchivo);
		// Abro el archivo destino para guardar los datos descomprimidos
		ArchivoDestino.AbrirArchivo(PathArchivo.c_str(), TRUE);
		// Actualizo el texto del archivo destino
		App.Ventana.EtiquetaNombreArchivo.Texto(PathArchivo);
		// Descomprimo los datos comprimidos en memória y los vuelco en el ArchivoDestino
		Descomprimir(Datos, TamDatos, ArchivoDestino);
		// Elimino los datos de memória
		delete[] Datos;
		// Si el MD5 no coincide...
		if (MD5.compare(ArchivoDestino.MD5_char()) != 0) {
			ErrorMD5++;
		}
		// Actualizo la barra de progreso
		App.Ventana.Barra.Valor(static_cast<float>(i));
		// Dejo correr los eventos
		App.Eventos_Mirar();
	}

	// Si hay errores en las comparaciones MD5 revierto la actualización
	if (ErrorMD5 > 0) {
		BorrarDirectorio(PathTmp.c_str());

		std::wstring Err = L"Algunos archivos están corruptos... Abortando actualización.";
		MessageBox(NULL, Err.c_str(), L"Error", MB_OK | MB_ICONERROR);

		// TODO : Ejecutar rave con parámetro que indique que el instalador es corrupto y borrar el instalador desde el reproductor

		PostQuitMessage(0);
	}

	PostQuitMessage(0);
}


void ActualizadorApp::CrearDirectorios(std::wstring &Path) {
	DWL::Strings::Split Sp(Path, L'\\');

	std::wstring TmpPath = Sp[0] + L'\\';
	if (Sp.Total() < 2) return;

	for (size_t i = 1; i < Sp.Total() - 1; i++) {
		TmpPath += Sp[i] + L'\\';
		if (GetFileAttributes(TmpPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
			CreateDirectory(TmpPath.c_str(), NULL);
		}
	}
}

//#define CHUNK 16384
#define CHUNK 7680

int ActualizadorApp::Descomprimir(char *Datos, DWORD TamDatos, DWL::DArchivoBinario& Destino) {
	int ret;
	unsigned have;
	z_stream strm;
	char in[CHUNK] = "";
	char out[CHUNK] = "";

	unsigned int Posicion = 0;

	/* allocate inflate state */
	strm.zalloc		= Z_NULL;
	strm.zfree		= Z_NULL;
	strm.opaque		= Z_NULL;
	strm.avail_in	= 0;
	strm.next_in	= Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK) return ret;

	/* decompress until deflate stream ends or end of file */
	do {
		if (Posicion + CHUNK < TamDatos) {
			memcpy(in, &Datos[Posicion], CHUNK);
			strm.avail_in = CHUNK;
		}
		else {
			memcpy(in, &Datos[Posicion], TamDatos - Posicion);
			strm.avail_in = TamDatos - Posicion;
		}
		Posicion += CHUNK;
		if (strm.avail_in == 0)
			break;
		strm.next_in = reinterpret_cast<Bytef*>(&in[0]);

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = reinterpret_cast<Bytef*>(&out[0]);
			ret = inflate(&strm, Z_NO_FLUSH);
			//            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
			have = CHUNK - strm.avail_out;
			Destino.Guardar(out, have);
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


void ActualizadorApp::BorrarDirectorio(const wchar_t* Directorio) {
	WIN32_FIND_DATA		FindInfoPoint;
	HANDLE				hFind = NULL;
	std::wstring        Path = Directorio;
	if (Path[Path.size() - 1] != TEXT('\\')) Path += TEXT('\\');
	Path += L'*';
	hFind = FindFirstFile(Path.c_str(), &FindInfoPoint);

	while (FindNextFile(hFind, &FindInfoPoint) != 0) {
		Path = Directorio;
		if (Path[Path.size() - 1] != TEXT('\\')) Path += TEXT('\\');
		// Miro que no sea "." o ".."
		if (EsNombreValido(FindInfoPoint.cFileName)) {
			Path += FindInfoPoint.cFileName;
			// Si es un directorio
			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Escaneo recursivo
				BorrarDirectorio(Path.c_str());				
			}
			// Si es un archivo
			else {
				DeleteFile(Path.c_str());
			}
		}
	}
	FindClose(hFind);
	RemoveDirectory(Directorio);
}


// Función que comprueba el nombre de archivo por si es válido o no ('.' y '..' no son válidos)
const BOOL ActualizadorApp::EsNombreValido(const wchar_t* nNombre) {
	if (nNombre[0] == TEXT('.') && nNombre[1] == 0)								return FALSE;
	if (nNombre[0] == TEXT('.') && nNombre[1] == TEXT('.') && nNombre[2] == 0)	return FALSE;
	return TRUE;
}

