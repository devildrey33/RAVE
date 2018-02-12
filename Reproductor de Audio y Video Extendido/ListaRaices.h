#pragma once

#include "DListaEx.h"

class ListaRaices : public DWL::DListaEx {
  public:
								ListaRaices(void);
						       ~ListaRaices(void);
	void						CrearListaRaiz(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const int cID);
	DWL::DListaEx_Item         *AgregarRaiz(const wchar_t *nPath);
	void						Evento_PintarSubItem(HDC hDC, const size_t NumItem, const size_t NumSubItem, RECT *Espacio);

	void                        Evento_MousePresionado(const UINT Boton, const int cX, const int cY, const UINT Param);
	void                        Evento_MouseSoltado(const UINT Boton, const int cX, const int cY, const UINT Param);
  protected:
	DWL::DListaIconos_Icono	  *_IconoPapelera;
	DWL::DListaIconos_Icono	  *_IconoPapeleraResaltado;
	size_t                     _ItemRaizPresionado;
	size_t                     _SubItemRaizPresionado;
};

