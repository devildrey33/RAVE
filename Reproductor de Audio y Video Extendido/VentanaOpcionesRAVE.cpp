#include "stdafx.h"
#include "VentanaOpcionesRAVE.h"
#include "resource.h"
#include "DDlgDirectorios.h"
#include "DStringUtils.h"

#define ID_BOTON_CERRAR                  999
#define ID_BOTON_BASEDATOS				1000
#define ID_BOTON_GENERAL				1001
#define ID_BOTON_ASOCIACIONESARCHIVOS	1002
#define ID_BOTON_LISTAS					1003
//#define ID_BOTON_VIDEO					1004
// Botones del tab superior
#define ID_MARCO_BASEDATOS				1005
#define ID_MARCO_GENERAL				1006
#define ID_MARCO_ASOCIACIONESARCHIVOS	1007
#define ID_MARCO_LISTAS					1008
#define ID_MARCO_VIDEO					1009
// Base de datos
#define ID_ETIQUETA_BASEDEDATOS			1010
#define ID_MARCA_MOSTRARANALISIS		1011
#define ID_MARCA_ANALIZARPENDIENTES		1012
// General
#define ID_ETIQUETAS_TECLADO            1020
#define ID_ASINGAR_TECLA                1040
#define ID_ETIQUETA_TECLASRAPIDAS		1060
#define ID_MARCA_BUSCARACTUALIZACIONES  1061
#define ID_ETIQUETA_TIEMPOANIMACIONES   1062
#define ID_ETIQUETA_TIEMPOANIMACIONES2  1063
#define ID_BARRA_TIEMPOANIMACION        1064
#define ID_ETIQUETA_TIEMPOTOOLTIP       1065
#define ID_ETIQUETA_TIEMPOTOOLTIP2      1066
#define ID_BARRA_TIEMPOTOOLTIP          1067
// Lista
#define ID_DESPLEGABLE_LISTAINICIO		1068


VentanaOpcionesRAVE::VentanaOpcionesRAVE(void) {
}

VentanaOpcionesRAVE::~VentanaOpcionesRAVE(void) {
}

