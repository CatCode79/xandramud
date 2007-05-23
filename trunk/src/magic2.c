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
#include <time.h>

#include "xandra.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"


extern char		*target_name;


ROOM_INDEX_DATA *check_place( CHAR_DATA *ch, char *argument )
{
	EXIT_DATA		*pExit;
	ROOM_INDEX_DATA *dest_room;
	int				number, door;
	int				range = ( ch->level / 10 ) + 1;
	char			arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	number = number_argument( argument, arg );
	if ( (door = find_exit(ch, arg, FALSE)) == -1 )
		return( NULL );

	dest_room = ch->in_room;
	while ( number > 0 )
	{
		number--;
		if ( --range < 1 ) return( NULL );
		{
			if ( (pExit = dest_room->exit[door]) == NULL
			|| (dest_room = pExit->to_room) == NULL
			|| IS_SET(pExit->exit_info, EX_CLOSED) )
			  	break;
		}
		if ( number < 1 )

			return( dest_room );
	}

	return( NULL );
}


void spell_portal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*portal, *stone;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.. o hai dimenticato qualcosa?\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	!can_see_room(ch, victim->in_room)
	||	IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	||	victim->level >= level + 3
	||	(!IS_MOB(victim) && victim->level >= LEVEL_HERO)	/* NOT trust */
	||	(IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(IS_MOB(victim) && saves_spell(level, victim, DAM_NONE)) )
	{
		send_to_char( "Non riesci a catalizzare l'energia in maniera appropriata.\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Non puoi creare il portale.\n\r", ch );
		return;
	}

	stone = get_hold_char( ch );
	if ( !IS_ADMIN(ch) && (stone == NULL || stone->item_type != ITEM_WARP_STONE) )
	{
		send_to_char( "Ehi.. ma non ti manca qualcosa?\n\r", ch );
		return;
	}

	if ( stone != NULL && stone->item_type == ITEM_WARP_STONE )
	{
		act( "Evochi l'energia secreta in $p creando un portale magico.", ch, stone, NULL, TO_CHAR, POS_REST );
		act( "$p emette un bagliore intenso.. poi crepita e si frantuma.", ch, stone, NULL, TO_CHAR, POS_REST );
		extract_obj( stone );
	}

	portal = create_object( get_obj_index(OBJ_VNUM_PORTAL), 0 );
	portal->timer = 2 + level / 25;
	portal->value[3] = victim->in_room->vnum;

	obj_to_room( portal, ch->in_room );

	act( "$p appare dal nulla.", ch, portal, NULL, TO_ROOM, POS_REST );
	act( "$p appare dal nulla.", ch, portal, NULL, TO_CHAR, POS_REST );
}


void spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA		*victim;
	OBJ_DATA		*portal, *stone;
	ROOM_INDEX_DATA *to_room, *from_room;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	from_room = ch->in_room;

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.. o hai dimenticato qualcosa?\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	(to_room = victim->in_room) == NULL
	||	!can_see_room(ch, to_room)
	||	!can_see_room(ch, from_room)
	||	IS_SET(to_room->room_flags, ROOM_SAFE)
	||	IS_SET(from_room->room_flags, ROOM_SAFE)
	||	IS_SET(to_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(to_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(to_room->room_flags, ROOM_NOSUMMON)
	||	victim->level >= level + 3
	||	(!IS_MOB(victim) && victim->level >= LEVEL_HERO)	/* NOT trust */
	||	(IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(IS_MOB(victim) && saves_spell(level, victim, DAM_NONE)) )
	{
		send_to_char( "Non riesci a catalizzare l'energia in maniera appropriata.\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Non riesci a creare il nesso dimensionale.\n\r", ch );
		return;
	}

	stone = get_hold_char( ch );
	if ( !IS_ADMIN(ch) && (stone == NULL || stone->item_type != ITEM_WARP_STONE) )
	{
		send_to_char( "Ehi, non ti manca qualcosa?\n\r", ch );
		return;
	}

	if ( stone != NULL && stone->item_type == ITEM_WARP_STONE )
	{
		act( "Richiami le energie da $p creando un portale magico.", ch, stone, NULL, TO_CHAR, POS_REST );
		act( "$p emette un bagliore intenso.. poi crepita e si frantuma.", ch, stone, NULL, TO_CHAR, POS_REST );
		extract_obj( stone );
	}

	/* portal one */
	portal = create_object( get_obj_index(OBJ_VNUM_PORTAL), 0 );
	portal->timer = 1 + level / 10;
	portal->value[3] = to_room->vnum;

	obj_to_room( portal, from_room );

	act( "$p appare dal nulla.", ch, portal, NULL, TO_ROOM, POS_REST );
	act( "$p appare dal nulla.", ch, portal, NULL, TO_CHAR, POS_REST );

	/* no second portal if rooms are the same */
	if ( to_room == from_room )
		return;

	/* portal two */
	portal = create_object( get_obj_index(OBJ_VNUM_PORTAL), 0 );
	portal->timer = 1 + level / 10;
	portal->value[3] = from_room->vnum;

	obj_to_room( portal, to_room );

	if ( to_room->people != NULL )
	{
		act( "$p appare dal nulla.", to_room->people, portal, NULL, TO_ROOM, POS_REST );
		act( "$p appare dal nulla.", to_room->people, portal, NULL, TO_CHAR, POS_REST );
	}
}


void spell_disintegrate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	CHAR_DATA	*tmp_ch;
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	int			i, dam = 0;
	OBJ_DATA	*tattoo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( saves_spell(level, victim, DAM_MENTAL) || number_bits(1) == 0 )
	{
		dam = dice( level, 24 );
		damage( ch, victim, dam, sn, DAM_MENTAL, TRUE );
		return;
	}

	act( "Richiami un potentissimo raggio disgregante contro $N!",
		ch, NULL, victim, TO_CHAR, POS_REST, RED );
	act( "L'ondata atomica disgregante di $n ti  $C###DISINTEGRA###!!!$c",
		ch, NULL, victim, TO_VICT, POS_REST, RED );
	act( "L'ondata atomica disgregante di $n $C###DISINTEGRA###$c $N!!!",
		ch, NULL, victim, TO_NOTVICT, POS_REST, RED );
	send_to_char( "Muori tra mille dolori lancinanti!\n\r", victim );

	act( "Il corpo di $N e' stato cancellato dall'esistenza!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "Il corpo di $N e' stato cancellato dall'esistenza!", ch, NULL, victim, TO_ROOM, POS_REST );
	act( "$N e' mort$X!", ch, NULL, victim, TO_CHAR, POS_REST );

	send_to_char( "Ora sei puro spirito e nessuno potra' nuocerti per un po'.\n\r", victim );
	send_to_char( "A meno che non sia tu a farlo ad altri..\n\r", victim );

	/* disintegrate the objects... */
	tattoo = get_eq_char( victim, WEAR_TATTOO );	/* keep tattoos for later */
	if ( tattoo != NULL )
		obj_from_char( tattoo );

	victim->gold = 0;
	victim->silver = 0;


	for ( obj = victim->carrying; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next_content;
		extract_obj( obj );
	}

	if ( IS_MOB(victim) )
	{
		victim->pIndexData->killed++;
		kill_table[URANGE( 0, victim->level, MAX_LEVEL - 1 )].killed++;
		extract_char( victim, TRUE );
		return;
	}

	extract_char( victim, FALSE );

	while ( victim->affected )
		affect_remove( victim, victim->affected );
	victim->affected_by = 0;
	for ( i = 0; i < 4; i++ )
		victim->armor[i] = 100;
	victim->position = POS_REST;
	victim->point[LIFE] = 1;
	victim->point[MANA] = 1;

	REMOVE_BIT( victim->act, PLR_WANTED );
	REMOVE_BIT( victim->act, PLR_BOUGHT_PET );

	victim->pcdata->condition[COND_THIRST] = 40;
	victim->pcdata->condition[COND_HUNGER] = 40;
	victim->pcdata->condition[COND_FULL] = 40;
	victim->pcdata->condition[COND_BLOODLUST] = 40;
	victim->pcdata->condition[COND_DESIRE] = 40;

	victim->last_death_time = current_time;

	if ( tattoo != NULL )
	{
		obj_to_char( tattoo, victim );
		equip_char( victim, tattoo, WEAR_TATTOO );
	}

	for ( tmp_ch = char_list; tmp_ch != NULL; tmp_ch = tmp_ch->next )
		if ( tmp_ch->last_fought == victim ) tmp_ch->last_fought = NULL;
}


void spell_poison_smoke( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*tmp_vict;
	/*~~~~~~~~~~~~~~~~~~*/

	send_to_char( "Una nube di fumi velenosi riempie l'ambiente.\n\r", ch );
	act( "Una nube di fumi velenosi riempie l'ambiente.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( tmp_vict = ch->in_room->people; tmp_vict != NULL; tmp_vict = tmp_vict->next_in_room )
	{
		if ( !is_safe_spell(ch, tmp_vict, TRUE) )
		{
			if ( !is_safe_spell(ch, tmp_vict, TRUE) )
			{
				if ( tmp_vict != ch
				&&	ch->fighting != tmp_vict
				&&	tmp_vict->fighting != ch
				&&	(IS_SET(tmp_vict->affected_by, AFF_CHARM) || IS_MOB(tmp_vict)) )	/* (TT) questa tipologia di controllo, dato che ora sono solo i mob a urlare */
				{
						do_yell( tmp_vict, "Aiuto! Mi stanno attaccando!" );
				}

				spell_poison( gsn_poison, ch->level, ch, tmp_vict, TARGET_CHAR );

				/* poison_effect(ch->in_room,level,level,TARGET_CHAR); */
				if ( tmp_vict != ch )
					multi_hit( tmp_vict, ch, TYPE_UNDEFINED );
			}
		}
	}
}


void spell_blindness_dust( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*tmp_vict;
	/*~~~~~~~~~~~~~~~~~~*/

	send_to_char( "Una nube di polvere avvolge l'ambiente.\n\r", ch );
	act( "Una nube di polvere avvolge l'ambiente.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( tmp_vict = ch->in_room->people; tmp_vict != NULL; tmp_vict = tmp_vict->next_in_room )
	{
		if ( !is_safe_spell(ch, tmp_vict, TRUE) )
		{
			if ( tmp_vict != ch
			&&	 ch->fighting != tmp_vict
			&&	 tmp_vict->fighting != ch
			&&	(IS_SET(tmp_vict->affected_by, AFF_CHARM) || IS_MOB(tmp_vict)) )
			{
				do_yell( tmp_vict, "Aiuto! Mi stanno attaccando!" );
			}

			spell_blindness( gsn_blindness, ch->level, ch, tmp_vict, TARGET_CHAR );
			if ( tmp_vict != ch )
				multi_hit( tmp_vict, ch, TYPE_UNDEFINED );
		}
	}
}


void spell_bark_skin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		if ( victim == ch )
			send_to_char( "La tua pelle ha gia' la consistenza della corteccia.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level;
	af.location = APPLY_AC;
	af.modifier = -( level * 1.5 );
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "La pelle di $n assume la consistenza di una spessa corteccia.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "La tua pelle assume la consistenza di una spessa corteccia.\n\r", victim );
}


void spell_bear_call( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*bear;
	CHAR_DATA	*bear2;
	AFFECT_DATA af;
	int			i;
	/*~~~~~~~~~~~~~~~*/

	send_to_char( "Richiami degli orsi in tuo aiuto!\n\r", ch );
	act( "$n richiama degli orsi in suo aiuto.", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai richiamato troppo di recente il loro aiuto, aspetta un po'.\n\r", ch );
		return;
	}

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch) && IS_AFFECTED(gch, AFF_CHARM) && gch->master == ch && gch->pIndexData->vnum == MOB_VNUM_BEAR )
		{
			send_to_char( "Beh? Non ti piace quello che hai?", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	bear = create_mobile( get_mob_index(MOB_VNUM_BEAR) );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		bear->perm_stat[i] = UMIN( 25, 2 * ch->perm_stat[i] );
	}

	bear->max_point[LIFE] = IS_MOB( ch ) ? ch->max_point[LIFE] : ch->pcdata->perm_point[LIFE];
	bear->point[LIFE] = bear->max_point[LIFE];
	bear->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	bear->point[MANA] = bear->max_point[MANA];
	bear->alignment = ch->alignment;
	bear->level = UMIN( 70, 1 * ch->level );
	for ( i = 0; i < 3; i++ )
		bear->armor[i] = interpolate( bear->level, 100, -100 );
	bear->armor[3] = interpolate( bear->level, 100, 0 );
	bear->sex = ch->sex;
	bear->gold = 0;

	bear2 = create_mobile( bear->pIndexData );
	clone_mobile( bear, bear2 );

	SET_BIT( bear->affected_by, AFF_CHARM );
	SET_BIT( bear2->affected_by, AFF_CHARM );
	bear->master = bear2->master = ch;
	bear->leader = bear2->leader = ch;

	char_to_room( bear, ch->in_room );
	char_to_room( bear2, ch->in_room );
	send_to_char( "Due orsi vengono in tuo aiuto!\n\r", ch );
	act( "Due orsi vengono ad affiancare $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );
}


void spell_ranger_staff( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*staff;
	AFFECT_DATA tohit;
	AFFECT_DATA todam;
	/*~~~~~~~~~~~~~~~*/

	staff = create_object( get_obj_index(OBJ_VNUM_RANGER_STAFF), level );
	send_to_char( "Crei da un grezzo tronco un bastone da ranger!\n\r", ch );
	act( "$n crea da un grezzo tronco un bastone da ranger.", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( ch->level < 50 )
		staff->value[2] = ( ch->level / 10 );
	else
		staff->value[2] = ( ch->level / 6 ) - 3;
	staff->level = ch->level;

	tohit.where = TO_OBJECT;
	tohit.type = sn;
	tohit.level = ch->level;
	tohit.duration = -1;
	tohit.location = APPLY_HITROLL;
	tohit.modifier = 2 + level / 5;
	tohit.bitvector = 0;
	affect_to_obj( staff, &tohit );

	todam.where = TO_OBJECT;
	todam.type = sn;
	todam.level = ch->level;
	todam.duration = -1;
	todam.location = APPLY_DAMROLL;
	todam.modifier = 2 + level / 5;
	todam.bitvector = 0;
	affect_to_obj( staff, &todam );

	staff->timer = level;

	obj_to_char( staff, ch );
}


void spell_vanish( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	ROOM_INDEX_DATA *pRoomIndex;
	CHAR_DATA		*victim = (CHAR_DATA *) vo;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( victim->in_room == NULL || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) )
	{
		send_to_char( "Umh no, da qui non puoi proprio farlo.\n\r", ch );
		return;
	}

	for ( i = 0; i < 65535; i++ )
	{
		pRoomIndex = get_room_index( number_range(0, 65535) );
		if ( pRoomIndex != NULL )
		{
			if ( can_see_room(victim, pRoomIndex)
			&&	!room_is_private(pRoomIndex)
			&&	victim->in_room->area == pRoomIndex->area )
				break;
		}
	}

	if ( pRoomIndex == NULL )
	{
		send_to_char( "Umh no, da qui non puoi proprio farlo.\n\r", ch );
		return;
	}

	act( "Una nuvola di fumo avvolge $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( !IS_MOB(ch) && ch->fighting != NULL && number_bits(1) == 1 )
	{
		send_to_char( "Non puoi farlo mentre stai combattendo, non credi?\n\r", ch );
		return;
	}

	act( "$n e' sparit$x!", victim, NULL, NULL, TO_ROOM, POS_REST );

	char_from_room( victim );
	char_to_room( victim, pRoomIndex );
	act( "$n appare dal nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );
	do_look( victim, "auto" );
	stop_fighting( victim, TRUE );
}


void spell_terangreal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) ) return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 10;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_SLEEP;
	affect_join( victim, &af );

	if ( IS_AWAKE(victim) )
	{
		send_to_char( "Improvvisamente senti che le forze ti abbandonano..\n\r", victim );
		act( "$n cade in un sonno profondo..", victim, NULL, NULL, TO_ROOM, POS_REST );
		victim->position = POS_SLEEP;
	}
}


void spell_kassandra( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Aspetta un po' prima di rifarlo.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 5;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );
	ch->point[LIFE] = UMIN( ch->point[LIFE] + 150, ch->max_point[LIFE] );
	update_pos( ch );
	send_to_char( "Una piacevole sensazione riscalda il tuo corpo.\n\r", ch );
	act( "$n sembra stare un po' meglio.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_sebat( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Aspetta un po' prima di rifarlo.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level;
	af.location = APPLY_AC;
	af.modifier = -30;
	af.bitvector = 0;
	affect_to_char( ch, &af );
	act( "$n viene circondat$x da un velo di energia.", ch, NULL, NULL, TO_ROOM, POS_REST );
	act( "Vieni circondat$x da un velo di energia.", ch, NULL, NULL, TO_CHAR, POS_REST );
}


void spell_matandra( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Aspetta un po' prima di rifarlo.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 5;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );
	dam = dice( level, 7 );

	damage( ch, victim, dam, sn, DAM_HOLY, TRUE );
}


void spell_amnesia( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int			i;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) )
		return;
	for ( i = 0; i < MAX_SKILL; i++ )
		victim->pcdata->learned[i] /= 2;

	act( "Un bagliore improvviso si accende nella tua mente.. cancellando ogni cosa..", victim, NULL, NULL, TO_CHAR, POS_REST );
	act( "$n impallidisce improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_tattoo( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*tattoo;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) )
	{
		act( "Proprio da $N vuoi farti adorare?!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	for ( i = 0; i < MAX_RELIGION; i++ )
	{
		if ( !str_cmp(ch->name, religion_table[i].leader) )
		{
			tattoo = get_eq_char( victim, WEAR_TATTOO );
			if ( tattoo != NULL )
			{
				act ( "$N ha gia' un tatuaggio, togli quello prima.", ch, NULL, victim, TO_CHAR, POS_REST );
				return;
			}
			else
			{
				tattoo = create_object( get_obj_index(religion_table[i].vnum), 60 );
				act( "Conferisci $p a  $N!", ch, tattoo, victim, TO_CHAR, POS_REST );
				act( "$n conferisce a $N $p!", ch, tattoo, victim, TO_NOTVICT, POS_REST );
				act( "$n ti conferisce $p!", ch, tattoo, victim, TO_VICT, POS_REST );

				obj_to_char( tattoo, victim );
				equip_char( victim, tattoo, WEAR_TATTOO );
				return;
			}
		}
	}

	send_to_char( "Non hai nessun tatuaggio religioso.\n\r", ch );
}


void spell_remove_tattoo( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*tattoo;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	tattoo = get_eq_char( victim, WEAR_TATTOO );
	if ( tattoo != NULL )
	{
		extract_obj( tattoo );
		act( "Il tatuaggio sulla tua pelle svanisce improvvisamente.", ch, NULL, victim, TO_VICT, POS_REST );
		act( "Il tatuaggio d $N svanisce..", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "Il taguaggio di $N svanisce..", ch, NULL, victim, TO_NOTVICT, POS_REST );
	}
	else
		act( "$N non ha alcun tatuaggio.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_wrath( int sn, int level, CHAR_DATA *ch, void *vo, int target )

{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(ch) && IS_EVIL(ch) )
		victim = ch;

	if ( IS_GOOD(victim) )
	{
		act( "$N non puo' essere danneggiato dall'ira dei suoi stessi Dei.", ch, NULL, victim, TO_ROOM, POS_REST );
		return;
	}

	if ( IS_NEUTRAL(victim) )
	{
		act( "$N e' immune a questo tipo di attacco.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	dam = dice( level, 12 );

	if ( saves_spell(level, victim, DAM_HOLY) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_HOLY, TRUE );

	if ( IS_AFFECTED(victim, AFF_CURSE) || saves_spell(level, victim, DAM_HOLY) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 2 * level;
	af.location = APPLY_HITROLL;
	af.modifier = -1 * ( level / 8 );
	af.bitvector = AFF_CURSE;
	affect_to_char( victim, &af );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = level / 8;
	affect_to_char( victim, &af );

	send_to_char( "Un immondo senso di vergogna ti assale.\n\r", victim );
	if ( ch != victim )
		act( "$N sembra un po' smarrit$X..", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_stalker( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*stalker;
	AFFECT_DATA af;
	int			i;
	char		buf_log[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	IS_MOB(victim)
	||	!IS_SET(victim->act, PLR_WANTED) )
	{
		send_to_char( "Non riesci a richiamare l'aiuto della sentinella.\n\r", ch );
		return;
	}

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai gia' richiamato la sentinella troppo di recente.\n\r", ch );
		return;
	}

	if ( !is_safe_nomessage(ch, victim) && !IS_SET(ch->act, PLR_CANINDUCT) )
	{
		send_to_char( "Faresti bene ad usare le guardie per questo tipo di soggetto.\n\r", ch );
		return;
	}

	send_to_char( "Richiami a gran voce una sentinella.\n\r", ch );
	act( "$n richiama a gran voce una sentinella.", ch, NULL, NULL, TO_ROOM, POS_REST );

	stalker = create_mobile( get_mob_index(MOB_VNUM_STALKER) );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 6;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		stalker->perm_stat[i] = victim->perm_stat[i];
	}

	stalker->max_point[LIFE] = UMIN( 30000, 2 * victim->max_point[LIFE] );
	stalker->point[LIFE] = stalker->max_point[LIFE];
	stalker->max_point[MANA] = victim->max_point[MANA];
	stalker->point[MANA] = stalker->max_point[MANA];
	stalker->level = victim->level;

	stalker->damage[DICE_NUMBER] = number_range( victim->level / 8, victim->level / 6 );
	stalker->damage[DICE_TYPE] = number_range( victim->level / 6, victim->level / 5 );
	stalker->damage[DICE_BONUS] = number_range( victim->level / 10, victim->level / 8 );

	stalker->hitroll = victim->level;
	stalker->damroll = victim->level;
	stalker->size = victim->size;
	for ( i = 0; i < 3; i++ ) stalker->armor[i] = interpolate( stalker->level, 100, -100 );
	stalker->armor[3] = interpolate( stalker->level, 100, 0 );
	stalker->gold = 0;
	stalker->invis_level = LEVEL_ADMIN;
	stalker->detection = ( A | B | C | D | E | F | G | H | ee );
	stalker->affected_by = ( H | J | N | O | U | V | aa | cc );

	char_to_room( stalker, victim->in_room );
	stalker->last_fought = victim;

	/* Xandra: (FF) more efficient doppelganger in fight.c */
	send_to_char( "Un cacciatore invisibile e' qui per te!\n\r", victim );
	act( "Un cacciatore invisibile e' qui per $n!", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Il cacciatore invisibile e' sulle tracce dell'obiettivo.\n\r", ch );

	sprintf( buf_log, "%s ha richiamato lo stalker contro %s", ch->name, victim->name );
	log_string( buf_log );
}


void spell_dragon_strength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Sei gia' sotto l'effetto di questo incantesimo.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 3;
	af.bitvector = 0;

	af.modifier = 2;
	af.location = APPLY_HITROLL;
	affect_to_char( ch, &af );

	af.modifier = 2;
	af.location = APPLY_DAMROLL;
	affect_to_char( ch, &af );

	af.modifier = 10;
	af.location = APPLY_AC;
	affect_to_char( ch, &af );

	af.modifier = 2;
	af.location = APPLY_STR;
	affect_to_char( ch, &af );

	af.modifier = -2;
	af.location = APPLY_DEX;
	affect_to_char( ch, &af );

	send_to_char( "La forza del Drago pervade il tuo corpo.\n\r", ch );
	act( "$n sembra piu' forte e imponente ora.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_dragon_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 6 );
	if ( !is_safe_spell(ch, victim, TRUE) )
	{
		if ( saves_spell(level, victim, DAM_FIRE) )
			dam /= 2;
		damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
	}
}


void spell_entangle( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->in_room->sector_type == SECT_INSIDE
	||	 ch->in_room->sector_type == SECT_CITY
	||	 ch->in_room->sector_type == SECT_DESERT
	||	 ch->in_room->sector_type == SECT_AIR )
	{
		send_to_char( "Non c'e' abbastanza vegetazione qui.\n\r", ch );
		return;
	}

	dam = number_range( level, 4 * level );
	if ( saves_spell(level, victim, DAM_PIERCE) )
		dam /= 2;

	damage( ch, victim, ch->level, gsn_entangle, DAM_PIERCE, TRUE );

	act( "La vegetazione cresce rapidamente sotto $n, intrappolando le sue gambe!", victim, NULL, NULL, TO_ROOM, POS_REST );
	act( "La vegetazione cresce rapidamente sotto di te intrappolandoti le gambe!", victim, NULL, NULL, TO_CHAR, POS_REST );

	victim->point[MOVE] -= dice( level, 8 );
	victim->point[MOVE] = UMAX( 0, victim->point[MOVE] );

	if ( !is_affected(victim, gsn_entangle) )
	{
		AFFECT_DATA todex;
		/*~~~~~~~~~~~~~~*/

		todex.type = gsn_entangle;
		todex.level = level;
		todex.duration = level / 10;
		todex.location = APPLY_DEX;
		todex.modifier = -( level / 10 );
		todex.bitvector = 0;
		affect_to_char( victim, &todex );
	}
}


void spell_love_potion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 50;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	send_to_char( "Pero'.. niente male questa bevanda..\n\r", ch );
}


void spell_protective_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questa protezione.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = number_fuzzy( level / 30 ) + 3;
	af.location = APPLY_AC;
	af.modifier = 20;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "$n viene circondat$x da uno scudo protettivo.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Uno scudo magico di protezione ti avvolge.\n\r", victim );
}


void spell_deafen( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch == victim )
	{
		send_to_char( "Specifica il bersaglio dell'incantesimo.\n\r", ch );
		return;
	}

	if ( is_affected(victim, sn) )
	{
		act( "$N e' gia' sord$X.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}


	if ( is_safe_nomessage(ch, victim) )
	{
		send_to_char( "Non puoi farlo qui ed ora.\n\r", ch );
		return;
	}

	if ( saves_spell(level, victim, DAM_NONE) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 10;
	af.modifier = 0;
	af.location = 0;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	act( "Hai assordato $N!", ch, NULL, victim, TO_CHAR, POS_REST );
	send_to_char( "Una violenta implosione ti dilania i timpani! Non senti piu' nulla!\n\r", victim );
}


void spell_honor_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			send_to_char( "Sono gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.modifier = -30;
	af.location = APPLY_AC;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	spell_remove_curse( skill_lookup("remove curse"), level, ch, victim, TARGET_CHAR );
	spell_bless( skill_lookup("bless"), level, ch, victim, TARGET_CHAR );

	send_to_char( "Senti una morsa d'orgoglio afferrarti il cuore.\n\r", victim );
	act( "$n e' ora protett$x dalla forza dell'Onore.", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_acute_vision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, ACUTE_VISION) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo. \n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ACUTE_VISION;
	affect_to_char( victim, &af );
	send_to_char( "I tuoi occhi cambiano colore..\n\r", victim );
	if ( ch != victim )
		send_to_char( "Incantesimo riuscito.\n\r", ch );
}


void spell_dragons_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	CHAR_DATA	*vch, *vch_next;
	int			dam, hp_dam, dice_dam;
	int			hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "Richiami il Signore dei Draghi in tuo aiuto!", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "$n richiama l'aiuto del Signore dei Draghi!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n viene avvolt$x da un circolo di fiamme!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Evochi il potere del soffio del Drago!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 10, ch->point[LIFE] );
	hp_dam = number_range( hpch / 9 + 1, hpch / 5 );
	dice_dam = dice( level, 20 );

	dam = UMAX( hp_dam + dice_dam / 5, dice_dam + hp_dam / 5 );

	switch ( dice(1, 5) )
	{
	case 1:
		fire_effect( victim->in_room, level, dam / 2, TARGET_ROOM );

		for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next_in_room;

			if ( is_safe_spell(ch, vch, TRUE)
			||	(IS_MOB(vch) && IS_MOB(ch) && (ch->fighting != vch || vch->fighting != ch)) )
				continue;
			if ( is_safe(ch, vch) )
				continue;

			if ( vch == victim )	/* full damage */
			{
				if ( saves_spell(level, vch, DAM_FIRE) )
				{
					fire_effect( vch, level / 2, dam / 4, TARGET_CHAR );
					damage( ch, vch, dam / 2, sn, DAM_FIRE, TRUE );
				}
				else
				{
					fire_effect( vch, level, dam, TARGET_CHAR );
					damage( ch, vch, dam, sn, DAM_FIRE, TRUE );
				}
			}
			else					/* partial damage */
			{
				if ( saves_spell(level - 2, vch, DAM_FIRE) )
				{
					fire_effect( vch, level / 4, dam / 8, TARGET_CHAR );
					damage( ch, vch, dam / 4, sn, DAM_FIRE, TRUE );
				}
				else
				{
					fire_effect( vch, level / 2, dam / 4, TARGET_CHAR );
					damage( ch, vch, dam / 2, sn, DAM_FIRE, TRUE );
				}
			}
		}
		break;

	case 2:
		if ( saves_spell(level, victim, DAM_ACID) )
		{
			acid_effect( victim, level / 2, dam / 4, TARGET_CHAR );
			damage( ch, victim, dam / 2, sn, DAM_ACID, TRUE );
		}
		else
		{
			acid_effect( victim, level, dam, TARGET_CHAR );
			damage( ch, victim, dam, sn, DAM_ACID, TRUE );
		}
		break;

	case 3:
		cold_effect( victim->in_room, level, dam / 2, TARGET_ROOM );

		for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next_in_room;

			if ( is_safe_spell(ch, vch, TRUE)
			||	(IS_MOB(vch) && IS_MOB(ch) && (ch->fighting != vch || vch->fighting != ch)) )
				continue;
			if ( is_safe(ch, vch) )
				continue;

			if ( vch == victim )	/* full damage */
			{
				if ( saves_spell(level, vch, DAM_COLD) )
				{
					cold_effect( vch, level / 2, dam / 4, TARGET_CHAR );
					damage( ch, vch, dam / 2, sn, DAM_COLD, TRUE );
				}
				else
				{
					cold_effect( vch, level, dam, TARGET_CHAR );
					damage( ch, vch, dam, sn, DAM_COLD, TRUE );
				}
			}
			else
			{
				if ( saves_spell(level - 2, vch, DAM_COLD) )
				{
					cold_effect( vch, level / 4, dam / 8, TARGET_CHAR );
					damage( ch, vch, dam / 4, sn, DAM_COLD, TRUE );
				}
				else
				{
					cold_effect( vch, level / 2, dam / 4, TARGET_CHAR );
					damage( ch, vch, dam / 2, sn, DAM_COLD, TRUE );
				}
			}
		}
		break;

	case 4:
		poison_effect( ch->in_room, level, dam, TARGET_ROOM );

		for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next_in_room;

			if ( is_safe_spell(ch, vch, TRUE)
			||	(IS_MOB(ch) && IS_MOB(vch) && (ch->fighting == vch || vch->fighting == ch)) )
				continue;
			if ( is_safe(ch, vch) )
				continue;

			if ( vch != ch
			&&	ch->fighting != vch
			&&	vch->fighting != ch
			&&	(IS_SET(vch->affected_by, AFF_CHARM) || IS_MOB(vch)) )
			{
				do_yell( vch, "Aiuto! Mi stanno attaccando!" );
			}

			if ( saves_spell(level, vch, DAM_POISON) )
			{
				poison_effect( vch, level / 2, dam / 4, TARGET_CHAR );
				damage( ch, vch, dam / 2, sn, DAM_POISON, TRUE );
			}
			else
			{
				poison_effect( vch, level, dam, TARGET_CHAR );
				damage( ch, vch, dam, sn, DAM_POISON, TRUE );
			}
		}
		break;

	case 5:
		if ( saves_spell(level, victim, DAM_LIGHTNING) )
		{
			shock_effect( victim, level / 2, dam / 4, TARGET_CHAR );
			damage( ch, victim, dam / 2, sn, DAM_LIGHTNING, TRUE );
		}
		else
		{
			shock_effect( victim, level, dam, TARGET_CHAR );
			damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
		}
		break;
	}
}


void spell_sand_storm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch, *vch_next;
	int			dam, hp_dam, dice_dam;
	int			hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (ch->in_room->sector_type == SECT_AIR)
	||	 (ch->in_room->sector_type == SECT_WATER_SWIM)
	||	 (ch->in_room->sector_type == SECT_WATER_NOSWIM) )
	{
		send_to_char( "Umh.. non e' che veda tanta sabbia io..\n\r", ch );
		ch->wait = 0;
		return;
	}

	act( "$n crea una tormenta di sabbia!", ch, NULL, NULL, TO_ROOM, POS_REST );
	act( "Crei una tormenta di sabbia!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 10, ch->point[LIFE] );
	hp_dam = number_range( hpch / 9 + 1, hpch / 5 );
	dice_dam = dice( level, 20 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );
	sand_effect( ch->in_room, level, dam / 2, TARGET_ROOM );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( is_safe_spell(ch, vch, TRUE)
		||	(IS_MOB(vch) && IS_MOB(ch) && (ch->fighting != vch /* || vch->fighting != ch */ )) )
			continue;

		if ( is_safe(ch, vch) )
			continue;

		if ( saves_spell(level, vch, DAM_COLD) )
		{
			sand_effect( vch, level / 2, dam / 4, TARGET_CHAR );
			damage( ch, vch, dam / 2, sn, DAM_COLD, TRUE );
		}
		else
		{
			sand_effect( vch, level, dam, TARGET_CHAR );
			damage( ch, vch, dam, sn, DAM_COLD, TRUE );
		}
	}
}


void spell_scream( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch, *vch_next;
	int			dam = 0, hp_dam, dice_dam;
	int			hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n emette un urlo violentissimo!.", ch, NULL, NULL, TO_ROOM, POS_REST );
	act( "Emetti un urlo violentissimo!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 10, ch->point[LIFE] );
	hp_dam = number_range( hpch / 9 + 1, hpch / 5 );
	dice_dam = dice( level, 20 );
	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );

	scream_effect( ch->in_room, level, dam / 2, TARGET_ROOM );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( is_safe_spell(ch, vch, TRUE) )
			continue;
		if ( is_safe(ch, vch) )
			continue;

		if ( saves_spell(level, vch, DAM_ENERGY) )
		{
			WAIT_STATE( vch, PULSE_VIOLENCE );
			scream_effect( vch, level / 2, dam / 4, TARGET_CHAR );

			/* damage(ch,vch,dam/2,sn,DAM_ENERGY,TRUE); if (vch->fighting) stop_fighting( vch , TRUE ); */
		}
		else
		{
			WAIT_STATE( vch, (skill_table[sn].beats + PULSE_VIOLENCE) );
			scream_effect( vch, level, dam, TARGET_CHAR );

			/* damage(ch,vch,dam,sn,DAM_ENERGY,TRUE); */
			if ( vch->fighting )
				stop_fighting( vch, TRUE );
		}
	}
}


