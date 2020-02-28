#include "stdafx.h"
#include "VentanaOpcionesRAVE (DEPRECATED).h"
#include "resource.h"
#include "DDlgDirectorios.h"
#include "DStringUtils.h"

#define ID_BOTON_CERRAR                  999
#define ID_BOTON_BASEDATOS				1000
#define ID_BOTON_GENERAL				1001
#define ID_BOTON_TECLADO				1002
#define ID_BOTON_LISTAS					1003
#define ID_BOTON_LISTAS2				1004
#define ID_BOTON_VIDEO2					1005
// Botones del tab superior
#define ID_MARCO_BASEDATOS				1006
#define ID_MARCO_GENERAL				1007
#define ID_MARCO_ASOCIACIONESARCHIVOS	1008
#define ID_MARCO_LISTAS					1009
#define ID_MARCO_LISTAS2				1010
#define ID_MARCO_VIDEO					1011
// Base de datos
#define ID_ETIQUETA_BASEDEDATOS			1012
#define ID_MARCA_MOSTRARANALISIS		1013
#define ID_MARCA_ANALIZARPENDIENTES		1014
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
#define ID_MARCA_OCULTARTOOLTIPS        1099 
#define ID_MARCA_MOSTRARMEDIOTITULO     1098
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
#define ID_MARCA_VIDEOGUARDARVALORES    1084

// Lista2
#define ID_ETIQUETA_LISTAMEZCLAR		1100
#define ID_ETIQUETA_LISTAGENERO         1101
#define ID_ETIQUETA_LISTAGRUPO          1102
#define ID_ETIQUETA_LISTADISCO          1103
#define ID_ETIQUETA_LISTA50CAN          1104
#define ID_ETIQUETA_LISTANOTA           1105
#define ID_DESPLEGABLE_LISTAGENERO		1106
#define ID_DESPLEGABLE_LISTAGRUPO		1107
#define ID_DESPLEGABLE_LISTADISCO		1108
#define ID_DESPLEGABLE_LISTA50CAN		1109
#define ID_DESPLEGABLE_LISTANOTA		1110

VentanaOpcionesRAVE::VentanaOpcionesRAVE(void) {
}

VentanaOpcionesRAVE::~VentanaOpcionesRAVE(void) {
}

