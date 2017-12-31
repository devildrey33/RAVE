#ifndef DLISTAEX_COLUMNA_H
#define DLISTAEX_COLUMNA_H

namespace DWL {

	class DListaEx;

	enum DListaEx_Columna_Alineacion {
		DListaEx_Columna_Alineacion_Derecha		= DT_RIGHT,
		DListaEx_Columna_Alineacion_Centrado	= DT_CENTER,
		DListaEx_Columna_Alineacion_Izquierda	= DT_LEFT
	};

	#define DLISTAEX_COLUMNA_ANCHO_AUTO -1	// Ancho de columna automático

	class DListaEx_Columna 	{
	  public:
									    DListaEx_Columna(void) : Alineacion(DListaEx_Columna_Alineacion_Derecha), Ancho(0) { };
									    DListaEx_Columna(const DListaEx_Columna_Alineacion nAlineacion , const LONG nAncho) : Alineacion(nAlineacion), Ancho(nAncho) { };
									   ~DListaEx_Columna(void) { };
		DListaEx_Columna_Alineacion		Alineacion;
		LONG                            Ancho;
	  protected:
		friend class DListaEx;
	};

}
#endif