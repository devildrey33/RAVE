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
#define ID_ETIQUETA_LISTAINICIO			1068
#define ID_DESPLEGABLE_LISTAINICIO		1069
#define ID_ETIQUETA_CANCIONES25			1070
#define ID_MARCA_CANCIONES25			1071
#define ID_ETIQUETA_NOGENERARLISTAS3	1072
#define ID_MARCA_NOGENERARLISTAS3		1073
#define ID_ETIQUETA_SUMAR005			1074
#define ID_MARCA_SUMAR005				1075
// Video
#define ID_ETIQUETA_VIDEOTIEMPO			1076
#define ID_BARRA_VIDEOTIEMPO			1077
#define ID_ETIQUETA_VIDEOOPACIDAD		1078
#define ID_BARRA_VIDEOOPACIDAD          1079
#define ID_ETIQUETA_VIDEOPOSICION		1080
#define ID_DESPLEGABLE_VIDEOPOSICION    1081
#define ID_ETIQUETA_VIDEOTIEMPOTA       1082
#define ID_ETIQUETA_VIDEOOPACIDADTA		1083

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
	SeparadorBD.Crear(&MarcoBaseDeDatos, 0, 254, RC.right - 10);
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
	for (int i = 0; i < static_cast<int>(App.TeclasRapidas.size()); i++) {
		EtiquetasTeclas[i].CrearEtiquetaEx(&MarcoGeneral, Textos[i], 10, 80 + (i * 25), 160, 20, ID_ETIQUETAS_TECLADO + i);
		EtiquetasTeclas[i].Fuente = Fuente18Normal;
		TeclasRapidas[i].Crear(&MarcoGeneral, 180, 80 + (i * 25), RC.right - 210, 20, ID_ASINGAR_TECLA, &App.TeclasRapidas[i]);
	}

	// Separador
	SeparadorGeneral.Crear(&MarcoGeneral, 0, 240, RC.right - 10);
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
//	BarraTiempoAnimaciones.MostrarValor(DBarraEx_MostrarValor_ValorInt);
	// Etiquetas de tiempo para ocultar el tooltip
	EtiquetaTiempoOcultarToolTips.CrearEtiquetaEx(&MarcoGeneral, L"Duraci�n de los tooltips de la parte inferior derecha", 10, 315, 330, 20, ID_ETIQUETA_TIEMPOTOOLTIP);
	TmpStr = std::to_wstring(App.BD.Opciones_TiempoToolTips()) + L" ms";
	EtiquetaTiempoOcultarToolTipsTA.CrearEtiquetaEx(&MarcoGeneral, TmpStr.c_str(), RC.right - 100, 315, 70, 20, ID_ETIQUETA_TIEMPOTOOLTIP2, DEtiquetaEx_Alineacion_Derecha);
	// Barra para asignar el tiempo de ocultaci�n de los tooltips
	BarraTiempoToolTips.CrearBarraDesplazamientoEx(&MarcoGeneral, 350, 315, RC.right - 450, 20, ID_BARRA_TIEMPOTOOLTIP, 1000, 10000, static_cast<float>(App.BD.Opciones_TiempoToolTips()));
