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
#include "magic.h"
#include "interp.h"


/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN( spec_breath_any );
DECLARE_SPEC_FUN( spec_breath_acid );
DECLARE_SPEC_FUN( spec_breath_fire );
DECLARE_SPEC_FUN( spec_breath_frost );
DECLARE_SPEC_FUN( spec_breath_gas );
DECLARE_SPEC_FUN( spec_breath_lightning );
DECLARE_SPEC_FUN( spec_cast_adept );
DECLARE_SPEC_FUN( spec_cast_cleric );
DECLARE_SPEC_FUN( spec_cast_judge );
DECLARE_SPEC_FUN( spec_cast_mage );
DECLARE_SPEC_FUN( spec_cast_beholder );
DECLARE_SPEC_FUN( spec_cast_undead );
DECLARE_SPEC_FUN( spec_executioner );
DECLARE_SPEC_FUN( spec_fido );
DECLARE_SPEC_FUN( spec_guard );
DECLARE_SPEC_FUN( spec_janitor );
DECLARE_SPEC_FUN( spec_mayor );
DECLARE_SPEC_FUN( spec_poison );
DECLARE_SPEC_FUN( spec_thief );
DECLARE_SPEC_FUN( spec_nasty );
DECLARE_SPEC_FUN( spec_cast_cabal );
DECLARE_SPEC_FUN( spec_special_guard );
DECLARE_SPEC_FUN( spec_stalker );
DECLARE_SPEC_FUN( spec_questmaster );
DECLARE_SPEC_FUN( spec_assassinater );
DECLARE_SPEC_FUN( spec_repairman );
DECLARE_SPEC_FUN( spec_repair_cloth );
DECLARE_SPEC_FUN( spec_captain );
DECLARE_SPEC_FUN( spec_headlamia );


/*
 * Cabal guardians
 */
DECLARE_SPEC_FUN( spec_fight_enforcer );
DECLARE_SPEC_FUN( spec_fight_invader );
DECLARE_SPEC_FUN( spec_fight_ivan );
DECLARE_SPEC_FUN( spec_fight_seneschal );
DECLARE_SPEC_FUN( spec_fight_powerman );
DECLARE_SPEC_FUN( spec_fight_protector );
DECLARE_SPEC_FUN( spec_fight_hunter );
DECLARE_SPEC_FUN( spec_fight_lionguard );


/*
 * The function table
 */
const struct spec_type	spec_table[] =
{
	{ "spec_breath_any",		spec_breath_any },
	{ "spec_breath_acid",		spec_breath_acid },
	{ "spec_breath_fire",		spec_breath_fire },
	{ "spec_breath_frost",		spec_breath_frost },
	{ "spec_breath_gas",		spec_breath_gas },
	{ "spec_breath_lightning",	spec_breath_lightning },
	{ "spec_cast_adept",		spec_cast_adept },
	{ "spec_cast_cleric",		spec_cast_cleric },
	{ "spec_cast_judge",		spec_cast_judge },
	{ "spec_cast_mage",			spec_cast_mage },
	{ "spec_cast_beholder",		spec_cast_beholder },
	{ "spec_cast_undead",		spec_cast_undead },
	{ "spec_executioner",		spec_executioner },
	{ "spec_fido",				spec_fido },
	{ "spec_guard",				spec_guard },
	{ "spec_janitor",			spec_janitor },
	{ "spec_mayor",				spec_mayor },
	{ "spec_poison",			spec_poison },
	{ "spec_thief",				spec_thief },
	{ "spec_nasty",				spec_nasty },
	{ "spec_cast_cabal",		spec_cast_cabal },
	{ "spec_stalker",			spec_stalker },
	{ "spec_special_guard",		spec_special_guard },
	{ "spec_questmaster",		spec_questmaster },
	{ "spec_assassinater",		spec_assassinater },
	{ "spec_repairman",			spec_repairman },
	{ "spec_repair_cloth",		spec_repair_cloth },
	{ "spec_captain",			spec_captain },
	{ "spec_headlamia",			spec_headlamia },
	{ "spec_fight_enforcer",	spec_fight_enforcer },
	{ "spec_fight_invader",		spec_fight_invader },
	{ "spec_fight_ivan",		spec_fight_ivan },
	{ "spec_fight_seneschal",	spec_fight_seneschal },
	{ "spec_fight_powerman",	spec_fight_powerman },
	{ "spec_fight_protector",	spec_fight_protector },
	{ "spec_fight_hunter",		spec_fight_hunter },
	{ "spec_fight_lionguard",	spec_fight_lionguard },
	{ NULL,						NULL }
};


/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
	int i;
	/*~~*/

	for ( i = 0; spec_table[i].name != NULL; i++ )
	{
		if ( LOWER(name[0]) == LOWER(spec_table[i].name[0]) && !str_prefix(name, spec_table[i].name) )
			return( spec_table[i].function );
	}

	return( 0 );
}


