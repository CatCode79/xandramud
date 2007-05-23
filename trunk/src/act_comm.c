/* Xandra Code 1.1 by Onirik and T2Team 2004 - 2006 */

/***************************************************************************
 *   ANATOLIA MUD is copyright 1996-2002 Serdar BULUT, Ibrahim CANPUNAR    *
 *   ANATOLIA has been brought to you by ANATOLIA consortium               *
 *     Serdar BULUT     {Chronos}  bulut@anatoliamud.org                   *
 *     Ibrahim Canpunar {Asena}    canpunar@anatoliamud.org                *
 *     Murat BICER      {KIO}      mbicer@anatoliamud.org                  *
 *     D.Baris ACAR     {Powerman} dbacar@anatoliamud.org                  *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ANATOLIA license, in the file doc/license.anatolia                    *
 ***************************************************************************/

/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in doc/license.diku as well the         *
 *  Merc license in doc/license.merc                                       *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *   ROM 2.4 is copyright 1993-1995 Russ Taylor                            *
 *   ROM has been brought to you by the ROM consortium                     *
 *      Russ Taylor      (rtaylor@pacinfo.com)                             *
 *      Gabrielle Taylor (gtaylor@pacinfo.com)                             *
 *      Brian Moore      (rom@rom.efn.org)                                 *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ROM license, in the file doc/license.rom                              *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#include "xandra.h"
#include "interp.h"
#include "file.h"
#include "recycle.h"


/*
 * Funzioni locali.
 */
bool	quit_org			( CHAR_DATA * ch, char *argument, bool Count );
bool	proper_order		( CHAR_DATA * ch, char *argument );
char	*translate			( CHAR_DATA *ch, CHAR_DATA *victim, char *argument );


/*
 * RT code to delete yourself.
 */
void do_delet( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Per cancellare la tua esistenza da questo mondo devi scrivere il comando completo.\n\r", ch );
}

void do_delete( CHAR_DATA *ch, char *argument )
{
	char	strsave[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( ch->pcdata->confirm_delete )
	{
		if ( argument[0] != '\0' )
		{
			send_to_char( "Continuerai a stare qui.\n\r", ch );
			ch->pcdata->confirm_delete = FALSE;
			return;
		}
		else
		{
			sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(ch->name) );
			wiznet( "$N ha cancellato la sua esistenza in questo mondo.", ch, NULL, 0, 0 );
			ch->last_fight_time = -1;
			do_quit_count( ch, "" );
			unlink( strsave );
			return;
		}
	}

	if ( argument[0] != '\0' )
	{
		send_to_char( "Basta scrivere delete.\n\r", ch );
		return;
	}

	send_to_char( "Scrivi ancora 'delete' per confermare il tuo suicidio.\n\r", ch );
	send_to_char( "ATTENZIONE: e' un processo irreversibile.\n\r", ch );
	send_to_char( "Puoi tornare sui tuoi passi scrivendo 'delete' seguito da qualunque cosa.\n\r", ch );
	ch->pcdata->confirm_delete = TRUE;
	wiznet( "$N sta contemplando il suicidio.", ch, NULL, 0, get_trust(ch) );
}


/*
 * RT code to display channel status
 */
void do_channels( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* lists all channels and their status */
	send_to_char( "   canale     stato\n\r", ch );
	send_to_char( "---------------------\n\r", ch );

	send_to_char( "aste                    ", ch );
	if ( !IS_SET(ch->comm, COMM_NOAUCTION) )
		send_to_char( "ON\n\r", ch );
	else
		send_to_char( "OFF\n\r", ch );

	if ( IS_ADMIN(ch) )
	{
		send_to_char( "canale divino           ", ch );
		if ( !IS_SET(ch->comm, COMM_NOWIZ) )
			send_to_char( "ON\n\r", ch );
		else
			send_to_char( "OFF\n\r", ch );
	}

	send_to_char( "tell                    ", ch );
	if ( !IS_SET(ch->comm, COMM_DEAF) )
		send_to_char( "ON\n\r", ch );
	else
		send_to_char( "OFF\n\r", ch );

	send_to_char( "modalita' silenziosa    ", ch );
	if ( IS_SET(ch->comm, COMM_QUIET) )
		send_to_char( "ON\n\r", ch );
	else
		send_to_char( "OFF\n\r", ch );

	if ( IS_SET(ch->comm, COMM_SNOOP_PROOF) )
		send_to_char( "Sei immune allo snooping.\n\r", ch );

	if ( ch->lines != PAGELEN )
	{
		if ( ch->lines )
		{
			sprintf( buf, "Puoi vedere %d linee sul tuo schermo.\n\r", ch->lines + 2 );
			send_to_char( buf, ch );
		}
		else
			send_to_char( "La paginazione delle linee sul tuo schermo e' disabilitata.\n\r", ch );
	}

	if ( IS_SET(ch->comm, COMM_NOTELL) )
		send_to_char( "Non puoi usare il tell.\n\r", ch );

	if ( IS_SET(ch->comm, COMM_NOCHANNELS) )
		send_to_char( "Non puoi usare alcun canale di comunicazione.\n\r", ch );

	if ( IS_SET(ch->comm, COMM_NOEMOTE) )
		send_to_char( "Non puoi usare l'emote.\n\r", ch );
}



void garble( char *garbled, char *speech )
{
	int i;
	/*~~*/

	for ( i = 0; speech[i] != (char) NULL; i++ )
	{
		if ( speech[i] >= 'a' && speech[i] <= 'z' )
			garbled[i] = 'a' + number_range( 0, 25 );
		else if ( speech[i] >= 'A' && speech[i] <= 'Z' )
			garbled[i] = 'A' + number_range( 0, 25 );
		else
			garbled[i] = speech[i];
	}

	garbled[i] = '\0';
}


/*
 * RT deaf blocks out all global messages
 */
void do_deaf( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->comm, COMM_DEAF) )
	{
		send_to_char( "Puoi ascoltare i messagi in tell ora.\n\r", ch );
		REMOVE_BIT( ch->comm, COMM_DEAF );
	}
	else
	{
		send_to_char( "D'ora in poi non ascolterai piu' i messaggi in tell.\n\r", ch );
		SET_BIT( ch->comm, COMM_DEAF );
	}
}


/*
 * RT quiet blocks out all communication
 */
void do_quiet( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->comm, COMM_QUIET) )
	{
		send_to_char( "Torni a sentire tutti i rumori che ti circondano.\n\r", ch );
		REMOVE_BIT( ch->comm, COMM_QUIET );
	}
	else
	{
		send_to_char( "D'ora in poi, potrai comunicare solo con quelli della stanza.\n\r", ch );
		SET_BIT( ch->comm, COMM_QUIET );
	}
}


void do_replay( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
	{
		send_to_char( "Non puoi risentire cio' che ti hanno inviato!\n\r", ch );
		return;
	}

	page_to_char( buf_string(ch->pcdata->buffer), ch );
	clear_buf( ch->pcdata->buffer );
}


void do_admtalk( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		if ( IS_SET(ch->comm, COMM_NOWIZ) )
		{
			send_to_char( "Il canale amministratori e' ora attivo\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_NOWIZ );
		}
		else
		{
			send_to_char( "Il canale amministratori e' ora disattivo\n\r", ch );
			SET_BIT( ch->comm, COMM_NOWIZ );
		}

		return;
	}

	REMOVE_BIT( ch->comm, COMM_NOWIZ );

	if ( !is_affected(ch, gsn_deafen) )
		act( "$n: $C$t$c", ch, argument, NULL, TO_CHAR, POS_DEAD, CYAN_BOLD );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING && IS_ADMIN(d->character) && !IS_SET(d->character->comm, COMM_NOWIZ) )
			act( "$n: $C$t$c", ch, argument, d->character, TO_VICT, POS_DEAD, CYAN_BOLD );
	}
}


/*
 * Canale say, con integrato lo snippet di Onirik sulle faccine.
 */
