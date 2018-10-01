#include "stdafx.h"
#include "DAnimacion.h"

namespace DWL {
	
/*	 _____                _                      _
	|  __ \   /\         (_)                    (_)            
	| |  | | /  \   _ __  _ _ __ ___   __ _  ___ _  ___  _ __  
	| |  | |/ /\ \ | '_ \| | '_ ` _ \ / _` |/ __| |/ _ \| '_ \ 
	| |__| / ____ \| | | | | | | | | | (_| | (__| | (_) | | | |
	|_____/_/    \_\_| |_|_|_| |_| |_|\__,_|\___|_|\___/|_| |_|
*/

	DAnimacion::DAnimacion(void) : _Timer(NULL) {
	}


	DAnimacion::~DAnimacion(void) {
		Terminar();
	}

	void DAnimacion::Iniciar(const float Desde, const float Hasta, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback) {
		std::vector<Valores> vValores;
		vValores.push_back(Valores(Desde, Hasta));
		Iniciar(vValores, Milisegundos, LambdaCallback);
	}

	void DAnimacion::Iniciar(std::vector<Valores> &Datos, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback) {
		if (_Timer != NULL) {
			#if DANIMACION_MOSTRARDEBUG == TRUE
				Debug_Escribir(L"DAnimacion::Iniciar -> ERROR la animación ya se está ejecutando!\n");
			#endif
			return;
		}
		_Valores.resize(Datos.size());
		_Datos			= Datos;
		_Callback		= LambdaCallback;
		_Duracion		= Milisegundos;
		_TiempoActual	= 0;
		BOOL Ret = CreateTimerQueueTimer(&_Timer, NULL, reinterpret_cast<WAITORTIMERCALLBACK>(_TimerProc), this, 16, 16, WT_EXECUTEINTIMERTHREAD);
		#if DANIMACION_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DAnimacion::Iniciar %d milisegundos.\n", Milisegundos);
		#endif

	}

	void DAnimacion::Invertir(void) {
		#if DANIMACION_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DAnimacion::Invertir %d milisegundos.\n", _TiempoActual);
		#endif

		_Duracion = _TiempoActual;
		_TiempoActual = 0;
		// El Desde pasa a ser el Hasta, y el valor actual se convierte en el Desde
		for (size_t i = 0; i < _Datos.size(); i++) {
			_Datos[i].Hasta = _Datos[i].Desde;
			_Datos[i].Desde = _Valores[i];
		}
	}

