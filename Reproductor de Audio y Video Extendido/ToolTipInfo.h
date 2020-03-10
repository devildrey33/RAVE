#pragma once


#include "DAnimacion.h"
#include "DVentana.h"
#include "EtiquetaBD.h"
#include "RaveBD.h"
#include "DFuente.h"
#include "resource.h"
#include "DTooltipEx.h"

class ToolTipsInfo;

enum ToolTipInfo_Tipo {
	ToolTipInfo_Tipo_SinTipo,
	ToolTipInfo_Tipo_Texto,
	ToolTipInfo_Tipo_2Columnas,
	ToolTipInfo_Tipo_Medio,
	ToolTipInfo_Tipo_Etiqueta
};

#define POS_MOUSE -1
// Depuración de los tooltips
#define DEBUG_TOOLTIPINFO FALSE

// Plantilla para un tooltip básico
class ToolTipInfo : public DWL::DVentana {
  public:
									ToolTipInfo(void) : _Ocultando(FALSE), _Padre(NULL), X(0), Y(0)								{ };
  							       ~ToolTipInfo(void)																			{ };
	void							Mostrar(const int cX, const int cY, const int cAncho, const int cAlto, ToolTipsInfo *nPadre, std::function<void(void)> CallbackOcultarTerminado);
	void							Mover(const int PosX = POS_MOUSE, const int PosY = POS_MOUSE);
	void							Ocultar(const BOOL Rapido = FALSE);
	inline const BOOL               Ocultando(void)																				{ return _Ocultando;  }
	LRESULT CALLBACK				GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
	DWL::DAnimacion					Ani;
									// Virtuales 
	virtual void					Pintar(HDC DC)																				{ };
	virtual SIZE					CalcularTam(void)																			{ SIZE R = { 0, 0 }; return R; };
	virtual inline ToolTipInfo_Tipo	Tipo(void)																					{ return ToolTipInfo_Tipo_SinTipo;	};
	int                             X;
	int                             Y;
	
	DWL::DToolTipEx_Skin            Skin;
  protected:
	void		                   _Evento_Pintar(void);
    void                           _Evento_Temporizador(INT_PTR tID);

	BOOL                           _Ocultando;
	ToolTipsInfo                  *_Padre;
	std::function<void(void)>      _CallbackOcultarTerminado;
	
};


// Clase que controla un unico tooltip informativo de texto
class ToolTipInfo_Texto : public ToolTipInfo {
  public:
								ToolTipInfo_Texto(std::wstring &nTexto) : ToolTipInfo(), _Str(nTexto) { _Fuente.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE); };
				               ~ToolTipInfo_Texto(void) { };
	SIZE						CalcularTam(void);
	void						Pintar(HDC DC);
	inline ToolTipInfo_Tipo		Tipo(void) { return ToolTipInfo_Tipo_Texto; };
  protected:
	virtual void               _PintarFondo(HDC Buffer, RECT *RC);
	DWL::DFuente               _Fuente;
	std::wstring	           _Str;
};

// Clase que controla un unico tooltip informativo de texto con un fondo rojo que simboliza un error
class ToolTipInfo_TextoError : public ToolTipInfo_Texto {
  public:
			ToolTipInfo_TextoError(std::wstring &nTexto) : ToolTipInfo_Texto(nTexto) { };
	       ~ToolTipInfo_TextoError(void) { }
  protected:
	  void _PintarFondo(HDC Buffer, RECT *RC);
};

// Información de una celda
class ToolTipInfo_Celda {
	public :
					ToolTipInfo_Celda(void) : Ancho(0) { };
					ToolTipInfo_Celda(std::wstring &nTexto, const int nAncho) : Texto(nTexto), Ancho(nAncho)	{ };
					ToolTipInfo_Celda(const wchar_t *nTexto, const int nAncho) : Texto(nTexto), Ancho(nAncho)	{ };
				   ~ToolTipInfo_Celda(void) { };
	std::wstring	Texto;
	int				Ancho;
};