void spell_attract_other( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->sex == victim->sex )
	{
		send_to_char( "E cosa cambierebbe?!\n\r", ch );
		return;
	}

	spell_charm_person( sn, level, ch, vo, target );
}


void spell_vampire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "E ora cosa sei? Un troll?\n\r", ch );
		return;
	}

	/* haste */
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 2;
	af.location = APPLY_DEX;
	af.modifier = 1 + ( level >= 18 ) + ( level >= 25 ) + ( level >= 32 );
	af.bitvector = AFF_HASTE;
	affect_to_char( ch, &af );

	/* giant strength */
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 2;
	af.location = APPLY_STR;
	af.modifier = 1 + ( level >= 18 ) + ( level >= 25 ) + ( level >= 32 );
	af.bitvector = 0;
	affect_to_char( ch, &af );

	/* cusse */
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 2;
	af.location = APPLY_SIZE;
	af.modifier = 1 + ( level >= 25 ) + ( level >= 50 ) + ( level >= 75 );
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );

	/* damroll */
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 2;
	af.location = APPLY_DAMROLL;
	af.modifier = ch->damroll;
	af.bitvector = AFF_BERSERK;
	affect_to_char( ch, &af );

	/* vampire flag */
	af.where = TO_ACT_FLAG;
	af.type = sn;
	af.level = level;
	af.duration = level / 2;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = PLR_VAMPIRE;
	affect_to_char( ch, &af );

	send_to_char( "Muti il tuo aspetto rivelando ogni stilla del tuo lato oscuro.\n\r", ch );
	act( "$n muta il suo aspetto in quello di una creatura della notte!", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_animate_dead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*undead;
	OBJ_DATA	*obj, *obj2, *next;
	AFFECT_DATA af;
	char		buf[MSL];
	char		buf2[MSL];
	char		buf3[MSL];
	char		*argument, *arg;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* deal with the object case first */
	if ( target == TARGET_OBJ )
	{
		obj = (OBJ_DATA *) vo;

		if ( !(obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC) )
		{
			send_to_char( "Puoi rianimare solo cadaveri!\n\r", ch );
			return;
		}

		/* if (obj->item_type == ITEM_CORPSE_PC) { send_to_char("The magic fails abruptly!\n\r",ch); return; } */
		if ( is_affected(ch, sn) )
		{
			send_to_char( "Aspetta di accumulare un po' di energie prima.\n\r", ch );
			return;
		}

		if ( count_charmed(ch) )
			return;

		if ( ch->in_room != NULL && IS_SET(ch->in_room->room_flags, ROOM_NO_MOB) )
		{
			send_to_char( "Questo non e' il luogo adatto per farlo..\n\r", ch );
			return;
		}

		if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE)
		||	 IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
		||	 IS_SET(ch->in_room->room_flags, ROOM_SOLITARY) )
		{
			send_to_char( "Questo non e' il luogo adatto per farlo..\n\r", ch );
			return;
		}

		undead = create_mobile( get_mob_index(MOB_VNUM_UNDEAD) );
		char_to_room( undead, ch->in_room );
		for ( i = 0; i < MAX_STATS; i++ )
		{
			undead->perm_stat[i] = UMIN( 25, 2 * ch->perm_stat[i] );
		}

		undead->max_point[LIFE] = IS_MOB( ch ) ? ch->max_point[LIFE] : ch->pcdata->perm_point[LIFE];
		undead->point[LIFE] = undead->max_point[LIFE];
		undead->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
		undead->point[MANA] = undead->max_point[MANA];
		undead->alignment = ch->alignment;
		undead->level = UMIN( 100, (ch->level - 2) );

		for ( i = 0; i < 3; i++ )
			undead->armor[i] = interpolate( undead->level, 100, -100 );
		undead->armor[3] = interpolate( undead->level, 50, -200 );
		undead->damage[DICE_NUMBER] = number_range( level / 20, level / 15 );
		undead->damage[DICE_TYPE] = number_range( level / 6, level / 3 );
		undead->damage[DICE_BONUS] = number_range( level / 12, level / 10 );
		undead->sex = ch->sex;
		undead->gold = 0;

		SET_BIT( undead->act, ACT_UNDEAD );
		SET_BIT( undead->affected_by, AFF_CHARM );
		undead->master = ch;
		undead->leader = ch;

		sprintf( buf, "Il corpo rianimato di %s", obj->name );
		undead->name = str_dup( buf );
		sprintf( buf2, "%s", obj->short_descr );
		argument = alloc_perm( MSL );
		arg = alloc_perm( MSL );
		argument = buf2;
		buf3[0] = '\0';
		while ( argument[0] != '\0' )
		{
			argument = one_argument( argument, arg );
			if ( !(!str_cmp(arg, "Il") || !str_cmp(arg, "corpo") || !str_cmp(arg, "rianimato")
			|| !str_cmp(arg, "rianimato") || !str_cmp(arg, "di")) )
			{
				if ( buf3[0] == '\0' )
					strcat( buf3, arg );
				else
				{
					strcat( buf3, " " );
					strcat( buf3, arg );
				}
			}
		}

		sprintf( buf, "Il corpo rianimato di %s", buf3 );
		undead->short_descr = str_dup( buf );
		sprintf( buf, "Un cadavere rianimato brancola qui intorno..\n\r" );
		undead->long_descr = str_dup( buf );

		for ( obj2 = obj->contains; obj2; obj2 = next )
		{
			next = obj2->next_content;
			obj_from_obj( obj2 );
			obj_to_char( obj2, undead );
		}

		interpret( undead, "wear all", TRUE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = ch->level;
		af.duration = ( ch->level / 10 );
		af.modifier = 0;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		affect_to_char( ch, &af );

		send_to_char( "Richiami alla vita il cadavere!\n\r", ch );
		sprintf( buf, "Con un'arcana formula magica %s riporta alla vita %s!", ch->name, obj->name );
		act( buf, ch, NULL, NULL, TO_ROOM, POS_REST );
		sprintf( buf, "%s ti guarda in maniera truce..", obj->short_descr );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
		extract_obj( obj );
		return;
	}

	victim = (CHAR_DATA *) vo;

	if ( victim == ch )
	{
		send_to_char( "Rianimarti? Sei un cadavere?\n\r", ch );
		return;
	}

	send_to_char( "Non e' ancora cadavere!\n\r", ch );
}


