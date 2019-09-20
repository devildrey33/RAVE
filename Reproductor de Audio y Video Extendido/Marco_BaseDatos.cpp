#include "stdafx.h"
#include "Marco_BaseDatos.h"

// Base de datos
#define ID_ETIQUETA_BASEDEDATOS			1012
#define ID_MARCA_MOSTRARANALISIS		1013
#define ID_MARCA_ANALIZARPENDIENTES		1014

void Marco_BaseDatos::Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD | WS_VISIBLE);

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
	for (size_t i = 0; i < App.BD.TotalRaices(); i++) {
		ListaRaiz.AgregarRaiz(App.BD.Raiz(i)->Path.c_str());
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