#include "stdafx.h"
#include "RaveOpciones.h"
#include <DDirectoriosWindows.h>

RaveOpciones::RaveOpciones(void) : _Volumen(0), _PosX(0), _PosY(0), _VentanaOpciones_PosX(0), _VentanaOpciones_PosY(0), _DlgDirectorios_PosX(0), _DlgDirectorios_PosY(0), 
								  _VentanaAnalizar_PosX(0), _VentanaAnalizar_PosY(0), _Ancho(0), _Alto(0), _Shufle(FALSE), _Repeat(Tipo_Repeat_NADA), _Inicio(Tipo_Inicio_NADA),
								  _Version(0.0f), _OcultarMouseEnVideo(0), _MostrarObtenerMetadatos(FALSE), _MostrarAsociarArchivos(FALSE), _AnalizarMediosPendientes(FALSE), 
								  _BuscarActualizacion(FALSE), _TiempoAnimaciones(0), _TiempoToolTips(0), _NoAgregarMedioMenos25(FALSE), _NoGenerarListasMenos3(FALSE), 
							 	  _Sumar005(FALSE), _AlineacionControlesVideo(0), _OpacidadControlesVideo(0), _EfectoFadeAudioMS(5000), _DlgDirectorios_Ancho(400), _DlgDirectorios_Alto(600), 
								  _VentanaMomentos_PosX(100), _VentanaMomentos_PosY(100), _OcultarTooltipsMouse(0), _MostrarMedioActualTitulo(TRUE), 
								  _MezclarListaGenero(0), _MezclarListaGrupo(0), _MezclarListaDisco(0), _MezclarLista50Can(0), _MezclarListaNota(0), 
								  _GuardarBSCP(1), _VersionOpciones(1.1f), _EfectoFadeAudio(FALSE) {
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
	if (_CrearTablas() == FALSE) {
		_ModificarTablas();
	}

	// Obtengo las opciones generales
	ObtenerOpciones();
	// Obtengo las teclas rapidas
	ObtenerTeclasRapidas();
	// Obtengo las raices de la base de datos
	ObtenerRaices();


	Debug_Escribir(L"RaveOpciones::Iniciar\n");

	return TRUE;
}


const BOOL RaveOpciones::_ModificarTablas(void) {
	int         Version = ObtenerVersionOpciones();
	int			SqlRet  = 0;
	BOOL        Ret		= FALSE;
	// Las opciones separadas de los medios empezaron por la versión 1.1 (la versión 1.0 estaba unida a la BD de medios)
	if (Version < 12) {
		// Añado la columna EfectoFadeAudio en las opciones
		SqlRet = Consulta(L"ALTER TABLE Opciones ADD COLUMN EfectoFadeAudio INTEGER AFTER VersionOpciones");

		// Actualizo el valor de EfectoFadeAudio a 0
		SqlRet = Consulta(L"UPDATE Opciones SET EfectoFadeAudio=0, EfectoFadeAudioMS=5000");
		_EfectoFadeAudio = FALSE;

		// Versión //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Actualizo la versión de la BD (MOVER ESTE UPDATE A LA ULTIMA VERSIÓN)			
		SqlRet = Consulta(L"UPDATE Opciones SET VersionOpciones=" RAVE_VERSIONOPCIONES L" WHERE Id=0");
		
		Ret = TRUE;
	}

	return Ret;
}


