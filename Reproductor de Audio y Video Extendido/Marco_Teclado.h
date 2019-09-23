#pragma once

#include <DMarcoScrollEx.h>
#include "AsignarTeclaRapida.h"

class Marco_Teclado : public DWL::DMarcoScrollEx {
  public:
							Marco_Teclado(void) : DMarcoScrollEx() { }
	void					Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);


	// Teclado
	DEtiquetaEx				EtiquetaTeclasRapidas;
	AsignarTeclaRapida		TeclasRapidas[9];
	DWL::DEtiquetaEx		EtiquetasTeclas[9];
	DEtiquetaEx			    EtiquetaTiempoAnimaciones;
	DEtiquetaEx             EtiquetaTiempoAnimacionesTA;

//	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

};
