#include "stdafx.h"
#include "BaseDatos.h"
#include "DStringUtils.h"
#include "Shlwapi.h"
#include <assert.h>
//#include "TablaCanciones.h"
#include "DDirectoriosWindows.h"

/*
  ____                 _____        _            
 |  _ \               |  __ \      | |           
 | |_) | __ _ ___  ___| |  | | __ _| |_ ___  ___ 
 |  _ < / _` / __|/ _ \ |  | |/ _` | __/ _ \/ __|
 | |_) | (_| \__ \  __/ |__| | (_| | || (_) \__ \
 |____/ \__,_|___/\___|_____/ \__,_|\__\___/|___/
                                                 
*/

BaseDatos::BaseDatos(void)  {
}


BaseDatos::~BaseDatos(void) {
}


const BOOL BaseDatos::IniciarBD(void) {
	// Creo ventana para los mensajes (para recibir datos desde el thread de buscar)
	CrearVentana(NULL, L"MensajesBDRave", L"MensajesBDRave", 0, 0, 10, 10, WS_POPUP);


	int Ret = 0;
	std::wstring PathBD;
	// Creo el directorio \AppData\Rave
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(PathBD);
	// Las bases de datos para 32 y 64 bits dan hash distintos y pueden duplicar medios, por ello cada configuración tiene su bd
	#ifdef _WIN64
		PathBD += L"\\Rave\\Rave_x64.BD";
	#else
		PathBD += L"\\Rave\\Rave_x86.BD";
	#endif
	// Creo / Abro la BD
	Ret = sqlite3_open16(PathBD.c_str(), &_BD);
	if (Ret) {
		sqlite3_close(_BD);
		return FALSE;
	}

	
	Debug_Escribir_Varg(L"BaseDatos::IniciarBD  BD = '%x'\n", _BD);

	// http://dcravey.wordpress.com/2011/03/21/using-sqlite-in-a-visual-c-application/
	CrearBD();

	Tabla_Opciones.ObtenerDatos(_BD);
	Tabla_Raiz.ObtenerDatos(_BD);
	

	
//	BOOL Ret2 = Tabla_Raiz.Argerar_Raiz(TEXT("D:\\MP3\\Iron Maiden"));
//	BOOL Ret2 = Tabla_Raiz.Argerar_Raiz(TEXT("G:\\MP3"));

	return TRUE;
//	sqlite3_close(_BD);
}

void BaseDatos::TerminarBD(void) {
	Debug_Escribir(L"BaseDatos::TerminarBD\n");
	sqlite3_close_v2(_BD);
}

const BOOL BaseDatos::CrearBD(void) {
	BOOL RetO = Tabla_Opciones.CrearTabla(_BD);
	BOOL RetR = Tabla_Raiz.CrearTabla(_BD);
	BOOL RetC = Tabla_Medios.CrearTabla(_BD);

	Debug_Escribir_Varg(L"BaseDatos::CrearBD  TablaRaiz='%d' TablaCanciones='%d'\n", RetR, RetC);

	if (RetO == FALSE || RetR == FALSE || RetC == FALSE) return FALSE;
	return TRUE;
}

void BaseDatos::ActualizarArbol(void) {
	App.VentanaRave.BarraTareas.Estado_Indeterminado();
//	if (_hWnd != NULL) return;
	App.VentanaRave.Arbol.BorrarTodo();
	App.Menu_ArbolBD[2]->Activado(FALSE);

	NodoBD *Tmp = NULL;

	for (size_t i = 0; i < Tabla_Raiz.Datos.size(); i++) {
		Tmp = Arbol_AgregarRaiz(&Tabla_Raiz.Datos[i]->Path);
		App.VentanaRave.Arbol.ExplorarPath(Tmp);
	}

	App.VentanaRave.Arbol.Repintar();

	// Inicio el thread de la busqueda
	_BuscarArchivos.Iniciar(hWnd());

}

