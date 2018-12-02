#pragma once



// Este objeto asocia el reproductor a windows de varias formas :
// - Añade este reproductor a la lista de reproductores de archivos (mpg, avi, mp3, mp4, etc...)
class AsociarReproductor {
  public:
						AsociarReproductor();
	                   ~AsociarReproductor();
						// Registra la aplicación y asigna las extensiones
	void				RegistrarApp(void);
						// Elimina las associaciones de archivo y los datos de la aplicación (en el registro de windows)
	void				DesRegistrarApp(void);
						// Comprueba si las asociaciones de los medios en el explorador ejecutaran este reproductor
	const BOOL          ComprobarAsociaciones(void);
};

