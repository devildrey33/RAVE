#include "stdafx.h"
#include "VentanaPrincipal.h"
#include "Resource.h"
#include "DMensajesWnd.h"
#include "DMouse.h"
#include "Rave_Skin.h"
#include "DDlgDirectorios.h"
#include <shellapi.h>
#include "DStringUtils.h"
#include <versionhelpers.h>

#define RAVE_MIN_ANCHO 670
#define RAVE_MIN_ALTO  280
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

	Arbol.CrearArbolEx(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_ARBOLBD);
	Arbol.Visible(TRUE);

	Lista.CrearListaEx(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_LISTAMEDIOS);

	// Columnas
	Lista.AgregarColumna(45, DListaEx_Columna_Alineacion_Izquierda);							// Icono y pista
	Lista.AgregarColumna(DLISTAEX_COLUMNA_ANCHO_AUTO, DListaEx_Columna_Alineacion_Izquierda);	// Nombre
	Lista.AgregarColumna(80, DListaEx_Columna_Alineacion_Derecha);								// Tiempo

	
	Video.Crear(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_VERVIDEO);

	// Opciones /////////////////////////////
	Opciones.Crear(this, (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, ID_OPCIONES);
	Opciones.Visible(FALSE);

	ListaRaiz.CrearListaRaiz(&Opciones, 10, 10, 300, 80, ID_LISTARAIZ);

	for (size_t i = 0; i < App.BD.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.BD.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Visible(TRUE);

/*	ComboRaiz.CrearDesplegable(&Opciones, 10, 100, 300, 30, ID_OPCIONES, DDesplegableEx_TipoEdicion_SinEdicion, DDesplegableEx_TipoDesplegable_ExplorarDirectorios);
	ComboRaiz.Visible(TRUE);*/
	BotonAgregarRaiz.CrearBotonEx(&Opciones, L"Agregar Raiz", 10, 100, 150, 24, ID_BOTON_AGREGARRAIZ);
	BotonAgregarRaiz.Fuente = _Fuente21Normal;
	/////////////////////////////////////////

	// Marco superior izquierdo /////////////
	MarcoSI.Crear(this, 10, 10, 380, 30, ID_MARCOSI);
	BotonAtras.CrearBotonEx(&MarcoSI,	 IDI_PREV32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,   0, 0, 30, 30, ID_BOTON_ANTERIOR);
	BotonPlay.CrearBotonEx(&MarcoSI,	 IDI_PLAY32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  40, 0, 30, 30, ID_BOTON_PLAY);
	BotonPausa.CrearBotonEx(&MarcoSI,	 IDI_PAUSA32, 32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO,  80, 0, 30, 30, ID_BOTON_PAUSA);
	BotonStop.CrearBotonEx(&MarcoSI,	 IDI_STOP32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 120, 0, 30, 30, ID_BOTON_STOP);
	BotonAdelante.CrearBotonEx(&MarcoSI, IDI_NEXT32,  32, DBOTONEX_CENTRADO, DBOTONEX_CENTRADO, 160, 0, 30, 30, ID_BOTON_SIGUIENTE);

	BotonMezclar.CrearBotonEx(&MarcoSI, L"Mezclar", 210, 0, 80, 30, ID_BOTON_MEZCLAR);
	if (App.BD.Opciones_Shufle() == TRUE) BotonMezclar.Marcado(TRUE);
	BotonRepetir.CrearBotonEx(&MarcoSI, L"Repetir", 300, 0, 80, 30, ID_BOTON_REPETIR);
	if (App.BD.Opciones_Repeat() > 0) BotonRepetir.Marcado(TRUE);
	//////////////////////////////////////////

	SliderTiempo.CrearBarraDesplazamientoEx(this, 10, 50, RC.right - 20, 20, ID_SLIDER_TIEMPO);
//	SliderTiempo.Crear(hWnd, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO, WS_CHILD | TBS_NOTICKS | WS_VISIBLE, 0, 30000, 0);
//	SliderTiempo.TamPagina(30000 / 50);

	// Marco superior derecho
	MarcoSD.Crear(this, RC.right - 260, 14, 250, 24, ID_MARCOSD);

	SliderVolumen.CrearBarraDesplazamientoEx(&MarcoSD, 120, 3, 90, 17, ID_SLIDER_VOLUMEN, 0, 200, static_cast<float>(App.BD.Opciones_Volumen()));
	std::wstring TxtVolumen = std::to_wstring(App.BD.Opciones_Volumen()) + L"%";
	LabelVolumen.CrearLabelEx(&MarcoSD, TxtVolumen.c_str(), 215, 2, 40, 20, ID_LABEL_VOLUMEN, WS_CHILD | WS_VISIBLE);

	LabelTiempoActual.CrearLabelEx(&MarcoSD, L"00:00", 0, 2, 55, 20, ID_LABEL_TIEMPOACTUAL, TRUE, WS_CHILD | WS_VISIBLE);
	LabelTiempoSeparador.CrearLabelEx(&MarcoSD, L"/", 55, 2, 10, 20, ID_LABEL_TIEMPOSEPARADOR,TRUE,  WS_CHILD | WS_VISIBLE);
	LabelTiempoTotal.CrearLabelEx(&MarcoSD, L"00:00", 65, 2, 55, 20, ID_LABEL_TIEMPOTOTAL, WS_CHILD | WS_VISIBLE);

	// Marco inferior izquierdo /////////////
	MarcoII.Crear(this, 10, 80, RAVE_BOTONES_LATERALES_ANCHO, 160, ID_MARCOSI);
	BotonBD.CrearBotonEx(&MarcoII, L"Base de datos", 0, 0, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_BD);
	BotonLista.CrearBotonEx(&MarcoII, L"Lista de medios", 0, 40, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_LISTA);
	BotonVideo.CrearBotonEx(&MarcoII, L"Ver video", 0, 80, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_VIDEO);
	BotonOpciones.CrearBotonEx(&MarcoII, L"Opciones", 0, 120, RAVE_BOTONES_LATERALES_ANCHO, 30, ID_BOTON_OPCIONES);
	/////////////////////////////////////////

	// Muestro la ventana principal
	ShowWindow(hWnd(), nCmdShow);

	// Timer que comprueba si se ha terminado la cancion
	SetTimer(hWnd(), TIMER_LISTA, 500, NULL);
	// Timer para el slider del tiempo
	SetTimer(hWnd(), TIMER_TIEMPO, 250, NULL);

	// Habilito el drag & drop para esta ventana
	DragAcceptFiles(hWnd(), TRUE);

	ActualizarArbol();

	// Compruebo las aosiciaciones de archivo y muestro la ventana si es necesario
	if (App.AsociarMedios.ComprobarAsociaciones() == FALSE) {
		VentanaAsociar.Mostrar();
	}

	return rhWnd;
}





void VentanaPrincipal::AjustarControles(RECT &RC) {
	// 50 ?? 60

	MoveWindow(Lista.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);
	MoveWindow(Arbol.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);
	MoveWindow(Video.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);
	MoveWindow(Opciones.hWnd(), (RAVE_BOTONES_LATERALES_ANCHO + 20), 81, RC.right - (RAVE_BOTONES_LATERALES_ANCHO + 30), RC.bottom - 90, TRUE);

	MoveWindow(SliderTiempo.hWnd(), 10, 50, RC.right - 20, 24, TRUE);
	MoveWindow(MarcoSD.hWnd(), RC.right - 260, 14, 255, 24, TRUE);

	//App.VLC.RepintarVLC();
	if (App.VLC.hWndVLC != NULL) {
		InvalidateRect(App.VLC.hWndVLC, &RC, TRUE);
		App.VLC.RepintarVLC();
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
	App.VentanaRave.RectMonitorActual = *lprcMonitor;
	return TRUE;
}


void VentanaPrincipal::Evento_Temporizador(const UINT cID) {
	static Estados_Medio Estado = SinCargar;
//	static wchar_t TiempoStr[64];
	std::wstring TiempoStr;
	Estado = App.VLC.ComprobarEstado();
	if (EstadoMedio != Estado) {
		EstadoMedio = Estado;
		Debug_Escribir_Varg(L"EstadoVLC %d\n", EstadoMedio);
	}

	switch (cID) {
		// Controles PantallaCompleta Inactividad
		case TIMER_CPC_INACTIVIDAD:
			App.ControlesPC.Ocultar();
			break;
		// Obtiene el hWnd de la ventana a pantalla completa del VLC
		case TIMER_OBTENERVLCWND : 
			EnumChildWindows(Video.hWnd(), &RaveVLC::EnumeracionVLC, NULL);
			if (App.VLC.hWndVLC != NULL) KillTimer(hWnd(), TIMER_OBTENERVLCWND);
			break;
		// Temporizador para repintar el VLC?¿?
		case TIMER_REPINTARVLC :
			App.VLC.RepintarVLC();
			KillTimer(hWnd(), TIMER_REPINTARVLC);
			break;
		// Temporizador para actualizar el tiempo actual del medio que se está reproduciendo
		case TIMER_TIEMPO :
			if (!Minimizado()) {
				// Si el slider del tiempo tiene la captura, es porque se esta modificando el tiempo, por lo que no hay que actualizar la posición en ese momento.
				if (SliderTiempo.Estado() != DBarraDesplazamientoEx_Estado_Presionado && App.ControlesPC.SliderTiempo.Estado() != DBarraDesplazamientoEx_Estado_Presionado) {
					SliderTiempo.Valor(App.VLC.TiempoActual());
//					SliderTiempo.Posicion(static_cast<UINT64>(App.VLC.TiempoActual() * 30000));
					App.ControlesPC.SliderTiempo.Valor(App.VLC.TiempoActual());
					// Lo mismo pasa con el tiempo actual
					App.VLC.TiempoStr(App.VLC.TiempoActualMs(), TiempoStr);
					LabelTiempoActual.Texto(TiempoStr);
					App.ControlesPC.LabelTiempoActual.Texto(TiempoStr);
				}
				// El tiempo total solo se actualiza cuando es distinto al anterior
				UINT64 T = App.VLC.TiempoTotalMs();
				if (T != App.VLC.TiempoTotal) {
					TiempoStr = L"";
					App.VLC.TiempoTotal = T;
					App.VLC.TiempoStr(T, TiempoStr);
					LabelTiempoTotal.Texto(TiempoStr);
					App.ControlesPC.LabelTiempoTotal.Texto(TiempoStr);
				}
			}
			break;
/*		case TIMER_OBTENER_TIEMPO_TOTAL :
			Timer_ObtenerTiempoTotal();
			break;*/

		// Temporizador que detecta cuando se termina un medio y avanza al siguiente según las reglas establecidas
		case TIMER_LISTA:
			// Si no hay items salgo
			if (Lista.TotalItems() == 0) return;

			// Si hay 10 errores en la lista, paro
			if (Lista.Errores > 10) {
				Lista_Stop();
				//Lista.Errores = 0; // ya lo hace el stop
				return;
			}

			// Un medio de la lista ha terminado
			if (Estado == Terminada) {				
				App.VLC.Stop();
				if (Lista.MedioActual == Lista.TotalItems() - 1) {
					Repeat();
				}
				else {
					Lista_Siguiente();
				}
			}

			break;
		}	
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
				Lista.MedioActual = -1;
				Lista.DesSeleccionarTodo();
				Lista.ItemMarcado(Lista.Item(0));
				Lista.Item(0)->Seleccionado = TRUE;
				Lista_Siguiente();
			}
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
			App.BD.AsignarTiempoMedio(static_cast<libvlc_time_t>(TiempoMS), rItem->Hash);

//			nMedioActual.Tiempo = static_cast<libvlc_time_t>(TiempoMS);

		}
		KillTimer(_hWnd, TIMER_OBTENER_TIEMPO_TOTAL);
	}

}*/


