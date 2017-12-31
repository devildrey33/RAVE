#ifndef DEDICIONTEXTO_H
#define DEDICIONTEXTO_H

#include "DControlEx.h"
namespace DWL {
	// Requiere DarbolEx, DListaEx editbox 
	enum DEdicionTexto_Tipo {
		DEdicionTexto_Tipo_Nada							= -1,// Por definir / no válido / NULL / del madrid / no vale....
		/////////////////////////////////////////////////////// Control DDesplegable (edit / label con lista / arbol)
		DEdicionTexto_Tipo_Lista						= 0, //   Combobox (label + lista) Lista que no permite la edición de texto
		DEdicionTexto_Tipo_ListaEdit_Texto				= 1, //   Combobox (edit + lista) que acepta cualquier caracter
		DEdicionTexto_Tipo_ListaEdit_Numerico			= 2, //   Combobox (edit + lista) que solo acepta numeros enteros (0-9)
		DEdicionTexto_Tipo_ListaEdit_NumericoDecimal	= 3, //   Combobox (edit + lista) que acepta numeros decimales (0-9), y ('.' y/o ',')
		DEdicionTexto_Tipo_ArbolEdit_Texto				= 4, //   Combobox (edit + arbol) que acepta cualquier carácer
		DEdicionTexto_Tipo_Arbol_Texto					= 5, //   Combobox (label + arbol) que acepta texto
		DEdicionTexto_Tipo_ArbolEdit_Path				= 6, //   Combobox (edit + arbol) que muestra un arbol de directorios
		DEdicionTexto_Tipo_Arbol_Path					= 7, //   Combobox (label + arbol) que muestra un arbol de directorios
		/////////////////////////////////////////////////////// Modo solo editbox (sin desplegable) -
		DEdicionTexto_Tipo_Edit_Texto					= 8, //   Editbox que acepta cualquier caracter
		DEdicionTexto_Tipo_Edit_Numerico				= 9, //   Editbox que acepta solo numeros enteros
		DEdicionTexto_Tipo_Edit_NumericoDecimal			= 10,//   Editbox que acepta numeros decimales (con . o ,)
	};

	/* Clase para crear una ventana de edición suplementaria para controles del tipo lista / arbol (requiere DDesplegable)
		Debes asegurarte de llamar a la función CancelarEdicionTexto() en el mensaje WM_SETFOCUS de tu conntrol extendido	*/
	class DEdicionTexto {
	public:
									DEdicionTexto(void);
		                           ~DEdicionTexto(void);
									// Devuelve el HWND del control de edición
		HWND						EmpezarEdicionTexto(HWND hWndDest, std::wstring &nTexto, RECT &Espacio, const DEdicionTexto_Tipo nTipo = DEdicionTexto_Tipo_Edit_Texto);
		void						TerminarEdicionTexto(void);
		void						CancelarEdicionTexto(void);
	  private:
		HWND                       _hWndEdicionTexto;
		static LRESULT CALLBACK    _GestorMensajesEdicionTexto(HWND dhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
									//
									//! WindowProcedure Original del Control de edición
		WNDPROC                    _GestorMensajesOriginalEdicionTexto;

		std::wstring               _Resultado;
	};

}
#endif