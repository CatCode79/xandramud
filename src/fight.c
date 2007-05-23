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
#include <unistd.h>

#include "xandra.h"
#include "interp.h"
#include "magic.h"
#include "file.h"


#define MAX_DAMAGE_MESSAGE	40


/*
 * Local functions.
 */
void check_assist			( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_dodge			( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_parry			( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_block			( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_blink			( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_hand				( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_cross			( CHAR_DATA * ch, CHAR_DATA * victim );
void dam_message			( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, bool immune, int dam_type );
void death_cry				( CHAR_DATA * ch, int part );
void group_gain				( CHAR_DATA * ch, CHAR_DATA * victim );
int	 xp_compute				( CHAR_DATA * gch, CHAR_DATA * victim, int tot_levels, int members );
void make_corpse			( CHAR_DATA * ch );
void mob_hit				( CHAR_DATA * ch, CHAR_DATA * victim, int dt );


/*
 * Esterne.
 */
bool	CLEVEL_OK	( CHAR_DATA *ch, int skill );


/*
 * Control the fights going on. Called periodically by update_handler.
 */
void violence_update( void )
{
	CHAR_DATA	*ch;
	CHAR_DATA	*ch_next;
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~*/

	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next = ch->next;

		if ( (victim = ch->fighting) == NULL || ch->in_room == NULL )
			continue;

		if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
			multi_hit( ch, victim, TYPE_UNDEFINED );
		else
			stop_fighting( ch, FALSE );

		if ( (victim = ch->fighting) == NULL )
			continue;

		if ( !IS_MOB(victim) )
			ch->last_fought = victim;

/*		More efficient DOPPLEGANGER
		if ( !IS_MOB(victim) )
			ch->last_fought = (is_affected(victim,gsn_doppelganger) && victim->doppel ) ?
				victim->doppel : victim;
*/
		ch->last_fight_time = current_time;

		for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
		{
			if ( IS_SET(obj->progtypes, OPROG_FIGHT) )
			{
				if ( ch->fighting == NULL )
					break;	/* previously death victims! */
				( obj->pIndexData->oprogs->fight_prog ) ( obj, ch );
			}
		}

		if ( (victim = ch->fighting) == NULL )
			continue;		/* death victim */

		if ( IS_SET(ch->progtypes, MPROG_FIGHT) && (ch->wait <= 0) )
			( ch->pIndexData->mprogs->fight_prog ) ( ch, victim );

		/* Fun for the whole family! */
		check_assist( ch, victim );
	}
}


/*
 * for auto assisting
 */
void check_assist( CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA	*rch, *rch_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
	{
		rch_next = rch->next_in_room;

		if ( IS_AWAKE(rch) && rch->fighting == NULL )
		{
			/* quick check for ASSIST_PLAYER */
			if ( !IS_MOB(ch) && IS_MOB(rch) && IS_SET(rch->off_flags, ASSIST_PLAYERS) && rch->level + 6 > victim->level )
			{
				do_emote( rch, "urla ed attacca!!" );
				multi_hit( rch, victim, TYPE_UNDEFINED );
				continue;
			}

			/* PCs next */
			if ( !IS_MOB(rch) || IS_AFFECTED(rch, AFF_CHARM) )
			{
				if ( ((!IS_MOB(rch) && IS_SET(rch->act, PLR_AUTOASSIST)) || IS_AFFECTED(rch, AFF_CHARM)) && is_same_group(ch, rch) )
					multi_hit( rch, victim, TYPE_UNDEFINED );

				continue;
			}

			if ( !IS_MOB(ch) && RIDDEN(rch) == ch )
			{
				multi_hit( rch, victim, TYPE_UNDEFINED );
				continue;
			}

			/* now check the NPC cases */
			if ( IS_MOB(ch) )
			{
				if ((IS_MOB(rch) && IS_SET(rch->off_flags, ASSIST_ALL))
				||	(IS_MOB(rch) && rch->race == ch->race && IS_SET(rch->off_flags, ASSIST_RACE))
				||	(IS_MOB(rch) &&	IS_SET(rch->off_flags, ASSIST_ALIGN)
					&&	((IS_GOOD(rch) && IS_GOOD(ch)) || (IS_EVIL(rch) && IS_EVIL(ch)) || (IS_NEUTRAL(rch) && IS_NEUTRAL(ch))))
				||	(rch->pIndexData == ch->pIndexData && IS_SET(rch->off_flags, ASSIST_VNUM)) )
				{
					CHAR_DATA	*vch;
					CHAR_DATA	*target;
					int			number;
					/*~~~~~~~~~~~~~~~~*/

					if ( number_bits(1) == 0 )
						continue;

					target = NULL;
					number = 0;

					for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
					{
						if ( can_see(rch, vch) && is_same_group(vch, victim) && number_range(0, number) == 0 )
						{
							target = vch;
							number++;
						}
					}

					if ( target != NULL )
					{

						do_emote( rch, "urla ed attacca!!" );
						multi_hit( rch, target, TYPE_UNDEFINED );
					}
				}
			}
		}
	}
}


/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	int chance;
	/*~~~~~~~*/

	/* decrement the wait */
	if ( ch->desc == NULL )
		ch->wait = UMAX( 0, ch->wait - PULSE_VIOLENCE );

	/* no attacks for stunnies -- just a check */
	if ( ch->position < POS_REST )
		return;

	/* ridden's adjustment */
	if ( RIDDEN(victim) && !IS_MOB(victim->mount) )
	{
		if ( victim->mount->fighting == NULL || victim->mount->fighting == ch )
			victim = victim->mount;
		else
			do_dismount( victim->mount, "" );
	}

	/* no attacks on ghosts or attacks by ghosts */
	if ( (!IS_MOB(victim) && IS_SET(victim->act, PLR_GHOST)) || (!IS_MOB(ch) && IS_SET(ch->act, PLR_GHOST)) )
		return;

	if ( IS_AFFECTED(ch, AFF_WEAK_STUN) )
	{
		act( "$CSei troppo stordit$x per rispondere all'attacco di $N.$c",
			ch, NULL, victim, TO_CHAR, POS_FIGHT, WHITE );
		act( "$C$n e' troppo stordit$x per rispondere al tuo attacco.$c",
			ch, NULL, victim, TO_VICT, POS_FIGHT, WHITE );
		act( "$C$n sembra stordit$x.$c", ch, NULL, victim, TO_NOTVICT, POS_FIGHT, WHITE );
		REMOVE_BIT( ch->affected_by, AFF_WEAK_STUN );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_STUN) )
	{
		act( "$CSei troppo stordit$x per rispondere all'attacco di $N.$c",
			ch, NULL, victim, TO_CHAR, POS_FIGHT, WHITE );
		act( "$C$n e' troppo stordit$x per rispondere al tuo attacco.$c", ch,
			NULL, victim, TO_VICT, POS_FIGHT, WHITE );
		act( "$C$n sembra stordit$x.$c", ch, NULL, victim, TO_NOTVICT, POS_FIGHT, WHITE );
		affect_strip( ch, gsn_power_stun );
		SET_BIT( ch->affected_by, AFF_WEAK_STUN );
		return;
	}

	if ( IS_MOB(ch) )
	{
		mob_hit( ch, victim, dt );
		return;
	}

	one_hit( ch, victim, dt, FALSE );

	if ( ch->fighting != victim )
		return;

	if ( CLEVEL_OK(ch, gsn_area_attack) && number_percent() < get_skill(ch, gsn_area_attack) )
	{
		int			count = 0, max_count;
		CHAR_DATA	*vch, *vch_next;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		check_improve( ch, gsn_area_attack, TRUE, 6 );

		if		( ch->level < 70 )		max_count = 1;
		else if ( ch->level < 80 )		max_count = 2;
		else if ( ch->level < 90 )		max_count = 3;
		else							max_count = 4;

		for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next_in_room;
			if ( (vch != victim && vch->fighting == ch) )
			{
				one_hit( ch, vch, dt, FALSE );
				count++;
			}

			if ( count == max_count )
				break;
		}
	}

	if ( IS_AFFECTED(ch, AFF_HASTE) )
		one_hit( ch, victim, dt, FALSE );

	if (ch->fighting != victim
	||	dt == gsn_backstab
	||	dt == gsn_cleave
	||	dt == gsn_ambush
	||	dt == gsn_dual_backstab
	||	dt == gsn_circle
	||	dt == gsn_assassinate
	||	dt == gsn_vampiric_bite )
		return;

	chance = get_skill( ch, gsn_second_attack ) / 2;
	if ( number_percent() < chance )
	{
		one_hit( ch, victim, dt, FALSE );
		check_improve( ch, gsn_second_attack, TRUE, 5 );
		if ( ch->fighting != victim )
			return;
	}

	chance = get_skill( ch, gsn_third_attack ) / 3;
	if ( number_percent() < chance )
	{
		one_hit( ch, victim, dt, FALSE );
		check_improve( ch, gsn_third_attack, TRUE, 6 );
		if ( ch->fighting != victim )
			return;
	}

	chance = get_skill( ch, gsn_fourth_attack ) / 5;
	if ( number_percent() < chance )
	{
		one_hit( ch, victim, dt, FALSE );
		check_improve( ch, gsn_fourth_attack, TRUE, 7 );
		if ( ch->fighting != victim )
			return;
	}

	chance = get_skill( ch, gsn_fifth_attack ) / 6;
	if ( number_percent() < chance )
	{
		one_hit( ch, victim, dt, FALSE );
		check_improve( ch, gsn_fifth_attack, TRUE, 8 );
		if ( ch->fighting != victim )
			return;
	}

	chance = 20 + ( get_skill(ch, gsn_second_weapon) * 0.8 );
	if ( number_percent() < chance )
	{
		if ( get_wield_char(ch, TRUE) )
		{
			one_hit( ch, victim, dt, TRUE );
			check_improve( ch, gsn_second_weapon, TRUE, 2 );
			if ( ch->fighting != victim )
				return;
		}
	}

	chance = get_skill( ch, gsn_secondary_attack ) / 4;
	if ( number_percent() < chance )
	{
		if ( get_wield_char(ch, TRUE) )
		{
			one_hit( ch, victim, dt, TRUE );
			check_improve( ch, gsn_secondary_attack, TRUE, 2 );
			if ( ch->fighting != victim )
				return;
		}
	}
}


/*
 * procedure for all mobile attacks
 */
void mob_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	int			chance, number;
	CHAR_DATA	*vch, *vch_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* no attacks on ghosts */
	if ( !IS_MOB(victim) && IS_SET(victim->act, PLR_GHOST) )
		return;

	/* no attack by ridden mobiles except spec_casts */
	if ( RIDDEN(ch) )
	{
		if ( ch->fighting != victim )
			set_fighting( ch, victim );
		return;
	}

	one_hit( ch, victim, dt, FALSE );

	if ( ch->fighting != victim )
		return;

	/* Area attack -- BALLS nasty! */
	if ( IS_SET(ch->off_flags, OFF_AREA_ATTACK) )
	{
		for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )

		{
			vch_next = vch->next_in_room;
			if ( (vch != victim && vch->fighting == ch) )
				one_hit( ch, vch, dt, FALSE );
		}
	}

	if ( IS_AFFECTED(ch, AFF_HASTE) || IS_SET(ch->off_flags, OFF_FAST) )
		one_hit( ch, victim, dt, FALSE );

	if (ch->fighting != victim
	||	dt == gsn_backstab
	||	dt == gsn_circle
	||	dt == gsn_dual_backstab
	||	dt == gsn_cleave
	||	dt == gsn_ambush
	||	dt == gsn_vampiric_bite )
		return;

	chance = get_skill( ch, gsn_second_attack ) / 2;
	if ( number_percent() < chance )
	{
		one_hit( ch, victim, dt, FALSE );
		if ( ch->fighting != victim )
			return;
	}

	chance = get_skill( ch, gsn_third_attack ) / 4;
	if ( number_percent() < chance )
	{
		one_hit( ch, victim, dt, FALSE );
		if ( ch->fighting != victim )
			return;
	}

	chance = get_skill( ch, gsn_fourth_attack ) / 6;
	if ( number_percent() < chance )
	{
		one_hit( ch, victim, dt, FALSE );
		if ( ch->fighting != victim )
			return;
	}

	chance = get_skill( ch, gsn_second_weapon ) / 2;
	if ( number_percent() < chance )
	{
		if ( get_wield_char(ch, TRUE) )
		{
			one_hit( ch, victim, dt, TRUE );
			if ( ch->fighting != victim )
				return;
		}
	}

	if ( IS_SET(ch->act, ACT_MAGE) )
	{
		if ( number_percent() < 60 && ch->spec_fun == NULL )
		{
			mob_cast_mage( ch, victim );
			return;
		}
	}

	if ( IS_SET(ch->act, ACT_CLERIC) )
	{
		if ( number_percent() < 60 && ch->spec_fun == NULL )
		{
			mob_cast_cleric( ch, victim );
			return;
		}
	}

	/* PC waits */
	if ( ch->wait > 0 )
		return;

	/* now for the skills */
	number = number_range( 0, 7 );

	switch ( number )
	{
	case ( 0 ):
		if ( IS_SET(ch->off_flags, OFF_BASH) )
			do_bash( ch, "" );
		break;

	case ( 1 ):
		if ( IS_SET(ch->off_flags, OFF_BERSERK) && !IS_AFFECTED(ch, AFF_BERSERK) )
			do_berserk( ch, "" );
		break;

	case ( 2 ):
		if (IS_SET(ch->off_flags, OFF_DISARM)
		||	(get_weapon_sn(ch, FALSE) != gsn_hand_to_hand && (IS_SET(ch->act, ACT_WARRIOR) || IS_SET(ch->act, ACT_THIEF))) )
			do_disarm( ch, "" );
		break;

	case ( 3 ):
		if ( IS_SET(ch->off_flags, OFF_KICK) )
			do_kick( ch, "" );
		break;

	case ( 4 ):
		if ( IS_SET(ch->off_flags, OFF_KICK_DIRT) )
			do_dirt( ch, "" );
		break;

	case ( 5 ):
		if ( IS_SET(ch->off_flags, OFF_TAIL) )
			do_tail( ch, "" );
		break;

	case ( 6 ):
		if ( IS_SET(ch->off_flags, OFF_TRIP) )
			do_trip( ch, "" );
		break;

	case ( 7 ):
		if ( IS_SET(ch->off_flags, OFF_CRUSH) )
			do_crush( ch, "" );
		break;
	}
}


