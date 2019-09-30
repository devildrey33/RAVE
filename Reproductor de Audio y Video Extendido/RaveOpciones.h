#pragma once

class RaveVLC_Medio;

#include "RaveSQLite.h"

class RaveOpciones : public RaveSQLite {
  public:
								RaveOpciones(void);

								// Inicia la base de datos
	const BOOL					Iniciar(void);

	const BOOL                  ObtenerTeclasRapidas(void);
	const BOOL                  GuardarTeclasRapidas(void);


	template <typename T> T     Select(const wchar_t* Tabla, const wchar_t* Variable) { 
									T				Ret      = 0;
									wchar_t*		SqlError = NULL;
									int				SqlRet   = 0;
									sqlite3_stmt   *SqlQuery = NULL;
									std::wstring    SqlStr = L"SELECT ";
									SqlStr += Variable;
									SqlStr += L" FROM ";
									SqlStr += Tabla;

									SqlRet = sqlite3_prepare16_v2(_BD, SqlStr.c_str(), -1, &SqlQuery, NULL);
									if (SqlRet) {
										_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
										return 0;
									}
									int VecesBusy = 0;
									while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
										SqlRet = sqlite3_step(SqlQuery);
										if (SqlRet == SQLITE_ROW) {
											Ret = static_cast<T>(sqlite3_column_double(SqlQuery, 0));
										}


										if (SqlRet == SQLITE_BUSY) {
											VecesBusy++;
											if (VecesBusy == 100) break;
										}
									}

									sqlite3_finalize(SqlQuery);

									if (SqlRet == SQLITE_ERROR) {
										_UltimoErrorSQL = static_cast<const wchar_t*>(sqlite3_errmsg16(_BD));
										return 0;
									}

									return (SqlRet != SQLITE_BUSY) ? Ret : 0;
								};

								// Función que obtiene las opciones de la base de datos
	const BOOL					ObtenerOpciones(void);
								// Función que devuelve la versión de la BD
	const int                   ObtenerVersionBD(void);
								// Función que devuelve la versión de las Opciones
	const int                   ObtenerVersionOpciones(void);

								// NOTA Esta función no guarda la posición en la BD
	inline void					AsignarPosVentana(const int nX, const int nY) { _PosX = nX; _PosY = nY; };
								// Opciones que se guardan en la BD
//	const BOOL					GuardarOpciones(void);
	const BOOL					GuardarPosTamVentana(void);
	const BOOL					GuardarPosVentanaOpciones(void);
