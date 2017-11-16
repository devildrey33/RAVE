#include "stdafx.h"
#include "DListView.h"

namespace DWL {
    void DListView_DatosEdicion::Combo_AgregarItem(const TCHAR *nTexto) {
        SendMessage(_hWndEdicion, CB_INSERTSTRING, -1, (LPARAM)nTexto);
    }

    void DListView_DatosEdicion::ComboEdit_AsignarTexto(const TCHAR *nTexto) {
	    SetDlgItemText(GetParent(_hWndEdicion), static_cast<int>(GetWindowLongPtr(_hWndEdicion, GWL_ID)), nTexto);
    }

    void DListView_DatosEdicion::ComboEdit_ObtenerTexto(std::wstring &nTexto) {
        int Tam = GetWindowTextLength(_hWndEdicion);
        if (Tam > 0) {
            TCHAR *TmpStr = new TCHAR[Tam + 1];
			GetDlgItemText(GetParent(_hWndEdicion), static_cast<int>(GetWindowLongPtr(_hWndEdicion, GWL_ID)), TmpStr, Tam + 1);
            nTexto = TmpStr;
            delete [] TmpStr;
        }
        else {
            nTexto.clear();
        }
    }

    void DListView_DatosEdicion::ComboStatic_AsignarSeleccion(const TCHAR *nTexto) {
        SendMessage(_hWndEdicion, CB_SELECTSTRING, -1, (LPARAM)nTexto);
    }

    void DListView_DatosEdicion::ComboStatic_AsignarSeleccion(const int nPos) {
        SendMessage(_hWndEdicion, CB_SETCURSEL , nPos, 0);
    }

    const int DListView_DatosEdicion::ComboStatic_ObtenerSeleccion(void) {
        return static_cast<int>(SendMessage(_hWndEdicion, CB_GETCURSEL , 0, 0));
    }

    void DListView_DatosEdicion::ComboStatic_ObtenerSeleccion(std::wstring &nTexto) {
        int Item = static_cast<int>(SendMessage(_hWndEdicion, CB_GETCURSEL , 0, 0));                                        
        int Tam = static_cast<int>(SendMessage(_hWndEdicion, CB_GETLBTEXTLEN, Item, 0));
        if (Tam > 0) {
	        TCHAR *Tmp = new TCHAR[Tam + 1];
            SendMessage(_hWndEdicion, CB_GETLBTEXT, Item, (LPARAM)Tmp);
	        nTexto = Tmp;
            delete [] Tmp;
        }
        else {
            nTexto.clear();
        }
    }

    void DListView_DatosEdicion::Edit_AsignarTexto(const TCHAR *nTexto) {
        SetDlgItemText(GetParent(_hWndEdicion), static_cast<int>(GetWindowLongPtr(_hWndEdicion, GWL_ID)), nTexto);
    }

    void DListView_DatosEdicion::Edit_ObtenerTexto(std::wstring &nTexto) {
        int Tam = GetWindowTextLength(_hWndEdicion);
        if (Tam > 0) {
            TCHAR *TmpStr = new TCHAR[Tam + 1];
			GetDlgItemText(GetParent(_hWndEdicion), static_cast<int>(GetWindowLongPtr(_hWndEdicion, GWL_ID)), TmpStr, Tam + 1);
            nTexto = TmpStr;
            delete [] TmpStr;
        }
        else {
			nTexto.clear();
        }
    }

};