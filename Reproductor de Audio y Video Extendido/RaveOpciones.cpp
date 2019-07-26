#include "stdafx.h"
#include "RaveOpciones.h"
#include <DDirectoriosWindows.h>

RaveOpciones::RaveOpciones(void) : _Volumen(0), _PosX(0), _PosY(0), _VentanaOpciones_PosX(0), _VentanaOpciones_PosY(0), _DlgDirectorios_PosX(0), _DlgDirectorios_PosY(0), 
								  _VentanaAnalizar_PosX(0), _VentanaAnalizar_PosY(0), _Ancho(0), _Alto(0), _Shufle(FALSE), _Repeat(Tipo_Repeat_NADA), _Inicio(Tipo_Inicio_NADA),
								  _Version(0.0f), _OcultarMouseEnVideo(0), _MostrarObtenerMetadatos(FALSE), _MostrarAsociarArchivos(FALSE), _AnalizarMediosPendientes(FALSE), 
								  _BuscarActualizacion(FALSE), _TiempoAnimaciones(0), _TiempoToolTips(0), _NoAgregarMedioMenos25(FALSE), _NoGenerarListasMenos3(FALSE), 
							 	  _Sumar005(FALSE), _AlineacionControlesVideo(0), _OpacidadControlesVideo(0), _EfectoFadeAudioMS(10000), _DlgDirectorios_Ancho(400), _DlgDirectorios_Alto(600), 
								  _VentanaMomentos_PosX(100), _VentanaMomentos_PosY(100), _OcultarTooltipsMouse(0), _MostrarMedioActualTitulo(TRUE), 
								  _MezclarListaGenero(0), _MezclarListaGrupo(0), _MezclarListaDisco(0), _MezclarLista50Can(0), _MezclarListaNota(0), 
								  _GuardarBSCP(1), _VersionOpciones(1.1f) {
}


const BOOL RaveOpciones::Iniciar(void) {
	int Ret = 0;
	std::wstring PathBD;
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(PathBD);
	PathBD += L"\\Rave\\";
	// Creo el directorio "\AppData\Rave" si no existe
	if (GetFileAttributes(PathBD.c_str()) == INVALID_FILE_ATTRIBUTES)
		CreateDirectory(PathBD.c_str(), NULL);

	PathBD += L"Opciones.BD";

	if (IniciarSQLite(PathBD.c_str()) == FALSE) {
		Debug_Escribir(L"RaveOpciones::Iniciar ERROR\n");
		return FALSE;
	}
	// Creo las tablas para la base de datos (si es necesario)
	_CrearTablas();

	ObtenerOpciones();

	Debug_Escribir(L"RaveOpciones::Iniciar\n");

	return TRUE;
}


