#include "stdafx.h"
#include "Historial_Medio.h"

Historial_Medio::Historial_Medio(const LONG_PTR IDMedio, const LONG_PTR IDLista) : IdLista(IDLista), IdMedio(IDMedio) {
	// Si IDLista es -1, se asignara la id de la ultima lista creada en el historial
	if (IDLista == -1) IdLista = App.BD.Historial_UltimaIDLista;
}