#pragma once

#include "DArbolEx.h"

class NodoBD : public DArbolEx_Nodo {
  public :
							NodoBD() : DArbolEx_Nodo(), Hash(0) {};
	                       ~NodoBD() {};
	sqlite3_int64			Hash;
  private :
};


class ArbolBD2 : public DArbolEx {
  public:
					ArbolBD2();
	               ~ArbolBD2();
	
};

