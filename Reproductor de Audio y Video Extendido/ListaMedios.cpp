#include "stdafx.h"
#include "ListaMedios.h"
#include "RAVE_Iconos.h"
#include <DArchivoBinario.h>
#include <DArchivoInternet.h>
#include <DStringUtils.h>

ListaMedios::ListaMedios(void) : MedioActual(NULL), Errores(0) {
}


ListaMedios::~ListaMedios(void) {
}


const LONG_PTR ListaMedios::PosMedio(ItemMedio *pMedio) {
	if (pMedio == NULL) return -1;
	for (LONG_PTR i = 0; i < static_cast<LONG_PTR>(_Items.size()); i++) {
		if (Medio(i) == pMedio) return i;
	}
	return -1;
}

// Obtiene el medio anterior (Si no hay medio anterior y SituarAlFinal es TRUE devuelve el ultimo Medio, Si SituarAlFinal es FALSE devolverá NULL)
ItemMedio *ListaMedios::MedioAnterior(ItemMedio *nMedio, const BOOL SituarAlFinal) {
	ItemMedio *Ret = NULL;
	// Si no hay items, asigno el medio actual a NULL y salgo
	if (_Items.size() != 0) {
		// Si el medio actual es NULL o es el primer medio de la lista, lo asigno al final de la lista
		if (nMedio == NULL || nMedio == static_cast<ItemMedio *>(_Items[0])) {
			Ret = static_cast<ItemMedio *>(_Items[_Items.size() - 1]);
		}
		// No es el primero de la lista, retrocedo un medio
		else {
			LONG_PTR Pm = PosMedio(nMedio);
			if (Pm > 0 && SituarAlFinal == TRUE) {
				Ret = static_cast<ItemMedio *>(_Items[Pm - 1]);
			}
		}
	}
	return Ret;
}

ItemMedio *ListaMedios::MedioSiguiente(ItemMedio *nMedio) {
	ItemMedio *Ret = NULL;
	// Si no hay items, asigno el medio actual a NULL y salgo
	if (_Items.size() != 0) {
		// Si el medio actual es el ultimo de la lista
		if (nMedio == NULL || nMedio == static_cast<ItemMedio *>(_Items[_Items.size() - 1])) {
			Ret = static_cast<ItemMedio *>(_Items[0]);
		}
		// No es el ultimo de la lista
		else {
			LONG_PTR Pm = PosMedio(nMedio);
			if (Pm > -1 && Pm < static_cast<LONG_PTR>(_Items.size())) {
				Ret = static_cast<ItemMedio *>(_Items[Pm + 1]);
			}
		}
	}
	return Ret;
}


void ListaMedios::BorrarListaReproduccion(void) {
//	App.VLC.Stop();
	App.MP.CerrarMedio();

	EliminarTodosLosItems();
	MedioActual = NULL;

	Repintar();
}

// Función que parsea un M3u para leer sus datos, e introducir los medios que contiene en la lista
// El M3u puede ser de internet, en ese caso hay que leer el M3u con un thread y esperar su respuesta.
void ListaMedios::AgregarM3u(std::wstring &PathM3u) {
	// M3u de internet
	if (BDMedio::Ubicacion(PathM3u) == Ubicacion_Medio_Internet) {
		DWL::DArchivoInternet::Obtener(PathM3u,
			[=](DWL::DPeticion &P) {
				_ParsearM3u(P.Url(), P.Datos());
			},
			[=](const UINT Error) {
				Debug_Escribir_Varg(L"Error cargando el M3u : %d\n", Error);
			},
			[=](const float Valor) {
				Debug_Escribir(L"Valor\n");
			}
		);
	}
	// M3u local o de red
	else {
		DWL::DArchivoBinario Archivo(PathM3u, FALSE);
		if (Archivo.EstaAbierto() == TRUE) {
			std::streamsize Tam = Archivo.Longitud();
			char *Buffer = new char[Tam + 1];
			size_t Final = Archivo.Leer(Buffer, Tam);
			Buffer[Final] = '\0';
			_ParsearM3u(PathM3u, Buffer);
			delete[] Buffer;
			Archivo.Cerrar();
		}
	}
}


