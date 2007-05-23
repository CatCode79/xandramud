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
#include <ctype.h>
#include <time.h>

#include "xandra.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"


extern char		*target_name;


/****************************
 * CABALA ARMY
 */


/*
 * Mortal Strike
 * E' gestito in fight.c
 */


/*
 * Bandage
 */

void do_bandage( CHAR_DATA *ch, char *argument )
{
	int heal;
	/*~~~~~*/

	if ( skill_failure_check(ch, gsn_bandage, 0, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_bandage) )
	{
		send_to_char( "Le bende stanno gia' facendo la loro parte.\n\r", ch );
		return;
	}

	if ( IS_MOB(ch) || number_percent() < get_skill(ch, gsn_bandage) )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		WAIT_STATE( ch, PULSE_VIOLENCE );

		send_to_char( "Tamponi le tue ferite con delle bende curative!\n\r", ch );
		act( "$n tampona le sue ferite con delle bene curative!", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_bandage, TRUE, 2 );

		heal = dice( 4, 8 ) + ch->level / 2;
		ch->point[LIFE] = UMIN( ch->point[LIFE] + heal, ch->max_point[LIFE] );
		update_pos( ch );
		send_to_char( "Cominci a sentirti giusto un po' meglio..\n\r", ch );

		af.where = TO_AFFECTS;
		af.type = gsn_bandage;
		af.level = ch->level;
		af.duration = ch->level / 10;
		af.modifier = UMIN( 15, ch->level / 2 );
		af.bitvector = AFF_REGENERATION;
		af.location = 0;
		affect_to_char( ch, &af );
	}
	else
	{
		WAIT_STATE( ch, PULSE_VIOLENCE );

		send_to_char( "Ma no.. hai fatto cadere la benda.. su riprova.\n\r", ch );
		check_improve( ch, gsn_bandage, FALSE, 2 );
	}
}


/*
 * Cabal recall
 * E' gestito in act_move.c
 */


/*
 * Bloodthirst
 */
void do_bloodthirst( CHAR_DATA *ch, char *argument )
{
	int chance, hp_percent;
	/*~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_bloodthirst, 1, 0, "Umh naa.. non fa per te..") )
		return;

	chance = get_skill( ch, gsn_bloodthirst );

	if ( IS_AFFECTED(ch, AFF_BLOODTHIRST) )
	{
		send_to_char( "Urli di rabbia mentre la sete di sangue ti scorre nel corpo.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CALM) )
	{
		send_to_char( "Mentre senti la furia crescere dentro di te avverti una candido soffio di tepore che la disperde..\n\r", ch );
		return;
	}

	if ( ch->fighting == NULL )
	{
		act( "Per far salire la tua sete di sangue devi essere impegnat$n in un combattimento.",
			ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	/* modifiers */
	hp_percent = 100 * ch->point[LIFE] / ch->max_point[LIFE];
	chance += 25 - hp_percent / 2;

	if ( number_percent() < chance )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		WAIT_STATE( ch, PULSE_VIOLENCE );

		send_to_char( "Sangue! Morte! Ammazzali! Ammazzali tutti!\n\r", ch );
		act( "Delle vivide fiamme cominciano a pulsare negli occhi di $n!!!", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_bloodthirst, TRUE, 2 );

		af.where = TO_AFFECTS;
		af.type = gsn_bloodthirst;
		af.level = ch->level;
		af.duration = 2 + ch->level / 18;
		af.modifier = 5 + ch->level / 4;
		af.bitvector = AFF_BLOODTHIRST;

		af.location = APPLY_HITROLL;
		affect_to_char( ch, &af );

		af.location = APPLY_DAMROLL;
		affect_to_char( ch, &af );

		af.modifier = -UMIN( ch->level - 5, 35 );
		af.location = APPLY_AC;
		affect_to_char( ch, &af );
	}
	else
	{
		WAIT_STATE( ch, 3 * PULSE_VIOLENCE );

		send_to_char( "Cerchi di richiamare a te l'ira cieca della battaglia.. ma essa si ferma ad un passo da te..\n\r", ch );
		check_improve( ch, gsn_bloodthirst, FALSE, 2 );
	}
}


/*
 * Spellbane
 */
void do_spellbane( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_spellbane, 1, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_spellbane) )
	{
		act( "Sei gia' circondat$x da un'aura d'ira..", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_spellbane].beats );

	af.where = TO_DETECTS;
	af.type = gsn_spellbane;
	af.level = ch->level;
	af.duration = ch->level / 3;
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -ch->level / 4;	/* (TT) ma quel meno era voluto oppure è scappato? mah.. sarebbe stato meglio un -= */
	af.bitvector = ADET_SPELLBANE;

	affect_to_char( ch, &af );

	act( "Un'aura d'ira ti circonda proteggendoti dalla magia..", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "$n viene circondat$x da un'aura d'ira..", ch, NULL, NULL, TO_ROOM, POS_REST );
	check_improve( ch, gsn_spellbane, TRUE, 1 );
}


/*
 * Resistance
 */
void do_resistance( CHAR_DATA *ch, char *argument )
{
	if ( skill_failure_check(ch, gsn_resistance, 1, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_resistance) )
	{
		send_to_char( "I tuoi muscoli sono gia' tesi allo spasimo.\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 50 )
	{
		act( "Sei troppo stanc$x.. recupera un po' di energie prima!", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_resistance].beats );

	if ( IS_MOB(ch) || number_percent() < get_skill(ch, gsn_resistance) )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		af.where = TO_DETECTS;
		af.type = gsn_resistance;
		af.level = ch->level;
		af.duration = ch->level / 6;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = ADET_PROTECTOR;

		affect_to_char( ch, &af );
		ch->point[MANA] -= 50;

		act( "Irrigidisci i tuoi muscoli che diventano resistenti come roccia!", ch, NULL, NULL, TO_CHAR, POS_REST );
		act( "$n irrigidisce i muscoli che diventano resistenti come roccia!", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_resistance, TRUE, 1 );
	}
	else
	{
		ch->point[MANA] -= 25;

		send_to_char( "Fletti i tuoi muscoli ma non abbastanza..\n\r", ch );
		act( "$n flette i muscoli per qualche istante.. ma non succede nulla di nuovo.", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_resistance, FALSE, 1 );
	}
}


/*
 * Deathblow
 * Viene gestito in fight.c
 */


/*
 * Trophy
 */
void do_trophy( CHAR_DATA *ch, char *argument )
{
	int			trophy_vnum;
	OBJ_DATA	*trophy;
	AFFECT_DATA af;
	OBJ_DATA	*part;
	char		arg[MIL];
	int			level;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_trophy, 0, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_trophy) )
	{
		send_to_char( "Ne hai fatto uno poco fa!\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 30 )
	{
		send_to_char( "Non riesci a trovare le energie per fare un trofeo proprio ora.. riposa un po' prima.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Si ok, facciamo questo trofeo.. ma con cosa?\n\r", ch );
		return;
	}

	if ( (part = get_obj_carry(ch, arg)) == NULL )
	{
		send_to_char( "Ehi, non hai niente con quel nome..\n\r", ch );
		return;
	}

	if ( number_percent() < (get_skill(ch, gsn_trophy) / 3) * 2 )
	{
		send_to_char( "Acc.. hai esagerato nello scuoiamento..\n\r", ch );
		extract_obj( part );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_trophy].beats );

	if ( part->pIndexData->vnum == OBJ_VNUM_SLICED_ARM )		trophy_vnum = OBJ_VNUM_ARMY_PONCHO;
	else if ( part->pIndexData->vnum == OBJ_VNUM_SLICED_LEG )	trophy_vnum = OBJ_VNUM_ARMY_PONCHO;
	else if ( part->pIndexData->vnum == OBJ_VNUM_SEVERED_HEAD )	trophy_vnum = OBJ_VNUM_ARMY_PONCHO;
	else if ( part->pIndexData->vnum == OBJ_VNUM_TORN_HEART )	trophy_vnum = OBJ_VNUM_ARMY_PONCHO;
	else if ( part->pIndexData->vnum == OBJ_VNUM_GUTS )			trophy_vnum = OBJ_VNUM_ARMY_PONCHO;
	else if ( part->pIndexData->vnum == OBJ_VNUM_BRAINS )
	{
		send_to_char( "Da questo non potrai mai ricavarlo..\n\r", ch );
		return;
	}
	else
	{
		send_to_char( "Da questo non potrai mai ricavarlo..\n\r", ch );
		return;
	}

	if ( part->from[0] == '\0' )
	{
		send_to_char( "Da questo non potrai mai ricavarlo..\n\r", ch );
		return;
	}

	if ( !IS_MOB(ch) && number_percent() < ch->pcdata->learned[gsn_trophy] )
	{
		af.where = TO_AFFECTS;
		af.type = gsn_trophy;
		af.level = ch->level;
		af.duration = ch->level / 2;
		af.modifier = 0;
		af.bitvector = 0;

		af.location = 0;
		affect_to_char( ch, &af );

		if ( trophy_vnum != 0 )
		{
			char	buf[MSL];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			level = UMIN( part->level + 5, MAX_LEVEL );

			trophy = create_object( get_obj_index(trophy_vnum), level );
			trophy->timer = ch->level * 2;

			sprintf( buf, trophy->short_descr, part->from );
			free_string( trophy->short_descr );
			trophy->short_descr = str_dup( buf );

			sprintf( buf, trophy->description, part->from );
			free_string( trophy->description );
			trophy->description = str_dup( buf );
			trophy->cost = 0;
			trophy->level = ch->level;
			ch->point[MANA] -= 30;
			af.where = TO_OBJECT;
			af.type = gsn_trophy;
			af.level = level;
			af.duration = -1;
			af.location = APPLY_DAMROLL;
			af.modifier = ch->level / 5;
			af.bitvector = 0;
			affect_to_obj( trophy, &af );

			af.location = APPLY_HITROLL;
			af.modifier = ch->level / 5;
			af.bitvector = 0;
			affect_to_obj( trophy, &af );

			af.location = APPLY_INT;
			af.modifier = level > 20 ? -2 : -1;
			affect_to_obj( trophy, &af );

			af.location = APPLY_STR;
			af.modifier = level > 20 ? 2 : 1;
			affect_to_obj( trophy, &af );

			trophy->value[0] = ch->level;
			trophy->value[1] = ch->level;
			trophy->value[2] = ch->level;
			trophy->value[3] = ch->level;

			obj_to_char( trophy, ch );
			check_improve( ch, gsn_trophy, TRUE, 1 );

			act( "Oh, ecco il tuo trofeo di battaglia!", ch, part, NULL, TO_CHAR, POS_REST );
			act( "$n mostra orgoglios$x il suo trofeo di guerra ricavato da $p!", ch, part, NULL, TO_ROOM, POS_REST );

			extract_obj( part );
			return;
		}
	}
	else
	{
		send_to_char( "Argh.. l'hai distrutto..\n\r", ch );
		extract_obj( part );
		ch->point[MANA] -= 15;
		check_improve( ch, gsn_trophy, FALSE, 1 );
	}
}


