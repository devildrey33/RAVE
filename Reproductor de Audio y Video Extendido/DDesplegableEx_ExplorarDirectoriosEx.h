#pragma once

#include "DExplorarDirectoriosEx.h"

namespace DWL {

	class DDesplegableEx;

	class DDesplegableEx_ExplorarDirectoriosEx : public DExplorarDirectoriosEx {
	  public:
							DDesplegableEx_ExplorarDirectoriosEx(DDesplegableEx *nPadre);
		                   ~DDesplegableEx_ExplorarDirectoriosEx();
//		virtual void		Evento_MouseMovimiento(const int cX, const int cY, const UINT Param)					{ };
		void				Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param);
		void				Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);

		void				Evento_FocoPerdido(HWND hWndNuevoFoco);


		void                Mostrar(void);


	  protected:
		DDesplegableEx    *_Padre;
		BOOL               _Mostrando;

	};

}
