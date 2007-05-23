/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 **************************************************************************/

/***************************************************************************
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                            *
 *   ROM has been brought to you by the ROM consortium                     *
 *       Russ Taylor (rtaylor@hypercube.org)                               *
 *       Gabrielle Taylor (gtaylor@hypercube.org)                          *
 *       Brian Moore (zump@rom.org)                                        *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ROM license, in the file Rom24/doc/rom.license                        *
 **************************************************************************/

/***************************************************************************
 *   DREAM.C BY ANDREW TOLMASOFF 2004 (C) QuickMUD Advanced                *
 ***************************************************************************/


#include <stdio.h>
#include "xandra.h"
#include "interp.h"


void dream_update( void )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA		*ch;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		ch = d->character;

		if ( d->character == NULL || d->connected != CON_PLAYING )
			continue;

		if ( IS_MOB(ch) )
			return;

		/* If the character is asleep. */
		if ( IS_AWAKE(ch) )
			continue;

		if ( ch->dreamn == 0 )
		{
			switch ( ch->dreamp )
			{
			case 0:
				send_to_char(
					"Un ticchettio regolare avanza lento, come pioggia fresca e leggera, poi saluta\n\r"
					"l'udito svanendo piano..\n\r"
					"E scendono lente gocce di liquido corallo, a colorare man mano ogni angolo\n\r"
					"visivo: la pace, l'odore dei fiori pallidi dopo la pioggia, boschi bagnati e\n\r"
					"agrifogli brillanti, i suoni timidi del risveglio delle creature alate..\n\r"
					"Ed e' tepore, ora.\n\r", ch );
				ch->dreamp++;
				break;
			case 1:
				send_to_char(
					"Il ticchettio riprende, piu' ritmato forse, ma ancora cede il passo al silenzio,\n\r"
					"presto..\n\r"
					"E improvvisi fasci di colore solido quasi solchi di colore intenso, rigano lo\n\r"
					"sguardo impadronendosene celermente: giallo acceso e sul finire arancio; arde il\n\r"
					"calore e tutto invade, ma non soffoca, si dipana esistendo, e' tattile ma non\n\r"
					"opprimente.. Ventagli dorati ruotano come pavoni leggiadri in vanitose danze e\n\r"
					"il grano scivola tra le loro flessuose pieghe come perle baciate dal sole..\n\r", ch );
				ch->dreamp++;
				break;
			case 2:
				send_to_char(
					"Ritorna adagio a ticchettare il cuore, seguendo un rallentamento quasi\n\r"
					"innaturale ma inevitabilmente tace, dopo..\n\r"
					"E un fuoco divampa da sotto e il cuore dietro di balzi l'accompagna come ad\n\r"
					"alimentare la vivida fiamma, poi diventa compagno e si veste di canti pacati,\n\r"
					"crepitando sommessamente: e i legni bruni a disegnare stelle mentre le fiamme si\n\r"
					"piegano nascondendosi tra i loro abbracci, lambendoli piano e a lungo.. L'aria\n\r"
					"profuma di castagni..\n\r", ch );
				ch->dreamp++;
				break;
			case 3:
				send_to_char(
					"E mentre il ticchettio riparte, la sera e' diventata corta e segue morendo il\n\r"
					"silenzio che torna rapido..\n\r"
					"E il bianco arriva come la fine di un'onda spumosa e rischiara abbagliando, poi\n\r"
					"si rompe in mille e piu' gocce e il vento le solleva gelandole: le protegge di\n\r"
					"ghiacci eleganti e di vitree corolle mentre, screziando il mondo, il grigio\n\r"
					"s'avvicina.. E' freddo ma il corpo si tempra e la terra si prepara a rinascere\n\r"
					"ancora e ancora..\n\r", ch );
				ch->dreamp++;
				break;
			case 4:
				send_to_char( "E forse e' passato un anno.\n\r", ch );
				dream_reset( ch );
				break;
			}
		}
		else if ( ch->dreamn == 1 )
		{
			switch ( ch->dreamp )
			{
			case 0:
				ch->dreamp++;
				break;
			case 1:
				send_to_char(
					"Ti svegli. Forse. Il gentile soffio di una fresca brezza ti sfiora.. accarezza\n\r"
					" il tuo sguardo portandolo lontano.. tra nubi e frastagliati colli.. al di la'\n\r"
					" dell'orizzonte e poi ancora indietro tra lussureggianti selve.\n\r"
					" Una calma placida e serena ti avvolge e ora fluttui tra i venti come in una\n\r"
					" sfera di calda luce, liberta' e pensiero, nulla ora sfugge.\n\r", ch );
				ch->dreamp++;
				break;
			case 2:
				send_to_char(
					"L'eco di un volteggiar d'ali ti giunge da lontano.. e strepitar di roboanti\n\r"
					"tuoni e agghiaggianti urla.. ma s'avvicinano pur rimanendo lontani, a guisa\n\r"
					"d'evanescenti ombre senza materia.. non ti sfiorano e non ti vedono.. pur\n\r"
					"rimanendo attori al tuo sguardo maesta' alate in duello, tra echi di ghiaccio\n\r"
					"e vampate di fuoco..\n\r"
					"Tutto sfuma..\n\r", ch );
				ch->dreamp++;
				break;
			case 3:
				send_to_char(
					"S'affievolisce il clamore della singolar tenzone, dentro e' ancora pace..e lenta\n\r"
					"calma. Delizioso un arpeggio sottende l'incanto.. migliaia di corde al vento\n\r"
					"sfiorano e colorano l'aria.\n\r", ch );
				ch->dreamp++;
				break;
			case 4:
				send_to_char(
					"Un corvo e' li' appollaiato, ti fissa con i suoi occhi torvi.. e cosi' parla:\n\r"
					"Sette destrieri per sette lunghi viaggi\n\r"
					"Tre di essi periranno\n\r"
					"Tre di quelli bruceranno\n\r"
					"Dalle vive fiamme sara' la nuova era\n\r"
					"Sconfitto il drago che avra' la vittoria\n\r"
					"Perduto sara' il regno che non ha storia\n\r"
					"Scacciata la menzogna sara' l'alba piu' vera\n\r", ch );
				dream_reset( ch );
				break;
			}
		}
		/* This dream is a Nightmare! */
		else if ( ch->dreamn == 2 )
		{
			switch ( ch->dreamp )
			{
			case 0:
				send_to_char(
					"Una soave musica raggiunge la tua mente, il suono di mille flauti, canti di\n\r"
					"triste melodia e di infinita dolcezza.\n\r"
					"Dura pochi istanti.. c'e' qualcosa. Una gelida stilettata sembra attraversarti\n\r"
					"i pensieri. Un improvviso biancore ti abbaglia, poi il buio.\n\r", ch );
				ch->dreamp++;
				break;
			case 1:
				send_to_char(
					"Cammini, ma senza guidare i tuoi passi. Ti guardi intorno ma vorresti chiudere\n\r"
					"gli occhi, fermarti, andar via. Gli alberi sembrano opprimerti.. ed hanno\n\r"
					"mille occhi.. mille fiamme che si muovono tra le foglie.\n\r", ch );
				ch->dreamp++;
				break;
			case 2:
				send_to_char( "Continui a camminare.. il silenzio ora avvolge tutto. Riesci a sentire solo il\n\r"
					"battito del tuo cuore.. e ad suo ritmo si muovono i tuoi passi.\n\r"
					"Veloci ardono le fiamme puntate su di te, schizzano tra un albero e l'altro,\n\r"
					"da un cespuglio a quello vicino, senza sosta, stai correndo, ora.\n\r", ch );
				ch->dreamp++;
				break;
			case 3:
				send_to_char(
					"Improvvisamente tutto si ferma. Ancora il silenzio.. ti circondano gli alberi,\n\r"
					"le gambe immobili.. non vedi via d'uscita.\n\r"
					"Ancora quella sensazione che qualcuno ti osservi.. All'unisono si aprono gli\n\r"
					"occhi tra le foglie e nei cespugli.\n\r", ch );
				ch->dreamp++;
				break;
			case 4:
				send_to_char(
					"Un sibilo. Quasi impercettibile.. gli altri non li conti, la prima freccia ti\n\r"
					"ha gia' colpito al cuore.\n\r", ch );
				send_to_char(
					"\n\rCerchi disperatamente di toglierti la freccia, il tuo ultimo e vano atto, ma\n\r"
					"afferri solo aria. Spasmodicamente riprovi, sempre con lo stesso risultato.\n\r"
					"Capisci d'esserti svegliato allora.\n\r", ch );
				do_wake( ch, "" );
				dream_kill( ch );
				break;
			}
		}
		else if ( ch->dreamn == 3 )
		{
			switch ( ch->dreamp )
			{
			case 0:
				send_to_char(
					"Una vaga sensazione d'umido ti costringe ad aprire gli occhi e scopri con\n\r"
					"ironico terrore che un echidna ti sta leccando la fronte..\n\r", ch );
				ch->dreamp++;
				break;
			case 1:
				send_to_char(
					"Dormire a viso in giu' nella terra comporta un assalto da parte di tutti gli\n\r"
					"insetti ivi residenti.. Si sa, la Natura non guarda in faccia a nessuno,\n\r"
					"perche' dovrebbe fare un'eccezione per te?\n\r", ch );
				ch->dreamp++;
				break;
			case 2:
				send_to_char(
					"Certo, essere svegliati da una lingua rosa che assapora ogni millimetro della\n\r"
					"tua pelle in cerca di qualche formica o ragno non e' quel che si definisce\n\r"
					"\"un buon mattino\" ma una volta nella vita bisogna pur provare..\n\r", ch );
				ch->dreamp++;
				break;
			case 3:
				send_to_char(
					"Poi senti una voce che ti chiama ti volti per vedere di chi si tratta e con\n\r"
					"grande sorpresa vedi.. te!\n\r", ch );
				ch->dreamp++;
				break;
			case 4:
				send_to_char(
					"Cos'e' questo prodigio..?\n\r"
					"Perche' tanta agitazione nella tua voce?!\n\r"
					"Forse e' la tua coscienza che ti avvisa di un pericolo imminente..?\n\r", ch );
				dream_reset( ch );
				break;
			}
		}
		/* Also a Nightmare! */
		else if ( ch->dreamn == 4 )
		{
			switch ( ch->dreamp )
			{
			case 0:
				send_to_char(
					"Un soffio di vento improvviso ti raggiunge, avverti un brivido sotto la nuca\n\r"
					"che ti costringe a spingere la mano sul collo per aiutarti a riprendere il\n\r"
					"respiro.. ma l'aria ristagna, come intrisa essa stessa di polvere e muffa..\n\r"
					"Tossisci violentemente. Rialzi il viso strizzando gli occhi, deglutisci.\n\r", ch );
				ch->dreamp++;
				break;
			case 1:
				send_to_char(
					"Sei in una stanza priva di porte, grigie le pareti ti opprimono soffocandoti\n\r"
					"ancora di piu', sembrano comprimersi all'interno chiudendosi su di te.. le\n\r"
					"tempie iniziano a pulsarti violentemente, come se ti schiacciassero.\n\r"
					"In fondo alla stanza c'e' un'antica poltrona.\n\r"
					"Cerchi ancora di guardarti intorno.. forse prima non c'era.. perche' ora..\n\r"
					"Torni a guardarla, e non e' piu' vuota.\n\r", ch );
				ch->dreamp++;
				break;
			case 2:
				send_to_char(
					"Sorride su di essa la spettrale figura di un uomo, le mani posate sulle gambe,\n\r"
					"una corda tra le sua braccia.\n\r"
					"Mentre osservi le sue scarne dita chiudersi sulla corda senti ancora quel\n\r"
					"brivido lungo la nuca scorrerti tutta la schiena fino ad accarezzarti ogni\n\r"
					"terminazione nervosa. Immobile lo guardi alzarsi e avvicinarsi a te.\n\r", ch );
				ch->dreamp++;
				break;
			case 3:
				send_to_char(
					"Fatto un nodo alla corda solleva ancora lentamente il capo.. ti sorride\n\r"
					"pacatamente mentre un brivido ancora piu' violento del primo ti scuote.. ed e'\n\r"
					"l'ultimo movimento che riesci a fare.  Tira la fune verso di te.. ma il laccio\n\r"
					"ti manca cadendo al tuo fianco.\n\r"
					"Pazientemente sorride ancora.. la raccoglie e si allontana di qualche passo.\n\r"
					"Stavolta va piu' vicino, solo di un soffio scivola via dal tuo capo.. ma lui\n\r"
					"non ha fretta.\n\r", ch );
				ch->dreamp++;
				break;
			case 4:
				send_to_char(
					"Si china e la riprende in mano, tre passi indietro, il braccio proteso, il\n\r"
					"laccio al tuo collo. Ancora quel sorriso, l'ultimo brivido, l'ultimo respiro.\n\r", ch );
				send_to_char(
					"\n\rTi svegli di soprassalto e rimani fermo, come paralizzato, per alcuni secondi,\n\r"
					"prima di renderti conto di aver avuto un incubo.\n\r", ch );
				do_wake( ch, "" );
				dream_kill( ch );
				break;
			}
		}
		else if ( ch->dreamn == 5 )
		{
			switch ( ch->dreamp )
			{
			case 0:
				send_to_char(
					"In cima ad una scogliera, aprire il mantello tendendo i lembi a destra e\n\r"
					"sinistra..\n\r"
					"Attendere paziente il vento della sera che non tarda.. non tarda, eccolo.\n\r", ch );
				ch->dreamp++;
				break;
			case 1:
				send_to_char(
					"Il mantello, goffamente cucito ai bordi della tunica sembra una grande\n\r"
					"membrana, prolungamento della schiena..\n\r", ch );
				ch->dreamp++;
				break;
			case 2:
				send_to_char(
					"......WWWWWWWWOOOOOOOOOOOOOOOOOOOHHHHHHHHHHH....\n\r", ch );
				ch->dreamp++;
				break;
			case 3:
				send_to_char(
					"E' piu' adirato stasera! Il manto s'oppone all'aria ma essa non si fa fermare\n\r"
					"da un trapezio di stoffa e velluti. Spinge, spinge non vuole arrestarsi.\n\r", ch );
				ch->dreamp++;
				break;
			case 4:
				send_to_char(
					"Spinge il mantello e me con esso. E' sorprendente trovarsi qui, ogni sera,\n\r"
					"strappare questi brevi minuti ed approfittare di una freccia temporale per\n\r"
					"librarsi come i gabbiani, con i gabbiani, sentirmi come loro..\n\r"
					"Chiudo gli occhi e mi lascio cullare dalla brezza mentre i miei piedi\n\r"
					"lentamente abbandonano il suolo e salgo, salgo sempre piu' in alto..\n\r", ch );
				dream_kill( ch );
				break;
			}
		}
		else if ( ch->dreamn == 6 )	/* Altro incubo. */
		{
			switch ( ch->dreamp )
			{
			case 0:
				send_to_char(
					"Percorri un lungo corridoio, stretto e buio, umido come le grotte dietro\n\r"
					"le cascate.\n\r"
					"Allunghi un braccio per seguire una parete, avvicinandoti ad essa per carpire\n\r"
					"la geometria del luogo.. La mano sfiora la nuda roccia, a tratti coperta da\n\r"
					"materiale viscido, sembrano alghe.. speri siano alghe.\n\r", ch );
				ch->dreamp++;
				break;
			case 1:
				send_to_char(
					"D'un tratto le dita percepiscono una cavita' nella parete di destra, una\n\r"
					"stretta fessura forse un'antica toppa per qualche misteriosa chiave.. Ti assale\n\r"
					"un dubbio: proseguire nell'oscurita' o tentare una via con questa fessura?\n\r", ch );
				ch->dreamp++;
				break;
			case 2:
				send_to_char(
					"Un'intuizione, un lampo di genio ti fa optare per la seconda.\n\r"
					"Infili due dita nel pertugio, ne studi la forma irregolare, come una sorta\n\r"
					"di micro-labirinto.. Ed ecco avviene.\n\r", ch );
				ch->dreamp++;
				break;
			case 3:
				send_to_char(
					"Uno scatto, un rumore metallico, l'incoscienza..\n\r"
					"Poi un'ondata di calore improvviso e senti le tue urla di dolore mentre ritrai\n\r"
					"la mano mutilata dalla fessura..\n\r"
					"Il sangue.. non puoi vederlo ma lo senti.. cola copioso sui tuoi panni mentre\n\r"
					"tenti faticosamente di fasciarti il moncherino..\n\r", ch );
				ch->dreamp++;
				break;
			case 4:
				send_to_char(
					"E finalmente ti svegli stringendoti la mano al petto..\n\r", ch );
				do_wake( ch, "" );
				dream_kill( ch );
				break;
			}
		}
	}
}


void dream_reset( CHAR_DATA *ch )
{

	ch->dreamp = 0;
	ch->dreamn = number_range( 0, 6 );
	save_char_obj( ch );
}


void dream_kill( CHAR_DATA *ch )
{
	ch->dreamp = 0;
	ch->dreamn = number_range( 0, 6 );
	save_char_obj( ch );
}
