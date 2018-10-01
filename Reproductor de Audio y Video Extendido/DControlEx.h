#ifndef DCONTROLEX_H
#define DCONTROLEX_H

#include "DhWnd.h"

namespace DWL {

	// TODO POR IMPLEMENTAR EN TODOS LOS CONTROLES EXTENDIDOS POSIBLES (DBotonEx y DMarcaEx FIJO)
	enum DControlEx_Estado {
		DControlEx_Estado_Normal		= 0,
		DControlEx_Estado_Resaltado		= 1,
		DControlEx_Estado_Presionado	= 2,
		DControlEx_Estado_Desactivado   = 3
	};


	/* Controles de usuario fabricados desde 0 */
	class DControlEx : public DhWnd {
	  public:
									DControlEx(void) : _MouseDentro(FALSE), DhWnd() { }
		                           ~DControlEx(void) { }

		HWND						CrearControlEx(DhWnd *nPadre, const TCHAR *nNombre, const TCHAR *nTexto, const INT_PTR cID, const int cX, const int cY, const int cAncho, const int cAlto, DWORD nEstilos, DWORD nEstilosExtendidos, UINT nEstilosClase = NULL, HBRUSH nColorFondo = NULL);

		inline virtual void			Activado(const BOOL nActivar)	{ EnableWindow(_hWnd, nActivar); };
		inline virtual const BOOL	Activado(void)					{ return IsWindowEnabled(_hWnd); };

		virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
//		inline void					Repintar(void) { RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT); };
	  protected:
		void					   _ConectarControl(const UINT nID, DhWnd *nPadre);
		static LRESULT CALLBACK    _GestorMensajes(HWND HandleVentana, UINT uMsg, WPARAM wParam, LPARAM lParam);

		const BOOL                 _MouseEntrando(void);
		//! Valor que determina si el mouse esta dentro del control
		BOOL                       _MouseDentro;
//		DControlEx_Estado          _Estado;

	};



}

#endif