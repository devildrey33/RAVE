#include "stdafx.h"
#include "ArbolBD.h"
#include "RAVE_Iconos.h"
#include "DStringUtils.h"

ArbolBD::ArbolBD(void) {
}


ArbolBD::~ArbolBD(void) {
}


void ArbolBD::Evento_TeclaPresionada(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_TeclaPresionada(DatosTeclado);
}

void ArbolBD::Evento_TeclaSoltada(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_TeclaSoltada(DatosTeclado);
}

void ArbolBD::Evento_Tecla(DWL::DEventoTeclado &DatosTeclado) {
	App.VentanaRave.Evento_Tecla(DatosTeclado);
}

NodoBD *ArbolBD::BuscarHash(sqlite3_int64 bHash) {
	if (_Raiz.TotalHijos() == 0) return NULL;
	NodoBD *Tmp = static_cast<NodoBD *>(_Raiz.Hijo(0));
	while (Tmp != NULL) {
		if (Tmp->Hash == bHash) return Tmp;
		Tmp = static_cast<NodoBD *>(BuscarNodoSiguiente(Tmp, FALSE, NULL));		
	}
	return NULL;
}



NodoBD *ArbolBD::AgregarBDNodo(const ArbolBD_TipoNodo nTipoNodo, NodoBD *nPadre, const TCHAR *cTexto, const sqlite3_int64 nHash) {

	// Busco si existe el hash
	if ((nTipoNodo == ArbolBD_TipoNodo_Cancion || nTipoNodo == ArbolBD_TipoNodo_Video) && _Raiz.TotalHijos() != 0) {
		NodoBD *Tmp = static_cast<NodoBD *>(_Raiz.Hijo(0));
		while (Tmp != NULL) {
			if (Tmp->Hash == nHash) return Tmp;
			Tmp = static_cast<NodoBD *>(BuscarNodoSiguiente(Tmp, FALSE, NULL));
		}
	}

	int                       nIcono   = 0;
	DhWnd_Fuente             *nFuente  = &Fuente;
	DArbolEx_MostrarExpansor  nExpansor = DArbolEx_MostrarExpansor_Mostrar;
//	size_t			nPos = 0;
	switch (nTipoNodo) {
		case ArbolBD_TipoNodo_Raiz:			// Raíz añadida por el usuario
			nFuente = &Fuente21Negrita;
			nIcono = RAVE_Iconos::RAVE_Icono_Raiz;
			break;
		case ArbolBD_TipoNodo_Directorio:	// Directorio dentro de una raíz
			nIcono = RAVE_Iconos::RAVE_Icono_Directorio;
			break;
		default:							// Medio dentro de una raíz o directorio
			nIcono = (nTipoNodo == ArbolBD_TipoNodo_Cancion) ? RAVE_Iconos::RAVE_Icono_Cancion : RAVE_Iconos::RAVE_Icono_Video;
			nExpansor = DArbolEx_MostrarExpansor_Ocultar;
			break;
	}

	NodoBD *nNodo	= AgregarNodo<NodoBD>(cTexto, nPadre, nIcono, nFuente, DARBOLEX_POSICIONNODO_ORDENADO);
	nNodo->Hash		= nHash;
	nNodo->TipoNodo = nTipoNodo;
	nNodo->MostrarExpansor(nExpansor);
	return nNodo;
	
}




const BOOL ArbolBD::AgregarNodoALista(DArbolEx_Nodo *nNodo) {
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
		case RAVE_Icono_Video : // Video
			SqlStr = L"SELECT * FROM Medios WHERE Hash =\"" + std::to_wstring(Hash) + L"\"";
			//			SqlStr = L"SELECT * FROM Medios WHERE Path LIKE\"" + Path + L"\"";
			break;
		case RAVE_Icono_Raiz :		 // Raiz (simbolitza l'arrel afegida per l'usuario que conte varis medis repartits o no en directoris)
		case RAVE_Icono_Directorio : // Directorio
			SqlStr = L"SELECT * FROM Medios WHERE Path LIKE \"?" + Path.substr(1) + L"%\" COLLATE NOCASE";	// Path.substr(1) se salta la letra de la unidad y deja el path sin letra/unidad.
			break;																							// La letra de unidad se substituye normalmente por '?' ya que puede ser un medio extraible y no tiene por que estar siempre en la misma letra
	}

	sqlite3_stmt   *SqlQuery = NULL;
	int SqlRet = sqlite3_prepare16_v2(App.BD(), SqlStr.c_str(), -1, &SqlQuery, NULL);
	if (SqlRet) {
		const char *Error = sqlite3_errmsg(App.BD());
		return FALSE;
	}

	int VecesBusy = 0;
	while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR && SqlRet != SQLITE_MISUSE) {
		SqlRet = sqlite3_step(SqlQuery);
		if (SqlRet == SQLITE_ROW) {
			BDMedio *Medio = new BDMedio(SqlQuery, App.BD.Unidades);
			
			App.VentanaRave.Lista.AgregarMedio(Medio);
			_AgregarMedio(static_cast<NodoBD *>(nNodo), Medio);

			delete Medio;
		}
		if (SqlRet == SQLITE_BUSY) {
			VecesBusy++;
			if (VecesBusy == 100) break;
		}

	}
	sqlite3_finalize(SqlQuery);


	// Ejecuto el shufle si es necesario
	if (App.BD.Opciones_Shufle() == TRUE) {
		App.VentanaRave.Lista.Mezclar(TRUE);
		App.VentanaRave.Lista.MedioActual = 0;
	}
	App.VentanaRave.Lista.Repintar();

	return (SqlRet != SQLITE_BUSY);
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
				TmpPadre = AgregarBDNodo(ArbolBD_TipoNodo_Directorio, TmpPadre, Filtrado.c_str());
			}
			else { // Medio
				std::wstring nPista;
				nMedio->PistaStr(nPista);
				std::wstring nNombre = nPista + L" " + nMedio->Nombre();
				App.BD.FiltroNombre(nNombre, Filtrado);
				ArbolBD_TipoNodo Tipo = (nMedio->TipoMedio == Tipo_Medio_Audio) ? ArbolBD_TipoNodo_Cancion : ArbolBD_TipoNodo_Video;
				Tmp = AgregarBDNodo(Tipo, TmpPadre, Filtrado.c_str(), nMedio->Hash);
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
		ExplorarPath(nNodo);
	}
}


