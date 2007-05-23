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

#include "xandra.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"


/*
 * Funzioni esterne:
 */
char *get_skill_alias( const int learned );


bool CLEVEL_OK( CHAR_DATA *ch, int skill )
{
	if ( ch->level >= skill_table[skill].skill_level[ch->class] )
		return TRUE;
	else
		return FALSE;
}


bool RACE_OK( CHAR_DATA *ch, int skill )
{
	if IS_SET( skill_table[skill].race, (1 << (ORG_RACE(ch) - 1)) )
		return TRUE;
	else
		return FALSE;
}


bool CABAL_OK( CHAR_DATA *ch, int skill )
{
	if ( skill_table[skill].cabal == CABAL_NONE || skill_table[skill].cabal == ch->cabal )
		return TRUE;
	else
		return FALSE;
}


bool ALIGN_OK( CHAR_DATA *ch, int skill )
{
	if ( skill_table[skill].align == -1 || skill_table[skill].align == ch->alignment )
		return TRUE;
	else
		return FALSE;
}


/*
 * Return types:
 * 0: success
 * 1: general failure
 * 2: no cabal powers
 */
int skill_failure_nomessage( CHAR_DATA *ch , int skill, int npc_ok )
{
	int i;

	if ( npc_ok && IS_MOB(ch) && CABAL_OK(ch, skill)
	&& (RACE_OK(ch, skill) || IS_SET(ch->off_flags, npc_ok)) )
	return 0;


	if ( !IS_MOB(ch) && CLEVEL_OK(ch, skill) && RACE_OK(ch, skill)
	&& CABAL_OK(ch, skill) && ALIGN_OK(ch, skill) )
	{
		if ( skill_table[skill].cabal == CABAL_NONE
		|| cabal_table[ch->cabal].obj_ptr == NULL
		|| cabal_table[ch->cabal].obj_ptr->in_room == NULL
		|| cabal_table[ch->cabal].obj_ptr->in_room->vnum ==	cabal_table[ch->cabal].room_vnum )
			return 0;

		for ( i = 1; i < MAX_CABAL; i++ )
		{
			if ( cabal_table[ch->cabal].obj_ptr->in_room->vnum == cabal_table[i].room_vnum )
				return 2;
		}
	}

	return 1;
}


int skill_failure_check( CHAR_DATA *ch, int skill, int mount_ok, int npc_ok, char *msg )
{
	int e;

	if ( !mount_ok && MOUNTED(ch) )
	{
		send_to_char( "Non puoi farlo mentre cavalchi!\n\r", ch );
		return 1;
	}

	if ( (e = skill_failure_nomessage(ch, skill, npc_ok)) != 0 )
	{
		if ( e == 2 )
			send_to_char( "Non riesci a emulare per nulla questo potere da Cabala.\n\r", ch );
		else if ( msg )
			act( msg, ch, NULL, NULL, TO_CHAR, POS_DEAD );
		else
			send_to_char( "Come?\n\r", ch );

		return 1;
	}

	return 0;
}


/*
 * Used to converter of prac and train
 */
