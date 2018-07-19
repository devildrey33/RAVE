#include "stdafx.h"
#include "DTemporizador.h"


DTemporizador::DTemporizador(void) {
}


DTemporizador::~DTemporizador(void) {
}

void DTemporizador::CrearTemporizador(const UINT nID, const DWORD Milisegundos, const BOOL UnaVez) {
	HANDLE nTimer = NULL;
	DTemporizador_Unico *Temporizador = new DTemporizador_Unico(nID, this, UnaVez);
	_Temporizadores.push_back(Temporizador);

	BOOL Ret = CreateTimerQueueTimer(&nTimer, NULL, reinterpret_cast<WAITORTIMERCALLBACK>(_TimerProc), Temporizador, Milisegundos, UnaVez, WT_EXECUTEINTIMERTHREAD);
	Temporizador->Timer = nTimer;
}

void CALLBACK DTemporizador::_TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {
	DTemporizador_Unico *ClaseTemporizador = static_cast<DTemporizador_Unico *>(lpParameter);
	ClaseTemporizador->Padre->Evento_Temporizador(ClaseTemporizador->ID);
	// Elimino el temporizador si solo es de una vez
	if (ClaseTemporizador->UnaVez == TRUE) {
		ClaseTemporizador->Padre->TerminarTemporizador(ClaseTemporizador->ID);
	}
}

void DTemporizador::TerminarTemporizador(const UINT nID) {
	size_t Pos = 0;
	if (_BuscarTemporizadorPos(nID, Pos) == TRUE) {
		DeleteTimerQueueTimer(NULL, _Temporizadores[Pos]->Timer, NULL);
		delete _Temporizadores[Pos];
		_Temporizadores.erase(_Temporizadores.begin() + Pos);
	}
}

const BOOL DTemporizador::_BuscarTemporizadorPos(const UINT bID, size_t &cPos) {
	for (size_t i = 0; i < _Temporizadores.size(); i++) {
		if (_Temporizadores[i]->ID == bID) {
			cPos = i;
			return TRUE;
		}
	}
	return FALSE;
}