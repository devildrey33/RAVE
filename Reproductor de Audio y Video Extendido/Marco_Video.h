#pragma once

#include <DMarcoScrollEx.h>

class Marco_Video : public DWL::DMarcoScrollEx {
  public:
							Marco_Video(void) : DMarcoScrollEx() { }
	void					Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

	void					Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	DWL::DMarcaEx			MarcaRecordarValoresVideo;

};
