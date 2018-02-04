#include "stdafx.h"
#include "DListaEx_Item.h"
#include "DListaIconos.h"

namespace DWL {
	/*
	DListaEx_Item::DListaEx_Item() : _Icono(NULL), Seleccionado(FALSE) {
	}
	/*
	DListaEx_Item::DListaEx_Item(const TCHAR *nTexto, DListaIconos_Icono *nIcono) : _Icono(nIcono), Seleccionado(FALSE) {
		Texto = nTexto;
	}

	DListaEx_Item::DListaEx_Item(std::wstring &nTexto, DListaIconos_Icono *nIcono) : Texto(nTexto), _Icono(nIcono), Seleccionado(FALSE) {
	}

	*/

	DListaEx_Item::~DListaEx_Item()	{
		for (size_t i = 0; i < _SubItems.size(); i++) {
			delete _SubItems[i];
		}
		_SubItems.resize(0);
	}

	void DListaEx_Item::Icono(const int nIconoRecursos) {
		_Icono = DListaIconos::AgregarIconoRecursos(nIconoRecursos, DLISTAEX_TAMICONO, DLISTAEX_TAMICONO);
	}

}