void VentanaOpcionesRAVE::Crear(void) {
	DVentana::CrearVentana(NULL, L"RAVE_VentanaOpciones", L"Opciones", App.BD.Opciones_VentanaOpciones_PosX(), App.BD.Opciones_VentanaOpciones_PosY(), 590, 500, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	// Creo los botones de cada pesta�a
	BotonBaseDeDatos.CrearBotonEx(this, L"Base de datos", 20, 20, 135, 30, ID_BOTON_BASEDATOS);
	BotonBaseDeDatos.Marcado(TRUE);
	BotonGeneral.CrearBotonEx(this, L"General", 155, 20, 90, 30, ID_BOTON_GENERAL);
//	BotonAsociacionesArchivos.CrearBotonEx(this, L"Asociaciones de archivos", 225, 20, 200, 20, ID_BOTON_ASOCIACIONESARCHIVOS);
	BotonListas.CrearBotonEx(this, L"Listas", 245, 20, 80, 30, ID_BOTON_LISTAS);
	BotonVideo.CrearBotonEx(this, L"Video", 325, 20, 70, 30, ID_BOTON_VIDEO);
	// Asigno la fuente de los botones
/*	BotonBaseDeDatos.Fuente				= DhWnd::Fuente18Negrita;
	BotonGeneral.Fuente					= DhWnd::Fuente18Negrita;
//	BotonAsociacionesArchivos.Fuente	= DhWnd::Fuente18Negrita;
	BotonListas.Fuente					= DhWnd::Fuente18Negrita;
	BotonVideo.Fuente					= DhWnd::Fuente18Negrita;*/
	// Creo los marcos para los grupos de opciones
	MarcoBaseDeDatos.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_BASEDATOS);
	MarcoGeneral.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_GENERAL);
	MarcoGeneral.Visible(FALSE);
	MarcoAsociacionesArchivos.Crear(this, 10, 40, RC.right - 20, RC.bottom - 100, ID_MARCO_ASOCIACIONESARCHIVOS);
	MarcoAsociacionesArchivos.Visible(FALSE);
	MarcoListas.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_LISTAS);
	MarcoListas.Visible(FALSE);
	MarcoVideo.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_VIDEO);
	MarcoVideo.Visible(FALSE);

	BotonCerrar.CrearBotonEx(this, L"Cerrar", (RC.right - 70) / 2, RC.bottom - 40, 70, 30, ID_BOTON_CERRAR);

	//////////////////////////////////////////////////////
	// Creo los controles dentro del marco Base de datos
	const wchar_t *E1 = L"Para usuarios con una colecci�n superior a 100 canciones, es muy recomendable\n"
						L"agregar uno o mas directorios que contengan dichas canciones para que puedan\n"
						L"ser analizadas y a�adidas a la base de datos.\n\n"
						L"Una vez analizadas se podr� generar listas aleat�rias por : genero, grupo y disco.\n"
						L"Adem�s tambien permitir� busquedas avanzadas por etiqueta.";
	EtiquetaBaseDeDatos1.CrearEtiquetaEx(&MarcoBaseDeDatos, E1, 10, 10, RC.right - 40, 120, ID_ETIQUETA_BASEDEDATOS);
	// Lista con las raices
	ListaRaiz.CrearListaRaiz(&MarcoBaseDeDatos, 10, 130, RC.right - 40, 80, ID_LISTARAIZ);
	for (size_t i = 0; i < App.BD.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.BD.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Visible(TRUE);
	// Boton agregar ra�z
	BotonAgregarRaiz.CrearBotonEx(&MarcoBaseDeDatos, L"Agregar Ra�z", ((RC.right - 120) / 2) - 10, 220, 120, 24, ID_BOTON_AGREGARRAIZ);
	BotonAgregarRaiz.Fuente = Fuente21Normal;
	// Separador
	SeparadorBD.Crear(&MarcoBaseDeDatos, 0, 254, RC.right - 20);
	// Marca Mostrar analisis en una ventana
	MarcaMostrarAnalisis.CrearMarcaEx(&MarcoBaseDeDatos, L"Mostrar ventana con el progreso del an�lisis de los medios.", 70, 269, 405, 20, ID_MARCA_MOSTRARANALISIS, IDI_CHECK2);
	MarcaMostrarAnalisis.Marcado(App.BD.Opciones_MostrarObtenerMetadatos());
	// Analizar medios pendientes al actualizar la base de datos
	MarcaAnalizarMediosPendientes.CrearMarcaEx(&MarcoBaseDeDatos, L"Analizar medios pendientes al actualizar la base de datos.", 70, 299, 395, 20, ID_MARCA_ANALIZARPENDIENTES, IDI_CHECK2);
	MarcaAnalizarMediosPendientes.Marcado(App.BD.Opciones_AnalizarMediosPendientes());

	////////////////////////////////////////////////
	// Creo los controles dentro del marco General
	EtiquetaTeclasRapidas.CrearEtiquetaEx(&MarcoGeneral, L"Las teclas r�pidas solo se pueden utilizar cuando el reproductor tiene el foco.\n"
														 L"Es muy recomendable configurar las teclas r�pidas con una combinaci�n de teclas\n"
														 L"Por ejemplo : Control + tecla, Alt + Shift + Tecla, Control + Alt + tecla, etc..", 10, 10, RC.right - 40, 60, ID_ETIQUETA_TECLASRAPIDAS);
	const wchar_t *Textos[6] = {
		L"Play / Pausa",
		L"Generar lista aleat�ria",
		L"Subir volumen",
		L"Bajar volumen",
		L"Reproducir siguiente",
		L"Reporducir anterior"
	};
	for (int i = 0; i < App.TeclasRapidas.size(); i++) {
		EtiquetasTeclas[i].CrearEtiquetaEx(&MarcoGeneral, Textos[i], 10, 80 + (i * 25), 160, 20, ID_ETIQUETAS_TECLADO + i);
		EtiquetasTeclas[i].Fuente = Fuente18Normal;
		TeclasRapidas[i].Crear(&MarcoGeneral, 180, 80 + (i * 25), RC.right - 210, 20, ID_ASINGAR_TECLA, &App.TeclasRapidas[i]);
	}

	// Separador
	SeparadorGeneral.Crear(&MarcoGeneral, 0, 240, RC.right - 20);
	// Marca buscar actualizaciones
	MarcaBuscarActualizaciones.CrearMarcaEx(&MarcoGeneral, L"Buscar nuevas actualizaciones al iniciar el reproductor", 10, 255, 380, 20, ID_MARCA_BUSCARACTUALIZACIONES, IDI_CHECK2);
	MarcaBuscarActualizaciones.Marcado(App.BD.Opciones_BuscarActualizacion());
	MarcaBuscarActualizaciones.Activado(FALSE);
	// Etiquetas Tiempo de la animaci�n
	EtiquetaTiempoAnimaciones.CrearEtiquetaEx(&MarcoGeneral, L"Duraci�n de las animaciones de los controles", 10, 285, 300, 20, ID_ETIQUETA_TIEMPOANIMACIONES);
	std::wstring TmpStr = std::to_wstring(App.BD.Opciones_TiempoAnimaciones()) + L" ms";
	EtiquetaTiempoAnimacionesTA.CrearEtiquetaEx(&MarcoGeneral, TmpStr.c_str(), RC.right - 90, 285, 60, 20, ID_ETIQUETA_TIEMPOANIMACIONES2, DEtiquetaEx_Alineacion_Derecha);
	// Barra para asignar el tiempo de las animaciones
	BarraTiempoAnimaciones.CrearBarraDesplazamientoEx(&MarcoGeneral, 350, 285, RC.right - 450, 20, ID_BARRA_TIEMPOANIMACION, 100, 1000, static_cast<float>(App.BD.Opciones_TiempoAnimaciones()));
	// Etiquetas de tiempo para ocultar el tooltip
	EtiquetaTiempoOcultarToolTips.CrearEtiquetaEx(&MarcoGeneral, L"Duraci�n de los tooltips de la parte inferior derecha", 10, 315, 330, 20, ID_ETIQUETA_TIEMPOTOOLTIP);
	TmpStr = std::to_wstring(App.BD.Opciones_TiempoToolTips()) + L" ms";
	EtiquetaTiempoOcultarToolTipsTA.CrearEtiquetaEx(&MarcoGeneral, TmpStr.c_str(), RC.right - 100, 315, 70, 20, ID_ETIQUETA_TIEMPOTOOLTIP2, DEtiquetaEx_Alineacion_Derecha);
	// Barra para asignar el tiempo de ocultaci�n de los tooltips
	BarraTiempoToolTips.CrearBarraDesplazamientoEx(&MarcoGeneral, 350, 315, RC.right - 450, 20, ID_BARRA_TIEMPOTOOLTIP, 1000, 10000, static_cast<float>(App.BD.Opciones_TiempoToolTips()));

	///////////////////////////////////////
	// Creo los controles del marco Listas
	DesplegableListaInicio.CrearDesplegable(&MarcoListas, 10, 10, 200, 30, ID_DESPLEGABLE_LISTAINICIO, DWL::DDesplegableEx_TipoEdicion_SinEdicion, DWL::DDesplegableEx_TipoDesplegable_Lista);
	DesplegableListaInicio.Texto(L"Nada");


	// Muestro la ventana de las opciones
	Visible(TRUE);
	Repintar();
}


