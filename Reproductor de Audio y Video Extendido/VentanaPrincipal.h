#pragma once


#include "DhWnd.h"
#include "ArbolBD.h"
#include "ListaMedios.h"
#include "DMenuEx.h"
#include "VerVideo.h"
#include "MarcoControles.h"
#include "MarcoOpciones.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"
#include "DBotonEx.h"
#include "DLabelEx.h"
#include "ListaRaices.h"
#include "DDesplegableEx.h"
#include "ThreadActualizarArbol.h"
#include "ThreadAgregarArchivosLista.h"
#include "ThreadAnalisis.h"
#include "VentanaAsociarReproductor.h"

using namespace DWL;

class VentanaPrincipal : public DWL::DVentana {
 public:
								VentanaPrincipal() : DWL::DVentana(), _PantallaCompleta(FALSE), EstadoMedio(EnStop), MousePos({ 0, 0 }), MonitorDisponible(FALSE) {};
					           ~VentanaPrincipal() { };
	HWND						Crear(int nCmdShow);

	void						AgregarRaiz(void);
	void						EliminarRaiz(std::wstring &Path);

//	void						Evento_Button_Mouse_Click(const UINT cID);
	void						Evento_BotonEx_Mouse_Click(const UINT cID);
	void                        Evento_BotonEx_Mouse_Down(const UINT cID);

	//void						Evento_ArbolEx_Click(DArbolEx_DatosClick *Datos, const UINT aID);
	void						Evento_Temporizador(const UINT cID);
//	void						Timer_ObtenerTiempoTotal(void);
	void						Evento_CambiandoTam(const UINT Lado, RECT *Rectangulo);

//	LRESULT						Evento_SliderH(WPARAM wParam, LPARAM lParam);

	void						Evento_SliderTiempo_Cambiado(void);
	void						Evento_SliderVolumen_Cambiado(void);
	void						Evento_SliderVolumen_Cambio(void);

	void						Evento_SoltarArchivos(WPARAM wParam);

	void						Evento_BorraFondo(HDC DC);


	void						Evento_Cerrar(void);

	void						AjustarControles(RECT &RC);

	void						PantallaCompleta(const BOOL nActivar);
	inline const BOOL			PantallaCompleta(void) { return _PantallaCompleta;  }
	
	void                        Repeat(void);
	void						Repetir_Click(void);

	void                        Mezclar_Click(void);

	DWL::DMenuEx				Menu_Repetir;
	DWL::DMenuEx 				Menu_ArbolBD;
//	DWL::DMenuEx 				Menu_Mezclar;


	void						ActualizarArbol(void);
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
	DWL::DBotonEx				BotonPausa;
	DWL::DBotonEx				BotonStop;
	DWL::DBotonEx				BotonAdelante;
	DWL::DBotonEx				BotonAtras;

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

	MarcoOpciones				Opciones;

	BarraVolumen				SliderVolumen;

	DWL::DLabelEx				LabelVolumen;

	DWL::DLabelEx				LabelTiempoActual;
	DWL::DLabelEx				LabelTiempoSeparador;
	DWL::DLabelEx				LabelTiempoTotal;

	DWL::DBotonEx				BotonBD;
	DWL::DBotonEx				BotonLista;
	DWL::DBotonEx				BotonVideo;

	DWL::DBotonEx				BotonOpciones;

	ListaRaices					ListaRaiz;
	DWL::DBotonEx				BotonAgregarRaiz;

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

	VentanaAsociarReproductor   VentanaAsociar;
  protected:
//	void                       _AgregarNodoALista(DArbolEx_Nodo *nNodo);
	BOOL                       _PantallaCompleta;

	DWORD                      _BotonExMouseDownTick;
//	BOOL                       _Mezclar;
};

