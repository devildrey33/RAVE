#pragma once

#include "RAVE.h"
#include "DBarraScrollEx.h"
#include "DArbolEx_Nodo.h"
#include "DListaIconos.h"

namespace DWL {

	#define DARBOLEX_MOSTRARDEBUG	FALSE

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

	/* Datos que devuelve un click / dobleclick */
	class DArbolEx_DatosClick {
	  public :
						DArbolEx_DatosClick() : X(0), Y(0), Boton(0), Nodo(NULL) { };
		  				DArbolEx_DatosClick(const int nX, const int nY, const int nBoton, DArbolEx_Nodo *nNodo) : X(nX), Y(nY), Boton(nBoton), Nodo(nNodo) { };
				       ~DArbolEx_DatosClick() { };
  		int				X;
		int				Y;
		int				Boton;
		DArbolEx_Nodo  *Nodo;
	};

	/* Comportamiento del ArbolEx */
/*	class DArbolEx_Comportamiento {
	  public : 
						DArbolEx_Comportamiento(void) : MultiSeleccion(TRUE), SubSeleccion(TRUE) { };
		               ~DArbolEx_Comportamiento(void) { };
		  BOOL			MultiSeleccion;
		  BOOL			SubSeleccion;
		  // labeledit
		  // drag & drop

	};*/

	class DDesplegableEx;


	/* Plantilla ArbolEx que se crea partiendo del tipo de nodo */
	class DArbolEx : public virtual DBarraScrollEx {
	  public:
														DArbolEx(void);
		                                               ~DArbolEx(void);

														// Crea el control ArbolEx
		HWND											CrearArbolEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

		void											Expandir(DArbolEx_Nodo *nNodo, const BOOL nExpandir);

														// Obtiene el nodo de la posición especificada (El máximo de hijos se obtiene con la función 'const size_t TotalHijos')
		inline DArbolEx_Nodo                           *Nodo(const size_t nPos) { return _Raiz.Hijo(nPos); };
														// Nodo raiz
		inline DArbolEx_Nodo                           *NodoRaiz(void)			{ return &_Raiz; }
														// Total de nodos
		inline const size_t								TotalNodos(void)		{ return _Raiz._Hijos.size(); }

														// Agrega un nodo personalizado (por defecto es del tipo DArbolEx_Nodo) para iconos de los recursos
		template <class TNodo = DArbolEx_Nodo> TNodo   *AgregarNodo(const TCHAR *nTexto, DArbolEx_Nodo *nPadre = NULL, const int nIcono = NULL, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN) {
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoRecursos(nIcono, DARBOLEX_TAMICONO, DARBOLEX_TAMICONO);
															TNodo *nNodo = new TNodo();
															_AgregarNodo(nNodo, nTexto, nPadre, TmpIco, nFuente, PosicionNodo);
															return nNodo;
														};

														// Agrega un nodo personalizado (por defecto es del tipo DArbolEx_Nodo) para iconos del sistema
		template <class TNodo = DArbolEx_Nodo> TNodo   *AgregarNodo(const TCHAR *nTexto, DArbolEx_Nodo *nPadre, const GUID &KnowFolderId, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN) {
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoKnownFolder(KnowFolderId, DARBOLEX_TAMICONO, DARBOLEX_TAMICONO);
															TNodo *nNodo = new TNodo();
															_AgregarNodo(nNodo, nTexto, nPadre, TmpIco, nFuente, PosicionNodo);
															return nNodo;
														};

														// Agrega un nodo personalizado (por defecto es del tipo DArbolEx_Nodo) para iconos especificos de un path
		template <class TNodo = DArbolEx_Nodo> TNodo   *AgregarNodo(const TCHAR *nTexto, DArbolEx_Nodo *nPadre, const wchar_t *PathIcono, const int nPosIcono = 0, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN) {
															DListaIconos_Icono *TmpIco = DListaIconos::AgregarIconoExterno(PathIcono, DARBOLEX_TAMICONO, DARBOLEX_TAMICONO, nPosIcono);
															TNodo *nNodo = new TNodo();
															_AgregarNodo(nNodo, nTexto, nPadre, TmpIco, nFuente, PosicionNodo);
															return nNodo;
														};

														// Primer nodo visible del arbol (devuelve _Raiz[0] o NULL)
		DArbolEx_Nodo								   *PrimerNodoVisible(void);
														// Ultimo nodo visible del arbol (devuelve el último nodo del arbol que su padre tenga el miembro _Expandido a TRUE)
		DArbolEx_Nodo								   *UltimoNodoVisible(void);
														// Eliminar Nodo
		void											EliminarNodo(DArbolEx_Nodo *nEliminar);
														// Elimina todos los nodos
		void											BorrarTodo(void);

		void											Pintar(HDC hDC);

														// Pintar Nodo
		void											PintarNodo(HDC hDC, RECT *Espacio, DArbolEx_Nodo *nNodo, const int PosH);

		void											Scrolls_EventoCambioPosicion(void);
														// Eventos virtuales
		virtual void          							Evento_MouseEntrando(void)																	{ };
		virtual void									Evento_MouseSaliendo(void)																	{ };
		virtual void									Evento_MouseMovimiento(const int cX, const int cY, const UINT Param)						{ };
		virtual void									Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param)		{ };
		virtual void									Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param)			{ };
		virtual void                                    Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey)		{ };
		virtual void									Evento_MouseDobleClick(const UINT Boton, const int cX, const int cY, const UINT Param)		{ };

		virtual void                                    Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params)		{ };
		virtual void                                    Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params)			{ };
		virtual void									Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param)					{ };

		virtual void                                    Evento_FocoObtenido(HWND hWndUltimoFoco)													{ };
		virtual void                                    Evento_FocoPerdido(HWND hWndNuevoFoco)														{ };
														// Al expandir / contraer un nodo
		virtual void									Evento_Nodo_Expandido(DWL::DArbolEx_Nodo *nNodo, const BOOL nExpandido)						{ };

		DArbolEx_Nodo                                  *HitTest(const int cX, const int cY, DArbolEx_ParteNodo &Parte);
		void											MostrarNodo(DArbolEx_Nodo *vNodo);
		const BOOL										ObtenerRectaNodo(DArbolEx_Nodo *rNodo, RECT &rRecta);

		LRESULT CALLBACK								GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);


		void											SeleccionarNodo(DArbolEx_Nodo *sNodo, const BOOL nSeleccionado);
														// Comportamiento del ArbolEx (Multiseleccion, subseleccion, drag&drop, etc..)