char *spec_name( SPEC_FUN *function )
{
	int i;
	/*~~*/

	for ( i = 0; spec_table[i].function != NULL; i++ )
	{
		if ( function == spec_table[i].function )
			return( spec_table[i].name );
	}

	return( NULL );
}


/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( ((RIDDEN(ch) && RIDDEN(ch)->fighting == victim) || victim->fighting == ch) && number_bits(3) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	if ( (sn = skill_lookup(spell_name)) < 0 )
		return( FALSE );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
	if ( ch->position != POS_FIGHT )
		return( FALSE );

	switch ( number_bits(3) )
	{
	case 0:		return( spec_breath_fire(ch) );
	case 1:
	case 2:		return( spec_breath_lightning(ch) );
	case 3:		return( spec_breath_gas(ch) );
	case 4:		return( spec_breath_acid(ch) );
	case 5:
	case 6:
	case 7:		return( spec_breath_frost(ch) );
	}

	return( FALSE );
}


bool spec_breath_acid( CHAR_DATA *ch )
{
	return( dragon(ch, "acid breath") );
}


bool spec_breath_fire( CHAR_DATA *ch )
{
	return( dragon(ch, "fire breath") );
}


bool spec_breath_frost( CHAR_DATA *ch )
{
	return( dragon(ch, "frost breath") );
}


bool spec_breath_gas( CHAR_DATA *ch )
{
	int sn;
	/*~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	if ( (sn = skill_lookup("gas breath")) < 0 )
		return( FALSE );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, NULL, TARGET_CHAR );
	return( TRUE );
}


bool spec_breath_lightning( CHAR_DATA *ch )
{
	return( dragon(ch, "lightning breath") );
}


bool spec_cast_adept( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	/*~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim != ch && can_see(ch, victim) && number_bits(1) == 0 && !IS_MOB(victim) && victim->level <= LEVEL_NEWBIE )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( number_bits(4) )
	{
	case 0:
		act( "$n pronuncia le parole 'abrazak'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_armor( skill_lookup("armor"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 1:
		act( "$n pronuncia le parole 'fido'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_bless( skill_lookup("bless"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 2:
		act( "$n pronuncia le parole 'judicandus noselacri'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_cure_blindness( skill_lookup("cure blindness"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 3:
		act( "$n pronuncia le parole 'judicandus dies'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_cure_light( skill_lookup("cure light"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 4:
		act( "$n pronuncia le parole 'judicandus sausabru'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_cure_poison( skill_lookup("cure poison"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 5:
		act( "$n pronuncia le parole 'candusima'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_refresh( skill_lookup("refresh"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 6:
		act( "$n pronuncia le parole 'judicandus eugzagz'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_cure_disease( skill_lookup("cure disease"), ch->level, ch, victim, TARGET_CHAR );
	}

	return( FALSE );
}


bool spec_cast_cleric( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(2) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	mob_cast_cleric( ch, victim );
	return( TRUE );
}


bool spec_cast_judge( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(2) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	spell = "high explosive";
	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_cast_mage( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(2) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	mob_cast_mage( ch, victim );
	return( TRUE );
}


bool spec_cast_undead( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(2) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	for ( ;; )
	{
		int min_level;
		/*~~~~~~~~~~*/

		switch ( number_bits(4) )
		{
		case 0:		min_level =  0;	spell = "curse";		break;
		case 1:		min_level =  3;	spell = "weaken";		break;
		case 2:		min_level =  6;	spell = "chill touch";	break;
		case 3:		min_level =  9;	spell = "blindness";	break;
		case 4:		min_level = 12;	spell = "poison";		break;
		case 5:		min_level = 15;	spell = "energy drain";	break;
		case 6:		min_level = 18;	spell = "harm";			break;
		case 7:		min_level = 21;	spell = "teleport";		break;
		case 8:		min_level = 20;	spell = "plague";		break;
		default:	min_level = 18;	spell = "harm";			break;
		}

		if ( ch->level >= min_level )
			break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_executioner( CHAR_DATA *ch )
{
	char		buf[MSL];
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*crime;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) || ch->fighting != NULL )
		return( FALSE );

	crime = "";
	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;

		if ( !IS_MOB(victim) && IS_SET(victim->act, PLR_WANTED) && can_see(ch, victim) )
		{
			crime = "CRIMINALE";
			break;
		}
	}

	if ( victim == NULL )
		return( FALSE );

	sprintf( buf, "%s e' %s! Prendiamol%c!", victim->name, crime, victim->sex == 2 ? 'a' : 'o' );
	REMOVE_BIT( ch->comm, COMM_NOYELL );
	do_yell( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return( TRUE );
}


