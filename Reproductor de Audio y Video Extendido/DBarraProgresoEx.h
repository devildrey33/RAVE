#ifndef DBARRAPROGRESOEX_H
#define DBARRAPROGRESOEX_H

#include "DControlEx.h"

namespace DWL {
	class DBarraProgresoEx : public DControlEx_FondoDinamico, public DControlEx_BordeDinamico {
	  public:
								DBarraProgresoEx();
		                       ~DBarraProgresoEx();
		HWND					CrearBarraProgresoEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo = 0.0f, const float nMaximo = 1.0f, const float nValor = 0.0);

		void					Minimo(const float nMinimo);
		inline const float		Minimo(void) { return _Minimo; }
		void					Maximo(const float nMaximo);
		inline const float		Maximo(void) { return _Maximo; }
		void					Valor(const float nValor);
		inline const float		Valor(void) { return _Valor; }

		void					PintarBarraEx(HDC DC);

		// virtuales
		virtual void			Evento_PintarBorde(HDC DC, RECT &RBorde);
		virtual void			Evento_PintarBarra(HDC DC, RECT &RBarra);
		virtual void			Evento_PintarFondo(HDC DC, RECT &RFondo);

		inline void				ColorBarra(COLORREF nColor) { _ColorBarra = nColor;		Repintar(); }
		inline const COLORREF	ColorBarra(void)            { return _ColorBarra; }

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		float				   _Minimo;
		float				   _Maximo;
		float				   _Valor;
		COLORREF			   _ColorBarra;
	};
}
#endif