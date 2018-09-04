#pragma once

// Fuente por defecto
#define FUENTE_NORMAL			L"tahoma"
#define FUENTE_NORMAL_TAM		21
#define FUENTE_PEQUE_TAM		18
// Tamaño ancho / alto de la barra de scroll (ancho si es vertical, alto si es horizontal)
//#define TAM_BARRA_SCROLL		16

#define COLOR_ROJO									RGB(234,  80,  78)
#define COLOR_ROJO_RESALTADO						RGB(254, 100,  98)
#define COLOR_ROJO_PRESIONADO						RGB(214,  60,  58)
#define COLOR_ROJO_MARCADO							RGB(155,   0,   0)

// Colores básicos para las ventanas y los controles
#define COLOR_FONDO									RGB( 31,  29,  34)
#define COLOR_FONDO_CLARO							RGB(240, 240, 240)
#define COLOR_FONDO_CLARO_RESALTADO					RGB(255, 255, 255)
#define COLOR_FONDO_CLARO_PRESIONADO				RGB(220, 220, 220)
#define COLOR_BORDE									RGB( 53,  51,  56)	//RGB(100, 100, 100)
#define COLOR_BORDE_RESALTADO						RGB(255,   0,   0)
#define COLOR_BORDE_PRESIONADO						RGB(150,   0,   0)
#define COLOR_TEXTO									RGB(235, 235, 235) //RGB(255, 255, 255)
#define COLOR_TEXTO_SOMBRA							RGB(  0,   0,   0) //RGB(255, 255, 255)
#define COLOR_TEXTO_RESALTADO						RGB(255, 255, 255) //RGB(255, 255, 255)
#define COLOR_TEXTO_PRESIONADO						RGB(215, 215, 215) //RGB(255, 255, 255)
#define COLOR_TEXTO_DESACTIVADO						RGB(100, 100, 100) //RGB(255, 255, 255)

#define COLOR_BORDE_POPUP                           RGB(255, 255, 255)

 
/*
------------------------------------------------------------------------------------------
  Creador de texte ascii : http://patorjk.com/software/taag/#p=display&f=Big&t=ToolTipEx
------------------------------------------------------------------------------------------

____                       ______
|  _ \                     |  ____|
| |_) | __ _ _ __ _ __ __ _| |__  __  __
|  _ < / _` | '__| '__/ _` |  __| \ \/ /
| |_) | (_| | |  | | | (_| | |____ >  <
|____/ \__,_|_|  |_|  \__,_|______/_/\_\

*/


#define COLOR_BARRA									COLOR_ROJO
#define COLOR_BARRA_RESALTADO						RGB(244,  90,  88) //RGB(  0,   0, 230)
#define COLOR_BARRA_PRESIONADO						RGB(224,  70,  68) //RGB(  0,   0, 160)
#define COLOR_BARRA_FONDO							COLOR_FONDO_CLARO  //RGB(190, 190, 190)
#define COLOR_BARRA_FONDO_RESALTADO					COLOR_FONDO_CLARO_RESALTADO //RGB(225, 225, 225)
#define COLOR_BARRA_FONDO_PRESIONADO				COLOR_FONDO_CLARO_PRESIONADO //RGB(160, 160, 160)

/*
*/
#define COLOR_TOOLTIP_BORDE							COLOR_BORDE_POPUP //RGB(155, 155, 155)
#define COLOR_TOOLTIP_FONDO							RGB( 21,  19,  24) //COLOR_FONDO
#define COLOR_TOOLTIP_TEXTO							COLOR_TEXTO //RGB(  0,   0,   0)

/*
  ____        _              ______      
 |  _ \      | |            |  ____|     
 | |_) | ___ | |_ ___  _ __ | |__  __  __
 |  _ < / _ \| __/ _ \| '_ \|  __| \ \/ /
 | |_) | (_) | || (_) | | | | |____ >  < 
 |____/ \___/ \__\___/|_| |_|______/_/\_\

*/
#define COLOR_BOTON									RGB( 17,  15,  20) //RGB(220, 220, 220)
#define COLOR_BOTON_RESALTADO						COLOR_ROJO
#define COLOR_BOTON_PRESIONADO						RGB(204,  50,  48)
#define COLOR_BOTON_TEXTO							COLOR_TEXTO            //RGB(235, 235, 235) //RGB(  0,   0,   0)
#define COLOR_BOTON_TEXTO_RESALTADO					COLOR_TEXTO_RESALTADO  //RGB(255, 255, 255) //RGB(100, 100, 100)
#define COLOR_BOTON_TEXTO_PRESIONADO				COLOR_TEXTO_PRESIONADO //RGB( 50,  50,  50)

