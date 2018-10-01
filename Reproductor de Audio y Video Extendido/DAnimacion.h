#ifndef DANIMACION_H
#define DANIMACION_H

#define DANIMACION_MOSTRARDEBUG	TRUE

#include <functional>

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
		void                                                    Invertir(void);
		void													Terminar(void);
		inline const BOOL                                       Animando(void) { return (_Timer != NULL); }
	   protected:
		static void CALLBACK                                   _TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		std::function<void(std::vector<float> &, const BOOL)>  _Callback;
		HANDLE                                                 _Timer;
		DWORD                                                  _Duracion;
		DWORD                                                  _TiempoActual;
		std::vector<Valores>                                   _Datos;
		std::vector<float>                                     _Valores;

	};



	class DAnimacionColor {
	   public:
		class Colores {
	   		public:
								Colores(void) : Desde(0), Hasta(0) { };
								Colores(const COLORREF nDesde, const COLORREF nHasta) : Desde(nDesde), Hasta(nHasta) { };
			                   ~Colores(void) { };
			inline const BYTE	DesdeR(void) { return GetRValue(Desde); };
			inline const BYTE	DesdeG(void) { return GetGValue(Desde); };
			inline const BYTE	DesdeB(void) { return GetBValue(Desde); };
			inline const BYTE	HastaR(void) { return GetRValue(Hasta); };
			inline const BYTE	HastaG(void) { return GetGValue(Hasta); };
			inline const BYTE	HastaB(void) { return GetBValue(Hasta); };
			COLORREF			Desde;
			COLORREF			Hasta;
		};
																	DAnimacionColor();
		                                                           ~DAnimacionColor();

																	// Iniciar para un solo valor (desde - hasta)
		void														Iniciar(const COLORREF Desde, const COLORREF Hasta, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback);
																	// Iniciar para dos valores (desde - hasta)
		void														Iniciar(const COLORREF Desde1, const COLORREF Hasta1, const COLORREF Desde2, const COLORREF Hasta2, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback);
																	// Iniciar para tres valores (desde - hasta)
		void														Iniciar(const COLORREF Desde1, const COLORREF Hasta1, const COLORREF Desde2, const COLORREF Hasta2, const COLORREF Desde3, const COLORREF Hasta3, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback);
																	// Iniciar para cuatro valores (desde - hasta)
		void														Iniciar(const COLORREF Desde1, const COLORREF Hasta1, const COLORREF Desde2, const COLORREF Hasta2, const COLORREF Desde3, const COLORREF Hasta3, const COLORREF Desde4, const COLORREF Hasta4, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback);
																	// Iniciar para un vector de valores (cada valor tiene un desde y un hasta)
		void														Iniciar(std::vector<Colores> &Valores, const DWORD Milisegundos, std::function<void(std::vector<COLORREF> &, const BOOL)> LambdaCallback);
																	// Invierte la animación (mientras se este ejecutando)
		void                                                        Invertir(void);
		void														Terminar(void);
		inline const BOOL                                           Animando(void) { return (_Timer != NULL);  }
	protected:
		static void CALLBACK									   _TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		std::function<void(std::vector<COLORREF> &, const BOOL)>   _Callback;
		HANDLE                                                     _Timer;
		DWORD                                                      _Duracion;
		DWORD                                                      _TiempoActual;
		std::vector<Colores>                                       _Datos;
		std::vector<COLORREF>                                      _Valores;

	};
};
#endif