void do_say( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*room_char;
	OBJ_DATA	*char_obj;
	CHAR_DATA	*vch;
	char		buf[MSL];
	char		trans[MSL];
	int			x;
	int			cutting = -1;
	int			expres_type = 0;
	int			length;
	bool		found;
	char		expres_smile[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Dire cosa?\n\r", ch );
		return;
	}

	if ( ch->in_room == NULL )
	{
		send_to_char( "Ma, non ti trovi in una stanza!\n\r", ch );
		return;
	}

	expres_smile[0] = '\0';
	if ( !IS_MOB(ch) )	/* (BB) non so perchè diavolo è che con i mob mi fa segmentation fault */
	{
		/* Cerca, nelle ultim 6 lettere, ! o ? per convertirli rispettivamente in esclama e domanda. */
		for ( x = 1;  x < 7;  x++ )
		{
			length = strlen( argument ) - x ;

			if ( length <= 0 )
				break;

			switch ( argument[length] )
			{
			case '!':	expres_type = 1;	cutting = length;	break;
			case '?':	expres_type = 2;	cutting = length;	break;
			}
	
			/* Esce subito se ha trovato qualcosa */
			if ( expres_type != 0 )
				break;
		}

		/* Cerca smile alla fine della frase per convertirli ne */
		found = TRUE;	/* parte con il presupposto che abbia trovato uno smile, poi controllerà */
		for ( x = 1;  x < 7 ;  x++ )
		{
			char	 smile_arg[MIL];

			length = strlen( argument ) - x;

			/* Converte solo gli smile divisi da spazio */
			while( length >= 0 && argument[length] != ' ' )
			{
				length--;
				x++;
			}

			if ( length <= 0 )
				break;

			one_argument( argument+length, smile_arg );
	
			if		( is_name(smile_arg, ":) :-) =) =-)"		) )		sprintf( expres_smile, "sorridendo " );
			else if ( is_name(smile_arg, ":)) :-)) =)) =-))"	) )		sprintf( expres_smile, "felice " );
			else if ( is_name(smile_arg, ">:) >:-) >=) >=-)"	) )		sprintf( expres_smile, "malignamente " );
			else if ( is_name(smile_arg, "O:) O:-) O=) O=-)"	) )		sprintf( expres_smile, "innocentemente " );
			else if ( is_name(smile_arg, ":(( :-(("				) )		sprintf( expres_smile, "infelice ");
			else if ( is_name(smile_arg, ":( :-("				) )		sprintf( expres_smile, "tristemente ");
			else if ( is_name(smile_arg, ":> :-> => =->"		) )		sprintf( expres_smile, "compiaciuto " );
			else if ( is_name(smile_arg, ":D :-D =D =-D ^_^"	) )		sprintf( expres_smile, "ridendo " );
			else if ( is_name(smile_arg, ":< :-<"				) )		sprintf( expres_smile, "afflitto ");
			else if ( is_name(smile_arg, ":' :-'"				) )		sprintf( expres_smile, "piangendo ");
			else if ( is_name(smile_arg, ":') :') =') =')"		) )		sprintf( expres_smile, "piangendo di gioia " );
			else if ( is_name(smile_arg, ":'( :'-("				) )		sprintf( expres_smile, "piangendo tristemente " );
			else if ( is_name(smile_arg, ":] :-] =] =-]"		) )		sprintf( expres_smile, "ironicamente " );
			else if ( is_name(smile_arg, ":[ :-["				) )		sprintf( expres_smile, "mesto " );
			else if ( is_name(smile_arg, ":\\ :/ :-\\ :-/"		) )		sprintf( expres_smile, "perplesso " );
			else if ( is_name(smile_arg, "%\\ %/ %-\\ %-/"		) )		sprintf( expres_smile, "confuso " );
			else if ( is_name(smile_arg, ":| :-|"				) )		sprintf( expres_smile, "con espressione neutra " );
			else if ( is_name(smile_arg, ":O :o :-O :-o :0 :-0"	) )		sprintf( expres_smile, "sorpreso " );
			else if ( is_name(smile_arg, ":() :-() =() =-()"	) )		sprintf( expres_smile, "baciando " );
			else if ( is_name(smile_arg, ":x :X :-x :-X"		) )		sprintf( expres_smile, "arrabbiato " );
			else if ( is_name(smile_arg, ":p :P :-p :-P =p =P =-p =-P") )	sprintf( expres_smile, "scherzando " );
			else if ( is_name(smile_arg, ":$ :-$"				) )		sprintf( expres_smile, "sarcasticamente " );
			else if ( is_name(smile_arg, ":* :-* =* =-*"		) )		sprintf( expres_smile, "cantando " );
			else if ( is_name(smile_arg, ":-9 :-6"				) )		sprintf( expres_smile, "leccandosi i baffi " );
			else if ( is_name(smile_arg, ":& :-& =-& =-&"		) )		sprintf( expres_smile, "mangiando " );
			else if ( is_name(smile_arg, ";) ;-) ') '-) .) .-)"	) )		sprintf( expres_smile, "ammiccando " );
			else if ( is_name(smile_arg, ":v :V :-v :-V"		) )		sprintf( expres_smile, "sbadigliando " );
			else if ( is_name(smile_arg, ":= :-="				) )		sprintf( expres_smile, "sbuffando " );
			else if ( is_name(smile_arg, ":+ :-+"				) )		sprintf( expres_smile, "fischiettando " );
			else
				found = FALSE;

			if ( found )
			{
				if ( cutting == -1 || cutting > length )
					cutting = length;
	
				break;
			}
		}

		/* Taglio la parte argument finale con il cutting se ci vuole */
		if ( cutting != -1 )
		{
			/* Se trova uno spazio precedentemente scala il cutting verso sinistra nella stringa */
			while ( cutting >= 0 && argument[cutting-1] == ' ' )	/* carattere spazio */
				cutting--;
	
			if      ( expres_type  == 0 )
				argument[cutting]	= '\0';
			else if ( expres_type == 1 )
			{
				argument[cutting]   = '!';
				argument[cutting+1] = '\0';
			}
			else if ( expres_type == 2 )
			{
				argument[cutting]   = '?';
				argument[cutting+1] = '\0';
			}
		}
	}

	if ( is_affected(ch, gsn_garble) )
		garble( buf, argument );
	else
		strcpy( buf, argument );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( !is_affected(vch, gsn_deafen) )
		{
			sprintf( trans, "%s'%s", expres_smile, translate(ch, vch, buf) );

			if		( expres_type == 0 )
				act( "$C$n dice $t'$c", ch, trans, vch, TO_VICT, POS_REST, GREEN );
			else if ( expres_type == 1 )
				act( "$C$n esclama $t'$c", ch, trans, vch, TO_VICT, POS_REST, GREEN );
			else if ( expres_type == 2 )
				act( "$C$n domanda $t'$c", ch, trans, vch, TO_VICT, POS_REST, GREEN );
		}
	}

	if ( !is_affected(ch, gsn_deafen) )
	{
		if		( expres_type == 0 )
			act( "$CDici $t'$T'$c", ch, expres_smile, buf, TO_CHAR, POS_REST, GREEN );
		else if ( expres_type == 1 )
			act( "$CEsclami $t'$T'$c", ch, expres_smile, buf, TO_CHAR, POS_REST, GREEN );
		else if ( expres_type == 2 )
			act( "$CDomandi $t'$T'$c", ch, expres_smile, buf, TO_CHAR, POS_REST, GREEN );
	}

	for ( room_char = ch->in_room->people; room_char != NULL; room_char = room_char->next_in_room )
	{
		if ( IS_SET(room_char->progtypes, MPROG_SPEECH) && room_char != ch )
			( room_char->pIndexData->mprogs->speech_prog ) ( room_char, ch, buf );
	}

	for ( char_obj = ch->carrying; char_obj != NULL; char_obj = char_obj->next_content )
	{
		if ( IS_SET(char_obj->progtypes, OPROG_SPEECH) )

			( char_obj->pIndexData->oprogs->speech_prog ) ( char_obj, ch, buf );
	}

	for ( char_obj = ch->in_room->contents; char_obj != NULL; char_obj = char_obj->next_content )
	{
		if ( IS_SET(char_obj->progtypes, OPROG_SPEECH) )
			( char_obj->pIndexData->oprogs->speech_prog ) ( char_obj, ch, buf );
	}
}


void do_think ( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	if ( argument[0] == '\0' )
	{
		send_to_char( "Pensare cosa ?\n\r", ch );
		return;
	}

	act( "Tu pensi : $C$t$c", ch, argument, NULL, TO_CHAR, POS_DEAD, CYAN_BOLD );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING
		&& IS_ADMIN(d->character)
		&& !IS_SET(d->character->comm, COMM_NOWIZ) )
		{
			act( "$n pensa: $C$t$c", ch, argument, d->character, TO_VICT, POS_DEAD, CYAN_BOLD );
		}
    }
}