/*
 * Truesight
 */
void do_truesight( CHAR_DATA *ch, char *argument )
{
	if ( skill_failure_check(ch, gsn_truesight, 1, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_truesight) )
	{
		act( "Sei gia' concentrat$x al massimo per cogliere ogni cosa.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( ch->point[MANA] < 50 )
	{
		send_to_char( "Non hai abbastanza energia per concentrarti sul tuo sguardo..\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_truesight].beats );

	if ( !IS_MOB(ch) && number_percent() < ch->pcdata->learned[gsn_truesight] )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		af.where = TO_DETECTS;
		af.type = gsn_truesight;
		af.level = ch->level;
		af.duration = ch->level / 2 + 5;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = DETECT_HIDDEN;
		affect_to_char( ch, &af );

		af.bitvector = DETECT_INVIS;
		affect_to_char( ch, &af );

		af.bitvector = DETECT_IMP_INVIS;
		affect_to_char( ch, &af );

		af.bitvector = ACUTE_VISION;
		affect_to_char( ch, &af );

		af.bitvector = DETECT_MAGIC;
		affect_to_char( ch, &af );

		ch->point[MANA] -= 50;

		act( "Ti concentri affinche' i tuoi occhi percepiscano tutto il possibile!", ch, NULL, NULL, TO_CHAR, POS_REST );
		act( "Gli occhi di $n sembrano risplendere di luce propria!", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_truesight, TRUE, 1 );
	}
	else
	{
		ch->point[MANA] -= 25;

		send_to_char( ".\n\r", ch );
		act( "Gli occhi di $n sembrano risplendere per un attimo! Ma solo per un attimo..", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_truesight, FALSE, 1 );
	}
}



/****************************
 * RULER
 */


/*
 * Ruler aura
 */
void spell_ruler_aura( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, sn) )
	{
		send_to_char( "Ora hai piu' padronanza di te.\n\r", ch );

		af.where = TO_IMMUNE;
		af.type = sn;
		af.duration = level / 4;
		af.level = ch->level;
		af.bitvector = IMM_CHARM;
		af.location = 0;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
	else
		send_to_char( "Sei gia' sotto l'effetto dell'aura del dominio.\n\r", ch );
}


/*
 * Sword of justice
 */
void spell_sword_of_justice( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (IS_GOOD(ch) && IS_GOOD(victim)) || (IS_EVIL(ch) && IS_EVIL(victim)) || IS_NEUTRAL(ch) )
	{
		if ( IS_MOB(victim) || !IS_SET(victim->act, PLR_WANTED) )
		{
			send_to_char( "L'incantesimo e' fallito!\n\r", ch );
			return;
		}
	}

	if ( !IS_MOB(victim) && IS_SET(victim->act, PLR_WANTED) )
		dam = dice( level, 20 );
	else
		dam = dice( level, 14 );

	if ( saves_spell(level, victim, DAM_MENTAL) ) dam /= 2;

	do_yell( ch, "La Spada della Giustizia!" );
	act( "Un'immensa spada di luce appare e colpisce in pieno $N!", ch, NULL, victim, TO_ALL, POS_REST );

	damage( ch, victim, dam, sn, DAM_MENTAL, TRUE );
}


/*
 * Wanted
 */
void do_wanted( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_wanted, 0, 0, NULL) )
		return;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
        send_to_char( "\n\rSintassi: wanted <nome> <Y|N>\n\r", ch );
		return;
	}

	victim = get_char_world( ch, arg1 );

	if ( (victim == NULL) || !(can_see(ch, victim)) )
	{
        send_to_char( "Non e' qui.\n\r", ch );
		return;
	}

	if ( victim->level >= LEVEL_ADMIN && (ch->level+5 < victim->level) )
	{
		act( "Non hai il potere di arrestare $N.", ch, NULL, victim, TO_CHAR, POS_REST);
		return;
	}

	if ( victim == ch )
	{
		act( "Non puoi farlo su te stess$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	switch ( arg2[0] )
	{
	case 'S':
	case 's':
	case 'Y':
	case 'y':
		if ( IS_SET(victim->act, PLR_WANTED) )
			act( "$n e' gia' ricercat$x.", ch, NULL, NULL, TO_CHAR, POS_REST);
		else
		{
			SET_BIT( victim->act, PLR_WANTED );
			act( "$n ora e' ricercat$x!!", victim, NULL, ch, TO_NOTVICT, POS_REST );
			act( "Da questo momento sei ricercat$x!!", victim, NULL, ch, TO_CHAR, POS_REST );
			send_to_char( "Ok.\n\r", ch );
		}
		break;

	case 'N':
	case 'n':
		if ( !IS_SET(victim->act, PLR_WANTED) )
			act( "$N non e' piu' ricercat$X dalla legge.", ch, NULL, victim, TO_CHAR, POS_REST);
		else
		{
			REMOVE_BIT( victim->act, PLR_WANTED );
			act( "$n non e' piu' ricercat$x.", victim, NULL, ch, TO_NOTVICT, POS_REST );
			act( "Non sei piu' ricercat$x.", victim, NULL, ch, TO_CHAR, POS_REST );
			send_to_char( "Ok.\n\r", ch );
		}
		break;

	default:
		send_to_char( "Utilizzo: wanted <nome> <Y|N>\n\r", ch );
		break;
	}
}


/*
 * Judge
 * Thanx zihni@karmi.emu.edu.tr for the code of do_judge
 */
void do_judge( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_judge, 1, 0, NULL) )
		return;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Giudicare chi?\n\r", ch );
		return;
	}

	/* judge thru world */
	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e'.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Non puoi farlo su di lui.\n\r", ch );
		return;
	}

	if ( IS_ADMIN(victim) && !IS_ADMIN(ch) )
	{
		send_to_char( "Non hai i poteri necessari per giudicare una divinita'.\n\r", ch );
		return;
	}

	sprintf( buf, "L'etica di %s e' %s e il suo allineamento e' %s.\n\r",
		victim->name,
		victim->ethos == 1 ? "Legale" : victim->ethos == 2 ? "Neutrale" : victim->ethos == 3 ? "Caotica" : "Sconosciuta",
		IS_GOOD(victim) ? "Buono" : IS_EVIL(victim) ? "Malvagio" : "Neutrale" );

	send_to_char( buf, ch );
}


/*
 * Manacles
 */
void spell_manacles( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_SET(victim->act, PLR_WANTED) )
	{
		act( "Le manette sono per i criminali!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !is_affected(victim, sn) && !saves_spell(ch->level, victim, DAM_CHARM) )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = 5 + level / 5;
		af.bitvector = 0;

		af.modifier = 0 - ( get_curr_stat(victim, DEX) - 4 );
		af.location = APPLY_DEX;
		affect_to_char( victim, &af );

		af.modifier = -5;
		af.location = APPLY_HITROLL;
		affect_to_char( victim, &af );

		af.modifier = -10;
		af.location = APPLY_DAMROLL;
		affect_to_char( victim, &af );

		spell_charm_person( gsn_charm_person, level, ch, vo, TARGET_CHAR );
	}
}


/*
 * Shield of ruler
 */
void spell_shield_ruler( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int			shield_vnum;
	OBJ_DATA	*shield;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if		( level >= 71 )		shield_vnum = OBJ_VNUM_RULER_SHIELD4;
	else if ( level >= 51 )		shield_vnum = OBJ_VNUM_RULER_SHIELD3;
	else if ( level >= 31 )		shield_vnum = OBJ_VNUM_RULER_SHIELD2;
	else						shield_vnum = OBJ_VNUM_RULER_SHIELD1;

	shield = create_object( get_obj_index(shield_vnum), level );
	shield->timer = level;
	shield->level = ch->level;
	shield->cost = 0;
	obj_to_char( shield, ch );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = level / 8;
	af.bitvector = 0;

	af.location = APPLY_HITROLL;
	affect_to_obj( shield, &af );


	af.location = APPLY_DAMROLL;
	affect_to_obj( shield, &af );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = -level / 2;
	af.bitvector = 0;
	af.location = APPLY_AC;
	affect_to_obj( shield, &af );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = -level / 9;
	af.bitvector = 0;
	af.location = APPLY_SAVING_SPELL;
	affect_to_obj( shield, &af );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = UMAX( 1, level / 30 );
	af.bitvector = 0;
	af.location = APPLY_CHA;
	affect_to_obj( shield, &af );

	act( "Richiamando l'aiuto della Giustizia Divina materializzi nelle tue mani lo $p!", ch, shield, NULL, TO_CHAR, POS_REST );
	act( "$n materializza nelle sue mani $p!", ch, shield, NULL, TO_ROOM, POS_REST );
}


/*
 * Guard call
 */
