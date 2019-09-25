#include "stdafx.h"
#include "Marco_Listas.h"


// Lista
#define ID_ETIQUETA_LISTAINICIO			1068
#define ID_DESPLEGABLE_LISTAINICIO		1069
#define ID_ETIQUETA_CANCIONES25			1070
#define ID_MARCA_CANCIONES25			1071
#define ID_ETIQUETA_NOGENERARLISTAS3	1072
#define ID_MARCA_NOGENERARLISTAS3		1073
#define ID_ETIQUETA_SUMAR005			1074
#define ID_MARCA_SUMAR005				1075

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

void Marco_Listas::Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD);

	RECT RC;
	GetClientRect(_hWnd, &RC);


	EtiquetaCanciones25.CrearEtiquetaEx(this, L"Por defecto todos los medios tienen una nota de 2.5 sobre 5, si alguna canción "
											  L"tiene una nota inferior a 2.5, el reproductor considerará el medio como no apto."	, 10, 0, RC.right - 40, 60, ID_ETIQUETA_CANCIONES25);		
	MarcaCanciones25.CrearMarcaEx(this, L"No añadir canciones con menos de 2.5 de nota a listas aleatórias"							, 10, 70, 448, 20, ID_MARCA_CANCIONES25, IDI_CHECK2);
	MarcaCanciones25.Marcado(App.Opciones.NoAgregarMedioMenos25());

	SeparadorLista1.Crear(this, 0, 100, RC.right - (_ScrollV_Ancho + 5));

	EtiquetaNoGenerarListas3.CrearEtiquetaEx(this,	L"Al generar una lista aleatória partiendo de una etiqueta, puede darse el caso que "
													L"esa etiqueta tenga menos de 3 canciones."									, 10, 110, RC.right - 40, 40, ID_ETIQUETA_NOGENERARLISTAS3);
	MarcaNoGenerarListas3.CrearMarcaEx(this, L"No generar listas aleatórias con menos de 3 canciones"							, 10, 160, 361, 20, ID_MARCA_NOGENERARLISTAS3, IDI_CHECK2);
	MarcaNoGenerarListas3.Marcado(App.Opciones.NoGenerarListasMenos3());

	SeparadorLista2.Crear(this, 0, 190, RC.right - (_ScrollV_Ancho + 5));
	EtiquetaSumar005.CrearEtiquetaEx(this,	L"Una vez el medio llegue al final se sumará 0.05 a su nota, esto quiere decir que "
											L"partiendo de una nota de 2.5, el medio alcanzará las 5 estrellas en 50 veces. "	, 10, 200, RC.right - 40, 60, ID_ETIQUETA_SUMAR005);
	MarcaSumar005.CrearMarcaEx(this, L"Sumar 0.05 a la nota de los medios reproducidos hasta el final"							, 10, 270, 449, 20, ID_MARCA_SUMAR005, IDI_CHECK2);
	MarcaSumar005.Marcado(App.Opciones.Sumar005());

	SeparadorLista3.Crear(this, 0, 300, RC.right - (_ScrollV_Ancho + 5));
/*	EtiquetaListaInicio.CrearEtiquetaEx(this, L"Que tipo de lista se debe generar al iniciar?"				, 10, 320, RC.right - 260, 20, ID_ETIQUETA_LISTAINICIO);
	DesplegableListaInicio.CrearListaDesplegable(this, L"", ID_DESPLEGABLE_LISTAINICIO, 0					, RC.right - 215, 320, 185, 20, DEdicionTextoEx_Entrada_SinEntrada, 300);
	ActualizarListaInicio(); */


