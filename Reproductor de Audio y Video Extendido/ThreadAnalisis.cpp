#include "stdafx.h"
#include "ThreadAnalisis.h"
#include "DStringUtils.h"
#include "DMensajesWnd.h"
#include "resource.h"

#define ID_BARRAPROGRESO1		1000
#define ID_BARRAPROGRESO2		1001
#define ID_BARRAPROGRESO3		1002
#define ID_BOTONCANCELAR		1003
#define ID_MARCANOMOSTRARMAS	1004
#define ID_BOTONOCULTAR			1005

ThreadAnalisis::ThreadAnalisis(void) : DWL::DVentana(), _FASE(0), _VentanaPlayer(NULL), _Cancelar(FALSE), _Thread(NULL) {
}


ThreadAnalisis::~ThreadAnalisis(void) {
}



const BOOL ThreadAnalisis::Iniciar(HWND nhWndDest) {
	// Se está ejecutando 
	if (_Thread != NULL) return FALSE;

	// Asigno Ventana para los mensajes
	_VentanaPlayer = nhWndDest;

	if (App.Opciones.MostrarObtenerMetadatos() == TRUE) {
		// Creo la ventana que mostrará el progreso
		CrearVentana(NULL, L"RAVE_ObtenerMetadatos", L"Analizando...", 300, 200, 660, 380, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, NULL, NULL, NULL, NULL, IDI_REPRODUCTORDEAUDIOYVIDEOEXTENDIDO);
		RECT RC;
		GetClientRect(_hWnd, &RC);
		_BarraProgreso1.CrearBarraProgresoEx(this, 30, 105, RC.right - 60, 20, ID_BARRAPROGRESO1);
		_BarraProgreso1.MostrarValor(DBarraEx_MostrarValor_ValorMaximoInt);
		_BarraProgreso1.Activado(TRUE);
		_BarraProgreso2.CrearBarraProgresoEx(this, 30, 205, RC.right - 60, 20, ID_BARRAPROGRESO2);
		_BarraProgreso2.MostrarValor(DBarraEx_MostrarValor_ValorMaximoInt);
		_BarraProgreso2.Activado(FALSE);
		_BarraProgreso3.CrearBarraProgresoEx(this, 30, 305, RC.right - 60, 20, ID_BARRAPROGRESO3);
		_BarraProgreso3.MostrarValor(DBarraEx_MostrarValor_ValorMaximoInt);
		_BarraProgreso3.Activado(FALSE);
		_BotonOcultar.CrearBotonEx(this, L"Ocultar", 300, 340, 100, 30, ID_BOTONOCULTAR);
		_BotonCancelar.CrearBotonEx(this, L"Cancelar", 410, 340, 100, 30, ID_BOTONCANCELAR);
		_MarcaNoMostrarMas.CrearMarcaEx(this, L"No volver a mostrar esta ventana.", 10, 346, 250, 20, ID_MARCANOMOSTRARMAS, IDI_CHECK2);
		_FuenteNormal.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE);
		_FuenteNegrita.CrearFuente(FUENTE_NORMAL, FUENTE_NOMBRE, TRUE);
	}

	// Iniciamos el Thread
	_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->_ThreadAnalisis, (void *)this, 0, NULL);

	Cancelar(FALSE);

	if (_Thread == NULL)
		return FALSE;

	SetThreadPriority(_Thread, 0);

	Debug_Escribir(L"ThreadAnalisis::Iniciado\n");
	return TRUE;
}

HANDLE ThreadAnalisis::Thread(void) {
	return _Thread;
}

void ThreadAnalisis::Terminar(void) {
	Visible(FALSE);
	Destruir();
	CloseHandle(_Thread);
	_Thread = NULL;
}

void ThreadAnalisis::Cancelar(const BOOL nCancelar) {
	_Mutex.lock();
	_Cancelar = nCancelar;
	_Mutex.unlock();
	if (nCancelar == TRUE && _hWnd != NULL) {
		ShowWindow(_hWnd, SW_HIDE);
	}
}

const BOOL ThreadAnalisis::Cancelar(void) {
	_Mutex.lock();
	BOOL Ret = _Cancelar;
	_Mutex.unlock();
	return Ret;
}

