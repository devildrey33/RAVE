#ifndef DHWND_H
	#define DHWND_H


	//! Macro para obtener la coordenada X de un LPARAM
	#define GET_X_LPARAM(lp)			((int)(short)LOWORD(lp))
	//! Macro para obtener la coordenada Y de un LPARAM 
	#define GET_Y_LPARAM(lp)			((int)(short)HIWORD(lp))

#include <string>
#include "Rave_Skin.h"
#include "DhWnd_Fuente.h"

namespace DWL {


	// Clase base para el HWND con algunas funciones
	// - Est� costruida de forma que solo se puede asignar el miembro _hWmd internamente.
	class DhWnd {
	  public:
								DhWnd(void) : _hWnd(0)			{ };
								DhWnd(HWND hWnd) : _hWnd(hWnd)	{ };
                               ~DhWnd(void)						{ Destruir(); };
		inline HWND				operator() (void)				{ return _hWnd; };
		inline const bool		operator== (HWND chWnd)			{ return (_hWnd == chWnd); };
		inline void				operator= (HWND nhWnd)          { _hWnd = nhWnd; };
		inline const BOOL		Activar(const BOOL nActivar)    { return EnableWindow(_hWnd, nActivar); };
		inline const BOOL		Activado(void)					{ return IsWindowEnabled(_hWnd); };
		inline const BOOL		Minimizar(void)					{ return ShowWindow(_hWnd, SW_MINIMIZE); };
		inline const BOOL		Minimizado(void)				{ return IsIconic(_hWnd); };
		const BOOL				Maximizada(void);
		inline const HWND		AsignarFoco(void)               { return SetFocus(_hWnd); };
		inline BOOL				Destruir(void)					{ if (_hWnd == NULL) { return TRUE; } BOOL R = DestroyWindow(_hWnd); _hWnd = NULL; return R; };
		inline LONG_PTR			ID(void)						{ return GetWindowLongPtr(_hWnd, GWL_ID); };
		inline HWND				Padre(void)						{ return GetParent(_hWnd); };
		inline const BOOL		Visible(const BOOL nMostrar)    { return ShowWindow(_hWnd, (nMostrar != TRUE) ? SW_HIDE : SW_SHOW); };
		inline const BOOL		Visible(void)                   { return IsWindowVisible(_hWnd); };

		ATOM					RegistrarClase(const TCHAR *nNombre, WNDPROC WindowProcedureInicial, UINT Estilos = 0, const int nIconoRecursos = 0 ,HBRUSH nColorFondo = NULL, HINSTANCE hInstance = NULL);
		HWND                   _hWnd;

		static DhWnd_Fuente    _Fuente;
		static DhWnd_Fuente    _FuenteB;

		static DhWnd_Fuente    _FuenteTest;

		//		static HFONT       _FuenteB;

		/*friend class DVentana;
		friend class DControl;
		friend class DControlEx;
		friend class DStaticControl;
		friend class DButton;
		friend class DTrackBar;
		friend class DListView;
		friend class DTreeView;*/
//		friend class DBarraEx;
	};




	class DVentana {
	  public:
									DVentana(void) { }
		                           ~DVentana(void) { }
									// Funci�n para crear una ventana
		HWND						CrearVentana(const TCHAR *nNombre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos = NULL, UINT nEstilosClase = NULL, HMENU nMenu = NULL, HBRUSH nColorFondo = NULL, const int nIconoRecursos = 32512);

		void						Titulo(std::wstring &nTitulo);

		virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd(), uMsg, wParam, lParam); };
		DhWnd						hWnd;
	  protected:
		static LRESULT CALLBACK    _GestorMensajes(HWND HandleVentana, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};




	/* Controles estandar del windows */
	class DControl  {
	  public:
									DControl(void) { }
		                           ~DControl(void) { }

		HWND						CrearControl(DhWnd &nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, HBRUSH nColorFondo = NULL);

		virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) { return _GestorMensajesOriginal(hWnd(), uMsg, wParam, lParam); };
	 	DhWnd						hWnd;
	  protected:
		void					   _ConectarControl(const UINT nID, DhWnd &nPadre);
		static LRESULT CALLBACK    _GestorMensajes(HWND HandleVentana, UINT uMsg, WPARAM wParam, LPARAM lParam);
									//! WindowProcedure Orignal del control est�ndar
		WNDPROC                    _GestorMensajesOriginal;

	};



}
#endif