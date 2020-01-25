#include "stdafx.h"
#include "ArbolBD.h"
#include "RAVE_Iconos.h"
#include "DStringUtils.h"

ArbolBD::ArbolBD(void) {
}


ArbolBD::~ArbolBD(void) {
}

/*
void ArbolBD::Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_TeclaPresionada(DatosTeclado);
}

void ArbolBD::Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_TeclaSoltada(DatosTeclado);
}

void ArbolBD::Evento_Tecla(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_Tecla(DatosTeclado);
}*/

NodoBD *ArbolBD::BuscarHash(sqlite3_int64 bHash) {
	if (_Raiz.TotalHijos() == 0) return NULL;
	NodoBD *Tmp = static_cast<NodoBD *>(_Raiz.Hijo(0));
	while (Tmp != NULL) {
		if (Tmp->Hash == bHash) return Tmp;
		Tmp = static_cast<NodoBD *>(BuscarNodoSiguiente(Tmp, FALSE, NULL));		
	}
	return NULL;
}



NodoBD *ArbolBD::AgregarBDNodo(const ArbolBD_TipoNodo nTipoNodo, NodoBD *nPadre, const TCHAR *cTexto, const sqlite3_int64 nHash, const UINT nId) {

	// Busco si existe el hash
	if ((nTipoNodo == ArbolBD_TipoNodo_Cancion || nTipoNodo == ArbolBD_TipoNodo_Video) && _Raiz.TotalHijos() != 0) {
		NodoBD *Tmp = static_cast<NodoBD *>(_Raiz.Hijo(0));
		while (Tmp != NULL) {
			if (Tmp->Hash == nHash) return Tmp;
			Tmp = static_cast<NodoBD *>(BuscarNodoSiguiente(Tmp, FALSE, NULL));
		}
	}

	int						 nIcono    = 0;
	DFuente					 nFuente   = Fuente;
	DArbolEx_MostrarExpansor nExpansor = DArbolEx_MostrarExpansor_Auto;
	switch (_ExpansorPorDefecto) {
		case DArbolEx_Expansor_Rectangulo		: nExpansor = DArbolEx_MostrarExpansor_MostrarRectangulo;		break;
		case DArbolEx_Expansor_Triangulo		: nExpansor = DArbolEx_MostrarExpansor_MostrarTriangulo;		break;
		case DArbolEx_Expansor_TrianguloLinea	: nExpansor = DArbolEx_MostrarExpansor_MostrarTrianguloLinea;	break;
	}

//	== DArbolEx_Expansor_Rectangulo) ? DArbolEx_MostrarExpansor_MostrarRectangulo : DArbolEx_MostrarExpansor_MostrarTriangulo;
//	size_t			nPos = 0;
	switch (nTipoNodo) {
		case ArbolBD_TipoNodo_Raiz:			// Raíz añadida por el usuario
			nFuente.CrearFuente(Skin.FuenteTam + 2, Skin.FuenteNombre.c_str(), TRUE, FALSE, FALSE);
			nIcono = RAVE_Iconos::RAVE_Icono_Raiz;
			break;
		case ArbolBD_TipoNodo_Directorio:	// Directorio dentro de una raíz
			nIcono = RAVE_Iconos::RAVE_Icono_Directorio;
			break;
		default:							// Medio dentro de una raíz o directorio
			switch(nTipoNodo) {
				case ArbolBD_TipoNodo_Cancion		:	nIcono = RAVE_Icono_Cancion;	break;
				case ArbolBD_TipoNodo_Video			:	nIcono = RAVE_Icono_Video;		break;
				case ArbolBD_TipoNodo_ListaCanciones:	nIcono = RAVE_Icono_Lista;		break;
			}
//			nIcono = (nTipoNodo == ArbolBD_TipoNodo_Cancion) ? RAVE_Iconos::RAVE_Icono_Cancion : RAVE_Iconos::RAVE_Icono_Video;

			// Si es el medio que se está reproduciendo pongo el icono del play o de la pausa
			if (App.MP.MedioActual().Nombre().size() > 0) {
				if (App.MP.MedioActual().Hash == nHash) {
					Estados_Medio Estado = App.MP.ComprobarEstado();
					if		(Estado == EnPlay)	nIcono = RAVE_Iconos::RAVE_Icono_Play;
					else if (Estado == EnPausa)	nIcono = RAVE_Iconos::RAVE_Icono_Pausa;
				}
			}
			nExpansor = DArbolEx_MostrarExpansor_Ocultar;
			break;
	}

	NodoBD *nNodo	= AgregarNodo<NodoBD>(cTexto, nPadre, nIcono, &nFuente, DARBOLEX_POSICIONNODO_ORDENADO);
	nNodo->Hash		= nHash;
	nNodo->Id		= nId;
	nNodo->TipoNodo = nTipoNodo;
	nNodo->MostrarExpansor(nExpansor);
	return nNodo;	
}