unsigned long ThreadAnalisis::_ThreadAnalisis(void *pThis) {
	ThreadAnalisis *This = reinterpret_cast<ThreadAnalisis *>(pThis);
	// Fase 1 : Obtener una lista de medios que no se han parseado
	This->_BD.Iniciar();
	This->_PorParsear.resize(0);
	This->_BD.ObtenerMediosPorParsear(This->_PorParsear, This->_Unidades);

	if (This->_PorParsear.size() > 0) {
		PostMessage(This->_VentanaPlayer, WM_TOM_MOSTRARVENTANA, 0, 0);		

		// Fase 2 : Iniciar la VLC en este thread
		const char * const Argumentos[] = { "-v", "--vout=vdummy" };
		libvlc_instance_t *VLC = libvlc_new(2, Argumentos);

		int Contador = 0;

		// Mando un mensaje con el total de medios en los que hay que analizar sus metadatos
		PostMessage(This->_hWnd, WM_TOM_INICIADO1, static_cast<WPARAM>(This->_PorParsear.size()), 0);

		// Inicio la transaccion por segunda vez
		This->_BD.Consulta(L"BEGIN TRANSACTION");
		// Fase 3 : analizar nuevos medios añadidos a la BD
		for (size_t i = 0; i < This->_PorParsear.size(); i++) {
			if (This->Cancelar() == TRUE) {
				break;
			}

			// Guardo los datos en la BD cada 50 iteraciones
			if (Contador < 50) { Contador++; }
			else {
				This->_BD.Consulta(L"COMMIT TRANSACTION");
				This->_BD.Consulta(L"BEGIN TRANSACTION");
				Contador = 0;
			}

			This->_Parsear(VLC, This->_PorParsear[i]);
			PostMessage(This->_VentanaPlayer, WM_TOM_TOTALMEDIOS1, i, static_cast<LPARAM>(This->_PorParsear.size()));
			if (This->_hWnd != NULL) PostMessage(This->_hWnd, WM_TOM_TOTALMEDIOS1, i, static_cast<LPARAM>(This->_PorParsear.size()));
		}

		if (This->_hWnd != NULL) PostMessage(This->_hWnd, WM_TOM_TOTALMEDIOS1, static_cast<WPARAM>(This->_PorParsear.size()), static_cast<LPARAM>(This->_PorParsear.size()));

		// Terminada la FASE 3
		libvlc_release(VLC);
		This->_BD.Consulta(L"COMMIT TRANSACTION");


		// FASE 4 Revisión de anomalias en los generos, grupos y discos (solo del Tag)
		This->_RevisarMedios();
		This->_LimpiarAnomalias();
	}
	This->_BD.Terminar();

	if (This->Cancelar() == TRUE)	PostMessage(This->_VentanaPlayer, WM_TOM_CANCELADO, 0, static_cast<LPARAM>(This->_PorParsear.size()));
	else                            PostMessage(This->_VentanaPlayer, WM_TOM_TERMINADO, 0, static_cast<LPARAM>(This->_PorParsear.size()));

	return 0;
}


