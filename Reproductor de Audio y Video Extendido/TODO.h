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
			-  Revisar expandir nodo (si redimensiones la finestra al minim i fas doble click a motorhead veuras.... s'hauria de quedar el nodo pare a _NodoInicioPagina)
			-  Falta fer una funció HacerVisible(Nodo) per solucionar el problema de la linea d'adalt
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
			- Falta comprobar les asosiacions i mostrar un dialeg com el del bubatronik
	    V  Shuffle / repeat (apagar windows sobretot)
	   +o- Dialeg/finestra per afegir/eliminar arrels


	RAVE 0.5 
		-  Analitzar tags dels medis.
			- Mostrar tooltip informatiu
		-  Treure tots els buttons estandar i posar dels meus
		-  Crear l'objecte DMenuEx
		-  Afegir opcions : assosiacions, llista d'inici, etc..
		-  Menu per la llista


*/