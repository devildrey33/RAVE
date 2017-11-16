#pragma once

#include "sqlite3.h"

class TablaBase {
  public:
	TablaBase();
	~TablaBase();

  protected: 
	const BOOL		       _Query(const wchar_t *nQuery);
	sqlite3               *_BD;
};