void do_tell( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm, COMM_DEAF) )
	{
		send_to_char( "Il tuo messaggio non  puo' giungere a destinazione.\n\r", ch );
		return;
	}

	if ( IS_SET(ch->comm, COMM_QUIET) )
	{
		send_to_char( "Devi disattivare la modalita' di silenzio per comunicare prima con qualcuno.\n\r", ch );
		return;
	}

	if ( IS_SET(ch->comm, COMM_DEAF) )
	{
		send_to_char( "Devi disattivare la modalita' di sordita' prima.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "Cosa vuoi dire a chi?\n\r", ch );
		return;
	}

	/* Can tell to PC's anywhere, but NPC's only in same room. -- Furey */
	if ( (victim = get_char_world(ch, arg)) == NULL || (IS_MOB(victim) && victim->in_room != ch->in_room) )
	{
		send_to_char( "Non c'e'.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		act( "Comunicare con te stess$x?", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( victim->desc == NULL && !IS_MOB(victim) )
	{
		char		buf[MSL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		act( "$N sembra non essere in questa dimensione.. prova ancora dopo.", ch, NULL, victim, TO_CHAR, POS_REST );
		sprintf( buf, "[OFFGDR] %s ti comunica '%s'\n\r", ch->name, argument );
		buf[0] = UPPER( buf[0] );
		add_buf( victim->pcdata->buffer, buf );
		return;
	}

	if ( (IS_SET(victim->comm, COMM_QUIET) || IS_SET(victim->comm, COMM_DEAF)) && !IS_ADMIN(ch) )
	{
		act( "$E non vuole sentire alcun messaggio.", ch, 0, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( IS_SET(victim->act, PLR_AFK) )
		act( "[OFFGDR] $CComunichi a $N [in AFK] '$t'$c", ch, argument, victim, TO_CHAR, POS_SLEEP, RED_BOLD );
	else
		act( "[OFFGDR] $CComunichi a $N '$t'$c", ch, argument, victim, TO_CHAR, POS_SLEEP, RED_BOLD );

#ifdef MXP
	act( "[OFFGDR] $C"
		MXPTAG("Pg $n")
		"$n"
		MXPTAG("/Pg")
		" ti comunica '$t'$c", ch, argument, victim, TO_VICT, POS_DEAD, RED_BOLD );
#else
	act( "[OFFGDR] $C$n ti comunica '$t'$c", ch, argument, victim, TO_VICT, POS_SLEEP, RED_BOLD );
#endif

	victim->reply = ch;
}


void do_reply( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_SET(ch->comm, COMM_NOTELL) )
	{
		send_to_char( "Il tuo messaggio non  puo' giungere a destinazione.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Rispondere cosa?\n\r", ch );
		return;
	}

	if ( (victim = ch->reply) == NULL )
	{
		send_to_char( "Non c'e'.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL && !IS_MOB(victim) )
	{
		char		buf[MSL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		act( "$N sembra non essere in questa dimensione.. prova ancora dopo.", ch, NULL, victim, TO_CHAR, POS_REST );
		sprintf( buf, "[OFFGDR] %s ti comunica '%s'\n\r", ch->name, argument );
		buf[0] = UPPER( buf[0] );
		add_buf( victim->pcdata->buffer, buf );
		return;
	}

	if ( (IS_SET(victim->comm, COMM_QUIET) || IS_SET(victim->comm, COMM_DEAF))&& !IS_ADMIN(ch) )
	{
		act( "$E non vuole ricevere messaggi.", ch, 0, victim, TO_CHAR, POS_DEAD );
		return;
	}

	if ( IS_SET(victim->act, PLR_AFK) )
		act( "[OFFGDR] $CComunichi a $N [in AFK] '$t'$c", ch, argument, victim, TO_CHAR, POS_SLEEP, RED_BOLD );
	else
		act( "[OFFGDR] $CComunichi a $N '$t'$c", ch, argument, victim, TO_CHAR, POS_SLEEP, RED_BOLD );

#ifdef MXP
	act( "[OFFGDR] $C"
		MXPTAG("Pg $n")
		"$n"
		MXPTAG("/Pg")
	" ti comunica '$t'$c", ch, argument, victim, TO_VICT, POS_SLEEP, RED_BOLD );
#else
	act( "[OFFGDR] $C$n ti comunica '$t'$c", ch, argument, victim, TO_VICT, POS_SLEEP, RED_BOLD );
#endif

	victim->reply = ch;
}


void do_yell( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	char			buf[MIL];
	char			trans[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Urlare cosa?\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_garble) )
		garble( buf, argument );
	else
		strcpy( buf, argument );

	if ( !is_affected(ch, gsn_deafen) )
		act( "Urli '$C$t$c'", ch, buf, NULL, TO_CHAR, POS_DEAD, BROWN );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected != CON_PLAYING )						continue;
		if ( d->character == ch )								continue;
		if ( !d->character->in_room || !ch->in_room )			continue;
		if ( d->character->in_room->area != ch->in_room->area )	continue;

		if ( !is_affected(d->character, gsn_deafen) )
		{
			CHAR_DATA		*victim;
			victim = d->original ? d->original : d->character;

			sprintf( trans, "%s", translate(ch, d->character, buf) );
			act( "$n urla '$C$t$c'", ch, trans, victim, TO_VICT, POS_DEAD, BROWN );
		}
	}
}


/*
 * Canale shout, si basa sullo yell
 */
void do_shout( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	char			buf[MIL];
	char			trans[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Gridare cosa?\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_garble) )
		garble( buf, argument );
	else
		strcpy( buf, argument );

	if ( !is_affected(ch, gsn_deafen) )
		act( "Gridi '$C$t$c'", ch, buf, NULL, TO_CHAR, POS_DEAD, BROWN );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected != CON_PLAYING )						continue;
		if ( d->character == ch )								continue;
		if ( !d->character->in_room || !ch->in_room )			continue;
		if ( d->character->in_room->area != ch->in_room->area )	continue;

		if ( !is_affected(d->character, gsn_deafen) )
		{
			CHAR_DATA		*victim;
			victim = d->original ? d->original : d->character;

			sprintf( trans, "%s", translate(ch, d->character, buf) );
			act( "$n grida '$C$t$c'", ch, trans, victim, TO_VICT, POS_DEAD, BROWN );
		}
	}
}


/*
 * Canale Whisper, si basa sul tell ma è rpg.
 */
void do_whisper( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		buf[MSL];
    CHAR_DATA  *victim;

    argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "Sussurrare a chi che cosa?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char( "Non si trova qui.\n\r", ch );
		return;
	}

	if ( !(IS_ADMIN(ch) && ch->level > LEVEL_ADMIN) && !IS_AWAKE(victim) )
	{
		act( "$E non ti puo' sentire.", ch, 0, victim, TO_CHAR, POS_REST );
		return;
	}
  
	if ( is_affected(ch,gsn_garble) )
		garble( buf, argument );
	else
		strcpy( buf, argument );

	if ( !is_affected(ch, gsn_deafen) )
	{
		if ( ch == victim )
			act( "$CSussurri a $N qualcosa.$c", ch, buf, victim, TO_CHAR, POS_REST, CYAN );
		else
			act( "$CSussurri a $N '$t'$c", ch, buf, victim, TO_CHAR, POS_REST, CYAN );
	}

	if ( ch == victim )
		act( "$C$n sussurra qualcosa a se stesso.$c", ch, NULL, NULL, TO_ROOM, POS_REST, CYAN );
	else
	{
		act( "$C$n ti sussurra '$t'$c", ch, buf, victim, TO_VICT, POS_REST, CYAN );
		act( "$C$n sussurra qualcosa a $N", ch, NULL, victim, TO_NOTVICT, POS_REST, CYAN );
	}
}


/*
 * Canale Murmur, si basa sul whisper.
 */
void do_murmur( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		buf[MSL];
    CHAR_DATA  *victim;

    argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "Mormorare a chi che cosa?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char( "Non si trova qui.\n\r", ch );
		return;
	}

	if ( !(IS_ADMIN(ch) && ch->level > LEVEL_ADMIN) && !IS_AWAKE(victim) )
	{
		act( "$E non ti puo' sentire.", ch, 0, victim, TO_CHAR, POS_REST );
		return;
	}
  
	if ( is_affected(ch,gsn_garble) )
		garble( buf, argument );
	else
		strcpy( buf, argument );

	if ( !is_affected(ch, gsn_deafen) )
	{
		if ( ch == victim )
			act( "$CMormori a te stesso '$t'$c", ch, buf, victim, TO_CHAR, POS_REST, CYAN );
		else
			act( "$CMormori a $N '$t'$c", ch, buf, victim, TO_CHAR, POS_REST, CYAN );
	}

	if ( ch == victim )
		act( "$C$n mormora qualcosa a se stesso.$c", ch, NULL, NULL, TO_ROOM, POS_REST, CYAN );
	else
	{
		act( "$C$n ti mormora '$t'$c", ch, buf, victim, TO_VICT, POS_REST, CYAN );
		act( "$C$n mormora qualcosa a $N$c", ch, NULL, victim, TO_NOTVICT, POS_REST, CYAN );
	}
}