void VentanaPrincipal::Lista_Pausa(void) {
	App.VLC.Pausa();
}


void VentanaPrincipal::Lista_Play(void) {
	if (Lista.TotalItems() == 0) return;

	Lista.Errores = 0;
	BDMedio NCan;
	switch (App.VLC.ComprobarEstado()) {
		case SinCargar:
			if (Lista.TotalItems() > 0) {
				App.BD.ObtenerMedio(Lista.Medio(Lista.MedioActual)->Hash, NCan);
//				NCan.Obtener(App.BD(), Lista.Medio(Lista.MedioActual)->Hash);
				if (App.VLC.AbrirMedio(NCan) == FALSE) Lista.Errores++;
				if (App.VLC.Play() == TRUE) {
//					Lista.Medio(Lista.Pos)->Icono(9);
				}
			}
			break;
		case Terminada:
			App.VLC.ActualizarIconos(0);
			Lista.MedioActual++;
			if (Lista.MedioActual >= static_cast<int>(Lista.TotalItems())) {
				Lista.MedioActual = 0;
			}
			App.BD.ObtenerMedio(Lista.Medio(Lista.MedioActual)->Hash, NCan);
//			NCan.Obtener(App.BD(), Lista.Medio(Lista.MedioActual)->Hash);
			if (App.VLC.AbrirMedio(NCan) == FALSE) Lista.Errores++;
			App.VLC.Play();
			break;
		case EnStop:
		case EnPausa:
			App.VLC.Play();
			break;
	}
}