const size_t ArbolBD::AgregarNodoALista(NodoBD *nNodo) {
	if (nNodo == NULL) return FALSE;
	NodoBD *TmpNodo = static_cast<NodoBD *>(nNodo);
	sqlite3_int64 Hash = TmpNodo->Hash;
	//	ArbolBD_Nodo *TmpNodo = aNodo->BDHijo();
	// Si no hay hijos cargo las canciones correspondientes.
	std::wstring Path, TmpStr;

	if (Hash == 0) { // Si no hay hash es que el nodo contiene uno o mas hijos
		while (TmpNodo->Padre() != NULL) {
			if (TmpNodo->IDIcono() == RAVE_Icono_Raiz)	{	Path = TmpNodo->Texto + Path;				}
			else										{	Path = TmpNodo->Texto + L"\\" + Path;		}
			TmpNodo = static_cast<NodoBD *>(TmpNodo->Padre());
		}
	}

	std::wstring SqlStr;
	switch (nNodo->IDIcono()) {
		case RAVE_Icono_Cancion : // Canción
		case RAVE_Icono_Video	: // Video
		case RAVE_Icono_Lista   : // Lista m3u
			SqlStr = L"SELECT * FROM Medios WHERE Hash =\"" + std::to_wstring(Hash) + L"\"";
			//			SqlStr = L"SELECT * FROM Medios WHERE Path LIKE\"" + Path + L"\"";
			break;
		case RAVE_Icono_Raiz :		 // Raiz 
		case RAVE_Icono_Directorio : // Directorio
			SqlStr = L"SELECT * FROM Medios WHERE Path LIKE \"%" + Path.substr(1) + L"%\" COLLATE NOCASE ORDER BY GrupoPath, DiscoPath, PistaPath, PistaTag ASC";	// Path.substr(1) se salta la letra de la unidad y deja el path sin letra/unidad.
			break;																											// La letra de unidad se substituye normalmente por '?' ya que puede ser un medio extraible y no tiene por que estar siempre en la misma letra
	}

	sqlite3_stmt   *SqlQuery = NULL;
	int SqlRet = sqlite3_prepare16_v2(App.BD(), SqlStr.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		const char* Error = sqlite3_errmsg(App.BD());
		return FALSE;
	}

	int		VecesBusy		= 0;
	size_t	MediosAgregados = 0;

	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_MISUSE) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDMedio *Medio = new BDMedio(SqlQuery, App.BD.Unidades, &App.BD);
			// Compruebo que el medio sea de internet o exista en el disco antes de agregar-lo
			if (Medio->Ubicacion() == Ubicacion_Medio_Internet || GetFileAttributes(Medio->Path.c_str()) != INVALID_FILE_ATTRIBUTES) {
				// Agrego el medio a la lista y sumo uno a los medios agregados
				if (App.VentanaRave.Lista.AgregarMedio(Medio) != nullptr) MediosAgregados++;

				// Aseguro que el medio agregado a la lista exista en el arbol.
				_AgregarMedio(static_cast<NodoBD*>(nNodo), Medio);
			}

			delete Medio;
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}
	sqlite3_finalize(SqlQuery);


	// Ejecuto el shufle si es necesario
	if (App.Opciones.Shufle() == TRUE) {
		App.VentanaRave.Lista.Mezclar(TRUE);
		App.VentanaRave.Lista.MedioActual = 0;
	}
	App.VentanaRave.Lista.Repintar();

	return MediosAgregados;
}

