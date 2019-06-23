#pragma once

#include <DApp.h>
#include "RAVE_LA_Configuracion.h"

class RAVE_Actualizador : public DWL::DApp {
  public:
			RAVE_Actualizador(void) : MutexActualizacion(NULL) { }
	void	Iniciar(void);
	HANDLE	MutexActualizacion;
};


#define App static_cast<RAVE_LA &>(*_Aplicacion)