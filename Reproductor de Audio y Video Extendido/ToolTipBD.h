#pragma once

#include "EtiquetaBD.h"
#include "DListaIconos.h"
#include "DVentana.h"

#define POS_MOUSE -1

class ToolTipBD_Celda {
  public :
					ToolTipBD_Celda(void) : Ancho(0) { };
					ToolTipBD_Celda(std::wstring &nTexto, const int nAncho) : Texto(nTexto), Ancho(nAncho) { };
					ToolTipBD_Celda(const wchar_t *nTexto, const int nAncho) : Texto(nTexto), Ancho(nAncho) { };
				   ~ToolTipBD_Celda(void) { };
	std::wstring	Texto;
	int				Ancho;
};

//class DListaIconos_Icono;

class ToolTipBD : public DWL::DVentana {
  public:
									ToolTipBD(void);
					               ~ToolTipBD(void);
								  
	void							Mostrar(BDMedio &nMedio, const int PosX = POS_MOUSE, const int PosY = POS_MOUSE);
	void							Mostrar(EtiquetaBD &nEtiqueta, const int PosX = POS_MOUSE, const int PosY = POS_MOUSE);
	void                            Mover(const int PosX = POS_MOUSE, const int PosY = POS_MOUSE);
	void							Pintar(HDC DC);

	LRESULT CALLBACK				GestorMensajes(UINT uMSg, WPARAM wParam, LPARAM lParam);

	inline std::wstring			   &Titulo(void) { return _Titulo; }
  protected:
	void                           _Mostrar(POINT &Tam, const int PosX = POS_MOUSE, const int PosY = POS_MOUSE);
	void			               _Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam);
	void			               _Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam);
	void		          	       _Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam);
	const POINT                    _CalcularEtiqueta(EtiquetaBD &nEtiqueta);
	const POINT                    _CalcularMedio(BDMedio &nMedio);
	void                           _Evento_Pintar(void);
	
	std::wstring                   _Titulo;
	int                            _AnchoCol1;

	DWL::DListaIconos_Icono       *_Icono;

	std::vector<ToolTipBD_Celda>   _Col1;
	std::vector<ToolTipBD_Celda>   _Col2;
	POINT                          _Tam;
};

