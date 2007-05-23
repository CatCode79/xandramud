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


DECLARE_SPEC_FUN( spec_special_guard );


/*
 * Local functions.
 */
int		hit_gain			( CHAR_DATA *ch );
int		mana_gain			( CHAR_DATA *ch );
int		move_gain			( CHAR_DATA *ch );
void	mobile_update		( void );
void	weather_update		( void );
void	dream_update		( void );
void	aggr_update			( void );
int		potion_cure_level	( OBJ_DATA *potion );
int		potion_arm_level	( OBJ_DATA *potion );
bool	potion_cure_blind	( OBJ_DATA *potion );
bool	potion_cure_poison	( OBJ_DATA *potion );
bool	potion_cure_disease	( OBJ_DATA *potion );
void	light_update		( void );
void	room_affect_update	( void );
void	check_reboot		( void );


/* Used for saving */
int							save_number = 0;


/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch )
{
	int		add_hp;
	int		add_mana;
	int		add_move;
	int		add_prac;
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
	{
		bug( "Advance_level: a mob to advance!", 0 );
		return;
	}

	ch->pcdata->last_level = ( ch->played + (int) (current_time - ch->logon) ) / 3600;

	add_hp = ( con_app[get_curr_stat(ch, CON)].lifep + number_range(1, 5) ) - 3;
	add_hp = ( add_hp * class_table[ch->class].hp_rate ) / 100;
	add_mana = number_range( get_curr_stat(ch, INT) / 2, (2 * get_curr_stat(ch, INT) + get_curr_stat(ch, WIS) / 5) );
	add_mana = ( add_mana * class_table[ch->class].mana_rate ) / 100;

	add_move = number_range( 1, (get_curr_stat(ch, CON) + get_curr_stat(ch, DEX)) / 6 );	/* Xandra */
	add_prac = wis_app[get_curr_stat( ch, WIS )].practice;

	add_hp = UMAX( 3, add_hp );
	add_mana = UMAX( 3, add_mana );
	add_move = UMAX( 3, add_move );

	if ( ch->sex == SEX_FEMALE )
	{
		add_hp -= 1;
		add_mana += 2;
	}

	ch->max_point[LIFE] += add_hp;
	ch->max_point[MANA] += add_mana;
	ch->max_point[MOVE] += add_move;
	ch->practice += add_prac;
	if ( number_range(0,3) == 0 )
		ch->train++;

	ch->pcdata->perm_point[LIFE] += add_hp;
	ch->pcdata->perm_point[MANA] += add_mana;
	ch->pcdata->perm_point[MOVE] += add_move;

	if ( IS_ADMIN(ch) )
	{
		sprintf( buf, "\n\rHai appena guadagnato un livello di esperienza e: %d/%d di vita, %d/%d di mana, %d/%d di movimento %d/%d di pratiche.\n\r",
			add_hp, ch->max_point[LIFE],
			add_mana, ch->max_point[MANA],
			add_move, ch->max_point[MOVE],
			add_prac, ch->practice );
		send_to_char( buf, ch );
	}
}


void gain_exp( CHAR_DATA *ch, int gain )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) || ch->level >= LEVEL_AVATAR )
		return;

	if ( IS_SET(ch->act, PLR_NO_EXP) )
	{
		send_to_char( "Non puoi guadagnare esperienza senza la tua anima.\n\r", ch );
		return;
	}

	ch->exp = UMAX( base_exp(ch, ch->pcdata->points), ch->exp + gain );

	while ( ch->level < LEVEL_AVATAR && exp_to_level(ch, ch->pcdata->points) <= 0 )
	{
		ch->level += 1;

		/* added for samurais by chronos */
		if ( ch->class == CLASS_SAMURAI && ch->level == LEVEL_NEWBIE+5 )
			ch->wimpy = 0;

		/* Level counting */
		if ( ch->level > LEVEL_NEWBIE )
			total_levels++;

		sprintf( buf, "$N ha raggiunto il %d livello!", ch->level );
		wiznet( buf, ch, NULL, WIZ_LEVELS, 0 );
		advance_level( ch );
		save_char_obj( ch );
	}
}


/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
	int gain;
	int number;
	/*~~~~~~~*/

	if ( ch->in_room == NULL )
		return( 0 );

	if ( IS_MOB(ch) )
	{
		gain = 5 + ch->level;
		if ( IS_AFFECTED(ch, AFF_REGENERATION) )
			gain *= 2;

		switch ( ch->position )
		{
		default:			gain /= 2; break;
		case POS_SLEEP:	gain = 3 * gain / 2; break;
		case POS_REST:	break;
		case POS_FIGHT:	gain /= 3; break;
		}
	}
	else
	{
		gain = UMAX( 3, 2 * get_curr_stat(ch, CON) + (7 * ch->level) / 4 );
		gain = ( gain * class_table[ch->class].hp_rate ) / 100;
		number = number_percent();
		if ( number < get_skill(ch, gsn_fast_healing) )
		{
			gain += number * gain / 100;
			if ( ch->point[LIFE] < ch->max_point[LIFE] )
				check_improve( ch, gsn_fast_healing, TRUE, 8 );
		}


		if ( number < get_skill(ch, gsn_trance) )
		{
			gain += number * gain / 150;
			if ( ch->point[MANA] < ch->max_point[MANA] )
				check_improve( ch, gsn_trance, TRUE, 8 );
		}

		switch ( ch->position )
		{
		default:			gain /= 4; break;
		case POS_SLEEP:	break;
		case POS_REST:	gain /= 2; break;
		case POS_FIGHT:	gain /= 6; break;
		}

		if ( ch->pcdata->condition[COND_HUNGER] < 0 )
			gain = 0;

		if ( ch->pcdata->condition[COND_THIRST] < 0 )
			gain = 0;
	}

	gain = gain * ch->in_room->heal_rate / 100;

	if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
		gain = gain * ch->on->value[3] / 100;

	if ( IS_AFFECTED(ch, AFF_POISON) )		gain /= 4;
	if ( IS_AFFECTED(ch, AFF_PLAGUE) )		gain /= 8;
	if ( IS_AFFECTED(ch, AFF_HASTE) )		gain /= 2;
	if ( IS_AFFECTED(ch, AFF_SLOW) )		gain *= 2;

	if ( get_curr_stat(ch, CON) > 20 )
		gain = ( gain * 14 ) / 10;

	if ( IS_SET(ch->act, PLR_HARA_KIRI) )
		gain *= 3;

	return( UMIN(gain, ch->max_point[LIFE] - ch->point[LIFE]) );
}


int mana_gain( CHAR_DATA *ch )
{
	int gain;
	int number;
	/*~~~~~~~*/

	if ( ch->in_room == NULL )
		return( 0 );

	if ( IS_MOB(ch) )
	{
		gain = 5 + ch->level;
		switch ( ch->position )
		{
		default:			gain /= 2; break;
		case POS_SLEEP:	gain = 3 * gain / 2; break;
		case POS_REST:	break;
		case POS_FIGHT:	gain /= 3; break;
		}
	}
	else
	{
		gain = get_curr_stat( ch, WIS ) + ( 2 * get_curr_stat(ch, INT) ) + ch->level;
		gain = ( gain * class_table[ch->class].mana_rate ) / 100;
		number = number_percent();
		if ( number < get_skill(ch, gsn_meditation) )
		{
			gain += number * gain / 100;
			if ( ch->point[MANA] < ch->max_point[MANA] )
				check_improve( ch, gsn_meditation, TRUE, 8 );
		}

		if ( number < get_skill(ch, gsn_trance) )
		{
			gain += number * gain / 100;
			if ( ch->point[MANA] < ch->max_point[MANA] )
				check_improve( ch, gsn_trance, TRUE, 8 );
		}

		if ( !class_table[ch->class].fMana )
			gain /= 2;

		switch ( ch->position )
		{
		default:			gain /= 4;	break;
		case POS_SLEEP:				break;
		case POS_REST:	gain /= 2;	break;
		case POS_FIGHT:	gain /= 6;	break;
		}

		if ( ch->pcdata->condition[COND_HUNGER] < 0 )
			gain = 0;

		if ( ch->pcdata->condition[COND_THIRST] < 0 )
			gain = 0;
	}

	gain = gain * ch->in_room->mana_rate / 100;

	if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
		gain = gain * ch->on->value[4] / 100;

	if ( IS_AFFECTED(ch, AFF_POISON) )		gain /= 4;
	if ( IS_AFFECTED(ch, AFF_PLAGUE) )		gain /= 8;
	if ( IS_AFFECTED(ch, AFF_HASTE) )		gain /= 2;
	if ( IS_AFFECTED(ch, AFF_SLOW) )		gain *= 2;

	if ( get_curr_stat(ch, INT) > 20 )
		gain = ( gain * 13 ) / 10;
	if ( get_curr_stat(ch, WIS) > 20 )
		gain = ( gain * 11 ) / 10;
	if ( IS_SET(ch->act, PLR_HARA_KIRI) )
		gain *= 3;

	return( UMIN(gain, ch->max_point[MANA] - ch->point[MANA]) );
}