void VentanaOpcionesRAVE::AsignarMarco(const INT_PTR Id) {
	MarcoBaseDeDatos.Visible(FALSE);
	MarcoGeneral.Visible(FALSE);
	MarcoAsociacionesArchivos.Visible(FALSE);
	MarcoListas.Visible(FALSE);
	MarcoVideo.Visible(FALSE);
	BotonBaseDeDatos.Marcado(FALSE);
	BotonGeneral.Marcado(FALSE);
//	BotonAsociacionesArchivos.Marcado(FALSE);
	BotonListas.Marcado(FALSE);
	BotonVideo.Marcado(FALSE);
	switch (Id) {
		case ID_BOTON_BASEDATOS:
			MarcoBaseDeDatos.Visible(TRUE);
			BotonBaseDeDatos.Marcado(TRUE);
			break;
		case ID_BOTON_GENERAL:
			MarcoGeneral.Visible(TRUE);
			BotonGeneral.Marcado(TRUE);
			break;
/*		case ID_BOTON_ASOCIACIONESARCHIVOS:
			MarcoAsociacionesArchivos.Visible(TRUE);
			BotonAsociacionesArchivos.Marcado(TRUE);
			break;*/
		case ID_BOTON_LISTAS:
			MarcoListas.Visible(TRUE);
			BotonListas.Marcado(TRUE);
			break;
		case ID_BOTON_VIDEO:
			MarcoVideo.Visible(TRUE);
			BotonVideo.Marcado(TRUE);
			break;
	}
}


