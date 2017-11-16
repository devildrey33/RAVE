#ifndef DBARRAEX_H
#define DBARRAEX_H


#include "DToolTipEx.h"


namespace DWL {

	enum DBarraEx_Estado {
		DBarraEx_Estado_Normal,
		DBarraEx_Estado_Resaltado,
		DBarraEx_Estado_Presionado
	};

	enum DBarraEx_ToolTip {
		DBarraEx_ToolTip_SinToolTip,
		DBarraEx_ToolTip_Superior,
		DBarraEx_ToolTip_Inferior
	};

	class DBarraEx : public DControlEx {
	 public :
								DBarraEx();
		                       ~DBarraEx();
		HWND					CrearBarraEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo = 0.0f, const float nMaximo = 1.0f, const float nValor = 0.0);

		void					Minimo(const float nMinimo);
		inline const float		Minimo(void) { return _Minimo; }
		void					Maximo(const float nMaximo);
		inline const float		Maximo(void) { return _Maximo; }
		void					Valor(const float nValor);
		inline const float		Valor(void) { return _Valor; }

		inline DBarraEx_Estado	Estado(void) { return _Estado;  }

		void					PintarBarraEx(HDC DC);

		void					ToolTip(DBarraEx_ToolTip nValor);
		void					OcultarToolTip(void);

		// virtuales
		virtual void			Evento_PintarBorde(HDC DC, RECT &RBorde);
		virtual void			Evento_PintarBarra(HDC DC, RECT &RBarra);
		virtual void			Evento_PintarFondo(HDC DC, RECT &RFondo);

		virtual void			Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MousePresionado(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MouseSoltado(const int cX, const int cY, const UINT wParam);

		virtual void			Evento_MostrarToolTip(float nValor, std::wstring &Texto);

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected :
		float				   _Minimo;
		float				   _Maximo;
		float				   _Valor;

		DBarraEx_ToolTip	   _MostrarToolTip;
		DToolTipEx			   _ToolTip;

		DBarraEx_Estado		   _Estado;
	};
}
#endif