const BOOL BaseDatos::SqlQuery(const wchar_t *Query, ...) {

	va_list			Marker;
	va_start(Marker, Query);
	static wchar_t VargQuery[4096];
	vswprintf_s(VargQuery, 4096, Query, Marker);
	va_end(Marker);

	//wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
//	const wchar_t  *SqlStr = L"SELECT * FROM Raiz";

	sqlite3_stmt   *SqlQuery = NULL;
	SqlRet = sqlite3_prepare16_v2(_BD, VargQuery, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		Debug_Escribir_Varg(L"BaseDatos::SqlQuery %s", _UltimoErrorSQL.c_str());
		return FALSE;
	}

	return TRUE;
}




// Gestor de mensajes para recibir datos desde el thread de buscar
LRESULT CALLBACK BaseDatos::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::wstring *TmpStr = NULL;
	switch (uMsg) {
		case WM_TBA_AGREGARRAIZ:
			TmpStr = reinterpret_cast<std::wstring *>(lParam);
			Arbol_AgregarRaiz(TmpStr);
			delete TmpStr; // Hay que borrar de memória el path (se crea en el thread BuscarArchivos y ya no es necesario)
			break;
		case WM_TBA_AGREGARDIR:
			TmpStr = reinterpret_cast<std::wstring *>(lParam);
			Arbol_AgregarDir(TmpStr, TRUE);
			delete TmpStr; // Hay que borrar de memória el path (se crea en el thread BuscarArchivos y ya no es necesario)
			break;
		case WM_TBA_AGREGARAUDIO:
			Arbol_AgregarCancion(static_cast<size_t>(lParam));
			break;
		case WM_TBA_TERMINADO :
			_BuscarArchivos._Thread = NULL;
			App.Menu_ArbolBD[2]->Activado(TRUE);
			App.VentanaRave.BarraTareas.Estado_SinProgreso();
			App.VentanaRave.BarraTareas.Resaltar();
			break;

	}
	return 0;
}

NodoBD *BaseDatos::Arbol_AgregarRaiz(std::wstring *Path) {
	// Busca si existe la raiz
	/*	BOOL Ret = TRUE;
	if (!App.VentanaRave.Arbol.BuscarTexto(*Path, NULL, FALSE))		App.VentanaRave.Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Raiz, NULL, Path->c_str());
	else	                                                   		Ret = FALSE;
	delete Path;
	return Ret;*/

	// Solo crea la raiz
	NodoBD *Tmp = App.VentanaRave.Arbol.BuscarHijoTxt(*Path);
	if (Tmp == NULL) {
		Tmp = App.VentanaRave.Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Raiz, NULL, Path->c_str());
		Tmp->Expandido = TRUE;
		App.VentanaRave.Arbol.Repintar();
	}
//	delete Path; // Hay que borrar de memória el path (se crea en el thread BuscarArchivos y ya no es necesario)
	return Tmp;
}

NodoBD *BaseDatos::Arbol_AgregarDir(std::wstring *Path, const BOOL nRepintar) {
	CeRaiz *Raiz = Tabla_Raiz.Buscar_Raiz(Path->c_str());
	if (Raiz == NULL) return NULL;
	ArbolBD &Arbol = App.VentanaRave.Arbol;
	// Busco la raíz
//	DWL::DSplit SplitRaiz(Raiz->Path, L'\\');
	NodoBD *TmpNodo = static_cast<NodoBD *>(Arbol.BuscarHijoTxt(Raiz->Path, NULL));
	// Busco el ultimo nodo padre
	std::wstring tmp = Path->substr(Raiz->Path.size(), Path->size() - Raiz->Path.size());
	DWL::DSplit Split(tmp, L'\\');
	
	for (size_t i = 0; i < static_cast<int>(Split.Total()) - 1; i++) {
		TmpNodo = static_cast<NodoBD *>(Arbol.BuscarHijoTxt(Split[i], TmpNodo));
	}

	std::wstring Filtrado;
	
	BaseDatos_Filtros::FiltroPath(Split[Split.Total() - 1], Filtrado);

	NodoBD *Ret = Arbol.BuscarHijoTxt(Filtrado, TmpNodo);
	// Agrego el directorio
	if (Ret == NULL) {
		Ret = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Directorio, TmpNodo, Filtrado.c_str());
	}
//	delete Path;
	if (nRepintar == TRUE)	Arbol.Repintar();

	return Ret;
}


