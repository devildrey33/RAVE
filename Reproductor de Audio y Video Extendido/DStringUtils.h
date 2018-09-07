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

		// Cuenta dentro de un string las veces que aparece un carácter
		const size_t DString_ContarCaracter(std::wstring &Texto, wchar_t Caracter);


		/* Convierte cualquier valor decimal a cadena de caracteres, se puede especificar si se quiere un número de decimales. */
		template <typename T> std::wstring DString_ToStrF(T Tmp, int Decimales = 0) {
			std::wostringstream out;
			out << std::fixed << std::setprecision(Decimales) << Tmp;
			return out.str();
		}



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
		template <typename T> T DString_StrTo(std::wstring &Tmp, T output) {
//			T output;
			std::wistringstream in(Tmp);
			in >> output;
			return output;
		}


		static const BOOL DString_AnsiToWide(char *IN_Ansi, std::wstring &OUT_Wide) {
			wchar_t TmpWTxt[4096];
			int Ret = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, IN_Ansi, -1, TmpWTxt, 4096);

			OUT_Wide = TmpWTxt;
			return (Ret > 0) ? TRUE : FALSE;
		}



		/* Devuelve un string con el numero de bytes en formato legible (Bytes, KiloBytes, MegaBytes, GigaBytes, TeraBytes) */
		static std::wstring &DString_FormatoBytes(const long long nBytes, std::wstring &OUT_StrBytes) {
			const wchar_t  *Medidas[5] = { L"B", L"KB", L"MB", L"GB", L"TB" };
			int				PosMedidas = 0;
			long double		Resultado  = static_cast<long double>(nBytes);
			long long       Bytes	   = nBytes;
			for (PosMedidas = 0; PosMedidas < 5 && Bytes >= 1024; PosMedidas++, Bytes /= 1024) {
				Resultado = static_cast<long double>(Bytes) / 1024.0f;
			}

			OUT_StrBytes = DString_ToStrF(Resultado, 2) + Medidas[PosMedidas];

			return OUT_StrBytes;
		}
	};
#endif