// Asegura que el medio agregado a la lista, tambien exista en el arbol
void ArbolBD::_AgregarMedio(NodoBD *nPadre, BDMedio *nMedio) {
	DWL::Strings::Split nSplit(nMedio->Path.c_str(), L'\\');
	size_t TA = nPadre->Ancestros() + 1;
	std::wstring Filtrado;
	NodoBD *TmpPadre = nPadre, *Tmp = NULL;
	// Busco si están creados los subdirectorios
	for (size_t TA = nPadre->Ancestros() + 1; TA < nSplit.Total(); TA++) {
		Tmp = BuscarHijoTxt(nSplit[TA], TmpPadre);
		if (Tmp != NULL) { // El nodo ya existe
			TmpPadre = Tmp;
		}
		else { // No existe, hay que crearlo
			if (TA < nSplit.Total() - 1) { // Directorio				
				App.BD.FiltroPath(nSplit[TA], Filtrado);
				TmpPadre = AgregarBDNodo(ArbolBD_TipoNodo_Directorio, TmpPadre, Filtrado.c_str(), 0, 0);
			}
			else { // Medio
				std::wstring nNombre = nMedio->Nombre();
				// Si tiene pista, la agrego al nombre				
				if (nMedio->Pista() != 0) {
					std::wstring nPista;
					nMedio->PistaStr(nPista);
					nNombre = nPista + L" " + nMedio->Nombre();
				}
				
				App.BD.FiltroNombre(nNombre, Filtrado);
				ArbolBD_TipoNodo Tipo = ArbolBD_TipoNodo_Indefinido;
				switch (nMedio->TipoMedio) {
					case Tipo_Medio_Audio:	Tipo = ArbolBD_TipoNodo_Cancion;		break;
					case Tipo_Medio_Video:	Tipo = ArbolBD_TipoNodo_Video;			break;
					case Tipo_Medio_IpTv:	Tipo = ArbolBD_TipoNodo_IpTv;			break;
					case Tipo_Medio_Lista:	Tipo = ArbolBD_TipoNodo_ListaCanciones;	break;
				}
				Tmp = AgregarBDNodo(Tipo, TmpPadre, Filtrado.c_str(), nMedio->Hash, nMedio->Id);
			}
		}
	}
}

/* Devuelve el primer hijo de nPadre que tiene el mismo texto que Buscar (NOTA no busca en nietos/descendientes) */
NodoBD *ArbolBD::BuscarHijoTxt(std::wstring &Buscar, NodoBD *nPadre) {
	if (_Raiz.TotalHijos() == 0) return NULL;
	if (nPadre == NULL) nPadre = static_cast<NodoBD *>(&_Raiz);

	if (nPadre->TotalHijos() == 0)
		return NULL;

//	NodoBD *Tmp = static_cast<NodoBD *>(nPadre->Hijo(0));
	for (size_t i = 0; i < nPadre->TotalHijos(); i++) {
		std::wstring &rTxt = nPadre->Hijo(i)->Texto;
		if (_wcsicmp(rTxt.c_str(), Buscar.c_str()) == 0)
			return static_cast<NodoBD *>(nPadre->Hijo(i));
	}
	return NULL;
}

void ArbolBD::Evento_Nodo_Expandido(DWL::DArbolEx_Nodo *nNodo, const BOOL nExpandido) {
	if (nExpandido == TRUE && nNodo->TotalHijos() == 0) {
		ExplorarPath(static_cast<NodoBD *>(nNodo));
	}
}

// Obtiene el path del nodo especificado terminado con \ si es un directorio
void ArbolBD::ObtenerPath(NodoBD *nNodo, std::wstring &rPath) {
	DArbolEx_Nodo *Tmp = nNodo;
	std::wstring TmpStr;
	while (Tmp != NULL) {
		if (Tmp->Padre() != NULL) { // Si el padre no es el nodo raiz 
			TmpStr = Tmp->Texto;
			if (TmpStr[TmpStr.size() - 1] == L'\\')
				rPath = Tmp->Texto + rPath;
			else
				rPath = Tmp->Texto + L'\\' + rPath;
		}
		Tmp = Tmp->Padre();
	}
}