bool spec_fido( CHAR_DATA *ch )
{
	OBJ_DATA	*corpse;
	OBJ_DATA	*c_next;
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) )
		return( FALSE );

	for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
	{
		c_next = corpse->next_content;
		if ( corpse->item_type != ITEM_CORPSE_NPC )
			continue;

		act( "$n divora selvaggiamente un cadavere.", ch, NULL, NULL, TO_ROOM, POS_REST );
		for ( obj = corpse->contains; obj; obj = obj_next )
		{
			obj_next = obj->next_content;
			obj_from_obj( obj );
			obj_to_room( obj, ch->in_room );
		}

		extract_obj( corpse );
		return( TRUE );
	}

	return( FALSE );
}


bool spec_janitor( CHAR_DATA *ch )
{
	OBJ_DATA	*trash;
	OBJ_DATA	*trash_next;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) )
		return( FALSE );

	for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
	{
		trash_next = trash->next_content;
		if ( !IS_SET(trash->wear_flags, ITEM_TAKE) || !can_loot(ch, trash) )
			continue;

		/* (TT) da testare */
		if ( trash->item_type == ITEM_CORPSE_NPC )
		{
			if ( number_range(1, 3) == 1 )
				do_say( ch, "Argh! Un cadavere! Vediamo se riesco a sacrificarlo agli Dei e fare pulizia" );
			do_sacrifice( ch, "corpo" );
			return( TRUE );
		}

		if ( trash->item_type == ITEM_CORPSE_PC )
		{
			sh_int	nr;
			/*~~~~~~~*/

			nr = number_range( 1, 30 );
			if ( nr == 1 )
				do_say( ch, "Per gli Dei un cadavere!" );
			else if ( nr == 2 )
				do_say( ch, "Ci e' scappato il morto.. Non vorranno mica che ripulisca io???" );
			else if ( nr == 3 )
				do_say( ch, "Che i vermi si mangino questo cadavere se nessuno viene a controllare al piu' presto.." );

			return( TRUE );
		}

		/* Xandra. Occhio, raccoglie gli oggetti cabala volendo, quindi regolatevi se non volete cio'. */
		if ( trash->item_type == ITEM_DRINK_CON
		||	 trash->item_type == ITEM_TRASH
		||	 trash->cost < 20
		||	 trash->item_type != ITEM_CONTAINER )
		{
			act( "$n raccoglie della spazzatura.", ch, NULL, NULL, TO_ROOM, POS_REST );
			obj_from_room( trash );
			obj_to_char( trash, ch );
			if ( IS_SET(trash->progtypes, OPROG_GET) )
				( trash->pIndexData->oprogs->get_prog ) ( trash, ch );
			return( TRUE );
		}
	}

	return( FALSE );
}


bool spec_mayor( CHAR_DATA *ch )
{
	/* (FF) Xandra, probabilmente dopo la nuova area saranno da rivedere */
	static const char	open_path[] = "W3a3003b000c000d111Oe333333Oe22c222112212111a1S.";
	static const char	close_path[] = "W3a3003b000c000d111CE333333CE22c222112212111a1S.";
	static const char	*path;
	static int			pos;
	static bool			move;
	OBJ_DATA			*key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !move )
	{
		if ( time_info.hour == 6 )
		{
			path = open_path;
			move = TRUE;
			pos = 0;
		}

		if ( time_info.hour == 20 )
		{
			path = close_path;
			move = TRUE;
			pos = 0;
		}
	}

	if ( !move || ch->position < POS_SLEEP )
		return( FALSE );

	switch ( path[pos] )
	{
	case '0':
	case '1':
	case '2':
	case '3':
		move_char( ch, path[pos] - '0', FALSE );
		break;

	case 'W':
		ch->position = POS_STAND;
		act( "$n si stiracchia borbottando rumorosamente.", ch, NULL, NULL, TO_ROOM, POS_REST );
		break;

	case 'S':
		ch->position = POS_SLEEP;
		act( "$n sbadiglia distendendosi mentre si prepara al sonno.", ch, NULL, NULL, TO_ROOM, POS_REST );
		break;

	case 'a':
		do_say( ch, "Ehila', quale buon vento!" );
		break;

	case 'b':
		do_say( ch, "Che orrore.. dovremmo fare qualcosa per questa maledetta discarica!" );
		break;

	case 'c':
		do_say( ch, "Questi vandali.. non hanno rispetto proprio per nulla!" );
		break;

	case 'd':
		do_say( ch, "Bungiorno a tutti!" );
		break;

	case 'e':
		do_say( ch, "E cosi' dichiaro i cancelli di Midgaard APERTI!!!" );
		break;

	case 'E':
		do_say( ch, "E cosi' dichiaro i cancelli di Midgaard CHIUSI!!!" );
		break;

	case 'O':
		do_unlock( ch, "gate" );
		do_open( ch, "gate" );
		interpret( ch, "sblocca la serratura del cancello e prende la chiave.", FALSE );
		for ( key = ch->in_room->contents; key != NULL; key = key->next_content )
			if ( key->pIndexData->vnum == 3379 )
				break;
		if ( key != NULL )
			SET_BIT( key->wear_flags, ITEM_TAKE );
		do_get( ch, "gatekey" );
		break;

	case 'C':
		do_close( ch, "gate" );
		do_lock( ch, "gate" );
		do_drop( ch, "key" );
		interpret( ch, "ferma la chiave al cancello con una grossa catena.", FALSE );
		for ( key = ch->in_room->contents; key != NULL; key = key->next_content )
			if ( key->pIndexData->vnum == 3379 )
				break;
		if ( key != NULL )
			REMOVE_BIT( key->wear_flags, ITEM_TAKE );
		break;

	case '.':
		move = FALSE;
		break;
	}

	pos++;
	return( FALSE );
}


