
#ifndef DCONTROLDESPLEGABLEEX_H
#define DCONTROLDESPLEGABLEEX_H

#include "DBotonEx.h"
#include "DEdicionTextoEx.h"

namespace DWL {
	// Base para los controles desplegables (viene a ser un control con un icono, un texto y un boton)
	class DControlDesplegableEx : public DControlEx {
	  public:
								DControlDesplegableEx();
		                       ~DControlDesplegableEx();
		virtual void            Evento_Desplegar(void) { };
		LRESULT CALLBACK		GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
//		void                    Pintar(HDC DC);
		void					CrearControlDesplegable(DhWnd *nPadre, const wchar_t *nTexto, const INT_PTR nID, const INT_PTR nIDIcono, const int cX, const int cY, const int cAncho, const int cAlto);
	protected:
		DBotonEx               _Boton;
		DEdicionTextoEx        _Edicion;
		DListaIconos_Icono    *_Icono;
	};

};
#endif