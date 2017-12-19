#include "stdafx.h"
#include "DArbolEx.h"

namespace DWL {

	DArbolEx_Nodo::DArbolEx_Nodo() :	_Siguiente(NULL), _Icono(NULL), _Fuente(NULL), _Ancestros(0), _AnchoTexto(0), _Arbol(NULL),
										_Expandido(FALSE), _Seleccionado(FALSE), _MostrarExpansor(DArbolEx_MostrarExpansor_Auto) {
	};
	
	DArbolEx_Nodo::~DArbolEx_Nodo(void) {
		// Elimino de la memória los hijos que pueda tener
		for (size_t i = 0; i < _Hijos.size(); i++) delete _Hijos[i]; 
	};

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