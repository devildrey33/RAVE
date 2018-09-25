#pragma once

#include "RaveBD.h"
#include <mutex>
#include "DhWnd.h"
#include "DBarraProgresoEx.h"
#include "DBotonEx.h"
#include "DMarcaEx.h"

// Coincidencia de texto en uno o mas medios 
class CoincidenciasTexto {
  public :
							CoincidenciasTexto(void) { }
							CoincidenciasTexto(BDMedio *Medio, std::wstring &nTexto) { Medios.push_back(Medio); Texto = nTexto; }
						   ~CoincidenciasTexto(void) { }
	std::wstring			Texto;
	std::vector<BDMedio *>	Medios;
};

// Anomalia de texto (2 o mas textos parecidos que deberian ser el mismo)
class AnomaliasTexto {
  public :	
											AnomaliasTexto(void) { };
											AnomaliasTexto(CoincidenciasTexto *NuevaAnomalia1, CoincidenciasTexto *NuevaAnomalia2) { Anomalias.push_back(NuevaAnomalia1); Anomalias.push_back(NuevaAnomalia2); };
									       ~AnomaliasTexto(void) { };
	  const BOOL							MirarAnomalia(CoincidenciasTexto *NuevaAnomalia1, CoincidenciasTexto *NuevaAnomalia2);

	  std::wstring                         &TextoFinal(size_t &OUT_Pos);
	  std::vector<CoincidenciasTexto *>		Anomalias;
};


class ThreadAnalisis :	public DWL::DVentana {
  public:
										ThreadAnalisis(void);
	                                   ~ThreadAnalisis(void);
    const BOOL	           				Iniciar(HWND nhWndDest);
	void								Terminar(void);
	void								Cancelar(const BOOL nCancelar);
	const BOOL							Cancelar(void);

	void								Pintar(HDC DC);
	LRESULT CALLBACK					GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void								Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse);
	HANDLE                              Thread(void);
  protected:
	void					           _Evento_Pintar(void);
//	void                               _PintarTexto(HDC DC, const wchar_t *pTexto, const int PosX, const int PosY, COLORREF ColorTexto = COLOR_TEXTO, COLORREF ColorSombra = COLOR_TEXTO_SOMBRA);
	static unsigned long               _ThreadAnalisis(void *pThis);

	void                               _Parsear(libvlc_instance_t *VLC, std::wstring &Path);
	std::wstring                       _ObtenerMeta(libvlc_media_t *Media, libvlc_meta_t Tipo);
	static void                        _ParsearTerminado(const libvlc_event_t *event, void *user_data);

	std::mutex                         _Mutex;
	BOOL			                   _Cancelar;
	HWND				               _VentanaPlayer;
	HANDLE				               _Thread;
	RaveBD                             _BD;
	std::vector<std::wstring>          _PorParsear;


	DWL::DBarraProgresoEx			   _BarraProgreso1;
	DWL::DBarraProgresoEx			   _BarraProgreso2;
	DWL::DBarraProgresoEx			   _BarraProgreso3;
	DWL::DBotonEx					   _BotonOcultar;
	DWL::DBotonEx					   _BotonCancelar;
	DWL::DMarcaEx					   _MarcaNoMostrarMas;

	const int                          _CompararCoincidencias(CoincidenciasTexto *Coincidencia1, CoincidenciasTexto *Coincidencia2, const int TipoCoincidencia);
	void                               _AgregarMedio(std::vector<CoincidenciasTexto *> &ListaMedios, BDMedio *Medio, std::wstring &Texto);
	void                               _LimpiarAnomalias(void);
	void                               _RevisarMedios(void);
	void                               _CrearListaAnomalias(std::vector<CoincidenciasTexto *> &Coincidencias, std::vector<AnomaliasTexto *> &Anomalias, const UINT TipoCoincidencia);
	void                               _AgregarEtiqueta(std::wstring &nTexto, const DWORD nTipo, const float nNota, const libvlc_time_t nTiempo, const ULONGLONG uLongitud);
	std::vector<BDMedio>			   _Medios;
	std::vector<CoincidenciasTexto *>  _Generos;
//	std::vector<CoincidenciasTexto *>  _GruposPath;
	std::vector<CoincidenciasTexto *>  _GruposTag;
//	std::vector<CoincidenciasTexto *>  _DiscosPath;
	std::vector<CoincidenciasTexto *>  _DiscosTag;
//	std::vector<CoincidenciasTexto *>  _NombrePath;
//	std::vector<CoincidenciasTexto *>  _NombreTag;

	std::vector<AnomaliasTexto *>      _AnomaliasGenero;
//	std::vector<AnomaliasTexto *>      _AnomaliasGrupoPath;
	std::vector<AnomaliasTexto *>      _AnomaliasGrupoTag;
//	std::vector<AnomaliasTexto *>      _AnomaliasDiscoPath;
	std::vector<AnomaliasTexto *>      _AnomaliasDiscoTag;
	
	std::vector<EtiquetaBD>			   _Etiquetas;

										// OJO esta variable solo se puede utilizar desde el thread principal, para ello si hay que actualizarla se utilizara un sendmessage desde el ThreadAnalisis al principal
	int                                _FASE; // Fase actual del analisis (para el pintado)
};