void ThreadAnalisis::_RevisarMedios(void) {
	// FASE 1 : Obtengo los medios revisables (solo los del Tipo_Audio)
	_BD.ObtenerMediosPorRevisar(_Medios, _Unidades);
	size_t i = 0, i2 = 0, i3 = 0, Pos = 0;


	// Mando un mensaje con el total de medios en los que hay que analizar sus metadatos


	// Creo las listas de generos, grupos y discos por comparar, y las enlazo con sus medios
	for (i = 0; i < _Medios.size(); i++) {
		_AgregarMedio(_Generos,		&_Medios[i], _Medios[i].Genero);
//		_AgregarMedio(_GruposPath,	&_Medios[i], _Medios[i].GrupoPath);
		_AgregarMedio(_GruposTag,	&_Medios[i], _Medios[i].GrupoTag);
//		_AgregarMedio(_DiscosPath,	&_Medios[i], _Medios[i].DiscoPath);
		_AgregarMedio(_DiscosTag,	&_Medios[i], _Medios[i].DiscoTag);
	}

	if (_hWnd != NULL) PostMessage(_hWnd, WM_TOM_INICIADO2, static_cast<WPARAM>(_Generos.size() + _GruposTag.size() + _DiscosTag.size()), 0);
	PostMessage(_VentanaPlayer, WM_TOM_INICIADO2, static_cast<WPARAM>(_Generos.size() + _GruposTag.size() + _DiscosTag.size()), 0);

	// FASE 2 : Creo las listas de anomalias por Genero, GrupoPath, GrupoTag, DiscoPath, DiscoTag. Y las resuelvo con la anómalia que tiene mas medios
	// Creo la lista de anomalias para los generos
	_CrearListaAnomalias(_Generos, _AnomaliasGenero, 0);
	// Creo la lista de anomalias para los grupos del path
//	_CrearListaAnomalias(_GruposPath, _AnomaliasGrupoPath, 1);
	// Creo la lista de anomalias para los grupos del tag
	_CrearListaAnomalias(_GruposTag, _AnomaliasGrupoTag, 1);
	// Creo la lista de anomalias para los discos del path
//	_CrearListaAnomalias(_DiscosPath, _AnomaliasDiscoPath, 2);
	// Creo la lista de anomalias para los discos del tag
	_CrearListaAnomalias(_DiscosTag, _AnomaliasDiscoTag, 2);

	if (Cancelar() == TRUE) return;

	// FASE 3 : Actualizo toda la memória para despues poder guardar en barrena todos los datos actualizados en la BD
	std::wstring TmpTxt;
	// Actualizo los datos de los Generos en memória
	for (i = 0; i < _AnomaliasGenero.size(); i++) {
		TmpTxt = _AnomaliasGenero[i]->TextoFinal(Pos);
		for (i2 = 0; i2 < _AnomaliasGenero[i]->Anomalias.size(); i2++) {
			if (i2 != Pos) { // Si no es la posición con el texto final
				for (i3 = 0; i3 < _AnomaliasGenero[i]->Anomalias[i2]->Medios.size(); i3++) {
					_AnomaliasGenero[i]->Anomalias[i2]->Medios[i3]->Genero = TmpTxt;
					_AnomaliasGenero[i]->Anomalias[i2]->Medios[i3]->Actualizar = TRUE;
				}
			}
		}
	}


	// Actualizo los datos de los GruposTag en memória
	for (i = 0; i < _AnomaliasGrupoTag.size(); i++) {
		TmpTxt = _AnomaliasGrupoTag[i]->TextoFinal(Pos);
		for (i2 = 0; i2 < _AnomaliasGrupoTag[i]->Anomalias.size(); i2++) {
			if (i2 != Pos) { // Si no es la posición con el texto final
				for (i3 = 0; i3 < _AnomaliasGrupoTag[i]->Anomalias[i2]->Medios.size(); i3++) {
					_AnomaliasGrupoTag[i]->Anomalias[i2]->Medios[i3]->GrupoTag = TmpTxt;
					_AnomaliasGrupoTag[i]->Anomalias[i2]->Medios[i3]->Actualizar = TRUE;
				}
			}
		}
	}

	
	// Actualizo los datos de los DiscosTag en memória
	for (i = 0; i < _AnomaliasDiscoTag.size(); i++) {
		TmpTxt = _AnomaliasDiscoTag[i]->TextoFinal(Pos);
		for (i2 = 0; i2 < _AnomaliasDiscoTag[i]->Anomalias.size(); i2++) {
			if (i2 != Pos) { // Si no es la posición con el texto final
				for (i3 = 0; i3 < _AnomaliasDiscoTag[i]->Anomalias[i2]->Medios.size(); i3++) {
					_AnomaliasDiscoTag[i]->Anomalias[i2]->Medios[i3]->DiscoTag = TmpTxt;
					_AnomaliasDiscoTag[i]->Anomalias[i2]->Medios[i3]->Actualizar = TRUE;
				}
			}
		}
	}

	// FASE 4 : Guardo los datos actualizados en la BD
	_BD.Consulta(L"BEGIN TRANSACTION");

	for (i = 0; i < _Medios.size(); i++) {
		if (_Medios[i].Actualizar == TRUE) {
			_BD.ActualizarMedioAnalisis(&_Medios[i]);
		}
	}

	_BD.Consulta(L"COMMIT TRANSACTION");
	

	// FASE 5 : Una vez actualizadas las anomalias, creo una lista de etiquetas, de los generos, grupos, y discos (tanto del tag como del path)
	if (_hWnd != NULL) PostMessage(_hWnd, WM_TOM_INICIADO3, static_cast<WPARAM>(_Medios.size()), 0);

	// TRUNCATE TABLE no se puede ejecutar dentro de una transacción.
	// TRUNCATE TABLE no funciona... hay que utilizar un DELETE sin clausula WHERE
	_BD.Consulta(L"DELETE FROM Etiquetas"); 

	_BD.Consulta(L"BEGIN TRANSACTION");

	for (i = 0; i < _Medios.size(); i++) {
		_AgregarEtiqueta(_Medios[i].Genero,		TIPO_GENERO,	_Medios[i].Nota, _Medios[i].Tiempo, _Medios[i].Longitud);
		_AgregarEtiqueta(_Medios[i].GrupoPath,	TIPO_GRUPOPATH, _Medios[i].Nota, _Medios[i].Tiempo, _Medios[i].Longitud);
		_AgregarEtiqueta(_Medios[i].GrupoTag,	TIPO_GRUPOTAG,  _Medios[i].Nota, _Medios[i].Tiempo, _Medios[i].Longitud);
		_AgregarEtiqueta(_Medios[i].DiscoPath,	TIPO_DISCOPATH, _Medios[i].Nota, _Medios[i].Tiempo, _Medios[i].Longitud);
		_AgregarEtiqueta(_Medios[i].DiscoTag,	TIPO_DISCOTAG,  _Medios[i].Nota, _Medios[i].Tiempo, _Medios[i].Longitud);
		if (_hWnd != NULL) PostMessage(_hWnd, WM_TOM_TOTALMEDIOS3, i, 0);
		PostMessage(_VentanaPlayer, WM_TOM_TOTALMEDIOS3, i, _Medios.size());
		if (Cancelar() == TRUE) return;
	}


	for (i = 0; i < _Etiquetas.size(); i++) {
		std::wstring Q = L"INSERT INTO Etiquetas (Texto, Tipo, Medios, Nota, Tiempo, Longitud) "
							L"VALUES(\"" +	_Etiquetas[i].Texto							+ L"\", " + 
											std::to_wstring(_Etiquetas[i].Tipo)			+ L", " + 
											std::to_wstring(_Etiquetas[i].Medios)		+ L", " +
											DWL::Strings::ToStrF(_Etiquetas[i].Nota, 2)	+ L", " + 
											std::to_wstring(_Etiquetas[i].Tiempo)		+ L", " + 
											std::to_wstring(_Etiquetas[i].Longitud)		+ L")";

//		_BD.ConsultaVarg(L"INSERT INTO Etiquetas (Texto, Tipo, Medios, Nota, Tiempo) VALUES(\"%s\", %d, %d, %f, %d)", _Etiquetas[i].Texto.c_str(), _Etiquetas[i].Tipo, _Etiquetas[i].Medios, _Etiquetas[i].Nota, _Etiquetas[i].Tiempo );
		_BD.Consulta(Q);
	}

	_BD.Consulta(L"COMMIT TRANSACTION");

	
}


