
#ifndef DDESPLEGABLEEX_H
#define DDESPLEGABLEEX_H

#include "DDesplegableEx_ExplorarDirectorios.h"
//#include "DListaEx.h"

/* ComboBox que puede desplegar un DListaEx o un DArbolEx, y que puede tener el texto editable o no */
/* Añadir facilidades para hacer un modo busqueda directorios o directorios y archivos */

namespace DWL {


	enum DDesplegableEx_TipoEdicion {
		DDesplegableEx_TipoEdicion_SinEdicion,
		DDesplegableEx_TipoEdicion_Texto,
		DDesplegableEx_TipoEdicion_NumericoEntero,
		DDesplegableEx_TipoEdicion_NumericoDecimal
	};

	enum DDesplegableEx_TipoDesplegable {
		DDesplegableEx_TipoDesplegable_Lista,
		DDesplegableEx_TipoDesplegable_Arbol,
		DDesplegableEx_TipoDesplegable_ExplorarDirectorios
	};

	/* Control del estilo ComboBox que puede mostrar una DListaEx o un DArbolEx además del editbox / static text iniciales */
	class DDesplegableEx : public DControlEx {
  	  public:
											DDesplegableEx(void);
		                                   ~DDesplegableEx(void);
		HWND								CrearDesplegable(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const DDesplegableEx_TipoEdicion nTipoEdicion = DDesplegableEx_TipoEdicion_Texto, const DDesplegableEx_TipoDesplegable nTipoDesplegable = DDesplegableEx_TipoDesplegable_Lista);
		void								MostrarDesplegable(void);
		void                                Pintar(HDC hDC);
		LRESULT CALLBACK					GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		void							   _Evento_Pintar(void);
		void							   _Evento_MouseMovimiento(const int cX, const int cY, const UINT Param);
		void							   _Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void							   _Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void                               _Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey);
//		void							   _Evento_MouseDobleClick(const UINT Boton, const int cX, const int cY, const UINT Param);
		void							   _Evento_MouseSaliendo(void);
		void                               _Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void                               _Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void							   _Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param);

		DDesplegableEx_TipoEdicion	       _TipoEdicion;				
		DDesplegableEx_TipoDesplegable	   _TipoDesplegable;			

		DhWnd_Fuente                       _Fuente;
	};
}
#endif