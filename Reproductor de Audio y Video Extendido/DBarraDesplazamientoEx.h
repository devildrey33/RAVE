#ifndef DBARRADESPLAZAMIENTOEX_H
#define DBARRADESPLAZAMIENTOEX_H

#include "DBarraProgresoEx.h"
#include "DToolTipEx.h"

namespace DWL {

	enum DBarraDesplazamientoEx_Estado {
		DBarraDesplazamientoEx_Estado_Normal,
		DBarraDesplazamientoEx_Estado_Resaltado,
		DBarraDesplazamientoEx_Estado_Presionado
	};

	enum DBarraDesplazamientoEx_ToolTip {
		DBarraDesplazamientoEx_ToolTip_SinToolTip,
		DBarraDesplazamientoEx_ToolTip_Superior,
		DBarraDesplazamientoEx_ToolTip_Inferior
	};



	class DBarraDesplazamientoEx : public DBarraProgresoEx	{
	  public:
												DBarraDesplazamientoEx();
		                                       ~DBarraDesplazamientoEx();
	   HWND										CrearBarraDesplazamientoEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo = 0.0f, const float nMaximo = 1.0f, const float nValor = 0.0);

	   inline DBarraDesplazamientoEx_Estado		Estado(void) { return _Estado; }
	   void										ToolTip(DBarraDesplazamientoEx_ToolTip nValor);
	   void										OcultarToolTip(void);

	   // virtuales
	   virtual void								Evento_PintarBorde(HDC DC, RECT &RBorde);
	   virtual void								Evento_PintarBarra(HDC DC, RECT &RBarra);
	   virtual void								Evento_PintarFondo(HDC DC, RECT &RFondo);
		
	   virtual void								Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam);
	   virtual void								Evento_MousePresionado(const int cX, const int cY, const UINT wParam);
	   virtual void								Evento_MouseSoltado(const int cX, const int cY, const UINT wParam);

	   virtual void								Evento_MostrarToolTip(float nValor, std::wstring &Texto);

	   inline void								ColorBarraResaltado(COLORREF nColor) { _ColorBarraResaltado = nColor;		Repintar(); }
	   inline const COLORREF					ColorBarraResaltado(void) { return _ColorBarraResaltado; }
	   inline void								ColorBarraPresionado(COLORREF nColor) { _ColorBarraPresionado = nColor;		Repintar(); }
	   inline const COLORREF					ColorBarraPresionado(void) { return _ColorBarraPresionado; }


	   LRESULT CALLBACK							GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
	   DBarraDesplazamientoEx_ToolTip		   _MostrarToolTip;
	   DToolTipEx							   _ToolTip;

	   COLORREF								   _ColorBarraResaltado;
	   COLORREF								   _ColorBarraPresionado;

	   DBarraDesplazamientoEx_Estado		   _Estado;

	};
}

#endif