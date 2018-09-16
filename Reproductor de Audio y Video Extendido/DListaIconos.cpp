#include "stdafx.h"
#include "DListaIconos.h"
#include <Shellapi.h>		// SHGetFileInfo
//#include <Shtypes.h>		
#include <Shlobj.h>			// SHGetSpecialFolderLocation 

namespace DWL {

	std::vector<DListaIconos_Icono *>  DListaIconos::_Iconos;
	INT_PTR                            DListaIconos::_IDNegativa = 0;

	DListaIconos::DListaIconos(void) {
	}


	DListaIconos::~DListaIconos(void) {
		for (size_t i = 0; i < _Iconos.size(); i++) {
			if (_Iconos[i]->_Icono != NULL) DeleteObject(_Iconos[i]->_Icono);
			delete _Iconos[i];
		}
	}


	DListaIconos_Icono *DListaIconos::AgregarIconoRecursos(const INT_PTR IDRecursos, const int nAncho, const int nAlto) {
		for (size_t i = 0; i < _Iconos.size(); i++) {
			if (_Iconos[i]->_ID == IDRecursos && _Iconos[i]->_Alto == nAlto && _Iconos[i]->_Ancho == nAncho) return _Iconos[i];
		}

		HICON hIcono = static_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDRecursos), IMAGE_ICON, nAncho, nAlto, LR_DEFAULTCOLOR));
		
		DListaIconos_Icono *nIcono = new DListaIconos_Icono(hIcono, IDRecursos, nAncho, nAlto);
		_Iconos.push_back(nIcono);
		return nIcono;
	}
	/* // DEPRECATED
	DListaIconos_Icono *DListaIconos::AgregarIconoCSIDL(const int CSIDL, const int nAncho, const int nAlto) {
		DWORD_PTR	 hr;
		LPITEMIDLIST pidl = 0;
		SHFILEINFO   sfi  = { 0 };
		hr = SHGetSpecialFolderLocation(0, CSIDL, &pidl);
		hr = SHGetFileInfo((LPCTSTR)pidl, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICONLOCATION);
		std::wstring Nombre = sfi.szDisplayName;
		int          PosIco = sfi.iIcon;
		DListaIconos_Icono *TmpIco = _BuscarIDStr(sfi.szDisplayName, sfi.iIcon);
		// No existe, lo creamos
		if (TmpIco == NULL) {
			hr = SHGetFileInfo((LPCTSTR)pidl, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICON);
			if (SUCCEEDED(hr)) {
				DListaIconos_Icono *nIcono = new DListaIconos_Icono(sfi.hIcon, --_IDNegativa, nAncho, nAlto, Nombre, PosIco);
				_Iconos.push_back(nIcono);
				return nIcono;
			}
		}
		return NULL;
	}*/

	DListaIconos_Icono *DListaIconos::AgregarIconoKnownFolder(const GUID &rfid, const int nAncho, const int nAlto) {
		PIDLIST_ABSOLUTE	npi;
		DWORD_PTR			Ret = SHGetKnownFolderIDList(rfid, NULL, NULL, &npi);
		SHFILEINFO			sfi = { 0 };
		Ret = SHGetFileInfo((LPCTSTR)npi, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICONLOCATION);
		std::wstring		Nombre = sfi.szDisplayName;
		int					PosIco = sfi.iIcon;

		DListaIconos_Icono *TmpIco = _BuscarIDStr(Nombre.c_str(), PosIco);
		// No existe, lo creamos
		if (TmpIco == NULL) {
			Ret = SHGetFileInfo((LPCTSTR)npi, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICON);
			if (SUCCEEDED(Ret)) {
				DListaIconos_Icono *nIcono = new DListaIconos_Icono(sfi.hIcon, --_IDNegativa, nAncho, nAlto, Nombre, PosIco);
				_Iconos.push_back(nIcono);
				// Borro el PIDLIST_ABSOLUTE
				ILFree(npi);
				return nIcono;
			}
		}
		// Borro el PIDLIST_ABSOLUTE
		ILFree(npi);
		return TmpIco;
	}

	DListaIconos_Icono *DListaIconos::AgregarIconoExterno(const wchar_t *Path, const int nAncho, const int nAlto, const int nPosicionIco) {
		DWORD_PTR	 hr;
		LPITEMIDLIST pidl = 0;
		SHFILEINFO   sfi  = { 0 };
		hr = SHGetFileInfo(Path, 0, &sfi, sizeof(sfi), SHGFI_ICONLOCATION);
		std::wstring	Nombre = sfi.szDisplayName;
		int				PosIco = sfi.iIcon;
		DListaIconos_Icono *TmpIco = _BuscarIDStr(sfi.szDisplayName, sfi.iIcon);
		// No existe, lo creamos
		if (TmpIco == NULL) {
			hr = SHGetFileInfo(Path, nPosicionIco, &sfi, sizeof(sfi), SHGFI_ICON);
			if (SUCCEEDED(hr)) {
				DListaIconos_Icono *nIcono = new DListaIconos_Icono(sfi.hIcon, --_IDNegativa, nAncho, nAlto, Nombre, PosIco);
				_Iconos.push_back(nIcono);
				return nIcono;
			}
		}
		return TmpIco;
	}


	DListaIconos_Icono *DListaIconos::_BuscarIDStr(const wchar_t *nIDStr, const int nPosicionStr) {
		for (size_t i = 0; i < _Iconos.size(); i++) {
			if (_Iconos[i]->_IDStr == nIDStr && _Iconos[i]->_IDStrPos == nPosicionStr) {
				return _Iconos[i];
			}
		}
		return NULL;
	}

}