// Función que explora el nodo expecificado
void ArbolBD::ExplorarPath(NodoBD *nNodo) {
	if (nNodo == NULL) return;
	ULONGLONG Tick = GetTickCount64();
	Debug_Escribir_Varg(L"ArbolBD::ExplorarPath  Nodo = '%s'\n", nNodo->Texto.c_str());

	WIN32_FIND_DATA		FindInfoPoint;
	HANDLE				hFind = NULL;
	std::wstring        nPath, nNombre, nTmpTxt, nTmpTxt2;
	ObtenerPath(nNodo, nPath);
	nTmpTxt = nPath;
	if (nTmpTxt[nTmpTxt.size() - 1] != TEXT('\\'))	{ nTmpTxt += TEXT("\\*.*");  nTmpTxt += L'\\';	}
	else											{ nTmpTxt += TEXT("*.*");	}

	BOOL AgregarMedios = FALSE;

	// Busco los archivos / directorios del path nodo
	hFind = FindFirstFile(nTmpTxt.c_str(), &FindInfoPoint);
	size_t nEntradas = 0;
	while (FindNextFile(hFind, &FindInfoPoint) != 0) {
		nNombre = FindInfoPoint.cFileName;
		// Si es un directorio
		if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { 
			if (nNombre != L"." && nNombre != L"..") {
				RaveBD::FiltroPath(nNombre, nTmpTxt);
				AgregarBDNodo(ArbolBD_TipoNodo_Directorio, static_cast<NodoBD *>(nNodo), nTmpTxt.c_str(), 0, 0);
				nEntradas++;
			}
		}
		// Es un archivo (Si hay archivos, hay que hacer una consulta que devuelva todos los archivos que tengan el path similar
		else { 
			AgregarMedios = TRUE;
			nEntradas++;
		}
	}
	FindClose(hFind);

	// El nodo no tiene ningun hijo (ni directorios ni medios)
	if (nEntradas == 0) {	nNodo->MostrarExpansor(DArbolEx_MostrarExpansor_Ocultar);	nNodo->Expandido = FALSE;	}
	else				{	nNodo->MostrarExpansor(DArbolEx_MostrarExpansor_Auto);		}

	// Consulta que obtiene todas las entradas que contienen el path
	if (AgregarMedios == TRUE) {
		std::wstring    SqlStr = L"SELECT Id, Hash, Path, NombrePath, TipoMedio, PistaPath FROM Medios WHERE Path LIKE \"%" + nPath.substr(1) + L"%\" COLLATE NOCASE";
		int				SqlRet = 0;
		sqlite3_stmt   *SqlQuery = NULL;
		
		SqlRet = sqlite3_prepare16_v2(App.BD(), SqlStr.c_str(), -1, &SqlQuery, NULL);
		if (SqlRet == SQLITE_ERROR) {
			std::wstring Error = static_cast<const wchar_t *>(sqlite3_errmsg16(App.BD()));
			return; // Error
		}

		size_t BarrasPath = DWL::Strings::ContarCaracter(nPath, L'\\'), BarrasMedio = 0; // Cuento las antibarras del path
		int VecesBusy = 0;
		while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
			SqlRet = sqlite3_step(SqlQuery);
			if (SqlRet == SQLITE_ROW) {
				UINT             mId        = static_cast<UINT>(sqlite3_column_int(SqlQuery, 0));
				size_t			 mHash		= static_cast<size_t>(sqlite3_column_int64(SqlQuery, 1));
				std::wstring	 mPath		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
				std::wstring     mNombre	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
				Tipo_Medio       mTipoMedio	= static_cast<Tipo_Medio>(sqlite3_column_int(SqlQuery, 4));
				UINT             mPista		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 5));
				ArbolBD_TipoNodo mTipoNodo = ArbolBD_TipoNodo_Indefinido;

				BarrasMedio = DWL::Strings::ContarCaracter(mPath, L'\\');

				if (BarrasPath == BarrasMedio) {
					// Si la pista es 0, no la pongo
					if      (mPista == 0)   { nTmpTxt = mNombre;                                         }
					// Si la pista es menor que 10, le añado un cero delante
					else if (mPista < 10)	{ nTmpTxt = L"0" + std::to_wstring(mPista) + L" " + mNombre; }
					// Pistas con 2 o más caracteres
					else				    { nTmpTxt = std::to_wstring(mPista) + L" " + mNombre;        }
					switch (mTipoMedio) {
						case Tipo_Medio_Audio:		mTipoNodo = ArbolBD_TipoNodo_Cancion;			break;
						case Tipo_Medio_Video:		mTipoNodo = ArbolBD_TipoNodo_Video;				break;
						case Tipo_Medio_IpTv:		mTipoNodo = ArbolBD_TipoNodo_IpTv;				break;
						case Tipo_Medio_Lista:		mTipoNodo = ArbolBD_TipoNodo_ListaCanciones;	break;
					}
					AgregarBDNodo(mTipoNodo, static_cast<NodoBD *>(nNodo), nTmpTxt.c_str(), mHash, mId);
				}
			}
			if (SqlRet == SQLITE_BUSY) {
				VecesBusy++;
				if (VecesBusy == 100) break;
			}
		}

		sqlite3_finalize(SqlQuery);
	}

	Debug_Escribir_Varg(L"ArbolBD::ExplorarPath terminado en = %dMS\n", GetTickCount64() - Tick);

}


