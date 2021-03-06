#include "stdafx.h"
#include "VentanaAsociarReproductor (DEPRECATED).h"
#include "DMensajesWnd.h"
#include "resource.h"
#include <shellapi.h>

#define ID_BOTONACEPTAR			1000
#define ID_BOTONCANCELAR		1001
#define ID_MARCANOMOSTRARMAS	1002


VentanaAsociarReproductor::VentanaAsociarReproductor(void) : DWL::DVentana() {
}


VentanaAsociarReproductor::~VentanaAsociarReproductor(void) {
}

void VentanaAsociarReproductor::Mostrar(void) {
	// Si la opci�n de mostrar la ventana de asociar archivos no est� habilitada, salgo de la funci�n
	if (App.Opciones.MostrarAsociarArchivos() == FALSE) return;

	// Creo la ventana que mostrar� el progreso
	CrearVentana(NULL, L"RAVE_AsociarReproductor", L"Asociar Reproductor", App.Opciones.DlgDirectorios_PosX(), App.Opciones.DlgDirectorios_PosY(), 510, 155, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
	RECT RC;
	GetClientRect(_hWnd, &RC);

	_BotonAceptar.CrearBotonEx(this, L"Aceptar", 275, 70, 100, 30, ID_BOTONACEPTAR);
	_BotonCancelar.CrearBotonEx(this, L"Cancelar", 385, 70, 100, 30, ID_BOTONCANCELAR);

	_MarcaNoMostrarMas.CrearMarcaEx(this, L"No volver a mostrar esta ventana.", 10, 76, 250, 20, ID_MARCANOMOSTRARMAS, IDI_CHECK2);
	_Fuente.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE);
	_FuenteNegrita.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE, TRUE);
}

void VentanaAsociarReproductor::Pintar(HDC DC) {
	static const wchar_t pTitulo[] = L"RAVE no es el reproductor predeterminado del sistema.\n";
	static const wchar_t pTexto[]  = L"Deseas asignar este reproductor como reproductor predeterminado?";
	RECT RC;
	GetClientRect(_hWnd, &RC);
	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);

	HFONT VFuente = static_cast<HFONT>(SelectObject(DC, _FuenteNegrita()));

	// Pinto el titulo
	SetBkMode(DC, TRANSPARENT);
	RECT RTS = { 11, 11, RC.right - 9, 91 };
	RECT RT  = { 10, 10, RC.right - 10, 90 };
	SetTextColor(DC, COLOR_TEXTO_SOMBRA);
	DrawText(DC, pTitulo, -1, &RTS, DT_LEFT);
	SetTextColor(DC, COLOR_TEXTO);
	DrawText(DC, pTitulo, -1, &RT, DT_LEFT);

	// Pinto el texto
	SelectObject(DC, _Fuente());
	RTS.top += 25; RT.top += 26;
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


void VentanaAsociarReproductor::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	Boton_Click(DatosMouse.ID());
}

void VentanaAsociarReproductor::Boton_Click(const INT_PTR IdBoton) {
	std::wstring PathExe = App.AppPath + L"RAVE.exe";

	if (IdBoton == ID_BOTONACEPTAR) {
		ShellExecute(NULL, TEXT("RunAs"), PathExe.c_str(), TEXT("-AsociarArchivos"), App.AppPath.c_str(), SW_SHOWNORMAL);
	}

	Visible(FALSE);
	_BotonAceptar.Destruir();
	_BotonCancelar.Destruir();
	_MarcaNoMostrarMas.Destruir();
//	SetFocus(App.VentanaRave.hWnd());
	Destruir();

}

LRESULT CALLBACK VentanaAsociarReproductor::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT			:	
			_Evento_Pintar();															
			return 0;
		case WM_ERASEBKGND		:	
			Pintar(reinterpret_cast<HDC>(wParam));										
			return TRUE;
		case DWL_BOTONEX_CLICK  :   
			Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));				
			return 0;
		case DWL_MARCAEX_CLICK  :   
			App.Opciones.MostrarAsociarArchivos(!_MarcaNoMostrarMas.Marcado());
			return 0;
		case WM_KEYUP			:	
			if (wParam == VK_ESCAPE) { 
				Boton_Click(ID_BOTONCANCELAR);
			}	
			break; // Los eventos de teclado tienen que pasar a la clase super base para poder obtener el teclado general
		case WM_CLOSE :
			Boton_Click(ID_BOTONCANCELAR);
			return 0;
		case WM_EXITSIZEMOVE:
//			App.Opciones.GuardarPosVentanaAsociar();
			return 0;

	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}