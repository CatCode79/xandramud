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


#include <sys/types.h>
#include <stdio.h>

#include "xandra.h"
#include "interp.h"
#include "recycle.h"


DECLARE_SPELL_FUN( spell_blindness );
DECLARE_SPELL_FUN( spell_poison );


/*
 * Procedura per il disarm, lista dei check da effettuare per chi invoca il comando.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim, int disarm_second )
{
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~*/

	if ( disarm_second )
	{
		if ( (obj = get_wield_char(victim, TRUE)) == NULL )
		{
			bug( "Disarm second with NULL DUAL_WIELD", 0 );
			return;
		}
	}
	else
	{
		if ( (obj = get_wield_char(victim, FALSE)) == NULL )
		{
			bug( "Disarm first with NULL WEAR_WIELD", 0 );
			return;
		}
	}

	if ( IS_OBJ_STAT(obj, ITEM_NOREMOVE) )
	{
		act( "Non riesci a smuovere l'arma di su$X!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "La tua arma non si smuove nonostante il tentativo di $n di disarmarti.", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n prova a disarmare $N ma il suo tentativo e' vano.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		return;
	}

	if ( !skill_failure_nomessage(victim, gsn_grip, 0) )
	{
		int skill = get_skill( victim, gsn_grip );
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		skill += ( get_curr_stat(victim, STR) - get_curr_stat(ch, STR) ) * 5;
		if ( number_percent() < skill )
		{
			act( "$N stringe a tal punto la sua arma che non riesci a disarmarl$X!",
				ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n cerca di disarmarti ma la tua stretta e' ben salda sull'arma!",
				ch, NULL, victim, TO_VICT, POS_REST );
			act( "$n cerca di disarmare $N ma fallisce miseramente.",
				ch, NULL, victim, TO_NOTVICT, POS_REST );
			check_improve( victim, gsn_grip, TRUE, 1 );
			return;
		}
		else
			check_improve( victim, gsn_grip, FALSE, 1 );
	}

	act( "$n ti  $CDISARMA$c, la tua arma cade a terra!",
		ch, NULL, victim, TO_VICT, POS_FIGHT, CYAN );
	act( "Riesci a $CDISARMARE$c $N, ottima tecnica!",
		ch, NULL, victim, TO_CHAR, POS_FIGHT, CYAN_BOLD );
	act( "$n riesce a $Cdisarmare$c $N!",
		ch, NULL, victim, TO_NOTVICT, POS_FIGHT, CYAN_BOLD );

	obj_from_char( obj );
	if ( IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_INVENTORY) )
		obj_to_char( obj, victim );
	else
	{
		obj_to_room( obj, victim->in_room );
		if ( IS_MOB(victim) && victim->wait == 0 && can_see_obj(victim, obj) )
		get_obj( victim, obj, NULL );
	}

/*	if ( (obj2 = get_wield_char(victim,TRUE)) != NULL )
	{
		act( "$CImpugni l'arma nella tua seconda mano nella principale!.$c",
			ch, NULL, victim, TO_VICT, POS_FIGHT, CYAN );
		act( "$C$N impugna l'arma nella seconda mano nella principale!$c",
			ch, NULL, victim, TO_CHAR, POS_FIGHT, CYAN_BOLD );
		act( "$C$N impugna l'arma nella seconda mano come principale!$c",
			ch, NULL, victim, TO_NOTVICT, POS_FIGHT, CYAN_BOLD );

		unequip_char( victim, obj2); equip_char( victim, obj2 , WEAR_WIELD);
	}
*/
}