//	const BOOL					GuardarPosVentanaAsociar(void);
	const BOOL					GuardarPosVentanaAnalizar(void);
	const BOOL					GuardarPosTamDlgDirectorios(RECT &RW, RECT &RC);
	const BOOL					GuardarPosVentanaMomentos(void);

	inline const long			Volumen(void) { return _Volumen; }
	inline long			       *VolumenPtr(void) { return &_Volumen; }
	void						Volumen(const long nVolumen);

	inline const int			PosX(void)                 { return _PosX; }
	inline const int			PosY(void)                 { return _PosY; }
	inline const int			VentanaOpciones_PosX(void) { return _VentanaOpciones_PosX; }
	inline const int			VentanaOpciones_PosY(void) { return _VentanaOpciones_PosY; }
	inline const int			DlgDirectorios_PosX(void)  { return _DlgDirectorios_PosX;  }
	inline const int			DlgDirectorios_PosY(void)  { return _DlgDirectorios_PosY;  }
	inline const int			DlgDirectorios_Ancho(void) { return _DlgDirectorios_Ancho; }
	inline const int			DlgDirectorios_Alto(void)  { return _DlgDirectorios_Alto;  }
	inline const int			VentanaAnalizar_PosX(void) { return _VentanaAnalizar_PosX; }
	inline const int			VentanaAnalizar_PosY(void) { return _VentanaAnalizar_PosY; }
	inline const int			VentanaMomentos_PosX(void) { return _VentanaMomentos_PosX; }
	inline const int			VentanaMomentos_PosY(void) { return _VentanaMomentos_PosY; }

	inline const int			Ancho(void) { return _Ancho; }
	inline const int			Alto(void)  { return _Alto;  }

	inline const BOOL			Shufle(void) { return _Shufle; }
	void						Shufle(const BOOL nShufle);

	inline const Tipo_Repeat	Repeat(void) { return _Repeat; }
	void						Repeat(const Tipo_Repeat nRepeat);

	inline const Tipo_Inicio	Inicio(void) { return _Inicio; }
	void						Inicio(const Tipo_Inicio nInicio);
								// Tiempo para ocultar los controles pantalla completa
	inline const int			OcultarMouseEnVideo(void) { return _OcultarMouseEnVideo; }
	void						OcultarMouseEnVideo(const int nOcultarMouseEnVideo);

	inline const BOOL			MostrarObtenerMetadatos(void) { return _MostrarObtenerMetadatos; }
	void						MostrarObtenerMetadatos(const BOOL nMostrarObtenerMetadatos);

	inline const BOOL			MostrarAsociarArchivos(void) { return _MostrarAsociarArchivos; }
	void						MostrarAsociarArchivos(const BOOL nMostrarAsociarArchivos);

	inline const BOOL			AnalizarMediosPendientes(void) { return _AnalizarMediosPendientes; }
	void						AnalizarMediosPendientes(const BOOL nAnalizarMediosPendientes);

	inline const BOOL			BuscarActualizacion(void) { return _BuscarActualizacion; }
	void						BuscarActualizacion(const BOOL nBuscarActualizacion);

	inline const UINT			TiempoAnimaciones(void) { return _TiempoAnimaciones; }
	void						TiempoAnimaciones(const UINT nTiempoAnimaciones);

	inline const UINT			TiempoToolTips(void) { return _TiempoToolTips; }
	void						TiempoToolTips(const UINT nTiempoToolTips);

	inline const BOOL			NoAgregarMedioMenos25(void) { return _NoAgregarMedioMenos25; }
	void						NoAgregarMedioMenos25(const BOOL nNoAgregarMedioMenos25);

	inline const BOOL			NoGenerarListasMenos3(void) { return _NoGenerarListasMenos3; }
	void						NoGenerarListasMenos3(const BOOL nNoGenerarListasMenos3);

	inline const BOOL			Sumar005(void) { return _Sumar005; }
	void						Sumar005(const BOOL n_pciones_Sumar005);

	inline const int			AlineacionControlesVideo(void) { return _AlineacionControlesVideo; }
	void						AlineacionControlesVideo(const int nAlineacionControlesVideo);

	inline const int			OpacidadControlesVideo(void) { return _OpacidadControlesVideo; }
	void						OpacidadControlesVideo(const int nOpacidadControlesVideo);

	inline const UINT			EfectoFadeAudioMS(void) { return _EfectoFadeAudioMS; }
	void						EfectoFadeAudioMS(const UINT nEfectoFadeAudioMS);

	inline const BOOL			EfectoFadeAudio(void) { return _EfectoFadeAudio; }
	void						EfectoFadeAudio(const BOOL nEfectoFadeAudio);

	inline const BOOL			OcultarTooltipsMouse(void) { return _OcultarTooltipsMouse; }
	void						OcultarTooltipsMouse(const BOOL nOcultarTooltipsMouse);

	inline const BOOL			MostrarMedioActualTitulo(void) { return _MostrarMedioActualTitulo; }
	void						MostrarMedioActualTitulo(const BOOL nMostrarMedioActualTitulo);

	inline const int 			MezclarListaGenero(void) { return _MezclarListaGenero; }
	void						MezclarListaGenero(const int nMezclarListaGenero);

	inline const int			MezclarListaGrupo(void) { return _MezclarListaGrupo; }
	void						MezclarListaGrupo(const int nMezclarListaGrupo);

	inline const int			MezclarListaDisco(void) { return _MezclarListaDisco; }
	void						MezclarListaDisco(const int nMezclarListaDisco);

	inline const int			MezclarLista50Can(void) { return _MezclarLista50Can; }
	void						MezclarLista50Can(const int nMezclarLista50Can);

	inline const int			MezclarListaNota(void) { return _MezclarListaNota; }
	void						MezclarListaNota(const int nMezclarListaNota);
								// Guardar brillo, contraste, saturación y proporción
	inline const BOOL			GuardarBSCP(void) { return _GuardarBSCP; }
	void						GuardarBSCP(const BOOL nGuardarBSCP);

  protected:

    const BOOL                 _CrearTablas(void);

	const BOOL                 _ModificarTablas(void);
	