int move_gain( CHAR_DATA *ch )
{
	int gain;
	/*~~~~~*/

	if ( ch->in_room == NULL )
		return( 0 );

	if ( IS_MOB(ch) )
	{
		gain = ch->level;
	}
	else
	{
		gain = UMAX( 15, 2 * ch->level );

		switch ( ch->position )
		{
		case POS_SLEEP:	gain += 2 * ( get_curr_stat(ch, DEX) ); break;
		case POS_REST:	gain += get_curr_stat( ch, DEX ); break;
		}

		if ( ch->pcdata->condition[COND_HUNGER] < 0 )
			gain = 3;

		if ( ch->pcdata->condition[COND_THIRST] < 0 )
			gain = 3;
	}

	gain = gain * ch->in_room->heal_rate / 100;

	if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
		gain = gain * ch->on->value[3] / 100;

	if ( IS_AFFECTED(ch, AFF_POISON) )								gain /= 4;
	if ( IS_AFFECTED(ch, AFF_PLAGUE) )								gain /= 8;
	if ( IS_AFFECTED(ch, AFF_HASTE) || IS_AFFECTED(ch, AFF_SLOW) )	gain /= 2;

	if ( get_curr_stat(ch, DEX) > 20 )
		gain *= ( 14 / 10 );

	if ( IS_SET(ch->act, PLR_HARA_KIRI) )
		gain *= 3;

	return( UMIN(gain, ch->max_point[MOVE] - ch->point[MOVE]) );
}


void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
	int			condition;
	int			damage_hunger;
	int			fdone;
	CHAR_DATA	*vch, *vch_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( value == 0 || IS_MOB(ch) || ch->level >= LEVEL_ADMIN )
		return;

	condition = ch->pcdata->condition[iCond];

	ch->pcdata->condition[iCond] = URANGE( -6, condition + value, 96 );

	if ( iCond == COND_FULL && (ch->pcdata->condition[COND_FULL] < 0) )
		ch->pcdata->condition[COND_FULL] = 0;

	if ( (iCond == COND_DRUNK) && (condition < 1) )
		ch->pcdata->condition[COND_DRUNK] = 0;

	if ( ch->pcdata->condition[iCond] < 1 && ch->pcdata->condition[iCond] > -6 )
	{
		switch ( iCond )
		{
		case COND_HUNGER:		act( "Sei affamat$x.", ch, NULL, NULL, TO_CHAR, POS_REST ); break;
		case COND_THIRST:		act( "Sei assetat$x.", ch, NULL, NULL, TO_CHAR, POS_REST ); break;
		case COND_DRUNK:		if ( condition != 0 ) act( "Sei sobri$x.", ch, NULL, NULL, TO_CHAR, POS_REST ); break;
		case COND_BLOODLUST:	if ( condition != 0 ) act( "Sei assetat$x di sangue.", ch, NULL, NULL, TO_CHAR, POS_REST ); break;
		case COND_DESIRE:		if ( condition != 0 ) send_to_char( "Hai nostalgia di casa.", ch ); break;
		}
	}

	if ( ch->pcdata->condition[iCond] == -6 && ch->level >= LEVEL_NEWBIE )
	{
		switch ( iCond )
		{
		case COND_HUNGER:
			act( "Sei molto affamat$x!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "$n e' affamat$x!", ch, NULL, NULL, TO_ROOM, POS_REST );
			damage_hunger = ch->max_point[LIFE] * number_range( 2, 4 ) / 100;
			if ( !damage_hunger )
				damage_hunger = 1;
			damage( ch, ch, damage_hunger, TYPE_HUNGER, DAM_HUNGER, TRUE );
			if ( ch->position == POS_SLEEP )
				return;
			break;

		case COND_THIRST:
			act( "Sei molto assetat$x!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "$n e' assetat$x!", ch, NULL, NULL, TO_ROOM, POS_REST );
			damage_hunger = ch->max_point[LIFE] * number_range( 2, 4 ) / 100;
			if ( !damage_hunger )
				damage_hunger = 1;
			damage( ch, ch, damage_hunger, TYPE_HUNGER, DAM_THIRST, TRUE );
			if ( ch->position == POS_SLEEP )
				return;
			break;

		case COND_BLOODLUST:
			fdone = 0;
			send_to_char( "Stai soffrendo per la sete di sangue!\n\r", ch );
			act( "$n sta soffrendo per la sete di sangue!", ch, NULL, NULL, TO_ROOM, POS_REST );
			if ( ch->in_room && ch->in_room->people && !ch->fighting )
			{
				if ( !IS_AWAKE(ch) )
					do_stand( ch, "" );
				for ( vch = ch->in_room->people; vch != NULL && ch->fighting == NULL; vch = vch_next )
				{
					vch_next = vch->next_in_room;
					if ( ch != vch && can_see(ch, vch) && !is_safe_nomessage(ch, vch) )
					{
						do_yell( ch, "SANGUE! HO BISOGNO DI SANGUE!" );
						do_murder( ch, vch->name );
						fdone = 1;
						break;
					}
				}
			}

			if ( fdone )
				break;
			damage_hunger = ch->max_point[LIFE] * number_range( 2, 4 ) / 100;
			if ( !damage_hunger )
				damage_hunger = 1;
			damage( ch, ch, damage_hunger, TYPE_HUNGER, DAM_THIRST, TRUE );
			if ( ch->position == POS_SLEEP )
				return;
			break;

		case COND_DESIRE:
			send_to_char( "Hai nostalgia di casa!\n\r", ch );
			if ( ch->position >= POS_STAND )
				move_char( ch, number_door(), FALSE );
			break;
		}
	}
}


/*
 * Mob autonomous action. This function takes 25% to 35% of ALL Merc cpu time. -- Furey
 */
void mobile_update( void )
{
	CHAR_DATA	*ch;
	CHAR_DATA	*ch_next;
	EXIT_DATA	*pexit;
	int			door;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~*/

	/* Examine all mobs. */
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next = ch->next;

		if ( IS_AFFECTED(ch, AFF_REGENERATION) && ch->in_room != NULL )
		{
			ch->point[LIFE] = UMIN( ch->point[LIFE] + ch->level / 10, ch->max_point[LIFE] );
			if ( ch->race == 15 /* troll */ )
				ch->point[LIFE] = UMIN( ch->point[LIFE] + ch->level / 10, ch->max_point[LIFE] );
			if ( ch->cabal == CABAL_ARMY && is_affected(ch, gsn_bandage) )
				ch->point[LIFE] = UMIN( ch->point[LIFE] + ch->level / 10, ch->max_point[LIFE] );
			if ( ch->point[LIFE] != ch->max_point[LIFE] )
				send_to_char( "", ch );
		}

		if ( IS_AFFECTED(ch, AFF_CORRUPTION) && ch->in_room != NULL )
		{
			ch->point[LIFE] -= ch->level / 10;
			if ( ch->point[LIFE] < 1 )
			{
				int sn = skill_lookup( "corruption" );
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if ( sn < 0 )
					sn = gsn_witch_curse;
				ch->point[LIFE] = 1;
				damage( ch, ch, 16, sn, DAM_NONE, FALSE );
				continue;
			}
			else
				send_to_char( "", ch );
		}

		if ( CAN_DETECT(ch, ADET_SUFFOCATE) && ch->in_room != NULL )
		{
			ch->point[LIFE] -= ch->level / 5;
			if ( ch->point[LIFE] < 1 )
			{
				int sn = skill_lookup( "suffocate" );
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if ( sn < 0 )
					sn = gsn_witch_curse;
				ch->point[LIFE] = 1;
				damage( ch, ch, 16, sn, DAM_NONE, FALSE );
				continue;
			}
			else
			{
				if ( number_percent() < 30 )
					send_to_char( "Non riesci a respirare!", ch );
			}
		}

		if ( ch->spec_fun == spec_special_guard )
		{
			if ( (*ch->spec_fun) (ch) )
				continue;
		}

		if ( !IS_MOB(ch) || ch->in_room == NULL || IS_AFFECTED(ch, AFF_CHARM) )
			continue;

		if ( IS_SET(ch->act, ACT_HUNTER) && ch->hunting && ch->fighting == NULL )
			hunt_victim( ch );

		if ( ch->in_room->area->empty && !IS_SET(ch->act, ACT_UPDATE_ALWAYS) )
			continue;

		/* Examine call for special procedure */
		if ( ch->spec_fun != 0 )
		{
			if ( (*ch->spec_fun) (ch) )
				continue;
		}

		if ( ch->pIndexData->pShop != NULL )
		{	/* give him some gold */
			if ( (ch->gold * 100 + ch->silver) < ch->pIndexData->wealth )
			{
				ch->gold += ch->pIndexData->wealth * number_range( 1, 20 ) / 5000000;
				ch->silver += ch->pIndexData->wealth * number_range( 1, 20 ) / 50000;
			}
		}

		/* Potion using and stuff for intelligent mobs */
		if ( ch->position == POS_STAND || ch->position == POS_REST || ch->position == POS_FIGHT )
		{
			if ( get_curr_stat(ch, INT) > 15
			&&	(	ch->point[LIFE] < ch->max_point[LIFE] *	0.9
				||	IS_AFFECTED(ch, AFF_BLIND)
				||	IS_AFFECTED(ch, AFF_POISON)
				||	IS_AFFECTED(ch, AFF_PLAGUE)
				||	ch->fighting != NULL) )
			{
				for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
				{
					if ( obj->item_type == ITEM_POTION )
					{
						if ( ch->point[LIFE] < ch->max_point[LIFE] * 0.9 )	/* hp curies */
						{
							int cl;
							/*~~~*/

							cl = potion_cure_level( obj );
							if ( cl > 0 )
							{
								if ( ch->point[LIFE] < ch->max_point[LIFE] * 0.5 && cl > 3 )
								{
									do_quaff( ch, obj->name );
									continue;
								}
								else if ( ch->point[LIFE] < ch->max_point[LIFE] * 0.7 )
								{
									do_quaff( ch, obj->name );
									continue;
								}
							}
						}

						if ( IS_AFFECTED(ch, AFF_POISON) && potion_cure_poison(obj) )
						{
							do_quaff( ch, obj->name );
							continue;
						}

						if ( IS_AFFECTED(ch, AFF_PLAGUE) && potion_cure_disease(obj) )
						{
							do_quaff( ch, obj->name );
							continue;
						}

						if ( IS_AFFECTED(ch, AFF_BLIND) && potion_cure_blind(obj) )
						{
							do_quaff( ch, obj->name );
							continue;
						}

						if ( ch->fighting != NULL )
						{
							int al;
							/*~~~*/

							al = potion_arm_level( obj );
							if ( ch->level - ch->fighting->level < 7 && al > 3 )
							{
								do_quaff( ch, obj->name );
								continue;
							}

							if ( ch->level - ch->fighting->level < 8 && al > 2 )
							{
								do_quaff( ch, obj->name );
								continue;
							}

							if ( ch->level - ch->fighting->level < 9 && al > 1 )
							{
								do_quaff( ch, obj->name );
								continue;
							}

							if ( ch->level - ch->fighting->level < 10 && al > 0 )
							{
								do_quaff( ch, obj->name );
								continue;
							}
							else
								continue;
						}
					}
				}
			}
		}

		/* That's all for sleeping / busy monster, and empty zones */
		if ( ch->position != POS_STAND )
			continue;

		if ( IS_SET(ch->progtypes, MPROG_AREA) && (ch->in_room->area->nplayer > 0) )
			( ch->pIndexData->mprogs->area_prog ) ( ch );

		if ( ch->position < POS_STAND )
			continue;

		/* Scavenge */
		if ( IS_SET(ch->act, ACT_SCAVENGER) && ch->in_room->contents != NULL && number_bits(6) == 0 )
		{
			OBJ_DATA	*obj_best;
			int			max;
			/*~~~~~~~~~~~~~~~~~~*/

			max = 1;
			obj_best = 0;
			for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
			{
				if ( CAN_WEAR(obj, ITEM_TAKE) && can_loot(ch, obj) && obj->cost > max && obj->cost > 0 )
				{
					obj_best = obj;
					max = obj->cost;
				}
			}

			if ( obj_best )
			{
				obj_from_room( obj_best );
				obj_to_char( obj_best, ch );
				act( "$n prende $p.", ch, obj_best, NULL, TO_ROOM, POS_REST );
				if ( IS_SET(obj_best->progtypes, OPROG_GET) )
					( obj_best->pIndexData->oprogs->get_prog ) ( obj_best, ch );
			}
		}

		/* Wander */
		if ( !IS_SET(ch->act, ACT_SENTINEL)
		&&	number_bits(3) == 0
		&&	(door = number_bits(5)) < MAX_DIR
		&&	!RIDDEN(ch)
		&&	(pexit = ch->in_room->exit[door]) != NULL
		&&	pexit->to_room != NULL
		&&	!IS_SET(pexit->exit_info, EX_CLOSED)
		&&	!IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		&&	(!IS_SET(ch->act, ACT_STAY_AREA) || pexit->to_room->area == ch->in_room->area)
		&&	(!IS_SET(ch->act, ACT_OUTDOORS) || !IS_SET(pexit->to_room->room_flags, ROOM_INDOORS))
		&&	(!IS_SET(ch->act, ACT_INDOORS) || IS_SET(pexit->to_room->room_flags, ROOM_INDOORS)) )
		{
			move_char( ch, door, FALSE );
		}
	}
}