void VentanaPrincipal::Lista_Stop(void) {
	Lista.Errores = 0;
	App.VLC.Stop();
}


void VentanaPrincipal::Lista_Siguiente(void) {
	if (Lista.TotalItems() == 0) return;

	Lista.MedioActual++;
	if (Lista.MedioActual >= static_cast<int>(Lista.TotalItems())) Lista.MedioActual = 0;


	BDMedio NCan;
	App.BD.ObtenerMedio(Lista.Medio(Lista.MedioActual)->Hash, NCan);
	if (App.VLC.AbrirMedio(NCan) == FALSE) Lista.Errores++;
	App.VLC.Play();
}


void VentanaPrincipal::Lista_Anterior(void) {
	if (Lista.TotalItems() == 0) return;

	int TotalItems = static_cast<int>(Lista.TotalItems()) - 1;
	if (TotalItems == -1) TotalItems = 0;
	Lista.MedioActual--;
	if (Lista.MedioActual < 0) Lista.MedioActual = TotalItems;

	if (Lista.MedioActual >= 0 && Lista.MedioActual <= TotalItems) {
		BDMedio NCan;
		App.BD.ObtenerMedio(Lista.Medio(Lista.MedioActual)->Hash, NCan);
//		TablaMedios_Medio NCan(App.BD(), Lista.Medio(Lista.MedioActual)->Hash);
		if (App.VLC.AbrirMedio(NCan) == FALSE) Lista.Errores++;
		App.VLC.Play();
	}
}