bool spec_poison( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT || (victim = ch->fighting) == NULL || number_percent() > 2 * ch->level )
		return( FALSE );

	act( "Mordi $N!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n morde $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n ti morde!", ch, NULL, victim, TO_VICT, POS_REST );
	spell_poison( gsn_poison, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_thief( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	long		gold, silver;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_STAND )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;

		if ( IS_MOB(victim) || victim->level >= LEVEL_ADMIN || number_bits(5) != 0 || !can_see(ch, victim) )
			continue;

		if ( IS_AWAKE(victim) && number_range(0, ch->level) == 0 )
		{
			act( "Con la coda dell'occhio noti la mano di $n frugarti nelle tasche!!!", ch, NULL, victim, TO_VICT, POS_REST );
			return( TRUE );
		}
		else
		{
			gold = victim->gold * UMIN( number_range(1, 20), ch->level / 2 ) / 100;
			gold = UMIN( gold, ch->level * ch->level * 10 );
			ch->gold += gold;
			victim->gold -= gold;
			silver = victim->silver * UMIN( number_range(1, 20), ch->level / 2 ) / 100;
			silver = UMIN( silver, ch->level * ch->level * 25 );
			ch->silver += silver;
			victim->silver -= silver;
			return( TRUE );
		}
	}

	return( FALSE );
}