int potion_cure_level( OBJ_DATA *potion )
{
	int cl;
	int i;
	/*~~~*/

	cl = 0;
	for ( i = 1; i < 5; i++ )
	{
		if ( skill_lookup("cure critical") == potion->value[i] )	cl += 3;
		if ( skill_lookup("cure light") == potion->value[i] )		cl += 1;
		if ( skill_lookup("cure serious") == potion->value[i] )		cl += 2;
		if ( skill_lookup("heal") == potion->value[i] )				cl += 4;
	}

	return( cl );
}


int potion_arm_level( OBJ_DATA *potion )
{
	int al;
	int i;
	/*~~~*/

	al = 0;
	for ( i = 1; i < 5; i++ )
	{
		if ( skill_lookup("armor") == potion->value[i] )		al += 1;
		if ( skill_lookup("shield") == potion->value[i] )		al += 1;
		if ( skill_lookup("stone skin") == potion->value[i] )	al += 2;
		if ( skill_lookup("sanctuary") == potion->value[i] )	al += 4;
		if ( skill_lookup("protection") == potion->value[i] )	al += 3;
	}

	return( al );
}


bool potion_cure_blind( OBJ_DATA *potion )
{
	int i;
	/*~~*/

	for ( i = 0; i < 5; i++ )
	{
		if ( skill_lookup("cure blindness") == potion->value[i] )
			return( TRUE );
	}

	return( FALSE );
}


bool potion_cure_poison( OBJ_DATA *potion )
{
	int i;

	/*~~*/

	for ( i = 0; i < 5; i++ )
	{
		if ( skill_lookup("cure poison") == potion->value[i] )
			return( TRUE );
	}

	return( FALSE );
}


bool potion_cure_disease( OBJ_DATA *potion )
{
	int i;
	/*~~*/

	for ( i = 0; i < 5; i++ )
	{
		if ( skill_lookup("cure disease") == potion->value[i] )
			return( TRUE );
	}

	return( FALSE );
}


#ifndef WEATHER
/*
 * Update the weather.
 */
void weather_update( void )
{
	char			buf[MSL];
	DESCRIPTOR_DATA *d;
	int				diff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	buf[0] = '\0';
	switch ( ++time_info.hour )
	{
	case 5:		weather_info.sunlight = SUN_LIGHT;	strcat( buf, "E' nato un nuovo giorno.\n\r" );		break;
	case 6:		weather_info.sunlight = SUN_RISE;	strcat( buf, "Il sole nasce verso est.\n\r" );		break;
	case 19:	weather_info.sunlight = SUN_SET;	strcat( buf, "Il sole lentamente sparisce verso ovest.\n\r" ); break;
	case 20:	weather_info.sunlight = SUN_DARK;	strcat( buf, "La notte e' iniziata.\n\r" );			break;
	case 24:	time_info.hour = 0; time_info.day++;													break;
	}

	if ( time_info.day >= 35 )
	{
		time_info.day = 0;
		time_info.month++;
	}

	if ( time_info.month >= 17 )
	{
		time_info.month = 0;
		time_info.year++;
	}

	/* Weather change. */
	if ( time_info.month >= 9 && time_info.month <= 16 )
		diff = weather_info.mmhg > 985 ? -2 : 2;
	else
		diff = weather_info.mmhg > 1015 ? -2 : 2;

	weather_info.change += diff * dice( 1, 4 ) + dice( 2, 6 ) - dice( 2, 6 );
	weather_info.change = UMAX( weather_info.change, -12 );
	weather_info.change = UMIN( weather_info.change, 12 );

	weather_info.mmhg += weather_info.change;
	weather_info.mmhg = UMAX( weather_info.mmhg, 960 );
	weather_info.mmhg = UMIN( weather_info.mmhg, 1040 );

	switch ( weather_info.sky )
	{
	default:
		bug( "Weather_update: bad sky %d.", weather_info.sky );
		weather_info.sky = SKY_CLOUDLESS;
		break;

	case SKY_CLOUDLESS:
		if ( weather_info.mmhg < 990 || (weather_info.mmhg < 1010 && number_bits(2) == 0) )
		{
			strcat( buf, "Il cielo si sta rannuvolando.\n\r" );
			weather_info.sky = SKY_CLOUDY;
		}
		break;

	case SKY_CLOUDY:
		if ( weather_info.mmhg < 970 || (weather_info.mmhg < 990 && number_bits(2) == 0) )
		{
			strcat( buf, "Comincia a piovere.\n\r" );
			weather_info.sky = SKY_RAINING;
		}

		if ( weather_info.mmhg > 1030 && number_bits(2) == 0 )
		{
			strcat( buf, "Le nubi spariscono.\n\r" );
			weather_info.sky = SKY_CLOUDLESS;
		}
		break;

	case SKY_RAINING:
		if ( weather_info.mmhg < 970 && number_bits(2) == 0 )
		{
			strcat( buf, "Un fulmine squarcia il cielo.\n\r" );
			weather_info.sky = SKY_LIGHTNING;
		}

		if ( weather_info.mmhg > 1030 || (weather_info.mmhg > 1010 && number_bits(2) == 0) )
		{
			strcat( buf, "Smette di piovere.\n\r" );
			weather_info.sky = SKY_CLOUDY;
		}
		break;

	case SKY_LIGHTNING:
		if ( weather_info.mmhg > 1010 || (weather_info.mmhg > 990 && number_bits(2) == 0) )
		{
			strcat( buf, "I fulmini cessano la loro danza.\n\r" );
			weather_info.sky = SKY_RAINING;
		}
		break;
	}

	if ( buf[0] != '\0' )
	{
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected == CON_PLAYING && IS_OUTSIDE(d->character)
			&& IS_AWAKE(d->character) )
			{
				send_to_char( buf, d->character );
			}
		}
	}
}
#endif


