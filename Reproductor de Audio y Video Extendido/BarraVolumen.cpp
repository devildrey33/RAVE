#include "stdafx.h"
#include "BarraVolumen.h"


BarraVolumen::BarraVolumen() {
}


BarraVolumen::~BarraVolumen() {
}


void BarraVolumen::Evento_PintarBarra(HDC DC, RECT &RBarra) {
	int Base = 0;
	switch (_Estado) {
		case DBarraEx_Estado_Normal:		Base = 20;			break;
		case DBarraEx_Estado_Resaltado:	Base = 40;			break;
		case DBarraEx_Estado_Presionado:	Base = 0;			break;
	}
	int R = Base + static_cast<int>(_Valor);
	int G = (Base + 200) - static_cast<int>(_Valor);
/*	if (R < 0) R = 0;
	if (G < 0) G = 0;*/

	Debug_Escribir_Varg(L"BarraVolumen::Evento_PintarBarra R:%d G:%d\n", R, G);

	HBRUSH BrochaBarra = CreateSolidBrush(RGB(R,G,0));
	FillRect(DC, &RBarra, BrochaBarra);
	DeleteObject(BrochaBarra);

}