/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary )
{
	OBJ_DATA	*wield;
	int			victim_ac;
	int			thac0;
	int			thac0_00;
	int			thac0_32;
	int			dam;
	int			diceroll;
	int			sn, skill;
	int			dam_type;
	bool		counter;
	bool		result;
	OBJ_DATA	*corpse;
	int			sercount;
	int			slay_mult = 1;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	sn = -1;
	counter = FALSE;

	/* just in case */
	if ( victim == ch || ch == NULL || victim == NULL )
		return;

	/* ghosts can't fight */
	if ( (!IS_MOB(victim) && IS_SET(victim->act, PLR_GHOST)) || (!IS_MOB(ch) && IS_SET(ch->act, PLR_GHOST)) )
		return;

	/* Can't beat a dead char! Guard against weird room-leavings. */
	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	/* Figure out the type of damage message. */
	wield = get_wield_char( ch, secondary );

	/* if there is no weapon held by pro-hand, and there is a weapon in the other hand, than don't fight with punch. */
	if ( !secondary && dt == TYPE_UNDEFINED && wield == NULL && get_wield_char(ch, TRUE) )
	{
		if ( ch->fighting != victim )
		{
			secondary = TRUE;
			wield = get_wield_char( ch, secondary );
		}
		else
			return;
	}

	/* Calcolo del moltiplicatore slay */
	if ( wield != NULL )
	{
		if ( IS_OBJ_STAT(wield, ITEM_SLAYGOOD) )
		{
			if ( IS_GOOD(victim) && !IS_GOOD(ch) )
				slay_mult = 1,2;
		}

		if ( IS_OBJ_STAT(wield, ITEM_SLAYEVIL) )
		{
			if ( IS_EVIL(victim) && !IS_EVIL(ch) )
				slay_mult = 1,2;
		}

		if ( IS_OBJ_STAT(wield, ITEM_SLAYNEUTRAL) )
		{
			if ( IS_NEUTRAL(victim) && !IS_NEUTRAL(ch) )
				slay_mult = 1.2;
		}
	}

	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON )
			dt += wield->value[3];
		else
			dt += ch->dam_type;
	}

	if ( dt < TYPE_HIT )
	{
		if ( wield != NULL )
			dam_type = attack_table[wield->value[3]].damage;
		else
			dam_type = attack_table[ch->dam_type].damage;
	}
	else
		dam_type = attack_table[dt - TYPE_HIT].damage;

	if ( dam_type == -1 )
		dam_type = DAM_BASH;

	/* get the weapon skill */
	sn = get_weapon_sn( ch, secondary );
	skill = 20 + get_weapon_skill( ch, sn );

	/* Calculate to-hit-armor-class-0 versus armor. */
	if ( IS_MOB(ch) )
	{
		thac0_00 = 20;
		thac0_32 = -4;	/* as good as a thief */
		if		( IS_SET(ch->act, ACT_WARRIOR) )	thac0_32 = -10;
		else if ( IS_SET(ch->act, ACT_THIEF) )		thac0_32 = -4;
		else if ( IS_SET(ch->act, ACT_CLERIC) )		thac0_32 = 2;
		else if ( IS_SET(ch->act, ACT_MAGE) )		thac0_32 = 6;
	}
	else
	{
		thac0_00 = class_table[ch->class].thac0_00;
		thac0_32 = class_table[ch->class].thac0_32;
	}

	thac0 = interpolate( ch->level, thac0_00, thac0_32 );

	if ( thac0 < 0 )
		thac0 = thac0 / 2;

	if ( thac0 < -5 )
		thac0 = -5 + ( thac0 + 5 ) / 2;

	thac0 -= GET_HITROLL( ch ) * skill / 100;
	thac0 += 5 * ( 100 - skill ) / 100;

	if ( dt == gsn_backstab )
		thac0 -= 10 * ( 100 - get_skill(ch, gsn_backstab) );

	if ( dt == gsn_dual_backstab )
		thac0 -= 10 * ( 100 - get_skill(ch, gsn_dual_backstab) );

	if ( dt == gsn_cleave )
		thac0 -= 10 * ( 100 - get_skill(ch, gsn_cleave) );

	if ( dt == gsn_ambush )
		thac0 -= 10 * ( 100 - get_skill(ch, gsn_ambush) );

	if ( dt == gsn_vampiric_bite )
		thac0 -= 10 * ( 100 - get_skill(ch, gsn_vampiric_bite) );

	switch ( dam_type )
	{
	case ( DAM_PIERCE ):	victim_ac = GET_AC( victim, AC_PIERCE ) / 10; break;
	case ( DAM_BASH ):		victim_ac = GET_AC( victim, AC_BASH ) / 10; break;
	case ( DAM_SLASH ):		victim_ac = GET_AC( victim, AC_SLASH ) / 10; break;
	default:				victim_ac = GET_AC( victim, AC_EXOTIC ) / 10; break;
	};

	if ( victim_ac < -15 )
		victim_ac = ( victim_ac + 15 ) / 5 - 15;

	if ( get_skill(victim, gsn_armor_use) > 70 )
	{
		check_improve( victim, gsn_armor_use, TRUE, 8 );
		victim_ac -= ( victim->level ) / 2;
	}

	if ( !can_see(ch, victim) )
	{
		if ( !skill_failure_nomessage(ch, gsn_blind_fighting, 0)
		&&	number_percent() < get_skill(ch, gsn_blind_fighting) )
		{
			check_improve( ch, gsn_blind_fighting, TRUE, 16 );
		}
		else
			victim_ac -= 4;
	}

	if ( victim->position < POS_FIGHT )
		victim_ac += 4;

	if ( victim->position < POS_REST )
		victim_ac += 6;

	/* The moment of excitement! */
	while ( (diceroll = number_bits(5)) >= 20 )
		;

	if ( diceroll == 0 || (diceroll != 19 && diceroll < thac0 - victim_ac) )
	{
		/* Miss. */
		damage( ch, victim, 0, dt, dam_type, TRUE );
		tail_chain();
		return;
	}

	/* Hit. Calc damage. */
	if ( IS_MOB(ch) && (!ch->pIndexData->new_format || wield == NULL) )
	{
		if ( !ch->pIndexData->new_format )
		{
			dam = number_range( ch->level / 2, ch->level * 3 / 2 );
			if ( wield != NULL )
				dam += dam / 2;
		}
		else
			dam = dice( ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE] );
	}
	else
	{
		if ( sn != -1 )
			check_improve( ch, sn, TRUE, 5 );
		if ( wield != NULL )
		{
			if ( wield->pIndexData->new_format )
				dam = dice( wield->value[1], wield->value[2] ) * skill / 100;
			else
				dam = number_range( wield->value[1] * skill / 100, wield->value[2] * skill / 100 );

			if ( get_shield_char(ch) == NULL )	/* no shield = more */
				dam = dam * 21 / 20;

			/* sharpness! */
			if ( IS_WEAPON_STAT(wield, WEAPON_SHARP) )
			{
				int percent;
				/*~~~~~~~~*/

				if ( (percent = number_percent()) <= (skill / 8) )
					dam = 2 * dam + ( dam * 2 * percent / 100 );
			}

			/* holy weapon */
			if ( IS_WEAPON_STAT(wield, WEAPON_HOLY) && IS_GOOD(ch) && IS_EVIL(victim) && number_percent() < 30 )
			{
				act( "$C$p slende di un'aura sacra.$c", ch, wield, NULL, TO_CHAR, POS_DEAD, YELLOW );
				act( "$C$p slende di un'aura sacra.$c", ch, wield, NULL, TO_ROOM, POS_DEAD, YELLOW );
				dam += dam * 120 / 100;
			}
		}
		else
		{
			if ( CLEVEL_OK(ch, gsn_hand_to_hand) )
			{
				if ( number_percent() < get_skill(ch, gsn_hand_to_hand) )
					dam = number_range( 4 + ch->level / 10, 2 * ch->level / 3 ) * skill / 100;
				else
				{
					dam = number_range( 5, ch->level / 2 ) * skill / 100;
					check_improve( ch, gsn_hand_to_hand, FALSE, 5 );
				}
			}
			else
				dam = number_range( 5, ch->level / 2 ) * skill / 100;

			if ( get_skill(ch, gsn_master_hand) > 0 )
			{
				int d;
				/*~~*/

				if ( (d = number_percent()) <= get_skill(ch, gsn_master_hand) )
				{
					check_improve( ch, gsn_master_hand, TRUE, 6 );
					dam *= 2;
					if ( d < 10 )
					{
						SET_BIT( victim->affected_by, AFF_WEAK_STUN );
						act( "$CColpisci $N con forza stordente!$c", ch, NULL, victim, TO_CHAR, POS_DEAD, RED );
						act( "$C$n ti colpisce con forza stordente!$c", ch, NULL, victim, TO_VICT, POS_DEAD, RED );
						act( "$C$n colpisce $N con forza stordente!$c", ch, NULL, victim, TO_NOTVICT, POS_DEAD, RED );
						check_improve( ch, gsn_master_hand, TRUE, 6 );
					}
				}
			}
		}
	}

	/* Bonuses. */
	if ( get_skill(ch, gsn_enhanced_damage) > 0 )
	{
		diceroll = number_percent();
		if ( diceroll <= get_skill(ch, gsn_enhanced_damage) )
		{
			int div;
			/*~~~~*/

			check_improve( ch, gsn_enhanced_damage, TRUE, 6 );
			div = ( ch->class == CLASS_WARRIOR ) ? 100 : ( ch->class == CLASS_CLERIC ) ? 130 : 114;
			dam += dam * diceroll / div;
		}
	}

	if ( get_skill(ch, gsn_master_sword) > 0 && sn == gsn_sword )
	{
		if ( number_percent() <= get_skill(ch, gsn_master_sword) )
		{
			OBJ_DATA	*katana;
			/*~~~~~~~~~~~~~~~~*/

			check_improve( ch, gsn_master_sword, TRUE, 6 );
			dam += dam * 110 / 100;

			if ( (katana = get_wield_char(ch, FALSE)) != NULL )
			{
				AFFECT_DATA *paf;
				/*~~~~~~~~~~~~~*/

				if ( IS_WEAPON_STAT(katana, WEAPON_KATANA) && strstr(katana->extra_descr->description, ch->name) != NULL )
				{
					katana->cost++;
					if ( katana->cost > 249 )
					{
						paf = affect_find( katana->affected, gsn_katana );
						if ( paf != NULL )
						{
							int old_mod = paf->modifier;
							/*~~~~~~~~~~~~~~~~~~~~~~~~*/

							paf->modifier = UMIN( (paf->modifier + 1), (ch->level / 3) );
							ch->hitroll += paf->modifier - old_mod;
							if ( paf->next != NULL )
							{
								paf->next->modifier = paf->modifier;
								ch->damroll += paf->modifier - old_mod;
							}

							act( "La katana di $n risplende di luce blu.", ch, NULL, NULL, TO_ROOM, POS_REST );
							send_to_char( "La tua katana risplende di luce blu.\n\r", ch );
						}

						katana->cost = 0;
					}
				}
			}
			else if ( (katana = get_wield_char(ch, TRUE)) != NULL )
			{
				AFFECT_DATA *paf;
				/*~~~~~~~~~~~~~*/

				if ( IS_WEAPON_STAT(katana, WEAPON_KATANA) && strstr(katana->extra_descr->description, ch->name) != NULL )
				{
					katana->cost++;
					if ( katana->cost > 249 )
					{
						paf = affect_find( katana->affected, gsn_katana );
						if ( paf != NULL )
						{
							int old_mod = paf->modifier;
							/*~~~~~~~~~~~~~~~~~~~~~~~~*/

							paf->modifier = UMIN( (paf->modifier + 1), (ch->level / 3) );
							ch->hitroll += paf->modifier - old_mod;
							if ( paf->next != NULL )
							{
								paf->next->modifier = paf->modifier;
								ch->damroll += paf->modifier - old_mod;
							}

							act( "La katana di $n risplende di luce blu.", ch, NULL, NULL, TO_ROOM, POS_REST );
							send_to_char( "La tua katana risplende di luce blu.\n\r", ch );
						}

						katana->cost = 0;
					}
				}
			}
		}
	}

	if ( !IS_AWAKE(victim) )
		dam *= 2;
	else if ( victim->position < POS_FIGHT )
		dam = dam * 3 / 2;

	sercount = number_percent();

	if ( dt == gsn_backstab || dt == gsn_vampiric_bite )
		sercount += 40; /* 80% chance decrease of counter */

	if ( victim->last_fight_time != -1 && (current_time - victim->last_fight_time) < FIGHT_DELAY_TIME )
		sercount += 25; /* 50% chance decrease of counter */

	sercount *= 2;

	if (victim->fighting == NULL
	&&	!IS_MOB(victim)
	&&	!is_safe_nomessage(victim, ch)
	&&	!is_safe_nomessage(ch, victim)
	&&	(victim->position == POS_SIT || victim->position == POS_STAND)
	&&	dt != gsn_assassinate
	&&	(sercount <= get_skill(victim, gsn_counter)) )
	{
		counter = TRUE;
		check_improve( victim, gsn_counter, TRUE, 1 );
		act( "$N respinge il tuo attacco contro di te!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "Respingi violentemente l'attacco di $n contro $e stess$x!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$N respinge violentemente l'attacco di $n contro $e stess$x!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		ch->fighting = victim;
	}
	else if ( !victim->fighting )
		check_improve( victim, gsn_counter, FALSE, 1 );

	if		( dt == gsn_backstab && wield != NULL )			dam = ( 1 + ch->level / 10 ) * dam + ch->level;
	else if ( dt == gsn_dual_backstab && wield != NULL )	dam = ( 1 + ch->level / 14 ) * dam + ch->level;
	else if ( dt == gsn_circle )							dam = ( ch->level / 40 + 1 ) * dam + ch->level;
	else if ( dt == gsn_vampiric_bite && IS_VAMPIRE(ch) )	dam = ( ch->level / 20 + 1 ) * dam + ch->level;
	else if ( dt == gsn_cleave && wield != NULL )
	{
		if ( number_percent() < URANGE(4, 5 + (ch->level - victim->level), 10) && !counter )
		{
			act( "Con un colpo secco spacchi $N $Ca meta'!$c", ch, NULL, victim, TO_CHAR, POS_REST, RED );
			act( "$n con un colpo secco ti spacca $Ca meta'!$c", ch, NULL, victim, TO_VICT, POS_REST, RED );
			act( "$n con un colpo secco spacca $N $Ca meta'!$c", ch, NULL, victim, TO_NOTVICT, POS_REST, RED );
			act( "Sei stato uccis$x!", victim, NULL, NULL, TO_CHAR, POS_REST );
			act( "$n e' mort$x!", victim, NULL, NULL, TO_ROOM, POS_REST );

			WAIT_STATE( ch, 2 );
			raw_kill( victim, -1 );
			if ( !IS_MOB(ch) && IS_MOB(victim) )
			{
				corpse = get_obj_list( ch, "corpo", ch->in_room->contents );

				if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains )	/* exists and not empty */
					do_get( ch, "all corpo" );

				if (IS_SET(ch->act, PLR_AUTOGOLD)
				&&	corpse
				&&	corpse->contains				/* exists and not empty */
				&&	!IS_SET(ch->act, PLR_AUTOLOOT) )
				{
					do_get( ch, "oro corpo" );
					do_get( ch, "argento corpo" );
					do_get( ch, "monete corpo" );
				}

				if ( IS_SET(ch->act, PLR_AUTOSAC) )
				{
					if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains )
						return;						/* leave if corpse has treasure */
					else
						do_sacrifice( ch, "corpo" );
				}
			}

			return;
		}
		else
			dam = ( dam * 2 + ch->level );
	}

	if ( dt == gsn_assassinate )
	{
		if ( number_percent() <= URANGE(10, 20 + (ch->level - victim->level) * 2, 50) && !counter )
		{
			act( "Hai $C+++ASSASSINATO+++$c $N!", ch, NULL, victim, TO_CHAR, POS_REST, RED );
			act( "$N e' mort$X!!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n ha $C+++ASSASSINATO+++$c $N!", ch, NULL, victim, TO_NOTVICT, POS_REST, RED );
			act( "$N e' mort$X!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			act( "$n ti ha $C++ASSASSINATO++$c!", ch, NULL, victim, TO_VICT, POS_DEAD, RED );

			act( "Sei stat$x uccis$x!", victim, NULL, NULL, TO_CHAR, POS_DEAD );
			check_improve( ch, gsn_assassinate, TRUE, 1 );
			raw_kill( victim, -1 );

			if ( !IS_MOB(ch) && IS_MOB(victim) )
			{
				corpse = get_obj_list( ch, "corpo", ch->in_room->contents );

				if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains )	/* exists and not empty */
					do_get( ch, "all corpo" );

				if (IS_SET(ch->act, PLR_AUTOGOLD)
				&&	corpse
				&&	corpse->contains			/* exists and not empty */
				&&	!IS_SET(ch->act, PLR_AUTOLOOT) )
				{
					do_get( ch, "oro corpo" );
					do_get( ch, "argento corpo" );
					do_get( ch, "moente corpo" );
				}

				if ( IS_SET(ch->act, PLR_AUTOSAC) )
				{
					if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains )
						return;					/* leave if corpse has treasure */
					else
						do_sacrifice( ch, "corpo" );
				}
			}

			return;
		}
		else
		{
			check_improve( ch, gsn_assassinate, FALSE, 1 );
			dam *= 2;
		}
	}

	dam += GET_DAMROLL( ch ) * UMIN( 100, skill ) / 100;

	if ( dt == gsn_ambush )
		dam *= 3;

	if ( !skill_failure_nomessage(ch, gsn_deathblow, 0)
	&& get_skill(ch,gsn_deathblow) > 1 )
	{
		if ( number_percent() < 0.125 * get_skill(ch, gsn_deathblow) )
		{
			act( "Sferri un colpo mortale!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "$n sferra un colpo mortale!", ch, NULL, NULL, TO_ROOM, POS_REST );
			if ( cabal_ok(ch, gsn_deathblow) )
			{
				dam *= ( (float) ch->level ) / 20;
				check_improve( ch, gsn_deathblow, TRUE, 1 );
			}
		}
		else
			check_improve( ch, gsn_deathblow, FALSE, 3 );
	}

	if ( dam <= 0 )
		dam = 1;

	if ( counter )
	{
		result = damage( ch, ch, slay_mult* 2 * dam, dt, dam_type, TRUE );
		multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	else
		result = damage( ch, victim, slay_mult * dam, dt, dam_type, TRUE );

	/* vampiric bite gives hp to ch from victim */
	if ( dt == gsn_vampiric_bite )
	{
		int hit_ga = UMIN( (dam / 2), victim->max_point[LIFE] );
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ch->point[LIFE] += hit_ga;
		ch->point[LIFE] = UMIN( ch->point[LIFE], ch->max_point[LIFE] );
		update_pos( ch );
		send_to_char( "Delizioso spuntino.. Rivitalizzante..\n\r", ch );
	}

	/* but do we have a funky weapon? */
	if ( result && wield != NULL )
	{
		if ( ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_POISON) )
		{
			int			level;
			AFFECT_DATA *poison, af;
			/*~~~~~~~~~~~~~~~~~~~~*/


			if ( (poison = affect_find(wield->affected, gsn_poison)) == NULL )
				level = wield->level;
			else
				level = poison->level;

			if ( !saves_spell(level / 2, victim, DAM_POISON) )
			{
				send_to_char( "Senti del veleno scorrere nelle tue vene.\n\r", victim );
				act( "$n e' stat$x avvelenat$x dal veleno di $p.", victim, wield, NULL, TO_ROOM, POS_REST );

				af.where = TO_AFFECTS;
				af.type = gsn_poison;
				af.level = level * 3 / 4;
				af.duration = level / 2;
				af.location = APPLY_STR;
				af.modifier = -1;
				af.bitvector = AFF_POISON;
				affect_join( victim, &af );
			}

			/* weaken the poison if it's temporary */
			if ( poison != NULL )
			{
				poison->level = UMAX( 0, poison->level - 2 );
				poison->duration = UMAX( 0, poison->duration - 1 );
				if ( poison->level == 0 || poison->duration == 0 )
					act( "Il veleno di $p e' terminato.", ch, wield, NULL, TO_CHAR, POS_REST );
			}
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_VAMPIRIC) )
		{
			dam = number_range( 1, wield->level / 5 + 1 );
			act( "$p toglie della vita a $n.", victim, wield, NULL, TO_ROOM, POS_REST );
			act( "Senti che $p sta risucchiando la tua vita.", victim, wield, NULL, TO_CHAR, POS_REST );
			damage( ch, victim, dam, 0, DAM_NEGATIVE, FALSE );
			ch->point[LIFE] += dam / 2;
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_FLAMING) )
		{
			dam = number_range( 1, wield->level / 4 + 1 );
			act( "$n e' bruciato da $p.", victim, wield, NULL, TO_ROOM, POS_REST );
			act( "$p brucia la tua carne.", victim, wield, NULL, TO_CHAR, POS_REST );
			fire_effect( (void *) victim, wield->level / 2, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_FROST) )
		{
			dam = number_range( 1, wield->level / 6 + 2 );
			act( "$p congela $n.", victim, wield, NULL, TO_ROOM, POS_REST );
			act( "Il tocco gelido di $p ti circonda di ghiaccio.", victim, wield, NULL, TO_CHAR, POS_REST );
			cold_effect( victim, wield->level / 2, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_COLD, FALSE );
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_SHOCKING) )
		{
			dam = number_range( 1, wield->level / 5 + 2 );
			act( "$n e' stata colpita da un fulmine di $p.", victim, wield, NULL, TO_ROOM, POS_REST );
			act( "Sei fulminato da $p.", victim, wield, NULL, TO_CHAR, POS_REST );
			shock_effect( victim, wield->level / 2, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
		}
	}

	tail_chain();
}