/*
 * Update all chars, including mobs.
 */
void char_update( void )
{
	CHAR_DATA		*ch;
	CHAR_DATA		*ch_next;
	CHAR_DATA		*ch_quit;
	static time_t	last_save_time = -1;
	bool			fTimeSync;
	int				l;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ch_quit = NULL;

	/* update save counter */
	save_number++;

	if ( save_number > 29 )
		save_number = 0;

	fTimeSync = check_time_sync();

	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		AFFECT_DATA *paf;
		AFFECT_DATA *paf_next;
		/*~~~~~~~~~~~~~~~~~~*/

		ch_next = ch->next;

		/* reset hunters path find */
		if ( !IS_MOB(ch) )
		{
			/* Time Sync due Midnight */

			if ( fTimeSync )
			{
				for( l = MAX_TIME_LOG - 1; l > 0; l--)
					ch->pcdata->log_time[l] = ch->pcdata->log_time[l - 1];

				/* Nothing for today */
				ch->pcdata->log_time[0] = 0;
			}

			if ( ch->cabal == CABAL_HUNTER )
			{
				if ( number_percent() < get_skill(ch,gsn_path_find) )
				{
					ch->point[MOVE] += ( get_skill(ch, gsn_path_find) / 4 );
					check_improve( ch, gsn_path_find, TRUE, 8 );
				}
				else
					check_improve( ch, gsn_path_find, FALSE, 16 );
			}
		}

		/* Remove caltraps effect after fight off */
		if ( is_affected(ch, gsn_caltraps) && !ch->fighting )
			affect_strip( ch, gsn_caltraps );

		/* Remove vampire effect when morning. */
		if ( IS_VAMPIRE(ch) && (weather_info.sunlight == SUN_LIGHT || weather_info.sunlight == SUN_RISE) )
			do_human( ch, "" );

		/* Reset sneak for vampire */
		if ( !(ch->fighting) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_VAMPIRE(ch) && !MOUNTED(ch) )
		{
			send_to_char( "Ricominci a muoverti sileziosamente.\n\r", ch );
			SET_BIT( ch->affected_by, AFF_SNEAK );
		}

		if ( !(ch->fighting) && !IS_AFFECTED(ch, AFF_SNEAK) && (race_table[ch->race].aff & AFF_SNEAK) && !MOUNTED(ch) )
			send_to_char( "Ricominci a muoverti sileziosamente.\n\r", ch );

		if ( !(ch->fighting) && !IS_AFFECTED(ch, AFF_HIDE) && (race_table[ch->race].aff & AFF_HIDE) && !MOUNTED(ch) )
			send_to_char( "Ritorni nell'ombra.\n\r", ch );

		SET_BIT( ch->affected_by, race_table[ch->race].aff );

		if ( !IS_MOB(ch) && IS_SET(ch->act, PLR_CHANGED_AFF) )
			REMOVE_BIT( ch->affected_by, AFF_FLYING );

		if ( MOUNTED(ch) )
			REMOVE_BIT( ch->affected_by, (AFF_IMP_INVIS|AFF_FADE|AFF_SNEAK|AFF_HIDE|AFF_CAMOUFLAGE) );

		if ( ch->timer > 20 && !IS_MOB(ch) )
			ch_quit = ch;

		if ( ch->position >= POS_STUN )
		{
			/* check to see if we need to go home */
			if ( IS_MOB(ch)
			&&	ch->zone != NULL
			&&	ch->zone != ch->in_room->area
			&&	ch->desc == NULL
			&&	ch->fighting == NULL
			&&	ch->progtypes == 0
			&&	!IS_AFFECTED(ch, AFF_CHARM)
			&&	ch->last_fought == NULL
			&&	!RIDDEN(ch)
			&&	number_percent() < 15 )
			{
				if ( ch->in_mind != NULL && ch->pIndexData->vnum > 100 )
					back_home( ch );
				else
				{
					act( "$n vaga verso casa.", ch, NULL, NULL, TO_ROOM, POS_REST );
					extract_char( ch, TRUE );
				}

				continue;
			}

			if ( ch->point[LIFE] < ch->max_point[LIFE] )
				ch->point[LIFE] += hit_gain( ch );
			else
				ch->point[LIFE] = ch->max_point[LIFE];

			if ( ch->point[MANA] < ch->max_point[MANA] )
				ch->point[MANA] += mana_gain( ch );
			else
				ch->point[MANA] = ch->max_point[MANA];

			if ( ch->point[MOVE] < ch->max_point[MOVE] )
				ch->point[MOVE] += move_gain( ch );
			else
				ch->point[MOVE] = ch->max_point[MOVE];
		}

		if ( ch->position == POS_STUN )
			update_pos( ch );

		if ( !IS_MOB(ch) && ch->level < LEVEL_ADMIN )
		{
			OBJ_DATA	*obj;
			/*~~~~~~~~~~~~~*/

			if ( (obj = get_light_char(ch)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] > 0 )
			{
				if ( --obj->value[2] == 0 && ch->in_room != NULL )
				{
					unequip_char( ch, obj );
					if ( get_light_char(ch) == NULL )
						--ch->in_room->light;
					act( "$p si spegne.", ch, obj, NULL, TO_ROOM, POS_REST );
					act( "$p vacilla e si spegne.", ch, obj, NULL, TO_CHAR, POS_REST );
					extract_obj( obj );
				}
				else if ( obj->value[2] <= 5 && ch->in_room != NULL )
					act( "$p vacilla.", ch, obj, NULL, TO_CHAR, POS_REST );
			}

			if ( IS_ADMIN(ch) )
				ch->timer = 0;

			if ( ++ch->timer >= 12 )
			{
				if ( ch->was_in_room == NULL && ch->in_room != NULL )
				{
					ch->was_in_room = ch->in_room;
					if ( ch->fighting != NULL )
						stop_fighting( ch, TRUE );
					act( "$n sparisce nel Vuoto.", ch, NULL, NULL, TO_ROOM, POS_REST );
					send_to_char( "Sparisci nel Vuoto.\n\r", ch );
					if ( ch->level > 1 )
						save_char_obj( ch );
					if ( ch->level < LEVEL_NEWBIE )
					{
						char_from_room( ch );
						char_to_room( ch, get_room_index(ROOM_VNUM_LIMBO) );
					}
				}
			}

			gain_condition( ch, COND_DRUNK, -1 );
			if ( ch->class == CLASS_VAMPIRE && ch->level > LEVEL_NEWBIE )
				gain_condition( ch, COND_BLOODLUST, -1 );
			gain_condition( ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2 );
			if ( ch->in_room->sector_type == SECT_DESERT )
				gain_condition( ch, COND_THIRST, ch->size > SIZE_MEDIUM ? -6 : -3 );
			else
				gain_condition( ch, COND_THIRST, ch->size > SIZE_MEDIUM ? -2 : -1 );
			gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1 );
		}

		for ( paf = ch->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			if ( paf->duration > 0 )
			{
				paf->duration--;

				if ( number_range(0, 4) == 0 && paf->level > 0 )
					paf->level--;

				/* spell strength fades with time */
			}
			else if ( paf->duration < 0 )
				;
			else
			{
				if ( paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0 )
				{
					if ( paf->type > 0 && skill_table[paf->type].msg_off )
					{
						send_to_char( skill_table[paf->type].msg_off, ch );
						send_to_char( "\n\r", ch );
					}
				}

				if ( paf->type == gsn_strangle )
				{
					AFFECT_DATA neck_af;
					/*~~~~~~~~~~~~~~~~*/

					REMOVE_BIT( ch->affected_by, AFF_SLEEP );
					do_wake( ch, "" );
					neck_af.type = gsn_neckguard;
					neck_af.where = TO_AFFECTS;
					neck_af.level = ch->level;
					neck_af.duration = 2 + ch->level / 50;
					neck_af.modifier = 0;
					neck_af.bitvector = 0;
					neck_af.location = APPLY_NONE;
					affect_join( ch, &neck_af );
				}
				else if ( paf->type == gsn_blackjack )
				{
					AFFECT_DATA head_af;
					/*~~~~~~~~~~~~~~~~*/

					REMOVE_BIT( ch->affected_by, AFF_SLEEP );
					do_wake( ch, "" );
					head_af.type = gsn_headguard;
					head_af.where = TO_AFFECTS;
					head_af.level = ch->level;
					head_af.duration = 2 + ch->level / 50;
					head_af.modifier = 0;
					head_af.bitvector = 0;
					head_af.location = APPLY_NONE;
					affect_join( ch, &head_af );
				}
				else if ( paf->type == gsn_vampiric_touch )
				{
					AFFECT_DATA b_af;
					/*~~~~~~~~~~~~~*/

					REMOVE_BIT( ch->affected_by, AFF_SLEEP );
					do_wake( ch, "" );
					b_af.type = gsn_blackguard;
					b_af.where = TO_AFFECTS;
					b_af.level = ch->level;
					b_af.duration = 2 + ch->level / 50;
					b_af.modifier = 0;
					b_af.bitvector = 0;
					b_af.location = APPLY_NONE;
					affect_join( ch, &b_af );
				}

				affect_remove( ch, paf );
			}
		}

		/*
		 * Careful with the damages here, MUST NOT refer to ch after damage taken, as it may be lethal damage (on
		 * NPC).
		 */
		if ( is_affected(ch, gsn_witch_curse) )
		{
			AFFECT_DATA *af, witch;
			/*~~~~~~~~~~~~~~~~~~~*/

			if ( ch->in_room == NULL )
				continue;

			act( "La maledizione delle streghe prende parte della vita di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
			send_to_char( "Senti la vita scivolarti via per colpa della maledizione delle streghe.\n\r", ch );

			for ( af = ch->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_witch_curse )
					break;
			}

			if ( af == NULL )
				continue;

			if ( af->level == 1 )
				continue;

			if ( af->modifier > -16001 )
			{
				witch.where = af->where;
				witch.type = af->type;
				witch.level = af->level;
				witch.duration = af->duration;
				witch.location = af->location;
				witch.modifier = af->modifier * 2;
				witch.bitvector = 0;

				affect_remove( ch, af );
				affect_to_char( ch, &witch );
				ch->point[LIFE] = UMIN( ch->point[LIFE], ch->max_point[LIFE] );
				if ( ch->point[LIFE] < 1 )
				{
					affect_strip( ch, gsn_witch_curse );
					ch->point[LIFE] = 1;
					damage( ch, ch, 16, gsn_witch_curse, DAM_NONE, FALSE );
					continue;
				}
			}
			else
			{
				affect_strip( ch, gsn_witch_curse );
				ch->point[LIFE] = 1;
				damage( ch, ch, 16, gsn_witch_curse, DAM_NONE, FALSE );
				continue;
			}
		}

		if ( IS_AFFECTED(ch, AFF_PLAGUE) && ch != NULL )
		{
			AFFECT_DATA *af, plague;
			CHAR_DATA	*vch;
			int			dam;
			/*~~~~~~~~~~~~~~~~~~~~*/

			if ( ch->in_room == NULL )
				continue;

			act( "$n si contorce agonizzante e piaghe pestilenti eruttano dalla sua pelle.", ch, NULL, NULL, TO_ROOM, POS_REST );
			send_to_char( "Ti contorci agonizzante per colpa delle piaghe pestilenti.\n\r", ch );
			for ( af = ch->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_plague )
					break;
			}

			if ( af == NULL )
			{
				REMOVE_BIT( ch->affected_by, AFF_PLAGUE );
				continue;
			}

			if ( af->level == 1 )
				continue;

			plague.where = TO_AFFECTS;
			plague.type = gsn_plague;
			plague.level = af->level - 1;
			plague.duration = number_range( 1, 2 * plague.level );
			plague.location = APPLY_STR;
			plague.modifier = -5;
			plague.bitvector = AFF_PLAGUE;

			for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
			{
				if (!saves_spell(plague.level + 2, vch, DAM_DISEASE)
				&&	!IS_ADMIN(vch)
				&&	!IS_AFFECTED(vch, AFF_PLAGUE)
				&&	number_bits(2) == 0 )
				{
					send_to_char( "Ti senti scottare e febbricitante.\n\r", vch );
					act( "$n sembra molto malat$x.", vch, NULL, NULL, TO_ROOM, POS_REST );
					affect_join( vch, &plague );
				}
			}

			dam = UMIN( ch->level, af->level / 5 + 1 );
			ch->point[MANA] -= dam;
			ch->point[MOVE] -= dam;
			if ( number_percent() < 70 )
				damage( ch, ch, dam + UMAX(ch->max_point[LIFE] / 20, 50), gsn_plague, DAM_DISEASE, TRUE );
			else
				damage( ch, ch, dam, gsn_plague, DAM_DISEASE, FALSE );
		}
		else if ( IS_AFFECTED(ch, AFF_POISON) && ch != NULL && !IS_AFFECTED(ch, AFF_SLOW) )
		{
			AFFECT_DATA *poison;
			/*~~~~~~~~~~~~~~~~*/

			poison = affect_find( ch->affected, gsn_poison );

			if ( poison != NULL )
			{
				act( "$n e' ammalat$x e sofferente.", ch, NULL, NULL, TO_ROOM, POS_REST );
				act( "Sei ammalat$x e sofferente.", ch, NULL, NULL, TO_CHAR, POS_REST );
				damage( ch, ch, poison->level / 10 + 1, gsn_poison, DAM_POISON, TRUE );
			}
		}
		else if ( ch->position == POS_INCAP && number_range(0, 1) == 0 )
		{
			damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE );
		}
		else if ( ch->position == POS_MORTAL )
		{
			damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE );
		}
	}

	/* Autosave and autoquit. Check that these chars still exist. */
	if ( last_save_time == -1 || current_time - last_save_time > 300 )
	{
		last_save_time = current_time;
		for ( ch = char_list; ch != NULL; ch = ch_next )
		{
			ch_next = ch->next;
			if ( !IS_MOB(ch) )
				save_char_obj( ch );
			if ( ch == ch_quit || ch->timer > 20 )
				do_quit( ch, "" );
		}
	}

	if ( fTimeSync )
		limit_time = current_time;
}