void spell_enhanced_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.modifier = -60;
	af.location = APPLY_AC;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	send_to_char( "Una robusta aura di energia ti riveste proteggendoti.\n\r", victim );
	if ( ch != victim )
		act( "Proteggi $N con la tua magia.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_meld_into_stone( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "La tua pelle e' gia' simile alla roccia.\n\r", ch );
		else
			act( "La pelle di $N e' gia' simile alla roccia.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 10;
	af.location = APPLY_AC;
	af.modifier = -100;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "La pelle di $n si fonde con la pietra.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "La tua pelle si fonde con la pietra..\n\r", victim );
}


void spell_web( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( saves_spell(level, victim, DAM_OTHER) ) return;

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			act( "$N e' gia' imprigionat$X nella ragnatela!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.type = sn;
	af.level = level;
	af.duration = 1;
	af.location = APPLY_HITROLL;
	af.modifier = -1 * ( level / 6 );
	af.where = TO_DETECTS;
	af.bitvector = ADET_WEB;
	affect_to_char( victim, &af );

	af.location = APPLY_DEX;
	af.modifier = -2;
	affect_to_char( victim, &af );

	af.location = APPLY_DAMROLL;
	af.modifier = -1 * ( level / 6 );
	affect_to_char( victim, &af );
	send_to_char( "Una fitta ragnatela ti avvolge completamente!\n\r", victim );
	if ( ch != victim )
		act( "Avvolgi $N in una fitta ragnatela!", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_group_defense( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	AFFECT_DATA af;
	int			shield_sn, armor_sn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	shield_sn = skill_lookup( "shield" );
	armor_sn = skill_lookup( "armor" );

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group(gch, ch) )
			continue;
		if ( is_affected(gch, armor_sn) )
		{
			if ( gch == ch )
				send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
			else
				act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, gch, TO_CHAR, POS_REST );
			continue;
		}

		af.type = armor_sn;
		af.level = level;
		af.duration = level;
		af.location = APPLY_AC;
		af.modifier = -20;
		af.bitvector = 0;
		affect_to_char( gch, &af );

		send_to_char( "Una luce protettiva riveste il tuo corpo.\n\r", gch );
		if ( ch != gch )
			act( "$N viene protett$X dalla tua magia.", ch, NULL, gch, TO_CHAR, POS_REST );
		if ( !is_same_group(gch, ch) )
			continue;
		if ( is_affected(gch, shield_sn) )
		{
			if ( gch == ch )
				send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
			else
				act( "$N ha gia' questo tipo di protezione.", ch, NULL, gch, TO_CHAR, POS_REST );
			continue;
		}

		af.type = shield_sn;
		af.level = level;
		af.duration = level;
		af.location = APPLY_AC;
		af.modifier = -20;
		af.bitvector = 0;
		affect_to_char( gch, &af );

		send_to_char( "Una luce protettiva ricopre il tuo corpo.\n\r", gch );
		if ( ch != gch )
			act( "$N viene avvolt$X da una luce protettiva.", ch, NULL, gch, TO_CHAR, POS_REST );
	}
}


void spell_inspire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	AFFECT_DATA af;
	int			bless_sn;
	/*~~~~~~~~~~~~~~~~~*/

	bless_sn = skill_lookup( "bless" );

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group(gch, ch) )
			continue;
		if ( is_affected(gch, bless_sn) )
		{
			if ( gch == ch )
				send_to_char( "Sei gia' sotto l'effetto di questa benedizione.\n\r", ch );
			else
				act( "$N e' gia' sotto l'effetto di questa benedizione.", ch, NULL, gch, TO_CHAR, POS_REST );
			continue;
		}

		af.type = bless_sn;
		af.level = level;
		af.duration = 6 + level;
		af.location = APPLY_HITROLL;
		af.modifier = level / 12;
		af.bitvector = 0;
		affect_to_char( gch, &af );

		af.location = APPLY_SAVING_SPELL;
		af.modifier = 0 - level / 12;
		affect_to_char( gch, &af );

		send_to_char( "Senti la benedizione degli Dei posarsi su di te!\n\r", gch );
		if ( ch != gch )
			act( "Fai in modo che la benedizione dei tuoi Dei si posi su $N.", ch, NULL, gch, TO_CHAR, POS_REST );
	}
}


void spell_mass_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	AFFECT_DATA af;
	int			sanc_sn;
	/*~~~~~~~~~~~~~~~~*/

	sanc_sn = skill_lookup( "sanctuary" );

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group(gch, ch) )
			continue;
		if ( IS_AFFECTED(gch, AFF_SANCTUARY) )
		{
			if ( gch == ch )
				send_to_char( "Sei gia' sotto l'effetto di questa protezione.\n\r", ch );
			else
				act( "$N e' gia' sotto l'effetto di questa protezioneis already in sanctuary.", ch, NULL, gch, TO_CHAR, POS_REST );
			continue;
		}

		af.type = sanc_sn;
		af.level = level;
		af.duration = number_fuzzy( level / 6 );
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_SANCTUARY;
		af.where = TO_AFFECTS;
		affect_to_char( gch, &af );

		send_to_char( "Una bianca aura sacra ti riveste.\n\r", gch );
		if ( ch != gch )
			act( "Una bianca aura sacra riveste $N.", ch, NULL, gch, TO_CHAR, POS_REST );
	}
}


void spell_mend( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	int			result, skill;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->condition > 99 )
	{
		send_to_char( "Quest'oggetto non ha bisogno di essere riparato.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Per riparare quest'oggetto devi equipaggiarlo.\n\r", ch );
		return;
	}

	skill = get_skill( ch, gsn_mend ) / 2;
	result = number_percent() + skill;

	if ( IS_OBJ_STAT(obj, ITEM_GLOW) )
		result -= 5;
	if ( IS_OBJ_STAT(obj, ITEM_MAGIC) )
		result += 5;

	if ( result >= 50 )
	{
		act( "Ripari $p richiamandone gli elementi!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n ripara $p richiamandone gli elementi.", ch, obj, NULL, TO_ROOM, POS_REST );
		obj->condition += result;
		obj->condition = UMIN( obj->condition, 100 );
		return;
	}
	else if ( result >= 10 )
	{
		send_to_char( "Non riesci a riparare l'oggetto.. qualcosa e' andato storto.\n\r", ch );
		return;
	}
	else
	{
		act( "$p prende fuoco e si distrugge!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p prende fuoco e si distrugge!", ch, obj, NULL, TO_ROOM, POS_REST );
		extract_obj( obj );
		return;
	}
}


