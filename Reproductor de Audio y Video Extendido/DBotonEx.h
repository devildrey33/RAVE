#ifndef DBOTONEX_H
#define DBOTONEX_H

#include "DControlEx.h"
#include "DAnimacion.h"

namespace DWL {
	enum DBotonEx_Estado {
		DBotonEx_Estado_Normal,
		DBotonEx_Estado_Resaltado,
		DBotonEx_Estado_Presionado
	};

	enum DBotonEx_Transicion {
		DBotonEx_Transicion_Normal,
		DBotonEx_Transicion_Resaltado,
		DBotonEx_Transicion_Presionado,
		DBotonEx_Transicion_Marcado,
		DBotonEx_Transicion_Desactivado
	};

	#define DBOTONEX_MOSTRARDEBUG	FALSE
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
		
		void					Pintar(HDC DC, const int nX = 0, const int nY = 0);

		virtual void			Evento_MouseMovimiento(DEventoMouse &DatosMouse)	{ };
		virtual void			Evento_MousePresionado(DEventoMouse &DatosMouse)	{ };
		virtual void			Evento_MouseSoltado(DEventoMouse &DatosMouse)		{ };
		virtual void			Evento_MouseClick(DEventoMouse &DatosMouse)			{ };
		virtual void            Evento_Pintar(HDC DC)								{ };

		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

		DhWnd_Fuente            Fuente;

		inline const BOOL       Marcado(void) { return _Marcado; }
		void                    Marcado(const BOOL nMarcar);

								// Función para asignar un icono para el boton
		void                    Icono(const int IDIcono, const int TamIcono, const int PosX = DBOTONEX_CENTRADO, const int PosY = DBOTONEX_CENTRADO);
		inline const INT_PTR    IDIcono(void) { return (_Icono != NULL) ? _Icono->ID() : 0; };

								// Funciones para asignar texto a un boton
		inline void             Texto(std::wstring &nTexto)  { _Texto = nTexto; Repintar(); };
		inline void             Texto(const wchar_t *nTexto) { _Texto = nTexto; Repintar(); };
								// Función que realiza una transición del estado actual al estado especificado
		void                    Transicion(const DBotonEx_Transicion nTransicion);
	  protected:
		void				   _Evento_MouseMovimiento(const WPARAM wParam, const LPARAM lParam);
		void				   _Evento_MousePresionado(const WPARAM wParam, const LPARAM lParam, const int Boton);
		void				   _Evento_MouseSoltado(const WPARAM wParam, const LPARAM lParam, const int Boton);
		void                   _Evento_MouseSaliendo(void);

		void                   _Evento_Pintar(void);
		HWND				   _CrearBotonEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const long Estilos);
		std::wstring           _Texto;
		DBotonEx_Estado		   _Estado;
		BOOL                   _Marcado;
		DListaIconos_Icono    *_Icono;
		int                    _PosIconoX;
		int                    _PosIconoY;
		DAnimacionColor        _AniTransicion;
		COLORREF               _ColorBorde;
		COLORREF               _ColorFondo;
		COLORREF               _ColorTexto;
	};

}

#endif