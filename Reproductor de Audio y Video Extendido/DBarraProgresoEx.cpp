#include "stdafx.h"
#include "DBarraProgresoEx.h"
#include "Rave_Skin.h"
#include "DMensajesWnd.h"
#include "DStringUtils.h"

namespace DWL {

	DBarraProgresoEx::DBarraProgresoEx(void) : _Minimo(0), _Maximo(0), _Valor(0), _Estado(DBarraEx_Estado_Normal), _ColorBarra(COLOR_BARRA), _ColorFondo(COLOR_BARRA_FONDO), _ColorBorde(COLOR_BORDE), _MostrarValor(DBarraEx_MostrarValor_Nada), _Activado(TRUE) {
	}


	DBarraProgresoEx::~DBarraProgresoEx(void) {
	}


	HWND DBarraProgresoEx::CrearBarraProgresoEx(DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID, const float nMinimo, const float nMaximo, const float nValor) {
//		if (hWnd()) { Debug_Escribir(L"DBarraProgresoEx::CrearBarraProgresoEx() Error : ya se ha creado la barra\n"); return hWnd(); }
		_hWnd = CrearControlEx(nPadre, L"DBarraProgresoEx", L"", cID, cX, cY, cAncho, cAlto, WS_CHILD | WS_VISIBLE, NULL, CS_HREDRAW | CS_VREDRAW);
//		ColorFondo = COLOR_BARRA_FONDO;
//		ColorBarra = COLOR_BARRA;
		_Minimo = nMinimo;
		_Maximo = nMaximo;
		_Valor = nValor;
		if (_Activado == TRUE) {
			_ColorBarra = COLOR_BARRA;
			_ColorFondo = COLOR_BARRA_FONDO;
			_ColorBorde = COLOR_BORDE;
		}
		else {
			_ColorBarra = COLOR_BARRA_DESACTIVADO;
			_ColorFondo = COLOR_BARRA_FONDO_DESACTIVADO;
			_ColorBorde = COLOR_BORDE;
			EnableWindow(_hWnd, FALSE);
		}
		return hWnd();
	}

	void DBarraProgresoEx::Minimo(const float nMinimo) {
		_Minimo = nMinimo;
		Repintar();
	}

	void DBarraProgresoEx::Maximo(const float nMaximo) {
		_Maximo = nMaximo;
		Repintar();
	}

	void DBarraProgresoEx::Valor(const float nValor) {
		_Valor = nValor;
		Repintar();
	}

	void DBarraProgresoEx::PintarBarraEx(HDC DC, const int nX, const int nY) {
		RECT    RC, RBarra, RFondo;
		GetClientRect(hWnd(), &RC);
		// Creo un buffer en memória para pintar el control
		HDC		Buffer = CreateCompatibleDC(NULL);
		// Creo un DC compatible para el buffer
		HBITMAP Bmp = CreateCompatibleBitmap(DC, RC.right, RC.bottom);
		HBITMAP BmpViejo = static_cast<HBITMAP>(SelectObject(Buffer, Bmp));

		// Calculo el tamaño de la barra para el valor actual
		RBarra = RC; RFondo = RC;
		RBarra.left++; RBarra.top++; RBarra.bottom--;  // Evito los bordes
		RFondo.top++; RFondo.bottom--; RFondo.right--; // Evito los bordes
		float Parte = static_cast<float>((RC.right - RC.left) - 2) / (_Maximo - _Minimo);
		RBarra.right = 1 + static_cast<int>(Parte * (_Valor - _Minimo));
		// Aseguro el valor máximo (a veces con los decimales redondeados no se ve la barra llena)
		if (_Valor >= _Maximo) RBarra.right = RC.right - 1;
		RFondo.left = RBarra.right;

		// Pinto el borde
		Evento_PintarBorde(Buffer, RC);

		// Pinto la barra
		Evento_PintarBarra(Buffer, RBarra);

		// Pinto el fondo
		Evento_PintarFondo(Buffer, RFondo);

		// Pinto el valor (si es necesario)
		Evento_PintarValor(Buffer, RC);

		// Copio el buffer al DC
		BitBlt(DC, nX + RC.left, nY + RC.top, RC.right, RC.bottom, Buffer, 0, 0, SRCCOPY);

		// Elimino el buffer de la memória
		SelectObject(Buffer, BmpViejo);
		DeleteObject(Bmp);
		DeleteDC(Buffer);
	}