void spell_shielding( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( saves_spell(level, victim, DAM_NONE) )
	{
		act( "$N diventa pallid$X per qualche istante..", ch, NULL, victim, TO_CHAR, POS_REST );
		send_to_char( "Ti senti le forze mancare per un attimo..\n\r", victim );
		return;
	}

	if ( is_affected(victim, sn) )
	{
		af.type = sn;
		af.level = level;
		af.duration = level / 20;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = 0;
		affect_to_char( victim, &af );
		act( "Allontani $N dalla fonte del Mana.", ch, NULL, victim, TO_CHAR, POS_REST );
		send_to_char( "Avverti che la distanza dalla fonte del Mana diventa piu' grande..\n\r", victim );
		return;
	}

	af.type = sn;
	af.level = level;
	af.duration = level / 15;
	af.location = APPLY_NONE;

	af.modifier = 0;
	af.bitvector = 0;
	affect_join( victim, &af );

	send_to_char( "Senti che la fonte del Mana si sta allontanando da te..\n\r", victim );
	act( "Allontani $N dalla fonte del Mana!", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_link( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			random, tmpmana;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	random = number_percent();
	tmpmana = ch->point[MANA];
	ch->point[MANA] = 0;
	ch->point[MOVE] /= 2;
	tmpmana = ( .5 * tmpmana );
	tmpmana = ( (tmpmana + random) / 2 );
	victim->point[MANA] = victim->point[MANA] + tmpmana;

	act("$N riceve la tua energia magica.", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n lega la sua anima alla tua rinvigorendo il tuo mana!", ch, NULL, victim, TO_VICT, POS_REST );
}


void spell_power_kill( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "Richiami le piu' nascoste energie delle tenebre colpendo in pieno $N!",
		ch, NULL, victim, TO_CHAR, POS_REST, RED );
	act( "Un'oscura esplosione partendo dalle mani di  $n dirompe su $N!",
		ch, NULL, victim, TO_NOTVICT, POS_REST, RED );
	act( "$N ti colpisce con una terribile esplosione di energie oscure!",
		victim, NULL, ch, TO_CHAR, POS_REST, RED );

	if ( saves_spell(level, victim, DAM_MENTAL) || number_bits(1) == 0 )
	{
		dam = dice( level, 24 );
		damage( ch, victim, dam, sn, DAM_MENTAL, TRUE );
		return;
	}

	send_to_char( "L'oscurita' ti avvolge! Muori cadendo in un tunnel senza fine.. !\n\r", victim );
	act( "$N e' mort$X!", ch, NULL, victim, TO_NOTVICT, POS_REST );

	raw_kill( victim, -1 );
}


void spell_eyed_sword( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char		buf[MSL];
	OBJ_DATA	*eyed;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * if (IS_SET(ch->quest,QUEST_EYE) { send_to_char("You created your sword ,before.\n\r",ch); return; }
	 * SET_BIT(ch->quest,QUEST_EYE);
	 */
	if ( IS_GOOD(ch) )		i = 0;
	else if ( IS_EVIL(ch) )	i = 2;
	else					i = 1;

	eyed = create_object( get_obj_index(OBJ_VNUM_EYED_SWORD), 0 );
	eyed->owner = str_dup( ch->name );
	eyed->from = str_dup( ch->name );
	eyed->altar = hometown_table[ch->hometown].altar[i];
	eyed->pit = hometown_table[ch->hometown].pit[i];
	eyed->level = ch->level;

	sprintf( buf, eyed->short_descr, ch->name );
	free_string( eyed->short_descr );
	eyed->short_descr = str_dup( buf );

	sprintf( buf, eyed->description, ch->name );
	free_string( eyed->description );
	eyed->description = str_dup( buf );

	sprintf( buf, eyed->pIndexData->extra_descr->description, ch->name );
	eyed->extra_descr = new_extra_descr();
	eyed->extra_descr->keyword = str_dup( eyed->pIndexData->extra_descr->keyword );
	eyed->extra_descr->description = str_dup( buf );
	eyed->extra_descr->next = NULL;

	eyed->value[2] = ( ch->level / 10 ) + 3;
	eyed->level = ch->level;
	eyed->cost = 0;
	obj_to_char( eyed, ch );
	send_to_char( "Ecco la tua spada personale.\n\r", ch );
/*	send_to_char( "Non dimenticare che non la potrai creare un'altra volta.\n\r", ch ); */
}


void spell_lion_help( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*lion;
	CHAR_DATA	*victim;
	AFFECT_DATA af;
	char		arg[MIL];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	target_name = one_argument( target_name, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Specifica la vittima.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' nessuno con quel nome qui.\n\r", ch );
		return;
	}

	if ( is_safe_nomessage(ch, victim) )
	{
		send_to_char( "Gli Dei proteggono la tua vittima.\n\r", ch );
		return;
	}

	send_to_char( "Richiami l'aiuto di un leone.\n\r", ch );
	act( "$n emette un particolarissimo urlo.. sembra un ruggito.", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Per ora non puoi gestire altri leoni.\n\r", ch );
		return;
	}

	if ( ch->in_room != NULL && IS_SET(ch->in_room->room_flags, ROOM_NO_MOB) )
	{
		send_to_char( "Nessun leone puoi soccorrerti qui.\n\r", ch );
		return;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	||	 IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
	||	 IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
	||	 room_is_dt(ch)
	||	(	ch->in_room->sector_type != SECT_FIELD
		&&	ch->in_room->sector_type != SECT_FOREST
		&&	ch->in_room->sector_type != SECT_MOUNTAIN
		&&	ch->in_room->sector_type != SECT_HILLS) )
	{
		send_to_char( "Nessun leone puo' ascoltarti da qui.\n\r", ch );
		return;
	}

	lion = create_mobile( get_mob_index(MOB_VNUM_HUNTER) );

	for ( i = 0; i < MAX_STATS; i++ )
		lion->perm_stat[i] = UMIN( 25, 2 * ch->perm_stat[i] );

	lion->max_point[LIFE] = UMIN( 30000, ch->max_point[LIFE] * 1.2 );
	lion->point[LIFE] = lion->max_point[LIFE];
	lion->max_point[MANA] = ch->max_point[MANA];
	lion->point[MANA] = lion->max_point[MANA];
	lion->alignment = ch->alignment;
	lion->level = UMIN( 100, ch->level );
	for ( i = 0; i < 3; i++ )
		lion->armor[i] = interpolate( lion->level, 100, -100 );
	lion->armor[3] = interpolate( lion->level, 100, 0 );
	lion->sex = ch->sex;
	lion->gold = 0;
	lion->damage[DICE_NUMBER] = number_range( level / 15, level / 10 );
	lion->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	lion->damage[DICE_BONUS] = number_range( level / 8, level / 6 );
	lion->detection = ( A | B | C | D | E | F | G | H | ee );

/*	SET_BIT(lion->affected_by, AFF_CHARM);
	lion->master = lion->leader = ch;
*/
	char_to_room( lion, ch->in_room );

	send_to_char( "Un leone accorre contro il tuo nemico!\n\r", ch );
	act( "Un leone accorre al richiamo di $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );
	SET_BIT( lion->act, ACT_HUNTER );
	lion->hunting = victim;
	hunt_victim( lion );
}


void spell_magic_jar( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	OBJ_DATA	*vial;
	OBJ_DATA	*fire;
	char		buf[MSL];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( victim == ch )
	{
		send_to_char( "Su di te? eh?\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Umh.. naaa che bisogno c'e'?!.\n\r", ch );
		return;
	}

	if ( saves_spell(level, victim, DAM_MENTAL) )
	{
		send_to_char( "Fallisci nel tentativo di intrappolare la sua anima.\n\r", ch );
		return;
	}

	for ( vial = ch->carrying; vial != NULL; vial = vial->next_content )
		if ( vial->pIndexData->vnum == OBJ_VNUM_POTION_VIAL ) break;

	if ( vial == NULL )
	{
		send_to_char( "Procurati un contenitore adatto prima.\n\r", ch );
		return;
	}

	extract_obj( vial );
	if ( IS_GOOD(ch) )		i = 0;
	else if ( IS_EVIL(ch) )	i = 2;
	else					i = 1;

	fire = create_object( get_obj_index(OBJ_VNUM_MAGIC_JAR), 0 );
	fire->owner = str_dup( ch->name );
	fire->from = str_dup( ch->name );
	fire->altar = hometown_table[ch->hometown].altar[i];
	fire->pit = hometown_table[ch->hometown].pit[i];
	fire->level = ch->level;

	sprintf( buf, fire->name, victim->name );
	free_string( fire->name );
	fire->name = str_dup( buf );

	sprintf( buf, fire->short_descr, victim->name );
	free_string( fire->short_descr );
	fire->short_descr = str_dup( buf );

	sprintf( buf, fire->description, victim->name );
	free_string( fire->description );
	fire->description = str_dup( buf );

	sprintf( buf, fire->pIndexData->extra_descr->description, victim->name );
	fire->extra_descr = new_extra_descr();
	fire->extra_descr->keyword = str_dup( fire->pIndexData->extra_descr->keyword );
	fire->extra_descr->description = str_dup( buf );
	fire->extra_descr->next = NULL;

	fire->level = ch->level;
	fire->timer = ch->level;
	fire->cost = 0;
	obj_to_char( fire, ch );
	SET_BIT( victim->act, PLR_NO_EXP );
	sprintf( buf, "Intrappoli nel contenitore lo spirito di %s.\n\r", victim->name );
	send_to_char( buf, ch );
}


void turn_spell( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam, align;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( victim != ch )
	{
		act( "$n solleva le braccia e una violenta esplosione di luce colpisce l'ambiente!", ch, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Sollevi le braccia e una violenta esplosione di luce invade l'ambiente!\n\r", ch );
	}

	if ( IS_GOOD(victim) || IS_NEUTRAL(victim) )
	{
		act( "$n non sembra soffrire la luce.", victim, NULL, victim, TO_ROOM, POS_REST );
		send_to_char( "Fortunatamente non soffri la luce..\n\r", victim );
		return;
	}

	dam = dice( level, 10 );
	if ( saves_spell(level, victim, DAM_HOLY) )
		dam /= 2;

	align = victim->alignment;
	align -= 360;

	if ( align < -1000 )
		align = -1000 + ( align + 1000 ) / 3;

	dam = ( dam * align * align ) / 1000000;

	damage( ch, victim, dam, sn, DAM_HOLY, TRUE );

	/* cabal guardians */
	if ( IS_MOB(victim) && victim->cabal != CABAL_NONE )
		return;

	if ( (IS_MOB(victim) && victim->position != POS_DEAD)
	||	(!IS_MOB(victim) && (current_time - victim->last_death_time) > 10) )
	{
		ROOM_INDEX_DATA *was_in;
		ROOM_INDEX_DATA *now_in;
		int				door;
		/*~~~~~~~~~~~~~~~~~~~~*/

		was_in = victim->in_room;
		for ( door = 0; door < MAX_DIR; door++ )
		{
			EXIT_DATA	*pexit;
			/*~~~~~~~~~~~~~~~*/

			if ((pexit = was_in->exit[door]) == 0
			||	pexit->to_room == NULL
			||	IS_SET(pexit->exit_info, EX_CLOSED)
			||	(IS_MOB(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)) )
				continue;

			move_char( victim, door, FALSE );
			if ( (now_in = victim->in_room) == was_in )
				continue;

			victim->in_room = was_in;
			act( "$n fugge via!", victim, NULL, NULL, TO_ROOM, POS_REST );
			victim->in_room = now_in;

			if ( IS_MOB(victim) )
				victim->last_fought = NULL;

			stop_fighting( victim, TRUE );
			return;
		}
	}
}


void spell_turn( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai usato questo potere troppo di recente.", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 5;
	af.modifier = 0;
	af.location = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );

	if ( IS_EVIL(ch) )
	{
		send_to_char( "Avverti un crescente bruciore all'interno del tuo corpo!\n\r", ch );
		turn_spell( sn, ch->level, ch, ch, target );
		return;
	}

	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( is_safe_spell(ch, vch, TRUE) )
			continue;
		if ( is_safe(ch, vch) )
			continue;
		turn_spell( sn, ch->level, ch, vch, target );
	}
}


void spell_fear( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (victim->class == CLASS_SAMURAI) && (victim->level >= LEVEL_NEWBIE+5) )
	{
		send_to_char( "L'obiettivo e' al di la' delle tue attuali capacita'.\n\r", ch );
		return;
	}

	if ( is_affected(victim, gsn_fear) || saves_spell(level, victim, DAM_OTHER) ) return;

	af.where = TO_DETECTS;
	af.type = gsn_fear;
	af.level = level;
	af.duration = level / 10;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = ADET_FEAR;
	affect_to_char( victim, &af );
	send_to_char( "Cominci a tremare vistosamente..\n\r", victim );
	act( "$n comincia a tremare vistosamente..", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_protection_heat( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, gsn_protection_heat) )
	{

		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_protection_cold) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_fire_shield) )
	{

		if ( victim == ch )
			send_to_char( "Hai gia' una protezione contro il fuoco.\n\r", ch );
		else
			act( "$N ha gia' una protezione contro il fuoco.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_protection_heat;
	af.level = level;
	af.duration = 24;
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	send_to_char( "Una spessa barriera di energia ti protegge dalle fiamme.\n\r", victim );
	if ( ch != victim )
		act( "$N viene rivestit$X da una spessa barriera di energia.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_protection_cold( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, gsn_protection_cold) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_protection_heat) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_fire_shield) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' una protezione contro il freddo.\n\r", ch );
		else
			act( "$N ha gia' una protezione contro il freddo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_protection_cold;
	af.level = level;
	af.duration = 24;
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	send_to_char( "Una barriera di cristalli bronzei d'energia ti riveste.\n\r", victim );
	if ( ch != victim )
		act( "Una barriera di bronzei cristalli d'energia circonda $N.", ch, NULL, victim, TO_CHAR, POS_REST );
}



void spell_fire_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char		buf[MSL];
	char		arg[MIL];
	OBJ_DATA	*fire;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	target_name = one_argument( target_name, arg );
	if ( !(!str_cmp(arg, "cold") || !str_cmp(arg, "fire")
		||!str_cmp(arg, "freddo") || !str_cmp(arg, "fuoco")) )
	{
		send_to_char( "Devi specificare il tipo.\n\r", ch );
		return;
	}

	if ( IS_GOOD(ch) )		i = 0;
	else if ( IS_EVIL(ch) )	i = 2;
	else					i = 1;

	fire = create_object( get_obj_index(OBJ_VNUM_FIRE_SHIELD), 0 );
	fire->owner = str_dup( ch->name );
	fire->from = str_dup( ch->name );
	fire->altar = hometown_table[ch->hometown].altar[i];
	fire->pit = hometown_table[ch->hometown].pit[i];
	fire->level = ch->level;

	sprintf( buf, fire->short_descr, arg );
	free_string( fire->short_descr );
	fire->short_descr = str_dup( buf );

	sprintf( buf, fire->description, arg );
	free_string( fire->description );
	fire->description = str_dup( buf );

	sprintf( buf, fire->pIndexData->extra_descr->description, arg );
	fire->extra_descr = new_extra_descr();
	fire->extra_descr->keyword = str_dup( fire->pIndexData->extra_descr->keyword );
	fire->extra_descr->description = str_dup( buf );
	fire->extra_descr->next = NULL;

	fire->level = ch->level;
	fire->cost = 0;
	fire->timer = 5 * ch->level;
	if		( IS_GOOD(ch) )		SET_BIT( fire->extra_flags, (ITEM_ANTI_NEUTRAL | ITEM_ANTI_EVIL | ITEM_NODROP | ITEM_NOUNCURSE) );
	else if ( IS_NEUTRAL(ch) )	SET_BIT( fire->extra_flags, (ITEM_ANTI_GOOD | ITEM_ANTI_EVIL | ITEM_NODROP | ITEM_NOUNCURSE) );
	else if ( IS_EVIL(ch) )		SET_BIT( fire->extra_flags, (ITEM_ANTI_NEUTRAL | ITEM_ANTI_GOOD | ITEM_NODROP | ITEM_NOUNCURSE) );
	obj_to_char( fire, ch );
	send_to_char( "Ecco il tuo scudo.\n\r", ch );
}


void spell_witch_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, gsn_witch_curse) )
	{
		send_to_char( "E' gia' sotto l'effetto di questo incantesimo.\n\r", ch );
		return;
	}

	if ( saves_spell((level + 5), victim, DAM_MENTAL) || number_bits(1) == 0 )
	{
		send_to_char( "Devi aver sbagliato qualcosa!\n\r", ch );
		return;
	}

	ch->point[LIFE] -= ( 2 * level );
	ch->point[LIFE] = UMAX( ch->point[LIFE], 1 );

	af.where = TO_AFFECTS;
	af.type = gsn_witch_curse;
	af.level = level;
	af.duration = 24;
	af.location = APPLY_LIFE;
	af.modifier = -level;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	send_to_char( "Ora cammina sul sentiero della morte..\n\r", ch );
}


void spell_knock( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char			arg[MIL];
	int				chance = 0;
	int				door;
	const sh_int	rev_dir[] = { 2, 3, 0, 1, 5, 4, 6, 7, 8, 9 };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	target_name = one_argument( target_name, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Specifica la porta o la direzione.\n\r", ch );
		return;
	}

	if ( ch->fighting )
	{
		send_to_char( "Aspetta che termini il combattimento no?\n\r", ch );
		return;
	}

	if ( (door = find_door(ch, arg)) >= 0 )
	{
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA		*pexit;
		EXIT_DATA		*pexit_rev;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pexit = ch->in_room->exit[door];
		if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			send_to_char( "E cosa c'e' da sbloccare? E' aperto..\n\r", ch );
			return;
		}

		if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
		{
			send_to_char( "Puoi gia' aprire.\n\r", ch );
			return;
		}

		if ( IS_SET(pexit->exit_info, EX_NOPASS) )
		{
			send_to_char( "Uno scudo mistico blocca l'uscita.\n\r", ch );
			return;
		}

		chance = ch->level / 5 + get_curr_stat( ch, INT ) + get_skill( ch, sn ) / 5;

		act( "Tendi le braccia verso $d cercando di sbloccarne l'apertura!", ch, NULL, pexit->keyword, TO_CHAR, POS_REST );
		act( "Tendi le braccia verso $d cercando di sbloccarne l'apertura!", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );

		if ( room_dark(ch->in_room) ) chance /= 2;

		/* now the attack */
		if ( number_percent() < chance )
		{
			REMOVE_BIT( pexit->exit_info, EX_LOCKED );
			REMOVE_BIT( pexit->exit_info, EX_CLOSED );
			act( "$n tende le braccia verso $d sbloccandone con un gesto l'apertura!", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );
			send_to_char( "Ben fatto!\n\r", ch );

			/* open the other side */
			if ((to_room = pexit->to_room) != NULL
			&&	(pexit_rev = to_room->exit[rev_dir[door]]) != NULL
			&&	pexit_rev->to_room == ch->in_room )
			{
				CHAR_DATA	*rch;
				/*~~~~~~~~~~~~~*/

				REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
				REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
				for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
					act( "$d si apre.", rch, NULL, pexit_rev->keyword, TO_CHAR, POS_REST );
			}
		}
		else
		{
			act( "Niente da fare!", ch, NULL, pexit->keyword, TO_CHAR, POS_REST );

		}

		return;
	}

	send_to_char( "Non vedo niente del genere qui..\n\r", ch );
}