/*
 * Inflict damage from a hit.
 */
bool damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int dam_type, bool show )
{
	OBJ_DATA	*corpse;
	bool		immune;
	int			lost_exp;
	/*~~~~~~~~~~~~~~~~~*/

	if ( victim->position == POS_DEAD )
		return( FALSE );

	/* Stop up any residual loopholes. */
	if ( dam > 1000 && !IS_ADMIN(ch) )
	{
		char	buf[MSL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		sprintf( buf, "%s:Danno maggiore di 1000 points :%d", ch->name, dam );
		bug( buf, 0 );
		if ( IS_MOB(ch) && !IS_MOB(ch) )
			dam = 1000;

/*		For a 100-leveled MUD?...

		dam = 1000;
		if ( !IS_ADMIN(ch) )
		{
			OBJ_DATA *obj;
			obj = get_wield_char( ch );
			send_to_char( "Non vorrai gabolare spero.\n\r", ch );
			if ( obj )
				extract_obj( obj );
		}
*/
	}

	if ( victim != ch )
	{
		/* Certain attacks are forbidden. Most other attacks are returned. */
		if ( is_safe(ch, victim) )
			return( FALSE );

		if ( victim->position > POS_STUN )
		{
			if ( victim->fighting == NULL )
				set_fighting( victim, ch );

			if ( victim->timer <= 4
			&& (victim->position != POS_FIGHT)
			&& (victim->position != POS_SIT)
			&& (victim->position != POS_REST) )
				victim->position = POS_FIGHT;
		}

		if ( victim->position == POS_REST )
		{
			victim->position = POS_SIT;
			victim->timer = 3;
		}

		if ( victim->position == POS_SIT && IS_MOB(victim) )
		{
			if ( victim->timer == 0 )
			{
				victim->position = POS_FIGHT;

				act( "$N si rialza da terra.", ch, NULL, victim, TO_CHAR, POS_REST );
				act( "$N si rialza da terra.", ch, NULL, victim, TO_NOTVICT, POS_REST );
			}
			else if ( victim->timer > 0 )
				victim->timer--;
		}

		if ( victim->position > POS_STUN )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );

			/* If victim is charmed, ch might attack victim's master. */
			if (IS_MOB(ch)
			&&	IS_MOB(victim)
			&&	IS_AFFECTED(victim, AFF_CHARM)
			&&	victim->master != NULL
			&&	victim->master->in_room == ch->in_room
			&&	number_bits(3) == 0 )
			{
				stop_fighting( ch, FALSE );
				multi_hit( ch, victim->master, TYPE_UNDEFINED );
				return( FALSE );
			}
		}

		/* More charm and group stuff. */
		if ( victim->master == ch )
			stop_follower( victim );

		if ( MOUNTED(victim) == ch || RIDDEN(victim) == ch )
			victim->riding = ch->riding = FALSE;
	}

	/* No one in combat can sneak, hide, or be invis or camoed. */
	if (IS_SET(ch->affected_by, AFF_HIDE)
	||	IS_SET(ch->affected_by, AFF_INVISIBLE)
	||	IS_SET(ch->affected_by, AFF_SNEAK)
	||	IS_SET(ch->affected_by, AFF_FADE)
	||	IS_SET(ch->affected_by, AFF_CAMOUFLAGE)
	||	IS_SET(ch->affected_by, AFF_IMP_INVIS)
	||	CAN_DETECT(ch, ADET_EARTHFADE) )
		do_visible( ch, "" );

	/* Damage modifiers. */
	if ( IS_AFFECTED(victim, AFF_SANCTUARY) && !((dt == gsn_cleave) && (number_percent() < 50)) )
		dam /= 2;
	else if ( CAN_DETECT(victim, ADET_PROTECTOR) )
		dam = ( 3 * dam ) / 5;

	if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch) )
		dam -= dam / 4;

	if ( IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch) )
		dam -= dam / 4;

	if ( CAN_DETECT(victim, ADET_AURA_CHAOS) && victim->cabal == CABAL_CHAOS && (!IS_MOB(victim) && IS_SET(victim->act, PLR_WANTED)) )
		dam -= dam / 4;

	if ( is_affected(victim, gsn_protection_heat) && (dam_type == DAM_FIRE) )
		dam -= dam / 4;

	if ( is_affected(victim, gsn_protection_cold) && (dam_type == DAM_COLD) )
		dam -= dam / 4;

	immune = FALSE;

	if ( dt > 0 && dt < MAX_SKILL )
	{
		if (CAN_DETECT(victim, ADET_ABSORB)
		&&	skill_table[dt].target == TAR_CHAR_OFFENSIVE
		&&	skill_table[dt].spell_fun != spell_null
		&&	ch != victim
		&&	(number_percent() < 2 * get_skill(victim, gsn_absorb) / 3)	/* update.c damages */
		&&	dt != gsn_poison
		&&	dt != gsn_plague
		&&	dt != gsn_witch_curse	/* update.c damages */
		&&	dt != gsn_mental_knife
		&&	dt != gsn_lightning_breath )
		{
			act( "Il tuo incantesimo fallisce nel passare il campo di enerigia di $N!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "Assorbi l'incantesimo di $n!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$N assorbe l'incantesimo di $n!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			check_improve( victim, gsn_absorb, TRUE, 1 );
			victim->point[MANA] += skill_table[dt].min_mana;
			return( FALSE );
		}

		if (CAN_DETECT(victim, ADET_SPELLBANE)
		&&	skill_table[dt].target != TAR_IGNORE
		&&	skill_table[dt].spell_fun != spell_null
		&&	(number_percent() < 2 * get_skill(victim, gsn_spellbane) / 3)	/* update.c damages */
		&&	dt != gsn_poison
		&&	dt != gsn_plague
		&&	dt != gsn_witch_curse	/* spellbane passing spell damages */
		&&	dt != gsn_mental_knife
		&&	dt != gsn_lightning_breath )
		{
			act( "$N riesce a deviare il tuo incantesimo!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "Riesci a deviare l'incantesimo di $n!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$N riesce a deviare l'incantesimo di $n!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			check_improve( victim, gsn_spellbane, TRUE, 1 );
			damage( victim, ch, 3 * victim->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			return( FALSE );
		}
	}

	/* Check for parry, and dodge. */
	if ( dt >= TYPE_HIT && ch != victim )
	{
		/* Some funny stuf. */
		if ( is_affected(victim, gsn_mirror) )
		{
			act( "$n si frantuma in piccoli pezzetti di vetro.", victim, NULL, NULL, TO_ROOM, POS_REST );
			extract_char( victim, TRUE );
			return( FALSE );
		}

		if ( check_parry(ch, victim) )		return( FALSE );
		if ( check_cross(ch, victim) )		return( FALSE );
		if ( check_block(ch, victim) )		return( FALSE );
		if ( check_dodge(ch, victim) )		return( FALSE );
		if ( check_hand (ch, victim) )		return( FALSE );
		if ( check_blink(ch, victim) )		return( FALSE );
	}

	switch ( check_immune(victim, dam_type) )
	{
	case ( IS_IMMUNE ):		immune = TRUE; dam = 0; break;
	case ( IS_RESISTANT ):	dam -= dam / 3; break;
	case ( IS_VULNERABLE ): dam += dam / 2; break;
	}

	if ( dt != TYPE_HUNGER && dt >= TYPE_HIT && ch != victim )
	{

		dam = critical_strike( ch, victim, dam );
		dam = ground_strike( ch, victim, dam );
	}

	if ( show )
		dam_message( ch, victim, dam, dt, immune, dam_type );

	if ( dam == 0 )
		return( FALSE );

	/* temporarily second wield doesn't inflict damage */
	if ( dt != TYPE_HUNGER && (dt >= TYPE_HIT && ch != victim) )
		check_weapon_destroy( ch, victim, FALSE );

	/* Hurt the victim. Inform the victim of his new state. make sure that negative overflow doesn't happen! */
	if ( dam < 0 || dam > (victim->point[LIFE] + 16) )
		victim->point[LIFE] = -16;
	else
	{
		if ( !IS_MOB(victim) && victim->level < LEVEL_NEWBIE )
		{
			dam -= victim->level - 1;
			if ( dam <= 0 )
				dam = 1;
			victim->point[LIFE] -= dam;
		}
		else
			victim->point[LIFE] -= dam;
	}

	if ( !IS_MOB(victim) && victim->level >= LEVEL_ADMIN && victim->point[LIFE] < 1 )
		victim->point[LIFE] = 1;

	update_pos( victim );

	switch ( victim->position )
	{
	case POS_MORTAL:
		if ( dam_type == DAM_HUNGER || dam_type == DAM_THIRST )
			break;
		act( "$n agonizza dolorosamente e morira' presto se nessuno l'aiuta.", victim, NULL, NULL, TO_ROOM, POS_REST );
		act( "Sei ferit$x mortalmente e morirai presto se nessuno ti aiuta.", victim, NULL, NULL, TO_CHAR, POS_REST );
		break;

	case POS_INCAP:
		if ( dam_type == DAM_HUNGER || dam_type == DAM_THIRST )
			break;
		act( "$n non riesce a muoversi.. morira' entro breve se nessuno l'aiuta.", victim, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Non riesci a muoverti.. continui a perdere sangue e morirai lentamente, se non ti aiutano.\n\r", victim);
		break;

	case POS_STUN:
		if ( dam_type == DAM_HUNGER || dam_type == DAM_THIRST )
			break;
		act( "$n e' in coma, non ha piu' coscienza di se' e continua a perdere sangue..",
			victim, NULL, NULL, TO_ROOM, POS_FIGHT, RED );
		send_to_char( "Svieni.. ma puoi ancora riprenderti.. se ti soccorrono..\n\r", victim );
		break;

	case POS_DEAD:
		act( "$n e' mort$x!!", victim, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "MUORI!\n\r",victim );
		send_to_char( "Senti il freddo abbraccio della morte.. il presente svanisce, il mondo gira intorno a te.\n\r\n\r",victim );

		break;

	default:
		if ( dam_type == DAM_HUNGER || dam_type == DAM_THIRST )
			break;
		if ( dam > victim->max_point[LIFE] / 4 )
			send_to_char( "Fa veramente tanto MALE!\n\r", victim );
		if ( victim->point[LIFE] < victim->max_point[LIFE] / 4 )
			send_to_char( "Stai sanguinando ovunque!\n\r", victim );
		break;
	}

	/* Sleep spells and extremely wounded folks. */
	if ( !IS_AWAKE(victim) )
		stop_fighting( victim, FALSE );

	/* Payoff for killing things. */
	if ( victim->position == POS_DEAD )
	{
		group_gain( ch, victim );

		if ( !IS_MOB(victim) )
		{
			/* Dying penalty: 2/3 way back. */
			if ( victim == ch || (IS_MOB(ch) && (ch->master == NULL && ch->leader == NULL)) || IS_SET(victim->act, PLR_WANTED) )
			{
				if ( victim->exp > exp_per_level(victim, victim->pcdata->points) * victim->level )
				{
					lost_exp = ( 2 * (exp_per_level(victim, victim->pcdata->points) * victim->level - victim->exp) / 3 ) + 100;
					gain_exp( victim, lost_exp );
				}
			}

			/* Die too much and deleted ... :( */
			if ( !IS_MOB(victim)
			&&	(victim == ch || (IS_MOB(ch) && (ch->master == NULL && ch->leader == NULL)) || IS_SET(victim->act, PLR_WANTED)) )
			{
				/* 50% di diminuire una stat a caso se essa e' maggiore di 5 */
				if ( number_range(0, 2) == 0 )
				{
					sh_int number_stat;
					/*~~~~~~~~~~~~~~~*/

					number_stat = number_range( 0 , MAX_STATS-1 );
					if ( victim->perm_stat[number_stat] > 5 )
						victim->perm_stat[number_stat]--;
				}

				victim->pcdata->death++;
				if ( victim->class == CLASS_SAMURAI )
				{
					if ( (victim->pcdata->death % 3) == 2 )
					{
						/* Diminuisce al samurai il carisma al posto della costituzione */
						if ( victim->perm_stat[CHA] > 3 )
							victim->perm_stat[CHA]--;

						if ( victim->pcdata->death > 10 )
						{
							char	strsave[160];
							/*~~~~~~~~~~~~~~~~~*/

							act( "Diventi un fantasma e lasci per sempre questo mondo.",
								victim,
								NULL,
								NULL,
								TO_CHAR,
								POS_REST );
							act( "$n e' mort$x, e non tornera' mai piu'.", victim, NULL, NULL, TO_ROOM, POS_REST );
							victim->last_fight_time = -1;
							victim->point[LIFE] = 1;
							victim->position = POS_STAND;
							sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(victim->name) );
							wiznet( "Il personaggio Samurai di $N e' stato cancellato per il limite delle dieci morti.",
								ch, NULL, WIZ_DEATHS, 0 );
							do_quit_count( victim, "" );
							unlink( strsave );
							return( TRUE );
						}
					}
				}
				else if ( (victim->pcdata->death % 3) == 2 )
				{
					victim->perm_stat[CON]--;
					if ( victim->perm_stat[CON] < 3 )
					{
						char	strsave[160];
						/*~~~~~~~~~~~~~~~~~*/

						act( "Sei diventat$x un fantasma per sempre lasciando questo mondo.", victim, NULL, NULL, TO_CHAR, POS_REST );
						act( "$n e' mort$x, e non tornera' mai piu'.", victim, NULL, NULL, TO_ROOM, POS_REST );
						victim->last_fight_time = -1;
						victim->point[LIFE] = 1;
						victim->position = POS_STAND;
						sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(victim->name) );
						wiznet( "Il personaggio di $N e' stato cancellato per mancanza di CON.",
							ch, NULL, WIZ_DEATHS, 0 );
						do_quit_count( victim, "" );
						unlink( strsave );
						return( TRUE );
					}
					else
						send_to_char( "Senti che la forza vitale e' diminuita dopo questa questa morte.\n\r", victim );
				}
			}
		}

		raw_kill( victim, -1 );

		/* don't remember killed victims anymore */
		if ( IS_MOB(ch) )
		{
			if ( ch->pIndexData->vnum == MOB_VNUM_STALKER )
				ch->status = 10;
			remove_mind( ch, victim->name );
			if ( IS_SET(ch->act, ACT_HUNTER) && ch->hunting == victim )
			{
				ch->hunting = NULL;
				REMOVE_BIT( ch->act, ACT_HUNTER );
			}
		}

		/* RT new auto commands */
		if ( !IS_MOB(ch) && IS_MOB(victim) )
		{
			corpse = get_obj_list( ch, "corpo", ch->in_room->contents );

			if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains )	/* exists and not empty */
				do_get( ch, "all corpo" );

			if ( IS_SET(ch->act, PLR_AUTOGOLD) && corpse && corpse->contains
			&& /* exists and not empty */ !IS_SET(ch->act, PLR_AUTOLOOT) )
			{
				do_get( ch, "oro corpo" );
				do_get( ch, "argento corpo" );
				do_get( ch, "monete corpo" );
			}

			if ( ch->class == CLASS_VAMPIRE && ch->level > LEVEL_NEWBIE && corpse )
			{
				act( "$C$n succhia il sangue dal corpo di $N!!$c", ch, NULL, victim, TO_ROOM, POS_SLEEP, RED_BOLD );
				act( "$CSucchi il sangue dal corpo!!$c", ch, NULL, NULL, TO_CHAR, POS_SLEEP, RED_BOLD );
				gain_condition( ch, COND_BLOODLUST, 3 );
			}

			if ( IS_SET(ch->act, PLR_AUTOSAC) )
			{
				if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains )
					return( TRUE ); /* leave if corpse has treasure */
				else
					do_sacrifice( ch, "corpo" );
			}
		}

		return( TRUE );
	}

	if ( victim == ch )
		return( TRUE );

	/* Take care of link dead people. */
	if ( !IS_MOB(victim) && victim->desc == NULL )
	{
		if ( number_range(0, victim->wait) == 0 )
		{
			if ( victim->level < LEVEL_NEWBIE+5 )
				do_recall( victim, "" );
			else
				do_flee( victim, "" );
			return( TRUE );
		}
	}

	/* Wimp out? */
	if ( IS_MOB(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2 )
	{
		if (((IS_SET(victim->act, ACT_WIMPY) && number_bits(2) == 0 && victim->point[LIFE] < victim->max_point[LIFE] / 5)
			||	(IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL && victim->master->in_room != victim->in_room) )
		||	(CAN_DETECT(victim, ADET_FEAR) && !IS_SET(victim->act, ACT_NOTRACK)) )
		{
			do_flee( victim, "" );
			victim->last_fought = NULL;
		}
	}

	if (!IS_MOB(victim)
	&&	victim->point[LIFE] > 0
	&&	(victim->point[LIFE] <= victim->wimpy || CAN_DETECT(victim, ADET_FEAR))
	&&	victim->wait < PULSE_VIOLENCE / 2 )
		do_flee( victim, "" );

	tail_chain();
	return( TRUE );
}


bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( is_safe_nomessage(ch, victim) )
	{
		act( "Gli Dei ti proteggono da $n.", ch, NULL, victim, TO_VICT, POS_REST );
		act( "Gli Dei proteggono $N.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "Gli Dei proteggono $N da $n.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		return( TRUE );
	}
	else
		return( FALSE );
}


bool is_safe_nomessage( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( victim->fighting == ch || ch == victim )
		return( FALSE );

	/* Ghosts are safe */
	if ( (!IS_MOB(victim) && IS_SET(victim->act, PLR_GHOST)) || (!IS_MOB(ch) && IS_SET(ch->act, PLR_GHOST)) )
		return( TRUE );

	/* link dead players whose adrenalin is not gushing are safe */
	if (!IS_MOB(victim)
	&&	((victim->last_fight_time == -1) || ((current_time - victim->last_fight_time) > FIGHT_DELAY_TIME))
	&&	victim->desc == NULL )
		return( TRUE );

	/* newbie */
	if ( (!IS_MOB(ch) && !IS_MOB(victim) && victim->level < LEVEL_NEWBIE) || (!IS_MOB(ch) && !IS_MOB(victim) && ch->level < LEVEL_NEWBIE) )
		return( TRUE );

	/* newly death staff */
	if (!IS_ADMIN(ch)
	&&	!IS_MOB(victim)
	&&	((ch->last_death_time != -1 && current_time - ch->last_death_time < 600)
		|| (victim->last_death_time != -1 && current_time - victim->last_death_time < 600)) )
	{
		return( TRUE );
	}

	return( FALSE );
}


