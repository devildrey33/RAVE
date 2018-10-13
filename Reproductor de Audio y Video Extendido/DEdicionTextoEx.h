#ifndef DEDICIONTEXTO_H
#define DEDICIONTEXTO_H

#include "DControlEx.h"
#include "DAnimacion.h"

namespace DWL {

	enum DEdicionTextoEx_Transicion {
		DEdicionTextoEx_Transicion_Normal,
		DEdicionTextoEx_Transicion_Resaltado,
		DEdicionTextoEx_Transicion_Presionado,
		DEdicionTextoEx_Transicion_Desactivado
	};

	enum DEdicionTextoEx_Alineacion {
		DEdicionTextoEx_Alineacion_Izquierda = DT_LEFT,
		DEdicionTextoEx_Alineacion_Centrado	 = DT_CENTER,
		DEdicionTextoEx_Alineacion_Derecha	 = DT_RIGHT
	};

	class DEdicionTextoEx : public DControlEx {
	  public:
									DEdicionTextoEx(void);
		                           ~DEdicionTextoEx(void);
		HWND						CrearEdicionTextoEx(DhWnd *nPadre, const TCHAR *nTxt, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, DListaIconos_Icono *nIcono = NULL, const long Estilos = WS_CHILD | WS_VISIBLE);
		virtual void				Activado(const BOOL nActivar);
		inline const BOOL			Activado(void) { return DhWnd::Activado(); }
		void						Pintar(HDC hDC, const int cX = 0, const int cY = 0);
		LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
		inline DListaIconos_Icono  *Icono(void) { return _Icono; }
		void                        Icono(DListaIconos_Icono *nIcono, const BOOL nRepintar = TRUE);
		inline const wchar_t       *Texto(void) { return _Texto.c_str(); };
		virtual void				Texto(std::wstring &nTexto, const BOOL nRepintar = TRUE);
		virtual void				Texto(const wchar_t * nTexto, const BOOL nRepintar = TRUE);

		void						Transicion(const DEdicionTextoEx_Transicion nTransicion);
		DhWnd_Fuente				Fuente;
		BOOL                        TextoEditable;
		DEdicionTextoEx_Alineacion	Alineacion;
	  protected:
		void					   _Evento_Pintar(void);
		void					   _Evento_MouseMovimiento(const WPARAM wParam, const LPARAM lParam);
		void					   _Evento_MousePresionado(const WPARAM wParam, const LPARAM lParam, const int Boton);
		void					   _Evento_MouseSoltado(const WPARAM wParam, const LPARAM lParam, const int Boton);
		void					   _Evento_MouseSaliendo(void);
		void                       _Evento_TeclaPresionada(WPARAM wParam, LPARAM lParam);
		void					   _Evento_TeclaSoltada(WPARAM wParam, LPARAM lParam);
		void					   _Evento_Tecla(WPARAM wParam, LPARAM lParam);
		void                       _Evento_FocoAsignado(void);
		void                       _Evento_FocoPerdido(void);
		void					   _Evento_Temporizador(const INT_PTR tID);
		DListaIconos_Icono        *_Icono;

		std::wstring               _Texto;

		COLORREF                   _ColorTexto;
		COLORREF                   _ColorFondo;
		COLORREF                   _ColorBorde;
		COLORREF                   _ColorCursor;

		size_t                     _PosCursor;
//		size_t                     _FinSeleccion;
		DAnimacionColor            _AniCursor;
		DAnimacionColor            _AniTransicion;		
		BOOL                       _Presionado;
	};



}
#endif