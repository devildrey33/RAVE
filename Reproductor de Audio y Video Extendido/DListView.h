/*! \file                       DListView.h
    \brief                      Archivo que contiene las definiciones para las clases necesarias para utilizar el ListView.

    \details
                                El listview es un control de windows que permite mostrar listas con datos que pueden estar ordenados de muchas formas. El ejemplo mas claro de listview lo podemos ver en el explorador de windows donde vemos carpetas y archivos a los que podemos acceder mediante el raton.\n\n
                                A lo largo de los años y a medida que han salido nuevas versiones de windows este control ha sufrido cambios y ha sido extendido con nuevas funciones, por lo que es posible que estas clases no engloben todas estas funciones pero si la gran mayoria.
                                Esta clase incluye entre otras capacidades basicas, la posibilidad de editar los items de 7 formas distintas 3 con edición normal (texto, enteros, decimales) y cuatro para editarlos con una combobox (estatica, texto, enteros, decimales). \n\n
                                La siguiente imagen hace referencia a una edición con un combo box estatico (el cual tiene valores fijos entre los que se pueden elegir) :\n
                                <img src='LV_COMBO1.png' alt='ListView edición' />

    \section utilizando         Utilizando el ListView :
                                
                                Para empezar a utilizar un ListView debes seguir los siguientes pasos :
                                <ul>
                                    <li>
                                        <b>Creación :</b>
                                        Para crear un ListView de forma dinámica debes utilizar la función Crear :
\code
#define ID_LISTVIEW1 1000
DListView MiListView;
// Creación de un ListView dinámico con el modo de vista detalles.
MiListView.Crear(Ventana, 10, 10, 200, 200, ID_LISTVIEW1, WS_CHILD | WS_VISIBLE | LVS_REPORT);
\endcode
                                        Tambien puedes asignar la clase DListView a un ListView previamente creado en el editor de recursos de un dialogo :
\code
DListView MiListView;
MiListView.Asignar(Dialogo, IDC_LISTVIEW1);
\endcode    \n
                                    </li>
                                    <li>
                                       <b>Preparación : </b>
                                        Una vez creado el listview para poder mostrar items es necesario crear como mínimo una columna.\n 
                                        <b>NOTA :</b> Para los modos de vista detalles y mosaico puedes crear más de una columna. 
\code
// Columna normal sin edición por defecto alineada a la izquierda
MiListView.AgregarColumna(TEXT("Texto para la columna"), 300);
// Columna normal sin edición alineada a la derecha
MiListView.AgregarColumna(TEXT("Columna alineada a la derecha"), 300, DEnum_ListView_AlineacionTexto_Derecha);
// Columna normal con edición de texto (solo valores enteros)
MiListView.AgregarColumna(TEXT("Columna valores enteros"), 300, DEnum_ListView_AlineacionTexto_Izquierda, DEnum_ListView_TipoEdicion_EditEntero);
\endcode    
                                        Si queremos que el ListView muestre iconos en sus items debemos crear un ImageList del tamaño adecuado, agregarle iconos e enlazarlo con el ListView.
\code
DImageList MiImageList;
// ImageList con iconos de 16x16 ideal para el modo detalles
MiImageList.Crear(16, 16); 
// Agregamos uno o más iconos al ImageList ....
MiImageList.AgregarIconoRecursos(IDI_LISTVIEW);
// Asignamos el ImageList de 16x16 al ListView
MiListView.AsignarImageList(&MiImageList, DEnum_ListView_ImageList_Peque);
\endcode    \n
                                    </li>
                                    <li>
                                        <b>Insertar items :</b>
                                        Para insertar items dentro del ListView debes tener en cuenta cuantas columnas has creado anteriormente, y utilizar la función AgregarItem con tantos parámetros opcionales como columnas -1 tengas :
\code
// Agregando un item normal, el primer valor es un texto alineado a la izquierda, el segundo se alinea a la derecha, y el tercero es un valor entero.
// El primer parametro apunta al primer icono del imagelist, el segundo y tercer parametro son la posición y el grupo del item.
MiListView.AgregarItem(0, -1, -1, TEXT("Item alineado a la izquierda"), TEXT("Item alineado a la derecha", TEXT("3333"));
\endcode
                                    </li>
                                </ul>


    \section notificaciones     Notificaciones disponibles :
                                El ListView manda una serie de notificaciones a su ventana padre para indicar distintos sucesos.\n
                                Tanto las ventanas padre como los dialogos heredan de la clase DWL::DEventosPadre, que contiene una serie de funciones virtuales que empiezan por "Evento" que estan diseñadas para contestar a notificaciones de controles hijos.\n
                                Esta libreria soporta las siguientes notificaciones :
                                <ul>
                                    <li>
                                        <b>LVN_BEGINDRAG</b> Informa de que se ha empezado una operación de arrastrar y soltar interna con el botón izquierdo del mouse.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_ArrastrarSoltar_Empezar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_BEGINLABELEDIT</b> Notificación desactivada, si deseas recibir cuando empieza se termina o se cancela una notificacion debes utilizar las siguientes funciones :\n
                                        Empezar edición DWL::DEventosPadre::Evento_ListView_Edicion_Empezar().\n
                                        Terminar edición DWL::DEventosPadre::Evento_ListView_Edicion_Terminar().\n
                                        Cancelar edición DWL::DEventosPadre::Evento_ListView_Edicion_Cancelar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_BEGINRDRAG</b> Informa de que se ha empezado una operación de arrastrar y soltar interna con el botón derecho del mouse.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_ArrastrarSoltar_Empezar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_BEGINSCROLL</b> Informa de que se ha empezado a arrastrar el scroll en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Scroll_Empezar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_COLUMNCLICK</b> Informa de que el usuario ha hecho click en una cabecera de las columnas del ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Columna_Click().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_DELETEALLITEMS</b> Informa de que se van a borrar todos los items en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Item_BorrarTodos().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_DELETEITEM</b> Informa de que se va a borrar un item en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Item_Borrar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_ENDLABELEDIT</b> Notificación desactivada.\n
                                        Para determinar cuando se termina de editar un item utiliza la función virtual DWL::DEventosPadre::Evento_ListView_Edicion_Terminar().\n
                                        Para determinar cuando se cancela la edición de un item utiliza la función virtual DWL::DEventosPadre::Evento_ListView_Edicion_Cancelar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_ENDSCROLL</b> Informa de que se ha terminado de arrastrar el scroll en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Scroll_Terminar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_HOTTRACK</b> Informa de que se esta moviendo el mouse por encima de un item en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Mouse_Movimiento().\n
                                        <b>NOTA :</b> Solo se recibira esta notificación cuando el ListView tenga el estilo extendido <i>LVS_EX_TRACKSELECT</i> activado.\n\n
                                    </li>
                                    <li>
                                        <b>LVN_INSERTITEM</b> Informa de que se ha agregado un item. en el ListView\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Item_Agregar().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_ITEMACTIVATE</b> Informa de que se ha activado un item en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Item_Activado().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_ITEMCHANGED</b> Informa de que un item ha cambiado en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Item_Cambiado().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_ITEMCHANGING</b> Informa de que un item va a cambiar en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Item_Cambiando().\n\n
                                    </li>
                                    <li>
                                        <b>LVN_KEYDOWN</b> Informa de que se ha presionado una tecla en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_TeclaPresionada().\n\n
                                    </li>
                                    <li>
                                        <b>NM_CLICK</b> Informa de que se ha hecho click con el boton izquierdo del mouse encima del ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Mouse_Click().\n\n
                                    </li>
                                    <li>
                                        <b>NM_DBLCLK</b> Informa de que se ha hecho doble click con el boton izquierdo del mouse encima del ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Mouse_DobleClick().\n\n
                                    </li>
                                    <li>
                                        <b>NM_SETFOCUS</b> Informa de que se ha recibido el foco del teclado en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Foco_Obtenido().\n\n
                                    </li>
                                    <li>
                                        <b>NM_KILLFOCUS</b> Informa de que se ha perdido el foco del teclado en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Foco_Perdido().\n\n
                                    </li>
                                    <li>
                                        <b>NM_RELEASECAPTURE</b> Informa de que se ha terminado la captura de eventos en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_TerminarCaptura().\n\n
                                    </li>
                                    <li>
                                        <b>NM_RETURN</b> Informa de que se ha pulsado la tecla intro en el ListView.\n
                                        Para contestar a esta notificación puedes utilizar la función virtual DWL::DEventosPadre::Evento_ListView_Teclado_Intro().\n
                                    </li>
                                </ul>
                                Las notificaciones del ListView que no esten nombradas aquí las puedes obtener mediante la función virtual DWL::DEventosPadre::Evento_Notificacion().\n
                                Para mas información sobre las notificaciones del ListView puedes consultar el siguente enlace : <a href='http://msdn.microsoft.com/en-us/library/bb774737(v=vs.85).aspx' target="_blank">MSDN ListView</a>.

    \section vistas             Tipos de vista disponibles :
                                Existen 5 tipos de vista para el ListView que pueden ser cambiadas dinamicamente al igual que puedes cambiar la vista en el explorador de windows para ver los archivos de distintas formas :\n
                                <ul>
                                    <li>
                                        <b>Vista iconos</b> o <i>icon</i> en ingles.\n
                                        Este tipo de vista admite items con texto y icono. El icono puede tener un tamaño de 32x32, 64x64, 96x96 y 128x128.
                                        <img src='LV_ICON.png' alt='ListView icon' />\n
                                    </li>
                                    <li>
                                        <b>Vista iconos pequeños</b> o <i>small icon</i> en ingles.\n
                                        Este tipo de vista admite items con texto y icono. El icono solo puede tener el tamaño de 16x16. Diseñada para arrastrar y soltar.
                                        <img src='LV_SMALLICON.png' alt='ListView small icon' />\n
                                    </li>
                                    <li>
                                        <b>Vista lista</b> o <i>list</i> en ingles.\n
                                        Este tipo de vista admite items con texto y icono. Es muy similar a la vista iconos pequeños, el icono solo puede tener el tamaño de 16x16, pero no se puede arrastrar los items.
                                        <img src='LV_LIST.png' alt='ListView list' />\n
                                    </li>
                                    <li>
                                        <b>Vista detalles</b> o <i>report</i> en ingles.\n
                                        Este tipo de vista permite crear varias columnas y añadir varias filas de datos, de forma que cada fila tenga tantos valores como columnas. \n
                                        Los iconos para esta vista estan limitados a 16x16.\n
                                        <img src='LV_REPORT.png' alt='ListView report' />\n
                                    </li>
                                    <li>
                                        <b>Vista mosaico</b> o <i>tile</i> en ingles.\n
                                        Este tipo de vista es la más reciente creada por microsoft y se implemento a partir de windows xp. \n
                                        Consiste en un icono de 32x32 o mas grande que tiene una o varias lineas de texto. Cada linea de texto vendria a ser el texto que sale en columnas en el modo detalles.
                                        <img src='LV_TILE.png' alt='ListView tile' />
                                    </li>
                                </ul>
                                Para más información sobre de las capacidades del control ListView te recomiendo la siguiente pagina de la MSDN : <a href='http://msdn.microsoft.com/en-us/library/bb774735(v=vs.85).aspx' target='_blank'>About ListView Controls</a>.\n\n
                                
    \section personalizacion    Personalizando el ListView :
                                Otra de las ventajas de esta clase es que puede ser personalizada para pintar todo tipo de graficos en vez de utilizar texto. Para ello dispone de varias funciones virtuales que puedes re-emplazar para hacer lo que necesites :
                                <ul>
                                    <li>
                                        DWL::DListView::Evento_PintarItem(), Esta función esta diseñada para poder pintar todo tipo de graficos en cualquier item/subitem del ListView.
                                    </li>
                                    <li>
                                        DWL::DListView::Evento_Mouse_Click(), Esta función es invocada cada vez que se hace click en el ListView.
                                    </li>
                                    <li>
                                        DWL::DListView::Evento_Mouse_Movimiento(), Esta función es invocada cada vez que se mueve el mouse en el ListView.
                                    </li>
                                    <li>
                                        DWL::DListView::Evento_Mouse_Saliendo(), Esta función es invocada cuando el mouse sale del area del ListView.\n\n
                                        <img src='LV_CUSTOMDRAW.png' alt='ListView custom draw' />
                                    </li>
                                </ul>

                                Por ultimo hay que mencionar que dispones de un ejemplo donde se muestran varias de las capacidades estandar del control, además de las capacidades añadidas : edición de items/subitems por el usuario, colores del texto independientes para cada item/subitem, y personalización del pintado de items/subitems.\n
                                Para ver el ejemplo, abre el proyecto <i>"DWL 2012 Ejemplos.sln"</i>, y establece como proyecto de inicio el proyecto <i>"Ejemplo ListView"</i>.

    \subpage NotasListView     "Notas de la versión".
    \todo   Indagar en el funcionamiento de los grupos y añadir mas funcionabilidad para estos.
    \todo   Añadir algunos throw para casos excepcionales como por ejemplo que no se cree el control, o que vayas a tocar un indice de item / columna que no exista.
    \todo   Probar fuentes distintas en subitems 0,1,2.

                                <hr>
    \author                     devildrey33
    \date                       Creado el [04/05/2002], ultima modificación el [04/02/2012]
	\remarks	                Archivo creado por devildrey33 para http://www.devildrey33.es														                                \n
				                Este archivo es parte de la DWL (Devildrey33 Windows Library) y se distribuye bajo la licencia GPL, para mas información consulta estos enlaces :	\n
				                 - http://www.gnu.org/licenses/gpl.html     (Ingles, documento oficial)																	            \n
				                 - http://www.viti.es/gnu/licenses/gpl.html (Castellano, traducción no oficial)															            \n
				                 - http://www.softcatala.cat/wiki/GPL3		(Catalá, traducción no oficial)																            \n

 */