void ListaMedios::_ParsearM3u(std::wstring &PathM3u, const char *Datos) {
	// Miro si es un path local o de internet
	wchar_t			Delimitador = (BDMedio::Ubicacion(PathM3u) == Ubicacion_Medio_Internet) ? L'/' : L'\\';
	// Busco la ultima barra del path
	size_t			Barra = PathM3u.find_last_of(Delimitador);
	std::string		Path, PathArchivo;
	std::wstring	PathFinal;
	BDMedio         Medio;
//	UINT			TiempoEnSecs;
	// Convierto el path recortado a UTF8
	DWL::Strings::WideToAnsi(PathM3u.substr(0, Barra + 1).c_str(), Path);

	std::string DatosStr = Datos;
	// Elimino posibles caracteres de retorno extras (en sistemas unix el retorno de carro se define con el caracter \n, en windows se usa \n y \r
	DatosStr.erase(std::remove(DatosStr.begin(), DatosStr.end(), '\r'), DatosStr.end());

	// Separo los datos por lineas
	DWL::Strings::SplitA Lineas(DatosStr, '\n');

	// Analizo las lineas una por una
	for (size_t i = 0; i < Lineas.Total(); i++) {
		// Es un medio y el path mide mas de dos carácteres
		if (Lineas[i].find('#') == std::string::npos && Lineas[i].size() > 2) {
			// Miro si el path de la línea es relativo
			// Es un path absoluto
			if (Lineas[i][1] == ':' || Lineas[i][1] == '\\' || (Lineas[i][0] == 'h' && Lineas[i][1] == 't')) {
				PathArchivo = Lineas[i];
			}
			// Es un medio con path relativo
			else {
				PathArchivo = Path + Lineas[i];
			}

			DWL::Strings::AnsiToWide(PathArchivo.c_str(), PathFinal);
			// pre-analizo el medio y lo inserto en la base de datos
			if (App.BD.AnalizarMedio(PathFinal, Medio) == 2) {
				// El medio ya existia, lo recargo
				App.BD.ObtenerMedio(PathFinal, Medio);
			}

			// Agrego el medio a la lista
			AgregarMedio(&Medio);
		}
		// Directiva de M3U extendido https://es.wikipedia.org/wiki/M3U
		else {
			// De momento omitiremos el tema de las directivas...
		}
	}
}

ItemMedio *ListaMedios::AgregarMedio(BDMedio *nMedio) {
	
	// Busco si existe el hash para no agregar 2 medios iguales a la lista
	for (size_t i = 0; i < _Items.size(); i++) {
		if (static_cast<ItemMedio *>(_Items[i])->Hash == nMedio->Hash) {
			return Medio(i); // Devuelvo el medio existente
		}
	}

	Ubicacion_Medio Ubicacion = BDMedio::Ubicacion(nMedio->Path);

	if (Ubicacion != Ubicacion_Medio_Internet) {
		// El medio no existe
		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(nMedio->Path.c_str())) {
			Debug_Escribir_Varg(L"ListaMedios::AgregarMedio El medio path : '%s', id : '%d' no existe!\n", nMedio->Path.c_str(), nMedio->Hash);
			std::wstring Err = L"Error! el archivo '" + nMedio->Path + L"' no existe!";
			App.MostrarToolTipPlayerError(Err);
			return NULL;
		}
	}

	// Es un M3u local o de red, lo parseo y devuelvo NULL
	if (nMedio->TipoMedio == Tipo_Medio_Lista && Ubicacion != Ubicacion_Medio_Internet) {
		AgregarM3u(nMedio->Path);
		return NULL;
	}
	

	// Miro el icono que necesita el medio
	int nIcono = RAVE_Iconos::RAVE_Icono_Cancion;
	switch (nMedio->TipoMedio) {
		case Tipo_Medio_Audio: nIcono = RAVE_Iconos::RAVE_Icono_Cancion;	break;
		case Tipo_Medio_Video: nIcono = RAVE_Iconos::RAVE_Icono_Video;		break;
		case Tipo_Medio_IpTv : nIcono = RAVE_Iconos::RAVE_Icono_IpTv;		break;
	}

	// Paso la pista a string formateada a 2 digitos
	std::wstring Pista;
	// Si la pista no es 0, creo el string para la pista
	if (nMedio->Pista() != 0) {
		Pista = std::to_wstring(nMedio->Pista());
		// Si la pista solo tiene un carácter, le añado un cero delante
		if (Pista.size() == 1) Pista = L"0" + Pista;
	}
	// Tiempo vacio por defecto
	std::wstring StrTiempo = L"--:--";
	// Si no es una IP TV, paso el tiempo a string formateado en mm:ss
	if (nMedio->TipoMedio != Tipo_Medio_IpTv)
		App.MP.TiempoStr(nMedio->Tiempo, StrTiempo);

	// Agrego el item
	ItemMedio* TmpMedio = AgregarItem<ItemMedio>(nIcono, DLISTAEX_POSICION_FIN, { Pista, nMedio->Nombre(), StrTiempo });
	// Agrego el item también en el vector MediosOrdenados (por si el shufle está activado)
	_MediosOrdenados.push_back(TmpMedio);
	// Asigno el hash y la Id al item
	TmpMedio->Hash	= nMedio->Hash;
	TmpMedio->Id	= nMedio->Id;

	return TmpMedio;
}

