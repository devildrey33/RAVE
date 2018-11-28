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

	const BOOL      AgregarPath(std::wstring &Path);
	const BOOL      ObtenerPaths(std::vector<std::wstring> &Paths);
	const BOOL      EliminarPaths(void);
	void            Destruir(void);
  protected:
	
	const BOOL	   _Leer(std::wstring &Texto);
	const BOOL	   _Escribir(std::wstring &Texto);
  
	HANDLE		   _hMapFile;
	std::wstring   _Nombre;	
};

