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


VentanaErrorCritico::VentanaErrorCritico() {
}


VentanaErrorCritico::~VentanaErrorCritico() {
}


HWND VentanaErrorCritico::Crear(void) {
	CrearVentana(L"Rave_ErrorCritico", L"Error crítico", 100, 100, 400, 280, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	
	RECT RC;
	GetClientRect(hWnd(), &RC);

	TextoTitulo.CrearLabelEx(hWnd, L"Error crítico!", 10, 10, RC.right - 20, 20, ID_VEC_TEXTOERROR, TRUE, WS_CHILD);
	TextoTitulo.Fuente(hWnd._FuenteB);
	TextoTitulo.ColorTexto(COLOR_BOTON_RESALTADO);
	TextoTitulo.hWnd.Visible(TRUE);

	std::wstring TE =	std::wstring(L"Se ha detectado un error, y el Reproductor de Audio y Video\nExtendido debe cerrarse.\n\n") + 
						std::wstring(L"Si lo deseas puedes mandar el informe de errores para que \n") +
						std::wstring(L"sea revisado pulsando el boton Enviar. De esta forma estaras \n") +
						std::wstring(L"ayudando al desarrollo de este reproductor.") +
						std::wstring(L"\n\nEn caso contrario pula el boton Salir. ");
	TextoError.CrearLabelEx(hWnd, TE.c_str(), 10, 40, 380, 130, ID_VEC_TEXTOERROR, FALSE, WS_CHILD | WS_VISIBLE);

	BarraProgreso.CrearBarraProgresoEx(hWnd, 10, 180, RC.right - 20, 10, ID_VEC_PROGRESO, 0.0f, 100.0f, 0.0f);
	
	BotonEnviar.CrearBotonEx(hWnd, L"Enviar", 85, 200, 100, 30, ID_VEC_ENVIAR);
	BotonSalir.CrearBotonEx(hWnd, L"Salir", 195, 200, 100, 30, ID_VEC_SALIR);
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
	E.Enviar(WPathDump, BarraProgreso);

/*	try {
		CSmtp mail;

		mail.SetSMTPServer("smtp.1and1.es", 25);
		mail.SetLogin("bubatronik.app");
		mail.SetPassword("cuentamandadumps");
		mail.SetSenderName("RAVE");
		mail.SetSenderMail("bubatronik.app@devildrey33.es");
		mail.SetReplyTo("bubatronik.app@devildrey33.es");
		mail.SetSubject("RAVE Dump");
		mail.AddRecipient("devildrey33@hotmail.com");
		mail.SetXPriority(XPRIORITY_NORMAL);
		mail.SetXMailer("The Bat! (v3.02) Professional");
		char Buffer[128];
		sprintf_s(Buffer, 128, "RAVE %f en %s", RAVE_VERSION, App.SO.c_str());
		mail.AddMsgLine(Buffer);
		mail.AddAttachment(PathDump.c_str());
		mail.Send();
	}
	catch (ECSmtp e) {
		//std::wstring StrError = e.GetErrorText().c_str();
		Debug_Escribir_Varg(L"VentanaErrorCritico::Enviar %s.\n", e.GetErrorText().c_str());
	}*/
}

void VentanaErrorCritico::Evento_BotonEx_Mouse_Click(const UINT nID) {
	switch (nID) {
		case ID_VEC_ENVIAR:
			Enviar();
			break;
		case ID_VEC_SALIR:
			PostMessage(hWnd(), WM_CLOSE, 0, 0);
			break;
	}
}

LRESULT CALLBACK VentanaErrorCritico::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_ERASEBKGND:
			Evento_BorraFondo(reinterpret_cast<HDC>(wParam));
			return TRUE;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case DWL_BOTONEX_CLICK:
			Evento_BotonEx_Mouse_Click(static_cast<UINT>(wParam));
			break;
	}
	return DefWindowProc(hWnd(), uMsg, wParam, lParam);
}