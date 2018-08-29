#include "stdafx.h"
#include "VentanaAsociarReproductor.h"
#include "DMensajesWnd.h"
#include "resource.h"
#include <shellapi.h>

#define ID_BOTONACEPTAR			1000
#define ID_BOTONCANCELAR		1001
#define ID_MARCANOMOSTRARMAS	1002


VentanaAsociarReproductor::VentanaAsociarReproductor(void) {
}


VentanaAsociarReproductor::~VentanaAsociarReproductor(void) {
}

void VentanaAsociarReproductor::Mostrar(void) {
	// Si la opción de mostrar la ventana de asociar archivos no está habilitada, salgo de la función
	if (App.BD.Opciones_MostrarAsociarArchivos() == FALSE) return;

	// Creo la ventana que mostrará el progreso
	CrearVentana(NULL, L"RAVE_AsociarReproductor", L"Asociar Reproductor", 300, 200, 510, 155, WS_CAPTION | WS_SYSMENU | WS_VISIBLE);
	RECT RC;
	GetClientRect(_hWnd, &RC);

	_BotonAceptar.CrearBotonEx(this, L"Aceptar", 275, 70, 100, 30, ID_BOTONACEPTAR);
	_BotonCancelar.CrearBotonEx(this, L"Cancelar", 385, 70, 100, 30, ID_BOTONCANCELAR);

	_MarcaNoMostrarMas.CrearMarcaEx(this, L"No volver a mostrar esta ventana.", 10, 76, 250, 20, ID_MARCANOMOSTRARMAS, IDI_CHECK2);
}

void VentanaAsociarReproductor::Pintar(HDC DC) {
	static const wchar_t pTexto[] = L"RAVE no es el reproductor predeterminado del sistema.\n"
								    L"Deseas asignar este reproductor como reproductor predeterminado?";
	RECT RC;
	GetClientRect(_hWnd, &RC);
	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);

	HFONT VFuente = static_cast<HFONT>(SelectObject(DC, DhWnd::_Fuente18Normal()));

	SetBkMode(DC, TRANSPARENT);
	RECT RTS = { 11, 11, RC.right - 9, 91 };
	RECT RT  = { 10, 10, RC.right - 10, 90 };
	SetTextColor(DC, COLOR_TEXTO_SOMBRA);
	DrawText(DC, pTexto, -1, &RTS, DT_LEFT);
	SetTextColor(DC, COLOR_TEXTO);
	DrawText(DC, pTexto, -1, &RT, DT_LEFT);

	SelectObject(DC, VFuente);
	DeleteObject(BrochaFondo);
}

void VentanaAsociarReproductor::_Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(_hWnd, &PS);
	Pintar(DC);
	EndPaint(_hWnd, &PS);
}


void VentanaAsociarReproductor::Evento_BotonEx_Mouse_Click(const UINT cID) {
	std::wstring PathExe = App.AppPath + L"RAVE.exe";

	if (cID == ID_BOTONACEPTAR) {
		ShellExecute(NULL, TEXT("RunAs"), PathExe.c_str(), TEXT("-AsociarArchivos"), App.AppPath.c_str(), SW_SHOWNORMAL);
	}
	
	Visible(FALSE);
	_BotonAceptar.Destruir();
	_BotonCancelar.Destruir();
	_MarcaNoMostrarMas.Destruir();
	Destruir();
}

LRESULT CALLBACK VentanaAsociarReproductor::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT			:	_Evento_Pintar();															return 0;
		case DWL_BOTONEX_CLICK  :   Evento_BotonEx_Mouse_Click(static_cast<UINT>(wParam));						return 0;
		case DWL_MARCAEX_CLICK  :   App.BD.Opciones_MostrarAsociarArchivos(!_MarcaNoMostrarMas.Marcado());		return 0;
		case WM_KEYUP			:	if (wParam == VK_ESCAPE) { Evento_BotonEx_Mouse_Click(ID_BOTONCANCELAR); }	return 0;
	}
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}