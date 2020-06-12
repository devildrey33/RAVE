#pragma once
#include "DVentana.h"
#include "BarraTiempo.h"
#include "BarraVolumen.h"
#include "DEtiquetaEx.h"
#include "DAnimacion.h"

enum CPC_Alineacion {
	Abajo,
	Izquierda,
	Arriba,
	Derecha
};

enum CPC_Transicion {
	CPC_Transicion_Mostrar,
	CPC_Transicion_Ocultar,
	CPC_Transicion_Resaltado,
	CPC_Transicion_Normal
};

class VerVideo;

class ControlesPantallaCompleta :public DVentana {
  public:
							ControlesPantallaCompleta() : DVentana(), Alineacion(Abajo), _Visible(FALSE), _MousePos({ 0, 0 }) { };
	                       ~ControlesPantallaCompleta() { };

	void					Crear(void);
	void					Mostrar(void);
	void					Ocultar(void);

	void                    MostrarBD(const BOOL Forzar = FALSE);
	void                    MostrarLista(const BOOL Forzar = FALSE);

							// Se está modificando (WM_MOUSEDOWN, WM_MOUSEHOVER)
	void					Evento_SliderTiempo_Cambio(void);
							// Se ha modificado (WM_MOUSEUP)
	void					Evento_SliderTiempo_Cambiado(void);

	void					Evento_SliderVolumen_Cambio(void);
	void					Evento_SliderVolumen_Cambiado(void);

	void					Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse);
	void					Evento_BarraEx_Cambiando(DWL::DEventoMouse &DatosMouse);

	void					Evento_BotonEx_Click(DWL::DEventoMouse &DatosMouse);

	void                    Evento_Temporizador(const UINT cID);
//	void					Evento_BorraFondo(HDC DC);
	void                    Evento_Pintar(void);
	void					Pintar(HDC DC);

	void                    Alinear(void);

	void                    Transicion(const CPC_Transicion nTransicion);

//	LRESULT					Evento_SliderH(WPARAM wParam, LPARAM lParam);


	DWL::DBotonEx			BotonPlay;
//	DWL::DBotonEx			BotonPausa;
	DWL::DBotonEx			BotonStop;
	DWL::DBotonEx			BotonAdelante;
	DWL::DBotonEx			BotonAtras;

	DWL::DEtiquetaEx		LabelRatio;

	DWL::DBotonEx			BotonMezclar;
	DWL::DBotonEx			BotonRepetir;

	DWL::DBotonEx			BotonLista;
	DWL::DBotonEx			BotonBD;

	DWL::DBotonEx           BotonRotar;

	BarraTiempo				SliderTiempo;
//	DWL::DTrackBar			SliderTiempo;
	BarraVolumen			SliderVolumen;

	DWL::DEtiquetaEx		LabelVolumen;

	DWL::DEtiquetaEx		LabelTiempoActual;
	DWL::DEtiquetaEx		LabelTiempoSeparador;
	DWL::DEtiquetaEx		LabelTiempoTotal;

	CPC_Alineacion		    Alineacion;

	void					RotarControles(const BOOL Invertido = FALSE);

	LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	DAnimacion             _AniMostrar;
protected :

	BOOL				   _Visible;

	POINT                  _MousePos;

	friend class VerVideo;
};

