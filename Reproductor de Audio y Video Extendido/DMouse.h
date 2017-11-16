#ifndef DMOUSE_H
    #define DMOUSE_H
    
//    #include "DObjeto.h"

    namespace DWL {

        enum DEnum_Mouse_Cursor {
	        DEnum_Mouse_Cursor_FlechaReloj		= 32650,	// IDC_APPSTARTING
	        DEnum_Mouse_Cursor_FlechaVertical	= 32516,	// IDC_UPARROW
	        DEnum_Mouse_Cursor_Flecha			= 32512,	// IDC_ARROW
	        DEnum_Mouse_Cursor_Cruz				= 32515,    // IDC_CROSS
	        DEnum_Mouse_Cursor_Mano				= 32649,	// IDC_HAND
	        DEnum_Mouse_Cursor_Ayuda			= 32651,	// IDC_HELP
	        DEnum_Mouse_Cursor_IBeam			= 32513,	// IDC_IBEAM
	        DEnum_Mouse_Cursor_Cancelar			= 32648,	// IDC_NO
	        DEnum_Mouse_Cursor_Tam				= 32646,	// IDC_SIZEALL
	        DEnum_Mouse_Cursor_TamNESO			= 32643,	// IDC_SIZENESW
	        DEnum_Mouse_Cursor_TamNS			= 32645,	// IDC_SIZENS
	        DEnum_Mouse_Cursor_TamNOSE			= 32642,	// IDC_SIZENWSE
	        DEnum_Mouse_Cursor_TamEO			= 32644,	// IDC_SIZEWE
	        DEnum_Mouse_Cursor_Procesando		= 32514     // IDC_WAIT
        }; 

		class DMouse {
          public:
//			virtual const DEnum_Objeto		Objeto_Tipo(void) { return DEnum_Objeto_Mouse; };
											DMouse(void) { };

			static HCURSOR					CambiarCursor(const DEnum_Mouse_Cursor nCursor = DEnum_Mouse_Cursor_Flecha);

			static HCURSOR					CambiarCursor(HCURSOR nCursor);

			static int						Visible(const BOOL nVisible);

			static BOOL						ObtenerPosicion(POINT *Pos);

			static BOOL						AsignarPosicion(const int cX, const int cY);

			static BOOL						AsignarPosicion(const POINT *nPos);

			static BOOL						AreaRaton(const RECT *nArea);
        };
    };

#endif