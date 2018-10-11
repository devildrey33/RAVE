
#ifndef DLISTADESPLEGABLEEX_H
#define DLISTADESPLEGABLEEX_H

#include "DControlDesplegableEx.h"
#include "DListaEx.h"

namespace DWL {
	class DListaDesplegable : public DControlDesplegableEx {
	  public:
						DListaDesplegable(void);
		               ~DListaDesplegable(void);
		void			CrearListaDesplegable(DhWnd *nPadre, const wchar_t *nTexto, INT_PTR nIcono, const int cX, const int cY, const int cAncho, const int cAlto);
		DListaEx        Lista;
	protected:
	};

};
#endif