bool is_safe_spell( CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
	if ( ch == victim && !area )
		return( TRUE );

	if ( IS_ADMIN(victim) && area )
		return( TRUE );

	if ( is_same_group(ch, victim) && area )
		return( TRUE );

	if ( ch == victim && area && ch->in_room->sector_type == SECT_INSIDE )
		return( TRUE );

	if ( (RIDDEN(ch) == victim || MOUNTED(ch) == victim) && area )
		return( TRUE );

	return( is_safe(ch, victim) );
}


/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance;
	/*~~~~~~~*/

	if ( !IS_AWAKE(victim) )
		return( FALSE );

	if ( get_wield_char(victim, FALSE) == NULL )
		return( FALSE );

	if ( IS_MOB(victim) )
	{
		chance = UMIN( 40, victim->level );
	}
	else
	{
		chance = get_skill( victim, gsn_parry ) / 2;
		if ( victim->class == CLASS_WARRIOR || victim->class == CLASS_SAMURAI )
			chance *= 1.2;
	}

	if ( number_percent() >= chance + victim->level - ch->level )
		return( FALSE );

	act( "Pari l'attacco di $n.", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$N para il tuo attacco.", ch, NULL, victim, TO_CHAR, POS_REST );
	check_weapon_destroyed( ch, victim, FALSE );
	if ( number_percent() > get_skill(victim, gsn_parry) )
	{
		/* size and weight */
		chance += ch->carry_weight / 25;
		chance -= victim->carry_weight / 20;

		if ( ch->size < victim->size )
			chance += ( ch->size - victim->size ) * 25;
		else
			chance += ( ch->size - victim->size ) * 10;

		/* stats */
		chance += get_curr_stat( ch, STR );
		chance -= get_curr_stat( victim, DEX ) * 4 / 3;

		if ( IS_AFFECTED(ch, AFF_FLYING) )
			chance -= 10;

		/* speed */
		if ( IS_SET(ch->off_flags, OFF_FAST) )
			chance += 10;
		if ( IS_SET(victim->off_flags, OFF_FAST) )
			chance -= 20;

		/* level */
		chance += ( ch->level - victim->level ) * 2;

		/* now the attack */
		if ( number_percent() < (chance / 20) )
		{
			act( "Non riesci a mantenere la posizione e cadi a terra!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$N non riesce a mantenere una posizione di lotta e cade a terra!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n con gran forza fa cadere per terra $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );

			WAIT_STATE( victim, skill_table[gsn_bash].beats );
			victim->position = POS_REST;
		}
	}

	check_improve( victim, gsn_parry, TRUE, 6 );
	return( TRUE );
}


/*
 * check blink
 */
bool check_blink( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance;
	/*~~~~~~~*/

	if ( !IS_SET(victim->act, PLR_BLINK_ON) )
		return( FALSE );

	if ( IS_MOB(victim) )
		return( FALSE );
	else
		chance = victim->pcdata->learned[gsn_blink] / 2;

	if ( (number_percent() >= chance + victim->level - ch->level) || (number_percent() < 50) || (victim->point[MANA] < 10) )
		return( FALSE );

	victim->point[MANA] -= UMAX( victim->level / 10, 1 );

	act( "Eviti l'attacco di $n.", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$N evita il tuo attacco.", ch, NULL, victim, TO_CHAR, POS_REST );
	check_improve( victim, gsn_blink, TRUE, 6 );
	return( TRUE );
}


/*
 * Check for shield block.
 */
bool check_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance;
	/*~~~~~~~*/

	if ( !IS_AWAKE(victim) )
		return( FALSE );

	if ( get_shield_char(victim) == NULL )
		return( FALSE );

	if ( IS_MOB(victim) )
	{
		chance = 10;
	}
	else
	{
		if ( get_skill(victim, gsn_shield_block) <= 1 )
			return( FALSE );
		chance = get_skill( victim, gsn_shield_block ) / 2;
		chance -= ( victim->class == CLASS_WARRIOR ) ? 0 : 10;
	}

	if ( number_percent() >= chance + victim->level - ch->level )
		return( FALSE );

	act( "Il tuo scudo blocca l'attacco di $n.", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$N blocca con lo scudo il tuo attacco.", ch, NULL, victim, TO_CHAR, POS_REST );
	check_shield_destroyed( ch, victim, FALSE );
	check_improve( victim, gsn_shield_block, TRUE, 6 );
	return( TRUE );
}


/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance;
	/*~~~~~~~*/

	if ( !IS_AWAKE(victim) )
		return( FALSE );

	if ( MOUNTED(victim) )
		return( FALSE );

	if ( IS_MOB(victim) )
		chance = UMIN( 30, victim->level );
	else
	{
		chance = get_skill( victim, gsn_dodge ) / 2;

		/* chance for high dex. */
		chance += 2 * ( get_curr_stat(victim, DEX) - 20 );
		if ( victim->class == CLASS_WARRIOR || victim->class == CLASS_SAMURAI )
			chance *= 1.2;
		if ( victim->class == CLASS_THIEF || victim->class == CLASS_NINJA )
			chance *= 1.1;
	}

	if ( number_percent() >= chance + (victim->level - ch->level) / 2 )
		return( FALSE );

	act( "Schivi l'attacco di $n.", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$N schiva il tuo attacco.", ch, NULL, victim, TO_CHAR, POS_REST );
	if ( number_percent() < (get_skill(victim, gsn_dodge) / 20) && !(IS_AFFECTED(ch, AFF_FLYING) || ch->position < POS_FIGHT) )
	{
		/* size */
		if ( victim->size < ch->size )
			chance += ( victim->size - ch->size ) * 10; /* bigger = harder to trip */

		/* dex */
		chance += get_curr_stat( victim, DEX );
		chance -= get_curr_stat( ch, DEX ) * 3 / 2;

		if ( IS_AFFECTED(victim, AFF_FLYING) )
			chance -= 10;

		/* speed */
		if ( IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE) )
			chance += 10;
		if ( IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE) )
			chance -= 20;

		/* level */
		chance += ( victim->level - ch->level ) * 2;

		/* now the attack */
		if ( number_percent() < (chance / 20) )
		{
			act( "$n hai perso l'equilibrio e cadi a terra!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$N perde l'equilibrio e cade a terra!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n con gran forza fa cadere per terra $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );

			WAIT_STATE( ch, skill_table[gsn_trip].beats );
			ch->position = POS_REST;
		}
	}

	check_improve( victim, gsn_dodge, TRUE, 6 );

	return( TRUE );
}


/*
 * Check for cross.
 */
bool check_cross( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance;
	/*~~~~~~~*/

	if ( !IS_AWAKE(victim) )
		return( FALSE );

	if ( get_wield_char(victim, FALSE) == NULL || get_wield_char(victim, TRUE) == NULL )
		return( FALSE );

	if ( IS_MOB(victim) )
	{
		chance = UMIN( 35, victim->level );
	}
	else
	{
		chance = get_skill( victim, gsn_cross_block ) / 3;
		if ( victim->class == CLASS_WARRIOR || victim->class == CLASS_SAMURAI )
			chance *= 1.2;
	}

	if ( number_percent() >= chance + victim->level - ch->level )
		return( FALSE );

	act( "Blocchi tra le due armi l'attacco di $n", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$N incrocia le armi bloccando il tuo attacco!", ch, NULL, victim, TO_CHAR, POS_REST );
	check_weapon_destroyed( ch, victim, FALSE );
	if ( number_percent() > get_skill(victim, gsn_cross_block) )
	{
		/* size and weight */
		chance += ch->carry_weight / 25;
		chance -= victim->carry_weight / 10;

		if ( ch->size < victim->size )
			chance += ( ch->size - victim->size ) * 25;
		else
			chance += ( ch->size - victim->size ) * 10;

		/* stats */
		chance += get_curr_stat( ch, STR );
		chance -= get_curr_stat( victim, DEX ) * 5 / 3;

		if ( IS_AFFECTED(ch, AFF_FLYING) )
			chance -= 20;

		/* speed */
		if ( IS_SET(ch->off_flags, OFF_FAST) )
			chance += 10;
		if ( IS_SET(victim->off_flags, OFF_FAST) )
			chance -= 20;

		/* level */
		chance += ( ch->level - victim->level ) * 2;

		/* now the attack */
		if ( number_percent() < (chance / 20) )
		{
			act( "$n hai perso l'equilibrio e cadi a terra!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$N perde l'equilibrio e cade a terra!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n con gran forza fa cadere per terra $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );

			WAIT_STATE( victim, skill_table[gsn_bash].beats );
			victim->position = POS_REST;
		}
	}

	check_improve( victim, gsn_cross_block, TRUE, 6 );
	return( TRUE );
}


/*
 * Check for hand.
 */
bool check_hand( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance;
	/*~~~~~~~*/

	if ( !IS_AWAKE(victim) )
		return( FALSE );

	if ( get_wield_char(victim, FALSE) != NULL )
		return( FALSE );

	if ( IS_MOB(victim) )
	{
		chance = UMIN( 35, victim->level );
	}
	else
	{
		chance = get_skill( victim, gsn_hand_block ) / 3;
		if ( victim->class == CLASS_NINJA )
			chance *= 1.5;
	}

	if ( number_percent() >= chance + victim->level - ch->level )
		return( FALSE );

	act( "Blocchi con le mani l'attacco di $n.", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$N blocca il tuo attacco con le mani!", ch, NULL, victim, TO_CHAR, POS_REST );
	check_improve( victim, gsn_hand_block, TRUE, 6 );
	return( TRUE );
}


/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
	if ( victim->point[LIFE] > 0 )
	{
		if ( victim->position <= POS_STUN )
			victim->position = POS_STAND;
		return;
	}

	if ( IS_MOB(victim) && victim->point[LIFE] < 1 )
	{
		victim->position = POS_DEAD;
		return;
	}

	if ( victim->point[LIFE] <= -11 )
	{
		victim->position = POS_DEAD;
		return;
	}

	if ( victim->point[LIFE] <= -6 )
		victim->position = POS_MORTAL;
	else if ( victim->point[LIFE] <= -3 )
		victim->position = POS_INCAP;
	else
		victim->position = POS_STUN;
}


/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( ch->fighting != NULL )

	{
		bug( "Set_fighting: already fighting", 0 );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_SLEEP) )
		affect_strip( ch, gsn_sleep );

	ch->fighting = victim;
	ch->position = POS_FIGHT;
}


/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
	CHAR_DATA	*fch;
	/*~~~~~~~~~~~~~*/

	for ( fch = char_list; fch != NULL; fch = fch->next )
	{
		if ( fch == ch || (fBoth && fch->fighting == ch) )
		{
			fch->fighting = NULL;
			fch->position = IS_MOB( fch ) ? ch->default_pos : POS_STAND;
			update_pos( fch );
		}
	}
}


