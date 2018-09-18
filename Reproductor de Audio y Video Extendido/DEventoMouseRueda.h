#ifndef DEVENTOMOUSERUEDA_H
#define DEVENTOMOUSERUEDA_H


//! Espacio de nombres DWL
namespace DWL {

	#define WPARAM_TO_DEVENTOMOUSERUEDA(WP) *reinterpret_cast<DWL::DEventoMouseRueda *>(WP)
	#define DEVENTOMOUSERUEDA_TO_WPARAM(DE) reinterpret_cast<WPARAM>(&DE)

	//! Clase que contiene los parametros de un evento del mouse (mouseup, down, move, click, dobleckick, etc...) para pasarlos a la función de su ventana padre.

	class DEventoMouseRueda	{
	  public:
										DEventoMouseRueda() : wParam(0), lParam(0), ID(0) { };
										//! Constructor que define todos los valores
										/*!	Constructor asignador de datos.
											\fn			DEventoMouseRueda(const int cX, const int cY, const UINT cID);
											\param[in]	wParam	: WPARAM del evento.
											\param[in]	lParam  : LPARAM del evento.
											\param[in]	cID		: ID del Control.
											\return		No devuelve nada.
										*/
										DEventoMouseRueda(WPARAM nwParam, LPARAM nlParam, const INT_PTR cID) : wParam(nwParam), lParam(nlParam), ID(cID) {
										};

		                               ~DEventoMouseRueda() { };

										//! Función que devuelve la coordenada X de este evento.
										/*! Función que devuelve la coordenada X de este evento.
											\fn			inline const int X(void);
											\return		devuelve la coordenada X.
										*/
		inline const int                X(void) {
											return GET_X_LPARAM(lParam);
										};

										//! Función que devuelve la coordenada Y de este evento.
										/*! Función que devuelve la coordenada Y de este evento. 
											\fn			inline const int Y(void);
											\return		devuelve la coordenada Y.
										*/
		inline const int                Y(void) {
											return GET_Y_LPARAM(lParam);
										};

										/* Utilizar varios botones ademas de la rueda me parece chungo como poco, ya se me hace dificil pensar el controlar si algun boton del mouse está presionado ademas del desplazamiento de la rueda.) */
		inline const int                Boton(void) {
											WORD Estado = GET_KEYSTATE_WPARAM(wParam);
											if (Estado & MK_LBUTTON)  return 0;
											if (Estado & MK_RBUTTON)  return 1;
											if (Estado & MK_MBUTTON)  return 2;
											if (Estado & MK_XBUTTON1) return 3;
											if (Estado & MK_XBUTTON2) return 4;
											return -1;
										};

		inline const BOOL               ControlPresionado(void) { 
											return GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL;
										};

		inline const BOOL               ShiftPresionado(void) { 
											return GET_KEYSTATE_WPARAM(wParam) & MK_SHIFT;
										};

		inline const short				Delta(void) {
											return GET_WHEEL_DELTA_WPARAM(wParam);
										};

		WPARAM							wParam;
		LPARAM							lParam;
		UINT_PTR                        ID;
	};
};
#endif