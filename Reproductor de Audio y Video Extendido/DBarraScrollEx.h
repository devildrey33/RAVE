#pragma once

#include "DControlEx.h"
#include "DEventoMouse.h"

namespace DWL {
	// Mostrar mensajes de depuración en la consola
	#define DBARRASCROLLEX_MOSTRARDEBUG		FALSE

	enum DBarraScrollEx_Estado {
		DBarraScrollEx_Estado_Invisible,
		DBarraScrollEx_Estado_Normal,
		DBarraScrollEx_Estado_Resaltado,
		DBarraScrollEx_Estado_Presionado
	};

	// Control que puede mostrar una barra de scroll vertical y una barra de scroll horizontal.
	class DBarraScrollEx : public DControlEx {
	  public:
									DBarraScrollEx();
		                           ~DBarraScrollEx();
//		HWND						CrearBarraScrollEx(DhWnd &Padre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT nID);
		void						Scrolls_Pintar(HDC hDC, RECT &RC); // El HDC especificado se supone que es un back buffer
		const BOOL					Scrolls_MouseMovimiento(DEventoMouse &DatosMouse);
		const BOOL					Scrolls_MousePresionado(DEventoMouse &DatosMouse);
		const BOOL					Scrolls_MouseSoltado(DEventoMouse &DatosMouse);

//		const BOOL					Scrolls_MouseEntrando();
		const BOOL                  Scrolls_MouseSaliendo();
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

		COLORREF				    ColorFondo;
		COLORREF				    ColorScroll;
		COLORREF				    ColorScrollResaltado;
		COLORREF                    ColorScrollPresionado;
		COLORREF				    ColorFondoScroll;
		COLORREF				    ColorFondoScrollResaltado;
		COLORREF				    ColorFondoScrollPresionado;

	protected:
		void					   _PintarBarraScrollEx(HDC hDC, RECT &RectaScroll, RECT &RectaBarra, const COLORREF pColorBarra, const COLORREF pColorFondo);
		const float				   _CalcularPosScrollH(const UINT nTam, const int nPos);
		const float				   _CalcularPosScrollV(const UINT nTam, const int nPos);
									
		POINT					   _Scroll_PosPresionado;		// Coordenadas del mouse al iniciar el drag
		float					   _Scroll_PosInicio;			// Posición del scroll al iniciar el drag

		int                        _ScrollV_Ancho;
		DBarraScrollEx_Estado      _ScrollV_Estado;
		float					   _ScrollV_Pagina;
		float					   _ScrollV_Posicion;
		int                        _ScrollH_Alto;
		DBarraScrollEx_Estado      _ScrollH_Estado;
		float					   _ScrollH_Pagina;
		float					   _ScrollH_Posicion;


	};

};