void do_emote( CHAR_DATA *ch, char *argument )
{
	char	buf[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
	{
		send_to_char( "Non puoi esprimere le tue emozioni.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Esprimere cosa?\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_garble) )
		garble( buf, argument );
	else
		strcpy( buf, argument );

	if ( isalpha(buf[strlen(buf)-1]) )
		strcat( buf, "." );

	act( "$n $T", ch, NULL, buf, TO_ROOM, POS_REST );
	act( "$n $T", ch, NULL, buf, TO_CHAR, POS_REST );
}

/*
 * All the posing stuff.
 */
struct pose_table_type
{
	char	*message[2*MAX_CLASS];
};

const struct pose_table_type pose_table[] =
{
	{
	{
		"Lampi di energia ti attraversano il corpo.",
		"Il corpo di $n e' attraversato da lampi di energia.",
		"Un'aura sacra ti illumina dall'alto.",
		"Un'aura sacra dall'alto scivola su $n.",
		"Prendi un mazzo di carte per poi lanciarle e farle sparire in aria.",
		"$n prende un mazzo di carte per poi lanciarle e farle sparire in aria.",
		"Gonfi i muscoli fieramente.",
		"$n gonfia fieramente i muscoli."
	}
	},

	{
	{
		"Apri la mano trandone una splendida farfalla che vola subito via.",
		"$n apre trandone una splendida farfalla.",
		"Versi dell'acqua in un bicchiere mutandola all'istante in vino.",
		"$n versa dell'acqua mutandola all'istante in vino.",
		"Disegni degli anelli di fumo con la pipa.",
		"$n disegna degli anelli di fumo con la pipa.",
		"Rompi tra le dita una noce di cocco.",
		"$n rompe tra le dita una noce di cocco."
	}
	},

	{
	{
		"Agiti le dita facendone sprizzare scintille blu!",
		"$n agita le dita facendone sprizzare scintille blu!",
		"Un'alone di luce si forma attorno al tuo viso.",
		"Un alone di luce circonda il volto di $n.",
		"Stiri le dita fino a toccarti con esse il dorso della mano.",
		"$n si stira le dita fino a toccarsi il dorso della mano..ouch!",
		"Fai stridere i denti.. brrr..",
		"$n fa stridere i denti sogghignando.. brr.."
	}
	},

	{
	{
		"I tuoi occhi sono illuminati da una strana luce rossa..",
		"Una fievole luce rossa illumina lo sguardo di $n.",
		"Snoccioli con disinvoltura una delle tue solite perle di saggezza.",
		"$n rende partecipi i presenti di una delle sue perle di saggezza.",
		"Lanci in aria la tua lama centrandone il fodero quando ricade!",
		"$n lancia in aria la sua lama centrandone il fodero quando ricade!",
		"Ti concentri fino a far sparire le tue pupille dalle orbite!",
		"$n si concentra fino a far sparire le pupille dalle orbite!"
	}
	},

	{
	{
		"Il vento porta l'alito di un'ombra che s'inchina a te, poi svanisce.",
		"Il vento porta l'alito di un'ombra che s'inchina a $n, poi svanisce.",
		"Preghi cosi' intensamente che vai in estasi ed inizi levitare.",
		"$n prega cosi' intensamente da andare in estasi e levitare.",
		"Ti fai prestare una borsa e la fai sparire con un gesto! MAGIA!",
		"$n si fa prestare una borsa e la fa sparire con un gesto! MAGIA!",
		"Apri delle noci spaccandole con un dito.",
		"$n apre delle noci spaccandole con un dito."
	}
	},

	{
	{
		"Ti siedi annoiat$x e cominci a costruire un castello di carte..",
		"Si siede annoiat$x e costruisce un castello di carte..",
		"Una luminosa pioggia di scintille dorate si posa su di te.",
		"Una luminosa pioggia di scintille dorate si posa su di $n.",
		"Lanci i dadi facendoli roteare sulle punte delle dita.",
		"$n lancia i dadi facendoli roteare sulle punte delle dita!",
		"Ti metti giu' e cominci a fare flessioni su di un solo braccio.",
		"$n si tira giu' e fa le flessioni su di un solo braccio."
	}
	},

	{
	{
		"Giochi con un globo di luce facendolo danzare tra le dita.",
		"$n gioca con un globo di luce faecendolo danzare tra le dita.",
		"Una luce innaturale attraversa il tuo corpo.",
		"Il corpo di $n viene illuminato da una luce sovrannaturale.",
		"Indovini l'ammontare delle monete d'oro nelle tasche di tutti!",
		"$n indovina l'ammontare delle monete d'oro nelle tue tasche!",
		"Stiracchi rumorosamente il collo.. croack..crock..",
		"Crock..croack.. $n stiracchia rumorosamente il collo."
	}
	},

	{
	{
		"Sbuffi un alone di fumo multicolore.",
		"$n sbuffa un alone di fumo multicolore.",
		"Delle enormi ali di luce si formano per un attimo sulle tue spalle.",
		"Per un attimo sulle spalle di $n si formano due ali di luce.",
		"Tieni in equilibrio sulla lingua un coltello appuntito.",
		"$n tiene in equilibrio sulla lingua un coltello appuntito.",
		"Fai una tranquilla camminata a piedi nudi su carboni ardenti.",
		"$n fa una tranquilla camminata a piedi nudi su carboni ardenti.."
	}
	},

	{
	{
		"Reciti una piccola poesia in lingua magica..",
		"$n recita una piccola poesia in lingua magica.",
		"Un coro angelico si solleva mentre tu preghi.",
		"Un coro angelico si solleva dal nulla appena $n inizia a pregare.",
		"Fai spuntare una moneta dall'orecchio di tutti.. e te la tieni.",
		"$n fa spuntare una moneta dal tuo orecchio sinistro.. e se la tiene.",
		"Fai scricchiolare le dita chiudendo lentamente il pugno.",
		"$n fa scricchiolare le dita chiudendo lentamente il pugno."
	}
	},

	{
	{
		"Ti passi una mano sul viso rendendo la tua testa invisibile!",
		"$n si passa una mano sul volto e la sua testa scompare!",
		"Una tiepida brezza ti ravviva i capelli.",
		"Una tiepida brezza ravviva i capelli a $n.",
		"Fai fare alla tua ombra un inchino ai presenti.",
		"$n fa' fare alla sua ombra un inchino ai presenti.",
		"Ti pulisci i denti con la punta della spada.",
		"$n si pulisce i denti con la punta della spada."
	}
	},

	{
	{
		"Un soffio infuocato illumina il tuo volto.",
		"Un soffio infuocato illumina il volto di $n.",
		"Un tiepido raggio di sole sortito dal nulla ti si posa sulla fronte.",
		"Un raggio di sole sortito dal nulla si posa sulla fronte di $n.",
		"Cammini elegantemente a testa in giu' reggendoti sulle braccia.",
		"$n cammina elegantemente a te sta in giu' reggendosi sulle braccia.",
		"Rompi un macigno con una testata!",
		"$n prende un macigno e lo frantuma con una testata!"
	}
	},

	{
	{
		"Il colore del cielo cambia fino a raggiungere quello dei tuoi occhi.",
		"Il colore del cielo cambia fino a diventare quello degli occhi di $n.",
		"Una nuvola scivola sul tuo viso rinfrescandoti con qualche goccia.",
		"Una nuvola scivola sul viso di $n rinfrescandol$x con qualche goccia.",
		"Sostituisci le armi di tutti i presenti con canne da zucchero!",
		"$n ti ha sostituito la fedele arma con una canna da zucchero!",
		"Con un calcio tiri giu' un albero!",
		"$n spacca in due un albero con un calcio!"
	}
	},

	{
	{
		"Manipoli in una magica danza una frotta di sassolini.",
		"$n manovra mentalmente alla danza una frotta di sassolini.",
		"Una serie di ovattate nuvole scivola fino a te per farti da cuscino.",
		"Una serie di ovattate nuvole scivola fino a $n per far da cuscino.",
		"Fai spuntare 'magicamente' dalle tue dita un portafogli!",
		"$n fa' spuntare 'magicamente' dalle sue dita il tuo portafogli!",
		"Una cinghia della tua armatura cede alla pressione dei tuoi muscoli.",
		"Una cinghia dell'armatura di $n cede sotto la pressione dei muscoli."
	}
	},

	{
	{
		"Il cielo cambia colore appena sorridi.",
		"Il cielo sembra cambiar colore appena $n sorride.",
		"Un corteo di fuochi fatui ti circonda per poi svanire.",
		"Un corteo di fuochi danzanti circonda $n per poi svanire.",
		"Fai scoppiettare scintille e botti nelle tasche dei presenti!",
		"Senti qualcosa scoppiettare nelle tue tasche!Scintille! $n!Colpa sua!",
		"Guardi male una roccia che si frantuma dopo pochi istanti.",
		"$n guarda in maniera torva una roccia che non resiste e si frantuma!"
	}
	},

	{
	{
		"Con un gesto nella stanza rendi trasparenti gli abiti di tutti!",
		"$n con un gesto rende trasparenti gli abiti di tutti!Anche i tuoi!",
		"Il sole appare per un attimo e ti ammicca.",
		"Il sole appare per un attimo ammiccando a $n.",
		"Giochi facendo roteare contemporaneamente 7 daghe in aria..",
		"$n gioca facendo roteare contemporaneamente 7 daghe in aria..",
		"Lanci in aria un masso e lo colpisci con il petto frantumandolo.",
		"$n lancia in aria un masso e lo colpisce con il petto frantumandolo."
	}
	},

	{
	{
		"Fai sparire il tuo addome materializzando un buco nero!",
		"$n fa' sparire il suo addome materializzando un buco nero!",
		"Una rosa bianca cade dal cielo finendo tra le tue mani.",
		"Una rosa bianca cade dal cielo finendo tra le mani di $n.",
		"Sparisci dietro la tua ombra.. e riappari dal nulla!",
		"$n sparisce nel nulla.. e ti riappare proprio davanti! Uah!",
		"Ti metti a testa in giu' tenendoti in equilibrio su due dita.",
		"$n si mette a testa in giu' tenendosi in equilibrio su due dita."
	}
	},

	{
	{
		"Fischietti.. e gli alberi muovono il fusto al tuo ritmo!",
		"$n fischietta.. e gli alberi si muovono al suo ritmo!",
		"Un falco ti porta un bastone.",
		"Un falco arrivato da chissa' dove porta un bastone a $n.",
		"Ora sei qui.. ora non sei piu' qui!",
		"$n svanisce improvvisamente! No e' qui.. era.. dov'e'?",
		"Respiri a pieni polmoni.. incrinando l'armatura che ti contiene.",
		"$n respira a pieni polmoni.. incrinando l'armatura che l$x contiene."
	}
	}
};



void do_pose( CHAR_DATA *ch, char *argument )
{
	int level;
	int pose;
	/*~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	level = UMIN( ch->level, sizeof(pose_table) / sizeof(pose_table[0]) - 1 );
	pose = number_range( 0, level );

	act( pose_table[pose].message[2 * ch->class +0], ch, NULL, NULL, TO_CHAR, POS_REST );
	act( pose_table[pose].message[2 * ch->class +1], ch, NULL, NULL, TO_ROOM, POS_REST );
}


void do_bug( CHAR_DATA *ch, char *argument )
{
	char	strtmp[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !argument || argument[0] == '\0' )
		return ;

	sprintf( strtmp, "%s\n%s%s\n\n", ch->name, ctime(&current_time), argument );
	append_file( ch, BUG_FILE, strtmp );
	send_to_char( "Bug registrato.\n\r", ch );
}


void do_typo( CHAR_DATA *ch, char *argument )
{
	char	strtmp[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !argument || argument[0] == '\0' )
		return ;

	sprintf( strtmp, "%s\n%s%s\n\n", ch->name, ctime(&current_time), argument );
	append_file( ch, TYPO_FILE, strtmp );
	send_to_char( "Typo registrato.\n\r", ch );
}


void do_afk( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	SET_BIT(ch->act, PLR_AFK );
	send_to_char( "Ti allontani dalla tastiera.\n\r", ch );
}


void do_qui( CHAR_DATA *ch, char *argument )
{
	char buf[MSL];

	if ( !IS_SET(ch->act, PLR_ENGLISH) )
		sprintf( buf, "Se vuoi andartene da questo mondo devi scrivere per esteso 'fine'.\n\r" );
	else
		sprintf( buf, "Se vuoi andartene da questo mondo devi scrivere per esteso 'quit'.\n\r" );

	send_to_char( buf, ch );
}


void do_quit( CHAR_DATA *ch, char *argument )
{
	quit_org( ch, argument, FALSE );
}


void do_quit_count( CHAR_DATA *ch, char *argument )
{
	quit_org( ch, argument, TRUE );
}


bool quit_org( CHAR_DATA *ch, char *argument, bool Count )
{
	DESCRIPTOR_DATA *d, *dr, *d_next;
	CHAR_DATA		*vch, *vch_next;
	OBJ_DATA		*obj, *obj_next;
	int				id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return( FALSE );

	if ( ch->position == POS_FIGHT )
	{
		send_to_char( "Neppure per sogno! Stai combattendo.\n\r", ch );
		return( FALSE );
	}

	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	{
		send_to_char( "Meglio alzarsi prima di tutto.\n\r", ch );
		return( FALSE );
	}

	if ( ch->position < POS_STUN )
	{
		send_to_char( "Non sei ancora morto.\n\r", ch );
		return( FALSE );
	}

	if ( ch->last_fight_time != -1 && !IS_ADMIN(ch) && (current_time - ch->last_fight_time) < FIGHT_DELAY_TIME )
	{
		send_to_char( "La tua adrenalina e' a mille! non puoi ancora andartene.\n\r", ch );
		return( FALSE );
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		send_to_char( "Non vorrai lasciare il tuo maestro?\n\r", ch );
		return( FALSE );
	}

	if ( IS_SET(ch->act, PLR_NO_EXP) )
	{
		send_to_char( "Non vorrai perdere il tuo spirito.\n\r", ch );
		return( FALSE );
	}

	if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller)) )
	{
		send_to_char( "Attendi che l'asta sia giunta a termine.\n\r", ch );
		return( FALSE );
	}

	if ( !IS_ADMIN(ch) && ch->in_room && IS_RAFFECTED(ch->in_room, AFF_ROOM_ESPIRIT) )
	{
		send_to_char( "Spiriti malvagi non ti consentono di abbadonare questo mondo.\n\r", ch );
		return( FALSE );
	}

	if ( !IS_ADMIN(ch) && ch->cabal != CABAL_INVADER && is_affected(ch, gsn_evil_spirit) )
	{
		send_to_char( "Spiriti malvagi non ti consentono di abbadonare questo mondo.\n\r", ch );
		return( FALSE );
	}

	if ( cabal_area_check(ch) )
	{
		send_to_char( "Non puoi abbandonare il gioco nel territorio delle altre cabale.\n\r", ch );
		return( FALSE );
	}

	send_to_char( "Purtroppo tutte le cose belle hanno una loro fine.\n\r", ch );
	sprintf( log_buf, "%s e' uscit%c dal gioco.", ch->name, (ch->pcdata->true_sex == SEX_FEMALE) ? 'a' : 'o' );
	log_string( log_buf );
	wiznet( log_buf, ch, NULL, WIZ_LOGINS, get_trust(ch) );

#ifdef TRE_PUNTO_UNO
	update_total_played( ch );
#endif

	for ( obj = object_list; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next;
		if ( obj->pIndexData->vnum == 84
		||	 obj->pIndexData->vnum == 85
		||	 obj->pIndexData->vnum == 86
		||	 obj->pIndexData->vnum == 97 )
		{
			if ( obj->extra_descr == NULL )
				extract_obj( obj );
			else if ( strstr(obj->extra_descr->description, ch->name) != NULL )
				extract_obj( obj );
		}
	}

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
		obj_next = obj->next_content;
		if ( obj->pIndexData->vnum == OBJ_VNUM_MAGIC_JAR )
		{
			extract_obj( obj );
		}

		if ( obj->pIndexData->vnum == 84 || obj->pIndexData->vnum == 85 || obj->pIndexData->vnum == 86 || obj->pIndexData->vnum == 97 )
		{
			if ( obj->extra_descr == NULL )
				extract_obj( obj );
			else if ( strstr(obj->extra_descr->description, ch->name) != NULL )
				extract_obj( obj );
			else
			{
				obj_from_char( obj );
				obj_to_room( obj, ch->in_room );
			}
		}
	}

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next;
		if ( is_affected(vch, gsn_doppelganger) && vch->doppel == ch )
		{
			send_to_char( "Ritorni ad apparire nella tua forma.\n\r", vch );
			affect_strip( vch, gsn_doppelganger );
		}

		if ( vch->guarding == ch )
		{
			act( "Smetti di proteggerti da $N.", vch, NULL, ch, TO_CHAR, POS_REST );
			act( "$n smette di proteggerti.", vch, NULL, ch, TO_VICT, POS_REST );
			act( "$n smette di proteggere $N.", vch, NULL, ch, TO_NOTVICT, POS_REST );
			vch->guarding = NULL;
			ch->guarded_by = NULL;
		}

		if ( vch->last_fought == ch )
		{
			vch->last_fought = NULL;
			back_home( vch );
		}

		if ( vch->hunting == ch )
			vch->hunting = NULL;
	}

	if ( ch->guarded_by != NULL )
	{
		ch->guarded_by->guarding = NULL;
		ch->guarded_by = NULL;
	}

	/* After extract_char the ch is no longer valid! */
	save_char_obj( ch );
	id = ch->id;
	dr = d = ch->desc;
	if ( Count || get_total_played(ch) < MIN_TIME_LIMIT )
		extract_char( ch, TRUE );
	else
		extract_char_nocount( ch, TRUE );

	if ( d != NULL )
		close_socket( d );

	/* toast evil cheating bastards */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
		CHAR_DATA	*tch;
		/*~~~~~~~~~~~~~*/

		d_next = d->next;
		tch = d->original ? d->original : d->character;
		if ( tch && tch->id == id )
		{
			extract_char_nocount( tch, TRUE );
			close_socket( d );
		}
	}

	iNumPlayers--;

	return( TRUE );
}


void do_save( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;
/*	if ( ch->level < 2 )
	{
		send_to_char( "Devi fare qualcosa di piu' affinche' la tua memoria venga salvata in questo mondo.\n\r", ch );
		return;
	}
*/
	save_char_obj( ch );
	send_to_char( "Salvato.\n\r", ch );
	WAIT_STATE( ch, PULSE_VIOLENCE );
}


void do_follow( CHAR_DATA *ch, char *argument )
{
	/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Seguire chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e'.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
	{
		act( "Ma dovresti seguire $N!", ch, NULL, ch->master, TO_CHAR, POS_REST );
		return;
	}

	if ( victim == ch )
	{
		if ( ch->master == NULL )
		{
			act( "Stai gia' seguendo te stess$x.", ch, NULL, victim, TO_CHAR, POS_REST );

			return;
		}

		stop_follower( ch );
		return;
	}

	if ( !IS_MOB(victim) && IS_SET(victim->act, PLR_NOFOLLOW) && !IS_ADMIN(ch) )
	{
		act( "Mi sa che $N non vuole che nessuno l$X segua.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	REMOVE_BIT( ch->act, PLR_NOFOLLOW );

	if ( ch->master != NULL )
		stop_follower( ch );

	add_follower( ch, victim );
}


void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
	if ( ch->master != NULL )
	{
		bug( "Add_follower: non-null master.", 0 );
		return;
	}

	ch->master = master;
	ch->leader = NULL;

	if ( can_see(master, ch) )
		act( "$C$n ora ti segue.$c", ch, NULL, master, TO_VICT, POS_REST, YELLOW );

	act( "$CSegui $N.$c", ch, NULL, master, TO_CHAR, POS_REST, YELLOW );
}


void stop_follower( CHAR_DATA *ch )
{
	if ( ch->master == NULL )
	{
		bug( "Stop_follower: null master.", 0 );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		REMOVE_BIT( ch->affected_by, AFF_CHARM );
		affect_strip( ch, gsn_charm_person );
	}

	if ( can_see(ch->master, ch) && ch->in_room != NULL )
	{
		act( "$C$n smette di segurti.$c", ch, NULL, ch->master, TO_VICT, POS_REST, BLUE );
		act( "$CSmetti di seguire $N.$c", ch, NULL, ch->master, TO_CHAR, POS_REST, BLUE );
	}

	if ( ch->master->pet == ch )
		ch->master->pet = NULL;

	ch->master = NULL;
	ch->leader = NULL;
}


/*
 * nukes charmed monsters and pets
 */
void nuke_pets( CHAR_DATA *ch )
{
	CHAR_DATA	*pet;
	/*~~~~~~~~~~~~~*/

	if ( (pet = ch->pet) != NULL )
	{
		stop_follower( pet );
		if ( pet->in_room != NULL )
			act( "$N lentamente svanisce e scompare.", ch, NULL, pet, TO_NOTVICT, POS_REST );
		extract_char_nocount( pet, TRUE );
	}

	ch->pet = NULL;
}


void die_follower( CHAR_DATA *ch )
{
	CHAR_DATA	*fch;
	CHAR_DATA	*fch_next;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( ch->master != NULL )
	{
		if ( ch->master->pet == ch )
			ch->master->pet = NULL;
		stop_follower( ch );
	}

	ch->leader = NULL;

	for ( fch = char_list; fch != NULL; fch = fch_next )
	{
		fch_next = fch->next;
		if ( fch->master == ch )
			stop_follower( fch );
		if ( fch->leader == ch )
			fch->leader = fch;
	}
}


bool proper_order( CHAR_DATA *ch, char *argument )
{
	bool	found;
	int		trust, cmd_num;
	DO_FUN	*cmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	found = FALSE;

	trust = get_trust( ch );

	for ( cmd_num = 0; cmd_table[cmd_num].name[0] != '\0'; cmd_num++ )
	{
		if ( argument[0] == cmd_table[cmd_num].name[0]
		&&	 !str_prefix(argument, cmd_table[cmd_num].name)
		&&	 cmd_table[cmd_num].level <= trust )
		{
			found = TRUE;
			break;
		}
	}

	if ( !found )
		return( TRUE );

	cmd = cmd_table[cmd_num].do_fun;
	if ( !IS_MOB(ch) )
	{
		if ( cmd == do_delete || cmd == do_induct || cmd == do_quest || cmd == do_practice || cmd == do_train )
			return( FALSE );
		return( TRUE );
	}

	if ( ((cmd == do_bash) || (cmd == do_dirt) || (cmd == do_kick) || (cmd == do_murder) || (cmd == do_trip)) && ch->fighting == NULL )
		return( FALSE );

	if ((cmd == do_assassinate)
	||	(cmd == do_ambush)
	||	(cmd == do_blackjack)
	||	(cmd == do_cleave)
	||	(cmd == do_kill)
	||	(cmd == do_murder)
	||	(cmd == do_recall)
	||	(cmd == do_strangle)
	||	(cmd == do_vtouch) )
		return( FALSE );

	if ( cmd == do_close || cmd == do_lock || cmd == do_open || cmd == do_unlock )
	{
		if ( race_table[ch->race].pc_race )
			return( TRUE );
		if (ch->race != 34		/* doll */
		&&	ch->race != 37		/* goblin */
		&&	ch->race != 38		/* hobgoblin */
		&&	ch->race != 39		/* kobolt */
		&&	ch->race != 40		/* lizard */
		&&	ch->race != 41		/* modron */
		&&	ch->race != 42 )	/* orc */
			return( FALSE );
		else
			return( TRUE );
	}

	if ( (cmd == do_backstab) || (cmd == do_hide) || (cmd == do_pick) || (cmd == do_sneak) )
	{
		if ( IS_SET(ch->act, ACT_THIEF) )
			return( TRUE );
		else
			return( FALSE );
	}

	return( TRUE );
}


void do_order( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MIL],
				arg2[MIL];
	CHAR_DATA	*victim;
	CHAR_DATA	*och;
	CHAR_DATA	*och_next;
	bool		found;
	bool		fAll;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );

	if ( !str_cmp(arg2, "cancella") || !str_cmp(arg2, "delete") )
	{
		send_to_char( "Questo non lo puoi ordinare!\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Ordinare a chi a fare cosa?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		send_to_char( "Senti che tu ora sei quello che prende ordini e non quello che li da'.\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "tutti") || !str_cmp(arg, "all") )
	{
		fAll = TRUE;
		victim = NULL;
	}
	else
	{
		fAll = FALSE;
		if ( (victim = get_char_room(ch, arg)) == NULL )
		{
			send_to_char( "Non c'e'.\n\r", ch );
			return;
		}

		if ( victim == ch )
		{
			send_to_char( "Bravo, continua cosi'..\n\r", ch );
			return;
		}

		if ( !IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch || (IS_ADMIN(victim) && victim->trust >= ch->trust) )
		{
			send_to_char( "Sbrigatela da solo!\n\r", ch );
			return;
		}
	}

	found = FALSE;
	for ( och = ch->in_room->people; och != NULL; och = och_next )
	{
		och_next = och->next_in_room;

		if ( IS_AFFECTED(och, AFF_CHARM) && och->master == ch && (fAll || och == victim) )
		{
			char	tcmd[MSL];

			found = TRUE;
			if ( !proper_order(och, arg2) )
				continue;

			sprintf( buf, "$n ti ordina '%s %s'.", arg2, argument );
			act( buf, ch, NULL, och, TO_VICT, POS_REST );

			sprintf( tcmd, "%s %s", translate_command(arg2, ch, och), argument );
			interpret( och, tcmd, TRUE );
		}
	}

	if ( found )
	{
		WAIT_STATE( ch, PULSE_VIOLENCE );
		send_to_char( "Ok.\n\r", ch );
	}
	else
		send_to_char( "Non hai nessun seguace.\n\r", ch );
}


void do_gwho( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*gch, *gch2, *chief;
	char		buf[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "%s+ + + + + + + + Gruppi di Xandra + + + + + + + +%s\n\r\n\r",
		CLR(ch, CYAN), CLR(ch, NORMAL) );
	send_to_char( buf, ch );
	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( gch->leader == NULL )
			continue;
		else
			chief = gch->leader;
		sprintf( buf, "%s -- %s%s%s\n\r",
			CLR(ch, CYAN), CLR(ch, WHITE), chief->name, CLR(ch, NORMAL) );
		send_to_char( buf, ch );

		for ( gch2 = char_list; gch2 != NULL; gch2 = gch2->next )
		{
			if ( is_same_group(gch2, chief) && (gch2 == chief) )
			{
				sprintf( buf, "%s -- %s%s\n\r",
					CLR(ch, CYAN),
					chief->name,
					CLR(ch, NORMAL) );
				send_to_char( buf, ch );
			}
		}

		send_to_char( "\n\r", ch );
	}

	sprintf( buf, "%s+ + + + + + + + + + + + + + + + + + + + + + + + + + + + + +%s",
		CLR(ch, CYAN),
		CLR(ch, NORMAL) );
	send_to_char( buf, ch );
}


void do_group( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( !IS_AWAKE(ch) )
	{
		send_to_char( "Non riesci a vedere bene il tuo gruppo se non sei in piedi.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		CHAR_DATA	*gch;
		CHAR_DATA	*leader;
		/*~~~~~~~~~~~~~~~~*/

		leader = ( ch->leader != NULL ) ? ch->leader : ch;
		sprintf( buf, "Gruppo di %s:\n\r", PERS(leader, ch) );
		send_to_char( buf, ch );

		for ( gch = char_list; gch != NULL; gch = gch->next )
		{
			if ( is_same_group(gch, ch) )
			{
				sprintf( buf, "%-16s %d%% hp %d%% mana %d%% mv  %s\n\r",
					capitalize(PERS(gch, ch)),
					gch->point[LIFE] * 100 / gch->max_point[LIFE],
					gch->point[MANA] * 100 / gch->max_point[MANA],
					gch->point[MOVE] * 100 / gch->max_point[MOVE],
					RACE_NAME(gch) );
				send_to_char( buf, ch );
			}
		}

		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e'.\n\r", ch );
		return;
	}

	if ( ch->master != NULL || (ch->leader != NULL && ch->leader != ch) )
	{
		send_to_char( "Stai seguendo un'altra persona!\n\r", ch );
		return;
	}

	if ( victim->master != ch && ch != victim )
	{
		act( "$N non ti sta seguendo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_CHARM) )
	{
		send_to_char( "Non puoi togliere i tuoi seguaci dal gruppo.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		act( "Ti piace troppo stare vicino al tuo maestro per abbandonarl$X!", ch, NULL, victim, TO_VICT, POS_REST );
		return;
	}

	if ( victim == ch )
	{
		act( "Vuoi fare gruppo con te stess$x?", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_same_group(victim, ch) && ch != victim )
	{
		if ( ch->guarding == victim || victim->guarded_by == ch )
		{
			act( "Non proteggi piu' $N.", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n non ti protegge piu'.", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$n non protegge piu' $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
			victim->guarded_by = NULL;
			ch->guarding = NULL;
		}

		victim->leader = NULL;
		act( "$C$n rimuove $N dal suo gruppo.$c",
			ch, NULL, victim, TO_NOTVICT, POS_SLEEP, YELLOW );
		act( "$C$n ti rimuove dal suo gruppo.$c",
			ch, NULL, victim, TO_VICT, POS_SLEEP, YELLOW );
		act( "$CRimuovi $N dal tuo gruppo.$c",
			ch, NULL, victim, TO_CHAR, POS_SLEEP, BLUE );

		if ( victim->guarded_by != NULL && !is_same_group(victim, victim->guarded_by) )
		{
			act( "Smetti di proteggere $N.", victim->guarded_by, NULL, victim, TO_CHAR, POS_REST );
			act( "$n smette di proteggerti.", victim->guarded_by, NULL, victim, TO_VICT, POS_REST );
			act( "$n non protegge piu' $N.", victim->guarded_by, NULL, victim, TO_NOTVICT, POS_REST );
			victim->guarded_by->guarding = NULL;
			victim->guarded_by = NULL;
		}

		return;
	}

	if ( ch->level - victim->level < -8 || ch->level - victim->level > 8 )
	{
		act( "$C$N non puo' far parte del gruppo di $n.$c",
			ch, NULL, victim, TO_NOTVICT, POS_SLEEP, RED_BOLD );
		act( "$CNon puoi far parte del suo gruppo.$c",
			ch, NULL, victim, TO_VICT, POS_SLEEP, RED_BOLD );
		act( "$C$N non puo' far parte del tuo gruppo.$c",
			ch, NULL, victim, TO_CHAR, POS_SLEEP, RED_BOLD );
		return;
	}

	if ( IS_GOOD(ch) && IS_EVIL(victim) )
	{
		act( "$CYou are too evil for $n's group.$c",
			ch, NULL, victim, TO_VICT, POS_SLEEP, RED );
		act( "$C$N is too evil for your group!$c",
			ch, NULL, victim, TO_CHAR, POS_SLEEP, RED );
		return;
	}

	if ( IS_GOOD(victim) && IS_EVIL(ch) )
	{
		act( "$CYou are too pure to join $n's group!$c",
			ch, NULL, victim, TO_VICT, POS_SLEEP, RED );
		act( "$C$N is too pure for your group!$c",
			ch, NULL, victim, TO_CHAR, POS_SLEEP, RED );
		return;
	}

	if ((ch->cabal == CABAL_RULER && victim->cabal == CABAL_CHAOS)
	||	(ch->cabal == CABAL_CHAOS && victim->cabal == CABAL_RULER)
	||	(ch->cabal == CABAL_KNIGHT && victim->cabal == CABAL_INVADER)
	||	(ch->cabal == CABAL_INVADER && victim->cabal == CABAL_KNIGHT)
	||	(ch->cabal == CABAL_YVALADRIN && victim->cabal == CABAL_ARMY)
	||	(ch->cabal == CABAL_ARMY && victim->cabal == CABAL_YVALADRIN) )
	{
		act( "$COdi la cabala di $n, come potresti entrare in gruppo accanto a $e?!$c",
			ch, NULL, victim, TO_VICT, POS_SLEEP, RED );
		act( "$COdi la cabala di $N, come puoi volerl$X in gruppo?!$c",
			ch, NULL, victim, TO_CHAR, POS_SLEEP, RED );
		return;
	}

	victim->leader = ch;
	act( "$C$N si unisce al gruppo di $n.$c",
		ch, NULL, victim, TO_NOTVICT, POS_SLEEP, YELLOW );
	act( "$CTi unisci al gruppo di $n.$c",
		ch, NULL, victim, TO_VICT, POS_SLEEP, YELLOW );
	act( "$C$N si unisce al tuo gruppo.$c",
		ch, NULL, victim, TO_CHAR, POS_SLEEP, BLUE );
}


/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg1[MIL], arg2[MIL];
	CHAR_DATA	*gch;
	int			members;
	int			amount_gold = 0, amount_silver = 0;
	int			share_gold, share_silver;
	int			extra_gold, extra_silver;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	one_argument( argument, arg2 );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "Quanto vorresti dividere?\n\r", ch );
		return;
	}

	amount_silver = atoi( arg1 );

	if ( arg2[0] != '\0' )
		amount_gold = atoi( arg2 );

	if ( amount_gold < 0 || amount_silver < 0 )
	{
		send_to_char( "Al tuo gruppo non piacera' la cosa.\n\r", ch );
		return;
	}

	if ( amount_gold == 0 && amount_silver == 0 )
	{
		send_to_char( "Difficile da spartire una quantita' nulla.\n\r", ch );
		return;
	}

	if ( ch->gold < amount_gold || ch->silver < amount_silver )
	{
		send_to_char( "Non hai cosi' tanto da dividere.\n\r", ch );
		return;
	}

	members = 0;
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM) )
			members++;
	}

	if ( members < 2 )
	{
		send_to_char( "Fai prima a tenerti tutto.\n\r", ch );
		return;
	}

	share_silver = amount_silver / members;
	extra_silver = amount_silver % members;

	share_gold = amount_gold / members;
	extra_gold = amount_gold % members;

	if ( share_gold == 0 && share_silver == 0 )
	{
		send_to_char( "Non riesci a dividere questi soldi con i membri del gruppo.\n\r", ch );
		return;
	}

	ch->silver -= amount_silver;
	ch->silver += share_silver + extra_silver;
	ch->gold -= amount_gold;
	ch->gold += share_gold + extra_gold;

	if ( share_silver > 0 )
	{
		sprintf( buf, "Dividi %d monete d'argento. La tua parte e' un totale di %d.\n\r", amount_silver, share_silver + extra_silver );
		send_to_char( buf, ch );
	}

	if ( share_gold > 0 )
	{
		sprintf( buf, "Dividi %d monete d'argento. La tua parte e' un totale di %d.\n\r", amount_gold, share_gold + extra_gold );
		send_to_char( buf, ch );
	}

	if ( share_gold == 0 )
		sprintf( buf, "$n divide %d monete d'argento. La tua parte e' un totale di %d.", amount_silver, share_silver );
	else if ( share_silver == 0 )
		sprintf( buf, "$n divide %d monete d'oro. La tua parte e' un totale di %d.", amount_gold, share_gold );
	else
	{
		sprintf( buf, "$n divide %d monete d'argento e %d monete d'oro, La tua parte e' di %d d'argento e %d d'oro.\n\r",
			amount_silver,
			amount_gold,
			share_silver,
			share_gold );
	}

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( gch != ch && is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM) )
		{
			act( buf, ch, NULL, gch, TO_VICT, POS_REST );
			gch->gold += share_gold;
			gch->silver += share_silver;
		}
	}
}