void spell_guard_call( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*guard;
	CHAR_DATA	*guard2;
	CHAR_DATA	*guard3;
	AFFECT_DATA af;
	char		buf[] = "Guardie! Guardie!";
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Ti manca la forza di richiamare altre guardie ora.\n\r", ch );
		return;
	}

	do_yell( ch, buf );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	gch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD )
		{
			do_say( gch, "Ehi, ci sono qua io!" );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	guard = create_mobile( get_mob_index(MOB_VNUM_SPECIAL_GUARD) );

	for ( i = 0; i < MAX_STATS; i++ )
		guard->perm_stat[i] = ch->perm_stat[i];

	guard->max_point[LIFE] = 2 * ch->max_point[LIFE];
	guard->point[LIFE] = guard->max_point[LIFE];
	guard->max_point[MANA] = ch->max_point[MANA];
	guard->point[MANA] = guard->max_point[MANA];
	guard->alignment = ch->alignment;
	guard->level = ch->level;
	for ( i = 0; i < 3; i++ )
		guard->armor[i] = interpolate( guard->level, 100, -200 );
	guard->armor[3] = interpolate( guard->level, 100, -100 );
	guard->sex = ch->sex;
	guard->gold = 0;
	guard->timer = 0;

	guard->damage[DICE_NUMBER] = number_range( level / 16, level / 12 );
	guard->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	guard->damage[DICE_BONUS] = number_range( level / 9, level / 6 );

	guard->detection = ( A | C | D | E | F | G | H | ee );
	SET_BIT( guard->affected_by, AFF_CHARM );
	SET_BIT( guard->affected_by, AFF_SANCTUARY );

	guard2 = create_mobile( guard->pIndexData );
	clone_mobile( guard, guard2 );

	guard3 = create_mobile( guard->pIndexData );
	clone_mobile( guard, guard3 );

	guard->master = guard2->master = guard3->master = ch;
	guard->leader = guard2->leader = guard3->leader = ch;

	char_to_room( guard, ch->in_room );
	char_to_room( guard2, ch->in_room );
	char_to_room( guard3, ch->in_room );
	send_to_char( "Delle guardie giungono in tuo aiuto!\n\r", ch );
	act( "Delle guardie giungono ad affiancare $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 6;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );
}


/*
 * Ruler badge
 */
void spell_ruler_badge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*badge;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	OBJ_DATA	*obj_next;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( count_worn(ch, WEAR_NECK) >= max_can_wear(ch, WEAR_NECK) )
	{
		send_to_char( "Prima togli cio' che indossi ora al suo posto.\n\r", ch );
		return;
	}

	for ( badge = ch->carrying; badge != NULL; badge = obj_next )
	{
		obj_next = badge->next_content;
		if ( badge->pIndexData->vnum == OBJ_VNUM_DEPUTY_BADGE || badge->pIndexData->vnum == OBJ_VNUM_RULER_BADGE )
		{
			act( "Il tuo $p si dissolve.", ch, badge, NULL, TO_CHAR, POS_REST );
			obj_from_char( badge );
			extract_obj( badge );
			continue;
		}
	}

	badge = create_object( get_obj_index(OBJ_VNUM_RULER_BADGE), level );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = 100 + level / 2;
	af.bitvector = 0;

	af.location = APPLY_LIFE;
	affect_to_obj( badge, &af );

	af.location = APPLY_MANA;
	affect_to_obj( badge, &af );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = level / 8;
	af.bitvector = 0;

	af.location = APPLY_HITROLL;
	affect_to_obj( badge, &af );

	af.location = APPLY_DAMROLL;
	affect_to_obj( badge, &af );

	badge->timer = 200;
	act( "Indossi il tuo distintivo.", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "$n indossa il suo distintivo!", ch, NULL, NULL, TO_ROOM, POS_REST );

	obj_to_char( badge, victim );
	equip_char( ch, badge, WEAR_NECK );
	ch->point[LIFE] = UMIN( (ch->point[LIFE] + 100 + level / 2), ch->max_point[LIFE] );
	ch->point[MANA] = UMIN( (ch->point[MANA] + 100 + level / 2), ch->max_point[MANA] );
}


/*
 * Remove badge
 */
void spell_remove_badge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*badge;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	OBJ_DATA	*obj_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	badge = 0;

	for ( badge = victim->carrying; badge != NULL; badge = obj_next )
	{
		obj_next = badge->next_content;
		if ( badge->pIndexData->vnum == OBJ_VNUM_DEPUTY_BADGE || badge->pIndexData->vnum == OBJ_VNUM_RULER_BADGE )
		{
			act( "Il tuo $p si dissolve.", ch, badge, NULL, TO_CHAR, POS_REST );
			act( "Il $p di $ns si dissolve.", ch, badge, NULL, TO_ROOM, POS_REST );

			obj_from_char( badge );
			extract_obj( badge );
			continue;
		}
	}
}



/****************************
 * CABALA INVADER
 */


/*
 * Evil spirit
 */
void spell_evil_spirit( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AREA_DATA		*pArea = ch->in_room->area;
	ROOM_INDEX_DATA *room;
	AFFECT_DATA		af, af2;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_RAFFECTED(ch->in_room, AFF_ROOM_ESPIRIT) || is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "La zona e' gia' contagiata da quest'incantesimo.\n\r", ch );
		return;
	}

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energie per lanciare ora quest'incantesimo.\n\r", ch );
		return;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) || IS_SET(ch->in_room->area->area_flag, AREA_HOMETOWN) )
	{
		send_to_char( "Un'aura sacra protegge la stanza..\n\r", ch );
		return;
	}

	af2.where = TO_AFFECTS;
	af2.type = sn;
	af2.level = ch->level;
	af2.duration = level / 5;
	af2.modifier = 0;
	af2.location = APPLY_NONE;
	af2.bitvector = 0;
	affect_to_char( ch, &af2 );

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 25;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_ESPIRIT;

	for ( i = pArea->min_vnum; i < pArea->max_vnum; i++ )
	{
		if ( (room = get_room_index(i)) == NULL )
			continue;
		affect_to_room( room, &af );
		if ( room->people )
			act( "Un denso spirito di malvagita' pervade la stanza..", room->people, NULL, NULL, TO_ALL, POS_REST );
	}
}


/*
 * Nightwalker
 */
void spell_nightwalker( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*walker;
	AFFECT_DATA af;
	char		buf[100];
	int			i;
	/*~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Ti senti troppo debole per evocare un nottambulo ora.\n\r", ch );
		return;
	}

	send_to_char( "Cospiri tra le ombre rendendo il tuo oscuro omaggio per evocare il loro aiuto.\n\r", ch );
	act( "$n sibila oscure formule mentre ombre danzanti si mostrano ai suoi piedi.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	gch->pIndexData->vnum == MOB_VNUM_NIGHTWALKER )
		{
			send_to_char( "Puoi controllare un solo nottambulo alla volta!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	walker = create_mobile( get_mob_index(MOB_VNUM_NIGHTWALKER) );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		walker->perm_stat[i] = ch->perm_stat[i];
	}

	walker->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		URANGE( 	ch->pcdata->perm_point[LIFE], 1 * ch->pcdata->perm_point[LIFE], 30000 );
	walker->point[LIFE] = walker->max_point[LIFE];
	walker->max_point[MANA] = ch->max_point[MANA];
	walker->point[MANA] = walker->max_point[MANA];
	walker->level = ch->level;
	for ( i = 0; i < 3; i++ )
		walker->armor[i] = interpolate( walker->level, 100, -100 );
	walker->armor[3] = interpolate( walker->level, 100, 0 );
	walker->gold = 0;
	walker->timer = 0;
	walker->damage[DICE_NUMBER] = number_range( level / 15, level / 10 );
	walker->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	walker->damage[DICE_BONUS] = 0;

	char_to_room( walker, ch->in_room );
	send_to_char( "Una creatura emerge dalle tenebre!\n\r", ch );
	act( "Una creatura emerge dalle tenebre!", ch, NULL, NULL, TO_ROOM, POS_REST );
	sprintf( buf, "Il nottambulo e' pronto a servirti." );
	send_to_char( buf, ch );
	sprintf( buf, "L'essere avvolto dall'ombra s'inchina davanti a %s!", ch->name );
	act( buf, ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( walker->affected_by, AFF_CHARM );
	walker->master = walker->leader = ch;
}


/*
 * Eyes of intrigue
 */
void spell_eyes( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA		*victim;
	ROOM_INDEX_DATA *ori_room;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Non riesci a trovare nessuno con tale nome nell'area.\n\r", ch );
		return;
	}

	if ( (victim->level > ch->level + 7) || saves_spell((ch->level + 9), victim, DAM_NONE) )
	{
		send_to_char( "Non riesci a individuare la vittima.\n\r", ch );
		return;
	}

	if ( ch == victim )
		do_look( ch, "auto" );
	else
	{
		ori_room = ch->in_room;
		char_from_room( ch );
		char_to_room( ch, victim->in_room );
		do_look( ch, "auto" );
		char_from_room( ch );
		char_to_room( ch, ori_room );
	}
}


/*
 * Fade
 */
void do_fade( CHAR_DATA *ch, char *argument )
{
	if ( skill_failure_check(ch, gsn_fade, 0, 0, NULL) )
		return;

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi svanire se stai cavalcando.\n\r", ch );
		return;
	}

	if ( !cabal_ok(ch, gsn_fade) )
		return;
	send_to_char( "Provi a svanire.\n\r", ch );

	SET_BIT( ch->affected_by, AFF_FADE );
	check_improve( ch, gsn_fade, TRUE, 3 );
}


/*
 * Shadow cloak
 */
