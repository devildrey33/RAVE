#include "stdafx.h"
#include "VentanaMomento.h"
#include "DStringUtils.h"

#define ID_LISTAMOMENTOS	WM_USER + 1000
#define ID_CREARMOMENTO		WM_USER + 1001
#define ID_EDITARMOMENTO    WM_USER + 1002
#define ID_ELIMINARMOMENTO	WM_USER + 1003
#define ID_TXTNOMBRE		WM_USER + 1004
#define ID_TXTINICIO		WM_USER + 1005
#define ID_TXTFINAL			WM_USER + 1006
#define ID_BARRAINICIO      WM_USER + 1007
#define ID_BARRAFINAL       WM_USER + 1008
#define ID_EXCLUIR          WM_USER + 1009
#define ID_GUARDAR          WM_USER + 1010
#define ID_CANCELAR         WM_USER + 1011
#define ID_INICIOMAS        WM_USER + 1012
#define ID_INICIOMENOS      WM_USER + 1013
#define ID_FINMAS           WM_USER + 1014
#define ID_FINMENOS         WM_USER + 1015

VentanaMomento::VentanaMomento(void) : PosMomento(-1) {
}

VentanaMomento::~VentanaMomento(void) {
}


void VentanaMomento::Mostrar(BDMedio& nMedio) {
	Medio = nMedio;
/*	if (_hWnd != NULL) {
		CargarMomentos();
		return;
	}*/
	CrearVentana(NULL, L"RAVE_Momentos", L"Momentos", App.Opciones.VentanaMomentos_PosX(), App.Opciones.VentanaMomentos_PosY(), 746, 240, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);

	ListaMomentos.CrearListaEx(this, 10, 10, 250, 140, ID_LISTAMOMENTOS, WS_CHILD | WS_VISIBLE);
	ListaMomentos.AgregarColumna();

	Boton_CrearMomento.CrearBotonEx(this, L"Crear", 10, 160, 70, 30, ID_CREARMOMENTO, WS_CHILD | WS_VISIBLE);
	Boton_EditarMomento.CrearBotonEx(this, L"Editar", 100, 160, 70, 30, ID_EDITARMOMENTO, WS_CHILD | WS_VISIBLE);
	Boton_EditarMomento.Activado(FALSE);
	Boton_EliminarMomento.CrearBotonEx(this, L"Eliminar", 190, 160, 70, 30, ID_ELIMINARMOMENTO, WS_CHILD | WS_VISIBLE);
	Boton_EliminarMomento.Activado(FALSE);

	Txt_Nombre.CrearEdicionTextoEx(this, L"", 270, 10, 450, 30, ID_TXTNOMBRE);
	Txt_Nombre.Placeholder = L"Nombre del momento";
	Txt_Nombre.Activado(FALSE);
	std::wstring Tiempo;
	App.MP.TiempoStr(0, Tiempo);
	Txt_TiempoInicio.CrearEdicionTextoEx(this, Tiempo.c_str(), 270, 50, 50, 20, ID_TXTINICIO);
	Txt_TiempoInicio.Entrada    = DEdicionTextoEx_Entrada_SinEntrada;
	Txt_TiempoInicio.Alineacion = DEdicionTextoEx_Alineacion_Derecha;
	Txt_TiempoInicio.Activado(FALSE);
	App.MP.TiempoStr(nMedio.Tiempo, Tiempo);
	Txt_TiempoFinal.CrearEdicionTextoEx(this, Tiempo.c_str(), 270, 80, 50, 20, ID_TXTFINAL);
	Txt_TiempoFinal.Entrada     = DEdicionTextoEx_Entrada_SinEntrada;
	Txt_TiempoFinal.Alineacion  = DEdicionTextoEx_Alineacion_Derecha;	
	Txt_TiempoFinal.Activado(FALSE);

	Boton_InicioMas.CrearBotonEx(this, L"+", 320, 50, 10, 10, ID_INICIOMAS, WS_CHILD | WS_VISIBLE);
	Boton_InicioMas.Fuente.CrearFuente(18, L"Courier New");
	Boton_InicioMenos.CrearBotonEx(this, L"-", 320, 60, 10, 10, ID_INICIOMENOS, WS_CHILD | WS_VISIBLE);
	Boton_InicioMenos.Fuente.CrearFuente(18, L"Courier New");

	Boton_FinMas.CrearBotonEx(this, L"+", 320, 80, 10, 10, ID_FINMAS, WS_CHILD | WS_VISIBLE);
	Boton_FinMas.Fuente.CrearFuente(18, L"Courier New");
	Boton_FinMenos.CrearBotonEx(this, L"-", 320, 90, 10, 10, ID_FINMENOS, WS_CHILD | WS_VISIBLE);
	Boton_FinMenos.Fuente.CrearFuente(18, L"Courier New");


	Barra_TiempoInicio.CrearBarraDesplazamientoEx(this, 340, 50, 380, 20, ID_BARRAINICIO);
	Barra_TiempoInicio.Activado(FALSE);
	Barra_TiempoFinal.CrearBarraDesplazamientoEx(this, 340, 80, 380, 20, ID_BARRAFINAL, 0.0f, 1.0f, 1.0f);
	Barra_TiempoFinal.Activado(FALSE);
	Check_Excluir.CrearMarcaEx(this, L"Momento a excluir de la reproducción (intro, creditos, etc...)", 270, 100, 413, 30, ID_EXCLUIR, IDI_CHECK2, WS_CHILD | WS_VISIBLE);
	Check_Excluir.Activado(FALSE);

	Boton_Guardar.CrearBotonEx(this, L"Guardar", 380, 135, 100, 30, ID_GUARDAR, WS_CHILD | WS_VISIBLE);
	Boton_Guardar.Activado(FALSE);
	Boton_Cancelar.CrearBotonEx(this, L"Cancelar", 520, 135, 100, 30, ID_CANCELAR, WS_CHILD | WS_VISIBLE);
	Boton_Cancelar.Activado(FALSE);

	CargarMomentos();
	Visible(TRUE);
}



