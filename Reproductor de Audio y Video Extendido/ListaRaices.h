#pragma once

#include "DListaEx.h"

class ListaRaices : public DWL::DListaEx {
  public:
								ListaRaices(void);
						       ~ListaRaices(void);
	void						CrearListaRaiz(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);
	DWL::DListaEx_Item         *AgregarRaiz(const wchar_t *nPath, const BOOL nRepintar = TRUE);
	void						Evento_PintarSubItem(HDC hDC, const LONGLONG NumItem, const LONGLONG NumSubItem, RECT *Espacio);

	void                        Evento_MousePresionado(DWL::DEventoMouse &DatosMouse);
	void                        Evento_MouseSoltado(DWL::DEventoMouse &DatosMouse);
  protected:
	DWL::DListaIconos_Icono	  *_IconoPapelera;
	DWL::DListaIconos_Icono	  *_IconoPapeleraResaltado;
	LONGLONG                   _ItemRaizPresionado;
	LONGLONG                   _SubItemRaizPresionado;
};