// TipoCoincidencia (0 : Genero, 1 : Grupo, 2 : Disco)
void ThreadAnalisis::_CrearListaAnomalias(std::vector<CoincidenciasTexto *> &Coincidencias, std::vector<AnomaliasTexto *> &Anomalias, const UINT TipoCoincidencia) {
	size_t i = 0, i2 = 0, i3 = 0;
	// Creo la lista de anomalias para los discos del tag
	for (i = 0; i < Coincidencias.size(); i++) {
		if (_hWnd != NULL) PostMessage(_hWnd, WM_TOM_TOTALMEDIOS2, 0, 0);
		PostMessage(_VentanaPlayer, WM_TOM_TOTALMEDIOS2, 0, 0);

		if (Cancelar() == TRUE) return;
		for (i2 = 0; i2 < Coincidencias.size(); i2++) {
			if (i2 != i) {
				if (_CompararCoincidencias(Coincidencias[i], Coincidencias[i2], TipoCoincidencia) == 1) {
					BOOL Existe = FALSE;
					for (i3 = 0; i3 < Anomalias.size(); i3++) {
						if (Anomalias[i3]->MirarAnomalia(Coincidencias[i], Coincidencias[i2]) == TRUE) {
							Existe = TRUE;
							break;
						}
					}
					if (Existe == FALSE) {
						AnomaliasTexto *An = new AnomaliasTexto(Coincidencias[i], Coincidencias[i2]);
						Anomalias.push_back(An);
					}
				}
			}
		}
	}
}


