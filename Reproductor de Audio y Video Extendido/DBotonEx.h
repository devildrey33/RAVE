#ifndef DBOTONEX_H
#define DBOTONEX_H

#include "DhWnd.h"

namespace DWL {
	enum DBotonEx_Estado {
		DBotonEx_Estado_Normal,
		DBotonEx_Estado_Resaltado,
		DBotonEx_Estado_Marcado,
		DBotonEx_Estado_MarcadoResaltado,
		DBotonEx_Estado_Presionado
	};

	class DBotonEx : public DControlEx_TextoDinamico {
	  public:
								DBotonEx();
				               ~DBotonEx();
		HWND					CrearBotonEx(DhWnd &nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);

		void					PintarBotonEx(HDC DC);

		virtual void			Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MousePresionado(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MouseSoltado(const int cX, const int cY, const UINT wParam);

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	  protected:
		DBotonEx_Estado		   _Estado;
	};

}

#endif