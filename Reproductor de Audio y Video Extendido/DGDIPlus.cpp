#include "stdafx.h"
#include "DGDIPlus.h"

namespace Gdiplus {
	// Constructor que obtiene el rectángulo con el área cliente de una ventana
	ClientRect::ClientRect(HWND hWnd) {
		RECT RC;
		GetClientRect(hWnd, &RC);
		X		= 0;
		Y		= 0;
		Width	= RC.right;
		Height	= RC.bottom;
	}







	// Constructor que obtiene el rectángulo con el área de una ventana
	WindowRect::WindowRect(HWND hWnd) {
		RECT RC;
		GetWindowRect(hWnd, &RC);
		X		= RC.left;
		Y		= RC.top;
		Width	= RC.right;
		Height	= RC.bottom;
	}











	// Constructor que obtiene el rectángulo con el área cliente de una ventana
	ClientRectF::ClientRectF(HWND hWnd) {
		RECT RC;
		GetClientRect(hWnd, &RC);
		X		= 0.0f;
		Y		= 0.0f;
		Width	= static_cast<Gdiplus::REAL>(RC.right);
		Height	= static_cast<Gdiplus::REAL>(RC.bottom);
	}







	// Constructor que obtiene el rectángulo con el área de una ventana
	WindowRectF::WindowRectF(HWND hWnd) {
		RECT RC;
		GetWindowRect(hWnd, &RC);
		X		= static_cast<Gdiplus::REAL>(RC.left);
		Y		= static_cast<Gdiplus::REAL>(RC.top);
		Width	= static_cast<Gdiplus::REAL>(RC.right);
		Height	= static_cast<Gdiplus::REAL>(RC.bottom);
	}



}