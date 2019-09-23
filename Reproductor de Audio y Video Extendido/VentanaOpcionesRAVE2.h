#pragma once

#include <DVentana.h>
#include "Marco_BaseDatos.h"
#include "Marco_Inicio.h"
#include "Marco_UI.h"
#include "Marco_Listas.h"
#include "Marco_Teclado.h"
#include "Marco_Video.h"
#include <DArbolEx.h>


#define NUM_OPCIONES 6

class VentanaOpcionesRAVE2 : public DWL::DVentana {
  public:
							VentanaOpcionesRAVE2(void) { };


	void					Crear(void);
	void					MostrarMarco(const size_t Marco);

	void					Evento_Cerrar(void);
	void					Evento_ArbolEx_Mouse_Click(DWL::DEventoMouse& DatosMouse);
	void					Evento_DlgDirectorios_CambioTamPos(HWND hWndDlg);

	LRESULT CALLBACK		GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	DWL::DArbolEx		    ArbolSecciones;

	Marco_BaseDatos			MarcoBaseDatos;
	Marco_Inicio			MarcoInicio;
	Marco_UI				MarcoUI;
	Marco_Listas			MarcoListas;
	Marco_Teclado			MarcoTeclado;
	Marco_Video				MarcoVideo;

	static const wchar_t   *OpcionesStr[NUM_OPCIONES];
};