void VentanaOpcionesRAVE::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BOTON_AGREGARRAIZ:
			AgregarRaiz();
			break;
		case ID_BOTON_BASEDATOS:
		case ID_BOTON_GENERAL:
		case ID_BOTON_ASOCIACIONESARCHIVOS:
		case ID_BOTON_LISTAS:
		case ID_BOTON_VIDEO:
			AsignarMarco(DatosMouse.ID());
			break;
		case ID_BOTON_CERRAR :
			Destruir();
			break;
	}
}


void VentanaOpcionesRAVE::AgregarRaiz(void) {
	DDlgDirectorios          DialogoDirectorios;
	std::wstring             Path;

	App.VentanaRave.ThreadActualizar.Cancelar(TRUE);
	App.VentanaRave.ThreadAnalizar.Cancelar(TRUE);

	BOOL Ret = DialogoDirectorios.Mostrar(this, Path);
	//	SetFocus(_hWnd);
	if (Ret == TRUE) {
		// Agrego la ra�z a la BD.
		//		BDRaiz *nRaiz = NULL;
		int AR = App.BD.AgregarRaiz(Path);
		// Puede que esa ra�z sea parte de otra ra�z existente o viceversa, en ese caso no se agrega una nueva ra�z a la lista, habr� que modificar la lista
		if (AR == 1) {
//			ListaRaiz.AgregarRaiz(Path.c_str());
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz Nueva ra�z agregada.\n");
			App.MostrarToolTipOpciones(L"Nueva raiz a�adida.");
		}
		else if (AR == 0) { // La raiz ya existia o es un error
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz La ra�z ya existia.\n");
			App.MostrarToolTipOpciones(L"La ra�z especificada ya existe.");
		}
		else {
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz La ra�z se ha fusionado a una ra�z existente.\n");
			App.MostrarToolTipOpciones(L"Se han fusionado una o mas raices con la nueva ra�z.");
		}
		// Espero a que se termine el thread del analisis
		while (App.VentanaRave.ThreadActualizar.Thread() != NULL) {
			App.Eventos_Mirar();
		}

		App.VentanaRave.ActualizarArbol();
	}
	else {
		Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz Cancelado por el usuario.\n");
	}

	ListaRaiz.EliminarTodosLosItems();
	for (size_t i = 0; i < App.BD.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.BD.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Repintar();
}

void VentanaOpcionesRAVE::EliminarRaiz(std::wstring &Path) {
	App.BD.EliminarRaiz(Path);
}

void VentanaOpcionesRAVE::Evento_Cerrar(void) {
//	ListaRaiz.Destruir();
	Destruir();
	App.OcultarToolTipOpciones();
}

void VentanaOpcionesRAVE::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_MOSTRARANALISIS :
			App.BD.Opciones_MostrarObtenerMetadatos(MarcaMostrarAnalisis.Marcado());
			if (MarcaMostrarAnalisis.Marcado() == TRUE)	App.MostrarToolTipOpciones(L"La ventana del an�lisis no se mostrar� m�s.");
			else                                        App.MostrarToolTipOpciones(L"La ventana del an�lisis se mostrar� siempre.");
			break;
		case ID_MARCA_ANALIZARPENDIENTES :
			App.BD.Opciones_AnalizarMediosPendientes(MarcaAnalizarMediosPendientes.Marcado());
			if (MarcaAnalizarMediosPendientes.Marcado() == TRUE)
				App.MostrarToolTipOpciones(L"Los medios pendientes se analizar�n autom�ticamente.");
			else
				App.MostrarToolTipOpciones(L"Los medios pendientes no se analizar�n automaticamente.\n"
										   L"Para analizar los medios pendientes, haz click derecho encima de la base de datos, y pulsa Analizar.");
			break;
		case ID_MARCA_BUSCARACTUALIZACIONES :
			App.BD.Opciones_BuscarActualizacion(MarcaBuscarActualizaciones.Marcado());
			if (MarcaBuscarActualizaciones.Marcado() == TRUE)
				App.MostrarToolTipOpciones(L"Se buscar�n actualizaciones autom�ticamente.");
			else 
				App.MostrarToolTipOpciones(L"No se buscar�n actualizaciones autom�ticamente.");
			break;
	}
}


