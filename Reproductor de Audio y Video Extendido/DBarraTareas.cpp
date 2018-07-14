#include "stdafx.h"
#include "DBarraTareas.h"

namespace DWL {

	ITaskbarList4 *DBarraTareas::_BarraTareas = NULL;

	DBarraTareas::DBarraTareas(void) : _hWnd(NULL), _Estado(DBarraTareas_Estado_SinProgreso) {
	}

	DBarraTareas::~DBarraTareas(void) {
	}

	void DBarraTareas::Resaltar(void) {
		FlashWindow(_hWnd, TRUE);
		CrearTemporizador(10, 1000, TRUE);
	}

	void DBarraTareas::Evento_Temporizador(const UINT cID) {
		if (cID == 10) FlashWindow(_hWnd, TRUE);
	}


	// No se mostrará ninguna barra de progreso
	void DBarraTareas::Estado_SinProgreso(void) {		
		if (_IniciarITaskBar() == TRUE) _BarraTareas->SetProgressState(_hWnd, TBPF_NOPROGRESS);
		_Estado = DBarraTareas_Estado_SinProgreso;
	}

	// Se mostrará un progreso indeterminado (haciendo cliclos de izquierda a derecha del tamaño del botón de la taskbar)
	void DBarraTareas::Estado_Indeterminado(void) {
		if (_IniciarITaskBar() == TRUE) _BarraTareas->SetProgressState(_hWnd, TBPF_INDETERMINATE);
		_Estado = DBarraTareas_Estado_Indeterminado;
	}

	// Se mostrará una barra de progreso normal (de color verde)
	void DBarraTareas::Estado_Normal(void) {
		if (_IniciarITaskBar() == TRUE) _BarraTareas->SetProgressState(_hWnd, TBPF_NORMAL);
		_Estado = DBarraTareas_Estado_Normal;
	}

	// Se mostrará una barra de progreso con error (de color rojo)
	void DBarraTareas::Estado_Error(void) {
		if (_IniciarITaskBar() == TRUE) _BarraTareas->SetProgressState(_hWnd, TBPF_ERROR);
		_Estado = DBarraTareas_Estado_Error;
	}

	// Se mostrará una barra de progreso en pausa (de color amarillo)
	void DBarraTareas::Estado_Pausado(void) {
		if (_IniciarITaskBar() == TRUE) _BarraTareas->SetProgressState(_hWnd, TBPF_PAUSED);
		_Estado = DBarraTareas_Estado_Pausado;
	}

	// Asigna un valor a la barra de progreso
	void DBarraTareas::Valor(const UINT nValor, const UINT nMaximo) {
		if (_IniciarITaskBar() == TRUE) _BarraTareas->SetProgressValue(_hWnd, nValor, nMaximo);
	}



	void DBarraTareas::_Iniciar(HWND hWndVentana) {
		_hWnd = hWndVentana;
	}


	const BOOL DBarraTareas::_IniciarITaskBar(void) {
		if (_BarraTareas == NULL) {
			// Inicio la instancia para controlar la barra de tareas
			if (CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList4, (void**)&_BarraTareas) == S_OK) {
				_BarraTareas->HrInit();
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
		else {
			return TRUE;
		}
	}
};