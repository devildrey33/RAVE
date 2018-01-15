#ifndef DLISTAEX_COLUMNA_H
#define DLISTAEX_COLUMNA_H

namespace DWL {

	class DListaEx;
	// Alineaci�n para el texto que contiene esta columna
	enum DListaEx_Columna_Alineacion {
		DListaEx_Columna_Alineacion_Derecha		= DT_RIGHT,
		DListaEx_Columna_Alineacion_Centrado	= DT_CENTER,
		DListaEx_Columna_Alineacion_Izquierda	= DT_LEFT
	};

	#define DLISTAEX_COLUMNA_ANCHO_AUTO -1	// Ancho de columna autom�tico


	/* Por el momento no est� previsto modificar el n�mero de columnas una vez se han a�adido items, ya que no es necesario en el contexto del reproductor */
	class DListaEx_Columna 	{
	  public:
										DListaEx_Columna(void) : Alineacion(DListaEx_Columna_Alineacion_Derecha), Ancho(0), _AnchoCalculado(0) { };
										DListaEx_Columna(const DListaEx_Columna_Alineacion nAlineacion, const LONG nAncho) : Alineacion(nAlineacion), Ancho(nAncho), _AnchoCalculado(0) { };
									   ~DListaEx_Columna(void) { };
 	    DListaEx_Columna_Alineacion		Alineacion;
		LONG                            Ancho;
	  protected:
		LONG                           _AnchoCalculado; // Ancho calculado para las columnas con ancho autom�tico
		friend class DListaEx;
	};

}
#endif