const int ThreadAnalisis::_CompararCoincidencias(CoincidenciasTexto *Coincidencia1, CoincidenciasTexto *Coincidencia2, const int TipoCoincidencia) {
	int Distancia = _BD.Distancia(Coincidencia1->Texto, Coincidencia2->Texto),
		Distancia2 = 100;
	std::wstring Tmp;
	if (TipoCoincidencia == 1 && Distancia != 1) {
		// Si la comprobación no es válida falta ver si solapando los dos strings se obtiene resultado
		// Parte 7.1 especial para los Grupos
		// Esta parte compara las dos entradas para ver si la entrada mas pequeña corresponde con la parte inicial de la entrada mas grande
		// Un ejemplo seria "Iron Maiden" y "Iron Maiden LP tal"
		//                   ===========     ===========
		// Para generos y discos parece ser bastante inutil, pero en gurpos algunas veces he encontrado cosas que podrian eliminarse		
		if (Coincidencia1->Texto.size() > Coincidencia2->Texto.size()) {
			Distancia2 = _BD.Distancia(Coincidencia1->Texto.substr(0, Coincidencia2->Texto.size()), Coincidencia2->Texto);
		}
		else if (Coincidencia2->Texto.size() > Coincidencia1->Texto.size()) {
			Distancia2 = _BD.Distancia(Coincidencia2->Texto.substr(0, Coincidencia1->Texto.size()), Coincidencia1->Texto);
		}

		// Si la comprobación ha dado valida miraremos los directorios de penultimo nivel a ver si coinciden
		// c:\mp3\rage\disc\can.mp3							5
		// c:\mp3\rage against the machine\disc\can.mp3		5
		// 0   1              2              3     4
		// En el caso de que cualquiera de los dos no tenga mas de 4 
		if (Distancia2 == 0 || Distancia2 == 1) {
			Strings::Split Split1(Coincidencia1->Medios[0]->Path, L'\\');
			Strings::Split Split2(Coincidencia2->Medios[0]->Path, L'\\');
			if (Split1.Total() > 3 && Split2.Total() > 3) {
				if (Split1[Split1.Total() - 3] == Split2[Split2.Total() - 3]) { // los dos directorios de penultimo nivel son iguales
					return 1;
				}
			}
		}

	}
	else if (Distancia == 1 && TipoCoincidencia == 2) {
		// Si la comprobación es valida y el tipo es disco, miramos si no es un disco doble
		// Parte 7.2 especial para discos
		// En el caso de que la comprobacion sea positiva y estemos analizando un disco, debemos tener en cuenta que puede ser un disco doble.
		// Un ejemplo de disco doble seria el sigueinte :
		//  c:\mp3\iron maiden\disc\disc 1\01 can.mp3       -> "disc 1", "cd 1", "CD I", "disk I", y miles mas..
		//  c:\mp3\iron maiden\disc\disc 2\01 can.mp3       -> "disc 2", "cd 2", "CD II", "disk II", y miles mas..
		// Si realmente existe una anomalia, lo mas logico es que dicha anomalia se encuentre en la misma carpeta, por lo que habra que compararlas

		// Escaneamos todas las anomalias para ver si tienen pista 01
		size_t z1 = 0, z2 = 0;
		BOOL PistaOrigen = FALSE, PistaDestino = FALSE;

		for (z1 = 0; z1 < Coincidencia1->Medios.size() && PistaOrigen == FALSE; z1++) {
			if (Coincidencia1->Medios[z1]->PistaPath == 1) PistaOrigen  = TRUE;
			if (Coincidencia1->Medios[z2]->PistaTag == 1)  PistaDestino = TRUE;
		}
		for (z2 = 0; z2 < Coincidencia2->Medios.size() && PistaDestino == FALSE; z2++) {
			if (Coincidencia2->Medios[z2]->PistaPath == 1) PistaDestino = TRUE;
			if (Coincidencia2->Medios[z2]->PistaTag == 1)  PistaDestino = TRUE;
		}
		// Existen canciones con la pista 01 en las dos anomalias, es un disco doble.
		if (PistaOrigen == TRUE && PistaDestino == TRUE) return 100;

		for (z1 = Coincidencia1->Medios[0]->Path.size() - 1; z1 > 0; z1--) {
			if (Coincidencia1->Medios[0]->Path[z1] == L'\\') break;
		}
		for (z2 = Coincidencia2->Medios[0]->Path.size() - 1; z2 > 0; z2--) {
			if (Coincidencia2->Medios[0]->Path[z2] == L'\\') break;
		}

		// Los directorios son iguales, no es un disco doble
		if (Coincidencia1->Medios[0]->Path.substr(0, z1) == Coincidencia2->Medios[0]->Path.substr(0, z2))	return 1;
		else																								return 100;
	}

	return Distancia;
}


void ThreadAnalisis::_LimpiarAnomalias(void) {
	size_t i = 0;
	for (i = 0; i < _Generos.size(); i++)				delete _Generos[i];
//	for (i = 0; i < _GruposPath.size(); i++)			delete _GruposPath[i];
	for (i = 0; i < _GruposTag.size(); i++)				delete _GruposTag[i];
//	for (i = 0; i < _DiscosPath.size(); i++)			delete _DiscosPath[i];
	for (i = 0; i < _DiscosTag.size(); i++)				delete _DiscosTag[i];
	for (i = 0; i < _AnomaliasGenero.size(); i++)		delete _AnomaliasGenero[i];
//	for (i = 0; i < _AnomaliasGrupoPath.size(); i++)	delete _AnomaliasGrupoPath[i];
	for (i = 0; i < _AnomaliasGrupoTag.size(); i++)		delete _AnomaliasGrupoTag[i];
//	for (i = 0; i < _AnomaliasDiscoPath.size(); i++)	delete _AnomaliasDiscoPath[i];
	for ( i = 0; i < _AnomaliasDiscoTag.size(); i++)	delete _AnomaliasDiscoTag[i];
	_AnomaliasGenero.resize(0);
//	_AnomaliasGrupoPath.resize(0);
	_AnomaliasGrupoTag.resize(0);
//	_AnomaliasDiscoPath.resize(0);
	_AnomaliasDiscoTag.resize(0);

	_Medios.resize(0);
	_Generos.resize(0);
//	_GruposPath.resize(0);
	_GruposTag.resize(0);
//	_DiscosPath.resize(0);
	_DiscosTag.resize(0);
}

