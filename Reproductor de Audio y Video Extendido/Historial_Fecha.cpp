#include "stdafx.h"
#include "Historial_Fecha.h"
#include <ctime>
#include <DStringUtils.h>

// Constructor que asigna la fecha actual
Historial_Fecha::Historial_Fecha(void) : _Tiempo({ 0, 0, 0, 0, 0, 0, 0, 0}) {
	AsignarFechaActual();
}

// Constructor que asigna la fecha a partir de una cadena de caracteres de este estilo "YYYY-MM-DD HH:MM:SS.SSS"
Historial_Fecha::Historial_Fecha(const wchar_t *TxtFecha) : _Tiempo({ 0, 0, 0, 0, 0, 0, 0, 0 }) {
	AsignarFechaStr(TxtFecha);
}

// Constructor que asigna la fecha a partir de un string de este estilo "YYYY-MM-DD HH:MM:SS.SSS"
Historial_Fecha::Historial_Fecha(std::wstring &StrFecha) : _Tiempo({ 0, 0, 0, 0, 0, 0, 0, 0 }) {
	AsignarFechaStr(StrFecha);
}

// Función que asigna la fecha actual
void Historial_Fecha::AsignarFechaActual(void) {
	GetLocalTime(&_Tiempo);
}

// Función que asigna la fecha partiendo de una cadena de caracteres con el siguiente formato : "yyyy-mm-dd hh:mm:ss"
void Historial_Fecha::AsignarFechaStr(const wchar_t *Str) {
	std::wstring TmpStr(Str);
	AsignarFechaStr(TmpStr);
}

// Función que asigna la fecha partiendo de un string con el siguiente formato : "yyyy-mm-dd hh:mm:ss"
void Historial_Fecha::AsignarFechaStr(std::wstring &Str) {
	// La fecha es mas pequeña de lo que debería, salgo de la función
	if (Str.size() < 19) return;
	// Año
	DWL::Strings::StrTo(Str.substr(0, 4), _Tiempo.wYear);
	// Mes
	DWL::Strings::StrTo(Str.substr(5, 7), _Tiempo.wMonth);
	// Dia
	DWL::Strings::StrTo(Str.substr(8, 10), _Tiempo.wDay);
	// Hora
	DWL::Strings::StrTo(Str.substr(11, 13), _Tiempo.wHour);
	// M;inuto
	DWL::Strings::StrTo(Str.substr(14, 16), _Tiempo.wMinute);
	// Segundo
	DWL::Strings::StrTo(Str.substr(17, 19), _Tiempo.wSecond);
}


// Constructor que asigna la fecha
Historial_Fecha::Historial_Fecha(const UINT Any, const UINT Mes, const UINT Dia, const UINT Hora, const UINT Minuto, const UINT Segundo) {
	Asignar(Any, Mes, Dia, Hora, Minuto, Segundo);
}

// Función que asigna una fecha y hora especificas
void Historial_Fecha::Asignar(const UINT Any, const UINT Mes, const UINT Dia, const UINT Hora, const UINT Minuto, const UINT Segundo) {
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
void Historial_Fecha::_ObtenerDiaSemana(void) {
	FILETIME ft;
	BOOL hrto	= SystemTimeToFileTime(&_Tiempo, &ft);
	BOOL hrback	= FileTimeToSystemTime(&ft, &_Tiempo);
}

// Función que devuelve el tiempo en un valor de 64 bits
const FILETIME Historial_Fecha::Valor(void) {
	FILETIME ft;
	BOOL hrto = SystemTimeToFileTime(&_Tiempo, &ft);
	return ft;
}

// Operador que asigna una nueva Historial_Fecha a este objeto
Historial_Fecha &Historial_Fecha::operator = (const Historial_Fecha &c) {
	_Tiempo = c._Tiempo;
	return *this;
}

// Operador que asigna una nueva Historial_Fecha a partir de la fecha de 64 bits especificada
Historial_Fecha &Historial_Fecha::operator = (const FILETIME nFecha) {
	FILETIME ft = nFecha;
	BOOL hrback = FileTimeToSystemTime(&ft, &_Tiempo);
	return *this;
}

// Devuelve una cadena de caracteres con el texto formateado para SQLite
const std::wstring &Historial_Fecha::Texto(std::wstring &OUT_Str) {
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
	TmpStr = std::to_wstring(_Tiempo.wMinute);
	while (TmpStr.size() < 2) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr + L":";

	// Convierto los segundos
	TmpStr = std::to_wstring(_Tiempo.wSecond);
	while (TmpStr.size() < 2) TmpStr.insert(0, L"0");
	OUT_Str += TmpStr;

	return OUT_Str;
}