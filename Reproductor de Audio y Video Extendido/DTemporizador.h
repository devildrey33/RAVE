#ifndef DTEMPORIZADOR_H
#define DTEMPORIZADOR_H

#include <vector>

namespace DWL {


	class DTemporizador {
   	  public:

		class DTemporizador_Unico {
   		  public:
									DTemporizador_Unico(const DTemporizador_Unico &Temporizador) : ID(Temporizador.ID), Timer(Temporizador.Timer), Padre(Temporizador.Padre) { };
									DTemporizador_Unico(const UINT nID, DTemporizador *nPadre, BOOL nUnaVez) : ID(nID), Timer(0), Padre(nPadre), UnaVez(nUnaVez) { };
								   ~DTemporizador_Unico() { };
			DTemporizador          *Padre;
			UINT                    ID;
			HANDLE                  Timer;
			BOOL                    UnaVez;
		};

											DTemporizador();
								           ~DTemporizador();
		void								CrearTemporizador(const UINT nID, const DWORD Milisegundos, const BOOL UnaVez = FALSE);
		void								TerminarTemporizador(const UINT nID);
		virtual void						Evento_Temporizador(const UINT cID) { };
	  private:
		const BOOL                         _BuscarTemporizadorPos(const UINT cID, size_t &cPos);
		static void CALLBACK               _TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		std::vector<DTemporizador_Unico *> _Temporizadores;
	};

}
#endif