ItemMedio *ListaMedios::BuscarHash(sqlite3_int64 bHash) {
	for (size_t i = 0; i < _Items.size(); i++) {
		if (Medio(i)->Hash == bHash) return Medio(i);
	}
	return NULL;
}

// Reproduce un medio previamente añadido a la lista
void ListaMedios::ReproducirMedio(BDMedio &nMedio, LONG PosMomento) {
	// Busco el nuevo medio actual
	for (size_t i = 0; i < _Items.size(); i++) {
		if (Medio(i)->Hash == nMedio.Hash) {
			MedioActual = Medio(i);
			break;
		}
	}

	App.MP.Stop();
	BDMedio NCan;
	App.BD.ObtenerMedio(MedioActual->Hash, NCan);
	NCan.PosMomento = PosMomento;
	BDMedio NCanS;
	ItemMedio* IMS = MedioSiguiente(MedioActual);
	if (IMS != NULL) {
		App.BD.ObtenerMedio(MedioSiguiente(MedioActual)->Hash, NCanS);
		if (App.MP.AbrirMedio(NCan, &NCanS) == FALSE) Errores++;
	}
	else {
		if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Errores++;
	}

	App.MP.Play(TRUE);
}

// Reproduce el medio especificado con el ratón
void ListaMedios::Evento_MouseDobleClick(DWL::DEventoMouse &EventoMouse) {
	if (EventoMouse.Boton == 0) {
		if (MedioResaltado() != NULL) {
			MedioActual = MedioResaltado();
			// Hay que parar la canción actual para que no se quede reproduciendo en el medio anterior
			App.MP.Stop();

			BDMedio NCan;
			App.BD.ObtenerMedio(MedioActual->Hash, NCan);
			BDMedio NCanS;
			ItemMedio *IMS = MedioSiguiente(MedioActual);
			if (IMS != NULL) {
				App.BD.ObtenerMedio(MedioSiguiente(MedioActual)->Hash, NCanS);
				if (App.MP.AbrirMedio(NCan, &NCanS) == FALSE) Errores++;
			}
			else {
				if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Errores++;
			}

//			if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
			App.MP.Play();
		}
	}
}

