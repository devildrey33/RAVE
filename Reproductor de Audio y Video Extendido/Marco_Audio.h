#pragma once

#include <DMarcoScrollEx.h>
#include <DEtiquetaEx.h>
#include <DMarcaEx.h>
#include <DBarraDesplazamientoEx.h>

class Marco_Audio : public DWL::DMarcoScrollEx {
  public:

	  void							Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

	  // Eventos
	  void							Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	  void							Evento_BarraEx_Cambiado(DWL::DEventoMouse& DatosMouse);
		
	  LRESULT CALLBACK				GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);


	  DWL::DEtiquetaEx				EtiquetaFadeInOut;
	  DWL::DMarcaEx					MarcaFadeInOut;

	  DWL::DEtiquetaEx				EtiquetaFadeInOut2;
	  DWL::DBarraDesplazamientoEx	BarraTiempoFadeInOut;

};