void ArbolBD::ObtenerPath(DArbolEx_Nodo *nNodo, std::wstring &rPath) {
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

void ArbolBD::ExplorarPath(DWL::DArbolEx_Nodo *nNodo) {
	if (nNodo == NULL) return;
	DWORD Tick = GetTickCount();
	Debug_Escribir_Varg(L"ArbolBD::ExplorarPath  Nodo = '%s'\n", nNodo->Texto.c_str());

	WIN32_FIND_DATA		FindInfoPoint;
	HANDLE				hFind = NULL;
	std::wstring        nPath, nNombre, nTmpTxt, nTmpTxt2;
	ObtenerPath(nNodo, nPath);
	nTmpTxt = nPath;
	if (nTmpTxt[nTmpTxt.size() - 1] != TEXT('\\'))	{ nTmpTxt += TEXT("\\*.*");  nTmpTxt += L'\\';	}
	else											{ nTmpTxt += TEXT("*.*");	}

	BOOL AgregarMedios = FALSE;

	hFind = FindFirstFile(nTmpTxt.c_str(), &FindInfoPoint);
	size_t nEntradas = 0;
	while (FindNextFile(hFind, &FindInfoPoint) != 0) {
		nNombre = FindInfoPoint.cFileName;

		if (FindInfoPoint.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { // Directorio
			if (nNombre != L"." && nNombre != L"..") {
				RaveBD::FiltroPath(nNombre, nTmpTxt);
				AgregarBDNodo(ArbolBD_TipoNodo_Directorio, static_cast<NodoBD *>(nNodo), nTmpTxt.c_str());
				nEntradas++;
			}
		}
		else { // Archivo (Si hay archivos, hay que hacer una consulta que devuelva todos los archivos que tengan el path similar
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
		std::wstring    SqlStr = L"SELECT * FROM Medios WHERE Path LIKE \"%" + nPath.substr(1) + L"%\" COLLATE NOCASE";
		int				SqlRet = 0;
		sqlite3_stmt   *SqlQuery = NULL;
		
		SqlRet = sqlite3_prepare16_v2(App.BD(), SqlStr.c_str(), -1, &SqlQuery, NULL);
		if (SqlRet) return; // Error

		size_t BarrasPath = DWL::Strings::ContarCaracter(nPath, L'\\'), BarrasMedio = 0; // Cuento las antibarras del path
		int VecesBusy = 0;
		while (SqlRet != SQLITE_DONE && SqlRet != SQLITE_ERROR) {
			SqlRet = sqlite3_step(SqlQuery);
			if (SqlRet == SQLITE_ROW) {
				size_t			 mHash		= static_cast<size_t>(sqlite3_column_int64(SqlQuery, 1));
				std::wstring	 mPath		= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 2));
				std::wstring     mNombre	= reinterpret_cast<const wchar_t *>(sqlite3_column_text16(SqlQuery, 3));
				Tipo_Medio       mTipoMedio	= static_cast<Tipo_Medio>(sqlite3_column_int(SqlQuery, 4));
				UINT             mPista		= static_cast<UINT>(sqlite3_column_int(SqlQuery, 13));
				ArbolBD_TipoNodo mTipoNodo = ArbolBD_TipoNodo_Indefinido;

				BarrasMedio = DWL::Strings::ContarCaracter(mPath, L'\\');

				if (BarrasPath == BarrasMedio) {
					if (mPista < 10) { nTmpTxt = L"0" + std::to_wstring(mPista) + L" " + mNombre; }
					else { nTmpTxt = std::to_wstring(mPista) + L" " + mNombre; }
					switch (mTipoMedio) {
						case Tipo_Medio_Audio:		mTipoNodo = ArbolBD_TipoNodo_Cancion;	break;
						case Tipo_Medio_Video:		mTipoNodo = ArbolBD_TipoNodo_Video;		break;
					}
					AgregarBDNodo(mTipoNodo, static_cast<NodoBD *>(nNodo), nTmpTxt.c_str(), mHash);
				}
			}
			if (SqlRet == SQLITE_BUSY) {
				VecesBusy++;
				if (VecesBusy == 100) break;
			}
		}

		sqlite3_finalize(SqlQuery);
	}

	Debug_Escribir_Varg(L"ArbolBD::ExplorarPath terminado en = %dMS\n", GetTickCount() - Tick);

}


void ArbolBD::Evento_MouseSoltado(DEventoMouse &DatosMouse) {
	if (DatosMouse.Boton == 1) {
		if (_NodoResaltado == NULL) {	// Anulo los menuitems agregar... si no hay un nodo marcado
			App.VentanaRave.Menu_ArbolBD.Menu(0)->Activado(FALSE); // Agregar a lista
			App.VentanaRave.Menu_ArbolBD.Menu(1)->Activado(FALSE); // Agregar a nueva lista
		}
		else {							// activo los menuitems agregar si hay un nodo marcado
			App.VentanaRave.Menu_ArbolBD.Menu(0)->Activado(TRUE); // Agregar a lista
			App.VentanaRave.Menu_ArbolBD.Menu(1)->Activado(TRUE); // Agregar a nueva lista
		}

		DMenuEx *Menu = App.VentanaRave.Menu_ArbolBD.MostrarModal(&App.VentanaRave);
		if (Menu != NULL) {
			switch (Menu->ID()) {
				case ID_MENUBD_AGREGARANUEVALISTA:
					App.VentanaRave.Lista.BorrarListaReproduccion();
					AgregarNodoALista(_NodoMarcado);
					App.VentanaRave.Lista_Play();
					App.MostrarToolTip(App.VentanaRave, L"\"" + _NodoMarcado->Texto + L"\" añadido a una nueva lista.");
					break;
				case ID_MENUBD_AGREGARALISTA:
					AgregarNodoALista(_NodoMarcado);
					App.MostrarToolTip(App.VentanaRave, L"\"" + _NodoMarcado->Texto + L"\" añadido a la lista.");
					if (App.VLC.ComprobarEstado() != EnPlay) App.VentanaRave.Lista_Play();
					break;
				case ID_MENUBD_ACTUALIZAR:
					App.VentanaRave.ActualizarArbol();					
					break;
				case ID_MENUBD_ANALIZAR :
					App.VentanaRave.AnalizarBD();					
					break;
			}
		}
	}
	// Muestro el nodo resaltado con un ToolTip
	else if (DatosMouse.Boton == 2 && _NodoResaltado != NULL) {
		NodoBD *NodoRes = static_cast<NodoBD *>(_NodoResaltado);

		//App.BD.Consulta();
		std::wstring EtiquetaFiltrada;
		RaveBD::FiltroNombre(NodoRes->Texto, EtiquetaFiltrada);
		EtiquetaBD *Etiqueta = NULL;
		BDMedio     Medio;


		switch (NodoRes->TipoNodo) {
			case ArbolBD_TipoNodo_Directorio :
			case ArbolBD_TipoNodo_Genero	 :
			case ArbolBD_TipoNodo_Grupo		 :
			case ArbolBD_TipoNodo_Disco		 :
				Etiqueta = App.BD.ObtenerEtiqueta(EtiquetaFiltrada);
				if (Etiqueta != NULL) {
					Debug_Escribir(L"Destruir1\n");
					_ToolTip.Destruir();
					_ToolTip.Mostrar(*Etiqueta);
				}
				break;
			case ArbolBD_TipoNodo_Cancion	 :
			case ArbolBD_TipoNodo_Video		 :
				if (App.BD.ObtenerMedio(NodoRes->Hash, Medio) == TRUE) {
					Debug_Escribir(L"Destruir1\n");
					_ToolTip.Destruir();
					_ToolTip.Mostrar(Medio);
				}				
				break;
		}


	}
	
	// Oculto el tooltip por que no hay Nodo resaltado, o por que se ha presionado un boton que no es el del medio
	if (DatosMouse.Boton != 2 || _NodoResaltado == NULL) {
		_ToolTip.Destruir();
	}



};

void ArbolBD::Evento_MouseMovimiento(DWL::DEventoMouse &DatosMouse) {
	if (_NodoResaltado == NULL) {
		_ToolTip.Destruir();
	}
	else {
		if (_NodoUResaltado != _NodoResaltado) {
			_ToolTip.Destruir();
		}
		else {
			_ToolTip.Mover();
		}
	}
}
