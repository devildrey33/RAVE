# Reproductor de Audio y Video Extendido
Reproductor de Audio y Video basado en la [LibVlc](https://www.videolan.org/vlc/index.es.html) para uso personal.

### Requisitos
Sistema operativo Windows 7 o superior (x86. x64)

## Empezando
 1. Descarga una de las siguientes versiones para windows : 
   *[RAVE x86 para 32 bits](http://devildrey33.github.io/RAVE/Instaladores/Instalador%20x86/SetupRave-x86.msi) 
   *[RAVE x64 para 64 bits](http://devildrey33.github.io/RAVE/Instaladores/Instalador%20x64/SetupRave-x64.msi)
 
 2. Instala y ejecuta el reproductor, luego abre la ventana de opciones.
 
 ![Reproductor -> Opciones](http://devildrey33.github.io/RAVE/Img/Readme1.png)
 
 3. Pulsa en 'Agregar Raíz' y selecciona una carpeta de tu ordenador que contenga una colección de música o vídeo (Por ejemplo C:\MP3, C:\Series, etc...), puede ser incluso un disco externo o de red que no este siempre disponible (en caso de no disponibilidad se omitirá).
 
 ![Opciones -> Agregar Raíz](http://devildrey33.github.io/RAVE/Img/Readme2.png)
 
 4. Repite el paso 3 todas las veces que sea necesario, y al terminar pulsa el boton 'Cerrar'. Veras una ventana que muestra el proceso del análisis de los medios, una vez terminado ya podrás crear listas aleatórias pulsando la tecla 'Insert'.
 
 ![Agregar Raíz -> Análisis](http://devildrey33.github.io/RAVE/Img/Readme3.png)

 
 ### Propósito
 * Reproductor diseñado para gestionar agilmente una amplia colección de música y vídeo.
 > Al tener todo registrado en una base de datos interna, puedo generar listas aleatórias agilmente, e incluso generar listas de canciones mas escuchadas, además de preservar los valores del brillo, contraste, saturación, proporción establecidos expresamente para el medio.
 
 > Al reproducir un medio completamente se le sumará una nota de 0.1 de forma que si todos los medios parten de una nota de 2.0 al reproducir-los 30 veces llegaran a una nota de 5.0, y por supuesto tienes la posibilidad de modificar la nota de cualquier medio.
 
 * Poder generar listas de música aleatória según las preferencias establecidas.
 > Con una sola tecla puedo generar una lista de canciones aleatória, que puede ser por genero, por grupo, por disco, o aleatória totalmente.
 
 > Tambien puedo pulsar el boton 'Lista de medios' con el boton derecho del mouse y crear una lista personalizada.
 
 * Capacidad de apagar el reproductor / ordenador al terminar la lista de medios.
 > Asi puedo reproducir un par o tres de mis capitulos favoritos al meterme en la cama, y que el ordenador se apague al finalizar la lista.
 
 
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
 * *Que es una raíz*, Una raíz es un direcotrio 'padre' desde el que el reproductor buscara medios que se puedan reproducir.
 
 * *Si agrego nuevos medios a una raíz, es necesario hacer algo?* La respuesta es NO, todo esto se hace automáitcamente por lo que no debes preocuparte por ello. El reproductor escanea todas las racices agregadas previamente cada vez que se inicia para buscar nuevos medios y para comprobar que los medios registrados anteriormente existen.
 
