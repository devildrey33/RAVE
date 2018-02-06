/*! \file                       DButton.h
    \brief                      Archivo que contiene las definiciones para la clase DButton, utilizada en botones de windows.

    \details                    El button es el control por excelencia de windows, y se puede utilizar con muchas finalidades. En esencia es un espacio donde hacemos click con el mouse para realizar alguna acción.\n\n
                                Ademas y pese a que no me acabe de gustar, el button no solo es el tipico boton rectangular, tambien puede ser un boton para marcar estados (checkbox), un boton para seleccionar opciones (option button),
                                un control para agrupar otros controles (group box o frame), o un boton del tipo split (algo similar a una combobox estatica pero personalizable), etc...\n

                                La verdad es que si yo hubiera programado dichos controles los hubiera hecho de forma separada, pero eso es otro cantar.\n

                                En definitiva si quieres controlar un CheckBox, o un OptionButton, o un GroupBox, o un SplitButton, o un LinkButton debes utilizar esta clase. Todos los controles que se ven en la siguiente imagen son buttons de algun tipo :
                                <img src='BUTTONS.png' alt='Tipos de Button' />

    \section notificaciones     Notificaciones disponibles :
                                El button manda una serie de notificaciones a traves de los mensajes WM_COMMAND y WM_NOTIFY, que estan todas encapsuladas dentro de la DWL, a continuación tienes una lista detallada de las notificaciones disponibles, y de que función debes utilizar para acceder a ellas :
                                <ul>
                                    <li>
                                        <b>BCN_DROPDOWN</b> Indica que se ha presionado en la parte de la flecha de un boton del tipo split. A partir de esta notificacion podemos por ejemplo crear un menu y desplegarlo.\n
                                        Para obtener esta notificación en la ventana padre debes sobre-escribir la siguiente función : DWL::DEventosPadre::Evento_Button_Desplegar().\n\n
                                    </li>
                                    <li>
                                        <b>BCN_HOTITEMCHANGE</b> Indica que el mouse ha entrado o ha salido del control.\n
                                        Para obtener esta notificación en la ventana padre debes sobre-escribir las siguientes función : \n
                                        Cuando el mouse ha entrado DWL::DEventosPadre::Evento_Button_Mouse_Entrando().\n
                                        Cuando el mouse ha salido DWL::DEventosPadre::Evento_Button_Mouse_Saliendo().\n\n
                                    </li>
                                    <li>
                                        <b>BN_CLICKED</b> Indica que el usuario ha hecho click en el boton.\n
                                        Para obtener esta notificación en la ventana padre debes sobre-escribir la siguiente función : DWL::DEventosPadre::Evento_Button_Mouse_Click().\n\n
                                    </li>
                                    <li>
                                        <b>BN_DBLCLK</b> y <b>BN_DOUBLECLICKED</b> Indica que se ha hecho doble click en el boton.\n
                                        Para obtener esta notificación en la ventana padre debes sobre-escribir la siguiente función : DWL::DEventosPadre::Evento_Button_Mouse_DobleClick().\n
                                        <b>NOTA :</b> Esta notificacion solo estara disponible si el button tiene el estilo BS_NOTIFY activado.\n\n
                                    </li>
                                    <li>
                                        <b>BN_KILLFOCUS</b> Indica que el boton ha perdido el foco.\n
                                        Para obtener esta notificación en la ventana padre debes sobre-escribir la siguiente función : DWL::DEventosPadre::Evento_Button_Foco_Perdido().\n
                                        <b>NOTA :</b> Esta notificacion solo estara disponible si el button tiene el estilo BS_NOTIFY activado.\n\n
                                    </li>
                                    <li>
                                        <b>BN_SETFOCUS</b> Indica que el boton ha recibido el foco.\n
                                        Para obtener esta notificación en la ventana padre debes sobre-escribir la siguiente función : DWL::DEventosPadre::Evento_Button_Foco_Obtenido().\n
                                        <b>NOTA :</b> Esta notificacion solo estara disponible si el button tiene el estilo BS_NOTIFY activado.\n
                                    </li>
                                </ul>
                                Las notificaciones del Button que no esten nombradas aquí las puedes obtener mediante la funciónes virtuales DWL::DEventosPadre::Evento_Notificacion() y DWL::DEventosPadre::Evento_Comando().\n

                                Para mas información sobre las notificaciones del Button puedes consultar el siguente enlace : <a href='http://msdn.microsoft.com/en-us/library/bb775943(v=vs.85).aspx' target="_blank">MSDN Button</a>.\n

                                Por ultimo hay que remarcar que dispones de un ejemplo donde puedes ver como crear todos los tipos de Button disponibles. Para ver el ejemplo abre el proyecto <i>"DWL 2012 Ejemplos.sln"</i>, y establece como proyecto de inicio el proyecto <i>"Ejemplo Button"</i>.\n\n

    \subpage NotasButton       "Notas de la versión".
                                <hr>
    \author                     devildrey33
    \date                       Creado el [10/01/2012], ultima modificación el [09/02/2012]
	\remarks	                Archivo creado por devildrey33 para http://www.devildrey33.es														                                \n
				                Este archivo es parte de la DWL (Devildrey33 Windows Library) y se distribuye bajo la licencia GPL, para mas información consulta estos enlaces :	\n
				                 - http://www.gnu.org/licenses/gpl.html     (Ingles, documento oficial)																	            \n
				                 - http://www.viti.es/gnu/licenses/gpl.html (Castellano, traducción no oficial)															            \n
				                 - http://www.softcatala.cat/wiki/GPL3		(Catalá, traducción no oficial)																            \n
*/

