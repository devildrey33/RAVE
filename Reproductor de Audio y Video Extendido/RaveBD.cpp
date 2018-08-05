#include "stdafx.h"
#include "RaveBD.h"
#include "DDirectoriosWindows.h"
#include "DStringUtils.h"

RaveBD::RaveBD(void) : _BD(NULL)  {
}


RaveBD::~RaveBD(void) {
}

// Crea / carga la base de datos
const BOOL RaveBD::Iniciar(void) {
	int Ret = 0;
	std::wstring PathBD;
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(PathBD);
	PathBD += L"\\Rave\\";
	// Creo el directorio "\AppData\Rave" si no existe
	if (GetFileAttributes(PathBD.c_str()) == INVALID_FILE_ATTRIBUTES)
		CreateDirectory(PathBD.c_str(), NULL);

	// Las bases de datos para 32 y 64 bits dan hash distintos y pueden duplicar medios, por ello cada configuración tiene su bd
#ifdef _WIN64
	PathBD += L"Rave_x64.BD";
#else
	PathBD += L"Rave_x86.BD";
#endif
	// Creo / Abro la BD
	Ret = sqlite3_open16(PathBD.c_str(), &_BD);
	if (Ret) {
		sqlite3_close(_BD);
		_BD = NULL;
		return FALSE;
	}
	// Creo las tablas para la base de datos (si es necesario)
	_CrearTablas();

	// Obtengo las raices de la base de datos
	ObtenerRaices();
	// Obtengo las opciones de la base de datos
	ObtenerOpciones();

	return TRUE;
}

// Cierra la Base de datos
void RaveBD::Terminar(void) {
	sqlite3_close_v2(_BD);
	_BD = NULL;
}


const int RaveBD::ConsultaVarg(const wchar_t *TxtConsulta, ...) {
	static wchar_t  Texto[4096];
	va_list			Marker;
	va_start(Marker, TxtConsulta);
	vswprintf_s(Texto, 4096, TxtConsulta, Marker);
	va_end(Marker);
	return Consulta(Texto);
}

// Función para realizar consultas simples 
const int RaveBD::Consulta(const wchar_t *TxtConsulta) {
	if (_BD == NULL) return FALSE;

	sqlite3_stmt *SqlQuery = NULL;
	int           SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta, -1, &SqlQuery, NULL);

	/*	SQLITE_FULL		: database or disk full
		SQLITE_IOERR	: disk I / O error
		SQLITE_BUSY		: database in use by another process
		SQLITE_NOMEM	:*/
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_BUSY && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
	}
	sqlite3_finalize(SqlQuery);
	// Si hay un error lo apunto
	if (SqlRet == SQLITE_ERROR || SqlRet == SQLITE_CONSTRAINT) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
	}

	return SqlRet;
}

const BOOL RaveBD::ObtenerMedio(const sqlite3_int64 mHash, BDMedio &OUT_Medio) {
	std::wstring    SqlStr = L"SELECT * FROM Medios WHERE Hash =" + DWL::DString_ToStr(mHash);
	return _ConsultaObtenerMedio(SqlStr, OUT_Medio);
}

const BOOL RaveBD::ObtenerMedio(std::wstring &mPath, BDMedio &OUT_Medio) {
	std::wstring TmpPath = mPath;
	TmpPath[0] = L'?'; // Los paths de la BD emnpiezan por ?
	std::wstring SqlStr = L"SELECT * FROM Medios WHERE Path =\"" + TmpPath + L"\" COLLATE NOCASE"; // COLLATE NOCASE = Comparar strings case insensitive
	return _ConsultaObtenerMedio(SqlStr, OUT_Medio);
}

