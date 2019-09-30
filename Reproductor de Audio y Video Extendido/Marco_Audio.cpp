#include "stdafx.h"
#include "Marco_Audio.h"
#include <DStringUtils.h>

#define ID_ETIQUETA_FADEINOUT	1000
#define ID_ETIQUETA_FADEINOUT2	1001
#define ID_MARCA_FADEINOUT		1002
#define ID_BARRA_FADEINOUT		1003

void Marco_Audio::Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD);

	RECT RC;
	GetClientRect(_hWnd, &RC);


	const wchar_t* E1 = L"Puedes habilitar el efecto fade in/out entre canciones para mezclar el final de una canción con el inicio de la siguiente.\n\n"
						L"La canción que se está reproduciendo reducira su volumen a 0, y la canción siguiente ampliará su volumen de 0 al volumen actual.\n\n"
						L"NOTA : Este efecto no se usara en caso de que el medio actual o el medio siguiente sean un video.";

	EtiquetaFadeInOut.CrearEtiquetaEx(this, E1, 10, 0, RC.right - 20, 160, ID_ETIQUETA_FADEINOUT);

	MarcaFadeInOut.CrearMarcaEx(this, L"Activar efecto fade in/out entre canciones.", 10, 180, 305, 20, ID_MARCA_FADEINOUT, IDI_CHECK2);
	MarcaFadeInOut.Marcado(App.Opciones.EfectoFadeAudio());


	EtiquetaFadeInOut2.CrearEtiquetaEx(this, L"Duración del efecto fade in/out", 10, 210, 210, 20, ID_ETIQUETA_FADEINOUT2);

	BarraTiempoFadeInOut.CrearBarraDesplazamientoEx(this, 230, 210, RC.right - 240, 20, ID_BARRA_FADEINOUT, 500, 10000, static_cast<float>(App.Opciones.EfectoFadeAudioMS()));
	BarraTiempoFadeInOut.MostrarValor(DBarraEx_MostrarValor_ValorInt);
	BarraTiempoFadeInOut.EventoFormatearValor = [=](std::wstring& Str) {	Str += L" ms"; };

	BarraTiempoFadeInOut.Activado(MarcaFadeInOut.Marcado());


	CalcularPagina();
}


void Marco_Audio::Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_MARCA_FADEINOUT :
			BarraTiempoFadeInOut.Activado(MarcaFadeInOut.Marcado());
			App.Opciones.EfectoFadeAudio(MarcaFadeInOut.Marcado());
			if (MarcaFadeInOut.Marcado() == TRUE)	App.MostrarToolTipOpciones(L"Efecto fade in/out entre caciones activado.");
			else									App.MostrarToolTipOpciones(L"Efecto fade in/out entre caciones des-activado.");
			break;
	}
}



void Marco_Audio::Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BARRA_FADEINOUT :
			App.Opciones.EfectoFadeAudioMS(static_cast<UINT>(BarraTiempoFadeInOut.Valor()));			
			App.MostrarToolTipOpciones(L"La duración del fade in/out se ha ajustado a " + DWL::Strings::ToStrF(BarraTiempoFadeInOut.Valor(), 0) + L" milisegundos");
			break;
	}
}


LRESULT CALLBACK Marco_Audio::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case DWL_MARCAEX_CLICK:
			Evento_MarcaEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;
		case DWL_BARRAEX_CAMBIADO:  // Se ha modificado	(mouse up)
			Evento_BarraEx_Cambiado(WPARAM_TO_DEVENTOMOUSE(wParam));
			return 0;

	}

	return DMarcoScrollEx::GestorMensajes(uMsg, wParam, lParam);
}