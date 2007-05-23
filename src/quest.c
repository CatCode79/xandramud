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

/****************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.01. Please do not remove this notice from this file. *
****************************************************************************/

/****************************************************************************
 *  Revised and adopted to Anatolia by chronos.                             *
 ****************************************************************************/

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
#include <string.h>

#include "xandra.h"
#include "recycle.h"


/*
 * Funzioni locali.
 */
void		do_tell_quest		( CHAR_DATA * ch, CHAR_DATA * victim, char *argument );
CHAR_DATA	*get_quest_world	( CHAR_DATA *ch, MOB_INDEX_DATA *victim );
void		generate_quest		( CHAR_DATA * ch, CHAR_DATA * questman );
bool		chance				( int num );


extern MOB_INDEX_DATA	*mob_index_hash[MAX_KEY_HASH];


/*
 * Object vnums for object quest 'tokens': object quest.
 */
#define QUEST_OBJQUEST1 84
#define QUEST_OBJQUEST2 85
#define QUEST_OBJQUEST3 86
#define QUEST_OBJQUEST4 97


/*
 * CHANCE function. I use this everywhere in my code, very handy::>
 */
bool chance( int num )
{
	if ( number_range(1, 100) <= num )
		return( TRUE );
	else
		return( FALSE );
}


/*
 * The main quest function
 */