void spell_magic_resistance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, sn) )
	{
		send_to_char( "Ora sei resistente alla magia.\n\r", ch );

		af.where = TO_RESIST;
		af.type = sn;
		af.duration = level / 10;
		af.level = ch->level;
		af.bitvector = RES_MAGIC;
		af.location = 0;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
	else
		send_to_char( "Sei gia' resistente alla magia.\n\r", ch );
}


void spell_hallucination( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	send_to_char( "That spell is under construction.\n\r", ch );
}


void spell_vam_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 12 );
	if ( saves_spell(level, victim, DAM_ACID) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_ACID, TRUE );
}


void spell_dragon_skin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level;
	af.location = APPLY_AC;
	af.modifier = -( 2 * level );
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "Il corpo di $n viene rivestito da dure scaglie di drago.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Il tuo corpo viene rivestito da dure scaglie di drago.\n\r", victim );
}


void spell_mind_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af, af2;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "L'ambiente e' gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_CONST;
	af.type = sn;
	af.level = level;
	af.duration = level / 30;
	af.location = APPLY_ROOM_MANA;
	af.modifier = level;
	af.bitvector = 0;
	affect_to_room( ch->in_room, &af );

	af2.where = TO_AFFECTS;
	af2.type = sn;
	af2.level = level;
	af2.duration = level / 10;
	af2.modifier = 0;
	af2.location = APPLY_NONE;
	af2.bitvector = 0;
	affect_to_char( ch, &af2 );
	send_to_char( "L'ambiente viene illuminato da una luce ristoratrice.\n\r", ch );
	act( "L'ambiente viene illuminato da una luce ristoratrice.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_insanity( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        /*Perche' mai le cose piu' divertenti non si possono fare ai png!?*/
	if ( IS_MOB(ch) )
	{
		send_to_char( "Mhh non e' un soggetto adatto, ok?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_BLOODTHIRST) || saves_spell(level, victim, DAM_OTHER) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 10;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = AFF_BLOODTHIRST;
	affect_to_char( victim, &af );
	send_to_char( "La follia pervade la tua mente!\n\r", victim );
	act( "$n assume un'espressione totalmente folle!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_power_stun( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) || saves_spell(level, victim, DAM_OTHER) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 90;
	af.location = APPLY_DEX;
	af.modifier = -3;
	af.bitvector = AFF_STUN;
	affect_to_char( victim, &af );
	act( "Ti senti stordit$x!",ch, NULL, victim, TO_VICT, POS_REST);
	act( "$n sembra stordit$x!", victim, NULL, NULL, TO_ROOM, POS_SLEEP, RED );
}


void spell_improved_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_IMP_INVIS) )
		return;

	act( "$n svanisce nel nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 10;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_IMP_INVIS;
	affect_to_char( victim, &af );
	send_to_char( "Invisibilizzi il tuo corpo dal piano materiale.\n\r", victim );
}


void spell_improved_detection( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, DETECT_IMP_INVIS) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 3;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = DETECT_IMP_INVIS;
	affect_to_char( victim, &af );
	send_to_char( "I tuoi occhi brillano per un attimo.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo e' andato a buon fine.\n\r", ch );
}


void spell_severity_force( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	char		buf[MSL];
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Crei una crepa nel terreno sotto %s.\n\r", victim->name );
	send_to_char( buf, ch );
	act( "$n crea una crepa sotto di te!.", ch, NULL, victim, TO_VICT, POS_REST );
	if ( IS_AFFECTED(victim, AFF_FLYING) )
		dam = 0;
	else
		dam = dice( level, 12 );
	damage( ch, victim, dam, sn, DAM_BASH, TRUE );
}


void spell_bless_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type != ITEM_WEAPON )
	{
		send_to_char( "Non mi sembra un'arma..\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "L'oggetto deve essere nell'inventario.\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_WEAPON )
	{
		if ( IS_WEAPON_STAT(obj, WEAPON_FLAMING)
		||	 IS_WEAPON_STAT(obj, WEAPON_FROST)
		||	 IS_WEAPON_STAT(obj, WEAPON_VAMPIRIC)
		||	 IS_WEAPON_STAT(obj, WEAPON_SHARP)
		||	 IS_WEAPON_STAT(obj, WEAPON_VORPAL)
		||	 IS_WEAPON_STAT(obj, WEAPON_SHOCKING)
		||	 IS_WEAPON_STAT(obj, WEAPON_HOLY)
		||	 IS_OBJ_STAT(obj, ITEM_BLESS)
		||	 IS_OBJ_STAT(obj, ITEM_BURN_PROOF) )
		{
			act( "$p sembra non subire alcuna alterazione.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}
	}

	if ( IS_WEAPON_STAT(obj, WEAPON_HOLY) )
	{
		act( "$p ha gia' questo tipo di benedizione.", ch, obj, NULL, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_WEAPON;
	af.type = sn;
	af.level = level / 2;
	af.duration = level / 8;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = WEAPON_HOLY;
	affect_to_obj( obj, &af );

	act( "$p e' ora un'arma piu' efficace contro i malvagi.", ch, obj, NULL, TO_ALL, POS_REST );
}


void spell_resilience( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, sn) )
	{
		send_to_char( "Ora hai una protezione contro gli attacchi a base di risucchio di energia.\n\r", ch );

		af.where = TO_RESIST;
		af.type = sn;
		af.duration = level / 10;
		af.level = ch->level;
		af.bitvector = RES_ENERGY;
		af.location = 0;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
	else
		send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
}


void spell_super_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			bonus = 170 + level + dice( 1, 20 );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	victim->point[LIFE] = UMIN( victim->point[LIFE] + bonus, victim->max_point[LIFE] );
	update_pos( victim );
	send_to_char( "Un caldo tepore benefico ti attraversa.\n\r", victim );
	if ( ch != victim )
		send_to_char( "Cura andata a buon fine.\n\r", ch );
}


void spell_master_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			bonus = 300 + level + dice( 1, 40 );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	victim->point[LIFE] = UMIN( victim->point[LIFE] + bonus, victim->max_point[LIFE] );
	update_pos( victim );
	send_to_char( "Un caldo tepore cura le tue ferite.\n\r", victim );
	if ( ch != victim )
		send_to_char( "Cura andata a buon fine.\n\r", ch );
}


void spell_group_healing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	int			heal_num, refresh_num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	heal_num = skill_lookup( "master healing" );
	refresh_num = skill_lookup( "refresh" );

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( (IS_MOB(ch) && IS_MOB(gch)) || (!IS_MOB(ch) && !IS_MOB(gch)) )
		{
			spell_heal( heal_num, level, ch, (void *) gch, TARGET_CHAR );
			spell_refresh( refresh_num, level, ch, (void *) gch, TARGET_CHAR );
		}
	}
}


void spell_restoring_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			nsn, mana_add;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_BLIND) )
	{
		nsn = skill_lookup( "cure blindness" );
		spell_cure_blindness( nsn, level, ch, (void *) victim, TARGET_CHAR );
	}

	if ( IS_AFFECTED(victim, AFF_CURSE) )
	{
		nsn = skill_lookup( "remove curse" );
		spell_remove_curse( nsn, level, ch, (void *) victim, TARGET_CHAR );
	}

	if ( IS_AFFECTED(victim, AFF_POISON) )
	{
		spell_cure_poison( gsn_cure_poison, level, ch, (void *) victim, TARGET_CHAR );
	}

	if ( IS_AFFECTED(victim, AFF_PLAGUE) )
	{
		nsn = skill_lookup( "cure disease" );
		spell_cure_disease( nsn, level, ch, (void *) victim, TARGET_CHAR );
	}

	if ( victim->point[LIFE] != victim->max_point[LIFE] )
	{
		mana_add = UMIN( (victim->max_point[LIFE] - victim->point[LIFE]), ch->point[MANA] );
		victim->point[LIFE] = UMIN( victim->point[LIFE] + mana_add, victim->max_point[LIFE] );
		ch->point[MANA] -= mana_add;
	}

	update_pos( victim );
	send_to_char( "Un caldo tepore di attraversa curandoti.\n\r", victim );
	if ( ch != victim )
		send_to_char( "Cura andata a buon fine.\n\r", ch );
}


void spell_summon_zombie( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*zombie;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per animare uno zombie proprio ora.\n\r", ch );
		return;
	}

	send_to_char( "L'oscuro dono della non morte accarezzi le logorate membra e ti dia il servitore..\n\r", ch );
	act( "$n pronuncia un'oscura litania imponendo le mani su di un raccapricciante cadavere..", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_ZOMBIE) )
		{
			i++;

			if ( i > 2 )
			{
				send_to_char( "Non puoi avere piu' zombie di cosi'!\n\r", ch );
				return;
			}
		}
	}

	if ( count_charmed(ch) )
		return;

	zombie = create_mobile( get_mob_index(MOB_VNUM_ZOMBIE) );

	for ( i = 0; i < MAX_STATS; i++ )
		zombie->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	zombie->perm_stat[STR] += 3;
	zombie->perm_stat[INT] -= 1;
	zombie->perm_stat[CON] += 2;

	zombie->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (2 * ch->pcdata->perm_point[LIFE]) + 400,	30000 );
	zombie->point[LIFE] = zombie->max_point[LIFE];
	zombie->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	zombie->point[MANA] = zombie->max_point[MANA];
	zombie->level = ch->level;
	for ( i = 0; i < 3; i++ )
		zombie->armor[i] = interpolate( zombie->level, 100, -100 );
	zombie->armor[3] = interpolate( zombie->level, 100, 0 );
	zombie->gold = 0;
	zombie->timer = 0;
	zombie->damage[DICE_NUMBER] = 3;
	zombie->damage[DICE_TYPE] = 10;
	zombie->damage[DICE_BONUS] = ch->level / 2;

	char_to_room( zombie, ch->in_room );
	send_to_char( "Ecco il tuo zombie, pronto a servirti!\n\r", ch );
	act( "$n riesce a rianimare la putrida carcassa.. e' disgustoso!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( zombie->affected_by, AFF_CHARM );
	zombie->master = zombie->leader = ch;
}


void spell_summon_skeleton( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*skeleton;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per creare un scheletro proprio ora.\n\r", ch );
		return;
	}

	send_to_char( "Anche alla fine della carne il cerchio della vita non e' chiuso..\n\r", ch );
	act( "$n raduna intorno a se' ossa di ogni tipo e pronuncia una terribile litania..", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_SKELETON) )
		{
			i++;
			if ( i > 2 )
			{
				send_to_char( "Hai gia' troppi scheletri sotto il tuo controllo!\n\r", ch );
				return;
			}
		}
	}

	if ( count_charmed(ch) )
		return;

	skeleton = create_mobile( get_mob_index(MOB_VNUM_SKELETON) );

	for ( i = 0; i < MAX_STATS; i++ )
		skeleton->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	skeleton->perm_stat[STR] += 3;
	skeleton->perm_stat[INT] -= 1;
	skeleton->perm_stat[CON] += 2;

	skeleton->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (5 * ch->pcdata->perm_point[LIFE]) + 2000, 30000 );
	skeleton->point[LIFE] = skeleton->max_point[LIFE];
	skeleton->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	skeleton->point[MANA] = skeleton->max_point[MANA];
	skeleton->level = ch->level;
	for ( i = 0; i < 3; i++ )
		skeleton->armor[i] = interpolate( skeleton->level, 100, -100 );
	skeleton->armor[3] = interpolate( skeleton->level, 100, 0 );
	skeleton->gold = 0;
	skeleton->timer = 0;
	skeleton->damage[DICE_NUMBER] = 8;
	skeleton->damage[DICE_TYPE] = 4;
	skeleton->damage[DICE_BONUS] = ch->level / 2;

	char_to_room( skeleton, ch->in_room );
	send_to_char( "La vita ti dono! Alzati cumulo di ossa! Sei vivo ed io sono il tuo padrone!\n\r", ch );
	act( "$n rianima il cumulo di ossa in un terribile scheletro!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( skeleton->affected_by, AFF_CHARM );
	skeleton->master = skeleton->leader = ch;
}


