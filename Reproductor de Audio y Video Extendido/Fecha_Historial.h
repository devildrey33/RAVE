#pragma once

#include <string>

class Fecha_Historial {
  public:									// Constructor que asignar la Fecha y hora actual
											Fecha_Historial(void); 
											// Constructor que asigna la fecha a partir de una cadena de caracteres de este estilo "YYYY-MM-DD HH:MM:SS.SSS"
//											Fecha_Historial(const wchar_t *TxtFecha);
											// Constructor que asigna la fecha a partir de un string de este estilo "YYYY-MM-DD HH:MM:SS.SSS"
//											Fecha_Historial(std::wstring &StrFecha);
											// Constructor que asigna la fecha
											Fecha_Historial(const UINT Ano, const UINT Mes, const UINT Dia, const UINT Hora, const UINT Minuto, const UINT Segundo = 0);

	void									Asignar(const UINT Ano, const UINT Mes, const UINT Dia, const UINT Hora, const UINT Minuto, const UINT Segundo = 0);
											// Función que asigna la fecha y la hora actual a este objeto
	void									AsignarFechaActual(void);

    Fecha_Historial						   &operator = (const Fecha_Historial &c);
//    Fecha_Historial						   &operator = (const wchar_t *TxtFecha);
//    Fecha_Historial						   &operator = (std::wstring &StrFecha);
	Fecha_Historial						   &operator = (const FILETIME nFecha);

											// Funciones para asignar y devolver la fecha en un UINT
	inline const WORD						Dia(void)						{ return _Tiempo.wDay;			};
	inline void								Dia(const WORD nDia)			{ _Tiempo.wDay = nDia;			};
	inline const WORD						DiaSemana(void)					{ return _Tiempo.wDayOfWeek;	};
	inline const WORD						Mes(void)						{ return _Tiempo.wMonth;		};
	inline void								Mes(const WORD nMes)			{ _Tiempo.wMonth = nMes;		};
	inline const WORD						Any(void)						{ return _Tiempo.wYear;			};
	inline void								Any(const WORD nAny)			{ _Tiempo.wYear = nAny;			};

											// Funciones para asignar y devolver el tiempo en un UINT
	inline const WORD						Hora(void)						{ return _Tiempo.wHour;			};
	inline void								Hora(const WORD nHora)			{ _Tiempo.wHour = nHora;		};
	inline const WORD						Minuto(void)					{ return _Tiempo.wMinute;		};
	inline void								Minuto(const WORD nMinuto)		{ _Tiempo.wMinute = nMinuto;	};
	inline const WORD						Segundo(void)					{ return _Tiempo.wSecond;		};
	inline void								Segundo(const WORD nSegundo)	{ _Tiempo.wSecond = nSegundo;	};
//	const UINT								MiliSegundo(void);
//	void									MiliSegundo(const UINT nMiliSegundo);

											// Función que devuelve la fecha y la hora en formato LONGLONG
    const FILETIME							Valor(void);
	const std::wstring                     &Texto(std::wstring &OUT_Str);
  protected:
	void                                   _ObtenerDiaSemana(void);
//    std::wstring                           _StrFecha;

	SYSTEMTIME                             _Tiempo;
};