bool spec_cast_cabal( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	/*~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim != ch && can_see(ch, victim) && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( number_bits(4) )
	{
	case 0:
		act( "$n pronuncia la parola 'abracal'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_armor( skill_lookup("armor"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 1:
		act( "$n pronuncia la parola 'balc'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_bless( skill_lookup("bless"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 2:
		act( "$n pronuncia la parola 'judicandus noselacba'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_cure_blindness( skill_lookup("cure blindness"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 3:
		act( "$n pronuncia la parola 'judicandus bacla'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_cure_light( skill_lookup("cure light"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 4:
		act( "$n pronuncia la parola 'judicandus sausabcla'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_cure_poison( skill_lookup("cure poison"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );

	case 5:
		act( "$n pronuncia la parola 'candabala'.", ch, NULL, NULL, TO_ROOM, POS_REST );
		spell_refresh( skill_lookup("refresh"), ch->level, ch, victim, TARGET_CHAR );
		return( TRUE );
	}

	return( FALSE );
}


bool spec_guard( CHAR_DATA *ch )
{
	char		buf[MSL];
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	CHAR_DATA	*ech;
	char		*crime;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) || ch->fighting != NULL )
		return( FALSE );

	ech = NULL;
	crime = "";

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;

		if ( !can_see(ch, victim) )
			continue;

		if ( IS_SET(ch->in_room->area->area_flag, AREA_HOMETOWN) && number_percent() < 1 )
		{
			do_say( ch, "Scusate un attimo." );
			if ( str_cmp(ch->in_room->area->name, hometown_table[victim->hometown].name) )
			{
				do_say( ch, "Non mi ricordo di voi, siete di questa citta'!" );
			}
			else
			{
				do_say( ch, "Oh salve a voi, buongiorno." );
				interpret( ch, "smile", FALSE );
			}
		}

		if ( !IS_MOB(victim) && IS_SET(victim->act, PLR_WANTED) )
		{
			crime = "Criminale";
			break;
		}

		if ( victim->fighting != NULL && victim->fighting != ch
		&& victim->ethos != 1 && !IS_GOOD(victim) && !IS_EVIL(victim->fighting) )
		{
			ech = victim;
			victim = NULL;
			break;
		}
	}

	if ( victim != NULL )
	{
		sprintf( buf, "%s e' %s! Prendiamol%c!",
			victim->name, crime, (victim->sex == SEX_FEMALE) ? 'a' : 'o' );
		do_yell( ch, buf );
		multi_hit( ch, victim, TYPE_UNDEFINED );
		return( TRUE );
	}

	if ( ech != NULL )
	{
		act( "$n urla 'Il crimine non paga!!!'", ch, NULL, NULL, TO_ROOM, POS_REST );
		multi_hit( ch, ech, TYPE_UNDEFINED );
		return( TRUE );
	}

	return( FALSE );
}


bool spec_special_guard( CHAR_DATA *ch )
{
	char		buf[MSL];
	CHAR_DATA	*victim, *ech;
	CHAR_DATA	*v_next;
	char		*crime;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) || ch->fighting != NULL )
		return( FALSE );

	crime = "";
	ech = NULL;

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;

		if ( !can_see(ch, victim) )
			continue;

		if ( !IS_MOB(victim) && IS_SET(victim->act, PLR_WANTED) )
		{
			crime = "CRIMINAL";
			break;
		}

		if ( victim->fighting != NULL && victim->fighting != ch && victim->fighting->cabal == CABAL_RULER )
		{
			ech = victim;
			victim = NULL;
			break;
		}
	}

	if ( victim != NULL )
	{
		sprintf( buf, "%s e' un %s! Tutti addosso!!!", victim->name, crime );
		do_yell( ch, buf );
		multi_hit( ch, victim, TYPE_UNDEFINED );
		return( TRUE );
	}

	if ( ech != NULL )
	{
		act( "$n screams 'La tua strada finisce qui, criminale!'", ch, NULL, NULL, TO_ROOM, POS_REST );
		multi_hit( ch, ech, TYPE_UNDEFINED );
		return( TRUE );
	}

	return( FALSE );
}


bool spec_stalker( CHAR_DATA *ch )
{
	char		buf[MSL];
	CHAR_DATA	*victim;
	CHAR_DATA	*wch;
	CHAR_DATA	*wch_next;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	victim = ch->last_fought;

	if ( ch->fighting != NULL )
		return( FALSE );

	if ( ch->status == 10 )
	{
		ch->cabal = CABAL_RULER;
		do_yell( ch, "Ho ucciso la mia preda..ora posso ritirarmi." );
		extract_char( ch, TRUE );
		return( TRUE );
	}

	if ( victim == NULL )
	{
		ch->cabal = CABAL_RULER;
		do_yell( ch, "La codardia ha portato via la mia vittima.. la aspettero' in altri luoghi.." );
		extract_char( ch, TRUE );
		return( TRUE );
	}

	if ( IS_GOOD(victim) )		i = 0;
	if ( IS_EVIL(victim) )		i = 2;
	else						i = 1;

	for ( wch = ch->in_room->people; wch != NULL; wch = wch_next )
	{
		wch_next = wch->next_in_room;
		if ( victim == wch )
		{
			sprintf( buf, "Oh..%s.. qui ti trovo finalmente..", victim->name );
			do_yell( ch, buf );
			multi_hit( ch, wch, TYPE_UNDEFINED );
			return( TRUE );
		}
	}

	do_track( ch, victim->name );

	if ( ch->status == 5 )
	{
		if ( ch->in_room != get_room_index(hometown_table[victim->hometown].recall[1]) )
		{
			char_from_room( ch );
			char_to_room( ch, get_room_index(hometown_table[victim->hometown].recall[i]) );
			do_track( ch, victim->name );
			return( TRUE );
		}
		else
		{
			ch->cabal = CABAL_RULER;
			sprintf( buf, "Ho perso le tracce %s. Non posso piu' continuare.. ora.", victim->name );
			do_yell( ch, buf );
			extract_char( ch, TRUE );
			return( TRUE );
		}
	}

	return( FALSE );
}


bool spec_nasty( CHAR_DATA *ch )
{
	CHAR_DATA	*victim, *v_next;
	long		gold;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_AWAKE(ch) )
	{
		return( FALSE );
	}

	if ( ch->position != POS_FIGHT )
	{
		for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
		{
			v_next = victim->next_in_room;
			if ( !IS_MOB(victim) && (victim->level > ch->level) && (victim->level < ch->level + 10) )
			{
				do_backstab( ch, victim->name );
				if ( ch->position != POS_FIGHT )
					do_murder( ch, victim->name );

				/* should steal some coins right away? :) */
				return( TRUE );
			}
		}

		return( FALSE );			/* No one to attack */
	}

	/* okay, we must be fighting.... steal some coins and flee */
	if ( (victim = ch->fighting) == NULL )
		return( FALSE );			/* let's be paranoid.... */

	switch ( number_bits(2) )
	{
	case 0:
		act( "$n strappa la tua borsa delle monete!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "Strappi la borsa delle monete di $N.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n strappa la borsa delle monete di $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		gold = victim->gold / 10;	/* steal 10% of his gold */
		victim->gold -= gold;
		ch->gold += gold;
		return( TRUE );

	case 1:
		do_flee( ch, "" );
		return( TRUE );

	default:
		return( FALSE );
	}
}


bool spec_questmaster( CHAR_DATA *ch )
{
	if ( !IS_AWAKE(ch) )
		return( FALSE );

	if ( number_range(0, 150) == 0 )
	{
		if ( number_range(0, 100) == 0 )
			do_say( ch, "Indovina.. Non vuoi una quest???" );
		else
			do_say( ch, "Non vuoi una quest?" );

		return( TRUE );
	}

	return( FALSE );
}


