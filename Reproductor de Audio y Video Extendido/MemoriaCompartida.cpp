#include "stdafx.h"
#include "MemoriaCompartida.h"
#include "DStringUtils.h"

#define TAM_BUFFER (4096 *4) * sizeof(wchar_t)

MemoriaCompartida::MemoriaCompartida(void) : _hMapFile(NULL) {
}

MemoriaCompartida::~MemoriaCompartida(void) {
	Destruir();
}

const BOOL MemoriaCompartida::Crear(std::wstring &nNombre) {
	_Nombre = nNombre;

	Debug_Escribir_Varg(L"MemoriaCompartida::Crear %s\n", _Nombre.c_str());

	_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, TAM_BUFFER, _Nombre.c_str());

	if (_hMapFile == NULL) {
		Debug_MostrarUltimoError();
		return FALSE;
	}

	return TRUE;
}

const BOOL MemoriaCompartida::Crear(const wchar_t *nNombre) {
	_Nombre = nNombre;

	Debug_Escribir_Varg(L"MemoriaCompartida::Crear %s\n", _Nombre.c_str());

	_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, TAM_BUFFER, _Nombre.c_str());

	if (_hMapFile == NULL) {
		Debug_MostrarUltimoError();
		return FALSE;
	}

	return TRUE;
}

const BOOL MemoriaCompartida::Obtener(std::wstring &nNombre) {
	_Nombre = nNombre;

	Debug_Escribir_Varg(L"MemoriaCompartida::Obtener %s\n", _Nombre.c_str());

	_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _Nombre.c_str());

	if (_hMapFile == NULL) {
		Debug_MostrarUltimoError();
		return FALSE;
	}

	return TRUE;
}

const BOOL MemoriaCompartida::Obtener(const wchar_t *nNombre) {
	_Nombre = nNombre;

	Debug_Escribir_Varg(L"MemoriaCompartida::Obtener %s\n", _Nombre.c_str());

	_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _Nombre.c_str());

	if (_hMapFile == NULL) {
		Debug_MostrarUltimoError();
		return FALSE;
	}

	return TRUE;
}


const BOOL MemoriaCompartida::AgregarPath(std::wstring &Path) {
	std::wstring Tmp;
	// Si hay un error al leer el contenido, salgo
	if (_Leer(Tmp) == FALSE) return FALSE;
	// Agrego el path
	if (Tmp.size() > 0 )	Tmp += L';' + Path;
	else					Tmp = Path;
	// Guardo los paths en la memoria
	return _Escribir(Tmp);
}


const BOOL MemoriaCompartida::ObtenerPaths(std::vector<std::wstring> &Paths) {
	Debug_Escribir_Varg(L"MemoriaCompartida::ObtenerPaths %s\n", _Nombre.c_str());
	std::wstring Tmp;
	// Si hay un error al leer el contenido, salgo
	if (_Leer(Tmp) == FALSE) return FALSE;

	DWL::Strings::Split Sp(Tmp, L';');
	for (size_t i = 0; i < Sp.Total(); i++) {
		Paths.push_back(Sp[i]);
	}

	return TRUE;
}


const BOOL MemoriaCompartida::EliminarPaths(void) {
	Debug_Escribir_Varg(L"MemoriaCompartida::EliminarPaths %s\n", _Nombre.c_str());

	LPCTSTR pBuf = (LPTSTR)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, TAM_BUFFER);

	if (pBuf == NULL) {
		Debug_MostrarUltimoError();
		return FALSE;
	}
	ZeroMemory((PVOID)pBuf, TAM_BUFFER);

	UnmapViewOfFile(pBuf);

	return TRUE;
}








const BOOL MemoriaCompartida::_Leer(std::wstring &Texto) {
	Debug_Escribir_Varg(L"MemoriaCompartida::_Leer %s\n", _Nombre.c_str());
	
	LPCTSTR pBuf = (LPTSTR)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, TAM_BUFFER);

	if (pBuf == NULL) {
		Debug_MostrarUltimoError();
		return FALSE;
	}

	Texto = pBuf;

	UnmapViewOfFile(pBuf);

	return TRUE;
}

const BOOL MemoriaCompartida::_Escribir(std::wstring &Texto) {
	Debug_Escribir_Varg(L"MemoriaCompartida::_Escribir %s\n", _Nombre.c_str());

	LPCTSTR pBuf = (LPTSTR)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, TAM_BUFFER);

	if (pBuf == NULL) {
		Debug_MostrarUltimoError();
		return FALSE;
	}
	ZeroMemory((PVOID)pBuf, TAM_BUFFER);
	CopyMemory((PVOID)pBuf, Texto.c_str(), (Texto.size() * sizeof(wchar_t)));

	UnmapViewOfFile(pBuf);

	return TRUE;
}

void MemoriaCompartida::Destruir(void) {
	Debug_Escribir_Varg(L"MemoriaCompartida::Destruir %s\n", _Nombre.c_str());

	if (_hMapFile != NULL) {
		CloseHandle(_hMapFile);
		_hMapFile = NULL;
		_Nombre.resize(0);
	}
}