/*	std::wstring               _UltimoErrorSQL;
	sqlite3                   *_BD;*/

								// Opciones en memória
	std::wstring               _PathAbrir;					// Ultimo path donde se ha abierto un archivo externo
	long                       _Volumen;
	int                        _PosX;
	int                        _PosY;
	int                        _VentanaOpciones_PosX;
	int                        _VentanaOpciones_PosY;
	int                        _DlgDirectorios_PosX;
	int                        _DlgDirectorios_PosY;
	int                        _DlgDirectorios_Ancho;
	int                        _DlgDirectorios_Alto;
	int                        _VentanaAnalizar_PosX;
	int                        _VentanaAnalizar_PosY;
	int                        _VentanaMomentos_PosX;
	int                        _VentanaMomentos_PosY;
	int                        _Ancho;
	int                        _Alto;
	BOOL                       _Shufle;
	Tipo_Repeat                _Repeat;
	Tipo_Inicio                _Inicio;						// Como se inicia el reproductor (lista de inicio)
	float                      _Version;					// Versión de la base de datos
	int                        _OcultarMouseEnVideo;		// Tiempo en MS que tarda en ocultarse el mouse y los controles de un video
	BOOL                       _MostrarObtenerMetadatos;	// Mostrar la ventana del thread obtener metadatos
	BOOL                       _MostrarAsociarArchivos;		// Mostrar la ventana para asociar este reproductor con todas las extensiones de medios conocidas.
	BOOL					   _AnalizarMediosPendientes;	// Analizar medios pendientes al actualizar el arbol de la base de datos
	BOOL					   _BuscarActualizacion;		// Buscar actualizaciones del reproductor al iniciar
	UINT                       _TiempoAnimaciones;			// Duración de las animaciones
	UINT                       _TiempoToolTips;				// Duración de los tooltips informativos de la parte inferior derecha

	BOOL                       _NoAgregarMedioMenos25;		// No Agregar medios con menos de 2.5 de nota a las listas aleatórias
	BOOL                       _NoGenerarListasMenos3;		// No generar listas aleatórias con menos de 3 medios
	BOOL                       _Sumar005;					// Sumar 0.05 a la nota al reproducir completamente un medio
	int				           _AlineacionControlesVideo;	// Alineación para los controles pantalla completa
	int						   _OpacidadControlesVideo;		// Opacidad máxima para los controles pantalla completa

	UINT                       _EfectoFadeAudioMS;			// Milisegundos para el efecto fade audio
	BOOL                       _EfectoFadeAudio;			// Activar / Desactivar efecto fade in/out
	BOOL					   _OcultarTooltipsMouse;		// Ocultar los tooltips al pasar encima con el mouse
	BOOL					   _MostrarMedioActualTitulo;	// Mostrar el nombre del medio actual en la barra de titulo
	int                        _MezclarListaGenero;			// Mezclar listas aleatórias por genero
	int                        _MezclarListaGrupo;			// Mezclar listas aleatórias por grupo
	int                        _MezclarListaDisco;			// Mezclar listas aleatórias por disco
	int                        _MezclarLista50Can;			// Mezclar listas aleatórias con 50 canciones
	int                        _MezclarListaNota;			// Mezclar listas por Nota
	BOOL                       _GuardarBSCP;				// Guardar Brillo, Saturación, Contraste y Proporción
	float                      _VersionOpciones;			// Versión de las opciones

//	friend class RaveVLC_Medio;
};

