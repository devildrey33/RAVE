#include "stdafx.h"
#include "ControlVisualizacion.h"
#include "resource.h"


#ifdef RAVE_VLC_DOBLE_MEDIO_FFT

#define ID_TIMER_REPINTAR 1000

int ControlVisualizacion::_TopeBarra[128];
//const double PI = 3.141592653589793238460;


ControlVisualizacion::ControlVisualizacion(void) : _Buffer(NULL), _Bmp(NULL), _BmpViejo(NULL), _BufferAB(NULL), _BmpAB(NULL), _BmpABViejo(NULL) {
	_FuncionPintado = ControlVisualizacion::PintarBarras;
	_Visualizacion = Visualizacion_Barras;
//	_FuncionPintado = ControlVisualizacion::PintarOscyloscope;
//	_Visualizacion = Visualizacion_Oscyloscope;
	for (int i = 0; i < 128; i++) _TopeBarra[i] = 0;
}


ControlVisualizacion::~ControlVisualizacion(void) {
	_BorrarBackBuffer();
}

void ControlVisualizacion::Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID) {
	DControlEx::CrearControlEx(nPadre, L"ControlVisualizacion", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
	RECT RC;
	GetClientRect(_hWnd, &RC);	

	// Creo el buffer para el pintado
	HDC DC = GetDC(_hWnd);
	_Buffer		= CreateCompatibleDC(NULL);
	_Bmp		= CreateCompatibleBitmap(DC, RC.right, RC.bottom);
	_BmpViejo	= static_cast<HBITMAP>(SelectObject(_Buffer, _Bmp));
	// Creo el buffer para borrar el fondo con alpha blend
	_BufferAB	= CreateCompatibleDC(NULL);
	_BmpAB		= CreateCompatibleBitmap(DC, RC.right, RC.bottom);
	_BmpABViejo	= static_cast<HBITMAP>(SelectObject(_BufferAB, _BmpAB));
	// Pinto el fondo en los dos buffers
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(_Buffer, &RC, BrochaFondo);
	FillRect(_BufferAB, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
	ReleaseDC(_hWnd, DC);

	SetTimer(_hWnd, ID_TIMER_REPINTAR, 16, NULL);
}

void ControlVisualizacion::AsignarVisualizacion(const Visualizacion nVisualizacion) {
	_Visualizacion = nVisualizacion;
	switch (nVisualizacion) {
		case Visualizacion_Barras :			
			//App.MP.TamVisualizacion = 32;
			_FuncionPintado = ControlVisualizacion::PintarBarras;
			break;
		case Visualizacion_Oscyloscope :
			//App.MP.TamVisualizacion = 128;
			_FuncionPintado = ControlVisualizacion::PintarOscyloscope;
			break;
	}
}

void ControlVisualizacion::Pintar(HDC hDC) {
	// Hay que pintar en el backbuffer _Buffer, y luego hacer un BitBlt de _Buffer a hDC
	// Para borrar el fondo, hay que hacer un alphablend a 0.33 de opacidad
	RECT RC;
	GetClientRect(_hWnd, &RC);
//	BLENDFUNCTION BF = { AC_SRC_OVER, 0, 84, 0 }; // 84 esun poco menos de un tercio de 255
//	AlphaBlend(_Buffer, 0, 0, RC.right, RC.bottom, _BufferAB, 0, 0, RC.right, RC.bottom, BF);

	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(_Buffer, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);

	// Utilizo la función de pintado (oscy / barras)
	_FuncionPintado(_Buffer, &RC);

	BitBlt(hDC, 0, 0, RC.right, RC.bottom, _Buffer, 0, 0, SRCCOPY);
}


void ControlVisualizacion::PintarBarras(HDC hDC, RECT *RC) {
	App.MP.ObtenerTTF();
	float(&Oscy)[2048] = App.MP.TTF;
	Complex test[32];
	for (int n = 0; n < 32; n++) {
		test[n] = Oscy[n];
	}
	CArray data(test, 32);
	_fft(data);

	HBRUSH BrochaBarra = CreateSolidBrush(COLOR_ROJO);
	HPEN   PlumaTope   = CreatePen(PS_SOLID, 1, COLOR_FONDO_CLARO_RESALTADO);
	HPEN   VPluma      = static_cast<HPEN>(SelectObject(hDC, PlumaTope));
	RECT RB;
	for (int i = 0; i < 32; i++) {
		long Valor = static_cast<long>(abs(Oscy[i] * static_cast<float>(RC->bottom - 3)));
		//long Valor = static_cast<long>(abs(data[i].imag())	* static_cast<float>(RC->bottom - 3));
		// Pinto la barra
		RB.left		= i * 3; RB.right = (i * 3) + 2;
		RB.bottom	= RC->bottom;
		RB.top		= RC->bottom - Valor;
		FillRect(hDC, &RB, BrochaBarra);
		// Calculo el tope para la barra
		if (Valor > _TopeBarra[i]) _TopeBarra[i] = Valor + 2;
		else {
			if (_TopeBarra[i] > 2) _TopeBarra[i] --;
		}
		// Pinto el tope para la barra
		MoveToEx(hDC, i * 3, RC->bottom - _TopeBarra[i], NULL);
		LineTo(hDC, (i * 3) + 2, RC->bottom - _TopeBarra[i]);
	}	
	DeleteObject(BrochaBarra);
	SelectObject(hDC, VPluma);
	DeleteObject(PlumaTope);
}


void ControlVisualizacion::PintarOscyloscope(HDC hDC, RECT *RC) {
	App.MP.ObtenerTTF();
	float(&Oscy)[2048] = App.MP.TTF;

	//BLENDFUNCTION BF = { AC_SRC_OVER, 0, 84, 0 }; // 84 esun poco menos de un tercio de 255
	//AlphaBlend(_Buffer, 0, 0, RC.right, RC.bottom, _BufferAB, 0, 0, RC.right, RC.bottom, BF);

	HPEN Pluma = CreatePen(PS_SOLID, 1, COLOR_ROJO);
	HPEN VPluma = static_cast<HPEN>(SelectObject(hDC, Pluma));
	int MitadV = RC->bottom / 2;
	MoveToEx(hDC, 0, MitadV, NULL);

	for (int i = 0; i < 128; i++) {
		LineTo(hDC, i, MitadV + static_cast<long>(static_cast<float>(MitadV) * Oscy[i]));
	}
	SelectObject(hDC, VPluma);
	DeleteObject(Pluma);
}


void ControlVisualizacion::_BorrarBackBuffer(void) {
	// Borro el backbuffer
	if (_Buffer != NULL) {
		SelectObject(_Buffer, _BmpViejo);
		DeleteObject(_Bmp);
		DeleteDC(_Buffer);
		SelectObject(_BufferAB, _BmpABViejo);
		DeleteObject(_BmpAB);
		DeleteDC(_BufferAB);
		_Buffer = NULL;
	}
}


// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
// !!! Warning : in some cases this code make result different from not optimased version above (need to fix bug)
// The bug is now fixed @2017/05/30 
void ControlVisualizacion::_fft(CArray &x) {
	// DFT
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
	while (k > 1) {
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++) {
			for (unsigned int a = l; a < N; a += n) {
				unsigned int b = a + k;
				Complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++) {
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a) {
			Complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
	//// Normalize (This section make it not working correctly)
	//Complex f = 1.0 / sqrt(N);
	//for (unsigned int i = 0; i < N; i++)
	//	x[i] *= f;
}

LRESULT CALLBACK ControlVisualizacion::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {		
		case WM_PAINT:
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(hWnd(), &PS);
			Pintar(DC);
			EndPaint(hWnd(), &PS);
			return 0;
		case WM_TIMER :
			//Repintar();
			HDC DC2;
			DC2 = GetDC(_hWnd);
			Pintar(DC2);
			ReleaseDC(_hWnd, DC2);
			return 0;
		case WM_LBUTTONUP :
			if (_Visualizacion == Visualizacion_Barras) AsignarVisualizacion(Visualizacion_Oscyloscope);
			else										AsignarVisualizacion(Visualizacion_Barras);
			break;
	}
	return DControlEx::GestorMensajes(uMsg, wParam, lParam);
}

#endif