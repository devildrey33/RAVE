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
	// - Está costruida de forma que solo se puede asignar el miembro _hWmd internamente.
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
									// Función para crear una ventana
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
									//! WindowProcedure Orignal del control estándar
		WNDPROC                    _GestorMensajesOriginal;

	};




	/* Controles de usuario fabricados desde 0 */
	class DControlEx  {
	 public:
									DControlEx(void) : _MouseDentro(FALSE) { }
		                           ~DControlEx(void) { }

		HWND						CrearControlEx(DhWnd &nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase = NULL, HBRUSH nColorFondo = NULL);

		virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd(), uMsg, wParam, lParam); };
		DhWnd						hWnd;
		inline void					Repintar(void) { RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT); };
	 protected:
		void					   _ConectarControl(const UINT nID, DhWnd &nPadre);
		static LRESULT CALLBACK    _GestorMensajes(HWND HandleVentana, UINT uMsg, WPARAM wParam, LPARAM lParam);

		const BOOL                 _MouseEntrando(void);
									//! Valor que determina si el mouse esta dentro del control
		BOOL                       _MouseDentro;

	};






	/* Control básico con borde, sin texto y sin foco */
	class DControlEx_BordeEstatico : virtual public DControlEx {
	  public:
								DControlEx_BordeEstatico(void) : DControlEx(), _ColorBorde(COLOR_BORDE) { }
		                       ~DControlEx_BordeEstatico(void)	{ }
		inline void				ColorBorde(COLORREF nColor)		{ _ColorBorde = nColor;		Repintar(); }
		inline const COLORREF	ColorBorde(void)				{ return _ColorBorde; }
	  protected:
		COLORREF               _ColorBorde;
	};


	/* Control básico con borde, sin texto que puede tener el foco (con estados resaltado y presionado) */
	class DControlEx_BordeDinamico : public DControlEx_BordeEstatico {
	  public:
								DControlEx_BordeDinamico(void) : DControlEx_BordeEstatico(), _ColorBordeResaltado(COLOR_BORDE_RESALTADO), _ColorBordePresionado(COLOR_BORDE_PRESIONADO) { }
		                       ~DControlEx_BordeDinamico(void) { }
		inline void				ColorBordeResaltado(COLORREF nColor)	{ _ColorBordeResaltado = nColor;	Repintar(); }
		inline const COLORREF	ColorBordeResaltado(void)				{ return _ColorBordeResaltado; }
		inline void				ColorBordePresionado(COLORREF nColor)	{ _ColorBordePresionado = nColor;	Repintar(); }
		inline const COLORREF	ColorBordePresionado(void)				{ return _ColorBordePresionado; }
	  protected:
		COLORREF               _ColorBordeResaltado;
		COLORREF               _ColorBordePresionado;
	};


	/* Control básico sin borde, sin texto y sin foco */
	class DControlEx_FondoEstatico : virtual public DControlEx {
	  public:
								DControlEx_FondoEstatico(void) : DControlEx(), _ColorFondo(COLOR_FONDO) { }
							   ~DControlEx_FondoEstatico(void)	{ }
		inline void				ColorFondo(COLORREF nColor)		{ _ColorFondo = nColor;		Repintar(); }
		inline const COLORREF	ColorFondo(void)				{ return _ColorFondo; }
	  protected:
		COLORREF               _ColorFondo;
	};


	/* Control básico sin borde, sin texto que puede tener el foco (con estados resaltado y presionado) */
	class DControlEx_FondoDinamico : public DControlEx_FondoEstatico {
	  public:
								DControlEx_FondoDinamico(void) : DControlEx_FondoEstatico(), _ColorFondoResaltado(COLOR_BOTON_RESALTADO), _ColorFondoPresionado(COLOR_BOTON_PRESIONADO) { }
		                       ~DControlEx_FondoDinamico(void) { }
		inline void				ColorFondoResaltado(COLORREF nColor)	{ _ColorFondoResaltado = nColor;	Repintar(); }
		inline const COLORREF	ColorFondoResaltado(void)				{ return _ColorFondoResaltado; }
		inline void				ColorFondoPresionado(COLORREF nColor)	{ _ColorFondoPresionado = nColor;	Repintar(); }
		inline const COLORREF	ColorFondoPresionado(void)				{ return _ColorFondoPresionado; }
  	  protected:
		COLORREF               _ColorFondoResaltado;
		COLORREF               _ColorFondoPresionado;
	};

	/* Control básico con texto y sin foco */
	class DControlEx_TextoEstatico : virtual public DControlEx {
	  public:
								DControlEx_TextoEstatico(void) : DControlEx(), _ColorTexto(COLOR_TEXTO), _ColorTextoSombra(COLOR_TEXTO_SOMBRA), _Fuente(NULL) { }
		                       ~DControlEx_TextoEstatico(void) { }
		inline void				ColorTexto(COLORREF nColor)			{ _ColorTexto = nColor;			Repintar(); }
		inline const COLORREF	ColorTexto(void)					{ return _ColorTexto; }
		inline void				ColorTextoSombra(COLORREF nColor)	{ _ColorTextoSombra = nColor;	Repintar(); }
		inline const COLORREF	ColorTextoSombra(void)				{ return _ColorTextoSombra; }
		inline void				Fuente(HFONT nFuente)				{ _Fuente = nFuente;			Repintar(); }
		inline void				Texto(std::wstring nTexto)			{ _Texto = nTexto; }
		inline std::wstring		Texto(void)							{ return _Texto; }
	  protected:
		std::wstring           _Texto;
		COLORREF               _ColorTexto;
		COLORREF               _ColorTextoSombra;
		DhWnd_Fuente           _Fuente;
	};

	/* Control básico con texto y que puede tener el foco (con estados resaltado y presionado) */
	class DControlEx_TextoDinamico : public DControlEx_TextoEstatico {
	  public:
								DControlEx_TextoDinamico(void) : DControlEx_TextoEstatico(), _ColorTextoResaltado(COLOR_TEXTO_RESALTADO), _ColorTextoPresionado(COLOR_TEXTO_PRESIONADO) { }
		                       ~DControlEx_TextoDinamico(void) { }
		inline void				ColorTextoResaltado(COLORREF nColor)	{ _ColorTextoResaltado = nColor;		Repintar(); }
		inline const COLORREF	ColorTextoResaltado(void)				{ return _ColorTextoResaltado; }
		inline void				ColorTextoPresionado(COLORREF nColor)	{ _ColorTextoPresionado = nColor;		Repintar(); }
		inline const COLORREF	ColorTextoPresionado(void)				{ return _ColorTextoPresionado; }
   	  protected:
		COLORREF               _ColorTextoResaltado;
		COLORREF               _ColorTextoPresionado;
	};


}
#endif