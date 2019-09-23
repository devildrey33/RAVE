#pragma once

#include <DMarcoScrollEx.h>
#include <DEtiquetaEx.h>
#include <DBotonEx.h>
#include <DMarcaEx.h>
#include "ListaRaices.h"
#include "ControlSeparador.h"

class Marco_BaseDatos : public DWL::DMarcoScrollEx {
  public:
							Marco_BaseDatos(void) : DMarcoScrollEx() { };
	
	void					Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

							// Eventos
	void					Evento_BotonEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	void					Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	void					Evento_ListaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);

	void					AgregarRaiz(void);
	void					EliminarRaiz(std::wstring& Path);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);
							
	DWL::DEtiquetaEx		EtiquetaBaseDeDatos1;
	ListaRaices				ListaRaiz;
	DWL::DBotonEx			BotonAgregarRaiz;
	DWL::DBotonEx			BotonEliminarRaiz;
	ControlSeparador		SeparadorBD;
	DWL::DMarcaEx			MarcaMostrarAnalisis;
	DWL::DMarcaEx			MarcaAnalizarMediosPendientes;

};