void spell_shadow_cloak( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->cabal != victim->cabal )
	{
		send_to_char( "Puoi usare questa protezione solo su di un compagno di gilda.\n\r", ch );
		return;
	}

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N e' gia' protett$x da questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.modifier = -level;
	af.location = APPLY_AC;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	send_to_char( "Un mantello di ombre ti avvolge proteggendoti.\n\r", victim );
	if ( ch != victim )
		act( "Un mantello di ombre avvolge $N proteggendol$X.", ch, NULL, victim, TO_CHAR, POS_REST );
}


/*
 * Nightfall
 */
void spell_nightfall( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	OBJ_DATA	*light;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai forze sufficienti per farlo.\n\r", ch );
		return;
	}

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		for ( light = vch->carrying; light != NULL; light = light->next_content )
		{
			if ( light->item_type == ITEM_LIGHT && light->value[2] != 0 && !is_same_group(ch, vch) )
			{
				if ( /* light->value[2] != -1 || */ saves_spell(level, vch, DAM_ENERGY) )
				{
					act( "$p perde tutta la sua luce!", ch, light, NULL, TO_CHAR, POS_REST );
					act( "$p perde tutta la sua luce!", ch, light, NULL, TO_ROOM, POS_REST );
					light->value[2] = 0;
					if ( get_light_char(ch) == NULL )
						ch->in_room->light--;
				}
			}
		}
	}

	for ( light = ch->in_room->contents; light != NULL; light = light->next_content )
	{
		if ( light->item_type == ITEM_LIGHT && light->value[2] != 0 )
		{
			act( "$p perde tutta la sua luce!", ch, light, NULL, TO_CHAR, POS_REST );
			act( "$p perde tutta la sua luce!", ch, light, NULL, TO_ROOM, POS_REST );
			light->value[2] = 0;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 2;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;
	affect_to_char( ch, &af );
}


/*
 * Shadowlife
 */
void spell_shadowlife( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	CHAR_DATA	*shadow;
	AFFECT_DATA af;
	int			i;
	char		buf[MSL];
	char		*name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) )
	{
		send_to_char( "Mh non mi sembra un bersaglio adatto!\n\r", ch );
		return;
	}

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energie per farlo,ora.\n\r", ch );
		return;
	}

	act( "Doni vita all'ombra di $N!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n dona vita all'ombra di $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n dona vita alla tua ombra!", ch, NULL, victim, TO_VICT, POS_REST );

	shadow = create_mobile( get_mob_index(MOB_VNUM_SHADOW) );

	for ( i = 0; i < MAX_STATS; i++ )
		shadow->perm_stat[i] = ch->perm_stat[i];

	shadow->max_point[LIFE] = ( 3 * ch->max_point[LIFE] ) / 4;
	shadow->point[LIFE] = shadow->max_point[LIFE];
	shadow->max_point[MANA] = ( 3 * ch->max_point[MANA] ) / 4;
	shadow->point[MANA] = shadow->max_point[MANA];
	shadow->alignment = ch->alignment;
	shadow->level = ch->level;
	for ( i = 0; i < 3; i++ )
		shadow->armor[i] = interpolate( shadow->level, 100, -100 );
	shadow->armor[3] = interpolate( shadow->level, 100, 0 );
	shadow->sex = victim->sex;
	shadow->gold = 0;

	name = IS_MOB( victim ) ? victim->short_descr : victim->name;
	sprintf( buf, shadow->short_descr, name );
	free_string( shadow->short_descr );
	shadow->short_descr = str_dup( buf );

	sprintf( buf, shadow->long_descr, name );
	free_string( shadow->long_descr );
	shadow->long_descr = str_dup( buf );

	sprintf( buf, shadow->description, name );
	free_string( shadow->description );
	shadow->description = str_dup( buf );

	char_to_room( shadow, ch->in_room );

	do_murder( shadow, victim->name );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );
}


/****************************
 * CABALA CHAOS
 */


/*
 * Disgrace
 */
void spell_disgrace( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !is_affected(victim, sn) && !saves_spell(level, victim, DAM_MENTAL) )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = level;
		af.location = APPLY_CHA;
		af.modifier = -( 5 + level / 5 );
		af.bitvector = 0;
		affect_to_char( victim, &af );

		act( "$N sembra improvvisamente a disagio..", ch, NULL, victim, TO_ALL, POS_REST );
	}
	else
		send_to_char( "L'incantesimo ha esito negativo.\n\r", ch );
}


/*
 * Aura of chaos
 */
void spell_aura_of_chaos( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->cabal != victim->cabal )
	{
		send_to_char( "Puoi usare quest'incantesimo solo su compagni di gilda.\n\r", ch );
		return;
	}

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' questo tipo di protezione.\n\r", ch );
		else
			act( "$N ha gia' questo tipo di protezione.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = ADET_AURA_CHAOS;
	affect_to_char( victim, &af );
	send_to_char( "Senti su di te la protezione degli Dei del Caos.\n\r", victim );
	if ( ch != victim )
		act( "Un'aura di Caos scende a proteggere $N.", ch, NULL, victim, TO_CHAR, POS_REST );
}


/*
 * Garble
 */
void spell_garble( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch == victim )
	{
		send_to_char( "Chi e' la vittima?\n\r", ch );
		return;
	}

	if ( is_affected(victim, sn) )
	{
		act( "$N e' gia' sotto l'effetto di questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_safe_nomessage(ch, victim) )
	{
		send_to_char( "Umh sembra che tu non possa farlo su quella persona..\n\r", ch );
		return;
	}

	if ( (victim->level > ch->level + 7) || saves_spell((ch->level + 9), victim, DAM_MENTAL) ) return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 10;
	af.modifier = 0;
	af.location = 0;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	act( "Hai alterato la parlata di $N!", ch, NULL, victim, TO_CHAR, POS_REST );
	send_to_char( "Ops.. senti la tua lingua contorcersi spasmodicamente..\n\r", victim );
}


/*
 * Mirror
 */
void spell_confuse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	CHAR_DATA	*rch;
	int			count = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, gsn_confuse) )
	{
		act( "$N e' gia' sotto l'effetto di questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( saves_spell(level, victim, DAM_MENTAL) ) return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 10;
	af.modifier = 0;
	af.location = 0;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch == ch && !can_see(ch, rch) && get_trust(ch) < rch->invis_level )
			count++;
		continue;
	}

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch != ch && can_see(ch, rch) && get_trust(ch) >= rch->invis_level && number_range(1, count) == 1 )
			break;
	}

	if ( rch )
		do_murder( victim, rch->name );

	do_murder( victim, ch->name );
}


/*
 * Confuse
 */
