#pragma once

#include "DhWnd.h"
#include "DButton.h"
#include "ArbolBD.h"
#include "ListaMedios.h"
//#include "DMenu.h"
#include "VerVideo.h"
#include "DTrackBar.h"
#include "TMedio.h"
#include "DStaticControl.h"
#include "MarcoControles.h"
#include "MarcoOpciones.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"
#include "DBotonEx.h"
#include "DLabelEx.h"


class VentanaPrincipal : public DWL::DVentana {
 public:
							VentanaPrincipal() : DWL::DVentana(), _PantallaCompleta(FALSE), EstadoMedio(EnStop), MousePos({ 0, 0 }), MonitorDisponible(FALSE) {};
					       ~VentanaPrincipal() { };
	HWND					Crear(int nCmdShow);

	void					Evento_Button_Mouse_Click(const UINT cID);
	void					Evento_BotonEx_Mouse_Click(const UINT cID);

	void				    Evento_TreeView_Mouse_Click(DTreeView_DatosClick *Datos, const UINT tID);
	void					Evento_ListView_Mouse_Click(DListView_DatosClick *DatosClick, const UINT IDListView);
	void					Evento_ListView_Mouse_DobleClick(DListView_DatosClick *DatosClick, const UINT IDListView);
	void					Evento_Temporizador(const UINT cID);
	void					Evento_CambiandoTam(const UINT Lado, RECT *Rectangulo);
//	LRESULT					Evento_SliderH(WPARAM wParam, LPARAM lParam);

	void					Evento_SliderTiempo_Cambiado(void);
	void					Evento_SliderVolumen_Cambiado(void);
	void					Evento_SliderVolumen_Cambio(void);

	void					Evento_BorraFondo(HDC DC);

//	void					Evento_CapturaSoltada(const UINT_PTR ID);

	void					AjustarControles(RECT &RC);

	void					PantallaCompleta(const BOOL nActivar);
	inline const BOOL		PantallaCompleta(void) { return _PantallaCompleta;  }

	LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static BOOL CALLBACK	EnumerarPantallas(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);


	void					Lista_Siguiente(void);
	void					Lista_Anterior(void);
	void					Lista_Play(void);
	void					Lista_Pausa(void);
	void					Lista_Stop(void);

	DWL::DButton			BotonPlay;
	DWL::DButton			BotonPausa;
	DWL::DButton			BotonStop;
	DWL::DButton			BotonAdelante;
	DWL::DButton			BotonAtras;

	DWL::DButton			BotonMezclar;
	DWL::DButton			BotonRepetir;

	BarraTiempo				SliderTiempo;
	//	DWL::DTrackBar			SliderTiempo;
							// Marco que contiene el slider del volumen y varios labels
	MarcoControles			MarcoSD;
							// Marco que contiene los controles play pausa stop
	MarcoControles			MarcoSI;
							// Marco que contiene los botones lista bd video
	MarcoControles			MarcoII;

	MarcoOpciones			Opciones;

	BarraVolumen			SliderVolumen;

	DWL::DLabelEx			LabelVolumen;

	DWL::DLabelEx			LabelTiempoActual;
	DWL::DLabelEx			LabelTiempoSeparador;
	DWL::DLabelEx			LabelTiempoTotal;

	DWL::DBotonEx			BotonBD;
	DWL::DBotonEx			BotonLista;
	DWL::DBotonEx			BotonVideo;

	DWL::DBotonEx			BotonOpciones;

	ListaMedios				Lista;
	ArbolBD					Arbol;
	VerVideo				Video;


	int						Errores;
	Estados_Medio			EstadoMedio;

	POINT					MousePos;

	BOOL                    MonitorDisponible;
	RECT                    RectMonitorActual;

							// Client to Window rect
							// Guardo la diferencia de ancho y altura entre el client rect y el window rect
	int                     CTW_ExtraX;
	int                     CTW_ExtraY;


  protected:
	BOOL                   _PantallaCompleta;
};