/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
	char		buf[MSL];
	OBJ_DATA	*corpse;
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	char		*name;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
	{
		name = ch->short_descr;
		corpse = create_object( get_obj_index(OBJ_VNUM_CORPSE_NPC), 0 );
		corpse->timer = number_range( 3, 6 );
		if ( ch->gold > 0 || ch->silver > 0 )
		{
			if ( IS_SET(ch->form, FORM_INSTANT_DECAY) )
				obj_to_room( create_money(ch->gold, ch->silver), ch->in_room );
			else
				obj_to_obj( create_money(ch->gold, ch->silver), corpse );
			ch->gold = 0;
		}

		corpse->from = str_dup( ch->short_descr );
		corpse->cost = 0;
	}
	else
	{
		if ( IS_GOOD(ch) )
			i = 0;
		if ( IS_EVIL(ch) )
			i = 2;
		else
			i = 1;

		name = ch->name;
		corpse = create_object( get_obj_index(OBJ_VNUM_CORPSE_PC), 0 );
		corpse->timer = number_range( 25, 40 );
		corpse->owner = str_dup( ch->name );
		corpse->from = str_dup( ch->name );
		corpse->altar = hometown_table[ch->hometown].altar[i];
		corpse->pit = hometown_table[ch->hometown].pit[i];

		if ( ch->gold > 0 || ch->silver > 0 )
		{
			obj_to_obj( create_money(ch->gold, ch->silver), corpse );
			ch->gold = 0;
			ch->silver = 0;
		}

		corpse->cost = 0;
	}

	corpse->level = ch->level;

	sprintf( buf, corpse->short_descr, name );
	free_string( corpse->short_descr );
	corpse->short_descr = str_dup( buf );

	sprintf( buf, corpse->description, name );
	free_string( corpse->description );
	corpse->description = str_dup( buf );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next_content;
		obj_from_char( obj );
		if ( obj->item_type == ITEM_POTION )
			obj->timer = number_range( 500, 1000 );
		if ( obj->item_type == ITEM_SCROLL )
			obj->timer = number_range( 1000, 2500 );
		if ( IS_SET(obj->extra_flags, ITEM_ROT_DEATH) )
		{
			obj->timer = number_range( 5, 10 );
			if ( obj->item_type == ITEM_POTION )
				obj->timer += obj->level * 20;
		}

		REMOVE_BIT( obj->extra_flags, ITEM_VIS_DEATH );
		REMOVE_BIT( obj->extra_flags, ITEM_ROT_DEATH );

		if (IS_SET(obj->extra_flags, ITEM_INVENTORY)
		||	(obj->pIndexData->limit != -1 && (obj->pIndexData->count > obj->pIndexData->limit)) )
		{
			extract_obj( obj );
			continue;
		}
		else if ( IS_SET(ch->form, FORM_INSTANT_DECAY) )
			obj_to_room( obj, ch->in_room );
		else
			obj_to_obj( obj, corpse );
	}

	obj_to_room( corpse, ch->in_room );
}


/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch, int part )
{
	ROOM_INDEX_DATA *was_in_room;
	char			*msg;
	int				door;
	int				vnum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	vnum = 0;
	msg = "$CSenti le urla di morte di $n.$c";

	if ( part == -1 )
		part = number_bits( 4 );

	switch ( part )
	{
	case 0:
		msg = "$C$n colpisce il suolo e.. MUORE!$c";
		break;

	case 1:
		if ( ch->material == 0 )
		{
			msg = "$n sporca di sangue la tua armatura.";
			break;
		}

	case 2:
		if ( IS_SET(ch->parts, PART_GUTS) )
		{
			msg = "$C$n riempie il suolo delle sue budella.$c";
			vnum = OBJ_VNUM_GUTS;
		}
		break;

	case 3:
		if ( IS_SET(ch->parts, PART_HEAD) )
		{
			msg = "$CLa testa mozzata di $n cade per terra.$c";
			vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;

	case 4:
		if ( IS_SET(ch->parts, PART_HEART) )
		{
			msg = "$CIl cuore di $n e' stato strappato dal petto.$c";
			vnum = OBJ_VNUM_TORN_HEART;
		}
		break;

	case 5:
		if ( IS_SET(ch->parts, PART_ARMS) )
		{
			msg = "$CUn braccio di $n si e' staccato dal corpo.$c";
			vnum = OBJ_VNUM_SLICED_ARM;
		}
		break;

	case 6:
		if ( IS_SET(ch->parts, PART_LEGS) )
		{
			msg = "$CUna gamba di $n si e' staccata dal corpo.$c";
			vnum = OBJ_VNUM_SLICED_LEG;
		}
		break;

	case 7:
		if ( IS_SET(ch->parts, PART_BRAINS) )
		{
			msg = "$CLa testa di $n si e' aperta e il cervello si sparge tutt'intorno.$c";
			vnum = OBJ_VNUM_BRAINS;
		}
	}

	act( msg, ch, NULL, NULL, TO_ROOM, POS_REST, RED );

	if ( vnum != 0 )
	{
		char		buf[MSL];
		OBJ_DATA	*obj;
		char		*name;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		name = IS_MOB( ch ) ? ch->short_descr : ch->name;
		obj = create_object( get_obj_index(vnum), 0 );
		obj->timer = number_range( 4, 7 );

		sprintf( buf, obj->short_descr, name );
		free_string( obj->short_descr );
		obj->short_descr = str_dup( buf );

		sprintf( buf, obj->description, name );
		free_string( obj->description );
		obj->description = str_dup( buf );

		obj->from = str_dup( name );

		if ( obj->item_type == ITEM_FOOD )
		{
			if ( IS_SET(ch->form, FORM_POISON) )
				obj->value[3] = 1;
			else if ( !IS_SET(ch->form, FORM_EDIBLE) )
				obj->item_type = ITEM_TRASH;
		}

		obj_to_room( obj, ch->in_room );
	}

	if ( IS_MOB(ch) )
		msg = "$CSenti qualcuno spirare il suo ultimo urlo di dolore.$c";
	else
		msg = "$CSenti qualcuno urlare per l'ultima volta.$c";

	was_in_room = ch->in_room;
	for ( door = 0; door < MAX_DIR; door++ )
	{
		EXIT_DATA	*pexit;
		/*~~~~~~~~~~~~~~~*/

		if ( (pexit = was_in_room->exit[door]) != NULL && pexit->to_room != NULL && pexit->to_room != was_in_room )
		{
			ch->in_room = pexit->to_room;
			act( msg, ch, NULL, NULL, TO_ROOM, POS_REST, RED );
		}
	}

	ch->in_room = was_in_room;
}


void raw_kill( CHAR_DATA *victim, int part )
{
	CHAR_DATA	*tmp_ch;
	OBJ_DATA	*obj, *obj_next;
	int			i;
	OBJ_DATA	*tattoo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Va' prima della stop_fighting */
	if ( !IS_MOB(victim) )
	{
		char			buf[MSL];
		char			name_ch[MSL];
		char			name_victim[MSL];
		char			csex;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* (bb) da fare con il sistema di nome attaccante variabile esterna */

		if ( victim->fighting && victim->fighting->short_descr[0] != '\0' )
			sprintf( name_ch, "%s", victim->fighting->short_descr );
		else
			strcpy( name_ch, "(null)" );

		if ( victim->short_descr[0] != '\0' )
			sprintf( name_victim, "%s", victim->short_descr );
		else
			sprintf( name_victim, "%s", victim->name );

		csex = (victim->sex == SEX_FEMALE) ? 'a' : 'o';

		if ( !victim->fighting || victim == victim->fighting )
		{
			sprintf( buf, "%s e' mort%c a %s (%d).",
				name_victim,
				csex,
				victim->in_room->name,
				victim->in_room->vnum );
		}
		else if ( !IS_MOB(victim) && victim->fighting && !IS_MOB(victim->fighting) )
		{
			sprintf( buf, "%s e' mort%c a %s (%d) per mano del pg: %s.",
				name_victim,
				csex,
				victim->in_room->name,
				victim->in_room->vnum,
				name_ch );
		}
		else
		{
			sprintf( buf, "%s e' mort%c a %s (%d) per mano del mob %s (%d).",
				name_victim,
				csex,
				victim->in_room->name,
				victim->in_room->vnum,
				name_ch,
				victim->fighting->pIndexData->vnum );
		}

		wiznet( buf, victim, NULL, WIZ_DEATHS, 0 );
	}

	stop_fighting( victim, TRUE );

	for ( obj = victim->carrying; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next_content;
		if ( IS_SET(obj->progtypes, OPROG_DEATH) && (obj->wear_loc != WEAR_NONE) )
		{
			if ( (obj->pIndexData->oprogs->death_prog) (obj, victim) )
			{
				victim->position = POS_STAND;
				return;
			}
		}
	}

	victim->last_fight_time = -1;
	if ( IS_SET(victim->progtypes, MPROG_DEATH) )
	{
		if ( (victim->pIndexData->mprogs->death_prog) (victim) )
		{
			victim->position = POS_STAND;
			return;
		}
	}

	victim->last_death_time = current_time;

	tattoo = get_eq_char( victim, WEAR_TATTOO );
	if ( tattoo != NULL )
		obj_from_char( tattoo );

	death_cry( victim, part );
	make_corpse( victim );

	if ( IS_MOB(victim) )
	{
		victim->pIndexData->killed++;
		kill_table[URANGE( 0, victim->level, MAX_LEVEL - 1 )].killed++;
		extract_char( victim, TRUE );
		return;
	}

	send_to_char( "Ti trasformi in un etereo fantasma per qualche tempo.\n\r", victim );
	extract_char( victim, FALSE );

	while ( victim->affected )
		affect_remove( victim, victim->affected );
	victim->affected_by = 0;
	victim->detection = 0;
	for ( i = 0; i < 4; i++ )
		victim->armor[i] = 100;
	victim->position = POS_REST;
	victim->point[LIFE] = victim->max_point[LIFE] / 10;
	victim->point[MANA] = victim->max_point[MANA] / 10;
	victim->point[MOVE] = victim->max_point[MOVE];

	/* RT added to prevent infinite deaths */
	REMOVE_BIT( victim->act, PLR_WANTED );
	REMOVE_BIT( victim->act, PLR_BOUGHT_PET );

	/* SET_BIT(victim->act, PLR_GHOST); */
	victim->pcdata->condition[COND_THIRST] = 40;
	victim->pcdata->condition[COND_HUNGER] = 40;
	victim->pcdata->condition[COND_FULL] = 40;
	victim->pcdata->condition[COND_BLOODLUST] = 40;
	victim->pcdata->condition[COND_DESIRE] = 40;

	if ( tattoo != NULL )
	{
		obj_to_char( tattoo, victim );
		equip_char( victim, tattoo, WEAR_TATTOO );
	}

	save_char_obj( victim );

	/* Calm down the tracking mobiles */
	for ( tmp_ch = char_list; tmp_ch != NULL; tmp_ch = tmp_ch->next )
	{
		if ( tmp_ch->last_fought == victim )
			tmp_ch->last_fought = NULL;
	}
}


bool IS_NECRO_SUMMON( CHAR_DATA *ch )
{
	if ( (IS_MOB(ch))
	&& (ch->pIndexData->vnum == MOB_VNUM_ZOMBIE
	||	ch->pIndexData->vnum == MOB_VNUM_SKELETON
	||	ch->pIndexData->vnum == MOB_VNUM_SPECTRE
	||	ch->pIndexData->vnum == MOB_VNUM_BANSHEE) )
		return TRUE;
	else
		return FALSE;
}


void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
	char		buf[MSL];
	CHAR_DATA	*gch;
	CHAR_DATA	*lch;
	int			xp;
	int			members;
	int			group_levels;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( victim == ch || (IS_MOB(victim) && victim->pIndexData->vnum < 100) )
		return;

	/* quest */
	if ( IS_NECRO_SUMMON(ch) && ch->master != NULL && ch->master->class == CLASS_NECROMANCER )
		gch = ch->master;
	else
		gch = ch;

	if ( !IS_MOB(gch) && IS_SET(gch->act, PLR_QUESTOR) && IS_MOB(victim) )
	{
		if ( gch->pcdata->questmob == victim->pIndexData->vnum )
		{
			send_to_char( "Hai quasi completato la tua missione!\n\r", gch );
			send_to_char( "Ritorna da colui che ti affido' il compito prima che sia troppo tardi!\n\r", gch );
			gch->pcdata->questmob = -1;
		}
	}

	/* end quest */
	if ( !IS_MOB(victim) )
		return;

	if ( IS_MOB(victim) && (victim->master != NULL || victim->leader != NULL) )
		return;

	members = 1;
	group_levels = 0;
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( is_same_group(gch, ch) )
		{
			if ( !IS_MOB(gch) && gch != ch )
				members++;
			group_levels += gch->level;
		}
	}

	lch = ( ch->leader != NULL ) ? ch->leader : ch;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		OBJ_DATA	*obj;
		OBJ_DATA	*obj_next;
		/*~~~~~~~~~~~~~~~~~~*/

		if ( !is_same_group(gch, ch) || IS_MOB(gch) )
			continue;

		if ( gch->level - lch->level > 8 )
		{
			send_to_char( "Sei troppo esperto per questo gruppo.\n\r", gch );
			continue;
		}

		if ( gch->level - lch->level < -8 )
		{
			send_to_char( "Sei troppo inesperto per questo gruppo.\n\r", gch );
			continue;
		}

		xp = xp_compute( gch, victim, group_levels, members );
		if ( IS_ADMIN(gch) )
		{
			sprintf( buf, "Ricevi %d punti d'esperienza.\n\r", xp );
			send_to_char( buf, gch );
		}
		sprintf( buf, "%s riceve %d punti d'esperienza uccidendo %s.",
			gch->name, xp, IS_MOB(victim) ? victim->short_descr : victim->name );
		wiznet( buf, gch, NULL, WIZ_LEVELS, 0 );

		gain_exp( gch, xp );

		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( obj->wear_loc == WEAR_NONE )
				continue;

			if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch))
			||	(IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch))
			||	(IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)) )
			{
				ch->point[LIFE] -= ch->level;
				act( "Sei stat$x fulminat$x da $p.", ch, obj, NULL, TO_CHAR, POS_REST );
				act( "$n e' stat$x fulminat$x da $p.", ch, obj, NULL, TO_ROOM, POS_REST );
				obj_from_char( obj );
				obj_to_room( obj, ch->in_room );
			}

			/* Controllo per le slay */
			if ( (IS_EVIL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYEVIL))
			||	 (IS_GOOD(ch) && IS_OBJ_STAT(obj, ITEM_SLAYGOOD))
			||	 (IS_NEUTRAL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYNEUTRAL)) )
			{
				ch->point[LIFE] -= ch->level*2;
				act( "L'aura di $p avvampa sulla tua mano costringendoti a liberartene!", ch, obj, NULL, TO_CHAR, POS_REST );
				act( "$n geme di dolore mentre butta a terra  $p.",  ch, obj, NULL, TO_ROOM, POS_REST );
				obj_from_char( obj );
				obj_to_room( obj, ch->in_room );
			}
		}
	}
}