void spell_mirror( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	int			mirrors, new_mirrors;
	CHAR_DATA	*gch;
	CHAR_DATA	*tmp_vict;
	char		long_buf[MSL];
	char		short_buf[20];
	int			order;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) )
	{
		send_to_char( "Puoi farlo solo su di un avventuriero.\n\r", ch );
		return;
	}

	for ( mirrors = 0, gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	is_affected(gch, gsn_mirror)
		&&	is_affected(gch, gsn_doppelganger)
		&&	gch->doppel == victim )
			mirrors++;
	}

	if ( mirrors >= level / 5 )
	{
		if ( ch == victim )
			act( "Non puoi produrre altre copie di te stess$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
		else
			act( "Non puoi produrre altre copie di $N.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.level = level;
	af.modifier = 0;
	af.location = 0;
	af.bitvector = 0;

	for ( tmp_vict = victim; is_affected(tmp_vict, gsn_doppelganger); tmp_vict = tmp_vict->doppel )
		;

	sprintf( long_buf, "%s e' qui.\n\r", tmp_vict->name );
	sprintf( short_buf, "%s", tmp_vict->name );

	order = number_range( 0, level / 5 - mirrors );

	for ( new_mirrors = 0; mirrors + new_mirrors < level / 5; new_mirrors++ )
	{
		gch = create_mobile( get_mob_index(MOB_VNUM_MIRROR_IMAGE) );
		free_string( gch->name );
		free_string( gch->short_descr );
		free_string( gch->long_descr );
		free_string( gch->description );
		gch->name = str_dup( tmp_vict->name );
		gch->short_descr = str_dup( short_buf );
		gch->long_descr = str_dup( long_buf );
		gch->description = ( tmp_vict->description == NULL ) ? NULL : str_dup( tmp_vict->description );
		gch->sex = tmp_vict->sex;

		af.type = gsn_doppelganger;
		af.duration = level;
		affect_to_char( gch, &af );
		af.type = gsn_mirror;
		af.duration = -1;
		affect_to_char( gch, &af );

		gch->max_point[LIFE] = gch->point[LIFE] = 1;
		gch->level = 1;
		gch->doppel = victim;
		gch->master = victim;
		char_to_room( gch, victim->in_room );

		if ( number_percent() < 20 )
		{
			ROOM_INDEX_DATA *ori_room;
			/*~~~~~~~~~~~~~~~~~~~~~~*/

			ori_room = victim->in_room;
			char_from_room( victim );
			char_to_room( victim, ori_room );
		}

		if ( new_mirrors == order )
		{
			char_from_room( victim );
			char_to_room( victim, gch->in_room );
		}

		if ( ch == victim )
		{
			send_to_char( "Una tua immagine speculare appare improvvisamente!!!\n\r", ch );
			act( "Un'immagine speculare di $n appare improvvisamente!", ch, NULL, victim, TO_ROOM, POS_REST );
		}
		else
		{
			act( "Un'immagine speculare di $N appare improvvisamente!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "Un'immagine speculare di $N appare improvvisamente!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			send_to_char( "Una tua immagine speculare appare improvvisamente!!!\n\r", victim );
		}
	}
}


/*
 * Doppelganger
 */
void spell_doppelganger( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (ch == victim) || (is_affected(ch, sn) && (ch->doppel == victim)) )
	{
		act( "Sei gia' simile a $E.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

/* Xandra
	if ( IS_MOB(victim) )
	{
		act( "$N na,non puoi proprio farlo..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}
*/

	if ( IS_ADMIN(victim) )
	{
		send_to_char( "Umh.. io dico che e' meglio di no.\n\r", ch );
		return;
	}

	if ( saves_spell(level, victim, DAM_CHARM) )
	{
		send_to_char( "Acc.. mi sa che hai sbagliato qualcosa..\n\r", ch );
		return;
	}

	act( "Oh! Riesci a camuffarti prendendo le sembianze di $N.", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n prende le tue sembianze!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "$n prende le sembianze di $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 2 * level ) / 3;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = 0;

	affect_to_char( ch, &af );
	ch->doppel = victim;
}


/*
 * Chaos blade
 */
void spell_chaos_blade( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*blade;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~*/

	blade = create_object( get_obj_index(OBJ_VNUM_CHAOS_BLADE), level );
	send_to_char( "Chiamando in aiuto le potenze dell'anarchia crei dal nulla la spada del Caos!\n\r", ch );
	act( "Una strana spada compare tra le mani di $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	blade->timer = level * 2;
	blade->level = ch->level;

	if		( ch->level <= 10 )						blade->value[2] = 2;
	else if ( ch->level > 10 && ch->level <= 20 )	blade->value[2] = 3;
	else if ( ch->level > 20 && ch->level <= 30 )	blade->value[2] = 4;
	else if ( ch->level > 30 && ch->level <= 40 )	blade->value[2] = 5;
	else if ( ch->level > 40 && ch->level <= 50 )	blade->value[2] = 6;
	else if ( ch->level > 50 && ch->level <= 60 )	blade->value[2] = 7;
	else if ( ch->level > 60 && ch->level <= 70 )	blade->value[2] = 9;
	else if ( ch->level > 70 && ch->level <= 80 )	blade->value[2] = 11;
	else											blade->value[2] = 12;

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = level / 6;
	af.bitvector = 0;

	af.location = APPLY_HITROLL;
	affect_to_obj( blade, &af );

	af.location = APPLY_DAMROLL;
	affect_to_obj( blade, &af );

	obj_to_char( blade, ch );
}


/*
 * Randomizer
 */
void spell_randomizer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af, af2;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Aspetta un po' prima di rifarlo.\n\r", ch );
		return;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) )
	{
		send_to_char( "Mh no, qui non funzionera' mai.\n\r", ch );
		return;
	}

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "Qui e' gia' stato fatto..\n\r", ch );
		return;
	}

	if ( number_bits(1) == 0 )
	{
		send_to_char( "Non riesci a cambiare lo stato delle cose..\n\r", ch );
		af2.where = TO_AFFECTS;
		af2.type = sn;
		af2.level = ch->level;
		af2.duration = level / 10;
		af2.modifier = 0;
		af2.location = APPLY_NONE;
		af2.bitvector = 0;
		affect_to_char( ch, &af2 );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 15;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_RANDOMIZER;
	affect_to_room( ch->in_room, &af );

	af2.where = TO_AFFECTS;
	af2.type = sn;
	af2.level = ch->level;
	af2.duration = level / 5;
	af2.modifier = 0;
	af2.location = APPLY_NONE;
	af2.bitvector = 0;
	affect_to_char( ch, &af2 );
	send_to_char( "Ecco qui!Ora da dove esci?\n\r", ch );
	send_to_char( "Lo sforzo ti ha proprio spossato..\n\r", ch );
	ch->point[LIFE] -= UMIN( 200, ch->point[LIFE] / 2 );
	act( "Qualcosa nell'ambiente sembra cambiato..", ch, NULL, NULL, TO_ROOM, POS_REST );
}


/*
 * Disperse
 */
void spell_disperse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA		*vch;
	CHAR_DATA		*vch_next;
	ROOM_INDEX_DATA *pRoomIndex;
	AFFECT_DATA		af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Umh no.. non ora, aspetta ancora un po'.\n\r", ch );
		return;
	}

	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( vch->in_room != NULL
		&&	!IS_SET(vch->in_room->room_flags, ROOM_NO_RECALL)
		&&	!IS_ADMIN(vch)
		&&	((IS_MOB(vch) && !IS_SET(vch->act, ACT_AGGRESSIVE))
		||	(!IS_MOB(vch) && vch->level > LEVEL_NEWBIE && (!is_safe_nomessage(ch, vch))))
		&&	vch != ch
		&&	!IS_SET(vch->imm_flags, IMM_SUMMON) )
		{
			for ( ;; )
			{
				pRoomIndex = get_room_index( number_range(0, 65535) );
				if ( pRoomIndex != NULL )
				{
					if ( can_see_room(ch, pRoomIndex)
					&&	!room_is_private(pRoomIndex)
					&&	!IS_SET(pRoomIndex->room_flags, ROOM_NO_RECALL) )
						break;
				}
			}

			send_to_char( "E.. ne rimarra' uno solo!\n\r", vch );
			act( "$n sparisce!", vch, NULL, NULL, TO_ROOM, POS_REST );
			char_from_room( vch );
			char_to_room( vch, pRoomIndex );
			act( "$n appare dal nulla!", vch, NULL, NULL, TO_ROOM, POS_REST );
			do_look( vch, "auto" );
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 10;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;
	affect_to_char( ch, &af );
}



/****************************
 * CABALA YVALADRIM
 */


/*
 * Transform
 */
void spell_transform( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) || ch->point[LIFE] > ch->max_point[LIFE] )
	{
		send_to_char( "Ancora di piu'?!.\n\r", ch );
		return;
	}

	ch->point[LIFE] += UMIN( 30000 - ch->max_point[LIFE], ch->max_point[LIFE] );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.location = APPLY_LIFE;
	af.modifier = UMIN( 30000 - ch->max_point[LIFE], ch->max_point[LIFE] );
	af.bitvector = 0;
	affect_to_char( ch, &af );

	send_to_char( "Senti il tuo corpo assurgere ad un nuovo vigore mentre la testa ti si appesantisce.\n\r", ch );
}


void spell_mana_transfer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( victim == ch )
	{
		send_to_char( "E' altamente pericoloso e controproducente farlo su se' stessi.\n\r", ch );
		return;
	}

	if ( ch->cabal != victim->cabal )
	{
		send_to_char( "Solo un tuo compagno di cabala puo' beneficiare di quest'incantesimo.\n\r", ch );
		return;
	}

	if ( ch->point[LIFE] < 50 )
		damage( ch, ch, 50, sn, DAM_NONE, TRUE );
	else
	{
		damage( ch, ch, 50, sn, DAM_NONE, TRUE );
		victim->point[MANA] = UMIN( victim->max_point[MANA], victim->point[MANA] + number_range(20, 120) );
	}
}


/*
 * Mental knife
 */
void spell_mental_knife( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->level < 40 )
		dam = dice( level, 8 );
	else if ( ch->level < 65 )
		dam = dice( level, 11 );
	else
		dam = dice( level, 14 );

	if ( saves_spell(level, victim, DAM_MENTAL) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_MENTAL, TRUE );

	if ( !is_affected(victim, sn) && !saves_spell(level, victim, DAM_MENTAL) )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = level;
		af.location = APPLY_INT;
		af.modifier = -7;
		af.bitvector = 0;
		affect_to_char( victim, &af );

		af.location = APPLY_WIS;
		affect_to_char( victim, &af );
		act( "Scagli una lama d'energia che brucia il cervello di $N!", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n scaglia una lama d'energia che ti brucia nel cervello!", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n scaglia una lama d'energia che brucia su $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	}
}


/*
 * Demon summon
 */
void spell_demon_summon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*demon;
	AFFECT_DATA af;
	int			i;
	/*~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai gia' evocato recentemente un Demone, aspetta ancora un po'.\n\r", ch );
		return;
	}

	send_to_char( "Richiami dagli inferi piu' nascosti un demone delle fiamme.\n\r", ch );
	act( "$n compie un rito di evocazione..", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if
		( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	gch->pIndexData->vnum == MOB_VNUM_DEMON )
		{
			send_to_char( "Non puoi controllare due demoni alla volta!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	demon = create_mobile( get_mob_index(MOB_VNUM_DEMON) );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		demon->perm_stat[i] = ch->perm_stat[i];
	}

	demon->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		URANGE(	ch->pcdata->perm_point[LIFE], ch->point[LIFE], 30000 );
	demon->point[LIFE] = demon->max_point[LIFE];
	demon->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	demon->point[MANA] = demon->max_point[MANA];
	demon->level = ch->level;
	for ( i = 0; i < 3; i++ )
		demon->armor[i] = interpolate( demon->level, 100, -100 );
	demon->armor[3] = interpolate( demon->level, 100, 0 );
	demon->gold = 0;
	demon->timer = 0;
	demon->damage[DICE_NUMBER] = number_range( level / 15, level / 12 );
	demon->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	demon->damage[DICE_BONUS] = number_range( level / 10, level / 8 );

	char_to_room( demon, ch->in_room );
	send_to_char( "Il Demone e' giunto, pronto a servirti.\n\r", ch );
	act( "Dal sottosuolo si materializza l'imponente e terribile figura di un demone fiammeggiante!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	if ( number_percent() < 40 )
	{
		if ( can_see(demon, ch) )
			do_say( demon, "Come osi disturbarmi?!!" );
		else
			do_say( demon, "Mortale, come ti permetti?!!" );
		do_murder( demon, ch->name );
	}
	else
	{
		SET_BIT( demon->affected_by, AFF_CHARM );
		demon->master = demon->leader = ch;
	}
}


/*
 * Scourge
 */
