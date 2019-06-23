#include "stdafx.h"
#include "AsignarTeclaRapida.h"

#define ANCHO_BOTON		  60
#define MARGEN            10
#define ID_BOTON_EDITAR 1000

AsignarTeclaRapida::AsignarTeclaRapida(void) : _Editando(FALSE), _TeclaRapida(NULL) {
}

AsignarTeclaRapida::~AsignarTeclaRapida(void) {
}

void AsignarTeclaRapida::Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID, TeclaRapida *nTecla) {
	CrearControlEx(nPadre, L"RAVE_AsignarTeclaRapida", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL);
	RECT RC;
	GetClientRect(_hWnd, &RC);
	_TeclaRapida = nTecla;
	_Texto = nTecla->String();
	_Boton.CrearBotonEx(this, L"Editar", RC.right - ANCHO_BOTON, 0, ANCHO_BOTON, RC.bottom, ID_BOTON_EDITAR);
	_Boton.Fuente.CrearFuente(18, _Boton.Skin.FuenteNombre.c_str(), TRUE);
	_Fuente.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE);
}

void AsignarTeclaRapida::Pintar(HDC DC) {
	RECT RC;
	GetClientRect(_hWnd, &RC);
	// Recta de pintado
	RECT RP = { RC.left, RC.top, RC.right - (ANCHO_BOTON + 10), RC.bottom };
	RECT RM = { RC.right - (ANCHO_BOTON + 10), RC.top, RC.right - ANCHO_BOTON, RC.bottom };
	// Creo un buffer para el pintado
	HDC		Buffer			= CreateCompatibleDC(DC);
	HBITMAP Bmp				= CreateCompatibleBitmap(DC, RC.right, RC.bottom);
	HBITMAP BmpViejo		= static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

	// Pinto el fondo oscuro
	HBRUSH BFondoO = CreateSolidBrush(COLOR_FONDO);
	FillRect(Buffer, &RM, BFondoO);
	DeleteObject(BFondoO);

	// Pinto el fondo claro
	HBRUSH BFondoC = CreateSolidBrush(COLOR_FONDO_CLARO);
	FillRect(Buffer, &RP, BFondoC);
	DeleteObject(BFondoC);

	// Pinto el borde para el fondo claro
	HBRUSH BBorde = CreateSolidBrush(COLOR_BORDE);
	FrameRect(Buffer, &RP, BBorde);
	DeleteObject(BBorde);

	// Pinto el texto que nos dice la tecla asignada
	SetBkMode(Buffer, TRANSPARENT);
	// Selecciono la fuente
	HFONT FuenteVieja = static_cast<HFONT>(SelectObject(Buffer, _Fuente()));

	// Vuelvo a seleccionar la fuente inicial del DC
	SelectObject(Buffer, FuenteVieja);
	if (_Editando == FALSE) {
		SetTextColor(Buffer, COLOR_ARBOL_TEXTO);
		DrawText(Buffer, _Texto.c_str(), static_cast<int>(_Texto.size()), &RP, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else {
		SetTextColor(Buffer, COLOR_ROJO);
		DrawText(Buffer, L"Editando...", -1, &RP, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// Pinto el buffer en el DC (sin pintar la parte del boton)
	BitBlt(DC, RC.left, RC.top, RC.right - ANCHO_BOTON, RC.bottom, Buffer, 0, 0, SRCCOPY);

	// Elimino objetos gdi de la memoria
	SelectObject(Buffer, BmpViejo);
	DeleteObject(Bmp);
	DeleteDC(Buffer);
}

void AsignarTeclaRapida::EditarTecla(void) {
	SetFocus(_hWnd); // REVISADO
	_Editando = TRUE;
	Repintar();
}

void AsignarTeclaRapida::Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado) {
}

void AsignarTeclaRapida::Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado) {
	if (DatosTeclado.TeclaVirtual() == VK_CONTROL || DatosTeclado.TeclaVirtual() == VK_MENU || DatosTeclado.TeclaVirtual() == VK_SHIFT)
		return;

	if (_Editando == TRUE) {
		if (DatosTeclado.TeclaVirtual() == VK_ESCAPE) {
			_Editando = FALSE;
			App.MostrarToolTipOpciones(L"Edición cancelada.");
			Repintar();
			return;
		}

		BOOL Control	= ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
		BOOL Alt		= ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
		BOOL Shift		= ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);

		if (ExisteTecla(DatosTeclado.TeclaVirtual(), Control, Shift, Alt) == FALSE) {
			_TeclaRapida->Asignar(DatosTeclado.TeclaVirtual(), Control, Alt, Shift);
			_Texto = _TeclaRapida->String();
			_Editando = FALSE;
			Repintar();
			App.MostrarToolTipOpciones(L"Nueva tecla rápida asignada.");
			App.BD.GuardarTeclasRapidas();
		}
		else {
			App.MostrarToolTipOpcionesError(L"La tecla ya está asignada");
		}
	}
}

const BOOL AsignarTeclaRapida::ExisteTecla(const int Tecla, const BOOL Control, const BOOL Shift, const BOOL Alt) {
	for (size_t i = 0; i < App.TeclasRapidas.size(); i++) {
		if (App.TeclasRapidas[i].Tecla == Tecla && App.TeclasRapidas[i].Control == Control && App.TeclasRapidas[i].Alt == Alt && App.TeclasRapidas[i].Shift == Shift) {			
			return TRUE;
		}
	}
	return FALSE;
}

void AsignarTeclaRapida::Evento_Tecla(DWL::DEventoTeclado &DatosTeclado) {

}


LRESULT CALLBACK AsignarTeclaRapida::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT:
			HDC         DC;
			PAINTSTRUCT PS;
			DC = BeginPaint(hWnd(), &PS);
			Pintar(DC);
			EndPaint(hWnd(), &PS);
			return 0;
		case DWL_BOTONEX_CLICK:
			EditarTecla();
			return 0;
			// Teclado
		case WM_KEYDOWN:
			Evento_TeclaPresionada(DWL::DEventoTeclado(wParam, lParam, this));
			break;	// Los eventos de teclado tienen que pasar a la clase super base para poder obtener el teclado general
		case WM_KEYUP:
			Evento_TeclaSoltada(DWL::DEventoTeclado(wParam, lParam, this));
			break;	// Los eventos de teclado tienen que pasar a la clase super base para poder obtener el teclado general
		case WM_CHAR:
			Evento_Tecla(DWL::DEventoTeclado(wParam, lParam, this));
			break;	// Los eventos de teclado tienen que pasar a la clase super base para poder obtener el teclado general

	}

	// Sobretodo hay que retornar el DefWindowProc para que no se procesen las teclas en el hook global del teclado 
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}


