	void DAnimacion::Terminar(void) {
		#if DANIMACION_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DAnimacion::Terminar\n");
		#endif
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







/*   _____                _                      _              _____      _            
	|  __ \   /\         (_)                    (_)            / ____|    | |           
	| |  | | /  \   _ __  _ _ __ ___   __ _  ___ _  ___  _ __ | |     ___ | | ___  _ __ 
	| |  | |/ /\ \ | '_ \| | '_ ` _ \ / _` |/ __| |/ _ \| '_ \| |    / _ \| |/ _ \| '__|
	| |__| / ____ \| | | | | | | | | | (_| | (__| | (_) | | | | |___| (_) | | (_) | |   
	|_____/_/    \_\_| |_|_|_| |_| |_|\__,_|\___|_|\___/|_| |_|\_____\___/|_|\___/|_|   
*/
                                                                                     
                                                                                    
	
	DAnimacionColor::DAnimacionColor(void) : _Timer(NULL) {
	}


	DAnimacionColor::~DAnimacionColor(void) {
		Terminar();
	}

	// Función para un color
	void DAnimacionColor::Iniciar(const COLORREF Desde, const COLORREF Hasta, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback) {
		std::vector<Colores> vValores;
		vValores.push_back(Colores(Desde, Hasta));
		Iniciar(vValores, Milisegundos, LambdaCallback);
	}

	// Función para dos colores
	void DAnimacionColor::Iniciar(const COLORREF Desde1, const COLORREF Hasta1, const COLORREF Desde2, const COLORREF Hasta2, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback) {
		std::vector<Colores> vValores;
		vValores.push_back(Colores(Desde1, Hasta1));
		vValores.push_back(Colores(Desde2, Hasta2));
		Iniciar(vValores, Milisegundos, LambdaCallback);
	}

	// Función para tres colores
	void DAnimacionColor::Iniciar(const COLORREF Desde1, const COLORREF Hasta1, const COLORREF Desde2, const COLORREF Hasta2, const COLORREF Desde3, const COLORREF Hasta3, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback) {
		std::vector<Colores> vValores;
		vValores.push_back(Colores(Desde1, Hasta1));
		vValores.push_back(Colores(Desde2, Hasta2));
		vValores.push_back(Colores(Desde3, Hasta3));
		Iniciar(vValores, Milisegundos, LambdaCallback);
	}

	// Función para cuatro colores
	void DAnimacionColor::Iniciar(const COLORREF Desde1, const COLORREF Hasta1, const COLORREF Desde2, const COLORREF Hasta2, const COLORREF Desde3, const COLORREF Hasta3, const COLORREF Desde4, const COLORREF Hasta4, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback) {
		std::vector<Colores> vValores;
		vValores.push_back(Colores(Desde1, Hasta1));
		vValores.push_back(Colores(Desde2, Hasta2));
		vValores.push_back(Colores(Desde3, Hasta3));
		vValores.push_back(Colores(Desde4, Hasta4));
		Iniciar(vValores, Milisegundos, LambdaCallback);
	}


	void DAnimacionColor::Iniciar(std::vector<Colores> &Datos, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback) {
		if (_Timer != NULL) {
			#if DANIMACION_MOSTRARDEBUG == TRUE
				Debug_Escribir(L"DAnimacionColor::Iniciar -> ERROR la animación ya se está ejecutando!\n");
			#endif
			return;
		}
		_Valores.resize(Datos.size());
		_Datos			= Datos;
		_Callback		= LambdaCallback;
		_Duracion		= Milisegundos;
		_TiempoActual	= 0;
		BOOL Ret = CreateTimerQueueTimer(&_Timer, NULL, reinterpret_cast<WAITORTIMERCALLBACK>(_TimerProc), this, 16, 16, WT_EXECUTEINTIMERTHREAD);

		#if DANIMACION_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DAnimacionColor::Iniciar %d milisegundos.\n", Milisegundos);
		#endif
	}

	void DAnimacionColor::Invertir(void) {
		#if DANIMACION_MOSTRARDEBUG == TRUE
			Debug_Escribir_Varg(L"DAnimacionColor::Invertir %d milisegundos.\n", _TiempoActual);
		#endif
		_Duracion = _TiempoActual;
		_TiempoActual = 0;
		// El Desde pasa a ser el Hasta, y el valor actual se convierte en el Desde
		for (size_t i = 0; i < _Datos.size(); i++) {
			_Datos[i].Hasta = _Datos[i].Desde;
			_Datos[i].Desde = _Valores[i];
		}
	}

	void DAnimacionColor::Terminar(void) {
		#if DANIMACION_MOSTRARDEBUG == TRUE
			Debug_Escribir(L"DAnimacionColor::Terminar\n");
		#endif
		if (_Timer != NULL) {
			DeleteTimerQueueTimer(NULL, _Timer, NULL);
		}
		_Timer = NULL;
	}

	void CALLBACK DAnimacionColor::_TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {
		DAnimacionColor *This = static_cast<DAnimacionColor *>(lpParameter);
		This->_TiempoActual += 16;
//		if (This->_TiempoActual > This->_Duracion) This->_TiempoActual = This->_Duracion;
		float ParteR, ParteG, ParteB;
		int ValorR, ValorG, ValorB;
		for (size_t i = 0; i < This->_Datos.size(); i++) {
			// Canal R
			ParteR = (This->_Datos[i].HastaR() - This->_Datos[i].DesdeR()) / static_cast<float>(This->_Duracion);
			ValorR = This->_Datos[i].DesdeR() + static_cast<int>(ParteR * This->_TiempoActual);
			if (This->_Datos[i].HastaR() > This->_Datos[i].DesdeR()) { if (ValorR > This->_Datos[i].HastaR()) 		ValorR = This->_Datos[i].HastaR(); }
			else                                                     { if (ValorR < This->_Datos[i].HastaR()) 		ValorR = This->_Datos[i].HastaR(); }

			// Canal G
			ParteG = (This->_Datos[i].HastaG() - This->_Datos[i].DesdeG()) / static_cast<float>(This->_Duracion);
			ValorG = This->_Datos[i].DesdeG() + static_cast<int>(ParteG * This->_TiempoActual);
			if (This->_Datos[i].HastaG() > This->_Datos[i].DesdeG()) { if (ValorG > This->_Datos[i].HastaG())		ValorG = This->_Datos[i].HastaG(); }
			else                                                     { if (ValorG < This->_Datos[i].HastaG())		ValorG = This->_Datos[i].HastaG(); }

			// Canal B
			ParteB = (This->_Datos[i].HastaB() - This->_Datos[i].DesdeB()) / static_cast<float>(This->_Duracion);
			ValorB = This->_Datos[i].DesdeB() + static_cast<int>(ParteB * This->_TiempoActual);
			if (This->_Datos[i].HastaB() > This->_Datos[i].DesdeR()) { if (ValorB > This->_Datos[i].HastaB())		ValorB = This->_Datos[i].HastaB(); }
			else                                                     { if (ValorB < This->_Datos[i].HastaB())		ValorB = This->_Datos[i].HastaB(); }

			This->_Valores[i] = RGB(ValorR, ValorG, ValorB);
			#if DANIMACION_MOSTRARDEBUG == TRUE
				Debug_Escribir_Varg(L"DAnimacion::_TimerProc Desde : %.02f, Hasta : %.02f, Valor : %d, T : %d\n", This->_Datos[i].Desde, This->_Datos[i].Hasta, This->_Valores[i], This->_TiempoActual);
			#endif
		}
		BOOL Terminado = (This->_TiempoActual > This->_Duracion);
		This->_Callback(This->_Valores, Terminado);
		if (Terminado == TRUE) {
			This->Terminar();
		}
	}



};