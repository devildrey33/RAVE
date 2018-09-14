#ifndef DMENUEX_H
#define DMENUEX_H


namespace DWL {

	#define DMENUEX_MOSTRARDEBUG	FALSE

	#define DMENUEX_TAMICONO  16	// Tama�o de los iconos
	#define DMENUEX_MARGEN_X   6	// Margen horizontal para el texto
	#define DMENUEX_MARGEN_Y   4	// Margen vertical para el texto
    #define DMENUEX_BORDE      1    // Tama�o del borde

	enum DMenuEx_Tipo {
		DMenuEx_Tipo_Raiz      = 0,
		DMenuEx_Tipo_Texto     = 1,
		DMenuEx_Tipo_Separador = 2
	};

	enum DMenuEx_Estado {
		DMenuEx_Estado_Normal		= 0,
		DMenuEx_Estado_Resaltado	= 1,
		DMenuEx_Estado_Presionado	= 2,
		DMenuEx_Estado_Desactivado  = 3
	};

	#define DMENUEX_TAMICONO				16 // Tama�o del icono

	#define WM_ESMENUEX						WM_USER + 400

	class DMenuEx : protected DWL::DVentana {
  	  public:
								DMenuEx(void);
							   ~DMenuEx(void);
							    // Agrega un men� con texto y icono en la posicion especificada (-1 es al final)
		DMenuEx                *AgregarMenu(const UINT nID, const wchar_t *nTexto, const int nIconoRecursos = NULL, const int Posicion = -1, const BOOL nActivado = TRUE);
		DMenuEx				   *AgregarSeparador(const int uID = 0, const int Posicion = -1);

								// Funci�n que muestra el Men� en el bucle principal de la aplicaci�n y espera su respuesta en un WM_COMMAND (NO DEVUELVE EL MEN� PRESIONADO)
		void					Mostrar(DhWnd *nPadre, const int PosX, const int PosY);
		void					Mostrar(DhWnd *nPadre);
								// Funci�n que muestra el Men� y espera a que se elija una opci�n para devolver-la
								//	NOTA : No desactiva la ventana padre
		DMenuEx			       *MostrarModal(DhWnd *nPadre, const int PosX, const int PosY);
		DMenuEx			       *MostrarModal(DhWnd *nPadre);

								// Asigna un icono a este men� 
		void                    Icono(const int nIconoRecursos);
		inline void             Icono(DListaIconos_Icono *nIcono)	{ _Icono = nIcono; }

								// Funciones para navegar entre menus
		inline DMenuEx         *Menu(const size_t Pos)				{ return _Menus[Pos]; }
		inline const size_t		TotalMenus(void)					{ return _Menus.size(); }
		inline DMenuEx         *MenuPadre(void)						{ return _Padre; }

		inline const UINT       ID(void)							{ return _ID; }

								// Funci�n para obtener el texto del men�
		inline std::wstring    &Texto(void)							{ return _Texto; }
								// Funciones para asignar el texto al men�
		void					Texto(const wchar_t *nTxt);
		void					Texto(std::wstring &nTxt);

								// Funci�nes que devuelven/asignan si el men� est� activado
		inline const BOOL		Activado(void)						{ return _Activado; };
		void					Activado(const BOOL nActivar);

		void                    Ocultar(const BOOL OcultarTodos = FALSE);		

		void                    Pintar(HDC hDC);

		inline void				Repintar(void)						{ RedrawWindow(hWnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT); };

		void					Terminar(void);
	  protected:
								// Constructor men� tipo texto (interno AgregarMenu)
								DMenuEx(DMenuEx *nPadre, DMenuEx_Tipo nTipo, HWND nhWndPadre, const UINT nID, const wchar_t *nTexto, const int nIconoRecursos = NULL, const BOOL nActivado = TRUE);
								// Constructor men� tipo separador (interno AgregarSeparador)
								DMenuEx(DMenuEx *nPadre, DMenuEx_Tipo nTipo, HWND nhWndPadre, const UINT nID);

		void				   _OcultarRecursivo(DMenuEx *oMenu);
								// Funci�n para calcular el tama�o de los menus.
		const POINT			   _CalcularMedidas(void);		
								// Funci�n que muestra este men� como un submen�
		void                   _MostrarSubMenu(HWND hWndDestMsg, DMenuEx *nPadre, const int cX, const int cY, const BOOL AsignarFoco = TRUE);
								// Funci�n que pinta una fila del men� (devuelve la altura de lo pintado)
		void                   _PintarMenu(HDC DC, DMenuEx *pMenu);
								// Funci�n que pinta el separador
		void                   _PintarSeparador(HDC DC, DMenuEx *pMenu);
								// Funci�n que pinta el expansor (flecha que indica que contiene uno o mas sub-menus)
		void                   _PintarExpansor(HDC DC, const int eX, const int eY);
								// Eventos internos
		void                   _Evento_Pintar(void);
		void                   _Evento_PintarNC(HRGN Region);
		void				   _Evento_MouseMovimiento(WPARAM wParam, LPARAM lParam);
		void				   _Evento_MousePresionado(const int Boton, WPARAM wParam, LPARAM lParam);
		void				   _Evento_MouseSoltado(const int Boton, WPARAM wParam, LPARAM lParam);
		void                   _Evento_TeclaPresionada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void                   _Evento_TeclaSoltada(const UINT Caracter, const UINT Repeticion, const UINT Params);
		void				   _Evento_Tecla(const UINT Caracter, const UINT Repeticion, const UINT Param);
		void                   _Evento_FocoPerdido(HWND UltimoFoco);

		RECT                   _Recta;

		LRESULT CALLBACK        GestorMensajes(UINT uMsg, WPARAM wParam, LPARAM lParam);
		std::vector<DMenuEx *> _Menus;
		UINT                   _ID;
		std::wstring           _Texto;
		DListaIconos_Icono	  *_Icono;
		DMenuEx               *_Padre;

		DMenuEx               *_MenuResaltado;
		DMenuEx               *_MenuDesplegado;
		DMenuEx               *_MenuPresionado;

		BOOL                   _Activado;
		DMenuEx_Tipo           _Tipo;
		static HWND            _hWndDest;

		BOOL				   _AnularMouseMove;
								// Menu resultado para la funci�n MostrarModal
		static DMenuEx        *_ResultadoModal;
	};
}

#endif