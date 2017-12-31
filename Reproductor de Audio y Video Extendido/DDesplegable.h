
#ifndef DDESPLEGABLE_H
#define DDESPLEGABLE_H

#include "DArbolEx.h"
//#include "DListaEx.h"

/* ComboBox que puede desplegar un DListaEx o un DArbolEx, y que puede tener el texto editable o no */
/* Añadir facilidades para hacer un modo busqueda directorios o directorios y archivos */

namespace DWL {

	enum DDesplegable_Tipo {
		DDesplegable_Tipo_Nada						= -1,// Por definir / no válido / NULL...
		// Control DDesplegable (edit / label con lista / arbol)
		DDesplegable_Tipo_Lista						= 0, // Combobox (label + lista) Lista que no permite la edición de texto
		DDesplegable_Tipo_ListaEdit_Texto			= 1, // Combobox (edit + lista) que acepta cualquier caracter
		DDesplegable_Tipo_ListaEdit_Numerico		= 2, // Combobox (edit + lista) que solo acepta numeros enteros (0-9)
		DDesplegable_Tipo_ListaEdit_NumericoDecimal	= 3, // Combobox (edit + lista) que acepta numeros decimales (0-9), y ('.' y/o ',')
		DDesplegable_Tipo_ArbolEdit_Texto			= 4, // Combobox (edit + arbol) que acepta cualquier carácer
		DDesplegable_Tipo_Arbol_Texto				= 5, // Combobox (label + arbol) que acepta texto
		DDesplegable_Tipo_ArbolEdit_Path			= 6, // Combobox (edit + arbol) que muestra un arbol de directorios
		DDesplegable_Tipo_Arbol_Path				= 7  // Combobox (label + arbol) que muestra un arbol de directorios
	};

	/* Control del estilo ComboBox que puede mostrar una DListaEx o un DArbolEx además del editbox / static text iniciales */
	class DDesplegable : public DControlEx {
  	  public:
							DDesplegable(void);
		                   ~DDesplegable(void);
		HWND				CrearDesplegable(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID, const DDesplegable_Tipo nTipo = DDesplegable_Tipo_ListaEdit_Texto);
		LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		DhWnd             *_ControlDesplegable; // Puede ser un DListaEx o un DArbolEx dependiendo del nTipo especificado crear el control con la función CrearDesplegable()
		DDesplegable_Tipo  _Tipo;				// Tipo de control desplegable
	};
}
#endif