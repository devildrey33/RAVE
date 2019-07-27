# Log de cambios

### Versión 0.526

* Ya no se agregan medios que esten temporalmente ausentes o no existan a las listas aleatórias.

* Las raices no disponibles ya no muestran un icono con una estrella en las opciones, y ahora muestran un texto (no disponible) para dejar claro que no está disponible.

* Las raices no disponibles ya no muestran su carpeta vacia en la base de datos.

* Se ha restaurado el icono para borrar raices en el mismo item que representa una raíz (en las opciones)

* Retocada la forma en que se guardan los medios a la BD, ahora ya no conservaran la ID del disco duro que los contiene.
	* Lo malo es que si tienes el mismo medio en dos discos distintos con la misma ruta (solo que cambie la unidad) el reproductor solo reconocerá el primero de los dos (lo que no debería ser problema, a no ser que realmente no sean el mismo medio).
	* Lo bueno es que los medios siguen sin necesitar la letra de unidad de su path, por lo que se puede mover el directorio a otro disco duro, y el reproductor seguirá reconociendolos y conservará todos sus datos (habra que añadir una nueva raíz con el nuevo disco para que se muestren en la BD)

* Solucionado bug que no almacenaba las etiquetas de los generos al analizar la canción.
	* Solucionado crash si se intentaba crear una lista aleatoria sin etiquetas.

* Solucionado el ancho de las etiquetas de Opciones -> Lista2 que se solapaban con los controles desplegables y no dejaban llegar los eventos de mouse en los primeros 60 pixeles de los desplegables.
	* Lo mismo con Opciones -> Lista, que tambien tiene un control desplegable (es el problema que tiene el copy/paste a veces...)

* Añadidas funcionabilidades básicas a los controles de edición de texto : alineación, selección, control + [c x v], placeholder, control [z y] con un buffer ilimitado.
	* No son 100% funcionales aún, pero para lo que necesita el reproductor creo que va sobrado.

* Se han separado la tabla Opciones y la tabla TeclasRapidas de la base de datos de medios, ahora las tienen su propio archivo "Opciones.BD".
	* Esto evita que no se guarden las opciones mientras se está ejecutando el thread de análisis.
	* Por otra parte se pierden las antiguas opciones configuradas, por lo que habrá que reconfigurar el reproductor de nuevo.



### Versión 0.525

* Ahora se puede cambiar el skin del arbol y la lista dinámicamente, esto permite mostrar el arbol y la lista con una configuración oscura en el modo pantalla completa.

* Creado modo oscuro para la lista, el arbol y la barra de scroll (modo pantalla completa).

* Los controles pantalla completa no se ocultarán si estas arrastrando el scroll de la lista o del arbol de la bd.

* Ahora el shufle se guardará en las opciones al crear una lista aleatória que requiera el mezclado.

* Ya no se muestra la pista 0 si el medio no tiene pista (tanto en la BD como en la Lista)

* Solucionado bug, que al acabar la lista dejaba los iconos del ultimo medio como si estuviera en pausa.

* Solucionado bug, que al hacer stop seguia mostrando el nombre del medio en la barra del titulo (siempre que la opcion esté activa)

* Retocado comportamiento de los slider del tiempo, ahora al hacer stop su valor será 0, y al terminar el medio su valor será el máximo.

* Solucionado bug, que al acabar un medio no asignaba la imagen del play al boton play / pausa.

* Solucionado bug visual en el arbol y la lista, que en el mouseleave mostraba el fondo del item / nodo de un color distinto al resto del fondo del control. (DAnimacion REBUILD)



### Versión 0.524

* Botón de las opciones fijado al borde inferior del reproductor.

* Ahora se puede ver la lista y la base de datos desde el modo pantalla completa (con SetParent, nada de tener 2 controles de lista y arbol)

* Solucionado bug con las teclas F2 y F3 en pantalla completa, que mostraban la lista o la BD a un tamaño exagerado y no dejaban ver nada.

* Solucionado bug al mostrar el menú de la lista, que seleccionaba 2 items si ya habia uno seleccionado.

* Solucionado bug con los controles desplegables con lista que no mostraban correctamente el foco de su ventana padre.

