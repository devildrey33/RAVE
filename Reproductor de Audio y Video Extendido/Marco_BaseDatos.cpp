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
	const wchar_t* E1 = L"Para usuarios con una colección superior a 100 canciones, es muy\n"
						L"recomendable agregar uno o mas directorios que contengan dichas\n"
						L"canciones para que puedan ser analizadas y añadidas a la BD.\n\n"
						L"Una vez analizadas se podrá generar listas aleatórias de todo tipo.\n"
						L"Además tambien permitirá busquedas avanzadas por etiqueta.";
	EtiquetaBaseDeDatos1.CrearEtiquetaEx(this, E1, 10, 0, RC.right - 20, 120, ID_ETIQUETA_BASEDEDATOS);
	// Lista con las raices
	ListaRaiz.CrearListaRaiz(this, 10, 130, RC.right - 20, 80, ID_LISTARAIZ);
	for (size_t i = 0; i < App.Opciones.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.Opciones.Raiz(i)->Path.c_str());
	}
	ListaRaiz.Visible(TRUE);
	ListaRaiz.MultiSeleccion = FALSE;

	int x = ((RC.right - 250) / 2) - 10;
	// Botón agregar raíz
	BotonAgregarRaiz.CrearBotonEx(this, L"Agregar Raíz", x, 220, 120, 24, ID_BOTON_AGREGARRAIZ);
	BotonAgregarRaiz.Fuente.CrearFuente(21, BotonAgregarRaiz.Skin.FuenteNombre.c_str());
	// Botón eliminar raíz
	BotonEliminarRaiz.CrearBotonEx(this, L"Eliminar Raiz", x + 130, 220, 120, 24, ID_BOTON_ELIMINARRAIZ);
	BotonEliminarRaiz.Fuente.CrearFuente(21, BotonEliminarRaiz.Skin.FuenteNombre.c_str());
	BotonEliminarRaiz.Activado(FALSE);

	// Separador
	SeparadorBD.Crear(this, 0, 254, RC.right);
	// Marca Mostrar analisis en una ventana
	MarcaMostrarAnalisis.CrearMarcaEx(this, L"Mostrar ventana con el progreso del análisis de los medios.", 10, 269, 405, 20, ID_MARCA_MOSTRARANALISIS, IDI_CHECK2);
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
			if (MarcaMostrarAnalisis.Marcado() == TRUE)				App.MostrarToolTipOpciones(L"La ventana del análisis se mostrará siempre.");
			else													App.MostrarToolTipOpciones(L"La ventana del análisis no se mostrará más.");
			break;
		case ID_MARCA_ANALIZARPENDIENTES :
			App.Opciones.AnalizarMediosPendientes(MarcaAnalizarMediosPendientes.Marcado());
			if (MarcaAnalizarMediosPendientes.Marcado() == TRUE)	App.MostrarToolTipOpciones(L"Los medios pendientes se analizarán automáticamente.");
			else													App.MostrarToolTipOpciones(L"Los medios pendientes no se analizarán automaticamente.\n"
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