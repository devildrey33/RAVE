#pragma once
class MemoriaCompartida {
  public:
					MemoriaCompartida(void);
				   ~MemoriaCompartida(void);
					
					// Función que crea una página de memória compartida
	const BOOL		Crear(std::wstring &nNombre);
	const BOOL		Crear(const wchar_t *nNombre);
					// Función que obtiene una página de memória compartida, previamente creada en otra instancia con la función Crear
	const BOOL		Obtener(std::wstring &nNombre);
	const BOOL		Obtener(const wchar_t *nNombre);

	void            Destruir(void);
	
	const BOOL		Leer(std::wstring &Texto);
	const BOOL		Escribir(std::wstring &Texto);
  protected:
	HANDLE		   _hMapFile;
	std::wstring   _Nombre;	
};

