#include "stdafx.h"
#include "DTrackBar.h"

namespace DWL {

	HWND DTrackBar::Crear(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID, DWORD nEstilos, const UINT nMin, const UINT nMax, const UINT nPos) {
		if (hWnd()) { Debug_Escribir(L"DTrackBar::Crear() Error : ya se ha creado el slider\n");  return hWnd(); }
		HWND hWndPadre = (nPadre != NULL) ? nPadre->hWnd() : NULL;
		_hWnd = CreateWindowEx(NULL, TRACKBAR_CLASS, L"", nEstilos, cX, cY, cAncho, cAlto, hWndPadre, reinterpret_cast<HMENU>(IntToPtr(cID)), GetModuleHandle(NULL), this);
		_ConectarControl(cID, nPadre);

		AsignarRango(nMin, nMax);
		Posicion(nPos);


		return hWnd();
	}



};