void water_float_update( void )
{
	OBJ_DATA	*obj_next;
	OBJ_DATA	*obj;
	CHAR_DATA	*ch;
	bool		fChar;
	/*~~~~~~~~~~~~~~~~~~*/

	for ( obj = object_list; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next;

		if ( obj == NULL )
		{
			dump_to_scr( "NULL OBJ encounter!\n\r" );
			break;
		}

		if ( obj->in_room == NULL )
			continue;

		if ( IS_WATER(obj->in_room) )
		{
			fChar = FALSE;
			ch = obj->in_room->people;
			if ( ch != NULL )
				fChar = TRUE;
			if ( obj->water_float != -1 )
				obj->water_float--;

			if ( obj->water_float < 0 )
				obj->water_float = -1;

			if ( obj->item_type == ITEM_DRINK_CON )
			{
				obj->value[1] = URANGE( 1, obj->value[1] + 8, obj->value[0] );
				if ( fChar )
				{
					act( "$p fa le bolle nell'acqua.", ch, obj, NULL, TO_CHAR, POS_REST );
					act( "$p fa le bolle nell'acqua.", ch, obj, NULL, TO_ROOM, POS_REST );
				}

				obj->water_float = obj->value[0] - obj->value[1];
				obj->value[2] = 0;
			}

			/* (RR) sistema di materiale-scioglimento */

			if ( obj->water_float == 0 )
			{
				if ( ((obj->item_type == ITEM_CORPSE_NPC)
					|| (obj->item_type == ITEM_CORPSE_PC)
					|| (obj->item_type == ITEM_CONTAINER))
					&&	fChar )
				{
					act( "$p s'immerge sottacqua lasciando dietro se' delle bolle.", ch, obj, NULL, TO_CHAR, POS_REST );
					act( "$p s'immerge sottacqua lasciando dietro se' delle bolle.", ch, obj, NULL, TO_ROOM, POS_REST );
				}
				else if ( fChar )
				{
					act( "$p s'immerge sottacqua.", ch, obj, NULL, TO_CHAR, POS_REST );
					act( "$p s'immerge sottacqua.", ch, obj, NULL, TO_ROOM, POS_REST );
				}

				extract_obj( obj );
				continue;
			}
		}
	}
}


/*
 * Update all objs. This function is performance sensitive.
 */
