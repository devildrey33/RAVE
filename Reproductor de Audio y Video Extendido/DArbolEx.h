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

	enum DArbolEx_ParteNodo {
		DArbolEx_ParteNodo_Nada		= 0,
		DArbolEx_ParteNodo_Expansor = 1,
		DArbolEx_ParteNodo_Nodo		= 2,
		DArbolEx_ParteNodo_Icono	= 3,
		DArbolEx_ParteNodo_Texto	= 4
	};


	class DArbolEx_Comportamiento {
	  public : 
		  DArbolEx_Comportamiento(void) : MultiSeleccion(TRUE), SubSeleccion(TRUE) { };
		  ~DArbolEx_Comportamiento(void) { };
		  BOOL MultiSeleccion;
		  BOOL SubSeleccion;
		  // labeledit
		  // drag & drop

	};

	/* Plantilla ArbolEx que se crea partiendo del tipo de nodo */
	class DArbolEx : public virtual DBarraScrollEx {
	  public:
														DArbolEx(void);
		                                               ~DArbolEx(void);

														// Crea el control ArbolEx
		HWND											CrearArbolEx(DhWnd &nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

		void											Expandir(DArbolEx_Nodo *nNodo, const BOOL nExpandir);

														// Obtiene el nodo de la posici�n especificada (El m�ximo de hijos se obtiene con la funci�n 'const size_t TotalHijos')
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
		void											Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void											Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);

		void                                            Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void                                            Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void											Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param);

		DArbolEx_Nodo                                  *HitTest(const int cX, const int cY, DArbolEx_ParteNodo &Parte);
		void											HacerVisible(DArbolEx_Nodo *vNodo);
		const BOOL										ObtenerRectaNodo(DArbolEx_Nodo *rNodo, RECT &rRecta);

		LRESULT CALLBACK								GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);


		virtual const BOOL								Evento_MouseEntrando(void);
		virtual const BOOL								Evento_MouseSaliendo(void);

		void											SeleccionarNodo(DArbolEx_Nodo *sNodo);
														// Comportamiento del ArbolEx (Multiseleccion, subseleccion, drag&drop, etc..)
		DArbolEx_Comportamiento							Comportamiento;

	  protected:
  	    void										   _DesSeleccionarTodo(void);

		void										   _CrearBufferNodo(const int nAncho, const int nAlto);
														// Buffer DC permanente para pintar UN nodo
		HDC                                            _BufferNodo;
		HBITMAP                                        _BufferNodoBmp;
		HBITMAP                                        _BufferNodoBmpViejo;

														// Teclas especiales
		void										   _Tecla_CursorArriba(void);
		void										   _Tecla_CursorAbajo(void);

	    void					  					   _CalcularNodosPagina(const size_t TamPagina);

		void										   _CalcularScrolls(void);


														// Busca el nodo anterior
		DArbolEx_Nodo								  *_BuscarNodoAnterior(DArbolEx_Nodo *nActual, const BOOL nVisible = FALSE);
														// Busca el nodo siguiente
		DArbolEx_Nodo								  *_BuscarNodoSiguiente(DArbolEx_Nodo *nActual, const BOOL nVisible = FALSE);


														// Agrega un nodo (se tiene que reservar mem�ria en la variable nNodo antes de agregar. ej nNodo = new DArbolEx_Nodo)
		DArbolEx_Nodo                                 *_AgregarNodo(DArbolEx_Nodo *nNodo, const TCHAR *nTexto, DArbolEx_Nodo *nPadre = NULL, const int nIcono = NULL, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN);
														// Obtiene el espacio en pixeles que necesita todo el arbol tal y como est�n los nodos expandidos
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
		DArbolEx_ParteNodo                             _NodoPresionadoParte;
														// Nodo resaltado
		DArbolEx_Nodo                                 *_NodoResaltado;
		DArbolEx_ParteNodo                             _NodoResaltadoParte;
														// Ultimo nodo resaltado
		DArbolEx_Nodo                                 *_NodoUResaltado;
		DArbolEx_ParteNodo                             _NodoUResaltadoParte;

														// Primer nodo visible en la p�gina
		DArbolEx_Nodo                                 *_NodoPaginaInicio;
														// Ultimo nodo visible en la p�gina
		DArbolEx_Nodo                                 *_NodoPaginaFin;
														// Diferencia de pixeles del nodo inicial y el principio de la p�gina (o es 0 o es un valor negativo)
		int											   _NodoPaginaVDif;
		int											   _NodoPaginaHDif;

		DhWnd_Fuente			                       _Fuente;

		friend class DArbolEx_Nodo;
	};


}

//#include "DArbolEx.tpp"
