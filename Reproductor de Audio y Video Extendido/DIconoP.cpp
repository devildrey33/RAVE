#include "stdafx.h"
#include <Shellapi.h>		// SHGetFileInfo
//#include <Shtypes.h>		
#include <Shlobj.h>			// SHGetSpecialFolderLocation 
#include "DIconoP.h"

namespace DWL {

	std::vector<DIconoP::DIconoP_Datos *>  DIconoP::_Iconos;
	INT_PTR                                DIconoP::_IDNegativa = 0;


	DIconoP::DIconoP(void) : _Icono(NULL) {
	}

	DIconoP::DIconoP(const INT_PTR nIDRecursos, const int nAncho, const int nAlto) : _Icono(NULL) {
		CrearIconoRecursos(nIDRecursos, nAncho, nAlto);
	}

	DIconoP::DIconoP(const GUID &rfid, const int nAncho, const int nAlto) : _Icono(NULL) {
		CrearIconoKnownFolder(rfid, nAncho, nAlto);
	}

	DIconoP::DIconoP(const wchar_t *Path, const int nAncho, const int nAlto, const int nPosicionIco) : _Icono(NULL) {
		CrearIconoExterno(Path, nAncho, nAlto);
	}


	DIconoP::~DIconoP(void) {
	}


	void DIconoP::CrearIconoRecursos(const INT_PTR IDRecursos, const int nAncho, const int nAlto) {
		// Sin icono
		if (IDRecursos == 0) {
			_Icono = NULL;
			return;
		}
		// Busco el icono de los recursos
		for (size_t i = 0; i < _Iconos.size(); i++) {
			if (_Iconos[i]->ID == IDRecursos && _Iconos[i]->Alto == nAlto && _Iconos[i]->Ancho == nAncho) {
				_Icono = _Iconos[i];
				return;
			}
		}

		HICON nIcono = static_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDRecursos), IMAGE_ICON, nAncho, nAlto, LR_DEFAULTCOLOR));

		_Icono = new DIconoP_Datos(nIcono, _CrearBitmap(nIcono), IDRecursos, nAncho, nAlto);
		_Iconos.push_back(_Icono);
//		DeleteObject(hIcono);
	}

	void DIconoP::CrearIconoKnownFolder(const GUID &rfid, const int nAncho, const int nAlto) {
		PIDLIST_ABSOLUTE	npi;
		DWORD_PTR			Ret = SHGetKnownFolderIDList(rfid, NULL, NULL, &npi);
		SHFILEINFO			sfi = { 0 };
		Ret = SHGetFileInfo((LPCTSTR)npi, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICONLOCATION);
		std::wstring		Nombre = sfi.szDisplayName;
		int					PosIco = sfi.iIcon;

		_Icono = _BuscarIDStr(Nombre.c_str(), PosIco);
		// No existe, lo creamos
		if (_Icono == NULL) {
			Ret = SHGetFileInfo((LPCTSTR)npi, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICON);
			if (SUCCEEDED(Ret)) {
				_Icono = new DIconoP_Datos(sfi.hIcon, _CrearBitmap(sfi.hIcon), --_IDNegativa, nAncho, nAlto, Nombre, PosIco);
				_Iconos.push_back(_Icono);
//				DeleteObject(sfi.hIcon);
			}
		}
		// Borro el PIDLIST_ABSOLUTE
		ILFree(npi);
	}

	void DIconoP::CrearIconoExterno(const wchar_t *Path, const int nAncho, const int nAlto, const int nPosicionIco) {
		DWORD_PTR	 hr;
		LPITEMIDLIST pidl = 0;
		SHFILEINFO   sfi = { 0 };
		hr = SHGetFileInfo(Path, 0, &sfi, sizeof(sfi), SHGFI_ICONLOCATION);
		std::wstring  Nombre = sfi.szDisplayName;
		std::transform(Nombre.begin(), Nombre.end(), Nombre.begin(), toupper); // transformo el path a mayusculas
		int			  PosIco = sfi.iIcon;
		// Busco el icono
		_Icono = _BuscarIDStr(Nombre.c_str(), sfi.iIcon);
		// No existe, lo creamos
		if (_Icono == NULL) {
			hr = SHGetFileInfo(Path, nPosicionIco, &sfi, sizeof(sfi), SHGFI_ICON);
			if (SUCCEEDED(hr)) {
				_Icono = new DIconoP_Datos(sfi.hIcon, _CrearBitmap(sfi.hIcon), --_IDNegativa, nAncho, nAlto, Nombre, PosIco);
				_Iconos.push_back(_Icono);
//				DeleteObject(sfi.hIcon);
				//return nIcono;
			}
		}
	}


	void DIconoP::EliminarIconos(void) {
		for (size_t i = 0; i < _Iconos.size(); i++) {
			if (_Iconos[i]->Icono != NULL) {
				DeleteObject(_Iconos[i]->hIcono);
				delete _Iconos[i]->Icono;
			}
			delete _Iconos[i];
		}
	}

	DIconoP::DIconoP_Datos *DIconoP::_BuscarIDStr(const wchar_t *nIDStr, const int nPosicionStr) {		
		for (size_t i = 0; i < _Iconos.size(); i++) {
			if (_Iconos[i]->IDStr == nIDStr && _Iconos[i]->IDStrPos == nPosicionStr) {
				return _Iconos[i];
			}
		}
		return NULL;
	}

	Gdiplus::Bitmap *DIconoP::_CrearBitmap(HICON nIcono) {
		// https://stackoverflow.com/questions/11338009/how-do-i-copy-an-hicon-from-gdi-to-gdi-with-transparency
		ICONINFO ii; 
		GetIconInfo(nIcono, &ii);
		
		BITMAP bmp; 
		GetObject(ii.hbmColor, sizeof(bmp), &bmp);
		
		Gdiplus::Bitmap		temp(ii.hbmColor, NULL);
		Gdiplus::BitmapData lockedBitmapData;
		Gdiplus::Rect		rc(0, 0, temp.GetWidth(), temp.GetHeight());

		temp.LockBits(&rc, Gdiplus::ImageLockModeRead, temp.GetPixelFormat(), &lockedBitmapData);

		Gdiplus::Bitmap *Imagen = new Gdiplus::Bitmap(
			lockedBitmapData.Width, lockedBitmapData.Height, lockedBitmapData.Stride,
			PixelFormat32bppARGB, reinterpret_cast<BYTE *>(lockedBitmapData.Scan0));

		temp.UnlockBits(&lockedBitmapData);

//		Gdiplus::Bitmap *Imagen = Gdiplus::Bitmap::FromHICON(nIcono);

		return Imagen;
	}
};