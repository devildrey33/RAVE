#pragma once

#include "RaveBD.h"
#include "DVentana.h"
#include "DBotonEx.h"
#include "DMarcaEx.h"
#include "DListaEx.h"
#include "BarraTiempo.h"
#include "DEdicionTextoEx.h"


/* Ventana para gestionar los momentos de un medio */
class VentanaMomento : public DWL::DVentana {
  public:
									VentanaMomento(void);
								   ~VentanaMomento(void);
	
	void                            Mostrar(BDMedio &Medio);
	void                            ActivarControles(const BOOL nActivar);
	void                            ActivarEditarEliminar(const BOOL nActivar);
	void                            GuardarMomento(void);
	void                            CargarMomentos(void);
	void                            MostrarMomento(const size_t Pos);
	void                            EliminarMomento(void);

	void							Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	void							Evento_BarraEx_Cambiando(DWL::DEventoMouse& DatosMouse);
	void							Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse);
	void							Evento_ListaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);

	LRESULT CALLBACK				GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BDMedio                         Medio;

	DWL::DListaEx                   ListaMomentos;
	BarraTiempo						Barra_TiempoInicio;
	BarraTiempo						Barra_TiempoFinal;
	DWL::DEdicionTextoEx			Txt_TiempoInicio;
	DWL::DEdicionTextoEx			Txt_TiempoFinal;
	DWL::DEdicionTextoEx			Txt_Nombre;						// Nombre para el momento
	DWL::DBotonEx					Boton_Guardar;
	DWL::DBotonEx					Boton_Cancelar;
	DWL::DBotonEx					Boton_CrearMomento;
	DWL::DBotonEx					Boton_EditarMomento;
	DWL::DBotonEx					Boton_EliminarMomento;
	DWL::DMarcaEx					Check_Excluir;					// Momento para excluir (intro o creditos por ejemplo)

	LONG_PTR                        PosMomento;
};

