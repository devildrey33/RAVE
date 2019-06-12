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



/* TODO : Moure els butons de l'esquerra a la dreta, y fer que la llista / bd / opcions / video quedin ajustats a l'esquerra per evitar els flickerings dels scrolls de la llista i l'arbre */
HWND VentanaPrincipal::Crear(int nCmdShow) {
	// Detección de monitores
	EnumDisplayMonitors(NULL, NULL, VentanaPrincipal::EnumerarPantallas, NULL);
	// Si el monitor donde se guardo la ultima posición no está disponible busco una nueva posición
	if (MonitorDisponible == FALSE) {
		App.BD.Opciones_AsignarPosVentana(RectMonitorActual.left + 100, RectMonitorActual.top + 100);
	}

	int ancho = (App.BD.Opciones_Ancho() > RAVE_MIN_ANCHO) ? App.BD.Opciones_Ancho() : RAVE_MIN_ANCHO;
	HWND rhWnd = DVentana::CrearVentana(NULL, L"RAVE_VentanaPrincipal", RAVE_TITULO, App.BD.Opciones_PosX(), App.BD.Opciones_PosY(), ancho, App.BD.Opciones_Alto(), WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
	
	RECT RC; // , RW;
	GetClientRect(hWnd(), &RC);

	Arbol.CrearArbolEx(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_ARBOLBD, WS_CHILD | WS_VISIBLE);
	SetWindowLongPtr(Arbol.hWnd(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
//	Arbol.SubSeleccion = TRUE;
//	Arbol.Visible(TRUE);

	Lista.CrearListaEx(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_LISTAMEDIOS, WS_CHILD);
	SetWindowLongPtr(Lista.hWnd(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
	Lista.MultiSeleccion = TRUE;
	Lista.MoverItemsDrag = TRUE;
	// Columnas
	Lista.AgregarColumna(50, DListaEx_Columna_Alineacion_Derecha);								// Icono y pista
	Lista.AgregarColumna(DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion_Izquierda);	// Nombre
	Lista.AgregarColumna(60, DListaEx_Columna_Alineacion_Derecha);								// Tiempo

	
	Video.Crear(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_VERVIDEO);

	// Marco superior izquierdo /////////////
	MarcoSI.Crear(this, 10, 10, 380, 30, ID_MARCOSI);
	BotonAtras.CrearBotonEx(&MarcoSI,	 IDI_PREV32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,   0, 0, 30, 30, ID_BOTON_ANTERIOR);
	BotonPlay.CrearBotonEx(&MarcoSI,	 IDI_PLAY32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  40, 0, 30, 30, ID_BOTON_PLAY);
//	BotonPausa.CrearBotonEx(&MarcoSI,	 IDI_PAUSA32, 32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  80, 0, 30, 30, ID_BOTON_PAUSA);
	BotonStop.CrearBotonEx(&MarcoSI,	 IDI_STOP32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  80, 0, 30, 30, ID_BOTON_STOP);
	BotonAdelante.CrearBotonEx(&MarcoSI, IDI_NEXT32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 120, 0, 30, 30, ID_BOTON_SIGUIENTE);

	LabelRatio.CrearEtiquetaEx(&MarcoSI, L"x1.0", 160, 5, 40, 30, ID_LABEL_RATIO, DEtiquetaEx_Alineacion_Centrado);

	BotonMezclar.CrearBotonEx(&MarcoSI, L"Mezclar", 210, 0, 70, 30, ID_BOTON_MEZCLAR);
	BotonMezclar.Fuente.CrearFuente(18, DBotonEx_Skin::FuenteNombre.c_str(), TRUE);
	if (App.BD.Opciones_Shufle() == TRUE) BotonMezclar.Marcado(TRUE);
	BotonRepetir.CrearBotonEx(&MarcoSI, L"Repetir", 290, 0, 70, 30, ID_BOTON_REPETIR);
	BotonRepetir.Fuente.CrearFuente(18, DBotonEx_Skin::FuenteNombre.c_str(), TRUE);
	if (App.BD.Opciones_Repeat() > 0) BotonRepetir.Marcado(TRUE);
	//////////////////////////////////////////

	SliderTiempo.CrearBarraDesplazamientoEx(this, 10, 50, RC.right - 20, 20, ID_SLIDER_TIEMPO);
//	SliderTiempo.Crear(hWnd, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO, WS_CHILD | TBS_NOTICKS | WS_VISIBLE, 0, 30000, 0);
//	SliderTiempo.TamPagina(30000 / 50);

	// Marco superior derecho
	MarcoSD.Crear(this, RC.right - 260, 16, 250, 24, ID_MARCOSD);

	SliderVolumen.CrearBarraVolumen(&MarcoSD, 120, 3, 90, 17, ID_SLIDER_VOLUMEN, 0, 200, static_cast<float>(App.BD.Opciones_Volumen()));
	std::wstring TxtVolumen = std::to_wstring(App.BD.Opciones_Volumen()) + L"%";
	LabelVolumen.CrearEtiquetaEx(&MarcoSD, TxtVolumen.c_str(), 215, 1, 40, 20, ID_LABEL_VOLUMEN, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);

	LabelTiempoActual.CrearEtiquetaEx(&MarcoSD, L"00:00", 0, 1, 55, 20, ID_LABEL_TIEMPOACTUAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);
	LabelTiempoSeparador.CrearEtiquetaEx(&MarcoSD, L"/", 55, 1, 10, 20, ID_LABEL_TIEMPOSEPARADOR, DEtiquetaEx_Alineacion_Centrado,  WS_CHILD | WS_VISIBLE);
	LabelTiempoTotal.CrearEtiquetaEx(&MarcoSD, L"00:00", 65, 1, 55, 20, ID_LABEL_TIEMPOTOTAL, DEtiquetaEx_Alineacion_Centrado, WS_CHILD | WS_VISIBLE);

	// Marco inferior izquierdo /////////////
	MarcoII.Crear(this, 10, 80, RAVE_BOTONES_LATERALES_ANCHO, 2000, ID_MARCOSI);
	BotonBD.CrearBotonEx(&MarcoII, L"Base de datos", 0, 0, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_BD);
	BotonBD.Marcado(TRUE);
	BotonLista.CrearBotonEx(&MarcoII, L"Lista de medios", 0, 35, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_LISTA);
	BotonVideo.CrearBotonEx(&MarcoII, L"Ver video", 0, 70, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_VIDEO);
	
	BotonOpciones.CrearBotonEx(&MarcoII, L"Opciones", 0, RC.bottom - 120, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_OPCIONES);
	
	#ifdef RAVE_VLC_DOBLE_MEDIO_FFT
		Vis.Crear(&MarcoII, 0, 160, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_VISUALIZACION);
	#endif
	/////////////////////////////////////////


	// Inicio la VLC justo antes de mostrar la ventana y de activar el thread para actualizar el arbol
	App.MP.Iniciar();


	// Muestro la ventana principal
	ShowWindow(hWnd(), nCmdShow);

	// Timer que comprueba si se ha terminado la cancion
//	SetTimer(hWnd(), TIMER_LISTA, 500, NULL);
	// Timer para el slider del tiempo
//	SetTimer(hWnd(), TIMER_TIEMPO, 250, NULL);

	// Habilito el drag & drop para esta ventana
	DragAcceptFiles(hWnd(), TRUE);	

	return rhWnd;
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
	// 50 ?? 60

	MoveWindow(Lista.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);
	MoveWindow(Arbol.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);
	MoveWindow(Video.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);

	MoveWindow(SliderTiempo.hWnd(), 10, 50, RC.right - 20, 20, TRUE);
	MoveWindow(MarcoSD.hWnd(), RC.right - 260, 14, 255, 24, TRUE);

	MoveWindow(BotonOpciones.hWnd(), 0, RC.bottom - 120, RAVE_BOTONES_LATERALES_ANCHO, 30, TRUE);

	//App.VLC.RepintarVLC();
	if (App.MP.hWndVLC != NULL) {
		InvalidateRect(App.MP.hWndVLC, &RC, TRUE);
//		App.VLC.RepintarVLC();
	}
//	RedrawWindow(App.VLC.hWndVLC, &RC, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);

//	RedrawWindow(hWnd(), NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
	
	/*	MoveWindow(SliderVolumen.hWnd(),	RC.right - 160, 10, 100, 24, TRUE);
	
	MoveWindow(LabelVolumen.hWnd(),			RC.right - 50, 12, 40, 20, TRUE);
	MoveWindow(LabelTiempoActual.hWnd(),	RC.right - 260, 12, 50, 20, TRUE);
	MoveWindow(LabelTiempoSeparador.hWnd(), RC.right - 210, 12, 10, 20, TRUE);
	MoveWindow(LabelTiempoTotal.hWnd(),		RC.right - 200, 12, 50, 20, TRUE);*/
}


BOOL CALLBACK VentanaPrincipal::EnumerarPantallas(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	POINT Pt = { App.BD.Opciones_PosX(), App.BD.Opciones_PosY() };
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
	switch (App.BD.Opciones_Repeat()) {
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
/*
void VentanaPrincipal::Timer_ObtenerTiempoTotal(void) {
	// Actualizo el tiempo del medio actual si es 00:00
	ItemMedio * rItem = Lista.Medio(Lista.MedioActual);
	UINT64 TiempoMS = App.VLC.TiempoTotalMs();
	if (Lista.MedioActual != -1 && Lista.TotalItems() != 0 && TiempoMS != 0) {
		std::wstring TextoItem = Lista.Item(Lista.MedioActual)->Texto(2);
		if (TextoItem == L"00:00") {
			App.VLC.TiempoStr(TiempoMS, TextoItem);
			rItem->Texto(2, TextoItem);
			Lista.Repintar();

//			BDMedio nMedioActual;
//			App.BD.ObtenerMedio(rItem->Hash, nMedioActual);
			App.BD.AsignarTiempoMedio(static_cast<INT64>(TiempoMS), rItem->Hash);

//			nMedioActual.Tiempo = static_cast<INT64>(TiempoMS);

		}
		KillTimer(_hWnd, TIMER_OBTENER_TIEMPO_TOTAL);
	}

}*/


void VentanaPrincipal::Lista_Pausa(void) {
	BotonPlay.Icono(IDI_PLAY32, 32);
	App.ControlesPC.BotonPlay.Icono(IDI_PLAY32, 32);
	BarraTareas.Boton_Icono(ID_BOTON_PLAY, IDI_PLAY32);

	App.MP.Pausa();
}


void VentanaPrincipal::Lista_Play(void) {
	if (Lista.TotalItems() == 0) return;

	if (Lista.MedioActual == NULL) {
		Lista.MedioActual = Lista.MedioSiguiente(NULL);
	}

	Lista.Errores = 0;
	BDMedio NCan, NCan2;
	ItemMedio *IMS = NULL;
	switch (App.MP.ComprobarEstado()) {
		case SinCargar:
			// Compruebo que el medio actual no sea mas grande que el total de medios
			//if (Lista.PosMedio(Lista.MedioActual) > Lista.TotalItems() - 1) Lista.MedioActual = Lista.Medio(Lista.TotalItems() - 1);
			App.BD.ObtenerMedio(Lista.MedioActual->Hash, NCan);
			IMS = Lista.MedioSiguiente(Lista.MedioActual);
			if (IMS != NULL) {
				App.BD.ObtenerMedio(IMS->Hash, NCan2);
				if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
			}
			else {
				if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
			}
//				NCan.Obtener(App.BD(), Lista.Medio(Lista.MedioActual)->Hash);
			//if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
			if (App.MP.Play() == TRUE) {
				BotonPlay.Icono(IDI_PAUSA32, 32);
				App.ControlesPC.BotonPlay.Icono(IDI_PAUSA32, 32);
				BarraTareas.Boton_Icono(ID_BOTON_PLAY, IDI_PAUSA32);
			}
			break;
		case Terminada:
//			App.MP.ActualizarIconos(0);
			Lista.MedioActual = Lista.MedioSiguiente(Lista.MedioActual);
			App.BD.ObtenerMedio(Lista.MedioActual->Hash, NCan);
			IMS = Lista.MedioSiguiente(Lista.MedioActual);
//			NCan.Obtener(App.BD(), Lista.Medio(Lista.MedioActual)->Hash);
			if (IMS == NULL) {
				if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
			}
			else {
				App.BD.ObtenerMedio(IMS->Hash, NCan2);
				if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
			}
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
//	if (Lista.MedioActual >= Lista.TotalItems()) Lista.MedioActual = 0;


	BDMedio NCan, NCan2;
	App.MP.Stop();
	App.BD.ObtenerMedio(Lista.MedioActual->Hash, NCan);
	ItemMedio *IMS = Lista.MedioSiguiente(Lista.MedioActual);
	if (IMS == NULL) {
		if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
	}
	else {
		App.BD.ObtenerMedio(IMS->Hash, NCan2);
		if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
	}
	App.MP.Play();
}


void VentanaPrincipal::Lista_Anterior(void) {
	if (Lista.TotalItems() == 0) return;

	LONG_PTR TotalItems = Lista.TotalItems() - 1;
	if (TotalItems == -1) TotalItems = 0;
	Lista.MedioActual = Lista.MedioAnterior(Lista.MedioActual);
/*	if (Lista.MedioActual < 0) Lista.MedioActual = TotalItems;

	if (Lista.MedioActual >= 0 && Lista.MedioActual <= TotalItems) {*/
	BDMedio NCan, NCan2;
	ItemMedio *IMS = Lista.MedioSiguiente(Lista.MedioActual);
	App.BD.ObtenerMedio(Lista.MedioActual->Hash, NCan);
	App.MP.Stop();
	if (IMS == NULL) {
		if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Lista.Errores++;
	}
	else {
		App.BD.ObtenerMedio(IMS->Hash, NCan2);
		if (App.MP.AbrirMedio(NCan, &NCan2) == FALSE) Lista.Errores++;
	}
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
	App.BD.ObtenerMedio(Lista.MedioMarcado()->Hash, nMedio);
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
	App.BD.ObtenerMedio(Lista.MedioMarcado()->Hash, nMedio);
	PIDLIST_ABSOLUTE pidl;
	pidl = ILCreateFromPath(nMedio.Path.c_str());
	if (pidl) {
		SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
		ILFree(pidl);
	}

}

void VentanaPrincipal::Lista_MostrarEnBaseDatos(void) {
	Arbol_MostrarMedio(Lista.MedioMarcado()->Hash);
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
				Lista.MedioActual = Lista.MedioAnterior(Lista.MedioActual, FALSE);
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
	App.BD.ObtenerMedio(Lista.MedioMarcado()->Hash, Medio);
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
	NodoBD *Tmp = static_cast<NodoBD *>(Arbol.NodoRaiz());
	while (Tmp != NULL) {
		if (Tmp->Seleccionado == TRUE) {
			Arbol.AgregarNodoALista(Tmp);
			App.MostrarToolTipPlayer(L"\"" + Tmp->Texto + nTexto);
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


void VentanaPrincipal::Evento_MenuEx_Click(const UINT cID) {
	switch (cID) {
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
		if (App.BD.Opciones_GuardarBSCP() == TRUE)	App.BD.ActualizarMedio(&App.MP.MedioActual());
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
			if (App.BD.Opciones_GuardarBSCP() == TRUE)		App.BD.ActualizarMedio(&App.MP.MedioActual());
			return;
		case ID_MENUVIDEO_CONTRASTE				:	
			App.MP.Contraste(ValorBarra);		
			if (App.BD.Opciones_GuardarBSCP() == TRUE)		App.BD.ActualizarMedio(&App.MP.MedioActual());
			return;
		case ID_MENUVIDEO_SATURACION			:	
			App.MP.Saturacion(ValorBarra);			
			if (App.BD.Opciones_GuardarBSCP() == TRUE)		App.BD.ActualizarMedio(&App.MP.MedioActual());
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

void VentanaPrincipal::Mezclar_Click(void) {
	BOOL nMezclar = !App.BD.Opciones_Shufle();
	Lista.Mezclar(nMezclar);
	App.BD.Opciones_Shufle(nMezclar);
	BotonMezclar.Marcado(nMezclar);
	App.ControlesPC.BotonMezclar.Marcado(nMezclar);
}
/*
void VentanaPrincipal::AgregarRaiz(void) {
	DDlgDirectorios          DialogoDirectorios;
	std::wstring             Path;

	BOOL Ret = DialogoDirectorios.Mostrar(this, Path);
//	SetFocus(_hWnd);
	if (Ret == TRUE) {
		// Agrego la raíz a la BD.
//		BDRaiz *nRaiz = NULL;
		// Puede que esa raíz sea parte de otra raíz existente o viceversa, en ese caso no se agrega una nueva raíz a la lista, habrá que modificar la lista
		if (App.BD.AgregarRaiz(Path) != NULL) {
			ListaRaiz.AgregarRaiz(Path.c_str());
			Debug_Escribir(L"VentanaPrincipal::AgregarRaiz Nueva raíz agregada.\n");
		}
		else { // La raiz ya existia o es un error
			Debug_Escribir(L"VentanaPrincipal::AgregarRaiz La raíz se ha fusionado a una raíz existente.\n");
		}
	}
	else {
		Debug_Escribir(L"VentanaPrincipal::AgregarRaiz Cancelado por el usuario.\n");
	}
}

void VentanaPrincipal::EliminarRaiz(std::wstring &Path) {
	App.BD.EliminarRaiz(Path);
}*/


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
			case ID_REPETIR_NO				:	App.BD.Opciones_Repeat(Tipo_Repeat_NADA);					break;
			case ID_REPETIR_SI				:	App.BD.Opciones_Repeat(Tipo_Repeat_RepetirLista);			break;
			case ID_REPETIR_SI_MEZCLAR		:	App.BD.Opciones_Repeat(Tipo_Repeat_RepetirListaShufle);		break;
			case ID_REPETIR_SI_APAGAR_REP	:	App.BD.Opciones_Repeat(Tipo_Repeat_ApagarReproductor);		break;	// No se guarda en la BD
			case ID_REPETIR_SI_APAGAR_WIN	:	App.BD.Opciones_Repeat(Tipo_Repeat_ApagarOrdenador);		break;	// No se guarda en la BD
			case ID_REPETIR_GENERAR         :
			case ID_REPETIR_LOQUESEA        :	App.BD.Opciones_Repeat(Tipo_Repeat_GenerarLoQueSea);		break;
			case ID_REPETIR_GENERO			:	App.BD.Opciones_Repeat(Tipo_Repeat_GenerarGenero);			break;
			case ID_REPETIR_GRUPO			:	App.BD.Opciones_Repeat(Tipo_Repeat_GenerarGrupo);			break;
			case ID_REPETIR_DISCO			:	App.BD.Opciones_Repeat(Tipo_Repeat_GenerarDisco);			break;
			case ID_REPETIR_50CANCIONES     :   App.BD.Opciones_Repeat(Tipo_Repeat_Generar50Canciones);		break;
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
			App.BD.Opciones_AsignarPosVentana(RectMonitorActual.left + 100, RectMonitorActual.top + 100);
		}

		App.ControlesPC._AniMostrar.Terminar();

		ShowWindow(hWnd(), SW_RESTORE);
//		GetClientRect(hWnd(), &RC);
//		BOOL R = App.VentanaRave.BarraTareas.Clip(&RC);
		SetWindowLongPtr(hWnd(), GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetWindowPos(_hWnd, HWND_TOP, App.BD.Opciones_PosX(), App.BD.Opciones_PosY(), App.BD.Opciones_Ancho(), App.BD.Opciones_Alto(), SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
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

//		EstilosEx = GetWindowLongPtr(Arbol.hWnd(), GWL_EXSTYLE);
//		BOOL T = EstilosEx & WS_EX_LAYERED;

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
//		AsignarFoco();
	}

	InvalidateRect(App.MP.hWndVLC, NULL, TRUE);
//	SetTimer(hWnd(), TIMER_REPINTARVLC, 1000, NULL);
//	PostMessage(Video.hWnd(), WM_PAINT, 0, 0);
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

void VentanaPrincipal::Evento_SliderVolumen_Cambio(void) {
	int Volumen = static_cast<int>(SliderVolumen.Valor());	
	App.MP.Volumen(Volumen);
}

// Guardo el volumen en las opciones (al soltar el slider del volumen)
void VentanaPrincipal::Evento_SliderVolumen_Cambiado(void) {
	App.BD.Opciones_Volumen(static_cast<int>(SliderVolumen.Valor()));
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
//	App.BD.Opciones_GuardarOpciones();
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
//				Arbol.Evento_Nodo_Expandido(TmpNodo, TRUE);
			}
			// El nodo si que existe, asigno TmpNodo2 a TmpNodo
			else {
				TmpNodo = TmpNodo2;
				Arbol.Expandir(TmpNodo, TRUE);
//				Arbol.Evento_Nodo_Expandido(TmpNodo, TRUE);
			}
		}
		else {
			std::wstring FiltroNombre;
//			RaveBD::FiltroNombre(nSplit[i], FiltroNombre);

			if (mMedio.Pista() < 10)	{ FiltroNombre = L"0" + std::to_wstring(mMedio.Pista()) + L" " + mMedio.Nombre(); }
			else						{ FiltroNombre = std::to_wstring(mMedio.Pista()) + L" " + mMedio.Nombre(); }


			TmpNodo2 = Arbol.BuscarHijoTxt(FiltroNombre, TmpNodo);
			Arbol.DesSeleccionarTodo();
			Arbol.SeleccionarNodo(TmpNodo2, TRUE);
			Arbol.NodoMarcado(TmpNodo2);
			Arbol.MostrarNodo(TmpNodo2);

//			Arbol.Evento_Nodo_Expandido(TmpNodo, TRUE);
		}
	}
//	Arbol.MostrarNodo(TmpNodo);
//	Arbol.Repintar();
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
		Tmp = Arbol_AgregarRaiz(&App.BD.Raiz(i)->Path);
		Arbol.ExplorarPath(Tmp);
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
/*		ItemMedio *Item = Lista.BuscarHash(Medio.Hash);
		for (LONG_PTR i = 0; i < Lista.TotalItems(); i++) {
			if (Item == Lista.Medio(i)) {
				Lista.MedioActual = i;
				break;
			}
		}		*/
		Lista.MedioActual = Lista.BuscarHash(Hash);
		//Lista_Stop();
		App.MP.CerrarMedio();
		Lista_Play();
	}
}
/*
void VentanaPrincipal::Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado) {
	
}

void VentanaPrincipal::Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado) {
	//DhWnd::Teclado[DatosTeclado.TeclaVirtual()] = false;
}

void VentanaPrincipal::Evento_Tecla(DWL::DEventoTeclado &DatosTeclado) {

}*/

void VentanaPrincipal::Evento_BarraEx_Cambiando(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_SLIDER_VOLUMEN: 					Evento_SliderVolumen_Cambio();					break;
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
	if (App.BD.Opciones_Shufle() == TRUE) {
		App.VentanaRave.Lista.Mezclar(TRUE);
		App.VentanaRave.Lista.MedioActual = 0;
	}
	Lista.Repintar();
	if (App.MP.ComprobarEstado() != EnPlay)	Lista_Play();
	//			Lista_Stop();
	//			Lista_Play();			
	if (App.BD.Opciones_AnalizarMediosPendientes() == TRUE) AnalizarBD();
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
		if (App.BD.Opciones_AnalizarMediosPendientes() == TRUE) AnalizarBD();
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


LRESULT CALLBACK VentanaPrincipal::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
//	std::wstring *TmpStr = NULL;
//	BDMedio *Medio = NULL;
	switch (uMsg) {
		case WM_AGREGARMEDIO :
			ExploradorAgregarMedio(FALSE);
			break;
		case WM_REPRODUCIRMEDIO :
			ExploradorAgregarMedio(TRUE);
			break;

		// Para el Thread AgregarArchivosLista, al agregar/obtener el archivo en la BD, ya se puede agregar a la lista.
		case WM_TAAL_AGREGARMEDIO	:			ThreadAgregarArchivosLista_AgregarMedio(wParam);								return 0;
		case WM_TAAL_TERMINADO		:			ThreadAgregarArchivosLista_Terminado();											return 0;

		// ThreadBuscarArchivos
		case WM_TBA_AGREGARRAIZ		:			ThreadABuscarArchivos_AgregarRaiz(lParam);										return 0;
		case WM_TBA_AGREGARDIR		:			ThreadABuscarArchivos_AgregarDirectorio(lParam);								return 0;
		case WM_TBA_TERMINADO		:			ThreadABuscarArchivos_Terminado(FALSE, lParam);									return 0;
		case WM_TBA_CANCELADO		:			ThreadABuscarArchivos_Terminado(TRUE, lParam);									return 0;

		// ThreadAnalizar
		case WM_TOM_INICIADO2		:			ThreadAnalizar_Iniciado2(wParam);												return 0;
		case WM_TOM_TOTALMEDIOS1	:			ThreadAnalizar_TotalMedios(wParam, lParam);										return 0;
		case WM_TOM_TOTALMEDIOS2	:			ThreadAnalizar_TotalMedios2();													return 0;
		case WM_TOM_TOTALMEDIOS3	:			ThreadAnalizar_TotalMedios(wParam, lParam);										return 0;
		case WM_TOM_CANCELADO		:			ThreadAnalizar_Terminado(TRUE, lParam);											return 0;
		case WM_TOM_TERMINADO		:			ThreadAnalizar_Terminado(FALSE, lParam);										return 0;
		case WM_TOM_MOSTRARVENTANA	:			ThreadAnalizar_MostrarVentana();												return 0;

		// Temporizadores con post message
//		case WM_TIMER_LISTA			:			App.MP.Temporizador_Lista();													return 0;
//		case WM_TIMER_TIEMPO		:			App.MP.Temporizador_Tiempo();													return 0;
//		case WM_MEDIO_TERMINADO		:           App.MP.Evento_Medio_Terminado(reinterpret_cast<RaveVLC_Medio *>(wParam));		return 0;

		case WM_DROPFILES :			
			Evento_SoltarArchivos(wParam);
			return 0;

		// Teclado
/*		case WM_KEYDOWN:		
			Evento_TeclaPresionada(DWL::DEventoTeclado(wParam, lParam, this));															
			break;	// Los eventos de teclado tienen que pasar a la clase super base para poder obtener el teclado general
		case WM_KEYUP:
			Evento_TeclaSoltada(DWL::DEventoTeclado(wParam, lParam, this));
			break;	// Los eventos de teclado tienen que pasar a la clase super base para poder obtener el teclado general
		case WM_CHAR:
			Evento_Tecla(DWL::DEventoTeclado(wParam, lParam, this));
			break;	// Los eventos de teclado tienen que pasar a la clase super base para poder obtener el teclado general*/



/*		case WM_TIMER:	
			this->Evento_Temporizador(static_cast<UINT>(wParam));
			return 0;*/
		case WM_CLOSE : 
			Evento_Cerrar();
			return 0;
		case DWL_MENUEX_CLICK :
			Evento_MenuEx_Click(LOWORD(wParam));
			return 0;
		case DWL_MENUEX_BARRA_CAMBIADO:
			Evento_MenuEx_Barra_Cambiado(LOWORD(wParam), static_cast<float>(lParam) / 100.0f);
			return 0;
		case DWL_MENUEX_BARRA_CAMBIANDO :
			Evento_MenuEx_Barra_Cambiando(LOWORD(wParam), static_cast<float>(lParam) / 100.0f);
			return 0;
		case WM_EXITSIZEMOVE  :
			if (App.VentanaRave.Maximizada() == FALSE) {
				App.BD.Opciones_GuardarPosTamVentana();
			}			
			Debug_Escribir_Varg(L"WM_EXITSIZEMOVE %d, %d\n", wParam, lParam);
			return 0;
		case WM_SIZE :
			RECT RCWMS;
			GetClientRect(hWnd(), &RCWMS);
			AjustarControles(RCWMS);
			Debug_Escribir_Varg(L"WM_SIZE %d, %d\n", wParam, lParam);
			return 0;

		case WM_SIZING :
			Evento_CambiandoTam(static_cast<UINT>(wParam), reinterpret_cast<RECT *>(lParam));
			return 0;
		case WM_MOVING :
			App.OcultarToolTipPlayer();
			return TRUE;

		case WM_ERASEBKGND :
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;

		// Barra de desplazamiento (barra de tiempo y volumen)
		case DWL_BARRAEX_CAMBIANDO:	// Se está modificando (mouse down)
			Evento_BarraEx_Cambiando(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case DWL_BOTONEX_CLICK :
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BOTONEX_MOUSEUP:
			Evento_BotonEx_Mouse_Soltado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BOTONEX_MOUSEDOWN :
			Evento_BotonEx_Mouse_Presionado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case WM_COMMAND : // Para los botones de la barra de tareas
			Evento_Comando(wParam, lParam);
			return 0;
		

		default :
			if (uMsg == BarraTareas.WM_TASK_BUTTON_CREATED()) {
				CrearBotonesThumb();
				return 0;
			}
			break;

/*		case DWL_ARBOLEX_CLICK :
			this->Evento_ArbolEx_Click(reinterpret_cast<DArbolEx_DatosClick *>(wParam), static_cast<UINT>(lParam));
			return 0;*/

/*		case DWL_TREEVIEW_CLICK:
			this->Evento_TreeView_Mouse_Click(reinterpret_cast<DTreeView_DatosClick *>(lParam), static_cast<UINT>(wParam));
			return 0;*/

/*		case DWL_LISTVIEW_CLICK:
			this->Evento_ListView_Mouse_Click(reinterpret_cast<DListView_DatosClick *>(lParam), static_cast<UINT>(wParam));
			return 0;
		case DWL_LISTVIEW_DOBLECLICK:
			this->Evento_ListView_Mouse_DobleClick(reinterpret_cast<DListView_DatosClick *>(lParam), static_cast<UINT>(wParam));
			return 0;*/

/*		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code) {
				/////////////////////////////
				// Notificaciones Button : //
				/////////////////////////////
/*				case BCN_DROPDOWN:
					return this->Evento_Button_Desplegar(((LPNMBCDROPDOWN)lParam)->rcButton, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case BCN_HOTITEMCHANGE:
					if (((LPNMBCHOTITEM)lParam)->dwFlags == (HICF_ENTERING | HICF_MOUSE))   return this->Evento_Button_Mouse_Entrando(static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
					else                                                                    return this->Evento_Button_Mouse_Saliendo(static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				///////////////////////////////
				// Notificaciones ListView : //
				///////////////////////////////
				case LVN_BEGINDRAG:        // Empezar Drag&Drop de un item interno (boton izquierdo)  (NMLISTVIEW iItem)
					return this->Evento_ListView_ArrastrarSoltar_Empezar(0, ((LPNMLISTVIEW)lParam)->iItem, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_BEGINRDRAG:       // Empezar Drag&Drop de un item interno (boton derecho)   (NMLISTVIEW iItem)
					return this->Evento_ListView_ArrastrarSoltar_Empezar(1, ((LPNMLISTVIEW)lParam)->iItem, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_BEGINLABELEDIT:   // Empezar labeledit del listview     (ANULAR devolver TRUE, POR HACER : probar de utilizar EmpezarEdicion del ListView)
					return TRUE;
				case LVN_BEGINSCROLL:      // Empieza una operacion de scroll en el listview    (NMLVSCROLL dx o dy)
					return this->Evento_ListView_Scroll_Empezar(((LPNMLVSCROLL)lParam)->dx, ((LPNMLVSCROLL)lParam)->dy, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_COLUMNCLICK:      // Click en una culumna  (NMLISTVIEW iSubItem)
					return this->Evento_ListView_Columna_Click(((LPNMLISTVIEW)lParam)->iSubItem, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_DELETEALLITEMS:   // Apunto de borrar todos los items (devolver true para recibir cuando se va a borrar cada item, false para no recibir nada)
					return this->Evento_ListView_Item_BorrarTodos(static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_DELETEITEM:       // apunto de borrar un item (NMLISTVIEW iItem)
					return this->Evento_ListView_Item_Borrar(((LPNMLISTVIEW)lParam)->iItem, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_ENDSCROLL:        // termina una operacion de scroll (NMLVSCROLL dx o dy)
					return this->Evento_ListView_Scroll_Terminar(((LPNMLVSCROLL)lParam)->dx, ((LPNMLVSCROLL)lParam)->dy, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_HOTTRACK:         // Indica que el mouse esta encima de un item (NMLISTVIEW iItem, iSubItem, ptAction   retornar 0 para continuar con la operación, o cualquier valor para que no se seleccione el item)
					return this->Evento_ListView_Mouse_Movimiento(((LPNMLISTVIEW)lParam)->iItem, ((LPNMLISTVIEW)lParam)->iSubItem, ((LPNMLISTVIEW)lParam)->ptAction.x, ((LPNMLISTVIEW)lParam)->ptAction.y, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_INSERTITEM:       // Informa que se ha agregado un item (NMLISTVIEW iItem)
					return this->Evento_ListView_Item_Agregar(((LPNMLISTVIEW)lParam)->iItem, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_ITEMACTIVATE:     // Informa que se ha activado un item... (NMITEMACTIVATE TODA)
					return this->Evento_ListView_Item_Activado(((LPNMITEMACTIVATE)lParam)->iItem, ((LPNMITEMACTIVATE)lParam)->iSubItem, ((LPNMITEMACTIVATE)lParam)->uNewState, ((LPNMITEMACTIVATE)lParam)->uOldState, ((LPNMITEMACTIVATE)lParam)->uChanged, ((LPNMITEMACTIVATE)lParam)->ptAction.x, ((LPNMITEMACTIVATE)lParam)->ptAction.y, ((LPNMITEMACTIVATE)lParam)->lParam, ((LPNMITEMACTIVATE)lParam)->uKeyFlags, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_ITEMCHANGED:      // Informa que se ha cambiado un item.... (NMLISTVIEW iItem [si es -1 es que se ha realizado un cambio a todos los items])
					return this->Evento_ListView_Item_Cambiado(((LPNMLISTVIEW)lParam)->iItem, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_ITEMCHANGING:     // Informa que se va a cambiar un item (NMLISTVIEW que dice los atributos que se van a cambiar)
					return this->Evento_ListView_Item_Cambiando(((LPNMLISTVIEW)lParam)->iItem, ((LPNMLISTVIEW)lParam)->iSubItem, ((LPNMLISTVIEW)lParam)->uNewState, ((LPNMLISTVIEW)lParam)->uOldState, ((LPNMLISTVIEW)lParam)->uChanged, ((LPNMLISTVIEW)lParam)->ptAction.x, ((LPNMLISTVIEW)lParam)->ptAction.y, ((LPNMLISTVIEW)lParam)->lParam, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
				case LVN_KEYDOWN:          // Tecla presionada (NMLVKEYDOWN wVKey)
					return this->Evento_ListView_TeclaPresionada(((LPNMLVKEYDOWN)lParam)->wVKey, static_cast<UINT>(GetWindowLongPtr(((LPNMHDR)lParam)->hwndFrom, GWLP_ID)));
//		        case LVN_COLUMNDROPDOWN :         // columna desplegable??? (NMLISTVIEW iSubItem) (win vista o superior....)
//              case LVN_COLUMNOVERFLOWCLICK :    // MSDN : Sent by a list-view control when its overflow button is clicked. :O   NMLISTVIEW iSubItem) (win vista o superior....)
//              case LVN_ENDLABELEDIT :           // terminar labeledit (no deberia recibirse ya que anulamos el principio)
//              case LVN_GETDISPINFO :            // obtener informacion necesaria para mostrar o ordenar un item (yo me la ahorraria....)
//              case LVN_GETEMPTYMARKUP :         // enviada cuando no tiene items, es una peticion para saber como marcar el texto :O (windows vista o superior... yo me la saltaba tambien..)
//              case LVN_GETINFOTIP :             // Enviada solo con el modo LARGE_ICON, y es una peticion para obtener informacion adicional para mostrar en un tooltip
//              case LVN_INCREMENTALSEARCH :      // Notifica que se esta haciendo una busqueda incremetal? :O (WIndows vista o superior, otra que pasare...)
//              case LVN_LINKCLICK :              // Notifica que se ha echo click en un link :O (windows vista o superior... otra que pasare...)
//              case LVN_ODCACHEHINT :            // Enviada por un listview virtual para notificar que su contenido ha cambiado.... (pasando...)
//              case LVN_ODFINDITEM :             // Enviada por un listview virtual para buscar un item... (pasando...)
//              case LVN_ODSTATECHANGED :         // Enviada por un listview virtual para notificar que uno o varios items han cambiado (pasando...)
//              case LVN_SETDISPINFO :            // Notifica que se va a actualizar la informacion que mantiene para un item??? (otra que pasare...)
//              case NM_CUSTROMDRAW :             // Se controla en los windowprocedures de DVentana, DDialogo y DDialogoModal porque dependiendo de si es una ventana o un dialogo hay que contestar distinto......
//              case LVN_MARQUEEBEGIN :           // MSDN : Notifies a list-view control's parent window that a bounding box (marquee) selection has begun. (To accept the notification code, return zero. To quit the bounding box selection, return nonzero.)
//                                                //          Remarks : A bounding box selection is the process of clicking the list-view window's client area and dragging to select multiple items simultaneously.
				*/
				///////////////////////////////
				// Notificaciones TreeView : // TODO hacer lista completa como en el listview y crear todos los eventos posibles
				///////////////////////////////
//				case TVN_DELETEITEM:		 // Borrar item del TreeView																
//					SendMessage(((LPNMTREEVIEW)lParam)->hdr.hwndFrom, DWL_TREEVIEW_BORRARNODO, 0, ((LPNMTREEVIEW)lParam)->itemOld.lParam);
//					break;
				////////////////////////////////
				// Notificaciones Estandard : //
				////////////////////////////////
//				case NM_RELEASEDCAPTURE:
//					Evento_CapturaSoltada(((LPNMHDR)lParam)->idFrom);
				/*case NM_CLICK:
				case NM_DBLCLK:
					//                                                            case NM_HOVER :
				case NM_KILLFOCUS:
				case NM_RCLICK:
				case NM_RDBLCLK:
				case NM_RETURN:
				case NM_SETFOCUS:
					// Tanto el ListView como el TreeView procesan los mensajes WM_?BUTTONUP internamente y no pasan por el WindowProcedure de estos.
					// Segun la MSDN cuando recibe un WM_?BUTTONDOWN estos controles entran en un bucle de mensajes interno para determinar si la operación involucra un Click o un Drag&Drop....
					// Por desgracia si quiero habilitar funcionabilidades extras como editar un SubItem con una ComboBox necesito obtener cuando se suelta el mouse...
					// A causa de esto cuando recibo un NM_CLICK lo devuelvo a su control (que sera un TreeView o un ListView, y asi consigo saber cuando se suelta el boton del mouse).
					SendMessage(((LPNMHDR)lParam)->hwndFrom, DWL_NOTIFICACION, wParam, lParam);
					break;*/

					// Notificación TVN_ITEMEXPANDING para recibir cuando se expande un nodo del treeview
					// Se necesita especificamente para la clase DTreeViewDirectorios ya que al expandir un nodo hay que escanear el directorio al que hace referencia.
/*				case TVN_ITEMEXPANDING:
					return SendMessage(((LPNMHDR)lParam)->hwndFrom, DWL_TREEVIEW_NODO_EXPANDIENDO, wParam, lParam);
					// Cambio de selección en el TreeView
				case TVN_SELCHANGED:
					return SendMessage(((LPNMHDR)lParam)->hwndFrom, DWL_TREEVIEW_NODO_CAMBIOSELECCION, wParam, lParam); */
//			}
	}
//	return FALSE;
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}