// Función que busca una raíz por el path
// Se ha adaptado para que encuentre raices parciales, es decir :
// - Si tenemos en la BD la raíz "c:\mp3\grupo" y buscamos "c:\mp3" devolvera la raíz "c:\mp3\grupo"
// - Si tenemos en la BD la raíz "c:\mp3" y buscamos "c:\mp3\grupo" devolverá la raíz "c:\mp3"
BDRaiz *RaveBD::BuscarRaiz(std::wstring &Path) {
	int Comp = 0;
	for (size_t i = 0; i < _Raices.size(); i++) {
		if (_CompararRaices(_Raices[i]->Path, Path) == TRUE) {
			return _Raices[i];
		}
/*		// El Path a buscar es de mayor tamaño
		if (_Raices[i]->Path.size() < Path.size())		Comp = _wcsicmp(_Raices[i]->Path.c_str(), Path.substr(0, _Raices[i]->Path.size()).c_str());
		// El Path a buscar es de menor tamaño
		else if (_Raices[i]->Path.size() > Path.size())	Comp = _wcsicmp(_Raices[i]->Path.substr(0, Path.size()).c_str(), Path.c_str());
		// Tienen el mismo tamaño
		else											Comp = _wcsicmp(_Raices[i]->Path.c_str(), Path.c_str());
		// Si son iguales retorno los datos
		if (Comp == 0) return _Raices[i];*/
	}
	return NULL;
}


// Función que compara 2 paths pertenecientes a posibles raices
// Se ha adaptado para que encuentre raices parciales, es decir :
// - Si tenemos en la BD la raíz "c:\mp3\grupo" y buscamos "c:\mp3" devolvera la raíz "c:\mp3\grupo"
// - Si tenemos en la BD la raíz "c:\mp3" y buscamos "c:\mp3\grupo" devolverá la raíz "c:\mp3"
const BOOL RaveBD::_CompararRaices(std::wstring &Path1, std::wstring &Path2) {
	int Comp = 0;
	// El Path a buscar es de mayor tamaño
	if (Path1.size() < Path2.size())		Comp = _wcsicmp(Path1.c_str(), Path2.substr(0, Path1.size()).c_str());
	// El Path a buscar es de menor tamaño
	else if (Path1.size() > Path2.size())	Comp = _wcsicmp(Path1.substr(0, Path2.size()).c_str(), Path2.c_str());
	// Tienen el mismo tamaño
	else									Comp = _wcsicmp(Path1.c_str(), Path2.c_str());
	// Devuelvo TRUE si son similares o iguales
	return (Comp == 0) ? TRUE : FALSE;
}

// Función que busca una raíz por su ID
BDRaiz *RaveBD::BuscarRaiz(const unsigned long bID) {
	for (size_t i = 0; i < _Raices.size(); i++) {
		if (_Raices[i]->Id == bID) return _Raices[i];
	}
	return NULL;
}



const BOOL RaveBD::_ConsultaObtenerMedio(std::wstring &TxtConsulta, BDMedio &OUT_Medio) {
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;
	SqlRet = sqlite3_prepare16_v2(_BD, TxtConsulta.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		//Debug_Escribir_Varg(L"TablaMedios_Medio::_Consulta terminada con error\n");
		return FALSE;
	}

	std::wstring TmpPath;

	//BDRaiz *TmpRaiz = NULL;
	DWL::DUnidadDisco *Unidad = NULL;


	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			OUT_Medio.ObtenerFila(SqlQuery, Unidades);

/*			Unidad = _Unidades.Buscar_Numero_Serie(OUT_Medio.IDDisco);
			if (Unidad != NULL) OUT_Medio.Path[0] = Unidad->Letra();*/
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t *>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return TRUE;
}

// Elimina las raices de la memória
void RaveBD::_BorrarRaices(void) {
	for (size_t i = 0; i < _Raices.size(); i++) delete _Raices[i];
	_Raices.resize(0);
}

