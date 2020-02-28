#include "stdafx.h"
#include "Marco_BaseDatos.h"
#include <DDlgDirectorios.h>

// Base de datos
#define ID_ETIQUETA_BASEDEDATOS			1012
#define ID_MARCA_MOSTRARANALISIS		1013
#define ID_MARCA_ANALIZARPENDIENTES		1014

void Marco_BaseDatos::Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	//////////////////////////////////////////////////////
	// Creo los controles dentro del marco Base de datos
	//////////////////////////////////////////////////////
	const wchar_t* E1 = L"Para usuarios con una colecci�n superior a 100 canciones, es muy\n"
						L"recomendable agregar uno o mas directorios que contengan dichas\n"
						L"canciones para que puedan ser analizadas y a�adidas a la BD.\n\n"
						L"Una vez analizadas se podr� generar listas aleat�rias de todo tipo.\n"
						L"Adem�s tambien permitir� busquedas avanzadas por etiqueta.";
	EtiquetaBaseDeDatos1.CrearEtiquetaEx(this, E1, 10, 0, RC.right - 20, 120, ID_ETIQUETA_BASEDEDATOS);
	// Lista con las raices
	ListaRaiz.CrearListaRaiz(this, 10, 130, RC.right - 20, 80, ID_LISTARAIZ);
	for (size_t i = 0; i < App.Opciones.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.Opciones.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Visible(TRUE);
	ListaRaiz.MultiSeleccion = FALSE;

	int x = ((RC.right - 250) / 2) - 10;
	// Bot�n agregar ra�z
	BotonAgregarRaiz.CrearBotonEx(this, L"Agregar Ra�z", x, 220, 120, 24, ID_BOTON_AGREGARRAIZ);
	BotonAgregarRaiz.Fuente.CrearFuente(21, BotonAgregarRaiz.Skin.FuenteNombre.c_str());
	// Bot�n eliminar ra�z
	BotonEliminarRaiz.CrearBotonEx(this, L"Eliminar Raiz", x + 130, 220, 120, 24, ID_BOTON_ELIMINARRAIZ);
	BotonEliminarRaiz.Fuente.CrearFuente(21, BotonEliminarRaiz.Skin.FuenteNombre.c_str());
	BotonEliminarRaiz.Activado(FALSE);

	// Separador
	SeparadorBD.Crear(this, 0, 254, RC.right);
	// Marca Mostrar analisis en una ventana
	MarcaMostrarAnalisis.CrearMarcaEx(this, L"Mostrar ventana con el progreso del an�lisis de los medios.", 10, 269, 405, 20, ID_MARCA_MOSTRARANALISIS, IDI_CHECK2);
	MarcaMostrarAnalisis.Marcado(App.Opciones.MostrarObtenerMetadatos());
	// Analizar medios pendientes al actualizar la base de datos
	MarcaAnalizarMediosPendientes.CrearMarcaEx(this, L"Analizar medios pendientes al actualizar la base de datos.", 10, 299, 395, 20, ID_MARCA_ANALIZARPENDIENTES, IDI_CHECK2);
	MarcaAnalizarMediosPendientes.Marcado(App.Opciones.AnalizarMediosPendientes());


	CalcularPagina();
}


void Marco_BaseDatos::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BOTON_AGREGARRAIZ:
			AgregarRaiz();
			break;
		case ID_BOTON_ELIMINARRAIZ:
			if (ListaRaiz.ItemMarcado() != NULL) {
				EliminarRaiz(ListaRaiz.ItemMarcado()->Texto(0));
			}
			break;
	}
}



void Marco_BaseDatos::AgregarRaiz(void) {
	DDlgDirectorios          DialogoDirectorios;
	std::wstring             Path;

	App.VentanaRave.ThreadActualizar.Cancelar(TRUE);
	App.VentanaRave.ThreadAnalizar.Cancelar(TRUE);

	// No muestro las redes
	DialogoDirectorios.ArbolDirectorios.MostrarRed = FALSE;

	BOOL Ret = DialogoDirectorios.Mostrar(&App.VentanaOpciones2, Path, App.Opciones.DlgDirectorios_PosX(), App.Opciones.DlgDirectorios_PosY(), App.Opciones.DlgDirectorios_Ancho(), App.Opciones.DlgDirectorios_Alto(), IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
	//	SetFocus(_hWnd);
	if (Ret == TRUE) {
		// Agrego la ra�z a la BD.
		//		BDRaiz *nRaiz = NULL;
		int AR = App.Opciones.AgregarRaiz(Path, App.Unidades);
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

		App.VentanaOpciones2.MarcoInicio.ActualizarListaInicio();
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

void Marco_BaseDatos::EliminarRaiz(std::wstring& Path) {
	App.VentanaRave.ThreadActualizar.Cancelar(TRUE);
	App.VentanaRave.ThreadAnalizar.Cancelar(TRUE);
	App.Opciones.EliminarRaiz(Path);
	ListaRaiz.EliminarItem(ListaRaiz.ItemMarcado());
	ListaRaiz.Repintar();
	App.VentanaOpciones2.MarcoInicio.ActualizarListaInicio();
	App.VentanaRave.ActualizarArbol();

	if (ListaRaiz.ItemMarcado() == NULL) {
		BotonEliminarRaiz.Activado(FALSE);
	}
}

void Marco_BaseDatos::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_MOSTRARANALISIS :
			App.Opciones.MostrarObtenerMetadatos(MarcaMostrarAnalisis.Marcado());
			if (MarcaMostrarAnalisis.Marcado() == TRUE)				App.MostrarToolTipOpciones(L"La ventana del an�lisis se mostrar� siempre.");
			else													App.MostrarToolTipOpciones(L"La ventana del an�lisis no se mostrar� m�s.");
			break;
		case ID_MARCA_ANALIZARPENDIENTES :
			App.Opciones.AnalizarMediosPendientes(MarcaAnalizarMediosPendientes.Marcado());
			if (MarcaAnalizarMediosPendientes.Marcado() == TRUE)	App.MostrarToolTipOpciones(L"Los medios pendientes se analizar�n autom�ticamente.");
			else													App.MostrarToolTipOpciones(L"Los medios pendientes no se analizar�n automaticamente.\n"
																							   L"Para analizar los medios pendientes, haz click derecho encima de la base de datos, y pulsa Analizar.");
			break;

	}
}

void Marco_BaseDatos::Evento_ListaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	if (DatosMouse.ID() == ID_LISTARAIZ) {
		if (ListaRaiz.ItemMarcado() != NULL) {
			BotonEliminarRaiz.Activado(TRUE);
		}
		else {
			BotonEliminarRaiz.Activado(FALSE);
		}
	}
}



LRESULT CALLBACK Marco_BaseDatos::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_BOTONEX_CLICK :
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_MARCAEX_CLICK :
			Evento_MarcaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_LISTAEX_CLICK :
			Evento_ListaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
	}

	return DMarcoScrollEx::GestorMensajes(uMsg, wParam, lParam);
}