#ifndef DLISTVIEW_H
    #define DLISTVIEW_H
    
    #include <vector>
	#include <string>
    #include "DhWnd.h"

    //! Espacio de nombres DWL
    namespace DWL {

        //! Tipos de edicion para los items / subitems
        enum DEnum_ListView_TipoEdicion {
            //! El item / subitem no puede ser editado por el usuario (PREDETERMINADO).
            DEnum_ListView_TipoEdicion_SinEdicion,
            //! El item / subitem mostrara un editbox para ser editado al pincharlo el cual puede contener texto.
            DEnum_ListView_TipoEdicion_EditTexto,           
            //! El item / subitem mostrara un editbox para ser editado al pincharlo el cual solo puede contener valores enteros.
            DEnum_ListView_TipoEdicion_EditEntero,          
            //! El item / subitem mostrara un editbox para ser editado al pincharlo el cual solo puede contener valores con decimales.
            DEnum_ListView_TipoEdicion_EditDecimal,         
            //! El item / subitem mostrara un combobox con un editbox para ser editado al pincharlo.
            DEnum_ListView_TipoEdicion_ComboEditTexto,      
            //! El item / subitem mostrara un combobox con un editbox para ser editado al pincharlo el cual solo puede contener valores enteros.
            DEnum_ListView_TipoEdicion_ComboEditEntero,     
            //! El item / subitem mostrara un combobox con un editbox para ser editado al pincharlo el cual solo puede contener valores con decimales.
            DEnum_ListView_TipoEdicion_ComboEditDecimal,   
             //! El item / subitem mostrara un combobox con el texto estatico para ser editado al pincharlo.
            DEnum_ListView_TipoEdicion_ComboStatic         
        };

        //! Alineación de las columnas
        enum DEnum_ListView_AlineacionTexto {
            //! Texto alineado a la izquierda
            DEnum_ListView_AlineacionTexto_Izquierda = LVCFMT_LEFT,     
            //! Texto alineado a la derecha
            DEnum_ListView_AlineacionTexto_Derecha   = LVCFMT_RIGHT,    
            //! Texto centrado
            DEnum_ListView_AlineacionTexto_Centrado  = LVCFMT_CENTER    
        };

        //! Tipos de vista en el ListView
        enum DEnum_ListView_Vista {
            //! Modo iconos 32x32, 64x64, 96x96, 128x128, etc...
            DEnum_ListView_Vista_Iconos         = LV_VIEW_ICON,         
            //! Modo columnas o report con iconos 16x16
            DEnum_ListView_Vista_Detalles       = LV_VIEW_DETAILS,      
            //! Modo iconos 16x16
            DEnum_ListView_Vista_IconosPeque    = LV_VIEW_SMALLICON,    
            //! Modo lista iconos 16x16
            DEnum_ListView_Vista_Lista          = LV_VIEW_LIST,         
            //! Modo mosaico iconos 32x32, 64x64, 96x96, 128x128, etc... (Win XP o superior)
            DEnum_ListView_Vista_Mosaico        = LV_VIEW_TILE          
        };

        //! Tipos de ImageList
        enum DEnum_ListView_ImageList {
            //! Iconos 32x32, 64x64, 96x96, 128x128, etc...
            DEnum_ListView_ImageList_Normal         = LVSIL_NORMAL,     
            //! Iconos 16x16
            DEnum_ListView_ImageList_Peque          = LVSIL_SMALL,      
            DEnum_ListView_ImageList_Estado         = LVSIL_STATE,
            DEnum_ListView_ImageList_GrupoCabecera  = LVSIL_GROUPHEADER
        };

        //! Clase que contiene una lista de imagenes para el ListView
        class DImageList;

        //! Clase para controlar un ListView de windows
		class DListView;

        //! Clase para controlar una columna del ListView de windows
		class DListView_Columna;

        //! Clase que contiene los datos para un SubItem
        /*! En esencia esta clase se ha diseñado para contener datos especificos de un subitem como son su fuente, su color de texto, su color de texto seleccionado, y su tipo de edición.
            \sa Cabecera DListView.h, clase DListView_Item, clase DListView_Columna, clase DListView, clase DImageList.
        */
		class DListView_SubItem {
          public: ////////////////////////
//			virtual const DEnum_Objeto	Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_ListView_SubItem; };

	                                    //! Constructor
                                        /*! Constructor asignador.
	                                            \fn			DListView_SubItem(const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);
                                                \param[in]  nTipoEdicion    Tipo de edición que se realizara al hacer click en este subitem. (Por defecto no hay edición)
                                                \return     No devuelve nada
                                        */
                                        DListView_SubItem(const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion) : ColorTexto(NULL), ColorTextoSeleccionado(NULL), Fuente(NULL), TipoEdicion(nTipoEdicion) { };

	                                    //! Destructor
                                        /*! Destructor.
	                                            \fn		   ~DListView_SubItem(void);
                                                \return     No devuelve nada
                                                \remarks    Este destructor se encarga de eliminar los colores que se puedan haber modificado.
                                        */
                                       ~DListView_SubItem(void) { 
                                           if (ColorTexto != NULL) delete ColorTexto;
                                           if (ColorTextoSeleccionado != NULL) delete ColorTextoSeleccionado;
                                        };

	                                    //! Constructor
                                        /*! Constructor copia.
	                                            \fn			DListView_SubItem(const DListView_SubItem &nSubItem);
                                                \param[in]  nSubItem    Clase DListView_SubItem a copiar.
                                                \return     No devuelve nada
                                        */
                                        DListView_SubItem(const DListView_SubItem &nSubItem) : ColorTexto(nSubItem.ColorTexto), ColorTextoSeleccionado(nSubItem.ColorTextoSeleccionado), Fuente(nSubItem.Fuente), TipoEdicion(nSubItem.TipoEdicion) { }

                                        // 
                                        //! Color del texto para el subitem
            COLORREF                   *ColorTexto;

                                        // 
                                        //! Color del texto seleccionado para el subitem
            COLORREF                   *ColorTextoSeleccionado;

                                        // 
                                        //! Fuente para el subitem
            HFONT                       Fuente;

                                        // 
                                        //! Tipo de edición para el subitem
            DEnum_ListView_TipoEdicion  TipoEdicion;
        };                              //
        //////////////////////////////////


		//! Clase para controlar un item del ListView de windows
        /*! En esencia esta clase se ha diseñado para poder manipular los datos de un item dentro del ListView
            \sa Cabecera DListView.h, clase DListView_SubItem, clase DListView_Columna, clase DListView, clase DImageList.
        */
		class DListView_Item {
		  public : /////////////////////////////// Miembros publicos
//			virtual const DEnum_Objeto			Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_ListView_Item; };

	                                            //! Constructor
                                                /*! Constructor por defecto.
	                                                    \fn			DListView_Item(void);
                                                        \return     No devuelve nada
                                                */
								                DListView_Item(void);

	                                            //! Constructor
                                                /*! Constructor que asigna el padre.
	                                                    \fn			DListView_Item(DListView *LV);
                                                        \return     No devuelve nada
                                                */
								                DListView_Item(DListView *LV);

                                                //! Función para asignar el texto a un item / subitem.
                                                /*! Esta función se usa para modificar el texto de un item o de un subitem.
                                                        \fn         void AsignarTexto(const UINT nSubItem, const TCHAR *nTexto);
                                                        \param[in]  nSubItem    Posición del subitem a editar, 0 para editar el texto del item.
                                                        \param[in]  nTexto     Nuevo texto que adoptara el Item/SubItem.
                                                        \return     No devuelve nada.
                                                        \remarks    El texto asignado esta limitado a 256 caracteres.
                                                */
		    void                                AsignarTexto(const UINT nSubItem, const TCHAR *nTexto);

                                                //! Función para obtener el texto a un item / subitem.
                                                /*! Esta función se usa para obtener el texto de un item o de un subitem.
                                                        \fn         void ObtenerTexto(const UINT nSubItem, std::wstring &nTexto);
                                                        \param[in]  nSubItem   Posición del subitem a obtener, 0 para obtener el texto del item.
                                                        \param[out] nTexto     Texto actual del item/subitem especificado
                                                        \return     No devuelve nada.
                                                        \remarks    El texto obtenido esta limitado a 256 caracteres.
                                                */
			void                                ObtenerTexto(const UINT nSubItem, std::wstring &nTexto);

                                                //! Función para marcar/desmarcar el checkbox de un item.
                                                /*! Esta función se usa para marcar/desmarcar el checkbox de un item.
                                                        \fn         void Marcar(const BOOL nMarcar);
                                                        \param[in]  nMarcar   TRUE para marcar el CheckBox, FALSE para desmarcarlo.
                                                        \return     No devuelve nada.
                                                        \remarks    Solo funciona con el estilo extendido LVS_EX_CHECKBOXES.
                                                */
            void                                Marcar(const BOOL nMarcar);

                                                //! Función para obtener si el checkbox de un item esta marcado o no.
                                                /*! Esta función se usa para saber si el checkbox de un item esta marcado o no.
                                                        \fn         void Marcar(const BOOL nMarcar);
                                                        \return     Devuelve TRUE si el CheckBox esta marcado, FALSE en caso contrario.
                                                        \remarks    Solo funciona con el estilo extendido LVS_EX_CHECKBOXES.
                                                */
            const BOOL                          Marcado(void);

                                                //! Función para asignar el icono para el item.
                                                /*! Esta función se usa para asignar un icono del imagelist a este item.
                                                        \fn         void Icono(const int nIcono);
                                                        \param[in]  nIcono   Posicion del icono dentro del imagelist.
                                                        \return     No devuelve nada.
                                                        \remarks    Solo funciona si se ha asignado un imagelist con el tamaño adecuado al ListView.
                                                */
		    void                                Icono(const int nIcono);

                                                //! Función para obtener el icono para el item.
                                                /*! Esta función se usa para obtener el icono del imagelist que lleva este item.
                                                        \fn         const int Icono(void);
                                                        \return     Devuelve la posición del icono dentro del imagelist.
                                                        \remarks    Solo funciona si se ha asignado un imagelist con el tamaño adecuado al ListView.
                                                */
		    const int                           Icono(void);

                                                //! Función para asignar una fuente al item y todos sus subitems.
                                                /*! Esta función se usa para asignar una fuente a este item y todos sus subitems.
                                                        \fn         void Fuente(HFONT nFuente);
                                                        \return     No devuelve nada
                                                        \remarks    Si quieres especificar la fuente de un SubItem en concreto puedes utilizar la función FuenteSubItem(). No borres la fuente hasta que no sea utilizada por el ListView.
                                                */
		    void                                Fuente(HFONT nFuente);

                                                //! Función que devuelve la fuente que utiliza el item/subitem especificado.
                                                /*! Esta función se usa para obtener la fuente que utiliza el item/subitem especificado.
                                                        \fn         HFONT FuenteSubItem(const UINT nSubItem);
                                                        \param[in]  nSubItem   Posición del subitem del que se quiere obtener la fuente. 0 para obtener la fuente del item.
                                                        \return     Devuelve la fuente especificada.
                                                        \remarks    Ni se te ocurra borrar la fuente mientras se esta utilizando por el ListView.....
                                                */
		    HFONT                               FuenteSubItem(const UINT nSubItem);

                                                //! Función para asignar una fuente al item/subitem especificado.
                                                /*! Esta función se usa para asignar una fuente a este item y todos sus subitems.
                                                        \fn         void FuenteSubItem(const UINT nSubItem, HFONT nFuente);
                                                        \param[in]  nSubItem   Posición del subitem en el que se quiere asignar la fuente. 0 para asignar la fuente del item.
                                                        \param[in]  nFuente    Fuente que utilizara el item/subitem especificado.
                                                        \return     No devuelve nada
                                                        \remarks    No borres la fuente hasta que no sea utilizada por el ListView.
                                                */
		    void                                FuenteSubItem(const UINT nSubItem, HFONT nFuente);

                                                //! Función para asignar el color del texto al item y todos sus subitems.
                                                /*! Esta función se usa para asignar el color del texto a este item y todos sus subitems.
                                                        \fn         void ColorTexto(COLORREF nColor);
                                                        \param[in]  nColor   Color que se utilizara para mostrar este item y sus subitems
                                                        \return     No devuelve nada
                                                        \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color para un subitem especifico puedes utilizar la función ColorTextoSubItem().
                                                */
		    void                                ColorTexto(COLORREF nColor);

                                                //! Función para asignar el color del texto al item/subitem especificado.
                                                /*! Esta función se usa para asignar el color del texto al item/subitem especificado.
                                                        \fn         void ColorTextoSubItem(const UINT nSubItem, COLORREF nColor);
                                                        \param[in]  nSubItem   Posición del item / subitem al que le quieres cambiar el color del texto.
                                                        \param[in]  nColor     Color que se utilizara para mostrar el item/subitem especificado.
                                                        \return     No devuelve nada
                                                        \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color todos los subitems puedes utilizar la función ColorTexto().
                                                */
		    void                                ColorTextoSubItem(const UINT nSubItem, COLORREF nColor);

                                                //! Función que devuelve el color del texto para el item/subitem especificado.
                                                /*! Esta función se usa para obtener el color del texto que utiliza el item/subitem especificado.
                                                        \fn         HFONT ColorTextoSubItem(const UINT nSubItem);
                                                        \param[in]  nSubItem   Posición del subitem del que se quiere obtener el color. 0 para obtener el color del item.
                                                        \return     Devuelve el color especificado.
                                                        \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles.
                                                */
		    COLORREF                            ColorTextoSubItem(const UINT nSubItem);

                                                //! Función para asignar el color del texto seleccionado al item y todos sus subitems.
                                                /*! Esta función se usa para asignar el color del texto seleccionado a este item y todos sus subitems.
                                                        \fn         void ColorTextoSeleccionado(COLORREF nColor);
                                                        \param[in]  nColor   Color que se utilizara para mostrar el texto seleccionado de este item y sus subitems
                                                        \return     No devuelve nada
                                                        \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color para un subitem especifico puedes utilizar la función ColorFondoSubItem().
                                                */
		    void                                ColorTextoSeleccionado(COLORREF nColor);

                                                //! Función para asignar el color del texto seleccionado al item/subitem especificado.
                                                /*! Esta función se usa para asignar el texto seleccionado del fondo al item/subitem especificado.
                                                        \fn         void ColorTextoSeleccionadoSubItem(const UINT nSubItem, COLORREF nColor);
                                                        \param[in]  nSubItem   Posición del item / subitem al que le quieres cambiar el color del texto seleccionado.
                                                        \param[in]  nColor     Color que se utilizara para mostrar el item/subitem especificado.
                                                        \return     No devuelve nada
                                                        \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles. Si quieres asignar el color todos los subitems puedes utilizar la función ColorFondo().
                                                */
		    void                                ColorTextoSeleccionadoSubItem(const UINT nSubItem, COLORREF nColor);

                                                //! Función que devuelve el color del texto seleccionado para el item/subitem especificado.
                                                /*! Esta función se usa para obtener el color del texto seleccionado que utiliza el item/subitem especificado.
                                                        \fn         HFONT ColorTextoSeleccionadoSubItem(const UINT nSubItem);
                                                        \param[in]  nSubItem   Posición del subitem del que se quiere obtener el color. 0 para obtener el color del item.
                                                        \return     Devuelve el color especificado.
                                                        \remarks    Los colores solo funcionan en modo DEnum_ListView_Vista_Detalles.
                                                */
		    COLORREF                            ColorTextoSeleccionadoSubItem(const UINT nSubItem);

                                                //! Función que asigna el estado de selección para este item.
                                                /*! Esta función se usa para asignar el estado de selección para este item y todos sus subitems.
                                                        \fn         void Seleccionado(const BOOL Sel);
                                                        \param[in]  Sel   TRUE para seleccionar el item, FALSE para des-seleccionarlo.
                                                        \return     No devuelve nada.
                                                */
		    void                                Seleccionado(const BOOL Sel);

                                                //! Función que obtiene el estado de selección para este item.
                                                /*! Esta función se usa para obtener el estado de selección para este item y todos sus subitems.
                                                        \fn         const BOOL Seleccionado(void);
                                                        \return     Devuelve TRUE si el item esta seleccionado, FALSE en caso contrario.
                                                */
		    const BOOL                          Seleccionado(void);

                                                //! Función que hace visible dentro del control este item.
                                                /*! Esta función se usa para mostrar este item en el rectangulo visible del control.
                                                        \fn         void HacerVisible(void);
                                                        \return     No devuelve nada.
                                                */
		    void                                HacerVisible(void);

                                                //! Función para asignar el tipo de edición que se utilizara en el item/subitem especificado.
                                                /*! Esta función se usa para asignar el tipo de edición que se utilizara en el item/subitem especificado.
                                                        \fn         void TipoEdicion(const UINT nSubItem, const DEnum_ListView_TipoEdicion nTipoEdicion);
                                                        \param[in]  nSubItem       Posición del item/subitem al que queremos asignar el tipo de edición.
                                                        \param[in]  nTipoEdicion   Tipo de edición que utilizara el item/subitem, que puede ser : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, DEnum_ListView_TipoEdicion_ComboStatic.
                                                        \return     No devuelve nada.
                                                */
            void                                TipoEdicion(const UINT nSubItem, const DEnum_ListView_TipoEdicion nTipoEdicion);

                                                //! Función para obtener el tipo de edición que se utilizara en el item/subitem especificado.
                                                /*! Esta función se usa para obtener el tipo de edición que se utilizara en el item/subitem especificado.
                                                        \fn         const DEnum_ListView_TipoEdicion    TipoEdicion(const UINT nSubItem);
                                                        \param[in]  nSubItem       Posición del item/subitem al que queremos asignar el tipo de edición.
                                                        \return     Puede devolver uno de los siguientes valores : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, DEnum_ListView_TipoEdicion_ComboStatic.
                                                */
            const DEnum_ListView_TipoEdicion    TipoEdicion(const UINT nSubItem);
		  protected : //////////////////////////// Miembros privados

                                                //! Función que asigna el ListView padre para este item.
                                                /*! Esta función se usa para asignar el ListView padre para este item.
                                                        \fn         void HacerVisible(void);
                                                        \return     No devuelve nada.
                                                        \remarks    Esta funcion es de uso interno.
                                                */
		    void                               _AsignarNuevo(DListView *LV);

                                                //
                                                //! Vector para los datos de los SubItems
            std::vector<DListView_SubItem>     _SubItems;

							                    //
							                    //! Puntero a la clase DListView padre de este Item
		    DListView		                  *_LV;

								                //
								                //! Esta clase es amiga de la DListView
		    friend class		                DListView;
        };                                      //
        //////////////////////////////////////////


		//! Clase para controlar una columna del ListView de windows
        /*! En esencia esta clase se ha diseñado para manipular los datos de una columna dentro de un ListView.
            \sa Cabecera DListView.h, clase DListView_Item, clase DListView_SubItem, clase DListView, clase DImageList.
        */
		class DListView_Columna {
		  public : /////////////////////////////////// Miembros publicos
//			  virtual const DEnum_Objeto			Objeto_Tipo(void) { return DEnum_Objeto_ListView_Columna; };

	                                                //! Constructor
                                                    /*! Constructor por defecto.
	                                                        \fn			DListView_Columna(void);
                                                            \return     No devuelve nada
                                                    */
                                                    DListView_Columna(void);

                                                    //! Constructor
                                                    /*! Constructor asignador.
	                                                        \fn			DListView_Columna(DListView *LV, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);
                                                            \param[in]  LV             Puntero de la clase DListView que hace de padre para esta columna
                                                            \param[in]  nTipoEdicion   Tipo de edición que se utilizara por defecto en todos los items/subitems pertenecientes a esta columna.
                                                            \return     No devuelve nada
                                                    */
									                DListView_Columna(DListView *LV, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);

                                                    //! Función que asigna el texto para esta columna.
                                                    /*! Esta función se usa para asignar el texto de esta columna.
                                                            \fn         void AsignarTexto(const TCHAR *nTexto);
                                                            \param[in]  nTexto   Nuevo texto para la columna.
                                                            \return     No devuelve nada.
                                                            \remarks    Esta función soporta strings con un máximo de 256 caracteres.
                                                    */
		    void			                        AsignarTexto(const TCHAR *nTexto);

                                                    //! Función que obtiene el texto para esta columna.
                                                    /*! Esta función se usa para obtener el texto de esta columna.
                                                            \fn         void ObtenerTexto(std::wstring &nTexto);
                                                            \param[out] nTexto   Clase std::wstring donde se almacenara el texto de esta columna.
                                                            \return     No devuelve nada.
                                                            \remarks    Esta función soporta strings con un máximo de 256 caracteres.
                                                    */
			void                                    ObtenerTexto(std::wstring &nTexto);

                                                    //! Función que asigna el ancho en pixeles para esta columna.
                                                    /*! Esta función se usa para asignar el ancho en pixeles de esta columna.
                                                            \fn         void Ancho(const int nAncho);
                                                            \param[in]  nAncho   Nuevo ancho en pixeles para la columna.
                                                            \return     No devuelve nada.
                                                    */
		    void					                Ancho(const int nAncho);

                                                    //! Función que obtiene el ancho en pixeles para esta columna.
                                                    /*! Esta función se usa para obtener el ancho en pixeles de esta columna.
                                                            \fn         const int Ancho(void);
                                                            \return     Devuelve el ancho en pixeles de esta columna.
                                                    */
		    const int				                Ancho(void);

                                                    //! Función que asigna la alineación del texto para los subitems de esta columna.
                                                    /*! Esta función se usa para asignar la alineación del texto para los subitems de esta columna.
                                                            \fn         void AlineacionTexto(const DEnum_ListView_AlineacionTexto nAlineacion);
                                                            \param[in]  nAlineacion   Nueva alineación del texto que puede ser : DEnum_ListView_AlineacionTexto_Izquierda, DEnum_ListView_AlineacionTexto_Derecha, DEnum_ListView_AlineacionTexto_Centrado.
                                                            \return     No devuelve nada.
                                                    */
            void                                    AlineacionTexto(const DEnum_ListView_AlineacionTexto nAlineacion);

                                                    //! Función que obtiene la alineación del texto para los subitems de esta columna.
                                                    /*! Esta función se usa para obtener la alineación del texto para los subitems de esta columna.
                                                            \fn         const DEnum_ListView_AlineacionTexto AlineacionTexto(void);
                                                            \return     Devuelve la alineacion del texto que puede ser : DEnum_ListView_AlineacionTexto_Izquierda, DEnum_ListView_AlineacionTexto_Derecha, DEnum_ListView_AlineacionTexto_Centrado.
                                                    */
            const DEnum_ListView_AlineacionTexto    AlineacionTexto(void);
		  protected : //////////////////////////////// Metodos protegidos

									                //
									                //! Clase DListView padre
		    DListView	                          *_LV;

                                                    //
                                                    //! Los subitems de esta columna mostraran un combobox para editar sus datos
            DEnum_ListView_TipoEdicion             _TipoEdicion;

									                //
									                // Esta clase es amiga de la DListView
		    friend class			                DListView;

									                //
									                // Esta clase es amiga de la DListView_Item
		    friend class			                DListView_Item;
		};							                //
		////////////////////////////////////////////// Final DListView_Columna




		//! Clase para controlar un ListView de windows
        /*! Clase base que permite controlar un ListView de windows.
            \sa Cabecera DListView.h, clase DListView_Item, clase DListView_SubItem, clase DListView_Columna, clase DImageList.
        */
		class DListView : public DControl {
		  public : /////////////////////////////// Miembros publicos
//			virtual const DEnum_Objeto			Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_ListView; };
                                                //! Constructor
                                                /*! Constructor por defecto.
                                                        \fn			DListView(void);
                                                        \return     No devuelve nada
                                                        \remarks    Este constructor hace una llamada a InitCommonControlsEx para registrar la clase ListView
                                                */
											    DListView(void);

                                                //! Destructor
                                                /*! Destructor.
                                                        \fn		   ~DListView(void);
                                                        \return     No devuelve nada
                                                        \remarks    Este destructor elimina los items y las columnas de memoria.
                                                */
										       ~DListView(void);

                                                //! Función para crear el ListView (CreateWindowEx).
                                                /*! Esta función se usa para crear un ListView dinamicamente.
                                                        \fn         HWND Crear(DBaseWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos = NULL);
                                                        \param[in]  nPadre               Clase DBaseWnd que va a hacer de padre para el ListView
                                                        \param[in]  cX                   Coordenada X relativa a la ventana padre.
                                                        \param[in]  cY                   Coordenada Y relativa a la ventana padre.
                                                        \param[in]  cAncho               Ancho del ListView en pixeles.
                                                        \param[in]  cAlto                Altura del ListView en pixeles
                                                        \param[in]  cID                  ID para poder identificar el ListView en sus eventos.
                                                        \param[in]  nEstilos             Estilos estandar para el ListView.
                                                        \param[in]  nEstilosExtendidos   Estilos extendidos para el ListView. Estos estilos son los que se asignan con la macro ListView_SetExtendedListViewStyleEx.
                                                        \return     Devuelve el HWND del ListView o NULL en caso de error.
                                                */
		    HWND	    					    Crear(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, DWORD nEstilosExtendidos = NULL);

                                                //! Función para asignar esta clase a un ListView de un dialogo.
                                                /*! Esta función se usa para asignar esta clase a un ListView de un dialogo.
                                                        \fn         HWND Asignar(DBaseWnd *Padre, const UINT cID);
                                                        \param[in]  nPadre      Clase DBaseWnd que va a hacer de padre para el ListView
                                                        \param[in]  cID         ID para poder identificar el ListView en sus eventos.
                                                        \return     Devuelve el HWND del ListView o NULL en caso de error.
                                                        \remarks    Esta función solo debe utilizarse si tenemos un ListView en un dialogo de los recursos.
                                                */
			HWND							    Asignar(DhWnd *nPadre, const UINT cID);

											    //! Función para activar / desactivar el ListView.
											    /*!	Esta función activa / desactiva el ListView, y cancela cualquier edición de un item/subitem que este utilizando un editbox o un combobox.
													    \fn			void Activar(const BOOL nActivar);
													    \param[in]	nActivar	: Parametro en el que especificaremos si queremos activar o desactivar el ListView
													    \return		No devuelve nada.
											    */
	        virtual const BOOL			        Activar(const BOOL nActivar);

											    //! Función para activar / desactivar el uso de grupos dentro del ListView.
											    /*!	Esta función se usa para activar / desactivar el uso de grupos dentro del ListView.
													    \fn			void ActivarGrupos(const bool nActivar);
													    \param[in]	nActivar	: Parametro en el que especificaremos si queremos activar o desactivar los grupos dentro del ListView.
													    \return		No devuelve nada.
                                                        \remarks    Los grupos son como un item padre que puede contener varios items de forma que sus items queden agrupados.
											    */
            void                                ActivarGrupos(const bool nActivar) { ListView_EnableGroupView(hWnd(), nActivar); };

                                                //! Función para agregar un item al ListView
                                                /*! Esta función se usa para agregar un item al ListView, se pueden añadir textos de los subitems si hay varias columnas.
                                                        \fn DListView_Item *AgregarItem(const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...);
                                                        \param[in]  nIcono   ID del icono dentro del ImageList asignado a este listview (si no hay ImageList puedes especificar cualquier valor)
                                                        \param[in]  nPos     Posicion donde se añadira el nuevo Item (-1 para añadirlo al final)
                                                        \param[in]  nGrupo   Grupo al que pertenece el Item (-1 si no pertenece a ningun grupo, o no hay grupos)
                                                        \param[in]  nTexto   Cadena de caracteres que contiene el texto para el Item.
                                                        \param[in]  ...      Parametros opcionales que deben ser tantas cadenas de caracteres como columnas - 1 tenga el ListView, que se refieren a los textos para los subitems.
                                                        \return     Devuelve un puntero al DListView_Item creado.
                                                */
		    DListView_Item					   *AgregarItem(const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...);

                                                //! Función para agregar un item al ListView
                                                /*! Esta función se usa para agregar un item al ListView, se pueden añadir textos de los subitems si hay varias columnas.
                                                        \fn DListView_Item *AgregarItem(DListView_Item *nItem, const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...);
                                                        \param[in]  nItem    Clase DListView_Item de la que partira este item
                                                        \param[in]  nIcono   ID del icono dentro del ImageList asignado a este listview (si no hay ImageList puedes especificar cualquier valor)
                                                        \param[in]  nPos     Posicion donde se añadira el nuevo Item (-1 para añadirlo al final)
                                                        \param[in]  nGrupo   Grupo al que pertenece el Item (-1 si no pertenece a ningun grupo, o no hay grupos)
                                                        \param[in]  nTexto   Cadena de caracteres que contiene el texto para el Item.
                                                        \param[in]  ...      Parametros opcionales que deben ser tantas cadenas de caracteres como columnas - 1 tenga el ListView, que se refieren a los textos para los subitems.
                                                        \return     Devuelve un puntero al DListView_Item creado.
                                                        \remarks    Esta funcion se ha diseñado para que puedas crear una clase que herede de DListView_Item, en la que puedas añadir mas datos.
                                                                    De esta forma al pasarle la clase heredada a esta funcion se añadira al vector como si fuenra una clase DListView_Item normal.
                                                                    Cuando necesites acceder a los datos extras de tu clase heredada bastara con buscar el item deseado y hacerle un casting.
                                                */
		    DListView_Item					   *AgregarItem(DListView_Item *nItem, const int nIcono, const int nPos, const int nGrupo, const TCHAR *nTexto, ...);

											    //! Función para buscar la posición de una clase DListView_Item dentro del vector de items de este ListView.
											    /*!	Esta función se usa para buscar la posición de una clase DListView_Item dentro del vector de items de este ListView.
													    \fn			void BuscarItem(DListView_Item *bItem);
													    \param[in]	bItem   Clase DListView_Item de la cual queremos saber la posición dentro del vector de este ListView.
													    \return		Devuelve la posicion que ocupa la clase DListView_Item especificada, o -1 si no existe.
											    */
		    const int 			    			BuscarItem(DListView_Item *bItem);

											    //! Función para borrar un item del ListView especificando su posición.
											    /*!	Esta función se usa para borrar un item del ListView especificando su posición.
													    \fn			void BorrarItem(const int nItem);
													    \param[in]	nItem   Posición del item que queremos eliminar dentro de este ListView.
													    \return		No devuelve nada.
                                                        \remarks    Al borrar un item del ListView recibiremos una llamada en la función DWL::DEventosPadre::Evento_ListView_Item_Borrar() de su ventana padre.
											    */
		    void				    			BorrarItem(const int nItem);

											    //! Función para borrar todos los items seleccionados dentro del ListView.
											    /*!	Esta función se usa para borrar todos los items seleccionados dentro del ListView.
													    \fn			void BorrarItemsSeleccionados(void);
													    \return		No devuelve nada.
                                                        \remarks    Al borrar los items seleccionados del ListView recibiremos una llamada en la función DWL::DEventosPadre::Evento_ListView_Item_Borrar() de su ventana padre, por cada item borrado.
											    */
		    void				    			BorrarItemsSeleccionados(void);

											    //! Función para borrar todos los items dentro del ListView.
											    /*!	Esta función se usa para borrar todos los items dentro del ListView.
													    \fn			void BorrarTodosLosItems(void);
													    \return		No devuelve nada.
                                                        \remarks    Al borrar los todos items del ListView recibiremos una llamada en la función DWL::DEventosPadre::Evento_ListView_Item_Borrar() de su ventana padre, por cada item borrado. 
                                                                    Ademas tambien recibiremos una llamada a la función DWL::DEventosPadre::Evento_ListView_Item_BorrarTodos().
											    */
	        void			    				BorrarTodosLosItems(void);

											    //! Función para agregar un grupo dentro del ListView.
											    /*!	Esta función se usa para agregar un grupo dentro del ListView.
													    \fn			void AgregarGrupo(const TCHAR *nTexto, const int nID, const int nPos = -1);
                                                        \param[in]  nTexto   Texto que se mostrara en el grupo.
                                                        \param[in]  nID      ID para el grupo que deberas especificar al agregar un item si lo quieres introducir dentro.
                                                        \param[in]  nPos     Posición donde se agregara el grupo (-1 para agregarlo al final)
													    \return		No devuelve nada.
											    */
            void                                AgregarGrupo(const TCHAR *nTexto, const int nID, const int nPos = -1);
                                                
											    //! Función para agregar una columna al ListView.
											    /*!	Esta función se usa para agregar una columna al ListView.
													    \fn			void AgregarColumna(const TCHAR *nTexto, const int cAncho, const DEnum_ListView_AlineacionTexto nAlineacion = DEnum_ListView_AlineacionTexto_Izquierda, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);
                                                        \param[in]  nTexto         Texto que se mostrara en la columna.
                                                        \param[in]  cAncho         Ancho en pixeles para la columna.
                                                        \param[in]  nAlineacion    Alineacion para los subitems de esta columna que puede ser : DEnum_ListView_AlineacionTexto_Izquierda, DEnum_ListView_AlineacionTexto_Derecha y DEnum_ListView_AlineacionTexto_Centrado
                                                        \param[in]  nTipoEdicion   Tipo de edición automatica para los subitems de esta columna que puede ser : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, y DEnum_ListView_TipoEdicion_ComboStatic.
													    \return		No devuelve nada.
                                                        \remarks    La primera columna siempre se alineara a la izquierda por mucho que especifiquemos lo contrario. En la MSDN lei que hay un truco que es agregar una columna dummy como primera columna, y luego borrarla. De esta forma en teoria si que se podria alinear la primera columna de otra forma, pero no lo he probado.
											    */
		    void			    				AgregarColumna(const TCHAR *nTexto, const int cAncho, const DEnum_ListView_AlineacionTexto nAlineacion = DEnum_ListView_AlineacionTexto_Izquierda, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);

											    //! Función para borrar una columna del ListView especificando su posición.
											    /*!	Esta función se usa para borrar una columna del ListView especificando su posición.
													    \fn			void BorrarColumna(const int bColumna);
													    \param[in]	bColumna   Posición de la columna que queremos eliminar dentro de este ListView.
													    \return		No devuelve nada.
											    */
		    void			    				BorrarColumna(const int bColumna);

											    //! Función para cambiar el tipo de vista del ListView.
											    /*!	Esta función se usa para cambiar la visualizacion de los items dentro del ListView.
													    \fn			void CambiarVista(const DEnum_ListView_Vista nVista);
													    \param[in]	nVista   Nueva vista que utilizara el ListView, que puede ser : DEnum_ListView_Vista_Icono, DEnum_ListView_Vista_IconoPeque, DEnum_ListView_Vista_Detalles, DEnum_ListView_Vista_Lista, y DEnum_ListView_Vista_Mosaico.
													    \return		No devuelve nada.
                                                        \remarks    Para los tipos de vista DEnum_ListView_Vista_Icono y DEnum_ListView_Vista_Mosaico se utiliza el imagelist del tipo DEnum_ListView_ImageList_Normal, y para los demas tipos de vista se utiliza el imagelist del tipo DEnum_ListView_ImageList_Peque
											    */
            void                                CambiarVista(const DEnum_ListView_Vista nVista);

											    //! Función para seleccionar y marcar con el foco el item especificado.
											    /*!	Esta función se usa para seleccionar y marcar con el foco el item especificado.
													    \fn			inline void Seleccion(const int Pos);
													    \param[in]	Pos   Posición del item que queremos seleccionar y marcar con el foco del teclado.
													    \return		No devuelve nada.
                                                        \remarks    Al marcar el item con el foco del teclado podemos utilizar los cursores para movernos desde ese item a otra posición. Si lo que quieres es unicamente seleccionar el item sin asignarle el foco deberas utilizar la función DListView_Item::Seleccionado(). Puedes acceder al DListView_Item mediante la función DListView::Item().
											    */
		    inline void		    				Seleccion(const int Pos) {
												    ListView_SetItemState(hWnd(), Pos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED); 
											    };

											    //! Función para obtener el item que esta seleccionado con el foco del teclado.
											    /*!	Esta función se usa para obtener el item que esta seleccionado con el foco del teclado.
													    \fn			const int Seleccion(void);
													    \return		Devuelve la posición del item que esta seleccionado y con el foco del teclado. Devuelve -1 si no hay item marcado con el foco.
                                                        \remarks    Si deseas saber que items estan seleccionados deberas recorrer todos los items utilizando la funciones DListView::Item() y DListView::TotalItems(), y luego consultar la función DListView_Item::Seleccionado() para saber si el item esta seleccionado.
											    */
		    const int			    			Seleccion(void);

											    //! Función para obtener cuantos items del ListView estan seleccionados.
											    /*!	Esta función se usa para obtener el numero total de items seleccionados dentro del ListView.
													    \fn			inline const int Seleccionados(void)
													    \return		Devuelve el total de items seleccionados dentro del ListView.
											    */
		    inline const int	    			Seleccionados(void) {
												    return ListView_GetSelectedCount(hWnd()); 
											    };

											    //! Función para eliminar el estado de seleccion a todos los items del ListView.
											    /*!	Esta función se usa para eliminar el estado de seleccion a todos los items del ListView
													    \fn			void DesSeleccionarTodo(void);
													    \return		No devuelve nada.
											    */
		    void				    			DesSeleccionarTodo(void);

                                                //! Función para asignar un ImageList al ListView.
                                                /*! Esta función se utiliza para asignar un ImageList al ListView.
                                                        \fn         void AsignarImageList(DImageList *nListaImagenes, const DEnum_ListView_ImageList nTipo);
                                                        \param[in]  nListaImagenes   Puntero a una clase DImageList con la lista de imagenes que queremos asignar. Si especificas NULL des-asignaras el ultimo imagelist enlazado del mismo tipo
                                                        \param[in]  nTipo            Tipo de ImageList a asginar, que puede ser : DEnum_ListView_ImageList_Normal, DEnum_ListView_ImageList_Peque, DEnum_ListView_ImageList_Estado, DEnum_ListView_ImageList_GrupoCabecera
                                                        \return     Esta función no devuelve nada.
                                                        \remarks    Si no utilizas el estilo LVS_SHAREIMAGELISTS el imagelist sera borrado al destruir el ListView, por lo que si lo estas utilizando en otro control podrias ocasionar algun error.
                                                */
            void                                AsignarImageList(DImageList *nListaImagenes, const DEnum_ListView_ImageList nTipo);
            
                                                //! Función para obtener el rectangulo donde se ubica el texto de un item / subitem dentro del ListView.
                                                /*! Esta función se utiliza para obtener el rectangulo que contiene el texto de un item/subitem. Esta función se ha diseñado especificamente para la función DListView::EmpezarEdicion(), ya que dependiendo de la vista del ListView me interesa tener un determinado tamaño.
                                                        \fn         void ObtenerRectaTexto(const UINT eItem, const UINT eSubItem, RECT &RectaDestino, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);
                                                        \param[in]  eItem          Posición del item del que queremos obtener el rectangulo.
                                                        \param[in]  eSubItem       Posición del subitem del que queremos obtener el rectangulo.
                                                        \param[out] RectaDestino   RECT donde se almacenaran los valores del rectangulo.
                                                        \param[in]  nTipoEdicion   Parametro que indica el tipo de edicion que ha requerido esta función. Especifica DEnum_ListView_TipoEdicion_SinEdicion si vas a utilizar esta funcion, de lo contrario puede darte un tamaño algo mas grande dependiendo de la edición que especifiques.
                                                        \return     Esta función no devuelve nada.
                                                        \remarks    Las macros ListView_GetItemRect y ListView_GetSubItemRect no me devolvian los tamaños deseados a la hora de crear los controles de edicion, por ello he tenido que crear esta funcion que se basa en una mezcla de las dos macros, dependiendo de la vista y del tipo de edición.
                                                */
            void                                ObtenerRectaTexto(const UINT eItem, const UINT eSubItem, RECT &RectaDestino, const DEnum_ListView_TipoEdicion nTipoEdicion = DEnum_ListView_TipoEdicion_SinEdicion);

                                                //! Función para obtener acceso al item de la posición especificada.
                                                /*! Esta función se utiliza para obtener un puntero a la clase DListView_Item referente al item de la posición especificada.
                                                        \fn         DListView_Item *Item(const size_t Pos);
                                                        \param[in]  Pos   Posición del item del que queremos obtener.
                                                        \return     Esta función devuelve un puntero a la clase DListView_Item que hace referencia a la posición especificada.
                                                        \remarks    Cuidate mucho de especificar posiciones correctas o provocaras una violación de acceso.
                                                */
		    DListView_Item                     *Item(const size_t Pos);

                                                //! Función para obtener acceso a la columna de la posición especificada.
                                                /*! Esta función se utiliza para obtener un puntero a la clase DListView_Columna referente a la columna de la posición especificada.
                                                        \fn         DListView_Columna *Columna(const size_t Pos);
                                                        \param[in]  Pos   Posición de la columna del que queremos obtener.
                                                        \return     Esta función devuelve un puntero a la clase DListView_Columna que hace referencia a la posición especificada.
                                                        \remarks    Cuidate mucho de especificar posiciones correctas o provocaras una violación de acceso.
                                                */
			DListView_Columna                  *Columna(const size_t Pos);

                                                //! Función para obtener el total de items que caben en la parte visible del ListView.
                                                /*! Esta función se utiliza para obtener el total de items que caben en la parte visible del ListView, en la vista actual.
                                                        \fn         inline const int ItemsPorPagina(void);
                                                        \return     Devuelve el total de items que se pueden mostrar a la vez en el ListView.
                                                */
		    inline const int	    			ItemsPorPagina(void) {
												    return ListView_GetCountPerPage(hWnd()); 
											    };

                                                //! Función para obtener el total de items que hay en el ListView.
                                                /*! Esta función se utiliza para obtener el total de items se han creado dentro de este ListView.
                                                        \fn         const UINT TotalItems(void);
                                                        \return     Devuelve el total de items del ListView.
                                                */
		    const UINT			    			TotalItems(void);

                                                //! Función para obtener el total de columnas que hay en el ListView.
                                                /*! Esta función se utiliza para obtener el total de columnas se han creado dentro de este ListView.
                                                        \fn         const UINT TotalColumnas(void);
                                                        \return     Devuelve el total de columnas del ListView.
                                                */
		    const UINT				    		TotalColumnas(void);

                                                //! Función para empezar la edición de un item/subitem.
                                                /*! Esta función se utiliza para permitir al usuario que edite el valor de un item/subitem.
                                                        \fn         HWND EmpezarEdicion(const UINT eItem, const UINT eSubItem, const DEnum_ListView_TipoEdicion nTipoEdicion);
                                                        \param[in]  eItem           Posición del item que se va a editar.
                                                        \param[in]  eSubItem        Posicion del subitem que se va a editar.
                                                        \param[in]  nTipoEdicion    Tipo de edicion que se va a aplicar al subitem, que puede ser : DEnum_ListView_TipoEdicion_SinEdicion, DEnum_ListView_TipoEdicion_EditTexto, DEnum_ListView_TipoEdicion_EditEntero, DEnum_ListView_TipoEdicion_EditDecimal, DEnum_ListView_TipoEdicion_ComboEditTexto, DEnum_ListView_TipoEdicion_ComboEditEntero, DEnum_ListView_TipoEdicion_ComboEditDecimal, y DEnum_ListView_TipoEdicion_ComboStatic.
                                                        \return     Devuelve el HWND del control que se ha creado para la edición.
                                                        \remarks    Si empiezas una edición manualmente utilizando esta funcion, debes terminarla con la función TerminarEdicion para guardar los datos, o con la función CancelarEdicion para descartar los datos introducidos.
                                                                    Al utilizar esta función la ventana padre del ListView recibira una llamada a su funcion virtual DWL::DEventosPadre::Evento_ListView_Edicion_Empezar().
                                                                    Si el usuario presiona INTRO, la edicion terminara y se guardaran los datos. Si el usuario presiona ESC, la edición se cancelara y se descartaran los datos.
                                                        \sa         TerminarEdicion(), CancelarEdicion(), DWL::DEventosPadre::Evento_ListView_Edicion_Empezar().
                                                */
            HWND                                EmpezarEdicion(const UINT eItem, const UINT eSubItem, const DEnum_ListView_TipoEdicion nTipoEdicion);

                                                //! Función para terminar la edición de un item/subitem.
                                                /*! Esta función se utiliza para terminar la edición de un item/subitem y guardar los datos introducidos.
                                                        \fn         void TerminarEdicion(void);
                                                        \return     No devuelve nada.
                                                        \remarks    Al utilizar esta función la ventana padre del ListView recibira una llamada a su funcion virtual DWL::DEventosPadre::Evento_ListView_Edicion_Terminar().
                                                        \sa         EmpezarEdicion(), CancelarEdicion(), DWL::DEventosPadre::Evento_ListView_Edicion_Terminar().
                                                */
            void                                TerminarEdicion(void);

                                                //! Función para cancelar la edición de un item/subitem.
                                                /*! Esta función se utiliza para cancelar la edición de un item/subitem y descartar los datos introducidos.
                                                        \fn         void CancelarEdicion(void);
                                                        \return     No devuelve nada.
                                                        \remarks    Al utilizar esta función la ventana padre del ListView recibira una llamada a su funcion virtual DWL::DEventosPadre::Evento_ListView_Edicion_Cancelar().
                                                        \sa         EmpezarEdicion(), TerminarEdicion(), DWL::DEventosPadre::Evento_ListView_Edicion_Cancelar().
                                                */
            void                                CancelarEdicion(void);

                                                //! Función para indicar al ListView cuantos subitems debe mostrar en la vista mosaico.
                                                /*! Esta función se utiliza para indicar al ListView cuantos subitems debe mostrar en la vista mosaico.
                                                        \fn         void MostrarSubItemsMosaico(const UINT NumSubItems);
                                                        \param[in]  NumSubItems     Numero de subitems que se mostraran en la vista mosaico.
														\param[in]  AnchoEtiqueta   Ancho en pixeles para la etiqueta del mosaico.
														\param[in]  AltoEtiqueta    Altura en pixeles para la etiqueta del mosaico.
														\return     No devuelve nada.
                                                        \remarks    Por defecto la vista mosaico no mostrara subitems.
                                                        \sa         CambiarVista().
                                                */
			void                                MostrarSubItemsMosaico(const UINT NumSubItems, const LONG AnchoEtiqueta = 0, const LONG AltoEtiqueta = 0);

                                                //! Función para obtener cuantos subitems se mostraran en la vista mosaico.
                                                /*! Esta función se utiliza para obtener cuantos subitems se mostraran en la vista mosaico.
                                                        \fn         const UINT MostrarSubItemsMosaico(void);
                                                        \return     Devuelve el numero de subitems que se mostraran. Si devuelve 0 solo se mostrara el item.
                                                        \remarks    Por defecto la vista mosaico no mostrara subitems.
                                                        \sa         CambiarVista().
                                                */
            const UINT                          MostrarSubItemsMosaico(void);

                                                //! Función para hacer que el listview enseñe un item determinado.
                                                /*! Esta función se utiliza para hacer que el listview muestre un item determinado que podria estar oculto por encima o por debajo de la vista actual.
                                                        \fn         inline void MostrarItem(const int Pos);
                                                        \param[in]  Pos     Posición del item que se quiere mostrar.
                                                        \return     No devuelve nada.
                                                */
		    inline void			    			MostrarItem(const int Pos) {
												    ListView_EnsureVisible(hWnd(), Pos, false); 
											    };

                                                //! Función que indica que item esta en unas determinadas coordenadas relativas al ListView.
                                                /*! Esta función se utiliza para obtener que item esta en unas determinadas coordenadas relativas a los items que se ven en el ListView.
                                                        \fn         const int ItemEnCoordenadas(const int cX, const int cY);
                                                        \param[in]  cX     Coordenada X.
                                                        \param[in]  cY     Coordenada Y.
                                                        \return     Devuelve la posición del item que esta debajo de esas coordenadas o -1 en caso de no haber ningun item debajo.
                                                */
		    const int			    			ItemEnCoordenadas(const int cX, const int cY);

                                                //! Función que indica que subitem esta en unas determinadas coordenadas relativas al ListView.
                                                /*! Esta función se utiliza para obtener que subitem esta en unas determinadas coordenadas relativas a los items que se ven en el ListView.
                                                        \fn         const int SubItemEnCoordenadas(const int cX, const int cY, int &cItem, int &cSubItem);
                                                        \param[in]  cX          Coordenada X.
                                                        \param[in]  cY          Coordenada Y.
                                                        \param[out] cItem       Variable donde se almacenara la posición del item debajo de las coordenadas, o -1 si no hay item en esas coordenadas.
                                                        \param[out] cSubItem    Variable donde se almacenara la posición del item debajo de las coordenadas, o -1 si no hay item en esas coordenadas.
                                                        \return     Esta función no devuelve nada.
                                                */
		    void    			    			SubItemEnCoordenadas(const int cX, const int cY, int &cItem, int &cSubItem);

                                                //! Función que devuelve la fuente que se esta utilizando en el ListView.
                                                /*! Esta función se utiliza para obtener la fuente que se esta utilizando para pintar el texto en el ListView.
                                                        \fn         inline HFONT Fuente(void);
                                                        \return     Devuelve la fuente que se esta utilizando en el ListView.
                                                        \remarks    Ni se te ocurra borrar esta fuente como mínimo hasta que no sea utilizada por el ListView.
                                                */
		    inline HFONT		    			Fuente(void) {
                                                    return (HFONT)SendMessage(hWnd(), WM_GETFONT, 0, 0);
											    };

                                                //! Función para asignar la fuente del ListView.
                                                /*! Esta función se utiliza para asignar la fuente que se utilizara para pintar el texto en el ListView.
                                                        \fn         inline void Fuente(HFONT nFuente);
                                                        \param[in]  nFuente     HFONT que pasara a utilizar el ListView para pintar sus textos.
                                                        \return     No devuelve nada.
                                                        \remarks    Puedes asignar una fuente para un item y sus subitems con la función DListView_Item::Fuente(), o asignar una fuente para un subitem determinado con la función DListView_Item::FuenteSubItem().\n
                                                                    Para acceder a estas funciones utiliza la función Item() especificando la posición del item que quieres editar la fuente.
                                                */
		    inline void			    			Fuente(HFONT nFuente) {
                                                    SendMessage(hWnd(), WM_SETFONT, (WPARAM)nFuente, 0);
											    };

                                                //! Función para asignar el color del fondo del ListView.
                                                /*! Esta función se utiliza para asignar el color del fondo que se utilizara en el ListView.
                                                        \fn         inline void ColorFondo(COLORREF nCol);
                                                        \param[in]  nCol     Nuevo color para el fondo.
                                                        \return     No devuelve nada.
                                                        \sa         ColorTexto(), DListView_Item::ColorTexto(), DListView_Item::ColorTextoSeleccionado(), DListView_Item::ColorTextoSubItem(), DListView_Item::ColorTextoSeleccionadoSubItem().
                                                */
		    inline void				    		ColorFondo(COLORREF nCol) {
												    ListView_SetBkColor(hWnd(), nCol); 
											    };

                                                //! Función para obtener el color del fondo del ListView.
                                                /*! Esta función se utiliza para obtener el color del fondo que se utilizara en el ListView.
                                                        \fn         inline COLORREF ColorFondo(void);
                                                        \return     Devuelve el color del fondo que esta utilizando el ListView.
                                                        \sa         ColorTexto(), DListView_Item::ColorTexto(), DListView_Item::ColorTextoSeleccionado(), DListView_Item::ColorTextoSubItem(), DListView_Item::ColorTextoSeleccionadoSubItem().
                                                */
		    inline COLORREF		    			ColorFondo(void) {
												    return ListView_GetBkColor(hWnd()); 
											    };

                                                //! Función para asignar el color del texto del ListView.
                                                /*! Esta función se utiliza para asignar el color del texto que se utilizara en el ListView.
                                                        \fn         inline void ColorTexto(COLORREF nCol);
                                                        \param[in]  nCol     Nuevo color para el texto.
                                                        \return     No devuelve nada.
                                                        \remarks    Puedes asignar un color para un item y sus subitems con la función DListView_Item::ColorTexto(), o asignar un color para un subitem determinado con la función DListView_Item::ColorTextoSubItem().\n
                                                                    Para acceder a estas funciones utiliza la función Item() especificando la posición del item que quieres editar la fuente.
                                                        \sa         ColorFondo(), DListView_Item::ColorTexto(), DListView_Item::ColorTextoSeleccionado(), DListView_Item::ColorTextoSubItem(), DListView_Item::ColorTextoSeleccionadoSubItem().
                                                */
		    inline void			    		    ColorTexto(COLORREF nCol) { 
												    ListView_SetTextColor(hWnd(), nCol);
											    };

                                                //! Función para obtener el color del texto del ListView.
                                                /*! Esta función se utiliza para obtener el color del texto que se utilizara en el ListView.
                                                        \fn         inline COLORREF ColorTexto(void);
                                                        \return     Devuelve el color del texto que esta utilizando el ListView.
                                                        \sa         ColorFondo(), DListView_Item::ColorTexto(), DListView_Item::ColorTextoSeleccionado(), DListView_Item::ColorTextoSubItem(), DListView_Item::ColorTextoSeleccionadoSubItem().
                                                */
		    inline COLORREF			    		ColorTexto(void) {
												    return ListView_GetTextColor(hWnd()); 
											    };

                                                //! Función virtual que permite insertar gráficos en la fase de pintado de un item/subitem.
                                                /*! El objetivo de esta función virtual es poder insertar gráficos extras en la fase de pintado de un item/subitem.
                                                        \fn         virtual LRESULT Evento_PintarItem(HDC hDC, const UINT nItem, const UINT nSubItem, DWORD nTipoItem);
                                                        \param[in]  hDC             HDC donde pintar los graficos del ListView.
                                                        \param[in]  nItem           Posición del item que se va a pintar.
                                                        \param[in]  nSubItem        Posición del subitem que se va a pintar.
                                                        \param[in]  nTipoItem       Indica que tipo de item se va a pintar, puede ser : LVCDI_ITEM, LVCDI_GROUP, o LVCDI_ITEMSLIST. 
                                                        \return     Debes devolver CDRF_SKIPDEFAULT si vas a insertar gráficos extras, o CDRF_DODEFAULT si no vas a pintar nada.
                                                        \remarks    En el modo de vista mosaico, no podras pintar graficos extras para el nSubItem 0, para los demas modos de vista si que puedes.\n
                                                        \sa         NM_CUSTOMDRAW ListView en la MSDN.
                                                */
            virtual LRESULT                     Evento_PintarItem(HDC hDC, const UINT pItem, const UINT pSubItem, DWORD pTipoItem);
                                                
                                                //! Función virtual que permite controlar cuando se hace click en el ListView.
                                                /*! El objetivo de esta función virtual es poder captar cuando se hace click en el ListView al heredar la clase.
                                                        \fn         virtual LRESULT Evento_Mouse_Click(const int nItem, const int nSubItem, const UINT nBoton, const int cX, const int cY);
                                                        \param[in]  nItem       Item en el que se ha hecho click (puede ser -1 si no hay item).
                                                        \param[in]  nSubItem    SubItem en el que se ha hecho click (puede ser -1 si no hay item/subitem).
                                                        \param[in]  nBoton      Boton del mouse presionado (0 izquierdo, 1 derecho).
                                                        \param[in]  cX          Coordenada X del mouse.
                                                        \param[in]  cY          Coordenada Y del mouse.
                                                        \return     Debes devolver 0.
                                                */
            virtual LRESULT                     Evento_Mouse_Click(const int nItem, const int nSubItem, const UINT nBoton, const int cX, const int cY);

                                                //! Función virtual que permite controlar cuando se moeve el mouse en el ListView.
                                                /*! El objetivo de esta función virtual es poder captar cuando se moeve el mouse en el ListView al heredar la clase.
                                                        \fn         virtual LRESULT Evento_Mouse_Movimiento(const int nItem, const int nSubItem, const int cX, const int cY);
                                                        \param[in]  nItem       Item en el que se ha hecho click (puede ser -1 si no hay item).
                                                        \param[in]  nSubItem    SubItem en el que se ha hecho click (puede ser -1 si no hay item/subitem).
                                                        \param[in]  cX          Coordenada X del mouse.
                                                        \param[in]  cY          Coordenada Y del mouse.
                                                        \return     Debes devolver 0.
                                                */
            virtual LRESULT                     Evento_Mouse_Movimiento(const int nItem, const int nSubItem, const int cX, const int cY);
            
                                                //! Función virtual que permite controlar cuando sale el mouse del ListView.
                                                /*! El objetivo de esta función virtual es poder captar cuando sale el mouse en el ListView al heredar la clase.
                                                        \fn         virtual LRESULT Evento_Mouse_Saliendo(void);
                                                        \return     Debes devolver 0.
                                                */
            virtual LRESULT                     Evento_Mouse_Saliendo(void);

                                                //! Nuevo WindowProcedure que se sobrepondra al WindowProcedure original del ListView.
                                                /*! Esta función es la que re-emplaza el WindowProcedure del ListView para poder hacer cosas como la edicion, y el pintado personalizado.
                                                        \fn         virtual LRESULT CALLBACK GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam)
                                                        \param[in]  uMsg        Mensaje
                                                        \param[in]  wParam      Parametro wide
                                                        \param[in]  lParam      Parametro long
                                                        \return     Dependiendo del mensaje procesado devolvera una cosa u otra...
                                                        \remarks    Hacer un SendMessage al HWND de este ListView o utilizar esta función es exactamente lo mismo.
                                                */
		    virtual LRESULT CALLBACK		    GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

                                                //! Función que indica al listview que debe repintar un item con sus subitems.
                                                /*! Esta función de uso interno utiliza ListView_RedrawItems para forzar su repintado.
                                                        \fn         void RepintarItem(DListView_Item *nItem);
                                                        \param[in]  nItem       Clase DListView_Item que identifica al item que queremos repintar.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
            void                                RepintarItem(DListView_Item *nItem);

                                                //! Función que indica al listview que debe repintar un item con sus subitems.
                                                /*! Esta función de uso interno utiliza ListView_RedrawItems para forzar su repintado.
                                                        \fn         void RepintarItem(const int nPos);
                                                        \param[in]  nPos       Posicion del item que queremos repintar.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
            void                                RepintarItem(const int nPos);

                                                //! Función que retorna los estilos del ListView.
                                                /*! Esta función retorna todos los estilos del ListView.
                                                        \fn         DWORD ObtenerEstilos(void);
                                                        \return     Devuelve los estilos del ListView.
                                                */
             DWORD                              ObtenerEstilos(void);

                                                //! Función para asignar los estilos del ListView.
                                                /*! Esta función permite asignar uno o mas estilos mediante el parametro nEstilos previamente preparado.
                                                        \fn         void AsignarEstilos(DWORD nEstilos)
                                                        \param[in]  nEstilos   DWORD que contiene los nuevos estilos a asignar.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función esta pensada para modificar todos los estilos de golpe, por lo que si quieres modificar un solo estilo conservando los valores de los demas,
                                                                    primero deberas obtener los estilos mediante la función ObtenerEstilos(), para luego modificar lo que te interese, y finalmente pasar los estilos a esta función
                                                */
             void                               AsignarEstilos(DWORD nEstilos);

                                                //! Función que retorna los estilos extendidos del ListView.
                                                /*! Esta función retorna todos los estilos extendidos del ListView.
                                                        \fn         DWORD ObtenerEstilosEx(void);
                                                        \return     Devuelve los estilos del ListView.
                                                */
             DWORD                              ObtenerEstilosEx(void);

                                                //! Función para asignar los estilos extendidos del ListView
                                                /*! Esta función permite asignar uno o mas estilos extendidos mediante el parametro nEstilos previamente preparado
                                                        \fn         void AsignarEstilosEx(DWORD nEstilosEx)
                                                        \param[in]  nEstilosEx   DWORD que contiene los nuevos estilos a asignar.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función esta pensada para modificar todos los estilos extendidos de golpe, por lo que si quieres modificar un solo estilo conservando los valores de los demas,
                                                                    primero deberas obtener los estilos mediante la función ObtenerEstilos(), para luego modificar lo que te interese, y finalmente pasar los estilos a esta función
                                                */
             void                               AsignarEstilosEx(DWORD nEstilosEx);

                                                //! Función que devuelve que tipo de clase BaseWnd es esta clase
                                                /*! Esta función devuelve que tipo de clase BaseWnd es esta clase
                                                        \fn         virtual const DEnum_TipoBaseWnd TipoBaseWnd(void);
                                                        \returns    Devuelve un valor de DEnum_TipoBaseWnd
                                                        \remars     Esta función identifica que tipo de ventana o control se esta utilizando.
                                                */
//            virtual const DEnum_TipoBaseWnd     TipoBaseWnd(void)   { return DEnum_TipoBaseWnd_ListView; };

		  protected :///////////////////////////// Declaraciones protegidas (Solo podran usarse en la clase heredada no desde fuera)

                                                //! Función que es llamada al recibir el mensaje WM_MOUSEMOVE en el ListView.
                                                /*! Esta función de uso interno en esencia comprueba si debe cambiar el mouse cuando se para por un item/subitem que tiene la edición activada.
                                                        \fn         LRESULT _Evento_Mouse_Movimiento(const int cX, const int cY, const UINT Param);
                                                        \param[in]  cX      Coordenada X del mouse.
                                                        \param[in]  cY      Coordenada Y del mouse.
                                                        \param[in]  Param   WPARAM.
                                                        \return     Siempre devuelve 0.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
            LRESULT                            _Evento_Mouse_Movimiento(const int cX, const int cY, const UINT Param);

                                                //! Función que es llamada al recibir el mensaje WM_MOUSELEAVE en el ListView.
                                                /*! Esta función de uso interno en esencia devuelve el mouse a su estado original cuando sale del control.
                                                        \fn         LRESULT _Evento_Mouse_Saliendo(void);
                                                        \return     Siempre devuelve 0.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
            LRESULT                            _Evento_Mouse_Saliendo(void);

                                                //! Función que es llamada cuando el ListView notifica de un click a su padre.
                                                /*! Esta función de uso interno comprueba si se ha pichado encima de un item/subitem que tenga un tipo de edición asignada, para empezar la edición.
                                                        \fn         LRESULT _Evento_Mouse_BotonSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);
                                                        \param[in]  Boton   Boton del mouse (puede ser 0 o 1. L/R)
                                                        \param[in]  cX      Coordenada X del mouse.
                                                        \param[in]  cY      Coordenada Y del mouse.
                                                        \param[in]  Param   WPARAM.
                                                        \return     Siempre devuelve 0.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
            LRESULT                            _Evento_Mouse_BotonSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);

                                                // 
                                                //! Array de indices que sirve para ordenar las columnas en la vista mosaico
            UINT                              *_OrdenColumnas;

                                                //! Función que procesa las notificaciones NM_CLICK, NM_DBLCLK, NM_RCLICK, NM_RDBLCLK.
                                                /*! Esta función de uso interno devuelve las notificaciones del tipo click a su ventana padre.
                                                        \fn         LRESULT _NotificacionClick(LPARAM lParam);
                                                        \param[in]  lParam   Datos para la notificación.
                                                        \return     Siempre devuelve 0.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
            LRESULT                            _NotificacionClick(LPARAM lParam);

                                                //! Función que busca el HWND del EditBox que hay dentro del ComboBox.
                                                /*! Esta función de uso interno unicamente tiene la finalidad de obtener el HWND del EditBox que hay dentro de una ComboBox.
                                                        \fn         static BOOL CALLBACK _BuscarEditEnCombo(HWND hWndEdit, LPARAM lParam);
                                                        \param[in]  hWndEdit    HWND que deberia ser el del EditBox
                                                        \param[in]  lParam      Clase DWlistView que pide la busqueda.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
            static BOOL CALLBACK               _BuscarEditEnCombo(HWND hWndEdit, LPARAM lParam);

                                                //! Función que recibe el mensaje NM_CUSTOMDRAW.
                                                /*! Esta función de uso interno es la base para pintar items con colores y fuentes distintas.
                                                        \fn         LRESULT _CustomDraw(LPARAM Datos);
                                                        \param[in]  Datos      Datos para el pintado.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.
                                                */
		    LRESULT                            _CustomDraw(LPARAM Datos);

											    //
											    //! Vector de punteros a DWLListView_Item
		    std::vector<DListView_Item *>      _Items;

											    //
											    //! Vector de punteros a DWLListView_Columna
		    std::vector<DListView_Columna *>   _Columnas;

											    //
											    //! Item en Edición 
		    int                                _eItem;

											    //
										    	//! SubItem en Edición 
		    int                                _eSubItem;

											    //
											    //! Valor para determinar si se esta editando un item/subitem
		    DEnum_ListView_TipoEdicion         _Editando;

                                                //
                                                //! HWND para el control de edición.
            HWND                               _hWndEdicion;

                                                //! Gestor de mensajes para los editbox de las ediciones.
                                                /*! Esta función es el gestor de mensajes para los editbox que se crean durante las ediciones de items/subitems.
                                                        \fn         static LRESULT CALLBACK _GestorMensajesEdicion(HWND dhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
                                                        \param[in]  dhWnd       HWND del EditBox.
                                                        \param[in]  uMsg        Mensaje.
                                                        \param[in]  wParam      WPARAM.
                                                        \param[in]  lParam      LPARAM.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función es para uso interno, y no se recomineda hacer llamadas a ella.\n
                                                                    Utilizada en los EditBox y los ComboBox. 
                                                */
		    static LRESULT CALLBACK            _GestorMensajesEdicion(HWND dhWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

										    	//
										    	//! WindowProcedure Original del EditBox
		    WNDPROC                            _GestorMensajesOriginalEdicion;
											    //
		    friend class DListView_Item;	    // 
		    friend class DListView_Columna;	    // 
		};									    //
		////////////////////////////////////////// Final class DListView

        //! Clase que contiene los datos al hacer click o dobleclick
        class DListView_DatosClick {
          public : /// Miembros publicos
//			virtual const DEnum_Objeto	Objeto_Tipo(void) { return DEnum_Objeto::DEnum_Objeto_ListView_DatosClick; };

										//! Constructor
										/*! Constructor asignador.
												\fn			DListView_DatosClick(const int nPosItem, const int nPosSubItem, const int nBoton, const int nX, const int nY);
												\param[in]  nPosItem        Posición del item.
												\param[in]  nPosSubItem     Posición del subitem.
												\param[in]  nBoton          Boton del mouse.
												\param[in]  nX              Coordenada X.
												\param[in]  nY              Coordenada Y
												\return     No devuelve nada
										*/
										DListView_DatosClick(const int nPosItem, const int nPosSubItem, const int nBoton, const int nX, const int nY) : PosItem(nPosItem), PosSubItem(nPosSubItem), Boton(nBoton), X(nX), Y(nY) { };

										//
										//! Posición del item
            int							PosItem;

										//
										//! Posición del subitem
            int							PosSubItem;

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


        //! Clase que contiene los datos de una edición
		class DListView_DatosEdicion  {
		  public : /////////////////////////////// Miembros publicos
//			  virtual const DEnum_Objeto		Objeto_Tipo(void) { return DEnum_Objeto_ListView_DatosEdicion; };

                                                //! Constructor
                                                /*! Constructor.
                                                        \fn			DListView_DatosEdicion(void);
                                                        \return     No devuelve nada
                                                */
                                                DListView_DatosEdicion(void) : PosItem(-1), PosSubItem(-1), _hWndEdicion(NULL), TipoEdicion(DEnum_ListView_TipoEdicion_SinEdicion) { };

                                                //! Constructor
                                                /*! Constructor asignador.
                                                        \fn			DListView_DatosEdicion(const int nPosItem, const int nPosSubItem, HWND hWndEdicion, const DEnum_ListView_TipoEdicion nTipoEdicion);
                                                        \param[in]  nPosItem        Posición del item.
                                                        \param[in]  nPosSubItem     Posición del subitem.
                                                        \param[in]  hWndEdicion     HWND del control de edición.
                                                        \param[in]  nTipoEdicion    Tipo de edición.
                                                        \return     No devuelve nada
                                                */
                                                DListView_DatosEdicion(const int nPosItem, const int nPosSubItem, HWND hWndEdicion, const DEnum_ListView_TipoEdicion nTipoEdicion) : PosItem(nPosItem), PosSubItem(nPosSubItem), _hWndEdicion(hWndEdicion), TipoEdicion(nTipoEdicion) { };

                                                //
                                                //! Posición del Item
            int                                 PosItem;

                                                //
                                                //! Posición del SubItem
            int                                 PosSubItem;

                                                //! Función para agregar un item dentro del combobox.
                                                /*! Esta función permite agregar un item dentro del combobox
                                                        \fn         void Combo_AgregarItem(const TCHAR *nTexto);
                                                        \param[in]  nTexto      Texto para el nuevo item.
                                                        \return     No devuelve nada.
                                                        \remarks    Lo ideal es utilizar esta función al re-escribir la función DWL::DEventosPadre::Evento_ListView_Edicion_Empezar().
                                                */
            void                                Combo_AgregarItem(const TCHAR *nTexto);

                                                //! Función para asignar el texto que se muestra en el combobox.
                                                /*! Esta función permite para asignar el texto que se muestra en el combobox.
                                                        \fn         void ComboEdit_AsignarTexto(const TCHAR *nTexto);
                                                        \param[in]  nTexto      Texto para el combobox.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función es unicamente para ediciones con DEnum_ListView_TipoEdicion_ComboEdit..., no sirve para el tipo DEnum_ListView_TipoEdicion_ComboStatic.
                                                */
            void                                ComboEdit_AsignarTexto(const TCHAR *nTexto);

                                                //! Función para obtener el texto que se muestra en el combobox.
                                                /*! Esta función permite para obtener el texto que se muestra en el combobox.
                                                        \fn         void ComboEdit_ObtenerTexto(std::wstring &nTexto);
                                                        \param[out] nTexto      Clase std::wstring en la que se almacenara el texto de la combobox.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función es unicamente para ediciones con DEnum_ListView_TipoEdicion_ComboEdit..., no sirve para el tipo DEnum_ListView_TipoEdicion_ComboStatic.
                                                */
			void                                ComboEdit_ObtenerTexto(std::wstring &nTexto);

                                                //! Función para asignar la selección en un combobox estatico.
                                                /*! Esta función permite asignar la selección en un combobox estatico.
                                                        \fn         void ComboStatic_AsignarSeleccion(const TCHAR *nTexto);
                                                        \param[in]  nTexto      Texto que se buscara para seleccionar.
                                                        \return     No devuelve nada.
                                                        \remarks    No puedes asignar cualquier texto para mostrar en la combostatic, ese texto solo puede ser uno de los items de la combo.
                                                */
            void                                ComboStatic_AsignarSeleccion(const TCHAR *nTexto);

                                                //! Función para asignar la selección en un combobox estatico.
                                                /*! Esta función permite asignar la selección en un combobox estatico.
                                                        \fn         void ComboStatic_AsignarSeleccion(const int nPos);
                                                        \param[in]  nPos      Posición del texto que quieres seleccionar.
                                                        \return     No devuelve nada.
                                                        \remarks    No puedes asignar cualquier texto para mostrar en la combostatic, ese texto solo puede ser uno de los items de la combo.
                                                */
            void                                ComboStatic_AsignarSeleccion(const int nPos);

                                                //! Función para obtener el texto de la selección en un combobox estatico.
                                                /*! Esta función permite obtener el texto de la selección en un combobox estatico.
                                                        \fn         void ComboStatic_ObtenerSeleccion(std::wstring &nTexto);
                                                        \param[out] nTexto      Clase std::wstring donde se asignara el texto del combobox.
                                                        \return     No devuelve nada.
                                                */
			void                                ComboStatic_ObtenerSeleccion(std::wstring &nTexto);

                                                //! Función para obtener la posición de la selección en un combobox estatico.
                                                /*! Esta función permite obtener la posición de la selección en un combobox estatico.
                                                        \fn         const int ComboStatic_ObtenerSeleccion(void);
                                                        \return     Devuelve la posición del item seleccionado.
                                                */
            const int                           ComboStatic_ObtenerSeleccion(void);

                                                //! Función para asignar el texto del EditBox.
                                                /*! Esta función permite asignar el texto que se muestra en un EditBox.
                                                        \fn         void Edit_AsignarTexto(const TCHAR *nTexto);
                                                        \param[in]  nTexto      Nuevo texto.
                                                        \return     No devuelve nada.
                                                */
            void                                Edit_AsignarTexto(const TCHAR *nTexto);

                                                //! Función para obtener el texto del EditBox.
                                                /*! Esta función permite obtener el texto que se muestra en un EditBox.
                                                        \fn         void Edit_ObtenerTexto(std::wstring &nTexto);
                                                        \param[out] nTexto      Clase std::wstring en la que se almacenara el texto.
                                                        \return     No devuelve nada.
                                                */
			void                                Edit_ObtenerTexto(std::wstring &nTexto);

                                                //
                                                //! Tipo de edición
            const DEnum_ListView_TipoEdicion    TipoEdicion;

          protected : //////////////////////////// Miembros protegidos
                                                    
                                                //
                                                //! HWND del control de edición
            HWND                               _hWndEdicion;

            friend class DListView;             // Amiga de la clase DListView
        };                                      //
        //////////////////////////////////////////

    
		#define DWL_ESTILOS_EX_LISTVIEW    (LVS_EX_AUTOAUTOARRANGE | LVS_EX_AUTOCHECKSELECT | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_BORDERSELECT | LVS_EX_CHECKBOXES | LVS_EX_COLUMNOVERFLOW | LVS_EX_COLUMNSNAPPOINTS |      \
											LVS_EX_DOUBLEBUFFER | LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_HEADERINALLVIEWS | LVS_EX_HIDELABELS | LVS_EX_INFOTIP |      \
											LVS_EX_JUSTIFYCOLUMNS | LVS_EX_LABELTIP | LVS_EX_MULTIWORKAREAS | LVS_EX_ONECLICKACTIVATE | LVS_EX_REGIONAL | LVS_EX_SIMPLESELECT | LVS_EX_SINGLEROW | LVS_EX_SNAPTOGRID |  \
											LVS_EX_SUBITEMIMAGES | LVS_EX_TRACKSELECT | LVS_EX_TRANSPARENTBKGND | LVS_EX_TRANSPARENTSHADOWTEXT | LVS_EX_TWOCLICKACTIVATE | LVS_EX_UNDERLINECOLD | LVS_EX_UNDERLINEHOT)

	};

#endif

/*! \page NotasListView   Detalles de las versiones del ListView :
<b>Versión 1.01 [14/02/2012]</b>
<ul>
    <li>Se ha re-emplazado el parametro "HWND hWndPadre" de las funciones "Crear" y "Asignar" por DBaseWnd *nPadre.</li>
</ul>

<b>Versión 1.0 [08/02/2012]</b>
<ul>
    <li>Retocado el evento click del mouse para que cuando se haga click en el icono no empieze la edición.</li>
    <li>
        Retocado el CUSTOMDRAW ya que no se pintaban bien los items dependiendo de la vista.\n
        Se ha eliminado el color del fondo ya que no quedaba muy bien con el tema del explorador (SetWindowTheme)\n
        Se ha vuelto a introducir el color del texto seleccionado.\n
        Los colores dentro de los subitems se guardan con un puntero, de esta forma se si se ha puesto un color o no. Antiguamente se guardaba un COLORREF con el color directamente, por lo que no se podia saber si realmente era el color que habia definido el programador, o el color original del control.\n
        Se han eliminado varias variables del ListView referente a la fuente, y colores de la seleccion que han quedado obsoletas, y no era necesario mantener dichas variables en memoria.\n
        Testeado en todos los modos de vista.\n
        Se han eliminado las antiguas funciones evento para pintar items y ahora solo existe una : Evento_PintarItem(HDC hDC, const UINT nItem, const UINT nSubItem, DWORD nTipoItem);
    </li>
    <li>Los controles de edición ya no tomaran los colores del ListView para el texto y el fondo.</li>
    <li>Se han cambiado los parametros de la funcion SubItemEnCoordenadas, para que se pueda obtener el item y el subitem que hay en las coordenadas con la misma funcion sin necesidad de llamar a ItemEnCoordenadas.</li>
    <li>Documentación del archivo DListView.h al estilo doxygen terminada.</li>
    <li>Completados los ejemplos del ListView, se ha creado un proyecto con un ejemplo del ListView en varias vistas dentro de un dialogo, y un ejemplo de pintado de items personalizado en una ventana.</li>
</ul>

<b>Versión 0.99 [02/02/2012]</b>
<ul>
    <li>Añadida opcion para alinear textos de los subitems a derecha, izquierda, centro...</li>
    <li>Retocado el CustomDraw para eliminar coores que no se utilizaban (como los de la selección...)</li>
    <li>Eliminadas funciones para modificar el color del texto seleccionado, y el color del fondo de la selección.</li>
    <li>Retocada función Crear del ListView y añadida una segunda función que utiliza las clases basadas en DEstilo. </li>
    <li>Añadida funcion para cambiar la vista del ListView dinamicamente.</li>
    <li>Reparadas las clases DEstilo para este control que no funcionaban del todo bien, ademas ahora el ejemplo muestra los valores de cada estilo.</li>
    <li>Retocada funcion EmpezarEdicion que no mostraba correctamente las ediciones en el SubItem 0. (falta por calcular la vista mosaico....)</li>
    <li>Clases DListView_Item, DListView_SubItem y DListView_Columna totalmente comentadas al estilo Doxygen.</li>
    <li>Retocada funcion para añadir ImageLists, ahora solo acepta clases DImageList, y el segundo parametro pasa a ser una enumeracion de los tipos posibles para el ImageList : DEnum_ListView_ImageList</li>
    <li>Eliminado evento Evento_ListView_ItemResaltado que correspondia a la notificacion NM_HOVER, ya que no parecia responder en ningun momento, ademas el evento Evento_ListView_Mouse_Movimiento deberia hacer su trabajo, y ademas nos dice el item cosa que el NM_HOVER no.</li>
    <li>Testeados todos los nuevos eventos.</li>
    <li>
        Retocadas funciones para la edicion de items/subitems de forma que se muestren y funcionen correctamente en todas las vistas, incluida la mosaico.\n
        Con la vista mosaico se han recalculado los datos del HitTest y del GetSubItemRect para que la recta devuelta llegue al final del marco. (Por defecto las rectas devueltas solo engloban el espacio del texto, y no lleguan hasta el final del marco por la izquierda)
    </li>
    <li>Añadida función ObtenerRectaTexto, que aunque esta diseñada para la función EmpezarLabelEdit se puede utilizar para obtener el rectangulo que ocupa el texto de un item/subitem de la misma forma que se calcula en la edición.</li>
</ul>

<b>Versión 0.98 [28/01/2012]</b>
<ul>
    <li>
        Se han eliminado todos los eventos del ListView, y se han creado nuevos eventos basados en las notificaciones que manda.\n
        Eliminados 9 eventos antiguos.\n
        Añadidas 20 notificaciones, y omitidas 13 notificaciones que no me cuadraban mucho, o que no sabia ni para que eran aun leyendo la MSDN....
    </li>
    <li>Se han agregado funciones para utilizar grupos dentro del listview.</li>
    <li>Se han re-emplazado las funciones para añadir items de forma que permitan enlazar los items a los grupos.</li>
    <li>Se ha creado un sistema mas intuitivo para modificar los estilos básicos y extendidos.</li>
    <li>Se ha implementado el tema del explorer de windows XP, Vista y 7 (EN FASE DE PRUEBAS, HAY ALGUN CONFLICTO CON EL CUSTOMDRAW).</li>
    <li>Implementado ejemplo para hacer pruebas del control.</li>
    <li>Añadidas funciones para asignar y obtener el estado marcado de un item (checkbox)</li>
</ul>

<b>Versión 0.97 [21/01/2012]</b>
<ul>
    <li>
        Unificado sistema de edición de items/subitems ahora se dispone de 7 formas de editar un item/subitem por el usuario :\n
        EditBox con texto, EditBox con valor entero, EditBox con valor decimal, ComboBox con texto, ComboBox con valor entero, ComboBox con valor decimal, ComboBox con texto estatico.
    </li>
    <li>Eliminadas funciones EmpezarLabelEdit, TerminarLabelEdit y CancelarLabelEdit. En su lugar se han creado las funciones EmpezarEdicion, TerminarEdicion y CancelarEdicion.</li>
    <li>Eliminadas clases DListView_DatosComboBox y DListView_TerminarLabelEdit que transmitian datos de la edicion de un item / subitem. En su lugar ahora esta la clase DListView_DatosEdicion.</li>
    <li>Las funciones para editar colores y fuentes de los subitems dentro de la clase DListView_Item han cambiado de nombre incluyendo al final "SubItem".</li>
    <li>Se han creado nuevas funciones en la clase DListView_Item que permiten editar todos los colores y las fuentes de los items (incluyendo todos sus SubItems) : ColorTexto, ColorFondo, Fuente</li>
    <li>Se ha revisado el CustomDraw del ListView que no funcionaba si el ListView estaba dentro de un Dialogo, ahora funciona tanto en dialogos como en ventanas.</li>
    <li>
        Se han eliminado los siguientes eventos de las ventanas padre : Evento_ListView_Mostrar_ComboBox, Evento_ListView_ComboBox_CambioSeleccion, Evento_ListView_EmpezarLabelEdit, Evento_ListView_TerminarLabelEdit, y Evento_ListView_CancelarLabelEdit.\n
        En su lugar se han creado los siguientes eventos : Evento_ListView_Edicion_Empezar, Evento_ListView_Edicion_Terminar, Evento_ListView_Edicion_Cancelar.
    </li>
    <li>Se ha corregido el orden en que se creaban las columnas dentro del ListView, antes se añadian por la izquierda, ahora se añaden por la derecha que es mas logico.</li>
</ul>

<b>Versión 0.96 [19/01/2012]</b>
<ul>
    <li>Renombradas todas las clases de DWLListView a DListView para que sigan las nuevas nomenclaturas de la DWL 2012.</li>
    <li>Implementado soporte para editar subitems mediante una ComboBox. Se ha añadido el evento Evento_ListView_Mostrar_ComboBox que permite rellenar la ComboBox antes de mostrarla.</li>
    <li>Añadido soporte para el evento Evento_ListView_ComboBox_CambioSeleccion que nos indica que se ha cambiado la selección en un ComboBox que estaba editando un item.</li>
    <li>Añadida clase DListView_DatosComboBox para pasar los datos de una edición que utilice un ComboBox.</li>
    <li>Se ha creado la clase DListView_SubItem para contener los datos de los SubItems, antes estaban separados independientemente en DListView_Item cada uno con un vector propio, lo cual quedaba bastante feo....</li>
    <li>Dentro de cada SubItem existe una variable ComboBox que determina si este debe ser editado con una ComboBox o no.</li>
    <li>Se ha elimiando el String temporal que se utilizaba para devolver textos de los items/subitems ya que resultaba bastante peligroso, ahora todas las funciones que retornan texto tienen un std::wstring por referencia en el que se almacena este.</li>
    <li>Redirigidos los eventos, que ahora pasan por el gestor de mensajes de DEventosBase, de esta forma resulta mucho mas cómodo editar cualquier evento de este control.</li>
</ul>

<b>Versión 0.95c [10/10/2007]</b>
<ul>
    <li>Eliminadas macros de ENLACE_ ahora las ventanas padre tienen las funciones para todos los eventos necesiarios por defecto.</li>
</ul>


<b>Version 0.95b [07/12/2005]</b>
<ul>
    <li>
        Se ha agregado una nueva clase para así hacer mas limpio el LabelEdit, esta clase contiene varios datos a tener en cuenta y permite validar la edición o no según convenga. \n
        Para esto se ha tenido que modificar la macro ENLACE_ListView_Evento_LabelEdit_Fin ademas del mensaje DWL_LISTVIEW_LABELEDIT_FIN para que haora retornen una clase del tipo DWLListView_TerminarLabelEdit.
    </li>
</ul>

<b>Version 0.95 [22/05/2005]</b>
<ul>
    <li>Se han eliminado todas las macros que empezaban por On. Haora existen nuevas macros que empiezan por ENLACE_</li>
    <li>
        Se han eliminado las notificaciones estandard que retornaba este control. Haora este control retorna sus propios mensajes :\n
        <table style='border-width:0px'>
            <tr><td><b>Mensaje</b></td><td><b>Macro</b></td></tr>
            <tr><td>DWL_LISTVIEW_CLICK_DERECHO</td>         <td>ENLACE_ListView_Evento_Mouse_Click</td></tr>
            <tr><td>DWL_LISTVIEW_CLICK_IZQUIERDO</td>       <td>ENLACE_ListView_Evento_Mouse_Click</td></tr>
            <tr><td>DWL_LISTVIEW_CLICK_MEDIO</td>           <td>ENLACE_ListView_Evento_Mouse_Click</td></tr>
            <tr><td>DWL_LISTVIEW_DOBLECLICK_DERECHO</td>    <td>ENLACE_ListView_Evento_Mouse_DobleClick</td></tr>
            <tr><td>DWL_LISTVIEW_DOBLECLICK_IZQUIERDO</td>  <td>ENLACE_ListView_Evento_Mouse_DobleClick</td></tr>
            <tr><td>DWL_LISTVIEW_DOBLECLICK_MEDIO</td>      <td>ENLACE_ListView_Evento_Mouse_DobleClick</td></tr>
            <tr><td>DWL_LISTVIEW_TECLADO_PRESIONADO</td>    <td>ENLACE_ListView_Evento_Teclado_TeclaPresionada</td></tr>
            <tr><td>DWL_LISTVIEW_TECLADO_SOLTADO</td>       <td>ENLACE_ListView_Evento_Teclado_TeclaSoltada</td></tr>
            <tr><td>DWL_LISTVIEW_TECLADO_INTRO</td>         <td>ENLACE_ListView_Evento_Teclado_Intro</td></tr>
            <tr><td>DWL_LISTVIEW_FOCO_OBTENIDO</td>         <td>ENLACE_ListView_Evento_Foco_Obtenido</td></tr>
            <tr><td>DWL_LISTVIEW_FOCO_PERDIDO</td>          <td>ENLACE_ListView_Evento_Foco_Perdido</td></tr>
        </table>
    </li>
</ul>

<b>Ver 0.9a</b>
<ul>
    <li>Fallo extraño semicorregido al añadir un item bajo threads se ha solucionando agregando primero los datos al vector y luego mostrando el listview (Gracias a cofruben)</li>
    <li>Se han modificado varias funciónes para que sean en linea (inline ...)</li>
    <li>El codigo ha sido modificado para poder editarlo mejor</li>
</ul>
*/