void do_berserk( CHAR_DATA *ch, char *argument )
{
	int chance, hp_percent;
	/*~~~~~~~~~~~~~~~~~~~*/

    if ( skill_failure_check(ch, gsn_berserk, 0, OFF_BERSERK, "Non e' il momento di scherzare, con chi vorresti prendertela?") )
		return;

    chance = get_skill( ch, gsn_berserk );

	if ( IS_AFFECTED(ch, AFF_BERSERK) || is_affected(ch, gsn_berserk) || is_affected(ch, skill_lookup("frenzy")) )
	{
		send_to_char( "L'ira gia' riempie il tuo corpo.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CALM) )
	{
		send_to_char( "Incollerirsi? Proprio ora? Non senti che pace interiore.. ?\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 50 )
	{
		act( "Stanc$x come sei non e' proprio il momento per incollerirsi..", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	/* modifiers */
	/* fighting */
	if ( ch->position == POS_FIGHT )
		chance += 10;

	/* damage -- below 50% of hp helps, above hurts */
	hp_percent = 100 * ch->point[LIFE] / ch->max_point[LIFE];
	chance += 25 - hp_percent / 2;

	if ( number_percent() < chance )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		WAIT_STATE( ch, PULSE_VIOLENCE );
		ch->point[MANA] -= 50;
		ch->point[MOVE] /= 2;

		/* heal a little damage */
		ch->point[LIFE] += ch->level * 2;
		ch->point[LIFE] = UMIN( ch->point[LIFE], ch->max_point[LIFE] );

		send_to_char( "Senti un'insana rabbia pervaderti il corpo dandoti forza!\n\r", ch );
		act( "$C$n viene circondat$x da un'aura d'ira!$c", ch, NULL, NULL, TO_ROOM, POS_FIGHT, RED );
		check_improve( ch, gsn_berserk, TRUE, 2 );

		af.where = TO_AFFECTS;
		af.type = gsn_berserk;
		af.level = ch->level;
		af.duration = number_fuzzy( ch->level / 8 );
		af.modifier = UMAX( 1, ch->level / 5 );
		af.bitvector = AFF_BERSERK;

		af.location = APPLY_HITROLL;
		affect_to_char( ch, &af );

		af.location = APPLY_DAMROLL;
		affect_to_char( ch, &af );

		af.modifier = UMAX( 10, 10 * (ch->level / 5) );
		af.location = APPLY_AC;
		affect_to_char( ch, &af );
	}
	else
	{
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		ch->point[MANA] -= 25;
		ch->point[MOVE] /= 2;

		send_to_char( "Non riesci a richiamare la tua ira..\n\r", ch );
		check_improve( ch, gsn_berserk, FALSE, 2 );
	}
}


void do_bash( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			chance, wait;
	bool		FightingCheck;
	int			damage_bash;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->fighting != NULL )
		FightingCheck = TRUE;
	else
		FightingCheck = FALSE;

	argument = one_argument( argument, arg );
	if ( arg[0] != '\0' && !str_cmp(arg, "door") && !str_cmp(arg, "porta") )
	{
		do_bash_door( ch, argument );
		return;
	}

	if ( skill_failure_check(ch, gsn_bash, 0, OFF_BASH, "Spingere? Sicur$x di saperlo fare?") )
		return;

	chance = get_skill( ch, gsn_bash );

	if ( arg[0] == '\0' )
	{
		victim = ch->fighting;
		if ( victim == NULL )
		{
			send_to_char( "Un muro? Una porta? O chi? Non stai mica combattendo..\n\r", ch );
			return;
		}
	}
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Umh non e' qui..\n\r", ch );
		return;
	}

	if ( victim->position < POS_FIGHT )
	{
		act( "Lascia prima che si rialzi da terra, ok?", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Gli Dei ci crearono saggiamente in maniera tale da non poterci prendere a calci da soli..e nemmeno spingere.\n\r", ch );
		return;
	}

	if ( MOUNTED(victim) )
	{
		send_to_char( "E' a cavallo, non riuscirai mai a spingerlo in queste condizioni!\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "Oh no, $N e' la tua guida..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_protective_shield) )
	{
		act( "$CUn campo di forza sembra proteggere $N dalla tua spinta..$c",
			ch, NULL, victim, TO_CHAR, POS_FIGHT, YELLOW );
		act( "$CCerca di buttarti a terra ma il tuo scudo protettivo respinge il suo attacco.$c",
			ch, NULL, victim, TO_VICT, POS_FIGHT, YELLOW );
		act( "$C$n Cerca di buttare a terra $N con uno spintone ma viene respint$n da una strana forza.$c",
			ch, NULL, victim, TO_NOTVICT, POS_FIGHT, YELLOW );
		return;
	}

	/* modifiers */
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

	if ( IS_AFFECTED(ch, AFF_FLYING) ) chance -= 10;

	/* speed */
	if ( IS_SET(ch->off_flags, OFF_FAST) )
		chance += 10;
	if ( IS_SET(victim->off_flags, OFF_FAST) )
		chance -= 20;

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* now the attack */
	if ( number_percent() < chance )
	{
		act( "$n ti spinge giu' con una vigorosa spallata!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "Spingi a terra $N con una vigorosa spallata!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n butta a terra $N con una potente spallata!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_bash, TRUE, 1 );

		wait = 3;

		switch ( number_bits(2) )
		{
		case 0: wait = 1; break;
		case 1: wait = 2; break;
		case 2: wait = 4; break;
		case 3: wait = 3; break;
		}

		WAIT_STATE( victim, wait * PULSE_VIOLENCE );
		WAIT_STATE( ch, skill_table[gsn_bash].beats );
		damage_bash = ( ch->damroll / 2 ) + number_range( 4, 4 + 4 * ch->size + chance / 10 );
		damage( ch, victim, damage_bash, gsn_bash, DAM_BASH, TRUE );
		victim->position = POS_REST;
	}
	else
	{
		damage( ch, victim, 0, gsn_bash, DAM_BASH, TRUE );
		act( "$N evita la tua spinta! Perdi l'equilibrio e cadi a terra!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$N evita la spinta di $n che non riesce a mantenere l'equilibrio e cade!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		act( "Eviti la spinta di $n che cade a terra al posto tuo!", ch, NULL, victim, TO_VICT, POS_REST );
		check_improve( ch, gsn_bash, FALSE, 1 );
		WAIT_STATE( ch, skill_table[gsn_bash].beats * 3 / 2 );
		ch->position = POS_REST;
	}
}


void do_dirt( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			chance;
	bool		FightingCheck;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( skill_failure_check(ch, gsn_dirt, 0, OFF_KICK_DIRT, "Sprofondi con un piede calciando per terra.. sicur$x che fosse cosi'?") )
		return;

	if ( ch->fighting != NULL )
		FightingCheck = TRUE;
	else
		FightingCheck = FALSE;

	one_argument( argument, arg );

    chance = get_skill(ch, gsn_dirt);

	if ( arg[0] == '\0' )
	{
		victim = ch->fighting;
		if ( victim == NULL )
		{
			send_to_char( "Prova a farlo durante un combattimento!\n\r", ch );
			return;
		}
	}
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non e' qui, meglio non sollevare inutili polveroni eh?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_FLYING) )
	{
		send_to_char( "No, da quest'altezza non riesci a toccar terra..\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_BLIND) )
	{
		act( "Nah, e' gia' priv$X della vista..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Fai prima a immergere la testa nella sabbia.\n\r", ch );
		return;
	}

	if ( MOUNTED(victim) )
	{
		act( "Non riesci a colpirl$X negli occhi mentre monta la sua cavalcatura!",
			ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "Ma no.. $N e' la tua guida, ricordi?", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	/* modifiers */
	/* dexterity */
	chance += get_curr_stat( ch, DEX );
	chance -= 2 * get_curr_stat( victim, DEX );

	/* speed */
	if ( IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE) )
		chance += 10;
	if ( IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, OFF_FAST) )
		chance -= 25;

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	if ( chance % 5 == 0 ) chance += 1;

	/* terrain */
	switch ( ch->in_room->sector_type )
	{
	case ( SECT_INSIDE ):		chance -= 20;	break;
	case ( SECT_CITY ):			chance -= 10;	break;
	case ( SECT_FIELD ):		chance += 5;	break;
	case ( SECT_FOREST ):						break;
	case ( SECT_HILLS ):						break;
	case ( SECT_MOUNTAIN ):		chance -= 10;	break;
	case ( SECT_WATER_SWIM ):	chance = 0;		break;
	case ( SECT_WATER_NOSWIM ): chance = 0;		break;
	case ( SECT_AIR ):			chance = 0;		break;
	case ( SECT_DESERT ):		chance += 10;	break;
	}

	if ( chance == 0 )
	{
		send_to_char( "Non c'e' terra qui..\n\r", ch );
		return;
	}

	/* now the attack */
	if ( number_percent() < chance )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		act( "$n viene accecat$x dalla terra negli occhi!", victim, NULL, NULL, TO_ROOM, POS_REST );
		damage( ch, victim, number_range(2, 5), gsn_dirt, DAM_NONE, TRUE );
		act( "$C$n scalcia della terra nei tuoi occhi! Non riesci piu' a vedere nulla!$c", ch, NULL, victim, TO_VICT, POS_REST, BROWN );
		check_improve( ch, gsn_dirt, TRUE, 2 );
		WAIT_STATE( ch, skill_table[gsn_dirt].beats );

		af.where = TO_AFFECTS;
		af.type = gsn_dirt;
		af.level = ch->level;
		af.duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = -4;
		af.bitvector = AFF_BLIND;

		affect_to_char( victim, &af );
	}
	else
	{
		damage( ch, victim, 0, gsn_dirt, DAM_NONE, TRUE );
		check_improve( ch, gsn_dirt, FALSE, 2 );
		WAIT_STATE( ch, skill_table[gsn_dirt].beats );
	}

	if ( IS_MOB(victim) && victim->position > POS_STUN && !FightingCheck )
	{
			do_yell( victim, "Aiuto! Mi sta attaccando!" );
	}
}


void do_trip( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			chance;
	bool		FightingCheck;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_trip, 0, OFF_TRIP, "Ah ah.. sicur$x d'averlo imparato?") )
		return;

	if ( ch->fighting != NULL )
		FightingCheck = TRUE;
	else
		FightingCheck = FALSE;

	one_argument( argument, arg );

	chance = get_skill(ch, gsn_dirt);

	if ( arg[0] == '\0' )
	{
		victim = ch->fighting;
		if ( victim == NULL )
		{
			send_to_char( "Non stai combattendo ora!\n\r", ch );
			return;
		}
	}
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "La tua vittima non e' qui.\n\r", ch );
		return;
	}

	if ( MOUNTED(victim) )
	{
		send_to_char( "Come vorresti sgambettare qualcuno che monta una cavalcatura?!\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(victim, AFF_FLYING) )
	{
		act( "Se avesse i piedi per terra magari sarebbe piu' facile.. o meglio, possibile.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim->position < POS_FIGHT )
	{
		act( "$N e' gia' a terra, aspetta che si rialzi.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim == ch )
	{
		act( "Ma cosa.. ecco, intralciati pure da sol$x!", ch, NULL, NULL, TO_CHAR, POS_REST );
		WAIT_STATE( ch, 2 * skill_table[gsn_trip].beats );
		act( "$n incrocia i piedi e perde l'equilibrio cadendo rovinosamente..",
			ch, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "$N e' la tua guida, non lo faresti mai. Per ora.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	/* modifiers */
	/* size */
	if ( ch->size < victim->size )
		chance += ( ch->size - victim->size ) * 10;	/* bigger = harder to trip */

	/* dex */
	chance += get_curr_stat( ch, DEX );
	chance -= get_curr_stat( victim, DEX ) * 3 / 2;

	if ( IS_AFFECTED(ch, AFF_FLYING) )
		chance -= 10;

	/* speed */
	if ( IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE) )
		chance += 10;
	if ( IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE) )
		chance -= 20;

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* now the attack */
	if ( number_percent() < chance )
	{
		act( "$n ti sgambetta mandandoti a terra!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "Fai perdere l'equilibrio a $N con uno sgambetto!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n manda a terra $N con uno sgambetto!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_trip, TRUE, 1 );

		WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
		WAIT_STATE( ch, skill_table[gsn_trip].beats );
		victim->position = POS_REST;
		damage( ch, victim, number_range(2, 2 + 2 * victim->size), gsn_trip, DAM_BASH, TRUE );
	}
	else
	{
		damage( ch, victim, 0, gsn_trip, DAM_BASH, TRUE );
		WAIT_STATE( ch, skill_table[gsn_trip].beats * 2 / 3 );
		check_improve( ch, gsn_trip, FALSE, 1 );
	}

	if ( IS_MOB(victim) && victim->position > POS_STUN && !FightingCheck )
	{
			do_yell( victim, "Aiuto! Mi sta attaccando!" );
	}
}


void do_backstab( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_backstab, 0, 0, "Pugnalare alle spalle eh? Si, certo.. un'altra volta..") )
		return;

	if ( arg[0] == '\0' )
	{
		send_to_char( "Ok, lo ammazziamo.. ma chi??\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "La tua adorata vittima non e' qui.. avra' presagito qualcosa?\n\r", ch );
		return;
	}

	if ( IS_MOB(ch) && !IS_MOB(victim) )
		return;

	if ( victim == ch )
	{
		send_to_char( "Non fai prima a buttarti da un ponte?\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( (obj = get_wield_char(ch, FALSE)) == NULL || attack_table[obj->value[3]].damage != DAM_PIERCE )
	{
		send_to_char( "Non ti serve una mazza per pugnalare alle spalle, devi avere una daga o un coltello!\n\r", ch );
		return;
	}

	if ( victim->fighting != NULL )
	{
		send_to_char( "La tua vittima non dev'essere impegnata in un combattimento per essere sorpresa alle spalle.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_backstab].beats );

	if ( victim->point[LIFE] < (0.7 * victim->max_point[LIFE]) && (IS_AWAKE(victim)) )
	{
		act( "$N sembra troppo inquiet$X e sospettos$X.. aspetta un altro momento per colpire.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( current_time - victim->last_fight_time < 300 && IS_AWAKE(victim) )
	{
		act( "$N sembra troppo inquiet$X e sospettos$X.. aspetta un altro momento per colpire.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !IS_AWAKE(victim) || IS_MOB(ch) || number_percent() < get_skill(ch, gsn_backstab) )
	{
		check_improve( ch, gsn_backstab, TRUE, 1 );
		if ( !IS_MOB(ch) && number_percent() < (get_skill(ch, gsn_dual_backstab) / 10) * 8 )
		{
			check_improve( ch, gsn_dual_backstab, TRUE, 1 );
			one_hit( ch, victim, gsn_backstab, FALSE );
			one_hit( ch, victim, gsn_dual_backstab, FALSE );
		}
		else
		{
			check_improve( ch, gsn_dual_backstab, FALSE, 1 );
			multi_hit( ch, victim, gsn_backstab );
		}
	}
	else
	{
		check_improve( ch, gsn_backstab, FALSE, 1 );
		damage( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE );
	}

	/* Mob yells if he doesn't die */
	if ( IS_MOB(victim) && victim->position == POS_FIGHT )
	{
		char temp[MSL];

		sprintf( temp, "Aaarghhh maledett%c!!", (ch->sex == SEX_FEMALE) ? 'a' : 'o' );
		do_yell( victim, temp );
	}
}


void do_cleave( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_cleave, 0, 0, "Si, adesso spacchiamo in due con.. senti ma chi te l'avrebbe insegnato?") )
		return;

	one_argument( argument, arg );

	if ( ch->master != NULL && IS_MOB(ch) )
		return;

	if ( arg[0] == '\0' )
	{
		send_to_char( "Si, ora siamo pronti a colpire.. ma chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Guarda meglio.. non e' qui.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Eh ti vuoi cosi' male?\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( (obj = get_wield_char(ch, FALSE)) == NULL )
	{
		send_to_char( "Ok, ma a mani nude mi sembra un po' un suicidio.\n\r", ch );
		return;
	}

	if ( victim->fighting != NULL )
	{
		send_to_char( "Sta combattendo.. aspetta che finisca, no?\n\r", ch );
		return;
	}

	if ( (victim->point[LIFE] < (0.9 * victim->max_point[LIFE])) && (IS_AWAKE(victim)) )
	{
		act( "$N e' ferit$X e sospettos$X, meglio aspettare un altro momento.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_cleave].beats );
	if ( !IS_AWAKE(victim) || IS_MOB(ch) || number_percent() < get_skill(ch, gsn_cleave) )
	{
		check_improve( ch, gsn_cleave, TRUE, 1 );
		multi_hit( ch, victim, gsn_cleave );
	}
	else
	{
		check_improve( ch, gsn_cleave, FALSE, 1 );
		damage( ch, victim, 0, gsn_cleave, DAM_NONE, TRUE );
	}

	/* Mob yells if he doesn't die */
	if ( IS_MOB(victim) && victim->position == POS_FIGHT )
	{
			do_yell( victim, "Argh! Maledizione!!" );
	}
}


void do_ambush( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_ambush, 0, 0, "Umh.. e dove l'avresti imparato?") )
		return;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Ok, siamo pronti.. ma chi e' la vittima?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Ehi.. guarda che non e' qui.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Magnifica idea.. da sbatterci la testa contro una quercia.\n\r", ch );
		return;
	}

	if ( !IS_AFFECTED(ch, AFF_CAMOUFLAGE) || can_see(victim, ch) )
	{
		send_to_char( "Un po' difficile dato che ti ha visto..\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	WAIT_STATE( ch, skill_table[gsn_ambush].beats );
	if ( !IS_AWAKE(victim) || IS_MOB(ch) || number_percent() < get_skill(ch, gsn_ambush) )
	{
		check_improve( ch, gsn_ambush, TRUE, 1 );
		multi_hit( ch, victim, gsn_ambush );
	}
	else
	{
		check_improve( ch, gsn_ambush, FALSE, 1 );
		damage( ch, victim, 0, gsn_ambush, DAM_NONE, TRUE );
	}

	/* Mob yells if he doesn't die */
	if ( IS_MOB(victim) && victim->position == POS_FIGHT )
	{
			do_yell( victim, "Aiuto! Mi sta attaccando!" );
	}
}


void do_rescue( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	CHAR_DATA	*fch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Che coraggio! Ma chi vuoi salvare?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Chi? Ma non e' qui!\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Aiutati che il ciel ti aiuta?\n\r", ch );
		return;
	}

	if ( !IS_MOB(ch) && IS_MOB(victim) )
	{
		send_to_char( "Non vuole il tuo aiuto!\n\r", ch );
		return;
	}

	if ( ch->fighting == victim )
	{
		send_to_char( "Spiacente, troppo tardi..\n\r", ch );
		return;
	}

	if ( (fch = victim->fighting) == NULL )
	{
		send_to_char( "Mmm non mi sembra che stia combattendo ed in pericolo di vita..\n\r", ch );
		return;
	}

	if ( IS_MOB(ch) && ch->master != NULL && IS_MOB(victim) )
		return;
	if ( is_safe(ch, fch) )
		return;

	if ( ch->master != NULL )
	{
		if ( is_safe(ch->master, fch) )
			return;
	}

	WAIT_STATE( ch, skill_table[gsn_rescue].beats );
	if ( (!IS_MOB(ch) && number_percent() > get_skill(ch, gsn_rescue)) || (victim->level > (ch->level + 30)) )
	{
		act( "Non riesci a salvarl$X! Ritenta, presto!.", ch, NULL, victim, TO_CHAR, POS_REST );
		check_improve( ch, gsn_rescue, FALSE, 1 );
		return;
	}

	act( "Sposti $N combattendo al suo posto!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n ti sposta combattendo al tuo posto!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$n rescues $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	check_improve( ch, gsn_rescue, TRUE, 1 );

	stop_fighting( fch, FALSE );
	stop_fighting( victim, FALSE );

	set_fighting( ch, fch );
	set_fighting( fch, ch );
}


void do_kick( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	int			kick_dam;
	int			chance;
	/*~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_kick, 0, OFF_KICK, "Non appena sollevi la gamba ti prende un crampo.. o forse non hai le calzature giuste..") )
		return;

	if ( (victim = ch->fighting) == NULL )
	{
		send_to_char( "Non stai combattendo, conserva le energie.\n\r", ch );
		return;
	}

	chance = number_percent();
	if ( IS_AFFECTED(ch, AFF_FLYING) )
		chance *= 1.1;
	WAIT_STATE( ch, skill_table[gsn_kick].beats );
	if ( IS_MOB(ch) || chance < get_skill(ch, gsn_kick) )
	{
		kick_dam = number_range( 1, ch->level );
		if ( (ch->class == CLASS_SAMURAI) && (get_eq_char(ch, WEAR_FEET) == NULL) )
			kick_dam *= 2;
		kick_dam += ch->damroll / 2;
		damage( ch, victim, kick_dam, gsn_kick, DAM_BASH, TRUE );
		check_improve( ch, gsn_kick, TRUE, 1 );
	}
	else
	{
		damage( ch, victim, 0, gsn_kick, DAM_BASH, TRUE );
		check_improve( ch, gsn_kick, FALSE, 1 );
	}
}


void do_circle( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*person;
	bool		second;
	/*~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_circle, 0, 0, "Non l'hai imparato da nessuna parte.." ) )
		return;

	if ( (victim = ch->fighting) == NULL )
	{
		send_to_char( "Vuoi combattere qualcuno o danzare in cerchio con un pugnale in mano?\n\r", ch );
		return;
	}

	second = FALSE;
	if ( (get_wield_char(ch, FALSE) == NULL) || attack_table[get_wield_char(ch, FALSE)->value[3]].damage != DAM_PIERCE )
	{
		if ( (get_wield_char(ch, TRUE) == NULL)
		||	 attack_table[get_wield_char(ch, TRUE)->value[3]].damage != DAM_PIERCE )
		{
			send_to_char( "Ti serve un'arma adatta.. prova una daga o un coltello.\n\r", ch );
			return;
		}

		second = TRUE;
	}

	if ( is_safe(ch, victim) )
		return;

	WAIT_STATE( ch, skill_table[gsn_circle].beats );

	for ( person = ch->in_room->people; person != NULL; person = person->next_in_room )
	{
		if ( person->fighting == ch )
		{
			send_to_char( "Non credo che il tuo avversario si lasci aggirare cosi' facilmente..\n\r", ch );
			return;
		}
	}

	if ( IS_MOB(ch) || number_percent() < get_skill(ch, gsn_circle) )
	{
		one_hit( ch, victim, gsn_circle, second );
		check_improve( ch, gsn_circle, TRUE, 1 );
	}
	else
	{
		damage( ch, victim, 0, gsn_circle, TYPE_UNDEFINED, TRUE );
		check_improve( ch, gsn_circle, FALSE, 1 );
	}
}


void do_disarm( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	int			chance, hth, ch_weapon, vict_weapon, ch_vict_weapon, disarm_second = 0;
	char		arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_disarm, 0, OFF_DISARM, "Disarmare? Tu? Ma se al massimo hai tagliato le unghie al tuo gatto..!") )
		return;

	hth = 0;

	if ( ch->master != NULL && IS_MOB(ch) )
		return;

	chance = get_skill( ch, gsn_disarm );

	if ( ch->class != CLASS_NINJA
	&&	(get_wield_char(ch, FALSE) == NULL
		|| ((IS_MOB(ch)
		&& !IS_SET(ch->off_flags, OFF_DISARM)))) )
	{
		send_to_char( "Devi impugnare un'arma per poter disarmare.\n\r", ch );
		return;
	}

	/* Xandra: controllo per i ninja, disarmano solo a mani nude */
	if ( ch->class == CLASS_NINJA
	&&	( get_wield_char(ch, FALSE) != NULL
		||	get_skill(ch, gsn_hand_to_hand) == 0
		||	(IS_MOB(ch) && !IS_SET(ch->off_flags, OFF_DISARM))) )
	{
		send_to_char( "Non hai la possibilita' di utilizzare le tue arti per disarmare.\n\r", ch );
		return;
	}

	/* Acquisisce la nuova hth per i ninja basandosi sulla skill hand to hand */
	if ( ch->class == CLASS_NINJA )
		hth = get_skill( ch, gsn_hand_to_hand );

	if ( (victim = ch->fighting) == NULL )
	{
		send_to_char( "Non stai combattendo contro nessuno.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );
	if ( !IS_MOB(ch) && arg[0] != '\0' )
	{
		if ( is_name(arg, "second seconda") )
			disarm_second = 1;
		else
			disarm_second = 0;
	}

	if ( (obj = get_wield_char(victim, disarm_second)) == NULL )
	{
		act( "Certo! Disarmiamol$X! Se avesse un'arma magari..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	/* find weapon skills */
	ch_weapon = get_weapon_skill( ch, get_weapon_sn(ch, FALSE) );

	vict_weapon = get_weapon_skill( victim, get_weapon_sn(victim, disarm_second) );
	ch_vict_weapon = get_weapon_skill( ch, get_weapon_sn(victim, disarm_second) );

	/* modifiers */
	/* skill */
	if ( get_wield_char(ch, FALSE) == NULL )
		chance = chance * hth / 150;
	else
		chance = chance * ch_weapon / 100;


	chance += ( ch_vict_weapon / 2 - vict_weapon ) / 2;

	/* dex vs. strength */
	chance += get_curr_stat( ch, DEX );
	chance -= 2 * get_curr_stat( victim, STR );

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* and now the attack */
	if ( number_percent() < chance )
	{
		WAIT_STATE( ch, skill_table[gsn_disarm].beats );
		disarm( ch, victim, disarm_second );
		check_improve( ch, gsn_disarm, TRUE, 1 );
	}
	else
	{
		WAIT_STATE( ch, skill_table[gsn_disarm].beats );
		act( "$N sfugge al tuo tentativo di disarmarl$X.. ", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n cerca di disarmarti.. ma ci vuole altro!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n cerca di disarmare $N ma senza successo..", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_disarm, FALSE, 1 );
	}
}


void do_nerve( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	char		arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_nerve, 0, 0, "Umh.. no, non sei proprio in grado di fare niente di simile!") )
		return;

	if ( ch->fighting == NULL )
	{
		send_to_char( "Ehi, guarda che non stai combattendo contro nessuno.\n\r", ch );
		return;
	}

	victim = ch->fighting;

	if ( is_safe(ch, victim) )
		return;

	if ( is_affected(ch, gsn_nerve) )
	{
		act( "L'hai gia' privat$X delle sue forze, farlo un'altra volta e' inutile.",
			ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_nerve].beats );

	if ( IS_MOB(ch) || number_percent() < (get_skill(ch, gsn_nerve) + ch->level + get_curr_stat(ch, DEX)) / 2 )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		af.where = TO_AFFECTS;
		af.type = gsn_nerve;
		af.level = ch->level;
		af.duration = ch->level * PULSE_VIOLENCE / PULSE_TICK;
		af.location = APPLY_STR;
		af.modifier = -3;
		af.bitvector = 0;

		affect_to_char( victim, &af );
		act( "Indebolisci $N con un colpo diretto alla base del collo.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n ti colpisce alla base del collo! Senti le forze venirti meno!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n colpisce $N alla base del collo privandol$X delle forze!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_nerve, TRUE, 1 );
	}
	else
	{
		send_to_char( "$N schiva il tuo colpo diretto al collo..\n\r", ch );
		act( "$n cerca di colpire un centro nervoso sul tuo collo.. ma riesci a evitare agilmente il colpo!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n cerca di colpire $N sul collo ma e' troppo lent$x..", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_nerve, FALSE, 1 );
	}

	multi_hit( victim, ch, TYPE_UNDEFINED );

	if ( IS_MOB(victim) && victim->position != POS_FIGHT )
	{
			do_yell( victim, "Aiuto! Mi sta attaccando!" );
	}
}


void do_endure( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_endure, 0, 0, "Non ti senti ancora pront$x.. riprova.") )
		return;

	if ( is_affected(ch, gsn_endure) )
	{
		act( "Ehi, ti ricordo che sei gia' concentrat$x al massimo.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_endure].beats );

	af.where = TO_AFFECTS;
	af.type = gsn_endure;
	af.level = ch->level;
	af.duration = ch->level / 4;
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1 * ( get_skill(ch, gsn_endure) / 10 );
	af.bitvector = 0;

	affect_to_char( ch, &af );

	send_to_char( "Richiami la tua energia interna in maniera che riempia e protegga il tuo corpo dalla magia.\n\r", ch );
	act( "$n rovescia il capo mentre le sue pupille spariscono, il suo sguardo e' ora piu' determinato.", ch, NULL, NULL, TO_ROOM, POS_REST );
	check_improve( ch, gsn_endure, TRUE, 1 );
}


void do_tame( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	char		arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_tame, 1, 0, "Naa.. non fa per te.") )
		return;

	if ( arg[0] == '\0' )
	{
		send_to_char( "Si.. cerca di darti una calmata!\n\r", ch );
		act( "$n innalza lo sguardo al cielo cercando di calmarsi..", ch, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Mmm si, mi sembra proprio pericoloso.. ma chi? Non lo trovo qui!\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) )
	{
		act( "$N non ne ha di certo bisogno..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !IS_SET(victim->act, ACT_AGGRESSIVE) )
	{
		act( "$N non sembra mica aggressiv$X..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_tame].beats );

	if ( number_percent() < get_skill(ch, gsn_tame) + 15 + 4 * (ch->level - victim->level) )
	{
		REMOVE_BIT( victim->act, ACT_AGGRESSIVE );
		SET_BIT( victim->affected_by, AFF_CALM );
		send_to_char( "Improvvisamente un'insolita pace raggiunge i tuoi sensi.\n\r", victim );
		act( "Riesci a sopire i bollenti spiriti di $N.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n riesce a calmare $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		stop_fighting( victim, TRUE );
		check_improve( ch, gsn_tame, TRUE, 1 );
	}
	else
	{
		act( "Fallisci nel tuo tentativo di calmare $N.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n cerca senza successo di calmare $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		act( "$n cerca senza successo di calmarti.", ch, NULL, victim, TO_VICT, POS_REST );
		check_improve( ch, gsn_tame, FALSE, 1 );
	}
}


void do_assassinate( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_assassinate, 0, 0, "Credo che nessuno ti abbia insegnato a farlo.. prova qualcosa di diverso su!" ) )
		return;

	one_argument( argument, arg );

	if ( ch->master != NULL && IS_MOB(ch) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		send_to_char( "Oh..e vorresti uccidere la tua amata guida? Oh no.. non ora perlomeno..\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Si! Assassiniamo! Mhhh.. scusa.. chi dobbiamo assassinare?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Adesso gli strappiamo il coll.. ehi, guarda che non e' qui!\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "E' ancora presto per suicidarsi, Ninja.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_ADMIN(victim) && !IS_MOB(victim) )
	{
		send_to_char( "L'immortale si fa beffe di te!\n\r", ch );
		return;
	}

	if ( victim->fighting != NULL )
	{
		send_to_char( "Assassinare una persona mentre combatte? Inventa qualcos'altro!\n\r", ch );
		return;
	}

	if ( (get_wield_char(ch, FALSE) != NULL) || (get_hold_char(ch) != NULL) )
	{
		send_to_char( "Libera le tue mani dai gingilli, devi afferrargli il collo..\n\r", ch );
		return;
	}

	if ( (victim->point[LIFE] < victim->max_point[LIFE]) && (can_see(victim, ch)) && (IS_AWAKE(victim)) )
	{
		act( "$N e' ferit$X e sospettos$X.. non ti conviene avvicinarti.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

/*	if (IS_SET(victim->imm_flags, IMM_WEAPON))
	{
		act("$N seems immune to your assassination attempt.", ch, NULL, victim, TO_CHAR, POS_REST);
		act("$N seems immune to $n's assassination attempt.", ch, NULL, victim, TO_ROOM, POS_REST);
		return;
	}
*/
	WAIT_STATE( ch, skill_table[gsn_assassinate].beats );
	if ( IS_MOB(ch) || !IS_AWAKE(victim) || number_percent() < get_skill(ch, gsn_assassinate) )
		multi_hit( ch, victim, gsn_assassinate );
	else
	{
		check_improve( ch, gsn_assassinate, FALSE, 1 );
		damage( ch, victim, 0, gsn_assassinate, DAM_NONE, TRUE );
	}

	/* Mob yells if he doesn't die */
	if ( IS_MOB(victim) && victim->position == POS_FIGHT )
			do_yell( victim, "Argh! Tentano di assassinarmi!" );
}


void do_caltraps( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim = ch->fighting;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_caltraps, 0, 0, "Che? No no.. non e' roba tua, pensane un'altra.") )
		return;

	if ( victim == NULL )
	{
		send_to_char( "Mmm e' una tecnica da combattimento.. e tu non stai combattendo.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) ) return;

	act( "Getti degli spuntoni acuminati ai piedi di $N!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n ti getta contro degli spuntoni acuminati! Occhio a non calpestarli!", ch, NULL, victim, TO_VICT, POS_REST );

	WAIT_STATE( ch, skill_table[gsn_caltraps].beats );

	if ( !IS_MOB(ch) && number_percent() >= get_skill(ch, gsn_caltraps) )
	{
		damage( ch, victim, 0, gsn_caltraps, DAM_PIERCE, TRUE );
		check_improve( ch, gsn_caltraps, FALSE, 1 );
		return;
	}

	damage( ch, victim, ch->level, gsn_caltraps, DAM_PIERCE, TRUE );

	if ( !is_affected(victim, gsn_caltraps) )
	{
		AFFECT_DATA tohit, todam, todex;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		tohit.where = TO_AFFECTS;
		tohit.type = gsn_caltraps;
		tohit.level = ch->level;
		tohit.duration = -1;
		tohit.location = APPLY_HITROLL;
		tohit.modifier = -5;
		tohit.bitvector = 0;
		affect_to_char( victim, &tohit );

		todam.where = TO_AFFECTS;
		todam.type = gsn_caltraps;
		todam.level = ch->level;
		todam.duration = -1;
		todam.location = APPLY_DAMROLL;
		todam.modifier = -5;
		todam.bitvector = 0;
		affect_to_char( victim, &todam );

		todex.type = gsn_caltraps;
		todex.level = ch->level;
		todex.duration = -1;
		todex.location = APPLY_DEX;
		todex.modifier = -5;
		todex.bitvector = 0;
		affect_to_char( victim, &todex );

		act( "$N incespica maldestramente tra gli spuntoni.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "Incespichi maldestramente tra gli spuntoni.", ch, NULL, victim, TO_VICT, POS_REST );
		check_improve( ch, gsn_caltraps, TRUE, 1 );
	}
}


void do_throw( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	char		arg[MIL];
	int			chance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( !str_cmp(arg, "spear") || !str_cmp(arg, "lancia") )
	{
		do_throw_spear( ch, argument );
		return;
	}

	if ( skill_failure_check(ch, gsn_throw, 0, 0, "Non hai mai imparato quest'abilita'.. sara' per un'altra volta.") )
		return;

	if ( IS_AFFECTED(ch, AFF_FLYING) )
	{
		send_to_char( "Metti i piedi a terra per bilanciarti.\n\r", ch );
		return;
	}

	if ( (victim = ch->fighting) == NULL )
	{
		send_to_char( "Non stai combattendo nessuno..\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "Oh no, non butterai $N con la faccia a terra.. e' la tua adorata guida..!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_safe(ch, victim) ) return;

	WAIT_STATE( ch, skill_table[gsn_throw].beats );

	if ( is_affected(victim, gsn_protective_shield) )
	{
		act( "$C$N e' protett$X da una misteriosa forza che ti impedisce di afferrarl$X!$c",
			ch, NULL, victim, TO_CHAR, POS_FIGHT, YELLOW );
		act( "$C$n cerca di afferrarti il il tuo scudo protettivo ti difende dal misero tentativo..$c",
			ch, NULL, victim, TO_VICT, POS_FIGHT, YELLOW );
		act( "$C$n cerca di afferrare $N ma uno scudo di energia impedisce la presa!$c",
			ch, NULL, victim, TO_NOTVICT, POS_FIGHT, YELLOW );
		return;
	}

	chance = get_skill( ch, gsn_throw );

	if ( ch->size < victim->size )
		chance += ( ch->size - victim->size ) * 10;
	else
		chance += ( ch->size - victim->size ) * 25;

	/* stats */
	chance += get_curr_stat( ch, STR );
	chance -= get_curr_stat( victim, DEX ) * 4 / 3;

	if ( IS_AFFECTED(victim, AFF_FLYING) )
		chance += 10;

	/* speed */
	if ( IS_SET(ch->off_flags, OFF_FAST) )
		chance += 10;
	if ( IS_SET(victim->off_flags, OFF_FAST) )
		chance -= 20;

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	if ( IS_MOB(ch) || number_percent() < chance )
	{
		act( "Afferri $N scaraventandol$X a terra con forza.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n ti afferra scaraventandoti con violenza a terra!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n afferra $N scaraventandol$X con forza a terra!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		WAIT_STATE( victim, 2 * PULSE_VIOLENCE );

		damage( ch, victim, ch->level + get_curr_stat(ch, STR), gsn_throw, DAM_BASH, TRUE );
		victim->position = POS_REST;
		check_improve( ch, gsn_throw, TRUE, 1 );
	}
	else
	{
		act( "$N si libera della presa prima che tu possa scaraventarl$X giu'!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "Cerca di afferrarti ma riesci a liberarti dalla presa prima che ti scaraventi a terra!", victim, NULL, ch, TO_CHAR, POS_REST );
		act( "$n cerca di afferrare $N per buttarl$X a terra ma non e' abbastanza lest$x.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_throw, FALSE, 1 );
	}
}


void do_strangle( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	AFFECT_DATA af;
	int			chance;
	/*~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_strangle, 0, 0, "E dove l'avresti imparato?") )
		return;

	if ( (victim = get_char_room(ch, argument)) == NULL )
	{
		send_to_char( "Si, ok sarebbe un ottimo metodo per far tacere.. se fosse qui.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' || ch == victim )
	{
		send_to_char( "Hai un nodo in gola?\n\r", ch );
		return;
	}

	if ( IS_ADMIN(victim) )
	{
		send_to_char( "Una forza misteriosa blocca le tue mani.", ch );
		return;

	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && victim == ch->leader )
	{
		send_to_char( "Strangolare la tua adorata guida? Oh no.. non subito..\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{
		act( "$N e' gia' stordit$X.. non c'e' bisogno di strangolarl$X.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_neckguard) )
	{
		act( "$N sembra troppo attent$X per cercare di coglierl$X di sorpresa con un attacco simile..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	victim->last_fight_time = current_time;
	ch->last_fight_time = current_time;

	WAIT_STATE( ch, skill_table[gsn_strangle].beats );

	if ( IS_MOB(ch) )
		chance = UMIN( 35, ch->level / 2 );
	else
		chance = 0.6 * get_skill( ch, gsn_strangle );

	if ( IS_MOB(victim) && victim->pIndexData->pShop != NULL )
		chance -= 40;

	if ( number_percent() < chance )
	{
		act( "Afferri $N per il collo e con un'improvvisa e violenta stretta alla nuca l$X metti a dormire..", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n ti afferra per il collo e con una stretta improvvisa ti blocca il respiro mettendoti a dormire..", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n afferra $N per il collo mettendol$X a dormire..", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_strangle, TRUE, 1 );

		af.type = gsn_strangle;
		af.where = TO_AFFECTS;
		af.level = ch->level;
		af.duration = ch->level / 50 + 1;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_SLEEP;
		affect_join( victim, &af );

		if ( IS_AWAKE(victim) ) victim->position = POS_SLEEP;
	}
	else
	{
		damage( ch, victim, 0, gsn_strangle, DAM_NONE, TRUE );
		check_improve( ch, gsn_strangle, FALSE, 1 );
		if ( IS_MOB(ch) && victim->position > POS_STUN)
				do_yell( victim, "Aiuto! Mi sta attaccando!" );

		af.type = gsn_neckguard;
		af.where = TO_AFFECTS;
		af.level = victim->level;
		af.duration = 2 + victim->level / 50;
		af.modifier = 0;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		affect_join( victim, &af );
	}
}


void do_blackjack( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	AFFECT_DATA af;
	int			chance;
	/*~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_blackjack, 0, 0, "Ah.. e dove l'avresti imparato?") )
		return;

	if ( (victim = get_char_room(ch, argument)) == NULL )
	{
		send_to_char( "Mmhh avvicinati con circospezione e.. ehi, guarda che non e' qui..\n\r", ch );
		return;
	}


	if ( argument[0] == '\0' || ch == victim )
	{
		send_to_char( "Ho un rimedio migliore, vedi il muro? Ecco, picchiaci la testa con forza!\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && victim == ch->leader )
	{
		send_to_char( "Addormentare la tua generosa guida con una botta simile.. oh no.. non lo faresti mai.. per ora..\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{
		act( "$N e' gia' nel mondo dei sogni..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_headguard) )
	{
		act( "$N e' troppo attent$X e sospettos$X..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_safe(ch, victim) )
		return;
	victim->last_fight_time = current_time;
	ch->last_fight_time = current_time;

	WAIT_STATE( ch, skill_table[gsn_blackjack].beats );

	chance = 0.5 * get_skill( ch, gsn_blackjack );
	chance += URANGE( 0, (get_curr_stat(ch, DEX) - 20) * 2, 10 );
	chance += can_see( victim, ch ) ? 0 : 5;
	if ( IS_MOB(victim) )
	{
		if ( victim->pIndexData->pShop != NULL )
			chance -= 40;
	}

	if ( IS_MOB(ch) || number_percent() < chance )
	{
		act( "Colpisci $N sulla nuca con il tuo randello mettendol$X a dormire!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "Senti una fitta terribile dietro la nuca.. i sensi ti vengono meno..", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n colpisce violentemente $N alla nuca mandandol$X nel mondo dei sogni..", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_blackjack, TRUE, 1 );

		af.type = gsn_blackjack;
		af.where = TO_AFFECTS;
		af.level = ch->level;
		af.duration = ch->level / 50 + 1;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_SLEEP;
		affect_join( victim, &af );

		if ( IS_AWAKE(victim) )
			victim->position = POS_SLEEP;
	}
	else
	{
		damage( ch, victim, ch->level / 2, gsn_blackjack, DAM_NONE, TRUE );
		check_improve( ch, gsn_blackjack, FALSE, 1 );
		if ( IS_MOB(victim) && victim->position > POS_STUN)
				do_yell( victim, "Aiuto! Mi sta attaccando!" );

		af.type = gsn_headguard;
		af.where = TO_AFFECTS;
		af.level = victim->level;
		af.duration = 2 + victim->level / 50;
		af.modifier = 0;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		affect_join( victim, &af );
	}
}


void do_warcry( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_warcry, 1, 0, "See.. al massimo potresti cantar messa..") )
		return;

	if ( is_affected(ch, gsn_bless) || is_affected(ch, gsn_warcry) )
	{
		send_to_char( "Hai gia' invocato il favore dei tuoi Dei!\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 30 )
	{
		send_to_char( "Ti servira' piu' energia per invocare l'aiuto dei tuoi Dei!\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_warcry].beats );

	if ( number_percent() > ch->pcdata->learned[gsn_warcry] )
	{
		send_to_char( "Ti viene a mancare la voce sul piu' bello..\n\r", ch );
		act( "$n prepara il suo urlo di battaglia.. ma s'interrompe subito..", ch, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	ch->point[MANA] -= 30;

	af.where = TO_AFFECTS;
	af.type = gsn_warcry;
	af.level = ch->level;
	af.duration = 6 + ch->level;
	af.location = APPLY_HITROLL;
	af.modifier = ch->level / 8;
	af.bitvector = 0;
	affect_to_char( ch, &af );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = 0 - ch->level / 8;
	affect_to_char( ch, &af );
	send_to_char( "Senti la mano dei tuoi dei guidarti verso la battaglia!!!\n\r", ch );
}


void do_guard( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_guard, 1, 0, NULL) )
		return;

	if ( !arg || arg[0] == '\0' )
	{
		send_to_char( "Chi vuoi proteggere?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non e' qui..\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )

	{
		act( "$N non vuole assolutamente la tua protezione!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !str_cmp(arg, "nessuno") || !str_cmp(arg, "se stesso")
	||	 !str_cmp(arg, "none") || !str_cmp(arg, "self") || victim == ch )
	{
		if ( ch->guarding == NULL )
		{
			send_to_char( "Si come no! Chi fa da se' fa per tre!\n\r", ch );
			return;
		}
		else
		{
			act( "Smetti di proteggere $N.", ch, NULL, ch->guarding, TO_CHAR, POS_REST );
			act( "$n smette di proteggerti.", ch, NULL, ch->guarding, TO_VICT, POS_REST );
			act( "$n smette di proteggere $N.", ch, NULL, ch->guarding, TO_NOTVICT, POS_REST );
			ch->guarding->guarded_by = NULL;
			ch->guarding = NULL;
			return;
		}
	}

	if ( ch->guarding == victim )
	{
		act( "L$X stai gia' proteggendo!$N!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( ch->guarding != NULL )
	{
		send_to_char( "Non puoi proteggere contemporaneamente due avventurieri, prima smetti con l'altro.\n\r", ch );
		return;
	}


	if ( victim->guarded_by != NULL )
	{
		act( "$N e' gia' protett$X da qualcun'altro.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim->guarding == ch )
	{
		act( "Vi proteggete a vicenda?! Ma dai..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !is_same_group(victim, ch) )
	{
		act( "Dovete essere nello stesso gruppo perche' tu possa proteggerl$X.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		act( "Limitati a proteggere la tua guida, come e quando te lo chiedera'!", ch, NULL, victim, TO_VICT, POS_REST );
		return;
	}

	if ( victim->fighting != NULL )
	{
		send_to_char( "Perche' non lasci che smettano di combattere prima?\n\r", ch );
		return;
	}

	if ( ch->fighting != NULL )
	{
		act( "So bene del tuo spirito cavalleresco.. ma bada alla tua pellaccia prima di proteggere $N.",
			ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	act( "$N e' ora sotto la tua protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "Ora sei sotto la protezione di $n.", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$n ora protegge $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );

	ch->guarding = victim;
	victim->guarded_by = ch;
}


CHAR_DATA *check_guard( CHAR_DATA *ch, CHAR_DATA *mob )
{
	int chance;
	/*~~~~~~~*/

	if ( ch->guarded_by == NULL || get_char_room(ch, ch->guarded_by->name) == NULL )
		return( ch );
	else
	{
		chance = ( get_skill(ch->guarded_by, gsn_guard) - (1.5 * (ch->level - mob->level)) );
		if ( number_percent() < UMIN(100, chance) )
		{
			act( "$n si frappone tra $N e il suo avversario!", ch->guarded_by, NULL, ch, TO_NOTVICT, POS_REST );
			act( "$n ti balza davanti prendendo le tue difese!", ch->guarded_by, NULL, ch, TO_VICT, POS_REST );
			act( "Balza davanti a $N prendendone le difese!", ch->guarded_by, NULL, ch, TO_CHAR, POS_REST );
			check_improve( ch->guarded_by, gsn_guard, TRUE, 3 );
			return( ch->guarded_by );
		}
		else
		{
			check_improve( ch->guarded_by, gsn_guard, FALSE, 3 );
			return( ch );
		}
	}
}


void do_explode( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim = ch->fighting;
	CHAR_DATA	*vch, *vch_next;
	int			dam = 0, hp_dam, dice_dam, mana;
	int			hpch, level = ch->level;
	char		arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_guard, 1, 0, "Ah certo, giochiamo al piccolo chimico pure? Ma dove l'hai imparato?") )
		return;

	if ( victim == NULL )
	{
		one_argument( argument, arg );
		if ( arg[0] == '\0' )
		{
			send_to_char( "Colpisci al volo un sacchetto di polvere nera con la lama.\n\r", ch );
			return;
		}

		if ( (victim = get_char_room(ch, arg)) == NULL )
		{
			send_to_char( "Non e' qui.\n\r", ch );
			return;
		}
	}

	mana = skill_table[gsn_explode].min_mana;

	if ( ch->point[MANA] < mana )
	{
		send_to_char( "Ti serve piu' energia per farlo..\n\r", ch );
		return;
	}

	ch->point[MANA] -= mana;

	act( "$n lancia in aria un sacchetto di polvere nera e prova a colpirlo con la lama.. ma non succede nulla..", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n colpisce al volo un sacchetto di polvere esplosiva e ti dilania con la sua lama fiammeggiante!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Bruciali tutti!.", ch, NULL, NULL, TO_CHAR, POS_REST );

	WAIT_STATE( ch, skill_table[gsn_explode].beats );

	if ( !IS_MOB(ch) && number_percent() >= ch->pcdata->learned[gsn_explode] )
	{
		damage( ch, victim, 0, gsn_explode, DAM_FIRE, TRUE );
		check_improve( ch, gsn_explode, FALSE, 1 );
		return;
	}

	hpch = UMAX( 10, ch->point[LIFE] );
	hp_dam = number_range( hpch / 9 + 1, hpch / 5 );
	dice_dam = dice( level, 20 );

	if ( !(is_safe(ch, victim)) )
	{
		dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );
		fire_effect( victim->in_room, level, dam / 2, TARGET_ROOM );
	}

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
			fire_effect( vch, level, dam, TARGET_CHAR );
			damage( ch, vch, dam, gsn_explode, DAM_FIRE, TRUE );
		}
		else					/* partial damage */
		{
			fire_effect( vch, level / 2, dam / 4, TARGET_CHAR );
			damage( ch, vch, dam / 2, gsn_explode, DAM_FIRE, TRUE );
		}
	}

	if ( !IS_MOB(ch) && number_percent() >= ch->pcdata->learned[gsn_explode] )

	{
		fire_effect( ch, level / 4, dam / 10, TARGET_CHAR );
		damage( ch, ch, (ch->point[LIFE] / 10), gsn_explode, DAM_FIRE, TRUE );
	}
}


void do_target( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_target, 0, 0, "Si certo.. ma siamo seri, quando l'avresti imparato?!") )
		return;

	if ( ch->fighting == NULL )
	{
		send_to_char( "Scegliere il tuo avversario.. ma se nemmeno stai combattendo?!.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Decidi il tuo bersaglio!\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, argument)) == NULL )
	{
		send_to_char( "Eh? E dove sarebbe?\n\r", ch );
		return;
	}

	/* check victim is fighting with him */
	if ( victim->fighting != ch )
	{
		send_to_char( "Ehi.. ma non ti sta mica combattendo!\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_target].beats );

	if ( !IS_MOB(ch) && number_percent() < (get_skill(ch, gsn_target) / 2) )
	{
		check_improve( ch, gsn_target, FALSE, 1 );

		ch->fighting = victim;

		act( "$n si volta concentrando i suoi attacchi su $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		act( "Ti volti impetuosamente volgendo a $N i tuoi attacchi!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n si volta impetuosamente volgendo a TE i suoi attacchi!", ch, NULL, victim, TO_VICT, POS_REST );
		return;
	}

	send_to_char( "Una schivata ti costringe a mantenere la posizione.. riprova!.\n\r", ch );
	check_improve( ch, gsn_target, FALSE, 1 );
}


void do_tiger( CHAR_DATA *ch, char *argument )
{
	int chance, hp_percent;
	/*~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_tiger_power, 0, 0, "Che cosa stai cercando di fare?!") )
		return;

	if ( (chance = get_skill(ch,gsn_tiger_power)) == 0 )
	{
		send_to_char( "Riesci a pronunciare solo qualche sillaba distorta..\n\r", ch );
		return;
	}

	act( "$n ruggisce evocando il potere delle tigri!.", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( IS_AFFECTED(ch, AFF_BERSERK)
	||	 is_affected(ch, gsn_berserk)
	||	 is_affected(ch, gsn_tiger_power)
	||	 is_affected(ch, skill_lookup("frenzy")) )
	{
		send_to_char( "Hai gia' tutta l'ira di cui hai bisogno.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CALM) )
	{
		send_to_char( "Naah.. in questo stato al massimo puoi richiamare la forza di 10 micini..\n\r", ch );
		return;
	}

	if ( ch->in_room->sector_type != SECT_FIELD
	&&	 ch->in_room->sector_type != SECT_FOREST
	&&	 ch->in_room->sector_type != SECT_MOUNTAIN
	&&	 ch->in_room->sector_type != SECT_HILLS )
	{
		send_to_char( "Oh beh.. da qui le tigri non ti sentiranno di certo..\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 50 )
	{
		send_to_char( "Hai bisogno di piu' energia per farlo!\n\r", ch );
		return;
	}

	/* modifiers */
	/* fighting */
	if ( ch->position == POS_FIGHT )
		chance += 10;

	hp_percent = 100 * ch->point[LIFE] / ch->max_point[LIFE];
	chance += 25 - hp_percent / 2;

	if ( number_percent() < chance )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		WAIT_STATE( ch, PULSE_VIOLENCE );
		ch->point[MANA] -= 50;
		ch->point[MOVE] /= 2;

		/* heal a little damage */
		ch->point[LIFE] += ch->level * 2;
		ch->point[LIFE] = UMIN( ch->point[LIFE], ch->max_point[LIFE] );

		send_to_char( "Lo spirito delle tigri viene in tuo soccorso dandoti la sua forza!\n\r", ch );
		act( "Lo spirito delle tigri si fonde con  $n aumentando la sua forza e collera!", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_tiger_power, TRUE, 2 );

		af.where = TO_AFFECTS;
		af.type = gsn_tiger_power;
		af.level = ch->level;
		af.duration = number_fuzzy( ch->level / 8 );
		af.modifier = UMAX( 1, ch->level / 5 );
		af.bitvector = AFF_BERSERK;

		af.location = APPLY_HITROLL;
		affect_to_char( ch, &af );

		af.location = APPLY_DAMROLL;
		affect_to_char( ch, &af );

		af.modifier = UMAX( 10, 10 * (ch->level / 5) );
		af.location = APPLY_AC;
		affect_to_char( ch, &af );
	}
	else
	{
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		ch->point[MANA] -= 25;
		ch->point[MOVE] /= 2;

		send_to_char( "Per un attimo ti senti piu' forte.. ma passa subito.. che delusione..\n\r", ch );
		check_improve( ch, gsn_tiger_power, FALSE, 2 );
	}
}


void do_hara( CHAR_DATA *ch, char *argument )
{
	int			chance;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_hara_kiri, 0, 0, "Guarda che se non lo sai fare e' inutile che ci provi..") )
		return;


	if ( (chance = get_skill(ch, gsn_hara_kiri)) == 0 )
	{
		send_to_char( "Guarda che se non lo sai fare e' inutile che ci provi.. riusciresti solo a farti male.. e tanto.\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_hara_kiri) )
	{
		send_to_char( "L'hai gia' fatto troppo di recente, e' troppo rischioso, samurai.\n\r", ch );
		return;
	}

	/* fighting */
	if ( ch->position == POS_FIGHT )
	{
		send_to_char( "Non ora, non e' il momento.\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_hara_kiri) )
	{
		send_to_char( "Non ora, non e' il momento.\n\r", ch );
		return;
	}

	if ( number_percent() < chance )
	{
		WAIT_STATE( ch, PULSE_VIOLENCE );

		ch->point[LIFE] = 1;
		ch->point[MANA] = 1;
		ch->point[MOVE] = 1;

		if ( ch->pcdata->condition[COND_HUNGER] < 40 )
			ch->pcdata->condition[COND_HUNGER] = 40;
		if ( ch->pcdata->condition[COND_THIRST] < 40 )
			ch->pcdata->condition[COND_THIRST] = 40;

		send_to_char( "Ti colpisci sull'addome fermando le tue funzioni vitali temporaneamente.. e svieni.\n\r", ch );
		act( "$C$n si colpisce sull'addome e impallidisce rapidamente.. poi sviene.$c",
			ch, NULL, NULL, TO_ROOM, POS_FIGHT, RED );
		check_improve( ch, gsn_hara_kiri, TRUE, 2 );
		do_sleep( ch, "" );
		SET_BIT( ch->act, PLR_HARA_KIRI );

		af.where = TO_AFFECTS;
		af.type = gsn_hara_kiri;
		af.level = ch->level;
		af.duration = 10;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = 0;
		affect_to_char( ch, &af );
	}
	else
	{
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

		af.where = TO_AFFECTS;
		af.type = gsn_hara_kiri;
		af.level = ch->level;
		af.duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = 0;
		affect_to_char( ch, &af );

		send_to_char( "Ahi, no! Non sullo sterno, riprova!\n\r", ch );
		check_improve( ch, gsn_hara_kiri, FALSE, 2 );
	}
}


/*
 * Ground strike
 */
int ground_strike( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
	int			diceroll;
	AFFECT_DATA baf;
	/*~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_nomessage(ch, gsn_ground_strike, 0) )
		return( dam );

	if ( ch->in_room->sector_type != SECT_HILLS
	&&	 ch->in_room->sector_type != SECT_MOUNTAIN
	&&	 ch->in_room->sector_type != SECT_FOREST
	&&	 ch->in_room->sector_type != SECT_FIELD )
		return( dam );

	diceroll = number_range( 0, 100 );
	if ( victim->level > ch->level ) diceroll += ( victim->level - ch->level ) * 2;
	if ( victim->level < ch->level ) diceroll -= ( ch->level - victim->level );


	if ( diceroll <= (get_skill(ch, gsn_ground_strike) / 3) )
	{
		check_improve( ch, gsn_ground_strike, TRUE, 2 );
		dam += dam * diceroll / 200;
	}

	if ( diceroll <= (get_skill(ch, gsn_ground_strike) / 15) )
	{
		diceroll = number_percent();
		if ( diceroll < 75 )
		{
			act( "$C$x ti colpisce con violenza sul collo facendoti barcollare!Che male!$c",
				ch, NULL, victim, TO_VICT,	POS_REST, RED );
			act( "$CColpisci $N con tremenda forza sul collo facendol$X barcollare!$c",
				ch, NULL, victim, TO_CHAR, POS_REST, RED );

			check_improve( ch, gsn_ground_strike, TRUE, 3 );
			WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
			dam += ( dam * number_range(2, 5) ) / 5;
			return( dam );
		}
		else if ( diceroll > 75 && diceroll < 95 )
		{
			act( "$C$n ti colpisce al viso con le dita protese verso gli occhi!Non vedi piu' nulla!$c",
				ch, NULL, victim, TO_VICT, POS_REST, BROWN );
			act( "$CColpisci $N al viso con la mano semichiusa ad artiglio accecandol$X!$c",
				ch, NULL, victim, TO_CHAR, POS_REST, BROWN );

			check_improve( ch, gsn_ground_strike, TRUE, 4 );
			if ( !IS_AFFECTED(victim, AFF_BLIND) )
			{
				baf.type = gsn_dirt;
				baf.level = ch->level;
				baf.location = APPLY_HITROLL;
				baf.modifier = -4;
				baf.duration = number_range( 1, 5 );
				baf.bitvector = AFF_BLIND;
				affect_to_char( victim, &baf );
			}

			dam += dam * number_range( 1, 2 );
			return( dam );
		}
		else if ( diceroll > 95 )
		{
			act( "$C$n ti colpisce con violenza sullo sterno, ti manca il respiro!!!$c",
				ch, NULL, victim, TO_VICT, POS_REST, RED );
			act( "$CColpisci con il pugno chiuso $N sullo sterno! Non riesce piu' a respirare!!!$c",
				ch, NULL, victim, TO_CHAR, POS_REST, RED );

			check_improve( ch, gsn_ground_strike, TRUE, 5 );
			dam += dam * number_range( 2, 5 );
			return( dam );
		}
	}

	return( dam );
}


/*
 * Critical strike
 */
int critical_strike( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
	int			diceroll;
	AFFECT_DATA baf;
	/*~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_nomessage(ch, gsn_critical, 0) )
		return( dam );

	if ( get_wield_char(ch, FALSE) != NULL
	&&	 get_wield_char(ch, TRUE) != NULL
	&&	 number_percent() > ((ch->point[LIFE] * 100) / ch->max_point[LIFE]) )
		return( dam );

	diceroll = number_range( 0, 100 );
	if ( victim->level > ch->level )
		diceroll += ( victim->level - ch->level ) * 2;
	if ( victim->level < ch->level )
		diceroll -= ( ch->level - victim->level );

	if ( diceroll <= (get_skill(ch, gsn_critical) / 2) )
	{
		check_improve( ch, gsn_critical, TRUE, 2 );
		dam += dam * diceroll / 200;
	}

	if ( diceroll <= (get_skill(ch, gsn_critical) / 13) )
	{
		diceroll = number_percent();
		if ( diceroll < 75 )
		{
			act( "$C$n ti colpisce al viso con violenza inaudita!Sei frastornat$X!$c",
				ch, NULL, victim, TO_VICT, POS_REST, RED );
			act( "$CColpisci $N al viso con violenza inaudita! Sembra frastornat$X!$c",
				ch, NULL, victim, TO_CHAR, POS_REST, RED );

			check_improve( ch, gsn_critical, TRUE, 3 );
			WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
			dam += ( dam * number_range(2, 5) ) / 5;
			return( dam );
		}
		else if ( diceroll > 75 && diceroll < 95 )
		{
			act( "$CTi rompe il naso con un colpo violentissimo!Il sangue sul viso non ti fa vedere nulla!$c",
				ch, NULL, victim, TO_VICT, POS_REST, BROWN );
			act( "$CRompi il naso a $N e il sangue gli impedisce di vederci bene!$c",
				ch, NULL, victim, TO_CHAR, POS_REST, BROWN );

			check_improve( ch, gsn_critical, TRUE, 4 );
			if ( !IS_AFFECTED(victim, AFF_BLIND) )
			{
				baf.type = gsn_dirt;
				baf.level = ch->level;
				baf.location = APPLY_HITROLL;
				baf.modifier = -4;
				baf.duration = number_range( 1, 5 );
				baf.bitvector = AFF_BLIND;
				affect_to_char( victim, &baf );
			}

			dam += dam * number_range( 1, 2 );
			return( dam );
		}
		else if ( diceroll > 95 )
		{
			act( "$C$n ti colpisce alla carotide con due dita.. soffochi!!!$c",
				ch, NULL, victim, TO_VICT, POS_REST, RED );
			act( "$CColpisci $N alla carotide.. sembra che stia soffocando!!!$c",
				ch, NULL, victim, TO_CHAR, POS_REST, RED );

			check_improve( ch, gsn_critical, TRUE, 5 );
			dam += dam * number_range( 2, 5 );
			return( dam );
		}
	}

	return( dam );
}


void do_shield( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	int			chance, ch_weapon, vict_shield;
	OBJ_DATA	*shield, *axe;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_shield_cleave, 0, 0, "Si.. l'hai imparato da un libro?") )
		return;

	chance = get_skill(ch,gsn_shield_cleave);

	if ( (victim = ch->fighting) == NULL )
	{
		send_to_char( "Non stai combattendo.\n\r", ch );
		return;
	}

	if ( (axe = get_wield_char(ch, FALSE)) == NULL )
	{
		send_to_char( "Beh si, glielo spacchi con le mani?\n\r", ch );
		return;
	}

	if ( (shield = get_shield_char(victim)) == NULL )
	{
		send_to_char( "Ok, ma quale scudo vorresti rompere visto che il tuo avversario ne e' privo?\n\r", ch );
		return;
	}

	if ( axe->value[0] == WEAPON_AXE )
		chance *= 1.2;
	else if ( axe->value[0] != WEAPON_SWORD )
	{
		send_to_char( "Quest'arma non e' adatta, prova con una spada o un'ascia\n\r", ch );
		return;
	}

	/* find weapon skills */
	ch_weapon = get_weapon_skill( ch, get_weapon_sn(ch, FALSE) );
	vict_shield = get_skill( ch, gsn_shield_block );

	/* modifiers */
	/* skill */
	chance = chance * ch_weapon / 200;
	chance = chance * 100 / vict_shield;

	/* dex vs. strength */
	chance += get_curr_stat( ch, DEX );
	chance -= 2 * get_curr_stat( victim, STR );

	/* level */
	/* chance += (ch->level - victim->level) * 2; */
	chance += ch->level - victim->level;
	chance += axe->level - shield->level;

    /* cleave proofness */
	if ( check_material(shield,"platinum") || shield->pIndexData->limit != -1 )
		chance = -1;

	/* and now the attack */
	SET_BIT( ch->affected_by, AFF_WEAK_STUN );
	if ( number_percent() < chance )
	{
		WAIT_STATE( ch, skill_table[gsn_shield_cleave].beats );
		act( "Bel colpo! Hai distrutto lo scudo di $N!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n ti ha rotto lo scudo! Ora e' inservibile!!! Fagliela pagare!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n vibrando un colpo impressionante rompe lo scudo a $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_shield_cleave, TRUE, 1 );
		extract_obj( get_shield_char(victim) );
	}
	else
	{
		WAIT_STATE( ch, skill_table[gsn_shield_cleave].beats );
		act( "Lo scudo di $N sembra resistere al tuo attacco, riprova!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n punta al tuo scudo per distruggerlo! Fortunatamente non ce la fa..", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n colpisce con forza lo scudo $N cercando di distruggerlo, ma fallisce.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_shield_cleave, FALSE, 1 );
	}
}


void do_weapon( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*wield, *axe;
	int			chance, ch_weapon, vict_weapon;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_shield_cleave, 0, 0, "Rompere la sua arma? Ma se al massimo hai spezzato uno stuzzicadenti..") )
		return;

	chance = get_skill(ch,gsn_weapon_cleave);

	if ( (victim = ch->fighting) == NULL )
	{
		send_to_char( "Ancora non stai combattendo nessuno.\n\r", ch );
		return;
	}

	if ( (axe = get_wield_char(ch, FALSE)) == NULL )
	{
		send_to_char( "Ehi, di' un po'.. vorresti farlo a mani nude?\n\r", ch );
		return;
	}

	if ( (wield = get_wield_char(victim, FALSE)) == NULL )
	{
		send_to_char( "Non mi sembra che abbia un arma.. pensa ad un'altra strategia.\n\r", ch );
		return;
	}

	if ( axe->value[0] == WEAPON_AXE )
		chance *= 1.2;
	else if ( axe->value[0] != WEAPON_SWORD )
	{
		send_to_char( "La tua arma non te lo permette, prova con una spada o un'ascia..\n\r", ch );
		return;
	}

	/* find weapon skills */
	ch_weapon = get_weapon_skill( ch, get_weapon_sn(ch, FALSE) );

	vict_weapon = get_weapon_skill( victim, get_weapon_sn(victim, FALSE) );

	/* modifiers */
	/* skill */
	chance = chance * ch_weapon / 200;
	chance = chance * 100 / vict_weapon;

	/* dex vs. strength */
	chance += get_curr_stat( ch, DEX ) + get_curr_stat( ch, STR );
	chance -= get_curr_stat( victim, STR ) + 2 * get_curr_stat( victim, DEX );

	chance += ch->level - victim->level;
	chance += axe->level - wield->level;

	if ( check_material(wield, "platinum") || wield->pIndexData->limit != -1 )
		chance = -1;

	/* and now the attack */
	SET_BIT( ch->affected_by, AFF_WEAK_STUN );
	if ( number_percent() < chance )
	{
		WAIT_STATE( ch, skill_table[gsn_weapon_cleave].beats );
		act( "Spezzi l'arma di $N con un'incredibile fendente!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n ti rompe l'arma con un colpo incredibile!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n rompe l'arma di $N con un colpo incredibile!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_weapon_cleave, TRUE, 1 );
		extract_obj( get_wield_char(victim, FALSE) );
	}
	else
	{
		WAIT_STATE( ch, skill_table[gsn_weapon_cleave].beats );
		act( "L'arma di $N incassa il tuo colpo senza rompersi.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n cerca di spezzarti l'arma ma fortunatamente questa resiste!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n cerca di rompere l'arma di $N ma non ha successo!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_weapon_cleave, FALSE, 1 );
	}
}



void do_tail( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			chance, wait;
	bool		FightingCheck;
	int			damage_tail;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_tail, 0, OFF_TAIL, NULL) )
		return;

	if ( ch->fighting != NULL )
		FightingCheck = TRUE;
	else
		FightingCheck = FALSE;

	one_argument( argument, arg );

	if ( (chance = get_skill(ch, gsn_tail)) == 0 )
	{
		send_to_char( "Con quale coda?!?\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		victim = ch->fighting;
		if ( victim == NULL )
		{
			send_to_char( "Prova a farlo in un combattimento, non per gioco!\n\r", ch );
			return;
		}
	}
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non e' qui il tuo avversario.\n\r", ch );
		return;
	}

/*	if (victim->position < POS_FIGHT)
	{
		act( "You'll have to let $M get back up first.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}
*/
	if ( victim == ch )
	{
		send_to_char( "Guarda che rischi di annodartela al collo..\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "$N e' la tua amata guida.. come potresti mai fargli cio'!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_protective_shield) )
	{
		act( "$CLa tua coda sembra rimbalzare su $N!$c",
			ch, NULL, victim, TO_CHAR, POS_FIGHT, YELLOW );
		act( "$CCerca di colpirti con la $n ma il tuo scudo di energia ti protegge!$c",
			ch, NULL, victim, TO_VICT, POS_FIGHT, YELLOW );
		act( "$C$n cerca di colpire $N con la coda ma questa rimbalza sul suo scudo di energia!$c",
			ch, NULL, victim, TO_NOTVICT, POS_FIGHT, YELLOW );
		return;
	}

	/* modifiers */
	/* size and weight */
	chance -= ch->carry_weight / 20;
	chance += victim->carry_weight / 25;

	if ( ch->size < victim->size )
		chance += ( ch->size - victim->size ) * 25;
	else
		chance += ( ch->size - victim->size ) * 10;

	/* stats */
	chance += get_curr_stat( ch, STR ) + get_curr_stat( ch, DEX );
	chance -= get_curr_stat( victim, DEX ) * 2;

	if ( IS_AFFECTED(ch, AFF_FLYING) )
		chance -= 10;

	/* speed */
	if ( IS_SET(ch->off_flags, OFF_FAST) )
		chance += 20;
	if ( IS_SET(victim->off_flags, OFF_FAST) )
		chance -= 30;

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* now the attack */
	if ( number_percent() < (chance / 4) )
	{
		act( "$n ti manda a terra con un colpo di coda!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "Mandi a terra $N con un colpo di coda!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n manda $N a terra con un colpo di coda!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_tail, TRUE, 1 );

		wait = 3;

		switch ( number_bits(2) )
		{
		case 0:		wait = 1;	break;
		case 1:		wait = 2;	break;
		case 2:		wait = 4;	break;
		case 3:		wait = 3;	break;
		}

		WAIT_STATE( victim, wait * PULSE_VIOLENCE );
		WAIT_STATE( ch, skill_table[gsn_tail].beats );
		victim->position = POS_REST;
		damage_tail = ch->damroll + ( 2 * number_range(4, 4 + 10 * ch->size + chance / 10) );
		damage( ch, victim, damage_tail, gsn_tail, DAM_BASH, TRUE );
	}
	else
	{
		damage( ch, victim, 0, gsn_tail, DAM_BASH, TRUE );
		act( "Manchi $N e perdi l'equilibrio cadendo a terra!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n perde l'equilibrio e scivola a terra!.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		act( "Sfuggi alla coda di $n che perde l'equilibrio finendo a terra.", ch, NULL, victim, TO_VICT, POS_REST );
		check_improve( ch, gsn_tail, FALSE, 1 );
		ch->position = POS_REST;
		WAIT_STATE( ch, skill_table[gsn_tail].beats * 3 / 2 );
	}

	if ( IS_MOB(victim) && victim->position > POS_STUN && !FightingCheck )
			do_yell( victim, "Aiuto! Mi sta attaccando!" );
}


void do_concentrate( CHAR_DATA *ch, char *argument )
{
	int chance;
	/*~~~~~~~*/

	if ( skill_failure_check(ch, gsn_concentrate, 0, 0, "Al massimo puoi provare a concentrarti su di un buon libro..") )
		return;

	if ( (chance = get_skill(ch, gsn_concentrate)) == 0 )
	{
		send_to_char( "Al massimo puoi provare a concentrarti su di un buon libro..\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_concentrate) )
	{
		act( "Sei gia' pront$x al combattimento.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( ch->point[MANA] < 50 )
	{
		send_to_char( "Hai bisogno di piu' energia per farlo.\n\r", ch );
		return;
	}

	/* fighting */
	if ( ch->fighting )
	{
		send_to_char( "Concentrati sul combattimento che stai affrontando ORA!\n\r", ch );
		return;
	}

	if ( number_percent() < chance )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		ch->point[MANA] -= 50;
		ch->point[MOVE] /= 2;

		do_sit( ch, "" );
		send_to_char( "Ti siedi a terra incrociando le gambe e respirando lentamente, preparandoti alla prossima battaglia!!\n\r", ch );
		act( "$C$n si siede a terra respirando profondamente, sembra determinat$x per il prossimo scontro.$c",
			ch, NULL, NULL, TO_ROOM, POS_FIGHT, RED );
		check_improve( ch, gsn_concentrate, TRUE, 2 );

		af.where = TO_AFFECTS;
		af.type = gsn_concentrate;
		af.level = ch->level;
		af.duration = number_fuzzy( ch->level / 8 );
		af.modifier = UMAX( 1, ch->level / 8 );
		af.bitvector = 0;

		af.location = APPLY_HITROLL;
		affect_to_char( ch, &af );

		af.location = APPLY_DAMROLL;
		affect_to_char( ch, &af );

		af.modifier = UMAX( 1, ch->level / 10 );
		af.location = APPLY_AC;
		affect_to_char( ch, &af );
	}
	else
	{
		send_to_char( "Non riesci a focalizzare l'attenzione.. riprova.\n\r", ch );
		check_improve( ch, gsn_concentrate, FALSE, 2 );
	}
}


void do_katana( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*katana;
	AFFECT_DATA af;
	OBJ_DATA	*part;
	char		arg[MIL];
	char		buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_katana, 0, 0, "Cos'e', il coltello da cucina non ti basta piu'?") )
		return;

	if ( is_affected(ch, gsn_katana) )
	{
		send_to_char( "Devi ritrovare l'ispirazione.. hai provato a farne una troppo di recente..!\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 300 )
	{
		send_to_char( "Sei troppo debole per cercare di concentrarti su di una katana.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Da quale oggetto vuoi ricavarla?\n\r", ch );
		return;
	}

	if ( (part = get_obj_carry(ch, arg)) == NULL )
	{
		send_to_char( "E il materiale per costruirla?\n\r", ch );
		return;
	}

	if ( part->pIndexData->vnum != OBJ_VNUM_CHUNK_IRON )
	{
		send_to_char( "Mmm no.. fammi pensare, dov'e' che trovo del buon ferro?\n\r", ch );
		return;
	}

	if ( number_percent() < (get_skill(ch, gsn_katana) / 3) * 2 )
	{
		send_to_char( "Sbagli la temperatura! Rovini inesorabilmente il blocco di ferro..\n\r", ch );
		extract_obj( part );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_katana].beats );

	if ( !IS_MOB(ch) && number_percent() < get_skill(ch, gsn_katana) )
	{
		af.where = TO_AFFECTS;
		af.type = gsn_katana;
		af.level = ch->level;
		af.duration = ch->level;
		af.modifier = 0;
		af.bitvector = 0;
		af.location = 0;
		affect_to_char( ch, &af );

		katana = create_object( get_obj_index(OBJ_VNUM_KATANA_SWORD), ch->level );
		katana->cost = 0;
		katana->level = ch->level;
		ch->point[MANA] -= 300;

		af.where = TO_OBJECT;
		af.type = gsn_katana;
		af.level = ch->level;
		af.duration = -1;
		af.location = APPLY_DAMROLL;
		af.modifier = ch->level / 10;
		af.bitvector = 0;
		affect_to_obj( katana, &af );

		af.location = APPLY_HITROLL;
		affect_to_obj( katana, &af );

		katana->value[2] = ch->level / 10;

		sprintf( buf, katana->pIndexData->extra_descr->description, ch->name );
		katana->extra_descr = new_extra_descr();
		katana->extra_descr->keyword = str_dup( katana->pIndexData->extra_descr->keyword );
		katana->extra_descr->description = str_dup( buf );
		katana->extra_descr->next = NULL;

		obj_to_char( katana, ch );
		check_improve( ch, gsn_katana, TRUE, 1 );

		act( "Ecco la tua meravigliosa Katana!", ch, part, NULL, TO_CHAR, POS_REST );
		act( "$n sfoggia la sua Katana appena forgiata!!", ch, part, NULL, TO_ROOM, POS_REST );

		extract_obj( part );
		return;
	}
	else
	{
		send_to_char( "Ops.. niente da fare, distrutto..\n\r", ch );
		extract_obj( part );
		ch->point[MANA] -= 150;
		check_improve( ch, gsn_katana, FALSE, 1 );
	}
}


void do_crush( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	int			chance = 100, wait = 0;
	int			damage_crush;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_crush, 0, OFF_CRUSH, NULL) )
		return;

	chance = get_skill( ch,gsn_crush );

	if ( (victim = ch->fighting) == NULL )
	{
		send_to_char( "Non stai combattendo contro nessuno.\n\r", ch);
		return;
	}

	if ( victim->position < POS_FIGHT )
		return;

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
		return;

	if ( is_affected(victim, gsn_protective_shield) )
	{
		act( "$CCerchi di spingere giu' $N ma qualcosa l$X dal tuo attacco!$c",
			ch, NULL, victim, TO_CHAR, POS_FIGHT, YELLOW );
		act( "$C$n cerca di schiacciarti a terra, ma il tuo scudo protettivo ti difende respingendol$x!$c",
			ch, NULL, victim, TO_VICT, POS_FIGHT, YELLOW );
		act( "$C$n cerca di schiacciare $N a terra, ma una misteriosa forza gli impedisce di spingerl$X!$c",
			ch, NULL, victim, TO_NOTVICT, POS_FIGHT, YELLOW );
		return;
	}

	/* modifiers */
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
	if ( number_percent() < chance )
	{
		act( "$n ti schiaccia a terra con tutto il suo peso!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "Schiacci a terra $N con tutto il tuo peso!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n schiaccia a terra $N con tutto il tuo peso!", ch, NULL, victim, TO_NOTVICT, POS_REST );

		wait = 3;

		switch ( number_bits(2) )
		{
		case 0:		wait = 1;	break;
		case 1:		wait = 2;	break;
		case 2:		wait = 4;	break;
		case 3:		wait = 3;	break;
		}

		WAIT_STATE( victim, wait * PULSE_VIOLENCE );
		WAIT_STATE( ch, skill_table[gsn_crush].beats );
		victim->position = POS_REST;
		damage_crush = ch->damroll + number_range( 4, 4 + 4 * ch->size + chance / 2 );
		if ( ch->level < 5 )
			damage_crush = UMIN( ch->level, damage_crush );
		damage( ch, victim, damage_crush, gsn_crush, DAM_BASH, TRUE );
	}
	else
	{
		damage( ch, victim, 0, gsn_crush, DAM_BASH, TRUE );
		act( "$N ti evita e cadi a terra!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$N evita $n che perde l'equilibrio cadendo a terra!", ch, NULL, victim, TO_NOTVICT, POS_REST );
		act( "Eviti $n che perde l'equilibrio cadendo a terra!", ch, NULL, victim, TO_VICT, POS_REST );
		ch->position = POS_REST;
		WAIT_STATE( ch, skill_table[gsn_crush].beats * 3 / 2 );
	}
}


void do_sense( CHAR_DATA *ch, char *argument )
{
	if ( skill_failure_check(ch, gsn_sense_life, 1, 0, "Non e' una delle TUE abilita'.") )
		return;

	if ( is_affected(ch, gsn_sense_life) )
	{
		send_to_char( "I tuoi sensi sono gia' espansi al massimo.\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 20 )
	{
		send_to_char( "Non hai abbastanza energia per concentrare i tuoi sensi.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_sense_life].beats );

	if ( !IS_MOB(ch) && number_percent() < ch->pcdata->learned[gsn_sense_life] )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		af.where = TO_DETECTS;
		af.type = gsn_sense_life;
		af.level = ch->level;
		af.duration = ch->level;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = DETECT_LIFE;
		affect_to_char( ch, &af );

		ch->point[MANA] -= 20;

		act( "I tuoi sensi si acuiscono!", ch, NULL, NULL, TO_CHAR, POS_REST );
		act( "$n acuisce i suoi sensi..", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_sense_life, TRUE, 1 );
	}
	else
	{
		ch->point[MANA] -= 10;

		send_to_char( "Non riesci ad espandere le tue sensazioni.\n\r", ch );
		check_improve( ch, gsn_sense_life, FALSE, 1 );
	}
}


void do_poison_smoke( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*tmp_vict;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_poison_smoke, 1, 0, NULL) )
		return;

	if ( ch->point[MANA] < skill_table[gsn_poison_smoke].min_mana )
	{
		send_to_char( "Hai bisogno di piu' energia..\n\r", ch );
		return;
	}

	ch->point[MANA] -= skill_table[gsn_poison_smoke].min_mana;
	WAIT_STATE( ch, skill_table[gsn_poison_smoke].beats );

	if ( number_percent() > get_skill(ch, gsn_poison_smoke) )
	{
		send_to_char( "Umh.. no, non era la polverina giusta.\n\r", ch );
		check_improve( ch, gsn_poison_smoke, FALSE, 1 );
		return;
	}

	send_to_char( "Una nube di fumo velenoso riempie rapidamente l'ambiente..\n\r", ch );
	act( "Una nube di fumo velenoso riempie l'ambiente!", ch, NULL, NULL, TO_ROOM, POS_REST );


	check_improve( ch, gsn_poison_smoke, TRUE, 1 );

	for ( tmp_vict = ch->in_room->people; tmp_vict != NULL; tmp_vict = tmp_vict->next_in_room )
	{
		if ( !is_safe_spell(ch, tmp_vict, TRUE) )
		{
			if( IS_MOB(ch)
			&&	tmp_vict != ch
			&&	ch->fighting != tmp_vict
			&&	tmp_vict->fighting != ch
			&&	(IS_SET(tmp_vict->affected_by, AFF_CHARM) || !IS_MOB(tmp_vict)) )
			{
					do_yell( tmp_vict, "Aiuto! Mi sta attaccando!" );
			}

			spell_poison( gsn_poison, ch->level, ch, tmp_vict, TARGET_CHAR );
			if ( tmp_vict != ch )
				multi_hit( tmp_vict, ch, TYPE_UNDEFINED );
		}
	}
}


void do_blindness_dust( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*tmp_vict;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_blindness_dust, 1, 0, NULL) )
		return;

	if ( ch->point[MANA] < skill_table[gsn_blindness_dust].min_mana )
	{
		send_to_char( "Hai bisogno di piu' energia per farlo!\n\r", ch );
		return;
	}

	ch->point[MANA] -= skill_table[gsn_blindness_dust].min_mana;
	WAIT_STATE( ch, skill_table[gsn_blindness_dust].beats );

	if ( number_percent() > get_skill(ch, gsn_blindness_dust) )
	{
		send_to_char( "Mh.. no, devi aver sbagliato qualcosa..\n\r", ch );
		check_improve( ch, gsn_blindness_dust, FALSE, 1 );
		return;
	}

	send_to_char( "Una nube di polvere riempie la stanza!\n\r", ch );
	act( "La stanza si riempie di una nube polverosa!", ch, NULL, NULL, TO_ROOM, POS_REST );

	check_improve( ch, gsn_blindness_dust, TRUE, 1 );

	for ( tmp_vict = ch->in_room->people; tmp_vict != NULL; tmp_vict = tmp_vict->next_in_room )
	{
		if ( !is_safe_spell(ch, tmp_vict, TRUE) )
		{
			if ( IS_MOB(ch)
			&&	 tmp_vict != ch
			&&	 ch->fighting != tmp_vict
			&&	 tmp_vict->fighting != ch
			&&	(IS_SET(tmp_vict->affected_by, AFF_CHARM) || !IS_MOB(tmp_vict)) )
			{
					do_yell( tmp_vict, "Aiuto! Mi sta attaccando!" );
			}

			spell_blindness( gsn_blindness, ch->level, ch, tmp_vict, TARGET_CHAR );
			if ( tmp_vict != ch )
				multi_hit( tmp_vict, ch, TYPE_UNDEFINED );
		}
	}
}


void do_lash( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			chance;
	bool		FightingCheck;
	int			damage_lash;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_lash, 1, 0, NULL) )
		return;

	if ( ch->fighting != NULL )
		FightingCheck = TRUE;
	else
		FightingCheck = FALSE;

	argument = one_argument( argument, arg );

	chance = get_skill( ch, gsn_lash );

	if ( arg[0] == '\0' )
	{
		victim = ch->fighting;
		if ( victim == NULL )
		{
			send_to_char( "Non stai affatto combattendo!\n\r", ch );
			return;
		}
	}
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non e' qui.\n\r", ch );
		return;
	}

	if ( get_weapon_char(ch, WEAPON_WHIP) == NULL )
	{
		send_to_char( "Hai bisogno di una frusta per frustare, cosa credi?\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Ok, e dormire su di un materasso colmo di vetri rotti?\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "$N e' la tua amorevole guida!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	/* modifiers */
	/* stats */
	chance += get_curr_stat( ch, STR ) / 2;
	chance -= get_curr_stat( victim, DEX ) * 2;

	if ( IS_AFFECTED(ch, AFF_FLYING) ) chance += 20;

	/* speed */
	if ( IS_AFFECTED(ch, AFF_HASTE) )		chance += 20;
	if ( IS_AFFECTED(victim, AFF_HASTE) )	chance -= 20;
	if ( IS_AFFECTED(ch, AFF_SLOW) )		chance -= 40;
	if ( IS_AFFECTED(victim, AFF_SLOW) )	chance += 20;
	if ( MOUNTED(ch) )						chance -= 20;
	if ( MOUNTED(victim) )					chance += 40;

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* now the attack */
	if ( number_percent() < chance )
	{
		check_improve( ch, gsn_lash, TRUE, 1 );

		WAIT_STATE( ch, PULSE_VIOLENCE );
		WAIT_STATE( victim, skill_table[gsn_lash].beats );
		damage_lash = number_range( 4, 4 + chance / 10 );
		damage( ch, victim, damage_lash, gsn_lash, DAM_BASH, TRUE );
	}
	else
	{
		damage( ch, victim, 0, gsn_lash, DAM_BASH, TRUE );
		act( "La frusta sfiora solamente $N!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n prova a frustare $N ma il colpo l$X sfiora soltanto.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		act( "$N cerca di colpirti con una sferzata ma riesci ad evitare facilmente!", ch, NULL, victim, TO_VICT, POS_REST );
		check_improve( ch, gsn_lash, FALSE, 1 );
		WAIT_STATE( ch, PULSE_VIOLENCE );
	}


	if ( IS_MOB(victim) && victim->position > POS_STUN && !FightingCheck )
			do_yell( victim, "Aiuto! Mi sta attaccando!" );
}

/* Sharpen skill by Froboz questions comments send to froboz@cyberdude.com */
void do_sharpen( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	OBJ_DATA	*sobj;
	AFFECT_DATA af;
	char		arg [MIL ];
	int			percent;

	if ( !IS_MOB(ch)
	&& ch->level < skill_table[gsn_sharpen].skill_level[ch->class] )
	{
		send_to_char( "Cosa pensi di essere, un guerriero?\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Cosa vorresti affilare?\n\r", ch );
		return;
	}

	if ( ch->fighting )
	{
		send_to_char( "Durante un combattimento? Mossa inteliggente..\n\r", ch );
		return;
	}

	if ( !(obj = get_obj_carry(ch, arg)) )
	{
		send_to_char( "Non possiedi questa arma.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_WEAPON )
	{
		send_to_char( "Tale oggetto non e' un'arma.\n\r", ch );
		return;
	}

	if ( IS_OBJ_STAT(obj, ITEM_SHARP) )
	{
		send_to_char( "Quest'arma e' gia' stata affilata.\n\r", ch );
		return;
	}

	/* Ok we have a sharpenable weapon but do we have the stone */
	for ( sobj = ch->carrying; sobj; sobj = sobj->next_content )
	{
		if ( sobj->pIndexData->vnum == OBJ_VNUM_STONE )
			break;
	}

	if ( !sobj )
	{
		send_to_char( "Non possiedi una pietra filatrice.\n\r", ch );
		return;
	}

	if ( !IS_MOB(ch)
	&& percent > ch->pcdata->learned[gsn_sharpen] )
	{
		send_to_char( "Fallisci nell'affilare e ti ferisci un dito. Ahio!\n\r", ch );
		damage( ch, ch, ch->level, gsn_sharpen, DAM_SLASH, TRUE );
		act( "$n si taglia un dito!", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_sharpen, FALSE, 1 );
		return;
	}

	af.type			= -1;
	af.duration		= 0;
	af.location		= APPLY_DAMROLL;
	af.modifier		= number_fuzzy( 6 );
	af.bitvector	= 0;
	affect_to_obj( obj, &af );

	act( "Affili $p.\n\r", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$n passa una pietra affilante sulla sua lama.", ch, obj, NULL, TO_ROOM, POS_REST );
	SET_BIT( obj->extra_flags, ITEM_SHARP );

	/*
	 * Sharp weapon control, if you don't mind surplus sharp weapons
	 *	just comment out the next line.
	 */
	obj->timer = 10 + ch->level;
	check_improve( ch, gsn_sharpen, TRUE, 1 );
}
