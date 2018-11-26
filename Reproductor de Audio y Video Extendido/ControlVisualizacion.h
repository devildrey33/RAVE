#pragma once

#ifdef RAVE_VLC_DOBLE_MEDIO_FFT

#include "DControlEx.h"
#include <complex>
#include <iostream>
#include <valarray>


enum Visualizacion {
	Visualizacion_Oscyloscope,
	Visualizacion_Barras
};

typedef void (*CV_FuncionPintado)(HDC hDC, RECT *RC);
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

class ControlVisualizacion : public DWL::DControlEx {
  public:
								ControlVisualizacion(void);
	                           ~ControlVisualizacion(void);
	void                        Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID);
	inline const Visualizacion	ObtenerVisualizacion(void) { return _Visualizacion; }
	void                        AsignarVisualizacion(const Visualizacion nVisualizacion);
	void                        Pintar(HDC hDC);
	static void                 PintarBarras(HDC hDC, RECT *RC);
	static void                 PintarOscyloscope(HDC hDC, RECT *RC);
	LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

  protected:
	static void                _fft(CArray &x);
    Visualizacion	           _Visualizacion;
	CV_FuncionPintado          _FuncionPintado;
	void                       _BorrarBackBuffer(void);
	HDC                        _Buffer;
	HBITMAP                    _Bmp;
	HBITMAP                    _BmpViejo;
								// Buffer para pintar el fondo con alphablend
	HDC                        _BufferAB;
	HBITMAP                    _BmpAB;
	HBITMAP                    _BmpABViejo;
	static int                 _TopeBarra[128];
};

#endif