void do_gain( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*trainer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	/* find a trainer */
	for ( trainer = ch->in_room->people; trainer != NULL; trainer = trainer->next_in_room )
	{
		if ( IS_MOB(trainer) && (IS_SET(trainer->act, ACT_PRACTICE) || IS_SET(trainer->act, ACT_TRAIN) || IS_SET(trainer->act, ACT_GAIN)) )
			break;
	}

	if ( trainer == NULL || !can_see(ch, trainer) )
	{
		send_to_char( "Non puoi farlo qui.\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		do_say( trainer, "Puoi convertire dieci sessioni di pratica in un allenamento." );
		do_say( trainer, "Puoi convertire una sessione di allenamento in una di pratica." );
		do_say( trainer, "Prova con 'gain convert' o 'gain revert'." );
		return;
	}

	if ( !str_prefix(arg, "inverti") || !str_prefix(arg, "revert") )
	{
		if ( ch->train < 1 )
		{
			act( "$N ti dice 'Eh no.. non ancora!'", ch, NULL, trainer, TO_CHAR, POS_REST );
			return;
		}

		act( "$N ti aiuta ad migliorarti attraverso l'allenamento.", ch, NULL, trainer, TO_CHAR, POS_REST );
		ch->practice += 10;
		ch->train -= 1;
		return;
	}

	if ( !str_prefix(arg, "converti") )
	{
		if ( ch->practice < 10 )
		{
			act( "$N ti dice 'Ehi, non cosi' in fretta!.'", ch, NULL, trainer, TO_CHAR, POS_REST );
			return;
		}

		act( "$N converte le tue sessioni di pratica in allenamenti.", ch, NULL, trainer, TO_CHAR, POS_REST);
		ch->practice -= 10;
		ch->train += 1;
		return;
	}

	act( "$N ti dice 'Non credo di capire..'", ch, NULL, trainer, TO_CHAR, POS_REST );
}


/*
 * RT spells and skills show the players spells (or skills)
 */
void do_spells( CHAR_DATA *ch, char *argument )
{
	char	spell_list[LEVEL_HERO][MSL];
	char	spell_columns[LEVEL_HERO];
	int		sn, lev, mana;
	bool	found = FALSE;
	char	buf[MSL];
	char	output[4 * MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	/* initilize data */
	output[0] = '\0';
	for ( lev = 0; lev < LEVEL_HERO; lev++ )
	{
		spell_columns[lev] = 0;
		spell_list[lev][0] = '\0';
	}

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;

		if ( skill_table[sn].skill_level[ch->class] < LEVEL_HERO
		&&	 skill_table[sn].spell_fun != spell_null
		&&	 RACE_OK(ch, sn)
		&&	(skill_table[sn].cabal == ch->cabal || skill_table[sn].cabal == CABAL_NONE) )
		{
			found = TRUE;
			lev = skill_table[sn].skill_level[ch->class];
			if ( ch->level < lev )
				continue;
			else
			{
				mana = UMAX( skill_table[sn].min_mana, 100 / (2 + ch->level - lev) );
				sprintf( buf, "%-18s  %3d mana  ", skill_table[sn].name, mana );
			}

			if ( spell_list[lev][0] == '\0' )
				sprintf( spell_list[lev], "\n\r%s", buf );
			else	/* append */
			{
				if ( ++spell_columns[lev] % 2 == 0 )
					strcat( spell_list[lev], "\n\r" );
				strcat( spell_list[lev], buf );
			}
		}
	}

	/* return results */
	if ( !found )
	{
		send_to_char( "Ah.. sai di magia? Tu?\n\r", ch );
		return;
	}

	for ( lev = 0; lev < LEVEL_HERO; lev++ )
	{
		if ( spell_list[lev][0] != '\0' )
			strcat( output, spell_list[lev] );
	}

	strcat( output, "\n\r" );
	page_to_char( output, ch );
}


void do_skills( CHAR_DATA *ch, char *argument )
{
	char	skill_list[LEVEL_HERO][MSL];
	char	skill_columns[LEVEL_HERO];
	int		sn, lev;
	bool	found = FALSE;
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	/* initilize data */
	for ( lev = 0;  lev < LEVEL_HERO;  lev++ )
	{
		skill_columns[lev] = 0;
		skill_list[lev][0] = '\0';
	}

	for ( sn = 0;  sn < MAX_SKILL;  sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;

		if ( skill_table[sn].skill_level[ch->class] < LEVEL_HERO
		&&	 skill_table[sn].spell_fun == spell_null
		&&	 RACE_OK(ch, sn)
		&&	(skill_table[sn].cabal == ch->cabal || skill_table[sn].cabal == CABAL_NONE) )
		{
			found = TRUE;
			lev = skill_table[sn].skill_level[ch->class];
			if ( ch->level < lev )
			{
				if ( IS_ADMIN(ch) )
					sprintf( buf, "%-18s n/a      ", skill_table[sn].name );
				else
					continue;
			}
			else
			{
				if ( IS_ADMIN(ch) )
					sprintf( buf, "%-18s %3d%%      ", skill_table[sn].name, ch->pcdata->learned[sn] );
				else
					sprintf( buf, "%-18s %10s      ", skill_table[sn].name, get_skill_alias(ch->pcdata->learned[sn]) );
			}

			if ( skill_list[lev][0] == '\0' )
			{
				if ( IS_ADMIN(ch) )
					sprintf( skill_list[lev], "\n\r[%-2d] %s", lev, buf );
				else
					sprintf( skill_list[lev], "\n\r%s", buf );
			}
			else	/* append */
			{
				if ( ++skill_columns[lev] % 2 == 0 )
				{
					if ( IS_ADMIN(ch) )
						strcat( skill_list[lev], "\n\r     " );
					else
						strcat( skill_list[lev], "\n\r" );
				}
				strcat( skill_list[lev], buf );
			}
		}
	}

	/* return results */
	if ( !found )
	{
		send_to_char( "Umh.. abilita'.. tu? Ma dai..\n\r", ch );
		return;
	}

	for ( lev = 0; lev < LEVEL_HERO; lev++ )
	{
		if ( skill_list[lev][0] != '\0' )
			send_to_char( skill_list[lev], ch );
	}

	send_to_char( "\n\r", ch );
}


int base_exp( CHAR_DATA *ch, int points )
{
	int expl;
	/*~~~~~*/

	if ( IS_MOB(ch) )
		return( 1000 );

	expl = 1000 + (ch->level * 25) + pc_race_table[ORG_RACE(ch)].points + class_table[ch->class].points;
	return( expl * pc_race_table[ORG_RACE(ch)].class_mult[ch->class] / 100 );
}


int exp_to_level( CHAR_DATA *ch, int points )
{
	int base;
	/*~~~~~*/

	base = base_exp( ch, points );
	return( base - exp_this_level(ch, ch->level, points) );
}


int exp_this_level( CHAR_DATA *ch, int level, int points )
{
	int base;
	/*~~~~~*/

	base = exp_per_level( ch, points );
	return( ch->exp - (ch->level * base) );
}


int exp_per_level( CHAR_DATA *ch, int points )
{
	int expl;
	/*~~~~~*/

	if ( IS_MOB(ch) )
		return( 1000 );

	expl = 1000 + (ch->level * 25) + pc_race_table[ORG_RACE(ch)].points + class_table[ch->class].points;

	return( expl * pc_race_table[ORG_RACE(ch)].class_mult[ch->class] / 100 );
}


/*
 *    checks for skill improvement
 */
void check_improve( CHAR_DATA *ch, int sn, bool success, int multiplier )
{
	int		chance;
	char	buf[100];
	/*~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( ch->level < skill_table[sn].skill_level[ch->class]
	||	 skill_table[sn].rating[ch->class] == 0
	||	 ch->pcdata->learned[sn] == 0
	||	 ch->pcdata->learned[sn] == 100 )
		return;	/* skill is not known */

	/* check to see if the character has a chance to learn */
	chance = 10 * int_app[get_curr_stat( ch, INT )].learn;
	chance /= ( multiplier * skill_table[sn].rating[ch->class] * 4 );
	chance += ch->level;

	if ( number_range(1, 1000) > chance )
		return;

	/* now that the character has a CHANCE to learn, see if they really have */
	if ( success )
	{
		chance = URANGE( 5, 100 - ch->pcdata->learned[sn], 95 );
		if ( number_percent() < chance )
		{
			if ( IS_ADMIN(ch) )
			{
				sprintf( buf, "$CMigliori la tua conoscenza in %s!$c", skill_table[sn].name );
				act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD, GREEN );
			}
			ch->pcdata->learned[sn]++;
			gain_exp( ch, skill_table[sn].rating[ch->class] );
		}
	}
	else
	{
		chance = URANGE( 5, ch->pcdata->learned[sn] / 2, 30 );
		if ( number_percent() < chance )
		{
			if ( IS_ADMIN(ch) )
			{
				sprintf( buf, "$CImpari dai tuoi errori, diventi migliore in %s.$c", skill_table[sn].name );
				act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD, GREEN );
			}

			/* Se non conosce la skill al 70% impara, altrimenti gli errori diventano devolutivi per la conoscenza */
			if ( ch->pcdata->learned[sn] < 70 )
				ch->pcdata->learned[sn]++;
			else
				ch->pcdata->learned[sn]--;

			ch->pcdata->learned[sn] = UMIN( ch->pcdata->learned[sn], 100 );
			gain_exp( ch, skill_table[sn].rating[ch->class] );
		}
	}
}