std::wstring &TeclaRapida::String(void) {
	static std::wstring Ret;
	Ret.resize(0);
	
	if (Control == TRUE)	Ret  = L"Control + ";
	if (Alt		== TRUE)	Ret += L"Alt + ";
	if (Shift	== TRUE)	Ret += L"Shift + ";
	
	switch (Tecla) {
		case VK_UP		: Ret += L"Cursor adelante";		break;
		case VK_DOWN	: Ret += L"Cursor atrás";			break;
		case VK_LEFT	: Ret += L"Cursor izquierda";		break;
		case VK_RIGHT	: Ret += L"Cursor derecha";			break;
		case VK_SPACE	: Ret += L"Espacio";				break;
		case VK_BACK	: Ret += L"Retroceso";				break;
		case VK_F1		: Ret += L"F1";						break;
		case VK_F2		: Ret += L"F2";						break;		
		case VK_F3		: Ret += L"F3";						break;
		case VK_F4		: Ret += L"F4";						break;
		case VK_F5		: Ret += L"F5";						break;
		case VK_F6		: Ret += L"F6";						break;
		case VK_F7		: Ret += L"F7";						break;
		case VK_F8		: Ret += L"F8";						break;
		case VK_F9		: Ret += L"F9";						break;
		case VK_F10		: Ret += L"F10";					break;
		case VK_F11		: Ret += L"F11";					break;
		case VK_F12		: Ret += L"F12";					break;
		case VK_F13		: Ret += L"F13";					break;
		case VK_F14		: Ret += L"F14";					break;
		case VK_F15		: Ret += L"F15";					break;
		case VK_F16		: Ret += L"F16";					break;
		case VK_F17		: Ret += L"F17";					break;
		case VK_F18		: Ret += L"F18";					break;
		case VK_F19		: Ret += L"F19";					break;
		case VK_F20		: Ret += L"F20";					break;
		case VK_F21		: Ret += L"F21";					break;
		case VK_F22		: Ret += L"F22";					break;
		case VK_F23		: Ret += L"F23";					break;
		case VK_F24		: Ret += L"F24";					break;
		case VK_INSERT	: Ret += L"Insert";					break;
		case VK_DELETE	: Ret += L"Suprimir";				break;
		case VK_HOME	: Ret += L"Inicio";					break;
		case VK_END		: Ret += L"Fin";					break;
		case VK_ESCAPE	: Ret += L"Esc";					break;
//		case VK_SHIFT	: Ret += L"Shift";
		case VK_PRINT	: Ret += L"Imprimir pantalla";		break;
		case VK_PLAY	: Ret += L"Play";					break;
//		case VK_PAUSE	: return TEXT("Play");
//		case VK_STOP	: return TEXT("Play");
//		case VK_PLAY	: return TEXT("Play");
		case VK_ADD		: Ret += L"+";						break;
		case VK_SUBTRACT: Ret += L"-";						break;
		case VK_MULTIPLY: Ret += L"*";						break;
		case VK_DIVIDE	: Ret += L"/";						break;
		case 219		: Ret += L"'";						break;
		case 221		: Ret += L"¡";						break;
		default			: Ret += static_cast<TCHAR>(Tecla);	break;
	}

	return Ret;
}