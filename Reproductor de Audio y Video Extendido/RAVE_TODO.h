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
		V  Refer totes les ConsultaVarg i ferles amb el Consulta normal... amb el printf no está garantit el que pasa quan un dels parametres es un string NULL.. i en x86 em peta el analisis, pero en x64 no.. 
		V  Analitzar tags dels medis. (libvlc_media_parse_with_options)
			V  Revisar tags dels medis, i trobar coincidencies semblants per unificarles
				V  Fer una llista d'etiquetes (les etiquetes poden ser d'una o mes propietats entre les segûents : Genero, GrupoPath, GrupoTag, DiscoPath, DiscoTag)
		    V  Mostrar tooltip informatiu				
				V  Fet tooltip per les etiquetes
				V  Fet tooltip pels medis
	    V  Treure tots els buttons estandar i posar de meus (falta posar uns icones menys bastos...)
		V  Generar llistes aleatories per etiqueta
			V poder escollir tipus d'etiqueta (estil, grup, disc)
			V Requereix tenir en memoria les etiquetes (que en un futur també servira per fer les cerques de la BD)
			V Fer l'inici amb llistes 
		V  Repintar MenuEx al modificar el estat activat, el texte o l'icona
		V  Crear tooltip abaix a la dreta de la finestra, per avisar a l'usuari de certes tasques, (analisis acabat, llista generada per.., etc...)

		// URGENT //////// (ara es el meu reproductor principal i necesito funcionabilitats bàsiques) /////////////////////////////////
		V  Mirar bug : Pantalla completa + Duplicar pantallas :
			V Pas 1 posar pantalla completa amb l'escriptori ampliat a 2 pantalles
			V Pas 2 Inicio + P -> Duplicado
			V Pas 3 Sortir de pantalla completa
			V Al fer aquests pasos la finestra es queda com maximitzada peró está normal, lo que ocupa tota la pantalla...

		V  Mirar bug : Al acabar un video en pantalla completa, en el següent video ja no surten els ComtrolesPantallaCompleta... :/
			V El bug es soluciona pasan a pantalla nomral, i despres unaltre cop a pantalla completa
		V  Mirar bug : no funciona el shufle ni el repeat desde els controles pantalla completa, bueno ni cap buto.. nomes les barres...
		V  Mirar bug : Matar el timer per ocultar els ControlesPantallaCompleta al fer mouseUp sobre un buto, i suposo que amb les barres també... (NO AMB LA DEL VOLUM, NI AMB LA DEL TEMPS)
		V  Mirar bug : al fer el ShowWindow dels controles pantalla completa el reproductor perd el focus... aviam si ho puc solucionar sense trencar moltes coses...
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		V  Fer que l'espai sigui play / pausa, quan el reproductor te el focus
		V  Eliminar buto pausa i fer-ho tot amb el mateix buto del play

		V  Al arrosegar arxius / directoris, s'ha d'afegir les seves arrels.
			V  Aixo vol dir que s'ha de modificar la funció agregar raiz de forma que cada cop que n'agregui una, comprovi si n'hi ha una o mes per fusionar.

		V  Hi ha un deadlock chunguisim... al reproduir un video, si fas un stop a traves d'un DMenuEx el libvlc_media_player_stop es queda pillat...
			V  GdiPlus eliminat i la cosa segueix igual....
			X  Probar de fer que els menus deixin la finestra creada i nomes la mostrin / ocultin (de moment queda anulat)
			V  El problema sembla que es el SetFocus, si la finestra del reproductor no te el foco es quan peta... (sembla que s'ha solucionat posant el focus a la finestra del reproductor UFF...)
				V ES un problema del DMenuEx a l'hora de controlar el focus amb el submenus...
					V s'ha de revisar _hWndDest i _hWndPadre exactament per que serveixen i com tractarles... (_hWndPadre FORA)
			V  DMenuEx re-adaptat sense SetFocus

		V  Afegit Longitud a les EtiquetaBD per sapiguer el tamany en bytes d'una etiqueta
			V  Afegit a la talua BD
			V  Afegit a les funcions obtenir / guardar
			V  Afegit en el ThreadAnalizar per sumar cada longitud

	    V  Solucionar glitch amb la llista i l'arbre, per exemple busca Symphony X sense tenir "Pelis i Series" afegit com arrel, y expandeix-lo
			V  Arbre solucionat, i a la llista no m'ha tornat a pasar res raro...
		V  Solucionar problema amb el buto PREV, que pot deixar al MedioActual a -1, per lo que calen dos NEXT per pasar al segon medi.		

		X  DMenuEx te que detectar el dobleclick i pasarlo a les finestres que no siguin un DMenuEx
			V  No es posible, pero gracies a aixo he detectat un bug que retornava el menu en MostrarModal fins i tot quan estava desactivat...

		V  Hi ha un bug amb els IID_ITaskbarList3 al posar pantalla completa s'asigna una mascara al área de la finestra del video, pero quan surt de pantalla completa la mascara segueix....
		V  Crear clase per fer animacións, es una mica rotllo TWEEN amb una lambda expresion per el callback
		V  Afegir label pel ratio a VentanaRAVE i a ControlesPantallaCompleta

		V  Separar evento MouseUp i Click en el DBotonEx (Mouseup salta sempre, i el click nomes salta si el mouse está a sobre del control)
		V  Sumar +1 al medi reproduit en la BD.
			V Mostrar en el TooltipBD
		V  Retocada funció RaveBD::FiltroNombre, que filtraba FATAL els espais.. (ara no pot haber 2 espais seguits ni un espai al principi o al final.)
		V  Solucionat bug amb els ControlesPantallaCompleta que no es mostraven correctament a pantalles amb la x negativa
		V  Alineats tots els controls de ControlesPantallaCompleta (principalment labels)

		-  Mostrar les opcions en una finestra nova
			-  Afegir opcions : 
				V BD
					V explicar el tema de les arrels, i posar controls per afegir arrels.
						- Falta refinar el dialeg per seleccionar directoris...
					V Analitzar metadades pendents automaticament al actualitzar la base de dades.
					V Mostrar finestra del analisis
				- General
					X Reproductor sempre devant
					V Buscar actualitzacions (amb la calma... perque haure de mirar si en el github ho puc fer...)
						V Fer la MarcaEx i enllaçar a les opcions de la BD, i el sistema per buscar actualitzacións el deixo per la versió 0.5
							V Deixar el MarcaEx desactivat un cop provi les opcions de la BD
					X Valor del ratio per MouseDown en butons |< >|
					V Teclat amb el focus en el reproductor (play / pausa, stop, volum +, volum -, següent medi, anterior medi, accelerar x2.0, frenar a x0.5)
						X De teclat global (hook) si que haig de mirar la tecla play / pausa , pero el tema del volum amb tecles exteses el porta el sistema operatiu, suposo que tambe interesa el següent i l'anterior medi...
					- Buto per associar els arxius d'audio i video al reproductor a saco, i liquido la pestanya asociaciones.
						- No m'acaba de molar.. aixo ho ha de fer el instalador.
				X Assosiacions d'arxiu
					X Asociacions individuals per cada extensio
					X Mostrar finestra per preguntar pel reproductor predeterminat
						X No se si es en win10 o que.. pero si desde l'explorador esculleixes un reproductor, per molt que fagi les asociacions, aquell tipus sembla que no funciona...
				- Llistes
					- llista d'inici
						- res
						- l'ultima llista
						- etiqueta aleatoria
						- etiqeuta estil aleatoria
						- etiqeuta grup aleatoria
						- etiqeuta disc aleatoria
					- Afegir cançons amb menys de 2.5 de nota a llistes aleatories.
					- No generar llistes amb menys de 3 medis
				- Video
					- Tiempo de inactividad para ocultar los controles
					X Mostrar nombre del video al abrir					
					- Alineación controles pantalla completa
						- Inferior (por defecto)
						- Izquierda
						- Superiorm 
						- Derecha
						- Donde pase el mouse (puff ja t'ho imagines amb el mousemove... xd buscar una solucio smart)
					? Fuente por defecto en subtitulos externos ???? molta matuja.. pero es posible que existeixi algo...


		-  Menu per la llista (val tant per audio com per video)
			- Mostrar en la BD
			- Mostrar directorio
			- Nota
			- Eliminar
			- Propiedades
		- 
		-  Crear un nou correu pels dumps... que pugui fer servir per enviar... (de moment encara tinc web .. xd, no corre molta presa)
		-  Crear instalador (amb lo que tingui el visual studio)
		-  Teclat per la DListaEx (suprimir, shift, control)
		-  Drag & drop intern a la DListaEx per moure items (multiselecció incosa)
		
		-  Anular VentanaAsociarReproductor SENSE ELIMINAR, simplement que no surti sempre, i ja veurem si la liquido del tot en un futur..
			
		-  Test básic en win 7 (nova bd, afegir arrels, buscar, analitzar, generar llista aleatoria, i jugar amb la AimatedToolTipParty
		
	RAVE 0.5
		-  Fer una taula Audio i una Video, i eliminar la taula Medios.... (ANALITZAR)
			- La idea es tenir les opcions suficients per posarles en un arbre (OJU que aixo es un canvi drástic i que a l'hora podria ordenar molt mes tot el projecte, inclus podria suposar crear una taula d'audio u una video separades i no com ara que els dos son un medio)
				- o inclus crear una taula medios global amb un tipo (audio / video / lista / cdaudio), i despres crear una taula per cada un d'ells amb una id
			-  Dividir Base de Datos en : Mi música , i Mis Videos, en 2 butons i en 2 arbres diferents
			-  Afegir buto mis listas


		- Buscar actualitzacións
		-  Crear Icona Buscar en l'arbre de la BD adal a la dreta
	RAVE 0.6
		- Posar icones en el thumbnail de la barra d'inici..
		- Finestra de propietats per un medi o una etiqueta
		- Mostrar la llista de medis al reproduir un medi de forma "diferent", per exemple una liista translucida reduida (pensa que aixó nomes pasara si el reproductor no está maximitzat i está reproduint video)
		- Si la llista / BD (autio/video) está buida mostrar un icona que inciti a arrosegar un arxiu / directori

*/