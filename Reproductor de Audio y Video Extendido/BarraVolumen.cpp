#include "stdafx.h"
#include "BarraVolumen.h"


BarraVolumen::BarraVolumen() : DBarraDesplazamientoEx(){
}


BarraVolumen::~BarraVolumen() {
}

void BarraVolumen::CrearBarraVolumen(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo, const float nMaximo, const float nValor) {
	CrearBarraDesplazamientoEx(nPadre, cX, cY, cAncho, cAlto, cID, nMinimo, nMaximo, nValor);
	_ColorBarra = RGB(20 + static_cast<int>(nValor), 220 - static_cast<int>(nValor), 0);
}


void BarraVolumen::Valor(const float nValor) {
	_Valor = nValor;
	if (_Estado == DBarraEx_Estado_Presionado) _AniTransicion.Terminar();
	_ColorBarra = RGB(20 + static_cast<int>(_Valor), 220 - static_cast<int>(_Valor), 0);
	Repintar();
}


void BarraVolumen::Transicion(const DBarraEx_Transicion nTransicion) {
	DWORD Duracion = DhWnd::TiempoAnimaciones;
	if (_AniTransicion.Animando() == TRUE) {
		Duracion = _AniTransicion.TiempoActual();
		_AniTransicion.Terminar();
	}

	COLORREF FondoHasta, BordeHasta, BarraHasta;
	switch (nTransicion) {
		case DBarraEx_Transicion_Normal:
			FondoHasta = COLOR_BARRA_FONDO;
			BarraHasta = RGB(20 + static_cast<int>(_Valor), 220 - static_cast<int>(_Valor), 0);;
			BordeHasta = COLOR_BORDE;
			break;
		case DBarraEx_Transicion_Resaltado:
			FondoHasta = COLOR_BARRA_FONDO_RESALTADO;
			BarraHasta = RGB(40 + static_cast<int>(_Valor), 240 - static_cast<int>(_Valor), 0);
			BordeHasta = COLOR_BORDE_RESALTADO;
			break;
		case DBarraEx_Transicion_Presionado:
			FondoHasta = COLOR_BARRA_FONDO_PRESIONADO;
			BarraHasta = RGB(static_cast<int>(_Valor), 200 - static_cast<int>(_Valor), 0);
			BordeHasta = COLOR_BORDE_PRESIONADO;
			break;
		case DBarraEx_Transicion_Desactivado:
			FondoHasta = COLOR_BARRA_FONDO_DESACTIVADO;
			BarraHasta = RGB(static_cast<int>(_Valor), 200 - static_cast<int>(_Valor), 0);
			BordeHasta = COLOR_BORDE;
			break;
	}

	_AniTransicion.Iniciar(_ColorFondo, FondoHasta, _ColorBorde, BordeHasta, _ColorBarra, BarraHasta, Duracion, [=](DAnimacion::Valores &Datos, const BOOL Terminado) {
		_ColorFondo = Datos[0].Color();
		_ColorBorde = Datos[1].Color();
		_ColorBarra = Datos[2].Color();
		Repintar();
	});

}


/*
void BarraVolumen::Resaltar(const BOOL Resaltado) {
	if (_AniResaltado.Animando() == TRUE) {
		_AniResaltado.Invertir();
		return;
	}

	COLORREF ColorBase			= RGB(20 + static_cast<int>(_Valor), 220 - static_cast<int>(_Valor), 0);
	COLORREF ColorBaseResaltado = RGB(40 + static_cast<int>(_Valor), 240 - static_cast<int>(_Valor), 0);

	COLORREF BordeDesde, BordeHasta, FondoDesde, FondoHasta, BarraDesde, BarraHasta;
	if (Resaltado == TRUE) {
		BordeDesde = COLOR_BORDE;
		BordeHasta = COLOR_BORDE_RESALTADO;
		FondoDesde = COLOR_BARRA_FONDO;
		FondoHasta = COLOR_BARRA_FONDO_RESALTADO;
		BarraDesde = ColorBase;
		BarraHasta = ColorBaseResaltado;
		_Estado = DBarraEx_Estado_Resaltado;
	}
	else {
		BordeDesde = COLOR_BORDE_RESALTADO;
		BordeHasta = COLOR_BORDE;
		FondoDesde = COLOR_BARRA_FONDO_RESALTADO;
		FondoHasta = COLOR_BARRA_FONDO;
		BarraDesde = ColorBaseResaltado;
		BarraHasta = ColorBase;
		_Estado = DBarraEx_Estado_Normal;
	}
	_AniResaltado.Iniciar(FondoDesde, FondoHasta, BordeDesde, BordeHasta, BarraDesde, BarraHasta, 400, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
		_ColorFondo = Valores[0];
		_ColorBorde = Valores[1];
		_ColorBarra = Valores[2];
		Repintar();
	});

}
*/