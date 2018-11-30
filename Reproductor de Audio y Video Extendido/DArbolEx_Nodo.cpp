#include "stdafx.h"
#include "DArbolEx.h"
#include "DListaIconos.h"


namespace DWL {

	DArbolEx_Nodo::DArbolEx_Nodo(void) :	_Siguiente(NULL), _Anterior(NULL), _Icono(NULL), _Fuente(NULL), _Ancestros(0), _AnchoTexto(0), _Arbol(NULL), 
											_ColorExpansor(COLOR_ARBOL_EXPANSOR_NORMAL), _ColorTexto(COLOR_ARBOL_TEXTO), _ColorTextoSombra(COLOR_ARBOL_TEXTO_SOMBRA), _ColorFondo(COLOR_ARBOL_FONDO),
											Expandido(FALSE), Seleccionado(FALSE), _SubSeleccionado(FALSE), _MostrarExpansor(DArbolEx_MostrarExpansor_Auto), _Activado(TRUE) {
	};
	
	DArbolEx_Nodo::~DArbolEx_Nodo(void) {
		// Elimino de la memória los hijos que pueda tener
		for (size_t i = 0; i < _Hijos.size(); i++) delete _Hijos[i]; 
	};

	/* Devuelve TRUE o FALSE dependiendo de si hay que mostrar el expansor o no.. (si MostrarExpansor está en Auto, devolverá TRUE si el nodo tiene hijos, FALSE enc aso contrario) */
	const DArbolEx_MostrarExpansor DArbolEx_Nodo::MostrarExpansor(void) {
		if (_MostrarExpansor == DArbolEx_MostrarExpansor_Auto) {
			DArbolEx_Expansor EPD = _Arbol->ExpansorPorDefecto();
			if (_Hijos.size() == 0) 
				return DArbolEx_MostrarExpansor_Ocultar;
			switch (EPD) {
				case DArbolEx_Expansor_Triangulo		:  return DArbolEx_MostrarExpansor_MostrarTriangulo;
				case DArbolEx_Expansor_TrianguloLinea	:  return DArbolEx_MostrarExpansor_MostrarTrianguloLinea;
				case DArbolEx_Expansor_Rectangulo		:  return DArbolEx_MostrarExpansor_MostrarRectangulo;
			}
		}
		return _MostrarExpansor;
	}


