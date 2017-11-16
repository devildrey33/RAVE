#pragma once

#include "TablaBase.h"


class TablaOpciones : public TablaBase {
  public:
							TablaOpciones();
						   ~TablaOpciones();
	const BOOL              CrearTabla(sqlite3 *BD);
	const UINT			    ObtenerDatos(sqlite3 *BD);

	const BOOL              GuardarDatos(void);
							// NOTA Esta función no guarda la posición en la BD
	inline void             AsignarPosVentana(const int nX, const int nY) { _PosX = nX; _PosY = nY; };
	const BOOL              GuardarPosTamVentana(void);
	inline const int		PosX(void)		{ return _PosX; }
	inline const int		PosY(void)		{ return _PosY;  }
	inline const int		Ancho(void)		{ return _Ancho; }
	inline const int		Alto(void)		{ return _Alto;  }

	inline const int		Volumen(void)	{ return _Volumen; }
	void					Volumen(const int nVolumen);

	inline const int		Shufle(void)	{ return _Shufle; }
	void					Shufle(const int nShufle);

	inline const int		Repeat(void)	{ return _Repeat; }
	void					Repeat(const int nRepeat);

	inline const int		Inicio(void)	{ return _Inicio; }
	void					Inicio(const int nInicio);

	inline const int		OcultarMouseEnVideo(void) { return _OcultarMouseEnVideo; }
	void					OcultarMouseEnVideo(const int nOcultarMouseEnVideo);
protected:
	
	std::wstring           _PathAbrir;
	int                    _Volumen;
	int                    _PosX;
	int                    _PosY;
	int                    _Ancho;
	int                    _Alto;
	int                    _Shufle;
	int                    _Repeat;
	int                    _Inicio;
							// Tiempo en MS que tarda en ocultarse el mouse y los controles de un video
	int                    _OcultarMouseEnVideo;
	
};