const BOOL RaveOpciones::_CrearTablas(void) {
	std::wstring Q;


	// Creo la tabla para las raices
	std::wstring CrearTablaRaiz = L"CREATE TABLE Raiz (Id INTEGER PRIMARY KEY, Path VARCHAR(260), IdDisco INTEGER)";
	if (Consulta(CrearTablaRaiz.c_str()) == SQLITE_ERROR) return FALSE;

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
			L"EfectoFadeAudioMS"     	L" INTEGER,"					// 29 Tiempo para el fade in/out entre canciones
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
			L"VersionOpciones"          L" DOUBLE,"						// 42 versión de las opciones
			L"EfectoFadeAudio"          L" INTEGER"						// 43 Efecto fade in/out
		L")";
	if (Consulta(CrearTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;
	//////////////////////////////////////////////////////////////////////////////////////////////////


	// Añado los datos por defecto de las opciones ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::wstring ValoresTablaOpciones = L"INSERT INTO Opciones (ID, Volumen, PathAbrir, PosX, PosY, Ancho, Alto, Shufle, Repeat, Inicio, OcultarMouseEnVideo, Version, MostrarObtenerMetadatos, MostrarAsociarArchivos, AnalizarMediosPendientes, VentanaOpciones_PosX, VentanaOpciones_PosY, VentanaAsociar_PosX, VentanaAsociar_PosY, VentanaAnalizar_PosX, VentanaAnalizar_PosY, BuscarActualizacion, TiempoAnimaciones, TiempoToolTips, NoAgregarMedioMenos25, NoGenerarListasMenos3, Sumar005, AlineacionControlesVideo, OpacidadControlesVideo, EfectoFadeAudioMS, DlgDirectorios_Ancho, DlgDirectorios_Alto, VentanaMomentos_PosX, VentanaMomentos_PosY, OcultarTooltipsMouse, MostrarMedioActualTitulo, MezclarListaGenero, MezclarListaGrupo, MezclarListaDisco, MezclarLista50Can, MezclarListaNota, GuardarBSCP, VersionOpciones, EfectoFadeAudio) "
		L"VALUES(0, 100, \"C:\\\", 100, 100, 660, 400, 0, 0, 0, 3000, " RAVE_VERSIONBD ", 1, 1, 1, 400, 300, 500, 400, 300, 200, 1, 400, 5500, 1, 1, 1, 0, 200, 5000, 350, 400, 100, 100, 0, 1, 0, 0, 0, 0, 0, 1, " RAVE_VERSIONOPCIONES ", 0)";
	if (Consulta(ValoresTablaOpciones.c_str()) == SQLITE_ERROR) return FALSE;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// Creo la tabla para las teclas rápidas ///////////////////////////////////////////////////
	std::wstring CrearTablaTeclasRapidas = L"CREATE TABLE TeclasRapidas ("
		L"Tecla"	L" INTEGER,"
		L"Control"	L" TINYINT(1),"
		L"Alt"		L" TINYINT(1),"
		L"Shift"	L" TINYINT(1)"
		L")";
	if (Consulta(CrearTablaTeclasRapidas.c_str()) == SQLITE_ERROR) return FALSE;
	// Agrego los valores por defecto en la tabla de las teclas rápidas
	for (size_t i = 0; i < App.TeclasRapidas.size(); i++) {
		Q = L"INSERT INTO TeclasRapidas (Tecla, Control, Alt, Shift) "
			L"VALUES(" + std::to_wstring(App.TeclasRapidas[i].Tecla) + L"," +
			std::to_wstring(App.TeclasRapidas[i].Control) + L"," +
			std::to_wstring(App.TeclasRapidas[i].Alt) + L"," +
			std::to_wstring(App.TeclasRapidas[i].Shift) + L")";
		if (Consulta(Q.c_str()) == SQLITE_ERROR) return FALSE;
	}
	////////////////////////////////////////////////////////////////////////////////////////////



	return TRUE;
}




const BOOL RaveOpciones::ObtenerTeclasRapidas(void) {
	std::wstring	Q = L"SELECT * FROM TeclasRapidas";
	wchar_t* SqlError = NULL;
	int				SqlRet = 0;
	sqlite3_stmt* SqlQuery = NULL;

	SqlRet = sqlite3_prepare16_v2(_BD, Q.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	int VecesBusy = 0;
	size_t R = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			if (R == App.TeclasRapidas.size()) break;
			App.TeclasRapidas[R].Tecla		= static_cast<int>(sqlite3_column_int(SqlQuery, 0));
			App.TeclasRapidas[R].Control	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 1));
			App.TeclasRapidas[R].Alt		= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 2));
			App.TeclasRapidas[R++].Shift	= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 3));
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