void do_gtell( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	CHAR_DATA	*gch;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Dire al tuo gruppo cosa?\n\r", ch );
		return;
	}

	if ( !IS_AWAKE(ch) )
	{
		send_to_char( "Riesci a parlare agli altri nel sonno?\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_garble) )
		garble( buf, argument );
	else
		strcpy( buf, argument );

	/* Note use of send_to_char, so gtell works on sleepers. */
	for ( i = 0, gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( gch->in_room != ch->in_room )
			continue;

		if ( !is_affected(gch, gsn_deafen) )
		{
			if ( is_same_group(gch, ch) )
			{
				act( "$C$n comunica al gruppo '$t'$c", ch, buf, gch, TO_VICT, POS_REST, MAGENTA );
				i++;
			}
			else
				act( "$C$n comunica al suo gruppo qualcosa.$c", ch, NULL, gch, TO_VICT, POS_REST, MAGENTA );
		}
	}

	if ( !is_affected(ch, gsn_deafen) )
	{
		if ( i > 1 )
			act( "$CComunichi al gruppo '$t'$c", ch, buf, NULL, TO_CHAR, POS_REST, CYAN );
		else
			act( "Smettila di parlare con te stess$x. Sei da sol$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
	}
}


void do_gt2( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	CHAR_DATA	*gch;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Dire al tuo gruppo cosa?\n\r", ch );
		return;
	}

	/* Note use of send_to_char, so gtell works on sleepers. */
	for ( i = 0, gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( is_same_group(gch, ch) )
		{
			act( "[OFFGDR] $C$n comunica al gruppo '$t'$c", ch, buf, gch, TO_VICT, POS_REST, MAGENTA );
			i++;
		}
	}

	if ( i > 1 )
		act( "[OFFGDR] $CComunichi al gruppo '$t'$c", ch, buf, NULL, TO_CHAR, POS_REST, CYAN );
	else
		act( "Sei da sol$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
}


/*
 * It is very important that this be an equivalence relation: (1) A ~ A (2) if A ~ B then B ~ A (3) if A ~ B and B ~
 *    C, then A ~ C
 */
bool is_same_group_old( CHAR_DATA *ach, CHAR_DATA *bch )
{
	if ( ach == NULL || bch == NULL )
		return( FALSE );

	if ( ach->leader != NULL )
		ach = ach->leader;
	if ( bch->leader != NULL )
		bch = bch->leader;

	return( ach == bch );
}


/*
 * New is_same_group by Chronos
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
	CHAR_DATA	*ch, *vch, *ch_next, *vch_next;
	int			count, vcount;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	count = vcount = 0;
	for ( ch = ach; ch != NULL; ch = ch_next )
	{
		ch_next = ch->leader;
		for ( vch = bch; vch != NULL; vch = vch_next )
		{
			vch_next = vch->leader;
			if ( ch == vch )
				return( TRUE );
			if ( ++vcount > 6 )
				break;	/* cyclic loop! */
		}

		if ( ++count > 6 )
			break;		/* cyclic loop! */
	}

	return( FALSE );
}