/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int tot_levels, int members )
{
	int xp;
	int exp_base;
	int level_range;
	bool neg_cha = FALSE;
	/*~~~~~~~~~~~~*/

	level_range = victim->level - gch->level;

	switch ( level_range )
	{
	default :	exp_base =   0;		break;
	case -8 :	exp_base =   1;		break;
	case -7 :	exp_base =   2;		break;
	case -6 :	exp_base =   4;		break;
	case -5 :	exp_base =   7;		break;
	case -4 :	exp_base =   8;		break;
	case -3 :	exp_base =  18;		break;
	case -2 :	exp_base =  28;		break;
	case -1 :	exp_base =  37;		break;
	case -0 :	exp_base =  53;		break;
	case  1 :	exp_base =  66;		break;
	case  2 :	exp_base =  75;		break;
	case  3 :	exp_base =  89;		break;
	case  4 :	exp_base = 110;		break;
	case  5 :	exp_base = 130;		break;
	}

	if ( level_range > 5 )
		exp_base = 130 + 20 * ( level_range - 5 );

	/* calculate exp multiplier */
	if ( IS_SET(victim->act, ACT_NOALIGN) )
		xp = exp_base;

	/* alignment */
	else if ( (IS_EVIL(gch) && IS_GOOD(victim)) || (IS_EVIL(victim) && IS_GOOD(gch)) )
		xp = exp_base * 8 / 5;
	else if ( IS_GOOD(gch) && IS_GOOD(victim) )
		xp = 0;
	else if ( !IS_NEUTRAL(gch) && IS_NEUTRAL(victim) )
		xp = exp_base * 1.1;
	else if ( IS_NEUTRAL(gch) && !IS_NEUTRAL(victim) )
		xp = exp_base * 1.3;
	else
		xp = exp_base;

	/* more exp at the low levels */
	if ( gch->level <= LEVEL_NEWBIE )
		xp = 12*xp / ( gch->level+4 );

	/* randomize the rewards */
	xp = number_range( xp*3 / 4, xp*5 / 4 );

	/* adjust for grouping */
	xp = xp * gch->level / tot_levels;

	if ( members == 2 || members == 3 )
		xp *= ( 3 / 2 );

	if ( gch->level < 15 )
		xp = UMIN( (255 + dice(1, 25)), xp );
	else if ( gch->level < 40 )
		xp = UMIN( (225 + dice(1, 20)), xp );
	else if ( gch->level < 60 )
		xp = UMIN( (190 + dice(1, 20)), xp );
	else
		xp = UMIN( (170 + dice(1, 20)), xp );

	xp += ( xp * (gch->max_point[LIFE] - gch->point[LIFE]) ) / ( gch->max_point[LIFE] * 5 );

	if ( IS_GOOD(gch) )
	{
		if ( IS_GOOD(victim) )
		{
			gch->pcdata->anti_killed++;
			neg_cha = TRUE;
		}
		else if ( IS_NEUTRAL(victim) )
			gch->pcdata->has_killed++;
		else if ( IS_EVIL(victim) )
			gch->pcdata->has_killed++;
	}

	if ( IS_NEUTRAL(gch) )
	{
		if ( xp > 0 )
		{
			if ( IS_GOOD(victim) )
				gch->pcdata->has_killed++;
			else if ( IS_NEUTRAL(victim) )
			{
				gch->pcdata->anti_killed++;
				neg_cha = TRUE;
			}
			else if ( IS_EVIL(victim) )
				gch->pcdata->has_killed++;
		}
	}

	if ( IS_EVIL(gch) )
	{
		if ( xp > 0 )
		{
			if ( IS_GOOD(victim) )
				gch->pcdata->has_killed++;
			else if ( IS_NEUTRAL(victim) )
				gch->pcdata->has_killed++;
			else if ( IS_EVIL(victim) )
			{
				gch->pcdata->anti_killed++;
				neg_cha = TRUE;
			}
		}
	}

	if ( neg_cha == TRUE )
	{
		if ( (gch->pcdata->anti_killed % 100) == 99 )
		{
			if ( gch->perm_stat[CHA] >= 5 )
			{
				if ( number_range(0, 2) == 0 )
					gch->perm_stat[CHA]--;
				if ( !IS_NEUTRAL(gch) )
					gch->perm_stat[CHA]--;	/* Per i non neutrali diminuiamo di sicuro. */
				send_to_char( "Il tuo carisma e' diminuito per le tue azioni fuori allineamento.\n\r", gch );
			}
		}
	}

	return( xp );
}


void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune, int dam_type )
{
	char		buf_bug[MSL],
				buf1[256],
				buf2[256],
				buf3[256];
	const char	*vs;
	const char	*attack;
	char		punct;
	int			art2 = 0;
	char		str_art1[16];
	char		str_art2[16];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if		( dam == 0 )		vs = "manca";
	else if ( dam <= 4 )		vs = "sfiora";
	else if ( dam <= 8 )		vs = "graffia";
	else if ( dam <= 12 )		vs = "colpisce di striscio";
	else if ( dam <= 16 )		vs = "colpisce";
	else if ( dam <= 20 )		vs = "colpisce in pieno";
	else if ( dam <= 24 )		vs = "ferisce";
	else if ( dam <= 28 )		vs = "frastorna";
	else if ( dam <= 32 )		vs = "squinterna";
	else if ( dam <= 36 )		vs = "storpia";
	else if ( dam <= 42 )		vs = "mutila";
	else if ( dam <= 52 )		vs = "lacera";
	else if ( dam <= 65 )		vs = "dilania";
	else if ( dam <= 80 )		vs = "DISTRUGGE";
	else if ( dam <= 100 )		vs = "SBUDELLA";
	else if ( dam <= 130 )		vs = "MUTILA";
	else if ( dam <= 175 )		vs = "FRACASSA";
	else if ( dam <= 250 )		vs = "** DECIMA **";
	else if ( dam <= 325 )		vs = "** DEVASTA **";
	else if ( dam <= 400 )		vs = "== SVENTRA ==";
	else if ( dam <= 500 )		vs = "=*=* MASSACRA *=*=";
	else if ( dam <= 650 )		vs = "=*=* SMEMBRA *=*=";
	else if ( dam <= 800 )		vs = "<*> <*> MACIULLA <*> <*>";
	else if ( dam <= 1000 )		vs = "<*> <*> SQUARTA <*> <*>";
	else if ( dam <= 1250 )		vs = "<-*#*-> DEMOLISCE <-*#*-> ";
	else if ( dam <= 1500 )		vs = "<-*#*-> ANNIENTA <-*#*->";
	else if ( dam <= 2000 )		vs = "<*>!(*)!<*>> TERMINA <<*)!(*)!<*>";
	else						vs = "\007=<*)-!!!-(*>= ! TERMINA ! =<*)-!!!- (*>=\007";

	if ( victim->level < 20 )
		punct = ( dam <= 24 ) ? '.' : '!';
	else if ( victim->level < 50 )
		punct = ( dam <= 50 ) ? '.' : '!';
	else
		punct = ( dam <= 75 ) ? '.' : '!';

	if ( (dt == TYPE_HIT) || (dt == TYPE_HUNGER) )
	{
		if ( ch == victim )
		{
			if ( dam_type == DAM_HUNGER )
			{
				sprintf( buf1, "Lo stomaco di $n sta brontolando." );
				sprintf( buf2, "La fame ti $C%s$c%c", vs, punct );
			}
			else if ( dam_type == DAM_THIRST )
			{
				sprintf( buf1, "Le labbra di $n avvizziscono per la sete.." );
				sprintf( buf2, "La sete ti $C%s$c%c", vs, punct );
			}
			else if ( dam_type == DAM_LIGHT_V )
			{
				sprintf( buf1, "$n sembra soffrire a causa della luce nella stanza." );
				sprintf( buf2, "La luce della stanza ti $C%s$c!%c", vs, punct );
			}
			else if ( dam_type == DAM_TRAP_ROOM )
			{
				sprintf( buf1, "La tensione elettrica nella stanza $C%s$c!%c $n!", vs, punct );
				sprintf( buf2, "Una trappola nel luogo ti $C%s$c!%c", vs, punct );
			}
			else
			{
				sprintf( buf1, "$n si $C%s$c%c", vs, punct );
				sprintf( buf2, "Ti $C%s$c%c", vs, punct );
			}
		}
		else
		{
			sprintf( buf1, "L'attacco di $n $C%s$c $N%c", vs, punct );
			sprintf( buf2, "Il tuo attacco $C%s$c $N%c", vs, punct );
			sprintf( buf3, "L'attacco di $n ti $C%s$c%c", vs, punct );
		}
	}
	else
	{
		if ( dt >= 0 && dt < MAX_SKILL )
		{
			attack = skill_table[dt].noun_damage;

			if ( skill_table[dt].article_damage == 'f' || skill_table[dt].article_damage == 'F' )
			{
				SET_BIT( art2, ART_FEMMINILE );
				sprintf( str_art1, "%s%s", "la ", "tua " );
			}
			else if ( skill_table[dt].article_damage == 'm' || skill_table[dt].article_damage == 'M' )
			{
				sprintf( str_art1, "%s%s", "il ", "tuo " );
			}
			else
			{
				sprintf( buf_bug, "Dam_message: articolo per il tipo di danno skill %s non definito", attack );
				bug( buf_bug, 0 );

				/* maschile di default se errore */
				sprintf( str_art1, "%s%s", "il ", "tuo " );
			}

			sprintf( str_art2, "%s", scegli_articolo(attack, art2) );
		}
		else if ( dt >= TYPE_HIT && dt <= TYPE_HIT + MAX_DAMAGE_MESSAGE )
		{
			attack = attack_table[dt - TYPE_HIT].noun;

			if ( attack_table[dt - TYPE_HIT].article == 'f' || attack_table[dt - TYPE_HIT].article == 'F' )
			{
				SET_BIT( art2, ART_FEMMINILE );
				sprintf( str_art1, "%s%s", "la ", "tua " );
			}
			else if ( attack_table[dt - TYPE_HIT].article == 'm' || attack_table[dt - TYPE_HIT].article == 'M' )
			{
				sprintf( str_art1, "%s%s", "il ", "tuo " );
			}
			else
			{
				sprintf( buf_bug, "Dam_message: articolo per il tipo di danno d'attacco %s non definito", attack );
				bug( buf_bug, 0 );

				/* maschile di default se errore */
				sprintf( str_art1, "%s%s", "il ", "tuo " );
			}

			sprintf( str_art2, "%s", scegli_articolo(attack, art2) );
		}
		else
		{
			bug( "Dam_message: dt errata %d.", dt );
			dt = TYPE_HIT;
			attack = attack_table[0].noun;

			if ( attack_table[0].article == 'f' || attack_table[0].article == 'F' )
			{
				SET_BIT( art2, ART_FEMMINILE );
				sprintf( str_art1, "%s%s", "la ", "tua " );
			}
			else if ( attack_table[0].article == 'f' || attack_table[0].article == 'F' )
			{
				sprintf( str_art1, "%s%s", "il ", "tuo " );
			}
			else
			{
				sprintf( buf_bug, "Dam_message: articolo per il tipo di danno d'attacco errato %s non definito", attack );
				bug( buf_bug, 0 );

				/* maschile di default se errore */
				sprintf( str_art1, "%s%s", "il ", "tuo " );
			}

			sprintf( str_art2, "%s", scegli_articolo(attack, art2) );
		}

		if ( immune )
		{
			if ( ch == victim )
			{
				sprintf( buf1, "$n e' immune a questo attacco." );
				sprintf( buf2, "Fortunatamente, sei immune a questo attacco." );
			}
			else
			{
				sprintf( buf1, "$N e' immune all'attacco di $n!" );
				sprintf( buf2, "$N e' immune al tuo attacco!" );
				sprintf( buf3, "Sei immune all'attacco di $n!" );
			}
		}
		else
		{
			if ( ch == victim )
			{
				sprintf( buf1, "%s%s $C%s$c $n%c", str_art1, attack, vs, punct );
				sprintf( buf2, "%s%s ti $C%s$c%c", str_art1, attack, vs, punct );
			}
			else
			{
				sprintf( buf1, "%s%s di $n $C%s$c $N%c", str_art2, attack, vs, punct );
				sprintf( buf2, "%s%s $C%s$c $N%c", str_art1, attack, vs, punct );
				sprintf( buf3, "%s%s di $n ti $C%s$c%c", str_art2, attack, vs, punct );
			}
		}
	}

	if ( ch == victim )
	{
		act( buf1, ch, NULL, NULL, TO_ROOM, POS_REST, RED );
		act( buf2, ch, NULL, NULL, TO_CHAR, POS_REST, RED_BOLD );
	}
	else
	{
		act( buf1, ch, NULL, victim, TO_NOTVICT, POS_REST, RED );
		act( buf2, ch, NULL, victim, TO_CHAR, POS_REST, RED_BOLD );
		act( buf3, ch, NULL, victim, TO_VICT, POS_REST, RED_BOLD );
	}
}


