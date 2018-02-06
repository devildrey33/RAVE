#pragma once

#include "DExplorarDirectoriosEx.h"

namespace DWL {

	class DDesplegableEx;

	class DDesplegableEx_ExplorarDirectorios : public DExplorarDirectoriosEx {
	  public:
							DDesplegableEx_ExplorarDirectorios();
		                   ~DDesplegableEx_ExplorarDirectorios();
		void				Crear();
	  protected:
		DDesplegableEx    *_Padre;
	};

}
