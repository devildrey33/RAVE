#ifndef DTEMPORIZADOR_H
#define DTEMPORIZADOR_H

#include <vector>

namespace DWL {


	class DTemporizador {
   	  public:

		class DTemporizador_Unico {
   		  public:
									DTemporizador_Unico(const DTemporizador_Unico &Temporizador) : ID(Temporizador.ID), Timer(Temporizador.Timer), Padre(Temporizador.Padre) { };
									DTemporizador_Unico(const UINT_PTR nID, DTemporizador *nPadre, BOOL nUnaVez) : ID(nID), Timer(0), Padre(nPadre), UnaVez(nUnaVez) { };
									DTemporizador_Unico(const UINT_PTR nID, DTemporizador *nPadre, BOOL nUnaVez, std::function<void(void)> nCallback) : ID(nID), Timer(0), Padre(nPadre), UnaVez(nUnaVez), Callback(nCallback) { };
									~DTemporizador_Unico() { };
			DTemporizador          *Padre;
			UINT_PTR                ID;
			HANDLE                  Timer;
			BOOL                    UnaVez;
			std::function<void(void)>      Callback;
		};

											DTemporizador();
								           ~DTemporizador();
		void								CrearTemporizador(const UINT_PTR nID, const DWORD Milisegundos, const BOOL UnaVez = FALSE);
		void                                CrearTemporizador(const DWORD Milisegundos, const BOOL UnaVez, std::function<void(void)> Callback);
		void								TerminarTemporizador(const UINT_PTR nID);
		void								TerminarTemporizadores(void);
		virtual void						Evento_Temporizador(const UINT_PTR cID) { };
	  private:
		const BOOL                         _BuscarTemporizadorPos(const UINT_PTR cID, size_t &cPos);
		static void CALLBACK               _TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		static void CALLBACK               _TimerProcLambda(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		std::vector<DTemporizador_Unico *> _Temporizadores;
		
	};

}
#endif