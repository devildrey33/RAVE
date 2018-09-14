#ifndef DHWND_H
	#define DHWND_H


	//! Macro para obtener la coordenada X de un LPARAM
	#define GET_X_LPARAM(lp)			((int)(short)LOWORD(lp))
	//! Macro para obtener la coordenada Y de un LPARAM 
	#define GET_Y_LPARAM(lp)			((int)(short)HIWORD(lp))

#include <string>
#include "Rave_Skin.h"
#include "DhWnd_Fuente.h"
#include "DBarraTareas.h"
#include "DEventoMouse.h"

namespace DWL {


	// Clase base para el HWND con algunas funciones
	// - Est� costruida de forma que solo se puede asignar el miembro _hWmd internamente.
	class DhWnd {
	  public:
									DhWnd(void) : _hWnd(0)			{ IniciarMiembrosEstaticos(); };
									DhWnd(HWND hWnd) : _hWnd(hWnd)	{ };
                                   ~DhWnd(void)						{ Destruir(); };
		inline HWND					hWnd(void)						{ return _hWnd; };
/*		inline HWND				operator() (void)				{ return _hWnd; };
		inline const bool		operator== (HWND chWnd)			{ return (_hWnd == chWnd); };
		inline const bool		operator== (DhWnd *chWnd)		{ return (_hWnd == chWnd->hWnd()); };
		inline void				operator= (HWND nhWnd)          { _hWnd = nhWnd; };*/
		inline virtual void			Activado(const BOOL nActivar)   { EnableWindow(_hWnd, nActivar); };
		inline virtual const BOOL	Activado(void)					{ return IsWindowEnabled(_hWnd); };
		inline const BOOL			Minimizar(void)					{ return ShowWindow(_hWnd, SW_MINIMIZE); };
		inline const BOOL			Minimizado(void)				{ return IsIconic(_hWnd); };
		const BOOL					Maximizada(void);
		inline const HWND			AsignarFoco(void)               { return SetFocus(_hWnd); };
		inline BOOL					Destruir(void)					{ if (_hWnd == NULL) { return TRUE; } BOOL R = DestroyWindow(_hWnd); _hWnd = NULL; return R; };
		inline UINT					ID(void)						{ return static_cast<UINT>(GetWindowLongPtr(_hWnd, GWL_ID)); };
		inline HWND					hWndPadre(void)					{ return GetParent(_hWnd); };
		inline const BOOL			Visible(const BOOL nMostrar)    { return ShowWindow(_hWnd, (nMostrar != TRUE) ? SW_HIDE : SW_SHOW); };
		inline const BOOL			Visible(void)                   { return IsWindowVisible(_hWnd); };

		inline void					BorrarBufferTeclado(void)		{ for (size_t i = 0; i < 256; i++) _Teclado[i] = false; }
		ATOM						RegistrarClase(const TCHAR *nNombre, WNDPROC WindowProcedureInicial, UINT Estilos = 0, const int nIconoRecursos = 0 ,HBRUSH nColorFondo = NULL, HINSTANCE hInstance = NULL);
		void						IniciarMiembrosEstaticos(void);
									
									// Debe utilizarse una sola vez al terminar la apliaci�n
		static void                 EliminarFuentesEstaticas();

		static DhWnd_Fuente         Fuente18Normal;
		static DhWnd_Fuente         Fuente18Negrita;
		static DhWnd_Fuente         Fuente21Normal;
		static DhWnd_Fuente         Fuente21Negrita;
		static DhWnd_Fuente         FuenteTest;
	  protected:
		HWND                       _hWnd;


		static bool                _Teclado[256];
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




	class DVentana : public DhWnd {
	  public:
									DVentana(void) : DhWnd() { }
		                           ~DVentana(void) { }
									// Funci�n para crear una ventana																																																	  /* NO SE UTILIZA */								
		HWND						CrearVentana(DhWnd *nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos = NULL, UINT nEstilosClase = NULL, HMENU nMenu = NULL, HBRUSH nColorFondo = NULL, const int nIconoRecursos = 32512);

		void						Titulo(std::wstring &nTitulo);

		DBarraTareas                BarraTareas;

		void						Opacidad(const BYTE nNivel);

		inline virtual void			Repintar(void) { RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT | RDW_FRAME); };

		virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		static LRESULT CALLBACK    _GestorMensajes(HWND HandleVentana, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};




	/* Controles estandar del windows */
	class DControl : public DhWnd {
	  public:
									DControl(void) : DhWnd() { }
		                           ~DControl(void) { }

		HWND						CrearControl(DhWnd *nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, HBRUSH nColorFondo = NULL);

		virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) { return _GestorMensajesOriginal(hWnd(), uMsg, wParam, lParam); };
	  protected:
		void					   _ConectarControl(const UINT nID, DhWnd *nPadre);
		static LRESULT CALLBACK    _GestorMensajes(HWND HandleVentana, UINT uMsg, WPARAM wParam, LPARAM lParam);
									//! WindowProcedure Orignal del control est�ndar
		WNDPROC                    _GestorMensajesOriginal;

	};



}
#endif