#ifndef DANIMACION_H
#define DANIMACION_H

#define DANIMACION_MOSTRARDEBUG	FALSE

namespace DWL {


	class DAnimacion {
	   public:
		class Valores {
	   		public:
					Valores(void) : Desde(0), Hasta(0) { };
					Valores(const float nDesde, const float nHasta) : Desde(nDesde), Hasta(nHasta) { };
			       ~Valores(void) { };
			float   Desde;
			float   Hasta;
		};
																DAnimacion();
		                                                       ~DAnimacion();

																// Iniciar para un solo valor (desde - hasta)
		void													Iniciar(const float Desde, const float Hasta, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback);
																// Iniciar para un vector de valores (cada valor tiene un desde y un hasta)
		void													Iniciar(std::vector<Valores> &Valores, const DWORD Milisegundos, std::function<void(std::vector<float> &, const BOOL)> LambdaCallback);
		void													Terminar(void);
	   protected:
		static void CALLBACK                                   _TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		std::function<void(std::vector<float> &, const BOOL)>  _Callback;
		HANDLE                                                 _Timer;
		DWORD                                                  _Duracion;
		DWORD                                                  _TiempoActual;
		std::vector<Valores>                                   _Datos;
		std::vector<float>                                     _Valores;

	};

};
#endif