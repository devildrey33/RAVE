#include "stdafx.h"
#include "DImageList.h"
#include <shlobj.h>
#include <string>

#include <Shellapi.h>

namespace DWL {

   /* DImageList_ImagenIconoRecursos::DImageList_ImagenIconoRecursos(const int IDRecursos, const int cAncho, const int cAlto) : DImageList_Imagen(), _Icono(NULL) { 
        _Tipo = DEnum_ImageList_TipoImagen_IconoRecursos;
        _Icono = static_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDRecursos), IMAGE_ICON, cAncho, cAlto, LR_DEFAULTCOLOR)); 
    };

    DImageList_ImagenIconoRecursos::~DImageList_ImagenIconoRecursos(void) {
       if (_Icono != NULL) DestroyIcon(_Icono);
       _Icono = NULL;
    };




    DImageList_ImagenIconoCSLID::DImageList_ImagenIconoCSLID(HICON nIcono) : DImageList_Imagen(), _Icono(nIcono) { 
        _Tipo = DEnum_ImageList_TipoImagen_IconoCSLID; 
    };

    DImageList_ImagenIconoCSLID::~DImageList_ImagenIconoCSLID(void) {
       if (_Icono != NULL) DestroyIcon(_Icono);
       _Icono = NULL;
    };*/












    void DImageList::Crear(const int cAncho, const int cAlto) { 
        Destruir(); 
        _ImageList = ImageList_Create(cAncho, cAlto, ILC_COLOR32 | ILC_MASK, 1, 0); 
    };

    void DImageList::Destruir(void) { 
/*        for (size_t i = 0; i < _ListaImagenes.size(); i++) {
            switch (_ListaImagenes[i]->Tipo()) {
                case DEnum_ImageList_TipoImagen_Indefinido : 
                    delete _ListaImagenes[i];
                    break;
                case DEnum_ImageList_TipoImagen_Icono :
                    delete static_cast<DImageList_ImagenIcono *>(_ListaImagenes[i]);
                    break;
                case DEnum_ImageList_TipoImagen_IconoCSLID :
                    delete static_cast<DImageList_ImagenIconoCSLID *>(_ListaImagenes[i]);
                    break;
                case DEnum_ImageList_TipoImagen_IconoRecursos :
                    delete static_cast<DImageList_ImagenIconoRecursos *>(_ListaImagenes[i]);
                    break;
            }
        }*/
        ImageList_Destroy(_ImageList); 
        _ImageList = NULL;
    };


    const int DImageList::AgregarIconoRecursos(const int cID) {
        int nAncho = 0;
        int nAlto = 0;
        ImageList_GetIconSize(_ImageList, &nAncho, &nAlto);
        HICON Icono = static_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(cID), IMAGE_ICON, nAncho, nAlto, LR_DEFAULTCOLOR)); 
//        DImageList_ImagenIconoRecursos *nIcono = new DImageList_ImagenIconoRecursos(cID, nAncho, nAlto);
        ImageList_AddIcon(_ImageList, Icono); 
        DestroyIcon(Icono);
        DImageList_ID IDIMG;
        _ListaImagenes.push_back(IDIMG);
        return static_cast<int>(_ListaImagenes.size());
//        _ListaImagenes.push_back(nIcono);
    };

    const int DImageList::AgregarIconoCSLID(const int CSIDL) {
        DWORD_PTR hr;
        LPITEMIDLIST pidl = 0;
        SHFILEINFO sfi = { 0 };
        hr = SHGetSpecialFolderLocation(0, CSIDL, &pidl);
        hr = SHGetFileInfo((LPCTSTR)pidl, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICONLOCATION);
        std::wstring Nombre;
        Nombre = sfi.szDisplayName;
        int	PosIco = sfi.iIcon;
        hr = SHGetFileInfo((LPCTSTR)pidl, -1, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_ICON);
//        DImageList_ImagenIconoCSLID *nIcono = new DImageList_ImagenIconoCSLID(sfi.hIcon);
        ImageList_AddIcon(_ImageList, sfi.hIcon); 
        DestroyIcon(sfi.hIcon);
        DImageList_ID IDIMG(Nombre.c_str(), PosIco);
        _ListaImagenes.push_back(IDIMG);
        return static_cast<int>(_ListaImagenes.size());
//        _ListaImagenes.push_back(nIcono);
    };

    const int DImageList::AgregarIconoExplorador(const TCHAR *nPath) {
		DWORD_PTR    hr;
		LPITEMIDLIST pidl = 0;
        SHFILEINFO   sfi = { 0 };
		std::wstring Nombre;
        int		     PosIco;
		hr = SHGetFileInfo(nPath, 0, &sfi, sizeof(sfi), SHGFI_ICONLOCATION);
		Nombre = sfi.szDisplayName;
		PosIco = sfi.iIcon;
		//DWLListaIconos_Icono *TmpIco = BuscarIDStr(Nombre(), PosIco);

        int PosImageList = _BuscarID(Nombre.c_str(), PosIco);
        if (PosImageList == -1) {
			hr = SHGetFileInfo(nPath, 0, &sfi, sizeof(sfi), SHGFI_ICON);
            if (SUCCEEDED(hr)) {
                ImageList_AddIcon(_ImageList, sfi.hIcon); 
                DestroyIcon(sfi.hIcon);
				DImageList_ID IDIMG(Nombre.c_str(), PosIco);
                _ListaImagenes.push_back(IDIMG);
                return static_cast<int>(_ListaImagenes.size()) - 1;
            }
        }
        return PosImageList;
    }

    const int DImageList::_BuscarID(const TCHAR *nPath, const int PosIco) {
        for (int i = 0; i < static_cast<int>(_ListaImagenes.size()); i++) {
            if (_ListaImagenes[i].Path == nPath && _ListaImagenes[i].PosIco == PosIco) 
                return i;
        }
        return -1;
    }

};