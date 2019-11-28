#include "stdafx.h"
#include "VerVideo.h"
#include "DMouse.h"
//#include "Rave_Skin.h"
#include "resource.h"

#define ID_TIEMPOOSCY 1000

VerVideo::VerVideo(void) {
}


VerVideo::~VerVideo(void) {
}


HWND VerVideo::Crear(DhWnd *Padre, const int cX, const int cY, const int cAncho, const int cAlto, const UINT cID) {

	DControlEx::CrearControlEx(Padre, L"VerVideo", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD, NULL, CS_DBLCLKS /*| CS_VREDRAW | CS_HREDRAW */);
//	SetTimer(_hWnd, ID_TIEMPOOSCY, 16, NULL);
	return _hWnd;
}

/*
void VerVideo::Evento_MenuEx_Click(const UINT cID) {
	// Pistas de audio
	if (cID >= ID_MENUVIDEO_AUDIO_PISTAS_AUDIO && cID < ID_MENUVIDEO_AUDIO_PISTAS_AUDIO_FIN) {
		// Des-marco todas las pistas de audio
		for (size_t i = 0; i < App.MenuPistasDeAudio->TotalMenus(); i++) {
			App.MenuPistasDeAudio->Menu(i)->Icono(0);
		}
		// Marco la pista de audio actual (si existe, ... que debería)
		DMenuEx *MenuClick = App.MenuPistasDeAudio->BuscarMenu(cID);
		if (MenuClick != NULL) MenuClick->Icono(IDI_CHECK2);

		App.VLC.AsignarPistaAudio(cID - ID_MENUVIDEO_AUDIO_PISTAS_AUDIO);
		return;
	}


	// Proporción
	if (cID >= ID_MENUVIDEO_PROPORCION_PREDETERMINADO && cID < ID_MENUVIDEO_PROPORCION_5A4 + 1) {
		// Des-marco todas las porporciones
		for (size_t i = 0; i < App.MenuProporcion->TotalMenus(); i++) {
			App.MenuProporcion->Menu(i)->Icono(0);
		}
		switch (cID) {
			case ID_MENUVIDEO_PROPORCION_PREDETERMINADO:		App.VLC.AsignarProporcion(NULL);			break;
			case ID_MENUVIDEO_PROPORCION_16A9:					App.VLC.AsignarProporcion("16:9");			break;
			case ID_MENUVIDEO_PROPORCION_4A3:					App.VLC.AsignarProporcion("4:3");			break;
			case ID_MENUVIDEO_PROPORCION_1A1:					App.VLC.AsignarProporcion("1:!");			break;
			case ID_MENUVIDEO_PROPORCION_16A10:					App.VLC.AsignarProporcion("16:10");			break;
			case ID_MENUVIDEO_PROPORCION_2P21A1:				App.VLC.AsignarProporcion("2.21:1");		break;
			case ID_MENUVIDEO_PROPORCION_2P35A1:				App.VLC.AsignarProporcion("2.35:1");		break;
			case ID_MENUVIDEO_PROPORCION_2P39A1:				App.VLC.AsignarProporcion("2.39:1");		break;
			case ID_MENUVIDEO_PROPORCION_5A4:					App.VLC.AsignarProporcion("5:4");			break;
		}

		App.MenuProporcion->Menu(cID - ID_MENUVIDEO_PROPORCION_PREDETERMINADO)->Icono(IDI_CHECK2);
	}

	if (cID == ID_MENUVIDEO_BRILLO) {
//		App.VLC.Hue(200);
//		App.VLC.Saturacion(1.0);
//		App.VLC.Saturacion(1.0);
	}
}*/

void VerVideo::_Evento_MouseRueda(WPARAM wParam, LPARAM lParam) {
	DEventoMouseRueda DatosMouse(wParam, lParam, this);
//	if (App.VLC.MediaPlayer() != NULL) {
		Debug_Escribir_Varg(L"VerVideo::_Evento_MouseRueda()  DatosMouse.Delta(%d)\n", DatosMouse.Delta());
		int Vol = App.MP.Volumen();
		Vol += (DatosMouse.Delta() > 0) ? 10 : -10;
		App.MP.Volumen(Vol);
		App.Opciones.Volumen(Vol);
//	}
}



void VerVideo::Pintar(HDC hDC) {
	RECT R;
	GetClientRect(hWnd(), &R);

//	HBRUSH Fondo = CreateSolidBrush(COLOR_TOOLTIP_FONDO);
	FillRect(hDC, &R, (HBRUSH)GetStockObject(BLACK_BRUSH));
//	DeleteObject(Fondo);
}


