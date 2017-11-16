#include <string>
#include <vector>

typedef unsigned __int64  UINT64;

class TMedioAudio;
// clase para guardar los tipos string iniciales (tipos , generos , grupos , discos , raíz)
class StrTp {
  public:
								StrTp(void) : Escuchado(0) { };

	std::wstring				Nombre;
	unsigned long				Escuchado; // (DWORD)
	std::vector<TMedioAudio *>	Medios;
};