void VentanaOpcionesRAVE::Crear(void) {
	DVentana::CrearVentana(NULL, L"RAVE_VentanaOpciones", L"Opciones", App.Opciones.VentanaOpciones_PosX(), App.Opciones.VentanaOpciones_PosY(), 590, 500, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	// Creo los botones de cada pestaña
	BotonBaseDeDatos.CrearBotonEx(this, L"Base de datos", 20, 20, 135, 30, ID_BOTON_BASEDATOS);
	BotonBaseDeDatos.Marcado(TRUE);
	BotonGeneral.CrearBotonEx(this, L"General", 155, 20, 85, 30, ID_BOTON_GENERAL);
	BotonTeclado.CrearBotonEx(this, L"Teclas", 240, 20, 75, 30, ID_BOTON_TECLADO);
	BotonListas.CrearBotonEx(this, L"Listas", 315, 20, 75, 30, ID_BOTON_LISTAS);
	BotonListas2.CrearBotonEx(this, L"Listas2", 390, 20, 80, 30, ID_BOTON_LISTAS2);
	BotonVideo.CrearBotonEx(this, L"Video", 470, 20, 70, 30, ID_BOTON_VIDEO2);
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
	MarcoTeclado.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_ASOCIACIONESARCHIVOS);
	MarcoTeclado.Visible(FALSE);
	MarcoListas.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_LISTAS);
	MarcoListas.Visible(FALSE);
	MarcoListas2.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_LISTAS2);
	MarcoListas2.Visible(FALSE);
	MarcoVideo.Crear(this, 10, 50, RC.right - 20, RC.bottom - 100, ID_MARCO_VIDEO);
	MarcoVideo.Visible(FALSE);

	BotonCerrar.CrearBotonEx(this, L"Cerrar", (RC.right - 70) / 2, RC.bottom - 40, 70, 30, ID_BOTON_CERRAR);

	//////////////////////////////////////////////////////
	// Creo los controles dentro del marco Base de datos
	//////////////////////////////////////////////////////
	const wchar_t *E1 = L"Para usuarios con una colección superior a 100 canciones, es muy recomendable\n"
						L"agregar uno o mas directorios que contengan dichas canciones para que puedan\n"
						L"ser analizadas y añadidas a la base de datos.\n\n"
						L"Una vez analizadas se podrá generar listas aleatórias por : genero, grupo y disco.\n"
						L"Además tambien permitirá busquedas avanzadas por etiqueta.";
	EtiquetaBaseDeDatos1.CrearEtiquetaEx(&MarcoBaseDeDatos, E1, 10, 10, RC.right - 40, 120, ID_ETIQUETA_BASEDEDATOS);
	// Lista con las raices
	ListaRaiz.CrearListaRaiz(&MarcoBaseDeDatos, 10, 140, RC.right - 40, 80, ID_LISTARAIZ);
	for (size_t i = 0; i < App.Opciones.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.Opciones.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Visible(TRUE);
	ListaRaiz.MultiSeleccion = FALSE;

	int x = ((RC.right - 250) / 2) - 10;
	// Botón agregar raíz
	BotonAgregarRaiz.CrearBotonEx(&MarcoBaseDeDatos, L"Agregar Raíz", x, 230, 120, 24, ID_BOTON_AGREGARRAIZ);
	BotonAgregarRaiz.Fuente.CrearFuente(21, BotonAgregarRaiz.Skin.FuenteNombre.c_str());
	// Botón eliminar raíz
	BotonEliminarRaiz.CrearBotonEx(&MarcoBaseDeDatos, L"Eliminar Raiz", x + 130, 230, 120, 24, ID_BOTON_ELIMINARRAIZ);
	BotonEliminarRaiz.Fuente.CrearFuente(21, BotonEliminarRaiz.Skin.FuenteNombre.c_str());
	BotonEliminarRaiz.Activado(FALSE);

	// Separador
	SeparadorBD.Crear(&MarcoBaseDeDatos, 0, 264, RC.right - 10);
	// Marca Mostrar analisis en una ventana
	MarcaMostrarAnalisis.CrearMarcaEx(&MarcoBaseDeDatos, L"Mostrar ventana con el progreso del análisis de los medios.", 70, 279, 405, 20, ID_MARCA_MOSTRARANALISIS, IDI_CHECK2);
	MarcaMostrarAnalisis.Marcado(App.Opciones.MostrarObtenerMetadatos());
	// Analizar medios pendientes al actualizar la base de datos
	MarcaAnalizarMediosPendientes.CrearMarcaEx(&MarcoBaseDeDatos, L"Analizar medios pendientes al actualizar la base de datos.", 70, 309, 395, 20, ID_MARCA_ANALIZARPENDIENTES, IDI_CHECK2);
	MarcaAnalizarMediosPendientes.Marcado(App.Opciones.AnalizarMediosPendientes());

	////////////////////////////////////////////////
	// Creo los controles dentro del marco Teclado
	///////////////////////////////////////////////
	EtiquetaTeclasRapidas.CrearEtiquetaEx(&MarcoTeclado, L"Las teclas rápidas solo se pueden utilizar cuando el reproductor tiene el foco.\n"
														 L"Es muy recomendable configurar las teclas rápidas con una combinación de teclas\n"
														 L"Por ejemplo : Control + tecla, Alt + Shift + Tecla, Control + Alt + tecla, etc..", 10, 10, RC.right - 40, 60, ID_ETIQUETA_TECLASRAPIDAS);
	const wchar_t *Textos[9] = {
		L"Play / Pausa",
		L"Generar lista aleatória",
		L"Subir volumen",
		L"Bajar volumen",
		L"Reproducir siguiente",
		L"Reporducir anterior",
		L"Mostrar informacion del medio",
		L"Mostrar el medio en la BD",
		L"Mostrar el medio en la lista"
	};

	for (int i = 0; i < static_cast<int>(App.TeclasRapidas.size()); i++) {
		EtiquetasTeclas[i].CrearEtiquetaEx(&MarcoTeclado, Textos[i], 10, 80 + (i * 25), 200, 20, ID_ETIQUETAS_TECLADO + i);
//		EtiquetasTeclas[i].Fuente = Fuente18Normal;
		TeclasRapidas[i].Crear(&MarcoTeclado, 220, 80 + (i * 25), RC.right - 250, 20, ID_ASINGAR_TECLA, &App.TeclasRapidas[i]);
	}

	////////////////////////////////////////////////
	// Creo los controles dentro del marco General
	////////////////////////////////////////////////
	MarcaBuscarActualizaciones.CrearMarcaEx(&MarcoGeneral, L"Buscar y descargar actualizaciones al iniciar el reproductor", 10, 10, 420, 20, ID_MARCA_BUSCARACTUALIZACIONES, IDI_CHECK2);
	MarcaBuscarActualizaciones.Marcado(App.Opciones.BuscarActualizacion());

	// Separador 
	SeparadorGeneral.Crear(&MarcoGeneral, 0, 39, RC.right - 10);

	// Etiquetas Tiempo de la animación
	EtiquetaTiempoAnimaciones.CrearEtiquetaEx(&MarcoGeneral, L"Duración de las animaciones de los controles", 10, 55, 300, 20, ID_ETIQUETA_TIEMPOANIMACIONES);
	std::wstring TmpStr = std::to_wstring(App.Opciones.TiempoAnimaciones()) + L" ms";
	EtiquetaTiempoAnimacionesTA.CrearEtiquetaEx(&MarcoGeneral, TmpStr.c_str(), RC.right - 90, 55, 60, 20, ID_ETIQUETA_TIEMPOANIMACIONES2, DEtiquetaEx_Alineacion_Derecha);
	// Barra para asignar el tiempo de las animaciones
	BarraTiempoAnimaciones.CrearBarraDesplazamientoEx(&MarcoGeneral, 350, 55, RC.right - 450, 20, ID_BARRA_TIEMPOANIMACION, 100, 1000, static_cast<float>(App.Opciones.TiempoAnimaciones()));
//	BarraTiempoAnimaciones.MostrarValor(DBarraEx_MostrarValor_ValorInt);
	// Etiquetas de tiempo para ocultar el tooltip
	EtiquetaTiempoOcultarToolTips.CrearEtiquetaEx(&MarcoGeneral, L"Duración de los tooltips de la parte inferior derecha", 10, 85, 340, 20, ID_ETIQUETA_TIEMPOTOOLTIP);
	TmpStr = std::to_wstring(App.Opciones.TiempoToolTips()) + L" ms";
	EtiquetaTiempoOcultarToolTipsTA.CrearEtiquetaEx(&MarcoGeneral, TmpStr.c_str(), RC.right - 100, 85, 70, 20, ID_ETIQUETA_TIEMPOTOOLTIP2, DEtiquetaEx_Alineacion_Derecha);
	// Barra para asignar el tiempo de ocultación de los tooltips
	BarraTiempoToolTips.CrearBarraDesplazamientoEx(&MarcoGeneral, 350, 85, RC.right - 450, 20, ID_BARRA_TIEMPOTOOLTIP, 1000, 10000, static_cast<float>(App.Opciones.TiempoToolTips()));
	// Marca para ocultar los tooltips al pasar con el mouse
	MarcaOcultarTooltipsMouse.CrearMarcaEx(&MarcoGeneral, L"Ocultar tooltips al pasar por encima con el mouse", 10, 105, 350, 30, ID_MARCA_OCULTARTOOLTIPS, IDI_CHECK2);
	MarcaOcultarTooltipsMouse.Marcado(App.Opciones.OcultarTooltipsMouse());
	// Marca para mostrar el nombre del medio actual en el titulo del reproductor
	MarcaMostrarNombreTitulo.CrearMarcaEx(&MarcoGeneral, L"Mostrar el nombre del medio actual en el titulo del reproductor", 10, 135, 435, 30, ID_MARCA_MOSTRARMEDIOTITULO, IDI_CHECK2);
	MarcaMostrarNombreTitulo.Marcado(App.Opciones.MostrarMedioActualTitulo());

	///////////////////////////////////////
	// Creo los controles del marco Listas
	///////////////////////////////////////
	EtiquetaCanciones25.CrearEtiquetaEx(&MarcoListas, L"Por defecto todos los medios tienen una nota de 2.5 sobre 5, si alguna canción\n"
													  L"tiene una nota inferior a 2.5, el reproductor considerará el medio como no apto."	, 10, 10, RC.right - 20, 40, ID_ETIQUETA_CANCIONES25);		
	MarcaCanciones25.CrearMarcaEx(&MarcoListas, L"No añadir canciones con menos de 2.5 de nota a listas aleatórias"							, 10, 60, 448, 20, ID_MARCA_CANCIONES25, IDI_CHECK2);
	MarcaCanciones25.Marcado(App.Opciones.NoAgregarMedioMenos25());

	SeparadorLista1.Crear(&MarcoListas, 0, 90, RC.right - 10);
	EtiquetaNoGenerarListas3.CrearEtiquetaEx(&MarcoListas,	L"Al generar una lista aleatória partiendo de una etiqueta, puede darse el caso que\n"
															L"esa etiqueta tenga menos de 5 canciones."									, 10, 100, RC.right - 20, 40, ID_ETIQUETA_NOGENERARLISTAS3);
	MarcaNoGenerarListas3.CrearMarcaEx(&MarcoListas, L"No generar listas aleatórias con menos de 5 medios"								, 10, 150, 361, 20, ID_MARCA_NOGENERARLISTAS3, IDI_CHECK2);
	MarcaNoGenerarListas3.Marcado(App.Opciones.NoGenerarListasMenos3());

	SeparadorLista2.Crear(&MarcoListas, 0, 180, RC.right - 10);
	EtiquetaSumar005.CrearEtiquetaEx(&MarcoListas,	L"Una vez el medio llegue al final se sumará 0.05 a su nota, esto quiere decir que\n"
													L"partiendo de una nota de 2.5, el medio alcanzará las 5 estrellas en 50 veces. "	, 10, 190, RC.right - 20, 40, ID_ETIQUETA_SUMAR005);
	MarcaSumar005.CrearMarcaEx(&MarcoListas, L"Sumar 0.05 a la nota de los medios reproducidos hasta el final"							, 10, 240, 449, 20, ID_MARCA_SUMAR005, IDI_CHECK2);
	MarcaSumar005.Marcado(App.Opciones.Sumar005());

	SeparadorLista3.Crear(&MarcoListas, 0, 270, RC.right - 10);
	EtiquetaListaInicio.CrearEtiquetaEx(&MarcoListas, L"Que tipo de lista se debe generar al iniciar?"				, 10, 280, RC.right - 260, 20, ID_ETIQUETA_LISTAINICIO);
	DesplegableListaInicio.CrearListaDesplegable(&MarcoListas, L"", ID_DESPLEGABLE_LISTAINICIO, 0					, RC.right - 215, 280, 185, 20, DEdicionTextoEx_Entrada_SinEntrada, 300);
	ActualizarListaInicio();



	///////////////////////////////////////
	// Creo los controles del marco Listas2
	///////////////////////////////////////
	EtiquetaListaMezclar.CrearEtiquetaEx(&MarcoListas2, L"Elige si se deben mezclar las listas generadas aleatóriamente. Si seleccionas\n"
														L"'Por defecto' solo se mezclara la lista si el botón mezclar ya estaba activado.", 10, 10, RC.right - 20, 40, ID_ETIQUETA_LISTAMEZCLAR);
	const wchar_t* Mezclar[3] = {
		L"Por defecto",
		L"Mezclar",
		L"No mezclar"
	};

	// Genero
	EtiquetaListaGenero.CrearEtiquetaEx(&MarcoListas2, L"Mezclar listas aleatórias por Genero", 10, 60, RC.right - 260, 20, ID_ETIQUETA_LISTAGENERO);
	DesplegableListaGenero.CrearListaDesplegable(&MarcoListas2, Mezclar[App.Opciones.MezclarListaGenero()], ID_DESPLEGABLE_LISTAGENERO, 0, RC.right - 240, 60, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaGenero.AgregarItem(Mezclar[i]);
	// Grupo
	EtiquetaListaGrupo.CrearEtiquetaEx(&MarcoListas2, L"Mezclar listas aleatórias por Grupo", 10, 85, RC.right - 260, 20, ID_ETIQUETA_LISTAGRUPO);
	DesplegableListaGrupo.CrearListaDesplegable(&MarcoListas2, Mezclar[App.Opciones.MezclarListaGrupo()], ID_DESPLEGABLE_LISTAGRUPO, 0, RC.right - 240, 85, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaGrupo.AgregarItem(Mezclar[i]);
	// Disco
	EtiquetaListaDisco.CrearEtiquetaEx(&MarcoListas2, L"Mezclar listas aleatórias por Disco", 10, 110, RC.right - 260, 20, ID_ETIQUETA_LISTADISCO);
	DesplegableListaDisco.CrearListaDesplegable(&MarcoListas2, Mezclar[App.Opciones.MezclarListaDisco()], ID_DESPLEGABLE_LISTADISCO, 0, RC.right - 240, 110, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaDisco.AgregarItem(Mezclar[i]);
	// 50 Can
	EtiquetaLista50Can.CrearEtiquetaEx(&MarcoListas2, L"Mezclar listas aleatórias con 50 Canciones", 10, 135, RC.right - 260, 20, ID_ETIQUETA_LISTA50CAN);
	DesplegableLista50Can.CrearListaDesplegable(&MarcoListas2, Mezclar[App.Opciones.MezclarLista50Can()], ID_DESPLEGABLE_LISTA50CAN, 0, RC.right - 240, 135, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableLista50Can.AgregarItem(Mezclar[i]);
	// Nota
	EtiquetaListaNota.CrearEtiquetaEx(&MarcoListas2, L"Mezclar listas por Nota", 10, 160, RC.right - 260, 20, ID_ETIQUETA_LISTANOTA);
	DesplegableListaNota.CrearListaDesplegable(&MarcoListas2, Mezclar[App.Opciones.MezclarListaNota()], ID_DESPLEGABLE_LISTANOTA, 0, RC.right - 240, 160, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaNota.AgregarItem(Mezclar[i]);




	///////////////////////////////////////
	// Creo los controles del marco Video
	///////////////////////////////////////
	EtiquetaVideoTiempo.CrearEtiquetaEx(&MarcoVideo, L"Tiempo para ocultar los controles (pantalla completa)", 10, 10, 350, 20, ID_ETIQUETA_VIDEOTIEMPO);
	BarraVideoTiempo.CrearBarraDesplazamientoEx(&MarcoVideo, 385, 10, 100, 20, ID_BARRA_VIDEOTIEMPO, 1000, 5000, static_cast<float>(App.Opciones.OcultarMouseEnVideo()));
	TmpStr = std::to_wstring(App.Opciones.OcultarMouseEnVideo()) + L" ms";
	EtiquetaVideoTiempoTA.CrearEtiquetaEx(&MarcoVideo, TmpStr.c_str(), 485, 10, 60, 20, ID_ETIQUETA_VIDEOTIEMPOTA, DEtiquetaEx_Alineacion_Derecha);
	SeparadorVideo1.Crear(&MarcoVideo, 0, 40, RC.right - 10);
	EtiquetaVideoOpacidad.CrearEtiquetaEx(&MarcoVideo, L"Opacidad máxima para los controles (pantalla completa)", 10, 50, 370, 20, ID_ETIQUETA_VIDEOOPACIDAD);
	BarraVideoOpacidad.CrearBarraDesplazamientoEx(&MarcoVideo, 385, 50, 100, 20, ID_BARRA_VIDEOOPACIDAD, 50, 255, static_cast<float>(App.Opciones.OpacidadControlesVideo()));
	TmpStr = std::to_wstring(App.Opciones.OpacidadControlesVideo());
	EtiquetaVideoOpacidadTA.CrearEtiquetaEx(&MarcoVideo, TmpStr.c_str(), 485, 50, 60, 20, ID_ETIQUETA_VIDEOOPACIDADTA, DEtiquetaEx_Alineacion_Derecha);
	SeparadorVideo2.Crear(&MarcoVideo, 0, 80, RC.right - 10);
	EtiquetaVideoPosicion.CrearEtiquetaEx(&MarcoVideo, L"Posición donde se mostrarán los controles (pantalla completa)", 10, 90, 410, 20, ID_ETIQUETA_VIDEOPOSICION);

	const wchar_t *PosVideo[4] = {
		L"Abajo",
		L"Izquierda",
		L"Arriba",
		L"Derecha"
	};
	DesplegableVideoPosicion.CrearListaDesplegable(&MarcoVideo, PosVideo[App.Opciones.AlineacionControlesVideo()], ID_DESPLEGABLE_VIDEOPOSICION, NULL, 425, 90, 120, 20, DEdicionTextoEx_Entrada_SinEntrada, 300);
	for (int i = 0; i < 4; i++) {
		DesplegableVideoPosicion.AgregarItem(PosVideo[i]);
	}
	
	SeparadorVideo3.Crear(&MarcoVideo, 0, 120, RC.right - 10);

	MarcaRecordarValoresVideo.CrearMarcaEx(&MarcoVideo, L"Guardar brillo, saturación, contraste, y proporción para cada medio.", 10, 130, 455, 30, ID_MARCA_VIDEOGUARDARVALORES, IDI_CHECK2);
	MarcaRecordarValoresVideo.Marcado(App.Opciones.GuardarBSCP());


	
	// Muestro la ventana de las opciones
	Visible(TRUE);
	Repintar();
}


void VentanaOpcionesRAVE::ActualizarListaInicio(void) {
	const wchar_t *Listas[8] = {
		L"Ninguna",					// 0
		L"Genero aleatório",
		L"Grupo aleatório",
		L"Disco aleatório",
		L"50 Canciones aleatórias",	// 4
		L"Por Nota",
		L"Lo que sea",
		L"Ultima lista"				// 7
	};
	DesplegableListaInicio.Texto(Listas[static_cast<int>(App.Opciones.Inicio())]);
	DesplegableListaInicio.EliminarTodosLosItems();
	// Si hay raices añado todas las opciones
	if (App.Opciones.TotalRaices() > 0) {
		for (int i = 0; i < 8; i++) {
			DesplegableListaInicio.AgregarItem(Listas[i]);
		}
	}
	// Si no hay raices solo valen 2 opciones
	else {
		DesplegableListaInicio.AgregarItem(Listas[0]);
//		DesplegableListaInicio.AgregarItem(Listas[4]);
		DesplegableListaInicio.AgregarItem(Listas[7]);
	}
}

void VentanaOpcionesRAVE::AsignarMarco(const INT_PTR Id) {
	MarcoBaseDeDatos.Visible(FALSE);
	MarcoGeneral.Visible(FALSE);
	MarcoTeclado.Visible(FALSE);
	MarcoListas.Visible(FALSE);
	MarcoListas2.Visible(FALSE);
	MarcoVideo.Visible(FALSE);
	BotonBaseDeDatos.Marcado(FALSE);
	BotonGeneral.Marcado(FALSE);
	BotonTeclado.Marcado(FALSE);
	BotonListas.Marcado(FALSE);
	BotonListas2.Marcado(FALSE);
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
		case ID_BOTON_TECLADO:
			MarcoTeclado.Visible(TRUE);
			BotonTeclado.Marcado(TRUE);
			break;
		case ID_BOTON_LISTAS:
			MarcoListas.Visible(TRUE);
			BotonListas.Marcado(TRUE);
			break;
		case ID_BOTON_LISTAS2:
			MarcoListas2.Visible(TRUE);
			BotonListas2.Marcado(TRUE);
			break;
		case ID_BOTON_VIDEO2:
			MarcoVideo.Visible(TRUE);
			BotonVideo.Marcado(TRUE);
			break;
	}
}

void VentanaOpcionesRAVE::Evento_ListaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	if (DatosMouse.ID() == ID_LISTARAIZ) {
		if (ListaRaiz.ItemMarcado() != NULL) {
			BotonEliminarRaiz.Activado(TRUE);
		}
		else {
			BotonEliminarRaiz.Activado(FALSE);
		}
	}
}

void VentanaOpcionesRAVE::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BOTON_AGREGARRAIZ:
			AgregarRaiz();
			break;
		case ID_BOTON_ELIMINARRAIZ:
			if (ListaRaiz.ItemMarcado() != NULL) {
				EliminarRaiz(ListaRaiz.ItemMarcado()->Texto(0));
			}
			break;
		case ID_BOTON_BASEDATOS:
		case ID_BOTON_GENERAL:
		case ID_BOTON_TECLADO:
		case ID_BOTON_LISTAS:
		case ID_BOTON_LISTAS2:
		case ID_BOTON_VIDEO2:
			AsignarMarco(DatosMouse.ID());
			break;
		case ID_BOTON_CERRAR :
			Destruir();
			break;
	}
}

void VentanaOpcionesRAVE::Evento_DlgDirectorios_CambioTamPos(HWND hWndDlg) {
	RECT RW, RC;
	GetWindowRect(hWndDlg, &RW);
	GetClientRect(hWndDlg, &RC);
	App.Opciones.GuardarPosTamDlgDirectorios(RW, RC);
}


void VentanaOpcionesRAVE::AgregarRaiz(void) {
	DDlgDirectorios          DialogoDirectorios;
	std::wstring             Path;

	App.VentanaRave.ThreadActualizar.Cancelar(TRUE);
	App.VentanaRave.ThreadAnalizar.Cancelar(TRUE);

	// No muestro las redes
	DialogoDirectorios.ArbolDirectorios.MostrarRed = FALSE;

	BOOL Ret = DialogoDirectorios.Mostrar(this, Path, App.Opciones.DlgDirectorios_PosX(), App.Opciones.DlgDirectorios_PosY(), App.Opciones.DlgDirectorios_Ancho(), App.Opciones.DlgDirectorios_Alto(), IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
	//	SetFocus(_hWnd);
	if (Ret == TRUE) {
		// Agrego la raíz a la BD.
		//		BDRaiz *nRaiz = NULL;
		int AR = App.Opciones.AgregarRaiz(Path, App.Unidades);
		// Puede que esa raíz sea parte de otra raíz existente o viceversa, en ese caso no se agrega una nueva raíz a la lista, habrá que modificar la lista
		if (AR == 1) {
//			ListaRaiz.AgregarRaiz(Path.c_str());
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz Nueva raíz agregada.\n");
			App.MostrarToolTipOpciones(L"Nueva raiz añadida.");
		}
		else if (AR == 0) { // La raiz ya existia o es un error
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz La raíz ya existia.\n");
			App.MostrarToolTipOpciones(L"La raíz especificada ya existe.");
		}
		else {
			Debug_Escribir(L"VentanaOpcionesRAVE::AgregarRaiz La raíz se ha fusionado a una raíz existente.\n");
			App.MostrarToolTipOpciones(L"Se han fusionado una o mas raices con la nueva raíz.");
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
	for (size_t i = 0; i < App.Opciones.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.Opciones.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Repintar();
}

void VentanaOpcionesRAVE::EliminarRaiz(std::wstring &Path) {
	App.VentanaRave.ThreadActualizar.Cancelar(TRUE);
	App.VentanaRave.ThreadAnalizar.Cancelar(TRUE);
	App.Opciones.EliminarRaiz(Path);
	ListaRaiz.EliminarItem(ListaRaiz.ItemMarcado());
	ListaRaiz.Repintar();
	ActualizarListaInicio();
	App.VentanaRave.ActualizarArbol();

	if (ListaRaiz.ItemMarcado() == NULL) {
		BotonEliminarRaiz.Activado(FALSE);
	}
}

void VentanaOpcionesRAVE::Evento_Cerrar(void) {
//	ListaRaiz.Destruir();
	Destruir();
	App.OcultarToolTipOpciones();
}

void VentanaOpcionesRAVE::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_MOSTRARANALISIS :
			App.Opciones.MostrarObtenerMetadatos(MarcaMostrarAnalisis.Marcado());
			if (MarcaMostrarAnalisis.Marcado() == TRUE)				App.MostrarToolTipOpciones(L"La ventana del análisis se mostrará siempre.");
			else													App.MostrarToolTipOpciones(L"La ventana del análisis no se mostrará más.");
			break;
		case ID_MARCA_ANALIZARPENDIENTES :
			App.Opciones.AnalizarMediosPendientes(MarcaAnalizarMediosPendientes.Marcado());
			if (MarcaAnalizarMediosPendientes.Marcado() == TRUE)	App.MostrarToolTipOpciones(L"Los medios pendientes se analizarán automáticamente.");
			else													App.MostrarToolTipOpciones(L"Los medios pendientes no se analizarán automaticamente.\n"
																							   L"Para analizar los medios pendientes, haz click derecho encima de la base de datos, y pulsa Analizar.");
			break;
		case ID_MARCA_BUSCARACTUALIZACIONES :
			App.Opciones.BuscarActualizacion(MarcaBuscarActualizaciones.Marcado());
			if (MarcaBuscarActualizaciones.Marcado() == TRUE)		App.MostrarToolTipOpciones(L"Se buscarán actualizaciones automáticamente.");
			else 													App.MostrarToolTipOpciones(L"No se buscarán actualizaciones automáticamente.");
			break;
		case ID_MARCA_OCULTARTOOLTIPS :
			App.Opciones.OcultarTooltipsMouse(MarcaOcultarTooltipsMouse.Marcado());
			if (MarcaOcultarTooltipsMouse.Marcado() == TRUE)		App.MostrarToolTipOpciones(L"Se ocultaran los tooltips al pasar por encima con el mouse.");
			else													App.MostrarToolTipOpciones(L"Los tooltips no se ocultaran al pasar por encima con el mouse.");
			break;
		case ID_MARCA_MOSTRARMEDIOTITULO :
			App.Opciones.MostrarMedioActualTitulo(MarcaMostrarNombreTitulo.Marcado());
			if (MarcaMostrarNombreTitulo.Marcado() == TRUE)			App.MostrarToolTipOpciones(L"El nombre del medio se mostrará en la barra de título.");
			else													App.MostrarToolTipOpciones(L"No se mostrará el nombre del medio en la barra de título.");
			App.MP.AsignarTitulo();
			break;
		case ID_MARCA_VIDEOGUARDARVALORES :
			App.Opciones.GuardarBSCP(MarcaRecordarValoresVideo.Marcado());
			if (MarcaRecordarValoresVideo.Marcado() == TRUE)		App.MostrarToolTipOpciones(L"Se recordaran los valores de brillo, contraste, saturación, y proporción para cada video.");
			else													App.MostrarToolTipOpciones(L"Los valores de brillo, contraste, saturación, y proporción serán generales para todos los videos.");
			break;
	}
}


void VentanaOpcionesRAVE::Evento_BarraEx_Cambiando(DWL::DEventoMouse &DatosMouse) {
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
			App.Opciones.TiempoAnimaciones(static_cast<UINT>(BarraTiempoAnimaciones.Valor()));
			DhWnd::TiempoAnimaciones = static_cast<DWORD>(BarraTiempoAnimaciones.Valor());			
			App.MostrarToolTipOpciones(L"La duración de las animaciones se ha ajustado a " + DWL::Strings::ToStrF(BarraTiempoAnimaciones.Valor(), 0) + L" milisegundos");
			break;
		case ID_BARRA_TIEMPOTOOLTIP:
			EtiquetaTiempoOcultarToolTipsTA.Texto(DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" ms");
			App.Opciones.TiempoToolTips(static_cast<UINT>(BarraTiempoToolTips.Valor()));
			App.MostrarToolTipOpciones(L"La duración de los tooltips se han ajustado a " + DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" milisegundos");
			break;
		case ID_BARRA_VIDEOTIEMPO:
			EtiquetaVideoTiempoTA.Texto(DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" ms");
			App.Opciones.OcultarMouseEnVideo(static_cast<int>(BarraVideoTiempo.Valor()));
			App.MostrarToolTipOpciones(L"Los controles del video se ocultaran tras " + DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" milisegundos de inactividad");
			break;
		case ID_BARRA_VIDEOOPACIDAD:
			EtiquetaVideoOpacidadTA.Texto(DWL::Strings::ToStrF(BarraVideoOpacidad.Valor(), 0));
			App.Opciones.OpacidadControlesVideo(static_cast<int>(BarraVideoOpacidad.Valor()));
			App.MostrarToolTipOpciones(L"La opacidad máxima de los controles del video se ha ajustado a " + DWL::Strings::ToStrF(BarraVideoOpacidad.Valor(), 0) + L" / 255");
			break;
	}
}

void VentanaOpcionesRAVE::Evento_ListaDesplegable_Cambio(INT_PTR nID) {
	// Lista de inicio
	if (nID == ID_DESPLEGABLE_LISTAINICIO) {
		std::wstring Opcion = DesplegableListaInicio.Texto();
		if (Opcion == L"Nada") {
			App.MostrarToolTipOpciones(L"No se generará ninguna lista al iniciar el reproductor");
			App.Opciones.Inicio(Tipo_Inicio_NADA);
		}
		else if (Opcion == L"Genero aleatório") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generará una lista aleatória por genero");
			App.Opciones.Inicio(Tipo_Inicio_Genero);
		}
		else if (Opcion == L"Grupo aleatório") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generará una lista aleatória por grupo");
			App.Opciones.Inicio(Tipo_Inicio_Grupo);
		}
		else if (Opcion == L"Disco aleatório") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generará una lista aleatória por disco");
			App.Opciones.Inicio(Tipo_Inicio_Disco);
		}
		else if (Opcion == L"50 Canciones aleatórias") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generará una lista con 50 canciones aleatórias");
			App.Opciones.Inicio(Tipo_Inicio_50Medios);
		}
		else if (Opcion == L"Lo que sea") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se generará una lista aleatória");
			App.Opciones.Inicio(Tipo_Inicio_LoQueSea);
		}
		else if (Opcion == L"Ultima lista") {
			App.MostrarToolTipOpciones(L"Al iniciar el reproductor se cargará la ultima lista");
			App.Opciones.Inicio(Tipo_Inicio_UltimaLista);
		}		
	}
	// Posición de la barra de controles en modo pantalla completa
	else if (nID == ID_DESPLEGABLE_VIDEOPOSICION) {
		std::wstring Opcion = DesplegableVideoPosicion.Texto();
		if (Opcion == L"Abajo") {
			App.Opciones.AlineacionControlesVideo(0);
			App.ControlesPC.Alineacion = Abajo;
		}
		else if (Opcion == L"Izquierda") {
			App.Opciones.AlineacionControlesVideo(1);
			App.ControlesPC.Alineacion = Izquierda;
		}
		else if (Opcion == L"Arriba") {
			App.Opciones.AlineacionControlesVideo(2);
			App.ControlesPC.Alineacion = Arriba;
		}
		else if (Opcion == L"Derecha") {
			App.Opciones.AlineacionControlesVideo(3);
			App.ControlesPC.Alineacion = Derecha;
		}
		App.MostrarToolTipOpciones(L"Los controles del video se mostrarán " + Opcion);
	}
	// Mezclar listas por genero
	else if (nID == ID_DESPLEGABLE_LISTAGENERO) {
		std::wstring Opcion = DesplegableListaGenero.Texto();
		if (Opcion == L"Por defecto") {
			App.MostrarToolTipOpciones(L"Las listas por genero solo se mezclaran si el botón Mezclar está marcado.");
			App.Opciones.MezclarListaGenero(0);
		}
		else if (Opcion == L"Mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por genero se mezclaran siempre.");
			App.Opciones.MezclarListaGenero(1);
		}
		else if (Opcion == L"No mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por genero no se mezclaran nunca.");
			App.Opciones.MezclarListaGenero(2);
		}
	}
	// Mezclar listas por grupo
	else if (nID == ID_DESPLEGABLE_LISTAGRUPO) {
		std::wstring Opcion = DesplegableListaGrupo.Texto();
		if (Opcion == L"Por defecto") {
			App.MostrarToolTipOpciones(L"Las listas por grupo solo se mezclaran si el botón Mezclar está marcado.");
			App.Opciones.MezclarListaGrupo(0);
		}
		else if (Opcion == L"Mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por grupo se mezclaran siempre.");
			App.Opciones.MezclarListaGrupo(1);
		}
		else if (Opcion == L"No mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por grupo no se mezclaran nunca.");
			App.Opciones.MezclarListaGrupo(2);
		}
	}
	// Mezclar listas por disco
	else if (nID == ID_DESPLEGABLE_LISTADISCO) {
		std::wstring Opcion = DesplegableListaDisco.Texto();
		if (Opcion == L"Por defecto") {
			App.MostrarToolTipOpciones(L"Las listas por disco solo se mezclaran si el botón Mezclar está marcado.");
			App.Opciones.MezclarListaDisco(0);
		}
		else if (Opcion == L"Mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por disco se mezclaran siempre.");
			App.Opciones.MezclarListaDisco(1);
		}
		else if (Opcion == L"No mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por disco no se mezclaran nunca.");
			App.Opciones.MezclarListaDisco(2);
		}
	}
	// Mezclar listas por disco
	else if (nID == ID_DESPLEGABLE_LISTA50CAN) {
		std::wstring Opcion = DesplegableLista50Can.Texto();
		if (Opcion == L"Por defecto") {
			App.MostrarToolTipOpciones(L"Las listas con 50 canciones solo se mezclaran si el botón Mezclar está marcado.");
			App.Opciones.MezclarLista50Can(0);
		}
		else if (Opcion == L"Mezclar") {
			App.MostrarToolTipOpciones(L"Las listas con 50 canciones se mezclaran siempre.");
			App.Opciones.MezclarLista50Can(1);
		}
		else if (Opcion == L"No mezclar") {
			App.MostrarToolTipOpciones(L"Las listas con 50 canciones no se mezclaran nunca.");
			App.Opciones.MezclarLista50Can(2);
		}
	}
	// Mezclar listas por nota
	else if (nID == ID_DESPLEGABLE_LISTANOTA) {
		std::wstring Opcion = DesplegableListaNota.Texto();
		if (Opcion == L"Por defecto") {
			App.MostrarToolTipOpciones(L"Las listas por nota solo se mezclaran si el botón Mezclar está marcado.");
			App.Opciones.MezclarListaNota(0);
		}
		else if (Opcion == L"Mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por nota se mezclaran siempre.");
			App.Opciones.MezclarListaNota(1);
		}
		else if (Opcion == L"No mezclar") {
			App.MostrarToolTipOpciones(L"Las listas por nota no se mezclaran nunca.");
			App.Opciones.MezclarListaNota(2);
		}
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
			App.Opciones.GuardarPosVentanaOpciones();
			return 0;
		case WM_MOVING :
			App.OcultarToolTipOpciones();
			return 0;
		// Barra de desplazamiento (barra de tiempo y volumen)
		case DWL_BARRAEX_CAMBIANDO:	// Se está modificando (mouse down)
			Evento_BarraEx_Cambiando(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_DESPLEGABLEEX_CAMBIO :
			Evento_ListaDesplegable_Cambio(static_cast<INT_PTR>(wParam));
			return 0;
		case DWL_LISTAEX_CLICK :
			Evento_ListaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		// El DlgDirectorios ha cambiado de tamaño o posición
		case DWL_DLGDIRECTORIOS_POSTAM_CAMBIADO :
			Evento_DlgDirectorios_CambioTamPos(WPARAM_TO_HWND(wParam));
			return 0;
	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}