const BOOL RaveOpciones::_CrearTablas(void) {

	// Creo la tabla para las opciones /////////////////////////////////////////////////////////////
	std::wstring CrearTablaOpciones = L"CREATE TABLE Opciones ("
		L"Id" 						L" INTEGER PRIMARY KEY,"		// 00
		L"Volumen"					L" INTEGER,"					// 01
		L"PathAbrir"				L" VARCHAR(260),"				// 02 NO SE USA	  
		L"PosX"						L" INTEGER,"					// 03
		L"PosY"						L" INTEGER,"					// 04
		L"Ancho"					L" INTEGER,"					// 05
		L"Alto"						L" INTEGER,"					// 06
		L"Shufle"					L" INTEGER,"					// 07
		L"Repeat"					L" INTEGER,"					// 08
		L"Inicio"					L" INTEGER,"					// 09
		L"OcultarMouseEnVideo"		L" INTEGER,"					// 10
		L"Version"					L" DOUBLE,"						// 11
		L"MostrarObtenerMetadatos"	L" INTEGER,"					// 12
		L"MostrarAsociarArchivos"	L" INTEGER,"					// 13 NO SE USA
		L"AnalizarMediosPendientes" L" INTEGER,"					// 14
		L"VentanaOpciones_PosX"		L" INTEGER,"					// 15
		L"VentanaOpciones_PosY"		L" INTEGER,"					// 16
		L"VentanaAsociar_PosX"		L" INTEGER,"					// 17 Ahora es Opciones_DlgDirectorios_PosX
		L"VentanaAsociar_PosY"		L" INTEGER,"					// 18 Ahora es Opciones_DlgDirectorios_PosY
		L"VentanaAnalizar_PosX"		L" INTEGER,"					// 19
		L"VentanaAnalizar_PosY"		L" INTEGER,"					// 20
		L"BuscarActualizacion"		L" INTEGER,"					// 21
		L"TiempoAnimaciones"		L" INTEGER,"					// 22
		L"TiempoToolTips"			L" INTEGER,"					// 23
		L"NoAgregarMedioMenos25"	L" INTEGER,"					// 24
		L"NoGenerarListasMenos3"	L" INTEGER,"					// 25
		L"Sumar005"					L" INTEGER,"					// 26 Sumar 0.05 a la nota una vez finalizado el medio
		L"AlineacionControlesVideo"	L" INTEGER,"					// 27 Alineación para los controles del video
		L"OpacidadControlesVideo"	L" INTEGER,"					// 28 Opacidad másima para los controles del video (ControlesPantallaCompleta)
		L"EfectoFadeAudioMS"     	L" INTEGER,"					// 29 (NO SE USA DE MOMENTO, COMO MUCHO SE USARA SI IMPLEMENTO LA FMOD)
		L"DlgDirectorios_Ancho"   	L" INTEGER,"					// 30
		L"DlgDirectorios_Alto"   	L" INTEGER,"					// 31
		L"VentanaMomentos_PosX"   	L" INTEGER,"					// 32 Posición X de la ventana de los momentos
		L"VentanaMomentos_PosY"   	L" INTEGER,"					// 33 Posición Y de la ventana de los momentos
		L"OcultarTooltipsMouse"     L" INTEGER,"					// 34 Ocultar tooltips al pasar por encima con el mouse
		L"MostrarMedioActualTitulo" L" INTEGER,"					// 35 Mostrar el nombre del medio actual en la barra de titulo
		L"MezclarListaGenero"       L" INTEGER,"					// 36 Mezclar listas por genero
		L"MezclarListaGrupo"        L" INTEGER,"					// 37 Mezclar listas por grupo
		L"MezclarListaDisco"        L" INTEGER,"					// 38 Mezclar listas por disco
		L"MezclarLista50Can"        L" INTEGER,"					// 39 Mezclar listas con 50 canciones
		L"MezclarListaNota"         L" INTEGER,"					// 40 Mezclar listas por nota
		L"GuardarBSCP"              L" INTEGER,"					// 41 guardar brillo, contraste, saturación, y proporción
		L"VersionOpciones"          L" DOUBLE"						// 42 versión de las opciones
		L")";
	if (Consulta(CrearTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;
	//////////////////////////////////////////////////////////////////////////////////////////////////


	// Añado los datos por defecto de las opciones ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::wstring ValoresTablaOpciones = L"INSERT INTO Opciones (ID, Volumen, PathAbrir, PosX, PosY, Ancho, Alto, Shufle, Repeat, Inicio, OcultarMouseEnVideo, Version, MostrarObtenerMetadatos, MostrarAsociarArchivos, AnalizarMediosPendientes, VentanaOpciones_PosX, VentanaOpciones_PosY, VentanaAsociar_PosX, VentanaAsociar_PosY, VentanaAnalizar_PosX, VentanaAnalizar_PosY, BuscarActualizacion, TiempoAnimaciones, TiempoToolTips, NoAgregarMedioMenos25, NoGenerarListasMenos3, Sumar005, AlineacionControlesVideo, OpacidadControlesVideo, EfectoFadeAudioMS, DlgDirectorios_Ancho, DlgDirectorios_Alto, VentanaMomentos_PosX, VentanaMomentos_PosY, OcultarTooltipsMouse, MostrarMedioActualTitulo, MezclarListaGenero, MezclarListaGrupo, MezclarListaDisco, MezclarLista50Can, MezclarListaNota, GuardarBSCP, VersionOpciones) "
		L"VALUES(0, 100, \"C:\\\", 100, 100, 660, 400, 0, 0, 0, 3000, " RAVE_VERSIONBD ", 1, 1, 1, 400, 300, 500, 400, 300, 200, 1, 400, 5500, 1, 1, 1, 0, 200, 10000, 350, 400, 100, 100, 0, 1, 0, 0, 0, 0, 0, 1, " RAVE_VERSIONOPCIONES ")";
	if (Consulta(ValoresTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return TRUE;
}



const BOOL RaveOpciones::ObtenerOpciones(void) {

	const wchar_t* SqlStr = L"SELECT * FROM Opciones";
	wchar_t* SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt* SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			_Volumen					= static_cast<int>(sqlite3_column_int(SqlQuery, 1));
			_PathAbrir					= reinterpret_cast<const wchar_t*>(sqlite3_column_text16(SqlQuery, 2));
			_PosX						= static_cast<int>(sqlite3_column_int(SqlQuery, 3));
			_PosY						= static_cast<int>(sqlite3_column_int(SqlQuery, 4));
			_Ancho						= static_cast<int>(sqlite3_column_int(SqlQuery, 5));
			_Alto						= static_cast<int>(sqlite3_column_int(SqlQuery, 6));
			_Shufle						= static_cast<int>(sqlite3_column_int(SqlQuery, 7));
			_Repeat						= static_cast<Tipo_Repeat>(sqlite3_column_int(SqlQuery, 8));
			_Inicio						= static_cast<Tipo_Inicio>(sqlite3_column_int(SqlQuery, 9));
			_OcultarMouseEnVideo		= static_cast<int>(sqlite3_column_int(SqlQuery, 10));
			_Version					= static_cast<float>(sqlite3_column_double(SqlQuery, 11));
			_MostrarObtenerMetadatos	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 12));
			_MostrarAsociarArchivos		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 13));
			_AnalizarMediosPendientes	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 14));
			_VentanaOpciones_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 15));
			_VentanaOpciones_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 16));
			_DlgDirectorios_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 17));
			_DlgDirectorios_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 18));
			_VentanaAnalizar_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 19));
			_VentanaAnalizar_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 20));
			_BuscarActualizacion		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 21));
			_TiempoAnimaciones			= static_cast<UINT>(sqlite3_column_int(SqlQuery, 22));
			DhWnd::TiempoAnimaciones = _TiempoAnimaciones;
			_TiempoToolTips				= static_cast<UINT>(sqlite3_column_int(SqlQuery, 23));
			_NoAgregarMedioMenos25		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 24));
			_NoGenerarListasMenos3		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 25));
			_Sumar005					= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 26));
			_AlineacionControlesVideo	= static_cast<int>(sqlite3_column_int(SqlQuery, 27));
			_OpacidadControlesVideo		= static_cast<int>(sqlite3_column_int(SqlQuery, 28));
			_EfectoFadeAudioMS			= static_cast<int>(sqlite3_column_int(SqlQuery, 29));
			_DlgDirectorios_Ancho		= static_cast<int>(sqlite3_column_int(SqlQuery, 30));
			_DlgDirectorios_Alto		= static_cast<int>(sqlite3_column_int(SqlQuery, 31));
			_VentanaMomentos_PosX		= static_cast<int>(sqlite3_column_int(SqlQuery, 32));
			_VentanaMomentos_PosY		= static_cast<int>(sqlite3_column_int(SqlQuery, 33));
			_OcultarTooltipsMouse		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 34));
			_MostrarMedioActualTitulo	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 35));
			_MezclarListaGenero			= static_cast<int>(sqlite3_column_int(SqlQuery, 36));
			_MezclarListaGrupo			= static_cast<int>(sqlite3_column_int(SqlQuery, 37));
			_MezclarListaDisco			= static_cast<int>(sqlite3_column_int(SqlQuery, 38));
			_MezclarLista50Can			= static_cast<int>(sqlite3_column_int(SqlQuery, 39));
			_MezclarListaNota			= static_cast<int>(sqlite3_column_int(SqlQuery, 40));
			_GuardarBSCP				= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 41));
			_VersionOpciones			= static_cast<float>(sqlite3_column_double(SqlQuery, 42));

			// Si hay alguna opcion de mezclar lista que no sea cero, establezco el shufle inicial a FALSE.
			if (_MezclarListaGenero != 0 || _MezclarListaGrupo != 0 || _MezclarListaDisco != 0 || _MezclarLista50Can != 0 || _MezclarListaNota != 0) {
				_Shufle = FALSE;
			}
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}

	return (SqlRet != SQLITE_BUSY);
}