* Solucionado bug con el control para editar teclas rápidas, que no mostraba correctamente el control al pusar esc para cancelar la edición.

* Agregada opción : Ocultar tooltips al mover el mouse por encima.

* Agregada opción : Mostrar nombre del medio actual en la barra de título.

* Agregada opción : Guardar valores de brillo, contraste, saturación, y proporción para cada medio.

* Agregada opción : Mezclar listas aleatórias por genero.

* Agregada opción : Mezclar listas aleatórias por grupo.

* Agregada opción : Mezclar listas aleatórias por disco.

* Agregada opción : Mezclar listas aleatórias con 50 canciones.

* Agregada opción : Mezclar listas por nota.



### Versión 0.523

* LibVLC actualizada a la versión 3.0.7

* Implementación básica para cargar subtitulos. (con algunos problemas...)

* Creado objeto DDlgAbrir que permite mostrar un dialogo para abrir archivos externos.

* Creada pestaña teclado en las opciones.

* Ya no puedes asignar la misma tecla rápida para mas de una acción.

* La ventana de opciones ya no puede ser redimensionada, que aunque lo fuera conservaba el tamaño inicial de los controles.

* Añadidas teclas F1, F2 y F3 a la lista de teclas rápidas editables en las opciones.

* Ahora se pueden generar listas por Nota > 2.5



### Versión 0.522

* Creado SubMenu Momentos para el menú de la base de datos, que muestra los momentos como sub-menu.

* Creado SubMenu Momentos para el menú del video, que muestra los momentos como sub-menu.

* Ahora no se activará el sub-menu Momentos en el menu de la lista si hay mas de un medio seleccionado.

* Se han fusionado todas las tablas de momentos en una sola, me resultará mas fácil tranajar con una sola tabla que con una por medio, en especial si hay que hacer listas aleatórias.

* Añadidos nuevos iconos para los menús que no tenian icono (aun faltan un par o tres)



### Versión 0.521

* Los momentos ya son funcionales con una precisión de 1000 milisegundos

* Añadidos varios botones para poder modificar el tiempo de los momentos de una forma más precisa.

* Si creas / editas / eliminas un momento del medio que se está reproduciendo, se actualizará automáticamente.



### Versión 0.520

* Solucionado bug visual de los DBotonEx al desactivar-los.

* Ahora al presionar la tecla Intro en la lista, pondrá a reproducir el medio marcado.

* Añadido modo de entrada para los controles de edición de texto, ahora puede ser : Texto, Entero, Decimal, Tiempo

* Añadidos colores para la sombra en los estados (Resaltado, presionado, y desactivado)

* Nueva ventana para poder crear momentos (ahora es posible marcar un momento dentro de un medio para ser recordado en futuras sesiones)

* Añadido modelo de tablas de la BD para los momentos.

* Añadidas nuevas columnas en las opciones para guardar la posición de los momentos.

* Añadida ventana para crear / modificar / eliminar momentos (no es 100% funcional aún...)

* Añadidos tooltips para la ventana de los momentos.



### Versión 0.519

* Ahora se guarda la posición y el tamaño de la ventana para seleccionar directorios.

* La tecla F2 ahora muestra el medio actual en la Base de datos.

* La tecla F3 ahora muestra el medio actual en la Lista.



### Versión 0.518

* Suavizado el drag de la Lista.

* Añadidos temporizadores para mover el scroll si en una operación de drag, si el mouse queda por encima o por debajo del control y hay mas items por explorar.

* Solucionado bug con los botones que se muestran en el preview de la barra de tareas, ahora ya funcionan todos correctamente.

* Los menus que no tienen iconos en sus submenus, ahora ya no dejan el margen a la izquierda para ellos.



### Versión 0.517

* Retocada ventana del error critico, que no mostraba correctamente todo el texto.

* Actualizada la base de datos del reproductor para poder guardar nuevos valores (v1.1)

* Ahora se cargan y se guardan los valores del brillo, saturación, contraste, y proporción de cada video individualmente.

* Solucionado bug a eliminar el ultimo item de la lista.



### Versión 0.516