void ArbolBD::Evento_MouseSoltado(DEventoMouse &DatosMouse) {
/*	HWND GC = GetCapture();
	if (GetCapture() == _hWnd) {
		RECT RC;
		GetClientRect(_hWnd, &RC);
		POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
		if (PtInRect(&RC, Pt) == FALSE) {
			ReleaseCapture();
			Visible(FALSE);
			return;
		}
	}*/

	
	NodoBD      *NodoRes = static_cast<NodoBD *>(_NodoResaltado);
	std::wstring EtiquetaFiltrada;
	EtiquetaBD  *Etiqueta = NULL;
	BDMedio      Medio;

	if (NodoRes != NULL) RaveBD::FiltroNombre(NodoRes->Texto, EtiquetaFiltrada);


	if (DatosMouse.Boton == 1) {
		BOOL nActivar = FALSE, nActivar2 = FALSE, nActivar3 = FALSE;
		// Compruebo si existe el nodo resaltado
		if (_NodoResaltado != NULL) {	
			// Si el nodo resaltado no es una raíz
			if (MedioResaltado()->TipoNodo != ArbolBD_TipoNodo_Raiz) { nActivar = TRUE; nActivar2 = TRUE; }
			// Miro el tipo de nodo para obtener su nota
			switch (NodoRes->TipoNodo) {
				case ArbolBD_TipoNodo_Directorio:
				case ArbolBD_TipoNodo_Genero:
				case ArbolBD_TipoNodo_Grupo:
				case ArbolBD_TipoNodo_Disco:
					// Hay que buscar la etiqueta que corresponde con el nodo
					Etiqueta = App.BD.ObtenerEtiqueta(EtiquetaFiltrada);
					if (Etiqueta != NULL) {
						App.BD.CalcularDatosEtiqueta(Etiqueta);
						App.VentanaRave.Menu_ArbolBD.Menu(4)->BarraValor(Etiqueta->Nota);
					}
					break;
				case ArbolBD_TipoNodo_Cancion:
				case ArbolBD_TipoNodo_Video:
					// Es un medio, hay que obtener su nota
					if (App.BD.ObtenerMedio(NodoRes->Hash, Medio) == TRUE) {
						App.VentanaRave.Menu_ArbolBD.Menu(4)->BarraValor(Medio.Nota);
					}

					BDMedio TmpMedio;
					App.BD.ObtenerMedio(MedioMarcado()->Hash, TmpMedio);

					DWL::DMenuEx* TmpMenu = NULL;
					App.VentanaRave.Menu_ArbolBD.Menu(3)->EliminarTodosLosMenus();
					if (TotalNodosSeleccionados() == 1) {
						nActivar3 = TRUE;
						for (size_t i = 0; i < TmpMedio.Momentos.size(); i++) {
							TmpMenu = NULL;
							TmpMenu = App.VentanaRave.Menu_ArbolBD.Menu(3)->AgregarMenu(ID_MENULISTA_MOMENTOS_MOMENTO + i, TmpMedio.Momentos[i]->Nombre, (TmpMedio.Momentos[i]->Excluir) ? IDI_MOMENTO_EXCLUIR : IDI_MOMENTO);
							if (TmpMenu) TmpMenu->Parametro = TmpMedio.Hash;
						}
					}
					break;
			}
		}
		// Si hay mas de un nodo seleccionado desactivo los menus : Abrir carpeta, Nota, y Propiedades
		if (TotalNodosSeleccionados() > 1) nActivar2 = FALSE;

		App.VentanaRave.Menu_ArbolBD.Menu(0)->Activado(nActivar); // Agregar a lista
		App.VentanaRave.Menu_ArbolBD.Menu(1)->Activado(nActivar); // Agregar a nueva lista
		
		App.VentanaRave.Menu_ArbolBD.Menu(2)->Activado(nActivar2); // Abrir carpeta
		App.VentanaRave.Menu_ArbolBD.Menu(3)->Activado(nActivar3); // Momentos
		App.VentanaRave.Menu_ArbolBD.Menu(4)->Activado(nActivar2); // Nota
		App.VentanaRave.Menu_ArbolBD.Menu(5)->Activado(nActivar2); // Propiedades

		App.VentanaRave.Menu_ArbolBD.Mostrar(&App.VentanaRave);
	}
	// Muestro el nodo resaltado con un ToolTip
	else if (DatosMouse.Boton == 2 && _NodoResaltado != NULL) {		
		switch (NodoRes->TipoNodo) {
			case ArbolBD_TipoNodo_Directorio :
			case ArbolBD_TipoNodo_Genero	 :
			case ArbolBD_TipoNodo_Grupo		 :
			case ArbolBD_TipoNodo_Disco		 :
				Etiqueta = App.BD.ObtenerEtiqueta(EtiquetaFiltrada);
				if (Etiqueta != NULL) {
					App.BD.CalcularDatosEtiqueta(Etiqueta);

					if (_ToolTipE.Etiqueta != Etiqueta || _ToolTipE.Visible() == FALSE) {
						_ToolTipM.Ocultar(TRUE);
						_ToolTipE.Ocultar(TRUE);
						_ToolTipE.MostrarFijo(Etiqueta);
					}
				}
				break;
			case ArbolBD_TipoNodo_Cancion	 :
			case ArbolBD_TipoNodo_Video		 :
				if (App.BD.ObtenerMedio(NodoRes->Hash, Medio) == TRUE) {
					if (Medio != _ToolTipM.Medio || _ToolTipM.Visible() == FALSE) {
						Debug_Escribir(L"Destruir2\n");
						_ToolTipM.Ocultar(TRUE);
						_ToolTipE.Ocultar(TRUE);
						_ToolTipM.MostrarFijo(Medio);
					}
				}				
				break;
		}


	}
	
	// Oculto el tooltip por que no hay Nodo resaltado, o por que se ha presionado un boton que no es el del medio
	if (DatosMouse.Boton != 2 || _NodoResaltado == NULL) {
		_ToolTipM.Ocultar(TRUE);
		_ToolTipE.Ocultar(TRUE);
	}



};

