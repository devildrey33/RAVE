#include "stdafx.h"
#include "Fecha_Historial.h"
#include <ctime>

// Constructor que asigna la fecha actual
Fecha_Historial::Fecha_Historial(void) {
	AsignarFechaActual();
}

// Función que asigna la fecha actual
void Fecha_Historial::AsignarFechaActual(void) {
	GetSystemTime(&_Tiempo);
}

// Constructor que asigna la fecha a partir de una cadena de caracteres de este estilo "YYYY-MM-DD HH:MM:SS.SSS"
/*Fecha_Historial::Fecha_Historial(const wchar_t *TxtFecha) {
	

}

// Constructor que asigna la fecha a partir de un string de este estilo "YYYY-MM-DD HH:MM:SS.SSS"
Fecha_Historial::Fecha_Historial(std::wstring &StrFecha) {

}*/

// Constructor que asigna la fecha
Fecha_Historial::Fecha_Historial(const UINT Any, const UINT Mes, const UINT Dia, const UINT Hora, const UINT Minuto, const UINT Segundo) {
	Asignar(Any, Mes, Dia, Hora, Minuto, Segundo);
}

// Función que asigna una fecha y hora especificas
void Fecha_Historial::Asignar(const UINT Any, const UINT Mes, const UINT Dia, const UINT Hora, const UINT Minuto, const UINT Segundo) {
	_Tiempo.wYear			= Any;
	_Tiempo.wMonth			= Mes;
	_Tiempo.wDay			= Dia;
	_Tiempo.wDayOfWeek		= -1;
	_Tiempo.wHour		    = Hora;
	_Tiempo.wMinute			= Minuto;
	_Tiempo.wSecond			= Segundo;
	_Tiempo.wMilliseconds	= 0;

	_ObtenerDiaSemana();
}

// Función interna para calcular el dia de la semana segun la fecha de este objeto
void Fecha_Historial::_ObtenerDiaSemana(void) {
	FILETIME ft;
	BOOL hrto	= SystemTimeToFileTime(&_Tiempo, &ft);
	BOOL hrback	= FileTimeToSystemTime(&ft, &_Tiempo);
}

// Función que devuelve el tiempo en un valor de 64 bits
const FILETIME Fecha_Historial::Valor(void) {
	FILETIME ft;
	BOOL hrto = SystemTimeToFileTime(&_Tiempo, &ft);
	return ft;
}

// Operador que asigna una nueva Fecha_Historial a este objeto
Fecha_Historial &Fecha_Historial::operator = (const Fecha_Historial &c) {
	_Tiempo = c._Tiempo;
	return *this;
}

// Operador que asigna una nueva Fecha_Historial a partir de la fecha de 64 bits especificada
Fecha_Historial &Fecha_Historial::operator = (const FILETIME nFecha) {
	FILETIME ft = nFecha;
	BOOL hrback = FileTimeToSystemTime(&ft, &_Tiempo);
	return *this;
}

// Devuelve una cadena de caracteres con el texto formateado para SQLite
const std::wstring &Fecha_Historial::Texto(std::wstring &OUT_Str) {
	std::wstring TmpStr;

	// Convierto el año
	TmpStr = std::to_wstring(_Tiempo.wYear);
	while (TmpStr.size() < 4) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr + L"-";

	// Convierto el mes
	TmpStr = std::to_wstring(_Tiempo.wMonth);
	while (TmpStr.size() < 2) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr + L"-";

	// Convierto el dia
	TmpStr = std::to_wstring(_Tiempo.wDay);
	while (TmpStr.size() < 2) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr + L" ";

	// Convierto la hora
	TmpStr = std::to_wstring(_Tiempo.wHour);
	while (TmpStr.size() < 2) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr + L":";

	// Convierto los minutos
	TmpStr = std::to_wstring(_Tiempo.wHour);
	while (TmpStr.size() < 2) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr + L":";

	// Convierto los segundos
	TmpStr = std::to_wstring(_Tiempo.wHour);
	while (TmpStr.size() < 2) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr;

	return OUT_Str;
}