NodoBD *BaseDatos::Arbol_AgregarCancion(const size_t Hash) {
	ArbolBD &Arbol = App.VentanaRave.Arbol;
	// Busco el medio en el arbol
	for (size_t i = 0; i < Arbol.TotalNodos(); i++) {
		if (Arbol.BDNodo(i)->Hash == Hash) 
			return NULL; // Ya exsite
	}

	// Obtengo la cancion de la bd

	TablaMedios_Medio Cancion(_BD, Hash);
	NodoBD *TmpNodo = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Directorio, NULL, Cancion.Path(), Hash);
//	TmpNodo->Hash = Hash;

	return TmpNodo;
}

const BOOL BaseDatos::ObtenerMedio(const sqlite3_int64 mHash, TablaMedios_Medio &mTMedio) {
	return mTMedio.Obtener(_BD, mHash);
}

const BOOL BaseDatos::ObtenerMedio(std::wstring &mPath, TablaMedios_Medio &mTMedio) {
//	CeRaiz         *mRaiz = App.BD.Tabla_Raiz.Buscar_Raiz(mPath.c_str());
//	sqlite3_int64   mHash = TablaMedios::CrearHash(mRaiz->ID_Disco, mPath);
	return mTMedio.Obtener(_BD, mPath);
}

















/*
  _______ _                        _ ____                                           _     _                
 |__   __| |                      | |  _ \                           /\            | |   (_)               
    | |  | |__  _ __ ___  __ _  __| | |_) |_   _ ___  ___ __ _ _ __ /  \   _ __ ___| |__  ___   _____  ___ 
    | |  | '_ \| '__/ _ \/ _` |/ _` |  _ <| | | / __|/ __/ _` | '__/ /\ \ | '__/ __| '_ \| \ \ / / _ \/ __|
    | |  | | | | | |  __/ (_| | (_| | |_) | |_| \__ \ (_| (_| | | / ____ \| | | (__| | | | |\ V / (_) \__ \
    |_|  |_| |_|_|  \___|\__,_|\__,_|____/ \__,_|___/\___\__,_|_|/_/    \_\_|  \___|_| |_|_| \_/ \___/|___/                                                                                                           
*/

sqlite3 *ThreadBuscarArchivos::_BD = NULL;


const BOOL ThreadBuscarArchivos::Iniciar(HWND nhWndDest) {
	if (_Thread != NULL) return FALSE;

	// Asigno Ventana para los mensajes
	hWndDest = nhWndDest;

	// Iniciamos el Thread
	_Thread = CreateThread(NULL, 0, (unsigned long(__stdcall *)(void *))this->_ThreadBusqueda, (void *)this, 0, NULL);
	_Cancelar = FALSE;
	if (_Thread) SetThreadPriority(_Thread, 0);
	else		 return FALSE;
	return TRUE;
}




unsigned long ThreadBuscarArchivos::_ThreadBusqueda(void *pThis) {
	
	DWORD Tick = GetTickCount();
	size_t                i = 0;
	ThreadBuscarArchivos *This = reinterpret_cast<ThreadBuscarArchivos *>(pThis);
	int Ret = 0;

//	This->_ListaHash.resize(0);

	std::wstring PathBD;
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(PathBD);
	PathBD += L"\\Rave\\";
	if (GetFileAttributes(PathBD.c_str()) == INVALID_FILE_ATTRIBUTES)
		CreateDirectory(PathBD.c_str(), NULL);

#ifdef _WIN64
	PathBD += L"\\Rave_x64.BD";
#else
	PathBD += L"\\Rave_x86.BD";
#endif


	Ret = sqlite3_open16(PathBD.c_str(), &_BD);
	if (Ret) {
		sqlite3_close(_BD);
		return 1;
	}
	TablaRaiz Tabla_Raiz;
	Tabla_Raiz.ObtenerDatos(_BD);

	Debug_Escribir_Varg(L"TBA::_ThreadBusqueda  TotalRaíz = '%d'\n", Tabla_Raiz.Datos.size());
	
	// Fase 1 : enumerar arxius i directoris
	UINT TotalArchivos = 0;
	char *SqlError; // Amb START TRANSACTION es thread safe, pero va lentisim si la BD esta buida...
	Ret = sqlite3_exec(_BD, "BEGIN TRANSACTION", NULL, NULL, &SqlError);
	if (Ret) 
		sqlite3_free(SqlError);
	
	for (i = 0; i < Tabla_Raiz.Datos.size(); i++) {
		TotalArchivos += This->_EscanearDirectorio(Tabla_Raiz.Datos[i]->Path.c_str(), Tabla_Raiz.Datos[i]);
	}

	Ret = sqlite3_exec(_BD, "COMMIT TRANSACTION", NULL, NULL, &SqlError);
	if (Ret) 
		sqlite3_free(SqlError);

	// Fase 2 : Estructurar arbol de la BD
/*	for (size_t i = 0; i < This->_ListaHash.size(); i++) {
		PostMessage(This->hWndDest, WM_TBA_AGREGARAUDIO, 0, static_cast<LPARAM>(This->_ListaHash[i]));
	}*/

	//	This->_Mutex = CreateMutex(NULL, FALSE, TEXT("Mutex_ThreadBuscarArchivos"));

	sqlite3_close(_BD);
	DWORD Tack = GetTickCount() - Tick;

	Debug_Escribir_Varg(L"TBA::_ThreadBusqueda ha escaneado %d archivos en %d milisegundos.\n", TotalArchivos, Tack);

	PostMessage(This->hWndDest, WM_TBA_TERMINADO, 0, static_cast<LPARAM>(TotalArchivos));
	_BD = NULL;
	return 0;
}


