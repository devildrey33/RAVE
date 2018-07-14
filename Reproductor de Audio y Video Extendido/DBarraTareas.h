#ifndef DBARRATAREAS_H
	#define DBARRATAREAS_H

	#include <Shobjidl.h>
	#include "DTemporizador.h"

	namespace DWL {
		class DVentana;

		enum DBarraTareas_Estado {
			DBarraTareas_Estado_SinProgreso		= TBPF_NOPROGRESS,
			DBarraTareas_Estado_Indeterminado	= TBPF_INDETERMINATE,
			DBarraTareas_Estado_Normal			= TBPF_NORMAL,
			DBarraTareas_Estado_Pausado			= TBPF_PAUSED,
			DBarraTareas_Estado_Error			= TBPF_ERROR
		};

		class DBarraTareas : public DTemporizador {
		  public:
												DBarraTareas(void);
								               ~DBarraTareas(void);
												// Resalta la ventana y el botón de la barra de tareas
			void								Resaltar(void);
												// No se mostrará ninguna barra de progreso
			void								Estado_SinProgreso(void);
												// Se mostrará un progreso indeterminado (haciendo cliclos de izquierda a derecha del tamaño del botón de la taskbar)
			void								Estado_Indeterminado(void);
												// Se mostrará una barra de progreso normal
			void								Estado_Normal(void);
												// Se mostrará una barra de progreso con error (de color rojo)
			void								Estado_Error(void);
												// Se mostrará una barra de progreso en pausa (de color amarillo)
			void								Estado_Pausado(void);
												// Asigna un valor a la barra de progreso
			void								Valor(const UINT nValor, const UINT nMaximo);
												// Devuelve el estado de la barra de progreso
			inline const DBarraTareas_Estado	Estado(void) { return _Estado; }
		protected:

			void								Evento_Temporizador(const UINT cID);

			void                               _Iniciar(HWND hWndVentana);
			const BOOL						   _IniciarITaskBar(void);
			static ITaskbarList4              *_BarraTareas;
			HWND                               _hWnd;
			DBarraTareas_Estado                _Estado;
			
			// Esta clase es amiga de DVentana
			friend class DVentana;
		};
	};
#endif