void ThreadAnalisis::_AgregarMedio(std::vector<CoincidenciasTexto *> &ListaMedios, BDMedio *Medio, std::wstring &Texto) {
	for (size_t i = 0; i < ListaMedios.size(); i++) {
		if (Texto == ListaMedios[i]->Texto) { // Ya existe
			ListaMedios[i]->Medios.push_back(Medio);
			return;
		}
	}
	// No existe
	ListaMedios.push_back(new CoincidenciasTexto(Medio, Texto));
}

void ThreadAnalisis::_AgregarEtiqueta(std::wstring &nTexto, const DWORD nTipo, const float nNota, const INT64 nTiempo, const ULONGLONG uLongitud) {
	if (nTexto.size() == 0) return;
	for (size_t i = 0; i < _Etiquetas.size(); i++) {
		if (nTexto == _Etiquetas[i].Texto) {
			_Etiquetas[i].AgregarTipo(nTipo);
			_Etiquetas[i].Medios	++;
			_Etiquetas[i].Nota		+= nNota;
			_Etiquetas[i].Tiempo	+= nTiempo;
			_Etiquetas[i].Longitud	+= uLongitud;
			return;
		}
	}
	_Etiquetas.push_back(EtiquetaBD(nTexto, nTipo, nNota, nTiempo, uLongitud));
}

// Función que obtiene un metadato desde la LibVLC
std::wstring ThreadAnalisis::_ObtenerMeta(libvlc_media_t *Media, libvlc_meta_t Tipo) {
	std::wstring Ret;
	char *Txt = libvlc_media_get_meta(Media, Tipo);
	if (Txt != NULL) {
		std::wstring TmpTxt; // = DString_ToStr(Txt);
		Strings::UTF8ToWide(Txt, TmpTxt);
		RaveBD::FiltroNombre(TmpTxt, Ret);
	}
	libvlc_free(Txt);
	return Ret;
}


void ThreadAnalisis::_Parsear(libvlc_instance_t *VLC, std::wstring &Path) {
	BDMedio Medio;
	_BD.ObtenerMedio(Path, Medio, _Unidades);

	// Compruebo que existe el archivo	
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(Path.c_str())) {
		Medio.Parseado = TRUE;
		_BD.ActualizarMedio(&Medio);
		return;
	}

	// Hay que convertir el path a UTF8 para que funcione en el VLC...
	char	Destino[MAX_PATH];
	size_t  TamnTexto = wcslen(Path.c_str()) + 1;
	int		TamRes = WideCharToMultiByte(CP_UTF8, NULL, Path.c_str(), static_cast<int>(TamnTexto), Destino, MAX_PATH, NULL, NULL);
	Destino[TamRes] = 0;	// finalizo el string porque en la versión RELEASE de WideCharToMultiByte no se pone el terminador de cadenas, en la debug si.... (NO TESTEADO DESDE VS2008)

	libvlc_media_t *Media = NULL;
	Media = libvlc_media_new_path(VLC, Destino);

	HANDLE Sem = CreateSemaphore(NULL, 0, 1, L"RAVE_ThreadAnalisis");
	//	vlc_sem_t sem;
	//	vlc_sem_init(&sem, 0);

	// Check to see if we are properly receiving the event.
	libvlc_event_manager_t *em = libvlc_media_event_manager(Media);
	libvlc_event_attach(em, libvlc_MediaParsedChanged, _ParsearTerminado, Sem);

	// Parse the media. This is synchronous.
	int i_ret = libvlc_media_parse_with_options(Media, libvlc_media_parse_local, -1);
	// Espero a que termine el parsing o 10 segundos máximo
	WaitForSingleObject(Sem, 10000);

	// Si se ha realizado el parsing correctamente
	if (libvlc_media_get_parsed_status(Media) == libvlc_media_parsed_status_done) {		

		std::wstring TmpTxt, TmpPath, Filtrado;
//		UINT nPista;

		char *Txt = libvlc_media_get_meta(Media, libvlc_meta_Title);
		Strings::UTF8ToWide(Txt, TmpTxt);
		libvlc_free(Txt);

//		TmpTxt			= _ObtenerMeta(Media, libvlc_meta_Title);

		size_t PosNombre = Medio.Path.find_last_of(L"\\");
		
		// No tiene el mismo nombre de archivo
		TmpPath = Medio.Path.substr(PosNombre + 1);
		if (Medio.Path.substr(PosNombre + 1) != TmpTxt) {
//			RaveBD::_AnalizarNombre(TmpTxt, Filtrado, nPista);
			Medio.NombreTag = Filtrado;
		}

		Medio.GrupoTag	= _ObtenerMeta(Media, libvlc_meta_Artist);
		Medio.DiscoTag	= _ObtenerMeta(Media, libvlc_meta_Album);
		Medio.Genero	= _ObtenerMeta(Media, libvlc_meta_Genre);
		TmpTxt			= _ObtenerMeta(Media, libvlc_meta_TrackNumber);
		Medio.PistaTag	= Strings::StrTo(TmpTxt, Medio.PistaTag);
		Medio.Parseado	= TRUE;


		Medio.Tiempo = libvlc_media_get_duration(Media);

/*		libvlc_media_track_t **pp_tracks;
		unsigned int i_count = libvlc_media_tracks_get(Media, &pp_tracks);
//		if (i_count > 0) {
			const char *Codec = (const char *)&pp_tracks[0]->i_codec;
			const char *FourCC = (const char *)&pp_tracks[0]->i_original_fourcc;
	//	}
		libvlc_media_tracks_release(pp_tracks, i_count);*/

		_BD.ActualizarMedio(&Medio);
	}
	// Ha fallado..
	else {
		libvlc_media_parse_stop(Media);
		Medio.Parseado = TRUE;
		_BD.ActualizarMedio(&Medio);
	}

	libvlc_media_release(Media);
}

