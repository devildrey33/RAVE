#pragma once

#include <DMarcoScrollEx.h>

class Marco_Listas : public DWL::DMarcoScrollEx {
  public:
							Marco_Listas(void) : DMarcoScrollEx() { }
	void					Crear(DWL::DhWnd* nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);

//	void					ActualizarListaInicio(void);

	void					Evento_ListaDesplegable_Cambio(INT_PTR nID);
	void					Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

//	DEtiquetaEx             EtiquetaListaInicio;
//	DListaDesplegableEx     DesplegableListaInicio;
	ControlSeparador	    SeparadorLista1;
	DEtiquetaEx             EtiquetaCanciones25;
	DWL::DMarcaEx			MarcaCanciones25;
	ControlSeparador	    SeparadorLista2;
	DEtiquetaEx             EtiquetaNoGenerarListas3;
	DWL::DMarcaEx			MarcaNoGenerarListas3;
	ControlSeparador	    SeparadorLista3;
	DEtiquetaEx             EtiquetaSumar005;
	DWL::DMarcaEx			MarcaSumar005;

//	ControlSeparador	    SeparadorLista4;

	// Lista2
	DEtiquetaEx             EtiquetaListaMezclar;
	DEtiquetaEx             EtiquetaListaGenero;
	DListaDesplegableEx     DesplegableListaGenero;
	DEtiquetaEx             EtiquetaListaGrupo;
	DListaDesplegableEx     DesplegableListaGrupo;
	DEtiquetaEx             EtiquetaListaDisco;
	DListaDesplegableEx     DesplegableListaDisco;
	DEtiquetaEx             EtiquetaLista50Can;
	DListaDesplegableEx     DesplegableLista50Can;
	DEtiquetaEx             EtiquetaListaNota;
	DListaDesplegableEx     DesplegableListaNota;

};