const UINT ThreadBuscarArchivos::_EscanearDirectorio(const wchar_t *nPath, CeRaiz *Raiz) {
	WIN32_FIND_DATA		FindInfoPoint;
	HANDLE				hFind = NULL;
	UINT        		TotalArchivosEscaneados = 0;
	static TCHAR		TmpPath[MAX_PATH + 1];


	//	if (nDirectorio == NULL) nDirectorio = Buscar_Raiz(nPath);

	//	DWL::DUnidadesDisco Unidades;
	// Compruebo que existe fisicamente en el disco
	if (GetFileAttributes(Raiz->Path.c_str()) == INVALID_FILE_ATTRIBUTES)
		return 0;
	//		GetShortPathName(nPath, TmpPath, MAX_PATH);
	//	if (TmpPath[0] == 0) return 0;

	std::wstring		Path;
	Path.reserve(MAX_PATH);
	Path = nPath;
//	DWL::DSplit Split(Path, TEXT('\\'));
	if (Path[Path.size() - 1] != TEXT('\\'))	Path += TEXT("\\*.*");
	else                                        Path += TEXT("*.*");

	std::wstring *StringDirectorio;

	if (_wcsicmp(Raiz->Path.c_str(), nPath) == 0) {
		StringDirectorio = new std::wstring(nPath);
		SendMessage(hWndDest, WM_TBA_AGREGARRAIZ, 0, reinterpret_cast<LPARAM>(StringDirectorio));
	}
	else {
		if ((DString_ContarCaracter(Raiz->Path, L'\\') + 1) == DString_ContarCaracter(Path, L'\\')) { // Si es un directorio hijo de la raiz (evito el resto de subdirectorios)
			StringDirectorio = new std::wstring(nPath);
			SendMessage(hWndDest, WM_TBA_AGREGARDIR, 0, reinterpret_cast<LPARAM>(StringDirectorio));
		}
	}
	
	// Hay que borrar el string de la memoria cuando se reciba el mensaje WM_TBA_AGREGARDIR en BaseDatos
	NodoBD *TmpNodo = NULL;
//	TMedio       *TmpMedio = NULL;
	size_t        Hash = 0;
	std::wstring  TmpTexto;

//	std::string   SqlStr;

	Debug_Escribir_Varg(L"TBA::_EscanDir '%s'\n", nPath);

	TmpTexto.reserve(128);
	hFind = FindFirstFile(Path.c_str(), &FindInfoPoint);
	while (FindNextFile(hFind, &FindInfoPoint) != 0) {
		Path = nPath;
		if (Path[Path.size() - 1] != TEXT('\\')) Path += TEXT('\\');

		if (_EsNombreValido(FindInfoPoint.cFileName)) {
			Path += FindInfoPoint.cFileName;
			// Si es un directorio y no empieza por ".", llamo a esta función recursiva de nuevo
			//			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && FindInfoPoint.cFileName[0] != TEXT('.')) {
			if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				TotalArchivosEscaneados += _EscanearDirectorio(Path.c_str(), Raiz);
			}
			else {
				TotalArchivosEscaneados++;
				Hash = _AnalizarMedio(Path.c_str(), Raiz, FindInfoPoint.nFileSizeLow);
/*				if (Hash) {
					_ListaHash.push_back(Hash);
				}*/


//					SendMessage(hWndDest, WM_TBA_AGREGARMEDIO, 0, reinterpret_cast<LPARAM>(TmpMedio));
/*					switch (TmpMedio->TipoMedio) {
					case Tipo_Medio_Audio:
						// AGREGARNODO
//						TmpTexto = std::to_wstring(static_cast<TMedioAudio *>(TmpMedio)->Pista) + TEXT(' ') + static_cast<TMedioAudio *>(TmpMedio)->Nombre;
//						TmpNodo = App.VentanaRave.Arbol.BuscarTexto(TmpTexto, Padre);
//						if (TmpNodo == NULL) App.VentanaRave.Arbol.AgregarNodo(Padre, TmpTexto.c_str(), 0);
						break;
					}
					*/

//				}
			}
		}
	}
	FindClose(hFind);

	return TotalArchivosEscaneados;
}