void VentanaPrincipal::Evento_BotonEx_Mouse_Down(const UINT cID) {
	_BotonExMouseDownTick = GetTickCount();
	switch (cID) {
		case ID_BOTON_ANTERIOR:	
			App.VLC.Ratio(0.5f);
			break;
		case ID_BOTON_SIGUIENTE:
			App.VLC.Ratio(2.0f);
			break;
	}

}

void VentanaPrincipal::Evento_BotonEx_Mouse_Click(const UINT cID) {
	switch (cID) {
		case ID_BOTON_OPCIONES: 
			Arbol.Visible(FALSE);
			Lista.Visible(FALSE);
			Video.Visible(FALSE);
			Opciones.Visible(TRUE);
			Opciones.AsignarFoco();
			break;
		case ID_BOTON_BD:
			Arbol.Visible(TRUE);
			Lista.Visible(FALSE);
			Video.Visible(FALSE);
			Opciones.Visible(FALSE);
			Arbol.AsignarFoco();
			break;
		case ID_BOTON_LISTA:
			Arbol.Visible(FALSE);
			Lista.Visible(TRUE);
			Video.Visible(FALSE);
			Opciones.Visible(FALSE);
			Lista.AsignarFoco();
			break;
		case ID_BOTON_VIDEO:
			Arbol.Visible(FALSE);
			Lista.Visible(FALSE);
			Video.Visible(TRUE);
			Opciones.Visible(FALSE);
			Video.AsignarFoco();
			break;
		case ID_BOTON_AGREGARRAIZ :
			AgregarRaiz();
			break;
		case ID_BOTON_MEZCLAR:
			Mezclar_Click();
			break;
		case ID_BOTON_REPETIR:
			Repetir_Click();
			break;
		case ID_BOTON_ANTERIOR:
			App.VLC.Ratio(1.0f);
			if (GetTickCount() < _BotonExMouseDownTick + 200) {
				Lista_Anterior();
			}
			break;
		case ID_BOTON_SIGUIENTE:
			App.VLC.Ratio(1.0f);
			if (GetTickCount() < _BotonExMouseDownTick + 200) {
				Lista_Siguiente();
			}
			break;
		case ID_BOTON_PLAY:
			Lista_Play();
			break;
		case ID_BOTON_PAUSA:
			Lista_Pausa();
			break;
		case ID_BOTON_STOP:
			Lista_Stop();
			break;

	}
}

