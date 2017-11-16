#ifndef DIMAGELIST_H
    #define DIMAGELIST_H

//    #include "DObjeto.h"
    #include <vector>
	#include <string>
    #include <commctrl.h>
    #include <shlobj.h>

    namespace DWL {


        class DImageList_ID {
          public:
							DImageList_ID(const TCHAR *nPath = NULL, const int nPosIco = 0) : PosIco(nPosIco) { 
							    if (nPath != NULL) Path = nPath; 
							};
            std::wstring	Path;
            int				PosIco;
        };


		class DImageList {
          public:
//			virtual const DEnum_Objeto			Objeto_Tipo(void) { return DEnum_Objeto_ImageList; };
												DImageList(void) : _ImageList(NULL) { };
                                               ~DImageList(void) { Destruir(); };

            void                                Crear(const int cAncho, const int cAlto);
            void                                Destruir(void);

            const int                           AgregarIconoCSLID(const int CSIDL);
            const int                           AgregarIconoRecursos(const int cID);
                                                // Función que carga el icono correspondiente a la ruta especificada
                                                // Por ejemplo si le pasamos "c:\main.cpp" buscara el icono que se utiliza con los archivos cpp
            const int                           AgregarIconoExplorador(const TCHAR *nPath);

            HIMAGELIST                          operator () (void) { 
                                                    return _ImageList;
                                                };
          protected:
            const int                          _BuscarID(const TCHAR *nPath, const int PosIco);
            HIMAGELIST                         _ImageList;

            std::vector<DImageList_ID>         _ListaImagenes;

        };
    };

#endif