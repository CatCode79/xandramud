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
#include <string.h>

#include "xandra.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"


DECLARE_SPEC_FUN( spec_special_guard );
DECLARE_SPEC_FUN( spec_stalker );


/*
 * Funzioni locali.
 */
void spell_cast			( CHAR_DATA *ch, char *argument );


/*
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
	int sn;
	/*~~~*/

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;
		if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0]) && !str_prefix(name, skill_table[sn].name) )
			return( sn );
	}

	return( -1 );
}


int find_spell( CHAR_DATA *ch, const char *name )
{
	/* finds a spell the character can cast if possible */
	int sn, found = -1;
	/*~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return( skill_lookup(name) );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;
		if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0]) && !str_prefix(name, skill_table[sn].name) )
		{
			if ( found == -1 )
				found = sn;
			if ( !skill_failure_nomessage(ch, sn, 0) )
				return( sn );
		}
	}

	return( found );
}


/*
 * Lookup a skill by slot number. Used for object loading.
 */
int slot_lookup( int slot )
{
	extern bool fBootDb;
	int			sn;
	/*~~~~~~~~~~~~~~~~*/

	if ( slot <= 0 )
		return( -1 );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( slot == skill_table[sn].slot )
			return( sn );
	}

	if ( fBootDb )
	{
		bug( "Slot_lookup: bad slot %d.", slot );
		abort();
	}

	return( -1 );
}


/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
	char		buf[MSL];
	char		buf2[MSL];
	char		*pName;
	int			iSyl;
	int			length;

	struct syl_type
	{
		char *old;
		char *new;
		char *cle;	/* Per chierici e paladini */
	};

	static const struct syl_type syl_table[] =
	{
		{ " ",		"ignis",	"durbentia "	  },
		{ "ar",		"abra",	 	"hatanoceo "	  },
		{ "au",		"kada",	 	"lausgera "		  },
		{ "bless",	"kalair",	"domus "		  },
		{ "blind",	"nose",	 	"noctis "		  },
		{ "bur",	"mosa",	 	"durbentia "	  },
		{ "cu",		"fa",	 	"judi "			  },
		{ "de",		"oculo", 	"exim'ha "		  },
		{ "en",		"unso",	 	"pestis "		  },
		{ "fire",	"nabt",		"maravita lux "	  },
		{ "fresh",	"ima",		"repens "		  },
		{ "light",	"seid",		"lumen "		  },
		{ "lo",		"hi",		"praansilenux "	  },
		{ "mor",	"zak",		"pretaanluxis "	  },
		{ "move",	"sido",		"permoveo "		  },
		{ "ness",	"lacri",	"rud'minoux "	  },
		{ "ning",	"illa",		"rudsceleratus "  },
		{ "per",	"duda",		"shaantitus "	  },
		{ "ra",		"gru",		"tuulenux vigra " },
		{ "re",		"rein",		"candus "		  },
		{ "son",	"sabru",	"vorox "		  },
		{ "tect",	"infra",	"caecux "		  },
		{ "tri",	"cula",		"seasrjit "		  },
		{ "ven",	"nofo",		"cruo "			  },
		{ "a",		"a",		"e" },
		{ "b",		"b",		"v" },
		{ "c",		"q",		"r" },
		{ "d",		"e",		"b" },
		{ "e",		"z",		"a" },
		{ "f",		"y",		"c" },
		{ "g",		"o",		"n" },
		{ "h",		"p",		"g" },
		{ "i",		"u",		"h" },
		{ "j",		"y",		"u" },
		{ "k",		"t",		"v" },
		{ "l",		"r",		"o" },
		{ "m",		"w",		"t" },
		{ "n",		"i",		"q" },
		{ "o",		"a",		"u" },
		{ "p",		"s",		"s" },
		{ "q",		"d",		"d" },
		{ "r",		"f",		"m" },
		{ "s",		"g",		"f" },
		{ "t",		"h",		"l" },
		{ "u",		"j",		"i" },
		{ "v",		"z",		"s" },
		{ "w",		"x",		"a" },
		{ "x",		"n",		"e" },
		{ "y",		"l",		"i" },
		{ "z",		"k",		"p" },
		{ "",		"",			"" }
	};
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	buf[0] = '\0';
	for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
	{
		for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
		{
			if ( !str_prefix(syl_table[iSyl].old, pName) )
			{
				/* Xandra: linguaggio chiericale */
				if ( ch->class == CLASS_CLERIC || ch->class == CLASS_PALADIN )
					strcat( buf, syl_table[iSyl].cle );
				else
					strcat( buf, syl_table[iSyl].new );

				break;
			}
		}

		if ( length == 0 )
			length = 1;

		/* Xandra: se l'ultima lettera e' uno spazio, la tronca (capita con il linguaggio chiericale) */
		if ( buf[strlen(buf)-1] == ' ' )
			buf[strlen(buf)-1] = '\0';
	}

	if ( ch->class == CLASS_CLERIC && ch->class == CLASS_PALADIN )
	{
		sprintf( buf2, "$n supplica una preghiera '%s'.", buf );
		sprintf( buf, "$n supplica la preghiera '%s'.", skill_table[sn].name );
	}
	else
	{
		sprintf( buf2, "$n pronuncia delle parole '%s'.", buf );
		sprintf( buf, "$n pronuncia le parole '%s'.", skill_table[sn].name );
	}
}


/*
 * Calcola il tiro salvezza. Modificatori negativi producono un tiro salvezza migliore.
 */
bool saves_spell( int level, CHAR_DATA *victim, int dam_type )
{
	int save;
	/*~~~~~*/

	save = 40 + ( victim->level - level ) * 4 - ( victim->saving_throw * 90 ) / UMAX( 45, victim->level );

	if ( IS_AFFECTED(victim, AFF_BERSERK) )
		save += victim->level / 5;

	switch ( check_immune(victim, dam_type) )
	{
	case IS_IMMUNE:									return( TRUE );
	case IS_RESISTANT:	save += victim->level / 5;	break;
	case IS_VULNERABLE: save -= victim->level / 5;	break;
	}

	if ( !IS_MOB(victim) && class_table[victim->class].fMana )
		save = 9 * save / 10;
	save = URANGE( 5, save, 95 );
	return( number_percent() < save );
}


/*
 * RT configuration smashed
 */
bool saves_dispel( int dis_level, int spell_level, int duration )
{
	int save;
	/*~~~~~*/

	/* impossible to dispel permanent effects */
	if ( duration == -2 ) return( 1 );
	if ( duration == -1 ) spell_level += 5;

	save = 50 + ( spell_level - dis_level ) * 5;
	save = URANGE( 5, save, 95 );
	return( number_percent() < save );
}


/*
 * routine d'appoggio per dispel magic e cancellation
 */
