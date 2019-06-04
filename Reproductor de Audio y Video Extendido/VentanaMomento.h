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

	void							Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	void							Evento_BarraEx_Cambiando(DWL::DEventoMouse& DatosMouse);
	void							Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse);

	LRESULT CALLBACK				GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BDMedio                         Medio;

	DWL::DListaEx                   ListaMomentos;
	BarraTiempo						Barra_TiempoInicio;
	BarraTiempo						Barra_TiempoFinal;
	DWL::DEdicionTextoEx			Txt_TiempoInicio;
	DWL::DEdicionTextoEx			Txt_TiempoFinal;
	DWL::DEdicionTextoEx			Nombre;						// Nombre para el momento
	DWL::DBotonEx					Guardar;
	DWL::DBotonEx					Cancelar;
	DWL::DBotonEx					CrearMomento;
	DWL::DBotonEx					EditarMomento;
	DWL::DBotonEx					EliminarMomento;
	DWL::DMarcaEx					Excluir;					// Momento para excluir (intro o creditos por ejemplo)
};