void do_pray( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	if ( IS_SET(ch->comm, COMM_NOCHANNELS) )
	{
		send_to_char( "Gli dei si rifiutano di ascoltare le tue preghiere ora.", ch );
		return;
	}

	send_to_char( "Preghi gli Dei per un aiuto!\n\r", ch );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING && IS_ADMIN(d->character) && !IS_SET(d->character->comm, COMM_NOWIZ) )
		{
			if ( argument[0] == '\0' )
				act( "$C$n sta pregando gli dei$c", ch, argument, d->character, TO_VICT, POS_DEAD, CYAN );
			else
				act( "$C$n sta pregando per: $t$c", ch, argument, d->character, TO_VICT, POS_DEAD, CYAN );
		}
	}
}


char char_lang_lookup( char c )
{
	int i;
	/*~~*/

	for ( i = 0; translation_table[i].common[0] != '\0'; i++ )
	{
		if ( translation_table[i].common[0] == c )
			return( translation_table[i].language[0] );
	}

	return( c );
}


/*
 * ch says victim hears
 */
char *translate( CHAR_DATA *ch, CHAR_DATA *victim, char *argument )
{
	static char trans[MSL];
	char		buf[MSL];
	char		c;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (*argument == '\0'
	||	(ch == NULL)
	||	(victim == NULL)
	||	IS_MOB(ch)
	||	IS_MOB(victim)
	||	IS_ADMIN(ch)
	||	IS_ADMIN(victim)
	||	ch->language == LANG_COMMON
	||	ch->language == pc_race_table[ORG_RACE(victim)].language )
	{
		if ( IS_ADMIN(victim) )
			sprintf( trans, "{%s} %s", language_table[ch->language].name, argument );
		else
			strcpy( trans, argument );
		return( trans );
	}

	for ( i = 0; *argument != '\0'; argument++, i++ )
	{
		c = char_lang_lookup( *argument );
		buf[i] = c;

	}

	buf[i] = '\0';

	sprintf( trans, "{%s} %s", language_table[ch->language].name, buf );
	return( trans );
}


