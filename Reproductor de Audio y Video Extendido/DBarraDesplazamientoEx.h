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
	   HWND										CrearBarraDesplazamientoEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const float nMinimo = 0.0f, const float nMaximo = 1.0f, const float nValor = 0.0);

	   inline DBarraDesplazamientoEx_Estado		Estado(void) { return _Estado; }
	   void										ToolTip(DBarraDesplazamientoEx_ToolTip nValor);
	   void										OcultarToolTip(void);

	   // virtuales
	   virtual void								Evento_PintarBorde(HDC DC, RECT &RBorde);
	   virtual void								Evento_PintarBarra(HDC DC, RECT &RBarra);
	   virtual void								Evento_PintarFondo(HDC DC, RECT &RFondo);
		
	   virtual void								Evento_MouseMovimiento(DEventoMouse &DatosMouse) { };
	   virtual void								Evento_MousePresionado(DEventoMouse &DatosMouse) { };
	   virtual void								Evento_MouseSoltado(DEventoMouse &DatosMouse) { };

	   virtual void								Evento_MostrarToolTip(float nValor, std::wstring &Texto);

	   COLORREF								    ColorBarraResaltado;
	   COLORREF								    ColorBarraPresionado;

	   COLORREF									ColorBordeResaltado;
	   COLORREF									ColorBordePresionado;


	   LRESULT CALLBACK							GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		void								   _Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam);
		void								   _Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam);
		void								   _Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam);

	    DBarraDesplazamientoEx_ToolTip		   _MostrarToolTip;
	    DToolTipEx							   _ToolTip;


	    DBarraDesplazamientoEx_Estado		   _Estado;

	};
}

#endif