#pragma once

#include "ArbolBD.h"
#include "ListaMedios.h"
#include "DMenuEx.h"
#include "VerVideo.h"
#include "MarcoControles.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"
#include "DBotonEx.h"
#include "DEtiquetaEx.h"
#include "ListaRaices.h"
#include "ThreadActualizarArbol.h"
#include "ThreadAgregarArchivosLista.h"
#include "ThreadAnalisis.h"
#include "VentanaMomento.h"

#ifdef RAVE_VLC_DOBLE_MEDIO_FFT
	#include "ControlVisualizacion.h"
#endif

using namespace DWL;

class VentanaPrincipal : public DWL::DVentana {
 public:						// Constructor
								VentanaPrincipal(void) : DWL::DVentana(), _PantallaCompleta(FALSE), /*EstadoMedio(EnStop),*/ MonitorDisponible(FALSE), _MaximoTotalMedios2(0), _ValorMedios2(0), RectMonitorActual({ 0,0,0,0 }), CTW_ExtraX(0), CTW_ExtraY(0), _BotonExMouseDownTick(0) {};
								// Destructor
					           ~VentanaPrincipal(void) { };
								// Función para crear la ventana principal
	HWND						Crear(int nCmdShow);
								// Función que crea los botones play, stop, av medio, re medio en el thumbnail de la barra de tareas
	void						CrearBotonesThumb(void);
								// Evento que recibe las pulsaciones del thumbnauil de la barra de tareas (nada mas, este reproductor no tiene controles estandar)
	void                        Evento_Comando(WPARAM wParam, LPARAM lParam);
								// Evento que recibe cuando se pulsa sobre un menú extendido
	void						Evento_MenuEx_Click(const UINT cID);
								// Evento que recibe cuando se está cambiando una barra dentro de un menu extendido
	void						Evento_MenuEx_Barra_Cambiando(const UINT cID, const float ValorBarra);
								// Evento que recibe cuando se ha cambiado una barra dentro de un menu extendido
	void						Evento_MenuEx_Barra_Cambiado(const UINT cID, const float ValorBarra);
								// Evento que recibe cuando se está haciendo click en un boton extendido
	void						Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se presiona un boton del mouse en un boton extendido
	void                        Evento_BotonEx_Mouse_Presionado(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se suelta un boton del mouse en un boton extendido
	void                        Evento_BotonEx_Mouse_Soltado(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se está cmabiando el tamaño de la ventana
	void						Evento_CambiandoTam(const UINT Lado, RECT *Rectangulo);
								// Evento que recibe cuando se ha cambiado la barra del tiempo
	void						Evento_SliderTiempo_Cambiado(void);
								// Evento que recibe cuando se ha cambiado la barra del volumen
	void						Evento_SliderVolumen_Cambiado(void);
								// Evento que recibe cuando se está cambiando la barra del volumen
	void						Evento_SliderVolumen_Cambiando(void);
								// Evento que se recibe cuando se está moviendo la rueda del mouse dentro de la barra del volumen
	void						Evento_SliderVolumen_Rueda(DEventoMouseRueda& DatosMouse);
								// Evento que se recibe cuando se sueltan archivos desde el explorador a esta ventana
	void						Evento_SoltarArchivos(WPARAM wParam);
								// Evento que se recibe cuando es necesario borrar el fondo de esta ventana
	void						Evento_BorraFondo(HDC DC);
								// Evento que se recibe cuando se cierra esta ventana
	void						Evento_Cerrar(void);
								// Evento que recibe cuando se está cambiando una barra extendida
	void						Evento_BarraEx_Cambiando(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se ha cambiado una barra extendida
	void						Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se ha movido la rueda del mouse dentro de una barra extendida
	void						Evento_BarraEx_Rueda(DWL::DEventoMouseRueda& DatosMouse);

								// Mensaje que se recibe del thread agregar archivos, que indica que se ha terminado
	void						ThreadAgregarArchivosLista_Terminado(void);
								// Mensaje que se recibe del thread agregar archivos, que indica que se ha agregado un nuevo medio
	void						ThreadAgregarArchivosLista_AgregarMedio(WPARAM wParam);
								// Mensaje que se recibe del thread buscar archivos, que indica que se ha agregado una raíz
	void						ThreadABuscarArchivos_AgregarRaiz(LPARAM lParam);
								// Mensaje que se recibe del thread buscar archivos, que indica que se ha agregado un directorio
	void						ThreadABuscarArchivos_AgregarDirectorio(LPARAM lParam);
								// Mensaje que se recibe del thread actualizar arbol que indica que se ha agregado una lista al historial
	void						ThreadABuscarArchivos_AgregarHistorial(LPARAM lParam);
								// Mensaje que se recibe del thread buscar archivos, que indica que se ha terminado
	void						ThreadABuscarArchivos_Terminado(const BOOL Cancelado, LPARAM lParam);
								// Mensaje que se recibe del thread analizar, que indica que se ha terminado
	void						ThreadAnalizar_Terminado(const BOOL Cancelado, LPARAM lParam);
								// Mensaje que se recibe del thread analizar, que indica que se tiene que mostrar la ventana del analisis
	void						ThreadAnalizar_MostrarVentana(void);
								// Mensaje que se recibe del thread analizar, que indica que ha empezado el análisis
	void                        ThreadAnalizar_Iniciado2(WPARAM wParam);
								// Mensaje que se recibe del thread analizar con el total de medios de la fase 1
	void						ThreadAnalizar_TotalMedios(WPARAM wParam, LPARAM lParam);
								// Mensaje que se recibe del thread analizar con el total de medios de la fase 2
	void						ThreadAnalizar_TotalMedios2(void);
								// Función que ajusta los controles al tamaño de la recta especificada
	void						AjustarControles(RECT &RC);
								// Función que genera una lista aleatória del tipo especificado
	void                        GenerarListaAleatoria(const TipoListaAleatoria nTipo = TLA_LoQueSea);
								// Función que restablece los filtros de video a sus valores por defecto
	void						FiltrosVideoPorDefecto(void);
								// Función para pasar del modo normal al modo pantalla completa
	void						PantallaCompleta(const BOOL nActivar);
								// Función que devuelve si el reproductor está en pantalla completa o en modo normal
	inline const BOOL			PantallaCompleta(void) { return _PantallaCompleta;  }
								// Función que ejectura el repeat
	void                        Repeat(void);
								// Función que recibe cuando se pulsa en el boton reprtir
	void						Repetir_Click(void);
								// Función que recibe cuando se pulsa en el boton mezclar
	void                        Mezclar_Click(void);
								// Función que mezcla / restaure la lista de reproducción, y marca / desmarca los botones mezclar.
	void                        Mezclar(const BOOL nMezclar);
								// Menu extendido para las opciones del repeat
	DWL::DMenuEx				Menu_Repetir;
								// Menu extendido para las opciones del arbol de la base de datos
	DWL::DMenuEx 				Menu_ArbolBD;
								// Menu extendido para las opciones de la lista de reproducción			
	DWL::DMenuEx                Menu_Lista;
								// Menu extendido para las opciones del vídeo
	DWL::DMenuEx                Menu_Video;
								// Menu extendido para el boton de la lista
	DWL::DMenuEx                Menu_BotonLista;
								// Menu extendido para el boton de la base de datos
	DWL::DMenuEx                Menu_BotonArbolBD;
								// Función que actualiza el arbol de la base de datos
	void						ActualizarArbol(void);
								// Función para iniciar el análisis de la base de datos (ThreadAnalisis)
	void                        AnalizarBD(void);
								// Funciónes para mostrar un medio en la base de datos
	const BOOL                  Arbol_MostrarMedio(BDMedio &mMedio);
	const BOOL                  Arbol_MostrarMedio(const sqlite3_int64 Hash);
								// Función para agregar un nodo a la lista de reproducción
	void                        Arbol_AgregarALista(const BOOL NuevaLista);
								// Función para abrir la carpeta del nodo especificado
	void						Arbol_AbrirCarpeta(void);
								// Función para mostrar un tooltip con información del nodo
	void						Arbol_Informacion(void);
								// Función para abrir la ventana de propiedades de windows del nodo especificado (puede ser un directorio o un medio)
	void						Arbol_Propiedades(void);
								// Función para asignar la nota de un nodo (si se hace en un directorio, se asignará la nota para todos sus medios)
	void                        Arbol_AsignarNota(const float nNota);
								// Función para abrir la ventana de momentos
	void                        Arbol_Momentos(void);
								// Función que agrega un directorio al arbol de la base de datos
	NodoBD                     *Arbol_AgregarDir(std::wstring *Path, const BOOL nRepintar = FALSE);
								// Función que agrega una raíz al arbol de la base de datos
	NodoBD                     *Arbol_AgregarRaiz(std::wstring *Path);
								// Función que agrega una lista del historial al arbol de la base de datos
	NodoBD                     *Arbol_AgregarHistorial_Lista(Historial_Lista &Lista, const BOOL AlFinal = TRUE);
								// Función que agrega una medio del historial al arbol de la base de datos
	NodoBD                     *Arbol_AgregarHistorial_Medio(Historial_Lista &Lista, Historial_Medio &nMedio, const BOOL AlFinal = TRUE);

								// Thread para actualizar el arbol de la base de datos
	ThreadActualizarArbol		ThreadActualizar;
								// Thread para añadir archivos externos a la lista
	ThreadAgregarArchivosLista  ThreadArchivosLista;
								// Thread para analizar los medios
	ThreadAnalisis				ThreadAnalizar;
								// Función que inicia el thread de descarga (sin mostrar la ventana)
	void						Actualizacion_DescargaEncontrada(const wchar_t* nVersion);
								// Función que descarga una nueva actualización
	void						Actualizacion_MostrarDescargar(const wchar_t *nVersion);
								// Función que mantiene la barra de descarga de la actualización
	void						Actualizacion_Barra(const float nValor);
								// Función que muestra un mensaje de error de la actualización
	void						Actualizacion_Error(void);
								// Función que muestra un mensaje conforme se ha cancelado la actualización
	void						Actualizacion_Cancelada(void);
								// Función que avisa al usuario de que se ha descargado la actualización
	void						Actualizacion_Descargada(void);
								// Función que muestra la ventana que pide al usuario instalar la actualización
	void						Actualizacion_Existente(void);
								// Función que muestra la ventana para abrir URL's
	void						MostrarVentanaURL(void);

								// Función que enumera las pantallas disponibles, y busca una donde ubicar el reproductor
	static BOOL CALLBACK		EnumerarPantallas(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
								// Función que ejecuta el siguiente medio de la lista (el primero si no se ha ejecutado ninguno)
	void						Lista_Siguiente(void);
								// Función que ejecuta el anterior medio de la lista (el ultimo si no se ha ejecutado ninguno)
	void						Lista_Anterior(void);
								// Función que hace play en el medio actual
	void						Lista_Play(void);
								// Función que hace pausa en el medio actual
	void						Lista_Pausa(void);
								// Función que para el medio actual
	void						Lista_Stop(void);
								// Función para mostrar el tooltip del medio marcado en la lista
	void						Lista_Informacion(void);
								// Función que muestra las propiedades el medio especificado
	void                        Lista_Propiedades(void);
								// Función que abre la carpeta del medio especificado
	void                        Lista_AbrirEnExplorador(void);
								// Función que muestra en la base de datos el medio especificado
	void						Lista_MostrarEnBaseDatos(void);
								// Función que elimina todos los medios seleccionados de la lista
	void                        Lista_EliminarSeleccionados(void);
								// Función que asigna la nota especificada a los medios seleccionados en la lista
	void                        Lista_AsignarNota(const float nNota);
								// Función que muestra la ventana de los momentos con el medio marcado en la lista
	void                        Lista_Momentos(void);
								// Función que muestra la ventana de los momentos con el medio especificado
	void                        Lista_MomentosAbrir(const UINT64 IdMedio, const int PosMomento);
								// Función que muestra la ventana de los momentos
	void                        Video_Momentos(void);
								// Función que muestra el dialogo para abrir archivos y seleccionar un subtitulo
	void                        Video_Subtitulos(void);
								// Ventana para los momentos
	VentanaMomento              Momentos;
								// Agrega un medio a la lista desde el explorador
	void						ExploradorAgregarMedio(const BOOL Reproducir);
								// Boton para el play
	DWL::DBotonEx				BotonPlay;
								// Boton stop
	DWL::DBotonEx				BotonStop;
								// Boton avanzar medio
	DWL::DBotonEx				BotonAdelante;
								// Boton retroceder medio
	DWL::DBotonEx				BotonAtras;
								// Label que muestra el ratio actual
	DWL::DEtiquetaEx			LabelRatio;
								// Boton para mezclar la lista
	DWL::DBotonEx				BotonMezclar;
								// Boton con las opciones para reprtir la lista
	DWL::DBotonEx				BotonRepetir;
								// Slider para ver / modificar el tiempo actual
	BarraTiempo					SliderTiempo;
								// Marco que contiene el slider del volumen y varios labels
	MarcoControles				MarcoSD;
								// Marco que contiene los controles play pausa stop
	MarcoControles				MarcoSI;
								// Marco que contiene los botones lista bd video
	MarcoControles				MarcoII;
								// Slider para el volumen
	BarraVolumen				SliderVolumen;
								// Label para mostrar el porcentaje actual del volumen
	DWL::DEtiquetaEx			LabelVolumen;
								// Label para mostrar el tiempo actual
	DWL::DEtiquetaEx			LabelTiempoActual;
								// Label con una barra para separar el tiempo actual del total
	DWL::DEtiquetaEx			LabelTiempoSeparador;
								// Label para mostrar el tiempo total
	DWL::DEtiquetaEx			LabelTiempoTotal;
								// Boton base de datos
	DWL::DBotonEx				BotonBD;
								// Boton lista
	DWL::DBotonEx				BotonLista;
								// Boton ver video
	DWL::DBotonEx				BotonVideo;
								// Boton opciones
	DWL::DBotonEx				BotonOpciones;
								// Control para la lista de medios
	ListaMedios					Lista;
								// Control para el video
	VerVideo					Video;
								// Control para el arbol de la base de datos
	ArbolBD 					Arbol;
								// Función que crea los skins oscuros para el arbol y la lista 
	void                        CrearSkins(void);
								// Skin claro para la lista
	DWL::DListaEx_Skin          ListaSkinClaro;
								// Skin oscuro para la lista
	DWL::DListaEx_Skin          ListaSkinOscuro;
								// Skin claro para el arbol
	DWL::DArbolEx_Skin          ArbolSkinClaro;
								// Skin oscuro para el arbol
	DWL::DArbolEx_Skin          ArbolSkinOscuro;
								// Skin claro para la barra de scroll
	DWL::DBarraScrollEx_Skin    ScrollSkinClaro;
								// Skin oscuro para la barra de scroll
	DWL::DBarraScrollEx_Skin    ScrollSkinOscuro;
								// Variable que determina si el monitor donde se ha creado la ventana principal está disponible
	BOOL						MonitorDisponible;
								// Medidas del monitor actual
	RECT						RectMonitorActual;

								// Client to Window rect
								// Guardo la diferencia de ancho y altura entre el client rect y el window rect
	int							CTW_ExtraX;
	int							CTW_ExtraY;
								// Función para mostrar el marco especificado (lista, bd, video)
	void					    MostrarMarco(const INT_PTR ID);
								// Gestor de mensajes para esta ventana
	LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	#ifdef RAVE_VLC_DOBLE_MEDIO_FFT
		ControlVisualizacion	Vis;
	#endif

  protected:
								// Evento WM_SIZE
	void					   _Evento_Size(void);
								// Evento WM_EXITSIZEMOVE
	void					   _Evento_ExitSizeMove(void);
								// Variable que determina si se ha entrado en modo patalla completa
	BOOL                       _PantallaCompleta;		
								// Ultimo tick del sistema al presionar el boton del mouse
	ULONGLONG                  _BotonExMouseDownTick;	
								// Total de medios en la fase 2 del analisis
	UINT                       _MaximoTotalMedios2;	
								// Valor actual de los medios analizados en la fase 2
	UINT                       _ValorMedios2;			
};

