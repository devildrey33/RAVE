#pragma once


#include "DAnimacion.h"


class ToolTipsInfo;

enum ToolTipInfo_Tipo {
	ToolTipInfo_Tipo_Texto,
	ToolTipInfo_Tipo_Medio
};

// Plantilla para un tooltip básico
class ToolTipInfo : public DVentana {
  public:
								ToolTipInfo(const ToolTipInfo_Tipo nTipo) : _Ocultando(FALSE), _Padre(NULL), _Tipo(nTipo)	{ };
  							   ~ToolTipInfo()																				{ };
	virtual void				Ocultar(const BOOL Rapido = FALSE)															{ };
	virtual LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam)										{ return DVentana::GestorMensajes(uMsg, wParam, lParam); };
//	virtual SIZE				CalcularTam(std::wstring &Str)																{ SIZE R = { 0, 0 }; return R; };
	DWL::DAnimacion			    Ani;
  protected:
	BOOL                       _Ocultando;
	ToolTipInfo_Tipo           _Tipo;
	ToolTipsInfo              *_Padre;
};


// Clase que controla un unico tooltip informativo
class ToolTipInfo_Texto : public ToolTipInfo {
  public:
								ToolTipInfo_Texto(void) : ToolTipInfo(ToolTipInfo_Tipo_Texto) { };
				               ~ToolTipInfo_Texto(void) { };
	SIZE						CalcularTam(std::wstring &Str);
	void						Mostrar(const int cX, const int cY, const int cAncho, const int cAlto, std::wstring &Str, ToolTipsInfo *nPadre, std::function<void(void)> CallbackOcultarTerminado);
	void						Ocultar(const BOOL Rapido = FALSE);
	void						Pintar(HDC DC);
	LRESULT CALLBACK			GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
//	DWL::DAnimacion				Ani;
  protected:
	void		               _Evento_Pintar(void);
	void                       _Evento_Temporizador(INT_PTR tID);
	std::wstring	           _Str;
//	ToolTipsInfo              *_Padre;
//	BOOL                       _Ocultando;
	std::function<void(void)>  _CallbackOcultarTerminado;
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