// Función que obtiene todas las raices de la base de datos, y las carga en memória
const BOOL RaveBD::ObtenerRaices(void) {
	_BorrarRaices();
	wchar_t		       *SqlError = NULL;
	int				    SqlRet = 0;
	const wchar_t      *SqlStr = L"SELECT * FROM Raiz";
	DWL::DUnidadesDisco nUnidades;
	DWL::DUnidadDisco  *Unidad   = NULL;
	sqlite3_stmt       *SqlQuery = NULL;
	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDRaiz *TmpRaiz		= new BDRaiz;
			TmpRaiz->Id			= static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 0));
			TmpRaiz->Path		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 1));
			TmpRaiz->ID_Disco	= static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 2));
			// Asigno la letra de unidad a la raíz ya que las unidades extraibles pueden cambiar de letra pero siempre tienen el mismo número de serie.
			Unidad = nUnidades.Buscar_Numero_Serie(TmpRaiz->ID_Disco);
			if (Unidad != NULL) {
				TmpRaiz->Path[0] = Unidad->Letra();
				TmpRaiz->Letra = Unidad->Letra();
			}
			_Raices.push_back(TmpRaiz);
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}

	return TRUE;
}


const BOOL RaveBD::_CrearTablas(void) {

	// Creo la tabla para las opciones
	std::wstring CrearTablaOpciones = L"CREATE TABLE Opciones (" L"Id" 				    L" INTEGER PRIMARY KEY, " \
																 L"Volumen"			    L" INTEGER, "             \
																 L"PathAbrir"		    L" VARCHAR(260), "	 	  \
																 L"PosX"				L" INTEGER, "             \
																 L"PosY"				L" INTEGER, "             \
																 L"Ancho"			    L" INTEGER, "             \
																 L"Alto"				L" INTEGER, "             \
																 L"Shufle"			    L" INTEGER, "             \
																 L"Repeat"			    L" INTEGER, "             \
																 L"Inicio"			    L" INTEGER, "             \
																 L"OcultarMouseEnVideo" L" INTEGER)";
	if (Consulta(CrearTablaOpciones.c_str()) == FALSE) return FALSE;

	// Añado los datos por defecto de las opciones
	std::wstring ValoresTablaOpciones = L"INSERT INTO Opciones (ID, Volumen, PathAbrir, PosX, PosY, Ancho, Alto, Shufle, Repeat, Inicio, OcultarMouseEnVideo) VALUES(0, 100, \"C:\\\", 100, 100, 660, 400, 0, 0, 0, 3000)";
	if (Consulta(ValoresTablaOpciones.c_str()) == FALSE) return FALSE;

	// Creo la tabla para las raices
	std::wstring CrearTablaRaiz = L"CREATE TABLE Raiz (Id INTEGER PRIMARY KEY, Path VARCHAR(260), IdDisco INTEGER)";
	if (Consulta(CrearTablaRaiz.c_str()) == FALSE) return FALSE;

	// Creo la tabla para los medios
	std::wstring CrearTablaMedios = L"CREATE TABLE Medios (Id INTEGER PRIMARY KEY, "	\
 														 L"Hash BIGINT UNIQUE,"			\
														 L"Path VARCHAR(260), "			\
														 L"Nombre VARCHAR(128), "		\
														 L"TipoMedio INT, "				\
														 L"Extension INT, "				\
														 L"Reproducido INT, "			\
														 L"Longitud INT, "				\
														 L"IDDisco INT, "				\
														 L"Nota SMALLINT, "				\
														 L"Genero INT, " 				\
														 L"Grupo INT, "	    			\
													 	 L"Disco INT, "  				\
														 L"Pista INT, "					\
												 		 L"Tiempo INT, "				\
														 L"Subtitulos VARCHAR(260))";
	if (Consulta(CrearTablaMedios.c_str()) == FALSE) return FALSE;

	return TRUE;
}



/*	NULL hay un error */

/*	Se puede dar el caso en que dada una raíz existente se quiera agregar otra raíz que es pariente de esta.
	En este no se añadirá ninguna raíz y la raíz existente pasara a tener el path que ocupe menos caracteres. */
