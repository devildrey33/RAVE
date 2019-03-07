#include "stdafx.h"
#include "BarraTiempo.h"
#include "RAVE.h"

BarraTiempo::BarraTiempo(void) : DBarraDesplazamientoEx() {
}

BarraTiempo::~BarraTiempo(void) {
}

void BarraTiempo::Evento_MostrarToolTip(float nValor, std::wstring &Texto) {
	double t = static_cast<double>(App.MP.TiempoTotalMs());
	App.MP.TiempoStr(static_cast<UINT64>(static_cast<double>(App.MP.TiempoTotalMs()) * nValor), Texto);
}