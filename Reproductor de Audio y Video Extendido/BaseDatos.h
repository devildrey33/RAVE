#pragma once

//#include "TMedio.h"
#include "ExtensionesValidas.h"
#include "DUnidadesDisco.h"
#include "ArbolBD.h"
#include "DhWnd.h"
//#include "sqlite3.h"

#include "TablaRaiz.h"
#include "TablaMedios.h"
#include "TablaOpciones.h"


#define WM_AGREGARNODO		WM_USER + 1000

//#define Debug_EscribirTBA()	Debug_Escribir
#define Debug_EscribirTBA()	


class ThreadBuscarArchivos {
 public:
												ThreadBuscarArchivos() : _Cancelar(FALSE), _Thread(NULL), _Mutex(NULL) { }
								               ~ThreadBuscarArchivos() { }
   const BOOL					            	Iniciar(HWND nhWndDest);
   static unsigned long		                   _ThreadBusqueda(void *pThis);
//   std::vector<TMedio *>			Medios;
//   std::vector<TRaiz *>				Raiz;
   HWND											hWndDest;
   HANDLE						               _Thread;
protected:
   const BOOL					               _AnalizarNombre(std::wstring &Analisis, std::wstring &nNombre, UINT &nPista);
   const BOOL                                  _EsNumero(const TCHAR Caracter);
   void							               _Filtro(std::wstring &iTexto, std::wstring &oTexto);
   const size_t                                _AnalizarMedio(const TCHAR *nPath, CeRaiz *Raiz, DWORD Longitud);
   const UINT					               _EscanearDirectorio(const wchar_t *nPath, CeRaiz *Raiz);
   const BOOL					               _EsNombreValido(const TCHAR *nNombre);
   HANDLE		                               _Mutex;
   BOOL										   _Cancelar;
   std::vector<size_t>                         _ListaHash;
   static sqlite3                             *_BD;

};


class BaseDatos : public DWL::DVentana {
  public:
									BaseDatos();
	                               ~BaseDatos();

	const BOOL						IniciarBD(void);
	const BOOL						CrearBD(void);

	void							TerminarBD(void);


	void							ActualizarArbol(void);

	const BOOL						Arbol_AgregarCancion(const size_t Hash);
	const BOOL						Arbol_AgregarDir(std::wstring *Path);
	const BOOL						Arbol_AgregarRaiz(std::wstring *Path);

									// Gestor de mensajes para recibir datos desde el thread de buscar
	LRESULT CALLBACK				GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);

	inline sqlite3                 *operator()(void) { return _BD; }

	TablaRaiz                       Tabla_Raiz;
	TablaMedios                     Tabla_Medios;
	TablaOpciones					Tabla_Opciones;
  protected:
	sqlite3                       *_BD;
	ThreadBuscarArchivos		   _BuscarArchivos;
	void						   _FiltroPath(std::wstring &iTexto, std::wstring &oTexto);

};


#define WM_TBA_AGREGARDIR			WM_USER + 2000
#define WM_TBA_AGREGARRAIZ			WM_USER + 2001
#define WM_TBA_AGREGARAUDIO			WM_USER + 2002
#define WM_TBA_TERMINADO			WM_USER + 2003