void ThreadAnalisis::_ParsearTerminado(const libvlc_event_t *event, void *user_data) {
	(void)event;
	HANDLE Sem = (HANDLE)user_data;
	ReleaseSemaphore(Sem, 1, NULL);
	//vlc_sem_post(sem);
}


void ThreadAnalisis::_Evento_Pintar(void) {
	PAINTSTRUCT PS;
	HDC DC = BeginPaint(_hWnd, &PS);
	Pintar(DC);
	EndPaint(_hWnd, &PS);
}

void ThreadAnalisis::Pintar(HDC DC) {

	RECT RC;
	GetClientRect(_hWnd, &RC);
	// Pinto el fondo
	HBRUSH BrochaFondo = CreateSolidBrush(COLOR_FONDO);
	FillRect(DC, &RC, BrochaFondo);

	HFONT VFuente = static_cast<HFONT>(SelectObject(DC, _FuenteNormal()));

	SetBkMode(DC, TRANSPARENT);
	PintarTexto(DC, L"Analizando los datos de los medios en segundo plano.", 10, 10);
	
	COLORREF ColFase1 = (_FASE == 1) ? COLOR_TEXTO_RESALTADO : COLOR_TEXTO_DESACTIVADO;
	COLORREF ColFase2 = (_FASE == 2) ? COLOR_TEXTO_RESALTADO : COLOR_TEXTO_DESACTIVADO;;
	COLORREF ColFase3 = (_FASE == 3) ? COLOR_TEXTO_RESALTADO : COLOR_TEXTO_DESACTIVADO;;

	SelectObject(DC, _FuenteNegrita());
	PintarTexto(DC, L"FASE 1 :", 10, 40, ColFase1);
	PintarTexto(DC, L"FASE 2 :", 10, 140, ColFase2);
	PintarTexto(DC, L"FASE 3 :", 10, 240, ColFase3);

	SelectObject(DC, _FuenteNormal());
	PintarTexto(DC, L"Examinar los metadatos de todos los medios para extraer : generos, grupos, discos, nombres,", 30, 60, ColFase1);
	PintarTexto(DC, L"pistas, y tiempos.", 30, 80, ColFase1);

	PintarTexto(DC, L"Analizar todos los datos en busca de coincidencias muy parecidas entre si, y determinar si", 30, 160, ColFase2);
	PintarTexto(DC, L"realmente son lo mismo o no.", 30, 180, ColFase2);

	PintarTexto(DC, L"Generar una lista de etiquetas de cada genero, grupo, y disco.", 30, 260, ColFase3);
	PintarTexto(DC, L"Con la lista de etiquetas será posible crear listas aleatórias partiendo de una etiqueta.", 30, 280, ColFase3);

	SelectObject(DC, VFuente);
	DeleteObject(BrochaFondo);
}
/*
void ThreadAnalisis::_PintarTexto(HDC DC, const wchar_t *pTexto, const int PosX, const int PosY, COLORREF ColorTexto, COLORREF ColorSombra) {
	SetTextColor(DC, ColorSombra);
	TextOut(DC, PosX + 1, PosY + 1, pTexto, static_cast<int>(wcslen(pTexto)));
	SetTextColor(DC, ColorTexto);
	TextOut(DC, PosX, PosY, pTexto, static_cast<int>(wcslen(pTexto)));
}*/

