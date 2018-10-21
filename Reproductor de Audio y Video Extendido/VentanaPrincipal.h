#pragma once

#include "ArbolBD.h"
#include "ListaMedios.h"
#include "DMenuEx.h"
//#include "DMenuEx2.h"
#include "VerVideo.h"
#include "MarcoControles.h"
//#include "MarcoOpciones.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"
#include "DBotonEx.h"
#include "DEtiquetaEx.h"
#include "ListaRaices.h"
//#include "DDesplegableEx.h"
#include "ThreadActualizarArbol.h"
#include "ThreadAgregarArchivosLista.h"
#include "ThreadAnalisis.h"

using namespace DWL;

class VentanaPrincipal : public DWL::DVentana {
 public:
								VentanaPrincipal(void) : DWL::DVentana(), _PantallaCompleta(FALSE), EstadoMedio(EnStop), MousePos({ 0, 0 }), MonitorDisponible(FALSE), _MaximoTotalMedios2(0), _ValorMedios2(0) {};
					           ~VentanaPrincipal(void) { };
	HWND						Crear(int nCmdShow);

	void						CrearBotonesThumb(void);

/*	void						AgregarRaiz(void);
	void						EliminarRaiz(std::wstring &Path);*/

	void						Evento_MenuEx_Click(const UINT cID, const float ValorBarra);
	void						Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
	void                        Evento_BotonEx_Mouse_Presionado(DWL::DEventoMouse &DatosMouse);
	void                        Evento_BotonEx_Mouse_Soltado(DWL::DEventoMouse &DatosMouse);
	//	void                        Evento_BotonEx_Mouse_Up(DWL::DEventoMouse &DatosMouse);
	void						Evento_Temporizador(const UINT cID);
	void						Evento_CambiandoTam(const UINT Lado, RECT *Rectangulo);
	void						Evento_SliderTiempo_Cambiado(void);
	void						Evento_SliderVolumen_Cambiado(void);
	void						Evento_SliderVolumen_Cambio(void);
	void						Evento_SoltarArchivos(WPARAM wParam);
	void						Evento_BorraFondo(HDC DC);
	void						Evento_Cerrar(void);
/*	void                        Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado);
	void                        Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado);
	void						Evento_Tecla(DWL::DEventoTeclado &DatosTeclado);*/
	void						Evento_BarraEx_Cambio(DWL::DEventoMouse &DatosMouse);
	void						Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse);



	void						ThreadAgregarArchivosLista_Terminado(void);
	void						ThreadAgregarArchivosLista_AgregarMedio(WPARAM wParam);

	void						ThreadABuscarArchivos_AgregarRaiz(LPARAM lParam);
	void						ThreadABuscarArchivos_AgregarDirectorio(LPARAM lParam);
	void						ThreadABuscarArchivos_Terminado(const BOOL Cancelado, LPARAM lParam);
	
	void						ThreadAnalizar_Terminado(const BOOL Cancelado, LPARAM lParam);
	void						ThreadAnalizar_MostrarVentana(void);
	void                        ThreadAnalizar_Iniciado2(WPARAM wParam);
	void						ThreadAnalizar_TotalMedios(WPARAM wParam, LPARAM lParam);
	void						ThreadAnalizar_TotalMedios2(void);

	void						AjustarControles(RECT &RC);

	void                        GenerarListaAleatoria(const TipoListaAleatoria nTipo = TLA_LoQueSea);

	void						FiltrosVideoPorDefecto(void);

	void						PantallaCompleta(const BOOL nActivar);
	inline const BOOL			PantallaCompleta(void) { return _PantallaCompleta;  }
	
	void                        Repeat(void);
	void						Repetir_Click(void);

	void                        Mezclar_Click(void);

	DWL::DMenuEx				Menu_Repetir;
	DWL::DMenuEx 				Menu_ArbolBD;
	DWL::DMenuEx                Menu_Lista;
	DWL::DMenuEx                Menu_Video;

	DWL::DMenuEx                Menu_BotonLista;
	DWL::DMenuEx                Menu_BotonArbolBD;
	//	DWL::DMenuEx 				Menu_Mezclar;

	void						ActualizarArbol(void);
	void                        AnalizarBD(void);
	ThreadActualizarArbol		ThreadActualizar;
	ThreadAgregarArchivosLista  ThreadArchivosLista;
	ThreadAnalisis				ThreadAnalizar;

//	NodoBD                     *Arbol_AgregarCancion(const size_t Hash);
	NodoBD                     *Arbol_AgregarDir(std::wstring *Path, const BOOL nRepintar = FALSE);
	NodoBD                     *Arbol_AgregarRaiz(std::wstring *Path);

	LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static BOOL CALLBACK		EnumerarPantallas(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);


	void						Lista_Siguiente(void);
	void						Lista_Anterior(void);
	void						Lista_Play(void);
	void						Lista_Pausa(void);
	void						Lista_Stop(void);

	void						ExploradorAgregarMedio(const BOOL Reproducir);

	DWL::DBotonEx				BotonPlay;
//	DWL::DBotonEx				BotonPausa;
	DWL::DBotonEx				BotonStop;
	DWL::DBotonEx				BotonAdelante;
	DWL::DBotonEx				BotonAtras;

	DWL::DEtiquetaEx			LabelRatio;

	DWL::DBotonEx				BotonMezclar;
	DWL::DBotonEx				BotonRepetir;

	BarraTiempo					SliderTiempo;
	//	DWL::DTrackBar			SliderTiempo;
								// Marco que contiene el slider del volumen y varios labels
	MarcoControles				MarcoSD;
								// Marco que contiene los controles play pausa stop
	MarcoControles				MarcoSI;
								// Marco que contiene los botones lista bd video
	MarcoControles				MarcoII;

//	MarcoOpciones				Opciones;

	BarraVolumen				SliderVolumen;

	DWL::DEtiquetaEx			LabelVolumen;

	DWL::DEtiquetaEx			LabelTiempoActual;
	DWL::DEtiquetaEx			LabelTiempoSeparador;
	DWL::DEtiquetaEx			LabelTiempoTotal;

	DWL::DBotonEx				BotonBD;
	DWL::DBotonEx				BotonLista;
	DWL::DBotonEx				BotonVideo;

	DWL::DBotonEx				BotonOpciones;

/*	ListaRaices					ListaRaiz;
	DWL::DBotonEx				BotonAgregarRaiz;*/

	ListaMedios					Lista;
	VerVideo					Video;
	ArbolBD 					Arbol;

	Estados_Medio				EstadoMedio;

	POINT						MousePos;

	BOOL						MonitorDisponible;
	RECT						RectMonitorActual;

								// Client to Window rect
								// Guardo la diferencia de ancho y altura entre el client rect y el window rect
	int							CTW_ExtraX;
	int							CTW_ExtraY;

  protected:
    void					   _MostrarMarco(const INT_PTR ID);

//	void                       _AgregarNodoALista(DArbolEx_Nodo *nNodo);
	BOOL                       _PantallaCompleta;

	DWORD                      _BotonExMouseDownTick;
	UINT                       _MaximoTotalMedios2; // Total de medios en la fase 2 del analisis
	UINT                       _ValorMedios2;
//	BOOL                       _Mezclar;
};