void ListaMedios::Evento_MouseSoltado(DWL::DEventoMouse& DatosMouse) {

/*	if (GetCapture() == _hWnd) {
		RECT RC;
		GetClientRect(_hWnd, &RC);
		POINT Pt = { DatosMouse.X(), DatosMouse.Y() };
		if (PtInRect(&RC, Pt) == FALSE) {
			ReleaseCapture();
			Visible(FALSE);
			return;
		}
	}*/

	// Boton del medio
	if (DatosMouse.Boton == 2) {		
		if (_ItemResaltado != -1) {
			BDMedio nMedio;
			App.BD.ObtenerMedio(Medio(_ItemResaltado)->Hash, nMedio);
			if (_ToolTip.Medio != nMedio ||_ToolTip.Visible() == FALSE) {
				_ToolTip.Ocultar(TRUE);
				_ToolTip.MostrarFijo(nMedio);
			}
		}
	}

	// Oculto el tooltip por que no hay Nodo resaltado, o por que se ha presionado un boton que no es el del medio
	if (DatosMouse.Boton != 2 || _ItemResaltado == -1) {
		_ToolTip.Ocultar(TRUE);
	}

	// Mostrar el menú
	if (DatosMouse.Boton == 1) {
		BOOL nActivar				= (_ItemResaltado == -1) ? FALSE : TRUE;
		BOOL nBuscarBDActivado		= nActivar;
		BOOL nMomentosActivado		= nActivar;
		BOOL nPropiedadesActivado	= nActivar;

		// Si el item marcado corresponde a un medio
		if (_ItemMarcado > -1 && _ItemMarcado < static_cast<LONG_PTR>(_Items.size())) {			
			BDMedio TmpMedio;
			App.BD.ObtenerMedio(Medio(_ItemMarcado)->Hash, TmpMedio);

			// Miro si el medio tiene una raíz (si no tiene raíz no saldrá en la base de datos)
			if (App.BD.BuscarRaiz(TmpMedio.Path) == NULL)
				nBuscarBDActivado = FALSE;

			// Asigno la nota al menu/barra de la nota
			App.VentanaRave.Menu_Lista.Menu(4)->BarraValor(TmpMedio.Nota);

			// Agrego los menus para los momentos
			DWL::DMenuEx *TmpMenu = NULL;
			App.VentanaRave.Menu_Lista.Menu(2)->EliminarTodosLosMenus();
			if (TotalItemsSeleccionados() == 1) {
				nMomentosActivado = TRUE;
				for (size_t i = 0; i < TmpMedio.Momentos.size(); i++) {
					TmpMenu = NULL;
					TmpMenu = App.VentanaRave.Menu_Lista.Menu(2)->AgregarMenu(ID_MENULISTA_MOMENTOS_MOMENTO + i, TmpMedio.Momentos[i]->Nombre, (TmpMedio.Momentos[i]->Excluir) ? IDI_MOMENTO_EXCLUIR : IDI_MOMENTO);
					if (TmpMenu) TmpMenu->Parametro = TmpMedio.Hash;
				}
			}
			else {
				nMomentosActivado	 = FALSE;
				nBuscarBDActivado	 = FALSE;
				nPropiedadesActivado = FALSE;
			}
		}

		/*
		if (_ItemMarcado > -1 && _ItemMarcado < static_cast<LONG_PTR>(_Items.size())) {
			BDMedio TmpMedio;
			App.BD.ObtenerMedio(Medio(_ItemMarcado)->Hash, TmpMedio);

			if (App.BD.BuscarRaiz(TmpMedio.Path) == NULL) 
				nBuscarBDActivado = FALSE;
		}

//		
//		if (App.BD.BuscarRaiz(Medio(_ItemMarcado)->) == NULL) nBuscarBDActivado = FALSE;
				
		// Asigno la nota al menu/barra de la nota
		if (_ItemResaltado != -1) {
			BDMedio mItem;
			App.BD.ObtenerMedio(Medio(_ItemResaltado)->Hash, mItem);
			App.VentanaRave.Menu_Lista.Menu(3)->BarraValor(mItem.Nota);
		}*/

		// Activo / desactivo los menus
		App.VentanaRave.Menu_Lista.Menu(0)->Activado(nActivar);
		App.VentanaRave.Menu_Lista.Menu(1)->Activado(nActivar);
		App.VentanaRave.Menu_Lista.Menu(2)->Activado(nMomentosActivado);
		App.VentanaRave.Menu_Lista.Menu(3)->Activado(nBuscarBDActivado);
		App.VentanaRave.Menu_Lista.Menu(4)->Activado(nActivar);
		App.VentanaRave.Menu_Lista.Menu(5)->Activado(nPropiedadesActivado);
/*		for (size_t i = 0; i < App.VentanaRave.Menu_Lista.TotalMenus(); i++) {
			if		(i == 2)	App.VentanaRave.Menu_Lista.Menu(i)->Activado(nMomentosActivado);
			else if (i == 3)	App.VentanaRave.Menu_Lista.Menu(i)->Activado(nBuscarBDActivado);
			else if (i == 5)	App.VentanaRave.Menu_Lista.Menu(i)->Activado(nPropiedadesActivado);
			else				App.VentanaRave.Menu_Lista.Menu(i)->Activado(nActivar);
		}*/
		App.VentanaRave.Menu_Lista.Mostrar(&App.VentanaRave);
	}
}

void ListaMedios::Evento_MouseMovimiento(DWL::DEventoMouse &DatosMouse) {
	if (_ItemResaltado == -1) {
		_ToolTip.Ocultar();
	}
	else {
		if (_ItemResaltado != _ItemUResaltado) {
			_ToolTip.Ocultar();
		}
		else {
			if (_ToolTip.Ocultando() == FALSE) {
				_ToolTip.Mover();
			}
		}
	}
}


