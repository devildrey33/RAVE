#include "stdafx.h"
#include "Marco_Video.h"
#include <DStringUtils.h>

// Video
#define ID_ETIQUETA_VIDEOTIEMPO			1076
#define ID_BARRA_VIDEOTIEMPO			1077
#define ID_ETIQUETA_VIDEOOPACIDAD		1078
#define ID_BARRA_VIDEOOPACIDAD          1079
#define ID_ETIQUETA_VIDEOPOSICION		1080
#define ID_DESPLEGABLE_VIDEOPOSICION    1081
#define ID_ETIQUETA_VIDEOTIEMPOTA       1082
#define ID_ETIQUETA_VIDEOOPACIDADTA		1083
#define ID_MARCA_VIDEOGUARDARVALORES    1084



void Marco_Video::Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD);

	RECT RC;
	GetClientRect(_hWnd, &RC);

	std::wstring TmpStr;

	MarcaRecordarValoresVideo.CrearMarcaEx(this, L"Guardar brillo, saturación, contraste, y proporción para cada video.", 10, 10, 460, 30, ID_MARCA_VIDEOGUARDARVALORES, IDI_CHECK2);
	MarcaRecordarValoresVideo.Marcado(App.Opciones.GuardarBSCP());


	CalcularPagina();
}



void Marco_Video::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_VIDEOGUARDARVALORES:
			App.Opciones.GuardarBSCP(MarcaRecordarValoresVideo.Marcado());
			if (MarcaRecordarValoresVideo.Marcado() == TRUE)		App.MostrarToolTipOpciones(L"Se recordaran los valores de brillo, contraste, saturación, y proporción para cada video.");
			else													App.MostrarToolTipOpciones(L"Los valores de brillo, contraste, saturación, y proporción serán generales para todos los videos.");
			break;
	}
}





LRESULT CALLBACK Marco_Video::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_MARCAEX_CLICK:
			Evento_MarcaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
	}

	return DMarcoScrollEx::GestorMensajes(uMsg, wParam, lParam);
}