void VentanaPrincipal::Mezclar_Click(void) {
	BOOL nMezclar = !App.BD.Opciones_Shufle();
	Lista.Mezclar(nMezclar);
	App.BD.Opciones_Shufle(nMezclar);
	BotonMezclar.Marcado(nMezclar);
	App.ControlesPC.BotonRepetir.Marcado(nMezclar);
}

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
}


void VentanaPrincipal::Repetir_Click(void) {
	RECT RW;
	GetWindowRect(BotonRepetir.hWnd(), &RW);
	DMenuEx *Ret = Menu_Repetir.MostrarModal(this, RW.left, RW.bottom);
	if (Ret != NULL) {
		// Elimino los iconos de todos los submenus
		for (size_t i = 0; i < Menu_Repetir.TotalMenus(); i++) {
			Menu_Repetir.Menu(i)->Icono(0);
		}
		// Asigno el icono de la marca al menú seleccionado
		Ret->Icono(IDI_CHECK);
		
		// Asigno el tipo de repeat 
		switch (Ret->ID()) {
			case ID_REPETIR_NO				:	App.BD.Opciones_Repeat(Tipo_Repeat_NADA);					break;
			case ID_REPETIR_SI				:	App.BD.Opciones_Repeat(Tipo_Repeat_RepetirLista);			break;
			case ID_REPETIR_SI_MEZCLAR		:	App.BD.Opciones_Repeat(Tipo_Repeat_RepetirListaShufle);		break;
			case ID_REPETIR_SI_APAGAR_REP	:	App.BD.Opciones_Repeat(Tipo_Repeat_ApagarReproductor);		break;	// No se guarda en la BD
			case ID_REPETIR_SI_APAGAR_WIN	:	App.BD.Opciones_Repeat(Tipo_Repeat_ApagarOrdenador);		break;	// No se guarda en la BD
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

//		RedrawWindow(App.VLC.hWndVLC, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);


		SetTimer(hWnd(), TIMER_REPINTARVLC, 250, NULL);
		/*		KillTimer(hWnd(), TIMER_CPC_INACTIVIDAD);
		SetTimer(hWnd(), TIMER_CPC_INACTIVIDAD,		, NULL);*/

	}

	else {
		ShowWindow(hWnd(), SW_RESTORE);
		SetWindowLongPtr(hWnd(), GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetWindowPos(_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		//MoveWindow(Video.hWnd(), 120, 71, RC.right - 120, RC.bottom - 70, TRUE);

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
		App.ControlesPC.Ocultar();
		DWL::DMouse::Visible(TRUE);
		KillTimer(hWnd(), TIMER_CPC_INACTIVIDAD);
	}

	InvalidateRect(App.VLC.hWndVLC, NULL, TRUE);



//	SetTimer(hWnd(), TIMER_REPINTARVLC, 1000, NULL);
//	PostMessage(Video.hWnd(), WM_PAINT, 0, 0);
}

void VentanaPrincipal::Evento_SliderTiempo_Cambiado() {
	std::wstring TiempoStr;
//	Debug_Escribir_Varg(L"Evento_SliderTiempo_Cambiado %d, %.02f\n", App.VLC.TiempoTotalMs(), SliderTiempo.Valor());
	App.VLC.TiempoStr(static_cast<UINT64>(static_cast<float>(App.VLC.TiempoTotalMs()) * SliderTiempo.Valor()), TiempoStr);
	LabelTiempoActual.Texto(TiempoStr);
	App.ControlesPC.LabelTiempoActual.Texto(TiempoStr);
	App.VLC.TiempoActual(SliderTiempo.Valor());
}

void VentanaPrincipal::Evento_SliderVolumen_Cambio() {
	int Volumen = static_cast<int>(SliderVolumen.Valor());	
	App.VLC.Volumen(Volumen);
}

// Guardo el volumen en las opciones (al soltar el slider del volumen)
void VentanaPrincipal::Evento_SliderVolumen_Cambiado() {
	App.BD.Opciones_Volumen(static_cast<int>(SliderVolumen.Valor()));
}

void VentanaPrincipal::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}

void VentanaPrincipal::Evento_Cerrar(void) {
	Visible(FALSE);
	App.BD.Opciones_GuardarOpciones();
	
	ThreadActualizar.Cancelar(TRUE);
	ThreadArchivosLista.Cancelar(TRUE);

	PostQuitMessage(0);
}

void VentanaPrincipal::Evento_SoltarArchivos(WPARAM wParam) {
	TCHAR        Archivo[MAX_PATH];
	unsigned int TotalSoltados = DragQueryFile((HDROP)wParam, static_cast<unsigned int>(-1), 0, 0);
	std::vector<std::wstring> Paths;

	for (unsigned int i = 0; i < TotalSoltados; i++) {
		DragQueryFile((HDROP)wParam, i, Archivo, MAX_PATH);
//		if (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(Archivo))	SoltarArchivos_BuscarArchivos(Archivo);
//		else														SoltarArchivos_AgregarArchivo(Archivo);
		Paths.push_back(Archivo);
	}
	DragFinish((HDROP)wParam);

	ThreadArchivosLista.Iniciar(_hWnd, Paths);
}



NodoBD *VentanaPrincipal::Arbol_AgregarRaiz(std::wstring *Path) {
	// Solo crea la raiz
	NodoBD *Tmp = Arbol.BuscarHijoTxt(*Path);
	if (Tmp == NULL) {
		Tmp = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Raiz, NULL, Path->c_str());
		Tmp->Expandido = TRUE;
		Arbol.Repintar();
	}
	return Tmp;
}


NodoBD *VentanaPrincipal::Arbol_AgregarDir(std::wstring *Path, const BOOL nRepintar) {
	BDRaiz *Raiz = App.BD.BuscarRaiz(*Path);
//	CeRaiz *Raiz = Tabla_Raiz.Buscar_Raiz(Path->c_str());
	if (Raiz == NULL) return NULL;
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

	RaveBD::FiltroPath(Split[Split.Total() - 1], Filtrado);

	NodoBD *Ret = Arbol.BuscarHijoTxt(Filtrado, TmpNodo);
	// Agrego el directorio
	if (Ret == NULL) {
		Ret = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Directorio, TmpNodo, Filtrado.c_str());
	}
	//	delete Path;
	if (nRepintar == TRUE)	Arbol.Repintar();

	return Ret;
}

