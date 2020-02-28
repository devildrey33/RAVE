#include "stdafx.h"
#include "Marco_Inicio.h"

#define ID_MARCA_BUSCARACTUALIZACIONES  1061
#define ID_ETIQUETA_LISTAINICIO			1068
#define ID_DESPLEGABLE_LISTAINICIO		1069

void Marco_Inicio::Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	MarcaBuscarActualizaciones.CrearMarcaEx(this, L"Buscar y descargar actualizaciones al iniciar el reproductor", 10, 10, 420, 20, ID_MARCA_BUSCARACTUALIZACIONES, IDI_CHECK2);
	MarcaBuscarActualizaciones.Marcado(App.Opciones.BuscarActualizacion());

	// Separador 
	SeparadorInicio.Crear(this, 0, 40, RC.right);

	EtiquetaListaInicio.CrearEtiquetaEx(this, L"Que tipo de lista se debe generar al iniciar?", 10, 50, RC.right - 260, 20, ID_ETIQUETA_LISTAINICIO);
	DesplegableListaInicio.CrearListaDesplegable(this, L"", ID_DESPLEGABLE_LISTAINICIO, 0, RC.right - 215, 50, 185, 20, DEdicionTextoEx_Entrada_SinEntrada, 300);
	ActualizarListaInicio();



	CalcularPagina();
}


void Marco_Inicio::ActualizarListaInicio(void) {
	const wchar_t* Listas[8] = {
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


void Marco_Inicio::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_BUSCARACTUALIZACIONES :
			App.Opciones.BuscarActualizacion(MarcaBuscarActualizaciones.Marcado());
			if (MarcaBuscarActualizaciones.Marcado() == TRUE)		App.MostrarToolTipOpciones(L"Se buscarán actualizaciones automáticamente.");
			else 													App.MostrarToolTipOpciones(L"No se buscarán actualizaciones automáticamente.");
			break;
	}
}

void Marco_Inicio::Evento_ListaDesplegable_Cambio(INT_PTR nID) {
	// Lista de inicio
	if (nID == ID_DESPLEGABLE_LISTAINICIO) {
		std::wstring Opcion = DesplegableListaInicio.Texto();
		if (Opcion == L"Ninguna") {
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
}



LRESULT CALLBACK Marco_Inicio::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_MARCAEX_CLICK:
			Evento_MarcaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_DESPLEGABLEEX_CAMBIO:
			Evento_ListaDesplegable_Cambio(static_cast<INT_PTR>(wParam));
			return 0;
	}

	return DMarcoScrollEx::GestorMensajes(uMsg, wParam, lParam);
}