#pragma once
#include <DListaEx.h>

class BDMedio;


class ItemMedio : public DWL::DListaEx_Item {
  public:
				ItemMedio(void) { };
	           ~ItemMedio(void) { };

	BDMedio		BdMedio;
};
