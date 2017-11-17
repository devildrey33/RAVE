#include "stdafx.h"
#include "BarraTiempo.h"
#include "RAVE.h"

BarraTiempo::BarraTiempo() : DBarraDesplazamientoEx() {
}

BarraTiempo::~BarraTiempo() {
}

void BarraTiempo::Evento_MostrarToolTip(float nValor, std::wstring &Texto) {
	float t = static_cast<float>(App.VLC.TiempoTotalMs());
	App.VLC.TiempoStr(static_cast<UINT64>(static_cast<float>(App.VLC.TiempoTotalMs()) * nValor), Texto);
}