void ListaMedios::Evento_MouseSaliendo(void) {
	_ToolTip.Ocultar();
}

void ListaMedios::Evento_TeclaSoltada(DEventoTeclado &DatosTeclado) {
	if (DatosTeclado.TeclaVirtual() == VK_DELETE) {
		App.VentanaRave.Lista_EliminarSeleccionados();
	}
	else if (DatosTeclado.TeclaVirtual() == VK_RETURN) {
		if (MedioMarcado() == NULL) return;

		MedioActual = MedioMarcado();
		// Hay que parar la canción actual para que no se quede reproduciendo en el medio anterior
		App.MP.Stop();

		BDMedio NCan;
		App.BD.ObtenerMedio(MedioActual->Hash, NCan);
		BDMedio NCanS;
		ItemMedio* IMS = MedioSiguiente(MedioActual);
		if (IMS != NULL) {
			App.BD.ObtenerMedio(MedioSiguiente(MedioActual)->Hash, NCanS);
			if (App.MP.AbrirMedio(NCan, &NCanS) == FALSE) Errores++;
		}
		else {
			if (App.MP.AbrirMedio(NCan, NULL) == FALSE) Errores++;
		}
		//			if (App.VLC.AbrirMedio(NCan) == FALSE) Errores++;
		App.MP.Play();
	}
}

// 	Función que mezcla las canciones de la lista (TRUE), o restaura el orden original en que se añadieron (FALSE)
const BOOL ListaMedios::Mezclar(const BOOL nMezclar) {
	size_t	i	= 0;
	BOOL	Ret = FALSE;
	// Mezclar
	if (nMezclar == TRUE) {
		// Guardo el orden original en el vector MediosOrdenados
		_MediosOrdenados.clear();
		for (i = 0; i < _Items.size(); i++) {	_MediosOrdenados.push_back(Medio(i));	}
		// Limpio el vector de la lista para empezar a meclar sus items
		for (i = 0; i < _Items.size(); i++) {	_Items[i] = NULL;						}
		// Mezclo los items
		size_t R = 0;
		for (i = 0; i < _Items.size(); i++) {
			R = App.Rand<size_t>(_Items.size(), 0);
			// Si el item en la pos R no es NULL pasamos al siguiente
			while (_Items[R] != NULL) {
				// Si R llega al tope de items, la reasignamos al principio
				if (++R == _Items.size()) R = 0;
			}
			_Items[R] = _MediosOrdenados[i];
		}
		// Guardo la posición del medio actual en la lista ordenada
//		MedioActualOrdenado = MedioActual;
		// Busco la posición del medio actual en la lista mezclada
/*		for (i = 0; i < _Items.size(); i++) {
			if (_Items[i] == _MediosOrdenados[MedioActual]) {
				MedioActual = static_cast<LONG_PTR>(i);
				MostrarItem(i);
				break;
			}
		}*/
		Ret = TRUE;
	}
	// Restaurar medios ordenados
	else {
		// Busco la posición del medio actual en la lista original
/*		for (i = 0; i < _Items.size(); i++) {
			if (_Items[MedioActual] == _MediosOrdenados[i]) {
				MedioActual = static_cast<LONG_PTR>(i);
				MostrarItem(i);
				break;
			}
		}*/
		// Restauro los items desde la lista original
		for (i = 0; i < _Items.size(); i++) {
			_Items[i] = _MediosOrdenados[i];
		}
	}

	// Repinto el control y devuelvo si la lista está mezclada o no
	Repintar();
	return Ret;
}


// Establece la opacidad de la ventana (0 transparente, 255 solido)
void ListaMedios::Opacidad(const BYTE nNivel) {
	if (App.VentanaRave.PantallaCompleta() == TRUE) {
		SetWindowLongPtr(_hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
		SetLayeredWindowAttributes(_hWnd, 0, nNivel, LWA_ALPHA);
	}
};

const BYTE ListaMedios::Opacidad(void) {
	BYTE Ret;
	DWORD Params = LWA_ALPHA;
	GetLayeredWindowAttributes(_hWnd, NULL, &Ret, &Params);
	return Ret;
}


/*
void ListaMedios::Evento_FocoPerdido(HWND hWndNuevoFoco) {
	if (App.VentanaRave.PantallaCompleta() == TRUE) Visible(FALSE);
}*/