void spell_scourge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*tmp_vict;
	CHAR_DATA	*tmp_next;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( ch->level < 40 )
		dam = dice( level, 6 );
	else if ( ch->level < 65 )
		dam = dice( level, 9 );
	else
		dam = dice( level, 12 );

	for ( tmp_vict = ch->in_room->people; tmp_vict != NULL; tmp_vict = tmp_next )
	{
		tmp_next = tmp_vict->next_in_room;

		if ( !is_safe_spell(ch, tmp_vict, TRUE) )
		{
			if ( tmp_vict != ch
			&&	ch->fighting != tmp_vict
			&&	tmp_vict->fighting != ch
			&&	(IS_SET(tmp_vict->affected_by, AFF_CHARM) || IS_MOB(tmp_vict)) )
			{
				do_yell( tmp_vict, "Aiuto! Mi stanno attaccando!" );
			}

			if ( !is_affected(tmp_vict, sn) )
			{
				if ( number_percent() < level )		spell_poison( gsn_poison, level, ch, tmp_vict, TARGET_CHAR );
				if ( number_percent() < level )		spell_blindness( gsn_blindness, level, ch, tmp_vict, TARGET_CHAR );
				if ( number_percent() < level )		spell_weaken( gsn_weaken, level, ch, tmp_vict, TARGET_CHAR );
				if ( saves_spell(level, tmp_vict, DAM_FIRE) )
					dam /= 2;
				damage( ch, tmp_vict, dam, sn, DAM_FIRE, TRUE );
			}
		}
	}
}


/*
 * Tesseract
 */