/*
  ____                        _____                _ _ ______      
 |  _ \                      / ____|              | | |  ____|     
 | |_) | __ _ _ __ _ __ __ _| (___   ___ _ __ ___ | | | |__  __  __
 |  _ < / _` | '__| '__/ _` |\___ \ / __| '__/ _ \| | |  __| \ \/ /
 | |_) | (_| | |  | | | (_| |____) | (__| | | (_) | | | |____ >  < 
 |____/ \__,_|_|  |_|  \__,_|_____/ \___|_|  \___/|_|_|______/_/\_\

*/

#define COLOR_SCROLL_FONDO							RGB(200, 200, 200)
#define COLOR_SCROLL_FONDO_RESALTADO				RGB(210, 210, 210)
#define COLOR_SCROLL_FONDO_PRESIONADO				RGB(190, 190, 190)
#define COLOR_SCROLL_BARRA							RGB(160, 160, 160)
#define COLOR_SCROLL_BARRA_RESALTADO				RGB(180, 180, 180)
#define COLOR_SCROLL_BARRA_PRESIONADO				RGB(140, 140, 140)

/*              _           _ ______      
     /\        | |         | |  ____|     
    /  \   _ __| |__   ___ | | |__  __  __
   / /\ \ | '__| '_ \ / _ \| |  __| \ \/ /
  / ____ \| |  | |_) | (_) | | |____ >  < 
 /_/    \_\_|  |_.__/ \___/|_|______/_/\_\    

*/
#define COLOR_ARBOL_FONDO							COLOR_FONDO_CLARO
//#define COLOR_ARBOL_FONDO_SCROLL					RGB(200, 200, 200)
//#define COLOR_ARBOL_FONDO_SCROLL_RESALTADO		RGB(170, 170, 170)
//#define COLOR_ARBOL_FONDO_SCROLL_PRESIONADO		RGB(110, 110, 110)

#define COLOR_ARBOL_FONDO_RESALTADO					COLOR_FONDO_CLARO_RESALTADO
#define COLOR_ARBOL_FONDO_PRESIONADO				COLOR_FONDO_CLARO_PRESIONADO
#define COLOR_ARBOL_TEXTO							RGB( 30,  30,  30)
#define COLOR_ARBOL_TEXTO_DESACTIVADO				RGB(128, 128, 128)
#define COLOR_ARBOL_TEXTO_SOMBRA					RGB(255, 255, 255)
#define COLOR_ARBOL_TEXTO_RESALTADO					RGB( 50,  50,  50)
//#define COLOR_ARBOL_TEXTO_PRESIONADO				RGB(  0,   0,   0) // El texto presionado está siempre seleccionado

#define COLOR_ARBOL_SELECCION						COLOR_ROJO
#define COLOR_ARBOL_SELECCION_RESALTADO				COLOR_ROJO_RESALTADO
#define COLOR_ARBOL_SELECCION_PRESIONADO			COLOR_ROJO_PRESIONADO
#define COLOR_ARBOL_SUBSELECCION					RGB(220,  170, 168)
#define COLOR_ARBOL_SUBSELECCION_RESALTADO			RGB(245,  190, 188)
//#define COLOR_ARBOL_SUBSELECCION_PRESIONADO			RGB(205,  130, 128) // El texto presionado está siempre seleccionado

#define COLOR_ARBOL_SELECCION_TEXTO					RGB(230, 230, 230)
#define COLOR_ARBOL_SELECCION_TEXTO_SOMBRA			RGB( 64,  64,  64)
#define COLOR_ARBOL_SELECCION_TEXTO_RESALTADO		RGB(255, 255, 255)
#define COLOR_ARBOL_SELECCION_TEXTO_PRESIONADO		RGB(190, 190, 190)
#define COLOR_ARBOL_SUBSELECCION_TEXTO				RGB(230, 230, 230)
#define COLOR_ARBOL_SUBSELECCION_TEXTO_SOMBRA		RGB(  0,   0,   0)
#define COLOR_ARBOL_SUBSELECCION_TEXTO_RESALTADO	RGB(255, 255, 255)
//#define COLOR_ARBOL_SUBSELECCION_TEXTO_PRESIONADO	RGB(190, 190, 190)

#define COLOR_ARBOL_EXPANSOR_BORDE					RGB(128, 128, 128)
#define COLOR_ARBOL_EXPANSOR_BORDE_RESALTADO		RGB(255,   0,   0)
#define COLOR_ARBOL_EXPANSOR_BORDE_PRESIONADO		RGB(200,   0,   0)

