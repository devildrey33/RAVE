#include "stdafx.h"
#include "DStringUtils.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace DWL {

	static std::wstring StringVacio;

	DSplit::DSplit(std::wstring &Texto, const TCHAR Separador) {
		_SepararWide(Texto.c_str(), Separador);
	}

	DSplit::DSplit(const TCHAR *Texto, const TCHAR Separador) {
		_SepararWide(Texto, Separador);
	}


	DSplit::~DSplit(void) {
	}


	std::wstring &DSplit::operator[](const size_t Pos) {
		if (Pos >= _TextoSeparado.size())	return StringVacio;
		else								return _TextoSeparado[Pos];
	}

	const bool DSplit::operator() (void) {
		return (_TextoSeparado.size() > 0);
	}


	void DSplit::_SepararWide(const TCHAR *Texto, const TCHAR Separador) {
		std::wstringstream Stream(Texto);
		std::wstring Tmp;
		while (std::getline(Stream, Tmp, Separador)) {
			if (!Tmp.empty())
				_TextoSeparado.push_back(Tmp);
		}
	}






	const size_t DString_ContarCaracter(std::wstring &Texto, wchar_t Caracter) {
		size_t Total = 0;
		for (size_t i = 0; i < Texto.size(); i++) {
			if (Texto[i] == Caracter) Total++;
		}
		return Total;
	}

};