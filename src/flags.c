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
#include <stdlib.h>

#include "xandra.h"
#include "tables.h"


/*
 * Funzioni esterne.
 */
int flag_lookup ( const char *name, const struct flag_type *flag_table );


void do_flag( CHAR_DATA *ch, char *argument )
{
	char					arg1[MIL], arg2[MIL], arg3[MIL];
	char					word[MIL];
	CHAR_DATA				*victim;
	long					*flag, old = 0, new = 0, marked = 0, pos;
	char					type;
	const struct flag_type	*flag_table;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	type = argument[0];

	if ( type == '=' || type == '-' || type == '+' )
		argument = one_argument( argument, word );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "\n\rSintassi:\n\r", ch );
		send_to_char( "  flag mob  <nome> <campo> <flags>\n\r", ch );
		send_to_char( "  flag char <nome> <campo> <flags>\n\r", ch );
		send_to_char( "  flag obj  <nome> <campo> <flags>\n\r", ch );
		send_to_char( "  flag room <stanza> <campo> <flags>\n\r", ch );
		send_to_char( "  mob  flags: act,aff,off,imm,res,vuln,form,part\n\r", ch );
		send_to_char( "  char flags: plr,comm,aff,imm,res,vuln,\n\r", ch );
		send_to_char( "  obj  flags: extra,wear,weap,cont,gate,exit\n\r", ch );
		send_to_char( "  room flags: room\n\r", ch );
		send_to_char( "  +: aggiunge una flag, -: rimuove una flag, = set equal to\n\r", ch );
		send_to_char( "  altrimenti si tolgono quelli presenti in precedenza.\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' )
	{
		send_to_char( "Dove vorresti mettere una flag?\n\r", ch );
		return;
	}

	if ( arg3[0] == '\0' )
	{
		send_to_char( "Devi specificare quale flag vuoi mettere.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Quale flag vuoi cambiare?\n\r", ch );
		return;
	}

	if ( !str_prefix(arg1, "mob") || !str_prefix(arg1, "char") )
	{
		victim = get_char_world( ch, arg2 );
		if ( victim == NULL )
		{
			send_to_char( "Non c'e' nulla del genere.\n\r", ch );
			return;
		}

		/* select a flag to set */
		if ( !str_prefix(arg3, "act") )
		{
			if ( !IS_MOB(victim) )
			{
				send_to_char( "Per i giocatori utilizza 'plr'.\n\r", ch );
				return;
			}

			flag = &victim->act;
			flag_table = act_flags;
		}
		else if ( !str_prefix(arg3, "plr") )
		{
			if ( IS_MOB(victim) )
			{
				send_to_char( "Per i mob utilizza 'act'.\n\r", ch );
				return;
			}

			flag = &victim->act;
			flag_table = plr_flags;
		}
		else if ( !str_prefix(arg3, "aff") )
		{
			flag = &victim->affected_by;
			flag_table = affect_flags;
		}
		else if ( !str_prefix(arg3, "immunity") )
		{
			flag = &victim->imm_flags;
			flag_table = imm_flags;
		}
		else if ( !str_prefix(arg3, "resist") )
		{
			flag = &victim->res_flags;
			flag_table = imm_flags;
		}
		else if ( !str_prefix(arg3, "vuln") )
		{
			flag = &victim->vuln_flags;
			flag_table = imm_flags;
		}
		else if ( !str_prefix(arg3, "form") )
		{
			if ( !IS_MOB(victim) )
			{
				send_to_char( "Sui giocatori non puoi utilizzare 'form'.\n\r", ch );
				return;
			}

			flag = &victim->form;
			flag_table = form_flags;
		}
		else if ( !str_prefix(arg3, "parts") )
		{
			if ( !IS_MOB(victim) )
			{
				send_to_char( "Sui giocatori non puoi utilizzare 'part'.\n\r", ch );
				return;
			}

			flag = &victim->parts;
			flag_table = part_flags;
		}
		else if ( !str_prefix(arg3, "comm") )
		{
			if ( IS_MOB(victim) )
			{
				send_to_char( "Non puoi utilizzare 'comm' sui mob.\n\r", ch );
				return;
			}

			flag = &victim->comm;
			flag_table = comm_flags;
		}
		else
		{
			send_to_char( "Questa flag non e' accettabile.\n\r", ch );
			return;
		}

		old = *flag;
		victim->zone = NULL;

		if ( type != '=' )
			new = old;

		/* mark the words */
		for ( ;; )
		{
			argument = one_argument( argument, word );

			if ( word[0] == '\0' )
				break;

			pos = flag_lookup( word, flag_table );
			if ( pos == 0 )
			{
				send_to_char( "Questa flag non esiste.\n\r", ch );
				return;
			}
			else
				SET_BIT( marked, pos );
		}

		for ( pos = 0; flag_table[pos].name != NULL; pos++ )
		{
			if ( !flag_table[pos].settable && IS_SET(old, flag_table[pos].bit) )
			{
				SET_BIT( new, flag_table[pos].bit );
				continue;
			}

			if ( IS_SET(marked, flag_table[pos].bit) )
			{
				switch ( type )
				{
				case '=':
				case '+':
					SET_BIT( new, flag_table[pos].bit );
					break;

				case '-':
					REMOVE_BIT( new, flag_table[pos].bit );
					break;

				default:
					if ( IS_SET(new, flag_table[pos].bit) )
						REMOVE_BIT( new, flag_table[pos].bit );
					else
						SET_BIT( new, flag_table[pos].bit );
				}
			}
		}

		*flag = new;
		return;
	}
}