void do_kill( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*wield;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Uccidere chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( ch->position == POS_FIGHT )
	{
		if ( victim == ch->fighting )
			send_to_char( "Stai facendo quello che puoi!\n\r", ch );
		else if ( victim->fighting != ch )
			send_to_char( "Una battaglia per volta!\n\r", ch );
		else
		{
			act( "Cominci a puntare a $N.", ch, NULL, victim, TO_CHAR, POS_REST );
			ch->fighting = victim;
		}

		return;
	}

	if ( !IS_MOB(victim) )
	{
		send_to_char( "Devi uccidere un giocatore.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Ti colpisci. Ouch!\n\r", ch );
		multi_hit( ch, ch, TYPE_UNDEFINED );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "$N e' il tuo amato maestro.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

	if ( !skill_failure_nomessage(ch, gsn_mortal_strike, 0)
	&&	(wield = get_wield_char(ch, FALSE)) != NULL
	&&	wield->level > (victim->level - 5) )
	{
		int chance = 1 + get_skill( ch, gsn_mortal_strike ) / 30;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		chance += ( ch->level - victim->level ) / 2;
		if ( number_percent() < chance )
		{
			act( "$CIl tuo colpo uccide all'istante $N!$c", ch, NULL, victim, TO_CHAR, POS_REST, RED );
			act( "$CIl colpo di $n uccide all'istante $N!$c", ch, NULL, victim, TO_NOTVICT, POS_REST, RED );
			act( "$CIl colpo di $n ti uccide all'istante!$c", ch, NULL, victim, TO_VICT, POS_DEAD, RED );
			damage( ch, victim, (victim->point[LIFE] + 1), gsn_mortal_strike, DAM_NONE, TRUE );
			check_improve( ch, gsn_mortal_strike, TRUE, 1 );
			return;
		}
		else
			check_improve( ch, gsn_mortal_strike, FALSE, 3 );
	}

	multi_hit( ch, victim, TYPE_UNDEFINED );
}


void do_murde( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Se vuoi uccidere, digita il comando per intero.\n\r", ch );
}


void do_murder( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*wield;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Assassinare chi?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) || (IS_MOB(ch) && IS_SET(ch->act, ACT_PET)) )
		return;

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Penso che dovresti pensarci bene prima di farlo..\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "$N e' l'amat$X maestr$X.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( ch->position == POS_FIGHT )
	{
		send_to_char( "Stai facendo quello che puoi!\n\r", ch );
		return;
	}

	WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
	if ( IS_MOB(victim) )
		do_yell( victim, "Aiuto! Mi sta attaccando!" );

	if ( !skill_failure_nomessage(ch, gsn_mortal_strike, 0 )
	&&	(wield = get_wield_char(ch, FALSE)) != NULL
	&&	wield->level > (victim->level - 5) )
	{
		int chance = 1 + get_skill( ch, gsn_mortal_strike ) / 30;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		chance += ( ch->level - victim->level ) / 2;
		if ( number_percent() < chance )
		{
			act( "$CIl tuo colpo uccide all'istante $N!$c", ch, NULL, victim, TO_CHAR, POS_REST, RED );
			act( "$CIl colpo di $n uccide all'istante $N!$c", ch, NULL, victim, TO_NOTVICT, POS_REST, RED );
			act( "$CIl colpo di $n ti uccide all'istante!$c", ch, NULL, victim, TO_VICT, POS_DEAD, RED );
			damage( ch, victim, (victim->point[LIFE] + 1), gsn_mortal_strike, DAM_NONE, TRUE );
			check_improve( ch, gsn_mortal_strike, TRUE, 1 );
			return;
		}
		else
			check_improve( ch, gsn_mortal_strike, FALSE, 3 );
	}

	multi_hit( ch, victim, TYPE_UNDEFINED );
}


void do_flee( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *was_in;
	ROOM_INDEX_DATA *now_in;
	CHAR_DATA		*victim;
	int				attempt;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( RIDDEN(ch) )
	{

		send_to_char( "Devi chiedere al tuo cavaliere!\n\r", ch );
		return;
	}

	if ( MOUNTED(ch) )
		do_dismount( ch, "" );

	if ( (victim = ch->fighting) == NULL )
	{
		if ( ch->position == POS_FIGHT )
			ch->position = POS_STAND;
		send_to_char( "Non stai combattendo ora.\n\r", ch );
		return;
	}

	if ( ch->class == CLASS_SAMURAI && ch->level >= LEVEL_NEWBIE+5 )
	{
		send_to_char( "Disonorarti con la fuga? MAI!\n\r", ch );
		return;
	}

	was_in = ch->in_room;
	for ( attempt = 0; attempt < 6; attempt++ )
	{
		EXIT_DATA	*pexit;
		int			door;
		/*~~~~~~~~~~~~~~~*/

		door = number_door();
		if ((pexit = was_in->exit[door]) == 0
		||	pexit->to_room == NULL
		||	(IS_SET(pexit->exit_info, EX_CLOSED)
			&&	(!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info, EX_NOPASS))
			&&	!IS_TRUSTED(ch, LEVEL_MASTER))
		||	(IS_SET(pexit->exit_info, EX_NOFLEE))
		||	(IS_MOB(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)) )
			continue;

		move_char( ch, door, FALSE );
		if ( (now_in = ch->in_room) == was_in )
			continue;

		ch->in_room = was_in;
		act( "$n e' fuggit$x", ch, NULL, NULL, TO_ROOM, POS_REST );
		ch->in_room = now_in;

		if ( !IS_MOB(ch) )
		{
			send_to_char( "Fuggi dallo scontro!\n\r", ch );
			if ( ch->class == CLASS_SAMURAI && ch->level >= LEVEL_NEWBIE+5 )
				gain_exp( ch, -(ch->level)*3 );
			else
				gain_exp( ch, -(ch->level)*2 );
		}
		else
			ch->last_fought = NULL;

		stop_fighting( ch, TRUE );
		return;
	}

	send_to_char( "PANICO! Non riesci a fuggire!!\n\r", ch );
}


void do_sla( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Se hai intenzione di uccidere, digita il comando per intero.\n\r", ch );
}


void do_slay( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	char		arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Uccidere chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Penso che dovresti pensarci bene prima di farlo..\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) && victim->level >= get_trust(ch) && !(IS_MOB(ch) && ch->cabal != CABAL_NONE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Fallisci.\n\r", ch );
		return;
	}

	act( "Uccidi $N a sangue freddo!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n ti uccide a sangue freddo!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$n uccide $N a sangue freddo!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	raw_kill( victim, -1 );
}


/*
 * Check for obj dodge.
 */
bool check_obj_dodge( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj, int bonus )
{
	int chance;
	/*~~~~~~~*/

	if ( !IS_AWAKE(victim) || MOUNTED(victim) )
		return( FALSE );

	if ( IS_MOB(victim) )
		chance = UMIN( 30, victim->level );
	else
	{
		chance = get_skill( victim, gsn_dodge ) / 2;

		/* chance for high dex. */
		chance += 2 * ( get_curr_stat(victim, DEX) - 20 );
		if ( victim->class == CLASS_WARRIOR || victim->class == CLASS_SAMURAI )
			chance *= 1.2;
		if ( victim->class == CLASS_THIEF || victim->class == CLASS_NINJA )
			chance *= 1.1;
	}

	chance -= ( bonus - 90 );
	chance /= 2;
	if ( number_percent() >= chance && (IS_MOB(victim) || victim->cabal != CABAL_ARMY) )
		return( FALSE );

	if ( !IS_MOB(victim) && victim->cabal == CABAL_ARMY && IS_SET(victim->act, PLR_CANINDUCT) )
	{
		act( "Prendi $p che ti e' stata lanciata.", ch, obj, victim, TO_VICT, POS_REST );
		act( "$N prende $p che gli e' stata lanciata.", ch, obj, victim, TO_CHAR, POS_REST );
		act( "$n prende $p che gli e' stata lanciata.", victim, obj, ch, TO_NOTVICT, POS_REST );
		obj_to_char( obj, victim );
		return( TRUE );
	}

	act( "Schivi $p che ti e' stata lanciata.", ch, obj, victim, TO_VICT, POS_REST );
	act( "$N schiva $p che gli e' stata lanciata.", ch, obj, victim, TO_CHAR, POS_REST );
	act( "$n schiva $p che gli e' stata lanciata.", victim, obj, ch, TO_NOTVICT, POS_REST );

	obj_to_room( obj, victim->in_room );
	check_improve( victim, gsn_dodge, TRUE, 6 );

	return( TRUE );
}


void do_dishonor( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *was_in;
	ROOM_INDEX_DATA *now_in;
	CHAR_DATA		*gch;
	int				attempt, level = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( RIDDEN(ch) )
	{
		send_to_char( "Devi chiedere al tuo cavaliere!\n\r", ch );
		return;
	}

	if ( ch->class != CLASS_SAMURAI || ch->level < LEVEL_NEWBIE+5 )
	{
		send_to_char( "Con quale onore?\n\r", ch );
		return;
	}

	if ( ch->fighting == NULL )
	{
		if ( ch->position == POS_FIGHT )
			ch->position = POS_STAND;
		send_to_char( "Non stai combattendo nessuno.\n\r", ch );
		return;
	}

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( is_same_group(gch, ch->fighting) || gch->fighting == ch )
			level += gch->level;
	}

	if ( (ch->fighting->level - ch->level) < 5 && ch->level > (level / 3) )
	{
		send_to_char( "Questa tua lotta non merita disonore.\n\r", ch );
		return;
	}

	was_in = ch->in_room;
	for ( attempt = 0; attempt < 6; attempt++ )
	{
		EXIT_DATA	*pexit;
		int			door;
		/*~~~~~~~~~~~~~~~*/

		door = number_door();
		if ((pexit = was_in->exit[door]) == 0
		||	pexit->to_room == NULL
		||	(IS_SET(pexit->exit_info, EX_CLOSED)
			&&	(!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info, EX_NOPASS))
			&&	!IS_TRUSTED(ch, LEVEL_MASTER))
		||	(IS_SET(pexit->exit_info, EX_NOFLEE))
		||	(IS_MOB(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)) )
			continue;

		move_char( ch, door, FALSE );
		if ( (now_in = ch->in_room) == was_in )
			continue;

		ch->in_room = was_in;
		act( "$n ha disonorato se stess$x con un'ignobile fuga!", ch, NULL, NULL, TO_ROOM, POS_REST );
		ch->in_room = now_in;

		if ( !IS_MOB(ch) )
		{
			act( "Disonori te stess$x e fuggi dalla battaglia.", ch, NULL, NULL, TO_CHAR, POS_REST );
			gain_exp( ch, -(ch->level)*4 );
		}
		else
			ch->last_fought = NULL;

		stop_fighting( ch, TRUE );
		if ( MOUNTED(ch) )
			do_dismount( ch, "" );

		return;
	}

	send_to_char( "PANICO! Non riesci a fuggire!!\n\r", ch );
}
