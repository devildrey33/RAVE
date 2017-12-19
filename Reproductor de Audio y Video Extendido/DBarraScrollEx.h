#pragma once

#include "DhWnd.h"

namespace DWL {

	enum DBarraScrollEx_Estado {
		DBarraScrollEx_Estado_Invisible,
		DBarraScrollEx_Estado_Normal,
		DBarraScrollEx_Estado_Resaltado,
		DBarraScrollEx_Estado_Presionado
	};

	// Control que puede mostrar una barra de scroll vertical y una barra de scroll horizontal.
	class DBarraScrollEx : public virtual DControlEx_FondoEstatico {
	  public:
									DBarraScrollEx();
		                           ~DBarraScrollEx();
//		HWND						CrearBarraScrollEx(DhWnd &Padre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT nID);
		void						Scrolls_Pintar(HDC hDC, RECT &RC);
		const BOOL					Scrolls_MouseMovimiento(const int cX, const int cY, const UINT Param);
		const BOOL					Scrolls_MousePresionado(const int cX, const int cY, const UINT Param);
		const BOOL					Scrolls_MouseSoltado(const int cX, const int cY, const UINT Param);
									// Obtiene el área que pertenece al control (RectaCliente es el resultado de GetClientRect, y RectaClienteSinScroll es el área del control excluyendo las barras de scroll) 
		void						ObtenerRectaCliente(RECT *RectaCliente, RECT *RectaClienteSinScroll);
									// Obtiene el área de los scrolls
		void						ObtenerRectasScroll(RECT &RC, RECT &RectaH, RECT &RectaV);
		void						ObtenerRectasScroll(RECT &RectaH, RECT &RectaV);
									// Obtiene el área de la barra dentro del scroll vertical
		void						ObtenerRectaBarraScrollV(RECT &RectaScroll, RECT &RectaBarra);
									// Obtiene el área de la barra dentro del scroll horizontal
		void						ObtenerRectaBarraScrollH(RECT &RectaScroll, RECT &RectaBarra);

		virtual void				Scrolls_EventoCambioPosicion(void) { };
		//		LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
		////////////////////////////// ScrollH
		void						ScrollH_Pagina(const float nValor);
		const float					ScrollH_Pagina(void);
		void						ScrollH_Posicion(const float nValor);
		const float					ScrollH_Posicion(void);
		void						ScrollH_Visible(const BOOL nVisible);
		////////////////////////////// ScrollV
		void						ScrollV_Pagina(const float nValor);
		const float					ScrollV_Pagina(void);
		void						ScrollV_Posicion(const float nValor);
		const float					ScrollV_Posicion(void);
		void						ScrollV_Visible(const BOOL nVisible);

		inline void					ColorScroll(COLORREF nColor)			{ _ColorScroll = nColor;			Repintar(); }
		inline const COLORREF		ColorScroll(void)						{ return _ColorScroll;							}
		inline void					ColorScrollResaltado(COLORREF nColor)	{ _ColorScrollResaltado = nColor;	Repintar(); }
		inline const COLORREF		ColorScrollResaltado(void)				{ return _ColorScrollResaltado;					}
		inline void					ColorScrollPresionado(COLORREF nColor)	{ _ColorScrollPresionado = nColor;	Repintar(); }
		inline const COLORREF		ColorScrollPresionado(void)				{ return _ColorScrollPresionado;				}
		inline void					ColorFondoScroll(COLORREF nColor)		{ _ColorFondoScroll = nColor;		Repintar(); }
		inline const COLORREF		ColorFondoScroll(void)					{ return _ColorFondoScroll;						}

	protected:
		void					   _PintarBarraScrollEx(HDC hDC, RECT &RectaScroll, RECT &RectaBarra, const COLORREF pColorBarra);
		const float				   _CalcularPosScrollH(const UINT nTam, const int nPos);
		const float				   _CalcularPosScrollV(const UINT nTam, const int nPos);
									
		POINT					   _Scroll_PosPresionado;		// Coordenadas del mouse al iniciar el drag
		float					   _Scroll_PosInicio;			// Posición del scroll al iniciar el drag

		DBarraScrollEx_Estado      _ScrollV_Estado;
		float					   _ScrollV_Pagina;
		float					   _ScrollV_Posicion;
		DBarraScrollEx_Estado      _ScrollH_Estado;
		float					   _ScrollH_Pagina;
		float					   _ScrollH_Posicion;

		COLORREF				   _ColorScroll;
		COLORREF				   _ColorScrollResaltado;
		COLORREF                   _ColorScrollPresionado;
		COLORREF				   _ColorFondoScroll;
	};

};