#include "stdafx.h"
#include "Marco_UI.h"
#include <DStringUtils.h>

#define ID_ETIQUETA_TIEMPOANIMACIONES   1062
#define ID_ETIQUETA_TIEMPOANIMACIONES2  1063
#define ID_BARRA_TIEMPOANIMACION        1064
#define ID_ETIQUETA_TIEMPOTOOLTIP       1065
#define ID_ETIQUETA_TIEMPOTOOLTIP2      1066
#define ID_BARRA_TIEMPOTOOLTIP          1067
#define ID_MARCA_OCULTARTOOLTIPS        1099 
#define ID_MARCA_MOSTRARMEDIOTITULO     1098
#define ID_ETIQUETA_PANTALLACOMPLETA    1097

#define ID_ETIQUETA_VIDEOTIEMPO			1076
#define ID_BARRA_VIDEOTIEMPO			1077
#define ID_ETIQUETA_VIDEOOPACIDAD		1078
#define ID_BARRA_VIDEOOPACIDAD          1079
#define ID_ETIQUETA_VIDEOPOSICION		1080
#define ID_DESPLEGABLE_VIDEOPOSICION    1081
#define ID_ETIQUETA_VIDEOTIEMPOTA       1082
#define ID_ETIQUETA_VIDEOOPACIDADTA		1083


void Marco_UI::Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD);

	RECT RC;
	GetClientRect(_hWnd, &RC);


	// Etiquetas Tiempo de la animación
	EtiquetaTiempoAnimaciones.CrearEtiquetaEx(this, L"Duración de las animaciones de color", 10, 10, 250, 20, ID_ETIQUETA_TIEMPOANIMACIONES);
	std::wstring TmpStr = std::to_wstring(App.Opciones.TiempoAnimaciones()) + L" ms";
	EtiquetaTiempoAnimacionesTA.CrearEtiquetaEx(this, TmpStr.c_str(), 410, 10, 60, 20, ID_ETIQUETA_TIEMPOANIMACIONES2, DEtiquetaEx_Alineacion_Derecha);
	// Barra para asignar el tiempo de las animaciones
	BarraTiempoAnimaciones.CrearBarraDesplazamientoEx(this, 260, 10, RC.right - 340, 20, ID_BARRA_TIEMPOANIMACION, 100, 1000, static_cast<float>(App.Opciones.TiempoAnimaciones()));
	// Etiquetas de tiempo para ocultar el tooltip
	EtiquetaTiempoOcultarToolTips.CrearEtiquetaEx(this, L"Duración de los tooltips", 10, 40, 250, 20, ID_ETIQUETA_TIEMPOTOOLTIP);
	TmpStr = std::to_wstring(App.Opciones.TiempoToolTips()) + L" ms";
	EtiquetaTiempoOcultarToolTipsTA.CrearEtiquetaEx(this, TmpStr.c_str(), 400, 40, 70, 20, ID_ETIQUETA_TIEMPOTOOLTIP2, DEtiquetaEx_Alineacion_Derecha);
	// Barra para asignar el tiempo de ocultación de los tooltips
	BarraTiempoToolTips.CrearBarraDesplazamientoEx(this, 260, 40, RC.right - 340, 20, ID_BARRA_TIEMPOTOOLTIP, 1000, 10000, static_cast<float>(App.Opciones.TiempoToolTips()));

	// Separador 
	SeparadorUI.Crear(this, 0, 70, RC.right);

	// Marca para ocultar los tooltips al pasar con el mouse
	MarcaOcultarTooltipsMouse.CrearMarcaEx(this, L"Ocultar tooltips al pasar por encima con el mouse", 10, 80, 350, 30, ID_MARCA_OCULTARTOOLTIPS, IDI_CHECK2);
	MarcaOcultarTooltipsMouse.Marcado(App.Opciones.OcultarTooltipsMouse());
	// Marca para mostrar el nombre del medio actual en el titulo del reproductor
	MarcaMostrarNombreTitulo.CrearMarcaEx(this, L"Mostrar el nombre del medio actual en el titulo del reproductor", 10, 110, 435, 30, ID_MARCA_MOSTRARMEDIOTITULO, IDI_CHECK2);
	MarcaMostrarNombreTitulo.Marcado(App.Opciones.MostrarMedioActualTitulo());

	// Separador 
	SeparadorUI2.Crear(this, 0, 150, RC.right);


	EtiquetaPantallaCompleta.CrearEtiquetaEx(this, L"Configuración para los controles en modo pantalla completa", 0, 160, 400, 20, ID_ETIQUETA_PANTALLACOMPLETA, DEtiquetaEx_Alineacion_Derecha);


	///////////////////////////////////////
	// Creo los controles del marco Video
	///////////////////////////////////////
	EtiquetaVideoTiempo.CrearEtiquetaEx(this, L"Tiempo para ocultar los controles", 10, 190, 220, 20, ID_ETIQUETA_VIDEOTIEMPO);
	BarraVideoTiempo.CrearBarraDesplazamientoEx(this, 240, 190, 160, 20, ID_BARRA_VIDEOTIEMPO, 1000, 5000, static_cast<float>(App.Opciones.OcultarMouseEnVideo()));
	TmpStr = std::to_wstring(App.Opciones.OcultarMouseEnVideo()) + L" ms";
	EtiquetaVideoTiempoTA.CrearEtiquetaEx(this, TmpStr.c_str(), 410, 190, 60, 20, ID_ETIQUETA_VIDEOTIEMPOTA, DEtiquetaEx_Alineacion_Derecha);

	EtiquetaVideoOpacidad.CrearEtiquetaEx(this, L"Opacidad para los controles", 10, 220, 200, 20, ID_ETIQUETA_VIDEOOPACIDAD);
	BarraVideoOpacidad.CrearBarraDesplazamientoEx(this, 240, 220, 160, 20, ID_BARRA_VIDEOOPACIDAD, 50, 255, static_cast<float>(App.Opciones.OpacidadControlesVideo()));
	TmpStr = std::to_wstring(App.Opciones.OpacidadControlesVideo()) + L"/255";
	EtiquetaVideoOpacidadTA.CrearEtiquetaEx(this, TmpStr.c_str(), 410, 220, 60, 20, ID_ETIQUETA_VIDEOOPACIDADTA, DEtiquetaEx_Alineacion_Derecha);

	EtiquetaVideoPosicion.CrearEtiquetaEx(this, L"Posición de los controles", 10, 250, 300, 20, ID_ETIQUETA_VIDEOPOSICION);

	const wchar_t* PosVideo[4] = {
		L"Abajo",
		L"Izquierda",
		L"Arriba",
		L"Derecha"
	};
	DesplegableVideoPosicion.CrearListaDesplegable(this, PosVideo[App.Opciones.AlineacionControlesVideo()], ID_DESPLEGABLE_VIDEOPOSICION, NULL, 320, 250, 150, 20, DEdicionTextoEx_Entrada_SinEntrada, 300);
	for (int i = 0; i < 4; i++) {
		DesplegableVideoPosicion.AgregarItem(PosVideo[i]);
	}


	CalcularPagina();
}