void VentanaMomento::Evento_BarraEx_Cambiando(DWL::DEventoMouse& DatosMouse) {
	static std::wstring TmpStr;
	switch (DatosMouse.ID()) {
		case ID_BARRAINICIO :
			App.MP.TiempoStr(static_cast<INT64>(static_cast<double>(Medio.Tiempo) * Barra_TiempoInicio.Valor()), TmpStr);
			Txt_TiempoInicio.Texto(TmpStr);
			break;
		case ID_BARRAFINAL:
			App.MP.TiempoStr(static_cast<INT64>(static_cast<double>(Medio.Tiempo) * Barra_TiempoFinal.Valor()), TmpStr);
			Txt_TiempoFinal.Texto(TmpStr);
			break;
	}
}

void VentanaMomento::Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse) {
	static std::wstring TmpStr;
	switch (DatosMouse.ID()) {
		case ID_BARRAINICIO :
			App.MP.TiempoStr(static_cast<INT64>(static_cast<double>(Medio.Tiempo) * Barra_TiempoInicio.Valor()), TmpStr);
			Txt_TiempoInicio.Texto(TmpStr);
			break;
		case ID_BARRAFINAL:
			App.MP.TiempoStr(static_cast<INT64>(static_cast<double>(Medio.Tiempo) * Barra_TiempoFinal.Valor()), TmpStr);
			Txt_TiempoFinal.Texto(TmpStr);
			break;
	}
}

