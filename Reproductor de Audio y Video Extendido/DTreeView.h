#ifndef DTREEVIEW_NODO_H
	#define DTREEVIEW_NODO_H
	
//	#include "DControlEstandard.h"
	#include <vector>
	#include <string>
	#include "DImageList.h"

	namespace DWL {


		class DTreeView;


		class DTreeView_Nodo {
		  public:
//			virtual const DEnum_Objeto	Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_TreeView_Nodo; };

										DTreeView_Nodo(void);
			virtual                    ~DTreeView_Nodo(void);

			const BOOL					AsignarTexto(std::wstring nTexto);
			const BOOL					AsignarTexto(const wchar_t *nTexto, size_t nTotalCaracteres = 0);
			const BOOL					ObtenerTexto(std::wstring &nTexto);

			const BOOL					Icono(const int nIcono);
			const int					Icono(void);

			DTreeView_Nodo			   *Padre(void);
			DTreeView_Nodo			   *Hijo(void);
			DTreeView_Nodo			   *Siguiente(void);
			DTreeView_Nodo			   *Anterior(void);
		protected:
			HTREEITEM				   _ID;
			DControl                  *_TV;

			DTreeView_Nodo            *_ObtenerLPARAM(HTREEITEM nItem);
			friend class DTreeView;
		};





		class DTreeView : public DControl {
		  public: /////////////////////////////// Miembros publicos


//			  virtual const DEnum_Objeto	Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_TreeView; };

											DTreeView(void);
			  virtual                      ~DTreeView(void);

											//! Función para crear el TreeView (CreateWindowEx).
											/*! Esta función se usa para crear un TreeView dinamicamente.
													\fn         HWND Crear(DBaseWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos = NULL);
													\param[in]  nPadre               Clase DBaseWnd que va a hacer de padre para el TreeView
													\param[in]  cX                   Coordenada X relativa a la ventana padre.
													\param[in]  cY                   Coordenada Y relativa a la ventana padre.
													\param[in]  cAncho               Ancho del TreeView en pixeles.
													\param[in]  cAlto                Altura del TreeView en pixeles
													\param[in]  cID                  ID para poder identificar el TreeView en sus eventos.
													\param[in]  nEstilos             Estilos estandar para el TreeView.
													\param[in]  nEstilosExtendidos   Estilos extendidos para el TreeView.
													\return     Devuelve el HWND del TreeView o NULL en caso de error.
											*/
			  HWND							Crear(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos = NULL);


										    //! Función para asignar esta clase a un TreeView de un dialogo.
										    /*! Esta función se usa para asignar esta clase a un TreeView de un dialogo.
												  \fn         HWND Asignar(DBaseWnd *Padre, const UINT cID);
												  \param[in]  nPadre      Clase DBaseWnd que va a hacer de padre para el TreeView
												  \param[in]  cID         ID para poder identificar el TreeView en sus eventos.
												  \return     Devuelve el HWND del TreeView o NULL en caso de error.
												  \remarks    Esta función solo debe utilizarse si tenemos un TreeView en un dialogo de los recursos.
										    */
			  HWND							Asignar(DhWnd *nPadre, const UINT cID);



			  DTreeView_Nodo               *AgregarNodo(DTreeView_Nodo *nNodo, DTreeView_Nodo *nPadre, const TCHAR *cTexto, const int cIko, DTreeView_Nodo *InsertarDespuesDe = NULL, const UINT Estado = NULL, const UINT MascaraEstado = NULL);
			  DTreeView_Nodo               *AgregarNodo(DTreeView_Nodo *nPadre, const TCHAR *cTexto, const int cIko, DTreeView_Nodo *InsertarDespuesDe = NULL, const UINT Estado = NULL, const UINT MascaraEstado = NULL);


			  void							BorrarNodo(DTreeView_Nodo *eNodo);

			  void							BorrarTodo(void);

											//
											//! Evento que es llamado cuando se expande / contrae un nodo del treeview
											//  virtual void TreeView_Evento_Nodo_Expandiendo(DTreeView_Nodo *nNodo);
			  virtual const BOOL			TreeView_Evento_Nodo_Expandiendo(DTreeView_Nodo *nNodo);

			  void							AsignarImageList(DImageList *nListaImagenes);

										    //! Nuevo WindowProcedure que se sobrepondra al WindowProcedure original del ListView.
										    /*! Esta función es la que re-emplaza el WindowProcedure del ListView para poder hacer cosas como la edicion, y el pintado personalizado.
												  \fn         virtual LRESULT CALLBACK GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam)
												  \param[in]  uMsg        Mensaje
												  \param[in]  wParam      Parametro wide
												  \param[in]  lParam      Parametro long
												  \return     Dependiendo del mensaje procesado devolvera una cosa u otra...
												  \remarks    Hacer un SendMessage al HWND de este ListView o utilizar esta función es exactamente lo mismo.
										    */
			  virtual LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
			  
			  const size_t					TotalNodos(void);

			  DTreeView_Nodo               *Nodo(size_t Pos);

			  DTreeView_Nodo               *PrimerNodo(void);

										    /* Función que devuelve el primer texto encontrado en el arbol igual que el parámetro buscar, o null si no lo encuentra
												  NOTA : la comparación se hace INSENSITIVE
												  Parámetros :
												  - Buscar    : Cadena a buscar
												  - Padre     : Nodo padre por donde empieza la busqueda, NULL si se busca desde la raíz (valor por defecto).
												  - Recursivo : Busqueda recursiva por los hijos (por defecto desactivada)
										    */

			  DTreeView_Nodo               *BuscarTexto(std::wstring &Buscar, DTreeView_Nodo *Padre = NULL, const BOOL Recursivo = FALSE);


											//! Función virtual que permite controlar cuando se hace click en el TreeView.
											/*! El objetivo de esta función virtual es poder captar cuando se hace click en el TreeView al heredar la clase.
													\fn         virtual LRESULT Evento_Mouse_Click(DTreeView_Nodo *cNodo, const UINT nBoton, const int cX, const int cY);
													\param[in]  cNodo       Nodo en el que se ha hecho click (puede ser NULL si no hay nodo).
													\param[in]  nBoton      Boton del mouse presionado (0 izquierdo, 1 derecho).
													\param[in]  cX          Coordenada X del mouse.
													\param[in]  cY          Coordenada Y del mouse.
													\return     Debes devolver 0.
											*/
			  virtual LRESULT               Evento_Mouse_Click(DTreeView_Nodo *cNodo, const UINT nBoton, const int cX, const int cY) { return 0; };

											//! Función virtual que permite controlar cuando se mueve el mouse en el TreeView.
											/*! El objetivo de esta función virtual es poder captar cuando se moeve el mouse en el TreeView al heredar la clase.
													\fn         virtual LRESULT Evento_Mouse_Movimiento(DTreeView_Nodo *mNodo, const int cX, const int cY);
													\param[in]  mNodo       Nodo que esta debajo del mouse (Puede ser NULL).
													\param[in]  cX          Coordenada X del mouse.
													\param[in]  cY          Coordenada Y del mouse.
													\return     Debes devolver 0.
											*/
			  virtual LRESULT               Evento_Mouse_Movimiento(DTreeView_Nodo *mNodo, const int cX, const int cY) { return 0; };			  

	/////////////////	
		protected: //
	/////////////////

			DTreeView_Nodo				  *_BuscarID(HTREEITEM bID);

//			DTreeView_Nodo				  *_HitTest(void);

			LRESULT						   _NotificacionClick(LPARAM lParam);

											// vector de nodos para hacer busquedas lineales mucho más rápido.
											// Los nodos se guardan en el LPARAM de cada HTREEITEM y tambien en este vector.
											// Unicamente guardo el puntero del Nodo por lo que hay que borrarlos al finalizar.
			std::vector<DTreeView_Nodo *>  _Nodos;

			friend class DTreeView_Nodo;

		};



		//! Clase que contiene los datos al hacer click o dobleclick
		class DTreeView_DatosClick {
		public: /// Miembros publicos
//			virtual const DEnum_Objeto	Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_TreeView_DatosClick; };

										//! Constructor
										/*! Constructor asignador.
												\fn			DTreeView_DatosClick(DTreeView_Nodo *nNodo, const int nBoton, const int nX, const int nY);
												\param[in]  nNodo			DTreeView_Nodo que contiene la información del nodo.
												\param[in]  nBoton          Boton del mouse.
												\param[in]  nX              Coordenada X.
												\param[in]  nY              Coordenada Y
												\return     No devuelve nada
										*/
										DTreeView_DatosClick(DTreeView_Nodo *nNodo, const int nBoton, const int nX, const int nY) : Nodo(nNodo), Boton(nBoton), X(nX), Y(nY) { };

										//
										//! Posición del item
			DTreeView_Nodo			   *Nodo;

										//
										//! Coordenada X
			int							X;

										//
										//! Coordenada Y
			int							Y;

										//
										//! Boton del mouse
			int							Boton;
		};								//
		//////////////////////////////////


	};

#endif