bool spec_assassinater( CHAR_DATA *ch )
{
	char		buf[MSL];
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	int			rnd_say;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->fighting != NULL )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		/* this should kill mobs as well as players */
		v_next = ch->next_in_room;
		if ( (victim->class != CLASS_THIEF) && (victim->class != CLASS_NINJA) )
			/* thieves & ninjas */
			break;
	}

	if ( victim == NULL || victim == ch || IS_ADMIN(victim) )	return( FALSE );
	if ( victim->level > ch->level + 7 || IS_MOB(victim) )			return( FALSE );
	if ( victim->point[LIFE] < victim->max_point[LIFE] )			return( FALSE );

	rnd_say = number_range( 1, 40 );

	switch ( rnd_say )
	{
	case 5:		sprintf( buf, "La vita e' per te un viaggio giunto al capolinea.." ); break;
	case 6:		sprintf( buf, "E' tempo di morire.." );								  break;
	case 7:		sprintf( buf, "Non sentirai nulla.." );								  break;
	case 8:		sprintf( buf, "Benvenuto al tuo destino.." );						  break;
	case 9:		sprintf( buf, "Un indegno sacrificio agli Dei.. la tua vita. " );	  break;
	case 10:	sprintf( buf, "Un altro ballo con il diavolo.." );					  break;
	default:																		  return( FALSE );
	}

	do_say( ch, buf );
	multi_hit( ch, victim, gsn_assassinate );
	return( TRUE );
}


bool spec_repairman( CHAR_DATA *ch )
{
	if ( !IS_AWAKE(ch) )
		return( FALSE );
	if ( number_range(0, 100) == 0 )
	{
		do_say( ch, "Hai qualche oggetto di metallo che ti posso riparare?" );
		return( TRUE );
	}

	return( FALSE );
}


bool spec_repair_cloth( CHAR_DATA *ch )
{
	if ( !IS_AWAKE(ch) )
		return( FALSE );
	if ( number_range(0, 100) == 0 )
	{
		do_say( ch, "Hai qualche oggetto di cuoio o di stoffa che io ti possa riparare?" );
		return( TRUE );
	}

	return( FALSE );
}


