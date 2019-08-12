// Actualizador_RAVE.exe.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Actualizador_RAVE.h"
#include <DArchivoBinario.h>
#include "..\zlib-1.2.11\zlib.h"
#include <DDirectoriosWindows.h>
#include <DStringUtils.h>
#include <shellapi.h>
#include <DRegistro.h>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	
	// No tiene privilegios de administración
#if ELEVAR_PRIVILEGIOS == TRUE
	if (IsUserAnAdmin() == FALSE) {
		// Ejecuto la aplicación y pido privilegios de administración
		ShellExecute(NULL, TEXT("RunAs"), App.Path(FALSE).c_str(), L"", App.Path(TRUE).c_str(), SW_SHOWNORMAL);
		// Salgo de esta aplicación que no tiene privilegios
		return 0;
		// Recuerda que una vez re-ejecutada como administrador no hay depurador...
	}
#endif

	
	// Creo la ventana principal, la cual inicia la actualización
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
	std::wstring			PathNuevo;							// Path temporal donde se descomprimen todos los archivos
	std::wstring			PathViejo;							// Path temporal donde se movera el reproductor viejo
	std::wstring            PathInstalador = App.Path(FALSE);	// Path donde está ubicado el instalador
	std::wstring			PathReproductor;					// Path donde está ubicado el reproductor
	std::wstring			PathEjecutable;						// Path del ejecutable principal del reproductor

	DWL::DArchivoBinario	Instalador;
	ULONG					ErrorMD5 = 0;
	std::wstring			Tmp, Texto;

	// Obtengo el path del reproductor desde el registro
	#ifdef _WIN64
		DWL::DRegistro::ObtenerValor_String(HKEY_CURRENT_USER, L"Software\\Rave", L"Path64", PathReproductor);
	#else
		DWL::DRegistro::ObtenerValor_String(HKEY_CURRENT_USER, L"Software\\Rave", L"Path32", PathReproductor);
	#endif
	// No se encuentra el path en el registro, abortando...
	if (PathReproductor.size() == 0) {
		MessageBox(NULL, L"No se encuentra el path del reproductor en el registro, re-instala y ejecuta el reproductor para solucionar el problema.", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}

	// Guardo el path del ejecutable
	PathEjecutable = PathReproductor + L"Rave.exe";

	// Creo el path Temporal
	DWL::DDirectoriosWindows::Comun_AppData(PathNuevo);
	PathViejo = PathNuevo;
	PathNuevo += L"\\Rave\\Nuevo\\";
	PathViejo += L"\\Rave\\Viejo\\";

	// Borro el directorio Tmp para asegurar que no hay nada antiguo
	BorrarDirectorio(PathNuevo.c_str());
	BorrarDirectorio(PathViejo.c_str());

	// Creo nuevamente el directorio Nuevo
	CreateDirectory(PathNuevo.c_str(), NULL);

	// Abro el instalador para lectura
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

	// Creo la firma para comparar
	std::string FirmaStr = "ActualizadorRAVE";

	// La firma no coincide termino la aplicación (solo en la configuración RELEASE)
	if (FirmaStr.compare(Firma) != 0) {
		std::wstring TmpFirma;
		DWL::Strings::AnsiToWide(Firma, TmpFirma);
		Tmp = L"La firma no coincide : " + TmpFirma;
		MessageBox(NULL, Tmp.c_str(), L"Error", MB_OK | MB_ICONERROR);
		#ifndef _DEBUG
			PostQuitMessage(0);
		#endif
		return;
	}

	// Leo el total de archivos
	size_t TotalArchivos = 0;
	Instalador.PosicionLectura((long)TamInstaladorVacio, FALSE);
	Instalador.Leer<size_t>(&TotalArchivos);

	// Si no hay archivos, salgo
	if (TotalArchivos == 0) {
		MessageBox(NULL, L"No hay archivos comprimidos", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}

	// Asigno el máximo para la barra
	App.Ventana.Barra.Maximo(static_cast<float>(TotalArchivos - 1));

	// Leo archivo por archivo para guardar los datos descomprimidos en un directorio temporal
	std::wstring			PathArchivoRelativo;
	std::wstring			PathArchivo;
	std::string				MD5, CompMD5;
	size_t					TamDatos = 0;
	char                   *Datos = nullptr;
	DWL::DArchivoBinario	ArchivoDestino;

	// Cierro el reproductor si no estaba cerrado
	HWND Reproductor = FindWindowEx(NULL, NULL, L"RAVE_VentanaPrincipal", NULL);
	if (Reproductor != NULL) SendMessage(Reproductor, WM_CLOSE, 0, 0);

	// Vuelvo a cerrar el reproductor si tenia 2 abiertos (el de debug)
//	Reproductor = FindWindowEx(NULL, NULL, L"RAVE_VentanaPrincipal", NULL);
//	if (Reproductor != NULL) SendMessage(Reproductor, WM_CLOSE, 0, 0);


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
		PathArchivo = PathNuevo + PathArchivoRelativo;
		// Creo los directorios que hagan falta para el archivo
		CrearDirectorios(PathArchivo);
		// Abro el archivo destino para guardar los datos descomprimidos
		ArchivoDestino.AbrirArchivo(PathArchivo.c_str(), TRUE);
		// Actualizo el texto del archivo destino
		App.Ventana.EtiquetaNombreArchivo.Texto(PathArchivo);
		// Descomprimo los datos comprimidos en memória y los vuelco en el ArchivoDestino
		Descomprimir(Datos, (DWORD)TamDatos, ArchivoDestino);
		// Elimino los datos de memória
		delete[] Datos;
		// Si el MD5 no coincide...
		CompMD5 = ArchivoDestino.MD5_char();
		if (MD5.compare(CompMD5) != 0) {
			ErrorMD5++;
		}
		// Cierro el archivo
		ArchivoDestino.Cerrar();
		// Actualizo la barra de progreso
		App.Ventana.Barra.Valor(static_cast<float>(i));
		// Dejo correr los eventos
		App.Eventos_Mirar();
	}

	// Si hay errores en las comparaciones MD5 revierto la actualización
	if (ErrorMD5 > 0) {
		// Borro el directorio temporal "Nuevo"
		BorrarDirectorio(PathNuevo.c_str());

		// Muestro un mensaje de error
		std::wstring Err = L"Uno o más archivos están corruptos... Abortando actualización.";
		MessageBox(NULL, Err.c_str(), L"Error", MB_OK | MB_ICONERROR);

		// TODO : Ejecutar rave con parámetro que indique que el instalador es corrupto y borrar el instalador desde el reproductor
		ShellExecute(NULL, L"open", PathEjecutable.c_str(), L"-ActualizadorCorrupto", PathReproductor.c_str(), SW_SHOW);

		// Salgo de la App
		PostQuitMessage(0);
		return;
	}

	
	// Muevo la carpeta del reproductor a ProgramData\Rave\Old
	BOOL Mover = MoveFileEx(PathReproductor.c_str(), PathViejo.c_str(), MOVEFILE_COPY_ALLOWED);

	if (Mover != FALSE) {
		// Muevo los nuevos archivos a la carpeta del reproductor
		Mover = MoveFileEx(PathNuevo.c_str(), PathReproductor.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		// Se ha movido correctamente
		if (Mover != FALSE) {
			// Ejecuto el reproductor
			ShellExecute(NULL, L"open", PathEjecutable.c_str(), L"-ActualizacionTerminada", PathReproductor.c_str(), SW_SHOW);
			// Borro el directorio con la versión vieja
			BorrarDirectorio(PathViejo.c_str());
		}
		// No se ha movido
		else {
			std::wstring Err = App.UltimoError();
			// Restauro la carpeta vieja del reproductor
			Mover = MoveFileEx(PathViejo.c_str(), PathReproductor.c_str(), MOVEFILE_COPY_ALLOWED);
			// Si no se ha podido mover.. lo mando todo a la mierda...
			if (Mover == FALSE) {
				MessageBox(NULL, L"Error fatal! descarga RAVE desde su pagina, e instalalo de nuevo.", L"Error", MB_OK | MB_ICONERROR);
			}
			// Se ha restaurado la versión antigua correctamente
			else {
				Texto = L"No se puede mover '" + PathNuevo + L"' a '" + PathReproductor + L"'\n" + App.UltimoError();
				MessageBox(NULL, Texto.c_str(), L"Error", MB_OK | MB_ICONERROR);
			}
			// Ejecuto el reproductor informando del error
			ShellExecute(NULL, L"open", PathEjecutable.c_str(), L"-ActualizadorCorrupto", PathReproductor.c_str(), SW_SHOW);
		}
	}
	// Error moviendo la carpeta a Old a partir de aqui ya no se que pasa... se puede haber movido parte de la carpeta y puede que falten arhivos...
	else {		
		Texto = L"No se puede mover '" + PathReproductor + L"' a '" + PathViejo + L"'\n" + App.UltimoError();
		MessageBox(NULL, Texto.c_str(), L"Error", MB_OK | MB_ICONERROR);
		// Ejecuto el reproductor informando del error
		ShellExecute(NULL, L"open", PathEjecutable.c_str(), L"-ActualizadorCorrupto", PathReproductor.c_str(), SW_SHOW);
	}

	// Terminado
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

#define CHUNK 16384
//#define CHUNK 7680 // NO USAR

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

