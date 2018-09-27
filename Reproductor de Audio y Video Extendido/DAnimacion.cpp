#include "stdafx.h"
#include "DAnimacion.h"

namespace DWL {

	DAnimacion::DAnimacion() : _Timer(NULL) {
	}


	DAnimacion::~DAnimacion() {
	}

	void DAnimacion::Iniciar(const float Desde, const float Hasta, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback) {
		std::vector<DAnimacion_Valores> Valores;
		Valores.push_back(DAnimacion_Valores(Desde, Hasta));
		Iniciar(Valores, Milisegundos, LambdaCallback);
	}

	void DAnimacion::Iniciar(std::vector<DAnimacion_Valores> &Datos, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback) {
		_Valores.resize(Datos.size());
		_Datos			= Datos;
		_Callback		= LambdaCallback;
		_Duracion		= Milisegundos;
		_TiempoActual	= 0;
		BOOL Ret = CreateTimerQueueTimer(&_Timer, NULL, reinterpret_cast<WAITORTIMERCALLBACK>(_TimerProc), this, 16, 16, WT_EXECUTEINTIMERTHREAD);
	}

	void DAnimacion::Terminar(void) {
		if (_Timer != NULL) {
			DeleteTimerQueueTimer(NULL, _Timer, NULL);
		}
		_Timer = NULL;
	}

	void CALLBACK DAnimacion::_TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {
		DAnimacion *This = static_cast<DAnimacion *>(lpParameter);
		This->_TiempoActual += 16;
		for (size_t i = 0; i < This->_Datos.size(); i++) {
			float Parte = (This->_Datos[i].Hasta - This->_Datos[i].Desde) / static_cast<float>(This->_Duracion);
			float Valor = This->_Datos[i].Desde + (Parte * This->_TiempoActual);

			if (This->_Datos[i].Hasta > This->_Datos[i].Desde)	{ if (Valor > This->_Datos[i].Hasta) Valor = This->_Datos[i].Hasta; }
			else												{ if (Valor < This->_Datos[i].Hasta) Valor = This->_Datos[i].Hasta; }
			This->_Valores[i] = Valor;
			#if DANIMACION_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DAnimacion::_TimerProc Desde : %.02f, Hasta : %.02f, Parte : %0.2f, Valor : %.02f, T : %d\n", This->_Datos[i].Desde, This->_Datos[i].Hasta, Parte, Valor, This->_TiempoActual);
			#endif
		}
		BOOL Terminado = (This->_TiempoActual > This->_Duracion);
		This->_Callback(This->_Valores, Terminado);
		if (Terminado == TRUE) {
			This->Terminar();
		}
	}

};