void VentanaOpcionesRAVE::Evento_BarraEx_Cambio(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BARRA_TIEMPOANIMACION :
			EtiquetaTiempoAnimacionesTA.Texto(DWL::Strings::ToStrF(BarraTiempoAnimaciones.Valor(), 0) + L" ms");
			break;
		case ID_BARRA_TIEMPOTOOLTIP :
			EtiquetaTiempoOcultarToolTipsTA.Texto(DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" ms");
			break;
	}
}

void VentanaOpcionesRAVE::Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BARRA_TIEMPOANIMACION :
			EtiquetaTiempoAnimacionesTA.Texto(DWL::Strings::ToStrF(BarraTiempoAnimaciones.Valor(), 0) + L" ms");
			App.BD.Opciones_TiempoAnimaciones(static_cast<UINT>(BarraTiempoAnimaciones.Valor()));
			DhWnd::TiempoAnimaciones = static_cast<DWORD>(BarraTiempoAnimaciones.Valor());			
			App.MostrarToolTipOpciones(L"La duraci�n de las animaciones se ha ajustado a " + DWL::Strings::ToStrF(BarraTiempoAnimaciones.Valor(), 0) + L" milisegundos");
			break;
		case ID_BARRA_TIEMPOTOOLTIP:
			EtiquetaTiempoOcultarToolTipsTA.Texto(DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" ms");
			App.BD.Opciones_TiempoToolTips(static_cast<UINT>(BarraTiempoToolTips.Valor()));
			App.MostrarToolTipOpciones(L"La duraci�n de los tooltips se han ajustado a " + DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" milisegundos");
			break;
	}
}

LRESULT CALLBACK VentanaOpcionesRAVE::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE :
			Evento_Cerrar();
			return 0;
		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_MARCAEX_CLICK:
			Evento_MarcaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case WM_EXITSIZEMOVE:
			App.BD.Opciones_GuardarPosVentanaOpciones();
			return 0;
		case WM_MOVING :
			App.OcultarToolTipOpciones();
			return 0;
			// Barra de desplazamiento (barra de tiempo y volumen)
		case DWL_BARRAEX_CAMBIO:	// Se est� modificando (mouse down)
			Evento_BarraEx_Cambio(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}