const BOOL ThreadBuscarArchivos::_EsNombreValido(const wchar_t *nNombre) {
	if (nNombre[0] == TEXT('.') && nNombre[1] == 0)								return FALSE;
	if (nNombre[0] == TEXT('.') && nNombre[1] == TEXT('.') && nNombre[2] == 0)	return FALSE;
	return TRUE;
}


/*
	NOTA :	Si el path existe en la BD retorna hash.
	Si el path no existe en la BD, lo añade a la BD y retorna el hash.
	Si el path es falso y no conduce a ningún archivo válido retorna NULL.
*/
const size_t ThreadBuscarArchivos::_AnalizarMedio(const wchar_t *nPath, CeRaiz *Raiz, DWORD Longitud) {
	//	TCHAR			TmpPath[MAX_PATH];
	std::wstring	Path = nPath;
	// Compruebo que existe fisicamente en el disco (comprobar velocidades usando CreateFile)
	if (GetFileAttributes(nPath) == INVALID_FILE_ATTRIBUTES)
		//	if (PathFileExists(nPath) == FALSE) 
		return NULL;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Obtenir temps d'un mp3 en milisegons																							//
	// http://stackoverflow.com/questions/3505575/how-can-i-get-the-duration-of-an-mp3-file-cbr-or-vbr-with-a-very-small-library	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Recorto el path y elimino los dos primeros caracteres que representan la unidad
	std::wstring    PathCortado = Path;
	PathCortado[0] = L'?';
	sqlite3_stmt   *SqlQuery = NULL;
	
	// https://www.mail-archive.com/sqlite-users@sqlite.org/msg63109.html
	// http://www.sqlite.org/c3ref/stmt.html
	// http://www.sqlite.org/c3ref/prepare.html
	// http://sqlite.1065341.n5.nabble.com/How-to-use-sqlite3-exec-to-execute-SQL-command-with-Unicode-text-td53339.html



	size_t			PosNombre = Path.find_last_of(TEXT("\\"));
	size_t			PosExtension = Path.find_last_of(TEXT("."));
	Extension_Medio	Extension = ExtensionesValidas::ObtenerExtension(Path.substr(PosExtension + 1, (Path.size() - PosExtension) - 1).c_str());
	Tipo_Medio		Tipo = ExtensionesValidas::OtenerTipoMedio(Extension);
//	TMedio         *Medio = NULL;
	UINT			Pista = 0;
	std::wstring	TmpNombre;
	std::wstring    NombreFinal;
	int				SqlRet = 0;
	std::wstring	SqlStr;
//	wchar_t        *SqlError = NULL;
	const char     *Error;
	sqlite3_int64   Hash = TablaMedios::CrearHash(Raiz->ID_Disco, Path);
//	std::hash<std::wstring> HashFunc;
//	size_t					Hash = HashFunc(DString_ToStr(Raiz->Id) + PathCortado);

	switch (Tipo) {
		case Tipo_Medio_INDEFINIDO:
			return NULL;
		case Tipo_Medio_Audio:
		case Tipo_Medio_Video:
			_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, Pista);
			BaseDatos_Filtros::FiltroNombre(TmpNombre, NombreFinal);
			SqlStr = L"INSERT INTO Medios (Hash, Path, Nombre, TipoMedio, Extension, Pista, Raiz, Longitud, Nota, Tiempo, Subtitulos) VALUES(" + DString_ToStr(Hash)		+ L", \"" +
																																				 PathCortado				+ L"\", \"" +
																																				 NombreFinal				+ L"\", " +
																																				 DString_ToStr(Tipo)		+ L", " +
																																				 DString_ToStr(Extension)	+ L", " +
																																				 DString_ToStr(Pista)		+ L", " +
																																				 DString_ToStr(Raiz->Id)	+ L", " +
																																				 DString_ToStr(Longitud)	+ L", " +
																																				 /* Nota */					+L"2," +
																																				 /* Tiempo*/				+L"0," +
																																											+L"\"\")";

			SqlRet = sqlite3_prepare16_v2(_BD, SqlStr.c_str(), -1, &SqlQuery, NULL);
			if (SqlRet) {
				Error = sqlite3_errmsg(_BD);
				return FALSE;
			}

			while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
				SqlRet = sqlite3_step(SqlQuery);
			}
			sqlite3_finalize(SqlQuery);
			if (SqlRet == SQLITE_DONE) 
				return static_cast<size_t>(Hash);
			else if (SqlRet == SQLITE_CONSTRAINT)
				return static_cast<size_t>(Hash); // Ya existe el hash
			else  /* Error ? */
				Error = sqlite3_errmsg(_BD);
		
			break;
