#ifndef DTRACKBAR_H
#define DTRACKBAR_H

#include "DhWnd.h"


namespace DWL {

	class DTrackBar : public DControl {	
	  public:
							DTrackBar(void) : DControl() { };
			               ~DTrackBar(void) {};

		HWND				Crear(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, const UINT nMin, const UINT nMax, const UINT nPos);
		inline void			AsignarRango(const UINT nMin, const UINT nMax)	{ SendMessage(hWnd(), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(nMin, nMax));  };
		inline void			Posicion(const UINT nPos)						{ SendMessage(hWnd(), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nPos); };
		inline const UINT   Posicion(void)                                  { return static_cast<UINT>(SendMessage(hWnd(), TBM_GETPOS, 0, 0)); };
		inline void			TamPagina(const UINT nTam)                      { SendMessage(hWnd(), TBM_SETPAGESIZE, NULL, (LPARAM)nTam); }
	};

};
#endif