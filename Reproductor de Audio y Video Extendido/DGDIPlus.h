#ifndef DGDIPLUS_H
#define DGDIPLUS_H

#include "DFuenteP.h"
#include "DIconoP.h"
// Cabecera para cargar la GDI+, con algunas utilidades

namespace Gdiplus {
	class ClientRect : public Gdiplus::Rect {
	  public:
		  ClientRect(HWND hWnd);
	};

	class WindowRect : public Gdiplus::Rect {
	  public:
		  WindowRect(HWND hWnd);
	};


	class ClientRectF : public Gdiplus::RectF {
	  public:
		ClientRectF(HWND hWnd);
	};

	class WindowRectF : public Gdiplus::RectF {
	  public:
		WindowRectF(HWND hWnd);
	};
}

using namespace Gdiplus;

namespace DWL {

}

#endif