void do_speak( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	char	arg[MIL];
	int		language;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		sprintf( buf, "Attualmente parli in %s.\n\r", language_table[ch->language].name );
		send_to_char( buf, ch );
		send_to_char( "Puoi parlare in:\n\r", ch );
		sprintf( buf, "       comune, %s\n\r", language_table[pc_race_table[ORG_RACE(ch)].language].name );
		send_to_char( buf, ch );
		return;
	}

	language = lang_lookup( arg );

	if ( language == -1 )
	{
		send_to_char( "Non hai mai sentito tale lingua.\n\r", ch );
		return;
	}

	if ( language >= MAX_LANGUAGE )
		ch->language = pc_race_table[ORG_RACE( ch )].language;
	else
		ch->language = language;

	sprintf( buf, "Ora parli %s.\n\r", language_table[ch->language].name );
	send_to_char( buf, ch );
}


bool cabal_area_check( CHAR_DATA *ch )
{
	if ( ch->in_room == NULL || IS_ADMIN(ch) )
		return( FALSE );

	if		( ch->cabal != CABAL_RULER && !str_cmp(ch->in_room->area->name, "Ruler") )			return( TRUE );
	else if ( ch->cabal != CABAL_INVADER && !str_cmp(ch->in_room->area->name, "Invader") )		return( TRUE );
	else if ( ch->cabal != CABAL_CHAOS && !str_cmp(ch->in_room->area->name, "Chaos") )			return( TRUE );
	else if ( ch->cabal != CABAL_YVALADRIN && !str_cmp(ch->in_room->area->name, "Yvaladrin") )	return( TRUE );
	else if ( ch->cabal != CABAL_ARMY && !str_cmp(ch->in_room->area->name, "Army") )			return( TRUE );
	else if ( ch->cabal != CABAL_KNIGHT && !str_cmp(ch->in_room->area->name, "Knight") )		return( TRUE );
	else if ( ch->cabal != CABAL_HUNTER && !str_cmp(ch->in_room->area->name, "Hunter") )		return( TRUE );
	else if ( ch->cabal != CABAL_LIONS && !str_cmp(ch->in_room->area->name, "Lions") )			return( TRUE );
	else
		return( FALSE );
}