/*
NodoBD *VentanaPrincipal::Arbol_AgregarCancion(const size_t Hash) {
//	ArbolBD &Arbol = App.VentanaRave.Arbol;
	// Busco el medio en el arbol
	for (size_t i = 0; i < Arbol.TotalNodos(); i++) {
		if (Arbol.BDNodo(i)->Hash == Hash)
			return NULL; // Ya exsite
	}

	BDMedio Medio;
	App.BD.ObtenerMedio(Hash, Medio);
	// Obtengo la cancion de la bd
//	TablaMedios_Medio Cancion(_BD, Hash);
	NodoBD *TmpNodo = Arbol.AgregarBDNodo(ArbolBD_TipoNodo_Directorio, NULL, Medio.Path.c_str(), Hash);
	//	TmpNodo->Hash = Hash;

	return TmpNodo;
}*/

void VentanaPrincipal::ActualizarArbol(void) {
	BarraTareas.Estado_Indeterminado();
	// Re-escaneo las unidades de disco
	App.BD.Unidades.Escanear_Unidades_Locales();
	//	if (_hWnd != NULL) return;
	Arbol.BorrarTodo();
	Menu_ArbolBD.Menu(2)->Activado(FALSE);

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
	std::wstring Param;
	App.MemCompartida.Leer(Param);
	BDMedio Medio;
	if (App.BD.ObtenerMedio(Param, Medio) == FALSE) {
		if (App.BD.AnalizarMedio(Param, Medio, 0) == FALSE) return;
	}

	Debug_Escribir_Varg(L"VentanaPrincipal::ExploradorAgregarMedio : %s\n", Param.c_str());
	Lista.AgregarMedio(&Medio);
	Lista.Repintar();

	if (Reproducir == TRUE) {
		ItemMedio *Item = Lista.BuscarHash(Medio.Hash);
		for (size_t i = 0; i < Lista.TotalItems(); i++) {
			if (Item == Lista.Medio(i)) {
				Lista.MedioActual = i;
				break;
			}
		}		
		Lista_Play();
	}
}


