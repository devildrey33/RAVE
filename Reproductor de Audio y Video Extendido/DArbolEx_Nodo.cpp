#include "stdafx.h"
#include "DArbolEx.h"

namespace DWL {

	DArbolEx_Nodo::DArbolEx_Nodo() :	_Siguiente(NULL), _Anterior(NULL), _Icono(NULL), _Fuente(NULL), _Ancestros(0), _AnchoTexto(0), _Arbol(NULL),
										_Expandido(FALSE), _Seleccionado(FALSE), _SubSeleccionado(FALSE), _MostrarExpansor(DArbolEx_MostrarExpansor_Auto) {
	};
	
	DArbolEx_Nodo::~DArbolEx_Nodo(void) {
		// Elimino de la memória los hijos que pueda tener
		for (size_t i = 0; i < _Hijos.size(); i++) delete _Hijos[i]; 
	};

	/* Devuelve TRUE o FALSE dependiendo de si hay que mostrar el expansor o no.. (si MostrarExpansor está en Auto, devolverá TRUE si el nodo tiene hijos, FALSE enc aso contrario) */
	const BOOL DArbolEx_Nodo::MostrarExpansor(void) { 
		switch (_MostrarExpansor) {
			case DArbolEx_MostrarExpansor_Auto		: return (_Hijos.size() > 0) ? TRUE : FALSE;
			case DArbolEx_MostrarExpansor_Mostrar	: return TRUE;
			case DArbolEx_MostrarExpansor_Ocultar	: return FALSE;
		}
		return _Expandido; 
	}

	/*
	const size_t DArbolEx_Nodo::Posicion(void) {
		if (_Padre != NULL) { 
			return std::find(_Padre->_Hijos.begin(), _Padre->_Hijos.end(), this) - _Padre->_Hijos.begin(); 
		}
		else { 
			return std::find(_Arbol->_Raiz._Hijos.begin(), _Arbol->_Raiz._Hijos.end(), this) - _Arbol->_Raiz._Hijos.begin(); 
		}
	}*/


}