/*
void VerVideo::PintarOscyloscopio(void) {
	RECT RC, RO;
	GetClientRect(_hWnd, &RC);
	HDC DC = GetDC(_hWnd);

	// Rectangulo donde se pintara el oscy
	RO.left		= (RC.right - 512) / 2;
	RO.right	= RO.left + 512;
	RO.top		= (RC.bottom - 256) / 2;
	RO.bottom	= RO.top + 256;


	// Creo un buffer en memória para pintar el control
	HDC		Buffer		= CreateCompatibleDC(NULL);
	// Creo un DC compatible para el buffer
	HBITMAP Bmp			= CreateCompatibleBitmap(DC, RO.right - RO.left, RO.bottom - RO.top);
	HBITMAP BmpViejo	= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

	double MaxOscy = App.VLC.MaxOscy;

//	long MaxAlto = ((MaxOscy * 2) > 512.0f) ? 512.0f : MaxOscy * 2;


	// Borro el fondo
	FillRect(Buffer, &RC, (HBRUSH)GetStockObject(BLACK_BRUSH));
		
	HPEN Pluma = CreatePen(PS_SOLID, 1, COLOR_ROJO);
	HPEN VPluma = static_cast<HPEN>(SelectObject(Buffer, Pluma));
	MoveToEx(Buffer, 0, 128, NULL);

	float (&Oscy)[2048] = App.VLC.Oscy;

	for (int i = 0; i < 512; i++) {
		LineTo(Buffer, i, 128 + static_cast<long>(128.0f * Oscy[i]));
	}
	SelectObject(Buffer, VPluma);
	DeleteObject(Pluma);

	// Copio el buffer al DC
	BitBlt(DC, RO.left, RO.top, RO.right - RO.left, RO.bottom - RO.top, Buffer, 0, 0, SRCCOPY);

	// Elimino el buffer de la memória
//	SelectObject(Buffer, vFuente);
	SelectObject(Buffer, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(Buffer);


	ReleaseDC(_hWnd, DC);
}*/

void VerVideo::MostrarMenuVideo(void) {
	
	App.MenuVideoMomentos->Activado(FALSE);
	// Creo los sub menus para los momentos (si es que existe algun momento)
	if (App.MP.MedioActual().Id != 0) {
		App.MenuVideoMomentos->Activado(TRUE);
		DWL::DMenuEx* TmpMenu = NULL;
		App.MenuVideoMomentos->EliminarTodosLosMenus();
		for (size_t i = 0; i < App.MP.MedioActual().Momentos.size(); i++) {
			TmpMenu = NULL;
			TmpMenu = App.MenuVideoMomentos->AgregarMenu(ID_MENULISTA_MOMENTOS_MOMENTO + i, App.MP.MedioActual().Momentos[i]->Nombre, (App.MP.MedioActual().Momentos[i]->Excluir) ? IDI_MOMENTO_EXCLUIR : IDI_MOMENTO);
			if (TmpMenu) TmpMenu->Parametro = App.MP.MedioActual().Hash;
		}
	}

	App.VentanaRave.Menu_Video.Mostrar(&App.VentanaRave);

}

LRESULT CALLBACK VerVideo::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static POINT nPos = { 0 , 0 };
	switch (uMsg) {
/*		case WM_TIMER :
			PintarOscyloscopio();
			return 0;*/
						/*		case WM_ERASEBKGND :
									return 1;*/
		case WM_PAINT:
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(hWnd(), &PS);
			Pintar(DC);
			EndPaint(hWnd(), &PS);
			return 0;
		case WM_SIZING:
			Repintar();
			return 0;
		case WM_MOUSEMOVE:
			//			Debug_Escribir(L"VerView::GestorMensajes uMsg = WM_MOUSEMOVE\n");
			DWL::DMouse::ObtenerPosicion(&nPos);
			if (App.VentanaRave.PantallaCompleta() == TRUE) {
				if (App.ControlesPC._MousePos.x != nPos.x || App.ControlesPC._MousePos.y != nPos.y) {
					App.ControlesPC._MousePos = nPos;
					App.ControlesPC.Mostrar();
				}
			}
			return 0;
		case WM_MOUSEWHEEL:
			_Evento_MouseRueda(wParam, lParam);
			return 0;
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
			App.VentanaRave.PantallaCompleta(!App.VentanaRave.PantallaCompleta());
			return 0;
		case WM_RBUTTONUP:
			MostrarMenuVideo();
			return 0;
			/*		case WM_KEYUP:
						if (wParam == VK_ESCAPE) {
							App.VentanaRave.PantallaCompleta(FALSE);
						}
						return 0;*/

	}
	return DControlEx::GestorMensajes(uMsg, wParam, lParam);
}