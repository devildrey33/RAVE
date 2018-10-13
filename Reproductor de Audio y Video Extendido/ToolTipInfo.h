#pragma once


#include "DAnimacion.h"


class ToolTipsInfo;

// Clase que controla un unico tooltip informativo
class ToolTipInfo : public DVentana {
  public:
						ToolTipInfo() : _Ocultando(FALSE) { };
				       ~ToolTipInfo() { };
	SIZE                CalcularTam(std::wstring &Str);
	void				Mostrar(const int cX, const int cY, const int cAncho, const int cAlto, std::wstring &Str, ToolTipsInfo *nPadre, std::function<void(void)> CallbackOcultarTerminado);
	void				Ocultar(const BOOL Rapido = FALSE);
	void				Pintar(HDC DC);
	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	DWL::DAnimacion     Ani;
protected:
	void		       _Evento_Pintar(void);
	void               _Evento_Temporizador(INT_PTR tID);
	std::wstring	   _Str;
	ToolTipsInfo      *_Padre;
	BOOL               _Ocultando;
	std::function<void(void)> _CallbackOcultarTerminado;
};


// Clase que controla un grupo de tooltips que se mostrarán en una ventana
class ToolTipsInfo {
  public :
									ToolTipsInfo() : _Padre(NULL) { };
	                               ~ToolTipsInfo() { };
	void                            Iniciar(DhWnd *Padre);
	void                            MostrarToolTip(const wchar_t *Texto);
	void                            MostrarToolTip(std::wstring &Texto);
//	void                            EliminarToolTip(ToolTipInfo *ToolTip);
	HWND                            Padre(void);
	inline DhWnd                   *DPadre(void) { return _Padre; };
	void                            Ocultar(void);
	void                            RecolocarToolTips(void);
  protected:
	DhWnd                         *_Padre; // Ventana que se usara como punto de partida para mostrar los tooltips
	std::vector <ToolTipInfo *>    _ToolTips;
};