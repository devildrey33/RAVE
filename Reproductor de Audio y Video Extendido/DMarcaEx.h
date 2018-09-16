#ifndef DMARCAEX_H
#define DMARCAEX_H

#include "DControlEx.h"

namespace DWL {
	enum DMarcaEx_Estado {
		DMarcaEx_Estado_Normal,
		DMarcaEx_Estado_Resaltado,
		DMarcaEx_Estado_Presionado
	};

	class DMarcaEx : public DControlEx	{
	  public:
								DMarcaEx(void);
		                       ~DMarcaEx(void);

		HWND					CrearMarcaEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const int IDIconoMarca, const long Estilos = WS_CHILD | WS_VISIBLE);
		
		virtual void			Evento_MouseMovimiento(DEventoMouse &DatosMouse) { };
		virtual void			Evento_MousePresionado(DEventoMouse &DatosMouse) { };
		virtual void			Evento_MouseSoltado(DEventoMouse &DatosMouse) { };
		virtual void			Evento_MouseSaliendo(void) { };

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
		inline const BOOL       Marcado(void) { return _Marcado; }
		void                    Marcado(const BOOL nMarcar);
		void                    Pintar(HDC DC);
	  protected:
		void			       _Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam);
		void		           _Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam);
		void			       _Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam);
		void				   _Evento_MouseSaliendo(void);
		void                   _Evento_Pintar(void);
		std::wstring           _Texto;
		BOOL                   _Marcado;
		DMarcaEx_Estado        _Estado;
		DListaIconos_Icono    *_Icono;
	};
	
};

#endif