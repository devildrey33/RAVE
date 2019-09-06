#include "stdafx.h"
#include "VentanaPrincipal.h"
#include "Resource.h"
#include "DMensajesWnd.h"
#include "DMouse.h"
//#include "Rave_Skin.h"
#include "DDlgDirectorios.h"
#include <shellapi.h>
#include "DStringUtils.h"
#include <versionhelpers.h>
#include <Shlobj.h>
#include "DDlgAbrir.h"

#define RAVE_MIN_ANCHO 670
#define RAVE_MIN_ALTO  289
#define RAVE_BOTONES_LATERALES_ANCHO 140



// Función que crea la ventana principal del reproductor
HWND VentanaPrincipal::Crear(int nCmdShow) {
	CrearSkins();

	// Detección de monitores
	EnumDisplayMonitors(NULL, NULL, VentanaPrincipal::EnumerarPantallas, NULL);
	// Si el monitor donde se guardo la ultima posición no está disponible busco una nueva posición
	if (MonitorDisponible == FALSE) {
		App.Opciones.AsignarPosVentana(RectMonitorActual.left + 100, RectMonitorActual.top + 100);
	}

	int ancho = (App.Opciones.Ancho() > RAVE_MIN_ANCHO) ? App.Opciones.Ancho() : RAVE_MIN_ANCHO;
	HWND rhWnd = DVentana::CrearVentana(NULL, L"RAVE_VentanaPrincipal", RAVE_TITULO, App.Opciones.PosX(), App.Opciones.PosY(), ancho, App.Opciones.Alto(), WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
	
	RECT RC; // , RW;
	GetClientRect(hWnd(), &RC);

	// Creo el arbol para cargar la base de datos
	Arbol.CrearArbolEx(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_ARBOLBD, WS_CHILD | WS_VISIBLE);
	SetWindowLongPtr(Arbol.hWnd(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
//	Arbol.SubSeleccion = TRUE;

	// Creo la lista de reproducción
	Lista.CrearListaEx(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_LISTAMEDIOS, WS_CHILD);
	SetWindowLongPtr(Lista.hWnd(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
	Lista.MultiSeleccion = TRUE;
	Lista.MoverItemsDrag = TRUE;
	// Columnas
	Lista.AgregarColumna(50, DListaEx_Columna_Alineacion_Derecha);								// Icono y pista
	Lista.AgregarColumna(DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion_Izquierda);	// Nombre
	Lista.AgregarColumna(60, DListaEx_Columna_Alineacion_Derecha);								// Tiempo

	// Creo el control donde se visualizará el vídeo
	Video.Crear(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_VERVIDEO);

	// Marco superior izquierdo /////////////
	MarcoSI.Crear(this, 10, 10, 380, 30, ID_MARCOSI);
	BotonAtras.CrearBotonEx(&MarcoSI,	 IDI_PREV32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,   0, 0, 30, 30, ID_BOTON_ANTERIOR);
	BotonPlay.CrearBotonEx(&MarcoSI,	 IDI_PLAY32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  40, 0, 30, 30, ID_BOTON_PLAY);
	BotonStop.CrearBotonEx(&MarcoSI,	 IDI_STOP32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  80, 0, 30, 30, ID_BOTON_STOP);
	BotonAdelante.CrearBotonEx(&MarcoSI, IDI_NEXT32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 120, 0, 30, 30, ID_BOTON_SIGUIENTE);

	LabelRatio.CrearEtiquetaEx(&MarcoSI, L"x1.0", 160, 5, 40, 30, ID_LABEL_RATIO, DEtiquetaEx_Alineacion_Centrado);

	BotonMezclar.CrearBotonEx(&MarcoSI, L"Mezclar", 210, 0, 70, 30, ID_BOTON_MEZCLAR);
	BotonMezclar.Fuente.CrearFuente(18, BotonMezclar.Skin.FuenteNombre.c_str(), TRUE);
	if (App.Opciones.Shufle() == TRUE) BotonMezclar.Marcado(TRUE);
	BotonRepetir.CrearBotonEx(&MarcoSI, L"Repetir", 290, 0, 70, 30, ID_BOTON_REPETIR);
	BotonRepetir.Fuente.CrearFuente(18, BotonRepetir.Skin.FuenteNombre.c_str(), TRUE);
	if (App.Opciones.Repeat() > 0) BotonRepetir.Marcado(TRUE);
	//////////////////////////////////////////

	// Creo el slider para mostrar / modificar el tiempo del medio actual
	SliderTiempo.CrearBarraDesplazamientoEx(this, 10, 50, RC.right - 20, 20, ID_SLIDER_TIEMPO);

	// Marco superior derecho
	MarcoSD.Crear(this, RC.right - 260, 16, 250, 24, ID_MARCOSD);
	// Creo el slider para modificar el volumen
	SliderVolumen.CrearBarraVolumen(&MarcoSD, 120, 3, 90, 17, ID_SLIDER_VOLUMEN, 0, 200, static_cast<float>(App.Opciones.Volumen()));
	std::wstring TxtVolumen = std::to_wstring(App.Opciones.Volumen()) + L"%";
	LabelVolumen.CrearEtiquetaEx(&MarcoSD, TxtVolumen.c_str(), 215, 1, 40, 20, ID_LABEL_VOLUMEN, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);
	// Creo los labels para mostrar el tiempo actual y el total
	LabelTiempoActual.CrearEtiquetaEx(&MarcoSD, L"00:00", 0, 1, 55, 20, ID_LABEL_TIEMPOACTUAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);
	LabelTiempoSeparador.CrearEtiquetaEx(&MarcoSD, L"/", 55, 1, 10, 20, ID_LABEL_TIEMPOSEPARADOR, DEtiquetaEx_Alineacion_Centrado,  WS_CHILD | WS_VISIBLE);
	LabelTiempoTotal.CrearEtiquetaEx(&MarcoSD, L"00:00", 65, 1, 55, 20, ID_LABEL_TIEMPOTOTAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);

	// Marco inferior izquierdo /////////////
	MarcoII.Crear(this, 10, 80, RAVE_BOTONES_LATERALES_ANCHO, 2000, ID_MARCOSI);
	BotonBD.CrearBotonEx(&MarcoII, L"Base de datos", 0, 0, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_BD);
	BotonBD.Marcado(TRUE); // Por defecto siempre se muestra la base de datos al empezar
	BotonLista.CrearBotonEx(&MarcoII, L"Lista de medios", 0, 35, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_LISTA);
	BotonVideo.CrearBotonEx(&MarcoII, L"Ver video", 0, 70, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_VIDEO);
	// El Boton Opciones queda siempre en la parte inferior del marco, para que quede separado de los otros (ya que es una ventana externa)
	BotonOpciones.CrearBotonEx(&MarcoII, L"Opciones", 0, RC.bottom - 120, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_OPCIONES);
	
	#ifdef RAVE_VLC_DOBLE_MEDIO_FFT
		Vis.Crear(&MarcoII, 0, 160, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_VISUALIZACION);
	#endif
	/////////////////////////////////////////


	// Inicio la VLC justo antes de mostrar la ventana y de activar el thread para actualizar el arbol
	App.MP.Iniciar();

	// Muestro la ventana principal
	ShowWindow(hWnd(), nCmdShow);

	// Habilito el drag & drop para esta ventana
	DragAcceptFiles(hWnd(), TRUE);	

	// Busco actualizaciones
	if (App.Opciones.BuscarActualizacion() == TRUE) {
		App.Actualizacion.Buscar();
	}

	return rhWnd;
}

void VentanaPrincipal::CrearSkins(void) {
	// Colores para el fondo (OJO los colores del fondo y del borde del control están en DBarraSroll_Skin)
	ListaSkinOscuro.FondoItemNormal						= COLOR_LISTA_OSCURA_FONDO;
	ListaSkinOscuro.FondoItemResaltado					= COLOR_LISTA_OSCURA_FONDO_RESALTADO;
	ListaSkinOscuro.FondoItemSeleccionado				= COLOR_LISTA_OSCURA_SELECCION;
	ListaSkinOscuro.FondoItemSeleccionadoResaltado		= COLOR_LISTA_OSCURA_SELECCION_RESALTADO;
	ListaSkinOscuro.FondoItemPresionado					= COLOR_LISTA_OSCURA_SELECCION_PRESIONADO;
	// Color para el borde del item marcado
	ListaSkinOscuro.BordeItemMarcado					= COLOR_LISTA_OSCURA_MARCA_ITEM;
	// Colores para el texto
	ListaSkinOscuro.TextoItemNormal						= COLOR_LISTA_OSCURA_TEXTO;
	ListaSkinOscuro.TextoItemResaltado					= COLOR_LISTA_OSCURA_TEXTO_RESALTADO;
	ListaSkinOscuro.TextoItemSombra						= COLOR_LISTA_OSCURA_TEXTO_SOMBRA;
	ListaSkinOscuro.TextoItemSeleccionado				= COLOR_LISTA_OSCURA_SELECCION_TEXTO;
	ListaSkinOscuro.TextoItemSeleccionadoSombra			= COLOR_LISTA_OSCURA_SELECCION_TEXTO_SOMBRA;
	ListaSkinOscuro.TextoItemSeleccionadoResaltado		= COLOR_LISTA_OSCURA_SELECCION_TEXTO_RESALTADO;
	ListaSkinOscuro.TextoItemPresionado					= COLOR_LISTA_OSCURA_SELECCION_TEXTO_PRESIONADO;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Colores para el fondo de los nodos
	ArbolSkinOscuro.FondoNodoResaltado					= COLOR_ARBOL_OSCURO_FONDO_RESALTADO;
	ArbolSkinOscuro.FondoNodoSeleccionado				= COLOR_ARBOL_OSCURO_SELECCION;
	ArbolSkinOscuro.FondoNodoSeleccionadoResaltado		= COLOR_ARBOL_OSCURO_SELECCION_RESALTADO;
	ArbolSkinOscuro.FondoNodoSubSeleccionado			= COLOR_ARBOL_OSCURO_SUBSELECCION;
	ArbolSkinOscuro.FondoNodoSubSeleccionadoResaltado	= COLOR_ARBOL_OSCURO_SUBSELECCION_RESALTADO;
	ArbolSkinOscuro.FondoNodoPresionado					= COLOR_ARBOL_OSCURO_SELECCION_PRESIONADO;
	// Marca de selección
	ArbolSkinOscuro.BordeNodoMarcado					= COLOR_ARBOL_OSCURO_NODO_MARCA;
	// colores para el Expansor
	ArbolSkinOscuro.ExpansorNormal						= COLOR_ARBOL_OSCURO_EXPANSOR_NORMAL;
	ArbolSkinOscuro.ExpansorResaltado					= COLOR_ARBOL_OSCURO_EXPANSOR_RESALTADO;
	ArbolSkinOscuro.ExpansorPresionado					= COLOR_ARBOL_OSCURO_EXPANSOR_PRESIONADO;
	// colores para el Texto
	ArbolSkinOscuro.TextoNodoNormal						= COLOR_ARBOL_OSCURO_TEXTO;
	ArbolSkinOscuro.TextoNodoResaltado					= COLOR_ARBOL_OSCURO_TEXTO_RESALTADO;
	ArbolSkinOscuro.TextoNodoDesactivado				= COLOR_ARBOL_OSCURO_TEXTO_DESACTIVADO;
	ArbolSkinOscuro.TextoNodoSombra						= COLOR_ARBOL_OSCURO_TEXTO_SOMBRA;
	ArbolSkinOscuro.TextoNodoSeleccionado				= COLOR_ARBOL_OSCURO_SELECCION_TEXTO;
	ArbolSkinOscuro.TextoNodoSeleccionadoSombra			= COLOR_ARBOL_OSCURO_SELECCION_TEXTO_SOMBRA;
	ArbolSkinOscuro.TextoNodoSeleccionadoResaltado		= COLOR_ARBOL_OSCURO_SELECCION_TEXTO_RESALTADO;
	ArbolSkinOscuro.TextoNodoSubSeleccionado			= COLOR_ARBOL_OSCURO_SUBSELECCION_TEXTO;
	ArbolSkinOscuro.TextoNodoSubSeleccionadoSombra		= COLOR_ARBOL_OSCURO_SUBSELECCION_TEXTO_SOMBRA;
	ArbolSkinOscuro.TextoNodoSubSeleccionadoResaltado	= COLOR_ARBOL_OSCURO_SUBSELECCION_TEXTO_RESALTADO;
	ArbolSkinOscuro.TextoNodoPresionado					= COLOR_ARBOL_OSCURO_SELECCION_TEXTO_RESALTADO;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Colores para le fondo del scroll
	ScrollSkinOscuro.FondoScrollNormal					= COLOR_SCROLL_OSCURO_FONDO;
	ScrollSkinOscuro.FondoScrollResaltado				= COLOR_SCROLL_OSCURO_FONDO_RESALTADO;
	ScrollSkinOscuro.FondoScrollPresionado				= COLOR_SCROLL_OSCURO_FONDO_PRESIONADO;
	ScrollSkinOscuro.BarraScrollNormal					= COLOR_SCROLL_OSCURO_BARRA;
	ScrollSkinOscuro.BarraScrollResaltado				= COLOR_SCROLL_OSCURO_BARRA_RESALTADO;
	ScrollSkinOscuro.BarraScrollPresionado				= COLOR_SCROLL_OSCURO_BARRA_PRESIONADO;
	// Colores del orde del control (donde no hay nodos ni items)
	ScrollSkinOscuro.BordeNormal						= COLOR_SCROLL_OSCURO_BORDE;
	ScrollSkinOscuro.BordeResaltado						= COLOR_SCROLL_OSCURO_BORDE_RESALTADO;
	// Colores del fondo del control (donde no hay nodos ni items)
	ScrollSkinOscuro.FondoNormal						= COLOR_SCROLL_OSCURO_CONTROL_FONDO;
	ScrollSkinOscuro.FondoResaltado						= COLOR_SCROLL_OSCURO_CONTROL_FONDO_RESALTADO;

}



void VentanaPrincipal::CrearBotonesThumb(void) {
	// Creo los botones del Thumb de la taskbar
	std::vector<DBarraTareas_Boton> Botones;
	Botones.push_back(DBarraTareas_Boton(IDI_PREV32, L"Medio Anterior", ID_BOTON_ANTERIOR));
	Botones.push_back(DBarraTareas_Boton(IDI_PLAY32, L"Play / Pausa", ID_BOTON_PLAY));
	Botones.push_back(DBarraTareas_Boton(IDI_STOP32, L"Stop", ID_BOTON_STOP));
	Botones.push_back(DBarraTareas_Boton(IDI_NEXT32, L"Medio Siguiente", ID_BOTON_SIGUIENTE));
	BarraTareas.AgregarBotones(Botones);
}




void VentanaPrincipal::AjustarControles(RECT &RC) {
	MoveWindow(Lista.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);
	MoveWindow(Arbol.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);
	MoveWindow(Video.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);

	MoveWindow(SliderTiempo.hWnd(), 10, 50, RC.right - 20, 20, TRUE);
	MoveWindow(MarcoSD.hWnd(), RC.right - 260, 14, 255, 24, TRUE);

	MoveWindow(BotonOpciones.hWnd(), 0, RC.bottom - 120, RAVE_BOTONES_LATERALES_ANCHO, 30, TRUE);

	if (App.MP.hWndVLC != NULL) {
		InvalidateRect(App.MP.hWndVLC, &RC, TRUE);
	}
}


BOOL CALLBACK VentanaPrincipal::EnumerarPantallas(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	POINT Pt = { App.Opciones.PosX(), App.Opciones.PosY() };
	if (PtInRect(lprcMonitor, Pt) != 0) {
		// El monitor está disponible, no hace falta seguir buscando
		App.VentanaRave.RectMonitorActual	= *lprcMonitor;
		App.VentanaRave.MonitorDisponible	= TRUE;
		return FALSE;
	}
	App.VentanaRave.MonitorDisponible = FALSE;
	App.VentanaRave.RectMonitorActual = *lprcMonitor;
	return TRUE;
}



// Función que ejecuta el repeat
void VentanaPrincipal::Repeat(void) {
	switch (App.Opciones.Repeat()) {
		case Tipo_Repeat_NADA :
			break;
		case Tipo_Repeat_RepetirLista :
			Lista_Siguiente();
			break;
		case Tipo_Repeat_RepetirListaShufle :
			if (Lista.TotalItems() > 0) {
				Lista.Mezclar(TRUE);
				Lista.MedioActual = NULL;
				Lista.DesSeleccionarTodo();
				Lista.ItemMarcado(Lista.Item(0));
				Lista.Item(0)->Seleccionado = TRUE;
				Lista_Siguiente();
			}
			break;
		case Tipo_Repeat_GenerarLoQueSea :
			GenerarListaAleatoria();
			break;
		case Tipo_Repeat_GenerarGenero:
			GenerarListaAleatoria(TLA_Genero);
			break;
		case Tipo_Repeat_GenerarGrupo:
			GenerarListaAleatoria(TLA_Grupo);
			break;
		case Tipo_Repeat_GenerarDisco:
			GenerarListaAleatoria(TLA_Disco);
			break;
		case Tipo_Repeat_Generar50Canciones:
			GenerarListaAleatoria(TLA_50Medios);
			break;
		case Tipo_Repeat_ApagarReproductor:
			PostMessage(_hWnd, WM_CLOSE, 0, 0);
			break;
		case Tipo_Repeat_ApagarOrdenador:
			App.CerrarSistema((IsWindows8OrGreater()) ? SOCerrarSistema_Hibernar : SOCerrarSistema_Apagar);
			break;
	}
}


void VentanaPrincipal::Lista_Pausa(void) {
	BotonPlay.Icono(IDI_PLAY32, 32);
	App.ControlesPC.BotonPlay.Icono(IDI_PLAY32, 32);
	BarraTareas.Boton_Icono(ID_BOTON_PLAY, IDI_PLAY32);
	App.MP.Pausa();
}


void VentanaPrincipal::Lista_Play(void) {
	if (Lista.TotalItems() == 0) return;

	if (Lista.MedioActual == nullptr) {
		Lista.MedioActual = Lista.MedioSiguiente(NULL);
	}
/*	if (Lista.MedioActual == nullptr) 
return; */

	Lista.Errores = 0;
	BDMedio NCan, NCan2;
	ItemMedio *IMS = NULL;
	switch (App.MP.ComprobarEstado()) {
		case SinCargar:
			if (App.MP.AbrirMedio(Lista.MedioActual) == FALSE) Lista.Errores++;
			/*
			App.BD.ObtenerMedio(Lista.MedioActual->BdMedio.Hash, NCan);
			IMS = Lista.MedioSiguiente(Lista.MedioActual);
			if (IMS != NULL) {
				App.BD.ObtenerMedio(IMS->BdMedio.Hash, NCan2);
				if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
			}
			else {
				if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
			}*/
			if (App.MP.Play() == TRUE) {
				BotonPlay.Icono(IDI_PAUSA32, 32);
				App.ControlesPC.BotonPlay.Icono(IDI_PAUSA32, 32);
				BarraTareas.Boton_Icono(ID_BOTON_PLAY, IDI_PAUSA32);
			}
			break;
		case Terminada:
			Lista.MedioActual = Lista.MedioSiguiente(Lista.MedioActual);
			if (App.MP.AbrirMedio(Lista.MedioActual) == FALSE) Lista.Errores++;
			/*App.BD.ObtenerMedio(Lista.MedioActual->BdMedio.Hash, NCan);
			IMS = Lista.MedioSiguiente(Lista.MedioActual);
			if (IMS == NULL) {
				if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
			}
			else {
				App.BD.ObtenerMedio(IMS->BdMedio.Hash, NCan2);
				if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
			}*/
			App.MP.Play();
			break;
		case EnStop:
		case EnPausa:
			if (App.MP.Play() == TRUE) {
				BotonPlay.Icono(IDI_PAUSA32, 32);
				App.ControlesPC.BotonPlay.Icono(IDI_PAUSA32, 32);
				BarraTareas.Boton_Icono(ID_BOTON_PLAY, IDI_PAUSA32);
			}			
			break;
		case EnPlay: 
			Lista_Pausa();
			break;		
	}
}


void VentanaPrincipal::Lista_Stop(void) {
	Lista.Errores = 0;
	BotonPlay.Icono(IDI_PLAY32, 32);
	App.ControlesPC.BotonPlay.Icono(IDI_PLAY32, 32);
	App.MP.Stop();
}


void VentanaPrincipal::Lista_Siguiente(void) {
	if (Lista.TotalItems() == 0) return;

	Lista.MedioActual = Lista.MedioSiguiente(Lista.MedioActual);
	// Se ha llegado al final de la lista, vuelvo a pedir el medio actual
	if (Lista.MedioActual == nullptr) Lista.MedioActual = Lista.MedioSiguiente(nullptr);

//	BDMedio NCan, NCan2;
	App.MP.Stop();
	/*
	App.BD.ObtenerMedio(Lista.MedioActual->BdMedio.Hash, NCan);
	ItemMedio *IMS = Lista.MedioSiguiente(Lista.MedioActual);
	if (IMS == NULL) {
		if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
	}
	else {
		App.BD.ObtenerMedio(IMS->BdMedio.Hash, NCan2);
		if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
	}*/
	if (App.MP.AbrirMedio(Lista.MedioActual) == FALSE) Lista.Errores++;
	App.MP.Play();
}


void VentanaPrincipal::Lista_Anterior(void) {
	if (Lista.TotalItems() == 0) return;

	LONG_PTR TotalItems = Lista.TotalItems() - 1;
	if (TotalItems == -1) TotalItems = 0;
	Lista.MedioActual = Lista.MedioAnterior(Lista.MedioActual);
	// Si es null es que es el primer item, y quiero el ultimo
	if (Lista.MedioActual == nullptr) Lista.MedioActual = Lista.MedioAnterior(nullptr);

	
/*	BDMedio NCan, NCan2;
	ItemMedio *IMS = Lista.MedioSiguiente(Lista.MedioActual);
	App.BD.ObtenerMedio(Lista.MedioActual->BdMedio.Hash, NCan);*/
	App.MP.Stop();
/*	if (IMS == NULL) {
		if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
	}
	else {
		App.BD.ObtenerMedio(IMS->BdMedio.Hash, NCan2);
		if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
	}*/
	if (App.MP.AbrirMedio(Lista.MedioActual) == FALSE) Lista.Errores++;
//		TablaMedios_Medio NCan(App.BD(), Lista.Medio(Lista.MedioActual)->Hash);
	
	App.MP.Play();
//	}
}

void VentanaPrincipal::GenerarListaAleatoria(const TipoListaAleatoria nTipo) {
	std::vector<BDMedio> Medios;
	App.BD.GenerarListaAleatoria(Medios, nTipo);
	Lista.BorrarListaReproduccion();
	for (size_t i = 0; i < Medios.size(); i++) {
		Lista.AgregarMedio(&Medios[i]);
	}
	if (BotonMezclar.Marcado() == TRUE) {
		Lista.Mezclar(TRUE);
		Lista.MedioActual = NULL;
	}
	Lista.Repintar();

	App.VentanaRave.Lista_Play();
}

void VentanaPrincipal::FiltrosVideoPorDefecto(void) {
	App.MP.Brillo(1.0f);
	App.MP.Contraste(1.0f);
	App.MP.Saturacion(1.0f);
	App.MenuVideoFiltros->Menu(0)->BarraValor(1.0f);
	App.MenuVideoFiltros->Menu(1)->BarraValor(1.0f);
	App.MenuVideoFiltros->Menu(2)->BarraValor(1.0f);
	App.BD.ActualizarMedio(&App.MP.MedioActual());
}

void VentanaPrincipal::Lista_Propiedades(void) {
	BDMedio nMedio; 
	App.BD.ObtenerMedio(Lista.MedioMarcado()->BdMedio.Hash, nMedio);
	SHELLEXECUTEINFO info = { 0 };
	info.cbSize = sizeof info;
	info.lpFile = nMedio.Path.c_str();
	info.nShow = SW_SHOW;
	info.fMask = SEE_MASK_INVOKEIDLIST;
	info.lpVerb = L"properties";

	ShellExecuteEx(&info);
}

void VentanaPrincipal::Lista_AbrirEnExplorador(void) {
	BDMedio nMedio;
	App.BD.ObtenerMedio(Lista.MedioMarcado()->BdMedio.Hash, nMedio);
	PIDLIST_ABSOLUTE pidl;
	pidl = ILCreateFromPath(nMedio.Path.c_str());
	if (pidl) {
		SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
		ILFree(pidl);
	}

}

void VentanaPrincipal::Lista_MostrarEnBaseDatos(void) {
	Arbol_MostrarMedio(Lista.MedioMarcado()->BdMedio.Hash);
	MostrarMarco(ID_BOTON_BD);
}

void VentanaPrincipal::Lista_EliminarSeleccionados(void) {
	LONG_PTR Ret = 0;
	BOOL MAC = FALSE;  // Medio Actual Cerrado
	for (LONG_PTR i = Lista.TotalItems() - 1; i > -1; i--) {
		if (Lista.Item(i)->Seleccionado == TRUE) {
			if (Lista.Medio(i) == Lista.MedioActual) {
				App.MP.CerrarMedio();
				MAC = TRUE;
				// Asigno el medio anterior o NULL si no hay medio anterior
				Lista.MedioActual = Lista.MedioAnterior(Lista.MedioActual);
			}
			Lista.EliminarItem(i);
			Ret++;
		}
	}

	std::wstring Txt = std::to_wstring(Ret) + L" medios eliminados de la lista";
	App.MostrarToolTipPlayer(Txt);

	if (MAC == TRUE) {
		if (Lista.TotalItems() == 0)	Lista.MedioActual = NULL;
		Lista_Play();
	}

	Lista.Repintar();

//	LONG_PTR nItems = Lista.EliminarItemsSeleccionados();
}


void VentanaPrincipal::Lista_Momentos(void) {
	if (Lista.TotalItems() == 0) return;
	BDMedio Medio;
	App.BD.ObtenerMedio(Lista.MedioMarcado()->BdMedio.Hash, Medio);
	Momentos.Mostrar(Medio);
}

// Tambien sirve para los momentos de la BD y del vídeo
void VentanaPrincipal::Lista_MomentosAbrir(const UINT64 HashMedio, const int PosMomento) {
	BDMedio Medio;
	App.BD.ObtenerMedio(HashMedio, Medio);
	Lista.ReproducirMedio(Medio, PosMomento);
}

void VentanaPrincipal::Video_Momentos(void) {
	Momentos.Mostrar(App.MP.MedioActual());
}

void VentanaPrincipal::Video_Subtitulos(void) {
	DWL::DDlgAbrir Abrir(L"", L"Archivos de Subtítulos\0*.srt\0", L"Abrir subtitulo", false, _hWnd);
	if (Abrir.Archivos.size() != 0) {
		int Ret = App.MP.AsignarSubtitulos(Abrir[0].c_str());
		int Ret2 = App.MP.EnumerarSubtitulos();
		App.MostrarToolTipPlayer(L"Subtitulos cargados");
	}	
}


void VentanaPrincipal::Arbol_AgregarALista(const BOOL NuevaLista) {
	std::wstring nTexto = L"\" añadido a la lista.";
	if (NuevaLista == TRUE) {
		nTexto = L"\" añadido a una nueva lista.";
		Lista.BorrarListaReproduccion();
	}

	// Busco los nodos seleccionados
	NodoBD *Tmp				= static_cast<NodoBD *>(Arbol.NodoRaiz());
	size_t  MediosAgregados = 0;
	while (Tmp != NULL) {
		if (Tmp->Seleccionado == TRUE) {
			MediosAgregados = Arbol.AgregarNodoALista(Tmp);
			if (MediosAgregados != 0)	App.MostrarToolTipPlayer(L"\"" + Tmp->Texto + nTexto);
		}
		Tmp = static_cast<NodoBD *>(Arbol.BuscarNodoSiguiente(Tmp, TRUE));
	}

	if (App.MP.ComprobarEstado() != EnPlay) App.VentanaRave.Lista_Play();
}

void VentanaPrincipal::Arbol_AbrirCarpeta(void) {
	std::wstring Path;
	NodoBD *Tmp = Arbol.MedioMarcado();

	Arbol.ObtenerPathNodo(Tmp, Path);	

	PIDLIST_ABSOLUTE pidl;
	pidl = ILCreateFromPath(Path.c_str());
	if (pidl) {
		SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
		ILFree(pidl);
	}

}

void VentanaPrincipal::Arbol_Propiedades(void) {
	std::wstring Path;

	Arbol.ObtenerPathNodo(Arbol.MedioMarcado(), Path);

	SHELLEXECUTEINFO info = { 0 };
	info.cbSize = sizeof info;
	info.lpFile = Path.c_str();
	info.nShow = SW_SHOW;
	info.fMask = SEE_MASK_INVOKEIDLIST;
	info.lpVerb = L"properties";

	ShellExecuteEx(&info);
}


void VentanaPrincipal::MostrarVentanaURL(void) {
	App.VentanaURL.Mostrar();
}


void VentanaPrincipal::Evento_MenuEx_Click(const UINT cID) {
	switch (cID) {
		// Menú boton lista
		case ID_MENUBOTONLISTA_GENERAR			:	GenerarListaAleatoria();				return;
		case ID_MENUBOTONLISTA_GENERAR_GENERO	:	GenerarListaAleatoria(TLA_Genero);		return;
		case ID_MENUBOTONLISTA_GENERAR_GRUPO	:	GenerarListaAleatoria(TLA_Grupo);		return;
		case ID_MENUBOTONLISTA_GENERAR_DISCO	:	GenerarListaAleatoria(TLA_Disco);		return;
		case ID_MENUBOTONLISTA_GENERAR_50MEDIOS	:	GenerarListaAleatoria(TLA_50Medios);	return;
		case ID_MENUBOTONLISTA_GENERAR_NOTA		:	GenerarListaAleatoria(TLA_Nota);		return;
		case ID_MENUBOTONLISTA_BORRAR			:
			Lista.BorrarListaReproduccion();					
			App.MostrarToolTipPlayer(L"Lista de reproducción borrada.");
			return;
		case ID_MENUBOTONLISTA_AGREGAR_URL		:	MostrarVentanaURL();					return;
		// Id's de los botones de la barra de tareas
		case ID_BOTON_PLAY						:	Lista_Play();							return;
		case ID_BOTON_STOP						:	Lista_Stop();							return;
		case ID_BOTON_SIGUIENTE					:	Lista_Siguiente();						return;
		case ID_BOTON_ANTERIOR					:	Lista_Anterior();						return;
		case ID_MENUVIDEO_PORDEFECTO			:	FiltrosVideoPorDefecto();				return;
		// Menú BotonBD, MenuBD
		case ID_MENUBD_ACTUALIZAR               :   ActualizarArbol();						return;
		case ID_MENUBD_ANALIZAR					:	AnalizarBD();							return;
		case ID_MENUBD_ABRIRCARPETA             :   Arbol_AbrirCarpeta();					return;
		case ID_MENUBD_PROPIEDADES              :   Arbol_Propiedades();					return;
		case ID_MENUBD_AGREGARANUEVALISTA		:	Arbol_AgregarALista(TRUE);				return;
		case ID_MENUBD_AGREGARALISTA			:	Arbol_AgregarALista(FALSE);				return;
		case ID_MENUBD_MOMENTOS					:   Arbol_Momentos();						return;
		case ID_MENUBD_AGREGAR_URL				:	MostrarVentanaURL();					return;
		// Menú Lista
		case ID_MENULISTA_ABRIRCARPETA			:	Lista_AbrirEnExplorador();				return;
		case ID_MENULISTA_PROPIEDADES			:	Lista_Propiedades();					return;
		case ID_MENULISTA_MOSTRARBD             :   Lista_MostrarEnBaseDatos();				return;
		case ID_MENULISTA_ELIMINAR              :   Lista_EliminarSeleccionados();			return;
		case ID_MENULISTA_MOMENTOS				:   Lista_Momentos();						return;
		// Menú Video
		case ID_MENUVIDEO_MOMENTOS              :   Video_Momentos();						return;
		case ID_MENUVIDEO_SUBTITULOS            :	Video_Subtitulos();						return;

	}

	// Menu Lista -> Momentos -> Momento. Menu BD -> Momentos -> Momento, Menu Video -> Momentos -> Momento
	if (cID >= ID_MENULISTA_MOMENTOS_MOMENTO && cID < ID_MENULISTA_MOMENTOS_MOMENTO_FIN) {
		DWL::DMenuEx* TmpMenu = App.VentanaRave.Menu_Lista.BuscarMenu(cID);
		if (TmpMenu != NULL) {
			Lista_MomentosAbrir(TmpMenu->Parametro, cID - ID_MENULISTA_MOMENTOS_MOMENTO);
			return;
		}
		
		TmpMenu = App.VentanaRave.Menu_ArbolBD.BuscarMenu(cID);
		if (TmpMenu != NULL) {
			Arbol.AgregarNodoALista(Arbol.MedioMarcado());
			// s'ha d'agregar a la llista
			Lista_MomentosAbrir(TmpMenu->Parametro, cID - ID_MENULISTA_MOMENTOS_MOMENTO);
			return;
		}

		TmpMenu = App.VentanaRave.Menu_Video.BuscarMenu(cID);
		if (TmpMenu != NULL) {
			Lista_MomentosAbrir(TmpMenu->Parametro, cID - ID_MENULISTA_MOMENTOS_MOMENTO);
			return;
		}

	}

	// Menu Video -> Pistas de audio
	if (cID >= ID_MENUVIDEO_AUDIO_PISTAS_AUDIO && cID < ID_MENUVIDEO_AUDIO_PISTAS_AUDIO_FIN) {
		// Des-marco todas las pistas de audio
		for (size_t i = 0; i < App.MenuVideoPistasDeAudio->TotalMenus(); i++) {
			App.MenuVideoPistasDeAudio->Menu(i)->Icono(0);
		}
		// Marco la pista de audio actual (si existe, ... que debería)
		DMenuEx *MenuClick = App.MenuVideoPistasDeAudio->BuscarMenu(cID);
		if (MenuClick != NULL) MenuClick->Icono(IDI_CHECK2);

		App.MP.AsignarPistaAudio(cID - ID_MENUVIDEO_AUDIO_PISTAS_AUDIO);
		return;
	}


	// Menu Video -> Proporción
	if (cID >= ID_MENUVIDEO_PROPORCION_PREDETERMINADO && cID < ID_MENUVIDEO_PROPORCION_5A4 + 1) {		
		App.MP.AsignarProporcion(cID);
		if (App.Opciones.GuardarBSCP() == TRUE)	App.BD.ActualizarMedio(&App.MP.MedioActual());
		return;
	}
}


void VentanaPrincipal::Arbol_AsignarNota(const float nNota) {
	NodoBD *Tmp = Arbol.MedioMarcado();
	if (Tmp != NULL) {
		// Es un medio
		if (Tmp->TipoNodo == ArbolBD_TipoNodo_Video || Tmp->TipoNodo == ArbolBD_TipoNodo_Cancion) {
			App.BD.MedioNota(Tmp, nNota);
		}
		// Es una etiqueta
		else {			
			std::wstring EtiquetaFiltrada;
			EtiquetaBD  *Etiqueta = NULL;
			BDMedio      Medio;

			if (Tmp != NULL) RaveBD::FiltroNombre(Tmp->Texto, EtiquetaFiltrada);
			Etiqueta = App.BD.ObtenerEtiqueta(EtiquetaFiltrada);
			if (Etiqueta != NULL) {
				App.BD.AsignarNotaEtiqueta(nNota, Etiqueta);
			}
		}
		App.MostrarToolTipPlayer(L"Nota de '" + Tmp->Texto + L"' actualizada a " + DWL::Strings::ToStrF(nNota, 2));
	}
	App.VentanaRave.Menu_ArbolBD.Ocultar(TRUE);
}

void VentanaPrincipal::Arbol_Momentos(void) {
	BDMedio Medio;
	App.BD.ObtenerMedio(Arbol.MedioMarcado()->Hash, Medio);
	Momentos.Mostrar(Medio);
}


void VentanaPrincipal::Lista_AsignarNota(const float nNota) {
	std::wstring StrMedio;
	for (LONG_PTR i = 0; i < Lista.TotalItems(); i++) {
		if (Lista.Medio(i)->Seleccionado == TRUE) {
			App.BD.MedioNota(Lista.Medio(i), nNota);
			StrMedio = Lista.Medio(i)->Texto(0) + L" " + Lista.Medio(i)->Texto(1);
		}
	}
	App.VentanaRave.Menu_Lista.Ocultar(TRUE);
	LONG_PTR TM;
	TM = Lista.TotalItemsSeleccionados();
	if (TM == 1)	{ App.MostrarToolTipPlayer(StrMedio + L" ahora tiene " + DWL::Strings::ToStrF(nNota, 2) + L" de nota"); }
	else			{ App.MostrarToolTipPlayer(L"Nota actualizada a " + DWL::Strings::ToStrF(nNota, 2));					}
}



// MouseDown o MouseMove en la barraEx del menú
void VentanaPrincipal::Evento_MenuEx_Barra_Cambiando(const UINT cID, const float ValorBarra) {
	switch (cID) {
		// Barras del menu del video
		case ID_MENUVIDEO_BRILLO				:	App.MP.Brillo(ValorBarra);				return;
		case ID_MENUVIDEO_CONTRASTE				:	App.MP.Contraste(ValorBarra);			return;
		case ID_MENUVIDEO_SATURACION			:	App.MP.Saturacion(ValorBarra);			return;
	}
}

// MouseUp en la barraEx del menú
void VentanaPrincipal::Evento_MenuEx_Barra_Cambiado(const UINT cID, const float ValorBarra) {
	std::wstring StrMedio;
	switch (cID) {
		// Barras del menu del video
		case ID_MENUVIDEO_BRILLO				:	
			App.MP.Brillo(ValorBarra);
			if (App.Opciones.GuardarBSCP() == TRUE)		App.BD.ActualizarMedio(&App.MP.MedioActual());
			return;
		case ID_MENUVIDEO_CONTRASTE				:	
			App.MP.Contraste(ValorBarra);		
			if (App.Opciones.GuardarBSCP() == TRUE)		App.BD.ActualizarMedio(&App.MP.MedioActual());
			return;
		case ID_MENUVIDEO_SATURACION			:	
			App.MP.Saturacion(ValorBarra);			
			if (App.Opciones.GuardarBSCP() == TRUE)		App.BD.ActualizarMedio(&App.MP.MedioActual());
			return;
		// Menu Lista -> Nota
		case ID_MENULISTA_NOTA					:	Lista_AsignarNota(ValorBarra);			return;
		// Menu BD -> Nota
		case ID_MENUBD_NOTA						:	Arbol_AsignarNota(ValorBarra);			return;
	}
}



void VentanaPrincipal::Evento_BotonEx_Mouse_Presionado(DWL::DEventoMouse &DatosMouse) {
	_BotonExMouseDownTick = GetTickCount();
	switch (DatosMouse.ID()) {
		case ID_BOTON_ANTERIOR:	
			App.MP.Ratio(0.5f);
			break;
		case ID_BOTON_SIGUIENTE:
			if		(DatosMouse.Boton == 0)		App.MP.Ratio(2.0f);
			else if (DatosMouse.Boton == 1)     App.MP.Ratio(4.0f);
			else                                App.MP.Ratio(8.0f);
			break;
	}
}

void VentanaPrincipal::Evento_BotonEx_Mouse_Soltado(DWL::DEventoMouse &DatosMouse) {
	if (DatosMouse.ID() == ID_BOTON_ANTERIOR || DatosMouse.ID() == ID_BOTON_SIGUIENTE) {
		App.MP.Ratio(1.0f);
	}
}


void VentanaPrincipal::MostrarMarco(const INT_PTR ID) {
	BotonBD.Marcado((ID == ID_BOTON_BD) ? TRUE: FALSE);
	BotonLista.Marcado((ID == ID_BOTON_LISTA) ? TRUE : FALSE);
	BotonVideo.Marcado((ID == ID_BOTON_VIDEO) ? TRUE : FALSE);
	Arbol.Visible((ID == ID_BOTON_BD) ? TRUE : FALSE);
	Lista.Visible((ID == ID_BOTON_LISTA) ? TRUE : FALSE);
	Video.Visible((ID == ID_BOTON_VIDEO) ? TRUE : FALSE);
}

// Mensajes para los botones de la barra de tareas
void VentanaPrincipal::Evento_Comando(WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
		case ID_BOTON_ANTERIOR:		Lista_Anterior();		break;
		case ID_BOTON_SIGUIENTE:	Lista_Siguiente();		break;
		case ID_BOTON_PLAY:			Lista_Play();			break;
		case ID_BOTON_STOP:			Lista_Stop();			break;
	}
}

void VentanaPrincipal::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	if (DatosMouse.Boton == 0) {
		switch (DatosMouse.ID()) {
			case ID_BOTON_OPCIONES:
				if (App.VentanaOpciones.hWnd() == NULL)		App.VentanaOpciones.Crear();
				else                                        SetFocus(App.VentanaOpciones.hWnd());
				break;
			case ID_BOTON_BD:
			case ID_BOTON_LISTA:
			case ID_BOTON_VIDEO:
				MostrarMarco(DatosMouse.ID());
				break;
/*			case ID_BOTON_AGREGARRAIZ:
				AgregarRaiz();
				break;*/
			case ID_BOTON_MEZCLAR:
				Mezclar_Click();
				break;
			case ID_BOTON_REPETIR:
				Repetir_Click();
				break;
			case ID_BOTON_ANTERIOR:
				// El boton lleva menos de 200 milisegundos presionado
				if (GetTickCount() < _BotonExMouseDownTick + 200) {
					Lista_Anterior();
				}
				break;
			case ID_BOTON_SIGUIENTE:
				// El boton lleva menos de 200 milisegundos presionado
				if (GetTickCount() < _BotonExMouseDownTick + 200) {
					Lista_Siguiente();
				}
				break;
			case ID_BOTON_PLAY:
				Lista_Play();
				break;
/*			case ID_BOTON_PAUSA:
				Lista_Pausa();
				break;*/
			case ID_BOTON_STOP:
				Lista_Stop();
				break;
		}
	}
	else if (DatosMouse.Boton == 1) {
		RECT RV;		
		switch (DatosMouse.ID()) {
			case ID_BOTON_BD:
				GetWindowRect(BotonBD.hWnd(), &RV);
				Menu_BotonArbolBD.Mostrar(this, RV.left, RV.bottom);
				break;
			case ID_BOTON_LISTA:
				GetWindowRect(BotonLista.hWnd(), &RV);
				Menu_BotonLista.Mostrar(this, RV.left, RV.bottom);
				break;
		}
	}
}

// Click en el boton Mezclar
void VentanaPrincipal::Mezclar_Click(void) {
	Mezclar(!App.Opciones.Shufle());
}

// Función que mezcla / restaura el orden de la lista
void VentanaPrincipal::Mezclar(const BOOL nMezclar) {
	Lista.Mezclar(nMezclar);
	App.Opciones.Shufle(nMezclar);
	BotonMezclar.Marcado(nMezclar);
	App.ControlesPC.BotonMezclar.Marcado(nMezclar);
}


void VentanaPrincipal::Repetir_Click(void) {
	RECT     RW;
	DMenuEx *Ret = NULL;
	if (_PantallaCompleta == TRUE) {	
		GetWindowRect(App.ControlesPC.BotonRepetir.hWnd(), &RW);
		POINT Espacio = Menu_Repetir.CalcularEspacio();
		switch (App.ControlesPC.Alineacion) {
			case Abajo		:	Ret = Menu_Repetir.MostrarModal(&App.ControlesPC, RW.left, RW.top - Espacio.y);		break;
			case Arriba		:	Ret = Menu_Repetir.MostrarModal(&App.ControlesPC, RW.left, RW.bottom);				break;
			case Izquierda	:	Ret = Menu_Repetir.MostrarModal(&App.ControlesPC, RW.right, RW.top);				break;
			case Derecha	:	Ret = Menu_Repetir.MostrarModal(&App.ControlesPC, RW.left - Espacio.x, RW.top);		break;
		}		
	}
	else {
		GetWindowRect(BotonRepetir.hWnd(), &RW);         
		Ret = Menu_Repetir.MostrarModal(this, RW.left, RW.bottom);
	}

	 
	if (Ret != NULL) {
		// Elimino los iconos de todos los submenus de la raíz
		for (size_t i = 0; i < Menu_Repetir.TotalMenus(); i++) {
			Menu_Repetir.Menu(i)->Icono(IDI_NOCHECK);
		}
		// Elimino los iconos de todos los submenus del submenu 'Generar'
		for (size_t i2 = 0; i2 < Menu_Repetir.Menu(3)->TotalMenus(); i2++) {
			Menu_Repetir.Menu(3)->Menu(i2)->Icono(IDI_NOCHECK);
		}

		// Asigno el icono de la marca al menú seleccionado
		Ret->Icono(IDI_CHECK);
		if (Ret->ID() == ID_REPETIR_GENERAR) {
			Menu_Repetir.Menu(3)->Menu(5)->Icono(IDI_CHECK); // Lo Que Sea
		}
		else if (Ret->ID() == ID_REPETIR_LOQUESEA || Ret->ID() == ID_REPETIR_GENERO || Ret->ID() == ID_REPETIR_GRUPO || Ret->ID() == ID_REPETIR_DISCO || Ret->ID() == ID_REPETIR_50CANCIONES) {
			Menu_Repetir.Menu(3)->Icono(IDI_CHECK);
		}
		
		// Asigno el tipo de repeat 
		switch (Ret->ID()) {
			case ID_REPETIR_NO				:	App.Opciones.Repeat(Tipo_Repeat_NADA);					break;
			case ID_REPETIR_SI				:	App.Opciones.Repeat(Tipo_Repeat_RepetirLista);			break;
			case ID_REPETIR_SI_MEZCLAR		:	App.Opciones.Repeat(Tipo_Repeat_RepetirListaShufle);	break;
			case ID_REPETIR_SI_APAGAR_REP	:	App.Opciones.Repeat(Tipo_Repeat_ApagarReproductor);		break;	// No se guarda en la BD
			case ID_REPETIR_SI_APAGAR_WIN	:	App.Opciones.Repeat(Tipo_Repeat_ApagarOrdenador);		break;	// No se guarda en la BD
			case ID_REPETIR_GENERAR         :
			case ID_REPETIR_LOQUESEA        :	App.Opciones.Repeat(Tipo_Repeat_GenerarLoQueSea);		break;
			case ID_REPETIR_GENERO			:	App.Opciones.Repeat(Tipo_Repeat_GenerarGenero);			break;
			case ID_REPETIR_GRUPO			:	App.Opciones.Repeat(Tipo_Repeat_GenerarGrupo);			break;
			case ID_REPETIR_DISCO			:	App.Opciones.Repeat(Tipo_Repeat_GenerarDisco);			break;
			case ID_REPETIR_50CANCIONES     :   App.Opciones.Repeat(Tipo_Repeat_Generar50Canciones);	break;
		}
		
		// Marco / desmarco el boton del repeat según la ID
		BOOL Marcar = (Ret->ID() != ID_REPETIR_NO) ? TRUE : FALSE;
		BotonRepetir.Marcado(Marcar);  
		App.ControlesPC.BotonRepetir.Marcado(Marcar);
	}
}



void VentanaPrincipal::Evento_CambiandoTam(const UINT Lado, RECT *Rectangulo) {
	// Comprobación de tamaño mínimo
	if ((Rectangulo->right - Rectangulo->left) < RAVE_MIN_ANCHO)	Rectangulo->right = Rectangulo->left + RAVE_MIN_ANCHO;
	if ((Rectangulo->bottom - Rectangulo->top) < RAVE_MIN_ALTO)		Rectangulo->bottom = Rectangulo->top + RAVE_MIN_ALTO;

	RECT RC;
	RC.left = 0;
	RC.top = 0;
	RC.right = (Rectangulo->right - Rectangulo->left) - CTW_ExtraX;
	RC.bottom = (Rectangulo->bottom - Rectangulo->top) - CTW_ExtraY;

	App.OcultarToolTipPlayer();

	Debug_Escribir_Varg(L"Evento_CambiandoTam %d, %d\n", RC.right, RC.bottom);

//	AjustarControles(RC);

	// repinto el control del video
//	int ExtraX = (GetSystemMetrics(SM_CXSIZEFRAME) * 2) + (GetSystemMetrics(SM_CXEDGE) * 2) + GetSystemMetrics(SM_CXPADDEDBORDER);
//	int ExtraY = (GetSystemMetrics(SM_CYSIZEFRAME) * 2) + (GetSystemMetrics(SM_CYEDGE) * 2) + GetSystemMetrics(SM_CYCAPTION);
}


void VentanaPrincipal::PantallaCompleta(const BOOL nActivar) {
	RECT RC; 
	_PantallaCompleta = nActivar;
	if (nActivar == TRUE) {
		App.OcultarToolTipPlayer();
		App.OcultarToolTipOpciones();
		// Desmarco los botones de BD y lista (ya que podian haber quedado marcados anteriormente)
		App.ControlesPC.BotonBD.Marcado(FALSE);
		App.ControlesPC.BotonLista.Marcado(FALSE);

		SetWindowLongPtr(hWnd(), GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_VISIBLE);
		ShowWindow(hWnd(), SW_MAXIMIZE);
		GetClientRect(hWnd(), &RC);

		Arbol.Visible(FALSE);
		Lista.Visible(FALSE);
		SliderTiempo.Visible(FALSE);
		MarcoSD.Visible(FALSE);
		MarcoSI.Visible(FALSE);
		MarcoII.Visible(FALSE);

		Video.Visible(FALSE);
		MoveWindow(Video.hWnd(), 0, 0, RC.right, RC.bottom, FALSE);
		Video.Visible(TRUE);
		App.ControlesPC.Mostrar();
		DWL::DMouse::Visible(TRUE);


		SetParent(Arbol.hWnd(), NULL);
		LONG_PTR Estilos = GetWindowLongPtr(Arbol.hWnd(), GWL_STYLE);
		Estilos &= ~WS_CHILD;
		SetWindowLongPtr(Arbol.hWnd(), GWL_STYLE, Estilos | WS_POPUP);
		SetWindowLongPtr(Arbol.hWnd(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
//		ShowWindow(Arbol.hWnd(), SW_SHOW);

		SetParent(Lista.hWnd(), NULL);
		SetWindowLongPtr(Lista.hWnd(), GWL_STYLE, Estilos | WS_POPUP);
		SetWindowLongPtr(Lista.hWnd(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);


		Lista.SkinScroll = ScrollSkinOscuro;
		Lista.Skin       = ListaSkinOscuro;
		Lista.ActualizarSkin();
		Arbol.SkinScroll = ScrollSkinOscuro;
		Arbol.Skin       = ArbolSkinOscuro;
		Arbol.ActualizarSkin();
		//		ShowWindow(Lista.hWnd(), SW_SHOW);

		//		Video.AsignarFoco();

//		RedrawWindow(App.VLC.hWndVLC, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);


//		SetTimer(hWnd(), TIMER_REPINTARVLC, 250, NULL);
		/*		KillTimer(hWnd(), TIMER_CPC_INACTIVIDAD);
		SetTimer(hWnd(), TIMER_CPC_INACTIVIDAD,		, NULL);*/

	}

	else {
		EnumDisplayMonitors(NULL, NULL, VentanaPrincipal::EnumerarPantallas, NULL);
		// Si el monitor donde se guardo la ultima posición no está disponible busco una nueva posición
		if (MonitorDisponible == FALSE) {
			App.Opciones.AsignarPosVentana(RectMonitorActual.left + 100, RectMonitorActual.top + 100);
		}

//		App.ControlesPC._AniMostrar.Terminar();

		ShowWindow(hWnd(), SW_RESTORE);
//		GetClientRect(hWnd(), &RC);
//		BOOL R = App.VentanaRave.BarraTareas.Clip(&RC);
		SetWindowLongPtr(hWnd(), GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

		RC = { App.Opciones.PosX(), App.Opciones.PosY(), App.Opciones.PosX() + App.Opciones.Ancho(), App.Opciones.PosY() + App.Opciones.Alto() };
		AdjustWindowRectEx(&RC, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW);

		SetWindowPos(_hWnd, HWND_TOP, RC.left, RC.top, RC.right - RC.left, RC.bottom - RC.top, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		//MoveWindow(Video.hWnd(), 120, 71, RC.right - 120, RC.bottom - 70, TRUE);

		App.ControlesPC.Ocultar();
		DWL::DMouse::Visible(TRUE);
		KillTimer(hWnd(), TIMER_CPC_INACTIVIDAD);


		LONG_PTR Estilos = GetWindowLongPtr(Arbol.hWnd(), GWL_STYLE);
		Estilos &= ~WS_POPUP;

		LONG_PTR EstilosEx = GetWindowLongPtr(Arbol.hWnd(), GWL_EXSTYLE);
		EstilosEx &= ~WS_EX_LAYERED;

		Arbol.Opacidad(255);
		SetParent(Arbol.hWnd(), _hWnd);
		SetWindowLongPtr(Arbol.hWnd(), GWL_STYLE, Estilos | WS_CHILD);
		SetWindowLongPtr(Arbol.hWnd(), GWL_EXSTYLE, EstilosEx);

		Lista.Opacidad(255);
		SetParent(Lista.hWnd(), _hWnd);
		SetWindowLongPtr(Lista.hWnd(), GWL_STYLE, Estilos | WS_CHILD);
		SetWindowLongPtr(Lista.hWnd(), GWL_EXSTYLE, EstilosEx);


		Arbol.Visible(FALSE);
		Lista.Visible(FALSE);
		GetClientRect(hWnd(), &RC);
		RC.right = RC.right - CTW_ExtraX;
		RC.bottom = RC.bottom - CTW_ExtraY;
		AjustarControles(RC);
		Video.Visible(TRUE);
		SliderTiempo.Visible(TRUE);
		MarcoSD.Visible(TRUE);
		MarcoSI.Visible(TRUE);
		MarcoII.Visible(TRUE);
		// Elimino la región de pintado en la barra de tareas
		BOOL R = BarraTareas.Clip(&RC);

		Lista.SkinScroll = ScrollSkinClaro;
		Lista.Skin       = ListaSkinClaro;
		Lista.ActualizarSkin();
		Arbol.SkinScroll = ScrollSkinClaro;
		Arbol.Skin       = ArbolSkinClaro;
		Arbol.ActualizarSkin();
	}

	InvalidateRect(App.MP.hWndVLC, NULL, TRUE);
}

void VentanaPrincipal::Evento_SliderTiempo_Cambiado(void) {
	if (App.MP.ComprobarEstado() != Estados_Medio::EnPlay && App.MP.ComprobarEstado() != Estados_Medio::EnPausa) return;
	std::wstring TiempoStr;
//	Debug_Escribir_Varg(L"Evento_SliderTiempo_Cambiado %d, %.02f\n", App.VLC.TiempoTotalMs(), SliderTiempo.Valor());
	App.MP.TiempoStr(static_cast<UINT64>(static_cast<double>(App.MP.TiempoTotalMs()) * static_cast<double>(SliderTiempo.Valor())), TiempoStr);
	LabelTiempoActual.Texto(TiempoStr);
	App.ControlesPC.LabelTiempoActual.Texto(TiempoStr);
	App.MP.TiempoActual(SliderTiempo.Valor());
}

void VentanaPrincipal::Evento_SliderVolumen_Cambiando(void) {
	int Volumen = static_cast<int>(SliderVolumen.Valor());	
	App.MP.Volumen(Volumen);
}

// Guardo el volumen en las opciones (al soltar el slider del volumen)
void VentanaPrincipal::Evento_SliderVolumen_Cambiado(void) {
	App.Opciones.Volumen(static_cast<int>(SliderVolumen.Valor()));
}

void VentanaPrincipal::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
//	HBRUSH BrochaFondo = CreateSolidBrush(RGB(70,70,70));
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}

void VentanaPrincipal::Evento_Cerrar(void) {

	ThreadActualizar.Cancelar(TRUE);
	ThreadArchivosLista.Cancelar(TRUE);

	App.OcultarToolTipPlayerRapido();
	App.OcultarToolTipOpcionesRapido();
	App.VentanaOpciones.Destruir();

	Visible(FALSE);

	App.MP.StopTODO();

	//	App.BD.Consulta(L"BEGIN TRANSACTION");
	App.BD.GuardarUltimaLista();
//	App.Opciones.GuardarOpciones();
//	App.BD.Consulta(L"COMMIT TRANSACTION");


//	Destruir();
	PostQuitMessage(0);
}

void VentanaPrincipal::Evento_SoltarArchivos(WPARAM wParam) {
	TCHAR        Archivo[1024];
	unsigned int TotalSoltados = DragQueryFile((HDROP)wParam, static_cast<unsigned int>(-1), 0, 0);
	App.MostrarToolTipPlayer(L"Añadiendo archivos desde el explorador.");
	
	ThreadActualizar.Cancelar(TRUE);
	ThreadAnalizar.Cancelar(TRUE);

	std::vector<std::wstring> Paths;
	std::wstring Tmp;
	for (unsigned int i = 0; i < TotalSoltados; i++) {
		DragQueryFile((HDROP)wParam, i, Archivo, 1024);
		Tmp = Archivo;
		// Si no es un directorio, recorto el path para agregar la raiz
		if ((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(Archivo)) == FALSE) Tmp = Tmp.substr(0, Tmp.find_last_of(L'\\'));
//		App.BD.AgregarRaiz(Tmp);
		Paths.push_back(Archivo);
	}
	DragFinish((HDROP)wParam);

	ThreadArchivosLista.Iniciar(_hWnd, Paths);
}


// Solo crea la raiz (si no está creada)
// Tiene que ser un puntero por que normalmente viene del ThreadActualizar, y es memória creada en el otro trhead que elimino desde el thread principal.
NodoBD *VentanaPrincipal::Arbol_AgregarRaiz(std::wstring *Path) {
	// Busco la raíz en el arbol
	NodoBD *Tmp = Arbol.BuscarHijoTxt(*Path);
	// Si no existe la raíz la creo
	if (Tmp == NULL) {
		Tmp = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Raiz, NULL, Path->c_str(), 0, 0);
		Tmp->Expandido = TRUE;
		Arbol.Repintar();
	}
	return Tmp;
}


NodoBD *VentanaPrincipal::Arbol_AgregarDir(std::wstring *Path, const BOOL nRepintar) {
	// Si no hay raíz no hay galletas
	BDRaiz *Raiz = App.BD.BuscarRaiz(*Path);
	if (Raiz == NULL) return NULL;

	// Busco el nodo de la raíz
	NodoBD *TmpNodo = static_cast<NodoBD *>(Arbol.BuscarHijoTxt(Raiz->Path, NULL));

	// Separo el path en directorios y archivo por las '\'
	std::wstring tmp = Path->substr(Raiz->Path.size(), Path->size() - Raiz->Path.size());
	DWL::Strings::Split nSplit(tmp, L'\\');

	// Busco el ultimo nodo padre
	for (int i = 0; i < static_cast<int>(nSplit.Total()) - 1; i++) {
		TmpNodo = Arbol.BuscarHijoTxt(nSplit[i], TmpNodo);
	}

	// Paso el filtro al nombre de archivo
	std::wstring Filtrado;
	RaveBD::FiltroPath(nSplit[nSplit.Total() - 1], Filtrado);

	NodoBD *Ret = Arbol.BuscarHijoTxt(Filtrado, TmpNodo);
	// Agrego el directorio
	if (Ret == NULL) {
		Ret = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Directorio, TmpNodo, Filtrado.c_str(), 0, 0);
	}
	//	delete Path;
	if (nRepintar == TRUE)	Arbol.Repintar();

	return Ret;
}

const BOOL VentanaPrincipal::Arbol_MostrarMedio(BDMedio &mMedio) {
	// Si no hay raíz no hay galletas
	BDRaiz *Raiz = App.BD.BuscarRaiz(mMedio.Path);
	if (Raiz == NULL) return FALSE;

	// Busco el nodo de la raíz
	NodoBD *TmpNodo = static_cast<NodoBD *>(Arbol.BuscarHijoTxt(Raiz->Path, NULL));
	NodoBD *TmpNodo2 = NULL;
	// Si el nodo raíz no existe lo creo
	if (TmpNodo == NULL) {
		TmpNodo = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Raiz, NULL, mMedio.Path.c_str(), 0, 0);
		TmpNodo->Expandido = TRUE;
	}
	
	// Excluyo la raíz del path, y separo el resto del path en directorios y archivo por las '\'
	std::wstring tmp = mMedio.Path.substr(Raiz->Path.size(), mMedio.Path.size() - Raiz->Path.size());
	DWL::Strings::Split nSplit(tmp, L'\\');
	std::wstring PathActual = Raiz->Path;
	// Recorro los nodos (TmpNodo hace de padre, y TmpNodo2 se usa para comprobar si existe o no el nodo especificado)
	for (size_t i = 0; i < nSplit.Total() ; i++) {
		if (i != nSplit.Total() - 1) {
			PathActual = PathActual + nSplit[i] + L"\\";
			TmpNodo2 = Arbol.BuscarHijoTxt(nSplit[i], TmpNodo);
			// El nodo no existe, lo creo
			if (TmpNodo2 == NULL && i != nSplit.Total() - 1) {
				TmpNodo = Arbol_AgregarDir(&PathActual);
				Arbol.Expandir(TmpNodo, TRUE);
			}
			// El nodo si que existe, asigno TmpNodo2 a TmpNodo
			else {
				TmpNodo = TmpNodo2;
				Arbol.Expandir(TmpNodo, TRUE);
			}
		}
		else {
			std::wstring FiltroNombre;

			if (mMedio.Pista() < 10)	{ FiltroNombre = L"0" + std::to_wstring(mMedio.Pista()) + L" " + mMedio.Nombre(); }
			else						{ FiltroNombre = std::to_wstring(mMedio.Pista()) + L" " + mMedio.Nombre(); }

			TmpNodo2 = Arbol.BuscarHijoTxt(FiltroNombre, TmpNodo);
			Arbol.DesSeleccionarTodo();
			Arbol.SeleccionarNodo(TmpNodo2, TRUE);
			Arbol.NodoMarcado(TmpNodo2);
			Arbol.MostrarNodo(TmpNodo2);
		}
	}
	return TRUE;
}

const BOOL VentanaPrincipal::Arbol_MostrarMedio(const sqlite3_int64 Hash) {
	BDMedio mMedio;
	// Si el medio existe lo muestro
	if (App.BD.ObtenerMedio(Hash, mMedio) == TRUE) {
		Arbol_MostrarMedio(mMedio);
		return TRUE;
	}
	// El medio no existe en la BD (lo que no debería pasar nunca)
	return FALSE;
}

void VentanaPrincipal::AnalizarBD(void) {
	// ya se está analizando
	if (Menu_ArbolBD.Menu(7)->Activado() == FALSE) return;	
	// Desactivo el menú analizar
	Menu_ArbolBD.Menu(7)->Activado(FALSE);
	Menu_BotonArbolBD.Menu(1)->Activado(FALSE);
	// Inicio el thread del analisis
	ThreadAnalizar.Iniciar(_hWnd);
}

void VentanaPrincipal::ActualizarArbol(void) {
	// Para evitar un posible dead lock miro si el menú está activado, y si no lo está es que se acaba de llamar a esta función y se está esperando a que termine el thread del analisis
	if (Menu_ArbolBD.Menu(6)->Activado() == FALSE) return;

	// Desactivo el menú actualizar
	Menu_ArbolBD.Menu(6)->Activado(FALSE);
	Menu_BotonArbolBD.Menu(0)->Activado(FALSE);

	// Envio la señal para cancelar el thread del analisis
	ThreadAnalizar.Cancelar(TRUE);
	// Espero a que se termine el thread del analisis
	while (ThreadAnalizar.Thread() != NULL) {
		App.Eventos_Mirar();
	}

	BarraTareas.Estado_Indeterminado();
	// Re-escaneo las unidades de disco
	App.BD.Unidades.Escanear_Unidades_Locales();
	//	if (_hWnd != NULL) return;
	Arbol.BorrarTodo();

	NodoBD *Tmp = NULL;

	for (size_t i = 0; i < App.BD.TotalRaices(); i++) {
		if (GetFileAttributes(App.BD.Raiz(i)->Path.c_str()) != INVALID_FILE_ATTRIBUTES) {
			Tmp = Arbol_AgregarRaiz(&App.BD.Raiz(i)->Path);
			Arbol.ExplorarPath(Tmp);
		}
	}

	Arbol.Repintar();

	// Inicio el thread de la busqueda
	ThreadActualizar.Iniciar(hWnd());
}


// Agrega un medio desde el explorador (haciendo dobleclick o desde el menú)
void VentanaPrincipal::ExploradorAgregarMedio(const BOOL Reproducir) {
	std::vector <std::wstring> Paths;
	// Obtengo los paths de la memoria compartida
	App.MemCompartida.ObtenerPaths(Paths);	
	// Si no hay paths salgo
	if (Paths.size() == 0) return;

	// Elimino los paths de la memoria compartida
	App.MemCompartida.EliminarPaths();

	sqlite3_int64 Hash = 0; 
	BDMedio Medio;
	for (size_t i = 0; i < Paths.size(); i++) {
		if (App.BD.ObtenerMedio(Paths[i], Medio) == FALSE) {
			App.BD.AnalizarMedio(Paths[i], Medio, 0);
			// Necesito volver a obtener el medio para que me devuelva su ID
			App.BD.ObtenerMedio(Medio.Hash, Medio);
//				return;
		}

		Debug_Escribir_Varg(L"VentanaPrincipal::ExploradorAgregarMedio : %s\n", Paths[i].c_str());
		Lista.AgregarMedio(&Medio);
		// Guardo el hash del primer medio, para asignarlo como medio actual si es necesario
		if (i == 0) Hash = Medio.Hash;		
	}
	Lista.Repintar();

	if (Reproducir == TRUE) {
		Lista.MedioActual = Lista.BuscarHash(Hash);
		//Lista_Stop();
		App.MP.CerrarMedio();
		Lista_Play();
	}
}


void VentanaPrincipal::Evento_BarraEx_Cambiando(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_SLIDER_VOLUMEN: 					Evento_SliderVolumen_Cambiando();				break;
	}
}

void VentanaPrincipal::Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_SLIDER_TIEMPO:						Evento_SliderTiempo_Cambiado();					break;
		case ID_SLIDER_VOLUMEN: 					Evento_SliderVolumen_Cambiado();				break;
	}
}


void VentanaPrincipal::ThreadAgregarArchivosLista_Terminado(void) {
	ThreadArchivosLista.Terminar();
	BarraTareas.Estado_SinProgreso();
	//			BarraTareas.Resaltar();
	// Ejecuto el shufle si es necesario
	if (App.Opciones.Shufle() == TRUE) {
		App.VentanaRave.Lista.Mezclar(TRUE);
		App.VentanaRave.Lista.MedioActual = 0;
	}
	Lista.Repintar();
	if (App.MP.ComprobarEstado() != EnPlay)	Lista_Play();
	//			Lista_Stop();
	//			Lista_Play();			
	if (App.Opciones.AnalizarMediosPendientes() == TRUE) AnalizarBD();
}

void VentanaPrincipal::ThreadAgregarArchivosLista_AgregarMedio(WPARAM wParam) {
	BDMedio *Medio = reinterpret_cast<BDMedio *>(wParam);
	Lista.AgregarMedio(Medio);
	delete Medio;
}


void VentanaPrincipal::ThreadABuscarArchivos_AgregarRaiz(LPARAM lParam) {
	std::wstring *TmpStr = reinterpret_cast<std::wstring *>(lParam);
	Arbol_AgregarRaiz(TmpStr);
	delete TmpStr; // Hay que borrar de memória el path (se crea en el thread BuscarArchivos y ya no es necesario)
}

void VentanaPrincipal::ThreadABuscarArchivos_AgregarDirectorio(LPARAM lParam) {
	std::wstring *TmpStr = reinterpret_cast<std::wstring *>(lParam);
	Arbol_AgregarDir(TmpStr, TRUE);
	delete TmpStr; // Hay que borrar de memória el path (se crea en el thread BuscarArchivos y ya no es necesario)
}

void VentanaPrincipal::ThreadABuscarArchivos_Terminado(const BOOL Cancelado, LPARAM lParam) {
	ThreadActualizar.Terminar();
	Menu_ArbolBD.Menu(6)->Activado(TRUE);  // Menu actualizar
	Menu_BotonArbolBD.Menu(0)->Activado(TRUE);
	BarraTareas.Estado_SinProgreso();
	//			BarraTareas.Resaltar();
	if (Cancelado == FALSE) {
		Debug_Escribir_Varg(L"ThreadActualizarArbol::Terminado %d archivos encontrados.\n", lParam);
		App.MostrarToolTipPlayer(L"Arbol actualizado.");
		// Si la opción de analizar medios pendientes está activa
		if (App.Opciones.AnalizarMediosPendientes() == TRUE) AnalizarBD();
	}
	else {
		Debug_Escribir_Varg(L"ThreadActualizarArbol::Cancelado %d archivos encontrados.\n", lParam);
		App.MostrarToolTipPlayer(L"Actualización del arbol Cancelada.");
	}
}


void VentanaPrincipal::ThreadAnalizar_Terminado(const BOOL Cancelado, LPARAM lParam) {
	ThreadAnalizar.Terminar();
	BarraTareas.Estado_SinProgreso();
	Menu_ArbolBD.Menu(7)->Activado(TRUE); // Menu analizar
	Menu_BotonArbolBD.Menu(1)->Activado(TRUE);

	if (Cancelado == FALSE) {
		App.BD.ObtenerEtiquetas();
		Debug_Escribir_Varg(L"ThreadAnalisis::Terminado %d archivos examinados.\n", lParam);
		App.MostrarToolTipPlayer(L"Análisis terminado, se han analizado " + std::to_wstring(lParam) + L" medios.");
	}
	else {
		Debug_Escribir_Varg(L"ThreadAnalisis::Cancelado %d total archivos a examinar.\n", lParam);
		App.MostrarToolTipPlayer(L"Análisis cancelado.");
	}
}

void VentanaPrincipal::ThreadAnalizar_MostrarVentana(void) {
	ShowWindow(ThreadAnalizar.hWnd(), SW_SHOW);
}

void VentanaPrincipal::ThreadAnalizar_Iniciado2(WPARAM wParam) {
	_MaximoTotalMedios2 = static_cast<UINT>(wParam);
	_ValorMedios2 = 0;
}

void VentanaPrincipal::ThreadAnalizar_TotalMedios(WPARAM wParam, LPARAM lParam) {
	BarraTareas.Valor(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
}

void VentanaPrincipal::ThreadAnalizar_TotalMedios2(void) {
	BarraTareas.Valor(++_ValorMedios2, _MaximoTotalMedios2);
}

void VentanaPrincipal::Actualizacion_DescargaEncontrada(const wchar_t* nVersion) {
	Debug_Escribir_Varg(L"VentanaPrincipal::Actualizacion_DescargaEncontrada  Nueva versión %s encontrada.\n", nVersion);
	App.Actualizacion.Descargar();
}

void VentanaPrincipal::Actualizacion_MostrarDescargar(const wchar_t *nVersion) {
	Debug_Escribir_Varg(L"VentanaPrincipal::Actualizacion_Descargar  Nueva versión %s encontrada.\n", nVersion);
	App.VentanaDescargarAct.Crear(nVersion);
}

// Función que mantiene la barra de descarga de la actualización
void VentanaPrincipal::Actualizacion_Barra(const float nValor) {
	if (App.VentanaDescargarAct.hWnd() == 0) return;
	App.VentanaDescargarAct.Barra.Valor(nValor);
	App.VentanaDescargarAct.Barra.Repintar();
}

// Función que muestra un mensaje de error de la actualización
void VentanaPrincipal::Actualizacion_Error(void) {
	App.VentanaDescargarAct.Destruir();
	App.MostrarToolTipPlayerError(L"Error descargando la actualización...");
}

// Función que muestra un mensaje conforme se ha cancelado la actualización
void VentanaPrincipal::Actualizacion_Cancelada(void) {
	App.MostrarToolTipPlayer(L"Descarga de la actualización cancelada.");
}

// Función que avisa al usuario de que se ha descargado la actualización
void VentanaPrincipal::Actualizacion_Descargada(void) {
	App.MostrarToolTipPlayer(L"Descarga de la actualización completada.");
	App.VentanaDescargarAct.Destruir();
	
	App.VentanaInstalarAct.Crear();
}

// Función que muestra la ventana que pide al usuario instalar la actualización
void VentanaPrincipal::Actualizacion_Existente(void) {
	App.VentanaInstalarAct.Crear();
}


void VentanaPrincipal::_Evento_Size(void) {
	RECT RCWMS;
	GetClientRect(hWnd(), &RCWMS);
	AjustarControles(RCWMS);
//	Debug_Escribir_Varg(L"WM_SIZE %d, %d\n", wParam, lParam);
}

void VentanaPrincipal::_Evento_ExitSizeMove(void) {
	if (App.VentanaRave.Maximizada() == FALSE) {
		App.Opciones.GuardarPosTamVentana();
	}
//	Debug_Escribir_Varg(L"WM_EXITSIZEMOVE %d, %d\n", wParam, lParam);
}

LRESULT CALLBACK VentanaPrincipal::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		// Mensajes desde el explorador de windows
		case WM_AGREGARMEDIO				:	ExploradorAgregarMedio(FALSE);															return 0;
		case WM_REPRODUCIRMEDIO				:	ExploradorAgregarMedio(TRUE);															return 0;

		// Para el thread que busca las actualizaciones
		case WM_ACTUALIZACION_ENCONTRADA	:	Actualizacion_DescargaEncontrada(reinterpret_cast<wchar_t*>(wParam));					return 0;
		case WM_ACTUALIZACION_EXISTENTE		:	Actualizacion_Existente();																return 0;
		case WM_ACTUALIZACION_MOSTRAR		:	Actualizacion_MostrarDescargar(reinterpret_cast<wchar_t*>(wParam));						return 0;
		case WM_ACTUALIZACION_BARRA			:	Actualizacion_Barra(reinterpret_cast<float &>(wParam));									return 0;
		case WM_ACTUALIZACION_ERROR			:	Actualizacion_Error();																	return 0;
		case WM_ACTUALIZACION_CANCELADA		:	Actualizacion_Cancelada();																return 0;
		case WM_ACTUALIZACION_DESCARGADA	:	Actualizacion_Descargada();																return 0;

		// Para el Thread AgregarArchivosLista, al agregar/obtener el archivo en la BD, ya se puede agregar a la lista.
		case WM_TAAL_AGREGARMEDIO			:	ThreadAgregarArchivosLista_AgregarMedio(wParam);										return 0;
		case WM_TAAL_TERMINADO				:	ThreadAgregarArchivosLista_Terminado();													return 0;

		// ThreadBuscarArchivos
		case WM_TBA_AGREGARRAIZ				:	ThreadABuscarArchivos_AgregarRaiz(lParam);												return 0;
		case WM_TBA_AGREGARDIR				:	ThreadABuscarArchivos_AgregarDirectorio(lParam);										return 0;
		case WM_TBA_TERMINADO				:	ThreadABuscarArchivos_Terminado(FALSE, lParam);											return 0;
		case WM_TBA_CANCELADO				:	ThreadABuscarArchivos_Terminado(TRUE, lParam);											return 0;

		// ThreadAnalizar
		case WM_TOM_INICIADO2				:	ThreadAnalizar_Iniciado2(wParam);														return 0;
		case WM_TOM_TOTALMEDIOS1			:	ThreadAnalizar_TotalMedios(wParam, lParam);												return 0;
		case WM_TOM_TOTALMEDIOS2			:	ThreadAnalizar_TotalMedios2();															return 0;
		case WM_TOM_TOTALMEDIOS3			:	ThreadAnalizar_TotalMedios(wParam, lParam);												return 0;
		case WM_TOM_CANCELADO				:	ThreadAnalizar_Terminado(TRUE, lParam);													return 0;
		case WM_TOM_TERMINADO				:	ThreadAnalizar_Terminado(FALSE, lParam);												return 0;
		case WM_TOM_MOSTRARVENTANA			:	ThreadAnalizar_MostrarVentana();														return 0;

		// Drag & Drop
		case WM_DROPFILES					:	Evento_SoltarArchivos(wParam);															return 0;
		// Evento cerrar ventana
		case WM_CLOSE						:	Evento_Cerrar();																		return 0;

		// MenuEx
		case DWL_MENUEX_CLICK				:	Evento_MenuEx_Click(LOWORD(wParam));													return 0;
		case DWL_MENUEX_BARRA_CAMBIADO		:	Evento_MenuEx_Barra_Cambiado(LOWORD(wParam), static_cast<float>(lParam) / 100.0f);		return 0;
		case DWL_MENUEX_BARRA_CAMBIANDO		:	Evento_MenuEx_Barra_Cambiando(LOWORD(wParam), static_cast<float>(lParam) / 100.0f);		return 0;
		
		// Mover y redimensionar la ventana
		case WM_EXITSIZEMOVE				:	_Evento_ExitSizeMove();																	return 0;
		case WM_SIZE						:	_Evento_Size();																			return 0;
		case WM_SIZING						:	Evento_CambiandoTam(static_cast<UINT>(wParam), reinterpret_cast<RECT *>(lParam));		return 0;
		case WM_MOVING						:	App.OcultarToolTipPlayer();																return TRUE;

		case WM_ERASEBKGND					:	Evento_BorraFondo(reinterpret_cast<HDC>(wParam));										return TRUE;

		// Barra de desplazamiento (barra de tiempo y volumen) 
		case DWL_BARRAEX_CAMBIANDO			:	Evento_BarraEx_Cambiando(WPARAM_TO_DEVENTOMOUSE(wParam));								return 0;	// Se está modificando (mouse down)		
		case DWL_BARRAEX_CAMBIADO			:  	Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));								return 0;	// Se ha modificado	(mouse up)

		// Mensajes para el BotonEx
		case DWL_BOTONEX_CLICK				:	Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));								return 0;
		case DWL_BOTONEX_MOUSEUP			:	Evento_BotonEx_Mouse_Soltado(WPARAM_TO_DEVENTOMOUSE(wParam));							return 0;
		case DWL_BOTONEX_MOUSEDOWN			:	Evento_BotonEx_Mouse_Presionado(WPARAM_TO_DEVENTOMOUSE(wParam));						return 0;

		// Para los botones de la barra de tareas
		case WM_COMMAND						: 	Evento_Comando(wParam, lParam);															return 0;
		
		default :
			if (uMsg == BarraTareas.WM_TASK_BUTTON_CREATED()) {
				CrearBotonesThumb();
				return 0;
			}
			break;

	}

	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}