* Solucionado bug con los controles de pantalla completa, que a veces no se mostraban con la opacidad correcta.

* Añadida posibilidad de arrastrar medios de la Lista y de este modo modificar su posición.

* Creada página sobre el reproductor en el github.



### Versión 0.512

* Añadidos menus para la BD : Abrir carpeta, Nota, y Propiedades(de windows) 100% funcionales.

* Mejorada velocidad de exploración de los directorios al requerir menos datos de la BD para comparaciones.

* Pequeños retoques visuales en la alineación de los controles.



### Versión 0.511

* Agregadas transiciones de resaltado y presionado para los items.

* Mejoras en los calculos del espacio interno de los Menus.



### Versión 0.509

* Creado objeto para poder cargar y pintar iconos facilmente.



### Versión 0.508

* Creado sistema interno para manejar distintos tipos de fuente mas facilmente.

* Añadido objeto skin para todos los controles...



### Versión 0.507

* Al mostrar los tooltips para carpetas ahora se calculan su tiempo total, su tamaño, y su nota media.

* Solucionado bug que mientras se estába cargando el núcleo de la VLC no hacia caso a los medios que se pudieran añadir desde el explorador.

* Transición de resaltado para el fondo del arbol.

* Inicio de un sistema básico para poder hacer skins con relativa facilidad.



### Versión 0.506

* Se ha re-escrito todo el tema de multiseleccion con el shift y varias teclas (cursores, inicio, fin avpag, etc..) del Arbol.

* Ahora se captura el mouse en los eventos MouseDown y MouseUp del Arbol.

* Implementadas transiciones de resaltado y presionado para los nodos del Arbol

* Ahora el Arbol soporta las teclas de la A a la Z para buscar nodos visibles.

* Retocados varios iconos del reproductor.



### Versión 0.505

* Añadido borde con transición de color a los controles que heredan de la barra de scroll (Arbol y Lista).

* Retocados expansores del Arbol, ahora hay 2 tipos de expansor y tienen una transición de resaltado y presionado.

* Solucionado bug con los controles pantalla completa que no se mostraban mas una vez terminado el primer vídeo.

* Pequeños retoques en los margenes y bordes de la barra de scroll.



### Versión 0.504

* Bug en la detección del sistema operativo que solo me permitia identificar hasta windows 8 solucionado. Ahora ya puedo identificar al windows 8.1 y al 10.

* Bug en el orden de los tooltips que a veces podian quedar detras de la ventana principal.

* Barra de scroll re-escrita, antes se podia ajustar del 0% al 100% sin contar el espacio de la página. Ahora se puede ajustar de 0.0 a 1.0 contando el tamaño de la página.

* Añadidas animaciones / transiciones de color en la barra de scroll.

* Retocados controles Lista y Arbol para adaptarse a los cambios de la barra de scroll.

* Retocados los colores de la barra de scroll para que resalten más.

* Los menús ahora comprueban que tengan suficiente espacio a la derecha para mostrar el sub-menu, y en caso contrario lo muestran por la izquierda.



### Versión 0.503

* Varias mejoras en el objeto que crea las animaciones de tiempo, que me ayudaran un poco mas a dar vida al reproductor.

* Bug en los colores de los controles marca, solucionado.

* Ahora los tooltips no desaparecen de golpe al mover la ventana, y se hace una animación.



### Versión 0.502

* La nota de los tooltips ahora se muestra con estrellas en vez de con un valor.

* Varios retoques internos al sistema de tooltips. Entre otras cosas ahora los tooltips ya no se ocultan al pasar por encima con el mouse, puedes hacer click para ocultar-los.



### Versión 0.501

* Retocado el tamaño de la primera columna de los medios para adaptarse mejor a la pista.

* Retoques internos en el manejo del medio actual.



### Versión 0.500

* Primera versión pública.

* Base de datos y escaneo de medios.

* Lista básica con shufle y repeat.

* Listas aleatórias.

* Controles básicos (play, pausa, stop, avanzar medio, retroceder medio, volumen, etc..)

* Modo pantalla completa para los vídeos.

* Tooltips informativos para los medios.