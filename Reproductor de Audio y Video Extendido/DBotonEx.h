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
							    // Funci�n que crea un BotonEx con texto
		HWND					CrearBotonEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);
								// Funci�n que crea un BotonEx con icono
		HWND					CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);
								// Funci�n que crea un BotonEx con icono y texto
		HWND					CrearBotonEx(DhWnd *nPadre, const int IDIcono, const int TamIcono, const int PosIconoX, const int PosIconoY, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos = WS_CHILD | WS_VISIBLE);

		virtual void			Activado(const BOOL nActivar);
		inline const BOOL       Activado(void) { return DhWnd::Activado(); }
		
		void					Pintar(HDC DC);

		virtual void			Evento_MouseMovimiento(DEventoMouse &DatosMouse)	{ };
		virtual void			Evento_MousePresionado(DEventoMouse &DatosMouse)	{ };
		virtual void			Evento_MouseSoltado(DEventoMouse &DatosMouse)		{ };
		virtual void			Evento_MouseClick(DEventoMouse &DatosMouse) { };

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

		DhWnd_Fuente            Fuente;

		inline const BOOL       Marcado(void) { return _Marcado; }
		void                    Marcado(const BOOL nMarcar);

								// Funci�n para asignar un icono para el boton
		void                    Icono(const int IDIcono, const int TamIcono, const int PosX = DBOTONEX_CENTRADO, const int PosY = DBOTONEX_CENTRADO);
		inline const INT_PTR    IDIcono(void) { return (_Icono != NULL) ? _Icono->ID() : 0; };

								// Funciones para asignar texto a un boton
		inline void             Texto(std::wstring &nTexto)  { _Texto = nTexto; Repintar(); };
		inline void             Texto(const wchar_t *nTexto) { _Texto = nTexto; Repintar(); };
	protected:
		void				   _Evento_MouseMovimiento(const WPARAM wParam, const LPARAM lParam);
		void				   _Evento_MousePresionado(const WPARAM wParam, const LPARAM lParam, const int Boton);
		void				   _Evento_MouseSoltado(const WPARAM wParam, const LPARAM lParam, const int Boton);

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