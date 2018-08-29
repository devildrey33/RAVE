#ifndef DBOTONEX_H
#define DBOTONEX_H

#include "DControlEx.h"

namespace DWL {
	enum DBotonEx_Estado {
		DBotonEx_Estado_Normal,
		DBotonEx_Estado_Resaltado,
		DBotonEx_Estado_Presionado
	};

	class DBotonEx : public DControlEx {
	  public:
								DBotonEx(void);
				               ~DBotonEx(void);
		HWND					CrearBotonEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);

		virtual void			Activado(const BOOL nActivar);
		inline const BOOL       Activado(void) { return DhWnd::Activado(); }
		
		void					PintarBotonEx(HDC DC);

		virtual void			Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MousePresionado(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MouseSoltado(const int cX, const int cY, const UINT wParam);

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

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