/*
 * Use for adding all skills available for that ch
 */
void group_add( CHAR_DATA *ch )
{
	int sn;
	/*~~~*/

	if ( IS_MOB(ch) )	/* NPCs do not have skills */
		return;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].cabal == 0
		&&	 RACE_OK(ch, sn)
		&&	 ch->pcdata->learned[sn] < 1
		&&	 skill_table[sn].skill_level[ch->class] < LEVEL_ADMIN )
			ch->pcdata->learned[sn] = 1;
	}

	return;
}


void do_slist( CHAR_DATA *ch, char *argument )
{
	char	skill_list[LEVEL_HERO][MSL];
	char	skill_columns[LEVEL_HERO];
	int		sn, lev, class;
	bool	found = FALSE;
	char	output[4 * MSL];
	char	buf[MSL];
	char	arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	output[0] = '\0';
	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "sintassi: slist <nome della classe>.\n\r", ch );
		return;
	}

	class = class_lookup( arg );
	if ( class == -1 )
	{
		send_to_char( "Non e' il nome di una classe compresa nell'albo.\n\r", ch );
		return;
	}

	/* initilize data */
	for ( lev = 0; lev < LEVEL_HERO; lev++ )
	{
		skill_columns[lev] = 0;
		skill_list[lev][0] = '\0';
	}

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;

		if ( skill_table[sn].skill_level[class] < LEVEL_HERO && skill_table[sn].cabal == CABAL_NONE && skill_table[sn].race == RACE_NONE )
		{
			found = TRUE;
			lev = skill_table[sn].skill_level[class];
			sprintf( buf, "%-18s          ", skill_table[sn].name );
			if ( skill_list[lev][0] == '\0' )
				sprintf( skill_list[lev], "\n\r[%-2d] %s", lev, buf );
			else	/* append */
			{
				if ( ++skill_columns[lev] % 2 == 0 )
					strcat( skill_list[lev], "\n\r     " );
				strcat( skill_list[lev], buf );
			}
		}
	}

	/* return results */
	if ( !found )
	{
		send_to_char( "Questa classe non ha nessun tipo di abilita'.\n\r", ch );
		return;
	}

	for ( lev = 0; lev < LEVEL_HERO; lev++ )
	{
		if ( skill_list[lev][0] != '\0' )
			strcat( output, skill_list[lev] );
	}

	strcat( output, "\n\r" );
	page_to_char( output, ch );
}