void obj_update( void )
{
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	OBJ_DATA	*t_obj,
				*pit,
				*next_obj;
	AFFECT_DATA *paf,
				*paf_next;
	static int	pit_count = 1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( obj = object_list; obj != NULL; obj = obj_next )
	{
		CHAR_DATA	*rch;
		char		*message;
		/*~~~~~~~~~~~~~~~~~*/

		obj_next = obj->next;

		/* go through affects and decrement */
		for ( paf = obj->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			if ( paf->duration > 0 )
			{
				paf->duration--;
				if ( number_range(0, 4) == 0 && paf->level > 0 )
					paf->level--;		/* spell strength fades with time */
			}
			else if ( paf->duration < 0 )
				;
			else
			{
				if ( paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0 )
				{
					if ( paf->type > 0 && skill_table[paf->type].msg_obj )
					{
						if ( obj->carried_by != NULL )
						{
							rch = obj->carried_by;
							act( skill_table[paf->type].msg_obj, rch, obj, NULL, TO_CHAR, POS_REST );
						}

						if ( obj->in_room != NULL && obj->in_room->people != NULL )
						{
							rch = obj->in_room->people;
							act( skill_table[paf->type].msg_obj, rch, obj, NULL, TO_ALL, POS_REST );
						}
					}
				}

				affect_remove_obj( obj, paf );
			}
		}

		for ( t_obj = obj; t_obj->in_obj; t_obj = t_obj->in_obj )
			;

		if ( IS_SET(obj->progtypes, OPROG_AREA) )
		{
			if ( (t_obj->in_room != NULL && (t_obj->in_room->area->nplayer > 0))
			||	(t_obj->carried_by && t_obj->carried_by->in_room && t_obj->carried_by->in_room->area->nplayer > 0) )
				( obj->pIndexData->oprogs->area_prog ) ( obj );
		}

		if ( check_material(obj, "ice") )
		{
			if ( obj->carried_by != NULL )
			{
				if ( obj->carried_by->in_room->sector_type == SECT_DESERT )
				{
					if ( number_percent() < 40 )
					{
						act( "Il caldo torrido fonde $p.", obj->carried_by, obj, NULL, TO_CHAR, POS_REST );
						extract_obj( obj );
						continue;
					}
				}
			}
			else if ( obj->in_room != NULL )
			{
				if ( obj->in_room->sector_type == SECT_DESERT )
				{
					if ( number_percent() < 50 )
					{
						if ( obj->in_room->people != NULL )
						{
							act( "Il caldo torrido fonde $p.", obj->in_room->people, obj, NULL, TO_ROOM, POS_REST );
							act( "Il caldo torrido fonde $p.", obj->in_room->people, obj, NULL, TO_CHAR, POS_REST );
						}

						extract_obj( obj );
						continue;
					}
				}
			}
		}

		if ( !check_material(obj, "glass") && obj->item_type == ITEM_POTION )
		{
			if ( obj->carried_by != NULL )
			{
				if ( obj->carried_by->in_room->sector_type == SECT_DESERT && !IS_MOB(obj->carried_by) )
				{
					if ( number_percent() < 20 )
					{
						act( "$p evapora.", obj->carried_by, obj, NULL, TO_CHAR, POS_REST );
						extract_obj( obj );
						continue;
					}
				}
			}
			else if ( obj->in_room != NULL )
			{
				if ( obj->in_room->sector_type == SECT_DESERT )
				{
					if ( number_percent() < 30 )
					{
						if ( obj->in_room->people != NULL )
						{
							act( "$p evapora per il caldo torrido.", obj->in_room->people, obj, NULL, TO_ROOM, POS_REST );
							act( "$p evapora per il caldo torrido.", obj->in_room->people, obj, NULL, TO_CHAR, POS_REST );
						}

						extract_obj( obj );
						continue;
					}
				}
			}
		}

		if ( obj->condition > -1 && (obj->timer <= 0 || --obj->timer > 0) )
		{
			if ( obj->in_room
			&&	 obj->in_room->sector_type == SECT_AIR
			&&  (obj->wear_flags & ITEM_TAKE)
			&&	 obj->in_room->exit[5]
			&&	 obj->in_room->exit[5]->to_room )
			{
				ROOM_INDEX_DATA *new_room = obj->in_room->exit[5]->to_room;

				if ( (rch = obj->in_room->people) != NULL )
				{
					act( "$p cade verso il basso.", rch, obj, NULL, TO_ROOM, POS_REST );
					act( "$p cade verso il basso.", rch, obj, NULL, TO_CHAR, POS_REST );
				}

				obj_from_room( obj );
				obj_to_room( obj, new_room );

				if ( (rch = obj->in_room->people) != NULL )
				{
					act( "$p cade dall'alto.", rch, obj, NULL, TO_ROOM, POS_REST );
					act( "$p cade dall'alto.", rch, obj, NULL, TO_CHAR, POS_REST );
				}
			}

			continue;
		}

		switch ( obj->item_type )
		{
		default:
			message = "$p si sbriciola in polvere.";
			break;

		case ITEM_FOUNTAIN:
			message = "$p si asciuga.";
			break;

		case ITEM_CORPSE_NPC:
			message = "$p si decompone in polvere.";
			break;

		case ITEM_CORPSE_PC:
			message = "$p si decompone in polvere.";
			break;

		case ITEM_FOOD:
			message = "$p si decompone.";
			break;

		case ITEM_POTION:
			message = "$p evapora per il disuso.";
			break;

		case ITEM_PORTAL:
			message = "$p scompare in un lampo.";
			break;

		case ITEM_CONTAINER:
			if ( CAN_WEAR(obj, ITEM_WEAR_FLOAT) )
			{
				if ( obj->contains )
					message = "$p vibra e svanisce, facendo cadere il suo contenuto sul pavimento.";
				else
					message = "$p vibra e svanisce.";
			}
			else
				message = "$p si sbriciola in polvere.";
			break;
		}

		if ( obj->carried_by != NULL )
		{
			if ( IS_MOB(obj->carried_by) && obj->carried_by->pIndexData->pShop != NULL )
				obj->carried_by->silver += obj->cost / 5;
			else
			{
				act( message, obj->carried_by, obj, NULL, TO_CHAR, POS_REST );
				if ( obj->wear_loc == WEAR_FLOAT )
					act( message, obj->carried_by, obj, NULL, TO_ROOM, POS_REST );
			}
		}
		else if ( obj->in_room != NULL && (rch = obj->in_room->people) != NULL )
		{
			if ( !(obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT && !CAN_WEAR(obj->in_obj, ITEM_TAKE)) )
			{
				act( message, rch, obj, NULL, TO_ROOM, POS_REST );
				act( message, rch, obj, NULL, TO_CHAR, POS_REST );
			}
		}

		++pit_count;
		pit_count = pit_count %120;	/* more or less an hour */
		if ( obj->pIndexData->vnum == OBJ_VNUM_PIT && pit_count == 121 )
		{
			for ( t_obj = obj->contains; t_obj != NULL; t_obj = next_obj )
			{
				next_obj = t_obj->next_content;
				obj_from_obj( t_obj );
				extract_obj( t_obj );
			}
		}

		if ( (obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT) && obj->contains )
		{	/* save the contents */
			for ( t_obj = obj->contains; t_obj != NULL; t_obj = next_obj )
			{
				next_obj = t_obj->next_content;
				obj_from_obj( t_obj );

				if ( obj->in_obj )					/* in another object */
					obj_to_obj( t_obj, obj->in_obj );
				else if ( obj->carried_by )
				{									/* carried */
					if ( obj->wear_loc == WEAR_FLOAT )
					{
						if ( obj->carried_by->in_room == NULL )
							extract_obj( t_obj );
						else
							obj_to_room( t_obj, obj->carried_by->in_room );
					}
					else
						obj_to_char( t_obj, obj->carried_by );
				}
				else if ( obj->in_room == NULL )	/* destroy it */
					extract_obj( t_obj );
				else
				{	/* to the pit */
					for ( pit = get_room_index(obj->altar)->contents; pit != NULL && pit->pIndexData->vnum != obj->pit; pit = pit->next )
						;

					if ( pit == NULL )
						obj_to_room( t_obj, obj->in_room );
					else
						obj_to_obj( t_obj, pit );
				}
			}
		}

		extract_obj( obj );
	}
}


/*
 * Aggress. for each mortal PC for each mob in room aggress on some random PC This function takes 25% to 35% of ALL
 *    Merc cpu time. Unfortunately, checking on each PC move is too tricky, because we don't the mob to just attack the
 *    first PC who leads the party into the room. -- Furey
 */