void do_quest( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA		*questman;
	OBJ_DATA		*obj = NULL, *obj_next;
	OBJ_INDEX_DATA	*questinfoobj;
	MOB_INDEX_DATA	*questinfo;
	char			buf[MSL];
	char			arg1[MIL];
	char			arg2[MIL];
	int				sn, trouble_vnum = 0, trouble_n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( IS_MOB(ch) )
		return;

	if ( !str_cmp(arg1, "info") || !str_cmp(arg1, "informazioni") )
	{
		if ( IS_SET(ch->act, PLR_QUESTOR) )
		{
			if ( ch->pcdata->questmob == -1 )
			{
				sprintf( buf, "Your quest is ALMOST complete!\n\rGet back to Questor before your time runs out!\n\r" );
				send_to_char( buf, ch );
			}
			else if ( ch->pcdata->questobj > 0 )
			{
				questinfoobj = get_obj_index( ch->pcdata->questobj );
				if ( questinfoobj != NULL )
				{
					sprintf( buf, "You are on a quest to recover the fabled %s!\n\r", questinfoobj->name );
					send_to_char( buf, ch );
				}
				else
					send_to_char( "You aren't currently on a quest.\n\r", ch );
				return;
			}
			else if ( ch->pcdata->questmob > 0 )
			{
				questinfo = get_mob_index( ch->pcdata->questmob );
				if ( questinfo != NULL )
				{
					sprintf( buf, "You are on a quest to slay the dreaded %s!\n\r", questinfo->short_descr );
					send_to_char( buf, ch );
				}
				else
					send_to_char( "You aren't currently on a quest.\n\r", ch );
				return;
			}
		}
		else
			send_to_char( "You aren't currently on a quest.\n\r", ch );
		return;
	}

	if ( !str_cmp(arg1, "points") || !str_cmp(arg1, "punti") )
	{
		sprintf( buf, "You have %d quest points.\n\r", ch->pcdata->questpoints );
		send_to_char( buf, ch );
		return;
	}

	/*
	 * Checks for a character in the room with spec_questmaster set. This special procedure must be defined in
	 * special.c. You could instead use an ACT_QUESTMASTER flag instead of a special procedure.
	 */
	for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
	{
		if ( !IS_MOB(questman) )
			continue;
		if ( questman->spec_fun == spec_lookup("spec_questmaster") )
			break;
	}

	if ( questman == NULL || questman->spec_fun != spec_lookup("spec_questmaster") )
	{
		send_to_char( "You can't do that here.\n\r", ch );
		return;
	}

	if ( questman->fighting != NULL )
	{
		send_to_char( "Wait until the fighting stops.\n\r", ch );
		return;
	}

	ch->pcdata->questgiver = questman->pIndexData->vnum;

	/*
	And, of course, you will need to change the following lines for YOUR
	quest item information. Quest items on Moongate are unbalanced, very
	very nice items, and no one has one yet, because it takes awhile to
	build up quest points :> Make the item worth their while.
	*/

	if ( !str_cmp(arg1, "list") || !str_cmp(arg1, "lista") )
	{
		char	buf_vampire[MIL];
		char	buf_samurai[MIL];
		char	buf_slay[MIL];
		char	buf_religion[MIL];

		act( "$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM, POS_REST);
		act ("You ask $N for a list of quest items.",ch, NULL, questman, TO_CHAR, POS_REST);

		if ( ch->class == CLASS_VAMPIRE )
			sprintf( buf_vampire, "   100qp.........Vampire skill (vampire)\n\r" );

		if ( ch->class == CLASS_SAMURAI )
		{
			sprintf( buf_samurai,
				"   200qp.........Katana quest (katana)\n\r"
				"   200qp.........Second katana quest(sharp)\n\r"
				"   100qp.........Decrease number of death (death)\n\r" );
		}

		sprintf( buf_slay,
			" 10000qp.........Ammazza-buoni sull'arma impugnata (slaygood)\n\r"
			" 10000qp.........Ammazza-neutrali sull'arma impugnata (slayneutral)\n\r"
			" 10000qp.........Ammazza-malvagi sull'arma impugnata (slayevil)\n\r" );

		sprintf( buf_religion,
			"   200qp.........tattoo of your religion (tattoo)\n\r"
			"    50qp.........remove tattoo of your religion (remove)\n\r"
			"    50qp.........set religion to none (set)\n\r" );

		sprintf( buf,
			"Current Quest Items available for Purchase:\n\r"
			"%s"
			"  5000qp.........the silk-adamantite backpack (backpack)\n\r"
			"  1000qp.........the Girth of Real Heroism (girth)\n\r"
			"  1000qp.........the Ring of Real Heroism (ring)\n\r"
			"  1000qp.........the Real Hero's Weapon (weapon)\n\r"
			"   500qp.........Decanter of Endless Water (decanter)\n\r"
			"   250qp.........1 Train\n\r"
			"%s%s%s"
			"To buy an item, type 'QUEST BUY <item>'.\n\r",
			buf_slay, buf_samurai, buf_vampire, buf_religion );
		send_to_char( buf, ch );
		return;
	}
	else if ( !str_cmp(arg1, "buy") || !str_cmp(arg1, "compra") )
	{
		if ( arg2[0] == '\0' )
		{
			send_to_char( "To buy an item, type 'QUEST BUY <item>'.\n\r", ch );
			return;
		}
		else if ( is_name(arg2, "slaygood slayneutral slayevil") )
		{
			OBJ_DATA *wield;

			wield = get_wield_char( ch, FALSE );
			if ( !wield )
			{
				do_tell_quest( ch, questman, "Impugna l'arma che vuoi legare al tuo spirito potenziandola!" );
				return;
			}

			if ( wield->item_type != ITEM_WEAPON )
			{
				do_tell_quest( ch, questman, "Quello che stai impugnando ora e' tutto furoche' un'arma." );
				return;
			}

			if ( wield->pIndexData->limit != -1 )
			{
				do_tell_quest( ch, questman, "Non su questo oggetto, e' fuori dalla mia portata." );
				return;
			}

			if ( (!str_cmp(arg2, "slaygood") && IS_GOOD(ch))
			||	 (!str_cmp(arg2, "slayevil") && IS_EVIL(ch))
			||	 (!str_cmp(arg2, "slayneutral") && IS_NEUTRAL(ch)) )
			{
				do_tell_quest( ch, questman, "Una volta che l'arma e' legata al tuo spirito non risultera' maggiormente efficace contro chi appartiene al tuo stesso allineamento!" );
				return;
			}

			if ( IS_OBJ_STAT(wield, ITEM_SLAYGOOD)
			||	 IS_OBJ_STAT(wield, ITEM_SLAYEVIL)
			||	 IS_OBJ_STAT(wield, ITEM_SLAYNEUTRAL) )
			{

				do_tell_quest( ch, questman, "Quest'arma e' gia' legata al tuo spirito." );
				return;
			}

			if ( ch->pcdata->questpoints >= 10000 )
			{
				ch->pcdata->questpoints -= 10000;

				do_tell_quest( ch, questman, "Per il potere dagli Dei concessomi quest'arma riceve l'impronta del tuo animo, possa essa accompagnarti a lungo." );

				if ( !str_cmp(arg2, "slaygood") )
				{
					SET_BIT( wield->extra_flags, ITEM_SLAYGOOD );
					act( "$N evoca un'aura rossa infuocata sull'arma impugnata da $n.. essa divampa ardendo intensamente.", ch, NULL, questman, TO_ROOM, POS_REST );
					act( "$N evoca un'aura rossa infuocata sull'arma da te impugnata.. essa divampa ardendo intensamente.", ch, NULL, questman, TO_CHAR, POS_REST );
					act( "$CIl bagliore rossastro di una stella vermiglia appare per un attimo nel cielo.$c", ch, NULL, questman, TO_ALL, POS_SLEEP, RED );
				}

				if ( !str_cmp(arg2, "slayevil") )
				{
					SET_BIT( wield->extra_flags, ITEM_SLAYEVIL );
					do_tell_quest( ch, questman, "Per il potere dagli Dei concessomi quest'arma riceve l'impronta del tuo animo, possa essa accompagnarti a lungo." );
					act( "$N evoca un'aura bianca sull'arma impugnata da $n.. essa risplende emettendo un intenso bagliore.", ch, NULL, questman, TO_ROOM, POS_REST );
					act( "$N evoca un'aura bianca sull'arma da te impugnata. Essa risplende emettendo un intenso bagliore.", ch, NULL, questman, TO_CHAR, POS_REST );
					act( "$CUna lucente nuova stella sembra apparire per un attimo in cielo.$c", ch, NULL, questman, TO_ALL, POS_SLEEP, WHITE );
				}

				if ( !str_cmp(arg2, "slayneutral") )
				{
					SET_BIT( wield->extra_flags, ITEM_SLAYNEUTRAL );
					do_tell_quest( ch, questman, "Per il potere dagli Dei concessomi quest'arma riceve l'impronta del tuo animo, possa essa accompagnarti a lungo." );
					act( "$N evoca un'aura soffusamente argentata sull'arma impugnata da $n..", ch, NULL, questman, TO_ROOM, POS_REST );
					act( "$N evoca un'aura soffusamente argentata sull'arma impugnata da $n..", ch, NULL, questman, TO_CHAR, POS_REST );
					act( "$CUna stella di un pallido colore argentato attraversa per un attimo il cielo.$c", ch, NULL, questman, TO_ALL, POS_SLEEP, WHITE );
				}
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "backpack zaino") )
		{
			if ( ch->pcdata->questpoints >= 5000 )
			{
				ch->pcdata->questpoints -= 5000;
				obj = create_object( get_obj_index(QUEST_ITEM4), ch->level );
				if ( IS_SET(ch->quest, QUEST_BACKPACK) || IS_SET(ch->quest, QUEST_BACKPACK2) || IS_SET(ch->quest, QUEST_BACKPACK3) )
					do_tell_quest( ch, questman, "This quest item is beyond the trouble option." );
				else
					SET_BIT( ch->quest, QUEST_BACKPACK );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "decanter caraffa") )
		{
			if ( ch->pcdata->questpoints >= 500 )
			{
				ch->pcdata->questpoints -= 500;
				obj = create_object( get_obj_index(QUEST_ITEM5), ch->level );
				if ( IS_SET(ch->quest, QUEST_DECANTER) || IS_SET(ch->quest, QUEST_DECANTER2) || IS_SET(ch->quest, QUEST_DECANTER3) )
					do_tell_quest( ch, questman, "This quest item is beyond the trouble option." );
				else
					SET_BIT( ch->quest, QUEST_DECANTER );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "girth cintura") )
		{
			if ( ch->pcdata->questpoints >= 1000 )
			{
				ch->pcdata->questpoints -= 1000;
				obj = create_object( get_obj_index(QUEST_ITEM1), ch->level );
				if ( IS_SET(ch->quest, QUEST_GIRTH) || IS_SET(ch->quest, QUEST_GIRTH2) || IS_SET(ch->quest, QUEST_GIRTH3) )
					do_tell_quest( ch, questman, "This quest item is beyond the trouble option." );
				else
					SET_BIT( ch->quest, QUEST_GIRTH );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "ring anello") )
		{
			if ( ch->pcdata->questpoints >= 1000 )
			{
				ch->pcdata->questpoints -= 1000;
				obj = create_object( get_obj_index(QUEST_ITEM2), ch->level );
				if ( IS_SET(ch->quest, QUEST_RING) || IS_SET(ch->quest, QUEST_RING2) || IS_SET(ch->quest, QUEST_RING3) )
					do_tell_quest( ch, questman, "This quest item is beyond the trouble option." );
				else
					SET_BIT( ch->quest, QUEST_RING );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "weapon arma") )
		{
			if ( ch->pcdata->questpoints >= 1000 )
			{
				ch->pcdata->questpoints -= 1000;
				obj = create_object( get_obj_index(QUEST_ITEM3), ch->level );
				if ( IS_SET(ch->quest, QUEST_WEAPON) || IS_SET(ch->quest, QUEST_WEAPON2) || IS_SET(ch->quest, QUEST_WEAPON3) )
					do_tell_quest( ch, questman, "This quest item is beyond the trouble option." );
				else
					SET_BIT( ch->quest, QUEST_WEAPON );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "vampire vampiro") )
		{
			if ( ch->class != CLASS_VAMPIRE )
			{
				sprintf( buf, "You cannot gain this skill, %s.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( ch->pcdata->questpoints >= 100 )
			{
				ch->pcdata->questpoints -= 100;
				sn = skill_lookup( "vampire" );
				ch->pcdata->learned[sn] = 100;
				act( "$N gives secret of undead to $n.", ch, NULL, questman, TO_ROOM, POS_REST );
				act( "$N gives you SECRET of undead.", ch, NULL, questman, TO_CHAR, POS_REST );
				act( "$CLightning flashes in the sky.$c", ch, NULL, questman, TO_ALL, POS_SLEEP, BLUE );
				return;
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "train allenamento") )
		{
			if ( ch->pcdata->questpoints >= 250 )
			{
				ch->pcdata->questpoints -= 250;
				ch->train += 1;
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "dead samurai death morte morti") )
		{
			if ( ch->class != CLASS_SAMURAI )
			{
				sprintf( buf, "Sorry, %s, but you are not a samurai.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( ch->pcdata->death < 1 )
			{
				sprintf( buf, "Sorry, %s, you haven't god any death yet.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( ch->pcdata->questpoints >= 100 )
			{
				ch->pcdata->questpoints -= 100;
				ch->pcdata->death -= 1;
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "katana sword spada") )
		{
			AFFECT_DATA af;
			OBJ_DATA	*katana;
			/*~~~~~~~~~~~~~~~~*/

			if ( ch->class != CLASS_SAMURAI )
			{
				sprintf( buf, "Sorry, %s, but you are not a samurai.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( (katana = get_obj_list(ch, "katana", ch->carrying)) == NULL )
			{
				sprintf( buf, "Sorry, %s, but you don't have your katana with you.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( IS_WEAPON_STAT(katana, WEAPON_KATANA) )
			{
				sprintf( buf, "Sorry, %s, but your katana has already passed the first quest.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( ch->pcdata->questpoints >= 200 )
			{
				ch->pcdata->questpoints -= 200;
				af.where = TO_WEAPON;
				af.type = gsn_reserved;
				af.level = 100;
				af.duration = -1;
				af.modifier = 0;
				af.bitvector = WEAPON_KATANA;
				af.location = APPLY_NONE;
				affect_to_obj( katana, &af );
				sprintf( buf, "As you wield it, you will feel that it is power will increase, continuously." );
				do_tell_quest( ch, questman, buf );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "sharp second seconda affila") )
		{
			AFFECT_DATA af;
			OBJ_DATA	*katana;
			/*~~~~~~~~~~~~~~~~*/

			if ( ch->class != CLASS_SAMURAI )
			{
				sprintf( buf, "Sorry, %s, but you are not a samurai.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( (katana = get_obj_list(ch, "katana", ch->carrying)) == NULL )
			{
				sprintf( buf, "Sorry, %s, but you don't have your katana with you.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( !IS_WEAPON_STAT(katana, WEAPON_KATANA) )
			{
				sprintf( buf, "Sorry, %s, but your katana hasn't passed the first quest.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}

			if ( ch->pcdata->questpoints >= 200 )
			{
				ch->pcdata->questpoints -= 200;
				af.where = TO_WEAPON;
				af.type = gsn_reserved;
				af.level = 100;
				af.duration = -1;
				af.modifier = 0;
				af.bitvector = WEAPON_SHARP;
				af.location = APPLY_NONE;
				affect_to_obj( katana, &af );
				sprintf( buf, "From now on, your katana will be as sharp as blades of titans." );
				do_tell_quest( ch, questman, buf );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "tattoo religion tatuaggio religione") )
		{
			OBJ_DATA	*tattoo;
			/*~~~~~~~~~~~~~~~~*/

			if ( !(ch->religion) )
			{
				send_to_char( "You don't have a religion to have a tattoo.\n\r", ch );
				return;
			}

			tattoo = get_eq_char( ch, WEAR_TATTOO );
			if ( tattoo != NULL )
			{
				send_to_char( "But you have already your tattoo!.\n\r", ch );
				return;
			}

			if ( ch->pcdata->questpoints >= 200 )
			{
				ch->pcdata->questpoints -= 200;

				tattoo = create_object( get_obj_index(religion_table[ch->religion].vnum), 100 );

				obj_to_char( tattoo, ch );
				equip_char( ch, tattoo, WEAR_TATTOO );
				act( "$N tattoos $n with $p!.", ch, tattoo, questman, TO_ROOM, POS_REST );
				act( "$N tattoos you with $p!.", ch, tattoo, questman, TO_CHAR, POS_REST );
				return;
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "remove rimuovi") )
		{
			OBJ_DATA	*tattoo;
			/*~~~~~~~~~~~~~~~~*/

			if ( ch->pcdata->questpoints >= 50 )
			{
				if ( (tattoo = get_eq_char(ch, WEAR_TATTOO)) == NULL )
				{
					sprintf( buf, "But you don't have any tattoo!" );
					do_tell_quest( ch, questman, buf );
					return;
				}

				ch->pcdata->questpoints -= 50;
				extract_obj( tattoo );
				act( "Through a painful process, your tattoo has been destroyed by $n.", questman, NULL, ch, TO_VICT, POS_REST );
				act( "$N's tattoo is destroyed by $n.", questman, NULL, ch, TO_NOTVICT, POS_REST );
				return;
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else if ( is_name(arg2, "set tatua") )
		{
			if ( ch->pcdata->questpoints >= 50 )
			{
				if ( get_eq_char(ch, WEAR_TATTOO) != NULL )
				{
					sprintf( buf, "Sorry, %s, but you have to remove your tattoo first!", ch->name );
					do_tell_quest( ch, questman, buf );
					return;
				}

				if ( ch->religion == RELIGION_NONE )
				{
					sprintf( buf, "But you are already an atheist!" );
					do_tell_quest( ch, questman, buf );
					return;
				}

				ch->pcdata->questpoints -= 50;
				ch->religion = RELIGION_NONE;
				send_to_char( "You don't believe any religion now.\n\r", ch );
				act( "$N's religion is set to NONE!.", questman, NULL, ch, TO_NOTVICT, POS_REST );
			}
			else
			{
				sprintf( buf, "Mi dispiace, %s, ma al momento non disponi di quanto sia necessario per avere cio' che desideri.", ch->name );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}
		else
		{
			sprintf( buf, "I don't have that item, %s.", ch->name );
			do_tell_quest( ch, questman, buf );
		}

		if ( obj != NULL )
		{
			if ( obj->pIndexData->vnum == QUEST_ITEM4 || obj->pIndexData->vnum == QUEST_ITEM5 )
			{
				sprintf( buf, obj->pIndexData->extra_descr->description, ch->name );
				obj->extra_descr = new_extra_descr();
				obj->extra_descr->keyword = str_dup( obj->pIndexData->extra_descr->keyword );
				obj->extra_descr->description = str_dup( buf );
				obj->extra_descr->next = NULL;
			}

			if ( obj->pIndexData->vnum == QUEST_ITEM1 || obj->pIndexData->vnum == QUEST_ITEM2 || obj->pIndexData->vnum == QUEST_ITEM3 )
			{
				sprintf( buf, obj->short_descr, ch->name );
				free_string( obj->short_descr );
				obj->short_descr = str_dup( buf );
				obj->level = ch->level;	/* Gli forziamo il livello del pg */
			}

			act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM, POS_REST );
			act( "$N gives you $p.", ch, obj, questman, TO_CHAR, POS_REST );
			obj_to_char( obj, ch );
		}

		return;
	}
	else if ( !str_cmp(arg1, "request") || !str_cmp(arg1, "richiedi") )
	{
		act( "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM, POS_REST );
		act( "You ask $N for a quest.", ch, NULL, questman, TO_CHAR, POS_REST );
		if ( IS_SET(ch->act, PLR_QUESTOR) )
		{
			sprintf( buf, "But you're already on a quest!" );
			do_tell_quest( ch, questman, buf );
			return;
		}

		if ( ch->pcdata->nextquest > 0 )
		{
			sprintf( buf, "You're very brave, %s, but let someone else have a chance.", ch->name );
			do_tell_quest( ch, questman, buf );
			sprintf( buf, "Come back later." );
			do_tell_quest( ch, questman, buf );
			return;
		}

		sprintf( buf, "Thank you, brave %s!", ch->name );
		do_tell_quest( ch, questman, buf );

		generate_quest( ch, questman );

		if ( ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0 )
		{
			ch->pcdata->countdown = number_range( 15, 30 );
			SET_BIT( ch->act, PLR_QUESTOR );
			sprintf( buf, "You have %d minutes to complete this quest.", ch->pcdata->countdown );
			do_tell_quest( ch, questman, buf );
			sprintf( buf, "May the gods go with you!" );
			do_tell_quest( ch, questman, buf );
		}

		return;
	}
	else if ( !str_cmp(arg1, "complete") || !str_cmp(arg1, "completata") )
	{
		act( "$n informs $N $e has completed su$x quest.", ch, NULL, questman, TO_ROOM, POS_REST );
		act( "You inform $N you have completed su$x quest.", ch, NULL, questman, TO_CHAR, POS_REST );
		if ( ch->pcdata->questgiver != questman->pIndexData->vnum )
		{
			sprintf( buf, "I never sent you on a quest! Perhaps you're thinking of someone else." );
			do_tell_quest( ch, questman, buf );
			return;
		}

		if ( IS_SET(ch->act, PLR_QUESTOR) )
		{
			if ( ch->pcdata->questmob == -1 && ch->pcdata->countdown > 0 )
			{
				int reward = 0, pointreward = 0, pracreward = 0, level;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				level = ch->level;
				reward = 100 + dice( level, 20 );
				reward = UMAX( 180, reward );
				pointreward = number_range( 20, 40 );

				sprintf( buf, "Congratulations on completing your quest!" );
				do_tell_quest( ch, questman, buf );

				if ( chance(70) )
				{
					sprintf( buf, "As a reward, I am giving you %d quest points, and %d gold.", pointreward, reward );
					do_tell_quest( ch, questman, buf );
				}

				if ( chance(1) )
				{
					pracreward = number_range( 1, 6 );
					sprintf( buf, "You gain %d practices!\n\r", pracreward );
					send_to_char( buf, ch );
					ch->practice += pracreward;
				}

				REMOVE_BIT( ch->act, PLR_QUESTOR );
				ch->pcdata->questgiver = 0;
				ch->pcdata->countdown = 0;
				ch->pcdata->questmob = 0;
				ch->pcdata->questobj = 0;
				ch->pcdata->nextquest = 5;
				ch->gold += reward;
				ch->pcdata->questpoints += pointreward;

				return;
			}
			else if ( ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0 )
			{
				bool	obj_found = FALSE;
				/*~~~~~~~~~~~~~~~~~~~~~~*/

				for ( obj = ch->carrying; obj != NULL; obj = obj_next )
				{
					obj_next = obj->next_content;

					if (obj != NULL
					&&	obj->pIndexData->vnum == ch->pcdata->questobj
					&&	strstr(obj->extra_descr->description, ch->name) != NULL )
					{
						obj_found = TRUE;
						break;
					}
				}

				if ( obj_found == TRUE )
				{
					int reward, pointreward, pracreward;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					reward = 200 + number_range( 1, 20 * ch->level );
					pointreward = number_range( 15, 40 );

					act( "You hand $p to $N.", ch, obj, questman, TO_CHAR, POS_REST );
					act( "$n hands $p to $N.", ch, obj, questman, TO_ROOM, POS_REST );

					sprintf( buf, "Congratulations on completing your quest!" );
					do_tell_quest( ch, questman, buf );

					if ( chance(80) )
					{
						sprintf( buf, "As a reward, I am giving you %d quest points, and %d gold.", pointreward, reward );
						do_tell_quest( ch, questman, buf );
					}

					if ( chance(10) )
					{
						pracreward = number_range( 1, 4 );
						sprintf( buf, "You gain %d practices!\n\r", pracreward );
						send_to_char( buf, ch );
						ch->practice += pracreward;
					}

					REMOVE_BIT( ch->act, PLR_QUESTOR );
					ch->pcdata->questgiver = 0;
					ch->pcdata->countdown = 0;
					ch->pcdata->questmob = 0;
					ch->pcdata->questobj = 0;
					ch->pcdata->nextquest = 5;
					ch->gold += reward;
					ch->pcdata->questpoints += pointreward;
					extract_obj( obj );
					return;
				}
				else
				{
					sprintf( buf, "You haven't completed the quest yet, but there is still time!" );
					do_tell_quest( ch, questman, buf );
					return;
				}

				return;
			}
			else if ( (ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0) && ch->pcdata->countdown > 0 )
			{
				sprintf( buf, "You haven't completed the quest yet, but there is still time!" );
				do_tell_quest( ch, questman, buf );
				return;
			}
		}

		if ( ch->pcdata->nextquest > 0 )
			sprintf( buf, "But you didn't complete your quest in time!" );
		else
			sprintf( buf, "You have to REQUEST a quest first, %s.", ch->name );
		do_tell_quest( ch, questman, buf );
		return;
	}
	else if ( !str_cmp(arg1, "trouble") || !str_cmp(arg1, "problema") )
	{
		if ( arg2[0] == '\0' )
		{
			send_to_char( "To correct a quest award's trouble, type: quest trouble <award>'.\n\r", ch );
			return;
		}

		trouble_n = 0;
		if ( is_name(arg2, "girth cintura") )
		{
			if ( IS_SET(ch->quest, QUEST_GIRTH) )
			{
				REMOVE_BIT( ch->quest, QUEST_GIRTH );
				SET_BIT( ch->quest, QUEST_GIRTH2 );
				trouble_n = 1;
			}
			else if ( IS_SET(ch->quest, QUEST_GIRTH2) )
			{
				REMOVE_BIT( ch->quest, QUEST_GIRTH2 );
				SET_BIT( ch->quest, QUEST_GIRTH3 );
				trouble_n = 2;
			}
			else if ( IS_SET(ch->quest, QUEST_GIRTH3) )
			{
				REMOVE_BIT( ch->quest, QUEST_GIRTH3 );
				trouble_n = 3;
			}

			if ( trouble_n )
				trouble_vnum = QUEST_ITEM1;
		}
		else if ( is_name(arg2, "backpack zaino") )
		{
			if ( IS_SET(ch->quest, QUEST_BACKPACK) )
			{
				REMOVE_BIT( ch->quest, QUEST_BACKPACK );
				SET_BIT( ch->quest, QUEST_BACKPACK2 );
				trouble_n = 1;
			}
			else if ( IS_SET(ch->quest, QUEST_BACKPACK2) )
			{
				REMOVE_BIT( ch->quest, QUEST_BACKPACK2 );
				SET_BIT( ch->quest, QUEST_BACKPACK3 );
				trouble_n = 2;
			}
			else if ( IS_SET(ch->quest, QUEST_BACKPACK3) )
			{
				REMOVE_BIT( ch->quest, QUEST_BACKPACK3 );
				trouble_n = 3;
			}

			if ( trouble_n )
				trouble_vnum = QUEST_ITEM4;
		}
		else if ( is_name(arg2, "decanter caraffa") )
		{
			if ( IS_SET(ch->quest, QUEST_DECANTER) )
			{
				REMOVE_BIT( ch->quest, QUEST_DECANTER );
				SET_BIT( ch->quest, QUEST_DECANTER2 );
				trouble_n = 1;
			}
			else if ( IS_SET(ch->quest, QUEST_DECANTER2) )
			{
				REMOVE_BIT( ch->quest, QUEST_DECANTER2 );
				SET_BIT( ch->quest, QUEST_DECANTER3 );
				trouble_n = 2;

			}
			else if ( IS_SET(ch->quest, QUEST_DECANTER3) )
			{
				REMOVE_BIT( ch->quest, QUEST_DECANTER3 );
				trouble_n = 3;
			}

			if ( trouble_n )
				trouble_vnum = QUEST_ITEM5;
		}
		else if ( is_name(arg2, "weapon arma") )
		{
			if ( IS_SET(ch->quest, QUEST_WEAPON) )
			{
				REMOVE_BIT( ch->quest, QUEST_WEAPON );
				SET_BIT( ch->quest, QUEST_WEAPON2 );
				trouble_n = 1;
			}
			else if ( IS_SET(ch->quest, QUEST_WEAPON2) )
			{
				REMOVE_BIT( ch->quest, QUEST_WEAPON2 );
				SET_BIT( ch->quest, QUEST_WEAPON3 );
				trouble_n = 2;
			}
			else if ( IS_SET(ch->quest, QUEST_WEAPON3) )
			{
				REMOVE_BIT( ch->quest, QUEST_WEAPON3 );
				trouble_n = 3;
			}

			if ( trouble_n )
				trouble_vnum = QUEST_ITEM3;
		}
		else if ( is_name(arg2, "ring anello") )
		{
			if ( IS_SET(ch->quest, QUEST_RING) )
			{
				REMOVE_BIT( ch->quest, QUEST_RING );
				SET_BIT( ch->quest, QUEST_RING2 );
				trouble_n = 1;
			}
			else if ( IS_SET(ch->quest, QUEST_RING2) )
			{
				REMOVE_BIT( ch->quest, QUEST_RING2 );
				SET_BIT( ch->quest, QUEST_RING3 );
				trouble_n = 2;
			}
			else if ( IS_SET(ch->quest, QUEST_RING3) )
			{
				REMOVE_BIT( ch->quest, QUEST_RING3 );
				trouble_n = 3;
			}

			if ( trouble_n )
				trouble_vnum = QUEST_ITEM2;
		}

		if ( !trouble_n )
		{
			sprintf( buf, "Sorry %s, but you haven't bought that quest award, yet.\n\r", ch->name );
			do_tell_quest( ch, questman, buf );
			return;
		}

		for ( obj = object_list; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next;
			if ( obj->pIndexData->vnum == trouble_vnum && strstr(obj->short_descr, ch->name) )
			{
				extract_obj( obj );
				break;
			}
		}

		obj = create_object( get_obj_index(trouble_vnum), ch->level );
		if ( obj->pIndexData->vnum == QUEST_ITEM4 || obj->pIndexData->vnum == QUEST_ITEM5 )
		{
			sprintf( buf, obj->pIndexData->extra_descr->description, ch->name );
			obj->extra_descr = new_extra_descr();
			obj->extra_descr->keyword = str_dup( obj->pIndexData->extra_descr->keyword );
			obj->extra_descr->description = str_dup( buf );
			obj->extra_descr->next = NULL;
		}

		if ( obj->pIndexData->vnum == QUEST_ITEM1 || obj->pIndexData->vnum == QUEST_ITEM2 || obj->pIndexData->vnum == QUEST_ITEM3 )
		{
			sprintf( buf, obj->short_descr, IS_GOOD(ch) ? "holy" : IS_NEUTRAL(ch) ? "blue-green" : "evil", ch->name );
			free_string( obj->short_descr );
			obj->short_descr = str_dup( buf );
		}

		act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM, POS_REST );
		act( "$N gives you $p.", ch, obj, questman, TO_CHAR, POS_REST );
		obj_to_char( obj, ch );
		sprintf
		(
			buf,
			"This is the %i%s time that i am giving that award back.",
			trouble_n,
			(trouble_n == 1) ? "st" : (trouble_n == 2) ? "nd" : "rd"
		);
		do_tell_quest( ch, questman, buf );
		if ( trouble_n == 3 )
		{
			sprintf( buf, "And I won't give you that again, with trouble option.\n\r" );
			do_tell_quest( ch, questman, buf );
		}

		return;
	}

	send_to_char( "QUEST COMMANDS: points info request complete list buy trouble.\n\r", ch );
	send_to_char( "For more information, type: help quests.\n\r", ch );
}


void generate_quest( CHAR_DATA *ch, CHAR_DATA *questman )
{
	CHAR_DATA		*victim;
	MOB_INDEX_DATA	*vsearch;
	ROOM_INDEX_DATA *room;
	OBJ_DATA		*eyed;
	char			buf[MSL];
	int				level_diff;
	register int	i;
	register int	mob_count;
	int				mob_buf[300];
	int				found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	room = alloc_perm( sizeof(*room) );

	mob_count = 0;
	for ( i = 0; i < MAX_KEY_HASH; i++ )
	{
		if ( (vsearch = mob_index_hash[i]) == NULL )					continue;

		level_diff = vsearch->level - ch->level;
		if ( ch->level < 51 && (level_diff > 4 || level_diff < -1) )	continue;
		if ( ch->level > 50 && (level_diff > 6 || level_diff <  0) )	continue;

		if ( vsearch->pShop != NULL )									continue;

		if ( IS_SET(vsearch->act, ACT_TRAIN) )							continue;
		if ( IS_SET(vsearch->act, ACT_PRACTICE) )						continue;
		if ( IS_SET(vsearch->act, ACT_IS_HEALER) )						continue;
		if ( IS_SET(vsearch->act, ACT_NOTRACK) )						continue;
		if ( IS_SET(vsearch->imm_flags, IMM_SUMMON) )					continue;

		mob_buf[mob_count] = vsearch->vnum;
		mob_count++;

		if ( mob_count > 1299 )
			break;
	}

	if ( chance(40) )
	{
		int objvnum = 0;
		/*~~~~~~~~~~~~*/

		if ( mob_count > 0 )
		{
			found = number_range( 0, mob_count-1 );
			for ( i = 0; i < mob_count; i++ )
			{
				if ( (vsearch = get_mob_index(mob_buf[found])) == NULL )
				{
					bug( "Unknown mob in generate_obj_quest: %d", mob_buf[found] );
					found++;
					if ( found > (mob_count - 1) )
						break;
					else
						continue;
				}
				else
					break;
			}
		}
		else
			vsearch = NULL;

		if ( vsearch == NULL || (victim = get_quest_world(ch, vsearch)) == NULL )
		{
			sprintf( buf, "I'm sorry, but I don't have any quests for you at this time." );
			do_tell_quest( ch, questman, buf );
			sprintf( buf, "Try again later." );
			do_tell_quest( ch, questman, buf );
			ch->pcdata->nextquest = 5;
			return;
		}

		if ( (room = victim->in_room) == NULL )
		{
			sprintf( buf, "I'm sorry, but I don't have any quests for you at this time." );
			do_tell_quest( ch, questman, buf );
			sprintf( buf, "Try again later." );
			do_tell_quest( ch, questman, buf );
			ch->pcdata->nextquest = 5;
			return;
		}

		switch ( number_range(0, 3) )
		{
		case 0: objvnum = QUEST_OBJQUEST1; break;
		case 1: objvnum = QUEST_OBJQUEST2; break;
		case 2: objvnum = QUEST_OBJQUEST3; break;
		case 3: objvnum = QUEST_OBJQUEST4; break;
		}

		if ( IS_GOOD(ch) )
			i = 0;
		else if ( IS_EVIL(ch) )
			i = 2;
		else
			i = 1;

		eyed = create_object( get_obj_index(objvnum), ch->level );
		eyed->owner = str_dup( ch->name );
		eyed->from = str_dup( ch->name );
		eyed->altar = hometown_table[ch->hometown].altar[i];
		eyed->pit = hometown_table[ch->hometown].pit[i];
		eyed->level = ch->level;

		sprintf( buf, eyed->description, ch->name );
		free_string( eyed->description );
		eyed->description = str_dup( buf );

		sprintf( buf, eyed->pIndexData->extra_descr->description, ch->name );
		eyed->extra_descr = new_extra_descr();
		eyed->extra_descr->keyword = str_dup( eyed->pIndexData->extra_descr->keyword );
		eyed->extra_descr->description = str_dup( buf );
		eyed->extra_descr->next = NULL;

		eyed->cost = 0;
		eyed->timer = 30;

		obj_to_room( eyed, room );
		ch->pcdata->questobj = eyed->pIndexData->vnum;

		sprintf( buf, "Vile pilferers have stolen %s from the royal treasury!", eyed->short_descr );
		do_tell_quest( ch, questman, buf );
		do_tell_quest( ch, questman, "My court wizardess, with her magic mirror, has pinpointed its location." );

		/*
		 * I changed my area names so that they have just the name of the area and none of the level stuff. You may
		 * want to comment these next two lines. - Vassago
		 */
		sprintf( buf, "Look in the general area of %s for %s!", room->area->name, room->name );
		do_tell_quest( ch, questman, buf );
		return;
	}

	/* Quest to kill a mob */
	else
	{
		if ( mob_count > 0 )
		{
			found = number_range( 0, mob_count - 1 );
			for ( i = 0; i < mob_count; i++ )
			{
				if ((vsearch = get_mob_index(mob_buf[found])) == NULL
				||	(IS_EVIL(vsearch) && IS_EVIL(ch))
				||	(IS_GOOD(vsearch) && IS_GOOD(ch))
				||	(IS_NEUTRAL(vsearch) && IS_NEUTRAL(ch)) )
				{
					if ( vsearch == NULL )
						bug( "Unknown mob in mob_quest: %d", mob_buf[found] );
					found++;
					if ( found > (mob_count - 1) )
					{
						vsearch = NULL;
						break;
					}
					else
						continue;
				}
				else
					break;
			}
		}
		else
			vsearch = NULL;

		if (vsearch == NULL
		||	(victim = get_quest_world(ch, vsearch)) == NULL
		||	(room = victim->in_room) == NULL
		||	IS_SET(room->area->area_flag, AREA_HOMETOWN) )
		{
			sprintf( buf, "I'm sorry, but I don't have any quests for you at this time." );
			do_tell_quest( ch, questman, buf );
			sprintf( buf, "Try again later." );
			do_tell_quest( ch, questman, buf );
			ch->pcdata->nextquest = 5;
			return;
		}

		if ( IS_GOOD(ch) )
		{
			sprintf( buf, "Rune's most heinous criminal, %s, has escaped from the dungeon!", victim->short_descr );
			do_tell_quest( ch, questman, buf );
			sprintf( buf, "Since the escape, %s has murdered %d civillians!", victim->short_descr, number_range(2, 20) );
			do_tell_quest( ch, questman, buf );
			do_tell_quest( ch, questman, "The penalty for this crime is death, and you are to deliver the sentence!" );
		}
		else
		{
			sprintf( buf, "An enemy of mine, %s, is making vile threats against the crown.", victim->short_descr );
			do_tell_quest( ch, questman, buf );
			sprintf( buf, "This threat must be eliminated!" );
			do_tell_quest( ch, questman, buf );
		}

		if ( room->name != NULL )
		{
			sprintf( buf, "Seek %s out in vicinity of %s!", victim->short_descr, room->name );
			do_tell_quest( ch, questman, buf );

			/*
			 * I changed my area names so that they have just the name of the area and none of the level stuff. You
			 * may want to comment these next two lines. - Vassago
			 */
			sprintf( buf, "That location is in the general area of %s.", room->area->name );
			do_tell_quest( ch, questman, buf );
		}

		ch->pcdata->questmob = victim->pIndexData->vnum;
	}
}


/*
 * Called from update_handler() by pulse_area
 */
void quest_update( void )
{
	CHAR_DATA	*ch, *ch_next;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next = ch->next;

		if ( IS_MOB(ch) )
			continue;

		if ( ch->pcdata->nextquest > 0 )
		{
			ch->pcdata->nextquest--;


			if ( ch->pcdata->nextquest == 0 )
			{
				send_to_char( "You may now quest again.\n\r", ch );
				continue;
			}
		}
		else if ( IS_SET(ch->act, PLR_QUESTOR) )
		{
			if ( --ch->pcdata->countdown <= 0 )
			{
				char	buf[MSL];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				ch->pcdata->nextquest = 0;
				sprintf( buf, "You have run out of time for your quest!\n\rYou may now quest again.\n\r" );
				send_to_char( buf, ch );
				REMOVE_BIT( ch->act, PLR_QUESTOR );
				ch->pcdata->questgiver = 0;
				ch->pcdata->countdown = 0;
				ch->pcdata->questmob = 0;
				ch->pcdata->questobj = 0;
			}

			if ( ch->pcdata->countdown > 0 && ch->pcdata->countdown < 6 )
			{
				send_to_char( "Better hurry, you're almost out of time for your quest!\n\r", ch );
				continue;
			}
		}
	}
}


void do_tell_quest( CHAR_DATA *ch, CHAR_DATA *victim, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "%s tells you %s\n\r", victim->name, argument );
	send_to_char( buf, ch );
}


CHAR_DATA *get_quest_world( CHAR_DATA *ch, MOB_INDEX_DATA *victim )
{
	CHAR_DATA	*wch;
	/*~~~~~~~~~~~~~*/

	for ( wch = char_list; wch != NULL; wch = wch->next )
	{
		if ( wch->in_room == NULL || wch->pIndexData != victim )
			continue;

		return( wch );
	}

	return( NULL );
}