/*
 *    returns group number
 */
int group_lookup( const char *name )
{
	int gr;
	/*~~~*/

	for ( gr = 0; prac_table[gr].sh_name != NULL; gr++ )
	{
		if ( LOWER(name[0]) == LOWER(prac_table[gr].sh_name[0]) && !str_prefix(name, prac_table[gr].sh_name) )
			return( gr );
	}

	return( -1 );
}


void do_glist( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	char	buf[MSL];
	int		group, count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "\n\rSintassi : glist <gruppo>\n\r", ch );
		return;
	}

	if ( (group = group_lookup(arg)) == -1 )
	{
		send_to_char( "Non e' un valido nome di gruppo.\n\r", ch );
		return;
	}

	sprintf( buf, "Lista del gruppo %s :\n\r", prac_table[group].sh_name );
	send_to_char( buf, ch );
	buf[0] = '\0';
	for ( count = 0; count < MAX_SKILL; count++ )
	{
		if ( (group == GROUP_NONE && !CLEVEL_OK(ch, count) && skill_table[count].group == GROUP_NONE)
		||	 (group != skill_table[count].group)
		||	 !CABAL_OK(ch, count) )
			continue;

		if ( buf[0] != '\0' )
		{
			sprintf( buf, "%-18s%-18s\n\r", buf, skill_table[count].name );
			send_to_char( buf, ch );
			buf[0] = '\0';
		}
		else
			sprintf( buf, "%-18s", skill_table[count].name );
	}
}


void do_slook( CHAR_DATA *ch, char *argument )
{
	int			sn;
	char		arg1[MIL];
	char		arg2[MIL];
	char		buf[MSL];
	CHAR_DATA	*victim;
	BUFFER		*buffer;
	bool		sem;
	sh_int		group;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' )
	{
		if ( IS_SET(ch->act, PLR_ENGLISH) )
		{
			send_to_char( "\n\rSintassi : slook <nome abilita' o incantesimo>.\n\r", ch );
			send_to_char( "           slook maestro <nome gruppo di insegnamento>.\n\r", ch );
		}
		else
		{
			send_to_char( "\n\rSintassi : categoria <nome abilita' o incantesimo>.\n\r", ch );
			send_to_char( "           categoria maestro <nome gruppo di insegnamento>.\n\r", ch );
		}
		return;
	}

	if ( !str_cmp(arg1, "maestro") || !str_cmp(arg1, "maestri")
	||	 !str_cmp(arg1, "master") || !str_cmp(arg1, "masters") )
	{
		if ( arg2[0] == '\0' )
		{
			send_to_char( "Di quale gruppo di abilita' vorresti cercare il maestro?", ch );
			return;
		}

		if ( is_name(argument, "nessuno none") )
		{
			send_to_char( "Prova dal tuo maestro di gilda.\n\r", ch );
			return;
		}

		if ( (group = group_lookup(argument)) == -1 )
		{
			send_to_char( "Non esiste nessun gruppo di insegnamento con tale nome.\n\r", ch );
			return;
		}

		sem = FALSE;
		buffer = new_buf();
		for ( victim = char_list; victim != NULL; victim = victim->next )
		{
			if ( victim -> in_room != NULL
			&&	 IS_SET(victim -> act, ACT_PRACTICE)
			&&	 can_see(ch, victim)
			&&	 can_see_room(ch, victim->in_room)
			&&	 IS_MOB(victim) )
			{
				if ( (victim->pIndexData->practicer == 0 && group == GROUP_NONE)
				||	 (victim->pIndexData->practicer & (1 << prac_table[group].number)) )
				{
					sem = TRUE;

					sprintf( buf, "Il maestro %s nell'area %s\n\r",
					IS_MOB(victim) ? victim->short_descr : victim->name,
					victim->in_room->area->name );
					add_buf( buffer,buf );
				}
			}
		}

		if ( !sem )
			send_to_char( "Non hai trovato nessun maestro.\n\r", ch );
		else
			page_to_char( buf_string(buffer), ch );

		free_buf( buffer );
		return;
	}

	if ( (sn = skill_lookup(arg1)) == -1 )
	{
		send_to_char( "Non e' ne' un'abilita' ne' un incantesimo.\n\r", ch );
		return;
	}

	sprintf( buf, "Abilita': %s nel gruppo %s.\n\r", skill_table[sn].name, prac_table[skill_table[sn].group].sh_name );
	send_to_char( buf, ch );
}


