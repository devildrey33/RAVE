#pragma once

#include <DApp.h>
#include "RAVE_LA_Configuracion.h"

class RAVE_LA : public DWL::DApp {
  public:
			RAVE_LA(void) : MutexLA(NULL) { }
	void	Iniciar(void);
	HANDLE	MutexLA;
};


#define App static_cast<RAVE_LA &>(*_Aplicacion)