//			_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, static_cast<TMedioVideo *>(Medio)->Pista);
			break;
		case Tipo_Medio_CDAudio:
	//		_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, static_cast<TMedioCDAudio *>(Medio)->Pista);
			break;
		case Tipo_Medio_Lista:
	//		_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, TmpPista);
			break;
	}
/*	Medio->Extension = Extension;
//	Medio->Raiz = nRaiz; // LA RAIZ SE TIENE QUE ASIGNAR UNA VEZ MANDADO EL MENSAJE CON LOS DATOS AL THREAD PRINCIPAL
	Medio->Path = Path;
	Medio->TipoMedio = Tipo;
	Medio->Disponible = true;
	Medios.push_back(Medio);

	return Medio;*/
	return  NULL;
}


/*
	IN  -> Analisis
	OUT -> nNombre
	OUT -> nPista
	Nombres de pista validos : 1, 01, 1x01, 1-12, 20x21
	Nunca se hace caso a 4 digitos seguidos o mas (ya que son o el año o otra cosa.. pero la pista seguro que no)
	Se analiza de izquierda a derecha (lo mas normal es que la pista salga a la izquieda nada mas empezar)
*/
const BOOL ThreadBuscarArchivos::_AnalizarNombre(std::wstring &Analisis, std::wstring &nNombre, UINT &nPista) {
	bool PistaEncontrada = false;
	BOOL C1, C2, C3, C4, C5;
	for (size_t i = 0; i < Analisis.size(); i++) {
		if (_EsNumero(Analisis[i]) == TRUE) {
			C1 = FALSE;	C2 = FALSE;	C3 = FALSE;	C4 = FALSE;	C5 = FALSE;
			if (Analisis.size() > i + 1) C1 = _EsNumero(Analisis[i + 1]);
			if (Analisis.size() > i + 2) C2 = _EsNumero(Analisis[i + 2]);
			if (Analisis.size() > i + 3) C3 = _EsNumero(Analisis[i + 3]);
			if (Analisis.size() > i + 4) C4 = _EsNumero(Analisis[i + 4]);
			if (Analisis.size() > i + 5) C5 = _EsNumero(Analisis[i + 5]);
			// 01?01? (si el separador es un espacio no se tomara como pista doble)
			if (C1 == TRUE && C2 == FALSE && Analisis[i + 2] != TEXT(' ') && C3 == TRUE && C4 == TRUE && C5 == FALSE) { // Pista doble, se multiplicara por 100 la primera parte de la pista
				nPista = (_wtoi(Analisis.substr(i, 2).c_str()) * 100) + _wtoi(Analisis.substr(i + 3, 2).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 5, Analisis.size() - (i + 5));
				return TRUE;
			}
			// 1?01? (si el separador es un espacio no se tomara como pista doble)
			if (C1 == FALSE && Analisis[i + 1] != TEXT(' ') && C2 == TRUE && C3 == TRUE && C4 == FALSE) { // Pista doble, se multiplicara por 100 la primera parte de la pista
				nPista = (_wtoi(Analisis.substr(i, 1).c_str()) * 100) + _wtoi(Analisis.substr(i + 2, 2).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 4, Analisis.size() - (i + 4));
				return TRUE;
			}
			// 001?
			if (C1 == TRUE && C2 == TRUE && C3 == FALSE) { // Pista unica tres digitos
				nPista = _wtoi(Analisis.substr(i, 3).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 3, Analisis.size() - (i + 3));
				return TRUE;
			}
			// 01?
			if (C1 == TRUE && C2 == FALSE) { // Pista unica dos digitos
				nPista = _wtoi(Analisis.substr(i, 2).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 2, Analisis.size() - (i + 2));
				return TRUE;
			}
			// 1?
			if (C1 == FALSE) { // Pista unica un digito
				nPista = _wtoi(Analisis.substr(i, 1).c_str());
				if (i != 0) nNombre = Analisis.substr(0, i);
				nNombre += Analisis.substr(i + 1, Analisis.size() - (i + 1));
				return TRUE;
			}
		}
	}

	nPista = 0;
	nNombre = Analisis;
	return FALSE;
}


