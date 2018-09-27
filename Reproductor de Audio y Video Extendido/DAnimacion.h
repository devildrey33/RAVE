#ifndef DANIMACION_H
#define DANIMACION_H

#define DANIMACION_MOSTRARDEBUG	TRUE

namespace DWL {

	class DAnimacion_Valores {
	   public:
				DAnimacion_Valores(void) : Desde(0), Hasta(0) { };
				DAnimacion_Valores(const float nDesde, const float nHasta) : Desde(nDesde), Hasta(nHasta) { };
		       ~DAnimacion_Valores(void) { };
		float   Desde;
		float   Hasta;
	};

	class DAnimacion {
	   public:
											DAnimacion();
		                                   ~DAnimacion();
		void								Iniciar(const float Desde, const float Hasta, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback);
		void								Iniciar(std::vector<DAnimacion_Valores> &Valores, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback);
		void                                Terminar(void);
	   protected:
		static void CALLBACK               _TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		std::function<void(std::vector<float> &, const BOOL)>   _Callback;
		HANDLE                             _Timer;
		DWORD                              _Duracion;
		DWORD                              _TiempoActual;
		float                              _Desde;
		float                              _Hasta;
		std::vector<DAnimacion_Valores>    _Datos;
		std::vector<float>                 _Valores;

	};

};
#endif