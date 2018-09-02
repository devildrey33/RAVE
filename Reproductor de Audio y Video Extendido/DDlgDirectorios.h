#ifndef DDLGDIRECTORIOS_H
#define DDLGDIRECTORIOS_H

//#include "DhWnd.h"
#include "DBotonEx.h"
#include "DExplorarDirectoriosEx.h"
#include "DEdicionTextoEx.h"

namespace DWL {

	class DDlgDirectorios : public DWL::DVentana {
	  public:
									DDlgDirectorios();
		                           ~DDlgDirectorios();

		const BOOL					Mostrar(DhWnd *nPadre, std::wstring &Path);

									// Controles
		DExplorarDirectoriosEx      ArbolDirectorios;
		DEdicionTextoEx				EdicionSeleccion;
		DBotonEx                    BotonAceptar;
		DBotonEx                    BotonCancelar;

		void                        Evento_Cerrar(void);
		void						Evento_BotonEx_Mouse_Click(const UINT cID);
		void						Evento_BorraFondo(HDC DC);
		virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	  protected :
		void                       _ActualizarAceptar(void);
		BOOL                       _Terminado;
	};

};

#endif