BDRaiz *RaveBD::AgregarRaiz(std::wstring &nPath) {
	BOOL			Ret       = 0;
	UINT			Total     = 0;
	int				SqlRet    = 0;
	size_t          i         = 0;
	sqlite3_stmt   *SqlQuery  = NULL;
	std::wstring	PathFinal = nPath;
	if (PathFinal[PathFinal.size() - 1] != TEXT('\\')) PathFinal += TEXT('\\');

	/*


	std::wstring    RaizFinal = PathFinal;

	// Busco raices similares
	std::vector<BDRaiz *> RaicesSimilares;
	UINT PathMasCorto = RaizFinal.size();
//	bool PMCO = true; // Path Mas Corto Original
	for (i = 0; i < _Raices.size(); i++) {
		if (_CompararRaices(_Raices[i]->Path, PathFinal) == TRUE) {
			// Si no son la misma raíz
			if (_Raices[i]->Path.size() != PathFinal.size()) {
				RaicesSimilares.push_back(_Raices[i]);
				// Busco el path mas corto
				if (PathMasCorto > _Raices[i]->Path.size()) {
					PathMasCorto = _Raices[i]->Path.size();
					RaizFinal = _Raices[i]->Path;
				}
			}
		}
	}
	// Hay una o mas raices similares
	if (RaicesSimilares.size() > 0) {
		// Hay mas de una raíz similar
		if (RaicesSimilares.size() > 1) {

		}
		// Sustituyo el path de la raíz por el que sea mas corto
		else {
			std::wstring SqlUpdateStr = L"UPDATE Raiz SET Path=\"" + RaizFinal + L"\"WHERE Id=" + DWL::DString_ToStr(RaicesSimilares[0]->Id);
			Consulta(SqlUpdateStr);
		}

	}
	*/


	

	BDRaiz *BR = BuscarRaiz(PathFinal);
	// Si existe una raiz similar o igual
	if (BR != NULL) {
		// La nueva raíz es el padre/abuelo/etc.. de una o mas raices definidas anteriormente (TODO !! solo arreglo una raiz)
		if (BR->Path.size() > PathFinal.size()) {
			std::wstring SqlUpdateStr = L"UPDATE Raiz SET Path=\"" + PathFinal + L"\"WHERE Id=" + DWL::DString_ToStr(BR->Id);
			SqlRet = sqlite3_prepare16_v2(_BD, SqlUpdateStr.c_str(), -1, &SqlQuery, NULL);
			Consulta(SqlUpdateStr);
			ObtenerRaices();
		}
		return BR;
	}
	
	
	// Si no existe una raíz similar
	DWL::DUnidadDisco  *Unidad = Unidades.Buscar_Letra(PathFinal[0]);
	if (Unidad == NULL)
		return NULL;

	std::wstring SqlStr = L"INSERT INTO Raiz (Path, IDDisco) VALUES(\"" + PathFinal + L"\", " + DWL::DString_ToStr(Unidad->Numero_Serie()) + L")";
	Consulta(SqlStr);
	
	ObtenerRaices();

	return BuscarRaiz(PathFinal);
}


const BOOL RaveBD::EliminarRaiz(std::wstring &nPath) {
	BOOL Ret = ConsultaVarg(L"DELETE FROM Raiz WHERE Path ='%s'", nPath.c_str());

	ObtenerRaices();
	return TRUE;
}

