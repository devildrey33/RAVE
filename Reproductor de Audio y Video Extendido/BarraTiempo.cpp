#include "stdafx.h"
#include "BarraTiempo.h"
#include "RAVE.h"

BarraTiempo::BarraTiempo(void) : DBarraDesplazamientoEx(), _Valor2(0.0f), _ColorBarra2(COLOR_BARRA2) {
}

BarraTiempo::~BarraTiempo(void) {
}

void BarraTiempo::Evento_MostrarToolTip(float nValor, std::wstring &Texto) {
	double t = static_cast<double>(App.MP.TiempoTotalMs());
	App.MP.TiempoStr(static_cast<UINT64>(static_cast<double>(App.MP.TiempoTotalMs()) * nValor), Texto);
}


void BarraTiempo::Evento_PintarPersonalizado(HDC DC, RECT &RFondo) {
	RECT RBarra = _RBarra;
	CalcularBarra(0.0f, 1.0f, _Valor2, RFondo, RBarra);
	// La barra empezara desde el final de la otra barra
	RBarra.left = _RBarra.right;
	if (RBarra.right > RBarra.left) {
		HBRUSH BrochaBarra = CreateSolidBrush(_ColorBarra2);
		FillRect(DC, &RBarra, BrochaBarra);
		DeleteObject(BrochaBarra);
	}
}

void BarraTiempo::DesAnimar(void) {
	_AniBarra2.Terminar();

	_AniBarra2.Iniciar({ COLOR_BARRA2 }, { _ColorFondo }, 300,
		[=](DAnimacion::Valores& Datos, const BOOL Terminado) {
			_ColorBarra2 = Datos[0].Color();
			Repintar();
			if (Terminado == TRUE) _Valor2 = 0.0f;
		}
	);

}


void BarraTiempo::Valor2(const float nValor2) { 
	// Si en nuevo valor no es cero, y el actual es cero, inicio la animación
	if (_Valor2 == 0.0f && nValor2 != 0.0f) {
		_AniBarra2.Terminar();
		// Animación
		_AniBarra2.Iniciar({ _ColorBarra }, { COLOR_BARRA2 }, 300,
			[=](DAnimacion::Valores& Datos, const BOOL Terminado) {
				_ColorBarra2 = Datos[0].Color();
				Repintar();
			}
		);
	}

	// Si el nuevo valor es cero, y el actual no es cero, inicio la des-animación
/*	if (_Valor2 != 0.0f && nValor2 == 0.0f) {
		// Animación
		_AniBarra2.Iniciar({ COLOR_BARRA2 }, { _ColorFondo }, 300,
			[=](DAnimacion::Valores& Datos, const BOOL Terminado) {
				_ColorBarra2 = Datos[0].Color();
				Repintar();
			}
		);
	}*/

	_Valor2 = nValor2; 
}