void spell_tesseract( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	CHAR_DATA	*wch;
	CHAR_DATA	*wch_next;
	bool		gate_pet;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	ch->in_room == NULL )
	{
		send_to_char( "Fallisci il tuo tentativo di trasporto.\n\r", ch );
		return;
	}

	if ( !can_see_room(ch, victim->in_room)
	||	(is_safe(ch, victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||	room_is_private(victim->in_room)
	||	IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	||	IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	(!IS_MOB(victim) && victim->level >= LEVEL_HERO)	/* NOT trust */
	||	(IS_MOB(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(!IS_MOB(victim) && IS_SET(victim->act, PLR_NOSUMMON) && is_safe_nomessage(ch, victim))
	||	(saves_spell(level, victim, DAM_NONE)) )
	{
		send_to_char( "Non riesci a trasferirti sul tuo obiettivo.\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Non puoi creare il portale.\n\r", ch );
		return;
	}

	if ( ch->pet != NULL && ch->in_room == ch->pet->in_room )
		gate_pet = TRUE;
	else
		gate_pet = FALSE;

	for ( wch = ch->in_room->people; wch != NULL; wch = wch_next )
	{
		wch_next = wch->next_in_room;
		if ( is_same_group(wch, ch) && wch != ch )
		{
			act( "$n pronuncia qualche arcana parola e il tempo sembra fermarsi per un attimo", ch, NULL, wch, TO_VICT, POS_REST	);
			if ( victim->in_room == NULL )
			{
				bug( "Tesseract: victim room has become NULL!!!", 0 );
				return;
			}

			char_from_room( wch );
			char_to_room( wch, victim->in_room );
			act( "$n arriva improvvisamente.. dal nulla!", wch, NULL, NULL, TO_ROOM, POS_REST );
			if ( wch->in_room == NULL )
				bug( "Tesseract: other char sent to NULL room", 0 );
			else
				do_look( wch, "auto" );
		}
	}

	act( "In un tiepido bagliore di luce $n scompare!", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Disegni un'ardita trama con le dita nell'aria, il tempo e lo spazio si fermano\n\r"
		"mentre tu e i tuoi compagni affrontate il viaggio verso la vittima..\n\r", ch );
	char_from_room( ch );
	char_to_room( ch, victim->in_room );

	act( "$n appare improvvisamente.", ch, NULL, NULL, TO_ROOM, POS_REST );
	if ( ch->in_room == NULL )
		bug( "Tesseract: char sent to NULL room", 0 );
	else
		do_look( ch, "auto" );

	if ( gate_pet )
	{
		send_to_char( "Il tempo e lo spazio ruotano intorno a te vorticosamente..\n\r", ch->pet );
		char_from_room( ch->pet );
		char_to_room( ch->pet, victim->in_room );
		act( "$n appare improvvisamente.", ch->pet, NULL, NULL, TO_ROOM, POS_REST );
		if ( ch->pet->in_room == NULL )
			bug( "Tesseract: pet sent to NULL room", 0 );
		else
			do_look( ch->pet, "auto" );
	}
}


/*
 * Brew
 */
void spell_brew( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	OBJ_DATA	*potion;
	OBJ_DATA	*vial;
	int			spell;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type != ITEM_TRASH && obj->item_type != ITEM_TREASURE && obj->item_type != ITEM_KEY )
	{
		send_to_char( "Umh.. prova con un altro tipo d'oggetto.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "L'oggetto dev'essere nell'inventario.\n\r", ch );
		return;
	}

	for ( vial = ch->carrying; vial != NULL; vial = vial->next_content )
	{
		if ( vial->pIndexData->vnum == OBJ_VNUM_POTION_VIAL )
			break;
	}

	if ( vial == NULL )
	{
		send_to_char( "Procurati una fiala in cui mettere la pozione, prima.\n\r", ch );
		return;
	}

	if ( number_percent() < 50 )
	{
		send_to_char( "Combini un piccolo disastro.. niente pozione, niente piu' oggetto.\n\r", ch );
		extract_obj( obj );
		return;
	}

	if		( obj->item_type == ITEM_TRASH )	potion = create_object( get_obj_index(OBJ_VNUM_POTION_SILVER), level );
	else if ( obj->item_type == ITEM_TREASURE )	potion = create_object( get_obj_index(OBJ_VNUM_POTION_GOLDEN), level );
	else										potion = create_object( get_obj_index(OBJ_VNUM_POTION_SWIRLING), level );

	spell = 0;

	potion->value[0] = level;

	if ( obj->item_type == ITEM_TRASH )
	{
		if ( number_percent() < 20 )		spell = skill_lookup( "fireball"		);
		else if ( number_percent() < 40 )	spell = skill_lookup( "cure poison" 	);
		else if ( number_percent() < 60 )	spell = skill_lookup( "cure blind"		);
		else if ( number_percent() < 80 )	spell = skill_lookup( "cure disease"	);
		else								spell = skill_lookup( "word of recall"	);
	}
	else if ( obj->item_type == ITEM_TREASURE )
	{
		switch ( number_bits(3) )
		{
		case 0:		spell = skill_lookup( "cure critical" );	break;
		case 1:		spell = skill_lookup( "haste" );			break;
		case 2:		spell = skill_lookup( "frenzy" );			break;
		case 3:		spell = skill_lookup( "create spring" );	break;
		case 4:		spell = skill_lookup( "holy word" );		break;
		case 5:		spell = skill_lookup( "invis" );			break;
		case 6:		spell = skill_lookup( "cure light" );		break;
		case 7:		spell = skill_lookup( "cure serious" );		break;
		}
	}
	else
	{
		if		( number_percent() < 20 )	spell = skill_lookup( "detect magic" );
		else if ( number_percent() < 40 )	spell = skill_lookup( "detect invis" );
		else if ( number_percent() < 65 )	spell = skill_lookup( "pass door" );
		else								spell = skill_lookup( "acute vision" );
	}

	potion->value[1] = spell;
	extract_obj( obj );
	act( "Ottieni $p!", ch, potion, NULL, TO_CHAR, POS_REST );
	act( "$n ricava $p.", ch, potion, NULL, TO_ROOM, POS_REST );

	obj_to_char( potion, ch );
	extract_obj( vial );
}


/*
 * Mastering spell
 * Gestita in magic.c
 */



/****************************
 * KNIGHT
 */


/*
 * Golden aura
 */
void spell_golden_aura( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA	*vch = vo;
	/*~~~~~~~~~~~~~~~~~~*/

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( !is_same_group(vch, ch) )
			continue;

		if ( is_affected(vch, sn) || is_affected(vch, gsn_bless) || IS_AFFECTED(vch, AFF_PROTECT_EVIL) )
		{
			if ( vch == ch )
				send_to_char( "Sei gia' sotto l'effetto di quest'aura.\n\r", ch );
			else
				act( "$N e' gia' circondat$X da quest'aura.", ch, NULL, vch, TO_CHAR, POS_REST );
			continue;
		}

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = 6 + level;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector = AFF_PROTECT_EVIL;
		affect_to_char( vch, &af );

		af.modifier = level / 8;
		af.location = APPLY_HITROLL;
		af.bitvector = 0;
		affect_to_char( vch, &af );

		af.modifier = 0 - level / 8;
		af.location = APPLY_SAVING_SPELL;
		affect_to_char( vch, &af );

		send_to_char( "Un'aura dorata ti circonda.\n\r", vch );
		if ( ch != vch )
			act( "Un'aura dorata circonda $N.", ch, NULL, vch, TO_CHAR, POS_REST );
	}
}


/*
 * Dragonplate
 */
void spell_dragonplate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int			plate_vnum;
	OBJ_DATA	*plate;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~*/

	plate_vnum = OBJ_VNUM_PLATE;

	plate = create_object( get_obj_index(plate_vnum), level + 5 );
	plate->timer = 2 * level;

	plate->cost = 0;
	plate->level = ch->level;

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = level / 8;
	af.bitvector = 0;

	af.location = APPLY_HITROLL;
	affect_to_obj( plate, &af );

	af.location = APPLY_DAMROLL;
	affect_to_obj( plate, &af );

	obj_to_char( plate, ch );

	act( "Invocando l'aiuto dei tuoi Dei forgi dal nulla $p!", ch, plate, NULL, TO_CHAR, POS_REST );
	act( "$n evoca dal nulla tramite l'aiuto dei suoi Dei $p!", ch, plate, NULL, TO_ROOM, POS_REST );
}


/*
 * Squire
 */
void spell_squire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*squire;
	AFFECT_DATA af;
	int			i;
	char		buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Aspetta un po' prima di farlo..\n\r", ch );
		return;
	}

	send_to_char( "Richiami a gran voce uno scudiero!\n\r", ch );
	act( "$n richiama a gran voce uno scudiero.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	gch->pIndexData->vnum == MOB_VNUM_SQUIRE )
		{
			send_to_char( "Due scudieri?Na.. troppi!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	squire = create_mobile( get_mob_index(MOB_VNUM_SQUIRE) );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		squire->perm_stat[i] = ch->perm_stat[i];
	}

	squire->max_point[LIFE] = ch->max_point[LIFE];
	squire->point[LIFE] = squire->max_point[LIFE];
	squire->max_point[MANA] = ch->max_point[MANA];
	squire->point[MANA] = squire->max_point[MANA];
	squire->level = ch->level;
	for ( i = 0; i < 3; i++ )
		squire->armor[i] = interpolate( squire->level, 100, -100 );
	squire->armor[3] = interpolate( squire->level, 100, 0 );
	squire->gold = 0;

	sprintf( buf, squire->short_descr, ch->name );
	free_string( squire->short_descr );
	squire->short_descr = str_dup( buf );

	sprintf( buf, squire->long_descr, ch->name );
	free_string( squire->long_descr );
	squire->long_descr = str_dup( buf );

	sprintf( buf, squire->description, ch->name );
	free_string( squire->description );
	squire->description = str_dup( buf );

	squire->damage[DICE_NUMBER] = number_range( level / 15, level / 12 );
	squire->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	squire->damage[DICE_BONUS] = number_range( level / 8, level / 6 );

	char_to_room( squire, ch->in_room );
	send_to_char( "Uno scudiero giunge immediatamente, pronto a servirti!\n\r", ch );
	act( "Uno scudiero arriva prontamente!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( squire->affected_by, AFF_CHARM );
	squire->master = squire->leader = ch;
}


/*
 * Dragonsword
 */
void spell_dragonsword( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int			sword_vnum;
	OBJ_DATA	*sword;
	char		arg[MIL];
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	target_name = one_argument( target_name, arg );
	sword_vnum = 0;

	if		( !str_cmp(arg, "spada")   || !str_cmp(arg, "sword") )	sword_vnum = OBJ_VNUM_DRAGONSWORD;
	else if ( !str_cmp(arg, "mazza")   || !str_cmp(arg, "mace") )	sword_vnum = OBJ_VNUM_DRAGONMACE;
	else if ( !str_cmp(arg, "pugnale") || !str_cmp(arg, "dagger") )	sword_vnum = OBJ_VNUM_DRAGONDAGGER;
	else if ( !str_cmp(arg, "lancia")  || !str_cmp(arg, "lance") )	sword_vnum = OBJ_VNUM_DRAGONLANCE;
	else
	{
		send_to_char( "E che tipo di dragonsword sarebbe?!", ch );
		return;
	}

	sword = create_object( get_obj_index(sword_vnum), level );
	sword->timer = level * 2;
	sword->cost = 0;
	if ( ch->level < 50 )
		sword->value[2] = ( ch->level / 10 );
	else
		sword->value[2] = ( ch->level / 6 ) - 3;
	sword->level = ch->level;

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = level / 5;
	af.bitvector = 0;

	af.location = APPLY_HITROLL;
	affect_to_obj( sword, &af );

	af.location = APPLY_DAMROLL;
	affect_to_obj( sword, &af );

	if		( IS_GOOD(ch) )		SET_BIT( sword->extra_flags, (ITEM_ANTI_NEUTRAL | ITEM_ANTI_EVIL) );
	else if ( IS_NEUTRAL(ch) )	SET_BIT( sword->extra_flags, (ITEM_ANTI_GOOD | ITEM_ANTI_EVIL) );
	else if ( IS_EVIL(ch) )		SET_BIT( sword->extra_flags, (ITEM_ANTI_NEUTRAL | ITEM_ANTI_GOOD) );
	obj_to_char( sword, ch );

	act( "Con l'aiuto degli Dei invochi dal nulla $p!", ch, sword, NULL, TO_CHAR, POS_REST );
	act( "$n con l'aiuto degli Dei invoca dal nulla $p!", ch, sword, NULL, TO_ROOM, POS_REST );
}


/*
 * Holy armor
 */
void spell_holy_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai gia' questo tipo di protezione.", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level;
	af.location = APPLY_AC;
	af.modifier = -UMAX( 10, 10 * (level / 5) );
	af.bitvector = 0;
	affect_to_char( ch, &af );
	act( "$n si circonda di una robusta aura protettiva.", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Ti circondi con una robusta aura protettiva.\n\r", ch );
}


/*
 * Riding
 * Gestita in act_move.c
 */



/****************************
 * LION
 */


/*
 * Guard dogs
 */
void spell_guard_dogs( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*dog;
	CHAR_DATA	*dog2;
	AFFECT_DATA af;
	int			i;
	/*~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non puoi farlo ora.. aspetta ancora un po'.\n\r", ch );
		return;
	}

	send_to_char( "Tenti di richiamare un cane.\n\r", ch );
	act( "$n fischia a lungo tentando forse di richiamare qualcosa..", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch) && IS_AFFECTED(gch, AFF_CHARM) && gch->master == ch && gch->pIndexData->vnum == MOB_VNUM_DOG )
		{
			send_to_char( "Puoi controllarne solo due alla volta!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	dog = create_mobile( get_mob_index(MOB_VNUM_DOG) );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		dog->perm_stat[i] = ch->perm_stat[i];
	}

	dog->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		URANGE(	ch->pcdata->perm_point[LIFE], ch->point[LIFE], 30000 );
	dog->point[LIFE] = dog->max_point[LIFE];
	dog->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	dog->point[MANA] = dog->max_point[MANA];
	dog->level = ch->level;
	for ( i = 0; i < 3; i++ )
		dog->armor[i] = interpolate( dog->level, 100, -100 );
	dog->armor[3] = interpolate( dog->level, 100, 0 );
	dog->gold = 0;
	dog->timer = 0;
	dog->damage[DICE_NUMBER] = number_range( level / 15, level / 12 );
	dog->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	dog->damage[DICE_BONUS] = number_range( level / 10, level / 8 );

	dog2 = create_mobile( dog->pIndexData );
	clone_mobile( dog, dog2 );

	SET_BIT( dog->affected_by, AFF_CHARM );
	SET_BIT( dog2->affected_by, AFF_CHARM );
	dog->master = dog2->master = ch;
	dog->leader = dog2->leader = ch;

	char_to_room( dog, ch->in_room );
	char_to_room( dog2, ch->in_room );
	send_to_char( "Ecco due canidi, pronti a servirti!\n\r", ch );
	act( "Noti improvvisamente l'arrivo di due canidi.", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );
}


/*
 * Eyes of tiger
 */
void spell_eyes_of_tiger( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA		*victim;
	ROOM_INDEX_DATA *ori_room;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Non vedi nessuno con questo nome in zona.\n\r", ch );
		return;

	}

	if ( IS_MOB(victim) || victim->cabal != CABAL_HUNTER )
	{
		send_to_char( "Puoi distinguere solo compagni di cabala!\n\r", ch );
		return;
	}

	if ( (victim->level > ch->level + 7) || saves_spell((ch->level + 9), victim, DAM_NONE) )
	{
		send_to_char( "Non riesci a concentrarti su nessuno con quel nome.\n\r", ch );
		return;
	}

	if ( ch == victim )
		do_look( ch, "auto" );
	else
	{
		ori_room = ch->in_room;
		char_from_room( ch );
		char_to_room( ch, victim->in_room );
		do_look( ch, "auto" );
		char_from_room( ch );
		char_to_room( ch, ori_room );
	}
}


/*
 * Lion shield
 */
void spell_lion_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*shield;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~*/

	shield = create_object( get_obj_index(OBJ_VNUM_LION_SHIELD), level );
	shield->timer = level;
	shield->level = ch->level;
	shield->cost = 0;
	obj_to_char( shield, ch );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = level / 8;
	af.bitvector = 0;

	af.location = APPLY_HITROLL;
	affect_to_obj( shield, &af );

	af.location = APPLY_DAMROLL;
	affect_to_obj( shield, &af );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = -( level * 2 ) / 3;
	af.bitvector = 0;
	af.location = APPLY_AC;
	affect_to_obj( shield, &af );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = UMAX( 1, level / 30 );
	af.bitvector = 0;

	af.location = APPLY_CHA;
	affect_to_obj( shield, &af );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = -1;
	af.modifier = -level / 9;
	af.bitvector = 0;
	af.location = APPLY_SAVING_SPELL;
	affect_to_obj( shield, &af );

	act( "Crei dal nulla $p!", ch, shield, NULL, TO_CHAR, POS_REST );
	act( "$n crea dal nulla $p!", ch, shield, NULL, TO_ROOM, POS_REST );
}


/*
 * Evolve lion
 */
void spell_evolve_lion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) || ch->point[LIFE] > ch->max_point[LIFE] )
	{
		send_to_char( "Sei gia' sotto l'effetto di questo incantesimo.\n\r", ch );
		return;
	}

	ch->point[LIFE] += ch->pcdata->perm_point[LIFE];

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3 + level / 30;
	af.location = APPLY_LIFE;
	af.modifier = ch->pcdata->perm_point[LIFE];
	af.bitvector = 0;
	affect_to_char( ch, &af );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3 + level / 30;
	af.location = APPLY_DEX;
	af.modifier = -( 1 + level / 30 );
	af.bitvector = AFF_SLOW;
	affect_to_char( ch, &af );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3 + level / 30;
	af.location = APPLY_DAMROLL;
	af.modifier = level / 2;
	af.bitvector = AFF_BERSERK;
	affect_to_char( ch, &af );

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3 + level / 30;
	af.location = APPLY_NONE;

	af.modifier = 0;
	af.bitvector = ADET_LION;
	affect_to_char( ch, &af );

	send_to_char( "Ti senti molto piu' forte!\n\r", ch );
	act( "La pelle di $n diventa viola!", ch, NULL, NULL, TO_ROOM, POS_REST );
}


/*
 * Claw
 */