//#pragma once
#ifndef DBUTTON_H
    #define DBUTTON_H
    
	#include <string>
	#include "DhWnd.h"

	enum DEnum_Button_Tipo {
		DEnum_Button_Tipo_Normal = 0,
		DEnum_Button_Tipo_PorDefecto,
		DEnum_Button_Tipo_PushLike,
		DEnum_Button_Tipo_Split,
		DEnum_Button_Tipo_SplitPorDefecto,
		DEnum_Button_Tipo_Link,
		DEnum_Button_Tipo_LinkPorDefecto
	};


    namespace DWL {
        //! Clase para controlar los distintos tipos de Button de windows.
		class DButton : public DControl {
		  ////////////////////////////////////////
		 public:
                                                //! Constructor
                                                /*! Constructor.
                                                        \fn			DButton(void);
                                                        \return     No devuelve nada
                                                */
												DButton(void) : DControl() { };

                                                //! Destructor
                                                /*! Destructor.
                                                        \fn		   ~DButton(void);
                                                        \return     No devuelve nada
                                                */
                                               ~DButton(void) { Destruir(); };

                                                //! Función para crear el Button (CreateWindowEx).
                                                /*! Esta función se usa para crear un Button dinamicamente.
                                                        \fn         HWND Crear(DBaseWnd *nPadre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, const DEnum_Button_Tipo TipoBoton);
                                                        \param[in]  nPadre               Clase DBaseWnd que contendra el button.
                                                        \param[in]  nTexto               Texto para el Button.
                                                        \param[in]  cX                   Coordenada X relativa a la ventana padre.
                                                        \param[in]  cY                   Coordenada Y relativa a la ventana padre.
                                                        \param[in]  cAncho               Ancho del Button en pixeles.
                                                        \param[in]  cAlto                Altura del Button en pixeles
														\param[in]  cID                  ID para poder identificar el Button en sus eventos.
														\param[in]  nVisible             Estado de visibilidad (TRUE/FALSE).
														\param[in]  TipoBoton            Tipo de boton, puede ser : DEnum_Button_Tipo_Normal (POR DEFECTO), DEnum_Button_Tipo_PorDefecto, DEnum_Button_Tipo_PushLike, DEnum_Button_Tipo_Split, DEnum_Button_Tipo_SplitPorDefecto, DEnum_Button_Tipo_Link, y DEnum_Button_Tipo_LinkPorDefecto.
                                                        \return     Devuelve el HWND del Button o NULL en caso de error.
                                                */
			HWND	    					    Crear(DhWnd *nPadre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, const BOOL nVisible = TRUE, const DEnum_Button_Tipo TipoBoton = DEnum_Button_Tipo_Normal);

			
/*			HWND	    					    CrearBoton(DhWnd *nPadre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, const BOOL nVisible = TRUE);
			HWND	    					    CrearPush(DhWnd *nPadre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, const BOOL nVisible = TRUE);
			HWND	    					    CrearSplit(DhWnd *nPadre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, const BOOL nVisible = TRUE);
			HWND	    					    CrearLink(DhWnd *nPadre, const TCHAR *nTexto, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, const BOOL nVisible = TRUE);*/

                                                //! Función para asignar esta clase a un Button de un dialogo.
                                                /*! Esta función se usa para asignar esta clase a un Button de un dialogo.
                                                        \fn         HWND Asignar(DBaseWnd *Padre, const UINT cID);
                                                        \param[in]  nPadre      Clase DBaseWnd que contendra el button.
                                                        \param[in]  cID         ID para poder identificar el Button en sus eventos.
                                                        \return     Devuelve el HWND del Button o NULL en caso de error.
                                                        \remarks    Esta función solo debe utilizarse si tenemos un Button en un dialogo de los recursos.
                                                */
			HWND							    Asignar(DhWnd *nPadre, const UINT cID);

                                                //! Función para asignar el texto del button.
                                                /*! Esta función se usa para asignar el texto del button.
                                                        \fn         void AsignarTexto(const TCHAR *nTexto);
                                                        \param[in]  nTexto   Nuevo texto para el button.
                                                        \return     No devuelve nada.
                                                        \sa         ObtenerTexto().
                                                */
			void                                AsignarTexto(const TCHAR *nTexto);

                                                //! Función para obtener el texto del button.
                                                /*! Esta función se usa para obtener el texto del button.
                                                        \fn         void ObtenerTexto(std::wstring &nTexto);
                                                        \param[out] nTexto   Clase DString donde se almacenara el texto del button.
                                                        \return     No devuelve nada.
                                                        \sa         AsignarTexto().
                                                */
			void                                ObtenerTexto(std::wstring &nTexto);

                                                //! Función que devuelve la fuente que se esta utilizando en el Button.
                                                /*! Esta función se utiliza para obtener la fuente que se esta utilizando para pintar el texto en el Button.
                                                        \fn         inline HFONT Fuente(void);
                                                        \return     Devuelve la fuente que se esta utilizando en el Button.
                                                        \remarks    Ni se te ocurra borrar esta fuente como mínimo hasta que no sea utilizada por el Button.
                                                */
		    inline HFONT					    Fuente(void) {
												    return (HFONT)SendMessage(hWnd(), WM_GETFONT, 0, 0);
											    };

                                                //! Función para asignar la fuente del Button.
                                                /*! Esta función se utiliza para asignar la fuente que se utilizara para pintar el texto en el Button.
                                                        \fn         inline void Fuente(HFONT nFuente);
                                                        \param[in]  nFuente     HFONT que pasara a utilizar el Button.
                                                        \return     No devuelve nada.
                                                */
		    inline void						    Fuente(HFONT nFuente) {
												    SendMessage(hWnd(), WM_SETFONT, (WPARAM)nFuente , 0);
											    };

                                                //! Función para obtener el estado marcado de un Button.
                                                /*! Esta función se utiliza para obtener el estado marcado de un Button (valida tambien para el CheckBox y el OptionButton).
                                                        \fn         inline const BOOL Marcado(void);
                                                        \return     Devuelve BST_UNCHECKED si no esta marcado, BST_CHECKED si esta marcado, y BST_INDETERMINATE si esta completamente pintado.
                                                        \remarks    Solo puede devolver BST_INDETERMINATE si el button en cuestion tiene el estilo BS_AUTO3STATE o BS_3STATE.
                                                */
            inline const BOOL                   Marcado(void) {
                                                    return static_cast<BOOL>(SendMessage(hWnd(), BM_GETCHECK, 0, 0));
                                                };

                                                //! Función para asignar el estado marcado de un Button.
                                                /*! Esta función se utiliza para asignar el estado marcado de un Button (valida tambien para el CheckBox y el OptionButton).
                                                        \fn         inline void Marcar(const BOOL nMarcar);
                                                        \param[in]  nMarcar     BST_UNCHECKED para desmarcar el boton, BST_CHECKED para marcar el boton, y BST_INDETERMINATE para pintarlo completamente.
                                                        \return     No devuelve nada.
                                                        \remarks    Solo se puede asignar BST_INDETERMINATE si el button en cuestion tiene el estilo BS_AUTO3STATE o BS_3STATE.
                                                */
            inline void                         Marcar(const BOOL nMarcar) {
                                                    SendMessage(hWnd(), BM_SETCHECK, nMarcar, 0);
                                                };

                                                //! Función para mostrar el icono de elevación de privilegios en el button.
                                                /*! Esta función se utiliza para mostrar el icono de elevación de privilegios en el button.
                                                        \fn         inline void IconoElevarPrivilegios(const BOOL nIcono);
                                                        \param[in]  nIcono     TRUE para mostrar la imagen, FALSE para no mostrarla.
                                                        \return     No devuelve nada.
                                                        \remarks    Esta función solo muestra el icono de elvacion de privilegios en el boton, si luego el boton no se utiliza para ese fin es cosa del programador.
                                                */
            inline void                         IconoElevarPrivilegios(const BOOL nIcono) {
                                                    SendMessage(hWnd(), BCM_SETSHIELD, 0, nIcono);
                                                };



        };                                      //
        ////////////////////////////////////////// FIN Clase DButton



};

#endif

/*! \page   NotasButton     Notas de las versiones de Button
<b>Versión 1.01 [14/02/2012]</b>
<ul>
    <li>Se ha re-emplazado el parametro "HWND hWndPadre" de las funciones "Crear" y "Asignar" por DBaseWnd *nPadre.</li>
</ul>

<b>Versión 1.0 [09/02/2012]</b>
<ul>
    <li>Creada clase para los estilos del Button DButton_Estilos.</li>
    <li>Creados 7 eventos para el boton.</li>
    <li>Agregada función IconoElevarPrivilegios que permite mostrar el icono de elevación de privilegios.</li>
    <li>Documentado todo el archivo DBoton.h al estilo doxygen.</li>
</ul>
*/