#ifndef DBOTONEX_H
#define DBOTONEX_H

#include "DControlEx.h"

namespace DWL {
	enum DBotonEx_Estado {
		DBotonEx_Estado_Normal,
		DBotonEx_Estado_Resaltado,
		DBotonEx_Estado_Presionado
	};

	#define DBOTONEX_CENTRADO -1

	class DBotonEx : public DControlEx {
	  public:
								DBotonEx(void);
				               ~DBotonEx(void);
							    // Función que crea un BotonEx con texto
		HWND					CrearBotonEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);
								// Función que crea un BotonEx con icono
		HWND					CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);
								// Función que crea un BotonEx con icono y texto
		HWND					CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);

		virtual void			Activado(const BOOL nActivar);
		inline const BOOL       Activado(void) { return DhWnd::Activado(); }
		
		void					Pintar(HDC DC);

		virtual void			Evento_MouseMovimiento(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MousePresionado(const int cX, const int cY, const UINT wParam);
		virtual void			Evento_MouseSoltado(const int cX, const int cY, const UINT wParam);

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

		DhWnd_Fuente            Fuente;

		inline const BOOL       Marcado(void) { return _Marcado; }
		void                    Marcado(const BOOL nMarcar);

								// Función para asignar un icono para el boton
		void                    Icono(const int IDIcono, const int TamIcono, const int PosX = DBOTONEX_CENTRADO, const int PosY = DBOTONEX_CENTRADO);

								// Funciones para asignar texto a un boton
		inline void             Texto(std::wstring &nTexto)  { _Texto = nTexto; Repintar(); };
		inline void             Texto(const wchar_t *nTexto) { _Texto = nTexto; Repintar(); };
	protected:
		void                   _Evento_Pintar(void);
		HWND				   _CrearBotonEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos);
		std::wstring           _Texto;
		DBotonEx_Estado		   _Estado;
		BOOL                   _Marcado;
		DListaIconos_Icono    *_Icono;
		int                    _PosIconoX;
		int                    _PosIconoY;
	};

}

#endif