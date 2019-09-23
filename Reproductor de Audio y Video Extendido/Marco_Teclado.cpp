#include "stdafx.h"
#include "Marco_Teclado.h"

#define ID_ETIQUETAS_TECLADO            1020
#define ID_ASINGAR_TECLA                1040
#define ID_ETIQUETA_TECLASRAPIDAS		1060

void Marco_Teclado::Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID) {
	CrearMarcoScrollEx(nPadre, cX, cY, cAncho, cAlto, cID, WS_CHILD);

	RECT RC;
	GetClientRect(_hWnd, &RC);


	////////////////////////////////////////////////
	// Creo los controles dentro del marco Teclado
	///////////////////////////////////////////////
	EtiquetaTeclasRapidas.CrearEtiquetaEx(this,  L"Las teclas rápidas solo se pueden utilizar cuando el reproductor tiene el foco del teclado. "
												 L"Es muy recomendable configurar las teclas rápidas con una combinación de teclas, "
												 L"por ejemplo : \n Control + tecla, Alt + Shift + Tecla, Control + Alt + tecla, etc..", 10, 0, RC.right - 40, 80, ID_ETIQUETA_TECLASRAPIDAS);
	const wchar_t *Textos[9] = {
		L"Play / Pausa",
		L"Generar lista aleatória",
		L"Subir volumen",
		L"Bajar volumen",
		L"Reproducir medio siguiente",
		L"Reporducir medio anterior",
		L"Mostrar informacion del medio",
		L"Mostrar el medio en la BD",
		L"Mostrar el medio en la lista"
	};

	for (int i = 0; i < static_cast<int>(App.TeclasRapidas.size()); i++) {
		EtiquetasTeclas[i].CrearEtiquetaEx(this, Textos[i], 10, 100 + (i * 25), 200, 20, ID_ETIQUETAS_TECLADO + i);
//		EtiquetasTeclas[i].Fuente = Fuente18Normal;
		TeclasRapidas[i].Crear(this, 220, 100 + (i * 25), RC.right - 230, 20, ID_ASINGAR_TECLA, &App.TeclasRapidas[i]);
	}


	CalcularPagina();
}


/*
LRESULT CALLBACK Marco_Teclado::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	}

	return DMarcoScrollEx::GestorMensajes(uMsg, wParam, lParam);
}*/