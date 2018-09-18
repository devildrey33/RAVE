#include "stdafx.h"
#include "VentanaOpcionesRAVE.h"
#include "resource.h"
#include "DDlgDirectorios.h"

#define ID_BOTON_BASEDATOS				1000
#define ID_BOTON_GENERAL				1001
#define ID_BOTON_ASOCIACIONESARCHIVOS	1002
#define ID_BOTON_LISTAS					1003
//#define ID_BOTON_VIDEO					1004

#define ID_MARCO_BASEDATOS				1005
#define ID_MARCO_GENERAL				1006
#define ID_MARCO_ASOCIACIONESARCHIVOS	1007
#define ID_MARCO_LISTAS					1008
#define ID_MARCO_VIDEO					1009

#define ID_ETIQUETA_BASEDEDATOS			1010
#define ID_MARCA_MOSTRARANALISIS		1011
#define ID_MARCA_ANALIZARPENDIENTES		1012

VentanaOpcionesRAVE::VentanaOpcionesRAVE(void) {
}

VentanaOpcionesRAVE::~VentanaOpcionesRAVE(void) {
}

void VentanaOpcionesRAVE::Crear(void) {
	DVentana::CrearVentana(NULL, L"RAVE_VentanaOpciones", L"Opciones", App.BD.Opciones_VentanaOpciones_PosX(), App.BD.Opciones_VentanaOpciones_PosY(), 590, 450, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	// Creo los botones de cada pestaña
	BotonBaseDeDatos.CrearBotonEx(this, L"Base de datos", 20, 20, 115, 20, ID_BOTON_BASEDATOS);
	BotonBaseDeDatos.Marcado(TRUE);
	BotonGeneral.CrearBotonEx(this, L"General", 145, 20, 70, 20, ID_BOTON_GENERAL);
	BotonAsociacionesArchivos.CrearBotonEx(this, L"Asociaciones de archivos", 225, 20, 200, 20, ID_BOTON_ASOCIACIONESARCHIVOS);
	BotonListas.CrearBotonEx(this, L"Listas", 435, 20, 60, 20, ID_BOTON_LISTAS);
	BotonVideo.CrearBotonEx(this, L"Video", 505, 20, 50, 20, ID_BOTON_VIDEO);
	// Asigno la fuente de los botones
	BotonBaseDeDatos.Fuente				= DhWnd::Fuente18Negrita;
	BotonGeneral.Fuente					= DhWnd::Fuente18Negrita;
	BotonAsociacionesArchivos.Fuente	= DhWnd::Fuente18Negrita;
	BotonListas.Fuente					= DhWnd::Fuente18Negrita;
	BotonVideo.Fuente					= DhWnd::Fuente18Negrita;
	// Creo los marcos para los grupos de opciones
	MarcoBaseDeDatos.Crear(this, 10, 40, RC.right - 20, RC.bottom - 50, ID_MARCO_BASEDATOS);
	MarcoGeneral.Crear(this, 10, 40, RC.right - 20, RC.bottom - 50, ID_MARCO_GENERAL);
	MarcoGeneral.Visible(FALSE);
	MarcoAsociacionesArchivos.Crear(this, 10, 40, RC.right - 20, RC.bottom - 50, ID_MARCO_ASOCIACIONESARCHIVOS);
	MarcoAsociacionesArchivos.Visible(FALSE);
	MarcoListas.Crear(this, 10, 40, RC.right - 20, RC.bottom - 50, ID_MARCO_LISTAS);
	MarcoListas.Visible(FALSE);
	MarcoVideo.Crear(this, 10, 40, RC.right - 20, RC.bottom - 50, ID_MARCO_VIDEO);
	MarcoVideo.Visible(FALSE);

	//////////////////////////////////////////////////////
	// Creo los controles dentro del marco Base de datos
	const wchar_t *E1 = L"Para usuarios con una colección superior a 100 canciones, es muy recomendable\n"
						L"agregar uno o mas directorios que contengan dichas canciones para que puedan\n"
						L"ser analizadas y añadidas a la base de datos.\n\n"
						L"Con esto el reproductor de generar listas aleatórias por : Genero, Grupo, Disco.\n"
						L"Además tambien permitirá busquedas avanzadas.";
	EtiquetaBaseDeDatos1.CrearEtiquetaEx(&MarcoBaseDeDatos, E1, 10, 10, RC.right - 40, 120, ID_ETIQUETA_BASEDEDATOS);
	// Lista con las raices
	ListaRaiz.CrearListaRaiz(&MarcoBaseDeDatos, 10, 130, RC.right - 40, 80, ID_LISTARAIZ);
	for (size_t i = 0; i < App.BD.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.BD.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Visible(TRUE);
	// Boton agregar raíz
	BotonAgregarRaiz.CrearBotonEx(&MarcoBaseDeDatos, L"Agregar Raíz", ((RC.right - 120) / 2) - 10, 220, 120, 24, ID_BOTON_AGREGARRAIZ);
	BotonAgregarRaiz.Fuente = Fuente21Normal;
	// Marca Mostrar analisis en una ventana
	MarcaMostrarAnalisis.CrearMarcaEx(&MarcoBaseDeDatos, L"Mostrar ventana con el progreso del análisis de los medios.", 70, 254, RC.right - 40, 20, ID_MARCA_MOSTRARANALISIS, IDI_CHECK2);
	MarcaMostrarAnalisis.Marcado(App.BD.Opciones_MostrarObtenerMetadatos());
	// Analizar medios pendientes al actualizar la base de datos
	MarcaAnalizarMediosPendientes.CrearMarcaEx(&MarcoBaseDeDatos, L"Analizar medios pendientes al actualizar la base de datos.", 70, 284, RC.right - 40, 20, ID_MARCA_ANALIZARPENDIENTES, IDI_CHECK2);
	MarcaAnalizarMediosPendientes.Marcado(App.BD.Opciones_AnalizarMediosPendientes());


	////////////////////////////////////////////////
	// Creo los controles dentro del marco General




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
	BotonAsociacionesArchivos.Marcado(FALSE);
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
		case ID_BOTON_ASOCIACIONESARCHIVOS:
			MarcoAsociacionesArchivos.Visible(TRUE);
			BotonAsociacionesArchivos.Marcado(TRUE);
			break;
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
	switch (DatosMouse.ID) {
		case ID_BOTON_AGREGARRAIZ:
			AgregarRaiz();
			break;
		case ID_BOTON_BASEDATOS:
		case ID_BOTON_GENERAL:
		case ID_BOTON_ASOCIACIONESARCHIVOS:
		case ID_BOTON_LISTAS:
		case ID_BOTON_VIDEO:
			AsignarMarco(DatosMouse.ID);
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
		// Agrego la raíz a la BD.
		//		BDRaiz *nRaiz = NULL;
		int AR = App.BD.AgregarRaiz(Path);
		// Puede que esa raíz sea parte de otra raíz existente o viceversa, en ese caso no se agrega una nueva raíz a la lista, habrá que modificar la lista
		if (AR == 1) {
//			ListaRaiz.AgregarRaiz(Path.c_str());
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz Nueva raíz agregada.\n");
			App.MostrarToolTip2(*this, L"Nueva raiz añadida.");
		}
		else if (AR == 0) { // La raiz ya existia o es un error
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz La raíz ya existia.\n");
			App.MostrarToolTip2(*this, L"La raíz especificada ya existe.");
		}
		else {
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz La raíz se ha fusionado a una raíz existente.\n");
			App.MostrarToolTip2(*this, L"Se han fusionado una o mas raices con la nueva raíz.");
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
}

void VentanaOpcionesRAVE::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID) {
		case ID_MARCA_MOSTRARANALISIS :
			App.BD.Opciones_MostrarObtenerMetadatos(MarcaMostrarAnalisis.Marcado());
			if (MarcaMostrarAnalisis.Marcado() == TRUE)	App.MostrarToolTip2(*this, L"La ventana del análisis no se mostrará más.");
			else                                        App.MostrarToolTip2(*this, L"La ventana del análisis se mostrará siempre.");
			break;
		case ID_MARCA_ANALIZARPENDIENTES :
			App.BD.Opciones_AnalizarMediosPendientes(MarcaAnalizarMediosPendientes.Marcado());
			if (MarcaAnalizarMediosPendientes.Marcado() == TRUE)
				App.MostrarToolTip2(*this,	L"Los medios pendientes se analizarán automaticamente.");
			else
				App.MostrarToolTip2(*this,	L"Los medios pendientes no se analizarán automaticamente.\n"
											L"Para analizar los medios pendientes, haz click derecho encima de la base de datos, y pulsa Analizar.");
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
			App.OcultarToolTip2();
			return 0;

	}
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}