#define PC_PRACTICER	123

void do_learn( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MIL];
	int			sn;
	CHAR_DATA	*mob;
	int			adept;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( !IS_AWAKE(ch) )
	{
		send_to_char( "Dovresti svegliarti prima..\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "\n\rSintassi: learn <abilita' | incantesimo> <avventuriero>.\n\r", ch );
		return;
	}

	if ( ch->practice <= 0 )
	{
		send_to_char( "Non hai alcuna sessione di pratica per imparare.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( (sn = find_spell(ch, arg)) < 0
	||	(!IS_MOB(ch)
		&& (ch->level < skill_table[sn].skill_level[ch->class]
			||	!RACE_OK(ch, sn)
			||	(skill_table[sn].cabal != ch->cabal && skill_table[sn].cabal != CABAL_NONE))) )
	{
		send_to_char( "Non puoi praticare cio'.\n\r", ch );
		return;
	}

	if ( sn == gsn_vampire )
	{
		send_to_char( "Non e' una cosa che puoi praticare cosi' su due piedi.. chiedi a Questor!\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( (mob = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Il tuo maestro eroe non e' qui.\n\r", ch );
		return;
	}

	if ( IS_MOB(mob) || mob->level != LEVEL_HERO )
	{
		send_to_char( "Devi trovare un Eroe che conosca perfettamente l'abilita' che vuoi imparare.\n\r", ch );
		return;
	}

	if ( mob->status != PC_PRACTICER )
	{
		send_to_char( "L'eroe non sembra disposto ad insegnare in questo momento.\n\r", ch );
		return;
	}

	if ( get_skill(mob, sn) < 100 )
	{
		send_to_char( "Il tuo maestro purtroppo non conosce abbastanza bene da insegnarti..\n\r", ch );
		return;
	}

	adept = class_table[ch->class].skill_adept;

	if ( ch->pcdata->learned[sn] >= adept )
	{
		sprintf( buf, "Hai gia' imparato quello che potevi riguardo %s.\n\r", skill_table[sn].name );
		send_to_char( buf, ch );
	}
	else
	{
		if ( !ch->pcdata->learned[sn] )
			ch->pcdata->learned[sn] = 1;
		ch->practice--;
		ch->pcdata->learned[sn] += int_app[get_curr_stat( ch, INT )].learn / UMAX( skill_table[sn].rating[ch->class], 1 );
		mob->status = 0;
		act( "Insegni $T.", mob, NULL, skill_table[sn].name, TO_CHAR, POS_REST );
		act( "$n insegna $T.", mob, NULL, skill_table[sn].name, TO_ROOM, POS_REST );
		if ( ch->pcdata->learned[sn] < adept )
		{
			act( "Impari $T.", ch, NULL, skill_table[sn].name, TO_CHAR, POS_REST );
			act( "$n impara $T.", ch, NULL, skill_table[sn].name, TO_ROOM, POS_REST );
		}
		else
		{
			ch->pcdata->learned[sn] = adept;
			act( "Hai appreso cio' che potevi riguardo $T.", ch, NULL, skill_table[sn].name, TO_CHAR, POS_REST );
			act( "$n ha appreso cio' che poteva riguardo $T.", ch, NULL, skill_table[sn].name, TO_ROOM, POS_REST );
		}
	}
}


void do_teach( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) || ch->level != LEVEL_HERO )
	{
		send_to_char( "Devi essere un Eroe per poter insegnare.\n\r", ch );
		return;
	}

	ch->status = PC_PRACTICER;
	send_to_char( "Ora puoi insegnare a chi ne ha bisogno, solo cio' che conosci perfettamente!\n\r", ch );
}
