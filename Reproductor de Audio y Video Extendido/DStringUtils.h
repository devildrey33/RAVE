#ifndef DSPLIT_H
	#define DSPLIT_H

//	#include "DObjeto.h"
	#include <vector>
	#include <string>
	#include <iomanip>
	// DString_ToStr, DString_StrTo
	#include <sstream>

	namespace DWL {

		/* Split para separar una cadena por un carácter delimitador */
		class DSplit {
		  public:
//			virtual const DEnum_Objeto	Objeto_Tipo(void) { return DEnum_Objeto_Split; };
										DSplit(std::wstring &Texto, const TCHAR Separador);
										DSplit(const TCHAR *Texto, const TCHAR Separador);
			                           ~DSplit(void);
			inline const size_t			Total(void) { return _TextoSeparado.size();  };
			std::wstring               &operator[] (const size_t Pos);
			const bool					operator() (void);
		  protected:
			void					   _SepararWide(const TCHAR *Texto, const TCHAR Separador);
			std::vector<std::wstring>  _TextoSeparado;
		};



		/* Convierte cualquier tipo básico a cadena, se puede especificar si se quiere un número fijo de digitos */
		template <typename T> std::wstring DString_ToStr(T Tmp, int Digitos = 0) {
			std::wostringstream out;

			if (Digitos > 1) 
				out << std::setfill(L'0') << std::setw(Digitos) << Tmp;
			else 
				out << Tmp;
			
			return out.str();
		}


		/* Convierte una cadena a un tipo básico */
		template <typename T> T DString_StrTo(std::wstring Tmp) {
			T output;
			std::wistringstream in(Tmp);
			in >> output;
			return output;
		}
	};
#endif