void VentanaMomento::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) { 
	switch (DatosMouse.ID()) {
		case ID_CREARMOMENTO :
			PosMomento = -1;
			ActivarControles(TRUE);
			return;
		case ID_EDITARMOMENTO :
			ActivarControles(TRUE);
			return;
		case ID_CANCELAR:
			ActivarControles(FALSE);
			return;
		case ID_GUARDAR:
			GuardarMomento();
			return;
		case ID_ELIMINARMOMENTO:
			EliminarMomento();
			return;
		case ID_INICIOMAS:
			Barra_TiempoInicio.Valor(AlterarTiempo(Txt_TiempoInicio, 1));
			return;
		case ID_INICIOMENOS:
			Barra_TiempoInicio.Valor(AlterarTiempo(Txt_TiempoInicio, -1));
			return;
		case ID_FINMAS:
			Barra_TiempoFinal.Valor(AlterarTiempo(Txt_TiempoFinal, 1));
			return;
		case ID_FINMENOS:
			Barra_TiempoFinal.Valor(AlterarTiempo(Txt_TiempoFinal, -1));
			return;
	}
}

// Función que altera el tiempo de los editbox para sumarle o restarle 1 segundo
const float VentanaMomento::AlterarTiempo(DWL::DEdicionTextoEx& Edicion, const UINT64 Valor) {
	INT64 Tiempo = static_cast<INT64>(App.MP.TiempoStr_Ms(Edicion.Texto()));
	Tiempo += (Valor * 1000);
	if (Tiempo < 0) Tiempo = 0;
	std::wstring TiempoStr;
	App.MP.TiempoStr(Tiempo, TiempoStr);
	Edicion.Texto(TiempoStr);
	// Devuelvo el tiempo en % de 0 a 1 (para las barras)
	float R = 1.0f / Medio.Tiempo;
	return R * static_cast<float>(Tiempo);
}

void VentanaMomento::Evento_ListaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	if (ListaMomentos.ItemMarcado() != NULL) {
		PosMomento = ListaMomentos.ItemPos(ListaMomentos.ItemMarcado());
		MostrarMomento(PosMomento);
		ActivarEditarEliminar(TRUE);
	}
	else {
		PosMomento = -1;
		ActivarEditarEliminar(FALSE);
	}
}

void VentanaMomento::EliminarMomento(void) {
	if (App.BD.EliminarMomento(&Medio, Medio.Momentos[PosMomento]->Id) == TRUE) {
		ListaMomentos.EliminarItem(PosMomento);
		ListaMomentos.Repintar(TRUE);
		//	CargarMomentos();
		PosMomento = ListaMomentos.ItemPos(ListaMomentos.ItemMarcado());
		ActivarEditarEliminar((PosMomento == -1) ? FALSE : TRUE);
	}
}

void VentanaMomento::MostrarMomento(const size_t Pos) {
	if (Pos < Medio.Momentos.size()) {
		std::wstring TmpStr;
		Txt_Nombre.Texto(Medio.Momentos[Pos]->Nombre.c_str(), TRUE);
		App.MP.TiempoStr(Medio.Momentos[Pos]->TiempoInicio, TmpStr);
		Txt_TiempoInicio.Texto(TmpStr);
		App.MP.TiempoStr(Medio.Momentos[Pos]->TiempoFinal, TmpStr);
		Txt_TiempoFinal.Texto(TmpStr);
		float v = (1.0f / static_cast<float>(Medio.Tiempo)) * static_cast<float>(Medio.Momentos[Pos]->TiempoInicio);
		Barra_TiempoInicio.Valor(v);
		Barra_TiempoFinal.Valor((1.0f / static_cast<float>(Medio.Tiempo)) * static_cast<float>(Medio.Momentos[Pos]->TiempoFinal));
		Check_Excluir.Marcado(Medio.Momentos[Pos]->Excluir);
	}
}

void VentanaMomento::ActivarControles(const BOOL nActivar) {
	Txt_Nombre.Activado(nActivar);
	Txt_TiempoInicio.Activado(nActivar);
	Txt_TiempoFinal.Activado(nActivar);
	Barra_TiempoInicio.Activado(nActivar);
	Barra_TiempoFinal.Activado(nActivar);
	Check_Excluir.Activado(nActivar);
	Boton_Guardar.Activado(nActivar);
	Boton_Cancelar.Activado(nActivar);
	Boton_InicioMas.Activado(nActivar);
	Boton_InicioMenos.Activado(nActivar);
	Boton_FinMas.Activado(nActivar);
	Boton_FinMenos.Activado(nActivar);
}

