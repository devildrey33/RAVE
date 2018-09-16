#pragma once

#include "DVentana.h"
#include "MarcoControles.h"
#include "ListaRaices.h"
#include "DEtiquetaEx.h"
#include "DMarcaEx.h"

class VentanaOpcionesRAVE : public DWL::DVentana {
  public:
						VentanaOpcionesRAVE(void);
	                   ~VentanaOpcionesRAVE(void);

	void				Crear(void);
	
	void				AsignarMarco(const INT_PTR Id);

	void				AgregarRaiz(void);
	void				EliminarRaiz(std::wstring &Path);

	void				Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
	void				Evento_MarcaEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
	void                Evento_Cerrar(void);

	LRESULT CALLBACK	GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	DWL::DBotonEx		BotonBaseDeDatos;
	DWL::DBotonEx		BotonAsociacionesArchivos;
	DWL::DBotonEx		BotonListas;
	DWL::DBotonEx		BotonGeneral;
	DWL::DBotonEx		BotonVideo;

	MarcoControles		MarcoBaseDeDatos;
	MarcoControles		MarcoAsociacionesArchivos;
	MarcoControles		MarcoListas;
	MarcoControles		MarcoGeneral;
	MarcoControles		MarcoVideo;

						// Base de datos
	DEtiquetaEx			EtiquetaBaseDeDatos1;
	ListaRaices			ListaRaiz;
	DWL::DBotonEx		BotonAgregarRaiz;
	DWL::DMarcaEx		MarcaMostrarAnalisis;
	DWL::DMarcaEx		MarcaAnalizarMediosPendientes;
};

