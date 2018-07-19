#pragma once
class MemoriaCompartida {
  public:
					MemoriaCompartida(void);
				   ~MemoriaCompartida(void);
					
					// Funci�n que crea una p�gina de mem�ria compartida
	const BOOL		Crear(std::wstring &nNombre);
	const BOOL		Crear(const wchar_t *nNombre);
					// Funci�n que obtiene una p�gina de mem�ria compartida, previamente creada en otra instancia con la funci�n Crear
	const BOOL		Obtener(std::wstring &nNombre);
	const BOOL		Obtener(const wchar_t *nNombre);

	void            Destruir(void);
	
	const BOOL		Leer(std::wstring &Texto);
	const BOOL		Escribir(std::wstring &Texto);
  protected:
	HANDLE		   _hMapFile;
	std::wstring   _Nombre;	
};

