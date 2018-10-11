
#ifndef DLISTADESPLEGABLEEX_H
#define DLISTADESPLEGABLEEX_H

#include "DListaEx.h"

namespace DWL {
	class DListaDesplegable {
	  public:
						DListaDesplegable(void);
		               ~DListaDesplegable(void);
	  protected:
		DListaEx        Lista;
	};

};
#endif