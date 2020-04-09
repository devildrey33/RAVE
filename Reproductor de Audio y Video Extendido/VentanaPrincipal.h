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
								// Funci�n para crear la ventana principal
	HWND						Crear(int nCmdShow);
								// Funci�n que crea los botones play, stop, av medio, re medio en el thumbnail de la barra de tareas
	void						CrearBotonesThumb(void);
								// Evento que recibe las pulsaciones del thumbnauil de la barra de tareas (nada mas, este reproductor no tiene controles estandar)
	void                        Evento_Comando(WPARAM wParam, LPARAM lParam);
								// Evento que recibe cuando se pulsa sobre un men� extendido
	void						Evento_MenuEx_Click(const UINT cID);
								// Evento que recibe cuando se est� cambiando una barra dentro de un menu extendido
	void						Evento_MenuEx_Barra_Cambiando(const UINT cID, const float ValorBarra);
								// Evento que recibe cuando se ha cambiado una barra dentro de un menu extendido
	void						Evento_MenuEx_Barra_Cambiado(const UINT cID, const float ValorBarra);
								// Evento que recibe cuando se est� haciendo click en un boton extendido
	void						Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se presiona un boton del mouse en un boton extendido
	void                        Evento_BotonEx_Mouse_Presionado(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se suelta un boton del mouse en un boton extendido
	void                        Evento_BotonEx_Mouse_Soltado(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se est� cmabiando el tama�o de la ventana
	void						Evento_CambiandoTam(const UINT Lado, RECT *Rectangulo);
								// Evento que recibe cuando se ha cambiado la barra del tiempo
	void						Evento_SliderTiempo_Cambiado(void);
								// Evento que recibe cuando se ha cambiado la barra del volumen
	void						Evento_SliderVolumen_Cambiado(void);
								// Evento que recibe cuando se est� cambiando la barra del volumen
	void						Evento_SliderVolumen_Cambiando(void);
								// Evento que se recibe cuando se est� moviendo la rueda del mouse dentro de la barra del volumen
	void						Evento_SliderVolumen_Rueda(DEventoMouseRueda& DatosMouse);
								// Evento que se recibe cuando se sueltan archivos desde el explorador a esta ventana
	void						Evento_SoltarArchivos(WPARAM wParam);
								// Evento que se recibe cuando es necesario borrar el fondo de esta ventana
	void						Evento_BorraFondo(HDC DC);
								// Evento que se recibe cuando se cierra esta ventana
	void						Evento_Cerrar(void);
								// Evento que recibe cuando se est� cambiando una barra extendida
	void						Evento_BarraEx_Cambiando(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se ha cambiado una barra extendida
	void						Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse);
								// Evento que recibe cuando se ha movido la rueda del mouse dentro de una barra extendida
	void						Evento_BarraEx_Rueda(DWL::DEventoMouseRueda& DatosMouse);

								// Mensaje que se recibe del thread agregar archivos, que indica que se ha terminado
	void						ThreadAgregarArchivosLista_Terminado(void);
								// Mensaje que se recibe del thread agregar archivos, que indica que se ha agregado un nuevo medio
	void						ThreadAgregarArchivosLista_AgregarMedio(WPARAM wParam);
								// Mensaje que se recibe del thread buscar archivos, que indica que se ha agregado una ra�z
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
								// Mensaje que se recibe del thread analizar, que indica que ha empezado el an�lisis
	void                        ThreadAnalizar_Iniciado2(WPARAM wParam);
								// Mensaje que se recibe del thread analizar con el total de medios de la fase 1
	void						ThreadAnalizar_TotalMedios(WPARAM wParam, LPARAM lParam);
								// Mensaje que se recibe del thread analizar con el total de medios de la fase 2
	void						ThreadAnalizar_TotalMedios2(void);
								// Funci�n que ajusta los controles al tama�o de la recta especificada
	void						AjustarControles(RECT &RC);
								// Funci�n que genera una lista aleat�ria del tipo especificado
	void                        GenerarListaAleatoria(const TipoListaAleatoria nTipo = TLA_LoQueSea);
								// Funci�n que restablece los filtros de video a sus valores por defecto
	void						FiltrosVideoPorDefecto(void);
								// Funci�n para pasar del modo normal al modo pantalla completa
	void						PantallaCompleta(const BOOL nActivar);
								// Funci�n que devuelve si el reproductor est� en pantalla completa o en modo normal
	inline const BOOL			PantallaCompleta(void) { return _PantallaCompleta;  }
								// Funci�n que ejectura el repeat
	void                        Repeat(void);
								// Funci�n que recibe cuando se pulsa en el boton reprtir
	void						Repetir_Click(void);
								// Funci�n que recibe cuando se pulsa en el boton mezclar
	void                        Mezclar_Click(void);
								// Funci�n que mezcla / restaure la lista de reproducci�n, y marca / desmarca los botones mezclar.
	void                        Mezclar(const BOOL nMezclar);
								// Menu extendido para las opciones del repeat
	DWL::DMenuEx				Menu_Repetir;
								// Menu extendido para las opciones del arbol de la base de datos
	DWL::DMenuEx 				Menu_ArbolBD;
								// Menu extendido para las opciones de la lista de reproducci�n			
	DWL::DMenuEx                Menu_Lista;
								// Menu extendido para las opciones del v�deo
	DWL::DMenuEx                Menu_Video;
								// Menu extendido para el boton de la lista
	DWL::DMenuEx                Menu_BotonLista;
								// Menu extendido para el boton de la base de datos
	DWL::DMenuEx                Menu_BotonArbolBD;
								// Funci�n que actualiza el arbol de la base de datos
	void						ActualizarArbol(void);
								// Funci�n para iniciar el an�lisis de la base de datos (ThreadAnalisis)
	void                        AnalizarBD(void);
								// Funci�nes para mostrar un medio en la base de datos
	const BOOL                  Arbol_MostrarMedio(BDMedio &mMedio);
	const BOOL                  Arbol_MostrarMedio(const sqlite3_int64 Hash);
								// Funci�n para agregar un nodo a la lista de reproducci�n
	void                        Arbol_AgregarALista(const BOOL NuevaLista);
								// Funci�n para abrir la carpeta del nodo especificado
	void						Arbol_AbrirCarpeta(void);
								// Funci�n para mostrar un tooltip con informaci�n del nodo
	void						Arbol_Informacion(void);
								// Funci�n para abrir la ventana de propiedades de windows del nodo especificado (puede ser un directorio o un medio)
	void						Arbol_Propiedades(void);
								// Funci�n para asignar la nota de un nodo (si se hace en un directorio, se asignar� la nota para todos sus medios)
	void                        Arbol_AsignarNota(const float nNota);
								// Funci�n para abrir la ventana de momentos
	void                        Arbol_Momentos(void);
								// Funci�n que agrega un directorio al arbol de la base de datos
	NodoBD                     *Arbol_AgregarDir(std::wstring *Path, const BOOL nRepintar = FALSE);
								// Funci�n que agrega una ra�z al arbol de la base de datos
	NodoBD                     *Arbol_AgregarRaiz(std::wstring *Path);
								// Funci�n que agrega una lista del historial al arbol de la base de datos
	NodoBD                     *Arbol_AgregarHistorial_Lista(Historial_Lista &Lista, const BOOL AlFinal = TRUE);
								// Funci�n que agrega una medio del historial al arbol de la base de datos
	NodoBD                     *Arbol_AgregarHistorial_Medio(Historial_Lista &Lista, Historial_Medio &nMedio, const BOOL AlFinal = TRUE);

								// Thread para actualizar el arbol de la base de datos
	ThreadActualizarArbol		ThreadActualizar;
								// Thread para a�adir archivos externos a la lista
	ThreadAgregarArchivosLista  ThreadArchivosLista;
								// Thread para analizar los medios
	ThreadAnalisis				ThreadAnalizar;
								// Funci�n que inicia el thread de descarga (sin mostrar la ventana)
	void						Actualizacion_DescargaEncontrada(const wchar_t* nVersion);
								// Funci�n que descarga una nueva actualizaci�n
	void						Actualizacion_MostrarDescargar(const wchar_t *nVersion);
								// Funci�n que mantiene la barra de descarga de la actualizaci�n
	void						Actualizacion_Barra(const float nValor);
								// Funci�n que muestra un mensaje de error de la actualizaci�n
	void						Actualizacion_Error(void);
								// Funci�n que muestra un mensaje conforme se ha cancelado la actualizaci�n
	void						Actualizacion_Cancelada(void);
								// Funci�n que avisa al usuario de que se ha descargado la actualizaci�n
	void						Actualizacion_Descargada(void);
								// Funci�n que muestra la ventana que pide al usuario instalar la actualizaci�n
	void						Actualizacion_Existente(void);
								// Funci�n que muestra la ventana para abrir URL's
	void						MostrarVentanaURL(void);

								// Funci�n que enumera las pantallas disponibles, y busca una donde ubicar el reproductor
	static BOOL CALLBACK		EnumerarPantallas(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
								// Funci�n que ejecuta el siguiente medio de la lista (el primero si no se ha ejecutado ninguno)
	void						Lista_Siguiente(void);
								// Funci�n que ejecuta el anterior medio de la lista (el ultimo si no se ha ejecutado ninguno)
	void						Lista_Anterior(void);
								// Funci�n que hace play en el medio actual
	void						Lista_Play(void);
								// Funci�n que hace pausa en el medio actual
	void						Lista_Pausa(void);
								// Funci�n que para el medio actual
	void						Lista_Stop(void);
								// Funci�n para mostrar el tooltip del medio marcado en la lista
	void						Lista_Informacion(void);
								// Funci�n que muestra las propiedades el medio especificado
	void                        Lista_Propiedades(void);
								// Funci�n que abre la carpeta del medio especificado
	void                        Lista_AbrirEnExplorador(void);
								// Funci�n que muestra en la base de datos el medio especificado
	void						Lista_MostrarEnBaseDatos(void);
								// Funci�n que elimina todos los medios seleccionados de la lista
	void                        Lista_EliminarSeleccionados(void);
								// Funci�n que asigna la nota especificada a los medios seleccionados en la lista
	void                        Lista_AsignarNota(const float nNota);
								// Funci�n que muestra la ventana de los momentos con el medio marcado en la lista
	void                        Lista_Momentos(void);
								// Funci�n que muestra la ventana de los momentos con el medio especificado
	void                        Lista_MomentosAbrir(const UINT64 IdMedio, const int PosMomento);
								// Funci�n que muestra la ventana de los momentos
	void                        Video_Momentos(void);
								// Funci�n que muestra el dialogo para abrir archivos y seleccionar un subtitulo
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
								// Funci�n que crea los skins oscuros para el arbol y la lista 
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
								// Variable que determina si el monitor donde se ha creado la ventana principal est� disponible
	BOOL						MonitorDisponible;
								// Medidas del monitor actual
	RECT						RectMonitorActual;

								// Client to Window rect
								// Guardo la diferencia de ancho y altura entre el client rect y el window rect
	int							CTW_ExtraX;
	int							CTW_ExtraY;
								// Funci�n para mostrar el marco especificado (lista, bd, video)
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