void Marco_UI::Evento_ListaDesplegable_Cambio(INT_PTR nID) {
	// Posición de la barra de controles en modo pantalla completa
	if (nID == ID_DESPLEGABLE_VIDEOPOSICION) {
		std::wstring Opcion = DesplegableVideoPosicion.Texto();
		if (Opcion == L"Abajo") {
			App.Opciones.AlineacionControlesVideo(0);
			App.ControlesPC.Alineacion = Abajo;
		}
		else if (Opcion == L"Izquierda") {
			App.Opciones.AlineacionControlesVideo(1);
			App.ControlesPC.Alineacion = Izquierda;
		}
		else if (Opcion == L"Arriba") {
			App.Opciones.AlineacionControlesVideo(2);
			App.ControlesPC.Alineacion = Arriba;
		}
		else if (Opcion == L"Derecha") {
			App.Opciones.AlineacionControlesVideo(3);
			App.ControlesPC.Alineacion = Derecha;
		}
		App.MostrarToolTipOpciones(L"Los controles del video se mostrarán " + Opcion);
	}
}


void Marco_UI::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_OCULTARTOOLTIPS :
			App.Opciones.OcultarTooltipsMouse(MarcaOcultarTooltipsMouse.Marcado());
			if (MarcaOcultarTooltipsMouse.Marcado() == TRUE)		App.MostrarToolTipOpciones(L"Se ocultaran los tooltips al pasar por encima con el mouse.");
			else													App.MostrarToolTipOpciones(L"Los tooltips no se ocultaran al pasar por encima con el mouse.");
			break;
		case ID_MARCA_MOSTRARMEDIOTITULO :
			App.Opciones.MostrarMedioActualTitulo(MarcaMostrarNombreTitulo.Marcado());
			if (MarcaMostrarNombreTitulo.Marcado() == TRUE)			App.MostrarToolTipOpciones(L"El nombre del medio se mostrará en la barra de título.");
			else													App.MostrarToolTipOpciones(L"No se mostrará el nombre del medio en la barra de título.");
			App.MP.AsignarTitulo();
			break;
	}
}