bool is_at_cabal_area( CHAR_DATA *ch )
{
	if ( ch->in_room == NULL || IS_ADMIN(ch) )
		return( FALSE );

	if (!str_cmp(ch->in_room->area->name, "Ruler")
	||	!str_cmp(ch->in_room->area->name, "Invader")
	||	!str_cmp(ch->in_room->area->name, "Chaos")
	||	!str_cmp(ch->in_room->area->name, "Yvaladrin")
	||	!str_cmp(ch->in_room->area->name, "Army")
	||	!str_cmp(ch->in_room->area->name, "Knight")
	||	!str_cmp(ch->in_room->area->name, "Hunter")
	||	!str_cmp(ch->in_room->area->name, "Lions") )
		return( TRUE );
	else
		return( FALSE );
}

#ifdef TRE_PUNTO_UNO
/*
 * Should be called only in quit_org and reboot_xandra
 */
void update_total_played( CHAR_DATA *ch )
{
	int l;
	int today = parse_date( current_time );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* add the player's new time */
	for ( l = 0; l < MAX_TIME_LOG; l++ )
	{
		if ( today == ch->pcdata->log_date[l] )
		{
			ch->pcdata->log_time[l] += parse_time_spec( current_time - ch->logon );
			ch->logon = current_time;
			return;
		}
	}

	/* so new day has started */
	for ( l = MAX_TIME_LOG - 1; l > 0; l-- )
		ch->pcdata->log_time[l] = ch->pcdata->log_time[l - 1];

	ch->pcdata->log_time[0] = parse_time_spec( current_time - ch->logon );
	ch->logon = current_time;
}
#endif