// Clase base para crear Tooltips de 2 columnas (para no repetir la función de pintado en ToolTipInfo_Medio y ToolTipInfo_Etiqueta)
class ToolTipInfo_2Columnas : public ToolTipInfo {
  public:
									ToolTipInfo_2Columnas(void) : ToolTipInfo(), _AnchoCol1(0) {
										_FuenteTitulo.CrearFuente(FUENTE_GRANDE, FUENTE_NOMBRE, TRUE); 
										_FuenteNombreValor.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE, TRUE); 
										_FuenteValor.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE, FALSE);	
										_IconoNota.CrearIconoRecursos(IDI_NOTA, 16, 16);
									};
									ToolTipInfo_2Columnas(std::wstring &Titulo) : ToolTipInfo(), _Titulo(Titulo), _AnchoCol1(0) { 
										_FuenteTitulo.CrearFuente(FUENTE_GRANDE, FUENTE_NOMBRE, TRUE);
										_FuenteNombreValor.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE, TRUE);
										_FuenteValor.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE, FALSE);
										_IconoNota.CrearIconoRecursos(IDI_NOTA, 16, 16);
									};
	                               ~ToolTipInfo_2Columnas(void) { };
	void							Pintar(HDC DC);
	virtual inline ToolTipInfo_Tipo	Tipo(void) { return ToolTipInfo_Tipo_2Columnas; };
	virtual void                    PintarNota(HDC DC, const int cX, const int cY) { };
  protected:
	DWL::DFuente                   _FuenteTitulo;
	DWL::DFuente                   _FuenteNombreValor;
	DWL::DFuente                   _FuenteValor;
	void                           _PintarNota(HDC DC, const int cX, const int cY, const float nNota);
	std::wstring                   _Titulo;
	std::vector<ToolTipInfo_Celda> _Col1;
	std::vector<ToolTipInfo_Celda> _Col2;
	DWL::DIcono                    _Icono;
	DWL::DIcono                    _IconoNota;
	int                            _AnchoCol1;
};

// Clase que controla un unico tooltip informativo de un medio
class ToolTipInfo_Medio : public ToolTipInfo_2Columnas {
  public:
									ToolTipInfo_Medio(void) : ToolTipInfo_2Columnas()                                           { };
									ToolTipInfo_Medio(BDMedio &nMedio) : ToolTipInfo_2Columnas(nMedio.Nombre()), Medio(nMedio)  { };
	                               ~ToolTipInfo_Medio(void)																		{ };
	inline ToolTipInfo_Tipo			Tipo(void)																					{ return ToolTipInfo_Tipo_Medio; };
	void                            MostrarFijo(BDMedio &nMedio);
	inline void						PintarNota(HDC DC, const int cX, const int cY)												{ _PintarNota(DC, cX, cY, Medio.Nota); }
	SIZE							CalcularTam(void);
	BDMedio	                        Medio;
};

// Clase que controla un unico tooltip informativo de una etiqueta
class ToolTipInfo_Etiqueta : public ToolTipInfo_2Columnas {
  public:
									ToolTipInfo_Etiqueta(void) : ToolTipInfo_2Columnas(), Etiqueta(NULL)										{ };
									ToolTipInfo_Etiqueta(EtiquetaBD *nEtiqueta) : ToolTipInfo_2Columnas(nEtiqueta->Texto), Etiqueta(nEtiqueta)	{ };
	                               ~ToolTipInfo_Etiqueta(void)																					{ };
	inline ToolTipInfo_Tipo			Tipo(void)																									{ return ToolTipInfo_Tipo_Etiqueta; };
	void                            MostrarFijo(EtiquetaBD *nEtiqueta);
	inline void						PintarNota(HDC DC, const int cX, const int cY)																{ _PintarNota(DC, cX, cY, Etiqueta->Nota); }
	SIZE							CalcularTam(void);
	EtiquetaBD	                   *Etiqueta;
};

// Clase que controla un grupo de tooltips que se mostrarán en una ventana
class ToolTipsInfo {
  public :
									ToolTipsInfo(void) : _Padre(NULL) { };
	                               ~ToolTipsInfo(void) { };
	void                            Iniciar(DWL::DhWnd *Padre);
	void                            MostrarToolTip(const wchar_t *Texto);
	void                            MostrarToolTip(std::wstring &Texto);
	void                            MostrarToolTipError(const wchar_t *Texto);
	void                            MostrarToolTipError(std::wstring &Texto);
	void                            MostrarToolTip(BDMedio &Medio);
	void                            MostrarToolTip(EtiquetaBD *Etiqueta);
//	void                            EliminarToolTip(ToolTipInfo *ToolTip);
	HWND                            Padre(void);
	inline DWL::DhWnd              *DPadre(void) { return _Padre; };
	void                            Ocultar(void);
	void                            OcultarRapido(void);
	void                            RecolocarToolTips(void);
  protected:
	void                           _MostrarToolTip(ToolTipInfo *TT);
	DWL::DhWnd                    *_Padre; // Ventana que se usara como punto de partida para mostrar los tooltips
	std::vector <ToolTipInfo *>    _ToolTips;
};