void VentanaMomento::ActivarEditarEliminar(const BOOL nActivar) {
	Boton_EditarMomento.Activado(nActivar);
	Boton_EliminarMomento.Activado(nActivar);
}

void VentanaMomento::CargarMomentos(void) {
	// Vuelvo a cargar el medio de la base de datos (por si hay nuevos momentos)
	if (App.BD.ObtenerMedio(Medio.Hash, Medio) == FALSE) return;

	ListaMomentos.EliminarTodosLosItems();
	for (size_t i = 0; i < Medio.Momentos.size(); i++) {
		ListaMomentos.AgregarItem((Medio.Momentos[i]->Excluir) ? IDI_MOMENTO_EXCLUIR : IDI_MOMENTO, DLISTAEX_POSICION_FIN, { Medio.Momentos[i]->Nombre });
	}
	ListaMomentos.Repintar(TRUE);

	ActivarEditarEliminar(FALSE);
}


void VentanaMomento::GuardarMomento(void) {
	// Error no hay nommbre
	if (Txt_Nombre.Texto().size() == 0) {
		App.MostrarToolTipMomentosError(L"Introduce un nombre para el momento");
		return;
	}

	// Error el momento no puede ser todo el medio
	UINT64 TiempoInicio = App.MP.TiempoStr_Ms(Txt_TiempoInicio.Texto());
	UINT64 TiempoFinal  = App.MP.TiempoStr_Ms(Txt_TiempoFinal.Texto());
//	DWL::Strings::StrTo(Txt_TiempoInicio.Texto(), TiempoInicio);
//	DWL::Strings::StrTo(Txt_TiempoFinal.Texto(), TiempoFinal);	
	if (TiempoInicio == 0 && TiempoFinal == Medio.Tiempo) {
		App.MostrarToolTipMomentosError(L"El momento debe ser una fracción del medio, no todo el medio.");
		return;
	}

	// Error el tiempo final es menor que el tiempo inicial
	if (TiempoInicio > TiempoFinal) {
		App.MostrarToolTipMomentosError(L"El tiempo inicial es mayor que el tiempo final.");
		return;
	}

	// Si es un nuevo momento
	if (PosMomento == -1) {
		if (App.BD.AgregarMomento(&Medio, Txt_Nombre.Texto(), TiempoInicio, TiempoFinal, Check_Excluir.Marcado())) {
			Medio.Momentos[PosMomento]->Nombre = Txt_Nombre.Texto();
			App.MostrarToolTipMomentos(L"Momento creado");
		}
		else {
			App.MostrarToolTipMomentosError(L"Error inesperado al crear el momento");
		}
	}
	// Si es un momento a editar
	else {
		if (App.BD.ModificarMomento(&Medio, Medio.Momentos[PosMomento]->Id, Txt_Nombre.Texto(), TiempoInicio, TiempoFinal, Check_Excluir.Marcado())) {
			Medio.Momentos[PosMomento]->Nombre = Txt_Nombre.Texto();
			App.MostrarToolTipMomentos(L"Momento actualizado");
		}
		else {
			App.MostrarToolTipMomentosError(L"Error inesperado al actualizar el momento");
		}
	}

	// Actualizo el BDMedio actual si es el mismo que el que se está editando
	if (Medio.Hash == App.MP.MedioActual().Hash) {
		Medio.PosMomento = -1;
		App.MP.MedioActual(Medio);
	}

	ActivarControles(FALSE);
	CargarMomentos();
}

LRESULT CALLBACK VentanaMomento::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		 
		case DWL_BARRAEX_CAMBIANDO:	// Se está modificando (mouse down)
			Evento_BarraEx_Cambiando(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

		case WM_EXITSIZEMOVE:
			App.Opciones.GuardarPosVentanaMomentos();
			return 0;

		case WM_MOVING:
			App.OcultarToolTipMomentos();
			return 0;

		case DWL_LISTAEX_MOUSESOLTADO :
			Evento_ListaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}