// Analiza e inserta el medio en la BD
const BOOL RaveBD::AnalizarMedio(std::wstring &nPath, BDMedio &OUT_Medio, const ULONG Longitud) {
	// Compruebo que existe fisicamente en el disco (comprobar velocidades usando CreateFile)
	if (GetFileAttributes(nPath.c_str()) == INVALID_FILE_ATTRIBUTES)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Obtenir temps d'un mp3 en milisegons																							//
	// http://stackoverflow.com/questions/3505575/how-can-i-get-the-duration-of-an-mp3-file-cbr-or-vbr-with-a-very-small-library	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Recorto el path y elimino los dos primeros caracteres que representan la unidad
	std::wstring		PathCortado = nPath;
	PathCortado[0] = L'?';
	DWL::DUnidadDisco  *UnidadDisco = Unidades.Buscar_Letra(nPath[0]);


	size_t				PosNombre		= nPath.find_last_of(TEXT("\\"));																					// Posición donde empieza el nombre
	size_t				PosExtension	= nPath.find_last_of(TEXT("."));																					// Posición donde empieza la extensión
	Extension_Medio		Extension		= ExtensionesValidas::ObtenerExtension(nPath.substr(PosExtension + 1, (nPath.size() - PosExtension) - 1).c_str());	// Obtengo el tipo de extensión
	Tipo_Medio			Tipo			= ExtensionesValidas::ObtenerTipoMedio(Extension);																	// Obtengo el tipo de medio
	sqlite3_int64		Hash			= CrearHash(UnidadDisco->Numero_Serie(), PathCortado);																// Creo un hash partiendo del path y el número de serie del disco
	UINT				Pista			= 0;																												// Número de pista
	std::wstring		TmpNombre;
	std::wstring		NombreFinal;
	std::wstring        SqlStr;
	int                 SqlRet = 0;
//	ULONG               Longitud = 0;


	switch (Tipo) {
		case Tipo_Medio_INDEFINIDO:
			return FALSE;
		case Tipo_Medio_Audio:
		case Tipo_Medio_Video:
			_AnalizarNombre(nPath.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, Pista);
			FiltroNombre(TmpNombre, NombreFinal);
			OUT_Medio.Hash			= Hash;
			OUT_Medio.Path			= PathCortado;
			OUT_Medio.Nombre		= NombreFinal;
			OUT_Medio.TipoMedio		= Tipo;
			OUT_Medio.Extension		= Extension;
			OUT_Medio.Pista			= Pista;
			OUT_Medio.IDDisco		= UnidadDisco->Numero_Serie();
			OUT_Medio.Longitud      = Longitud;
			OUT_Medio.Nota          = 2;
			OUT_Medio.Tiempo        = 0;
			SqlStr = L"INSERT INTO Medios (Hash, Path, Nombre, TipoMedio, Extension, Pista, IDDisco, Longitud, Nota, Tiempo, Subtitulos) VALUES(" + DString_ToStr(Hash)							+ L", \"" +
																																					PathCortado									+ L"\", \"" +
																																					NombreFinal									+ L"\", " +
																																					DString_ToStr(Tipo)							+ L", " +
																																					DString_ToStr(Extension)					+ L", " +
																																					DString_ToStr(Pista)						+ L", " +
																																					DString_ToStr(UnidadDisco->Numero_Serie())	+ L", " + // IDDisco
//																																					DString_ToStr(nRaiz->Id)					+ L", " +
																																					DString_ToStr(Longitud)						+ L", " +
																													/* Nota */						+L"2," +
																													/* Tiempo*/						+L"0," +
																																				 +	L"\"\")";
			SqlRet = Consulta(SqlStr);
			if (SqlRet == SQLITE_DONE)
				return TRUE; // No existe el hash
			else if (SqlRet == SQLITE_CONSTRAINT)
				return TRUE; // Ya existe el hash
			else  /* Error ? */
				return FALSE;

			break;
		//			_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, static_cast<TMedioVideo *>(Medio)->Pista);
//		break;
		case Tipo_Medio_CDAudio:
		//		_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, static_cast<TMedioCDAudio *>(Medio)->Pista);
			break;
		case Tipo_Medio_Lista:
		//		_AnalizarNombre(Path.substr(PosNombre + 1, (PosExtension - PosNombre) - 1), TmpNombre, TmpPista);
			break;
	}
	return FALSE;
}


/* Función estatica para crear un hash a partir de un numero de serie del disco y el path de la canción */
/* NOTA se eliminan los dos primeros carácteres del path que son los de la unidad, ya que los medios extraibles pueden cambiar de letra. */
const sqlite3_int64 RaveBD::CrearHash(DWORD NumeroSerieDisco, std::wstring &nPath) {
	//	std::wstring PathCortado = Path.substr(2, Path.size() - 2);
	std::hash<std::wstring> HashFunc;
	return HashFunc(DWL::DString_ToStr(NumeroSerieDisco) + nPath.substr(2, nPath.size() - 2));
}


