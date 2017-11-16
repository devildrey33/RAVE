#include "stdafx.h"
#include "DConsola.h"

namespace DWL {

	DConsola::DConsola(void) : _Mutex(NULL), _Consola(NULL), _UltimoTick(0) { 
	}

	DConsola::~DConsola(void) {
		if (_Mutex) CloseHandle(_Mutex);
	}

	void DConsola::Crear(const wchar_t *Titulo) {
		if (!_Mutex) 
			_Mutex = CreateMutex(NULL, FALSE, TEXT("Mutex_DConsola"));
		if (!GetConsoleWindow()) {
			AllocConsole();
			SetConsoleTitle(Titulo);
			_Consola = GetStdHandle(STD_OUTPUT_HANDLE);
			_UltimoTick = GetTickCount();
		}
	}


/*	const BOOL DConsola::Escribir(const wchar_t *Texto) {
		DWORD CaracteresEscribidos = 0;
		return WriteConsole(_Consola, Texto, static_cast<DWORD>(wcslen(Texto)), &CaracteresEscribidos, NULL);
	}*/

	const BOOL DConsola::Escribir(const wchar_t *Texto, ...) {
		WaitForSingleObject(_Mutex, INFINITE);
		static wchar_t  T[512];
		va_list			Marker;
		va_start(Marker, Texto);
		int TotalCaracteres = vswprintf_s(T, 512, Texto, Marker);
		DWORD CaracteresEscribidos = 0;
		BOOL Ret = WriteConsole(_Consola, T, TotalCaracteres, &CaracteresEscribidos, NULL);
		ReleaseMutex(_Mutex);
		return Ret;
	}

	const BOOL DConsola::Escribir(std::wstring &Texto) {
		WaitForSingleObject(_Mutex, INFINITE);
		DWORD CaracteresEscribidos = 0;
		BOOL Ret = WriteConsole(_Consola, Texto.c_str(), static_cast<DWORD>(Texto.size()), &CaracteresEscribidos, NULL);
		ReleaseMutex(_Mutex);
		return Ret;
	}

	const BOOL DConsola::Leer(std::wstring &Texto) {
		WaitForSingleObject(_Mutex, INFINITE);
		DWORD CaracteresLeidos = 0;
		static wchar_t Caracteres[512];
		SecureZeroMemory(Caracteres, 512 * sizeof(wchar_t));
		BOOL R = ReadConsole(_Consola, Caracteres, 512, &CaracteresLeidos, NULL);
		Texto = Caracteres;
		ReleaseMutex(_Mutex);
		return R;
	}






/*	const BOOL DConsola::EscribirMS(const wchar_t *Texto) {

		DWORD CaracteresEscribidos = 0;
		static wchar_t Txt[512];
		DWORD TmpTick = GetTickCount();
		int TotalCaracteres = swprintf_s(Txt, 512, L"[%.8d] %s", TmpTick - _UltimoTick, Texto);
		_UltimoTick = TmpTick;
		return WriteConsole(_Consola, Txt, TotalCaracteres, &CaracteresEscribidos, NULL);
	}*/

	const BOOL DConsola::EscribirMS(const wchar_t *Texto, ...) {
		WaitForSingleObject(_Mutex, INFINITE);
		static wchar_t  T[512];
		va_list			Marker;
		va_start(Marker, Texto);
		int TotalCaracteres = vswprintf_s(T, 512, Texto, Marker);
		DWORD CaracteresEscribidos = 0;
		static wchar_t Txt[512];
		DWORD TmpTick = GetTickCount();
		TotalCaracteres = swprintf_s(Txt, 512, L"[%.8d] %s", TmpTick - _UltimoTick, T);
		_UltimoTick = TmpTick;
		BOOL Ret = WriteConsole(_Consola, Txt, TotalCaracteres, &CaracteresEscribidos, NULL);
		ReleaseMutex(_Mutex);
		return Ret;
	}

	const BOOL DConsola::EscribirMS(std::wstring &Texto) {
		WaitForSingleObject(_Mutex, INFINITE);
		DWORD CaracteresEscribidos = 0;
		static wchar_t Txt[512];
		DWORD TmpTick = GetTickCount();
		int TotalCaracteres = swprintf_s(Txt, 512, L"[%.8d] %s", TmpTick - _UltimoTick, Texto.c_str());
		_UltimoTick = TmpTick;
		BOOL Ret = WriteConsole(_Consola, Txt, TotalCaracteres, &CaracteresEscribidos, NULL);
		ReleaseMutex(_Mutex);
		return Ret;
	}

};