	void DArbolEx_Nodo::Icono(const int nIconoRecursos) {
		_Icono = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DARBOLEX_TAMICONO, DARBOLEX_TAMICONO);
	}

	void DArbolEx_Nodo::Icono(const GUID nIconoKnownFolder) {
		_Icono = DListaIconos::AgregarIconoKnownFolder(nIconoKnownFolder, DARBOLEX_TAMICONO, DARBOLEX_TAMICONO);
	}

	void DArbolEx_Nodo::Icono(const wchar_t *nIconoPath, const int nIconoPos) {
		_Icono = DListaIconos::AgregarIconoExterno(nIconoPath, DARBOLEX_TAMICONO, DARBOLEX_TAMICONO, nIconoPos);
	}

	void DArbolEx_Nodo::BorrarHijos(void) {
		_Arbol->_Repintar = TRUE;
		for (size_t i = 0; i < _Hijos.size(); i++) {
			delete _Hijos[i];
		}
		_Hijos.resize(0);
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

	void DArbolEx_Nodo::_TransicionExpansor(const DArbolEx_TransicionExpansor nTransicion) {
		DWORD Duracion = DhWnd::TiempoAnimaciones;
		if (_AniTransicionExpansor.Animando() == TRUE) {
			Duracion = _AniTransicionExpansor.TiempoActual();
			_AniTransicionExpansor.Terminar();
		}

		COLORREF ExpansorHasta;
		switch (nTransicion) {
		case DArbolEx_TransicionExpansor_Normal:
			ExpansorHasta = COLOR_ARBOL_EXPANSOR_NORMAL;
			break;
		case DArbolEx_TransicionExpansor_Resaltado:
			ExpansorHasta = COLOR_ARBOL_EXPANSOR_RESALTADO;
			break;
		case DArbolEx_TransicionExpansor_Presionado:
			ExpansorHasta = COLOR_ARBOL_EXPANSOR_PRESIONADO;
			break;
		}

		_AniTransicionExpansor.Iniciar(_ColorExpansor, ExpansorHasta, Duracion, [=](DAnimacion::Valores &Datos, const BOOL Terminado) {
			_ColorExpansor = Datos[0].Color();
			_Arbol->RepintarAni();
		});
	}
	
	void DArbolEx_Nodo::_Transicion(const DArbolEx_TransicionNodo nTransicion) {
		DWORD Duracion = DhWnd::TiempoAnimaciones;
		if (_AniTransicion.Animando() == TRUE) {
			Duracion = _AniTransicion.TiempoActual();
			_AniTransicion.Terminar();
		}

		COLORREF TextoHasta = 0, SombraHasta = 0, FondoHasta = 0;
		switch (nTransicion) {
			case DArbolEx_TransicionNodo_Normal:
				TextoHasta  = COLOR_ARBOL_TEXTO;
				SombraHasta = COLOR_ARBOL_TEXTO_SOMBRA;
				FondoHasta  = _Arbol->_ColorFondo;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> Normal)\n", Texto.c_str());
				#endif			
				break;
			case DArbolEx_TransicionNodo_Resaltado:
				TextoHasta  = COLOR_ARBOL_TEXTO_RESALTADO;
				SombraHasta = COLOR_ARBOL_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_FONDO_RESALTADO;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> Resaltado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_Seleccionado:
				TextoHasta  = COLOR_ARBOL_SELECCION_TEXTO;
				SombraHasta = COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_SELECCION;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> Seleccionado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_SeleccionadoResaltado :
				TextoHasta  = COLOR_ARBOL_SELECCION_TEXTO_RESALTADO;
				SombraHasta = COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_SELECCION_RESALTADO;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> Seleccionado resaltado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_SeleccionadoPresionado :
				TextoHasta  = COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO;
				SombraHasta = COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_SELECCION_PRESIONADO;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> Seleccionado presionado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_SubSeleccionado:
				TextoHasta  = COLOR_ARBOL_SUBSELECCION_TEXTO;
				SombraHasta = COLOR_ARBOL_SUBSELECCION_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_SUBSELECCION;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> SubSeleccionado resaltado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_SubSeleccionadoResaltado:
				TextoHasta  = COLOR_ARBOL_SUBSELECCION_TEXTO_RESALTADO;
				SombraHasta = COLOR_ARBOL_SUBSELECCION_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_SUBSELECCION_RESALTADO;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> SubSeleccionado resaltado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_SubSeleccionadoPresionado:
				TextoHasta  = COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO;
				SombraHasta = COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_SELECCION_PRESIONADO;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> SubSeleccionado presionado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_Desactivado:
				TextoHasta  = COLOR_ARBOL_TEXTO_DESACTIVADO;
				SombraHasta = COLOR_ARBOL_TEXTO_SOMBRA;
				FondoHasta  = _Arbol->_ColorFondo;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> Desactivado)\n", Texto.c_str());
				#endif
				break;
			case DArbolEx_TransicionNodo_DesactivadoResaltado	:
				TextoHasta  = COLOR_ARBOL_TEXTO_DESACTIVADO;
				SombraHasta = COLOR_ARBOL_TEXTO_SOMBRA;
				FondoHasta  = COLOR_ARBOL_FONDO_RESALTADO;
				#if DARBOLEX_MOSTRARDEBUG == TRUE
					Debug_Escribir_Varg(L"DArbolEx_Nodo::_Transicion('%s' -> Desactivado resaltado)\n", Texto.c_str());
				#endif
				break;
		}

		_AniTransicion.Iniciar(_ColorTexto, TextoHasta, _ColorTextoSombra, SombraHasta, _ColorFondo, FondoHasta, Duracion, [=](DAnimacion::Valores &Datos, const BOOL Terminado) {
			_ColorTexto			= Datos[0].Color();
			_ColorTextoSombra	= Datos[1].Color();
			_ColorFondo			= Datos[2].Color();
			_Arbol->RepintarAni();
		});
	}

	void DArbolEx_Nodo::Activado(const BOOL nActivado) {
		_AniTransicion.Terminar();
		_Activado = nActivado;
		
		if (_Activado == TRUE) {
			if (Seleccionado == TRUE) {
				if (_SubSeleccionado == TRUE) {					
					_ColorTexto			= COLOR_ARBOL_SUBSELECCION_TEXTO;
					_ColorTextoSombra	= COLOR_ARBOL_SUBSELECCION_TEXTO_SOMBRA;
					_ColorFondo			= COLOR_ARBOL_SUBSELECCION;
				}
				else {
					_ColorTexto			= COLOR_ARBOL_SELECCION_TEXTO;
					_ColorTextoSombra	= COLOR_ARBOL_SELECCION_TEXTO_SOMBRA;
					_ColorFondo			= COLOR_ARBOL_SELECCION;
				}
			}
			else {
				_ColorTexto			= COLOR_ARBOL_TEXTO;
				_ColorTextoSombra	= COLOR_ARBOL_TEXTO_SOMBRA;
				_ColorFondo			= COLOR_ARBOL_FONDO;
			}
		}
		else {
			_ColorTexto			= COLOR_ARBOL_TEXTO_DESACTIVADO;
			_ColorTextoSombra	= COLOR_ARBOL_TEXTO_SOMBRA;
			_ColorFondo			= COLOR_ARBOL_FONDO;
		}
		
	}


	void DArbolEx_Nodo::_AsignarColores(COLORREF nColTexto, COLORREF nColTextoSombra, COLORREF nColFondo) {
		_AniTransicion.Terminar();
		_ColorTexto			= nColTexto;
		_ColorTextoSombra	= nColTextoSombra;
		_ColorFondo			= nColFondo;
	}


	void DArbolEx_Nodo::_TransicionResaltado(void) {
		_Transicion(
			(_Activado == TRUE) ? 
				(_SubSeleccionado == TRUE) ? 
					DArbolEx_TransicionNodo_SubSeleccionadoResaltado	 :
					(Seleccionado == TRUE) ?
						DArbolEx_TransicionNodo_SeleccionadoResaltado : 
						DArbolEx_TransicionNodo_Resaltado : 
				DArbolEx_TransicionNodo_DesactivadoResaltado
		);
	}

	void DArbolEx_Nodo::_TransicionNormal(void) {
		_Transicion(
			(_Activado == TRUE) ? 
				(_SubSeleccionado == TRUE) ? 
					DArbolEx_TransicionNodo_SubSeleccionado	 :
					(Seleccionado == TRUE) ?
						DArbolEx_TransicionNodo_Seleccionado : 
						DArbolEx_TransicionNodo_Normal : 
				DArbolEx_TransicionNodo_Desactivado
		);
	}

}