#define COLOR_ARBOL_NODO_MARCA						RGB(255,   0,   0)
//#define COLOR_ARBOL_NODO_MARCA_SELECCIONADO			RGB(  0,   0 ,  0)

/*
  _      _     _        ______      
 | |    (_)   | |      |  ____|     
 | |     _ ___| |_ __ _| |__  __  __
 | |    | / __| __/ _` |  __| \ \/ /
 | |____| \__ \ || (_| | |____ >  < 
 |______|_|___/\__\__,_|______/_/\_\                                                                

*/

#define COLOR_LISTA_FONDO							COLOR_FONDO_CLARO
//#define COLOR_LISTA_FONDO_SCROLL					RGB(200, 200, 200)
#define COLOR_LISTA_FONDO_RESALTADO					COLOR_FONDO_CLARO_RESALTADO
#define COLOR_LISTA_FONDO_PRESIONADO                COLOR_FONDO_CLARO_PRESIONADO// Solo cuando MostrarSeleccion está desactivado

#define COLOR_LISTA_TEXTO							RGB( 30,  30,  30)
#define COLOR_LISTA_TEXTO_SOMBRA					RGB(255, 255, 255)
#define COLOR_LISTA_TEXTO_RESALTADO					RGB( 50,  50,  50)

#define COLOR_LISTA_SELECCION						COLOR_ROJO
#define COLOR_LISTA_SELECCION_RESALTADO				COLOR_ROJO_RESALTADO
#define COLOR_LISTA_SELECCION_PRESIONADO			COLOR_ROJO_PRESIONADO

#define COLOR_LISTA_SELECCION_TEXTO					RGB(230, 230, 230)
#define COLOR_LISTA_SELECCION_TEXTO_SOMBRA			RGB( 64,  64,  64)
#define COLOR_LISTA_SELECCION_TEXTO_RESALTADO		RGB(255, 255, 255)
#define COLOR_LISTA_SELECCION_TEXTO_PRESIONADO		RGB(190, 190, 190)

#define COLOR_LISTA_NODO_MARCA						RGB(255,   0,   0)

/* 
______    _ _      _          _______        _        ______
|  ____|  | (_)    (_)        |__   __|      | |      |  ____|
| |__   __| |_  ___ _  ___  _ __ | | _____  _| |_ ___ | |__  __  __
|  __| / _` | |/ __| |/ _ \| '_ \| |/ _ \ \/ / __/ _ \|  __| \ \/ /
| |___| (_| | | (__| | (_) | | | | |  __/>  <| || (_) | |____ >  <
|______\__,_|_|\___|_|\___/|_| |_|_|\___/_/\_\\__\___/|______/_/\_\

*/

#define COLOR_EDICION_CURSOR						COLOR_ROJO
#define COLOR_EDICION_FONDO							COLOR_FONDO_CLARO
#define COLOR_EDICION_TEXTO							RGB( 30,  30,  30)
#define COLOR_EDICION_TEXTO_SOMBRA					RGB(255, 255, 255)
#define COLOR_EDICION_SELECCION						COLOR_ROJO
#define COLOR_EDICION_SELECCION_TEXTO				RGB(230, 230, 230)
#define COLOR_EDICION_SELECCION_TEXTO_SOMBRA		RGB( 64,  64,  64)



/*
  __  __                  ______      
 |  \/  |                |  ____|     
 | \  / | ___ _ __  _   _| |__  __  __
 | |\/| |/ _ \ '_ \| | | |  __| \ \/ /
 | |  | |  __/ | | | |_| | |____ >  < 
 |_|  |_|\___|_| |_|\__,_|______/_/\_\
                                      
*/                                      

#define COLOR_MENU_BORDE                            COLOR_BORDE_POPUP
#define COLOR_MENU_SEPARADOR                        RGB(150, 150, 150)
#define COLOR_MENU_FONDO							COLOR_FONDO
#define COLOR_MENU_FONDO_RESALTADO					COLOR_ROJO_RESALTADO
#define COLOR_MENU_FONDO_PRESIONADO					COLOR_ROJO_PRESIONADO
#define COLOR_MENU_TEXTO							COLOR_TEXTO
#define COLOR_MENU_TEXTO_SOMBRA						COLOR_TEXTO_SOMBRA
#define COLOR_MENU_TEXTO_RESALTADO					COLOR_TEXTO_RESALTADO
#define COLOR_MENU_TEXTO_PRESIONADO					COLOR_TEXTO_PRESIONADO
#define COLOR_MENU_TEXTO_DESACTIVADO				COLOR_TEXTO_DESACTIVADO