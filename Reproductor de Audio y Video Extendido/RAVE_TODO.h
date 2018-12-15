/* 
	RAVE 0.3 
	    V  Errors critics i enviar DUPMS
		V  Control DBoton
		V  Control DLabel
		V  Control DToolTip basic
		V  Controls DBarraDesplazamiento, DBarraProgreso, i DBarraScroll
		V  Poder afegir i eliminar arrels a la BD desde les opcions
	   +o- Control DArbolEx
		    V  Acabar el teclat i el mouse amb el shift
			V  Implementar sub-selecció
			-  Implementar LabelEdit (estic pensant amb un control d'edició personalitzat que tindra DArbolEx i DListaEx de posibilitats) [DEdicionTextoEx]
	    V  Control DListaEx
			V  Implementar teclat complexe (shifts, controls, av, repag, i tal...) 
	   +o- Control DExplorarDirectorios que crea un arbre amb l'escriptori, MisDocuemntos, MiPC, i Entorno de Red (per poguer seleccionar un directori) [Extensió de DArbolEx]
			V  Directoris desactivats si no hi ha privilegis per explorar-los
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
		V  Solucionat glitch del DAnimacion al Invertir sense que hagi pasat temps l'animacio (perque el vector de valors no habia estat inicialitzat)
		V  Solucionat Bug de la rodeta del mouse en pantalla completa que sempre posava el volum a 10%
		V  Solucionat bug al afegir arxius desde l'explorador que a vegades es liaba a l'hora de reproduir, i feia un pausa quan habia de tancar el medi i posar un de nou a reporduir
		V  Eliminats events de teclat innecesari (VentanaPrincipal, ArbolBD, ListaMedios) que vaig fer abans de fer el hook global del teclat		
		V  DAnimacion
			X Al fer un mouseDown ha d'acabar l'animació a tots els controlEx que tinguin _AniResaltado
			V O unaltre opció es animar tots els estats (marcat, desmarcat, mousedown, mouseup, resaltat, normal, desactivat)
		V  En la DBarraProgresoEx si el minim no es 0, no es calcula be la posició de la barra en el mouse down.
		V  S'ha de implementar alineacions per el control DEtiquetaEx (fins ara nomes hi ha esquerra i centrat, falta la cap a la dreta)
		V  Solucionat bug al DMenuEx que mostrava els menus si el pare estava desactivat
		V  Ara el menu "Proporción" del video s'activa / desactiva segons si el medi actual es video / audio
		V  Crear un DMenuEx amb una barra de desplaçament per la nota i el brillo.
			V He tingut que tornar al model SetFocus, però sense el SetCapture, (nomes es fa el SetCapture al presionar el buto del mouse)
				V No sembla que faci el deadlock com abans, igual es per que ara retorno TRUE en el WM_NCACTIVATE
		V  Mostrar valor en DBarraProgreso / DBarraDesplazamiento (de moment hi ha 4 opcions (int/decimal | valor/maxim i valor)		
		V  Sumar 0.05 a la nota per cada reproducció completa (d'aquesta forma amb 50 reproduccións arribara a 5 estrelles)
		V  Adaptar DBarraProgreso i DBarraDesplazamiento  a varies alineacións : Adal -> Abaix, Abaix -> Adal, Dreta -> Esquerra, Esquerra -> Dreta
			V Adaptar tooltips interns a les alineacións
		V  Mostrar el ControlesPantallaCompleta en varies posicións : Abaix, Esquerra, Adal, Dreta
		V  Eliminar Nota de la taula Etiquetas, la nota s'ha de calcular al fer el tooltip
	    V  Fer animació de la Y i la opacitat al ControlesPantallaCompleta al mostrar/ocultar el control
			X Y descartada de moment la animació es nomes de opacitat...
		V  Mostrar les opcions en una finestra nova
			V  Crear control separador (MOLT SIMPLE) com si fos un </ br>
			V  Afegir opcions : 
				V BD
					V explicar el tema de les arrels, i posar controls per afegir arrels.
						- Falta refinar el dialeg per seleccionar directoris...
					V Analitzar metadades pendents automaticament al actualitzar la base de dades.
					V Mostrar finestra del analisis
				V General
					X Reproductor sempre devant
					V Buscar actualitzacions (amb la calma... perque haure de mirar si en el github ho puc fer...)
						V Fer la MarcaEx i enllaçar a les opcions de la BD, i el sistema per buscar actualitzacións el deixo per la versió 0.5
							V Deixar el MarcaEx desactivat un cop provi les opcions de la BD
					X Valor del ratio per MouseDown en butons |< >|
					V Teclat amb el focus en el reproductor (play / pausa, stop, volum +, volum -, següent medi, anterior medi, accelerar x2.0, frenar a x0.5)
						X De teclat global (hook) si que haig de mirar la tecla play / pausa , pero el tema del volum amb tecles exteses el porta el sistema operatiu, suposo que tambe interesa el següent i l'anterior medi...
					X Buto per associar els arxius d'audio i video al reproductor a saco, i liquido la pestanya asociaciones.
						- No m'acaba de molar.. aixo ho ha de fer el instalador.
					V Velocitat de les animacions
						V Barra o editbox? si es editbox l'haig de fer desde 0 o desde 0.5 aprofitan un TEXTBOX, també tinc un DEdicionTextoEx però está molt verd... (AMB BARRA)
					V Duració del tooltip inferior dreta
				X Assosiacions d'arxiu
					X Asociacions individuals per cada extensio
					X Mostrar finestra per preguntar pel reproductor predeterminat
						X No se si es en win10 o que.. pero si desde l'explorador esculleixes un reproductor, per molt que fagi les asociacions, aquell tipus sembla que no funciona...
				V Llistes
					V llista d'inici 
						V res
						V l'ultima llista
						V etiqueta aleatoria
						V etiqeuta estil aleatoria
						V etiqeuta grup aleatoria
						V etiqeuta disc aleatoria
						X major nota
					V Afegir cançons amb menys de 2.5 de nota a llistes aleatories.
					V No generar llistes amb menys de 5 medis
					V Sumar 0.05 a la nota dels medis reproduits completament
				V Video
					V Temps d'inactivitat per ocultar els controls
					V Opacitat máxima dels controls del video (50-255)
					X Mostrar nom del video al obrir (ja ho fan els tooltipinfo, tot i que es algo petit..)
					V Alineació controls pantalla completa
						V Inferior (per defecte)
						V Esquerra
						V Superior
						V Dreta						
					? Font per defecte en subtituls externs ???? molta matuja.. pero es posible que existeixi algo...
					X Proporción por defecto (Si ho pensem bé el que seria necesari es guardar una proporcio per cada medi, ja que cada video es diferent...)

		V  Menu per la llista (val tant per audio com per video)
			V Mostrar en la BD 
				V oju perque hi ha medis que no tenen per que existir a la BD
			V Mostrar directorio (explorer.exe /select,"C:\Folder\subfolder\file.txt")
			V Nota
			V Eliminar
			V Propiedades (les de l'explorador de moment...)
		V  Repat lista aleatória
		V  Crear un nou correu pels dumps... que pugui fer servir per enviar... (de moment encara tinc web .. xd, no corre molta presa)
			X si es un email de gmail, hotmail, yahoo, etc.. necesito fer un client TLS de email... que es chungo...
				X una opció podria ser recompilar la CSmtp per que funcioni en x64 (les llibreries del OpenSSL son de 32)
				V o lo mes facil (ja que no haig de adaaptar codi) seria trobar una conta que no requereixi encriptació [MERCI BARBA]
	    V  Crear instalador (amb lo que tingui el visual studio)
			V  Falta fer el tema del registre de windows (associar arxius)
			V  Falta porbar en win7 aviam si li he posat els runtimes que toca
				- Els runtimes funcionen, lo que necesito un win7 en condicions... (format al canto... però amb la calma, o igual un virtual box es mes facil)
		V  Teclat per la DListaEx i el DArbolEx (suprimir, shift, control)
			V  DListaEx Shift : Cursors, AvPag, Repag, Inicio, Fin, MouseClick.  Control : MouseClick.
		
		V  Pasar les opcions Opciones_VentanaAsociar_Pos* a la finestra dels dialegs, i renombrarles. (En la BD segueix el nom igual per compatibilitat..., i aixi reaprofito 2 valors que no s'utilitzaven)

		V  Anular VentanaAsociarReproductor SENSE ELIMINAR, simplement que no surti sempre, i ja veurem si la liquido del tot en un futur..
			V Amb l'instalador puc posar entrades al registre, aixi que en principi es pot eliminar la VentanaAsociarReproductor i el AsociaciarReproductor (UN COP FACI EL REGISTRE AL INSTALADOR, OFCOURSE...)			
				V De moment encara les conservare com a DEPRECATED
		X  Test básic en win 7 (nova bd, afegir arrels, buscar, analitzar, generar llista aleatoria, i jugar amb la AnimatedToolTipParty
			+o- Instalar amb els instalador (x86) (x64 FUNCIONA)
		 	 V  A la merda, que ho provi el krusti, i ja em dira xd
		 
		V  Al afegir arxius externs NO S'HA D'AGREGAR L'ARREL.
		V  Al fer click amb el buto dret sobre l'arbre, si nomes hi ha un seleccionat, es desselecciona, i es selecciona el item que hi ha sota el mouse.
		V  No es permet afegir arrels a la llista
		V  El tamany de les columnes de DListaEx no es calcula correctament.
		V  Re-retocat el icona IDI_CHECK, i ara he afegit el icona IDI_NOCHECK
		V  Posar icones en el thumbnail de la barra d'inici..

	RAVE 0.6
		V  ToolTipBD ha de ser integrat a ToolTipInfo, per poguer mostrar tooltips amb columnes en els tooltipinfo
			V  De moment he fet una clase base ToolTipInfo, i ara hi ha el unaltre clase ToolTipInfo_Texto que hereda de ToolTipInfo, falta el tipus ToolTipInfo_Medio.
			V  Molaria que compartisin windowrpocedure tots els tipus de tooltip.
			V  Objecte ToolTipBD eliminat, ara l'arbre i la llista utilitzen ToolTipInfo_Medio i ToolTipInfo_Etiqueta
			V  S'ha de posar el tooltip del play amb un ToolTipInfo_Medio
				V Solucionar bug al reposicionar els tooltips que es posicionen uns pixels per sobre
			V  S'ha de suavitzar el mostrar dels tooltips, ja que si es fa un doble click es talla l'animacio i torna a començar
			V  Pintar la nota amb icones d'estrelles als ToolTipInfo_Medio i ToolTipInfo_Etiqueta
			-  Missio compactar al máxim el tooltip
				V Pas 1 Afegir la pista al nom, aixi trec una fila (uns 20px px d'altura)
				- Pas 2 Si el nom + pista supera en un 140% del tamany necesari per les columnes s'ha de partir
					- (buscar exemples)

		V  Afegir funcións de temps a les DAnimacion i DAnimacion_Color
			V Fusionat DAnimacion_Color amb DAnimacionm ara puc fer animacions decimals i de color desde el mateix objecte a l'hora.
			V Retocats els parametres de les funcions lambda, ja no apuntan a vectors blancs, ara apunten a un objecte que fa de vector i de helper

		V  Els tooltips d'etiquetes han de generar totes les seves dades (nota, tiempo, longitud, totalmedios)
			X  S'ha de treure del objecte EtiquetaBD els membres : Medios, Tiempo, Longitud (i de rebot he recuperat el Nota)
				X  També s'ha de treure de la BD
			V  Lo millor es anar refrescant les dades al mostrar el tooltip, haig de fer un multisqlselect chachi 
			-  No hi han etiquetes pels videos de la BD...
		
		V  Tractar arxius del tipus CRDOWNLOAD i OPDOWNLOAD per renombrar-los al acabar de reproduïr, OFC també s'haura de guardar el nou path a la BD.

		V  Solucionar problema amb el manifest que no em deixa obtenir el SO correctament i a partir de Win8 ja no em diu el SO correcte.		
		V  Solucionar Follón de l'ordre de les finestres amb els tooltips quan hi ha la finestra opcions o la del threadanalizar
		V  Solucionar bug dels ControlesPantallaCompleta que no es mostren despres d'acabar un medi (no val apretar el Next, ha de ser acabat totalment)

		V  DBarraScrollEx
			V  La barra del scroll ha de tenir un mínim PERCEPTIBLE (no val 1 pixel xd)
			V  Els calculs no estan ben fets, s'ha de restar la pagina al total, i mirar si em resulta mes facil calcular-ho de 0.0 a 1.0 i no de 0.0 a 100.0
				V Aixó implica revisar calculs en DArbolEx i DListaEx...
					V Revisar el teclat que fa coses rares amb els dos controls
			V  Direfenciar estat de la barra resaltat fondo, barra
			V  Afegir transicións de resaltat i presionat
			V  Minimitza el G:\MP3... pasa algo raro, hi ha mes barra de escroll que fondo..
				V  I ja de pas obre pelis i series, -> startrek i flipa amb el scrollH
			V  Afegir borde amb transició

		-  DArbolEx 
			V  Separar l'icona un pel mes del texte (per que no quedi l'icona enganxat a la selecció)  [subgerència : afegir un doble padding en ves d'un padding normal]
			V  Refer Shift + AvPag & RePag
		    V  Fer expansors amb un triangle, i deixar les dues opcions.
				V També he refet els expansors normals, i ara nomes utilitzen un color
					V Transicións pels expansors de resaltat i presionat
			V  Transicións de resaltat i presionat
			V  Afegir borde amb transició
			V  MouseDown y MouseUp s'han de fer amb un SetCapture
			V  Acabar el teclat amb els caracters imprimibles (fer que es pugui tenir mes d'un caracter amb un temporitzador)
			V  Fer resaltat del fondo amb el resaltat del borde... OJU perque será un pilotaço, ja que cada nodo te el seu porpi color... 
				X  Subgeriment.. el color del fondo del nodo ha de ser un punter que apunti al color del fondo general de l'arbre, i quan faci animacións ha de crearse el seu propi punter (obviament al acabar haig de tornar a apuntar al color del fondo general)
					V   Quan el color del fondo del nodo sigui com el color de fondo arbre normal/resaltat, no s'ha de pintar.
					
			-  Resaltar texte buscat (ha de ser una capa per pintar el fondo abans de pintar el texte)

		-  DListaEx
			V  Al fer click amb el buto dret, detectar si nomes hi ha un item seleccionat, per desseleccionar tot o no,
			-  Drag & drop intern a la DListaEx per moure items (multiselecció inclosa)
			V  Transicións de resaltat i presionat		
			V  Afegir borde amb transició
			-  Resaltar texte buscat (ha de ser una capa per pintar el fondo abans de pintar el texte)
			V  Quan no hi ha scroll la marca ha d'arribar fins al tope de la dreta
			V  MouseDown y MouseUp s'han de fer amb un SetCapture
		-  ListaMedios
			-  Mostrar el % del medi reproduit en el item que es el MedioActual
				- Necesitare crear virtuals a DListaEx

		V  Mostrar en la BD no funciona correctament.., proba-ho amb Simphpony X -> Prelude to the new milenium
			V  S'ha solucionat al calculat bé la DBarraScrollEx (com ja imaginava... era per la diferencia de la barra)

		V  DMenuEx
			V  MostrarMenu i _MostrarSubMenu ha de detectar la pantalla on es mostra per sapiguer si te suficient espai per mostrar el submenu o l'ha de posar a l'altre banda
			V  Afegir transició al mostrar (porbar ample i altura amb la bounce que vaig fer)
				V  Es millor fer la opacitat, pels submenus mes que res.. (NO AFEGIR TRANSICIÓ AL OCULTAR)
			X  Fer alphablend amb els icones quan el menu está desactivat (amb la transició de desactivat)
				V Sense GDI+ chungo.. i el veig molt desfasat
			V  Retocat el calcul de la mida del menu per mirar si hi han submenus amb fills, i deixar un espai per la marca
			-  S'ha de mirar si hi ha algun submenu amb icones, i si no n'hi ha cap treure l'espai del principi de l'icona

		-  DDlgDirectorios
			-  Captar WM_RESIZE i modificar tamany dels controls en consequencia.
				- Guardar el tamany a les opcions de la BD

		V  S'ha de poder afegir medis desde l'explorador mentres es carrega la libvlc.
			-  s'ha de probar desde la RELEASE quan em carregui la libvlc per primer cop

		-  Afegir al MenuBD : 
			-  Nota (per poguer modificar la nota de tots els medis d'un directori)
			-  Abrir carpeta
			-  Propiedades
			-  Orden!?!?! saps el follón que suposa no? xd ja veurem... jo no el trobo a faltar...

		-  Buscar actualitzacións
		-  Afegir a les etiquetes les arrels, sense medir la distancia

		-  Afegir tecles ràpides F1, F2, F3 -> Mostrar medio actual en : Tooltip, BD, Lista
			V  F1
		-  Afegir menu tipo Marca, que serveixi tant com a menú, o com a marca pel seu menú pare.
			- La idea es que el menu generar llista mostri els seus sub-menus amb una marca, la marca al presionarla no oculta el menú, i despres si presionem en el pare (generar lista) generara una llista del que estigui marcat.
				- aixó necesitara guardar les marques en la BD, i aixi es "personalitza" el tema de generar llisters aleatories.
			- I ja que hi som, un nou tipus de llista aleatoria podria estar basat en el jamendo... el VLC pot carregar llistes jamendo... osigui que no hauria de ser un problema imposible.
		

		-  Fer una taula Audio i una Video, i eliminar la taula Medios.... (ANALITZAR IDEA..)
			- La idea es tenir les opcions suficients per posarles en un arbre (OJU que aixo es un canvi drástic i que a l'hora podria ordenar molt mes tot el projecte, inclus podria suposar crear una taula d'audio u una video separades i no com ara que els dos son un medio)
				- o inclus crear una taula medios global amb un tipo (audio / video / lista / cdaudio), i despres crear una taula per cada un d'ells amb una id
			-  Dividir Base de Datos en : Mi música , i Mis Videos, en 2 butons i en 2 arbres diferents
			-  Afegir buto mis listas
		
		-  Crear Icona Buscar en l'arbre de la BD adal a la dreta (m'estic plantejant si el necesito també per la llista, i si l'haig d'implementar com una part del DArbolEx i la DListaEx)
		-  Si la llista / BD (autio/video) está buida mostrar un icona que inciti a arrosegar un arxiu / directori

		-  La llista s'ha de poguer veure desde els ControlesPantallaCompleta
			-  Mostrar la llista de medis al reproduir un video de forma "diferent", per exemple una liista translucida reduida (pensa que aixó nomes pasara si el reproductor no está maximitzat i está reproduint video)

		V  Hi ha un bug raro al afegir un medi desde l'explorador (inician el reproductor amb -r pathmedi), i encara no he aconseguit reproduir-lo..
			V Sempre que pasa surt el thread analizar (posible solució : utilitzar la funció del drag&drop per carregar els arxius que venen en els ARGS)
				X  DE MOMENT NO HA PASAT MES DESDE QUE VAIG ARRECLAR L'EXTENSIO CRDOWNLOAD.
				V  Ha tornat a pasar al grabar un directori nou dintre d'una arrel existent, pero aquest cop m'ha deixat fer play desde la llista
				V  Ho he estat probant en debug sense BD i ja no pasa mes :/
				V  El problema era que asigno ? a la lletra d'unitat al afegir el medi en la BD, ara afegire la unitat corresponent, però no sera retrocompatible amb la ID del disc dur.

		-  Separar el anáilisis del tag i la obtenció de dades de les etiquetes

		-  Opcions 
			-  Llista
				-  Mezclar listas aleatoíras generadas por genero, grupo, 50 canciones.
				-  No Mezclar listas aleatórias generadas por disco.
			-  General 
				- Ocultar tooltips al pasar por encima con el mouse
				- Tamaño tooltips (fer 3 mides : petit, normal [que es la actual], gran)

		-  Les llistes aleatóries per estil, grup, 50 can han d'activar el shufle
			- Les llistes aleatóries per disc han de desactivar el shufle

	RAVE 0.7
		-  Carregar subtituls
		-  Carregar llistes m3u
		-  Carregar pistes CDAudio
		-  Suport per dvds de video
		-  Actualitzar la BD automáticament al detectar una nova unitat per si es un cd d'audio o un DVD de video.
		-  Finestra de propietats per un medi o una etiqueta
		-  Sistema per fer skins amb un txt que pugui utilitzar definicions de colors i funcions de colors per sumar/ restar al rgb
			V  De moment tots els controls ex tenen un Objecte_Skin que te tots els colors i fonts per defecte.

*/