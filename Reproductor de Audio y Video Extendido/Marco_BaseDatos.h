#pragma once

#include <DMarcoScrollEx.h>
#include <DEtiquetaEx.h>
#include <DBotonEx.h>
#include <DMarcaEx.h>
#include "ListaRaices.h"
#include "ControlSeparador.h"

class Marco_BaseDatos : public DWL::DMarcoScrollEx {
  public:
							Marco_BaseDatos(void) { };
	
	void					Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

							// Base de datos
	DWL::DEtiquetaEx		EtiquetaBaseDeDatos1;
	ListaRaices				ListaRaiz;
	DWL::DBotonEx			BotonAgregarRaiz;
	DWL::DBotonEx			BotonEliminarRaiz;
	ControlSeparador		SeparadorBD;
	DWL::DMarcaEx			MarcaMostrarAnalisis;
	DWL::DMarcaEx			MarcaAnalizarMediosPendientes;

};