	void DBarraProgresoEx::Evento_PintarBarra(HDC DC, RECT &RBarra) {
		HBRUSH BrochaBarra = CreateSolidBrush(_ColorBarra);
		FillRect(DC, &RBarra, BrochaBarra);
		DeleteObject(BrochaBarra);
	}

	void DBarraProgresoEx::Evento_PintarFondo(HDC DC, RECT &RFondo) {
		HBRUSH BrochaFondo = CreateSolidBrush(_ColorFondo);
		FillRect(DC, &RFondo, BrochaFondo);
		DeleteObject(BrochaFondo);
	}

	void DBarraProgresoEx::Evento_PintarBorde(HDC DC, RECT &RBorde) {
		HBRUSH BrochaBorde = CreateSolidBrush(_ColorBorde);
		FrameRect(DC, &RBorde, BrochaBorde);
		DeleteObject(BrochaBorde);
	}

	void DBarraProgresoEx::Evento_PintarValor(HDC DC, RECT &RC) {
		if (_MostrarValor != DBarraEx_MostrarValor_Nada) {
			HFONT VFuente = static_cast<HFONT>(SelectObject(DC, Fuente16Normal()));
			std::wstring TxtValor;
			switch (_MostrarValor) {
				case DBarraEx_MostrarValor_Valor2Decimales :			TxtValor = DWL::Strings::ToStrF(_Valor, 2);													break;
				case DBarraEx_MostrarValor_ValorInt:					TxtValor = DWL::Strings::ToStrF(_Valor, 0);													break;
				case DBarraEx_MostrarValor_ValorMaximo2Decimales:		TxtValor = DWL::Strings::ToStrF(_Valor, 2) + L" / " + DWL::Strings::ToStrF(_Maximo, 2);		break;
				case DBarraEx_MostrarValor_ValorMaximoInt:				TxtValor = DWL::Strings::ToStrF(_Valor, 0) + L" / " + DWL::Strings::ToStrF(_Maximo, 0);		break;
			}
			Evento_FormatearValor(TxtValor);
			SetTextColor(DC, COLOR_BARRA_TEXTO);
			SetBkMode(DC, TRANSPARENT);
			DrawText(DC, TxtValor.c_str(), static_cast<int>(TxtValor.size()), &RC, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			SelectObject(DC, VFuente);
		}
	}

	const float DBarraProgresoEx::_ValorMouse(RECT &RC, const int cX) {
		float Parte = (_Maximo - _Minimo) / static_cast<float>(((RC.right - RC.left) - 2));
		float nValor = _Minimo + (static_cast<float>(cX - RC.left) * Parte);
		if (nValor > _Maximo) { nValor = _Maximo; }
		if (nValor < _Minimo) { nValor = _Minimo; }
		return nValor;
	}

	void DBarraProgresoEx::_Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam) {
		if (_MouseEntrando() == TRUE) {
			// Mouse enter
			if (_Estado != DBarraEx_Estado_Presionado) {
				_Estado = DBarraEx_Estado_Resaltado;
				//Repintar();
//				Resaltar(TRUE);
				Transicion(DBarraEx_Transicion_Resaltado);
			}
		}
	}
	void DBarraProgresoEx::_Evento_MouseSaliendo(void) {
		_MouseDentro = FALSE;
		if (_Estado != DBarraEx_Estado_Presionado) {
			_Estado = DBarraEx_Estado_Normal;
			//Repintar();
			//Resaltar(FALSE);
			Transicion(DBarraEx_Transicion_Normal);
		}
	}

	void DBarraProgresoEx::Activado(const BOOL nActivar) {
		BOOL Ret = FALSE;
		_Activado = nActivar;
		Ret = EnableWindow(_hWnd, nActivar);
//		if (_hWnd != NULL) {
			Transicion((nActivar == TRUE) ? DBarraEx_Transicion_Normal : DBarraEx_Transicion_Desactivado);
/*		}
		else {
			if (_Activado == TRUE) {
				_ColorBarra = COLOR_BARRA;
				_ColorFondo = COLOR_BARRA_FONDO;
				_ColorBorde = COLOR_BORDE;
			}
			else {
				_ColorBarra = COLOR_BARRA_DESACTIVADO;
				_ColorFondo = COLOR_BARRA_FONDO_DESACTIVADO;
				_ColorBorde = COLOR_BORDE;
			}
		}*/
		//Repintar();
	}