void ArbolBD::Evento_MouseMovimiento(DWL::DEventoMouse &DatosMouse) {
	if (_NodoResaltado == NULL) {
		_ToolTipM.Ocultar();
		_ToolTipE.Ocultar();
	}
	else {
		if (_NodoUResaltado != _NodoResaltado) {
			_ToolTipM.Ocultar();
			_ToolTipE.Ocultar();
		}
		else {
			if (_ToolTipM.Ocultando() == FALSE)	_ToolTipM.Mover();
			if (_ToolTipE.Ocultando() == FALSE) _ToolTipE.Mover();
		}
	}
}


void ArbolBD::ObtenerPathNodo(NodoBD *pNodo, std::wstring &OUT_Path) {
	NodoBD *Tmp = pNodo;

	// Si es un medio
	if (Tmp->TipoNodo == ArbolBD_TipoNodo_Cancion || Tmp->TipoNodo == ArbolBD_TipoNodo_Video) {
		BDMedio Medio;
		App.BD.ObtenerMedio(Tmp->Hash, Medio);
		OUT_Path = Medio.Path;
	}
	else {
		while (Tmp != NULL) {
			if (Tmp->TipoNodo != ArbolBD_TipoNodo_Raiz) { OUT_Path = Tmp->Texto + L"\\" + OUT_Path;					}
			else										{ OUT_Path = Tmp->Texto + OUT_Path;					break;	}
			Tmp = static_cast<NodoBD *>(Tmp->Padre());
		}
	}

}

// Establece la opacidad de la ventana (0 transparente, 255 solido)
void ArbolBD::Opacidad(const BYTE nNivel) {
	if (App.VentanaRave.PantallaCompleta() == TRUE) {
		SetWindowLongPtr(_hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		SetLayeredWindowAttributes(_hWnd, 0, nNivel, LWA_ALPHA);
	}
};

const BYTE ArbolBD::Opacidad(void) {
	BYTE Ret;
	DWORD Params = LWA_ALPHA;
	GetLayeredWindowAttributes(_hWnd, NULL, &Ret, &Params);
	return Ret;
}

/*
void ArbolBD::Evento_FocoPerdido(HWND hWndNuevoFoco) {
	if (App.VentanaRave.PantallaCompleta() == TRUE) Visible(FALSE);
}*/