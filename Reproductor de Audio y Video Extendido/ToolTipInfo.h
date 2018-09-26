#pragma once

class ToolTipsInfo;

// Clase que controla un unico tooltip informativo
class ToolTipInfo : public DVentana {
  public:
						ToolTipInfo() { };
				       ~ToolTipInfo() { };
	SIZE                CalcularTam(std::wstring &Str);
	void				Mostrar(const int cX, const int cY, const int cAncho, const int cAlto, std::wstring &Str, ToolTipsInfo *nPadre);
	void				Ocultar(const BOOL Rapido = FALSE);
	void				Pintar(HDC DC);
//	const BOOL          Destruir(void);
	LRESULT CALLBACK	GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
  protected:
	void		       _Evento_Pintar(void);
	std::wstring	   _Str;
//	DhWnd_Fuente       _Fuente;
	ToolTipsInfo      *_Padre;
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
	void                            Ocultar(void);
  protected:
	DhWnd                         *_Padre; // Ventana que se usara como punto de partida para mostrar los tooltips
	std::vector <ToolTipInfo *>    _ToolTips;
};