void aggr_update( void )
{
	CHAR_DATA	*wch;
	CHAR_DATA	*wch_next;
	CHAR_DATA	*ch;
	CHAR_DATA	*ch_next;
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	CHAR_DATA	*victim;
	char		buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( wch = char_list; wch != NULL; wch = wch_next )
	{
		if ( !IS_VALID(wch) )
		{
			bug( "Aggr_update: Invalid char.", 0 );
			break;
		}

		wch_next = wch->next;

		if ( IS_AFFECTED(wch, AFF_BLOODTHIRST) && IS_AWAKE(wch) && wch->fighting == NULL )
		{
			for ( vch = wch->in_room->people; vch != NULL && wch->fighting == NULL; vch = vch_next )
			{
				vch_next = vch->next_in_room;
				if ( wch != vch && can_see(wch, vch) && !is_safe_nomessage(wch, vch) )
				{
					act( "$CPIU' SANGUE! HO BISOGNO DI PIU' SANGUE!!!$c",
						wch, NULL, NULL, TO_CHAR, POS_REST, RED );
					do_murder( wch, vch->name );
				}
			}
		}

		if ( wch->cabal != CABAL_NONE && IS_MOB(wch) )
		{
			for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
			{
				ch_next = ch->next_in_room;
				if ( !IS_MOB(ch) && !IS_ADMIN(ch) && ch->cabal != wch->cabal && ch->fighting == NULL )
					multi_hit( wch, ch, TYPE_UNDEFINED );
			}

			continue;
		}

		if ( IS_MOB(wch) || wch->level >= LEVEL_ADMIN || wch->in_room == NULL || wch->in_room->area->empty )
			continue;

		for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
		{
			int count;
			/*~~~~~~*/

			ch_next = ch->next_in_room;

			if ( !IS_MOB(ch)
			||	(!IS_SET(ch->act, ACT_AGGRESSIVE) && (ch->last_fought == NULL))
			||	IS_SET(ch->in_room->room_flags, ROOM_SAFE)
			||	IS_AFFECTED(ch, AFF_CALM)
			||	ch->fighting != NULL
			||	RIDDEN(ch)
			||	IS_AFFECTED(ch, AFF_CHARM)
			||	IS_AFFECTED(ch, AFF_SCREAM)
			||	!IS_AWAKE(ch)
			||	(IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch))
			||	!can_see(ch, wch)
			||	number_bits(1) == 0
			||	is_safe_nomessage(ch, wch) )
				continue;

			/* Mad mob attacks! */
			if ( ch->last_fought == wch )
			{
				sprintf( buf, "%s! Muori!",
					(is_affected(wch, gsn_doppelganger) && !IS_SET(ch->act, PLR_HOLYLIGHT)) ? PERS(wch->doppel, ch) : PERS(wch, ch) );
				do_yell( ch, buf );
				wch = check_guard( wch, ch );

				multi_hit( ch, wch, TYPE_UNDEFINED );
				continue;
			}

			if ( ch->last_fought != NULL )
				continue;

			/*
			 * Ok we have a 'wch' player character and a 'ch' npc aggressor. Now make the aggressor fight a RANDOM pc
			 * victim in the room, giving each 'vch' an equal chance of selection.
			 */
			count = 0;
			victim = NULL;
			for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
			{
				vch_next = vch->next_in_room;

				if ( !IS_MOB(vch)
				&&	vch->level < LEVEL_ADMIN
				&&	ch->level >= vch->level - 5
				&&	(!IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch))
				&&	can_see(ch, vch)
				&&	vch->class != CLASS_VAMPIRE /* do not attack vampires */
				&&	!(IS_GOOD(ch) && IS_GOOD(vch)) )	/* good vs good :( */
				{
					if ( number_range(0, count) == 0 )
						victim = vch;
					count++;
				}
			}

			if ( victim == NULL )
				continue;

			if ( !is_safe_nomessage(ch, victim) )
			{
				victim = check_guard( victim, ch );
				if ( IS_SET(ch->off_flags, OFF_BACKSTAB) && get_wield_char(ch, FALSE) )
					multi_hit( ch, victim, gsn_backstab );
				else
					multi_hit( ch, victim, TYPE_UNDEFINED );
			}
		}
	}
}


/*
 * Handle all kinds of updates. Called once per pulse from game loop. Random times to defeat tick-timing clients and
 *    players.
 */
void update_handler( void )
{
	static int	pulse_area;
	static int	pulse_mobile;
	static int	pulse_violence;
	static int	pulse_point;
	static int	pulse_water_float;
	static int	pulse_raffect;
	static int	pulse_track;
	static int	pulse_dream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( --pulse_area <= 0 )
	{
		wiznet( "area & room tick!", NULL, NULL, WIZ_TICKS, 0 );
		pulse_area = PULSE_AREA;
		area_update( );
		room_update( );
	}

	if ( --pulse_mobile <= 0 )
	{
		pulse_mobile = PULSE_MOBILE;
		mobile_update( );
		light_update( );
	}

	if ( --pulse_violence <= 0 )
	{
		pulse_violence = PULSE_VIOLENCE;
		violence_update( );
	}

	if ( --pulse_water_float <= 0 )
	{
		pulse_water_float = PULSE_WATER_FLOAT;
		water_float_update( );
	}

	if ( --pulse_raffect <= 0 )
	{
		pulse_raffect = PULSE_RAFFECT;
		room_affect_update( );
	}

	if ( --pulse_dream <= 0 )
	{
		pulse_dream = PULSE_DREAM;
		dream_update( );
	}

	if ( --pulse_track <= 0 )
	{
		pulse_track = PULSE_TRACK;
		track_update( );
	}

	if ( --pulse_point <= 0 )
	{
		CHAR_DATA	*ch;
		/*~~~~~~~~~~~~*/

		pulse_point = PULSE_TICK;

		wiznet( "char tick!", NULL, NULL, WIZ_TICKS, 0 );

		weather_update( );
		char_update( );
		quest_update( );
		obj_update( );
		check_reboot( );

		/* room counting */
		for ( ch = char_list; ch != NULL; ch = ch->next )
		{
			if ( !IS_MOB(ch) && ch->in_room != NULL )
				ch->in_room->area->count = UMIN( ch->in_room->area->count + 1, 5000000 );
		}
	}

	aggr_update( );
	auction_update( );

	tail_chain( );
}


void light_update( void )
{
	CHAR_DATA		*ch;
	int				dam_light;
	DESCRIPTOR_DATA *d, *d_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( d = descriptor_list; d != NULL; d = d_next )
	{
		d_next = d->next;
		if ( d->connected != CON_PLAYING )
			continue;

		ch = ( d->original != NULL ) ? d->original : d->character;

		if ( IS_ADMIN(ch) )
			continue;

		if ( ch->class != CLASS_VAMPIRE )
			continue;

		/* also checks vampireness */
		if ( (dam_light = isn_dark_safe(ch)) == 0 )
			continue;

		if ( dam_light != 2 && number_percent() < get_skill(ch, gsn_light_res) )
		{
			check_improve( ch, gsn_light_res, TRUE, 32 );
			continue;
		}

		if ( dam_light == 1 )
			send_to_char( "La luce della stanza ti disturba.\n\r", ch );
		else
			send_to_char( "La luce del sole ti da' fastidio.\n\r", ch );

		dam_light = ( ch->max_point[LIFE] * 4 ) / 100;
		if ( !dam_light )
			dam_light = 1;
		damage( ch, ch, dam_light, TYPE_HUNGER, DAM_LIGHT_V, TRUE );

		if ( ch->position == POS_STUN )
			update_pos( ch );

		if ( number_percent() < 10 )
			gain_condition( ch, COND_DRUNK, -1 );
	}
}


void room_update( void )
{
	ROOM_INDEX_DATA *room;
	ROOM_INDEX_DATA *room_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( room = top_affected_room; room; room = room_next )
	{
		AFFECT_DATA *paf;

		AFFECT_DATA *paf_next;
		/*~~~~~~~~~~~~~~~~~~*/

		room_next = room->aff_next;

		for ( paf = room->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			if ( paf->duration > 0 )
			{
				paf->duration--;

				/*
				 * if (number_range(0,4) == 0 && paf->level > 0) paf->level--; spell strength shouldn't fade with
				 * time because checks safe_rpsell with af->level
				 */
			}
			else if ( paf->duration < 0 )
				;
			else
			{
				if ( paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0 )
				{
					/*
					 * if ( paf->type > 0 && skill_table[paf->type].msg_off ) { act( skill_table[paf->type].msg_off,
					 * ch, NULL, NULL, TO_ROOM, POS_REST ); send_to_char( "\n\r", ch ); }
					 */
				}

				affect_remove_room( room, paf );
			}
		}
	}
}