	void DBarraProgresoEx::Transicion(const DBarraEx_Transicion nTransicion) {
		DWORD Duracion = DhWnd::TiempoAnimaciones;
		if (_AniTransicion.Animando() == TRUE) {
			Duracion = _AniTransicion.TiempoActual();
			_AniTransicion.Terminar();
		}

		COLORREF FondoHasta, BordeHasta, BarraHasta;
		switch (nTransicion) {
			case DBarraEx_Transicion_Normal:
				FondoHasta = COLOR_BARRA_FONDO;
				BarraHasta = COLOR_BARRA;
				BordeHasta = COLOR_BORDE;
				break;
			case DBarraEx_Transicion_Resaltado:
				FondoHasta = COLOR_BARRA_FONDO_RESALTADO;
				BarraHasta = COLOR_BARRA_RESALTADO;
				BordeHasta = COLOR_BORDE_RESALTADO;
				break;
			case DBarraEx_Transicion_Presionado:
				FondoHasta = COLOR_BARRA_FONDO_PRESIONADO;
				BarraHasta = COLOR_BARRA_PRESIONADO;
				BordeHasta = COLOR_BORDE_PRESIONADO;
				break;
			case DBarraEx_Transicion_Desactivado:
				FondoHasta = COLOR_BARRA_FONDO_DESACTIVADO;
				BarraHasta = COLOR_BARRA_DESACTIVADO;
				BordeHasta = COLOR_BORDE;
				break;
		}

		_AniTransicion.Iniciar(_ColorFondo, FondoHasta, _ColorBorde, BordeHasta, _ColorBarra, BarraHasta, Duracion, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
			_ColorFondo = Valores[0];
			_ColorBorde = Valores[1];
			_ColorBarra = Valores[2];
			Repintar();
		});

	}
	/*
	void DBarraProgresoEx::Resaltar(const BOOL Resaltado) {
		if (_AniTransicion.Animando() == TRUE) {
			_AniTransicion.Invertir();
			return;
		}

		COLORREF BordeDesde, BordeHasta, FondoDesde, FondoHasta, BarraDesde, BarraHasta;
		if (Resaltado == TRUE) {
			BordeDesde = COLOR_BORDE;
			BordeHasta = COLOR_BORDE_RESALTADO;
			FondoDesde = COLOR_BARRA_FONDO;
			FondoHasta = COLOR_BARRA_FONDO_RESALTADO;
			BarraDesde = COLOR_BARRA;
			BarraHasta = COLOR_BARRA_RESALTADO;
			_Estado = DBarraEx_Estado_Resaltado;
		}
		else {
			BordeDesde = COLOR_BORDE_RESALTADO;
			BordeHasta = COLOR_BORDE;
			FondoDesde = COLOR_BARRA_FONDO_RESALTADO;
			FondoHasta = COLOR_BARRA_FONDO;
			BarraDesde = COLOR_BARRA_RESALTADO;
			BarraHasta = COLOR_BARRA;
			_Estado = DBarraEx_Estado_Normal;
		}
		_AniTransicion.Iniciar(FondoDesde, FondoHasta, BordeDesde, BordeHasta, BarraDesde, BarraHasta, 400, [=](std::vector<COLORREF> &Valores, const BOOL Terminado) {
			_ColorFondo = Valores[0];
			_ColorBorde = Valores[1];
			_ColorBarra = Valores[2];
			Repintar();
		});

	}*/

	LRESULT CALLBACK DBarraProgresoEx::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_ERASEBKGND:
				PintarBarraEx(reinterpret_cast<HDC>(wParam));
				return TRUE;
			case WM_PAINT:
				HDC         DC;
				PAINTSTRUCT PS;
				DC = BeginPaint(hWnd(), &PS);
				PintarBarraEx(DC);
				EndPaint(hWnd(), &PS);
				return 0;
			case WM_MOUSEMOVE:
				_Evento_MouseMovimiento(wParam, lParam);
				return 0;
			case WM_MOUSELEAVE:
				_Evento_MouseSaliendo();
				return 0;

		}
		return DControlEx::GestorMensajes(uMsg, wParam, lParam);
	}
}