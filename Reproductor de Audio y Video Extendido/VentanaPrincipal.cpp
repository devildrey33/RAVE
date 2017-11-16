#include "stdafx.h"
#include "VentanaPrincipal.h"
#include "Resource.h"
#include "DMensajesWnd.h"
#include "DMouse.h"
#include "RAVE_Colores.h"

/* TODO : Moure els butons de l'esquerra a la dreta, y fer que la llista / bd / opcions / video quedin ajustats a l'esquerra per evitar els flickerings dels scrolls de la llista i l'arbre */
HWND VentanaPrincipal::Crear(int nCmdShow) {
	// Detección de monitores
	EnumDisplayMonitors(NULL, NULL, VentanaPrincipal::EnumerarPantallas, NULL);
	// Si el monitor donde se guardo la ultima posición no está disponible busco una nueva posición
	if (MonitorDisponible == FALSE) {
		App.BD.Tabla_Opciones.AsignarPosVentana(RectMonitorActual.left + 100, RectMonitorActual.top + 100);
	}

//	int x = App.BD.Tabla_Opciones.PosX(), y = App.BD.Tabla_Opciones.PosY();

	HWND rhWnd = DVentana::CrearVentana(L"VentanaPrincipal", RAVE_TITULO, App.BD.Tabla_Opciones.PosX(), App.BD.Tabla_Opciones.PosY(), App.BD.Tabla_Opciones.Ancho(), App.BD.Tabla_Opciones.Alto(), WS_OVERLAPPEDWINDOW);

	RECT RC, RW;
	GetClientRect(hWnd(), &RC);
	GetWindowRect(hWnd(), &RW);
	CTW_ExtraX = (RW.right - RW.left) - RC.right;
	CTW_ExtraY = (RW.bottom - RW.top) - RC.bottom;

	Arbol.Crear(hWnd, 120, 81, RC.right - 120, RC.bottom - 80, ID_ARBOLBD, WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_TRACKSELECT | TVS_NOHSCROLL, TVS_EX_DOUBLEBUFFER | TVS_EX_RICHTOOLTIP);
	Arbol.AsignarImageList(&App.ListaImagenes16);

	Lista.Crear(hWnd, 120, 81, RC.right - 120, RC.bottom - 80, ID_LISTAMEDIOS, WS_CHILD, LVS_EX_FULLROWSELECT);
	Lista.AsignarImageList(&App.ListaImagenes16, DEnum_ListView_ImageList_Peque);
	Lista.AgregarColumna(TEXT("Pista"), 50);
	Lista.AgregarColumna(TEXT("Nombre"), RC.right - 250);
/*	Lista.AgregarColumna(TEXT("Disco"), 50);
	Lista.AgregarColumna(TEXT("Grupo"), 50);
	Lista.AgregarColumna(TEXT("Genero"), 50);*/
	Lista.AgregarColumna(TEXT("Tiempo"), 60, DWL::DEnum_ListView_AlineacionTexto::DEnum_ListView_AlineacionTexto_Derecha);
	Lista.CambiarVista(DWL::DEnum_ListView_Vista::DEnum_ListView_Vista_Detalles);
	
	Video.Crear(hWnd, 120, 81, RC.right - 120, RC.bottom - 80, ID_VERVIDEO);
	Opciones.Crear(hWnd, 120, 81, RC.right - 120, RC.bottom - 80, ID_OPCIONES);
	Opciones.hWnd.Visible(FALSE);

	MarcoSI.Crear(hWnd, 10, 10, 360, 30, ID_MARCOSI);
	BotonAtras.Crear(MarcoSI.hWnd, L"<", 0, 0, 30, 30, ID_BOTON_ANTERIOR);
	BotonPlay.Crear(MarcoSI.hWnd, L"P", 40, 0, 30, 30, ID_BOTON_PLAY);
	BotonPausa.Crear(MarcoSI.hWnd, L"||", 80, 0, 30, 30, ID_BOTON_PAUSA);
	BotonStop.Crear(MarcoSI.hWnd, L"S", 120, 0, 30, 30, ID_BOTON_STOP);
	BotonAdelante.Crear(MarcoSI.hWnd, L">", 160, 0, 30, 30, ID_BOTON_SIGUIENTE);

	BotonMezclar.Crear(MarcoSI.hWnd, L"Mezclar", 210, 0, 70, 30, ID_BOTON_MEZCLAR, TRUE, DEnum_Button_Tipo_Split);
	BotonRepetir.Crear(MarcoSI.hWnd, L"Repetir", 290, 0, 70, 30, ID_BOTON_REPETIR, TRUE, DEnum_Button_Tipo_Split);

	SliderTiempo.CrearBarraEx(hWnd, 10, 50, RC.right - 20, 20, ID_SLIDER_TIEMPO);
//	SliderTiempo.Crear(hWnd, 10, 45, RC.right - 20, 24, ID_SLIDER_TIEMPO, WS_CHILD | TBS_NOTICKS | WS_VISIBLE, 0, 30000, 0);
//	SliderTiempo.TamPagina(30000 / 50);

	MarcoSD.Crear(hWnd, RC.right - 260, 14, 250, 24, ID_MARCOSD);
	SliderVolumen.CrearBarraEx(MarcoSD.hWnd, 120, 3, 90, 14, ID_SLIDER_VOLUMEN, 0, 200, 100);
	LabelVolumen.CrearLabelEx(MarcoSD.hWnd, L"100%", 215, 2, 40, 20, ID_LABEL_VOLUMEN, WS_CHILD | WS_VISIBLE);

	LabelTiempoActual.CrearLabelEx(MarcoSD.hWnd, L"00:00", 0, 2, 55, 20, ID_LABEL_TIEMPOACTUAL, WS_CHILD | WS_VISIBLE);
	LabelTiempoSeparador.CrearLabelEx(MarcoSD.hWnd, L"/", 55, 2, 10, 20, ID_LABEL_TIEMPOSEPARADOR, WS_CHILD | WS_VISIBLE);
	LabelTiempoTotal.CrearLabelEx(MarcoSD.hWnd, L"00:00", 65, 2, 55, 20, ID_LABEL_TIEMPOTOTAL, WS_CHILD | WS_VISIBLE);


	MarcoII.Crear(hWnd, 10, 80, 100, 160, ID_MARCOSI);

	BotonBD.CreadBotonEx(MarcoII.hWnd, L"Base de datos", 0, 0, 100, 30, ID_BOTON_BD);
	BotonLista.CreadBotonEx(MarcoII.hWnd, L"Lista de medios", 0, 40, 100, 30, ID_BOTON_LISTA);
	BotonVideo.CreadBotonEx(MarcoII.hWnd, L"Ver video", 0, 80, 100, 30, ID_BOTON_VIDEO);

	BotonOpciones.CreadBotonEx(MarcoII.hWnd, L"Opciones", 0, 120, 100, 30, ID_BOTON_OPCIONES);

	ShowWindow(hWnd(), nCmdShow);

	// Timer que comprueba si se ha terminado la cancion
	SetTimer(hWnd(), TIMER_LISTA, 500, NULL);
	// Timer para el slider del tiempo
	SetTimer(hWnd(), TIMER_TIEMPO, 250, NULL);

	return rhWnd;
}





