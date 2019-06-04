# Log de cambios

### Versi�n 0.519

* Ahora se guarda la posici�n y el tama�o de la ventana para seleccionar directorios.

* La tecla F2 ahora muestra el medio actual en la Base de datos.

* La tecla F3 ahora muestra el medio actual en la Lista.

### Versi�n 0.518

* Suavizado el drag de la Lista.

* A�adidos temporizadores para mover el scroll si en una operaci�n de drag, si el mouse queda por encima o por debajo del control y hay mas items por explorar.

* Solucionado bug con los botones que se muestran en el preview de la barra de tareas, ahora ya funcionan todos correctamente.

* Los menus que no tienen iconos en sus submenus, ahora ya no dejan el margen a la izquierda para ellos.

### Versi�n 0.517

* Retocada ventana del error critico, que no mostraba correctamente todo el texto.

* Actualizada la base de datos del reproductor para poder guardar nuevos valores (v1.1)

* Ahora se cargan y se guardan los valores del brillo, saturaci�n, contraste, y proporci�n de cada video individualmente.

* Solucionado bug a eliminar el ultimo item de la lista.

### Versi�n 0.516

* Solucionado bug con los controles de pantalla completa, que a veces no se mostraban con la opacidad correcta.

* A�adida posibilidad de arrastrar medios de la Lista y de este modo modificar su posici�n.

* Creada p�gina sobre el reproductor en el github.

### Versi�n 0.512

* A�adidos menus para la BD : Abrir carpeta, Nota, y Propiedades(de windows) 100% funcionales.

* Mejorada velocidad de exploraci�n de los directorios al requerir menos datos de la BD para comparaciones.

* Peque�os retoques visuales en la alineaci�n de los controles.

### Versi�n 0.511

* Agregadas transiciones de resaltado y presionado para los items.

* Mejoras en los calculos del espacio interno de los Menus.

### Versi�n 0.509

* Creado objeto para poder cargar y pintar iconos facilmente.

### Versi�n 0.508

* Creado sistema interno para manejar distintos tipos de fuente mas facilmente.

* A�adido objeto skin para todos los controles...

### Versi�n 0.507

* Al mostrar los tooltips para carpetas ahora se calculan su tiempo total, su tama�o, y su nota media.

* Solucionado bug que mientras se est�ba cargando el n�cleo de la VLC no hacia caso a los medios que se pudieran a�adir desde el explorador.

* Transici�n de resaltado para el fondo del arbol.

* Inicio de un sistema b�sico para poder hacer skins con relativa facilidad.

### Versi�n 0.506

* Se ha re-escrito todo el tema de multiseleccion con el shift y varias teclas (cursores, inicio, fin avpag, etc..) del Arbol.

* Ahora se captura el mouse en los eventos MouseDown y MouseUp del Arbol.

* Implementadas transiciones de resaltado y presionado para los nodos del Arbol

* Ahora el Arbol soporta las teclas de la A a la Z para buscar nodos visibles.

* Retocados varios iconos del reproductor.

### Versi�n 0.505

* A�adido borde con transici�n de color a los controles que heredan de la barra de scroll (Arbol y Lista).

* Retocados expansores del Arbol, ahora hay 2 tipos de expansor y tienen una transici�n de resaltado y presionado.

* Solucionado bug con los controles pantalla completa que no se mostraban mas una vez terminado el primer v�deo.

* Peque�os retoques en los margenes y bordes de la barra de scroll.

### Versi�n 0.504

* Bug en la detecci�n del sistema operativo que solo me permitia identificar hasta windows 8 solucionado. Ahora ya puedo identificar al windows 8.1 y al 10.

* Bug en el orden de los tooltips que a veces podian quedar detras de la ventana principal.

* Barra de scroll re-escrita, antes se podia ajustar del 0% al 100% sin contar el espacio de la p�gina. Ahora se puede ajustar de 0.0 a 1.0 contando el tama�o de la p�gina.

* A�adidas animaciones / transiciones de color en la barra de scroll.

* Retocados controles Lista y Arbol para adaptarse a los cambios de la barra de scroll.

* Retocados los colores de la barra de scroll para que resalten m�s.

* Los men�s ahora comprueban que tengan suficiente espacio a la derecha para mostrar el sub-menu, y en caso contrario lo muestran por la izquierda.

### Versi�n 0.503

* Varias mejoras en el objeto que crea las animaciones de tiempo, que me ayudaran un poco mas a dar vida al reproductor.

* Bug en los colores de los controles marca, solucionado.

* Ahora los tooltips no desaparecen de golpe al mover la ventana, y se hace una animaci�n.

### Versi�n 0.502

* La nota de los tooltips ahora se muestra con estrellas en vez de con un valor.

* Varios retoques internos al sistema de tooltips. Entre otras cosas ahora los tooltips ya no se ocultan al pasar por encima con el mouse, puedes hacer click para ocultar-los.


### Versi�n 0.501

* Retocado el tama�o de la primera columna de los medios para adaptarse mejor a la pista.

* Retoques internos en el manejo del medio actual.

### Versi�n 0.500

* Primera versi�n p�blica.

* Base de datos y escaneo de medios.

* Lista b�sica con shufle y repeat.

* Listas aleat�rias.

* Controles b�sicos (play, pausa, stop, avanzar medio, retroceder medio, volumen, etc..)

* Modo pantalla completa para los v�deos.

* Tooltips informativos para los medios.