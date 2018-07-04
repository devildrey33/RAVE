#ifndef DCONTROLEX_EVENTOMOUSE_H
#define DCONTROLEX_EVENTOMOUSE_H


//! Espacio de nombres DWL
namespace DWL {

	//! Clase que contiene los parametros de un evento del mouse (mouseup, down, move, click, dobleckick, etc...) para pasarlos a la funci�n de su ventana padre.
	class DControlEx_EventoMouse {
	  public: //////////////////////////// Miembros publicos

										//! Constructor.
										/*!	Constructor por defecto.
											\fn			DControlEx_EventoMouse(void);
											\return		No devuelve nada.
										*/
										DControlEx_EventoMouse(void) : wParam(0), lParam(0), ID(0), Boton(-1) {
										};

										//! Constructor que define todos los valores
										/*!	Constructor asignador de datos.
											\fn			DControlEx_EventoMouse(const int cX, const int cY, const UINT cID, const UINT nBoton);
											\param[in]	wParam	: WPARAM del evento.
											\param[in]	lParam  : LPARAM del evento.
											\param[in]	cID		: ID del ArbolEx.
											\param[in]	cBoton	: Bot�n del mouse pulsado (si es -1 es que no se ha pulsado ning�n bot�n).
											\return		No devuelve nada.
										*/
										DControlEx_EventoMouse(WPARAM nwParam, LPARAM nlParam, const UINT cID, const int cBoton = -1) :
											wParam(nwParam), lParam(nlParam), ID(cID), Boton(cBoton) {
										};

										//! Destructor.
										/*!	Destructor.
											\fn		   ~DControlEx_EventoMouse(void);
											\return		No devuelve nada.
										*/
			                            ~DControlEx_EventoMouse(void) {
										};

										//! Funci�n que devuelve la coordenada X de este evento.
										/*! Funci�n que devuelve la coordenada X de este evento.
											\fn			inline const int X(void);
											\return		devuelve la coordenada X.
										*/
		inline const int                X(void) {
											return GET_X_LPARAM(lParam);
										};

										//! Funci�n que devuelve la coordenada Y de este evento.
										/*! Funci�n que devuelve la coordenada Y de este evento. 
											\fn			inline const int Y(void);
											\return		devuelve la coordenada Y.
										*/
		inline const int                Y(void) {
											return GET_Y_LPARAM(lParam);
										};

										//! Funci�n que devuelve el bot�n del mouse que se ha presionado.
										/*! Funci�n que devuelve el bot�n del mouse que se ha presionado.
											\fn			const int Boton(void);
											\return		devuelve el boton del mouse presionado.
										*/
										// ANULADA (El mouseUp no devuelve el bot�n.....)
/*		const int                       Boton(void) {
											if (wParam & MK_LBUTTON)		return 0; // Bot�n izquierdo
											if (wParam & MK_MBUTTON)		return 1; // Bot�n derecho
											if (wParam & MK_RBUTTON)		return 2; // Bot�n medio
											if (wParam & MK_XBUTTON1)		return 3; // Bot�n extra 1
											if (wParam & MK_XBUTTON2)		return 4; // Bot�n extra 2						
											return -1;								  // No se ha presionado ning�n bot�n del mouse.
										};*/

										//! Funci�n que devuelve si se ha presionado la tecla Control.
										/*! Funci�n que devuelve si se ha presionado la tecla Control.
											\fn			inline const int Control(void);
											\return		devuelve TRUE si la tecla control est� presionada.
										*/
		inline const BOOL               Control(void) {
											return (wParam & MK_CONTROL);
										};

										//! Funci�n que devuelve si se ha presionado la tecla Shift.
										/*! Funci�n que devuelve si se ha presionado la tecla Shift.
											\fn			inline const int Shift(void);
											\return		devuelve TRUE si la tecla shift est� presionada.
										*/
		inline const BOOL               Shift(void) {
											return (wParam & MK_SHIFT);
										};
										
										// wParam que devuelven los mensajes del mouse
		WPARAM                          wParam;
										// wParam que devuelven los mensajes del mouse
		LPARAM                          lParam;
										// ID del control
		UINT                            ID;
										// Bot�n presionado (puede ser -1)
		int                             Boton;

	  private: /////////////////////////// Miembros privados

										//! Constructor copia des-habilitado
										DControlEx_EventoMouse(const DControlEx_EventoMouse &) : wParam(0), lParam(0) { };
										//! Operador = des-habilitado
		inline DControlEx_EventoMouse  &operator=(const DControlEx_EventoMouse &) { return *this; };


	};									//
	////////////////////////////////////// DControlEx_EventoMouse
};

#endif