void RaveOpciones::Volumen(const int nVolumen) {
	_Volumen = nVolumen;
	std::wstring Q = L"Update Opciones SET Volumen=" + std::to_wstring(nVolumen) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveOpciones::Shufle(const BOOL nShufle) {
	_Shufle = nShufle;
	std::wstring Q = L"Update Opciones SET Shufle=" + std::to_wstring(nShufle) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveOpciones::Repeat(const Tipo_Repeat nRepeat) {
	_Repeat = nRepeat;

	// Guardo el valor del repeat en la BD, siempre que el repeat no implique apagar el reproductor o el windows
	if (nRepeat != Tipo_Repeat_ApagarReproductor && nRepeat != Tipo_Repeat_ApagarOrdenador /*&& nRepeat != Tipo_Repeat_HibernarOrdenador*/) {
		std::wstring Q = L"Update Opciones SET Repeat=" + std::to_wstring(nRepeat) + L" WHERE Id=0";
		Consulta(Q.c_str());
	}
}

void RaveOpciones::Inicio(const Tipo_Inicio nInicio) {
	_Inicio = nInicio;
	std::wstring Q = L"Update Opciones SET Inicio=" + std::to_wstring(nInicio) + L" WHERE Id=0";
	Consulta(Q.c_str());
}


void RaveOpciones::OcultarMouseEnVideo(const int nOcultarMouseEnVideo) {
	_OcultarMouseEnVideo = nOcultarMouseEnVideo;
	std::wstring Q = L"Update Opciones SET OcultarMouseEnVideo=" + std::to_wstring(nOcultarMouseEnVideo) + L" WHERE Id=0";
	Consulta(Q.c_str());
}

void RaveOpciones::MostrarObtenerMetadatos(const BOOL nMostrarObtenerMetadatos) {
	_MostrarObtenerMetadatos = nMostrarObtenerMetadatos;
	std::wstring Q = L"Update Opciones SET MostrarObtenerMetadatos=" + std::to_wstring(nMostrarObtenerMetadatos) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::MostrarAsociarArchivos(const BOOL nMostrarAsociarArchivos) {
	_MostrarObtenerMetadatos = nMostrarAsociarArchivos;
	std::wstring Q = L"Update Opciones SET MostrarAsociarArchivos=" + std::to_wstring(nMostrarAsociarArchivos) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::AnalizarMediosPendientes(const BOOL nAnalizarMediosPendientes) {
	_AnalizarMediosPendientes = nAnalizarMediosPendientes;
	std::wstring Q = L"Update Opciones SET AnalizarMediosPendientes=" + std::to_wstring(nAnalizarMediosPendientes) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::BuscarActualizacion(const BOOL nBuscarActualizacion) {
	_BuscarActualizacion = nBuscarActualizacion;
	std::wstring Q = L"Update Opciones SET BuscarActualizacion=" + std::to_wstring(nBuscarActualizacion) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::TiempoAnimaciones(const UINT nTiempoAnimaciones) {
	_TiempoAnimaciones = nTiempoAnimaciones;
	std::wstring Q = L"Update Opciones SET TiempoAnimaciones=" + std::to_wstring(nTiempoAnimaciones) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::TiempoToolTips(const UINT nTiempoToolTips) {
	_TiempoToolTips = nTiempoToolTips;
	std::wstring Q = L"Update Opciones SET ToolTips=" + std::to_wstring(nTiempoToolTips) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
	Ret = Ret;
}

void RaveOpciones::NoAgregarMedioMenos25(const BOOL nNoAgregarMedioMenos25) {
	_NoAgregarMedioMenos25 = nNoAgregarMedioMenos25;
	std::wstring Q = L"Update Opciones SET NoAgregarMedioMenos25=" + std::to_wstring(nNoAgregarMedioMenos25) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::NoGenerarListasMenos3(const BOOL nNoGenerarListasMenos3) {
	_NoGenerarListasMenos3 = nNoGenerarListasMenos3;
	std::wstring Q = L"Update Opciones SET NoGenerarListasMenos3=" + std::to_wstring(nNoGenerarListasMenos3) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::Sumar005(const BOOL nSumar005) {
	_Sumar005 = nSumar005;
	std::wstring Q = L"Update Opciones SET Sumar005=" + std::to_wstring(nSumar005) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::AlineacionControlesVideo(const int nAlineacionControlesVideo) {
	_AlineacionControlesVideo = nAlineacionControlesVideo;
	std::wstring Q = L"Update Opciones SET AlineacionControlesVideo=" + std::to_wstring(nAlineacionControlesVideo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::OpacidadControlesVideo(const int nOpacidadControlesVideo) {
	_OpacidadControlesVideo = nOpacidadControlesVideo;
	std::wstring Q = L"Update Opciones SET OpacidadControlesVideo=" + std::to_wstring(nOpacidadControlesVideo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::EfectoFadeAudioMS(const UINT nEfectoFadeAudioMS) {
	_EfectoFadeAudioMS = nEfectoFadeAudioMS;
	std::wstring Q = L"Update Opciones SET EfectoFadeAudioMS=" + std::to_wstring(nEfectoFadeAudioMS) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::OcultarTooltipsMouse(const BOOL nOcultarTooltipsMouse) {
	_OcultarTooltipsMouse = nOcultarTooltipsMouse;
	std::wstring Q = L"Update Opciones SET OcultarTooltipsMouse=" + std::to_wstring(nOcultarTooltipsMouse) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::MostrarMedioActualTitulo(const BOOL nMostrarMedioActualTitulo) {
	_MostrarMedioActualTitulo = nMostrarMedioActualTitulo;
	std::wstring Q = L"Update Opciones SET MostrarMedioActualTitulo=" + std::to_wstring(nMostrarMedioActualTitulo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::MezclarListaGenero(const int nMezclarListaGenero) {
	_MezclarListaGenero = nMezclarListaGenero;
	std::wstring Q = L"Update Opciones SET MezclarListaGenero=" + std::to_wstring(nMezclarListaGenero) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::MezclarListaGrupo(const int nMezclarListaGrupo) {
	_MezclarListaGrupo = nMezclarListaGrupo;
	std::wstring Q = L"Update Opciones SET MezclarListaGrupo=" + std::to_wstring(nMezclarListaGrupo) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::MezclarListaDisco(const int nMezclarListaDisco) {
	_MezclarListaDisco = nMezclarListaDisco;
	std::wstring Q = L"Update Opciones SET MezclarListaDisco=" + std::to_wstring(nMezclarListaDisco) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::MezclarLista50Can(const int nMezclarLista50Can) {
	_MezclarLista50Can = nMezclarLista50Can;
	std::wstring Q = L"Update Opciones SET MezclarLista50Can=" + std::to_wstring(nMezclarLista50Can) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::MezclarListaNota(const int nMezclarListaNota) {
	_MezclarListaNota = nMezclarListaNota;
	std::wstring Q = L"Update Opciones SET MezclarListaNota=" + std::to_wstring(nMezclarListaNota) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}

void RaveOpciones::GuardarBSCP(const BOOL nGuardarBSCP) {
	_GuardarBSCP = nGuardarBSCP;
	std::wstring Q = L"Update Opciones SET GuardarBSCP=" + std::to_wstring(nGuardarBSCP) + L" WHERE Id=0";
	int Ret = Consulta(Q.c_str());
}



// Función que devuelve la versión de la BD (REHACER SELECT para que solo mire la version)
const float RaveOpciones::ObtenerVersionBD(void) {
	float			RetVersion = 0.0f;
	const wchar_t*	SqlStr = L"SELECT Version FROM Opciones";
	wchar_t*		SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt*	SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			RetVersion = static_cast<float>(sqlite3_column_double(SqlQuery, 0));
		}


		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return 0.0f;
	}

	return (SqlRet != SQLITE_BUSY) ? RetVersion : 0.0f;
}


// Función que devuelve la versión de las opciones (REHACER SELECT para que solo mire la version)
const float RaveOpciones::ObtenerVersionOpciones(void) {
	float			RetVersion = 0.0f;
	const wchar_t*	SqlStr = L"SELECT VersionOpciones FROM Opciones";
	wchar_t*		SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt*	SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			RetVersion = static_cast<float>(sqlite3_column_double(SqlQuery, 0));
		}


		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}
	}

	sqlite3_finalize(SqlQuery);

	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return 0.0f;
	}

	return (SqlRet != SQLITE_BUSY) ? RetVersion : 0.0f;
}



const BOOL RaveOpciones::GuardarPosTamVentana(void) {
	if (App.VentanaRave.Maximizada() == FALSE) {
		RECT RC;
		GetWindowRect(App.VentanaRave.hWnd(), &RC);
		_PosX = RC.left;
		_PosY = RC.top;
		_Ancho = abs(RC.right - RC.left);
		_Alto = abs(RC.bottom - RC.top);

		std::wstring Q = L"UPDATE Opciones SET PosX=" + std::to_wstring(RC.left) + L", PosY=" + std::to_wstring(RC.top) + L", Ancho=" + std::to_wstring(_Ancho) + L", Alto=" + std::to_wstring(_Alto) + L" WHERE Id=0";
		int SqlRet = Consulta(Q);
		if (SqlRet == SQLITE_ERROR) {
			_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

const BOOL RaveOpciones::GuardarPosVentanaOpciones(void) {
	RECT RV;
	GetWindowRect(App.VentanaOpciones.hWnd(), &RV);
	_VentanaOpciones_PosX = RV.left;
	_VentanaOpciones_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaOpciones_PosX=" + std::to_wstring(RV.left) + L", VentanaOpciones_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}


const BOOL RaveOpciones::GuardarPosVentanaMomentos(void) {
	RECT RV;
	GetWindowRect(App.VentanaRave.Momentos.hWnd(), &RV);
	_VentanaMomentos_PosX = RV.left;
	_VentanaMomentos_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaMomentos_PosX=" + std::to_wstring(RV.left) + L", VentanaMomentos_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveOpciones::GuardarPosVentanaAnalizar(void) {
	RECT RV;
	GetWindowRect(App.VentanaRave.ThreadAnalizar.hWnd(), &RV);
	_VentanaAnalizar_PosX = RV.left;
	_VentanaAnalizar_PosY = RV.top;
	std::wstring Q = L"UPDATE Opciones SET VentanaAnalizar_PosX=" + std::to_wstring(RV.left) + L", VentanaAnalizar_PosY=" + std::to_wstring(RV.top) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}

const BOOL RaveOpciones::GuardarPosTamDlgDirectorios(RECT& RW) {
	_DlgDirectorios_PosX = RW.left;
	_DlgDirectorios_PosY = RW.top;
	_DlgDirectorios_Ancho = abs(RW.right - RW.left);
	_DlgDirectorios_Alto = abs(RW.bottom - RW.top);

	std::wstring Q = L"UPDATE Opciones SET VentanaAsociar_PosX=" + std::to_wstring(RW.left) + L", VentanaAsociar_PosY=" + std::to_wstring(RW.top) + L", DlgDirectorios_Ancho=" + std::to_wstring(_DlgDirectorios_Ancho) + L", DlgDirectorios_Ancho=" + std::to_wstring(_DlgDirectorios_Alto) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}