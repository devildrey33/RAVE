#pragma once

#include "DBotonEx.h"
#include "DFuente.h"

class TeclaRapida {
  public:
					TeclaRapida(void) : Tecla(0), Control(FALSE), Alt(FALSE), Shift(FALSE) { };
	    			TeclaRapida(const int nTecla, const BOOL nControl, const BOOL nAlt, const BOOL nShift) : Tecla(nTecla), Control(nControl), Alt(nAlt), Shift(nShift) { };
	               ~TeclaRapida(void) { };

	std::wstring   &String(void);
	inline void     Asignar(const int nTecla, const BOOL nControl, const BOOL nAlt, const BOOL nShift) { Tecla = nTecla; Control = nControl; Alt = nAlt; Shift = nShift; };

	int				Tecla;
	BOOL			Control;
	BOOL			Alt;
	BOOL			Shift;
};


class AsignarTeclaRapida : public DWL::DControlEx {
  public:
						AsignarTeclaRapida(void);
			           ~AsignarTeclaRapida(void);

	void				Crear(DWL::DhWnd *nPadre, const int cX, const int cY, const int cAncho, const int cAlto, const INT_PTR cID, TeclaRapida *nTecla);

	void                EditarTecla(void);
	const BOOL          ExisteTecla(const int Tecla, const BOOL Control, const BOOL Shift, const BOOL Alt);

	void				Pintar(HDC DC);
	
	
	void                Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado);
	void                Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado);
	void				Evento_Tecla(DWL::DEventoTeclado &DatosTeclado);

	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
  protected:
	DFuente            _Fuente;
	BOOL               _Editando;
	DWL::DBotonEx	   _Boton;
	std::wstring       _Texto;
	TeclaRapida       *_TeclaRapida;
};