void Marco_UI::Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BARRA_TIEMPOANIMACION:
			EtiquetaTiempoAnimacionesTA.Texto(DWL::Strings::ToStrF(BarraTiempoAnimaciones.Valor(), 0) + L" ms");
			App.Opciones.TiempoAnimaciones(static_cast<UINT>(BarraTiempoAnimaciones.Valor()));
			DhWnd::TiempoAnimaciones = static_cast<DWORD>(BarraTiempoAnimaciones.Valor());
			App.MostrarToolTipOpciones(L"La duración de las animaciones se ha ajustado a " + DWL::Strings::ToStrF(BarraTiempoAnimaciones.Valor(), 0) + L" milisegundos");
			break;
		case ID_BARRA_TIEMPOTOOLTIP:
			EtiquetaTiempoOcultarToolTipsTA.Texto(DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" ms");
			App.Opciones.TiempoToolTips(static_cast<UINT>(BarraTiempoToolTips.Valor()));
			App.MostrarToolTipOpciones(L"La duración de los tooltips se han ajustado a " + DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" milisegundos");
			break;
		case ID_BARRA_VIDEOTIEMPO:
			EtiquetaVideoTiempoTA.Texto(DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" ms");
			App.Opciones.OcultarMouseEnVideo(static_cast<int>(BarraVideoTiempo.Valor()));
			App.MostrarToolTipOpciones(L"Los controles del video se ocultaran tras " + DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" milisegundos de inactividad");
			break;
		case ID_BARRA_VIDEOOPACIDAD:
			EtiquetaVideoOpacidadTA.Texto(DWL::Strings::ToStrF(BarraVideoOpacidad.Valor(), 0) + L"/255");
			App.Opciones.OpacidadControlesVideo(static_cast<int>(BarraVideoOpacidad.Valor()));
			App.MostrarToolTipOpciones(L"La opacidad máxima de los controles del video se ha ajustado a " + DWL::Strings::ToStrF(BarraVideoOpacidad.Valor(), 0) + L" / 255");
			break;

	}
}


void Marco_UI::Evento_BarraEx_Cambiando(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BARRA_TIEMPOANIMACION:
			EtiquetaTiempoAnimacionesTA.Texto(DWL::Strings::ToStrF(BarraTiempoAnimaciones.Valor(), 0) + L" ms");
			break;
		case ID_BARRA_TIEMPOTOOLTIP:
			EtiquetaTiempoOcultarToolTipsTA.Texto(DWL::Strings::ToStrF(BarraTiempoToolTips.Valor(), 0) + L" ms");
			break;
		case ID_BARRA_VIDEOTIEMPO:
			EtiquetaVideoTiempoTA.Texto(DWL::Strings::ToStrF(BarraVideoTiempo.Valor(), 0) + L" ms");
			break;
		case ID_BARRA_VIDEOOPACIDAD:
			EtiquetaVideoOpacidadTA.Texto(DWL::Strings::ToStrF(BarraVideoOpacidad.Valor(), 0) + L"/255");
			break;

	}
}



LRESULT CALLBACK Marco_UI::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_MARCAEX_CLICK:
			Evento_MarcaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_DESPLEGABLEEX_CAMBIO:
			Evento_ListaDesplegable_Cambio(static_cast<INT_PTR>(wParam));
			return 0;
			// Barra de desplazamiento (barra de tiempo y volumen)
		case DWL_BARRAEX_CAMBIANDO:	// Se está modificando (mouse down)
			Evento_BarraEx_Cambiando(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
	}

	return DMarcoScrollEx::GestorMensajes(uMsg, wParam, lParam);
}