void VentanaPrincipal::AjustarControles(RECT &RC) {
	// 50 ?? 60

	MoveWindow(Lista.hWnd(), 120, 81, RC.right - 120, RC.bottom - 80, TRUE);
	// Columnas de la lista  Ancho LV, Ancho Pista, Ancho Tiempo = Ancho restante para el nombre
	Lista.Columna(1)->Ancho(RC.right - 250);
	MoveWindow(Arbol.hWnd(), 120, 81, RC.right - 120, RC.bottom - 80, TRUE);
	MoveWindow(Video.hWnd(), 120, 81, RC.right - 120, RC.bottom - 80, TRUE);
	MoveWindow(Opciones.hWnd(), 120, 81, RC.right - 120, RC.bottom - 80, TRUE);

	MoveWindow(SliderTiempo.hWnd(), 10, 50, RC.right - 20, 24, TRUE);
	MoveWindow(MarcoSD.hWnd(), RC.right - 255, 14, 255, 24, TRUE);

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
	POINT Pt = { App.BD.Tabla_Opciones.PosX(), App.BD.Tabla_Opciones.PosY() };
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
			if (!hWnd.Minimizado()) {
				// Si el slider del tiempo tiene la captura, es porque se esta modificando el tiempo, por lo que no hay que actualizar la posición en ese momento.
				if (SliderTiempo.Estado() != DBarraEx_Estado_Presionado && App.ControlesPC.SliderTiempo.Estado() != DBarraEx_Estado_Presionado) {
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
		// Temporizador que detecta cuando se termina un medio y avanza al siguiente según las reglas establecidas
		case TIMER_LISTA:
			if (Errores > 10) {
				Lista_Stop();
				return;
			}
			// Un medio de la lista ha terminado
			if (Estado == Terminada) {				
				App.VLC.Stop();
				if (Lista.Pos == Lista.TotalItems() - 1) {
					// REPEAT
				}
				else {
					Lista_Siguiente();
				}
			}

			break;
		}	
}



void VentanaPrincipal::Lista_Pausa(void) {
	App.VLC.Pausa();
}


void VentanaPrincipal::Lista_Play(void) {
	if (Lista.TotalItems() == 0) return;

	Errores = 0;
	TablaMedios_Medio NCan;
	switch (App.VLC.ComprobarEstado()) {
		case SinCargar:
			if (Lista.TotalItems() > 0) {
				NCan.Obtener(App.BD(), Lista.Medio(Lista.Pos)->Hash);
				if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
				if (App.VLC.Play() == TRUE) {
//					Lista.Medio(Lista.Pos)->Icono(9);
				}
			}
			break;
		case Terminada:
			App.VLC.ActualizarIconos(0);
			Lista.Pos++;
			if (Lista.Pos >= static_cast<int>(Lista.TotalItems())) {
				Lista.Pos = 0;
			}
			NCan.Obtener(App.BD(), Lista.Medio(Lista.Pos)->Hash);
			if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
			App.VLC.Play();
			break;
		case EnStop:
		case EnPausa:
			App.VLC.Play();
			break;
	}
}


void VentanaPrincipal::Lista_Stop(void) {
	Errores = 0;
	App.VLC.Stop();
}


void VentanaPrincipal::Lista_Siguiente(void) {
	if (Lista.TotalItems() == 0) return;

	Lista.Pos++;
	if (Lista.Pos >= static_cast<int>(Lista.TotalItems())) Lista.Pos = 0;


	TablaMedios_Medio NCan(App.BD(), Lista.Medio(Lista.Pos)->Hash);
	if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
	App.VLC.Play();
}


void VentanaPrincipal::Lista_Anterior(void) {
	if (Lista.TotalItems() == 0) return;

	int TotalItems = static_cast<int>(Lista.TotalItems()) - 1;
	if (TotalItems == -1) TotalItems = 0;
	Lista.Pos--;
	if (Lista.Pos < 0) Lista.Pos = TotalItems;

	if (Lista.Pos >= 0 && Lista.Pos <= TotalItems) {
		TablaMedios_Medio NCan(App.BD(), Lista.Medio(Lista.Pos)->Hash);
		if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
		App.VLC.Play();
	}
}


void VentanaPrincipal::Evento_BotonEx_Mouse_Click(const UINT cID) {
	switch (cID) {
		case ID_BOTON_OPCIONES: 
			Arbol.hWnd.Visible(FALSE);
			Lista.hWnd.Visible(FALSE);
			Video.hWnd.Visible(FALSE);
			Opciones.hWnd.Visible(TRUE);
			Opciones.hWnd.AsignarFoco();
			break;
		case ID_BOTON_BD:
			Arbol.hWnd.Visible(TRUE);
			Lista.hWnd.Visible(FALSE);
			Video.hWnd.Visible(FALSE);
			Opciones.hWnd.Visible(FALSE);
			Arbol.hWnd.AsignarFoco();
			break;
		case ID_BOTON_LISTA:
			Arbol.hWnd.Visible(FALSE);
			Lista.hWnd.Visible(TRUE);
			Video.hWnd.Visible(FALSE);
			Opciones.hWnd.Visible(FALSE);
			Lista.hWnd.AsignarFoco();
			break;
		case ID_BOTON_VIDEO:
			Arbol.hWnd.Visible(FALSE);
			Lista.hWnd.Visible(FALSE);
			Video.hWnd.Visible(TRUE);
			Opciones.hWnd.Visible(FALSE);
			Video.hWnd.AsignarFoco();
			break;
	}
}

void VentanaPrincipal::Evento_Button_Mouse_Click(const UINT cID) {
	switch (cID) {
		case ID_BOTON_ANTERIOR:
			Lista_Anterior();
			break;
		case ID_BOTON_SIGUIENTE:
			Lista_Siguiente();
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



void VentanaPrincipal::Evento_TreeView_Mouse_Click(DTreeView_DatosClick *Datos, const UINT tID) {
	//Datos->Boton;
	if (Datos->Boton == 1 && tID == ID_ARBOLBD) {
		const BOOL IdMenu = App.Menu_ArbolBD.Mostrar(hWnd());
		switch (IdMenu) {
			case ID_MENUBD_AGREGARANUEVALISTA:
				Lista.BorrarListaReproduccion();
				Arbol.AgregarNodoALista(Datos->Nodo);
				Lista_Play();
				break;
			case ID_MENUBD_AGREGARALISTA:
				Arbol.AgregarNodoALista(Datos->Nodo);
				break;
			case ID_MENUBD_ACTUALIZAR:
				App.BD.ActualizarArbol();
				break;
		}
	}
}


void VentanaPrincipal::Evento_ListView_Mouse_Click(DListView_DatosClick *DatosClick, const UINT IDListView) {

}


void VentanaPrincipal::Evento_ListView_Mouse_DobleClick(DListView_DatosClick *DatosClick, const UINT IDListView) {
	if (DatosClick->Boton == 0 && IDListView == ID_LISTAMEDIOS) {		
		if (DatosClick->PosItem != -1) {
			Lista.Pos = DatosClick->PosItem;
			TablaMedios_Medio NCan(App.BD(), Lista.Medio(DatosClick->PosItem)->Hash);
			if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
			App.VLC.Play();
		}
	}
}



void VentanaPrincipal::Evento_CambiandoTam(const UINT Lado, RECT *Rectangulo) {
	// Comprobación de tamaño mínimo
	if ((Rectangulo->right - Rectangulo->left) < 650) Rectangulo->right = Rectangulo->left + 660;
	if ((Rectangulo->bottom - Rectangulo->top) < 260) Rectangulo->bottom = Rectangulo->top + 280;

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

		Arbol.hWnd.Visible(FALSE);
		Lista.hWnd.Visible(FALSE);
		SliderTiempo.hWnd.Visible(FALSE);
		MarcoSD.hWnd.Visible(FALSE);
		MarcoSI.hWnd.Visible(FALSE);
		MarcoII.hWnd.Visible(FALSE);

		Video.hWnd.Visible(FALSE);
		MoveWindow(Video.hWnd(), 0, 0, RC.right, RC.bottom, FALSE);
		Video.hWnd.Visible(TRUE);
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
		//MoveWindow(Video.hWnd(), 120, 71, RC.right - 120, RC.bottom - 70, TRUE);

		Arbol.hWnd.Visible(FALSE);
		Lista.hWnd.Visible(FALSE);
		GetClientRect(hWnd(), &RC);
		RC.right = RC.right - CTW_ExtraX;
		RC.bottom = RC.bottom - CTW_ExtraY;
		AjustarControles(RC);
		Video.hWnd.Visible(TRUE);
		SliderTiempo.hWnd.Visible(TRUE);
		MarcoSD.hWnd.Visible(TRUE);
		MarcoSI.hWnd.Visible(TRUE);
		MarcoII.hWnd.Visible(TRUE);
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

	std::wstring StrVol = std::to_wstring(Volumen) + L"%";
	LabelVolumen.Texto(StrVol);
	App.ControlesPC.LabelVolumen.Texto(StrVol);
}

// Guardo el volumen en las opciones (al soltar el slider del volumen)
void VentanaPrincipal::Evento_SliderVolumen_Cambiado() {
	App.BD.Tabla_Opciones.Volumen(static_cast<int>(SliderVolumen.Valor()));
}

void VentanaPrincipal::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}

LRESULT CALLBACK VentanaPrincipal::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_TIMER:	
			this->Evento_Temporizador(static_cast<UINT>(wParam));
			return 0;
		case WM_CLOSE : 
			App.BD.Tabla_Opciones.GuardarPosTamVentana();
			PostQuitMessage(0);
			break;
		case WM_COMMAND :
			if (HIWORD(wParam) == BN_CLICKED) {
				Evento_Button_Mouse_Click(LOWORD(wParam));
				return 0;
			}
			break;
		case WM_EXITSIZEMOVE  :
			if (App.VentanaRave.hWnd.Maximizada() == FALSE) {
				App.BD.Tabla_Opciones.GuardarPosTamVentana();
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
			break;


		case DWL_TREEVIEW_CLICK:
			this->Evento_TreeView_Mouse_Click(reinterpret_cast<DTreeView_DatosClick *>(lParam), static_cast<UINT>(wParam));
			return 0;

		case DWL_LISTVIEW_CLICK:
			this->Evento_ListView_Mouse_Click(reinterpret_cast<DListView_DatosClick *>(lParam), static_cast<UINT>(wParam));
			return 0;
		case DWL_LISTVIEW_DOBLECLICK:
			this->Evento_ListView_Mouse_DobleClick(reinterpret_cast<DListView_DatosClick *>(lParam), static_cast<UINT>(wParam));
			return 0;


		case WM_NOTIFY:
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
				case TVN_DELETEITEM:		 // Borrar item del TreeView																
					SendMessage(((LPNMTREEVIEW)lParam)->hdr.hwndFrom, DWL_TREEVIEW_BORRARNODO, 0, ((LPNMTREEVIEW)lParam)->itemOld.lParam);
					break;
				////////////////////////////////
				// Notificaciones Estandard : //
				////////////////////////////////
//				case NM_RELEASEDCAPTURE:
//					Evento_CapturaSoltada(((LPNMHDR)lParam)->idFrom);
				case NM_CLICK:
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
					break;

					// Notificación TVN_ITEMEXPANDING para recibir cuando se expande un nodo del treeview
					// Se necesita especificamente para la clase DTreeViewDirectorios ya que al expandir un nodo hay que escanear el directorio al que hace referencia.
				case TVN_ITEMEXPANDING:
					return SendMessage(((LPNMHDR)lParam)->hwndFrom, DWL_TREEVIEW_NODO_EXPANDIENDO, wParam, lParam);
					// Cambio de selección en el TreeView
				case TVN_SELCHANGED:
					return SendMessage(((LPNMHDR)lParam)->hwndFrom, DWL_TREEVIEW_NODO_CAMBIOSELECCION, wParam, lParam);
			}
	}
//	return FALSE;
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}