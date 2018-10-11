#ifndef DEDICIONTEXTO_H
#define DEDICIONTEXTO_H

#include "DControlEx.h"
namespace DWL {
	/* Clase que contiene las funciones necesarias para la edición de texto 
		Esta clase se ha diseñado para funcionar independientemente de la ventana / control que la contenga */
	class DEdicionTextoEx_Nucleo {
	  public:
								DEdicionTextoEx_Nucleo(void);
		                       ~DEdicionTextoEx_Nucleo(void);

								// Inicia los valores necesarios para la edición de texto
		void				    IniciarEdicionTextoEx(RECT &Espacio);
								// El HDC especificado se supone que es un back buffer
		void                    EdicionTexto_Pintar(HDC hDC);

								// Eventos que hay que enlazar (devuelven TRUE si hay que repintar)
		void                    EdicionTexto_Evento_MouseMovimiento(DEventoMouse &DatosMouse);
		void                    EdicionTexto_Evento_MousePresionado(DEventoMouse &DatosMouse);
		void                    EdicionTexto_Evento_MouseSoltado(DEventoMouse &DatosMouse);

		const BOOL              EdicionTexto_Evento_TeclaPresionada(DEventoTeclado &DatosTeclado);
		const BOOL              EdicionTexto_Evento_TeclaSoltada(DEventoTeclado &DatosTeclado);
		const BOOL				EdicionTexto_Evento_Tecla(DEventoTeclado &DatosTeclado);

		void				    EdicionTexto_Evento_FocoObtenido(HWND hWndUltimoFoco);
		void					EdicionTexto_Evento_FocoPerdido(HWND hWndNuevoFoco);

		inline const wchar_t   *Texto(void) { return _Texto.c_str(); };
		virtual void            Texto(std::wstring &nTexto);
		virtual void            Texto(const wchar_t * nTexto);
		BOOL                    TextoEditable;
		
		COLORREF                ColorCursor;
		COLORREF                ColorTexto;
		COLORREF                ColorTextoSombra;
		COLORREF                ColorTextoSeleccionado;
		COLORREF                ColorTextoSombraSeleccionado;
		COLORREF                ColorFondo;
		COLORREF                ColorFondoSeleccionado;

		DhWnd_Fuente			Fuente;
	  protected:				
		std::wstring           _Texto;
		RECT                   _EspacioEdicion; // Espacio que ocupa el control
		size_t                 _PosCursor;
		size_t                 _FinSeleccion;
	};



	class DEdicionTextoEx : public DControlEx, public DEdicionTextoEx_Nucleo {
	  public:
									DEdicionTextoEx(void);
		                           ~DEdicionTextoEx(void);
		HWND						CrearEdicionTextoEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, DListaIconos_Icono *nIcono = NULL, const long Estilos = WS_CHILD | WS_VISIBLE);
		void						Pintar(HDC hDC);
		LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
		inline DListaIconos_Icono  *Icono(void) { return _Icono; }
		void                        Icono(DListaIconos_Icono *nIcono, const BOOL nRepintar = TRUE);
		inline const wchar_t       *Texto(void) { return _Texto.c_str(); };
		virtual void				Texto(std::wstring &nTexto, const BOOL nRepintar = TRUE);
		virtual void				Texto(const wchar_t * nTexto, const BOOL nRepintar = TRUE);
	  protected:
		void					   _Evento_Pintar(void);
		void                       _Evento_Size(void);
		void                       _Evento_TeclaPresionada(WPARAM wParam, LPARAM lParam);
		void					   _Evento_TeclaSoltada(WPARAM wParam, LPARAM lParam);
		void					   _Evento_Tecla(WPARAM wParam, LPARAM lParam);

		DListaIconos_Icono        *_Icono;
	};



}
#endif