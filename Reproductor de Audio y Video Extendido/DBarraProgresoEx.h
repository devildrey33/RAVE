#ifndef DBARRAPROGRESOEX_H
#define DBARRAPROGRESOEX_H

#include "DControlEx.h"
#include "DAnimacion.h"

namespace DWL {

	enum DBarraEx_Estado {
		DBarraEx_Estado_Normal,
		DBarraEx_Estado_Resaltado,
		DBarraEx_Estado_Presionado
	};


	class DBarraProgresoEx : public DControlEx {
	  public:
									DBarraProgresoEx();
							       ~DBarraProgresoEx();
		HWND						CrearBarraProgresoEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo = 0.0f, const float nMaximo = 1.0f, const float nValor = 0.0);

		void						Minimo(const float nMinimo);
		inline const float			Minimo(void)					{ return _Minimo; }
		void						Maximo(const float nMaximo);
		inline const float			Maximo(void)					{ return _Maximo; }
		virtual void				Valor(const float nValor);
		virtual inline const float	Valor(void)						{ return _Valor; }

		void						PintarBarraEx(HDC DC);

		// virtuales
		virtual void				Evento_PintarBorde(HDC DC, RECT &RBorde);
		virtual void				Evento_PintarBarra(HDC DC, RECT &RBarra);
		virtual void				Evento_PintarFondo(HDC DC, RECT &RFondo);

		virtual void                Resaltar(const BOOL Resaltado);

		LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		void					   _Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam);
		void                       _Evento_MouseSaliendo(void);

		COLORREF				   _ColorBarra;
		COLORREF		           _ColorBorde;
		COLORREF		           _ColorFondo;
		float					   _Minimo;
		float					   _Maximo;
		float					   _Valor;
		DBarraEx_Estado			   _Estado;
		DAnimacionColor            _AniResaltado;

	};	
}
#endif