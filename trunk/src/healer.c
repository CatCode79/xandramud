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
#include "interp.h"
#include "magic.h"


void do_heal( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*mob;
	char		arg[MIL];
	int			cost, sn;
	SPELL_FUN	*spell;
	char		*words;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* check for healer */
	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
		if ( IS_MOB(mob) && IS_SET(mob->act, ACT_IS_HEALER) )
		{
			if ( ch->cabal && is_name("cabal cabala", mob->name) )
			{
				if ( is_name(cabal_table[ch->cabal].short_name, mob->name) )
					break;
				else
					continue;
			}
			else
				break;
		}
	}

	if ( mob == NULL )
	{
		send_to_char( "Non puoi farlo.\n\r", ch );
		return;
	}

	if ( ch->cabal == CABAL_ARMY )
	{
		send_to_char( "Appartieni alla cabala dell'Armata, non tolleri la magia.\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		/* display price list */
		act( "Il curatore offre i seguenti servigi:", ch, NULL, mob, TO_CHAR, POS_REST );
		send_to_char( " leggere     : cure light wounds     10 d'oro\n\r", ch );
		send_to_char( " serie       : cure serious wounds   20 d'oro\n\r", ch );
		send_to_char( " critiche    : cure critical wounds  30 d'oro\n\r", ch );
		send_to_char( " cura        : healing spell         60 d'oro\n\r", ch );
		send_to_char( " cecita'     : cure blindness        25 d'oro\n\r", ch );
		send_to_char( " malattia    : cure disease          20 d'oro\n\r", ch );
		send_to_char( " veleno      : cure poison           30 d'oro\n\r", ch );
		send_to_char( " maledizione : remove curse         100 d'oro\n\r", ch );
		send_to_char( " ristora     : restore movement      10 d'oro\n\r", ch );
		send_to_char( " mana        : restore mana          20 d'oro\n\r", ch );
		send_to_char( " guarigione  : master heal spell    250 d'oro\n\r", ch );
		send_to_char( " energia     : restore 300 mana     300 d'oro\n\r", ch );
		send_to_char( " Scrivi heal <tipo> per essere curato.\n\r", ch );
		return;
	}

	if ( !str_prefix(arg, "leggere") )
	{
		spell = spell_cure_light;
		sn = skill_lookup( "cure light" );
		words = "judicandus dies";
		cost = 1000;
	}
	else if ( !str_prefix(arg, "serie") )
	{
		spell = spell_cure_serious;
		sn = skill_lookup( "cure serious" );
		words = "judicandus gzfuajg";
		cost = 1600;
	}
	else if ( !str_prefix(arg, "critiche") )
	{
		spell = spell_cure_critical;
		sn = skill_lookup( "cure critical" );
		words = "judicandus qfuhuqar";
		cost = 2500;
	}
	else if ( !str_prefix(arg, "cura") )
	{
		spell = spell_heal;
		sn = skill_lookup( "heal" );
		words = "pzar";
		cost = 5000;
	}
	else if ( !str_prefix(arg, "cecita'") )
	{
		spell = spell_cure_blindness;
		sn = skill_lookup( "cure blindness" );
		words = "judicandus noselacri";
		cost = 2000;
	}
	else if ( !str_prefix(arg, "malattia") )
	{
		spell = spell_cure_disease;
		sn = skill_lookup( "cure disease" );
		words = "judicandus eugzagz";
		cost = 1500;
	}
	else if ( !str_prefix(arg, "veleno") )
	{
		spell = spell_cure_poison;
		sn = skill_lookup( "cure poison" );
		words = "judicandus sausabru";
		cost = 2500;
	}
	else if ( !str_prefix(arg, "maledizione") )
	{
		spell = spell_remove_curse;
		sn = skill_lookup( "remove curse" );
		words = "candussido judifgz";
		cost = 5000;
	}
	else if ( !str_prefix(arg, "mana") )
	{
		spell = NULL;
		sn = -3;
		words = "candamira";
		cost = 1000;
	}
	else if ( !str_prefix(arg, "ristora") )
	{
		spell = spell_refresh;
		sn = skill_lookup( "refresh" );
		words = "candusima";
		cost = 500;
	}
	else if ( !str_prefix(arg, "guarigione") )
	{
		spell = spell_master_heal;
		sn = skill_lookup( "master healing" );
		words = "candastra nikazubra";
		cost = 20000;
	}
	else if ( !str_prefix(arg, "energia") )
	{
		spell = NULL;
		sn = -2;
		words = "energizer";
		cost = 20000;
	}
	else
	{
		act( "Non vi e' nessuna cura con tale nome. Digita 'heal' per la lista.", ch, NULL, mob, TO_CHAR, POS_REST );
		return;
	}

	if ( cost > (ch->gold * 100 + ch->silver) )
	{
		act( "Non hai abbastanza denaro con te.", ch, NULL, mob, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, PULSE_VIOLENCE );

	deduct_cost( ch, cost );
	mob->gold += cost / 100;

	act( "$n pronuncia le parole '$T'.", mob, NULL, words, TO_ROOM, POS_REST );
	if ( sn == -2 )
	{
		ch->point[MANA] += 300;
		ch->point[MANA] = UMIN( ch->point[MANA], ch->max_point[MANA] );
		send_to_char( "Una morbida luce ti attraversa.\n\r", ch );
	}

	if ( sn == -3 )
	{
		ch->point[MANA] += dice( 2, 8 ) + mob->level / 3;
		ch->point[MANA] = UMIN( ch->point[MANA], ch->max_point[MANA] );
		send_to_char( "Una morbida luce ti attraversa.\n\r", ch );
	}

	if ( sn < 0 )
		return;

	spell( sn, mob->level, mob, ch, TARGET_CHAR );
}


void heal_army( CHAR_DATA *mob, CHAR_DATA *ch )
{
	int sn;
	/*~~~*/

	if ( is_name(cabal_table[ch->cabal].short_name, mob->name) )
		return;

	if ( IS_MOB(ch) || ch->cabal != CABAL_ARMY )
	{
		do_say( mob, "Non ti voglio aiutare." );
		return;
	}

	if ( !IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_PLAGUE) && !IS_AFFECTED(ch, AFF_POISON) && !IS_AFFECTED(ch, AFF_CURSE) )
	{
		do_say( mob, "Non hai bisogno del mio aiuto. Ma giusto nel caso!" );
		sn = skill_lookup( "remove curse" );
		spell_remove_curse( sn, mob->level, mob, ch, TARGET_CHAR );
		return;
	}

	act( "$n ti da' delle erbe da ingerire.", mob, NULL, ch, TO_VICT, POS_REST );
	act( "Mangi queste erbe.", mob, NULL, ch, TO_VICT, POS_REST );
	act( "Dai le erbe a  $N.", mob, NULL, ch, TO_CHAR, POS_REST );
	act( "$N mangia le erbe che gli hai dato.", mob, NULL, ch, TO_CHAR, POS_REST );
	act( "$n da' le erbe a $N.", mob, NULL, ch, TO_NOTVICT, POS_REST );
	act( "$n mangia le erbe che $N gli ha passato.", mob, NULL, ch, TO_NOTVICT, POS_REST );

	WAIT_STATE( ch, PULSE_VIOLENCE );

	if ( IS_AFFECTED(ch, AFF_BLIND) )
	{
		sn = skill_lookup( "cure blindness" );
		spell_cure_blindness( sn, mob->level, mob, ch, TARGET_CHAR );
	}

	if ( IS_AFFECTED(ch, AFF_PLAGUE) )
	{
		sn = skill_lookup( "cure disease" );
		spell_cure_disease( sn, mob->level, mob, ch, TARGET_CHAR );
	}

	if ( IS_AFFECTED(ch, AFF_POISON) )
	{
		sn = skill_lookup( "cure poison" );
		spell_cure_poison( sn, mob->level, mob, ch, TARGET_CHAR );
	}

	if ( IS_AFFECTED(ch, AFF_CURSE) )
	{
		sn = skill_lookup( "remove curse" );
		spell_remove_curse( sn, mob->level, mob, ch, TARGET_CHAR );
	}
}
