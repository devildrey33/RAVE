#pragma once

#include "RAVE.h"
#include "DBarraScrollEx.h"
#include "DArbolEx_Nodo.h"
//#include "DControl_Fuente.h"

namespace DWL {

#ifdef _WIN64
	#define DARBOLEX_POSICIONNODO_INICIO	0
	#define DARBOLEX_POSICIONNODO_FIN		_UI64_MAX - 1
	#define DARBOLEX_POSICIONNODO_ORDENADO	_UI64_MAX - 2
#else
	#define DARBOLEX_POSICIONNODO_INICIO	0
	#define DARBOLEX_POSICIONNODO_FIN		_UI32_MAX - 1
	#define DARBOLEX_POSICIONNODO_ORDENADO	_UI32_MAX - 2
#endif
	enum DArbolEx_PosicionNodo {
		DArbolEx_PosicionNodo_Inicio,
		DArbolEx_PosicionNodo_Fin,
		DArbolEx_PosicionNodo_Ordenado,
	};

	enum DArbolEx_HitTest {
		DArbolEx_HitTest_Icono,
		DArbolEx_HitTest_Texto,
		DArbolEx_HitTest_Expansor,
		DArbolEx_HitTest_Todo		// (expansor, icono y texto)
	};

	/* Plantilla ArbolEx que se crea partiendo del tipo de nodo */
	class DArbolEx : public virtual DBarraScrollEx {
	  public:
														DArbolEx(void);
		                                               ~DArbolEx(void);

														// Crea el control ArbolEx
		HWND											CrearArbolEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

		void											Expandir(DArbolEx_Nodo *nNodo, const BOOL nExpandir);

														// Obtiene el nodo de la posición especificada (El máximo de hijos se obtiene con la función 'const size_t TotalHijos')
		inline DArbolEx_Nodo                           *Nodo(const size_t nPos) { return _Raiz.Hijo(nPos); };
														// Nodo raiz
		inline DArbolEx_Nodo                           *NodoRaiz(void)			{ return &_Raiz; }
														// Total de nodos
		inline const size_t								TotalNodos(void)		{ return _Raiz._Hijos.size(); }

														// Agrega un nodo personalizado (por defecto es del tipo DArbolEx_Nodo)
		template <class TNodo = DArbolEx_Nodo> TNodo   *AgregarNodo(const TCHAR *nTexto, DArbolEx_Nodo *nPadre = NULL, const int nIcono = NULL, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN) {
															TNodo *nNodo = new TNodo();
															_AgregarNodo(nNodo, nTexto, nPadre, nIcono, nFuente, PosicionNodo);
															return nNodo;
														};

														// Eliminar Nodo
		void											EliminarNodo(DArbolEx_Nodo *nEliminar);

		void											Pintar(HDC hDC);

														// Pintar Nodo
		void											PintarNodo(HDC hDC, RECT *Espacio, DArbolEx_Nodo *nNodo, const int PosH);

		void											Scrolls_EventoCambioPosicion(void);

		void											Evento_MouseMovimiento(const int cX, const int cY, const UINT Param);

		DArbolEx_Nodo                                  *HitTest(const int cX, const int cY, const DArbolEx_HitTest nTipo);

		LRESULT CALLBACK								GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected:
		void										   _CrearBufferNodo(const int nAncho, const int nAlto);
		HDC                                            _BufferNodo;
		HBITMAP                                        _BufferNodoBmp;
		HBITMAP                                        _BufferNodoBmpViejo;

	    void					  					   _CalcularNodosPagina(const size_t TamPagina);

		void										   _CalcularScrolls(void);
														// Busca el siguiente nodo
		DArbolEx_Nodo								  *_BuscarSiguienteNodo(DArbolEx_Nodo *nActual);
														// Busca el siguiente nodo visible
		DArbolEx_Nodo								  *_BuscarSiguienteNodoVisible(DArbolEx_Nodo *nActual);
														// Agrega un nodo (se tiene que reservar memória en la variable nNodo antes de agregar. ej nNodo = new DArbolEx_Nodo)
		DArbolEx_Nodo                                 *_AgregarNodo(DArbolEx_Nodo *nNodo, const TCHAR *nTexto, DArbolEx_Nodo *nPadre = NULL, const int nIcono = NULL, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN);
														// Obtiene el espacio en pixeles que necesita todo el arbol tal y como están los nodos expandidos
		void									       _CalcularTotalEspacioVisible(void);
														// Total de ancho necesario para los nodos visibles
		size_t										   _TotalAnchoVisible;
														// Total de alto necesario para los nodos visibles
		size_t										   _TotalAltoVisible;
														// Maximo de altura para un nodo
		size_t                                         _MaxAltoNodo;

														// Raiz que contiene todos los nodos
		DArbolEx_Nodo	                               _Raiz;
														// Nodo que tiene el foco del teclado
		DArbolEx_Nodo	                              *_NodoMarcado;
														// Nodo presionado
		DArbolEx_Nodo                                 *_NodoPresionado;
														// Nodo resaltado
		DArbolEx_Nodo                                 *_NodoResaltado;
														// Ultimo nodo resaltado
		DArbolEx_Nodo                                 *_UNodoResaltado;

														// Primer nodo visible en la página
		DArbolEx_Nodo                                 *_NodoPaginaInicio;
														// Ultimo nodo visible en la página
		DArbolEx_Nodo                                 *_NodoPaginaFin;
														// Diferencia de pixeles del nodo inicial y el principio de la página (o es 0 o es un valor negativo)
		int											   _NodoPaginaVDif;
		int											   _NodoPaginaHDif;

		DhWnd_Fuente			                       _Fuente;

		friend class DArbolEx_Nodo;
	};


}

//#include "DArbolEx.tpp"
