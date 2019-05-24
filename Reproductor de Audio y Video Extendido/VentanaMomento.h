#pragma once

#include "RaveBD.h"
#include "DVentana.h"
#include "DBotonEx.h"
#include "DMarcaEx.h"
#include "DListaEx.h"
#include "DBarraDesplazamientoEx.h"
#include "DEdicionTextoEx.h"


/* Ventana para gestionar los momentos de un medio */
class VentanaMomento : public DWL::DVentana {
  public:
									VentanaMomento(void);
								   ~VentanaMomento(void);

	void                            Mostrar(BDMedio &Medio);

	DWL::DListaEx                   ListaMomentos;
	DWL::DBarraDesplazamientoEx		Barra_TiempoInicio;
	DWL::DBarraDesplazamientoEx		Barra_TiempoFinal;
	DWL::DEdicionTextoEx			Txt_TiempoInicio;
	DWL::DEdicionTextoEx			Txt_TiempoFInal;
	DWL::DEdicionTextoEx			Nombre;						// Nombre para el momento
	DWL::DBotonEx					CrearMomento;
	DWL::DBotonEx					EliminarMomento;
	DWL::DMarcaEx					Excluir;					// Momento para excluir (intro o creditos por ejemplo)
};