//	BarraTiempoToolTips.MostrarValor(DBarraEx_MostrarValor_ValorInt);

	///////////////////////////////////////
	// Creo los controles del marco Listas
	EtiquetaCanciones25.CrearEtiquetaEx(&MarcoListas, L"Por defecto todos los medios tienen una nota de 2.5 sobre 5, si alguna canci�n\n" 
													  L"tiene una nota inferior a 2.5, el reproductor considerar� el medio como no apto."	, 10, 10, RC.right - 20, 40, ID_ETIQUETA_CANCIONES25);		
	MarcaCanciones25.CrearMarcaEx(&MarcoListas, L"No a�adir canciones con menos de 2.5 de nota a listas aleat�rias"							, 10, 60, 448, 20, ID_MARCA_CANCIONES25, IDI_CHECK2);
	MarcaCanciones25.Marcado(App.BD.Opciones_NoAgregarMedioMenos25());

	SeparadorLista1.Crear(&MarcoListas, 0, 90, RC.right - 10);
	EtiquetaNoGenerarListas3.CrearEtiquetaEx(&MarcoListas,	L"Al generar una lista aleat�ria partiendo de una etiqueta, puede darse el caso que\n"
															L"esa etiqueta tenga menos de 5 canciones."									, 10, 100, RC.right - 20, 40, ID_ETIQUETA_NOGENERARLISTAS3);
	MarcaNoGenerarListas3.CrearMarcaEx(&MarcoListas, L"No generar listas aleat�rias con menos de 5 medios"								, 10, 150, 361, 20, ID_MARCA_NOGENERARLISTAS3, IDI_CHECK2);
	MarcaNoGenerarListas3.Marcado(App.BD.Opciones_NoGenerarListasMenos3());

	SeparadorLista2.Crear(&MarcoListas, 0, 180, RC.right - 10);
	EtiquetaSumar005.CrearEtiquetaEx(&MarcoListas,	L"Una vez el medio llegue al final se sumar� 0.05 a su nota, esto quiere decir que\n"
													L"partiendo de una nota de 2.5, el medio alcanzar� las 5 estrellas en 50 veces. "	, 10, 190, RC.right - 20, 40, ID_ETIQUETA_SUMAR005);
	MarcaSumar005.CrearMarcaEx(&MarcoListas, L"Sumar 0.05 a la nota de los medios reproducidos hasta el final"							, 10, 240, 449, 20, ID_MARCA_SUMAR005, IDI_CHECK2);
	MarcaSumar005.Marcado(App.BD.Opciones_Sumar005());

	SeparadorLista3.Crear(&MarcoListas, 0, 270, RC.right - 10);
	EtiquetaListaInicio.CrearEtiquetaEx(&MarcoListas, L"Selecciona que lista debe cargar el reproductor al iniciar"	, 10, 280, RC.right - 205, 20, ID_ETIQUETA_LISTAINICIO);
	DesplegableListaInicio.CrearListaDesplegable(&MarcoListas, L"", ID_DESPLEGABLE_LISTAINICIO, 0					, RC.right - 205, 280, 185, 20, FALSE, 160);
	ActualizarListaInicio();


	///////////////////////////////////////
	// Creo los controles del marco Video
	EtiquetaVideoTiempo.CrearEtiquetaEx(&MarcoVideo, L"Tiempo para ocultar los controles (pantalla completa)", 10, 10, 350, 20, ID_ETIQUETA_VIDEOTIEMPO);
	BarraVideoTiempo.CrearBarraDesplazamientoEx(&MarcoVideo, 385, 10, 100, 20, ID_BARRA_VIDEOTIEMPO, 500, 5000, static_cast<float>(App.BD.Opciones_OcultarMouseEnVideo()));
	TmpStr = std::to_wstring(App.BD.Opciones_OcultarMouseEnVideo()) + L" ms";
	EtiquetaVideoTiempoTA.CrearEtiquetaEx(&MarcoVideo, TmpStr.c_str(), 485, 10, 60, 20, ID_ETIQUETA_VIDEOTIEMPOTA, DEtiquetaEx_Alineacion_Derecha);
	SeparadorVideo1.Crear(&MarcoVideo, 0, 40, RC.right - 10);
	EtiquetaVideoOpacidad.CrearEtiquetaEx(&MarcoVideo, L"Opacidad m�xima para los controles (pantalla completa)", 10, 50, 370, 20, ID_ETIQUETA_VIDEOOPACIDAD);
	BarraVideoOpacidad.CrearBarraDesplazamientoEx(&MarcoVideo, 385, 50, 100, 20, ID_BARRA_VIDEOOPACIDAD, 50, 255, static_cast<float>(App.BD.Opciones_OpacidadControlesVideo()));
	TmpStr = std::to_wstring(App.BD.Opciones_OpacidadControlesVideo());
	EtiquetaVideoOpacidadTA.CrearEtiquetaEx(&MarcoVideo, TmpStr.c_str(), 485, 50, 60, 20, ID_ETIQUETA_VIDEOOPACIDADTA, DEtiquetaEx_Alineacion_Derecha);
	SeparadorVideo2.Crear(&MarcoVideo, 0, 80, RC.right - 10);
	EtiquetaVideoPosicion.CrearEtiquetaEx(&MarcoVideo, L"Posici�n donde se mostrar�n los controles (pantalla completa)", 10, 90, 410, 20, ID_ETIQUETA_VIDEOPOSICION);
	
	const wchar_t *PosVideo[4] = {
		L"Abajo",
		L"Izquierda",
		L"Arriba",
		L"Derecha"
	};
	DesplegableVideoPosicion.CrearListaDesplegable(&MarcoVideo, PosVideo[App.BD.Opciones_AlineacionControlesVideo()], ID_DESPLEGABLE_VIDEOPOSICION, NULL, 425, 90, 120, 20, FALSE, 100);
	for (int i = 0; i < 4; i++) {
		DesplegableVideoPosicion.AgregarItem(PosVideo[i]);
	}
	//	SeparadorVideo3;



	// Muestro la ventana de las opciones
	Visible(TRUE);
	Repintar();
}


