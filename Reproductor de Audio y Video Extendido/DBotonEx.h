#ifndef DBOTONEX_H
#define DBOTONEX_H

#include "DControlEx.h"

namespace DWL {
	enum DBotonEx_Estado {
		DBotonEx_Estado_Normal,
		DBotonEx_Estado_Resaltado,
/*		DBotonEx_Estado_Marcado,
		DBotonEx_Estado_MarcadoResaltado,*/
		DBotonEx_Estado_Presionado
	};

	class DBotonEx : public DControlEx {
	  public:
								DBotonEx();
				               ~DBotonEx();
		HWND					CrearBotonEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);

		virtual const BOOL		Activar(const BOOL nActivar);

		void					PintarBotonEx(HDC DC);

		virtual void			Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MousePresionado(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MouseSoltado(const int cX, const int cY, const UINT wParam);

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

/*		COLORREF                ColorBorde;
		COLORREF                ColorBordeResaltado;
		COLORREF                ColorBordePresionado;
		COLORREF                ColorFondo;
		COLORREF                ColorFondoResaltado;
		COLORREF                ColorFondoPresionado;

		COLORREF                ColorTexto;
		COLORREF                ColorTextoResaltado;
		COLORREF                ColorTextoPresionado;
		COLORREF                ColorTextoDesactivado;

		COLORREF                ColorTextoSombra;*/

		DhWnd_Fuente            Fuente;

		inline const BOOL       Marcado(void) { return _Marcado; }
		void                    Marcado(const BOOL nMarcar);
	  protected:
		std::wstring           _Texto;
		DBotonEx_Estado		   _Estado;
		BOOL                   _Marcado;
	};

}

#endif