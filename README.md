# Reproductor de Audio y Video Extendido
Reproductor de Audio y Video basado en la [LibVlc](https://www.videolan.org/vlc/index.es.html) para uso personal.

### Requisitos
Sistema operativo Windows 7 o superior (x86. x64)

## Empezando
 1. Descarga una de las siguientes versiones para windows : 
   * [RAVE x86 para 32 bits](http://devildrey33.github.io/RAVE/Instaladores/Instalador%20x86/SetupRave-x86.msi) 
   * [RAVE x64 para 64 bits](http://devildrey33.github.io/RAVE/Instaladores/Instalador%20x64/SetupRave-x64.msi)
 
 2. Instala y ejecuta el reproductor, luego abre la ventana de opciones.
 
 ![Reproductor -> Opciones](http://devildrey33.github.io/RAVE/Img/Readme1.png)
 
 3. Pulsa en 'Agregar Raíz' y selecciona una carpeta de tu ordenador que contenga una colección de música o vídeo (Por ejemplo C:\MP3, C:\Series, etc...), puede ser incluso un disco externo o de red que no este siempre disponible (en caso de no disponibilidad se omitirá).
 
 ![Opciones -> Agregar Raíz](http://devildrey33.github.io/RAVE/Img/Readme2.png)
 
 4. Repite el paso 3 todas las veces que sea necesario, y al terminar pulsa el boton 'Cerrar'. Veras una ventana que muestra el proceso del análisis de los medios, una vez terminado ya podrás crear listas aleatórias pulsando la tecla 'Insert'.
 
 ![Agregar Raíz -> Análisis](http://devildrey33.github.io/RAVE/Img/Readme3.png)

 
 ### Propósito
 * Reproductor diseñado para gestionar agilmente una amplia colección de música y vídeo utilizando su propia base de datos.
 > Al tener todo registrado en una base de datos interna, puedo generar listas aleatórias agilmente, e incluso generar listas de canciones mas escuchadas.
 
 > Tambien preservara los valores del brillo, contraste, saturación, proporción establecidos expresamente para cada video.
  
 > Al reproducir un medio completamente se le sumará una nota de 0.05 de forma que si todos los medios parten de una nota de 2.5, y al reproducir-los 50 veces llegaran a una nota de 5.0. Por supuesto tienes la posibilidad de modificar la nota de cualquier medio o incluso de un grupo de medios a la vez.
 
 ![Nota](http://devildrey33.github.io/RAVE/Img/Readme5.png)
 
 * Poder generar listas de música aleatória según las preferencias establecidas.
 > Con una sola tecla puedo generar una lista de canciones aleatória, que puede ser por genero, por grupo, por disco, o aleatória totalmente (Por defecto es la tecla 'Insert').
 
 > Tambien puedo pulsar el boton 'Lista de medios' con el boton derecho del mouse y crear una lista personalizada.

 ![Listas aleatórias](http://devildrey33.github.io/RAVE/Img/Readme4.png)

 * Capacidad de apagar el reproductor / ordenador al terminar la lista de medios.
 > Asi puedo reproducir un par o tres de mis capitulos favoritos al meterme en la cama, y que el ordenador se apague al finalizar la lista.
 
 ![Nota](http://devildrey33.github.io/RAVE/Img/Readme6.png)
 
### Teclado por defecto
Teclas por defecto cuando el reproductor tiene le foco del teclado (excepto si el foco lo tiene una caja de edición de texto)

 Combinación de teclas       | Acción
---------------------------- | ----------------------------
*Espacio*                    | `play / pausa`
*Insert*                     | `Generar lista aleatória`
*+*                          | `Subir volumen`
*-*                          | `Bajar volumen`
*Control + Cursor derecha*   | `Reproducir siguiente` 
*Control + Cursor izquierda* | `Reproducir anterior`

### FAQ
 * 'Que es una raíz?'
 > Una raíz es un direcotrio 'padre' desde el que el reproductor buscara medios que se puedan reproducir. Puedes añadir mas de una raíz al reproductor, pero aunque es posible... no se recomienda agregar unidades de disco directamente como una raíz.
 
 * 'Si agrego o elimino medios de una raíz, es necesario actualizar la base de datos?' 
 > Si tienes el reproductor encendido mientras has realizado dichos cambios, debes desplegar el menú de la base de datos, y pulsar actualizar.
 > Si no tienes el reproductor encendido, no te preocupes ya que la próxima vez que se inicie re-escaneara la raíz automáticamente para cerciorarse de que todos los medios existen, y de paso para detectar nuevos medios.
 