void ThreadAnalisis::Evento_BotonEx_Mouse_Click(DWL::DEventoMouse &DatosMouse) {
	switch (DatosMouse.ID()) {
		case ID_BOTONCANCELAR	:	Cancelar(TRUE);						break;
		case ID_BOTONOCULTAR	:	ShowWindow(_hWnd, SW_MINIMIZE);		break;
	}

}

LRESULT CALLBACK ThreadAnalisis::GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT			 :	_Evento_Pintar();																					return 0;
		case WM_TOM_TOTALMEDIOS1 :	_BarraProgreso1.Valor(static_cast<float>(wParam));													return 0;
		case WM_TOM_INICIADO1    :  _BarraProgreso1.Maximo(static_cast<float>(wParam));	 _FASE = 1;  Repintar();						return 0;
		case WM_TOM_TOTALMEDIOS2 :	_BarraProgreso2.Valor(_BarraProgreso2.Valor() + 1);													return 0;
		case WM_TOM_INICIADO2    :  
			_BarraProgreso1.Activado(FALSE);
			_BarraProgreso2.Activado(TRUE);
			_BarraProgreso3.Activado(FALSE);
			_BarraProgreso2.Maximo(static_cast<float>(wParam));
			_FASE = 2;	 
			Repintar();					
			return 0;
		case WM_TOM_TOTALMEDIOS3 :	_BarraProgreso3.Valor(static_cast<float>(wParam));													return 0;
		case WM_TOM_INICIADO3    :
			_BarraProgreso1.Activado(FALSE);
			_BarraProgreso2.Activado(FALSE);
			_BarraProgreso3.Activado(TRUE);
			_BarraProgreso3.Maximo(static_cast<float>(wParam));
			_FASE = 3; 
			Repintar();		
			return 0;
		case WM_CLOSE			 :   Cancelar(TRUE);																					return 0;
		case DWL_BOTONEX_CLICK   :   Evento_BotonEx_Mouse_Click(WPARAM_TO_DEVENTOMOUSE(wParam));										return 0;
		case DWL_MARCAEX_CLICK   :   App.Opciones.MostrarObtenerMetadatos(!_MarcaNoMostrarMas.Marcado());								return 0;
		case WM_KEYUP			 :	if (wParam == VK_ESCAPE) { Cancelar(TRUE); }														break;		// Los eventos de teclado tienen un break para que puedan llegar al hook global de teclado para la aplicación
		case WM_EXITSIZEMOVE     :  App.Opciones.GuardarPosVentanaAnalizar();															return 0;
	}
	return DVentana::GestorMensajes(uMsg, wParam, lParam);
}


























// Funcion que determina si alguna de las dos anomalias existe
// y las introduce en el vector
// Retorna false si no existe ninguna de las 2
const BOOL AnomaliasTexto::MirarAnomalia(CoincidenciasTexto *NuevaAnomalia1, CoincidenciasTexto *NuevaAnomalia2) {
	size_t	i		= 0;
	size_t	z		= 0;
	BYTE	Existe	= 0;
	for (i = 0; i < Anomalias.size(); i++) {
		if (NuevaAnomalia1->Texto == Anomalias[i]->Texto) Existe += 2;
		if (NuevaAnomalia2->Texto == Anomalias[i]->Texto) Existe += 4;
	}
	switch (Existe) {
		case 2: // existe el primero
			for (z = 0; z < Anomalias.size(); z++) {
				if (Anomalias[z]->Medios.size() <= NuevaAnomalia2->Medios.size()) {
					Anomalias.insert(Anomalias.begin() + z, NuevaAnomalia2);
					return TRUE;
				}
			}
			Anomalias.push_back(NuevaAnomalia2);
			return TRUE;
		case 4: // Esixte el segundo
			for (z = 0; z < Anomalias.size(); z++) {
				if (Anomalias[z]->Medios.size() <= NuevaAnomalia1->Medios.size()) {
					Anomalias.insert(Anomalias.begin() + z, NuevaAnomalia1);
					return TRUE;
				}
			}
			Anomalias.push_back(NuevaAnomalia1);
			return TRUE;
			break;
		case 6: // Existen los dos...
			return TRUE;
		default: // No existe ninguno
			return FALSE;
	}
	return FALSE;
};

// Función que devuelve el texto con mas coincidencias
std::wstring &AnomaliasTexto::TextoFinal(size_t &OUT_Pos) {
	size_t Tam = 0;
	OUT_Pos = 0;
	for (size_t i = 0; i < Anomalias.size(); i++) {
		if (Anomalias[i]->Medios.size() > Tam) {
			Tam = Anomalias[i]->Medios.size();
			OUT_Pos = i;
		}
	}
	return Anomalias[OUT_Pos]->Texto;
}