void VentanaOpcionesRAVE::ActualizarListaInicio(void) {
	const wchar_t *Listas[7] = {
		L"Nada",					// 0
		L"Genero aleat�rio",
		L"Grupo aleat�rio",
		L"Disco aleat�rio",
		L"50 Canciones aleat�rias",	// 4
		L"Lo que sea",
		L"Ultima lista"				// 6
	};
	DesplegableListaInicio.Texto(Listas[static_cast<int>(App.BD.Opciones_Inicio())]);
	DesplegableListaInicio.EliminarTodosLosItems();
	// Si hay raices a�ado todas las opciones
	if (App.BD.TotalRaices() > 0) {
		for (int i = 0; i < 7; i++) {
			DesplegableListaInicio.AgregarItem(Listas[i]);
		}
	}
	// Si no hay raices solo valen 3 opciones
	else {
		DesplegableListaInicio.AgregarItem(Listas[0]);
		DesplegableListaInicio.AgregarItem(Listas[4]);
		DesplegableListaInicio.AgregarItem(Listas[6]);
	}
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

		ActualizarListaInicio();
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
		case ID_BARRA_VIDEOTIEMPO :
			EtiquetaVideoTiempoTA.Texto(DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" ms");
			break;
		case ID_BARRA_VIDEOOPACIDAD :
			EtiquetaVideoOpacidadTA.Texto(DWL::Strings::ToStrF(BarraVideoOpacidad.Valor(), 0));
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
		case ID_BARRA_VIDEOTIEMPO:
			EtiquetaVideoTiempoTA.Texto(DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" ms");
			App.BD.Opciones_OcultarMouseEnVideo(static_cast<int>(BarraVideoTiempo.Valor()));
			App.MostrarToolTipOpciones(L"Los controles del video se ocultaran tras " + DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" milisegundos de inactividad");
			break;
		case ID_BARRA_VIDEOOPACIDAD:
			EtiquetaVideoOpacidadTA.Texto(DWL::Strings::ToStrF(BarraVideoOpacidad.Valor(), 0));
			App.BD.Opciones_OpacidadControlesVideo(static_cast<int>(BarraVideoOpacidad.Valor()));
			App.MostrarToolTipOpciones(L"La opacidad m�xima de los controles del video se ha ajustado a " + DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0));
			break;
	}
}

void VentanaOpcionesRAVE::Evento_ListaDesplegable_Cambio(INT_PTR nID) {
	if (nID == ID_DESPLEGABLE_LISTAINICIO) {
		std::wstring Opcion = DesplegableListaInicio.Texto();
		if (Opcion == L"Nada") {
			App.MostrarToolTipOpciones(L"No se generar� ninguna lista al iniciar el reproductor");
			App.BD.Opciones_Inicio(Tipo_Inicio_NADA);
		}
		else if (Opcion == L"Genero aleat�rio") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generar� una lista aleat�ria por genero");
			App.BD.Opciones_Inicio(Tipo_Inicio_Genero);
		}
		else if (Opcion == L"Grupo aleat�rio") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generar� una lista aleat�ria por grupo");
			App.BD.Opciones_Inicio(Tipo_Inicio_Grupo);
		}
		else if (Opcion == L"Disco aleat�rio") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generar� una lista aleat�ria por disco");
			App.BD.Opciones_Inicio(Tipo_Inicio_Disco);
		}
		else if (Opcion == L"50 Canciones aleat�rias") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generar� una lista con 50 canciones aleat�rias");
			App.BD.Opciones_Inicio(Tipo_Inicio_50Medios);
		}
		else if (Opcion == L"Lo que sea") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generar� una lista aleat�ria");
			App.BD.Opciones_Inicio(Tipo_Inicio_LoQueSea);
		}
		else if (Opcion == L"Ultima lista") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se cargar� la ultima lista");
			App.BD.Opciones_Inicio(Tipo_Inicio_UltimaLista);
		}		
	}
	else if (nID == ID_DESPLEGABLE_VIDEOPOSICION) {
		std::wstring Opcion = DesplegableVideoPosicion.Texto();
		if (Opcion == L"Abajo") {
			App.BD.Opciones_AlineacionControlesVideo(0);
			App.ControlesPC.Alineacion = Abajo;
		}
		else if (Opcion == L"Izquierda") {
			App.BD.Opciones_AlineacionControlesVideo(1);
			App.ControlesPC.Alineacion = Izquierda;
		}
		else if (Opcion == L"Arriba") {
			App.BD.Opciones_AlineacionControlesVideo(2);
			App.ControlesPC.Alineacion = Arriba;
		}
		else if (Opcion == L"Derecha") {
			App.BD.Opciones_AlineacionControlesVideo(3);
			App.ControlesPC.Alineacion = Derecha;
		}
		App.MostrarToolTipOpciones(L"Los controles del video se mostrar�n " + Opcion);
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
		case DWL_DESPLEGABLEEX_CAMBIO :
			Evento_ListaDesplegable_Cambio(static_cast<INT_PTR>(wParam));
			return 0;

	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}