void room_affect_update( void )
{
	ROOM_INDEX_DATA *room;
	ROOM_INDEX_DATA *room_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( room = top_affected_room; room; room = room_next )
	{
		room_next = room->aff_next;

		while ( IS_ROOM_AFFECTED(room, AFF_ROOM_PLAGUE) && room->people != NULL )
		{
			AFFECT_DATA *af, plague;
			CHAR_DATA	*vch;
			/*~~~~~~~~~~~~~~~~~~~~*/

			for ( af = room->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_black_death )
					break;
			}

			if ( af == NULL )
			{
				REMOVE_BIT( room->affected_by, AFF_ROOM_PLAGUE );
				break;
			}

			if ( af->level == 1 )
				af->level = 2;

			plague.where = TO_AFFECTS;
			plague.type = gsn_plague;
			plague.level = af->level - 1;
			plague.duration = number_range( 1, ((plague.level / 2) + 1) );
			plague.location = APPLY_NONE;
			plague.modifier = -5;
			plague.bitvector = AFF_PLAGUE;

			for ( vch = room->people; vch != NULL; vch = vch->next_in_room )
			{
				if (!saves_spell(plague.level, vch, DAM_DISEASE)
				&&	!IS_ADMIN(vch)
				&&	!is_safe_rspell(af->level, vch)
				&&	!IS_AFFECTED(vch, AFF_PLAGUE)
				&&	number_bits(3) == 0 )
				{
					send_to_char( "Ti senti scottare e tremi per la febbre.\n\r", vch );
					act( "$n sembra molto ammalat$x.", vch, NULL, NULL, TO_ROOM, POS_REST );
					affect_join( vch, &plague );
				}
			}
			break;
		}

		while ( IS_ROOM_AFFECTED(room, AFF_ROOM_POISON) && room->people != NULL )
		{
			AFFECT_DATA *af, paf;
			CHAR_DATA	*vch;
			/*~~~~~~~~~~~~~~~~~*/

			for ( af = room->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_deadly_venom )
					break;
			}

			if ( af == NULL )
			{
				REMOVE_BIT( room->affected_by, AFF_ROOM_POISON );
				break;
			}

			if ( af->level == 1 )
				af->level = 2;

			paf.where = TO_AFFECTS;
			paf.type = gsn_poison;
			paf.level = af->level - 1;
			paf.duration = number_range( 1, ((paf.level / 5) + 1) );
			paf.location = APPLY_NONE;
			paf.modifier = -5;
			paf.bitvector = AFF_POISON;

			for ( vch = room->people; vch != NULL; vch = vch->next_in_room )
			{
				if (!saves_spell(paf.level, vch, DAM_POISON)
				&&	!IS_ADMIN(vch)
				&&	!is_safe_rspell(af->level, vch)
				&&	!IS_AFFECTED(vch, AFF_POISON)
				&&	number_bits(3) == 0 )
				{
					act( "Ti senti molto ammalat$x.", vch, NULL, NULL, TO_CHAR, POS_REST );
					act( "$n sembra molto ammalat$x.", vch, NULL, NULL, TO_ROOM, POS_REST );
					affect_join( vch, &paf );
				}
			}
			break;
		}

		while ( IS_ROOM_AFFECTED(room, AFF_ROOM_SLOW) && room->people != NULL )
		{
			AFFECT_DATA *af, paf;
			CHAR_DATA	*vch;
			/*~~~~~~~~~~~~~~~~~*/

			for ( af = room->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_lethargic_mist )
					break;
			}

			if ( af == NULL )
			{
				REMOVE_BIT( room->affected_by, AFF_ROOM_SLOW );
				break;
			}

			if ( af->level == 1 )
				af->level = 2;

			paf.where = TO_AFFECTS;
			paf.type = gsn_slow;
			paf.level = af->level - 1;
			paf.duration = number_range( 1, ((paf.level / 5) + 1) );
			paf.location = APPLY_NONE;
			paf.modifier = -5;
			paf.bitvector = AFF_SLOW;

			for ( vch = room->people; vch != NULL; vch = vch->next_in_room )
			{
				if (!saves_spell(paf.level, vch, DAM_OTHER)
				&&	!IS_ADMIN(vch)
				&&	!is_safe_rspell(af->level, vch)
				&&	!IS_AFFECTED(vch, AFF_SLOW)
				&&	number_bits(3) == 0 )
				{
					send_to_char( "Ti muovi meno velocemente di prima.\n\r", vch );
					act( "$n sembra che si muova meno velocemente.", vch, NULL, NULL, TO_ROOM, POS_REST );
					affect_join( vch, &paf );
				}
			}
			break;
		}

		while ( IS_ROOM_AFFECTED(room, AFF_ROOM_SLEEP) && room->people != NULL )
		{
			AFFECT_DATA *af, paf;
			CHAR_DATA	*vch;
			/*~~~~~~~~~~~~~~~~~*/

			for ( af = room->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_mysterious_dream )
					break;
			}

			if ( af == NULL )
			{
				REMOVE_BIT( room->affected_by, AFF_ROOM_SLEEP );
				break;
			}

			if ( af->level == 1 )
				af->level = 2;

			paf.where = TO_AFFECTS;
			paf.type = gsn_sleep;
			paf.level = af->level - 1;
			paf.duration = number_range( 1, ((paf.level / 10) + 1) );
			paf.location = APPLY_NONE;
			paf.modifier = -5;
			paf.bitvector = AFF_SLEEP;

			for ( vch = room->people; vch != NULL; vch = vch->next_in_room )
			{
				if (!saves_spell(paf.level - 4, vch, DAM_CHARM)
				&&	!IS_ADMIN(vch)
				&&	!is_safe_rspell(af->level, vch)
				&&	!(IS_MOB(vch) && IS_SET(vch->act, ACT_UNDEAD))
				&&	!IS_AFFECTED(vch, AFF_SLEEP)
				&&	number_bits(3) == 0 )
				{
					if ( IS_AWAKE(vch) )
					{
						act( "Ti senti cosi' assonnato.. Ronf.", vch, NULL, NULL, TO_CHAR, POS_REST );
						act( "$n comincia a dorminre.", vch, NULL, NULL, TO_ROOM, POS_REST );
						vch->position = POS_SLEEP;
					}

					affect_join( vch, &paf );
				}
			}
			break;
		}

		while ( IS_ROOM_AFFECTED(room, AFF_ROOM_ESPIRIT) && room->people != NULL )
		{
			AFFECT_DATA *af, paf;
			CHAR_DATA	*vch;
			/*~~~~~~~~~~~~~~~~~*/

			for ( af = room->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_evil_spirit )
					break;
			}

			if ( af == NULL )
			{
				REMOVE_BIT( room->affected_by, AFF_ROOM_ESPIRIT );
				break;
			}

			if ( af->level == 1 )
				af->level = 2;

			paf.where = TO_AFFECTS;
			paf.type = gsn_evil_spirit;
			paf.level = af->level;
			paf.duration = number_range( 1, (paf.level / 30) );
			paf.location = APPLY_NONE;
			paf.modifier = 0;
			paf.bitvector = 0;

			for ( vch = room->people; vch != NULL; vch = vch->next_in_room )
			{
				if (!saves_spell(paf.level + 2, vch, DAM_MENTAL)
				&&	!IS_ADMIN(vch)
				&&	!is_safe_rspell(af->level, vch)
				&&	!is_affected(vch, gsn_evil_spirit)
				&&	number_bits(3) == 0 )
				{
					act( "Ti senti piu' cattiv$x del solito.", vch, NULL, NULL, TO_CHAR, POS_REST );
					affect_join( vch, &paf );
				}
			}
			break;
		}

/* new ones here
		while (IS_ROOM_AFFECTED(room, AFF_ROOM_) && room->people != NULL)
		{
			AFFECT_DATA *af, paf;
			CHAR_DATA *vch;

			for ( af = room->affected; af != NULL; af = af->next )
			{
				if ( af->type == gsn_ )
					break;
			}

			if ( af == NULL )
			{
				REMOVE_BIT(room->affected_by,AFF_ROOM_);
				break;
			}

			if ( af->level == 1 )
				af->level = 2;

			paf.where			= TO_AFFECTS;
			paf.type		= gsn_;
			paf.level		= af->level - 1;
			paf.duration	= number_range(1,((paf.level/5)+1));
			paf.location	= APPLY_NONE;
			paf.modifier	= -5;
			paf.bitvector   = AFF_;

			for ( vch = room->people; vch != NULL; vch = vch->next_in_room)
			{
				if (!saves_spell(paf.level + 2,vch,DAM_)
				&&  !IS_ADMIN(vch)
				&&  !is_safe_rspell(af->level,vch)
				&&  !IS_AFFECTED(vch,AFF_) && number_bits(3) == 0)
				{
					send_to_char("Ti senti scottare e brividi di febbre.\n\r",vch);
					act("$n sembra molto malat$x.",vch,NULL,NULL,TO_ROOM, POS_REST);
					affect_join(vch,&paf);
				}
			}
	 		break;
		}
*/
	}
}


void check_reboot( void )
{
	char			buf[MSL];
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch ( reboot_counter )
	{
	default:
		reboot_counter--;
		break;

	case -1:
		break;

	case 0:
		reboot_xandra( TRUE, XANDRA_REBOOT );
		return;

	case 15:
	case 10:
	case 5:
	case 4:
	case 3:
	case 2:
		sprintf( buf, "\007***** REBOOT IN %i MINUTI *****\007\n\r", reboot_counter );
		for ( d = descriptor_list; d != NULL; d = d->next )
			write_to_buffer( d, buf, 0 );
		reboot_counter--;
		break;

	case 1:
		sprintf( buf, "\007***** REBOOT IN %i MINUTO *****\007\n\r", reboot_counter );
		for ( d = descriptor_list; d != NULL; d = d->next )
			write_to_buffer( d, buf, 0 );
		reboot_counter--;
		break;
	}
}


void track_update( void )
{
	CHAR_DATA	*ch, *ch_next;
	CHAR_DATA	*vch, *vch_next;
	char		buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next = ch->next;

		if (IS_MOB(ch)
		&&	!IS_AFFECTED(ch, AFF_CALM)
		&&	!IS_AFFECTED(ch, AFF_CHARM)
		&&	ch->fighting == NULL
		&&	ch->in_room != NULL
		&&	IS_AWAKE(ch)
		&&	!IS_SET(ch->act, ACT_NOTRACK)
		&&	!RIDDEN(ch)
		&&	!IS_AFFECTED(ch, AFF_SCREAM) )
		{
			if ( ch->last_fought != NULL && ch->in_room != ch->last_fought->in_room )
				do_track( ch, ch->last_fought->name );
			else if ( ch->in_mind != NULL )
			{
				for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
				{
					vch_next = vch->next_in_room;

					if ( ch == vch )
						continue;
					if ( !IS_ADMIN(vch) && can_see(ch, vch)
					&& !is_safe_nomessage(ch, vch) && is_name(vch->name, ch->in_mind) )
					{
						sprintf( buf, "Eccoti, finalmente ti scovo, %s", vch->name );
						do_yell( ch, buf );
						do_murder( ch, vch->name );
						break;	/* one fight at a time */
					}
				}
			}
		}
	}
}