LRESULT CALLBACK VentanaPrincipal::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::wstring *TmpStr = NULL;
	BDMedio *Medio = NULL;
	switch (uMsg) {
		case WM_AGREGARMEDIO :
			ExploradorAgregarMedio(FALSE);
			break;
		case WM_REPRODUCIRMEDIO :
			ExploradorAgregarMedio(TRUE);
			break;

		// Para el Thread AgregarArchivosLista, al agregar/obtener el archivo en la BD, ya se puede agregar a la lista.
		case WM_TAAL_AGREGARMEDIO :
			Medio = reinterpret_cast<BDMedio *>(wParam);
			Lista.AgregarMedio(Medio);
			delete Medio;
			break;
		case WM_TAAL_TERMINADO:
			ThreadArchivosLista.Terminar();
			BarraTareas.Estado_SinProgreso();
			BarraTareas.Resaltar();
			Lista.Repintar();
			Lista_Play();
			break;
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
		case WM_TOM_INICIADO2 :
			_MaximoTotalMedios2 = static_cast<UINT>(wParam);
			_ValorMedios2 = 0;
			break;
		case WM_TOM_TOTALMEDIOS1:
			BarraTareas.Valor(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
			break;
		case WM_TOM_TOTALMEDIOS2:
			BarraTareas.Valor(++_ValorMedios2, _MaximoTotalMedios2);
			break;
		case WM_TOM_TOTALMEDIOS3:
			BarraTareas.Valor(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
			break;
		case WM_TOM_TERMINADO:
			ThreadAnalizar.Terminar();
			Debug_Escribir_Varg(L"ThreadAnalisis::Terminado %d archivos examinados.\n", lParam);
			BarraTareas.Estado_SinProgreso();
			BarraTareas.Resaltar();
			break;
			//		case WM_TBA_AGREGARAUDIO:
//			Arbol_AgregarCancion(static_cast<size_t>(lParam));
//			break;
		case WM_TBA_TERMINADO:
			ThreadActualizar.Terminar();
			Menu_ArbolBD.Menu(2)->Activado(TRUE);
			BarraTareas.Estado_SinProgreso();
//			BarraTareas.Resaltar();
			Debug_Escribir_Varg(L"ThreadActualizarArbol::Terminado %d archivos encontrados.\n", lParam);
			ThreadAnalizar.Iniciar(_hWnd);
			break;

		case WM_DROPFILES :
			Evento_SoltarArchivos(wParam);
			break;

		case WM_KEYDOWN:
			Debug_Escribir_Varg(L"t:%d r:%d p:%d\n", static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_KEYUP:
			Debug_Escribir_Varg(L"t:%d r:%d p:%d\n", static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_CHAR:
			Debug_Escribir_Varg(L"t:%d r:%d p:%d\n", static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
			//				Debug_Escribir(L"DArbolEx::CrearArbolEx() Error : ya se ha creado el arbol\n");
			return 0;



		case WM_TIMER:	
			this->Evento_Temporizador(static_cast<UINT>(wParam));
			return 0;
		case WM_CLOSE : 
			Evento_Cerrar();
			return 0;
		case WM_COMMAND :
			Debug_Escribir_Varg(L"WM_COMMAND %d, %d\n", HIWORD(wParam), LOWORD(wParam));
/*			if (HIWORD(wParam) == BN_CLICKED) { // Botón estándar
				Evento_Button_Mouse_Click(LOWORD(wParam));
				return 0;
			}*/
/*			else { // Menu estándar
				Evento_Menu(LOWORD(wParam));
				return 0;
			}*/
			break;
		case WM_EXITSIZEMOVE  :
			if (App.VentanaRave.Maximizada() == FALSE) {
				App.BD.Opciones_GuardarPosTamVentana();
			}			
			Debug_Escribir_Varg(L"WM_EXITSIZEMOVE %d, %d\n", wParam, lParam);
			break;
		case WM_SIZE :
			RECT RCWMS;
			GetClientRect(hWnd(), &RCWMS);
			AjustarControles(RCWMS);
			Debug_Escribir_Varg(L"WM_SIZE %d, %d\n", wParam, lParam);
			break;

		case WM_SIZING :
			Evento_CambiandoTam(static_cast<UINT>(wParam), reinterpret_cast<RECT *>(lParam));
			return 0;

		case WM_ERASEBKGND :
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;

		// Barra de desplazamiento (barra de tiempo y volumen)
		case DWL_BARRAEX_CAMBIO:
			switch (wParam) {
				case ID_SLIDER_VOLUMEN: 					Evento_SliderVolumen_Cambio();					break;
			}
			break;
		case DWL_BARRAEX_CAMBIADO:
			switch (wParam) {
				case ID_SLIDER_TIEMPO:						Evento_SliderTiempo_Cambiado();					break;
				case ID_SLIDER_VOLUMEN: 					Evento_SliderVolumen_Cambiado();				break;
			}
			break;

		case DWL_BOTONEX_CLICK :
			Evento_BotonEx_Mouse_Click(static_cast<UINT>(wParam));
			return 0;
		case DWL_BOTONEX_MOUSEDOWN :
			Evento_BotonEx_Mouse_Down(static_cast<UINT>(wParam));
			return 0;

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
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}