/*
	IN  -> Analisis
	OUT -> nNombre
	OUT -> nPista
	Nombres de pista validos : 1, 01, 1x01, 1-12, 20x21
	Nunca se hace caso a 4 digitos seguidos o mas (ya que son o el año o otra cosa.. pero la pista seguro que no)
	Se analiza de izquierda a derecha (lo mas normal es que la pista salga a la izquieda nada mas empezar)
*/
const BOOL RaveBD::_AnalizarNombre(std::wstring &Analisis, std::wstring &nNombre, UINT &nPista) {
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


const BOOL RaveBD::_EsNumero(const wchar_t Caracter) {
	if (Caracter >= TEXT('0') && Caracter <= TEXT('9'))	return TRUE;
	return FALSE;
}


/* Para filtrar caracteres molestos en nombres de genero grupo disco y canciones */
void RaveBD::FiltroNombre(std::wstring &In, std::wstring &Out) {
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

void RaveBD::FiltroPath(std::wstring &In, std::wstring &Out) {
	Out.reserve(In.size());
	for (size_t i = 0; i < In.size(); i++) {
		if (i == 0) Out = static_cast<wchar_t>(toupper(In[0]));
		else        Out += static_cast<wchar_t>(tolower(In[i]));
	}

}





void RaveBD::Opciones_Volumen(const int nVolumen) {
	_Opciones_Volumen = nVolumen;
	std::wstring Q = L"Update Opciones SET Volumen=" + DWL::DString_ToStr(nVolumen) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveBD::Opciones_Shufle(const int nShufle) {
	_Opciones_Shufle = nShufle;
	std::wstring Q = L"Update Opciones SET Shufle=" + DWL::DString_ToStr(nShufle) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveBD::Opciones_Repeat(const int nRepeat) {
	_Opciones_Repeat = nRepeat;
	std::wstring Q = L"Update Opciones SET Repeat=" + DWL::DString_ToStr(nRepeat) + L" WHERE Id=0";
	Consulta(Q.c_str());

}

void RaveBD::Opciones_Inicio(const int nInicio) {
	_Opciones_Inicio = nInicio;
	std::wstring Q = L"Update Opciones SET Inicio=" + DWL::DString_ToStr(nInicio) + L" WHERE Id=0";
	Consulta(Q.c_str());
}


void RaveBD::Opciones_OcultarMouseEnVideo(const int nOcultarMouseEnVideo) {
	_Opciones_OcultarMouseEnVideo = nOcultarMouseEnVideo;
	std::wstring Q = L"Update Opciones SET OcultarMouseEnVideo=" + DWL::DString_ToStr(nOcultarMouseEnVideo) + L" WHERE Id=0";
	Consulta(Q.c_str());
}


const BOOL RaveBD::ObtenerOpciones(void) {

	const wchar_t  *SqlStr = L"SELECT * FROM Opciones";
	wchar_t		   *SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt   *SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			_Opciones_Volumen				= static_cast<int>(sqlite3_column_int(SqlQuery, 1));
			_Opciones_PathAbrir				= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
			_Opciones_PosX					= static_cast<int>(sqlite3_column_int(SqlQuery, 3));
			_Opciones_PosY					= static_cast<int>(sqlite3_column_int(SqlQuery, 4));
			_Opciones_Ancho					= static_cast<int>(sqlite3_column_int(SqlQuery, 5));
			_Opciones_Alto					= static_cast<int>(sqlite3_column_int(SqlQuery, 6));
			_Opciones_Shufle				= static_cast<int>(sqlite3_column_int(SqlQuery, 7));
			_Opciones_Repeat				= static_cast<int>(sqlite3_column_int(SqlQuery, 8));
			_Opciones_Inicio				= static_cast<int>(sqlite3_column_int(SqlQuery, 9));
			_Opciones_OcultarMouseEnVideo	= static_cast<int>(sqlite3_column_int(SqlQuery, 10));
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}

	return TRUE;
}

const BOOL RaveBD::AsignarTiempoMedio(const libvlc_time_t nTiempo, const sqlite3_int64 mHash) {
	int SqlRet = ConsultaVarg(L"UPDATE Medios SET Tiempo=%d WHERE Hash=%d", nTiempo, mHash);
	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}
	return TRUE;
}




/* NOTA es mejor tener 2 selects para las opciones, uno para el tamaño y posición de la ventana, y otro para el resto de valores (shufle, repeat, volumen, etc...)
Y no viene de 15 milisegundos mas a la hora de cerrar el reproductor */
const BOOL RaveBD::Opciones_GuardarOpciones(void) {
	Opciones_GuardarPosTamVentana();
	std::wstring Q = L"UPDATE Opciones SET"
		L" Volumen=" + std::to_wstring(_Opciones_Volumen) +
		//						L", Volumen=" + std::to_wstring(_Volumen) +
		L", Shufle=" + std::to_wstring(_Opciones_Shufle) +
		L", Repeat=" + std::to_wstring(_Opciones_Repeat) +
		L" WHERE Id=0";
	int SqlRet = Consulta(Q);

	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveBD::Opciones_GuardarPosTamVentana(void) {
	if (App.VentanaRave.Maximizada() == FALSE) {
		RECT RC;
		GetWindowRect(App.VentanaRave.hWnd(), &RC);
		_Opciones_PosX = RC.left;
		_Opciones_PosY = RC.top;
		_Opciones_Ancho = abs(RC.right - RC.left);
		_Opciones_Alto = abs(RC.bottom - RC.top);

		std::wstring Q = L"UPDATE Opciones SET PosX=" + DWL::DString_ToStr(RC.left) + L", PosY=" + DWL::DString_ToStr(RC.top) + L", Ancho=" + DWL::DString_ToStr(_Opciones_Ancho) + L", Alto=" + DWL::DString_ToStr(_Opciones_Alto) + L" WHERE Id=0";
		int SqlRet = Consulta(Q);
		if (SqlRet == SQLITE_ERROR) {
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}






















// Constructor que obtiene los datos de una fila
BDMedio::BDMedio(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades) {
	ObtenerFila(SqlQuery, Unidades);
}

/* Función que obtiene los datos de una fila

	Nombre		Posición	   Tipo
	--------------------------------
	Id				0		INTEGER PRIMARY KEY
	Hash			1		INT UNIQUE
	Path			2		VARCHAR(260)
	Nombre			3		VARCHAR(128)
	TipoMedio		4		INT
	Extension		5		INT
	Reproducido		6		INT
	Longitud		7		INT
	Raiz			8		INT
	Nota			9		SMALLINT
	Genero		   10		INT
	Grupo		   11		INT
	Disco		   12		INT
	Pista		   13		INT
	Tiempo		   14		INT
	Subtitulos	   15		VARCHAR(260)
	*/
void BDMedio::ObtenerFila(sqlite3_stmt *SqlQuery, DWL::DUnidadesDisco &Unidades) {
	Id			= static_cast<UINT>(sqlite3_column_int(SqlQuery, 0));
	Hash		= sqlite3_column_int64(SqlQuery, 1);
	Path		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
	Nombre		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
	TipoMedio	= static_cast<Tipo_Medio>(sqlite3_column_int(SqlQuery, 4));
	Extension	= static_cast<Extension_Medio>(sqlite3_column_int(SqlQuery, 5));
	Reproducido = static_cast<UINT>(sqlite3_column_int(SqlQuery, 6));
	Longitud	= static_cast<DWORD>(sqlite3_column_int(SqlQuery, 7));
	IDDisco		= static_cast<DWORD>(sqlite3_column_int(SqlQuery, 8));
	Nota		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 9));
	Genero		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 10));
	Grupo		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 11));
	Disco		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 12));
	Pista		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 13));
	Tiempo		= static_cast<libvlc_time_t>(sqlite3_column_int(SqlQuery, 14));
	Subtitulos	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 15));	

	DWL::DUnidadDisco *Unidad = Unidades.Buscar_Numero_Serie(IDDisco);
	if (Unidad != NULL) Path[0] = Unidad->Letra();

}

void BDMedio::PistaStr(std::wstring &nPistaStr) {
	if (Pista < 10) { nPistaStr = L"0" + std::to_wstring(Pista); }
	else { nPistaStr = std::to_wstring(Pista); }
}