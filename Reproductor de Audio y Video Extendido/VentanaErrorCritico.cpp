#include "stdafx.h"
#include "VentanaErrorCritico.h"
#include "resource.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"
#include "DDirectoriosWindows.h"
#include "EnviarDump.h"
//#include "CSmtp.h"
#include <locale> 
#include <codecvt>
#include "DMouse.h"

#define ID_TEMPORIZADOR 100

VentanaErrorCritico::VentanaErrorCritico(void) : DWL::DVentana() {
}


VentanaErrorCritico::~VentanaErrorCritico(void) {
}


HWND VentanaErrorCritico::Crear(void) {
	CrearVentana(NULL, L"Rave_ErrorCritico", L"Error crítico", 100, 100, 440, 300, WS_OVERLAPPEDWINDOW | WS_VISIBLE, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
	
	RECT RC;
	GetClientRect(hWnd(), &RC);

	TextoTitulo.CrearEtiquetaEx(this, L"Error crítico!", 10, 10, RC.right - 20, 20, ID_VEC_TEXTOERROR, TRUE, WS_CHILD);
	TextoTitulo.Fuente = Fuente21Negrita;
	TextoTitulo.ColorTexto = COLOR_BOTON_RESALTADO;
	TextoTitulo.Visible(TRUE);

	std::wstring TE =	L"Se ha detectado un error, y el Reproductor de Audio y Video\nExtendido debe cerrarse.\n\n"
						L"Si lo deseas puedes mandar el informe de errores para que \n"
						L"sea revisado pulsando el boton Enviar. De esta forma estaras \n"
						L"ayudando al desarrollo de este reproductor."
						L"\n\nEn caso contrario pula el boton Salir. ";
	TextoError.CrearEtiquetaEx(this, TE.c_str(), 10, 40, RC.right - 20, 150, ID_VEC_TEXTOERROR, FALSE, WS_CHILD | WS_VISIBLE);
	TextoError.Fuente = Fuente18Normal;

	BarraProgreso.CrearBarraProgresoEx(this, 10, 200, RC.right - 20, 10, ID_VEC_PROGRESO, 0.0f, 100.0f, 0.0f);
	
	BotonEnviar.CrearBotonEx(this, L"Enviar", 100, 220, 100, 30, ID_VEC_ENVIAR);
	BotonSalir.CrearBotonEx(this, L"Salir", RC.right - 200, 220, 100, 30, ID_VEC_SALIR);

	BarraTareas.Estado_Error();
	BarraTareas.Valor(100, 100);

//	SetTimer(_hWnd, ID_TEMPORIZADOR, 500, NULL);
	return hWnd();
}

void VentanaErrorCritico::Evento_BorraFondo(HDC DC) {
	RECT RC;
	GetClientRect(hWnd(), &RC);
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);
	DeleteObject(BrochaFondo);
}

void VentanaErrorCritico::Enviar(void) {
	// Path para el archivo dump
	std::wstring WPathDump;
	BOOL R = DWL::DDirectoriosWindows::Comun_AppData(WPathDump);
	WPathDump += L"\\Rave\\ErrorCritico.dmp";

/*	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	std::string PathDump = converterX.to_bytes(WPathDump.c_str());*/

	EnviarDump E;
	BarraTareas.Estado_Indeterminado();
	E.Enviar(WPathDump, _hWnd);
	DMouse::CambiarCursor(DMouse_Cursor_FlechaReloj);

	BotonEnviar.Activado(FALSE);
}

void VentanaErrorCritico::Evento_BotonEx_Mouse_Click(DEventoMouse &DatosTeclado) {
	switch (DatosTeclado.ID()) {
		case ID_VEC_ENVIAR:
			Enviar();
			break;
		case ID_VEC_SALIR:
			PostMessage(hWnd(), WM_CLOSE, 0, 0);
			break;
	}
}
/*
void VentanaErrorCritico::Evento_Temporizador(void) {
	_EstadoError = !_EstadoError;
	if (_EstadoError == FALSE)	BarraTareas.Estado_Error();
	else                        BarraTareas.Estado_Pausado();
}*/

LRESULT CALLBACK VentanaErrorCritico::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_THREAD_ENVIARVALOR :
			BarraProgreso.Maximo(static_cast<float>(wParam));
			BarraProgreso.Valor(static_cast<float>(lParam));
			BarraTareas.Estado_Normal();
			BarraTareas.Valor(static_cast<UINT>(lParam), static_cast<UINT>(wParam));
			break;
		case WM_THREAD_TERMINADO :
			if (wParam == 1)	{	Debug_Escribir_Varg(L"EnviarDump::Enviar Error durante la transmisión.\n");	}
			else				{	Debug_Escribir_Varg(L"EnviarDump::Enviar Se ha enviado correctamente.\n");	}
			PostMessage(_hWnd, WM_CLOSE, 0, 0);
			break;

/*		case WM_TIMER :
			Evento_Temporizador();
			break;*/
		case WM_ERASEBKGND:
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));
			break;
	}
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}