bool check_dispel( int dis_level, CHAR_DATA *victim, int sn )
{
	AFFECT_DATA *af;
	/*~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		for ( af = victim->affected; af != NULL; af = af->next )
		{
			if ( af->type == sn )
			{
				if ( !saves_dispel(dis_level, af->level, af->duration) )
				{
					affect_strip( victim, sn );
					if ( skill_table[sn].msg_off )
					{
						send_to_char( skill_table[sn].msg_off, victim );
						send_to_char( "\n\r", victim );
					}

					return( TRUE );
				}
				else
					af->level--;
			}
		}
	}

	return( FALSE );
}


/*
 * computo del costo in mana, versione temporanea
 */
int mana_cost( CHAR_DATA *ch, int min_mana, int level )
{
	if ( ch->level + 2 == level )
		return( 1000 );

	return( UMAX(min_mana, (100 / (2 + ch->level - level))) );
}


/*
 * per incantesimi che interessano diverse stanze Il valore di ritorno comprende il raggio d'azione
 */
int allowed_other( CHAR_DATA *ch, int sn )
{
	if ( skill_table[sn].minimum_position == POS_STAND
	||	 skill_table[sn].skill_level[ch->class] < 26
	||	 sn == find_spell(ch, "chain lightning") )
		return( 0 );
	else
		return( skill_table[sn].skill_level[ch->class] / 10 );
}


/*
 * Codice generico per incantesimi che necessitano di piu' parametri dalla linea di comando.
 */
char	*target_name;


void do_cast( CHAR_DATA *ch, char *argument )
{
	/* Switched NPC's can cast spells, but others can't. */
	if ( IS_MOB(ch) && ch->desc == NULL )
		return;

	if ( (ch->class == CLASS_CLERIC || ch->class == CLASS_PALADIN) )
	{
		send_to_char( "Non puoi invocare l'aiuto degli Dei in questa maniera, prova a supplicarli.\n\r", ch );
		return;
	}
	else
		spell_cast( ch, argument );
}


void do_supplication( CHAR_DATA *ch, char *argument )
{
	/* Switched NPC's can cast spells, but others can't. */
	if ( IS_MOB(ch) && ch->desc == NULL )
		return;

	if ( (ch->class == CLASS_CLERIC || ch->class == CLASS_PALADIN) )
		spell_cast( ch, argument );
	else
	{
		send_to_char( "Non ti e' permesso invocare il potere degli Dei.\n\r", ch );
		return;
	}
}


void spell_cast( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	void		*vo;
	int			mana;
	int			sn;
	int			target;
	int			cast_far = 0, door, range;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Switched NPC's can cast spells, but others can't. */
	if ( IS_MOB(ch) && ch->desc == NULL )
		return;

	if ( is_affected(ch, gsn_shielding) )
	{
		send_to_char( "Mentre cerchi di richiamare il mana senti una Forza misteriosa che ti ostacola, impedendoti di proseguire.\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_garble) || is_affected(ch, gsn_deafen) )
	{
		send_to_char( "Le sillabe ti si bloccano in gola.. non riesci a recitare la formula.\n\r", ch );
		return;
	}

	target_name = one_argument( argument, arg1 );
	one_argument( target_name, arg2 );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "La formula va completata con il nome del bersaglio per essere efficace.\n\r", ch );
		return;
	}

	if ( ch->cabal == CABAL_ARMY && !IS_ADMIN(ch) )
	{
		send_to_char( "Prega che i tuoi compagni di Cabala non abbiano assistito a questo tuo patetico tentativo di lanciare un incantesimo!\n\r", ch );
		return;
	}

	if ( (sn = find_spell(ch, arg1)) < 0 || skill_failure_nomessage(ch, sn, 1) )
	{
		send_to_char( "No, decisamente non e' il nome di nessun incantesimo che attualmente conosci.. dove l'hai sentito?\n\r", ch );
		return;
	}

	if ( ch->class == CLASS_VAMPIRE && !IS_VAMPIRE(ch) && skill_table[sn].cabal == CABAL_NONE )
	{
		send_to_char( "Avrai bisogno di mutare la tua forma in quella vampirica per poter usare i tuoi incantesimi..\n\r", ch );
		return;
	}

	if ( skill_table[sn].spell_fun == spell_null )
	{
		send_to_char( "Umh.. nessun incantesimo ha un nome simile.. dove l'hai letto?\n\r", ch );
		return;
	}

	if ( ch->position < skill_table[sn].minimum_position )
	{
		send_to_char( "Non riesci a focalizzare la tua concentrazione sull'incantesimo.. e' perduto.\n\r", ch );
		return;
	}

	if ( !cabal_ok(ch, sn) )
		return;

	if ( IS_SET(ch->in_room->room_flags, ROOM_NO_MAGIC) )
	{
		send_to_char( "La formula da te recitata diventa una inutile cantilena.. l'energia richiamata si disperde rapidamente..\n\r", ch );
		act( "$n cerca di richiamare un incantesimo ma l'energia richiamata si disperde in fretta non producendo alcun effetto.", ch, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( ch->level + 2 == skill_table[sn].skill_level[ch->class] )
		mana = 50;
	else
		mana = UMAX( skill_table[sn].min_mana, 100 / (2 + ch->level - skill_table[sn].skill_level[ch->class]) );

	/*
	 * Localizzazione del bersaglio.
	 */
	victim = NULL;
	obj = NULL;
	vo = NULL;
	target = TARGET_NONE;

	switch ( skill_table[sn].target )
	{
	default:
		bug( "Do_cast: bad target for sn %d.", sn );
		return;

	case TAR_IGNORE:
		if ( is_affected(ch, gsn_spellbane) )
		{
			WAIT_STATE( ch, skill_table[sn].beats );
			act( "L'incantesimo si ferma sull'aura d'ira che circonda il tuo corpo!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "Una violenta aura circonda $n disperdendo l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_spellbane, TRUE, 1 );
			damage( ch, ch, 3 * ch->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			return;
		}
		break;

	case TAR_CHAR_OFFENSIVE:
		if ( arg2[0] == '\0' )
		{
			if ( (victim = ch->fighting) == NULL )
			{
				send_to_char( "Specifica il bersaglio dell'incantesimo.. altrimenti la formula non ha effetto.\n\r", ch );
				return;
			}
		}
		else
		{
			if ( (range = allowed_other(ch, sn)) > 0 )
			{
				if ( (victim = get_char_spell(ch, target_name, &door, range)) == NULL )
					return;

				if ( victim->in_room != ch->in_room
				&&	((IS_MOB(victim) && IS_SET(victim->act, ACT_NOTRACK))
					||	is_at_cabal_area(ch)
					||	is_at_cabal_area(victim)) )
				{
					act( "$N e' troppo distante.. avvicinati un po' perche' l'incantesimo sia efficace.", ch, NULL, victim, TO_CHAR, POS_REST );
					return;
				}

				cast_far = 1;
			}
			else if ( (victim = get_char_room(ch, target_name)) == NULL )
			{
				send_to_char( "Il bersaglio dell'incantesimo non e' qui..\n\r", ch );
				return;
			}
		}

		if ( !IS_MOB(ch) && is_safe(ch, victim) )
			return;

/*		if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
		{
			send_to_char( "Non e' consigliabile farlo su di un seguace..\n\r", ch );
			return;
		}
*/
		vo = (void *) victim;
		target = TARGET_CHAR;
		if ( victim != ch
		&&	ch->fighting != victim
		&&	victim->fighting != ch
		&&	(IS_SET(victim->affected_by, AFF_CHARM) || IS_MOB(victim)) )
		{
			do_yell( victim, "Aiuto! Mi stanno attaccando!" );
		}

		if ( is_affected(victim, gsn_spellbane)
		&&	(number_percent() < 2 * get_skill(victim, gsn_spellbane) / 3)
		&&	sn != slot_lookup(524)
		&&	sn != slot_lookup(204) )
		{
			WAIT_STATE( ch, skill_table[sn].beats );
			if ( ch == victim )
			{
				act( "L'incantesimo si ferma sull'aura d'ira che circonda il tuo corpo!", ch, NULL, NULL, TO_CHAR, POS_REST );
				act( "Una violenta aura circonda $n disperdendo l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( ch, ch, 3 * ch->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			}
			else
			{
				act( "L'aura di $N annulla il tuo incantesimo!", ch, NULL, victim, TO_CHAR, POS_REST );
				act( "La tua aura annulla l'incantesimo di $n!", ch, NULL, victim, TO_VICT, POS_REST );
				act( "L'aura di $N annulla l'incantesimo di $n!", ch, NULL, victim, TO_NOTVICT, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( victim, ch, 3 * victim->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
				multi_hit( victim, ch, TYPE_UNDEFINED );
			}

			return;
		}

		if ( ch != victim
		&&	CAN_DETECT(victim, ADET_ABSORB)
		&&	(number_percent() < 2 * get_skill(victim, gsn_absorb) / 3)
		&&	sn != slot_lookup(524)
		&&	sn != slot_lookup(204) )
		{
			act( "Il tuo incantesimo non riesce a varcare le difese di $N fermandosi sul suo scudo di energia!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "Il campo di energia che ti avvolge assorbe l'incantesimo di $n!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$N absorbs $n's spell!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			check_improve( victim, gsn_absorb, TRUE, 1 );
			victim->point[MANA] += mana;
			return;
		}
		break;

	case TAR_CHAR_DEFENSIVE:
		if ( arg2[0] == '\0' )
			victim = ch;
		else
		{
			if ( (victim = get_char_room(ch, target_name)) == NULL )
			{
				send_to_char( "Il bersaglio del tuo incantesimo non e' qui..\n\r", ch );
				return;
			}
		}

		vo = (void *) victim;
		target = TARGET_CHAR;
		if ( is_affected(victim, gsn_spellbane) )
		{
			WAIT_STATE( ch, skill_table[sn].beats );
			if ( ch == victim )
			{
				act( "La tua aura annulla l'incantesimo!", ch, NULL, NULL, TO_CHAR, POS_REST );
				act( "L'aura di $N annulla l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( victim, ch, 3 * victim->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			}
			else
			{
				act( "L'aura di $N annulla il tuo incantesimo!!", ch, NULL, victim, TO_CHAR, POS_REST );
				act( "La tua aura annulla l'incantesimo di $n!!", ch, NULL, victim, TO_VICT, POS_REST );
				act( "L'aura di $N annulla l'incantesimo di $n!", ch, NULL, victim, TO_NOTVICT, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( victim, ch, 3 * victim->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			}

			return;
		}

		if ( ch != victim
		&&	CAN_DETECT(victim, ADET_ABSORB)
		&&	(number_percent() < 2 * get_skill(victim, gsn_absorb) / 3) )
		{
			act( "Il tuo incantesimo non riesce a varcare le difese di $N fermandosi sul suo scudo di energia!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "Il campo di energia che ti avvolge assorbe l'incantesimo di $n!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "Il campo di energia che avvolge $N assorbe l'incantesimo di $n annullandolo!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			check_improve( victim, gsn_absorb, TRUE, 1 );
			victim->point[MANA] += mana;
			return;
		}
		break;

	case TAR_CHAR_SELF:
		if ( arg2[0] != '\0' && !is_name(target_name, ch->name) )
		{
			send_to_char( "L'energia di questo incantesimo non puo' transitare su altri.. solo su te..\n\r", ch );
			return;
		}

		vo = (void *) ch;
		target = TARGET_CHAR;

		if ( is_affected(ch, gsn_spellbane) )
		{
			WAIT_STATE( ch, skill_table[sn].beats );
			act( "La tua aura annulla l'incantesimo!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "L'aura di $N annulla l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_spellbane, TRUE, 1 );
			damage( ch, ch, 3 * ch->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			return;
		}
		break;

	case TAR_OBJ_INV:
		if ( arg2[0] == '\0' )
		{
			send_to_char( "Al termine della formula devi specificare il nome dell'oggetto bersaglio dell'incantesimo..\n\r", ch );
			return;
		}

		if ( (obj = get_obj_carry(ch, target_name)) == NULL )
		{
			send_to_char( "Cerca meglio,quell'oggetto non e' nelle tue mani.\n\r", ch );
			return;
		}

		vo = (void *) obj;
		target = TARGET_OBJ;
		if ( is_affected(ch, gsn_spellbane) )
		{
			WAIT_STATE( ch, skill_table[sn].beats );
			act( "La tua aura annulla l'incantesimo!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "L'aura di $N annulla l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_spellbane, TRUE, 1 );
			damage( ch, ch, 3 * ch->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			return;
		}
		break;

	case TAR_OBJ_CHAR_OFF:
		if ( arg2[0] == '\0' )
		{
			if ( (victim = ch->fighting) == NULL )
			{
				send_to_char( "La formula che hai pronunciato richiede la specificazione del bersaglio..\n\r", ch );
				return;
			}

			target = TARGET_CHAR;
		}
		else if ( (victim = get_char_room(ch, target_name)) != NULL )
			target = TARGET_CHAR;

		if ( target == TARGET_CHAR )	/* check the sanity of the attack */
		{
			if ( is_safe_spell(ch, victim, FALSE) && victim != ch )
			{
				send_to_char( "Nonostante la formula sia stata recitata correttamente l'incantesimo non sembra aver effetto..\n\r", ch );
				return;
			}

			if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
			{
				send_to_char( "Non e' consigliabile fare un incantesimo simile ad un seguace.\n\r", ch );
				return;
			}

			if ( is_safe(ch, victim) )
				return;

			vo = (void *) victim;
		}
		else if ( (obj = get_obj_here(ch, target_name)) != NULL )
		{
			vo = (void *) obj;
			target = TARGET_OBJ;
		}
		else
		{
			send_to_char( "Il bersaglio della formula dell'incantesimo non e' qui..\n\r", ch );
			return;
		}
		break;

	case TAR_OBJ_CHAR_DEF:
		if ( arg2[0] == '\0' )
		{
			vo = (void *) ch;
			target = TARGET_CHAR;
		}
		else if ( (victim = get_char_room(ch, target_name)) != NULL )
		{
			vo = (void *) victim;
			target = TARGET_CHAR;
		}
		else if ( (obj = get_obj_carry(ch, target_name)) != NULL )
		{
			vo = (void *) obj;
			target = TARGET_OBJ;
		}
		else
		{
			send_to_char( "Il bersaglio dell'incantesimo non e' qui.\n\r", ch );
			return;
		}
		break;
	}

	if ( !IS_MOB(ch) && ch->point[MANA] < mana )
	{
		send_to_char( "Hai assolutamente bisogno di riequilibrare il Mana nel tuo corpo prima di lanciare un incantesimo simile!\n\r", ch );
		return;
	}

	if ( str_cmp(skill_table[sn].name, "ventriloquate") )
		say_spell( ch, sn );

	WAIT_STATE( ch, skill_table[sn].beats );

	if ( number_percent() > get_skill(ch, sn) )
	{
		send_to_char( "Non riesci a completare la formula in maniera corretta.. l'incantesimo e' perduto.\n\r", ch );
		check_improve( ch, sn, FALSE, 1 );
		ch->point[MANA] -= mana / 2;
		if ( cast_far )
			cast_far = 2;
	}
	else
	{
		int slevel;
		/*~~~~~~~*/

		if ( class_table[ch->class].fMana )
			slevel = ch->level - UMAX( 0, (ch->level / 20) );
		else
			slevel = ch->level - UMAX( 5, (ch->level / 10) );

		if ( skill_table[sn].cabal != CABAL_NONE )
			slevel = ch->level;

		if ( ch->level > skill_table[gsn_spell_craft].skill_level[ch->class] )
		{
			if ( number_percent() < get_skill(ch, gsn_spell_craft) )
			{
				slevel = ch->level;
				check_improve( ch, gsn_spell_craft, TRUE, 1 );
			}

			check_improve( ch, gsn_spell_craft, FALSE, 1 );
		}

		if ( ch->cabal == CABAL_YVALADRIN
		&&	 ch->level > skill_table[gsn_mastering_spell].skill_level[ch->class]
		&&	 cabal_ok(ch, gsn_mastering_spell) )
		{
			if ( number_percent() < get_skill(ch, gsn_mastering_spell) )
			{
				slevel += number_range( 1, 4 );
				check_improve( ch, gsn_mastering_spell, TRUE, 1 );
			}
		}

		ch->point[MANA] -= mana;
		if ( get_curr_stat(ch, INT) > 21 )
			slevel = UMAX( 1, (slevel + (get_curr_stat(ch, INT) - 21)) );
		else
			slevel = UMAX( 1, slevel );

		if ( IS_MOB(ch) )
			( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, vo, target );
		else
			( *skill_table[sn].spell_fun ) ( sn, slevel, ch, vo, target );
		check_improve( ch, sn, TRUE, 1 );
	}

	if ( cast_far == 1 && door != -1 )
		path_to_track( ch, victim, door );
	else if (  (skill_table[sn].target == TAR_CHAR_OFFENSIVE
			|| (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
		&&	victim != ch
		&&	victim->master != ch )
	{
		CHAR_DATA	*vch;
		CHAR_DATA	*vch_next;
		/*~~~~~~~~~~~~~~~~~~*/

		for ( vch = ch->in_room->people; vch; vch = vch_next )
		{
			vch_next = vch->next_in_room;
			if ( victim == vch && victim->fighting == NULL )
			{
				if ( victim->position != POS_SLEEP )
					multi_hit( victim, ch, TYPE_UNDEFINED );
				break;
			}
		}
	}
}


/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
	void	*vo = NULL;
	int		target = TARGET_NONE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( sn <= 0 )
		return;

	if ( sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
	{
		bug( "Obj_cast_spell: bad sn %d.", sn );
		return;
	}

	if ( (IS_MOB(ch) && ch->position == POS_DEAD) || (!IS_MOB(ch) && (current_time - ch->last_death_time) < 10) )
	{
		bug( "Obj_cast_spell: Ch is dead! But it is ok.", sn );
		return;
	}

	if ( victim != NULL )
	{
		if ( (IS_MOB(victim) && victim->position == POS_DEAD)
		||	(!IS_MOB(victim) && (current_time - victim->last_death_time) < 10) )
		{
			bug( "Obj_cast_spell: Victim is dead! But it is ok.. ", sn );
			return;
		}
	}

	switch ( skill_table[sn].target )
	{
	default:
		bug( "Obj_cast_spell: bad target for sn %d.", sn );
		return;

	case TAR_IGNORE:
		vo = NULL;
		break;

	case TAR_CHAR_OFFENSIVE:
		/* Xandra, sanno di baco questi due  controlli uguali qui */
		if ( victim == NULL )
			victim = ch->fighting;

		if ( victim == NULL )
		{
			send_to_char( "Il bersaglio che completa la tua formula non e' corretto.\n\r", ch );
			return;
		}

		if ( is_safe(ch, victim) && ch != victim )
		{
			send_to_char( "Qualcosa nell'incantesimo non va.. eppure la formula e' giusta!\n\r", ch );
			return;
		}

		vo = (void *) victim;
		target = TARGET_CHAR;
		if (is_affected(victim, gsn_spellbane)
		&&	( /* IS_MOB(victim) || */ number_percent() < 2 * get_skill(victim, gsn_spellbane) / 3) )
		{
			if ( ch == victim )
			{
				act( "La tua aura annulla l'incantesimo!", ch, NULL, NULL, TO_CHAR, POS_REST );
				act( "L'aura di $N annulla l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( victim, ch, 10 * level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			}
			else
			{
				act( "L'aura di $N annulla l'incantesimo!", ch, NULL, victim, TO_CHAR, POS_REST );
				act( "La tua aura annulla l'incantesimo!", ch, NULL, victim, TO_VICT, POS_REST );
				act( "L'aura di $N annulla l'incantesimo di $n!", ch, NULL, victim, TO_NOTVICT, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( victim, ch, 10 * victim->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			}

			return;
		}

		if (ch != victim
		&&	CAN_DETECT(victim, ADET_ABSORB)
		&&	(number_percent() < 2 * get_skill(victim, gsn_absorb) / 3)
		&&	sn != slot_lookup(524)
		&&	sn != slot_lookup(204) )
		{
			act( "Il tuo incantesimo non riesce a varcare le difese di $N fermandosi sul suo scudo di energia!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "Il campo di energia che ti avvolge assorbe l'incantesimo di $n!", ch, NULL, victim, TO_VICT, POS_REST );
			act( "Il campo di energia che avvolge $N assorbe l'incantesimo di $n annullandolo!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			check_improve( victim, gsn_absorb, TRUE, 1 );
			victim->point[MANA] += skill_table[sn].min_mana;
			return;
		}
		break;

	case TAR_CHAR_DEFENSIVE:
	case TAR_CHAR_SELF:
		if ( victim == NULL )
			victim = ch;
		vo = (void *) victim;
		target = TARGET_CHAR;
		if ( is_affected(victim, gsn_spellbane) )
		{
			if ( ch == victim )
			{
				act( "La tua aura annulla l'incantesimo!", ch, NULL, NULL, TO_CHAR, POS_REST );
				act( "L'aura di $N annulla l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( victim, ch, 10 * victim->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			}
			else
			{
				act( "L'aura di $N annulla il tuo incantesimo!", ch, NULL, victim, TO_CHAR, POS_REST );
				act( "La tua aura annulla l'incantesimo di $n!", ch, NULL, victim, TO_VICT, POS_REST );
				act( "L'aura di $N annulla l'incantesimo di $n!", ch, NULL, victim, TO_NOTVICT, POS_REST );
				check_improve( victim, gsn_spellbane, TRUE, 1 );
				damage( victim, ch, 10 * victim->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			}

			return;
		}
		break;

	case TAR_OBJ_INV:
		if ( obj == NULL )
		{
			send_to_char( "Non riesci a vedere il bersaglio del tuo incantesimo.\n\r", ch );
			return;
		}

		vo = (void *) obj;
		target = TARGET_OBJ;
		if ( is_affected(ch, gsn_spellbane) )
		{
			act( "La tua aura annulla l'incantesimo!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "L'aura di $N annulla l'incantesimo!", ch, NULL, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_spellbane, TRUE, 1 );
			damage( ch, ch, 3 * ch->level, gsn_spellbane, DAM_NEGATIVE, TRUE );
			return;
		}
		break;

	case TAR_OBJ_CHAR_OFF:
		if ( victim == NULL && obj == NULL )
		{
			if ( ch->fighting != NULL )
				victim = ch->fighting;
			else
			{
				send_to_char( "Il bersaglio del tuo incantesimo non e' qui..\n\r", ch );
				return;
			}
		}

		if ( victim != NULL )
		{
			if ( is_safe_spell(ch, victim, FALSE) && ch != victim )
			{
				send_to_char( "Qualcosa sembra non andare come dovrebbe.. la formula e' esatta ma l'incantesimo non produce alcun effetto.\n\r", ch );
				return;
			}

			vo = (void *) victim;
			target = TARGET_CHAR;
		}
		else
		{
			vo = (void *) obj;
			target = TARGET_OBJ;
		}
		break;

	case TAR_OBJ_CHAR_DEF:
		if ( victim == NULL && obj == NULL )
		{
			vo = (void *) ch;
			target = TARGET_CHAR;
		}
		else if ( victim != NULL )
		{
			vo = (void *) victim;
			target = TARGET_CHAR;
		}
		else
		{
			vo = (void *) obj;
			target = TARGET_OBJ;
		}
		break;
	}

	target_name = "";
	( *skill_table[sn].spell_fun ) ( sn, level, ch, vo, target );

	if ( ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
		|| (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
	&&	victim != ch
	&&	victim->master != ch )
	{
		CHAR_DATA	*vch;
		CHAR_DATA	*vch_next;
		/*~~~~~~~~~~~~~~~~~~*/

		for ( vch = ch->in_room->people; vch; vch = vch_next )
		{
			vch_next = vch->next_in_room;
			if ( victim == vch && victim->fighting == NULL )
			{
				multi_hit( victim, ch, TYPE_UNDEFINED );

				break;
			}
		}
	}
}


/*
 * Spell functions.
 */
void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 18 );
	if ( saves_spell(level, victim, DAM_ACID) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_ACID, TRUE );
}


void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' attorno a te 'lenergia dell'armatura magica.\n\r", ch );
		else
			act( "$N is already armored.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 7 + level / 6;
	af.modifier = -1 * UMAX( 20, 10 + level / 4 );	/* af.modifier = -20; */
	af.location = APPLY_AC;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	send_to_char( "Una soffusa luce si materializza attorno al tuo corpo prima di formare un'armatura magica per proteggerti.\n\r", victim );
	if ( ch != victim )
		act( "Con pochi gesti richiami un'armatura magica attorno al corpo di $N.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~*/

	/* deal with the object case first */
	if ( target == TARGET_OBJ )
	{
		obj = (OBJ_DATA *) vo;
		if ( IS_OBJ_STAT(obj, ITEM_BLESS) )
		{
			act( "$p ha gia' una benedizione.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( IS_OBJ_STAT(obj, ITEM_EVIL) )
		{
			AFFECT_DATA *paf;
			/*~~~~~~~~~~~~~*/

			paf = affect_find( obj->affected, gsn_curse );
			if ( !saves_dispel(level, paf != NULL ? paf->level : obj->level, 0) )
			{
				if ( paf != NULL ) affect_remove_obj( obj, paf );
				act( "$p risplende di una fievole luce bluastra.", ch, obj, NULL, TO_ALL, POS_REST );
				REMOVE_BIT( obj->extra_flags, ITEM_EVIL );
				return;
			}
			else
			{
				act( "$p comincia a tremare nervosamente e scaccia la tua benedizione emettendo una maligna luce rossastra.", ch, obj, NULL, TO_CHAR, POS_REST );
				return;
			}
		}

		af.where = TO_OBJECT;
		af.type = sn;
		af.level = level;
		af.duration = ( 6 + level / 2 );
		af.location = APPLY_SAVES;
		af.modifier = -1;
		af.bitvector = ITEM_BLESS;
		affect_to_obj( obj, &af );

		act( "$p risplende di una chiara e bianca luce.", ch, obj, NULL, TO_ALL, POS_REST );
		return;
	}

	/* character target */
	victim = (CHAR_DATA *) vo;

	if ( victim->position == POS_FIGHT || is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Senti la benedizione degli Dei giungere al tuo animo.\n\r", ch );
		else
			act( "$N ha gia' ricevuto la benedizione degli Dei.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 6 + level / 2 );
	af.location = APPLY_HITROLL;
	af.modifier = level / 8;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = 0 - level / 8;
	affect_to_char( victim, &af );
	send_to_char( "Senti che gli Dei ora accompagnano i passi della tua anima, hai il loro favore.\n\r", victim );
	if ( ch != victim )
		act( "Preghi il tuo Dio di intercedere in favore di $N.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_BLIND) || saves_spell(level, victim, DAM_OTHER) )
	{
		send_to_char( "Gli occhi del nemico si oscurano per un attimo!\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_HITROLL;
	af.modifier = -4;
	af.duration = 3 + level / 15;
	af.bitvector = AFF_BLIND;
	affect_to_char( victim, &af );
	send_to_char( "La vista ti si offusca improvvisamente!Non riesci piu' a distinguere nulla!\n\r", victim );
	act( "$n sbarra gli occhi.. sembra che non sia piu' in grado di vedere distintamente!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_burning_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 2 ) + 7;
	if ( saves_spell(level, victim, DAM_FIRE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
}


void spell_call_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( !IS_OUTSIDE(ch) )
	{
		send_to_char( "Puoi richiamare le energie celesti solo all'esterno.\n\r", ch );
		return;
	}

	if ( weather_info.sky < SKY_RAINING )
	{
		send_to_char( "Troppe poche nuvole per richiamare un fulmine.. ci vorrebbe un po' di pioggia.\n\r", ch );
		return;
	}

	dam = dice( level, 14 );

	send_to_char( "L'ira dei cieli scende giu' seguendo il tuo braccio per colpire il nemico!\n\r", ch );
	act( "$n richiama le energie celesti scatenando l'ira dei fulmini sui suoi nemici!", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next;
		if ( vch->in_room == NULL )
			continue;
		if ( vch == ch )
			continue;
		if ( vch->in_room == ch->in_room )
		{
			if ( vch != ch && !is_same_group(ch, vch) )
			{
				if ( is_safe(ch, vch) )
					continue;
			}

			if ( CAN_DETECT(vch, ADET_GROUNDING) )
			{
				send_to_char( "Inaspettatamente l'energia da te richiamata si disperde in innocue scintille multicolore ai tuoi piedi..\n\r", vch );
				act( "Improvvisamente ai piedi di $N scoppiettano migliaia di scintille multicolore..",
					ch, NULL, vch, TO_ROOM, POS_REST );
				continue;
			}

			if ( saves_spell(level, vch, DAM_LIGHTNING) )
				dam /= 2;
			damage( ch, vch, dam, sn, DAM_LIGHTNING, TRUE );
			continue;
		}

		if ( vch->in_room->area == ch->in_room->area && IS_OUTSIDE(vch) && IS_AWAKE(vch) )
			send_to_char( "Il cielo sembra rabbioso mentre nuvole e fulmini lo percorrono senza tregua.\n\r", vch );
	}
}


/*
 * RT calm spell stops all fighting in the room
 */
void spell_calm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	int			mlevel = 0;
	int			count = 0;
	int			high_level = 0;
	int			chance;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get sum of all mobile levels in the room */
	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( vch->position == POS_FIGHT )
		{
			count++;
			if ( IS_MOB(vch) )
				mlevel += vch->level;
			else
				mlevel += vch->level / 2;
			high_level = UMAX( high_level, vch->level );
		}
	}

	/* compute chance of stopping combat */
	chance = 4 * level - high_level + 2 * count;

	if ( IS_ADMIN(ch) )						/* always works */
		mlevel = 0;

	if ( number_range(0, chance) >= mlevel )	/* hard to stop large fights */
	{
		for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
		{
			if ( IS_MOB(vch) && (IS_SET(vch->imm_flags, IMM_MAGIC) || IS_SET(vch->act, ACT_UNDEAD)) )
				return;

			if ( IS_AFFECTED(vch, AFF_CALM)
			||	 IS_AFFECTED(vch, AFF_BERSERK)
			||	 is_affected(vch, skill_lookup("frenzy")) )
				return;

			send_to_char( "Avverti un alone di pace attraversarti l'anima.\n\r", vch );

			if ( vch->fighting || vch->position == POS_FIGHT ) stop_fighting( vch, FALSE );

			af.where = TO_AFFECTS;
			af.type = sn;
			af.level = level;
			af.duration = level / 4;
			af.location = APPLY_HITROLL;
			if ( !IS_MOB(vch) )
				af.modifier = -5;
			else
				af.modifier = -2;
			af.bitvector = AFF_CALM;
			affect_to_char( vch, &af );

			af.location = APPLY_DAMROLL;
			affect_to_char( vch, &af );
		}
	}
}


void spell_cancellation( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	bool		found = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	level += 2;

	if ( (!IS_MOB(ch) && IS_MOB(victim) && !(IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim))
	||	 (IS_MOB(ch) && !IS_MOB(victim)) )
	{
		send_to_char( "L'incantesimo si disperde rapidamente.. Evidentemente non e' quello giusto per raggiungere il tuo obiettivo.\n\r", ch );
		return;
	}

	if ( !is_same_group(ch, victim) && ch != victim && (IS_MOB(victim) || IS_SET(victim->act, PLR_NOCANCEL)) )
	{
		act( "$N non e' un bersaglio accettabile per questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	/* unlike dispel magic, the victim gets NO save */
	/* begin running through the spells */
	if ( check_dispel(level, victim, skill_lookup("armor")) )			found = TRUE;
	if ( check_dispel(level, victim, skill_lookup("enhanced armor")) )	found = TRUE;
	if ( check_dispel(level, victim, skill_lookup("bless")) )			found = TRUE;

	if ( check_dispel(level, victim, skill_lookup("blindness")) )
	{
		found = TRUE;
		act( "Gli occhi di $n perdono l'alone oscuro che li ottenebrava.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("calm")) )
	{
		found = TRUE;
		act( "$n sembra aver perso un po' di serenita'..", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("change sex")) )
	{
		found = TRUE;
		act( "$n torna ad assumere le sue solite sembianze..", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("charm person")) )
	{
		found = TRUE;
		act( "$n libera il suo arbitrio dalle magiche catene che lo opprimevano.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("chill touch")) )
	{
		found = TRUE;
		act( "$n smette di tremare mentre la sua epidermide torna alla normale colorazione.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("curse")) )
	{
		found = TRUE;
		act( "La maledizione abbandona $n.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect evil")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere il male.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect good")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere il bene.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect hidden")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere le cose nascoste.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect invis")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere l'invisibile.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect magic")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere il magico.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("faerie fire")) )
	{
		act( "Le strana sagoma fluorescente attorno a $n svanisce nel nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("fly")) )
	{
		act( "$n smette di fluttuare tornando rapidamente a terra!", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("frenzy")) )
	{
		act( "Lo sguardo quasi collerico di $n si placa improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("giant strength")) )
	{
		act( "La muscolatura sul corpo di $n torna normale.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("haste")) )
	{
		act( "$n torna a muoversi normalmente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("infravision")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di vedere nelle tenebre.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("invis")) )
	{
		act( "$n riappare lentamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("mass invis")) )
	{
		act( "$n riappare lentamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("pass door")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di passare attraverso le porte.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("protection evil")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal male.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("protection good")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal bene.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("sanctuary")) )
	{
		act( "La forte aura bianca che proteggeva $n comincia a diradarsi per poi sparire nel nulla.",
			victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("shield")) )
	{
		act( "Lo scudo protettivo sul corpo di $n si dissolve magicamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("sleep")) )
	{
		found = TRUE;
		act( "$n si sveglia improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("slow")) )
	{
		act( "$n torna a muoversi normalmente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("stone skin")) )
	{
		act( "La rocciosa epidermide di $n si sgretola svanendo nel nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("weaken")) )
	{
		act( "La debolezza che assaliva $n svanisce di colpo. ", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("shielding")) )
	{
		found = TRUE;
		act( "$n non ha la protezione magica.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("fear")) )
	{
		found = TRUE;
		act( "$n non ha timore della battaglia.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("protection heat")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal caldo.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("protection cold")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal freddo.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("magic resistance")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal magico.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("hallucination")) )
	{
		found = TRUE;
		act( "$n torna a distinguere chiaramente la realta'.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("terangreal")) )
		found = TRUE;

	if ( check_dispel(level, victim, skill_lookup("power word stun")) )
	{
		found = TRUE;
		act( "$n non e' piu' sotto pietrificazione.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("corruption")) )
	{
		act( "$n sembra riacquisire un po' di salute.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("web")) )
	{
		act( "Gli spessi filamenti di ragnatela che imprigionavano $n si dissolvono.",
			victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( found )
		send_to_char( "Incantesimo terminato.\n\r", ch );
	else
		send_to_char( "L'incantesimo e' stato interrotto..puoi riprovare.\n\r", ch );
}


void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	send_to_char( "Il tuo tocco provoca dolore al tuo nemico!\n\r", ch );
	damage( ch, (CHAR_DATA *) vo, dice(1, 8) + level / 3, sn, DAM_HARM, TRUE );
}


void spell_cause_critical( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	send_to_char( "Il tuo tocco provoca molto dolore al tuo nemico!\n\r", ch );
	damage( ch, (CHAR_DATA *) vo, dice(3, 8) + level - 6, sn, DAM_HARM, TRUE );
}


void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	send_to_char( "Il tuo tocco provoca parecchio dolore al tuo nemico!\n\r", ch );
	damage( ch, (CHAR_DATA *) vo, dice(2, 8) + level / 2, sn, DAM_HARM, TRUE );
}


void spell_chain_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	CHAR_DATA	*tmp_vict, *last_vict, *next_vict;
	bool		found;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* first strike */
	act( "Le mani di $n vengono avviluppate da sinuose spirali di elettricita' che si scagliano improvvisamente su $N!",
		ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "Sinuose spirali di elettricita' ti accarezzano le mani formando da queste un arco che va ad investire $N!",
		ch, NULL, victim, TO_CHAR, POS_REST );
	act( "Le mani di $n vengono avviluppate da sinuose spirali di elettricita' che di scagliano su di te!!!",
		ch, NULL, victim, TO_VICT, POS_REST );

	dam = dice( level, 6 );

	if ( CAN_DETECT(victim, ADET_GROUNDING) )
	{
		send_to_char( "Richiami l'elettricita' sulle tue mani, ma al momento di scagliarla sul tuo avversario essa si traduce in mille scintille ai suoi piedi.\n\r", victim );
		act( "Dalle mani di $N si materializzano deboli scariche elettriche che finiscono al suolo come scintille.",
			ch, NULL, victim, TO_ROOM, POS_REST );
	}
	else
	{
		if ( saves_spell(level, victim, DAM_LIGHTNING) )
			dam /= 3;
		damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
	}

	if ( victim != ch
	&&	 ch->fighting != victim
	&&	 victim->fighting != ch
	&&	(IS_SET(victim->affected_by, AFF_CHARM) || IS_MOB(victim)) )
		do_yell( victim, "Aiuto! Mi stanno attaccando!" );

	last_vict = victim;
	level -= 4;						/* decrement damage */

	/* new targets */
	while ( level > 0 )
	{
		found = FALSE;
		for ( tmp_vict = ch->in_room->people; tmp_vict != NULL; tmp_vict = next_vict )
		{
			next_vict = tmp_vict->next_in_room;
			if ( !is_safe_spell(ch, tmp_vict, TRUE) && tmp_vict != last_vict )
			{
				found = TRUE;
				last_vict = tmp_vict;
				if ( is_safe(ch, tmp_vict) )
				{
					act( "L'arco di energia aggira il corpo di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
					act( "L'arco di energia aggira il tuo corpo.", ch, NULL, NULL, TO_CHAR, POS_REST );
				}
				else
				{
					act( "Il fulmine colpisce $n in pieno!", tmp_vict, NULL, NULL, TO_ROOM, POS_REST );
					act( "La violenta scarica di elettricita' ti colpisce!", tmp_vict, NULL, NULL, TO_CHAR, POS_REST );
					dam = dice( level, 6 );

					if ( tmp_vict != ch
					&&	 ch->fighting != tmp_vict
					&&	 tmp_vict->fighting != ch
					&&	(IS_SET(tmp_vict->affected_by, AFF_CHARM) || IS_MOB(tmp_vict)) )
					{
						do_yell( tmp_vict, "Aiuto! Mi stanno attaccando!" );
					}

					if ( CAN_DETECT(tmp_vict, ADET_GROUNDING) )
					{
						send_to_char( "Richiami l'elettricita' sulle tue mani, ma al momento di scagliarla sul tuo avversario essa si traduce in mille scintille ai suoi piedi.\n\r", tmp_vict );
						act( "Dalle mani di $N si materializzano deboli scariche elettriche che finiscono al suolo come scintille.",
							ch, NULL, tmp_vict, TO_ROOM, POS_REST );
					}
					else
					{
						if ( saves_spell(level, tmp_vict, DAM_LIGHTNING) )
							dam /= 3;
						damage( ch, tmp_vict, dam, sn, DAM_LIGHTNING, TRUE );
					}

					level -= 4;		/* decrement damage */
				}
			}
		}							/* end target searching loop */

		if ( !found )				/* no target found, hit the caster */
		{
			if ( ch == NULL )
				return;

			if ( last_vict == ch )	/* no double hits */
			{
				act( "Il fulmine scaturisce con violenza dalle tue mani.", ch, NULL, NULL, TO_ROOM, POS_REST );
				act( "Il fulmine attraversa il tuo corpo prima di arrivare a terra!.", ch, NULL, NULL, TO_CHAR, POS_REST );
				return;
			}

			last_vict = ch;
			act( "Un arco di elettricita' si sprigiona dalle mani di $n colpendolo in pieno!", ch, NULL, NULL, TO_ROOM, POS_REST );
			send_to_char( "L'arco di elettricita' inverte la sua rotta e torna indietro colpendoti in pieno!\n\r", ch );
			dam = dice( level, 6 );

			if ( CAN_DETECT(ch, ADET_GROUNDING) )
			{
				send_to_char( "Richiami l'elettricita' sulle tue mani, ma al momento di scagliarla sul tuo avversario essa si traduce in mille scintille ai suoi piedi.\n\r", ch );
				act( "Dalle mani di $N si materializzano deboli scariche elettriche che finiscono al suolo come scintille.",
					ch, NULL, ch, TO_ROOM, POS_REST );
			}
			else
			{
				if ( saves_spell(level, ch, DAM_LIGHTNING) )
					dam /= 3;
				damage( ch, ch, dam, sn, DAM_LIGHTNING, TRUE );
			}

			level -= 4;				/* decrement damage */
			if ( ch == NULL )
				return;
		}

		/* now go back and find more targets */
	}
}


void spell_healing_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af, af2;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "Avverti che in questa stanza e' gia' presente l'aura benefica della sacra luce.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_CONST;
	af.type = sn;
	af.level = level;
	af.duration = level / 25;
	af.location = APPLY_ROOM_HEAL;
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
	send_to_char( "Richiami dalle stelle nascoste la forza della luce e lasci che per il tuo corpo essa giunga intorno a te.\n\r", ch );
	act( "Un'aura bianca benefica pervade l'ambiente..", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	char		buf[MIL];
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_safe(ch, victim) )
		return;
	if ( count_charmed(ch) )
		return;

	if ( victim == ch )
	{
		send_to_char( "E' l'unico modo che hai di perdonarti qualcosa?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_CHARM)
	||	IS_AFFECTED(ch, AFF_CHARM)
	||	(ch->sex != SEX_FEMALE && level < victim->level)
	||	(ch->sex == SEX_FEMALE && level < (victim->level - 2))
	||	IS_SET(victim->imm_flags, IMM_CHARM)
	||	saves_spell(level, victim, DAM_CHARM)
	||	(IS_MOB(victim) && victim->pIndexData->pShop != NULL) )
		return;

	if (victim->position == POS_SLEEP )
	{
		send_to_char( "Difficile poter affascinare una persona che dorme..\n\r", ch );
		return;
	}

	if ( victim->master )
		stop_follower( victim );
	add_follower( victim, ch );
	victim->leader = ch;
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = number_fuzzy( level / 5 );
	af.location = 0;
	af.modifier = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char( victim, &af );
	act( "In un soffio di vento la tua coscienza svanisce.. e riconosci in $n l'unica guida per la tua vita.", ch, NULL, victim, TO_VICT, POS_REST );
	if ( ch != victim )
		act( "$N ti guarda con rispetto, pendendo letteralmente dalle tue labbra.", ch, NULL, victim, TO_CHAR, POS_REST );

	if ( IS_MOB(victim) && !IS_MOB(ch) )
	{
		if ( number_percent() < (4 + (victim->level - ch->level)) * 10 )
			add_mind( victim, ch->name );
		else if ( victim->in_mind == NULL )
		{
			sprintf( buf, "%d", victim->in_room->vnum );
			victim->in_mind = str_dup( buf );
		}
	}
}


void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = number_range( 1, level );
	if ( !saves_spell(level, victim, DAM_COLD) )
	{
		act( "$n rabbrividisce mentre i suoi muscoli congelano!", victim, NULL, NULL, TO_ROOM, POS_REST );
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = 6;
		af.location = APPLY_STR;
		af.modifier = -1;
		af.bitvector = 0;
		affect_join( victim, &af );
	}
	else
		dam /= 2;

	damage( ch, victim, dam, sn, DAM_COLD, TRUE );
}


void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 3 ) + 13;
	if ( saves_spell(level, victim, DAM_LIGHT) )
		dam /= 2;
	else
		spell_blindness( skill_lookup("blindness"), level / 2, ch, (void *) victim, TARGET_CHAR );

	damage( ch, victim, dam, sn, DAM_LIGHT, TRUE );
}


void spell_continual_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*light;
	/*~~~~~~~~~~~~~~~*/

	if ( target_name[0] != '\0' )	/* do a glow on some object */
	{
		light = get_obj_carry( ch, target_name );

		if ( light == NULL )
		{
			send_to_char( "L'oggetto richiamato nella formula non e' in tuo possesso.\n\r", ch );
			return;
		}

		if ( IS_OBJ_STAT(light, ITEM_GLOW) )
		{
			act( "$p gia' brilla di luce propria.", ch, light, NULL, TO_CHAR, POS_REST );
			return;
		}

		SET_BIT( light->extra_flags, ITEM_GLOW );
		act( "$p comincia a irradiare una luce bianca.", ch, light, NULL, TO_ALL, POS_REST );
		return;
	}

	light = create_object( get_obj_index(OBJ_VNUM_LIGHT_BALL), 0 );
	obj_to_room( light, ch->in_room );
	act( "$n incrocia le mani pronunciando una lenta cantilena mentre una $p appare tra le sue dita.", ch, light, NULL, TO_ROOM, POS_REST );
	act( "Incroci le mani pronunciando una lenta cantilena mentre una $p appare tra le tue dita.", ch, light, NULL, TO_CHAR, POS_REST );
}


#ifndef WEATHER
void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( !str_cmp(target_name, "better") || !str_cmp(target_name, "migliore") )
		weather_info.change += dice( level / 3, 4 );
	else if ( !str_cmp(target_name, "worse") || !str_cmp(target_name, "peggiore") )
		weather_info.change -= dice( level / 3, 4 );
	else
	{
		send_to_char( "Vuoi che il tempo migliori o peggiori?(better/worse)\n\r", ch );
		return;
	}

	send_to_char( "Il cielo sembra aver recepito il tuo messaggio..\n\r", ch );
}
#endif


void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*mushroom;
	/*~~~~~~~~~~~~~~~~~~*/

	mushroom = create_object( get_obj_index(OBJ_VNUM_MUSHROOM), 0 );
	mushroom->value[0] = level / 2;
	mushroom->value[1] = level;
	obj_to_room( mushroom, ch->in_room );
	act( "$p si materializza improvvisamente!", ch, mushroom, NULL, TO_ROOM, POS_REST );
	act( "$p si materializza improvvisamente!", ch, mushroom, NULL, TO_CHAR, POS_REST );
}


void spell_create_rose( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*rose;
	/*~~~~~~~~~~~~~~*/

	rose = create_object( get_obj_index(OBJ_VNUM_ROSE), 0 );
	act( "$n fa apparire tra le sue dita una splendida rosa rossa.", ch, rose, NULL, TO_ROOM, POS_REST );
	send_to_char( "Disegnando lente spirali d'aria sulla tua mano sinistra fai apparire dal nulla una splendida rosa rossa.\n\r", ch );
	obj_to_char( rose, ch );
}


void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*spring;
	/*~~~~~~~~~~~~~~~~*/

	spring = create_object( get_obj_index(OBJ_VNUM_SPRING), 0 );
	spring->timer = level;
	obj_to_room( spring, ch->in_room );
	act( "$p nasce dal terreno.", ch, spring, NULL, TO_ROOM, POS_REST );
	act( "$p nasce dal terreno.", ch, spring, NULL, TO_CHAR, POS_REST );
}


void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	int			water;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		send_to_char( "Non mi sembra un contenitore adatto all'acqua, meglio cercare qualcosa di diverso..\n\r", ch );
		return;
	}

	if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
	{
		send_to_char( "C'e' dell'altro liquido.. meglio non mischiarlo con l'acqua.\n\r", ch );
		return;
	}

	water = UMIN( level * (weather_info.sky >= SKY_RAINING ? 4 : 2), obj->value[0] - obj->value[1] );

	if ( water > 0 )
	{
		obj->value[2] = LIQ_WATER;
		obj->value[1] += water;
		if ( !is_name("water acqua", obj->name) )
		{
			char	buf[MSL];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			sprintf( buf, "%s acqua", obj->name );
			free_string( obj->name );
			obj->name = str_dup( buf );
		}

		act( "Il contenitore e' gia' colmo.", ch, obj, NULL, TO_CHAR, POS_REST );
	}
}


void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !is_affected(victim, gsn_blindness) )
	{
		if ( victim == ch )
			send_to_char( "Ci vedi gia' bene.. non hai bisogno di cure.\n\r", ch );
		else
			act( "$N sembra non avere alcun problema di vista..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( check_dispel(level, victim, gsn_blindness) )
	{
		send_to_char( "La nebbia oscura che appannava il tuo sguardo va via, torni a vedere!\n\r", victim );
		act( "Gli occhi di $n tornano normali, ora sembra tornare a vederci!", victim, NULL, NULL, TO_ROOM, POS_REST );
	}
	else
		send_to_char( "Non riesci a scacciare l'oscura nebbia dallo sguardo.\n\r", ch );
}


void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			heal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	heal = dice( 3, 8 ) + level / 2;
	victim->point[LIFE] = UMIN( victim->point[LIFE] + heal, victim->max_point[LIFE] );
	update_pos( victim );
	send_to_char( "Le tue ferite iniziano a rimarginarsi!\n\r", victim );
	if ( ch != victim )
		send_to_char( "La cura e' andata a buon fine.\n\r", ch );
}


/*
 * RT added to cure plague
 */
void spell_cure_disease( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !is_affected(victim, gsn_plague) )
	{
		if ( victim == ch )
			send_to_char( "Non sei affetto da alcuna malattia al momento.\n\r", ch );
		else
			act( "Preoccupazione inutile,$N non sembra soffrire alcuna malattia al momento..", ch, NULL, victim, TO_CHAR, POS_REST );

		return;
	}

	if ( check_dispel(level, victim, gsn_plague) )
	{
		send_to_char( "Un senso di sollievo ti accompagna mentre senti la malattia abbandonare il tuo corpo.\n\r", victim );
		act( "$n sembra rilassarsi mentre la malattia abbandona il suo corpo.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}
	else
		send_to_char( "Non riesci ad aver ragione del morbo.. insisti.\n\r", ch );
}


void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			heal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	heal = dice( 1, 8 ) + level / 4 + 5;
	victim->point[LIFE] = UMIN( victim->point[LIFE] + heal, victim->max_point[LIFE] );
	update_pos( victim );
	send_to_char( "Le tue ferite si rimarginano lievemente!\n\r", victim );
	if ( ch != victim )
		send_to_char( "La cura ha avuto effetto.\n\r", ch );
}


void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !is_affected(victim, gsn_poison) )
	{
		if ( victim == ch )
			send_to_char( "Non hai alcun veleno nelle tue vene.\n\r", ch );
		else
			act( "$N non sembra soffrire alcun veleno.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( check_dispel(level, victim, gsn_poison) )
	{
		send_to_char( "Un caldo soffio di vita attraversa il tuo corpo.\n\r", victim );
		act( "$n sembra stare molto meglio.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}
	else
		send_to_char( "L'incantesimo non produce alcun effetto.\n\r", ch );
}


void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			heal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	heal = dice( 2, 8 ) + level / 3 + 10;
	victim->point[LIFE] = UMIN( victim->point[LIFE] + heal, victim->max_point[LIFE] );
	update_pos( victim );
	send_to_char( "Senti il dolore arrecato dalle tue ferite diminuire sensibilmente!\n\r", victim );
	if ( ch != victim )
		send_to_char( "La cura ha avuto effetto.\n\r", ch );
}


void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~*/

	/* deal with the object case first */
	if ( target == TARGET_OBJ )
	{
		obj = (OBJ_DATA *) vo;

		if ( obj->wear_loc != WEAR_NONE )
		{
			act( "Perche' l'incantesimo sia efficace l'oggetto deve essere nell'inventario.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( IS_OBJ_STAT(obj, ITEM_EVIL) )
		{
			act( "Blasfemi presagi e oscure luci gia' possiedono  $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( IS_OBJ_STAT(obj, ITEM_BLESS) )
		{
			AFFECT_DATA *paf;
			/*~~~~~~~~~~~~~*/

			paf = affect_find( obj->affected, skill_lookup("bless") );
			if ( !saves_dispel(level, paf != NULL ? paf->level : obj->level, 0) )
			{
				if ( paf != NULL )
					affect_remove_obj( obj, paf );
				act( "Vapori maligni avviluppano $p che brilla di una malvagia aura rossa.", ch, obj, NULL, TO_ALL, POS_REST );
				REMOVE_BIT( obj->extra_flags, ITEM_BLESS );
				return;
			}
			else
			{
				act( "Un'aura benefica circonda l'oggetto.. e la tua maledizione non riesce a varcarla.", ch, obj, NULL, TO_CHAR, POS_REST );
				return;
			}
		}

		af.where = TO_OBJECT;
		af.type = sn;
		af.level = level;
		af.duration = ( 8 + level / 5 );
		af.location = APPLY_SAVES;
		af.modifier = +1;
		af.bitvector = ITEM_EVIL;
		affect_to_obj( obj, &af );

		act( "$p risplende di un'aura malefica densa per un attimo di mille vapori arcani.", ch, obj, NULL, TO_ALL, POS_REST );
		return;
	}

	/* character curses */
	victim = (CHAR_DATA *) vo;

	if ( IS_AFFECTED(victim, AFF_CURSE) || saves_spell(level, victim, DAM_NEGATIVE) )
		return;
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 8 + level / 10 );
	af.location = APPLY_HITROLL;
	af.modifier = -1 * ( level / 8 );
	af.bitvector = AFF_CURSE;
	affect_to_char( victim, &af );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = level / 8;
	affect_to_char( victim, &af );

	send_to_char( "All'improvviso senti d'aver perso ogni favore degli Dei..\n\r", victim );
	if ( ch != victim )
		act( "$N sembra aver smarrito ogni favore degli dei..", ch, NULL, victim, TO_CHAR, POS_REST );
}


/*
 * RT replacement demonfire spell
 */
void spell_demonfire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(ch) && !IS_EVIL(ch) )
	{
		victim = ch;

		send_to_char( "Il fuoco nero che hai evocato si ribella contro di te!\n\r", ch );
		act( "$n evoca un fuoco nero che si ribella avvolgendol$x!", ch, NULL, victim, TO_NOTVICT, POS_REST);
	}

	if ( victim != ch )
	{
		act( "Evochi l'ira di un Demone nero su $N!",ch, NULL, victim, TO_CHAR, POS_REST);
		act( "$n evoca l'ira di un Demone nero contro di te!",ch, NULL, victim, TO_VICT, POS_REST, RED );
		act( "$n evoca l'ira di un Demone nero contro $N!",ch, NULL, victim, TO_NOTVICT, POS_REST);

	}

	dam = dice( level, 10 );
	if ( saves_spell(level, victim, DAM_NEGATIVE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE );
	spell_curse( gsn_curse, 3 * level / 4, ch, (void *) victim, TARGET_CHAR );
}


/*
 * added by chronos
 */
void spell_bluefire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(ch) && !IS_NEUTRAL(ch) )
	{
		victim = ch;
		send_to_char( "Le fiamme blu che hai richiamato bruciano sul tuo corpo!\n\r", ch );
	}

	if ( victim != ch )
	{
		act( "$n richiama l'energia del fuoco blu bruciando la terra intorno a $N!", ch, NULL, victim, TO_ROOM, POS_REST );
		act( "$n richiama intorno a te le fiamme blu della terra!", ch, NULL, victim, TO_VICT, POS_REST );
		send_to_char( "Richiami con un rituale l'energia delle fiamme della terra!\n\r", ch );
	}

	dam = dice( level, 10 );
	if ( saves_spell(level, victim, DAM_FIRE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
}


void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, DETECT_EVIL) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sensibile al male.\n\r", ch );
		else
			act( "$N e' gia' in grado di distinguere il male.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 5 + level / 3 );
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = DETECT_EVIL;
	affect_to_char( victim, &af );
	send_to_char( "I tuoi occhi risplendono per un attimo.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo e' andato a buon fine.\n\r", ch );
}


void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, DETECT_GOOD) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' in grado di distinguere il bene.\n\r", ch );
		else
			act( "$N e' gia' in gradi di distinguere il bene.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 5 + level / 3 );
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = DETECT_GOOD;
	affect_to_char( victim, &af );
	send_to_char( "I tuoi occhi brillano per un attimo.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo ha sortito il suo effetto.\n\r", ch );
}


void spell_detect_hidden( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, DETECT_HIDDEN) )
	{
		if ( victim == ch )
			send_to_char( "Ti stai gia' sforzando di identificare anche le ombre piu' nascoste..\n\r", ch );
		else
			act( "$N si sta gia' sforzando di cogliere anche le ombre piu' nascoste.", ch, NULL, victim, TO_CHAR, POS_REST );

		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 5 + level / 3 );
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = DETECT_HIDDEN;
	affect_to_char( victim, &af );
	send_to_char( "Ti sforzi di cogliere anche le ombre piu' nascoste.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo sembra aver avuto esito positivo.\n\r", ch );
}


void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, DETECT_INVIS) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' in grado di distinguere cio' che ai comuni occhi e' invisibile.\n\r", ch );
		else
			act( "$N e' gia' in grado di distinguere l'invisibile.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 5 + level / 3 );
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = DETECT_INVIS;
	affect_to_char( victim, &af );
	send_to_char( "Un biancore attraversa il tuo sguardo, ed ora molte cose prima invisibili ti sono piu' chiare.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo sembra aver sortito il suo effetto.\n\r", ch );
}


void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, DETECT_MAGIC) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' in grado di distinguere le auree magiche.\n\r", ch );
		else
			act( "$N e' gia' in grado di distinguere le auree magiche.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 5 + level / 3 );
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = DETECT_MAGIC;
	affect_to_char( victim, &af );
	send_to_char( "Una strana luce attraversa i tuoi occhi.. e tutto sembra un po' diverso da com'era prima.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo e' andato a buon fine.\n\r", ch );
}


void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
	{
		if ( obj->value[3] != 0 )
			send_to_char( "Percepisci tracce di veleno..\n\r", ch );
		else
			send_to_char( "Non ti sembra di percepire tracce di veleno.\n\r", ch );
	}
	else
		send_to_char( "Non sembra affatto cibo avvelenato.\n\r", ch );
}


void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(ch) && IS_EVIL(ch) ) victim = ch;

	if ( IS_GOOD(victim) )
	{
		act( "Gli dei benevoli sono dalla parte di $N.", ch, NULL, victim, TO_ROOM, POS_REST );
		return;
	}

	if ( IS_NEUTRAL(victim) )
	{
		act( "L'incantesimo non risveglia in $N alcun dolore.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim->point[LIFE] > (ch->level * 4) )
		dam = dice( level, 4 );
	else
		dam = UMAX( victim->point[LIFE], dice(level, 4) );

	if ( saves_spell(level, victim, DAM_HOLY) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_HOLY, TRUE );
}


void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(ch) && IS_GOOD(ch) )
		victim = ch;

	if ( IS_EVIL(victim) )
	{
		act( "Gli Dei maligni proteggono $N!", ch, NULL, victim, TO_ROOM, POS_REST );
		return;
	}

	if ( IS_NEUTRAL(victim) )
	{
		act( "L'incantesimo non sembra risvegliare alcun dolore su $N.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim->point[LIFE] > (ch->level * 4) )
		dam = dice( level, 4 );
	else
		dam = UMAX( victim->point[LIFE], dice(level, 4) );

	if ( saves_spell(level, victim, DAM_NEGATIVE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE );
}


/*
 * Modified for enhanced use
 */
void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	bool		found = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( saves_spell(level, victim, DAM_OTHER) )
	{
		send_to_char( "Avverti un soffio di energia attraversarti il corpo, senza alcun effetto.\n\r", victim );
		send_to_char( "L'incantesimo non ha vinto la resistenza del bersaglio.. riprova!\n\r", ch );
		return;
	}

	/* begin running through the spells */
	if ( check_dispel(level, victim, skill_lookup("armor")) )
	{
		found = TRUE;
		act( "L'armatura che circondava $n sembra svanire", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("enhanced armor")) )
	{
		found = TRUE;
		act( "$n sembra avere meno protezione dagli Dei.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("bless")) )			found = TRUE;

	if ( check_dispel(level, victim, skill_lookup("blindness")) )
	{
		found = TRUE;
		act( "Gli occhi di $n perdono l'alone oscuro che li ottenebrava.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("calm")) )
	{
		found = TRUE;
		act( "$n sembra aver perso un po' di serenita'..", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("change sex")) )
	{
		found = TRUE;
		act( "$n torna ad assumere le sue solite sembianze..", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("charm person")) )
	{
		found = TRUE;
		act( "$n libera il suo arbitrio dalle magiche catene che lo opprimevano.", victim, NULL, NULL, TO_ROOM, POS_REST );

		stop_follower( victim );
		victim->master = NULL;
	}

	if ( check_dispel(level, victim, skill_lookup("chill touch")) )
	{
		found = TRUE;
		act( "$n smette di tremare mentre la sua epidermide torna alla normale colorazione.",
			victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("curse")) )
	{
		found = TRUE;
		act( "La maledizione abbandona $n.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect evil")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere il male.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect good")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere il bene.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect hidden")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere le cose nascoste.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect invis")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere l'invisibile.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("detect magic")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di distinguere il magico.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("faerie fire")) )
	{
		act( "Le strana sagoma fluorescente attorno a $n svanisce nel nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("fly")) )
	{
		act( "$n smette di fluttuare tornando rapidamente a terra!", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("frenzy")) )
	{
		act( "Lo sguardo quasi collerico di $n si placa improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("giant strength")) )
	{
		act( "La muscolatura sul corpo di $n torna normale.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("haste")) )
	{
		act( "$n torna a muoversi normalmente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("infravision")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di vedere nelle tenebre.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("invis")) )
	{
		found = TRUE;
		act( "$n riappare lentamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("mass invis")) )
	{
		found = TRUE;
		act( "$n riappare lentamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("pass door")) )
	{
		found = TRUE;
		act( "$n non e' piu' in grado di passare attraverso le porte.", victim, NULL, NULL, TO_ROOM , POS_REST);
	}

	if ( check_dispel(level, victim, skill_lookup("protection evil")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal male.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("protection good")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal bene.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("sanctuary")) )
	{
		act( "La forte aura bianca che proteggeva $n comincia a diradarsi per poi sparire nel nulla.",
			victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( IS_AFFECTED(victim, AFF_SANCTUARY)
	&&	!saves_dispel(level, victim->level, -1)
	&&	!is_affected(victim, skill_lookup("sanctuary"))
	&&	!(victim->spec_fun == spec_special_guard || victim->spec_fun == spec_stalker) )
	{
		REMOVE_BIT( victim->affected_by, AFF_SANCTUARY );
		act( "La forte aura bianca che proteggeva $n comincia a diradarsi per poi sparire nel nulla.",
			victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("shield")) )
	{
		act( "Lo scudo protettivo sul corpo di $n si dissolve magicamente.",
			victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("sleep")) )
	{
		found = TRUE;
		act( "$n si sveglia improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("slow")) )
	{
		act( "$n torna a muoversi normalmente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("stone skin")) )
	{
		act( "La rocciosa epidermide di $n si sgretola svanendo nel nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("weaken")) )
	{
		act( "La debolezza che assaliva $n svanisce di colpo. ", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("shielding")) )
	{
		found = TRUE;
		act( "$n non ha la protezione magica.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("fear")) )
	{
		found = TRUE;
		act( "$n non ha timore della battaglia.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("protection heat")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal caldo.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("protection cold")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal freddo.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("magic resistance")) )
	{
		found = TRUE;
		act( "$n non ha piu' la protezione dal magico.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("hallucination")) )
	{
		found = TRUE;
		act( "$n torna a distinguere chiaramente la realta'.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("terangreal")) )
		found = TRUE;

	if ( check_dispel(level, victim, skill_lookup("power word stun")) )
	{
		found = TRUE;
		act( "$n non e' piu' sotto pietrificazione.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( check_dispel(level, victim, skill_lookup("corruption")) )
	{
		act( "$n sembra riacquisire un po' di salute.", victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( check_dispel(level, victim, skill_lookup("web")) )
	{
		act( "Gli spessi filamenti di ragnatela che imprigionavano $n si dissolvono.",
			victim, NULL, NULL, TO_ROOM, POS_REST );
		found = TRUE;
	}

	if ( found )
		send_to_char( "Incantesimo compiuto.\n\r", ch );
	else
		send_to_char( "L'incantesimo ha fallito.\n\r", ch );
}


void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	OBJ_DATA	*obj, *obj_next, *corpse;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	send_to_char( "La terra si muove ribellandosi sotto di te!\n\r", ch );
	act( "$n invoca gli elementi della terra facendola tremare!.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next;
		if ( vch->in_room == NULL )
			continue;
		if ( vch->in_room == ch->in_room )
		{
			if ( vch != ch && !is_safe_spell(ch, vch, TRUE) && !is_same_group(ch, vch) )
			{
				if ( is_safe(ch, vch) )
					continue;
			}

			if ( ch == vch )
				continue;
			if ( IS_AFFECTED(vch, AFF_FLYING) )
				damage( ch, vch, 0, sn, DAM_BASH, TRUE );
			else
				damage( ch, vch, level + dice(2, 8), sn, DAM_BASH, TRUE );
			continue;
		}

		if ( vch->in_room->area == ch->in_room->area )
			send_to_char( "La terra e' attraversata da violente scosse..\n\r", vch );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next_content;
		if ( obj->pIndexData->vnum == OBJ_VNUM_GRAVE_STONE
		&&	(corpse = obj->contains) != NULL
		&&	number_percent() < get_skill(ch, sn) )
		{
			obj_from_obj( corpse );
			REMOVE_BIT( corpse->extra_flags, ITEM_BURIED );
			obj_to_room( corpse, ch->in_room );
			extract_obj( obj );
			corpse->timer = number_range( 25, 40 );
			act( "Il moto terrestre riporta alla luce $p.", ch, corpse, NULL, TO_ALL, POS_REST );
		}
	}
}


void spell_enchant_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA *paf;
	int			result, fail;
	int			ac_bonus, added;
	bool		ac_found = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type != ITEM_ARMOR )
	{
		send_to_char( "Questo incantesimo funziona solo su oggetti che offrono protezione, come armature o elmi.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Per incantare l'oggetto dovresti prima.. possederlo? .\n\r", ch );
		return;
	}

	/* this means they have no bonus */
	ac_bonus = 0;
	fail = 25;					/* base 25% chance of failure */

	/* find the bonuses */
	if ( !obj->enchanted )
	{
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location == APPLY_AC )
			{
				ac_bonus = paf->modifier;
				ac_found = TRUE;
				fail += 4 * ( ac_bonus * ac_bonus );
			}
			else
				/* things get a little harder */
				fail += 20;
		}
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->location == APPLY_AC )
		{
			ac_bonus = paf->modifier;
			ac_found = TRUE;
			fail += 4 * ( ac_bonus * ac_bonus );
		}
		else
			/* things get a little harder */
			fail += 20;
	}

	/* apply other modifiers */
	fail -= level;

	if ( IS_OBJ_STAT(obj, ITEM_BLESS) ) fail -= 15;
	if ( IS_OBJ_STAT(obj, ITEM_GLOW) )	fail -=  5;

	fail = URANGE( 5, fail, 85 );

	result = number_percent();

	/* the moment of truth */
	if ( result < (fail / 5) )	/* item destroyed */
	{
		act( "$p risplende fino quasi ad accecare, ma dura solo un attimo, il bagliore svanisce lasciando solo polvere..", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende fino quasi ad accecare, ma dura solo un attimo, il bagliore svanisce lasciando solo polvere..", ch, obj, NULL, TO_ROOM, POS_REST );
		extract_obj( obj );
		return;
	}

	if ( result < (fail / 3) )	/* item disenchanted */
	{
		AFFECT_DATA *paf_next;
		/*~~~~~~~~~~~~~~~~~~*/

		act( "$p risplende di una luce fioca.. che svanisce subito", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende di una luce fioca.. che svanisce subito", ch, obj, NULL, TO_ROOM, POS_REST );
		obj->enchanted = TRUE;

		/* remove all affects */
		for ( paf = obj->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			free_affect( paf );
		}

		obj->affected = NULL;

		/* clear all flags */
		obj->extra_flags = 0;
		return;
	}

	if ( result <= fail )		/* failed, no bad result */
	{
		send_to_char( "Esamini l'oggetto.. ma l'incantesimo sembra non aver prodotto alcun effetto.\n\r", ch );
		return;
	}

	/* okay, move all the old flags into new vectors if we have to */
	if ( !obj->enchanted )
	{
		AFFECT_DATA *af_new;
		/*~~~~~~~~~~~~~~~~*/

		obj->enchanted = TRUE;

		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			af_new = new_affect();

			af_new->next = obj->affected;
			obj->affected = af_new;

			af_new->where = paf->where;
			af_new->type = UMAX( 0, paf->type );
			af_new->level = paf->level;
			af_new->duration = paf->duration;
			af_new->location = paf->location;
			af_new->modifier = paf->modifier;
			af_new->bitvector = paf->bitvector;
		}
	}

	if ( result <= (90 - level / 5) )	/* success! */
	{
		act( "Un'aura dorata si forgia attorno a $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "Un'aura dorata si forgia attorno a $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		SET_BIT( obj->extra_flags, ITEM_MAGIC );
		added = -1;
	}
	else	/* exceptional enchant */
	{
		act( "Una splendida aura dorata riveste completamente $p!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "Una splendida aura dorata riveste completamente $p!", ch, obj, NULL, TO_ROOM, POS_REST );
		SET_BIT( obj->extra_flags, ITEM_MAGIC );
		SET_BIT( obj->extra_flags, ITEM_GLOW );
		added = -2;
	}

	/* now add the enchantments */
	if ( obj->level < LEVEL_HERO ) obj->level = UMIN( LEVEL_HERO - 1, obj->level + 1 );

	if ( ac_found )
	{
		for ( paf = obj->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location == APPLY_AC )
			{
				paf->type = sn;
				paf->modifier += added;
				paf->level = UMAX( paf->level, level );
			}
		}
	}
	else	/* add a new affect */
	{
		paf = new_affect();

		paf->where = TO_OBJECT;
		paf->type = sn;
		paf->level = level;
		paf->duration = -1;
		paf->location = APPLY_AC;
		paf->modifier = added;
		paf->bitvector = 0;
		paf->next = obj->affected;
		obj->affected = paf;
	}
}


void spell_enchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA *paf;
	int			result, fail;
	int			hit_bonus, dam_bonus, added;
	bool		hit_found = FALSE, dam_found = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type != ITEM_WEAPON )
	{
		send_to_char( "L'incantesimo richiede un'arma.Una vera..\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "L'incantesimo richiede che tu abbia l'arma in inventario perche' sia efficace.\n\r", ch );
		return;
	}

	/* this means they have no bonus */
	hit_bonus = 0;
	dam_bonus = 0;
	fail = 25;					/* base 25% chance of failure */

	/* find the bonuses */
	if ( !obj->enchanted )
	{
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location == APPLY_HITROLL )
			{
				hit_bonus = paf->modifier;
				hit_found = TRUE;
				fail += 3 * ( hit_bonus * hit_bonus );
			}
			else if ( paf->location == APPLY_DAMROLL )
			{
				dam_bonus = paf->modifier;
				dam_found = TRUE;
				fail += 3 * ( dam_bonus * dam_bonus );
			}
			else
				/* things get a little harder */
				fail += 25;
		}
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->location == APPLY_HITROLL )
		{
			hit_bonus = paf->modifier;
			hit_found = TRUE;
			fail += 3 * ( hit_bonus * hit_bonus );
		}
		else if ( paf->location == APPLY_DAMROLL )
		{
			dam_bonus = paf->modifier;
			dam_found = TRUE;
			fail += 3 * ( dam_bonus * dam_bonus );
		}
		else
			/* things get a little harder */
			fail += 25;
	}

	/* apply other modifiers */
	fail -= 3 * level / 2;

	if ( IS_OBJ_STAT(obj, ITEM_BLESS) ) fail -= 15;
	if ( IS_OBJ_STAT(obj, ITEM_GLOW) )	fail -=  5;

	fail = URANGE( 5, fail, 95 );

	result = number_percent();

	/* the moment of truth */
	if ( result < (fail / 5) )	/* item destroyed */
	{
		act( "$p diventa incandescente e brucia!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p diventa incandescente e brucia!", ch, obj, NULL, TO_ROOM, POS_REST );
		extract_obj( obj );
		return;
	}

	if ( result < (fail / 2) )	/* item disenchanted */
	{
		AFFECT_DATA *paf_next;
		/*~~~~~~~~~~~~~~~~~~*/

		act( "$p splende per un attimo.. diventa incandescente.. e torna normale.. peccato!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p splende per un attimo.. diventa incandescente.. e torna normale..", ch, obj, NULL, TO_ROOM, POS_REST );
		obj->enchanted = TRUE;

		/* remove all affects */
		for ( paf = obj->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			free_affect( paf );
		}

		obj->affected = NULL;

		/* clear all flags */
		obj->extra_flags = 0;
		return;
	}

	if ( result <= fail )		/* failed, no bad result */
	{
		send_to_char( "L'oggetto non ha subito nessuna trasformazione..\n\r", ch );
		return;
	}

	/* okay, move all the old flags into new vectors if we have to */
	if ( !obj->enchanted )
	{
		AFFECT_DATA *af_new;
		/*~~~~~~~~~~~~~~~~*/

		obj->enchanted = TRUE;

		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			af_new = new_affect();

			af_new->next = obj->affected;
			obj->affected = af_new;

			af_new->where = paf->where;
			af_new->type = UMAX( 0, paf->type );
			af_new->level = paf->level;
			af_new->duration = paf->duration;
			af_new->location = paf->location;
			af_new->modifier = paf->modifier;
			af_new->bitvector = paf->bitvector;
		}
	}

	if ( result <= (100 - level / 5) )	/* success! */
	{
		act( "$p risplende di una fievole luce bluastra.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende di una fievole luce bluastra.", ch, obj, NULL, TO_ROOM, POS_REST );
		SET_BIT( obj->extra_flags, ITEM_MAGIC );
		added = 1;
	}
	else	/* exceptional enchant */
	{
		act( "$p risplende di una meravigliosa luce bianca!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende di una meravigliosa luce bianca!", ch, obj, NULL, TO_ROOM, POS_REST );
		SET_BIT( obj->extra_flags, ITEM_MAGIC );
		SET_BIT( obj->extra_flags, ITEM_GLOW );
		added = 2;
	}

	/* now add the enchantments */
	if ( obj->level < LEVEL_HERO - 1 )
		obj->level = UMIN( LEVEL_HERO - 1, obj->level + 1 );

	if ( dam_found )
	{
		for ( paf = obj->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location == APPLY_DAMROLL )
			{
				paf->type = sn;
				paf->modifier += added;
				paf->level = UMAX( paf->level, level );
				if ( paf->modifier > 4 ) SET_BIT( obj->extra_flags, ITEM_HUM );
			}
		}
	}
	else	/* add a new affect */
	{
		paf = new_affect();

		paf->where = TO_OBJECT;
		paf->type = sn;
		paf->level = level;
		paf->duration = -1;
		paf->location = APPLY_DAMROLL;
		paf->modifier = added;
		paf->bitvector = 0;
		paf->next = obj->affected;
		obj->affected = paf;
	}

	if ( hit_found )
	{
		for ( paf = obj->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location == APPLY_HITROLL )
			{
				paf->type = sn;
				paf->modifier += added;
				paf->level = UMAX( paf->level, level );
				if ( paf->modifier > 4 )
					SET_BIT( obj->extra_flags, ITEM_HUM );
			}
		}
	}
	else	/* add a new affect */
	{
		paf = new_affect();

		paf->type = sn;
		paf->level = level;
		paf->duration = -1;
		paf->location = APPLY_HITROLL;
		paf->modifier = added;
		paf->bitvector = 0;
		paf->next = obj->affected;
		obj->affected = paf;
	}
}


/*
 * Drain XP, MANA, HP. Caster gains HP.
 */
void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( saves_spell(level, victim, DAM_NEGATIVE) )
	{
		send_to_char( "Senti un brivido gelido sfiorarti.. ma passa in fretta.\n\r", victim );
		return;
	}

	if ( victim->level <= 2 )
	{
		dam = ch->point[LIFE] + 1;
	}
	else
	{
		gain_exp( victim, 0 - number_range(level / 3, 4 * level / 5) );
		victim->point[MANA] /= 2;
		victim->point[MOVE] /= 2;
		dam = dice( 1, level );
		ch->point[LIFE] += dam;
	}

	act( "$n richiama una sinistra ombra sul tuo corpo, la tua vita scivola via!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Alzi un braccio verso $N deviando a te il suo flusso vitale!", ch, NULL, victim, TO_CHAR, POS_REST );
	act( "$n richiama una sinistra ombra sul corpo di $N che agonizza!", ch, NULL, victim, TO_NOTVICT, POS_REST );

	damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE );
}


void spell_hellfire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 7 );

	damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
}


void spell_iceball( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*tmp_vict;
	CHAR_DATA	*tmp_next;
	int			dam;
	int			movedam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 12 );
	movedam = number_range( ch->level, 2 * ch->level );

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

			if ( saves_spell(level, tmp_vict, DAM_COLD) )
				dam /= 2;
			damage( ch, tmp_vict, dam, sn, DAM_COLD, TRUE );
			tmp_vict->point[MOVE] -= UMIN( tmp_vict->point[MOVE], movedam );
		}
	}
}


void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*tmp_vict;
	CHAR_DATA	*tmp_next;
	int			dam;
	int			movedam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 12 );
	movedam = number_range( ch->level, 2 * ch->level );

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

			if ( saves_spell(level, tmp_vict, DAM_FIRE) )
				dam /= 2;
			damage( ch, tmp_vict, dam, sn, DAM_FIRE, TRUE );
			tmp_vict->point[MOVE] -= UMIN( tmp_vict->point[MOVE], movedam );
		}
	}
}


void spell_fireproof( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_OBJ_STAT(obj, ITEM_BURN_PROOF) )
	{
		act( "$p ha gia' una protezione contro le fiamme.", ch, obj, NULL, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = number_fuzzy( level / 4 );
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ITEM_BURN_PROOF;

	affect_to_obj( obj, &af );

	act( "Avvolgi $p con una spessa barriera contro le fiamme.", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$n avvolge $p con una spessa barriera magica.", ch, obj, NULL, TO_ROOM, POS_REST );
}


void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 10 );
	if ( saves_spell(level, victim, DAM_FIRE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
}


void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
		return;
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 10 + level / 5;
	af.location = APPLY_AC;
	af.modifier = 2 * level;
	af.bitvector = AFF_FAERIE_FIRE;
	affect_to_char( victim, &af );
	send_to_char( "Il tuo corpo viene avvolto da una sagoma fluorescente.\n\r", victim );
	act( "Il corpo di $n viene avvolto da una sagoma fluorescente.", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*ich;
	/*~~~~~~~~~~~~~*/

	act( "$n evoca una fitta nube rosa che riempie l'ambiente.", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Evochi una fitta nube rosa che riempie l'ambiente.\n\r", ch );

	for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
	{
		if ( ich->invis_level > 0 )
			continue;
		if ( ich == ch || saves_spell(level, ich, DAM_OTHER) )
			continue;

		affect_strip( ich, gsn_invis );
		affect_strip( ich, gsn_mass_invis );
		affect_strip( ich, gsn_imp_invis );
		REMOVE_BIT( ich->affected_by, AFF_HIDE );
		REMOVE_BIT( ich->affected_by, AFF_FADE );
		REMOVE_BIT( ich->affected_by, AFF_INVISIBLE );
		REMOVE_BIT( ich->affected_by, AFF_IMP_INVIS );

		/* An elf sneaks eternally */
		if ( IS_MOB(ich) || !IS_SET(race_table[ich->race].aff, AFF_SNEAK) )
		{
			affect_strip( ich, gsn_sneak );
			REMOVE_BIT( ich->affected_by, AFF_SNEAK );
		}

		act( "Gli sbuffi di polvere rosa rivelano la presenza di $n!", ich, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Gli sbuffi di polvere rosa ti rendono visibile agli altri!\n\r", ich );
	}
}


void spell_floating_disc( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*disc, *floating;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	floating = get_eq_char( ch, WEAR_FLOAT );
	if ( floating != NULL && IS_OBJ_STAT(floating, ITEM_NOREMOVE) )
	{
		act( "Non e' di certo un vestito che puoi mettere e togliere..", ch, floating, NULL, TO_CHAR, POS_REST );
		return;
	}

	disc = create_object( get_obj_index(OBJ_VNUM_DISC), 0 );
	disc->value[0] = ch->level * 10;	/* 10 pounds per level capacity */
	disc->value[3] = ch->level * 5;		/* 5 pounds per level max per item */
	disc->timer = ch->level * 2 - number_range( 0, level / 2 );

	act( "$n allarga le braccia disegnando nell'aria un grande cerchio che si materializza in un disco nero fluttuante.", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Allarghi le braccia disegnando nell'aria un cerchio che con un soffio di polvere materializzi creando il tuo disco fluttuante.\n\r", ch );
	obj_to_char( disc, ch );
	wear_obj( ch, disc, TRUE );
}


void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_FLYING) )
	{
		if ( victim == ch )
			send_to_char( "Ops.. sei gia' in volo!\n\r", ch );
		else
			act( "$N e' gia' in volo, non ha bisogno del tuo aiuto.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level + 3;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = AFF_FLYING;
	affect_to_char( victim, &af );
	act( "$n si solleva lentamente da terra..", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Ti sollevi lentamente da terra..\n\r", victim );

}


/*
 * RT clerical berserking spell
 */
void spell_frenzy( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) || IS_AFFECTED(victim, AFF_BERSERK) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' in te la benedizione della furia.\n\r", ch );
		else
			act( "$N ha gia' in se' la benedizione della furia.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, skill_lookup("calm")) )
	{
		if ( victim == ch )
			send_to_char( "L'ira mal si accompagna con un animo mite come il tuo.. in questo momento.\n\r", ch );
		else
			act( "$N ha uno sguardo cosi' mite e placido.. perche' mai dovrebbe infuriarsi?", ch, NULL, victim, TO_CHAR, POS_REST );

		return;
	}

	if( (IS_GOOD(ch) && !IS_GOOD(victim))
	||	(IS_NEUTRAL(ch) && !IS_NEUTRAL(victim))
	||	(IS_EVIL(ch) && !IS_EVIL(victim)) )
	{
		act( "I tuoi dei rifiutano di dare la loro forza a $N.. scegli meglio le tue amicizie.", ch, NULL, victim, TO_CHAR, POS_REST );
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

	send_to_char( "Richiami l'ira degli Dei nelle tue vene!\n\r", victim );
	act( "Un forte luce avvolge $n mentre dai suoi occhi scaturiscono fiamme!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	bool		gate_pet;
	/*~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	!can_see_room(ch, victim->in_room)
	||	IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	victim->level >= level + 3
	||	saves_spell(level, victim, DAM_OTHER)	/* || (!IS_MOB(victim) && victim->level >= LEVEL_HERO) * NOT trust */
	||	(IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(!IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||	(!IS_MOB(victim) && ch->in_room->area != victim->in_room->area)
	||	(IS_MOB(victim) && saves_spell(level, victim, DAM_OTHER)) )
	{
		send_to_char( "Non riesci a concentrarti bene sul bersaglio e aggirarne le difese.. riprova.\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Non puoi creare il portale dimensionale.\n\r",ch );
		return;
	}

	if ( ch->pet != NULL && ch->in_room == ch->pet->in_room )
		gate_pet = TRUE;
	else
		gate_pet = FALSE;

	act( "$n materializza dal nulla un portale nel quale scompare misteriosamente.", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Punti due dita davanti a te disegnando un'ellisse nell'aria, il portale si materializza conducendoti a destinazione.\n\r", ch );
	char_from_room( ch );
	char_to_room( ch, victim->in_room );

	act( "Un misterioso portale si materializza per un attimo rivelando la presenza di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );

	/* da controllare il pezzo seguente a cosa si riferisca cosi' lo modifico di conseguenza */
	if ( gate_pet )
	{
		act( "$n attraversa un portale e svanisce nel nulla.", ch->pet, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Attraversi un portale dimensionale svanendo nel nulla.\n\r", ch->pet );
		char_from_room( ch->pet );
		char_to_room( ch->pet, victim->in_room );
		act( "$n arriva attraverso un portale dimensionale.", ch->pet, NULL, NULL, TO_ROOM, POS_REST );
		do_look( ch->pet, "auto" );
	}
}


void spell_giant_strength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "L'incantesimo non ha alcun effetto.. sei gia' piu' forte di quanto tu possa essere!\n\r", ch );
		else
			act( "Richiami su $N la forza degli elementi ma non succede nulla di nuovo.", ch, NULL, victim, TO_CHAR, POS_REST );

		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 10 + level / 3 );
	af.location = APPLY_STR;
	af.modifier = UMAX( 2, level / 10 );
	af.bitvector = 0;
	affect_to_char( victim, &af );
	send_to_char( "Avverti delle forti scariche elettriche attraversarti il corpo!\n\r", victim );
	send_to_char( "Ti senti piu' in forze!\n\r", victim );
	act( "Il corpo di $n e' attraversato per un attimo da scariche elettriche!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = UMAX( 20, victim->point[LIFE] - dice(1, 4) );
	if ( saves_spell(level, victim, DAM_HARM) )
		dam = UMIN( 50, dam / 2 );
	dam = UMIN( 100, dam );
	damage( ch, victim, dam, sn, DAM_HARM, TRUE );
}


/*
 * RT haste spell
 */
void spell_haste( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) || IS_AFFECTED(victim, AFF_HASTE) || IS_SET(victim->off_flags, OFF_FAST) )
	{
		if ( victim == ch )
			send_to_char( "Andare ancora piu' veloce potrebbe essere dannoso per il tuo organismo, abbi rispetto dei consigli accademici!\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_SLOW) )
	{
		if ( !check_dispel(level, victim, skill_lookup("slow")) )
		{
			if ( victim != ch )
				send_to_char( "L'incantesimo non e' andato a buon fine.\n\r", ch );
			send_to_char( "Ti senti per un attimo piu' veloce.. ma torni subito a muoverti come prima.\n\r", victim );
			return;
		}

		act( "$n torni a muoverti in maniera normale.", victim, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	if ( victim == ch )
		af.duration = level / 2;
	else
		af.duration = level / 4;
	af.location = APPLY_DEX;
	af.modifier = UMAX( 2, level / 12 );
	af.bitvector = AFF_HASTE;
	affect_to_char( victim, &af );
	send_to_char( "Nuove energie attraversano il tuo corpo e senti di poterti muovere piu' velocemente.\n\r", victim );
	act( "$n sembra riuscire a muoversi molto piu' velocemente ora.", victim, NULL, NULL, TO_ROOM, POS_REST );
	if ( ch != victim )
		send_to_char( "L'incantesimo e' andato a buon fine.\n\r", ch );
}


void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	victim->point[LIFE] = UMIN( victim->point[LIFE] + 100 + level / 10, victim->max_point[LIFE] );
	update_pos( victim );
	send_to_char( "Un caldo soffio di vita riempie il tuo corpo.\n\r", victim );
	if ( ch != victim )
		send_to_char( "La cura ha avuto effetto.\n\r", ch );
}


void spell_heat_metal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	OBJ_DATA	*obj_lose, *obj_next;
	int			dam = 0;
	bool		fail = TRUE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !saves_spell(level + 2, victim, DAM_FIRE) && !IS_SET(victim->imm_flags, IMM_FIRE) )
	{
		for ( obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next )
		{
			obj_next = obj_lose->next_content;
			if ( number_range(1, 2 * level) > obj_lose->level
			&&	!saves_spell(level, victim, DAM_FIRE)
			&&	is_metal(obj_lose)
			&&	!IS_OBJ_STAT(obj_lose, ITEM_BURN_PROOF) )
			{
				switch ( obj_lose->item_type )
				{
				case ITEM_ARMOR:
					if ( obj_lose->wear_loc != -1 ) /* remove the item */
					{
						if ( can_drop_obj(victim, obj_lose)
						&&	(obj_lose->weight / 10) < number_range(1, 2 * get_curr_stat(victim, DEX))
						&&	remove_obj(victim, obj_lose, TRUE) )
						{
							act( "$n urla dal dolore mentre getta $p lontano da se'!", victim, obj_lose, NULL, TO_ROOM, POS_REST );
							act( "$p brucia sul tuo corpo costringendoti liberartene all'istante!", victim, obj_lose, NULL, TO_CHAR, POS_REST );
							dam += ( number_range(1, obj_lose->level) / 3 );
							obj_from_char( obj_lose );
							obj_to_room( obj_lose, victim->in_room );
							fail = FALSE;
						}
						else						/* stuck on the body! ouch! */
						{
							act( "$p BRUCIA e ustiona la tua pelle!", victim, obj_lose, NULL, TO_CHAR, POS_REST );
							dam += ( number_range(1, obj_lose->level) );
							fail = FALSE;
						}
					}
					else		/* drop it if we can */
					{
						if ( can_drop_obj(victim, obj_lose) )
						{
							act( "$n urla dal dolore e getta via $p!", victim, obj_lose, NULL, TO_ROOM, POS_REST );
							act( "Butti via $p prima che inizi a bruciarti!", victim, obj_lose, NULL, TO_CHAR, POS_REST );
							dam += ( number_range(1, obj_lose->level) / 6 );
							obj_from_char( obj_lose );
							obj_to_room( obj_lose, victim->in_room );
							fail = FALSE;
						}
						else	/* cannot drop */
						{
							act( "$p BRUCIA e ustiona la tua pelle!", victim, obj_lose, NULL, TO_CHAR, POS_REST );
							dam += ( number_range(1, obj_lose->level) / 2 );
							fail = FALSE;
						}
					}
					break;

				case ITEM_WEAPON:
					if ( obj_lose->wear_loc != -1 ) /* try to drop it */
					{
						if ( IS_WEAPON_STAT(obj_lose, WEAPON_FLAMING) ) continue;

						if ( can_drop_obj(victim, obj_lose) && remove_obj(victim, obj_lose, TRUE) )
						{
							act( "$p brucia e ustiona $n che se ne libera immediatamente!", victim, obj_lose, NULL, TO_ROOM, POS_REST );
							send_to_char( "La tua arma diventa rovente come il fuoco e non riesci a tenerla piu'!\n\r", victim );
							dam += 1;
							obj_from_char( obj_lose );
							obj_to_room( obj_lose, victim->in_room );
							fail = FALSE;
						}
						else						/* YOWCH! */
						{
							send_to_char( "La tua arma diventa rovente e BRUCIA le tue carni!\n\r", victim );
							dam += number_range( 1, obj_lose->level );
							fail = FALSE;
						}
					}
					else		/* drop it if we can */
					{
						if ( can_drop_obj(victim, obj_lose) )
						{
							act( "$n getta via $p prima di ustionarsi!", victim, obj_lose, NULL, TO_ROOM, POS_REST );
							act( "Ti liberi immediatamente di $p prima che ti bruci!", victim, obj_lose, NULL, TO_CHAR, POS_REST );
							dam += ( number_range(1, obj_lose->level) / 6 );
							obj_from_char( obj_lose );
							obj_to_room( obj_lose, victim->in_room );
							fail = FALSE;
						}
						else	/* cannot drop */
						{
							act( "Le tue carni vengono bruciate vive da $p!", victim, obj_lose, NULL, TO_CHAR, POS_REST );
							dam += ( number_range(1, obj_lose->level) / 2 );
							fail = FALSE;
						}
					}
					break;
				}
			}
		}
	}

	if ( fail )
	{
		send_to_char( "Il tuo incantesimo non riesce a varcare le difese del bersaglio.\n\r", ch );
		send_to_char( "Ti senti per un solo attimo soffocare per il caldo.\n\r", victim );
	}
	else						/* damage! */
	{
		if ( saves_spell(level, victim, DAM_FIRE) )
			dam = 2 * dam / 3;
		damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
	}
}


/*
 * RT really nasty high-level attack spell
 */
void spell_holy_word( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	int			dam;
	int			bless_num, curse_num, frenzy_num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	bless_num = skill_lookup( "bless" );
	curse_num = skill_lookup( "curse" );
	frenzy_num = skill_lookup( "frenzy" );

	act( "$n invoca l'aiuto degli dei con una potente preghiera!", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Richiami a te l'aiuto degli Dei con una potente preghiera!\n\r", ch );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( (IS_GOOD(ch) && IS_GOOD(vch)) || (IS_EVIL(ch) && IS_EVIL(vch)) || (IS_NEUTRAL(ch) && IS_NEUTRAL(vch)) )
		{
			send_to_char( "Senti nuove energie attraversarti il corpo!\n\r", vch );
			spell_frenzy( frenzy_num, level, ch, (void *) vch, TARGET_CHAR );
			spell_bless( bless_num, level, ch, (void *) vch, TARGET_CHAR );
		}
		else if ( (IS_GOOD(ch) && IS_EVIL(vch)) || (IS_EVIL(ch) && IS_GOOD(vch)) )
		{
			if ( !is_safe_spell(ch, vch, TRUE) )
			{
				if ( vch != ch
				&&	ch->fighting != vch
				&&	vch->fighting != ch
				&&	(IS_SET(vch->affected_by, AFF_CHARM) || IS_MOB(vch)) )
				{
					do_yell( vch, "Aiuto! Mi stanno attaccando!" );
				}

				spell_curse( curse_num, level, ch, (void *) vch, TARGET_CHAR );
				send_to_char( "Senti le tue forze venir lievemente meno..\n\r", vch );
				dam = dice( level, 6 );
				damage( ch, vch, dam, sn, DAM_ENERGY, TRUE );
			}
		}
		else if ( IS_NEUTRAL(ch) )
		{
			if ( !is_safe_spell(ch, vch, TRUE) )
			{
				if ( vch != ch
				&&	ch->fighting != vch
				&&	vch->fighting != ch
				&&	(IS_SET(vch->affected_by, AFF_CHARM) || IS_MOB(vch)) )
				{
					do_yell( vch, "Aiuto! Mi stanno attaccando!" );
				}

				spell_curse( curse_num, level / 2, ch, (void *) vch, TARGET_CHAR );
				send_to_char( "Senti le forse venirti lievemente meno..\n\r", vch );
				dam = dice( level, 4 );
				damage( ch, vch, dam, sn, DAM_ENERGY, TRUE );
			}
		}
	}

	send_to_char( "Senti come se ti avessero svuotato..\n\r", ch );
	gain_exp( ch, -1 * number_range(1, 15) * 5 );
	ch->point[MOVE] /= ( 4 / 3 );
	ch->point[LIFE] /= ( 4 / 3 );
}


void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	char		buf[MSL];
	AFFECT_DATA *paf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Oggetto '%s' di tipo %s, extra %s.\n\rPeso %d, valore %d.\n\r",
		obj->name,
		item_type_name(obj),
		extra_bit_name(obj->extra_flags),
		obj->weight / 10,
		obj->cost );
	send_to_char( buf, ch );

	if ( obj->pIndexData->limit != -1 && IS_ADMIN(ch))
	{
		sprintf( buf, "Esistono solo %d esemplari di questo oggetto!\n\r", obj->pIndexData->limit );
		send_to_char( buf, ch );
	}

	switch ( obj->item_type )
	{
	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
		if ( IS_ADMIN(ch) )
			sprintf( buf, "Livello %d dell'incantesimo: ", obj->value[0] );
		else
			sprintf( buf, "Incantesimo: " );
		send_to_char( buf, ch );

		if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
		{
			send_to_char( "'", ch );
			send_to_char( skill_table[obj->value[1]].name, ch );
			send_to_char( "'", ch );
		}

		if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
		{
			send_to_char( "'", ch );
			send_to_char( skill_table[obj->value[2]].name, ch );
			send_to_char( "'", ch );
		}

		if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
		{
			send_to_char( "'", ch );
			send_to_char( skill_table[obj->value[3]].name, ch );
			send_to_char( "'", ch );
		}

		if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL )
		{
			send_to_char( "'", ch );
			send_to_char( skill_table[obj->value[4]].name, ch );
			send_to_char( "'", ch );
		}

		send_to_char( ".\n\r", ch );
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		sprintf( buf, "Ha %d cariche di livello %d", obj->value[2], obj->value[0] );
		send_to_char( buf, ch );

		if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[obj->value[3]].name, ch );
			send_to_char( "'", ch );
		}

		send_to_char( ".\n\r", ch );
		break;

	case ITEM_DRINK_CON:
		sprintf( buf, "Contiene %s di colore %s.\n\r",
			liq_table[obj->value[2]].liq_name, liq_table[obj->value[2]].liq_color );
		send_to_char( buf, ch );
		break;

	case ITEM_CONTAINER:
		sprintf( buf, "Capacita': %d#  Massimo peso trasportabile: %d#  flags: %s\n\r",
			obj->value[0], obj->value[3], cont_bit_name(obj->value[1]) );
		send_to_char( buf, ch );
		if ( obj->value[4] != 100 )
		{
			sprintf( buf, "Moltiplicatore del peso: %d%%\n\r", obj->value[4] );
			send_to_char( buf, ch );
		}
		break;

	case ITEM_WEAPON:
		send_to_char( "Tipo dell'arma ", ch );
		switch ( obj->value[0] )
		{
		case ( WEAPON_EXOTIC ):		send_to_char( "esotica.\n\r", ch );			break;
		case ( WEAPON_SWORD ):		send_to_char( "spada.\n\r", ch );			break;
		case ( WEAPON_DAGGER ):		send_to_char( "daga.\n\r", ch );			break;
		case ( WEAPON_SPEAR ):		send_to_char( "giavellotto.\n\r", ch );		break;
		case ( WEAPON_MACE ):		send_to_char( "mazza/clava.\n\r", ch );		break;
		case ( WEAPON_AXE ):		send_to_char( "ascia.\n\r", ch );			break;
		case ( WEAPON_FLAIL ):		send_to_char( "correggia.\n\r", ch );		break;
		case ( WEAPON_WHIP ):		send_to_char( "frusta.\n\r", ch );			break;
		case ( WEAPON_POLEARM ):	send_to_char( "alabarda.\n\r", ch );		break;
		case ( WEAPON_BOW ):		send_to_char( "arco.\n\r", ch );			break;
		case ( WEAPON_ARROW ):		send_to_char( "freccia.\n\r", ch );			break;
		case ( WEAPON_LANCE ):		send_to_char( "lancia.\n\r", ch );			break;
		default:					send_to_char( "sconosciuto.\n\r", ch );		break;
		}

		if ( obj->pIndexData->new_format )
		{
			sprintf( buf, "Il danno corrisponde a %dd%d (media %d).\n\r",
				obj->value[1], obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2 );
		}
		else
		{
			sprintf( buf, "Danno %d a %d (media %d).\n\r",
				obj->value[1], obj->value[2], (obj->value[1] + obj->value[2]) / 2 );
		}

		send_to_char( buf, ch );
		if ( obj->value[4] )	/* weapon flags */
		{
			sprintf( buf, "Flags dell'arma: %s\n\r", weapon_bit_name(obj->value[4]) );
			send_to_char( buf, ch );
		}
		break;

	case ITEM_ARMOR:
		sprintf( buf, "Bonus sull'armatura %d pugnalata, %d contusioni, %d fendenti, and %d armi di tipo esotico.\n\r",
			obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
		send_to_char( buf, ch );
		break;
	}

	if ( !obj->enchanted )
	{
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location != APPLY_NONE && paf->modifier != 0 )
			{
				sprintf( buf, "Sotto l'effetto di %s da %d.\n\r", affect_loc_name(paf->location), paf->modifier );
				send_to_char( buf, ch );
				if ( paf->bitvector )
				{
					switch ( paf->where )
					{
					case TO_AFFECTS:
						sprintf( buf, "Aggiunge effetto %s.\n", affect_bit_name(paf->bitvector) );
						break;

					case TO_OBJECT:
						sprintf( buf, "Aggiunge %s flag all'oggetto.\n", extra_bit_name(paf->bitvector) );
						break;

					case TO_IMMUNE:
						sprintf( buf, "Aggiunge immunita' a %s.\n", imm_bit_name(paf->bitvector) );
						break;

					case TO_RESIST:
						sprintf( buf, "Aggiunge resistenza a %s.\n\r", imm_bit_name(paf->bitvector) );
						break;

					case TO_VULN:
						sprintf( buf, "Aggiunge vulnerabilita' a %s.\n\r", imm_bit_name(paf->bitvector) );
						break;

					case TO_DETECTS:
						sprintf( buf, "Aggiunge capacita' di distinguere il %s.\n\r", detect_bit_name(paf->bitvector) );
						break;

					default:
						sprintf( buf, "sconosciuto potere: %d: %d\n\r", paf->where, paf->bitvector );
						break;
					}

					send_to_char( buf, ch );
				}
			}
		}
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->location != APPLY_NONE && paf->modifier != 0 )
		{
			sprintf( buf, "Aggiunge %s da %d", affect_loc_name(paf->location), paf->modifier );
			send_to_char( buf, ch );
			if ( paf->duration > -1 )
				sprintf( buf, ", %d ore.\n\r", paf->duration );
			else
				sprintf( buf, ".\n\r" );
			send_to_char( buf, ch );

			if ( paf->bitvector )
			{
				switch ( paf->where )
				{
				case TO_AFFECTS:
					sprintf( buf, "Conferisce %s effetto.\n", affect_bit_name(paf->bitvector) );
					break;

				case TO_OBJECT:
					sprintf( buf, "Conferisce %s flag all'oggetto.\n", extra_bit_name(paf->bitvector) );
					break;

				case TO_WEAPON:
					sprintf( buf, "Aggiunge %s flag all'arma.\n", weapon_bit_name(paf->bitvector) );
					break;

				case TO_IMMUNE:
					sprintf( buf, "Conferisce immunita' a %s.\n", imm_bit_name(paf->bitvector) );
					break;

				case TO_RESIST:
					sprintf( buf, "Conferisce resistenza a %s.\n\r", imm_bit_name(paf->bitvector) );
					break;

				case TO_VULN:
					sprintf( buf, "Conferisce vulnerabilita' a %s.\n\r", imm_bit_name(paf->bitvector) );
					break;

				case TO_DETECTS:
					sprintf( buf, "Aggiunge la capacita' di distinguere %s.\n\r", detect_bit_name(paf->bitvector) );
					break;

				default:
					sprintf( buf, "potere sconosciuto %d: %d\n\r", paf->where, paf->bitvector );
					break;
				}

				send_to_char( buf, ch );
			}
		}
	}
}


void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_INFRARED) )
	{
		if ( victim == ch )
			send_to_char( "I tuoi occhi sono gia' in grado di ingannare l'oscurita'.\n\r", ch );
		else
			act( "$N e' gia' in grado di vedere nell'oscurita'.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	act( "$n riesce ora a penetrare l'oscurita' con il suo sguardo.", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 2 * level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_INFRARED;
	affect_to_char( victim, &af );
	send_to_char( "I tuoi occhi ora riescono a penetrare facilmente l'oscurita'.\n\r", victim );
}


void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~*/

	/* object invisibility */
	if ( target == TARGET_OBJ )
	{
		obj = (OBJ_DATA *) vo;

		if ( IS_OBJ_STAT(obj, ITEM_INVIS) )
		{
			act( "$p e' gia' invisibile.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		af.where = TO_OBJECT;
		af.type = sn;
		af.level = level;
		af.duration = level / 4 + 12;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = ITEM_INVIS;
		affect_to_obj( obj, &af );

		act( "$p sparisce dalla vista.", ch, obj, NULL, TO_ALL, POS_REST );
		return;
	}

	/* character invisibility */
	victim = (CHAR_DATA *) vo;

	if ( IS_AFFECTED(victim, AFF_INVISIBLE) ) return;

	act( "$n svanisce improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( level / 8 + 10 );
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char( victim, &af );
	send_to_char( "Svanisci dal piano materiale della vista.\n\r", victim );
}


void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	char		*msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_GOOD(victim) )
		msg = "$N ha un'aura giusta e dorata.";
	else if ( IS_NEUTRAL(victim) )
		msg = "$N sembra non avere nessuna aura.";
	else
		msg = "$N ha un'aura rossa e malvagia.";

	act( msg, ch, NULL, victim, TO_CHAR, POS_REST );

	if ( !IS_MOB(victim) )
	{
		if ( victim->ethos == 1 )
			msg = "$N e' dalla parte della legge.";
		else if ( victim->ethos == 2 )
			msg = "$N ha un comportamento ambivalente nei confronti della societa'.";
		else if ( victim->ethos == 3 )
			msg = "$N sembra non curarsi affatto della legge.";
		else
			msg = "$N non ha idea di che cosa siano le leggi.";
		act( msg, ch, NULL, victim, TO_CHAR, POS_REST );
	}
}


/*
 * (TT) mi viene il dubbio di non aver inteso bene il seguente.. da provare
 */
void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, ADET_GROUNDING) )
	{
		send_to_char( "Inaspettatamente l'energia da te richiamata si disperde in innocue scintille multicolore ai tuoi piedi..\n\r", victim );
		act( "Improvvisamente ai piedi di $N scoppiettano migliaia di scintille multicolore..",
			ch, NULL, victim, TO_ROOM, POS_REST );
		return;
	}

	dam = dice( level, 4 ) + 12;
	if ( saves_spell(level, victim, DAM_LIGHTNING) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
}


void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char		buf[MIL];
	BUFFER		*buffer;
	OBJ_DATA	*obj;
	OBJ_DATA	*in_obj;
	bool		found;
	int			number = 0, max_found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !strcmp(target_name, "") )
	{
		send_to_char( "Cosa vorresti trovare?\n\r", ch );
		return;
	}

	found = FALSE;
	number = 0;
	max_found = IS_ADMIN( ch ) ? 200 : 2 * level;

	buffer = new_buf();

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( !can_see_obj(ch, obj)
		||	!is_name(target_name, obj->name)
		||	IS_OBJ_STAT(obj, ITEM_NOLOCATE)
		||	number_percent() > 2 * level
		||	ch->level < obj->level )
			continue;

		found = TRUE;
		number++;

		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
			;

		if ( in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by) )
			sprintf( buf, "Un oggetto di questo tipo e' posseduto da %s\n\r", PERS(in_obj->carried_by, ch) );
		else
		{
			if ( IS_ADMIN(ch) && in_obj->in_room != NULL )
			{
				sprintf( buf, "un oggetto di questo tipo e' in %s [Room %d]\n\r",
					in_obj->in_room->name, in_obj->in_room->vnum );
			}
			else
			{
				sprintf( buf, "un oggetto di questo tipo e' in %s\n\r",
					in_obj->in_room == NULL ? "da qualche parte.." : in_obj->in_room->name );
			}
		}

		buf[0] = UPPER( buf[0] );
		add_buf( buffer, buf );

		if ( number >= max_found )
			break;
	}

	if ( !found )
		send_to_char( "Non riesci a trovare nulla del genere in cielo o in terra..\n\r", ch );
	else
		page_to_char( buf_string(buffer), ch );

	free_buf( buffer );
}


void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA			*victim = (CHAR_DATA *) vo;
	static const sh_int dam_each[] =
	{
		0,		4,		4,		5,		5,		6,		6,		6,		6,		6,
		6,		7,		7,		7,		7,		7,		8,		8,		8,		8,
		8,		9,		9,		9,		9,		9,		10,		10,		10,		10,
		10,		11,		11,		11,		11,		11,		12,		12,		12,		12,
		12,		13,		13,		13,		13,		13,		14,		14,		14,		14,
		14
	};
	int					dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, gsn_shield) )
	{
		if ( ch->level > 4 )
		{
			act( "Dal palmo della tua mano si liberano dei dardi incantati che vanno a infrangersi su $N non arrecandogli alcun danno.",
				ch, NULL, victim, TO_CHAR, POS_REST );
			act( "La tua protezione magica blocca i dardi incantati di $N.", victim, NULL, ch, TO_CHAR, POS_REST );
		}
		else
		{
			act( "Dal palmo della tua mano si liberano dei dardi incantati che vanno a infrangersi su $N non arrecandogli alcun danno.",
				ch, NULL, victim, TO_CHAR, POS_REST );
			act( "La tua protezione magica blocca i dardi incantati di $N.", victim, NULL, ch, TO_CHAR, POS_REST );
		}

		return;
	}

	level = UMIN( level, sizeof(dam_each) / sizeof(dam_each[0]) - 1 );
	level = UMAX( 0, level );
	if ( ch->level > 50 )
		dam = level / 4;
	else
		dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );

	if ( saves_spell(level, victim, DAM_ENERGY) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
	if ( ch->level > 4 )
	{
		dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );
		if ( saves_spell(level, victim, DAM_ENERGY) )
			dam /= 2;
		damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
	}

	if ( ch->level > 8 )
	{
		dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );
		if ( saves_spell(level, victim, DAM_ENERGY) )
			dam /= 2;
		damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
	}

	if ( ch->level > 12 )
	{
		dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );
		if ( saves_spell(level, victim, DAM_ENERGY) )
			dam /= 2;
		damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
	}

	if ( ch->level > 16 )
	{
		dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );
		if ( saves_spell(level, victim, DAM_ENERGY) )
			dam /= 2;
		damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
	}
}


void spell_mass_healing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	int			heal_num, refresh_num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	heal_num = skill_lookup( "heal" );
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


void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA	*gch;
	/*~~~~~~~~~~~~~*/

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group(gch, ch) || IS_AFFECTED(gch, AFF_INVISIBLE) )
			continue;
		act( "$n lentamente si avvolge d'un velo di invisibilita'.", gch, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Un velo magico si posa su di te rendendoti invisibile.\n\r", gch );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level / 2;
		af.duration = 24;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_INVISIBLE;
		affect_to_char( gch, &af );
	}

	send_to_char( "Incantesimo riuscito.\n\r", ch );
}


void spell_null( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	send_to_char( "Non esiste alcun incantesimo con questo nome.. ma dove l'hai letto?\n\r", ch );
}


void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' in grado di passare attraverso i corpi solidi.\n\r", ch );
		else
			act( "$N e' gia' in grado di passare attraverso i corpi solidi.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = number_fuzzy( level / 4 );
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_PASS_DOOR;
	affect_to_char( victim, &af );
	act( "$n diventa translucente.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Diventi translucente, sei ora in grado di attraversare i corpi solidi.\n\r", victim );
}


/*
 * RT plague spell, very nasty
 */
void spell_plague( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( saves_spell(level, victim, DAM_DISEASE) || (IS_MOB(victim) && IS_SET(victim->act, ACT_UNDEAD)) )
	{
		if ( ch == victim )
			send_to_char( "Per un attimo avverti la pestilenza sfiorare il tuo corpo.. ma passa oltre.\n\r", ch );
		else
			act( "Il morbo non riesce ad infettare $N!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level * 3 / 4;
	af.duration = ( 10 + level / 10 );
	af.location = APPLY_STR;
	af.modifier = -1 * UMAX( 1, 3 + level / 15 );
	af.bitvector = AFF_PLAGUE;
	affect_join( victim, &af );

	send_to_char( "Urli mentre senti che la pestilenza contagia il tuo corpo rovinandoti..\n\r", victim );
	act( "$n agonizza mentre il morbo divora il suo corpo.", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~*/

	if ( target == TARGET_OBJ )
	{
		obj = (OBJ_DATA *) vo;

		if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON )
		{
			if ( IS_OBJ_STAT(obj, ITEM_BLESS) || IS_OBJ_STAT(obj, ITEM_BURN_PROOF) )
			{
				act( "Non riesci ad avvelenare $p.", ch, obj, NULL, TO_CHAR, POS_REST );
				return;
			}

			obj->value[3] = 1;
			act( "$p emette malefici vapori.", ch, obj, NULL, TO_ALL, POS_REST );
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
				act( "Non sembra un oggetto che si lasci avvelenare facilmente..", ch, obj, NULL, TO_CHAR, POS_REST );
				return;
			}

			if ( IS_WEAPON_STAT(obj, WEAPON_POISON) )
			{
				act( "$p e' gia' forte del veleno.", ch, obj, NULL, TO_CHAR, POS_REST );
				return;
			}

			af.where = TO_WEAPON;
			af.type = sn;
			af.level = level / 2;
			af.duration = level / 8;
			af.location = 0;
			af.modifier = 0;
			af.bitvector = WEAPON_POISON;
			affect_to_obj( obj, &af );

			act( "$p reca ora con se' l'influsso malefico del veleno.", ch, obj, NULL, TO_ALL, POS_REST );
			return;
		}

		act( "$p non si puo' avvelenare .", ch, obj, NULL, TO_CHAR, POS_REST );
		return;
	}

	victim = (CHAR_DATA *) vo;

	if ( saves_spell(level, victim, DAM_POISON) )
	{
		act( "Il veleno colpisce $n ma non va oltre le sue difese.", victim, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Senti il malefico influsso del veleno scorrerti per un attimo nelle vene.. fortunatamente riesci a reagire.\n\r", victim );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 10 + level / 10 );
	af.location = APPLY_STR;
	af.modifier = -2;
	af.bitvector = AFF_POISON;
	affect_join( victim, &af );
	send_to_char( "Il veleno ti infetta scorrendoti velocemente nelle vene..\n\r", victim );
	act( "Il veleno penetra le vene $n che comincia subito a soffrirne.", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_protection_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL) || IS_AFFECTED(victim, AFF_PROTECT_GOOD) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' la tua aura di protezione contro il male.\n\r", ch );
		else
			act( "$N ha gia' un'aura di protezione contro il male.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 10 + level / 5 );
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1;
	af.bitvector = AFF_PROTECT_EVIL;
	affect_to_char( victim, &af );
	send_to_char( "Un'aura di protezione contro il male ti avvolge.\n\r", victim );
	if ( ch != victim )
		act( "Un'aura di protezione benevola avvolge $N.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_protection_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_PROTECT_GOOD) || IS_AFFECTED(victim, AFF_PROTECT_EVIL) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' protetto contro il bene.\n\r", ch );
		else
			act( "$N ha gia' un'aura protettiva contro il bene.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 10 + level / 5 );
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1;
	af.bitvector = AFF_PROTECT_GOOD;
	affect_to_char( victim, &af );
	send_to_char( "La malevola oscurita' ti avvolge proteggendoti dal bene.\n\r", victim );
	if ( ch != victim )
		act( "Un'aura oscura e maligna avvolge $N.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_ray_of_truth( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam, align;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if ( IS_EVIL(ch) )
	{
		victim = ch;
		act( "$n richiama le energie positive provocando una forte implosione nel tuo corpo!!",
			ch, NULL, victim, TO_VICT, POS_REST );
	}

	if ( victim != ch )
	{
		act( "$n alza le braccia al cielo richiamandone un violento raggio di luce!", ch, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Alzi le braccia al cielo richiamando un violento raggio di luce!\n\r", ch );
	}


	if ( IS_GOOD(victim) )
	{
		act( "$n non sembra soffrire i danni arrecati dalle energie positive.", victim, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "La luce ti arride non ferendoti minimamente.\n\r", victim );
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
	spell_blindness( gsn_blindness, 3 * level / 4, ch, (void *) victim, TARGET_CHAR );
}


void spell_recharge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	int			chance, percent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF )
	{
		send_to_char( "Questo oggetto e' gia' privo di cariche.\n\r", ch );
		return;
	}

	if ( obj->value[3] >= 3 * level / 2 )
	{
		send_to_char( "Non hai abbastanza esperienza per ricaricare questo oggetto.\n\r", ch );
		return;
	}

	if ( obj->value[1] == 0 )
	{
		send_to_char( "Questo oggetto e' gia' stato ricaricato una volta.\n\r", ch );
		return;
	}

	if ( obj->value[2] == obj->value[1] )
	{
		send_to_char("Quell'oggetto e' gia' caricato al massimo possibile.\n\r",ch);
		return;
	}

	chance = 40 + 2 * level;

	chance -= obj->value[3];	/* harder to do high-level spells */
	chance -= ( obj->value[1] - obj->value[2] ) * ( obj->value[1] - obj->value[2] );

	chance = UMAX( level / 2, chance );

	percent = number_percent();

	if ( percent < chance / 2 )
	{
		act( "$p risplende di una fioca luce.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende di una fioca luce.", ch, obj, NULL, TO_ROOM, POS_REST );
		obj->value[2] = UMAX( obj->value[1], obj->value[2] );
		obj->value[1] = 0;
		return;
	}
	else if ( percent <= chance )
	{
		int chargeback, chargemax;
		/*~~~~~~~~~~~~~~~~~~~~~~*/

		act( "$p risplende debolmente.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende debolmente.", ch, obj, NULL, TO_CHAR, POS_REST );

		chargemax = obj->value[1] - obj->value[2];

		if ( chargemax > 0 )
			chargeback = UMAX( 1, chargemax * percent / 100 );
		else
			chargeback = 0;

		obj->value[2] += chargeback;
		obj->value[1] = 0;
		return;
	}
	else if ( percent <= UMIN(95, 3 * chance / 2) )
	{
		send_to_char( "L'incantesimo non sembra aver sortito alcun effetto.\n\r", ch );
		if ( obj->value[1] > 1 )
			obj->value[1]--;
		return;
	}
	else						/* whoops! */
	{
		act( "$p risplende con intermittenza.. ma crepita e si distrugge!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende con intermittenza.. ma crepita e si distrugge!", ch, obj, NULL, TO_ROOM, POS_REST );
		extract_obj( obj );
	}
}


void spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	victim->point[MOVE] = UMIN( victim->point[MOVE] + level, victim->max_point[MOVE] );
	if ( victim->max_point[MOVE] == victim->point[MOVE] )
		send_to_char( "La stanchezza ti abbandona completamente!\n\r", victim );
	else
		send_to_char( "Nuove forze ti rinvigoriscono scacciando in parte la stanchezza.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo e' andato a buon fine.\n\r", ch );
}


void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	bool		found = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* do object cases first */
	if ( target == TARGET_OBJ )
	{
		obj = (OBJ_DATA *) vo;

		if ( IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_NOREMOVE) )
		{
			if ( !IS_OBJ_STAT(obj, ITEM_NOUNCURSE) && !saves_dispel(level + 2, obj->level, 0) )
			{
				REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
				REMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
				act( "$p s'illumina di un'aura blu.", ch, obj, NULL, TO_ALL, POS_REST );
				return;
			}

			act( "La maledizione che pesa su $p va oltre il tuo potere.. riprova!", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}
		else
		{
			send_to_char( "Non riesci a varcare le difese dell'oggetto..\n\r", ch );
			return;
		}
	}

	/* characters */
	victim = (CHAR_DATA *) vo;

	if ( check_dispel(level, victim, gsn_curse) )
	{
		send_to_char( "Il tuo cammino appare ora piu' sereno.\n\r", victim );
		act( "Un'aura di serenita' torna ora su $n.", victim, NULL, NULL, TO_ROOM, POS_REST );
	}

	for ( obj = victim->carrying; (obj != NULL && !found); obj = obj->next_content )
	{
		if ( (IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_NOREMOVE)) && !IS_OBJ_STAT(obj, ITEM_NOUNCURSE) )
		{	/* attempt to remove curse */
			if ( !saves_dispel(level, obj->level, 0) )
			{
				found = TRUE;
				REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
				REMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
				act( "$p risplende di una opaca aura blu.", victim, obj, NULL, TO_CHAR, POS_REST );
				act( "$p di $n risplende di una opaca aura blu.", victim, obj, NULL, TO_ROOM, POS_REST );
			}
		}
	}
}


void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	{
		if ( victim == ch )
			send_to_char( "Hai gia' la protezione di questo incantesimo.\n\r", ch );
		else
			act( "$N ha gia' la protezione di questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = level / 6;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_SANCTUARY;
	affect_to_char( victim, &af );
	act( "Una forte e sacra aura bianca protettiva circonda $n.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Una forte e sacra aura bianca protettiva ti circonda.\n\r", victim );
}


void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
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
	af.duration = ( 8 + level / 3 );
	af.location = APPLY_AC;
	af.modifier = -1 * UMAX( 20, 10 + level / 3 );	/* af.modifier = -20; */
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "Uno scudo di forza magica circonda e protegge $n.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Uno scudo di forza magica ti circonda proteggendoti.\n\r", victim );
}


void spell_shocking_grasp( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA			*victim = (CHAR_DATA *) vo;
	static const int	dam_each[] =
	{
		6,		8,		10,		12,		14,		16,		18,		20,		25,		29,
		33,		36,		39,		39,		39,		40,		40,		41,		41,		42,
		42,		43,		43,		44,		44,		45,		45,		46,		46,		47,
		47,		48,		48,		49,		49,		50,		50,		51,		51,		52,
		52,		53,		53,		54,		54,		55,		55,		56,		56,		57,
		57
	};
	int					dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	level = UMIN( level, sizeof(dam_each) / sizeof(dam_each[0]) - 1 );
	level = UMAX( 0, level );
	if ( ch->level > 50 )
		dam = level / 2;
	else
		dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );
	if ( saves_spell(level, victim, DAM_LIGHTNING) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
}


void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_SLEEP)
	||	(IS_MOB(victim) && IS_SET(victim->act, ACT_UNDEAD))
	||	level < victim->level
	||	saves_spell(level - 4, victim, DAM_CHARM) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 1 + level / 10;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_SLEEP;
	affect_join( victim, &af );

	if ( IS_AWAKE(victim) )
	{
		send_to_char( "Improvvisamente il sonno ti coglie e non riesci piu' a rimanere in piedi con gli occhi aperti.. zzzzzz..\n\r",
			victim );
		act( "$n si addormenta improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		victim->position = POS_SLEEP;
	}
}


void spell_slow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) || IS_AFFECTED(victim, AFF_SLOW) )
	{
		if ( victim == ch )
			send_to_char( "Non hai decisamente bisogno di muoverti piu' lentamente!\n\r", ch );
		else
			act( "$N si muove gia' abbastanza lentamente.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( saves_spell(level, victim, DAM_OTHER) || IS_SET(victim->imm_flags, IMM_MAGIC) )
	{
		if ( victim != ch )
			send_to_char( "L'incantesimo non sembra aver sortito alcun effetto.\n\r", ch );
		send_to_char( "Un tiepido torpore ti attraversa.. ma svanisce subito.\n\r", victim );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_HASTE) )
	{
		if ( !check_dispel(level, victim, skill_lookup("haste")) )
		{
			if ( victim != ch )
				send_to_char( "L'incantesimo non ha sortito alcun effetto.\n\r", ch );
			send_to_char( "Un freddo torpore ti attraversa.. ma svanisce subito.\n\r", victim );
			return;
		}

		act( "$n comincia a muoversi meno rapidamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 4 + level / 12 );
	af.location = APPLY_DEX;
	af.modifier = -UMAX( 2, level / 12 );
	af.bitvector = AFF_SLOW;
	affect_to_char( victim, &af );
	send_to_char( "Perdi coordinazione nei movimenti e cominci a muoverti piu' lentamente..\n\r", victim );
	act( "$n sembra perdere coordinazione ed inizia a muoversi piu' lentamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' sotto l'effetto di questa protezione.\n\r", ch );
		else
			act( "$N e' gia' sotto l'effetto di questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 10 + level / 5 );
	af.location = APPLY_AC;
	af.modifier = -1 * UMAX( 40, 20 + level / 2 );	/* af.modifier=-40; */
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "La pelle di $n diventa spessa e dura come la roccia.", victim, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "La tua pelle diventa spessa e solida come roccia.\n\r", victim );
}


void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	char		buf[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	(IS_MOB(victim) && IS_SET(victim->act, ACT_AGGRESSIVE))
	||	victim->level >= level + 3
	||	(!IS_MOB(victim) && victim->level >= LEVEL_ADMIN)
	||	victim->fighting != NULL
	||	(IS_MOB(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(IS_MOB(victim) && victim->pIndexData->pShop != NULL)
	||	(!IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||	(saves_spell(level, victim, DAM_OTHER))
	||	(ch->in_room->area != victim->in_room->area && !IS_MOB(victim)) )
	{
		send_to_char( "L'evocazione fallisce.\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Non puoi evocare.\n\r",ch );
		return;
	}

	if ( IS_MOB(victim) && victim->in_mind == NULL )
	{
		sprintf( buf, "%d", victim->in_room->vnum );
		victim->in_mind = str_dup( buf );
	}


	char_from_room( victim );
	char_to_room( victim, ch->in_room );
	act( "$n appare di fronte a te.", victim, NULL, NULL, TO_ROOM, POS_REST );
	act( "$n ti ha evocato", ch, NULL, victim, TO_VICT, POS_REST );
	do_look( victim, "auto" );
}


void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA		*victim = (CHAR_DATA *) vo;
	ROOM_INDEX_DATA *pRoomIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_MOB(ch) ) victim = ch;

	if (victim->in_room == NULL
	||	IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	||	(victim != ch && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(!IS_MOB(ch) && victim->fighting != NULL)
	||	(victim != ch && (saves_spell(level - 5, victim, DAM_OTHER))) )
	{
		send_to_char( "Fallisci il trasferimento.\n\r", ch );
		return;
	}

	pRoomIndex = get_random_room( victim );

	if ( victim != ch )
		send_to_char( "Improvvisamente ti ritrovi altrove!\n\r", victim );

	act( "$n sparisce nel nulla!", victim, NULL, NULL, TO_ROOM, POS_REST );
	char_from_room( victim );
	char_to_room( victim, pRoomIndex );
	act( "$n appare improvvisamente dal nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );
	do_look( victim, "auto" );
}


void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char		buf1[MSL];
	char		buf2[MSL];
	char		speaker[MIL];
	CHAR_DATA	*vch;
	bool found = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	target_name = one_argument( target_name, speaker );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( is_name(speaker, vch->name) )
			found = TRUE;
    }

	if ( !found )
	{
		sprintf( buf1, "%s non c'e' qui.\n\r", speaker );
		send_to_char( buf1, ch );
		return;
	}

	sprintf( buf1, "%s dice '%s'.\n\r", speaker, target_name );
	sprintf( buf2, "Sembra che %s stia dicendo '%s'.\n\r", speaker, target_name );
	buf1[0] = UPPER( buf1[0] );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( !is_name(speaker, vch->name) )
			send_to_char( saves_spell(level, vch, DAM_OTHER) ? buf2 : buf1, vch );
	}
}


void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) || saves_spell(level, victim, DAM_OTHER) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 4 + level / 12 );
	af.location = APPLY_STR;
	af.modifier = -1 * ( 2 + level / 12 );
	af.bitvector = AFF_WEAKEN;
	affect_to_char( victim, &af );
	send_to_char( "Senti le tue forze scivolare via..\n\r", victim );
	act( "Sembra che le forze abbiano abbandonato $n proprio ora.", victim, NULL, NULL, TO_ROOM, POS_REST );
}


/*
 * RT recall spell is back
 */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA		*victim = (CHAR_DATA *) vo;
	ROOM_INDEX_DATA *location;
	int				to_room_vnum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (ch->class == CLASS_SAMURAI) && (ch->fighting) && (victim == NULL) )
	{
		send_to_char( "Sei un samurai!Combatti fino alla fine, dopo tornerai a casa!.\n\r", ch );
		return;
	}

	if ( victim != NULL )
	{
		if ( (victim->fighting) && (victim->class == CLASS_SAMURAI) )
		{
			send_to_char( "Non puoi eseguire questo incantesimo su di un samurai!.\n\r", ch );
			return;
		}
	}

	if ( IS_MOB(victim) )
		return;
	to_room_vnum = hometown_table[victim->hometown].recall[IS_GOOD( victim ) ? 0 : IS_NEUTRAL( victim ) ? 1 : IS_EVIL( victim ) ? 2 : 1];

	if ( (location = get_room_index(to_room_vnum)) == NULL )
	{
		send_to_char( "Hai completamente smarrito la via..\n\r", victim );
		return;
	}

/*	if (victim->desc != NULL && (current_time - victim->last_fight_time) < FIGHT_DELAY_TIME)
	{
		send_to_char("Hai troppa adrenalina nelle vene per pregare ora.\n\r",victim);
		return;
	}
*/

	if ( IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	||	 IS_AFFECTED(victim, AFF_CURSE)
	||	 IS_RAFFECTED(victim->in_room, AFF_ROOM_CURSE) )
	{
		send_to_char( "L'incantesimo ha fallito.\n\r", victim );
		return;
	}

	if ( victim->fighting != NULL )
	{
		if ( victim == ch )
			gain_exp( victim, -(victim->level*2) );

		stop_fighting( victim, TRUE );
	}

	ch->point[MOVE] /= 4;
	act( "$n scompare davanti ai tuoi occhi.", victim, NULL, NULL, TO_ROOM, POS_REST );
	char_from_room( victim );
	char_to_room( victim, location );
	do_visible( ch, "" );
	act( "$n appare dal nulla.", victim, NULL, NULL, TO_ROOM, POS_REST );
	do_look( victim, "auto" );

	if ( victim->pet != NULL )
	{
		char_from_room( victim->pet );
		char_to_room( victim->pet, location );
		do_look( victim->pet, "auto" );
	}
}


/*
 * Draconian spells.
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam, hp_dam, dice_dam, hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n sputa acido verso $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n sputa un tremendo getto acido contro di te.", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Sputi un tremendo getto acido contro $N.", ch, NULL, victim, TO_CHAR, POS_REST );

	hpch = UMAX( 12, ch->point[LIFE] );
	hp_dam = number_range( hpch / 11 + 1, hpch / 6 );
	dice_dam = dice( level, 16 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );

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
}


void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	CHAR_DATA	*vch, *vch_next;
	int			dam, hp_dam, dice_dam;
	int			hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n soffia un potente cono di fuoco!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n ti soffia contro un potente cono di fuoco!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Soffi un potente cono di fuoco!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 10, ch->point[LIFE] );
	hp_dam = number_range( hpch / 9 + 1, hpch / 5 );
	dice_dam = dice( level, 20 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );
	fire_effect( victim->in_room, level, dam / 2, TARGET_ROOM );

	for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if (is_safe_spell(ch, vch, TRUE)
		||	(IS_MOB(vch) && IS_MOB(ch) && (ch->fighting != vch /* || vch->fighting != ch */ )) )
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
}


void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	CHAR_DATA	*vch, *vch_next;
	int			dam, hp_dam, dice_dam, hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n soffia un imponente cono di ghiaccio gelido!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n soffia un imponente cono di ghiaccio gelido contro di te!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Soffi un imponente cono di ghiaccio gelido!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 12, ch->point[LIFE] );
	hp_dam = number_range( hpch / 11 + 1, hpch / 6 );
	dice_dam = dice( level, 16 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );
	cold_effect( victim->in_room, level, dam / 2, TARGET_ROOM );

	for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( is_safe_spell(ch, vch, TRUE)
		||	(IS_MOB(vch) && IS_MOB(ch) && (ch->fighting != vch /* || vch->fighting != ch */ )) )
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
}


void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	int			dam, hp_dam, dice_dam, hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n soffia una nube di gas velenoso!", ch, NULL, NULL, TO_ROOM, POS_REST );
	act( "Soffi una potente nube di gas velenoso!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 16, ch->point[LIFE] );
	hp_dam = number_range( hpch / 15 + 1, 8 );
	dice_dam = dice( level, 12 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );
	poison_effect( ch->in_room, level, dam, TARGET_ROOM );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if (is_safe_spell(ch, vch, TRUE)
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
}


void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam, hp_dam, dice_dam, hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n scaglia una scarica fulminante contro $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n ti scaglia addosso una scarica fulminante!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Scagli addosso a $N una scarica fulminante!", ch, NULL, victim, TO_CHAR, POS_REST );

	hpch = UMAX( 10, ch->point[LIFE] );
	hp_dam = number_range( hpch / 9 + 1, hpch / 5 );
	dice_dam = dice( level, 20 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );

	/* Xandra (aml prob): stesso discorso di prima, da controllare */
	if ( CAN_DETECT(victim, ADET_GROUNDING) )
	{
		send_to_char( "L'elettricita' scoppietta ai piedi dei tuoi avversari.\n\r", victim );
		act( "La scarica elettrica scoppietta innocua sul terreno.", ch, NULL, victim, TO_ROOM, POS_REST );
		return;
	}

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
}


/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = number_range( 25, 100 );
	if ( saves_spell(level, victim, DAM_PIERCE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_PIERCE, TRUE );
}


void spell_high_explosive( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = number_range( 30, 120 );
	if ( saves_spell(level, victim, DAM_PIERCE) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_PIERCE, TRUE );
}


void spell_lightning_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af, af2;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "Questa stanza e' gia' pervasa da un incantesimo simile.\n\r", ch );
		return;
	}

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai gia' usato troppo di recente questo incantesimo per lanciarlo nuovamente.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 40;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_L_SHIELD;
	affect_to_room( ch->in_room, &af );

	af2.where = TO_AFFECTS;
	af2.type = sn;
	af2.level = ch->level;
	af2.duration = level / 10;
	af2.modifier = 0;
	af2.location = APPLY_NONE;
	af2.bitvector = 0;
	affect_to_char( ch, &af2 );

	ch->in_room->owner = str_dup( ch->name );
	send_to_char( "Lievi fulmini magici attraversano la stanza creando un campo di forza.\n\r", ch );
	act( "Lievi fulmini magici attraversano la stanza creando un campo di forza.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_shocking_trap( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af, af2;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_affected_room(ch->in_room, sn) )
	{
		send_to_char( "Vi sono gia' delle trappole simili nella stanza.\n\r", ch );
		return;
	}

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Hai gia' usato troppo di recente questo incantesimo per lanciarlo nuovamente.\n\r", ch );
		return;
	}

	af.where = TO_ROOM_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = level / 40;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_ROOM_SHOCKING;
	affect_to_room( ch->in_room, &af );

	af2.where = TO_AFFECTS;
	af2.type = sn;
	af2.level = level;
	af2.duration = ch->level / 10;
	af2.modifier = 0;
	af2.location = APPLY_NONE;
	af2.bitvector = 0;
	affect_to_char( ch, &af2 );
	send_to_char( "La stanza si riempie lentamente di onde elettriche pronte a scaricare la loro energia.\n\r", ch );
	act( "La stanza si riempie lentamente di onde elettriche pronte a scaricare la loro energia.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_acid_arrow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 12 );
	if ( saves_spell(level, victim, DAM_ACID) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_ACID, TRUE );
}


/*
 * energy spells
 */
void spell_etheral_fist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 12 );
	if ( saves_spell(level, victim, DAM_ENERGY) )
		dam /= 2;
	act( "Una mano nera si materializza dal nulla e stordisce $N con un colpo improvviso!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
}


void spell_spectral_furor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 8 );
	if ( saves_spell(level, victim, DAM_ENERGY) )
		dam /= 2;
	act( "Uno spettro diabolico assale $N con una stretta micidiale!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
}


void spell_disruption( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 9 );
	if ( saves_spell(level, victim, DAM_ENERGY) )
		dam /= 2;
	act( "Una strana e malefica energia racchiude e imprigiona $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
}


void spell_sonic_resonance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 7 );
	if ( saves_spell(level, victim, DAM_ENERGY) )
		dam /= 2;
	act( "Un cilindro di energia cinetica avvolge e assorda $N!!!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_ENERGY, TRUE );
	WAIT_STATE( victim, skill_table[sn].beats );
}


/*
 * mental
 */
void spell_mind_wrack( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 7 );
	if ( saves_spell(level, victim, DAM_MENTAL) )
		dam /= 2;
	act( "$n richiama energie negative sprigionandone il potere nella mente di $N che sembra abbandonarsi..", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_MENTAL, TRUE );
}


void spell_mind_wrench( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 9 );
	if ( saves_spell(level, victim, DAM_MENTAL) ) dam /= 2;
	act( "$n richiama energie negative sprigionandone il potere nella mente di $N che sembra impazzire!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_MENTAL, TRUE );
}


/*
 * acid
 */
void spell_sulfurus_spray( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 7 );
	if ( saves_spell(level, victim, DAM_ACID) )
		dam /= 2;
	act( "Un getto di maligni liquidi sulfurei ricopre $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_ACID, TRUE );
}


void spell_caustic_font( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 9 );
	if ( saves_spell(level, victim, DAM_ACID) )
		dam /= 2;
	act( "Un potente getto di liquido corrosivo si riversa su $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_ACID, TRUE );
}


void spell_acetum_primus( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 8 );
	if ( saves_spell(level, victim, DAM_ACID) )
		dam /= 2;
	act( "Un primitivo ma letale getto di acido avvolge $N!!!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_ACID, TRUE );
}


/*
 * Electrical
 */
void spell_galvanic_whip( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 7 );
	if ( saves_spell(level, victim, DAM_LIGHTNING) ) dam /= 2;
	act( "$n evoca una frusta di particelle ionizzate che va a colpire ferocemente  $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
}


void spell_magnetic_trust( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 8 );
	if ( saves_spell(level, victim, DAM_LIGHTNING) ) dam /= 2;
	act( "Una nascosta forma di energia ti si muove attorno prima di avvolgerti totalmente", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
}


void spell_quantum_spike( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 9 );
	if ( saves_spell(level, victim, DAM_LIGHTNING) ) dam /= 2;
	act( "$N si decompone in migliaia di cubetti.. poi si riassesta con un doloroso CLACK!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
}


/*
 * negative
 */
void spell_hand_of_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( saves_spell(level, victim, DAM_NEGATIVE) )
	{
		send_to_char( "Per un momento avverti un freddo gelido..\n\r", victim );
		return;
	}

	if ( (IS_MOB(victim) && IS_SET(victim->act, ACT_UNDEAD)) || IS_VAMPIRE(victim) )
	{
		send_to_char( "Questo incantesimo non sembra aver efficacia sul bersaglio scelto.\n\r", ch );
		return;
	}

	if ( victim->level <= 2 )
		dam = ch->point[LIFE] + 1;
	else
	{
		dam = dice( level, 10 );
		victim->point[MANA] /= 2;
		victim->point[MOVE] /= 2;
		ch->point[LIFE] += dam / 2;
	}

	send_to_char( "Senti la tua vita scivolare via dolorosamente!\n\r", victim );
	act( "Una mano invisibile strappa via la vita da $N!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE );
}


/*
 * travel via astral plains
 */
void spell_astral_walk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	bool		gate_pet;
	char		buf[512];
	/*~~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	!can_see_room(ch, victim->in_room)
	||	IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	victim->level >= level + 3	/* || (!IS_MOB(victim) && victim->level >= LEVEL_HERO) * NOT trust */
	||	saves_spell(level, victim, DAM_OTHER)
	||	(IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(!IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||	(!IS_MOB(victim) && ch->in_room->area != victim->in_room->area)
	||	(IS_MOB(victim) && saves_spell(level, victim, DAM_OTHER)) )
	{
		send_to_char( "Non riesci a giungere a destinazione..\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char("Non riesci a creare il passaggio astrale.\n\r",ch );
		return;
	}

	if ( ch->pet != NULL && ch->in_room == ch->pet->in_room )
		gate_pet = TRUE;
	else
		gate_pet = FALSE;

	act( "$n sparisce in un bagliore improvviso!", ch, NULL, NULL, TO_ROOM, POS_REST );
	sprintf( buf, "Lasci che le stelle ti guidino verso %s.\n\r", victim->name );
	send_to_char( buf, ch );
	char_from_room( ch );
	char_to_room( ch, victim->in_room );

	act( "$n appare dal vuoto in un corteo di brillanti stelle!", ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );

	if ( gate_pet )
	{
		act( "$n sparisce lasciando una scia di stelle!", ch->pet, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( buf, ch->pet );
		char_from_room( ch->pet );
		char_to_room( ch->pet, victim->in_room );
		act( "$n appare dal vuoto in un corteo di brillanti stelle!", ch->pet, NULL, NULL, TO_ROOM, POS_REST );
		do_look( ch->pet, "auto" );
	}
}


/*
 * vampire version astral walk
 */
void spell_mist_walk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	!IS_VAMPIRE(ch)
	||	victim->in_room == NULL
	||	!can_see_room(ch, victim->in_room)
	||	IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	victim->level >= level - 5	/* || (!IS_MOB(victim) && victim->level >= LEVEL_HERO) * NOT trust */
	||	saves_spell(level, victim, DAM_OTHER)
	||	(IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(!IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||	(!IS_MOB(victim) && ch->in_room->area != victim->in_room->area)
	||	(IS_MOB(victim) && saves_spell(level, victim, DAM_OTHER)) )
	{
		send_to_char( "Non riesci a tracciare bene la vittima.. riprova.\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Non riesci a sfruttare il potere delle nebbie.\n\r", ch );
		return;
	}

	act( "$n si dissolve in una nebbia mistica quindi svanisce in un lampo!", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Ti trasformi in nebbia per raggiungere la tua vittima..\n\r", ch );

	char_from_room( ch );
	char_to_room( ch, victim->in_room );

	act( "Una nebbia fitta ti investe prima di rivelare la presenza di $n!", ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );
}


/*
 * Cleric version of astra_walk
 */
void spell_solar_flight( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( time_info.hour > 18 || time_info.hour < 8 )
	{
		send_to_char( "Hai bisogno della luce del sole per lanciare questo incantesimo.\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	!can_see_room(ch, victim->in_room)
	||	IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	victim->level >= level + 1	/* || (!IS_MOB(victim) && victim->level >= LEVEL_HERO) * NOT trust */
	||	saves_spell(level, victim, DAM_OTHER)
	||	(IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(!IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||	(!IS_MOB(victim) && ch->in_room->area != victim->in_room->area)
	||	(IS_MOB(victim) && saves_spell(level, victim, DAM_OTHER)) )
	{
		send_to_char( "Non riesci a invocare l'incantesimo in maniera corretta.\n\r", ch );
		return;
	}

	if ( (room_is_dt(ch) == TRUE && !IS_ADMIN(ch))
	||	 (room_is_dt(victim) == TRUE && !IS_ADMIN(victim)) )
	{
		send_to_char( "Non riesci a sfruttare la luce solare..\n\r", ch );
		return;
	}

	act( "$n svanisce in un bagliore di luce accecante!", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Svanisci in un balgiore di luce accecante!\n\r", ch );

	char_from_room( ch );
	char_to_room( ch, victim->in_room );

	act( "Un bagliore di luce accecante ti investe rivelando al suo ritiro la presenza di $n!",
		ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );
}


/*
 * travel via astral plains
 */
void spell_helical_flow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( (victim = get_char_world(ch, target_name)) != NULL
	&&	 (victim = get_char_area(ch, target_name)) == NULL )
	{
		send_to_char( "Il tuo bersaglio si trova troppo lontano.\n\r", ch );
		return;
	}

	if ( (victim = get_char_area(ch, target_name)) == NULL
	||	victim == ch
	||	victim->in_room == NULL
	||	!can_see_room(ch, victim->in_room)
	||	IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||	IS_SET(ch->in_room->room_flags, ROOM_NOSUMMON)
	||	IS_SET(victim->in_room->room_flags, ROOM_NOSUMMON)
	||	victim->level >= level + 3	/* || (!IS_MOB(victim) && victim->level >= LEVEL_HERO) * NOT trust */
	||	saves_spell(level, victim, DAM_OTHER)
	||	(IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	||	(!IS_MOB(victim) && is_safe_nomessage(ch, victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||	(!IS_MOB(victim) && ch->in_room->area != victim->in_room->area)
	||	(IS_MOB(victim) && saves_spell(level, victim, DAM_OTHER)) )
	{
		send_to_char( "L'incantesimo non ha esito positivo..\n\r", ch );
		return;
	}

	act ( "Una colonna di mille colori porta via $n che sparisce svanendo nell'aria!", ch, NULL, NULL, TO_ROOM, POS_REST );
	send_to_char( "Ti lasci portare a destinazione da un'immensa spirale di colori!\n\r", ch );

	char_from_room( ch );
	char_to_room( ch, victim->in_room );

	act( "Una lenta spirale di colori appare rivelando la presenza di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );
}


void spell_corruption( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_AFFECTED(victim, AFF_CORRUPTION) )
	{
		act( "$N e' gia' preda di questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( saves_spell(level, victim, DAM_NEGATIVE) || (IS_MOB(victim) && IS_SET(victim->act, ACT_UNDEAD)) )
	{
		if ( ch == victim )
			send_to_char( "Per un attimo senti il tuo corpo macerare.. ma passa subito.\n\r", ch );
		else
			act( "$N non sembra subire questo incantesimo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level * 3 / 4;
	af.duration = ( 10 + level / 5 );
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_CORRUPTION;
	affect_join( victim, &af );

	send_to_char( "Urli dal dolore mentre il tuo corpo muta dolorosamente in polvere.\n\r", victim );
	act( "$n urla in agonia mentre il suo corpo muta dolorosamente in polvere!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_hurricane( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	int			dam, hp_dam, dice_dam, hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n invoca l'aiuto degli elementi Celesti evocando un tornado!", ch, NULL, NULL, TO_NOTVICT, POS_REST );
	act( "Invochi l'aiuto degli elementi Celesti evocando un tornado!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 16, ch->point[LIFE] );
	hp_dam = number_range( hpch / 15 + 1, 8 );
	dice_dam = dice( level, 12 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );

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

		if ( !IS_AFFECTED(vch, AFF_FLYING) )	dam /= 2;

		if		( vch->size == SIZE_TINY )		dam *= 1.5;
		else if ( vch->size == SIZE_SMALL )		dam *= 1.3;
		else if ( vch->size == SIZE_MEDIUM )	dam *= 1;
		else if ( vch->size == SIZE_LARGE )		dam *= 0.9;
		else if ( vch->size == SIZE_HUGE )		dam *= 0.7;
		else									dam *= 0.5;

		if ( saves_spell(level, vch, DAM_OTHER) )
			damage( ch, vch, dam / 2, sn, DAM_OTHER, TRUE );
		else
			damage( ch, vch, dam, sn, DAM_OTHER, TRUE );
	}
}


void spell_detect_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( CAN_DETECT(victim, DETECT_UNDEAD) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' in grado di distinguere i non morti.\n\r", ch );
		else
			act( "$N e' gia' in grado di distinguere i non morti.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 5 + level / 3 );
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = DETECT_UNDEAD;
	affect_to_char( victim, &af );
	send_to_char( "I tuoi occhi sono ora in grado di distinguere la non vita.\n\r", victim );
	if ( ch != victim )
		send_to_char( "L'incantesimo ha avuto effetto.\n\r", ch );
}

void spell_firestream( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n emette un getto di fiamme ustionanti!", ch, NULL, victim, TO_NOTVICT, POS_REST );
	act( "$n ti getta contro un'ondata di fiamme ustionanti!", ch, NULL, victim, TO_VICT, POS_REST );
	act( "Scagli un'ondata di fiamme ustionante contro $N!", ch, NULL, victim, TO_CHAR, POS_REST );

	dam = dice( level, 8 );

	if ( saves_spell(level, victim, DAM_FIRE) )
	{
		fire_effect( victim, level / 2, dam / 4, TARGET_CHAR );
		damage( ch, victim, dam / 2, sn, DAM_FIRE, TRUE );
	}
	else
	{
		fire_effect( victim, level, dam, TARGET_CHAR );
		damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
	}
}


void spell_summon_earth_elm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*elm;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per richiamare ancora un elementale della terra.\n\r", ch );
		return;
	}

	send_to_char( "Cerchi di plasmare un elementale della terra richiamandone il soffio vitale.\n\r", ch );
	act( "$n cerca di plasmare un elementale della terra richiamandone il soffio vitale.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_ELM_EARTH) )
		{
			i++;
			if ( i > 2 )
			{
				send_to_char( "Hai sotto il tuo controllo troppi elementali della terra!\n\r", ch );
				return;
			}
		}
	}

	if ( count_charmed(ch) )
		return;

	elm = create_mobile( get_mob_index(MOB_VNUM_ELM_EARTH) );

	for ( i = 0; i < MAX_STATS; i++ )
		elm->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	elm->perm_stat[STR] += 3;
	elm->perm_stat[INT] -= 1;
	elm->perm_stat[CON] += 2;

	elm->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (2 * ch->pcdata->perm_point[LIFE]) + 400, 30000 );
	elm->point[LIFE] = elm->max_point[LIFE];
	elm->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	elm->point[MANA] = elm->max_point[MANA];
	elm->level = ch->level;
	for ( i = 0; i < 3; i++ )
		elm->armor[i] = interpolate( elm->level, 100, -100 );
	elm->armor[3] = interpolate( elm->level, 100, 0 );
	elm->gold = 0;
	elm->timer = 0;
	elm->damage[DICE_NUMBER] = 3;
	elm->damage[DICE_TYPE] = 10;
	elm->damage[DICE_BONUS] = ch->level / 2;

	char_to_room( elm, ch->in_room );
	send_to_char( "Il tuo elementale e' pronto a servirti!\n\r", ch );
	act( "$n riesce nel suo intento ed il suo elementale e' pronto a servire!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( elm->affected_by, AFF_CHARM );
	elm->master = elm->leader = ch;
}


void spell_frostbolt( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 10 );
	if ( saves_spell(level, victim, DAM_COLD) )
		dam /= 2;
	damage( ch, victim, dam, sn, DAM_COLD, TRUE );
}


void spell_summon_air_elm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*elm;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per plasmare un altro elementale ora.\n\r", ch );
		return;
	}

	send_to_char( "Cerchi di plasmare un elementale dell'aria richiamandone il soffio vitale.\n\r", ch );
	act( "$n cerca di plasmare un elementale dell'aria richiamandone il soffio vitale.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_ELM_AIR) )
		{
			i++;
			if ( i > 2 )
			{
				send_to_char( "Hai sotto il tuo controllo troppi elementali dell'aria!\n\r", ch );
				return;
			}
		}
	}

	if ( count_charmed(ch) )
		return;

	elm = create_mobile( get_mob_index(MOB_VNUM_ELM_AIR) );

	for ( i = 0; i < MAX_STATS; i++ )
		elm->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	elm->perm_stat[STR] += 3;
	elm->perm_stat[INT] -= 1;
	elm->perm_stat[CON] += 2;

	elm->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (4 * ch->pcdata->perm_point[LIFE]) + 1000, 30000 );
	elm->point[LIFE] = elm->max_point[LIFE];
	elm->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	elm->point[MANA] = elm->max_point[MANA];
	elm->level = ch->level;
	for ( i = 0; i < 3; i++ )
		elm->armor[i] = interpolate( elm->level, 100, -100 );
	elm->armor[3] = interpolate( elm->level, 100, 0 );
	elm->gold = 0;
	elm->timer = 0;
	elm->damage[DICE_NUMBER] = 7;
	elm->damage[DICE_TYPE] = 4;
	elm->damage[DICE_BONUS] = ch->level / 2;

	char_to_room( elm, ch->in_room );
	send_to_char( "Riesci nel tuo intento!L'elementale dell'aria e' pronto al tuo servizio!\n\r", ch );
	act( "$n riesce nel suo intento!L'elementale dell'aria e' ora al suo servizio!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( elm->affected_by, AFF_CHARM );
	elm->master = elm->leader = ch;
}


void spell_summon_water_elm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*elm;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per creare un altro elementale dell'acqua ora.\n\r", ch );
		return;
	}

	send_to_char( "Cerchi di plasmare un elementale dell'acqua richiamandone il soffio vitale.\n\r", ch );
	act( "$n cerca di plasmare un elementale dell'aria richiamandone il soffio vitale.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_ELM_WATER) )
		{
			i++;
			if ( i > 2 )
			{
				send_to_char( "Hai sotto il tuo controllo troppi elementali dell'acqua!\n\r", ch );
				return;
			}
		}
	}

	if ( count_charmed(ch) )
		return;

	elm = create_mobile( get_mob_index(MOB_VNUM_ELM_WATER) );

	for ( i = 0; i < MAX_STATS; i++ )
		elm->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	elm->perm_stat[STR] += 3;
	elm->perm_stat[INT] -= 1;
	elm->perm_stat[CON] += 2;

	elm->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (5 * ch->pcdata->perm_point[LIFE]) + 2000, 30000 );
	elm->point[LIFE] = elm->max_point[LIFE];
	elm->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	elm->point[MANA] = elm->max_point[MANA];
	elm->level = ch->level;
	for ( i = 0; i < 3; i++ )
		elm->armor[i] = interpolate( elm->level, 100, -100 );
	elm->armor[3] = interpolate( elm->level, 100, 0 );
	elm->gold = 0;
	elm->timer = 0;
	elm->damage[DICE_NUMBER] = 8;
	elm->damage[DICE_TYPE] = 4;
	elm->damage[DICE_BONUS] = ch->level / 2;

	char_to_room( elm, ch->in_room );
	send_to_char( "L'elementale dell'acqua e' pronto a servirti!\n\r", ch );
	act( "L'elementale dell'acqua e' pronto a servire $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( elm->affected_by, AFF_CHARM );
	elm->master = elm->leader = ch;
}


void spell_summon_fire_elm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*elm;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per creare un altro elementale del fuoco ora.\n\r", ch );
		return;
	}

	send_to_char( "Cerchi di plasmare un elementale del fuoco richiamandone il soffio vitale.\n\r", ch );
	act( "$n cerca di plasmare un elementale dell'aria richiamandone il soffio vitale.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_ELM_FIRE) )
		{
			i++;
			if ( i > 2 )
			{
				send_to_char( "Hai sotto il tuo controllo troppi elementali del fuoco!\n\r", ch );
				return;
			}
		}
	}

	if ( count_charmed(ch) )
		return;

	elm = create_mobile( get_mob_index(MOB_VNUM_ELM_FIRE) );

	for ( i = 0; i < MAX_STATS; i++ )
		elm->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	elm->perm_stat[STR] += 3;
	elm->perm_stat[INT] -= 1;
	elm->perm_stat[CON] += 2;

	elm->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (10 * ch->pcdata->perm_point[LIFE]) + 1000, 30000 );
	elm->point[LIFE] = elm->max_point[LIFE];
	elm->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	elm->point[MANA] = elm->max_point[MANA];
	elm->level = ch->level;
	for ( i = 0; i < 3; i++ )
		elm->armor[i] = interpolate( elm->level, 100, -100 );
	elm->armor[3] = interpolate( elm->level, 100, 0 );
	elm->gold = 0;
	elm->timer = 0;
	elm->damage[DICE_NUMBER] = 11;
	elm->damage[DICE_TYPE] = 5;
	elm->damage[DICE_BONUS] = ch->level / 2 + 10;

	char_to_room( elm, ch->in_room );
	send_to_char( "L'elementale del fuoco e' pronto a servirti!\n\r", ch );
	act( "L'elementale del fuoco e' pronto a servire $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( elm->affected_by, AFF_CHARM );
	elm->master = elm->leader = ch;
}


void spell_summon_light_elm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*elm;
	AFFECT_DATA af;
	int			i = 0;
	/*~~~~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia per richiamare un altro elementale della luce ora.\n\r", ch );
		return;
	}

	send_to_char( "Cerchi di plasmare un elementale della luce aria richiamandone il soffio vitale.\n\r", ch );
	act( "$n cerca di plasmare un elementale della luce richiamandone il soffio vitale.", ch, NULL, NULL, TO_ROOM, POS_REST );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if( IS_MOB(gch)
		&&	IS_AFFECTED(gch, AFF_CHARM)
		&&	gch->master == ch
		&&	(gch->pIndexData->vnum == MOB_VNUM_ELM_LIGHT) )
		{
			i++;
			if ( i > 2 )
			{
				send_to_char( "Hai sotto il tuo controllo troppi elementali della luce!\n\r", ch );
				return;
			}
		}
	}

	if ( count_charmed(ch) )
		return;

	elm = create_mobile( get_mob_index(MOB_VNUM_ELM_LIGHT) );

	for ( i = 0; i < MAX_STATS; i++ )
		elm->perm_stat[i] = UMIN( 25, 15 + ch->level / 10 );

	elm->perm_stat[STR] += 3;
	elm->perm_stat[INT] -= 1;
	elm->perm_stat[CON] += 2;

	elm->max_point[LIFE] = IS_MOB( ch ) ? URANGE( ch->max_point[LIFE], 1 * ch->max_point[LIFE], 30000 ) :
		UMIN( (10 * ch->pcdata->perm_point[LIFE]) + 4000, 30000 );
	elm->point[LIFE] = elm->max_point[LIFE];
	elm->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	elm->point[MANA] = elm->max_point[MANA];
	elm->level = ch->level;
	for ( i = 0; i < 3; i++ )
		elm->armor[i] = interpolate( elm->level, 100, -100 );
	elm->armor[3] = interpolate( elm->level, 100, 0 );
	elm->gold = 0;
	elm->timer = 0;
	elm->damage[DICE_NUMBER] = 13;
	elm->damage[DICE_TYPE] = 9;
	elm->damage[DICE_BONUS] = ch->level / 2 + 10;

	char_to_room( elm, ch->in_room );
	send_to_char( "L'elementale della luce e' pronto a servirti!\n\r", ch );
	act( "L'elementale della luce e' pronto a servire $n", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	SET_BIT( elm->affected_by, AFF_CHARM );
	elm->master = elm->leader = ch;
}


void spell_fire_and_ice( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*tmp_vict;
	CHAR_DATA	*tmp_next;
	int			dam, dam_sn;
	int			movedam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dam = dice( level, 8 );
	movedam = number_range( ch->level, 2 * ch->level );

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

			if ( saves_spell(level, tmp_vict, DAM_FIRE) )
				dam /= 2;
			if ( (dam_sn = skill_lookup("fireball")) < 0 )
				dam_sn = sn;
			damage( ch, tmp_vict, dam, dam_sn, DAM_FIRE, TRUE );
			tmp_vict->point[MOVE] -= UMIN( tmp_vict->point[MOVE], movedam );

			if ( (IS_MOB(tmp_vict) && tmp_vict->position == POS_DEAD)
			||	(!IS_MOB(tmp_vict) && (current_time - tmp_vict->last_death_time) < 10) )
			{
				if ( saves_spell(level, tmp_vict, DAM_COLD) )
					dam /= 2;
				if ( (dam_sn = skill_lookup("iceball")) < 0 )
					dam_sn = sn;
				damage( ch, tmp_vict, dam, dam_sn, DAM_COLD, TRUE );
			}
		}
	}
}


void spell_grounding( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Sei gia' a terra.\n\r", ch );
		else
			act( "$N e' gia' a terra.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = 5 + level / 8;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = ADET_GROUNDING;
	affect_to_char( victim, &af );
	send_to_char( "Una forza magnetica ti riporta a terra.\n\r", victim );
	if ( ch != victim )
		act( "Una forza magnetica riporta $N a terra.", ch, NULL, victim, TO_CHAR, POS_REST );
}


void spell_tsunami( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (ch->in_room->sector_type != SECT_WATER_SWIM) && (ch->in_room->sector_type != SECT_WATER_NOSWIM) )
	{
		send_to_char( "Hai bisogno di una fonte acquifera per creare uno Tsunami.\n\r", ch );
		ch->wait = 0;
		return;
	}

	act( "Al tuo gesto le acque si sollevano formando un pugno enorme che va a colpire in pieno $n.",
		victim, NULL, NULL, TO_ROOM, POS_REST );
	act( "Al gesto di $N le acque si sollevano formando un pugno enorme che si schianta con violenza su di te!!!",
		victim, NULL, NULL, TO_CHAR, POS_REST );
	dam = dice( level, 16 );
	damage( ch, victim, dam, sn, DAM_DROWNING, TRUE );
}


void spell_disenchant_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA *paf;
	int			result, fail;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Devi avere l'oggetto in inventario per poter applicare l'incantesimo.\n\r", ch );
		return;
	}

	/* calcola i bonus */
	fail = 75;
	fail -= ( level - obj->level ) * 5;
	if ( IS_SET(obj->extra_flags, ITEM_MAGIC) )
		fail += 25;

	fail = URANGE( 5, fail, 95 );

	result = number_percent();

	/* il momento della verita' */
	if ( result < (fail / 5) )	/* oggetto distrutto */
	{
		act( "$p risplende fievolmente.. e si liquefa!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende fievolmente.. e si liquefa!", ch, obj, NULL, TO_ROOM, POS_REST );
		extract_obj( obj );
		return;
	}

	if ( result > (fail / 2) )	/* oggetto disincantato */
	{
		AFFECT_DATA *paf_next;
		/*~~~~~~~~~~~~~~~~~~*/

		act( "$p risplende per un attimo.. poi perde ogni luce.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende per un attimo.. poi perde ogni luce.", ch, obj, NULL, TO_ROOM, POS_REST );
		obj->enchanted = TRUE;

		/* rimuove tutti gli affects */
		for ( paf = obj->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			free_affect( paf );
		}

		obj->affected = NULL;

		obj->enchanted = FALSE;

		/* clear some flags */
		REMOVE_BIT( obj->extra_flags, ITEM_GLOW );
		REMOVE_BIT( obj->extra_flags, ITEM_HUM );
		REMOVE_BIT( obj->extra_flags, ITEM_MAGIC );
		REMOVE_BIT( obj->extra_flags, ITEM_INVIS );
		REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
		REMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );

		return;
	}

	send_to_char( "Non sembra essere accaduto nulla.\n\r", ch );
}


void spell_disenchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA *paf;
	int			result, fail;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj->item_type != ITEM_WEAPON )
	{
		send_to_char( "Quest'incantesimo funziona solo sulle armi.Sulle armi VERE.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Devi avere l'arma in inventario per poterla incantare.\n\r", ch );
		return;
	}

	/* calcola i bonus */
	fail = 75;
	fail -= ( level - obj->level ) * 5;
	if ( IS_SET(obj->extra_flags, ITEM_MAGIC) )
		fail += 25;

	fail = URANGE( 5, fail, 95 );

	result = number_percent();

	/* momento della verita' */
	if ( result < (fail / 5) )	/* oggetto distrutto */
	{
		act( "$p brilla con intermittenza ed esplode!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p brilla con intermittenza ed esplode!", ch, obj, NULL, TO_ROOM, POS_REST );
		extract_obj( obj );
		return;
	}

	if ( result > (fail / 2) )	/* item disenchanted */
	{
		AFFECT_DATA *paf_next;
		/*~~~~~~~~~~~~~~~~~~*/

		act( "$p risplende debolmente.. poi perde ogni luce.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p risplende debolmente.. poi perde ogni luce.", ch, obj, NULL, TO_ROOM, POS_REST );
		obj->enchanted = TRUE;

		/* remove all affects */
		for ( paf = obj->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			free_affect( paf );
		}

		obj->affected = NULL;

		obj->enchanted = FALSE;

		/* clear some flags */
		REMOVE_BIT( obj->extra_flags, ITEM_GLOW );
		REMOVE_BIT( obj->extra_flags, ITEM_HUM );
		REMOVE_BIT( obj->extra_flags, ITEM_MAGIC );
		REMOVE_BIT( obj->extra_flags, ITEM_INVIS );
		REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
		REMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );

		return;
	}

	send_to_char( "Non sembra essere accaduto nulla.\n\r", ch );
}


void spell_absorb( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Sei gia' in atto di assorbire l'energia magica intorno a te.\n\r", ch );
		return;
	}

	af.where = TO_DETECTS;
	af.type = gsn_absorb;
	af.level = level;
	af.duration = 3 + level / 10;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = ADET_ABSORB;
	affect_to_char( ch, &af );
	send_to_char( "Il tuo corpo viene rivestito di una protezione magica.\n\r", ch );
}


void spell_transfer_object( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	send_to_char( "Non implementato!\n\r", ch );
}


void spell_animate_object( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*mob;
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA af;
	char		buf[MSL];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !(obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_ARMOR) )
	{
		send_to_char( "Puoi animare solo armi o armature.\n\r", ch );
		return;
	}

	if ( is_affected(ch, sn) )
	{
		send_to_char( "Non hai abbastanza energia spirituale per controllare altri corpi non morti.\n\r", ch );
		return;
	}

	if ( obj->level > level )
	{
		act( "$p e' troppo potente perche' tu lo possa animare.", ch, obj, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( count_charmed(ch) )
		return;

	if ( obj->item_type == ITEM_ARMOR
	&&	!(CAN_WEAR(obj, ITEM_WEAR_BODY) || CAN_WEAR(obj, ITEM_WEAR_HANDS) || CAN_WEAR(obj, ITEM_WEAR_SHIELD)) )
	{
		send_to_char( "Non puoi animare quel tipo di armatura.\n\r", ch );
		return;
	}

	if ( number_percent() > get_skill(ch, sn) )
	{
		act( "$p esplode con un boato!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p esplode con un boato!", ch, obj, NULL, TO_ROOM, POS_REST );
		extract_obj( obj );
		return;
	}

	if ( obj->item_type == ITEM_WEAPON )
		mob = create_mobile( get_mob_index(MOB_VNUM_WEAPON) );
	else
		mob = create_mobile( get_mob_index(MOB_VNUM_ARMOR) );

	sprintf( buf, "anima %s", obj->name );
	free_string( mob->name );
	mob->name = str_dup( buf );

	sprintf( buf, mob->short_descr, obj->short_descr );
	free_string( mob->short_descr );
	mob->short_descr = str_dup( buf );

	sprintf( buf, "%s e' qui.. vibrando irregolarmente.\n\r", capitalize(obj->short_descr) );
	free_string( mob->long_descr );
	mob->long_descr = str_dup( buf );

	char_to_room( mob, ch->in_room );
	mob->level = obj->level;

	for ( i = 0; i < MAX_STATS; i++ )
		mob->perm_stat[i] = UMIN( 25, ch->perm_stat[i] );
	for ( i = 0; i < 3; i++ )
		mob->armor[i] = interpolate( mob->level, 100, -100 );
	mob->armor[3] = interpolate( mob->level, 100, 0 );

	if ( obj->item_type == ITEM_WEAPON )
	{
		mob->point[LIFE] = IS_MOB( ch ) ? 100 : UMIN( (25 * mob->level) + 1000, 30000 );
		mob->max_point[LIFE] = mob->point[LIFE];
		mob->point[MANA] = ch->level * 40;
		mob->max_point[MANA] = mob->point[MANA];
		mob->point[MOVE] = ch->level * 40;
		mob->max_point[MOVE] = mob->point[MOVE];
		mob->timer = 0;
		mob->damage[DICE_NUMBER] = obj->value[1];
		mob->damage[DICE_TYPE] = obj->value[2];
		mob->damage[DICE_BONUS] = number_range( level / 10, level / 8 );
	}

	if ( obj->item_type == ITEM_ARMOR )
	{
		mob->point[LIFE] = IS_MOB( ch ) ? 100 : UMIN( (100 * mob->level) + 2000, 30000 );
		mob->max_point[LIFE] = mob->point[LIFE];
		mob->point[MANA] = ch->level * 40;
		mob->max_point[MANA] = mob->point[MANA];
		mob->point[MOVE] = ch->level * 40;
		mob->max_point[MOVE] = mob->point[MOVE];
		mob->timer = 0;
		mob->damage[DICE_NUMBER] = number_range( level / 15, level / 12 );
		mob->damage[DICE_TYPE] = number_range( level / 3, level / 2 );
		mob->damage[DICE_BONUS] = number_range( level / 10, level / 8 );
	}

	mob->sex = ch->sex;
	mob->gold = 0;
	mob->master = mob->leader = ch;
	SET_BIT( mob->affected_by, AFF_CHARM );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = ch->level;
	af.duration = 1 + ( obj->level / 30 );
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;
	affect_to_char( ch, &af );

	act( "$p ha ora l'anima che tu hai evocato!", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$n evoca un'anima all'interno di $p!", ch, obj, NULL, TO_ROOM, POS_REST );

	extract_obj( obj );
}


void spell_windwall( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	int			dam, hp_dam, dice_dam, hpch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	act( "$n innalza un muro di vento che investe tutti!", ch, NULL, NULL, TO_ROOM, POS_REST );
	act( "Evochi le potenze elementali innalzando un muro di vento che investe tutti!", ch, NULL, NULL, TO_CHAR, POS_REST );

	hpch = UMAX( 16, ch->point[LIFE] );
	hp_dam = number_range( hpch / 15 + 1, 8 );
	dice_dam = dice( level, 12 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( is_safe_spell(ch, vch, TRUE)
		||	(IS_MOB(ch) && IS_MOB(vch) && (ch->fighting == vch || vch->fighting == ch)) )
			continue;

		if ( is_safe(ch, vch) )
			continue;

		if (vch != ch
		&&	ch->fighting != vch
		&&	vch->fighting != ch
		&&	(IS_SET(vch->affected_by, AFF_CHARM) || IS_MOB(vch)) )
		{
			do_yell( vch, "Aiuto! Mi stanno attaccando!" );
		}

		if ( !IS_AFFECTED(vch, AFF_FLYING) )	dam /= 2;

		if ( vch->size == SIZE_TINY )			dam *= 1.5;
		else if ( vch->size == SIZE_SMALL )		dam *= 1.3;
		else if ( vch->size == SIZE_MEDIUM )	dam *= 1;
		else if ( vch->size == SIZE_LARGE )		dam *= 0.9;
		else if ( vch->size == SIZE_HUGE )		dam *= 0.7;
		else									dam *= 0.5;

		if ( saves_spell(level, vch, DAM_OTHER) )
			damage( ch, vch, dam / 2, sn, DAM_OTHER, TRUE );
		else
			damage( ch, vch, dam, sn, DAM_OTHER, TRUE );
	}
}


void spell_earthfade( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( CAN_DETECT(ch, ADET_EARTHFADE) ) return;

	if (ch->in_room->sector_type == SECT_AIR
	||	ch->in_room->sector_type == SECT_WATER_SWIM
	||	ch->in_room->sector_type == SECT_WATER_NOSWIM )
	{
		send_to_char( "Per svanire nella terra.. deve esserci terra, non credi?\n\r", ch );
		return;
	}

	if ( MOUNTED(ch) )
	{
		send_to_char( "Non puoi svanire nella terra in questa posizione..\n\r", ch );
		return;
	}

	act( "$n svanisce nella terra.", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_DETECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( level / 8 + 10 );
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ADET_EARTHFADE;
	affect_to_char( ch, &af );
	send_to_char( "Svanisci nella terra.\n\r", ch );
}


void spell_earthmaw( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	char		buf[MSL];
	int			dam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Con un gesto fai smuovere la terra intorno a %s!\n\r", victim->name );
	send_to_char( buf, ch );
	act( "$n con un gesto fa smuovere la terra sotto di te!", ch, NULL, victim, TO_VICT, POS_REST );
	if ( IS_AFFECTED(victim, AFF_FLYING) )
		dam = 0;
	else
		dam = dice( level, 16 );
	damage( ch, victim, dam, sn, DAM_BASH, TRUE );
}


void spell_drain( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA	*obj = (OBJ_DATA *) vo;
	AFFECT_DATA *paf;
	int			drain;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_SET(obj->extra_flags, ITEM_MAGIC) )
	{
		send_to_char( "Questo oggetto non e' magico.\n\r", ch );
		return;
	}

	switch ( obj->item_type )
	{
	default:
		drain = 1;
		break;

	case ITEM_ARMOR:
		drain = obj->value[3];
		break;

	case ITEM_TREASURE:
		drain = 4;
		break;

	case ITEM_POTION:
		drain = 8;
		break;

	case ITEM_SCROLL:
	case ITEM_STAFF:
	case ITEM_WAND:
		drain = 12;
		break;

	case ITEM_WEAPON:
		drain = obj->value[1] + obj->value[2] / 2;
		break;

	case ITEM_LIGHT:
		if ( obj->value[2] == -1 )
			drain = 10;
		else
			drain = 4;
		break;
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
		drain += 5;

	drain *= dice( 2, 5 );
	drain += obj->level / 2;

	if ( number_percent() > get_skill(ch, sn) )
	{
		act( "$p evapora!", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "$p evapora.. ma non riesci a canalizzare l'energia.", ch, obj, NULL, TO_CHAR, POS_REST );
	}
	else
	{
		act( "$p evapora mentre $n ne assorbe l'energia!", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "$p evapora mentre tu ne assorbi l'energia!", ch, obj, NULL, TO_CHAR, POS_REST );
		ch->point[MANA] = UMIN( ch->point[MANA] + drain, ch->max_point[MANA] );
	}

	extract_obj( obj );
}


void spell_soften( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 5 + level / 10;
	af.location = APPLY_AC;
	af.modifier = 4 * level;
	af.bitvector = AFF_FAERIE_FIRE;
	affect_to_char( victim, &af );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 10 + level / 5 );
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	send_to_char( "La tua pelle comincia a raggrinzire!\n\r", victim );
	act( "La pelle di $n comincia a raggrinzire visibilmente!", victim, NULL, NULL, TO_ROOM, POS_REST );
}


void spell_fumble( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA	*victim = (CHAR_DATA *) vo;
	OBJ_DATA	*obj;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(victim, sn) )
	{
		if ( victim == ch )
			send_to_char( "Ti muovi gia' abbastanza maldestramente..\n\r", ch );
		else
			act( "$N non puo' di certo muoversi piu' maldestramente..", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( saves_spell(level, victim, DAM_OTHER) || IS_SET(victim->imm_flags, IMM_MAGIC) )
	{
		if ( victim != ch )
			send_to_char( "Nulla sembra accadere..\n\r", ch );
		send_to_char( "Per un momento sembri perdere ogni coordinazione..\n\r", victim );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_HASTE) )
	{
		if ( !check_dispel(level, victim, skill_lookup("haste")) )
		{
			if ( victim != ch )
				send_to_char( "L'incantesimo non ha effetto.\n\r", ch );
			send_to_char( "Per un attimo ti senti privare di ogni coordinazione.\n\r", victim );
			return;
		}

		act( "$n si muove meno rapidamente ora.", victim, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = ( 4 + level / 12 );
	af.location = APPLY_DEX;
	af.modifier = -UMAX( 2, level / 6 );
	af.bitvector = 0;
	affect_to_char( victim, &af );

	if ( (obj = get_weapon_char(victim, TRUE)) != NULL )
	{
		if ( can_drop_obj(victim, obj) && remove_obj(victim, obj, TRUE) )
		{
			act( "$n non riesce piu' a reggere $p e se ne libera.", victim, obj, NULL, TO_ROOM, POS_REST );
			send_to_char( "Non riesci piu' a reggere la tua seconda arma e te ne liberi!\n\r", victim );
			obj_from_char( obj );
			obj_to_room( obj, victim->in_room );
		}
	}

	if ( (obj = get_weapon_char(victim, FALSE)) != NULL )
	{
		if ( can_drop_obj(victim, obj) && remove_obj(victim, obj, TRUE) )
		{
			act( "$n non riesce piu' a reggere $p e se ne libera.", victim, obj, NULL, TO_ROOM, POS_REST );
			send_to_char( "Non riesci piu' a reggere la tua arma e te ne liberi!\n\r", victim );
			obj_from_char( obj );
			obj_to_room( obj, victim->in_room );
		}
	}

	WAIT_STATE( victim, PULSE_VIOLENCE );
	send_to_char( "Non riesci piu' a coordinare decentemente i tuoi movimenti!\n\r", victim );
	act( "$n comincia a muoversi in maniera decisamente maldestra..", victim, NULL, NULL, TO_ROOM, POS_REST );
}