//	SeparadorLista4.Crear(this, 0, 350, RC.right - 10);



	///////////////////////////////////////
	// Creo los controles del marco Listas2
	/////////////////////////////////////// 
	EtiquetaListaMezclar.CrearEtiquetaEx(this,  L"Elige si se deben mezclar las listas generadas aleatóriamente.\n"
												L"Si seleccionas 'Por defecto' solo se mezclara la lista si el botón mezclar ya estaba activado.", 10, 310, RC.right - 20, 60, ID_ETIQUETA_LISTAMEZCLAR);
	const wchar_t* Mezclar[3] = {
		L"Por defecto",
		L"Mezclar",
		L"No mezclar"
	};

	// Genero
	EtiquetaListaGenero.CrearEtiquetaEx(this, L"Mezclar listas por Genero", 10, 380, RC.right - 260, 20, ID_ETIQUETA_LISTAGENERO);
	DesplegableListaGenero.CrearListaDesplegable(this, Mezclar[App.Opciones.MezclarListaGenero()], ID_DESPLEGABLE_LISTAGENERO, 0, RC.right - 240, 380, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaGenero.AgregarItem(Mezclar[i]);
	// Grupo
	EtiquetaListaGrupo.CrearEtiquetaEx(this, L"Mezclar listas por Grupo", 10, 405, RC.right - 260, 20, ID_ETIQUETA_LISTAGRUPO);
	DesplegableListaGrupo.CrearListaDesplegable(this, Mezclar[App.Opciones.MezclarListaGrupo()], ID_DESPLEGABLE_LISTAGRUPO, 0, RC.right - 240, 405, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaGrupo.AgregarItem(Mezclar[i]);
	// Disco
	EtiquetaListaDisco.CrearEtiquetaEx(this, L"Mezclar listas por Disco", 10, 430, RC.right - 260, 20, ID_ETIQUETA_LISTADISCO);
	DesplegableListaDisco.CrearListaDesplegable(this, Mezclar[App.Opciones.MezclarListaDisco()], ID_DESPLEGABLE_LISTADISCO, 0, RC.right - 240, 430, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaDisco.AgregarItem(Mezclar[i]);
	// 50 Can
	EtiquetaLista50Can.CrearEtiquetaEx(this, L"Mezclar listas con 50 Canciones", 10, 455, RC.right - 260, 20, ID_ETIQUETA_LISTA50CAN);
	DesplegableLista50Can.CrearListaDesplegable(this, Mezclar[App.Opciones.MezclarLista50Can()], ID_DESPLEGABLE_LISTA50CAN, 0, RC.right - 240, 455, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableLista50Can.AgregarItem(Mezclar[i]);
	// Nota
	EtiquetaListaNota.CrearEtiquetaEx(this, L"Mezclar listas por Nota", 10, 480, RC.right - 260, 20, ID_ETIQUETA_LISTANOTA);
	DesplegableListaNota.CrearListaDesplegable(this, Mezclar[App.Opciones.MezclarListaNota()], ID_DESPLEGABLE_LISTANOTA, 0, RC.right - 240, 480, 210, 20, DEdicionTextoEx_Entrada_SinEntrada, 80);
	for (int i = 0; i < 3; i++) DesplegableListaNota.AgregarItem(Mezclar[i]);



	CalcularPagina();
}


/*
void Marco_Listas::ActualizarListaInicio(void) {
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
	if (App.BD.TotalRaices() > 0) {
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
}*/


void Marco_Listas::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_CANCIONES25:
			if (MarcaCanciones25.Marcado() == TRUE) {
				App.Opciones.NoAgregarMedioMenos25(TRUE);
				App.MostrarToolTipOpciones(L"No se agregaran medios con una nota inferior a 2.5 a las listas aleatórias.");
			}
			else {
				App.Opciones.NoAgregarMedioMenos25(FALSE);
				App.MostrarToolTipOpciones(L"Se agregaran medios con cualquier nota a las listas aleatórias.");
			}
			break;
		case ID_MARCA_NOGENERARLISTAS3:
			if (MarcaNoGenerarListas3.Marcado() == TRUE) {
				App.Opciones.NoGenerarListasMenos3(TRUE);
				App.MostrarToolTipOpciones(L"No se generaran listas aleatórias con menos de 3 canciones.");
			}
			else {
				App.Opciones.NoGenerarListasMenos3(FALSE);
				App.MostrarToolTipOpciones(L"Las listas aleatórias pueden tener menos de 3 canciones.");
			}
			break;
		case ID_MARCA_SUMAR005:
			if (MarcaSumar005.Marcado() == TRUE) {
				App.Opciones.Sumar005(TRUE);
				App.MostrarToolTipOpciones(L"Al terminar un medio se sumara 0.05 a su nota.");
			}
			else {
				App.Opciones.Sumar005(FALSE);
				App.MostrarToolTipOpciones(L"No se modificara la nota al terminar un medio.");
			}
			break;
	}
}


void Marco_Listas::Evento_ListaDesplegable_Cambio(INT_PTR nID) {


	// Mezclar listas por genero
	if (nID == ID_DESPLEGABLE_LISTAGENERO) {
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



LRESULT CALLBACK Marco_Listas::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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