const BOOL ThreadBuscarArchivos::_EsNumero(const wchar_t Caracter) {
	if (Caracter >= TEXT('0') && Caracter <= TEXT('9'))	return TRUE;
	return FALSE;
}















/*   ______ _ _ _                       _        _            _
	|  ____(_) | |                     | |      | |          | |
	| |__   _| | |_ _ __ ___  ___    __| | ___  | |_ _____  _| |_ ___
	|  __| | | | __| '__/ _ \/ __|  / _` |/ _ \ | __/ _ \ \/ / __/ _ \
	| |    | | | |_| | | (_) \__ \ | (_| |  __/ | ||  __/>  <| || (_) |
	|_|    |_|_|\__|_|  \___/|___/  \__,_|\___|  \__\___/_/\_\\__\___/
*/

/* Para filtrar caracteres molestos en nombres de genero grupo disco y canciones */
void BaseDatos_Filtros::FiltroNombre(std::wstring &In, std::wstring &Out) {
	Out.resize(0);
	Out.reserve(In.size());
	bool Espacio = false;
	for (size_t i = 0; i < In.size(); i++) {
		switch (In[i]) {
			case 65279: // caracter invisible inutil del unicode
				break;
			case TEXT(':'): case TEXT('_'): case TEXT(','): case TEXT('.'): case 96  /*'*/: case TEXT('('): case TEXT(')'):
			case TEXT('!'): case TEXT('?'): case TEXT('¿'): case TEXT('^'): case TEXT('"'): case TEXT('#'): case TEXT('$'):
			case TEXT('%'): case TEXT('/'): case TEXT('\\'): case TEXT('-'): case TEXT('['): case TEXT(']'): case TEXT('~'):
			case ' ':
				Espacio = false;
				if (Out.size() != 0) {
					if (In[Out.size() - 1] != TEXT(' '))
						Espacio = true;
				}
				else
					Espacio = true;


				// Si el caracter anterior no es un espacio y no es el primer caracter, pongo un espacio. 
				if (Espacio == true && Out.size() != 0) Out += TEXT(' ');
				break;
			default:
				Out += static_cast<TCHAR>(tolower(In[i]));
				break;
		}
	}
	// Primer caracter en mayusculas
	Out[0] = toupper(Out[0]);
}

void BaseDatos_Filtros::FiltroPath(std::wstring &In, std::wstring &Out) {
	Out.reserve(In.size());
	for (size_t i = 0; i < In.size(); i++) {
		if (i == 0) Out = static_cast<wchar_t>(toupper(In[0]));
		else        Out += static_cast<wchar_t>(tolower(In[i]));
	}

}