bool spec_captain( CHAR_DATA *ch )
{
	static const char	open_path[]  = "Wn0onc0oe1f2212211s2tw3xw3xd3322a22b22yO00d00a0011e1fe1fn0o3300300w3xs2ts2tS.";
	static const char	close_path[] = "Wn0on0oe1f2212211s2twc3xw3x3322d22a22EC0a00d0b0011e1fe1fn0o3300300w3xs2ts2tS.";
	static const char	*path;
	static int			pos;
	static bool			move;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !move )
	{
		if ( time_info.hour == 6 )
		{
			path = open_path;
			move = TRUE;
			pos = 0;
		}

		if ( time_info.hour == 20 )
		{
			path = close_path;
			move = TRUE;
			pos = 0;
		}
	}

	if ( ch->fighting != NULL )
		return( spec_cast_cleric(ch) );

	if ( !move || ch->position < POS_SLEEP )
		return( FALSE );

	switch ( path[pos] )
	{
	case '0':
	case '1':
	case '2':
	case '3':
		move_char( ch, path[pos] - '0', FALSE );
		break;

	case 'W':
		ch->position = POS_STAND;
		act( "$C$n si sveglia improvvisamente e sbadiglia..$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, WHITE );
		break;

	case 'S':
		ch->position = POS_SLEEP;
		act( "$C$n si distende e comincia a dormire.$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, WHITE );
		break;

	case 'a':
		act( "$C$n says 'Buongiorno!E buona caccia a tutti!'$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, YELLOW );
		break;

	case 'b':
		act( "$C$n says 'Mi raccomando, non sporcate le strade.'$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, YELLOW );
		break;

	case 'c':
		act( "$C$n mugugna 'Devo fare qualcosa per queste maledette porte.$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, YELLOW );
		act( "$C$n says, 'Uff non ne usciro' mai.'$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, YELLOW );
		break;

	case 'd':
		act( "$C$n says 'Buongiorno a tutti, cittadini!'$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, YELLOW );
		break;

	case 'y':
		act( "$C$n says 'Dichiaro la citta' di Solace aperta!'$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, YELLOW );
		break;

	case 'E':
		act( "$C$n says 'Dichiaro chiusa la citta' di Solace!'$c",
			ch, NULL, NULL, TO_ROOM, POS_REST, YELLOW );
		break;

	case 'O':
		do_unlock( ch, "gate" );
		do_open( ch, "gate" );
		break;

	case 'C':
		do_close( ch, "gate" );
		do_lock( ch, "gate" );
		break;

	case 'n':		do_open( ch, "north" );		break;
	case 'o':		do_close( ch, "south" );	break;
	case 's':		do_open( ch, "south" );		break;
	case 't':		do_close( ch, "north" );	break;
	case 'e':		do_open( ch, "east" );		break;
	case 'f':		do_close( ch, "west" );		break;
	case 'w':		do_open( ch, "west" );		break;
	case 'x':		do_close( ch, "east" );		break;

	case '.':
		move = FALSE;
		break;
	}

	pos++;
	return( FALSE );
}


bool spec_headlamia( CHAR_DATA *ch )
{
	static const char	path[] = "T111111100003332222232211.";
	static int			pos = 0;
	static bool			move;
	static int			count = 0;
	CHAR_DATA			*vch, *vch_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !move )
	{
		if ( count++ == 10000 )
			move = 1;
	}

	if ( ch->position < POS_SLEEP || ch->fighting )
		return( FALSE );

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
		if ( !IS_MOB(vch) && vch->pIndexData->vnum == 3143 )
		{
			do_kill( ch, vch->name );
			break;
		}
	}

	if ( !move )
		return( FALSE );

	switch ( path[pos] )
	{
	case '0':
	case '1':
	case '2':
	case '3':
		move_char( ch, path[pos] - '0', FALSE );
		pos++;
		break;

	case 'T':
		pos++;
		for ( vch = char_list; vch; vch = vch_next )
		{
			vch_next = vch->next;
			if ( !IS_MOB(vch) )
				continue;
			if ( vch->pIndexData->vnum == 5201 )
			{
				if ( !vch->fighting && !vch->last_fought )
				{
					char_from_room( vch );
					char_to_room( vch, ch->in_room );
					vch->master = ch;
					vch->leader = ch;
				}
			}
		}
		break;

	case '.':
		move = FALSE;
		count = 0;
		pos = 0;
		break;
	}

	return( FALSE );
}


bool spec_cast_beholder( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( dice(1, 16) )
	{
	case 0:		spell = "fear";				break;
	case 1:		spell = "fear";				break;
	case 2:		spell = "slow";				break;
	case 3:		spell = "cause serious";	break;
	case 4:		spell = "cause critical";	break;
	case 5:		spell = "harm";				break;
	case 6:		spell = "harm";				break;
	case 7:		spell = "dispel magic";		break;
	case 8:		spell = "dispel magic";		break;
	default:	spell = "";					break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_fight_enforcer( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( dice(1, 16) )
	{
	case 0:
	case 1:		spell = "dispel magic";		break;
	case 2:
	case 3:		spell = "acid arrow";		break;
	case 4:
	case 5:		spell = "caustic font";		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:	spell = "acid blast";		break;

	default:
		spell = "";
		break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_fight_invader( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( dice(1, 16) )
	{
	case 0:
	case 1:		spell = "blindness";		break;
	case 2:
	case 3:		spell = "dispel magic";		break;
	case 4:
	case 5:		spell = "weaken";			break;
	case 6:
	case 7:		spell = "energy drain";		break;
	case 8:
	case 9:		spell = "plague";			break;
	case 10:
	case 11:	spell = "acid arrow";		break;
	case 12:
	case 13:
	case 14:	spell = "acid blast";		break;

	case 15:
		if ( ch->point[LIFE] < (ch->max_point[LIFE] / 3) )
			spell = "shadow cloak";
		else
			spell = "";
		break;

	default:
		spell = "";
		break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_fight_ivan( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( dice(1, 16) )
	{
	case 0:
	case 1:		spell = "dispel magic";		break;
	case 2:
	case 3:		spell = "acid arrow";		break;
	case 4:
	case 5:		spell = "caustic font";		break;
	case 6:
	case 7:
	case 8:		spell = "acid blast";		break;
	case 9:		spell = "disgrace";			break;

	case 10:
		if ( ch->point[LIFE] < (ch->max_point[LIFE] / 3) )
			spell = "garble";
		else
			spell = "";
		break;

	default:
		spell = "";
		break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_fight_seneschal( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( dice(1, 16) )
	{
	case 0:		spell = "blindness";		break;
	case 1:		spell = "dispel magic";		break;
	case 2:		spell = "weaken";			break;
	case 3:		spell = "blindness";		break;
	case 4:		spell = "acid arrow";		break;
	case 5:		spell = "caustic font";		break;
	case 6:		spell = "energy drain";		break;
	case 7:
	case 8:
	case 9:		spell = "acid blast";		break;
	case 10:	spell = "plague";			break;
	case 11:	spell = "acid blast";		break;
	case 12:
	case 13:	spell = "lightning breath";	break;
	case 14:
	case 15:	spell = "mental knife";		break;

	default:	spell = "";					break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_fight_powerman( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	ch->cabal = CABAL_ARMY;

	if ( !is_affected(ch, gsn_spellbane) )
		do_spellbane( ch, "" );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	if ( number_percent() < 33 )
	{
		act( "You colpisci con un micidiale attacco triplo!", ch, NULL, NULL, TO_CHAR, POS_REST );
		act( "$n colpisce con un micidiale attacco triplo!", ch, NULL, NULL, TO_ROOM, POS_REST );
		one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
		one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
		one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
	}

	if ( !is_affected(ch, gsn_resistance) )
		do_resistance( ch, "" );

	if ( ch->point[LIFE] < (ch->max_point[LIFE] / 3) && !IS_AFFECTED(ch, AFF_REGENERATION) )
		do_bandage( ch, "" );

	return( TRUE );
}


bool spec_fight_protector( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	switch ( dice(1, 16) )
	{
	case 0:
	case 1:		spell = "dispel magic";		break;
	case 2:
	case 3:		spell = "acid arrow";		break;
	case 4:
	case 5:		spell = "caustic font";		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:	spell = "acid blast";		break;

	default:
		spell = "";
		break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_fight_lionguard( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	if ( victim == NULL )
		return( FALSE );

	if ( number_percent() < 33 )
	{
		int damage_claw;
		/*~~~~~~~~~~~~*/

		damage_claw = dice( ch->level, 24 ) + ch->damroll;
		damage( ch, victim, damage_claw, gsn_claw, DAM_BASH, TRUE );
		return( TRUE );
	}

	switch ( dice(1, 16) )
	{
	case 0:
	case 1:		spell = "dispel magic";		break;
	case 2:
	case 3:		spell = "acid blast";		break;
	case 4:
	case 5:		spell = "caustic font";		break;
	case 6:
	case 7:
	case 8:		spell = "acid arrow";		break;
	default:	spell = "";					break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool spec_fight_hunter( CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*v_next;
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( ch->position != POS_FIGHT )
		return( FALSE );

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( victim->fighting == ch && number_bits(1) == 0 )
			break;
	}

	switch ( dice(1, 16) )
	{
	case 0:
	case 1:		spell = "dispel magic";		break;
	case 2:
	case 3:		spell = "acid arrow";		break;
	case 4:
	case 5:		spell = "caustic font";		break;
	case 6:
	case 7:
	case 8:
	case 9:		spell = "acid blast";		break;
	default:	spell = "";					break;
	}

	if ( victim == NULL )
		return( FALSE );

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool mob_cast_cleric( CHAR_DATA *ch, CHAR_DATA *victim )
{
	char	*spell;
	int		sn;
	/*~~~~~~~~~~~*/

	for ( ;; )
	{
		int min_level;
		/*~~~~~~~~~~*/

		switch ( number_bits(4) )
		{
		case 0:			min_level = 0;		spell = "blindness";		break;
		case 1:			min_level = 3;		spell = "cause serious";	break;
		case 2:			min_level = 7;		spell = "earthquake";		break;
		case 3:			min_level = 9;		spell = "cause critical";	break;
		case 4:			min_level = 10;		spell = "dispel evil";		break;
		case 5:			min_level = 12;		spell = "curse";			break;
		case 6:			min_level = 14;		spell = "cause critical";	break;
		case 7:			min_level = 18;		spell = "flamestrike";		break;
		case 8:
		case 9:
		case 10:		min_level = 20;		spell = "harm";				break;
		case 11:		min_level = 25;		spell = "plague";			break;
		case 12:
		case 13:		min_level = 45;		spell = "severity force";	break;

		default:		min_level = 26;		spell = "dispel magic";		break;
		}

		if ( ch->level >= min_level )
			break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );
	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}


bool mob_cast_mage( CHAR_DATA *ch, CHAR_DATA *victim )
{
	char	*spell;
	int		sn;
	/*~~~~~~~~~~~*/

	for ( ;; )
	{
		int min_level;
		/*~~~~~~~~~~*/

		switch ( number_bits(4) )
		{
		case  0:		min_level = 0;			spell = "blindness";		break;
		case  1:		min_level = 3;			spell = "chill touch";		break;
		case  2:		min_level = 7;			spell = "weaken";			break;
		case  3:		min_level = 9;			spell = "teleport";			break;
		case  4:		min_level = 14;			spell = "colour spray";		break;
		case  5:		min_level = 19;			spell = "caustic font";		break;
		case  6:		min_level = 25;			spell = "energy drain";		break;
		case  7:
		case  8:
		case  9:		min_level = 35;			spell = "caustic font";		break;
		case 10:		min_level = 40;			spell = "plague";			break;
		case 11:
		case 12:
		case 13:		min_level = 40;			spell = "acid arrow";		break;

		default:		min_level = 55;			spell = "acid blast";		break;
		}

		if ( ch->level >= min_level )
			break;
	}

	if ( (sn = skill_lookup(spell)) < 0 )
		return( FALSE );

	say_spell( ch, sn );
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
	return( TRUE );
}