void spell_summon_spectre( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*spectre;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per creare uno spettro proprio ora.\n\r", ch );
		return;
	}

	send_to_char( "Disegni il cerchio della costrizione attorno a te per richiamare un'anima dannata.\n\r", ch );
	act( "$n compie degli ampi gesti sussurrando antiche litanie..", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if (IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_SPECTRE) )
		{
			send_to_char( "Hai gia' troppi spettri sotto il tuo controllo!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) ) return;

	spectre = create_mobile( get_mob_index(MOB_VNUM_SPECTRE) );

	for ( i = 0; i < MAX_STATS; i++ ) spectre->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	spectre->perm_stat[STR] += 3;
	spectre->perm_stat[INT] -= 1;
	spectre->perm_stat[CON] += 2;

	spectre->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (10 * ch->pcdata->perm_point[LIFE]) + 1000, 30000 );
	spectre->point[LIFE] = spectre->max_point[LIFE];
	spectre->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	spectre->point[MANA] = spectre->max_point[MANA];
	spectre->level = ch->level;
	for ( i = 0; i < 3; i++ )
		spectre->armor[i] = interpolate( spectre->level, 100, -100 );
	spectre->armor[3] = interpolate( spectre->level, 100, 0 );
	spectre->gold = 0;
	spectre->timer = 0;
	spectre->damage[DICE_NUMBER] = 11;
	spectre->damage[DICE_TYPE] = 5;
	spectre->damage[DICE_BONUS] = ch->level / 2 + 10;

	char_to_room( spectre, ch->in_room );
	send_to_char( "La maledizione e' il tuo giogo su questa terra.\n\r", ch );
	send_to_char(" Ed ora diviene il mio scettro perche' sono il tuo padrone!\n\r", ch );
	act( "Dal nulla appare uno spettro che immediatamente si rivolge a $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( spectre->affected_by, AFF_CHARM );
	spectre->master = spectre->leader = ch;
}


void spell_summon_banshee( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*banshee;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per creare un banshee proprio ora.\n\r", ch );
		return;
	}

	send_to_char( "Del piu' antico dolore e delle piu' grandi sofferenze e paure, vieni a me.\n\r", ch );
	send_to_char( "Della morte son maestro e la mia sapienza sara' il tuo giogo!\n\r", ch);
	act( "Le mani di $n si circondano di fuochi fatui mentre la sua voce cantilena senza sosta..", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if (IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_BANSHEE) )
		{
			send_to_char( "Hai gia' troppe banshee sotto il tuo controllo!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	banshee = create_mobile( get_mob_index(MOB_VNUM_BANSHEE) );

	for ( i = 0; i < MAX_STATS; i++ )
		banshee->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	banshee->perm_stat[STR] += 3;
	banshee->perm_stat[INT] -= 1;
	banshee->perm_stat[CON] += 2;

	banshee->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (10 * ch->pcdata->perm_point[LIFE]) + 4000, 30000 );
	banshee->point[LIFE] = banshee->max_point[LIFE];
	banshee->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	banshee->point[MANA] = banshee->max_point[MANA];
	banshee->level = ch->level;
	for ( i = 0; i < 3; i++ )
		banshee->armor[i] = interpolate( banshee->level, 100, -100 );
	banshee->armor[3] = interpolate( banshee->level, 100, 0 );
	banshee->gold = 0;
	banshee->timer = 0;
	banshee->damage[DICE_NUMBER] = 13;
	banshee->damage[DICE_TYPE] = 9;
	banshee->damage[DICE_BONUS] = ch->level / 2 + 10;

	char_to_room( banshee, ch->in_room );
	send_to_char( "Accogli la tua banshee.. e non guardarla mai negli occhi o prestar fede alle sue parole.\n\r", ch );
	act( "Una terrorizzante figura di donna emerge dal nulla.. ti osserva con occhi di ghiaccio..", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( banshee->affected_by, AFF_CHARM );
	banshee->master = banshee->leader = ch;
}


void spell_sanctify_lands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( number_bits(1) == 0 )
	{
		send_to_char( "L'incantesimo fallisce.\n\r", ch );
		return;
	}

	if ( IS_RAFFECTED(ch->in_room, AFF_ROOM_CURSE) )
	{
		affect_strip_room( ch->in_room, gsn_cursed_lands );
		send_to_char( "La maledizione svanisce lentamente..\n\r", ch );
		act( "La maledizione svanisce lentamente..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_RAFFECTED(ch->in_room, AFF_ROOM_POISON) )
	{
		affect_strip_room( ch->in_room, gsn_deadly_venom );
		send_to_char( "L'ambiente sembra piu' confortevole ora.\n\r", ch );
		act( "L'ambiente sembra piu' confortevole ora.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_RAFFECTED(ch->in_room, AFF_ROOM_SLEEP) )
	{
		send_to_char( "L'ambiente sembra piu' confortevole ora.\n\r", ch );
		act( "L'ambiente sembra piu' confortevole ora.", ch, NULL, NULL, TO_ROOM, POS_REST );
		affect_strip_room( ch->in_room, gsn_mysterious_dream );
	}

	if ( IS_RAFFECTED(ch->in_room, AFF_ROOM_PLAGUE) )
	{
		send_to_char( "L'ambiente sembra piu' confortevole ora.\n\r", ch );
		act( "L'ambiente sembra piu' confortevole ora.", ch, NULL, NULL, TO_ROOM, POS_REST );
		affect_strip_room( ch->in_room, gsn_black_death );
	}

	if ( IS_RAFFECTED(ch->in_room, AFF_ROOM_SLOW) )
	{
		send_to_char( "L'ambiente sembra piu' confortevole ora.\n\r", ch );
		act( "L'ambiente sembra piu' confortevole ora.", ch, NULL, NULL, TO_ROOM, POS_REST );
		affect_strip_room( ch->in_room, gsn_lethargic_mist );
	}
}

void spell_deadly_venom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) )
	{
		send_to_char( "Questa zona e' protetta dagli Dei.\n\r", ch );
		return;
	}

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "Non c'e' bisogno di rincarare la dose..\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 15;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_POISON;
	affect_to_room( ch->in_room, &af );

	send_to_char( "La stanza viene riempita di un gas venefico.\n\r", ch );
	act( "La stanza viene riempita di un gas venefico.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_cursed_lands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) )
	{
		send_to_char( "Questa zona e' protetta dagli Dei.\n\r", ch );
		return;
	}

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "C'e' gia' una maledizione che grava qui.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 15;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_CURSE;
	affect_to_room( ch->in_room, &af );

	send_to_char( "Un senso di malessere diffuso pervade la stanza.\n\r", ch );
	act( "Un senso di malessere diffuso pervade la stanza.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_lethargic_mist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) )
	{
		send_to_char( "Questa stanza e' protetta dagli Dei.\n\r", ch );
		return;
	}

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "Quest'incantesimo grava gia' in questa zona.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 15;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_SLOW;
	affect_to_room( ch->in_room, &af );

	send_to_char( "L'atmosfera nella stanza sembra rallentare i tuoi movimenti..\n\r", ch );
	act( "L'atmosfera nella stanza sembra rallentare i tuoi movimenti..", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_black_death( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) )
	{
		send_to_char( "Questa zona e' protetta dagli Dei.\n\r", ch );
		return;
	}

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "La zona e' gia' affetta da quest'incantesimo.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 15;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_PLAGUE;
	affect_to_room( ch->in_room, &af );

	send_to_char( "Nocivi fumi riempiono la stanza..\n\r", ch );
	act( "Nocivi fumi riempiono la stanza..", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_mysterious_dream( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) )
	{
		send_to_char( "La zona e' protetta dagli Dei.\n\r", ch );
		return;
	}

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "La zona e' gia' affetta dall'incantesimo.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 15;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_SLEEP;
	affect_to_room( ch->in_room, &af );

	send_to_char( "L'aria della stanza sembra darti una certa sonnolenza..\n\r", ch );
	act( "L'aria della stanza sembra darti una certa sonnolenza..", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_polymorph( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int			race;
	/*~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai gia' cambiato il tuo aspetto.\n\r", ch );
		return;
	}

	if ( target_name == NULL )
	{
		send_to_char( "Utilizzo: cast 'polymorph' <nome della razza>.\n\r", ch );
		return;
	}

	race = race_lookup( target_name );

	if ( race == 0 || !race_table[race].pc_race )
	{
		send_to_char( "Non e' una razza valida per questo incantesimo.\n\r", ch );
		return;
	}

	af.where = TO_RACE;
	af.type = sn;
	af.level = level;
	af.duration = level / 10;
	af.location = APPLY_NONE;
	af.modifier = race;
	af.bitvector = 0;
	affect_to_char( ch, &af );

	act( "$n muta il suo aspetto in  $t.",
		ch, (ch->sex == SEX_FEMALE) ? race_table[race].ftranslate : race_table[race].translate,
		NULL, TO_ROOM, POS_REST );
	act( "Muti il tuo aspetto in $t.",
		ch, (ch->sex == SEX_FEMALE) ? race_table[race].ftranslate : race_table[race].translate,
		NULL, TO_CHAR, POS_REST );
}

void spell_plant_form( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if (!ch->in_room
	||	IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
	||	(ch->in_room->sector_type != SECT_FOREST && ch->in_room->sector_type != SECT_FIELD)
	||	IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) )
	{
		send_to_char( "Non qui.\n\r", ch );
		return;
	}

	af.where = TO_DETECTS;
	af.type = gsn_fear;
	af.level = level;
	af.duration = level / 10;
	af.location = 0;
	af.modifier = 0;

	if ( ch->in_room->sector_type == SECT_FOREST )
	{
		send_to_char( "Muti il tuo aspetto in quello di un albero!\n\r", ch );
		act( "$n muta il suo aspetto in quello di un albero!", ch, NULL, NULL, TO_ROOM, POS_REST );
		af.bitvector = ADET_FORM_TREE;
	}
	else
	{
		send_to_char( "Muti il tuo aspetto in quello di un cespuglio!\n\r", ch );
		act( "$n muta il suo aspetto in quello di un cespuglio!", ch, NULL, NULL, TO_ROOM, POS_REST );
		af.bitvector = ADET_FORM_GRASS;
	}

	affect_to_char( ch, &af );
}


void spell_blade_barrier( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "Un cerchio di affilate lame appare intorno a $N infilzandol$X ripetutamente!", ch, NULL, victim, TO_ROOM, POS_REST );
	act( "Un cerchio di affilate lame appare intorno a $N infilzandol$X ripetutamente!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "Un cerchio di affilate lame appare intorno a te infilzandoti ripetutamente!", ch, NULL, victim, TO_VICT, POS_REST );

	dam = dice( level, 7 );
	if ( saves_spell(level, victim, DAM_PIERCE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_PIERCE, TRUE );

	if ( victim != ch
	&&	ch->fighting != victim
	&&	victim->fighting != ch
	&&	(IS_SET(victim->affected_by, AFF_CHARM) || IS_MOB(victim)) )
	{
		do_yell( victim, "Aiuto! Mi stanno attaccando!" );
	}

	act( "Le lame si conficcano nelle carni di $n!", victim, NULL, NULL, TO_ROOM, POS_REST );
	dam = dice( level, 5 );
	if ( saves_spell(level, victim, DAM_PIERCE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_PIERCE, TRUE );
	act( "Le lame ti dilaniano le carni!", victim, NULL, NULL, TO_CHAR, POS_REST );

	if ( number_percent() < 50 )
		return;

	act( "Le lame si conficcano nelle carni di $n!", victim, NULL, NULL, TO_ROOM, POS_REST );
	dam = dice( level, 4 );
	if ( saves_spell(level, victim, DAM_PIERCE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_PIERCE, TRUE );
	act( "Le lame ti dilaniano le carni!", victim, NULL, NULL, TO_CHAR, POS_REST );

	if ( number_percent() < 50 )
		return;

	act( "Le lame si conficcano nelle carni di $n!", victim, NULL, NULL, TO_ROOM, POS_REST );
	dam = dice( level, 2 );
	if ( saves_spell(level, victim, DAM_PIERCE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_PIERCE, TRUE );
	act( "Le lame ti dilaniano le carni!", victim, NULL, NULL, TO_CHAR, POS_REST );
}


void spell_protection_negative( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, sn) )
	{
		send_to_char( "Ora sei immune agli attacchi negativi.\n\r", ch );

		af.where = TO_IMMUNE;
		af.type = sn;
		af.duration = level / 4;
		af.level = ch->level;
		af.bitvector = IMM_NEGATIVE;
		af.location = 0;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
	else
		send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
}


void spell_control_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(victim) || !IS_SET(victim->act, ACT_UNDEAD) )
	{
		act( "$N non mi sembra un nonmorto..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	spell_charm_person( sn, level, ch, vo, target );
}


void spell_assist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai usato questo potere troppo di recente.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 1 + level / 50;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );

	victim->point[LIFE] += 100 + level * 5;
	update_pos( victim );
	send_to_char( "Le tue ferite si rimarginano lievemente.\n\r", victim );
	act( "$n sta un po' meglio.", victim, NULL, NULL, TO_ROOM, POS_REST );
	if ( ch != victim )
		send_to_char( "Cura riuscita.\n\r", ch );
}


void spell_aid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai usato questo potere troppo di recente.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 50;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );

	victim->point[LIFE] += level * 5;
	update_pos( victim );
	send_to_char( "Un caldo ristoratore ti attraversa il corpo.\n\r", victim );
	act( "$n sembra stare meglio.", victim, NULL, NULL, TO_ROOM, POS_REST );
	if ( ch != victim )
		send_to_char( "Cura riuscita.\n\r", ch );
}


void spell_summon_shadow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*shadow;
	AFFECT_DATA af;
	int			i;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Ti manca l'energia per evocare un'altra ombra, per ora.\n\r", ch );
		return;
	}

	send_to_char( "Incroci le braccia stabilendo un contatto con il mondo delle ombre.\n\r", ch );
	act( "$n incrocia le braccia sibilando un'oscura litania.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if (IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	gch->pIndexData->vnum == MOB_VNUM_SUM_SHADOW )
		{
			send_to_char( "Nei puoi controllare solo una alla volta!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	shadow = create_mobile( get_mob_index(MOB_VNUM_SUM_SHADOW) );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		shadow->perm_stat[i] = ch->perm_stat[i];
	}

	shadow->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		URANGE( ch->pcdata->perm_point[LIFE], ch->point[LIFE], 30000 );
	shadow->point[LIFE] = shadow->max_point[LIFE];
	shadow->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	shadow->point[MANA] = shadow->max_point[MANA];
	shadow->level = ch->level;
	for ( i = 0; i < 3; i++ )
		shadow->armor[i] = interpolate( shadow->level, 100, -100 );
	shadow->armor[3] = interpolate( shadow->level, 100, 0 );
	shadow->gold = 0;
	shadow->timer = 0;
	shadow->damage[DICE_NUMBER] = number_range( level / 15, level / 10 );
	shadow->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	shadow->damage[DICE_BONUS] = number_range( level / 8, level / 6 );

	char_to_room( shadow, ch->in_room );
	act( "Un'ombra si materializza dal nulla!", ch, NULL, NULL, TO_ALL, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( shadow->affected_by, AFF_CHARM );
	shadow->master = shadow->leader = ch;
}


void spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	ROOM_INDEX_DATA *room, *oldr;
	int				mount;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (room = check_place(ch, target_name)) == NULL )
	{
		send_to_char( "Non riesci a vedere nulla in quella direzione.\n\r", ch );
		return;
	}

	if ( ch->in_room == room )
		do_look( ch, "auto" );
	else
	{
		mount = MOUNTED( ch ) ? 1 : 0;
		oldr = ch->in_room;
		char_from_room( ch );
		char_to_room( ch, room );
		do_look( ch, "auto" );
		char_from_room( ch );
		char_to_room( ch, oldr );
		if ( mount )
		{
			ch->riding = TRUE;
			MOUNTED( ch )->riding = TRUE;
		}
	}
}


void spell_remove_fear( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( check_dispel(level, victim, gsn_fear) )
	{
		send_to_char( "Il timore viene spazzato via dal tuo animo.\n\r", victim );
		act( "$n sembra piu' sicur$x di se'.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}
	else
		send_to_char( "L'incantesimo sembra aver fallito.\n\r", ch );
}


void spell_desert_fist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (ch->in_room->sector_type != SECT_HILLS)
	&&	 (ch->in_room->sector_type != SECT_MOUNTAIN)
	&&	 (ch->in_room->sector_type != SECT_DESERT) )
	{
		send_to_char( "E la sabbia dove sarebbe?\n\r", ch );
		ch->wait = 0;
		return;
	}

	act( "La sabbia sotto di te forma un pugno che va a colpire $n con violenza!", victim, NULL, NULL, TO_ROOM, POS_REST );
	act( "La sabbia sotto di te forma un pugno che ti colpisce in pieno!", victim, NULL, NULL, TO_CHAR, POS_REST );
	dam = dice( level, 16 );
	damage( ch, victim, dam, sn, DAM_OTHER, TRUE );
	sand_effect( victim, level, dam, TARGET_CHAR );
}


void spell_holy_aura( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !IS_GOOD(victim) )
	{
		send_to_char( "Il suo allineamento non e' appropriato per una protezione simile!", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 7 + level / 6;
	af.modifier = -( 20 + level / 4 );
	af.location = APPLY_AC;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	af.where = TO_RESIST;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = RES_NEGATIVE;
	affect_to_char( ch, &af );

	send_to_char( "Un antico e sacro potere ti protegge.\n\r", victim );
	if ( ch != victim )
		act( "$N viene protett$X da un antico e sacro potere.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_holy_fury( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) || IS_AFFECTED(victim, AFF_BERSERK) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, skill_lookup("calm")) )
	{
		if ( victim == ch )
			send_to_char( "Furia? Ora?! Naa..\n\r", ch );
		else
			act( "$N non sembra cosi' dispost$X alla furia..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !IS_GOOD(victim) )
	{
		act( "$N non e' dell'allineamento appropriato per ricevere tale incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 3;
	af.modifier = level / 6;
	af.bitvector = 0;

	af.location = APPLY_HITROLL;
	affect_to_char( victim, &af );

	af.location = APPLY_DAMROLL;
	affect_to_char( victim, &af );

	af.modifier = 10 * ( level / 12 );
	af.location = APPLY_AC;
	affect_to_char( victim, &af );

	send_to_char( "Una sacra furia t'investe!\n\r", victim );

	act( "$n e' una furia!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_light_arrow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 12 );
	if ( saves_spell(level, victim, DAM_HOLY) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_HOLY, TRUE );
}


void spell_hydroblast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(	(ch->in_room->sector_type != SECT_WATER_SWIM)
	&&	(ch->in_room->sector_type != SECT_WATER_NOSWIM)
	&&	(weather_info.sky != SKY_RAINING || !IS_OUTSIDE(ch)) )
	{
		send_to_char( "Ti serve una fonte d'acqua nelle vicinanze.\n\r", ch );
		ch->wait = 0;
		return;
	}

	act( "$n evoca un enorme getto d'acqua che ti colpisce con violenza!",ch, NULL, victim, TO_VICT, POS_REST);
	act( "Un enorme getto d'acqua va a colpire in pieno $N!", ch, NULL, victim, TO_NOTVICT, POS_REST);
	act( "Le molecole d'acqua si radunano intorno a te formando un pugno che va a colpire $N in pieno!", ch, NULL, NULL, TO_CHAR, POS_REST );
	dam = dice( level, 14 );
	damage( ch, victim, dam, sn, DAM_BASH, TRUE );
}


void spell_chromatic_orb( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 14 );
	if ( saves_spell(level, victim, DAM_LIGHT) )
		dam /= 2;

	if ( number_percent() < (0.7 * get_skill(ch, sn)) )
	{
		spell_blindness( skill_lookup("blindness"), (level - 10), ch, (void *) victim, TARGET_CHAR );

		spell_slow( skill_lookup("slow"), (level - 10), ch, (void *) victim, TARGET_CHAR );
	}

	damage( ch, victim, dam, sn, DAM_LIGHT, TRUE );
}


void spell_suffocate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, ADET_SUFFOCATE) )
	{
		act( "$N e' gia' incapace di respirare.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( saves_spell(level, victim, DAM_NEGATIVE) || (IS_MOB(victim) && IS_SET(victim->act, ACT_UNDEAD)) )
	{
		if ( ch == victim )
			send_to_char( "Per un attimo ti senti soffocare, ma passi subito.\n\r", ch );
		else
			act( "$N non sembra soffrire quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level * 3 / 4;
	af.duration = 3 + level / 30;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ADET_SUFFOCATE;
	affect_join( victim, &af );

	send_to_char( "Qualcosa ti blocca il respiro!\n\r", victim );
	act( "$n sembra soffocare!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_mummify( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*undead;
	OBJ_DATA	*obj, *obj2, *next;
	AFFECT_DATA af;
	char		buf[MSL];
	char		buf2[MSL];
	char		buf3[MSL];
	char		*argument, *arg;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* deal with the object case first */
	if ( target == TARGET_OBJ )
	{
		obj = (OBJ_DATA *) vo;

		if ( !(obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC) )
		{
			send_to_char( "Puoi rinominare solo dei cadaveri!\n\r", ch );
			return;
		}

		if ( obj->level > level + 10 )
		{
			send_to_char( "L'essenza rinchiusa nel corpo e' troppo potente per te!\n\r", ch );
			return;
		}

		if ( is_affected(ch, sn) )
		{
			send_to_char( "Non hai forze sufficienti per rianimare altri corpi, ora.\n\r", ch );
			return;
		}

		if ( count_charmed(ch) )
			return;

		if ( ch->in_room != NULL && IS_SET(ch->in_room->room_flags, ROOM_NO_MOB) )
		{
			send_to_char( "Non puoi farlo proprio qui.\n\r", ch );
			return;
		}

		if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE)
		||	 IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
		||	 IS_SET(ch->in_room->room_flags, ROOM_SOLITARY) )
		{
			send_to_char( "Non puoi farlo proprio qui.\n\r", ch );
			return;
		}

		undead = create_mobile( get_mob_index(MOB_VNUM_UNDEAD) );
		char_to_room( undead, ch->in_room );
		for ( i = 0; i < MAX_STATS; i++ )
		{
			undead->perm_stat[i] = UMIN( 25, 2 * ch->perm_stat[i] );
		}

		undead->level = obj->level;
		undead->max_point[LIFE] = ( undead->level < 30 ) ? ( undead->level * 30 ) : ( undead->level < 60 ) ? ( undead->level * 60 ) : ( undead->level * 90 );
		undead->point[LIFE] = undead->max_point[LIFE];
		undead->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
		undead->point[MANA] = undead->max_point[MANA];
		undead->alignment = ch->alignment;

		for ( i = 0; i < 3; i++ )
			undead->armor[i] = interpolate( undead->level, 100, -100 );
		undead->armor[3] = interpolate( undead->level, 50, -200 );
		undead->damage[DICE_NUMBER] = number_range( undead->level / 20, undead->level / 15 );
		undead->damage[DICE_TYPE] = number_range( undead->level / 6, undead->level / 3 );
		undead->damage[DICE_BONUS] = number_range( undead->level / 12, undead->level / 10 );
		undead->sex = ch->sex;
		undead->gold = 0;

		SET_BIT( undead->act, ACT_UNDEAD );
		SET_BIT( undead->affected_by, AFF_CHARM );
		undead->master = ch;
		undead->leader = ch;

		sprintf( buf, "%s corpo rianimato", obj->name );
		undead->name = str_dup( buf );
		sprintf( buf2, "%s", obj->short_descr );
		argument = alloc_perm( MSL );
		arg = alloc_perm( MSL );
		argument = buf2;
		buf3[0] = '\0';
		while ( argument[0] != '\0' )
		{
			argument = one_argument( argument, arg );
			if ( !(!str_cmp(arg, "Il") || !str_cmp(arg, "corpo") || !str_cmp(arg, "rianimato")
			|| !str_cmp(arg, "rianimato") || !str_cmp(arg, "di")) )
			{
				if ( buf3[0] == '\0' )
					strcat( buf3, arg );
				else
				{
					strcat( buf3, " " );
					strcat( buf3, arg );
				}
			}
		}

		sprintf( buf, "Il corpo mummificato di %s", buf3 );
		undead->short_descr = str_dup( buf );
		sprintf( buf, "Un cadavere mummificato barcolla qui intorno.\n\r" );
		undead->long_descr = str_dup( buf );

		for ( obj2 = obj->contains; obj2; obj2 = next )
		{
			next = obj2->next_content;
			obj_from_obj( obj2 );
			obj_to_char( obj2, undead );
		}

		interpret( undead, "wear all", TRUE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = ch->level;
		af.duration = ( ch->level / 10 );
		af.modifier = 0;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		affect_to_char( ch, &af );

		send_to_char( "Richiami con un'antica litania energia nel cadavere, mummificandolo e dandogli vita!\n\r", ch );
		sprintf( buf, "Con un'antica litania %s rida' vita al %s!", ch->name, obj->name );
		act( buf, ch, NULL, NULL, TO_ROOM, POS_REST );
		sprintf( buf, "%s e' pront$x a servirti!", obj->short_descr );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
		extract_obj( obj );
		return;
	}

	victim = (CHAR_DATA *) vo;

	if ( victim == ch )
	{
		send_to_char( "Non sembri un cadavere.\n\r", ch );
		return;
	}

	send_to_char( "Non sembra un cadavere..\n\r", ch );
}


void spell_soul_bind( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->pet != NULL )
	{
		send_to_char( "La tua anima e' gia legata altrove.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) || !(IS_AFFECTED(victim, AFF_CHARM) && victim->master == ch) )
	{
		send_to_char( "Non puoi legare quell'anima a te.\n\r", ch );
		return;
	}

	victim->leader = ch;
	ch->pet = victim;

	act( "Leghi a te l'anima di $N!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n lega a se' l'anima di $N!", ch, NULL, victim, TO_ROOM, POS_REST );
}


void spell_forcecage( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, ADET_PROTECTOR) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 6;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ADET_PROTECTOR;
	affect_to_char( victim, &af );
	act( "$n richiama antichi poteri per generare intorno a se' una gabbia di forza.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Richiami antichi poteri generando attorno a te una gabbia di forza.\n\r", victim );
}


void spell_iron_body( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, ADET_PROTECTOR) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 6;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ADET_PROTECTOR;
	affect_to_char( victim, &af );
	act( "La pelle di $n assume la consistenza del ferro!", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "La tua pelle assume la consistenza del ferro!\n\r", victim );
}



void spell_elemental_sphere( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, ADET_PROTECTOR) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di quest'incantesimo.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di quest'incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 6;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ADET_PROTECTOR;
	affect_to_char( victim, &af );

act( "Una barriera di varie forze elementali avvolge $n in una sfera protettiva.", victim, NULL, NULL, TO_ROOM, POS_REST );
send_to_char( "Una sfera protettiva di natura elementale ti circonda.\n\r", victim );
}
