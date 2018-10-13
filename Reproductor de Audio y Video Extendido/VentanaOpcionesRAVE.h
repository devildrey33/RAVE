#pragma once

#include "DVentana.h"
#include "MarcoControles.h"
#include "ListaRaices.h"
#include "DEtiquetaEx.h"
#include "DMarcaEx.h"
#include "AsignarTeclaRapida.h"
#include "ControlSeparador.h"
#include "DBarraDesplazamientoEx.h"
#include "DListaDesplegableEx.h"

class VentanaOpcionesRAVE : public DWL::DVentana {
  public:
							VentanaOpcionesRAVE(void);
	                       ~VentanaOpcionesRAVE(void);

	void					Crear(void);
	
	void					AsignarMarco(const INT_PTR Id);

	void					AgregarRaiz(void);
	void					EliminarRaiz(std::wstring &Path);

	void					Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
	void					Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
	void					Evento_Cerrar(void);
	void					Evento_BarraEx_Cambio(DWL::DEventoMouse &DatosMouse);
	void					Evento_BarraEx_Cambiado(DWL::DEventoMouse &DatosMouse);
	void					Evento_ListaDesplegable_Cambio(INT_PTR nID);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	DWL::DBotonEx			BotonCerrar;

	DWL::DBotonEx			BotonBaseDeDatos;
//	DWL::DBotonEx			BotonAsociacionesArchivos;
	DWL::DBotonEx			BotonListas;
	DWL::DBotonEx			BotonGeneral;
	DWL::DBotonEx			BotonVideo;

	MarcoControles			MarcoBaseDeDatos;
	MarcoControles			MarcoAsociacionesArchivos;
	MarcoControles			MarcoListas;
	MarcoControles			MarcoGeneral;
	MarcoControles			MarcoVideo;

							// Base de datos
	DEtiquetaEx				EtiquetaBaseDeDatos1;
	ListaRaices				ListaRaiz;
	DWL::DBotonEx			BotonAgregarRaiz;
	ControlSeparador		SeparadorBD;
	DWL::DMarcaEx			MarcaMostrarAnalisis;
	DWL::DMarcaEx			MarcaAnalizarMediosPendientes;

							// General
	DEtiquetaEx				EtiquetaTeclasRapidas;
	AsignarTeclaRapida		TeclasRapidas[6];
	DWL::DEtiquetaEx		EtiquetasTeclas[6];
	ControlSeparador	    SeparadorGeneral;
	DWL::DMarcaEx			MarcaBuscarActualizaciones;
	DEtiquetaEx			    EtiquetaTiempoAnimaciones;
	DEtiquetaEx             EtiquetaTiempoAnimacionesTA;
	DBarraDesplazamientoEx	BarraTiempoAnimaciones;
	DEtiquetaEx			    EtiquetaTiempoOcultarToolTips;
	DBarraDesplazamientoEx	BarraTiempoToolTips;
	DEtiquetaEx			    EtiquetaTiempoOcultarToolTipsTA;

							// Lista
	DListaDesplegableEx     DesplegableListaInicio;
};

