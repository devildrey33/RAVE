/* 
	RAVE 0.3 
	    V  Errors critics i enviar DUPMS
		V  Control DBoton
		V  Control DLabel
		V  Control DToolTip basic
		V  Controls DBarraDesplazamiento, DBarraProgreso, i DBarraScroll
		V  Poder afegir i eliminar arrels a la BD desde les opcions
	   +o- Control DArbolEx
		   +o- Acabar el teclat i el mouse amb el shift
			V  Implementar sub-selecció
			-  Implementar LabelEdit (estic pensant amb un control d'edició personalitzat que tindra DArbolEx i DListaEx de posibilitats) [DEdicionTextoEx]
	   +o- Control DListaEx
			-  Implementar teclat complexe (shifts, controls, av, repag, i tal...) 
	   +o- Control DExplorarDirectorios que crea un arbre amb l'escriptori, MisDocuemntos, MiPC, i Entorno de Red (per poguer seleccionar un directori) [Extensió de DArbolEx]
			+  Directoris desactivats si no hi ha privilegis per explorar-los
		o- Clase adaptable per formar part d'un control DEdicionTextoEx
			-  Edició de textes bàsic amb una sola linea (s'ha de poder especificar la part del control que mostrará el control per editar el texte)
			   +o- Pensat per tenir : una font amb tamany fixe, un color de texte, sombra i un de fondo, amb posibilitat de Resaltat i Presionat
				-  Al final necesita un backbuffer per poguer pintar el texte en un espai limitat 
		V  Clase DTemporizador per crear timers sense necesitat de finestra
	
	
	RAVE 0.4
		V  Asociacions d'arxius amb l'explorador
			V Falta comprobar les asosiacions i mostrar un dialeg com el del bubatronik
	    V  Shuffle / repeat (apagar windows sobretot)
	   +o- Dialeg/finestra per afegir arrels
		V  Control DMenuEx
			V Implementar events interns del mouse
			V Implementar teclat básic


	RAVE 0.5 
		V  Analitzar tags dels medis. (libvlc_media_parse_with_options)
			- Mostrar tooltip informatiu
			- Fer una llista d'etiquetes (per poder buscar i fer llistes aleatories)
				- Aixó requereix tenir un array d'etiquetes amb tipus (Nom, Estil, Grup, Disc), i pasar un algoritme de distancia.... etc...
		-  Treure tots els buttons estandar i posar de meus
		-  Mostrar les opcions en una finestra nova
			-  Afegir opcions : assosiacions d'arxiu, llista d'inici, explicar el tema de les arrels, etc..
				- Analizar metadatos automaticamente, o anializar metadatos solo al reproducir.
		-  Menu per la llista
		-  Crear Icona Buscar en l'arbre de la BD adal a la dreta
	   +o- Solucionar glitch amb la llista i l'arbre, per exemple busca Symphony X sense tenir "Pelis i Series" afegit com arrel, y expandeix-lo
			- Arbre solucionat
		-  Buscar noves actualitzacións
*/