const BOOL RaveOpciones::GuardarTeclasRapidas(void) {
	// Borro todos los datos de la tabla TeclasRapidas sin borrar la tabla
	Consulta(L"DELETE FROM TeclasRapidas");
	std::wstring Q;
	int          SqlRet;
	for (size_t i = 0; i < App.TeclasRapidas.size(); i++) {
		Q = L"INSERT INTO TeclasRapidas (Tecla, Control, Alt, Shift) VALUES(" +
			std::to_wstring(App.TeclasRapidas[i].Tecla) + L"," +
			std::to_wstring(App.TeclasRapidas[i].Control) + L"," +
			std::to_wstring(App.TeclasRapidas[i].Alt) + L"," +
			std::to_wstring(App.TeclasRapidas[i].Shift) +
			L")";
		SqlRet = Consulta(Q);
		if (SqlRet != SQLITE_DONE) return FALSE;
	}
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
			_Volumen					= static_cast<long>(sqlite3_column_int(SqlQuery, 1));
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
			_EfectoFadeAudio			= static_cast<BOOL>(sqlite3_column_int(SqlQuery, 43));

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


void RaveOpciones::Volumen(const long nVolumen) {
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

void RaveOpciones::EfectoFadeAudio(const BOOL nEfectoFadeAudio) {
	_EfectoFadeAudio = nEfectoFadeAudio;
	std::wstring Q = L"Update Opciones SET EfectoFadeAudio=" + std::to_wstring(nEfectoFadeAudio) + L" WHERE Id=0";
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
const int RaveOpciones::ObtenerVersionBD(void) {
	return (int)std::round(Select<float>(L"Opciones", L"Version") * 10.0);
}


// Función que devuelve la versión de las opciones (REHACER SELECT para que solo mire la version)
const int RaveOpciones::ObtenerVersionOpciones(void) {
	return (int)std::round(Select<float>(L"Opciones", L"VersionOpciones") * 10.0);
}



const BOOL RaveOpciones::GuardarPosTamVentana(void) {
	if (App.VentanaRave.Maximizada() == FALSE) {
		RECT RW, RC;
		GetWindowRect(App.VentanaRave.hWnd(), &RW);
		GetClientRect(App.VentanaRave.hWnd(), &RC);
		_PosX = RW.left;
		_PosY = RW.top;
		_Ancho = RC.right;
		_Alto = RC.bottom;

		std::wstring Q = L"UPDATE Opciones SET PosX=" + std::to_wstring(_PosX) + L", PosY=" + std::to_wstring(_PosY) + L", Ancho=" + std::to_wstring(_Ancho) + L", Alto=" + std::to_wstring(_Alto) + L" WHERE Id=0";
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
	GetWindowRect(App.VentanaOpciones2.hWnd(), &RV);
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

const BOOL RaveOpciones::GuardarPosTamDlgDirectorios(RECT& RW, RECT &RC) {
	_DlgDirectorios_PosX	= RW.left;
	_DlgDirectorios_PosY	= RW.top;
	_DlgDirectorios_Ancho	= RC.right;
	_DlgDirectorios_Alto	= RC.bottom;

	std::wstring Q = L"UPDATE Opciones SET VentanaAsociar_PosX=" + std::to_wstring(RW.left) + L", VentanaAsociar_PosY=" + std::to_wstring(RW.top) + L", DlgDirectorios_Ancho=" + std::to_wstring(_DlgDirectorios_Ancho) + L", DlgDirectorios_Ancho=" + std::to_wstring(_DlgDirectorios_Alto) + L" WHERE Id=0";
	int SqlRet = Consulta(Q);
	if (SqlRet == SQLITE_ERROR) {
		_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
		return FALSE;
	}
	return TRUE;
}



// Función que busca una raíz por el path
// Se ha adaptado para que encuentre raices parciales, es decir :
// - Si tenemos en la BD la raíz "c:\mp3\grupo" y buscamos "c:\mp3" devolvera la raíz "c:\mp3\grupo"
// - Si tenemos en la BD la raíz "c:\mp3" y buscamos "c:\mp3\grupo" devolverá la raíz "c:\mp3"
BDRaiz* RaveOpciones::BuscarRaiz(std::wstring& Path) {
	int Comp = 0;

	for (size_t i = 0; i < _Raices.size(); i++) {
		if (_CompararRaices(_Raices[i]->Path, Path) == TRUE) {
			return _Raices[i];
		}
	}
	return NULL;
}


// Función que compara 2 paths pertenecientes a posibles raices
// Se ha adaptado para que encuentre raices parciales, es decir :
// - Si tenemos en la BD la raíz "c:\mp3\grupo" y buscamos "c:\mp3" devolvera la raíz "c:\mp3\grupo"
// - Si tenemos en la BD la raíz "c:\mp3" y buscamos "c:\mp3\grupo" devolverá la raíz "c:\mp3"
const BOOL RaveOpciones::_CompararRaices(std::wstring& Path1, std::wstring& Path2) {
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
BDRaiz* RaveOpciones::BuscarRaiz(const unsigned long bID) {
	for (size_t i = 0; i < _Raices.size(); i++) {
		if (_Raices[i]->Id == bID) return _Raices[i];
	}
	return NULL;
}
// Elimina las raices de la memória
void RaveOpciones::_BorrarRaices(void) {
	for (size_t i = 0; i < _Raices.size(); i++) delete _Raices[i];
	_Raices.resize(0);
}

// Función que obtiene todas las raices de la base de datos, y las carga en memória
const BOOL RaveOpciones::ObtenerRaices(void) {
	_BorrarRaices();
	int				    SqlRet = 0;
	const wchar_t* SqlStr = L"SELECT * FROM Raiz";
	DWL::DUnidadesDisco nUnidades;
	DWL::DUnidadDisco* Unidad = NULL;
	sqlite3_stmt* SqlQuery = NULL;
	SqlRet = sqlite3_prepare16_v2(_BD, SqlStr, -1, &SqlQuery, NULL);
	if (SqlRet) return FALSE;
	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_CONSTRAINT) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDRaiz* TmpRaiz = new BDRaiz;
			TmpRaiz->Id = static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 0));
			TmpRaiz->Path = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(SqlQuery, 1));
			TmpRaiz->ID_Disco = static_cast<unsigned long>(sqlite3_column_int64(SqlQuery, 2));
			// Asigno la letra de unidad a la raíz ya que las unidades extraibles pueden cambiar de letra pero siempre tienen el mismo número de serie.
			Unidad = nUnidades.Buscar_Numero_Serie(TmpRaiz->ID_Disco);
			if (Unidad != NULL) {
				TmpRaiz->Path[0] = Unidad->Letra();
				TmpRaiz->Letra = Unidad->Letra();
			}
			_Raices.push_back(TmpRaiz);
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



/* Devuelve 0 si ya existe una raiz igual, devuelve 1 si se ha agregado una nueva raiz, devuelve 2 si se ha agregado una nueva raiz, pero han desaparecido otras raices parientes */

/*	Se puede dar el caso en que dada una raíz existente se quiera agregar otra raíz que es pariente de esta.
	En este no se añadirá ninguna raíz y la raíz existente pasara a tener el path que ocupe menos caracteres. */
const int RaveOpciones::AgregarRaiz(std::wstring& nPath, DWL::DUnidadesDisco &Unidades) {
	int				Ret = 1;
	UINT			Total = 0;
	int				SqlRet = 0;
	size_t          i = 0;
	sqlite3_stmt* SqlQuery = NULL;
	std::wstring	PathFinal = nPath;
	if (PathFinal[PathFinal.size() - 1] != TEXT('\\')) PathFinal += TEXT('\\');

	std::wstring    RaizFinal = PathFinal;		// Path final mas corto
	size_t			PathMasCorto = RaizFinal.size();

	// Busco raices similares
	std::vector<BDRaiz*> RaicesSimilares;
	for (i = 0; i < _Raices.size(); i++) {
		// Si las raices se parecen y no tienen el mismo número de caracteres
		if (_CompararRaices(_Raices[i]->Path, PathFinal) == TRUE) {
			if (_Raices[i]->Path.size() != PathFinal.size()) {
				RaicesSimilares.push_back(_Raices[i]);
				// Busco el path mas corto que será el definitivo
				if (PathMasCorto > _Raices[i]->Path.size()) {
					PathMasCorto = _Raices[i]->Path.size();
					RaizFinal = _Raices[i]->Path;
				}
			}
			// La raiz es igual
			else {
				return 0;
			}
		}
	}

	// Borro todas las raices similares
	for (i = 0; i < RaicesSimilares.size(); i++) {
		std::wstring Q = L"DELETE FROM Raiz WHERE Path=\"" + RaicesSimilares[i]->Path + L"\"";
		SqlRet = Consulta(Q);
		Ret = 2;
	}

	DWL::DUnidadDisco *Unidad = Unidades.Buscar_Letra(RaizFinal[0]);
	if (Unidad == NULL)
		return NULL;

	std::wstring SqlStr = L"INSERT INTO Raiz (Path, IDDisco) VALUES(\"" + RaizFinal + L"\", " + std::to_wstring(Unidad->Numero_Serie()) + L")";
	SqlRet = Consulta(SqlStr);

	ObtenerRaices();

	return Ret;
}


const BOOL RaveOpciones::EliminarRaiz(std::wstring& nPath) {
	std::wstring Q = L"DELETE FROM Raiz WHERE Path=\"" + nPath + L"\"";
	int SqlRet = Consulta(Q);
	//	BOOL Ret = ConsultaVarg(L"DELETE FROM Raiz WHERE Path ='%s'", nPath.c_str());
	if (SqlRet == SQLITE_ERROR) {
		return FALSE;
	}
	ObtenerRaices();
	return TRUE;
}