//		DArbolEx_Comportamiento							Comportamiento;
		BOOL											MultiSeleccion;
		BOOL											SubSeleccion;


		void											DesSeleccionarTodo(void);

														// Busca el nodo anterior
		DArbolEx_Nodo								   *BuscarNodoAnterior(DArbolEx_Nodo *nActual, const BOOL nVisible = FALSE);
														// Busca el nodo siguiente
		DArbolEx_Nodo								   *BuscarNodoSiguiente(DArbolEx_Nodo *nActual, const BOOL nVisible = FALSE, DArbolEx_Nodo *DentroDe = NULL);

	  protected:		
														// Valor que determina si hay que recalcular los tamaños antes de pintar
		BOOL                                           _CalcularValores;
														// Eventos internos
		void										   _Evento_Pintar(void);
		void										   _Evento_MouseMovimiento(const int cX, const int cY, const UINT Param);
		void										   _Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void										   _Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void                                           _Evento_MouseRueda(const short Delta, const int cX, const int cY, const UINT VirtKey);
		void										   _Evento_MouseDobleClick(const UINT Boton, const int cX, const int cY, const UINT Param);
		void										   _Evento_MouseSaliendo(void);

		void                                           _Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void                                           _Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void										   _Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param);

		void										   _Evento_FocoObtenido(HWND hWndUltimoFoco);
		void										   _Evento_FocoPerdido(HWND hWndNuevoFoco);

		void                                           _AplicarShift(const LONG nPosShift);

		void										   _CrearBufferNodo(const int nAncho, const int nAlto);
														// Buffer DC permanente para pintar UN nodo
		HDC                                            _BufferNodo;
		HBITMAP                                        _BufferNodoBmp;
		HBITMAP                                        _BufferNodoBmpViejo;

														// Teclas especiales
		void										   _Tecla_CursorArriba(void);
		void										   _Tecla_CursorAbajo(void);
		void										   _Tecla_Inicio(void);
		void										   _Tecla_Fin(void);
		void										   _Tecla_AvPag(void);
		void										   _Tecla_RePag(void);

	    void					  					   _CalcularNodosPagina(const size_t TamPagina);

		void										   _CalcularScrolls(void);

														// Agrega un nodo (se tiene que reservar memória en la variable nNodo antes de agregar. ej nNodo = new DArbolEx_Nodo)
//		DArbolEx_Nodo                                 *_AgregarNodo(DArbolEx_Nodo *nNodo, const TCHAR *nTexto, DArbolEx_Nodo *nPadre = NULL, const int nIcono = NULL, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN);
		DArbolEx_Nodo                                 *_AgregarNodo(DArbolEx_Nodo *nNodo, const TCHAR *nTexto, DArbolEx_Nodo *nPadre = NULL, DListaIconos_Icono *nIcono = NULL, DhWnd_Fuente *nFuente = NULL, const size_t PosicionNodo = DARBOLEX_POSICIONNODO_FIN);

														// Obtiene el espacio en pixeles que necesita todo el arbol tal y como están los nodos expandidos
		void									       _CalcularTotalEspacioVisible(void);
														// Total de ancho necesario para los nodos visibles
		size_t										   _TotalAnchoVisible;
														// Total de alto necesario para los nodos visibles
		size_t										   _TotalAltoVisible;
														// Maximo de altura para un nodo VISIBLE (para crear el BufferDC que se usa para pintar los nodos individualmente)
		size_t                                         _MaxAltoNodo;

														// Raiz que contiene todos los nodos
		DArbolEx_Nodo	                               _Raiz;
														// Nodo que tiene el foco del teclado
		DArbolEx_Nodo	                              *_NodoMarcado;
														// Nodo desde el que se ha presionado la tecla shift
		DArbolEx_Nodo							      *_NodoShift;
														// Posiciones que se ha desplazado el shift
		LONG                                           _PosShift;
														// Nodo presionado
		DArbolEx_Nodo                                 *_NodoPresionado;
		DArbolEx_ParteNodo                             _NodoPresionadoParte;
														// Nodo resaltado
		DArbolEx_Nodo                                 *_NodoResaltado;
		DArbolEx_ParteNodo                             _NodoResaltadoParte;
														// Ultimo nodo resaltado
		DArbolEx_Nodo                                 *_NodoUResaltado;
		DArbolEx_ParteNodo                             _NodoUResaltadoParte;

														// Primer nodo visible en la página
		DArbolEx_Nodo                                 *_NodoPaginaInicio;
														// Ultimo nodo visible en la página
		DArbolEx_Nodo                                 *_NodoPaginaFin;
														// Diferencia de pixeles del nodo inicial y el principio de la página (o es 0 o es un valor negativo)
		LONG										   _NodoPaginaVDif;
		LONG										   _NodoPaginaHDif;

		DhWnd_Fuente			                       _Fuente;

//		bool										   _Teclado[256];

		friend class DArbolEx_Nodo;

		friend class DDesplegableEx;
	};


}

//#include "DArbolEx.tpp"