void do_claw( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			chance;
	bool		FightingCheck;
	int			damage_claw;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_claw, 1, 0, NULL) )
		return;

	if ( ch->fighting != NULL )
		FightingCheck = TRUE;
	else
		FightingCheck = FALSE;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		victim = ch->fighting;
		if ( victim == NULL )
		{
			send_to_char( "Ehi sveglia.. non stai combattendo.\n\r", ch );
			return;
		}
	}
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Ps.. non e' qui!\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Meglio una ghigliottina.\n\r", ch );
		return;

	}

	if ( is_safe(ch, victim) )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "Oh non alla tua adorabile guida.. giusto?", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( ch->point[MANA] < 50 )
	{
		act( "Sei spossat$x, hai bisogno di piu' energia.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	/* size and weight */
	chance = get_skill( ch, gsn_claw );

	if ( CAN_DETECT(ch, ADET_LION) )
		chance += 25;

	/* stats */
	chance += get_curr_stat( ch, STR ) + get_curr_stat( ch, DEX );
	chance -= get_curr_stat( victim, DEX ) * 2;

	if ( IS_AFFECTED(ch, AFF_FLYING) )
		chance -= 10;

	/* speed */
	if ( IS_AFFECTED(ch, AFF_HASTE) )
		chance += 10;
	if ( IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE) )
		chance -= 20;

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* now the attack */
	if ( number_percent() < chance )
	{
		ch->point[MANA] -= 50;
		check_improve( ch, gsn_claw, TRUE, 1 );
		victim->position = POS_REST;

		damage_claw = ch->size * 10;
		if ( CAN_DETECT(ch, ADET_LION) )
		{
			WAIT_STATE( ch, skill_table[gsn_claw].beats / 2 );
			damage_claw += dice( ch->level, 12 ) + ch->damroll;
		}
		else
		{
			WAIT_STATE( ch, skill_table[gsn_claw].beats );
			damage_claw += dice( ch->level, 24 ) + ch->damroll;
		}

		damage( ch, victim, damage_claw, gsn_claw, DAM_BASH, TRUE );
	}
	else
	{
		ch->point[MANA] -= 25;
		damage( ch, victim, 0, gsn_claw, DAM_BASH, TRUE );
		check_improve( ch, gsn_claw, FALSE, 1 );
		ch->position = POS_REST;
		WAIT_STATE( ch, skill_table[gsn_claw].beats / 2 );
	}
}


/*
 * Prevent
 */
void spell_prevent( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af, af2;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "La zona e' gia' sotto l'effetto di quest'incantesimo!\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 30;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_PREVENT;
	affect_to_room( ch->in_room, &af );

	af2.where = TO_AFFECTS;
	af2.type = sn;
	af2.level = level;
	af2.duration = level / 10;
	af2.modifier = 0;
	af2.location = APPLY_NONE;
	af2.bitvector = 0;
	affect_to_char( ch, &af2 );
	send_to_char( "La zona e' ora protetta dalla vendetta dei Cacciatori!\n\r", ch );
	act( "Uno strano alone si diffonde per la zona.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_enlarge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Ancora?!\n\r", ch );
		else
			act( "$N e' gia' abbastanza gross$X!.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 2;
	af.location = APPLY_SIZE;
	af.modifier = 1 + ( level >= 35 ) + ( level >= 65 );
	af.modifier = UMIN( (SIZE_GARGANTUAN - victim->size), af.modifier );
	af.bitvector = 0;
	affect_to_char( victim, &af );

	send_to_char( "Senti i tuoi muscoli ingrossare.. le ossa allungarsi.. diventi enorme!\n\r", victim );
	act( "Il corpo $n comincia a modificarsi.. come quello di un gigante!", victim, NULL, NULL, TO_ROOM, POS_REST );
}



/****************************
 * HUNTER
 */


/*
 * Hunt
 * Gestito in act_hera.c
 */


/*
 * Find object
 */
void spell_find_object( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char		buf[MIL];
	BUFFER		*buffer;
	OBJ_DATA	*obj;
	OBJ_DATA	*in_obj;
	bool		found;
	int			number = 0, max_found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	found = FALSE;
	number = 0;
	max_found = IS_ADMIN( ch ) ? 200 : 2 * level;

	buffer = new_buf();

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( !can_see_obj(ch, obj)
		||	!is_name(target_name, obj->name)
		||	number_percent() > 2 * level
		||	ch->level < obj->level )
			continue;

		found = TRUE;
		number++;

		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
			;

		if ( in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by) )
			sprintf( buf, "Un oggetto di questo tipo e' in possesso di %s\n\r", PERS(in_obj->carried_by, ch) );
		else
		{
			if ( IS_ADMIN(ch) && in_obj->in_room != NULL )
			{
				sprintf( buf, "Un oggetto di questo tipo e' in %s [Room %d]\n\r",
					in_obj->in_room->name, in_obj->in_room->vnum );
			}
			else
			{
				sprintf( buf, "Un oggetto di questo tipo e' in %s\n\r",
					in_obj->in_room == NULL ? "da qualche parte" : in_obj->in_room->name );
			}
		}

		buf[0] = UPPER( buf[0] );
		add_buf( buffer, buf );

		if ( number >= max_found )
			break;
	}

	if ( !found )
		send_to_char( "Non riesci a trovare nulla del genere ne' in cielo ne' in terra.\n\r", ch );
	else
		page_to_char( buf_string(buffer), ch );

	free_buf( buffer );
}


/*
 * Path find
 * Gestito in update.c
 */


/*
 * Wolf
 */
void spell_wolf( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*wolf;
	AFFECT_DATA af;
	int			i;
	/*~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza forse, per ora.\n\r", ch );
		return;
	}

	send_to_char( "Lanci il richiamo del lupo..\n\r", ch );
	act( "$n emette uno strano urlo.. sembra un ululato.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch) && IS_AFFECTED(gch, AFF_CHARM) && gch->master == ch && gch->pIndexData->vnum == MOB_VNUM_WOLF )
		{
			send_to_char( "Puoi controllare solo un lupo alla volta!\n\r", ch );
			return;
		}
	}

	if ( count_charmed(ch) )
		return;

	wolf = create_mobile( get_mob_index(MOB_VNUM_WOLF) );

	for ( i = 0; i < MAX_STATS; i++ )
	{
		wolf->perm_stat[i] = ch->perm_stat[i];
	}

	wolf->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		URANGE( ch->pcdata->perm_point[LIFE], ch->point[LIFE], 30000 );
	wolf->point[LIFE] = wolf->max_point[LIFE];
	wolf->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	wolf->point[MANA] = wolf->max_point[MANA];
	wolf->level = ch->level;
	for ( i = 0; i < 3; i++ )
		wolf->armor[i] = interpolate( wolf->level, 100, -100 );
	wolf->armor[3] = interpolate( wolf->level, 100, 0 );
	wolf->gold = 0;
	wolf->timer = 0;
	wolf->damage[DICE_NUMBER] = number_range( level / 15, level / 10 );
	wolf->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
	wolf->damage[DICE_BONUS] = number_range( level / 6, level / 5 );

	char_to_room( wolf, ch->in_room );
	send_to_char( "Ecco il lupo, pronto a servirti!\n\r", ch );
	act( "Un lupo arriva da.. da.. da dove?!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( wolf->affected_by, AFF_CHARM );
	wolf->master = wolf->leader = ch;
}


/*
 * Wolf spirit
 */
void spell_wolf_spirit( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Sei gia' sotto questo tipo di incantesimo!\n\r", ch );
		return;
	}

	/* haste */
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3 + level / 30;
	af.location = APPLY_DEX;
	af.modifier = 1 + ( level > 40 ) + ( level > 60 );
	af.bitvector = AFF_HASTE;
	affect_to_char( ch, &af );

	/* damroll */
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3 + level / 30;
	af.location = APPLY_DAMROLL;
	af.modifier = level / 2;
	af.bitvector = AFF_BERSERK;
	affect_to_char( ch, &af );

	/* infravision */
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3 + level / 30;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_INFRARED;
	affect_to_char( ch, &af );

	send_to_char( "Il sangue nelle tue vene comincia a scorrere piu' velocemente.\n\r", ch );
	act( "Gli occhi di $n diventano completamente ROSSI!", ch, NULL, NULL, TO_ROOM, POS_REST );
}


/*
 * Armor use
 * Gestita in fight.c
 */


/*
 * World find
 * Gestita in act_hera.c
 */


/*
 * Take revenge
 */
void spell_take_revenge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA		*obj;
	OBJ_DATA		*in_obj;
	ROOM_INDEX_DATA *room = NULL;
	bool			found = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) || ch->last_death_time == -1 || current_time - ch->last_death_time > 600 )
	{
		send_to_char( "E' troppo tardi per la vendetta.. ormai.\n\r", ch );
		return;
	}

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( obj->pIndexData->vnum != OBJ_VNUM_CORPSE_PC || !is_name(ch->name, obj->short_descr) )
			continue;

		found = TRUE;
		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
			;

		if ( in_obj->carried_by != NULL )
			room = in_obj->carried_by->in_room;
		else
			room = in_obj->in_room;
		break;
	}

	if ( !found || room == NULL )
		send_to_char( "Il tuo corpo e' stato divorato..\n\r", ch );
	else
	{
		ROOM_INDEX_DATA *prev_room;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		prev_room = ch->in_room;
		act( "$n prega gli Dei perche' lo trasportino.", ch, NULL, NULL, TO_ROOM, POS_REST );
		char_from_room( ch );
		char_to_room( ch, room );
		if ( cabal_area_check(ch) || IS_ROOM_AFFECTED(ch->in_room, AFF_ROOM_PREVENT) )
		{
			send_to_char( " Non riesci a raggiungere il tuo corpo.\n\r", ch );
			char_from_room( ch );
			char_to_room( ch, prev_room );
		}
		else
			do_look( ch, "auto" );
	}
}
