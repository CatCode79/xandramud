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
#include <ctype.h>
#include <time.h>

#include "xandra.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"


char *const where_name[] =
{
	"<al dito>           ",
	"<al collo>          ",
	"<sul corpo>         ",
	"<sulla testa>       ",
	"<sulle gambe>       ",
	"<ai piedi>          ",
	"<alle mani>         ",
	"<sulle braccia>     ",
	"<sulle spalle>      ",
	"<alla vita>         ",
	"<attorno al polso>  ",
	"<mano secondaria>%c  ",
	"<mano principale>%c  ",
	"<le mani impugnano> ",
	"<fluttuante vicino> ",
	"<tatuaggio>         ",
	"<conficcato>        "
};

/* for do_count */
int			max_on = 0;


/*
 * Funzioni locali.
 */
char	*format_obj_to_char		( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort );
#ifdef MXP
void	show_list_to_char		( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing, const int iDefaultAction );
#else
void	show_list_to_char		( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing );
#endif
void	show_char_to_char_0		( CHAR_DATA *victim, CHAR_DATA *ch );
void	show_char_to_char_1		( CHAR_DATA *victim, CHAR_DATA *ch, bool equip );
void	show_char_to_char		( CHAR_DATA *list, CHAR_DATA *ch );
bool	check_blind				( CHAR_DATA *ch );
bool	show_vwear_to_char		( CHAR_DATA *ch, OBJ_DATA *obj );
bool	show_cwear_to_char		( CHAR_DATA *ch, OBJ_DATA *obj );
char	*get_stat_alias 		( CHAR_DATA *ch, int which );
char	*get_ac_alias			( CHAR_DATA *ch );


/*
 * Variabili esterne.
 */
extern	char *const		dir_name_to[];


/*
 * Funzioni esterne.
 */
char *get_exit_string( char *arg );


bool show_vwear_to_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( can_see_obj(ch, obj) )
	{
		sprintf( buf, where_name[obj->wear_loc], ' ' );
		send_to_char( buf, ch );
		send_to_char( format_obj_to_char(obj, ch, TRUE), ch );
		send_to_char( "\n\r", ch );
		return( TRUE );
	}

	return( FALSE );
}


bool show_cwear_to_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (obj->wear_loc == WEAR_LEFT && LEFT_HANDER(ch)) || (obj->wear_loc == WEAR_RIGHT && RIGHT_HANDER(ch)) )
		sprintf( buf, where_name[obj->wear_loc], '*' );
	else
		sprintf( buf, where_name[obj->wear_loc], ' ' );
	send_to_char( buf, ch );

	if ( can_see_obj(ch, obj) )
		send_to_char( format_obj_to_char(obj, ch, TRUE), ch );
	else
		send_to_char( "qualcosa.\n\r", ch );
	send_to_char( "\n\r", ch );

	return( TRUE );
}


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
	static char buf[MSL];
	static char buf_con[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	buf[0] = '\0';
	buf_con[0] = '\0';

	if ( obj->pIndexData->vnum > 5 )
	{	/* money, gold, etc */
		sprintf( buf_con, " [%s%s%s]",
			CLR(ch, GREEN),
			get_cond_alias(obj),
			CLR(ch, WHITE_BOLD) );
	}
	else
		buf_con[0] = '\0';

	if ( (fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
	||	(obj->description == NULL || obj->description[0] == '\0') )
		return( buf );

	if ( IS_OBJ_STAT(obj, ITEM_SHARP) )
		strcat( buf, "(Affilato) " );

	if ( IS_OBJ_STAT(obj, ITEM_BURIED) )
	{
		strcat( buf, CLR(ch, WHITE) );
		strcat( buf, "(Sepolto) " );
	}

	if ( IS_OBJ_STAT(obj, ITEM_INVIS) )
	{
		strcat( buf, CLR(ch, WHITE) );
		strcat( buf, "(Invisibile) " );
	}

	if ( CAN_DETECT(ch, DETECT_EVIL) && IS_OBJ_STAT(obj, ITEM_EVIL) )
	{
		strcat( buf, CLR(ch, RED) );
		strcat( buf, "(Aura Rossa) " );
	}

	if ( CAN_DETECT(ch, DETECT_GOOD) && IS_OBJ_STAT(obj, ITEM_BLESS) )
	{
		strcat( buf, CLR(ch, BLUE) );
		strcat( buf, "(Aura Blu) " );
	}

	if ( CAN_DETECT(ch, DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_MAGIC) )
	{
		strcat( buf, CLR(ch, YELLOW) );
		strcat( buf, "(Magico) " );
	}

	if ( IS_OBJ_STAT(obj, ITEM_GLOW) )
	{
		strcat( buf, CLR(ch, CYAN) );
		strcat( buf, "(Splendente) " );
	}

	if ( IS_OBJ_STAT(obj, ITEM_HUM) )
	{
		strcat( buf, CLR(ch, BROWN) );
		strcat( buf, "(Ronzante) " );
	}

	strcat( buf, CLR(ch, WHITE_BOLD) );

	if ( fShort )
	{
		if ( obj->short_descr != NULL )
		{
			strcat( buf, obj->short_descr );
			strcat( buf, buf_con );
		}
	}
	else
	{
		if ( obj->description != NULL )
		{
			char	tmp[70];
			/*~~~~~~~~~~~~*/

			if ( obj->in_room != NULL )
			{
				if ( IS_WATER(obj->in_room) )
				{
					strcpy( tmp, obj->short_descr );
					tmp[0] = UPPER( tmp[0] );
					strcat( buf, tmp );
					switch ( dice(1, 3) )
					{
					case 1:	strcat( buf, " galleggia cullato dall'acqua." );				break;
					case 2:	strcat( buf, " viene trasportato dai movimenti dell'acqua." );	break;
					case 3:	strcat( buf, " e' completamente zuppo d'acqua." );				break;
					}
				}
				else
					strcat( buf, obj->description );
			}
			else
				strcat( buf, obj->description );
		}
	}

	return( buf );
}


/*
 * Show a list to a character. Can coalesce duplicated items.
 */
#ifdef MXP
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing, const int iDefaultAction )
#else
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
#endif
{
	char		buf[MSL];
	BUFFER		*output;
	char		**prgpstrShow;
	int			*prgnShow;
	char		*pstrShow;
	OBJ_DATA	*obj;
	int			nShow;
	int			iShow;
	int			count;
	bool		fCombine;
#ifdef MXP
	char		**prgpstrName;
	char		**prgpstrShortName;
	char		*pstrName;
	char		*pstrShortName;
	char		*pAction = NULL;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->desc == NULL )
		return;

#ifdef MXP
	/* work out which MXP tag to use */
	switch ( iDefaultAction )
	{
	case eItemGet:	pAction = "Get";	break;	/* item on ground */
	case eItemDrop:	pAction = "Drop";	break;	/* item in inventory */
	} /* end of switch on action */
#endif

	/* Alloc space for output lines. */
	output = new_buf();

	count = 0;
	for ( obj = list; obj != NULL; obj = obj->next_content )
		count++;
	prgpstrShow = alloc_mem( count * sizeof(char *) );
#ifdef MXP
	prgpstrName = alloc_mem( count * sizeof(char *) );
	prgpstrShortName = alloc_mem( count * sizeof(char *) );
#endif
	prgnShow = alloc_mem( count * sizeof(int) );
	nShow = 0;

	/* Format the list of objects. */
	for ( obj = list; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) )
		{
			pstrShow = format_obj_to_char( obj, ch, fShort );
#ifdef MXP
			pstrName = obj->name;
			pstrShortName = obj->short_descr;
#endif
			fCombine = FALSE;

			if ( IS_MOB(ch) || IS_SET(ch->comm, COMM_COMBINE) )
			{
				/* Look for duplicates, case sensitive. Matches tend to be near end so run loop backwords. */
				for ( iShow = nShow - 1; iShow >= 0; iShow-- )
				{
					if ( !strcmp(prgpstrShow[iShow], pstrShow) )
					{
						prgnShow[iShow]++;
						fCombine = TRUE;
						break;
					}
				}
			}

			/* Couldn't combine, or didn't want to. */
			if ( !fCombine )
			{
				prgpstrShow[nShow] = str_dup( pstrShow );
#ifdef MXP
				prgpstrName [nShow] = str_dup( pstrName );
				prgpstrShortName [nShow] = str_dup( pstrShortName );
#endif
				prgnShow[nShow] = 1;
				nShow++;
			}
		}
	}

	/* Output the formatted list. */
	for ( iShow = 0; iShow < nShow; iShow++ )
	{
		if ( prgpstrShow[iShow][0] == '\0' )
		{
			free_string( prgpstrShow[iShow] );
#ifdef MXP
			free_string( prgpstrName[iShow] );
			free_string( prgpstrShortName[iShow] );
#endif
			continue;
		}

		if ( IS_MOB(ch) || IS_SET(ch->comm, COMM_COMBINE) )
		{
			if ( prgnShow[iShow] != 1 )
			{
				sprintf( buf, "%2d ", prgnShow[iShow] );
				add_buf( output, buf );
			}
			else
				add_buf( output, "   " );
		}

#ifdef MXP
		if ( pAction )
		{
			int		i = 0;
			char	*p = prgpstrName[iShow];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for ( ; *p && !isspace (*p); p++, i++ )
				;

			sprintf( buf, MXPTAG("%s '%.*s' '%s'"), pAction, i, prgpstrName[iShow], prgpstrShortName[iShow] );
			add_buf( output, buf );
		}
#endif
		add_buf( output,prgpstrShow[iShow] );
#ifdef MXP
		if ( pAction )
		{
			sprintf( buf, MXPTAG("/%s"), pAction );
			add_buf( output, buf );
		}
#endif
		add_buf( output, "\n\r" );
		free_string( prgpstrShow[iShow] );
#ifdef MXP
		free_string(prgpstrName[iShow]);
		free_string(prgpstrShortName[iShow]);
#endif
	}

	if ( fShowNothing && nShow == 0 )
	{
		if ( IS_MOB(ch) || IS_SET(ch->comm, COMM_COMBINE) )
			send_to_char( "     ", ch );
		send_to_char( "Nulla.\n\r", ch );
	}

	page_to_char( buf_string(output), ch );

	/* Clean up. */
	free_buf( output );
	free_mem( prgpstrShow, count * sizeof(char *) );
#ifdef MXP
	free_mem( prgpstrName, count * sizeof(char *) );
	free_mem( prgpstrShortName, count * sizeof(char *) );
#endif
	free_mem( prgnShow, count * sizeof(int) );
}


void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
	char	buf[MSL], message[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	buf[0] = '\0';

	/* Quest staff */
	if ( !IS_MOB(ch) && IS_MOB(victim) && ch->pcdata->questmob > 0 && victim->pIndexData->vnum == ch->pcdata->questmob )
		strcat( buf, "[OBIETTIVO] " );

	if ( RIDDEN(victim) )											strcat( buf, "(Cavalcante) " );
	if ( IS_AFFECTED(victim, AFF_INVISIBLE) )						strcat( buf, "(Invisibile) " );
	if ( IS_AFFECTED(victim, AFF_IMP_INVIS) )						strcat( buf, "(Improved) " );
	if ( victim->invis_level >= LEVEL_HERO )						strcat( buf, "(Wizinvis) " );
	if ( IS_AFFECTED(victim, AFF_HIDE) )							strcat( buf, "(Nascost$X) " );
	if ( IS_AFFECTED(victim, AFF_FADE) )							strcat( buf, "(Sbiadit$X) " );
	if ( IS_AFFECTED(victim, AFF_CAMOUFLAGE) )						strcat( buf, "(Camuffat$X) " );
	if ( CAN_DETECT(victim, ADET_EARTHFADE) )						strcat( buf, "(Terra) " );
	if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master == ch )	strcat( buf, "(Affascinat$X) " );
	if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )						strcat( buf, "(Traslucente) " );
	if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
	{
		strcat( buf, CLR(ch, MAGENTA) );
		strcat( buf, "(Aura Rosa) " );
		strcat( buf, CLR(ch, WHITE_BOLD) );
	}
	if ( IS_MOB(victim) && IS_SET(victim->act, ACT_UNDEAD)
	&&	CAN_DETECT(ch, DETECT_UNDEAD) )								strcat( buf, "(Non Mort$X) " );
	if ( IS_EVIL(victim) && CAN_DETECT(ch, DETECT_EVIL) )
	{
		strcat( buf, CLR(ch, RED) );
		strcat( buf, "(Aura Rossa) " );
		strcat( buf, CLR(ch, WHITE_BOLD) );
	}
	if ( IS_GOOD(victim) && CAN_DETECT(ch, DETECT_GOOD) )
	{
		strcat( buf, CLR(ch, YELLOW) );
		strcat( buf, "(Aura Dorata) " );
		strcat( buf, CLR(ch, WHITE_BOLD) );
	}
	if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	{
		strcat( buf, CLR(ch, WHITE) );
		strcat( buf, "(Aura Bianca) " );
		strcat( buf, CLR(ch, WHITE_BOLD) );
	}
	if ( !IS_MOB(victim) && IS_SET(victim->act, PLR_WANTED) )		strcat( buf, "(CRIMINALE) " );

	if ( victim->position == victim->start_pos && victim->long_descr[0] != '\0' )
	{
		strcat( buf, victim->long_descr );
		act( buf, ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( IS_SET(ch->act, PLR_HOLYLIGHT) && is_affected(victim, gsn_doppelganger) )
	{
		strcat( buf, "{" );
		strcat( buf, PERS(victim, ch) );
		strcat( buf, "} " );
	}

	if ( is_affected(victim, gsn_doppelganger) && victim->doppel->long_descr[0] != '\0' )
	{
		strcat( buf, victim->doppel->long_descr );
		act( buf, ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( victim->long_descr[0] != '\0' && !is_affected(victim, gsn_doppelganger) )
	{
		strcat( buf, victim->long_descr );
		act( buf, ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_doppelganger) )
		strcat( buf, PERS(victim->doppel, ch) );
	else
		strcat( buf, PERS(victim, ch) );

	switch ( victim->position )
	{
	case POS_DEAD:		strcat( buf, " e' mort$X!!" );				break;
	case POS_MORTAL:	strcat( buf, " e' ferit$X mortalmente." );	break;
	case POS_INCAP:		strcat( buf, " e' incapacitat$X." );		break;
	case POS_STUN:		strcat( buf, " e' tramortit$X." );			break;
	case POS_SLEEP:
		if ( victim->on != NULL )
		{
			if ( IS_SET(victim->on->value[2], SLEEP_AT) )
			{
				sprintf( message, " sta dormendo a %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else if ( IS_SET(victim->on->value[2], SLEEP_ON) )
			{
				sprintf( message, " sta dormendo sul %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else
			{
				sprintf( message, " sta dormendo in %s.", victim->on->short_descr );
				strcat( buf, message );
			}
		}
		else
			strcat( buf, " sta dormendo qui." );
		break;

	case POS_REST:
		if ( victim->on != NULL )
		{
			if ( IS_SET(victim->on->value[2], REST_AT) )
			{
				sprintf( message, " sta riposando a %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else if ( IS_SET(victim->on->value[2], REST_ON) )
			{
				sprintf( message, " sta riposando su %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else
			{
				sprintf( message, " sta riposando in %s.", victim->on->short_descr );
				strcat( buf, message );
			}
		}
		else
			strcat( buf, " sta riposando qui." );
		break;

	case POS_SIT:
		if ( victim->on != NULL )
		{
			if ( IS_SET(victim->on->value[2], SIT_AT) )
			{
				sprintf( message, " e' sedut$X a %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else if ( IS_SET(victim->on->value[2], SIT_ON) )
			{
				sprintf( message, " e' sedut$X su %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else
			{
				sprintf( message, " e' sedut$X in %s.", victim->on->short_descr );
				strcat( buf, message );
			}
		}
		else
			strcat( buf, " e' sedut$X qui." );
		break;

	case POS_STAND:
		if ( victim->on != NULL )
		{
			if ( IS_SET(victim->on->value[2], STAND_AT) )
			{
				sprintf( message, " e' in piedi a %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else if ( IS_SET(victim->on->value[2], STAND_ON) )
			{
				sprintf( message, " e' in piedi su %s.", victim->on->short_descr );
				strcat( buf, message );
			}
			else
			{
				sprintf( message, " e' in piedi in %s.", victim->on->short_descr );
				strcat( buf, message );
			}
		}
		else if ( MOUNTED(victim) )
		{
			sprintf( message, " e' qui, cavalcando %s.", PERS(MOUNTED(victim), ch) );
			strcat( buf, message );
		}
		else
			strcat( buf, " e' qui." );
		break;

	case POS_FIGHT:
		strcat( buf, " e' qui, combattendo " );
		if ( victim->fighting == NULL )
			strcat( buf, "l'aria??" );
		else if ( victim->fighting == ch )
			strcat( buf, "TE!" );
		else if ( victim->in_room == victim->fighting->in_room )
		{
			strcat( buf, PERS(victim->fighting, ch) );
			strcat( buf, "." );
		}
		else
			strcat( buf, "contro qualcosa o qualcuno??" );
		break;
	}

	buf[0] = UPPER( buf[0] );
	act( buf, ch, NULL, victim, TO_CHAR, POS_REST );
}


char *get_alias_wound( CHAR_DATA *ch )
{
	int		percent;

	if ( ch->max_point[LIFE] > 0 )
		percent = ( 100 * ch->point[LIFE] ) / ch->max_point[LIFE];
	else
		percent = -1;

	if		( percent >= 100 )		return( "e' in perfetta salute." );
	else if ( percent >=  90 )		return( "ha qualche graffio." );
	else if ( percent >=  75 )		return( "ha qualche piccolo e disgustoso taglio." );
	else if ( percent >=  50 )		return( "ha delle sanguinanti ferite su tutto il corpo." );
	else if ( percent >=  30 )		return( "e' un fiotto di sangue." );
	else if ( percent >=  15 )		return( "si contorce agonizzando." );
	else if ( percent >=   0 )		return( "si contorce per terra." );
	else							return( "sta per abbracciare la morte." );
}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch, bool equip )
{
	char		buf[MSL];
	CHAR_DATA	*vict;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	vict = is_affected( victim, gsn_doppelganger ) ? victim->doppel : victim;

	if ( can_see(victim, ch) )
	{
		if ( ch == victim )
			act( "$n guarda se stess$x.", ch, NULL, NULL, TO_ROOM, POS_REST );
		else
		{
			act( "$n ti guarda.", ch, NULL, victim, TO_VICT, POS_REST );
			act( "$n guarda $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		}
	}

	if ( !equip )	/* Se non è stato inviato "look equip" invia la descrizione */
	{
		int			percent;

		if ( vict->description[0] != '\0' )
			send_to_char( vict->description, ch );
		else
			act( "Non vedi nulla di speciale.", ch, NULL, victim, TO_CHAR, POS_REST );

		if ( MOUNTED(victim) )
		{
			sprintf( buf, "%s sta cavalcando %s.\n\r", PERS(victim, ch), PERS(MOUNTED(victim), ch) );
			send_to_char( buf, ch );
		}

		if ( RIDDEN(victim) )
		{
			sprintf( buf, "%s e' cavalcat$X da %s.\n\r", PERS(victim, ch), PERS(RIDDEN(victim), ch) );
			act( buf, ch, NULL, victim, TO_CHAR, POS_REST );
		}

		if ( victim->max_point[LIFE] > 0 )
			percent = ( 100 * victim->point[LIFE] ) / victim->max_point[LIFE];
		else
			percent = -1;

		/* Vampiro */
		if ( percent < 90 && ch->class == CLASS_VAMPIRE && ch->level > LEVEL_NEWBIE+5 )
			gain_condition( ch, COND_BLOODLUST, -1 );

		sprintf( buf, "(%s) %s, %s.\n\r",
			RACE_NAME(vict),
			PERS(vict, ch),
			(vict->sex == 0) ? "asessuato" : (vict->sex == 1) ? "maschio" : "femmina" );

		sprintf( buf, "%s\n\r", capitalize(get_alias_wound(vict)) );

		buf[0] = UPPER( buf[0] );
		send_to_char( buf, ch );
	}
	else
	{
		OBJ_DATA	*obj;
		int			iWear;
		bool		found;

		found = FALSE;
		for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
		{
			if ( iWear == WEAR_FINGER
			||	 iWear == WEAR_NECK
			||	 iWear == WEAR_WRIST
			||	 iWear == WEAR_TATTOO
			||	 iWear == WEAR_STUCK_IN )
			{
				for ( obj = vict->carrying; obj != NULL; obj = obj->next_content )
				{
					if ( obj->wear_loc == iWear )
					{
						if ( !found )
						{
							act( "$N sta usando:", ch, NULL, victim, TO_CHAR, POS_REST );
							send_to_char( "\n\r", ch );
							found = TRUE;
						}

						show_vwear_to_char( ch, obj );
					}
				}
			}
			else
			{
				if ( (obj = get_eq_char(vict, iWear)) != NULL )
				{
					if ( !found )
					{
						act( "$N sta usando:", ch, NULL, victim, TO_CHAR, POS_REST );
						send_to_char( "\n\r", ch );
						found = TRUE;
					}

					show_vwear_to_char( ch, obj );
				}
			}
		}

		if ( victim != ch && !IS_MOB(ch) && number_percent() < get_skill(ch, gsn_peek) )
		{
			send_to_char( "\n\rSbirci nell'inventario:\n\r", ch );
			check_improve( ch, gsn_peek, TRUE, 4 );
#ifdef MXP
			show_list_to_char( victim->carrying, ch, TRUE, TRUE, eItemNothing );
#else
			show_list_to_char( vict->carrying, ch, TRUE, TRUE );
#endif
		}
	}
}


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
	CHAR_DATA	*rch;
	int			life_count = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( rch = list; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch == ch )
			continue;

		if ( get_trust(ch) < rch->invis_level )
			continue;

		if ( can_see(ch, rch) )
			show_char_to_char_0( rch, ch );
		else if ( room_is_dark(ch) && IS_AFFECTED(rch, AFF_INFRARED) )
		{
			send_to_char( "Vedi due occhi rossi e incandescenti fissare te!\n\r", ch );
			if ( !IS_ADMIN(rch) )
				life_count++;
		}
		else if ( !IS_ADMIN(rch) )
			life_count++;
	}

	if ( life_count && CAN_DETECT(ch, DETECT_LIFE) )
	{
		char	buf[MSL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		sprintf( buf, "Senti %d %s di vita nel luogo.\n\r", life_count, (life_count == 1) ? "forma" : "forme" );
		send_to_char( buf, ch );
	}
}


bool check_blind( CHAR_DATA *ch )
{
	if ( !IS_MOB(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
		return( TRUE );

	if ( IS_AFFECTED(ch, AFF_BLIND) )
	{
		send_to_char( "Non vedi un tubo!\n\r", ch );
		return( FALSE );
	}

	return( TRUE );
}


void do_clear( CHAR_DATA *ch, char *argument )
{
	if ( !IS_MOB(ch) )
		send_to_char( "\033[0;0H\033[2J", ch );
}


/*
 * changes your scroll
 */
void do_scroll( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	char	buf[100];
	int		lines;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		if ( ch->lines == 0 )
			send_to_char( "Non puoi visualizzare messaggi cosi' lunghi.\n\r", ch );
		else
		{
			sprintf( buf, "Puoi visualizzare %d linee per pagina.\n\r", ch->lines + 2 );
			send_to_char( buf, ch );
		}

		return;
	}

	if ( !is_number(arg) )
	{
		send_to_char( "Devi inserire un numero.\n\r", ch );
		return;
	}

	lines = atoi( arg );

	if ( lines == 0 )
	{
		send_to_char( "Visualizzazione pagine disabilitata.\n\r", ch );
		ch->lines = 0;
		return;
	}

	if ( lines < 10 || lines > 100 )
	{
		send_to_char( "Devi inserire un numero ragionevole.\n\r", ch );
		return;
	}

	sprintf( buf, "Pagina lunga %d linee.\n\r", lines );
	send_to_char( buf, ch );
	ch->lines = lines - 2;
}


/*
 * RT does socials
 */
void do_socials( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	int		iSocial;
	int		col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	col = 0;

	for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ )
	{
		sprintf( buf, "%-14s", social_table[iSocial].name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
			send_to_char( "\n\r", ch );
	}

	if ( col % 6 != 0 )
		send_to_char( "\n\r", ch );
}


/*
 * RT Commands to replace news, motd, imotd, etc from ROM
 */
void do_motd( CHAR_DATA *ch, char *argument )
{
	do_help( ch, "motd" );
}


void do_imotd( CHAR_DATA *ch, char *argument )
{
	do_help( ch, "imotd" );
}


void do_rules( CHAR_DATA *ch, char *argument )
{
	do_help( ch, "rules" );
}


void do_wizlist( CHAR_DATA *ch, char *argument )
{
	do_help( ch, "wizlist" );
}


/*
 * RT this following section holds all the auto commands from ROM, as well as replacements for config
 */
void do_autolist( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* lists most player flags */
	if ( IS_MOB(ch) )
		return;

	sprintf( buf, "%sAzione               Stato\n\r", CLR(ch, BROWN) );
	send_to_char( buf, ch );
	sprintf( buf, "%s-------------------------\n\r", CLR(ch, CYAN_BOLD) );
	send_to_char( buf, ch );

	sprintf( buf, "%s| %sColori             ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_COLOR) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sAutoassist         ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_AUTOASSIST) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sAutoexit           ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_AUTOEXIT) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

#ifdef WEATHER
	sprintf( buf, "%s| %sAutoweather        ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_AUTOWEATHER) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
#endif

	sprintf( buf, "%s| %sAutogold           ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_AUTOGOLD) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sAutoloot           ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_AUTOLOOT) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sAutosac            ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_AUTOSAC) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sAutosplit          ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->act, PLR_AUTOSPLIT) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sModalita' compatta   ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->comm, COMM_COMPACT) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sPrompt             ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->comm, COMM_PROMPT) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s| %sCombinare oggetti  ",
		CLR(ch, BLUE_BOLD),
		CLR(ch, CYAN) );
	send_to_char( buf, ch );
	if ( IS_SET(ch->comm, COMM_COMBINE) )
	{
		sprintf( buf, "%s|  %sON  %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, GREEN_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "%s|  %sOFF %s|\n\r",
			CLR(ch, BLUE_BOLD),
			CLR(ch, RED_BOLD),
			CLR(ch, BLUE_BOLD) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "%s-------------------------%s\n\r",
		CLR(ch, CYAN_BOLD),
		CLR(ch, WHITE_BOLD) );
	send_to_char( buf, ch );

	if ( IS_SET(ch->act, PLR_NOSUMMON) )
	{
		sprintf( buf, "Puoi essere evocat$x solamente da coloro vicino al tuo livello." );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
	}
	else
	{
		sprintf( buf, "Puoi essere evocat$x da chiunque." );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
	}

	if ( IS_SET(ch->act, PLR_NOFOLLOW) )
	{
		sprintf( buf, "Non accetti che delle persone ti seguano.\n\r" );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "Accetti che delle persone ti seguano.\n\r" );
		send_to_char( buf, ch );
	}

	if ( IS_SET(ch->act, PLR_NOCANCEL) )
	{
		sprintf( buf, "Non accetti che altri utilizzino su di te l'incantesimo di cancellazione.\n\r" );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "Accetti che altri possano utilizzare su di te l'incantesimo di cancellazione.\n\r" );
		send_to_char( buf, ch );
	}

	if ( IS_SET(ch->act, PLR_ENGLISH) )
	{
		sprintf( buf, "Digiti i comandi in inglese." );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "Digiti i comandi in italiano." );
		send_to_char( buf, ch );
	}

	send_to_char( "\n\r", ch );
}


void do_autoassist( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_AUTOASSIST) )
	{
		send_to_char( "Ora non assisterai nei combattimenti chi ha bisogno di aiuto.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_AUTOASSIST );
	}
	else
	{
		send_to_char( "Assisterai chi ne ha bisogno.\n\r", ch );
		SET_BIT( ch->act, PLR_AUTOASSIST );
	}
}


void do_autoexit( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_AUTOEXIT) )
	{
		send_to_char( "Le uscite non saranno automaticamente visualizzate.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_AUTOEXIT );
	}
	else
	{
		send_to_char( "Vedrai le uscite automaticamente.\n\r", ch );
		SET_BIT( ch->act, PLR_AUTOEXIT );
	}
}


void do_autogold( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_AUTOGOLD) )
	{
		send_to_char( "Ora non raccoglierai automaticamente le monete dai cadaveri.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_AUTOGOLD );
	}
	else
	{
		send_to_char( "Raccoglierai le monete dai cadaveri.\n\r", ch );
		SET_BIT( ch->act, PLR_AUTOGOLD );
	}
}


void do_autoloot( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_AUTOLOOT) )
	{
		send_to_char( "Ora non prenderai automaticamente gli oggetti dai cadaveri.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_AUTOLOOT );
	}
	else
	{
		send_to_char( "Prenderai automaticamente gli oggetti dai cadaveri.\n\r", ch );
		SET_BIT( ch->act, PLR_AUTOLOOT );
	}
}


void do_autosac( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_AUTOSAC) )
	{
		send_to_char( "Ora non sacrificherai automaticamente i cadaveri.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_AUTOSAC );
	}
	else
	{
		send_to_char( "Sacrificherai i corpi degli sconfitti agli Dei.\n\r", ch );
		SET_BIT( ch->act, PLR_AUTOSAC );
	}
}


void do_autosplit( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_AUTOSPLIT) )
	{
		send_to_char( "Ora non dividerai automaticamente i soldi con il gruppo.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_AUTOSPLIT );
	}
	else
	{
		send_to_char( "Dividerai con il gruppo le monete raccolte.\n\r", ch );
		SET_BIT( ch->act, PLR_AUTOSPLIT );
	}
}


void do_color( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_COLOR) )
	{
		send_to_char( "Colori disabilitati.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_COLOR );
	}
	else
	{
		send_to_char( "Colori abilitati.\n\r", ch );
		SET_BIT( ch->act, PLR_COLOR );
	}
}


void do_brief( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->comm, COMM_BRIEF) )
	{
		send_to_char( "Descrizioni lunghe delle stanze attivate.\n\r", ch );
		REMOVE_BIT( ch->comm, COMM_BRIEF );
	}
	else
	{
		send_to_char( "Descrizioni brevi delle stanze attivate.\n\r", ch );
		SET_BIT( ch->comm, COMM_BRIEF );
	}
}


void do_compact( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->comm, COMM_COMPACT) )
	{
		send_to_char( "Modalita' compatta disattiva.\n\r", ch );
		REMOVE_BIT( ch->comm, COMM_COMPACT );
	}
	else
	{
		send_to_char( "Modalita' compatta attiva.\n\r", ch );
		SET_BIT( ch->comm, COMM_COMPACT );
	}
}


void do_show( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->comm, COMM_SHOW_AFFECTS) )
	{
		send_to_char( "Non vedrai gli effetti degli incantesimi nel tuo stato.\n\r", ch );
		REMOVE_BIT( ch->comm, COMM_SHOW_AFFECTS );
	}
	else
	{
		send_to_char( "Potrai vedere gli effetti degli incantesimi nel tuo stato.\n\r", ch );
		SET_BIT( ch->comm, COMM_SHOW_AFFECTS );
	}
}


void do_prompt( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		if ( IS_SET(ch->comm, COMM_PROMPT) )
		{
			send_to_char( "D'ora in poi non visualizzerai piu' il prompt.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_PROMPT );
		}
		else
		{
			send_to_char( "Ora visualizzerai il prompt.\n\r", ch );
			SET_BIT( ch->comm, COMM_PROMPT );
		}

		return;
	}

	if ( !str_cmp(argument, "all") || !str_cmp(argument, "tutto") )
		strcpy( buf, "<%n: %h vita %v mov %m mana Opp:(%o)>" );
	else
	{
		if ( strlen(argument) > 160 )
			argument[160] = '\0';
		strcpy( buf, argument );
		smash_tilde( buf );
		if ( str_suffix("%c", buf) )
			strcat( buf, " " );
	}

	free_string( ch->prompt );
	ch->prompt = str_dup( buf );
	sprintf( buf, "Prompt modificato a: %s\n\r", ch->prompt );
	send_to_char( buf, ch );
}


void do_combine( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->comm, COMM_COMBINE) )
	{
		send_to_char( "Vedrai gli oggetti uno ad uno.\n\r", ch );
		REMOVE_BIT( ch->comm, COMM_COMBINE );
	}
	else
	{
		send_to_char( "Vedrai gli oggetti combinati tra loro.\n\r", ch );
		SET_BIT( ch->comm, COMM_COMBINE );
	}
}


void do_nofollow( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;
	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		send_to_char( "Non vorrai lasciare da solo il tuo incommensurabile maestro??\n\r", ch );
		return;
	}

	if ( IS_SET(ch->act, PLR_NOFOLLOW) )
	{
		send_to_char( "Ora accetti che delle persone ti seguano.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_NOFOLLOW );
	}
	else
	{
		send_to_char( "Non accetti pu' che persone ti seguano.\n\r", ch );
		SET_BIT( ch->act, PLR_NOFOLLOW );
		die_follower( ch );
	}
}


void do_nosummon( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
	{
		if ( IS_SET(ch->imm_flags, IMM_SUMMON) )
		{
			act( "Puoi essere evocat$x da chiunque.", ch, NULL, NULL, TO_CHAR, POS_REST );
			REMOVE_BIT( ch->imm_flags, IMM_SUMMON );
		}
		else
		{
			send_to_char( "Sei immune alle evocazioni.\n\r", ch );
			SET_BIT( ch->imm_flags, IMM_SUMMON );
		}
	}
	else
	{
		if ( IS_SET(ch->act, PLR_NOSUMMON) )
		{
			act( "Puoi essere evocat$x da coloro vicino al tuo livello.", ch, NULL, NULL, TO_CHAR, POS_REST );
			REMOVE_BIT( ch->act, PLR_NOSUMMON );
		}
		else
		{
			act( "Puoi essere evocat$x da chiunque vicino al tuo livello.", ch, NULL, NULL, TO_CHAR, POS_REST );
			SET_BIT( ch->act, PLR_NOSUMMON );
		}
	}
}


void do_english( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_ENGLISH) )
	{
		send_to_char( "Ora potrai inviare i comandi nella lingua italiana.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_ENGLISH );
	}
	else
	{
		send_to_char( "Ora potrai inviare i comandi nella lingua inglese.\n\r", ch );
		SET_BIT( ch->act, PLR_ENGLISH );
	}

#ifdef MXP
	/* Resetta i menu' MXP secondo il linguaggio scelto dal pg */
	mxp_toggle_language( ch->desc );
#endif
}


void do_look( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg1[MIL];
	char		arg2[MIL];
	char		arg3[MIL];
	EXIT_DATA	*pexit;
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	char		*pdesc;
	int			door;
	int			number, count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->desc == NULL )
		return;

	if ( ch->position < POS_SLEEP )
	{
		send_to_char( "Non vedi un tubo.. anzi no.. delle stelle..\n\r", ch );
		return;
	}

	if ( ch->position == POS_SLEEP )
	{
		send_to_char( "Non puoi vedere nulla, stai dormendo!\n\r", ch );
		return;
	}

	if ( !check_blind(ch) )
		return;

	if ( !IS_MOB(ch) && !IS_SET(ch->act, PLR_HOLYLIGHT) && room_is_dark(ch) )
	{
		send_to_char( "E' buio come la pece.. \n\r", ch );
		show_char_to_char( ch->in_room->people, ch );
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	number = number_argument( arg1, arg3 );
	count = 0;

	if ( arg1[0] == '\0' || !str_cmp(arg1, "auto") )
	{
		if ( !IS_SET(ch->comm, COMM_COMPACT) )
			send_to_char( "\n\r", ch );

		/* 'look' or 'look auto' */
		send_to_char( CLR(ch, RED_BOLD), ch );
		send_to_char( ch->in_room->name, ch );
		send_to_char( CLR(ch, NORMAL), ch );

		if ( IS_ADMIN(ch) && (IS_MOB(ch) || IS_SET(ch->act, PLR_HOLYLIGHT)) )
		{
			sprintf( buf, " [Vnum Stanza %d]", ch->in_room->vnum );
			send_to_char( buf, ch );
		}

		send_to_char( "\n\r", ch );

		if ( arg1[0] == '\0' || (!IS_MOB(ch) && !IS_SET(ch->comm, COMM_BRIEF)) )
		{
			send_to_char( "  ", ch );
			send_to_char( ch->in_room->description, ch );
		}

#ifdef WEATHER
		if ( IS_SET(ch->act, PLR_AUTOWEATHER) && IS_OUTSIDE(ch) )
			show_weather( ch );
#endif

		/* if ( !IS_SET(ch->comm,COMM_COMPACT) ) send_to_char( "\n\r", ch ); */
		if ( !IS_MOB(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
			do_exits( ch, "auto" );
		else
			do_exits( ch, "" );

#ifdef MXP
		show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE, eItemGet );
#else
		show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
#endif
		show_char_to_char( ch->in_room->people, ch );
		return;
	}

	if ( !str_cmp(arg1, "in") || !str_cmp(arg1, "dentro") )
	{
		/* 'look in' */
		if ( arg2[0] == '\0' )
		{
			send_to_char( "Guardare dentro cosa?\n\r", ch );
			return;
		}

		if ( (obj = get_obj_here(ch, arg2)) == NULL )
		{
			send_to_char( "Non vedi nulla di cio' qui.\n\r", ch );
			return;
		}

		switch ( obj->item_type )
		{
		default:
			send_to_char( "Questo non e' un contenitore.\n\r", ch );
			break;

		case ITEM_DRINK_CON:
			if ( obj->value[1] <= 0 )
			{
				send_to_char( "E' vuoto.\n\r", ch );
				break;
			}

			sprintf( buf, "E' riempito %s con un liquido %s.\n\r",
				obj->value[1] < obj->value[0]/4 ?
					"meno della meta'" : obj->value[1] < 3 * obj->value[0]/4 ?
					"per meta'" : "piu' della meta'",
				liq_table[obj->value[2]].liq_color );

			send_to_char( buf, ch );
			break;

		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			if ( IS_SET(obj->value[1], CONT_CLOSED) )
			{
				send_to_char( "E' chiuso.\n\r", ch );
				break;
			}

			act( "$p contiene:", ch, obj, NULL, TO_CHAR, POS_REST );
#ifdef MXP
			show_list_to_char( obj->contains, ch, TRUE, TRUE, eItemNothing );
#else
			show_list_to_char( obj->contains, ch, TRUE, TRUE );
#endif
			break;
		}

		return;
	}

	if ( (victim = get_char_room(ch, arg1)) != NULL )
	{
		show_char_to_char_1( victim, ch, FALSE );

		/* Love potion */
		if ( is_affected(ch, gsn_love_potion) && (victim != ch) )
		{
			AFFECT_DATA af;
			/*~~~~~~~~~~~*/

			affect_strip( ch, gsn_love_potion );

			if ( ch->master )
				stop_follower( ch );
			add_follower( ch, victim );
			ch->leader = victim;

			af.where = TO_AFFECTS;
			af.type = gsn_charm_person;
			af.level = ch->level;
			af.duration = number_fuzzy( victim->level / 4 );
			af.bitvector = AFF_CHARM;
			af.modifier = 0;
			af.location = 0;
			affect_to_char( ch, &af );

			act( "$n.. e' cosi' carino!", victim, NULL, ch, TO_VICT, POS_REST );
			act( "$N ti guarda con occhi adoranti.", victim, NULL, ch, TO_CHAR, POS_REST );
			act( "$N guarda $n con occhi adoranti.", victim, NULL, ch, TO_NOTVICT, POS_REST );
		}

		return;
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) )
		{	/* player can see object */
			pdesc = get_extra_descr( arg3, obj->extra_descr );
			if ( pdesc != NULL )
			{
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return;
				}
				else
					continue;
			}

			pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
			if ( pdesc != NULL )
			{
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return;
				}
				else
					continue;
			}

			if ( is_name(arg3, obj->name) )
			{
				if ( ++count == number )
				{
					send_to_char( "Non vedi nulla di speciale.\n\r", ch );
					return;
				}
			}
		}
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
	{
		if ( can_see_obj(ch, obj) )
		{
			pdesc = get_extra_descr( arg3, obj->extra_descr );
			if ( pdesc != NULL )
			{
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return;
				}
			}

			pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
			if ( pdesc != NULL )
			{
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return;
				}
			}
		}

		if ( is_name(arg3, obj->name) )
		{
			if ( ++count == number )
			{
				send_to_char( obj->description, ch );
				send_to_char( "\n\r", ch );
				return;
			}
		}
	}

	pdesc = get_extra_descr( arg3, ch->in_room->extra_descr );
	if ( pdesc != NULL )
	{
		if ( ++count == number )
		{
			send_to_char( pdesc, ch );
			return;
		}
	}

	if ( count > 0 && count != number )
	{
		if ( count == 1 )
			sprintf( buf, "Vedi un unico %s qui.\n\r", arg3 );
		else
			sprintf( buf, "Vedi %d di questi qua.\n\r", count );

		send_to_char( buf, ch );
		return;
	}

	door = find_exit( ch, arg1, FALSE );
	if ( door == -1 )
	{
		send_to_char( "Non vedi nulla del genere qui.\n\r", ch );
		return;
	}

	/* 'look direction' */
	if ( (pexit = ch->in_room->exit[door]) == NULL )
	{
		send_to_char( "Nulla di speciale.\n\r", ch );
		return;
	}

	if ( pexit->description != NULL && pexit->description[0] != '\0' )
		send_to_char( pexit->description, ch );
	else
		send_to_char( "Nulla di speciale.\n\r", ch );

	if ( pexit->keyword != NULL && pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ' )
	{
		if ( IS_SET(pexit->exit_info, EX_CLOSED) )
			act( "L'uscita e' chiusa.", ch, NULL, pexit->keyword, TO_CHAR, POS_REST );
		else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
			act( "L'uscita e' aperta.", ch, NULL, pexit->keyword, TO_CHAR, POS_REST );
	}
}


/*
 * RT added back for the hell of it
 */
void do_read( CHAR_DATA *ch, char *argument )
{
	do_look( ch, argument );
}


void do_examine( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MIL];
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Esaminare cosa?\n\r", ch );
		return;
	}

	do_look( ch, arg );

	if ( (obj = get_obj_here(ch, arg)) != NULL )
	{
		switch ( obj->item_type )
		{
		default:
			break;

		case ITEM_MONEY:
			if ( obj->value[0] == 0 )
			{
				if ( obj->value[1] == 0 )
					sprintf( buf, "Che strano.. non vi sono monete nel mucchio.\n\r" );
				else if ( obj->value[1] == 1 )
					sprintf( buf, "Wow. Una moneta d'oro.\n\r" );
				else
					sprintf( buf, "Vi sono %d monete d'oro.\n\r", obj->value[1] );
			}
			else if ( obj->value[1] == 0 )
			{
				if ( obj->value[0] == 1 )
					sprintf( buf, "Wow. Una moneta d'argento.\n\r" );
				else
					sprintf( buf, "Vi sono %d monete d'argento.\n\r", obj->value[0] );
			}
			else
				sprintf( buf, "Vi sono %d monet%c d'oro e %d monet%c d'argento.\n\r",
					obj->value[1], (obj->value[1] == 1) ? 'a' : 'e',
					obj->value[0], (obj->value[0] == 1) ? 'a' : 'e');
			send_to_char( buf, ch );
			break;

		case ITEM_DRINK_CON:
		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			sprintf( buf, "in %s", argument );
			do_look( ch, buf );
		}
	}
}


/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
	extern char *const	dir_name[];
	extern char *const	dir_name_en[];
	char				buf[MSL];
	char				color[MIL];
	EXIT_DATA			*pexit;
	bool				found;
	bool				fAuto;
	int					door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fAuto = !str_cmp( argument, "auto" );

	if ( !check_blind(ch) )
		return;

	sprintf( buf, "%sUscite:%s%s",
		CLR(ch, BLUE_BOLD),
		CLR(ch, NORMAL),
		fAuto ? " " : "\n\r" );

	found = FALSE;
	for ( door = 0; door < MAX_DIR; door++ )
	{
		switch ( door )
		{
		case 0:	sprintf( color, CLR(ch, CYAN) );		break;
		case 1:	sprintf( color, CLR(ch, YELLOW) );		break;
		case 2:	sprintf( color, CLR(ch, WHITE) );		break;
		case 3:	sprintf( color, CLR(ch, RED) );			break;
		case 4:	sprintf( color, CLR(ch, CYAN_BOLD) );	break;
		case 5:	sprintf( color, CLR(ch, BROWN) );		break;
		case 6:	sprintf( color, CLR(ch, GREEN) );		break;
		case 7:	sprintf( color, CLR(ch, GREEN_BOLD) );	break;
		case 8:	sprintf( color, CLR(ch, BLUE) );		break;
		case 9:	sprintf( color, CLR(ch, MAGENTA) );		break;
		}

		if ((pexit = ch->in_room->exit[door]) != NULL
		&&	pexit->to_room != NULL
		&&	can_see_room(ch, pexit->to_room)
		&&	!IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			found = TRUE;
			if ( fAuto )
			{
				strcat( buf, " " );
				strcat( buf, color );
#ifdef MXP
				strcat( buf, MXPTAG("Ex") );
#endif
				strcat( buf, (IS_SET(ch->act, PLR_ENGLISH))  ?  dir_name_en[door]  :  dir_name[door] );
#ifdef MXP
				strcat( buf, MXPTAG("/Ex") );
#endif
				strcat( buf, CLR(ch, NORMAL) );
			}
			else
			{
				sprintf( buf + strlen(buf),
					"%s"
#ifdef MXP
					MXPTAG("Ex")
#endif
					"%-11s"
#ifdef MXP
					MXPTAG("/Ex")
#endif
					"%s - %s",
					color,
					(IS_SET(ch->act, PLR_ENGLISH))  ?  capitalize(dir_name_en[door])  :  capitalize(dir_name[door]),
					CLR(ch, NORMAL),
					room_dark(pexit->to_room) ? "E' buio come la pece" : pexit->to_room->name );

				if ( IS_ADMIN(ch) )
					sprintf( buf + strlen(buf), " (stanza %d)\n\r", pexit->to_room->vnum );
				else
					sprintf( buf + strlen(buf), "\n\r" );
			}
		}

		if (number_percent() < get_skill(ch, gsn_perception)
		&&	(pexit = ch->in_room->exit[door]) != NULL
		&&	pexit->to_room != NULL
		&&	can_see_room(ch, pexit->to_room)
		&&	IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			check_improve( ch, gsn_perception, TRUE, 5 );
			found = TRUE;
			if ( fAuto )
			{
				strcat( buf, " *" );
				strcat( buf, color );
#ifdef MXP
				strcat( buf, MXPTAG("Ex") );
#endif
				strcat( buf, dir_name[door] );
#ifdef MXP
				strcat( buf, MXPTAG("/Ex") );
#endif
				strcat( buf, CLR(ch, NORMAL) );
				strcat( buf, "*" );
			}
			else
			{
				sprintf( buf + strlen(buf),
					"(%s"
#ifdef MXP
					MXPTAG("Ex")
#endif
					"%-9s"
#ifdef MXP
					MXPTAG("/Ex")
#endif
					"%s) - %s",
					color,
					capitalize(dir_name[door]),
					CLR(ch, NORMAL),
					pexit->keyword );

				if ( IS_ADMIN(ch) )
					sprintf( buf + strlen(buf), " (stanza %d)\n\r", pexit->to_room->vnum );
				else
					sprintf( buf + strlen(buf), "\n\r" );
			}
		}
	}

	if ( !found )
		strcat( buf, fAuto ? " nessuna" : "- Nessuna.\n\r" );

	if ( fAuto )
	{
		strcat( buf, "\n\r" );
		strcat( buf, CLR(ch, WHITE_BOLD) );
	}

	send_to_char( buf, ch );

/*	if ( !IS_SET(ch->comm,COMM_COMPACT) ) send_to_char( "\n\r", ch ); */
}


void do_worth( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	int		total_played;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Possiedi %ld monet%c d'oro e %ld monet%c d'argento.\n\r",
		ch->gold, (ch->gold == 1) ? 'a' : 'e',
		ch->silver, (ch->silver == 1) ? 'a' : 'e' );
	send_to_char( buf, ch );

	if ( IS_MOB(ch) )
		return;

	sprintf( buf, "Possiedi %d di esperienza.\n\r", ch->exp );
	send_to_char( buf, ch );

	sprintf( buf, "Hai ucciso %d %s e %d %s.\n\r",
		ch->pcdata->has_killed,
		IS_GOOD(ch) ? "non-buoni" : IS_EVIL(ch) ? "non-malvagi" : "non-neutrali",
		ch->pcdata->anti_killed,
		IS_GOOD(ch) ? "buoni" : IS_EVIL(ch) ? "malvagi" : "neutrali" );
	send_to_char( buf, ch );

	total_played = get_total_played( ch );
	sprintf( buf, "Negli ultimi %d giorni, hai giocato %d or%c e %d minut%c.\n\r",
		MAX_TIME_LOG,
		(int) (total_played / 60), ((int) (total_played / 60) == 1) ? 'a' : 'e',
		(total_played % 60), ((total_played % 60) == 1) ? 'o' : 'i' );
	send_to_char( buf, ch );

	if ( IS_ADMIN(ch) )
	{
		int l, today, d_time;
		/*~~~~~~~~~~~~~~~~~*/

		for ( l = 0; l < MAX_TIME_LOG; l++ )
		{
			today  = get_played_day( l );
			d_time = get_played_time( ch, l );
			sprintf( buf, "  Giorno: %3d  Tempo giocato: %3d nimut%c.\n\r",
				today, d_time, (d_time == 1) ? 'o' : 'i' );
			send_to_char( buf, ch );
		}
	}
}


char *const day_name[] =
{
	"della Luna",
	"del Toro",
	"dell'Inganno",
	"del Tuono",
	"della Liberta'",
	"degli Dei",
	"del Sole"
};

char *const month_name[] =
{
	"dell'Inverno",
	"dell'Inverno del Lupo",
	"del Gigante di Ghiaccio",
	"dell'Arcano Passato",
	"della Grande Lotta",
	"della Primavera",
	"della Natura",
	"della Futilita'",
	"del Dragone",
	"del Sole",
	"dell'Estate",
	"della Battaglia",
	"delle Scure Ombre",
	"delle Ombre",
	"delle Lunghe Ombre",
	"dell'Antica Oscurita'",
	"del Grande Male"
};

#define COLOR_DAWN		(IS_SET(ch->act, PLR_COLOR)) ? "\033[1;34;44m" : ""
#define COLOR_MORNING	(IS_SET(ch->act, PLR_COLOR)) ? "\033[1;37;44m" : ""
#define COLOR_DAY		(IS_SET(ch->act, PLR_COLOR)) ? "\033[1;33;44m" : ""
#define COLOR_EVENING	(IS_SET(ch->act, PLR_COLOR)) ? "\033[1;31;44m" : ""
#define COLOR_NIGHT		(IS_SET(ch->act, PLR_COLOR)) ? "\033[0;30;44m" : ""

void do_date( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	char	buf2[MSL];
	char	*suf;
	int		hour;
	int		day;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hour = time_info.hour;
	day = time_info.day + 1;

	if		( hour > 0 && hour < 5 )		suf = "la notte";
	else if ( hour > 4 && hour < 12 )		suf = "il mattino";
	else if ( hour == 12 )					suf = "E' mezzogiorno";
	else if ( hour > 12 && hour < 18 )		suf = "il pomeriggio";
	else if ( hour > 17 && hour < 22 )		suf = "la sera";
	else if ( hour > 21 && hour < 24 )		suf = "la notte";
	else									suf = "E' mezzanotte";

	sprintf( buf, "E' %s del giorno %s, l'%d del mese %s.\n\r",
		suf, day_name[day % 7], day, month_name[time_info.month] );
	send_to_char( buf, ch );

	sprintf( buf, "E' $C%s$c.",
		(hour >= 5 && hour < 9)	  ? "l'alba" :
		(hour >= 9 && hour < 12)  ? "mattina" :
		(hour >= 12 && hour < 18) ? "pomeriggio" :
		(hour >= 18 && hour < 21) ? "sera" : "notte" );

	act( buf, ch, NULL, NULL, TO_CHAR, POS_REST,
		(hour >= 5 && hour < 9)	  ? COLOR_DAWN :
		(hour >= 9 && hour < 12)  ? COLOR_MORNING :
		(hour >= 12 && hour < 18) ? COLOR_DAY :
		(hour >= 18 && hour < 21) ? COLOR_EVENING : COLOR_NIGHT,
		CLR(ch, NORMAL) );

	if ( !IS_ADMIN(ch) )
		return;

	sprintf( buf2, "%s", (char *) ctime(&boot_time) );
	sprintf( buf, "Xandra ha eseguito il Reboot alle %s\n\rL'ora di sistema e' %s\n\r",
		buf2, (char *) ctime(&current_time) );

	send_to_char( buf, ch );
}


#ifndef WEATHER
void do_weather( CHAR_DATA *ch, char *argument )
{
	char				buf[MSL];
	static char *const	sky_look[4] = { "sereno", "nuvoloso", "grigio", "illuminato da qualche lampo ogni tanto" };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_OUTSIDE(ch) )
	{
		send_to_char( "Non puoi conoscere che tempo fa al chiuso.\n\r", ch );
		return;
	}

	sprintf( buf, "Il cielo e' %s e %s.\n\r",
		sky_look[weather_info.sky],
		weather_info.change >= 0 ? "una calda brezza proveniente da sud spira delicatamente." :
			"soffia un gelido vento settentrionale." );
	send_to_char( buf, ch );
}
#endif


void do_help( CHAR_DATA *ch, char *argument )
{
	HELP_DATA	*pHelp;
	char		argall[MIL], argone[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
		argument = "summary";

	/* this parts handles help a b so that it returns help 'a b' */
	argall[0] = '\0';
	while ( argument[0] != '\0' )
	{
		argument = one_argument( argument, argone );
		if ( argall[0] != '\0' )
			strcat( argall, " " );
		strcat( argall, argone );
	}

	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
	{
		if ( pHelp->level > get_trust(ch) )
			continue;

		if ( is_name(argall, pHelp->keyword) )
		{
			if ( pHelp->level >= 0 && str_cmp(argall, "IMOTD") )
			{
				send_to_char( pHelp->keyword, ch );
				send_to_char( "\n\r", ch );
			}

			/* Strip leading '.' to allow initial blanks. */
			if ( pHelp->text[0] == '.' )
				page_to_char( pHelp->text + 1, ch );
			else
				page_to_char( pHelp->text, ch );
			return;
		}
	}

	send_to_char( "Nessun aiuto per questa parola.\n\r", ch );
}


void do_inventory( CHAR_DATA *ch, char *argument )
{
	send_to_char( "\n\rStai trasportando:\n\r", ch );
#ifdef MXP
	show_list_to_char( ch->carrying, ch, TRUE, TRUE, eItemDrop );
#else
	show_list_to_char( ch->carrying, ch, TRUE, TRUE );
#endif
}


void do_equipment( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	CHAR_DATA	*victim;
	int			iWear;
	bool		found;
	/*~~~~~~~~~~~~~~*/

	if ( !argument || argument[0] == '\0' )
	{
		send_to_char( "Stai usando:\n\r", ch );
		found = FALSE;
		for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
		{
			if ( iWear == WEAR_FINGER || iWear == WEAR_NECK || iWear == WEAR_WRIST || iWear == WEAR_TATTOO || iWear == WEAR_STUCK_IN )
			{
				for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
				{
					if ( obj->wear_loc == iWear && show_cwear_to_char(ch, obj) )
						found = TRUE;
				}
			}
			else
			{
				if ( (obj = get_eq_char(ch, iWear)) != NULL && show_cwear_to_char(ch, obj) )
					found = TRUE;
			}
		}

		if ( !found )
			send_to_char( "Nulla.\n\r", ch );
	}
	else
	{
		if ( (victim = get_char_room(ch, argument)) != NULL )
		{
			show_char_to_char_1( victim, ch, TRUE );
			return;
		}
	}
}


void do_compare( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	OBJ_DATA	*obj1;
	OBJ_DATA	*obj2;
	int			value1;
	int			value2;
	char		*msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	if ( arg1[0] == '\0' )
	{
		send_to_char( "Comparare cosa con cosa?\n\r", ch );
		return;
	}

	if ( (obj1 = get_obj_carry(ch, arg1)) == NULL )
	{
		send_to_char( "Non possiedi il primo oggetto.\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' )
	{
		for ( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content )
		{
			if (obj2->wear_loc != WEAR_NONE
			&&	can_see_obj(ch, obj2)
			&&	obj1->item_type == obj2->item_type
			&&	(obj1->wear_flags & obj2->wear_flags &~ITEM_TAKE) != 0 )
				break;
		}

		if ( obj2 == NULL )
		{
			send_to_char( "non indossi nulla di simile da poterlo comparare.\n\r", ch );
			return;
		}
	}
	else if ( (obj2 = get_obj_carry(ch, arg2)) == NULL )
	{
		send_to_char( "Non possiedi questo oggetto.\n\r", ch );
		return;
	}

	msg = NULL;
	value1 = 0;
	value2 = 0;

	if ( obj1 == obj2 )
		msg = "Compari $p a se stesso. Chissa' come mai sembra identico..";
	else if ( obj1->item_type != obj2->item_type )
		msg = "Non puoi comparare $p and $P.";
	else
	{
		switch ( obj1->item_type )
		{
		default:
			msg = "Non puoi comparare $p and $P.";
			break;

		case ITEM_ARMOR:
			value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
			value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
			break;

		case ITEM_WEAPON:
			if ( obj1->pIndexData->new_format )
				value1 = ( 1 + obj1->value[2] ) * obj1->value[1];
			else
				value1 = obj1->value[1] + obj1->value[2];

			if ( obj2->pIndexData->new_format )
				value2 = ( 1 + obj2->value[2] ) * obj2->value[1];
			else
				value2 = obj2->value[1] + obj2->value[2];
			break;
		}
	}

	if ( msg == NULL )
	{
		if		( value1 == value2 )	msg = "$p e $P risultano simili.";
		else if ( value1 > value2 )		msg = "$p sembra migliore di $P.";
		else							msg = "$p sembra peggiore di $P.";
	}

	act( msg, ch, obj1, obj2, TO_CHAR, POS_REST );
}


void do_credits( CHAR_DATA *ch, char *argument )
{
	do_help( ch, "diku" );
}


void do_where( CHAR_DATA *ch, char *argument )
{
	char			buf[MSL];
	char			arg[MIL];
	CHAR_DATA		*victim;
	DESCRIPTOR_DATA *d;
	bool			found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( !check_blind(ch) )
		return;

	if ( room_is_dark(ch) && !IS_SET(ch->act, PLR_HOLYLIGHT) )
	{
		send_to_char( "Troppo buio per saperlo.\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "protettori") || !str_cmp(arg, "protector") )
	{
		if ( IS_SET(ch->in_room->area->area_flag, AREA_PROTECTED) )
			send_to_char( "Non vi sono protettori nell'area.\n\r", ch );
		else
			send_to_char( "Questa area non e' protetta.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Avventurieri nei pressi:\n\r", ch );
		found = FALSE;
		for ( d = descriptor_list; d; d = d->next )
		{
			if (d->connected == CON_PLAYING
			&&	(victim = d->character) != NULL
			&&	!IS_MOB(victim)
			&&	!is_safe_nomessage(ch, victim)
			&&	victim->in_room != NULL
			&&	victim->in_room->area == ch->in_room->area
			&&	can_see(ch, victim) )
			{
				found = TRUE;
				sprintf( buf, "%-28s %s\n\r",
					(is_affected(victim, gsn_doppelganger) && !IS_SET(ch->act, PLR_HOLYLIGHT)) ?
						victim->doppel->name : victim->name,
					victim->in_room->name );
				send_to_char( buf, ch );
			}
		}

		if ( !found )
			send_to_char( "Nessuno\n\r", ch );
	}
	else
	{
		found = FALSE;
		for ( victim = char_list; victim != NULL; victim = victim->next )
		{
			if (victim->in_room != NULL
			&&	victim->in_room->area == ch->in_room->area
			&&	!IS_AFFECTED(victim, AFF_HIDE)
			&&	!IS_AFFECTED(victim, AFF_FADE)
			&&	!IS_AFFECTED(victim, AFF_SNEAK)
			&&	can_see(ch, victim)
			&&	is_name(arg, victim->name) )
			{
				found = TRUE;
				sprintf( buf, "%-28s %s\n\r", PERS(victim, ch), victim->in_room->name );
				send_to_char( buf, ch );
				break;
			}
		}

		if ( !found )
			act( "Non trovi nessun $T.", ch, NULL, arg, TO_CHAR, POS_REST );
	}
}


void do_consider( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	char		*msg;
	char		*align;
	int			diff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Considerare di uccidere chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
	{
		send_to_char( "Non pensarci nemmeno.\n\r", ch );
		return;
	}

	diff = victim->level - ch->level;

	if		( diff <= -10 )	msg = "Ad una rapida occhiata immagini di poter sconfiggere $N combattendo al minimo delle tue potenzialita'.";
	else if ( diff <= -5 )	msg = "$N non ha alcuna speranza di batterti.";
	else if ( diff <= -2 )	msg = "$N potrebbe essere una facile preda per te.";
	else if ( diff <= 1 )	msg = "Confrontarti con $N darebbe luogo ad uno scontro piuttosto equilibrato";
	else if ( diff <= 4 )	msg = "Affrontalo solo se ti senti in piena forma :).";
	else if ( diff <= 9 )	msg = "Uno scontro con $N e' forse al di la' delle tue capacita'";
	else					msg = "Non e' proprio il caso di cercare guai con $N..";

	if		( IS_EVIL(ch) && IS_EVIL(victim) )			align = "$N ti guarda con disprezzo.";
	else if ( IS_GOOD(victim) && IS_GOOD(ch) )			align = "$N ti sorride serenamente.";
	else if ( IS_GOOD(victim) && IS_EVIL(ch) )			align = "$N ti sorride lievemente.";
	else if ( IS_EVIL(victim) && IS_GOOD(ch) )			align = "$N sogghigna verso di te.";
	else if ( IS_NEUTRAL(ch) && IS_EVIL(victim) )		align = "$N sogghigna maleficamente.";
	else if ( IS_NEUTRAL(ch) && IS_GOOD(victim) )		align = "$N sorride serenamente.";
	else if ( IS_NEUTRAL(ch) && IS_NEUTRAL(victim) )	align = "$N si guarda intorno in maniera disinteressata.";
	else												align = "$N si guarda intorno pigramente..";

	act( msg, ch, NULL, victim, TO_CHAR, POS_REST );
	act( align, ch, NULL, victim, TO_CHAR, POS_REST );
}


/*
 * Modifica il title per i pg.
 * Elenca i tile dei pg online e li può da da modificare per gli admin.
 */
void do_title( CHAR_DATA *ch, char *argument )
{
	if ( argument[0] == '\0' )
	{
		send_to_char( "Sintassi: title <titolo>\n\r", ch );
		send_to_char( "Sintassi: title cancella\n\r\n\r", ch );

		send_to_char( "Il titolo andra' al posto del nome del pg e verra' visto dai giocatori esterni il piu' delle volte quando compirai azioni.\n\r", ch );
		send_to_char( "Cancellandolo si riotterra' il nome pulito.\n\r", ch );
	}

	if ( !IS_ADMIN(ch) && strlen(argument) > 20 )
	{
		send_to_char( "Titolo troppo lungo. Massimo consentito e' di 20 caratteri\n\r", ch );
		return;
	}

	if ( !str_cmp(argument, "cancella") || !str_cmp(argument, "delete") )
	{
		free_string( ch->short_descr );
		ch->short_descr = str_dup( "" );
		send_to_char( "Titolo cancellato.\n\r", ch );
	}
	else
	{
		if ( !IS_ADMIN(ch) && !is_name(ch->name, argument) )
		{
			send_to_char( "Devi inserire il tuo nome nel titolo.\n\r", ch );
			return;
		}

		free_string( ch->short_descr );
		ch->short_descr = str_dup( argument );
		send_to_char( "Titolo modificato.\n\r", ch );
	}
}


void do_description( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] != '\0' )
	{
		buf[0] = '\0';
		smash_tilde( argument );

		if ( argument[0] == '-' )
		{
			int		len;
			bool	found = FALSE;
			/*~~~~~~~~~~~~~~~~~~*/

			if ( ch->description == NULL || ch->description[0] == '\0' )
			{
				send_to_char( "Nessuna linea da rimuovere.\n\r", ch );
				return;
			}

			strcpy( buf, ch->description );

			for ( len = strlen(buf); len > 0; len-- )
			{
				if ( buf[len] == '\r' )
				{
					if ( !found )	/* back it up */
					{
						if ( len > 0 )
							len--;
						found = TRUE;
					}
					else			/* found the second one */
					{
						buf[len + 1] = '\0';
						free_string( ch->description );
						ch->description = str_dup( buf );
						send_to_char( "La tua descrizione e':\n\r", ch );
						send_to_char( ch->description ? ch->description : "(Nulla).\n\r", ch );
						return;
					}
				}
			}

			buf[0] = '\0';
			free_string( ch->description );
			ch->description = str_dup( buf );
			send_to_char( "Descrizione cancellata.\n\r", ch );
			return;
		}

		if ( argument[0] == '+' )
		{
			if ( ch->description != NULL )
				strcat( buf, ch->description );
			argument++;
			while ( isspace(*argument) )
				argument++;
		}

		if ( strlen(buf) + strlen(argument) >= MSL - 2 )
		{
			send_to_char( "Descrizione troppo lunga.\n\r", ch );
			return;
		}

		strcat( buf, argument );
		strcat( buf, "\n\r" );
		free_string( ch->description );
		ch->description = str_dup( buf );
	}

	send_to_char( "La tua descrizione e':\n\r", ch );
	send_to_char( ch->description ? ch->description : "(Nulla).\n\r", ch );
}


/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	char	arg[MIL];
	int		wimpy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( ch->class == CLASS_SAMURAI && ch->level >= LEVEL_NEWBIE )
	{
		sprintf( buf, "Il tuo onore non ti permette minimamente di pensare a tali codardie.\n\r" );
		send_to_char( buf, ch );
		if ( ch->wimpy != 0 )
			ch->wimpy = 0;
		return;
	}

	if ( arg[0] == '\0' )
		wimpy = ch->max_point[LIFE] / 5;
	else
		wimpy = atoi( arg );

	if ( wimpy < 0 )
	{
		send_to_char( "Il tuo coraggio eccede la tua saggezza.\n\r", ch );
		return;
	}

	if ( wimpy > ch->max_point[LIFE] / 2 )
	{
		send_to_char( "Such cowardice ill becomes you.\n\r", ch );
		return;
	}

	ch->wimpy = wimpy;

	sprintf( buf, "La fuga avverra' quando avrai %d punti di vita.\n\r", wimpy );
	send_to_char( buf, ch );
}


void do_password( CHAR_DATA *ch, char *argument )
{
	char	arg1[MIL];
	char	arg2[MIL];
	char	*pArg;
	char	*pwdnew;
	char	*p;
	char	cEnd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	/* Can't use one_argument here because it smashes case. So we just steal all its code. Bleagh. */
	pArg = arg1;
	while ( isspace(*argument) )
		argument++;

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"' )
		cEnd = *argument++;

	while ( *argument != '\0' )
	{
		if ( *argument == cEnd )
		{
			argument++;
			break;
		}

		*pArg++ = *argument++;
	}

	*pArg = '\0';

	pArg = arg2;
	while ( isspace(*argument) )
		argument++;

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"' )
		cEnd = *argument++;

	while ( *argument != '\0' )
	{
		if ( *argument == cEnd )
		{
			argument++;
			break;
		}

		*pArg++ = *argument++;
	}

	*pArg = '\0';

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "\n\rSintassi: password <vecchia> <nuova>.\n\r", ch );
		return;
	}

	if ( strcmp(crypt(arg1, ch->pcdata->pwd), ch->pcdata->pwd) )
	{
		send_to_char( "Password errata.\n\r", ch );
		return;
	}

	if ( strlen(arg2) < 6 )
	{
		send_to_char( "La nuova password deve avere una lunghezza di almeno 6 caratteri.\n\r", ch );
		return;
	}

	/* No tilde allowed because of player file format. */
	pwdnew = crypt( arg2, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
		if ( *p == '~' )
		{
			send_to_char( "Nuova password non accettabile, prova ancora.\n\r", ch );
			return;
		}
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd = str_dup( pwdnew );
	save_char_obj( ch );
	send_to_char( "Hai cambiato la tua password.\n\r", ch );
}


/*
 * RT configure command
 */
void do_scan( CHAR_DATA *ch, char *argument )
{
	char			dir[MIL];
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA		*pexit;
	int				door;
	int				range;
	char			buf[MSL];
	int				i;
	CHAR_DATA		*person, *rch;
	int				numpeople;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, dir );

	if ( dir[0] == '\0' )
	{
		do_scan2( ch, "" );
		return;
	}

	/* Doppio comando per le dir, inglese-italiano */
	door = find_exit( ch, dir, FALSE );
	if ( door == -1 )
	{
		send_to_char( "Non vedi nulla del genere qui.\n\r", ch );
		return;
	}

	act( "Guardi $t.", ch, dir_name_to[door], NULL, TO_CHAR, POS_REST );
	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch == ch )
			continue;
		if ( IS_MOB(rch) )
			continue;
		if ( can_see(rch, ch) )
			act( "$t guarda $T.", rch, ch->name, dir_name_to[door], TO_CHAR, POS_REST );
	}

	if ( !check_blind(ch) )
		return;

	range = 1 + ( ch->level ) / 10;

	in_room = ch->in_room;

	for ( i = 1; i <= range; i++ )
	{
		if ( (pexit = in_room->exit[door]) == NULL
		|| (to_room = pexit->to_room) == NULL
		|| IS_SET(pexit->exit_info, EX_CLOSED) )
			return;

		for ( numpeople = 0, person = to_room->people; person != NULL; person = person->next_in_room )
		{
			if ( can_see(ch, person) )
				numpeople++;
		}

		if ( numpeople > 0 )
		{
			sprintf( buf, "***** Range %d *****\n\r", i );
			send_to_char( buf, ch );
			show_char_to_char( to_room->people, ch );
			send_to_char( "\n\r", ch );
		}
		else
			send_to_char( "Nessuno nei paraggi\n\r", ch );

		in_room = to_room;
	}
}


void do_request( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( is_affected(ch, gsn_reserved) )
	{
		send_to_char( "Aspetta un po' prima di chiedere ancora.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( IS_MOB(ch) )
		return;

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Richiedere cosa a chi ?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg2)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) )
	{
		send_to_char( "Perche' non chiederglielo direttamente?\n\r", ch );
		return;
	}

	if ( !IS_GOOD(ch) )
	{
		do_say( victim, "Non donero' nulla a una persona cosi' impura." );
		return;
	}

	if ( ch->point[MOVE] < (50 + ch->level) )
	{
		do_say( victim, "Sembri stanco morto, perche' non ti riposi un po' prima?" );
		return;
	}

	WAIT_STATE( ch, PULSE_VIOLENCE );
	ch->point[MOVE] -= 10;
	ch->point[MOVE] = UMAX( ch->point[MOVE], 0 );

	if ( victim->level >= ch->level + 10 || victim->level >= ch->level * 2 )
	{
		do_say( victim, "In tempi migliori, figliolo" );
		return;
	}

	if ( ((obj = get_obj_carry(victim, arg1)) == NULL && (obj = get_obj_wear(victim, arg1)) == NULL)
	||	IS_SET(obj->extra_flags, ITEM_INVENTORY) )
	{
		do_say( victim, "Spiacente, non possiedo cio'." );
		return;
	}

	if ( !IS_GOOD(victim) )
	{
		do_say( victim, "Non ho nessuna intenzione di darti cio'.. anzi.. te le do'!" );
		do_murder( victim, ch->name );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE )
		unequip_char( victim, obj );

	if ( !can_drop_obj(ch, obj) )
	{
		do_say( victim, "Cio' che desideri e' maledetto, non posso dartelo." );
		return;
	}

	if ( ch->carry_number + get_obj_number(obj) > can_carry_n(ch) )
	{
		send_to_char( "Hai le mani ingombro.\n\r", ch );
		return;
	}

	if ( ch->carry_weight + get_obj_weight(obj) > can_carry_w(ch) )
	{
		send_to_char( "Non puoi trasportare altri oggetti.\n\r", ch );
		return;
	}

	if ( !can_see_obj(ch, obj) )
	{
		act( "Non lo puoi vedere e non riesco a dartelo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	obj_from_char( obj );
	obj_to_char( obj, ch );
	act( "$n richiede $p da $N.", ch, obj, victim, TO_NOTVICT, POS_REST );
	act( "Richiedi $p da $N.", ch, obj, victim, TO_CHAR, POS_REST );
	act( "$n ti richiede $p.", ch, obj, victim, TO_VICT, POS_REST );

	if ( IS_SET(obj->progtypes, OPROG_GIVE) )
		( obj->pIndexData->oprogs->give_prog ) ( obj, ch, victim );

	if ( IS_SET(victim->progtypes, MPROG_GIVE) )
		( victim->pIndexData->mprogs->give_prog ) ( victim, ch, obj );

	ch->point[MOVE] -= ( 50 + ch->level );
	ch->point[MOVE] = UMAX( ch->point[MOVE], 0 );
	ch->point[LIFE] -= 3 * ( ch->level / 2 );
	ch->point[LIFE] = UMAX( ch->point[LIFE], 0 );

	act( "Ti senti riconoscente per la fiducia di $N.", ch, NULL, victim, TO_CHAR, POS_REST );

	af.type = gsn_reserved;
	af.where = TO_AFFECTS;
	af.level = ch->level;
	af.duration = ch->level / 10;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );
}


void do_hometown( CHAR_DATA *ch, char *argument )
{
	int		amount;
	char	buf[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
	{
		send_to_char( "Non puoi cambiare la tua patria!\n\r", ch );
		return;
	}

	if ( ORG_RACE(ch) == 11 )
	{
		send_to_char( "La tua Patria e' la Valle dei Titani!\n\r", ch );
		return;
	}

	if ( ch->class == CLASS_VAMPIRE || ch->class == CLASS_NECROMANCER )
	{
		send_to_char( "La tua Patria e' Old Midgaard!\n\r", ch );
		return;
	}

	if ( !IS_SET(ch->in_room->room_flags, ROOM_REGISTRY) )
	{
		send_to_char( "Per poter cambiare la tua stanza devi essere nella stanza del Registro.\n\r", ch );
		return;
	}

	amount = ( ch->level * ch->level * 250 ) + 1000;

	if ( argument[0] == '\0' )
	{
		sprintf( buf, "Questa operazione ti costera' %d monete d'oro.\n\r", amount );
		send_to_char( buf, ch );
		return;
	}

	if ( ch->gold < amount )
	{
		send_to_char( "Non hai monete a sufficienza per poter cambiare patria!\n\r", ch );
		return;
	}

	if ( !str_prefix(argument, "midgaard") )
	{
		if ( ch->hometown == 0 )
		{
			send_to_char( "Ma sei gia' un cittadino di Midgaard!\n\r", ch );
			return;
		}

		ch->gold -= amount;
		send_to_char( "La tua patria ora e' Midgaard.\n\r", ch );
		ch->hometown = 0;
		return;
	}
	else if ( !str_prefix(argument, "thalos") )
	{
		if ( ch->hometown == 1 )
		{
			send_to_char( "Ma sei gia' un cittadino di Nuova Thalos!\n\r", ch );
			return;
		}

		ch->gold -= amount;
		send_to_char( "La tua patria ora e' Nuova Thalos.\n\r", ch );
		ch->hometown = 1;
		return;
	}
	else if ( !str_prefix(argument, "titania") || !str_prefix(argument, "titans") )
	{
		if ( ch->hometown == 2 )
		{
			send_to_char( "Ma sei gia' un cittadino della Valle Titania!\n\r", ch );
			return;
		}

		ch->gold -= amount;
		send_to_char( "La tua patria ora e' Valle Titania.\n\r", ch );
		ch->hometown = 2;
		return;
	}
	else if ( !str_prefix(argument, "ofcol") )
	{
		if ( !IS_NEUTRAL(ch) )
		{
			send_to_char( "Solo le persone di animo neutralepossono vivere in Ofcol.\n\r", ch );
			return;
		}

		if ( ch->hometown == 3 )
		{
			send_to_char( "Ma sei gia' un cittadino di Ofcol!\n\r", ch );
			return;
		}

		ch->gold -= amount;
		send_to_char( "La tua patria ora e' Ofcol.\n\r", ch );
		ch->hometown = 3;
		return;
	}
	else if ( !str_prefix(argument, "midgaardvecchia") || !str_prefix(argument, "oldmidgaard") )
	{
		if ( ch->class == CLASS_VAMPIRE || ch->class == CLASS_NECROMANCER )
		{
			send_to_char( "Solo i vampiri e i necromanti possono vivere li'.\n\r", ch );
			return;
		}

		if ( ch->hometown == 4 )
		{
			send_to_char( "Ma sei gia' un cittadino di Midgaard Vecchia!\n\r", ch );
			return;
		}

		ch->gold -= amount;
		send_to_char( "La tua patria ora e' Midgaard Vecchia.\n\r", ch );
		ch->hometown = 4;
		return;
	}
	else
	{
		send_to_char( "Non e' una scelta valida.\n\r", ch );
		send_to_char( "Scegli tra: Midgaard, Thalos (Nuova), Titania (Valle), Ofcol (Nuova) e Midgaard (Vecchia).\n\r", ch );
		return;
	}
}


void do_detect_hidden( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_detect_hidden, 1, 0, NULL) )
		return;

	if ( CAN_DETECT(ch, DETECT_HIDDEN) )
	{
		send_to_char( "Sei gia' concentrato nel cogliere ogni minimo movimento sospetto. \n\r", ch );
		return;
	}

	if ( number_percent() > get_skill(ch, gsn_detect_hidden) )
	{
		send_to_char( "Scruti nell'oscurita' con attenzione ma non cogli nulla di sospetto.\n\r", ch );
		return;
	}

	af.where = TO_DETECTS;
	af.type = gsn_detect_hidden;
	af.level = ch->level;
	af.duration = ch->level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = DETECT_HIDDEN;
	affect_to_char( ch, &af );
	act( "Ti sei concentrat$x per cogliere qualsiasi movimento nell'ombra.", ch, NULL, NULL, TO_CHAR, POS_REST );

	/* (FF) Xandra, check_improve */
}


void do_bear_call( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*bear;
	CHAR_DATA	*bear2;
	AFFECT_DATA af;
	int			i;
	/*~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_bear_call, 1, 0, NULL) )
		return;

	send_to_char( "Richiami in tuo soccorso l'aiuto degli orsi.\n\r", ch );
	act( "$n emette uno strano richiamo gutturale.", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( is_affected(ch, gsn_bear_call) )
	{
		send_to_char( "Non hai abbastanza energie per richiamare altri orsi in tuo soccorso.\n\r", ch );
		return;
	}

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch) && IS_AFFECTED(gch, AFF_CHARM) && gch->master == ch && gch->pIndexData->vnum == MOB_VNUM_BEAR )
		{
			send_to_char( "Perche' vorresti un altro orso?Non ti va bene quello che hai?", ch );
			return;
		}
	}

	if ( ch->in_room != NULL && IS_SET(ch->in_room->room_flags, ROOM_NO_MOB) )
	{
		send_to_char( "Da qui nessun orso puo' sentirti.\n\r", ch );
		return;
	}

	if ( number_percent() > get_skill(ch, gsn_bear_call) )
	{
		send_to_char( "Nessun orso riesce a sentire il tuo richiamo.\n\r", ch );
		check_improve( ch, gsn_bear_call, TRUE, 1 );
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
	||	room_is_dt(ch)
	||	(	ch->in_room->sector_type != SECT_FIELD
		&&	ch->in_room->sector_type != SECT_FOREST
		&&	ch->in_room->sector_type != SECT_MOUNTAIN
		&&	ch->in_room->sector_type != SECT_HILLS)	)
	{
		send_to_char( "Nessun orso riesce a raggiungerti.\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 125 )
	{
		send_to_char( "Non hai abbastanza energie per il richiamo.\n\r", ch );
		return;
	}

	ch->point[MANA] -= 125;

	check_improve( ch, gsn_bear_call, TRUE, 1 );
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
	bear->level = UMIN( 100, 1 * ch->level - 2 );
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
	send_to_char( "Due orsi sono giunti in tuo soccorso!\n\r", ch );
	act( "Due orsi giungono dal nulla accerchaindo $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = gsn_bear_call;
	af.level = ch->level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );
}


void do_identify( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	CHAR_DATA	*rch;
	/*~~~~~~~~~~~~~*/

	if ( (obj = get_obj_carry(ch, argument)) == NULL )
	{
		send_to_char( "Non hai nessun oggetto con quel nome.\n\r", ch );
		return;
	}

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( IS_MOB(rch) && rch->pIndexData->vnum == MOB_VNUM_SAGE )
			break;
	}

	if ( !rch )
	{
		send_to_char( "Non c'e' chi possa dirti qualcosa riguardo quell'oggetto.\n\r", ch );
		return;
	}

	if ( IS_ADMIN(ch) )
		act( "$n ti guarda negli occhi titubante!", rch, obj, ch, TO_VICT, POS_REST );
	else if ( ch->gold < 1 )
	{
		act( "$n esamina scrupolosamente $p.", rch, obj, 0, TO_ROOM, POS_REST );
		send_to_char( "Serve almeno una moneta d'oro.\n\r", ch );
		return;
	}
	else
	{
		ch->gold -= 1;
		send_to_char( "Una moneta ben spesa.\n\r", ch );
	}

	act( "$n esamina scrupolosamente $p.", rch, obj, 0, TO_ROOM, POS_REST );
	spell_identify( 0, 0, ch, obj, 0 );
}


void do_affects( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA *paf, *paf_last = NULL;
	char		buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->affected != NULL )
	{
		send_to_char( "Sei affetto dai seguenti incantesimi:\n\r", ch );
		for ( paf = ch->affected; paf != NULL; paf = paf->next )
		{
			if ( paf_last != NULL && paf->type == paf_last->type )
			{
				if ( ch->level >= 20 )
					sprintf( buf, "                      " );
				else
					continue;
			}
			else
			{
				sprintf( buf, "%sIncantesimo%s: %s%-15s%s",
					CLR(ch, RED),
					CLR(ch, WHITE_BOLD),
					CLR(ch, YELLOW),
					skill_table[paf->type].name,
					CLR(ch, WHITE_BOLD) );
			}

			send_to_char( buf, ch );

			if ( ch->level >= 20 )
			{
				sprintf( buf, ": modifica %s%s%s da %s%d%s ",
					CLR(ch, MAGENTA),
					affect_loc_name(paf->location),
					CLR(ch, WHITE_BOLD),
					CLR(ch, MAGENTA),
					paf->modifier,
					CLR(ch, WHITE_BOLD) );
				send_to_char( buf, ch );
				if ( paf->duration == -1 || paf->duration == -2 )
					sprintf( buf, "%spermanentemente%s", CLR(ch, CYAN), CLR(ch, WHITE_BOLD) );
				else
				{
					sprintf( buf, "per %s%d%s ore",
						CLR(ch, MAGENTA),
						paf->duration,
						CLR(ch, WHITE_BOLD) );
				}

				send_to_char( buf, ch );
			}

			send_to_char( "\n\r", ch );
			paf_last = paf;
		}
	}
	else
		send_to_char( "Non sei affetto da alcun incantesimo.\n\r", ch );
}


void do_lion_call( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*gch;
	CHAR_DATA	*bear;
	CHAR_DATA	*bear2;
	AFFECT_DATA af;
	int			i;
	/*~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_lion_call, 1, 0, NULL) )
		return;

	send_to_char( "Chiami per dei leoni che ti aiutino.\n\r", ch );
	act( "$n urla il richiamo dei leoni.", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( is_affected(ch, gsn_lion_call) )
	{
		send_to_char( "Non hai la forza necessaria per chiamare altri leoni per ora..\n\r", ch );
		return;
	}

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
		if ( IS_MOB(gch) && IS_AFFECTED(gch, AFF_CHARM) && gch->master == ch && gch->pIndexData->vnum == MOB_VNUM_LION )
		{
			send_to_char( "Cosa c'e' che non va con il leone al tuo fianco?", ch );
			return;
		}
	}

	if ( ch->in_room != NULL && IS_SET(ch->in_room->room_flags, ROOM_NO_MOB) )
	{
		send_to_char( "Nessun leone ti sente.\n\r", ch );
		return;
	}

	if ( number_percent() > get_skill(ch, gsn_lion_call) )
	{
		send_to_char( "Nessun lenone ti sente.\n\r", ch );
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
	||	room_is_dt(ch)
	||	(	ch->in_room->sector_type != SECT_FIELD
		&&	ch->in_room->sector_type != SECT_FOREST
		&&	ch->in_room->sector_type != SECT_MOUNTAIN
		&&	ch->in_room->sector_type != SECT_HILLS)	)
	{
		send_to_char( "Nessun leoni giunge al tuo richiamo.\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 125 )
	{
		send_to_char( "Non possiedi abbastanza energie per chiamare i leoni.\n\r", ch );
		return;
	}

	ch->point[MANA] -= 125;

	bear = create_mobile( get_mob_index(MOB_VNUM_LION) );

	for ( i = 0; i < MAX_STATS; i++ )
		bear->perm_stat[i] = UMIN( 25, 2 * ch->perm_stat[i] );

	bear->max_point[LIFE] = IS_MOB( ch ) ? ch->max_point[LIFE] : ch->pcdata->perm_point[LIFE];
	bear->point[LIFE] = bear->max_point[LIFE];
	bear->max_point[MANA] = IS_MOB( ch ) ? ch->max_point[MANA] : ch->pcdata->perm_point[MANA];
	bear->point[MANA] = bear->max_point[MANA];
	bear->alignment = ch->alignment;
	bear->level = UMIN( 100, 1 * ch->level - 2 );
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
	send_to_char( "Due leoni giungono in tuo aiuto!\n\r", ch );
	act( "Due leoni giungono ad aiutare $n!", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = gsn_lion_call;
	af.level = ch->level;
	af.duration = 24;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );
}


/*
 * object condition aliases
 */
char *get_cond_alias( OBJ_DATA *obj )
{
	char	*stat;
	int		istat;
	/*~~~~~~~~~~*/

	istat = obj->condition;

	if		( istat > 99 )		stat = "eccellente";
	else if ( istat >= 80 )		stat = "buono";
	else if ( istat >= 60 )		stat = "discreto";
	else if ( istat >= 40 )		stat = "rovinato";
	else if ( istat >= 20 )		stat = "povero";
	else						stat = "fragile";

	return( stat );
}


/*
 * room affects
 */
void do_raffects( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA *paf, *paf_last = NULL;
	char		buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->in_room->affected != NULL )
	{
		if ( !IS_ADMIN(ch) )
		{
			send_to_char( "L'aria e' permeata da una sensibile aura magica.", ch );
			return;
		}

		send_to_char( "Nella stanza sono attivi i seguenti incantesimi:\n\r", ch );
		for ( paf = ch->in_room->affected; paf != NULL; paf = paf->next )
		{
			if ( paf_last != NULL && paf->type == paf_last->type )
			{
				if ( ch->level >= 20 )
					sprintf( buf, "                      " );
				else
					continue;
			}
			else
				sprintf( buf, "Incantesimi: %-15s", skill_table[paf->type].name );

			send_to_char( buf, ch );

			sprintf( buf, ": modifia %s di %d ", raffect_loc_name(paf->location), paf->modifier );
			send_to_char( buf, ch );
			if ( paf->duration == -1 || paf->duration == -2 )
				sprintf( buf, "permanentemente" );
			else
				sprintf( buf, "per %d ore", paf->duration );
			send_to_char( buf, ch );
		}

		send_to_char( "\n\r", ch );
		paf_last = paf;
	}
	else
		send_to_char( "Nella stanza non e' attivo nessun incantesimo.\n\r", ch );
}


char *get_skill_alias( const int learned )
{
	if		( learned >=  1 && learned <	5 )		return( "dilettante" );
	else if ( learned >=  5 && learned <   20 )		return( "scarso" );
	else if ( learned >= 20 && learned <   40 )		return( "mediocre" );
	else if ( learned >= 40 && learned <   60 )		return( "sufficiente" );
	else if ( learned >= 60 && learned <   80 )		return( "buono" );
	else if ( learned >= 80 && learned <   95 )		return( "esperto" );
	else if ( learned >= 95 && learned <= 100 )		return( "superbo" );

	/* altrimenti e' un valore errato */
	bug( "Get_skill_alias: errore nel valore learned: %d", learned );
	return( "(errore)" );
}


void do_practice( CHAR_DATA *ch, char *argument )
{
	char		buf2[10 * MSL];
	char		buf[MSL];
	CHAR_DATA	*mob;
	int			sn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( argument[0] == '\0' )
	{
		int col;
		/*~~~~*/

		col = 0;
		strcpy( buf2, "\n\r" );
		for ( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if ( skill_table[sn].name == NULL )
				break;

			if ( skill_failure_nomessage(ch, sn, 0) )
				continue;

			for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
			{
				if ( IS_MOB(mob) && IS_SET(mob->act, ACT_PRACTICE) )
				{
					if ( skill_table[sn].cabal == CABAL_NONE )
					{
						if ((mob->pIndexData->practicer == 0 && skill_table[sn].group == GROUP_NONE)
						||	(mob->pIndexData->practicer & (1 << prac_table[skill_table[sn].group].number)) )
							break;
					}
					else
					{
						if ( ch->cabal == mob->cabal )
							break;
					}
				}
			}

			/* Se si e' immortali si vedono i valori in percentuale, altrimenti una scritta aggettivale */
			if ( IS_ADMIN(ch) )
			{
				sprintf( buf, "%s%-25s%s  %3d%%   ",
					mob ? CLR(ch, GREEN) : "",
					skill_table[sn].name,
					mob ? CLR(ch, NORMAL) : "",
					ch->pcdata->learned[sn] );
			}
			else
			{
				sprintf( buf, "%s%-25s%s  %10s   ",
					mob ? CLR(ch, GREEN) : "",
					skill_table[sn].name,
					mob ? CLR(ch, NORMAL) : "",
					get_skill_alias(ch->pcdata->learned[sn]) );
			}

			strcat( buf2, buf );
			if ( ++col % 2 == 0 )
				strcat( buf2, "\n\r" );
		}

		if ( col % 2 != 0 )
			strcat( buf2, "\n\r" );

		sprintf( buf, "Possiedi ancora %d sessioni di pratica.\n\r", ch->practice );
		strcat( buf2, buf );

		page_to_char( buf2, ch );
	}
	else
	{
		int			adept;
		int			cost;
		/*~~~~~~~~~~~~~~*/

		if ( !IS_AWAKE(ch) )
		{
			send_to_char( "Nei tuoi sogni o cosa?\n\r", ch );
			return;
		}

		if ( ch->practice <= 0 )
		{
			send_to_char( "Non possiedi nessuna sessione di pratica.\n\r", ch );
			return;
		}

		if( (sn = find_spell(ch, argument)) < 0
		|| skill_failure_nomessage(ch, sn, 0) )
		{
			send_to_char( "Non puoi praticare cio'.\n\r", ch );
			return;
		}

		if ( !str_cmp("vampire", skill_table[sn].name) )
		{
			send_to_char( "Puoi praticare questa cosa solo da un Questor.\n\r", ch );
			return;
		}

		/* Fino al livello 20 compreso si paga in monete d'argento, altrimenti a peso d'oro */
		if ( ch->level <= 20 )
			cost = ch->pcdata->learned[sn] * ch->level;
		else
			cost = ch->pcdata->learned[sn] * ch->level * 100;

		/* i niubbi non pagano */
		if ( ch->level > LEVEL_NEWBIE && (ch->silver + ch->gold*100) < cost )
		{
			send_to_char( "Non hai abbastanza denaro.\n\r", ch );
			return;
		}

		for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
		{
			if ( IS_MOB(mob) && IS_SET(mob->act, ACT_PRACTICE) )
			{
				if ( skill_table[sn].cabal == CABAL_NONE )
				{
					if ((mob->pIndexData->practicer == 0
					&&	(	skill_table[sn].group == GROUP_NONE
						||	skill_table[sn].group == GROUP_CREATION
						||	skill_table[sn].group == GROUP_HARMFUL
						||	skill_table[sn].group == GROUP_PROTECTIVE
						||	skill_table[sn].group == GROUP_DETECTION
						||	skill_table[sn].group == GROUP_WEATHER))
					||	(mob->pIndexData->practicer & (1 << prac_table[skill_table[sn].group].number)) )
					{
						break;
					}
				}
				else
				{
					if ( ch->cabal == mob->cabal )
						break;
				}
			}
		}

		if ( mob == NULL )
		{
			send_to_char( "Non puoi imparare questo qui..\n\r", ch );
			return;
		}

		adept = IS_MOB( ch ) ? 100 : class_table[ch->class].skill_adept;

		if ( ch->pcdata->learned[sn] >= adept )
		{
			sprintf( buf, "Hai gia' imparato %s abbastanza.\n\r", skill_table[sn].name );
			send_to_char( buf, ch );
		}
		else
		{
			if ( !ch->pcdata->learned[sn] )
				ch->pcdata->learned[sn] = 1;
			ch->practice--;
			ch->pcdata->learned[sn] +=
				int_app[get_curr_stat(ch, INT)].learn / UMAX( skill_table[sn].rating[ch->class], 1 );

			if ( ch->pcdata->learned[sn] < adept )
				act( "Pratichi $T.", ch, NULL, skill_table[sn].name, TO_CHAR, POS_REST );
			else
			{
				ch->pcdata->learned[sn] = adept;
				act( "Ora hai imparato qualcosa riguardo a $T.", ch, NULL, skill_table[sn].name, TO_CHAR, POS_REST );
			}
		}
	}
}


/*
 * New who command by Chronos
 */
void do_who( CHAR_DATA *ch, char *argument )
{
	char			buf[MSL];
	char			buf2[MSL];
	char			output[MSL*4];
	DESCRIPTOR_DATA *d;
	int				nMatch;
	int				count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	send_to_char( "\n\r                            Giocatori di XandraMud:\n\r", ch );
	send_to_char( "------------------------- - -- === ========= === -- - -------------------------\n\r", ch );
	if ( IS_TRUSTED(ch, LEVEL_ADMIN) )
		send_to_char( "Inf Nome         Razza         Classe        Liv Inv Cabala       Act\n\r", ch );
	else
		send_to_char( "Inf Nome         Razza\n\r", ch );
	send_to_char( "-------------------------------------------------------------------------------\n\r", ch );

	/* Now show matching pgs. */
	nMatch = 0;
	buf[0] = '\0';
	output[0] = '\0';
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		CHAR_DATA	*wch;
		char		classbuf[100];
		char		level_buf[100];
		char		invis_buf[100];
		char		cabal_buf[100];
		char		act_buf[100];
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		wch = ( d->original != NULL ) ? d->original : d->character;

		/* Controlla se è in gioco, accetta le connessioni link death */
		if ( d->connected != CON_PLAYING || ch -> desc == NULL )
			continue;

		/* Se la vittima è un immortale a meno che chi richiama il who non sia un immortale non vede */
/*		if ( IS_ADMIN(wch) && !IS_ADMIN(ch) )
			continue;
*/
		if ( get_trust(ch) < wch->invis_level )
			continue;
	
		if ( get_trust(ch) < wch->incog_level && ch->in_room != wch->in_room )
			continue;

		nMatch++;

		/* CLASS_BUF */
		sprintf( classbuf, "%s%-13s%s",
			CLR(ch, YELLOW),
			IS_ADMIN(wch) ? "Staff"
				: (wch->sex == SEX_FEMALE) ? class_table[wch->class].fname :  class_table[wch->class].name,
			CLR(ch, WHITE_BOLD) );

		/* LEVEL_BUF */
		sprintf( level_buf, "%s%3d%s",
			CLR(ch, CYAN),
			wch->level,
			CLR(ch, WHITE_BOLD) );

		/* WIZINV_BUF */
		sprintf( invis_buf, "%s%3d%s",
			CLR(ch, CYAN),
			wch->invis_level,
			CLR(ch, WHITE_BOLD) );

		/* CABAL_BUF */
		if ( (wch->cabal && ch->cabal == wch->cabal)
		||	 IS_ADMIN(ch)
		||	 (IS_SET(wch->act, PLR_CANINDUCT) && wch->cabal == 1)
		||	 wch->cabal == CABAL_HUNTER
		||	 (wch->cabal == CABAL_RULER && is_equiped_n_char(wch, OBJ_VNUM_RULER_BADGE, WEAR_NECK)) )
		{
			sprintf( cabal_buf, "[%s%s%s] ",
				CLR(ch, CYAN),
				cabal_table[wch->cabal].short_name,
				CLR(ch, WHITE_BOLD) );
		}
		else
			cabal_buf[0] = '\0';

		if ( wch->cabal == 0 )
			cabal_buf[0] = '\0';

		/* ACT_BUF */
		act_buf[0] = '\0';
		sprintf( act_buf, "%s%s%s",
			CLR(ch, WHITE),
			IS_SET(wch->act, PLR_WANTED) ? "RIC" : "",
			CLR(ch, WHITE_BOLD) );

		/* Format it up. */
		if ( IS_TRUSTED(ch, LEVEL_ADMIN) )
		{
			sprintf( buf, "%-3s %-12s %-13s %s %s %s %-12s %-3s\n\r",
				(ch -> desc == NULL) ? "LKD" :
					(IS_SET(wch->act, PLR_AFK)) ? "AFK" : "",
				wch->name,
				(wch->race < MAX_PC_RACE) ?
					(wch->sex == SEX_FEMALE) ? pc_race_table[wch->race].fname : pc_race_table[wch->race].name
					: "",
				classbuf,
				level_buf,
				invis_buf,
				cabal_buf,
				act_buf );
		}
		else
		{
			sprintf( buf, "%-3s %-12s %-13s\n\r",
				(IS_SET(wch->act, PLR_AFK)) ? "AFK" : "",
				wch->name,
				(wch->race < MAX_PC_RACE) ?
					(wch->sex == SEX_FEMALE) ? pc_race_table[wch->race].fname : pc_race_table[wch->race].name
					: "" );
		}

		strcat( output, buf );
	}	/* end for */

	count = 0;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING )
			count++;
	}

	max_on = UMAX( count, max_on );
	sprintf( buf2, "\n\rAvventurieri nel Mud: %d. Massimo di oggi: %d.\n\r", nMatch, max_on );
	strcat( output, buf2 );
	page_to_char( output, ch );
}


void do_camp( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af, af2;
	/*~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_camp, 0, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_camp) )
	{
		send_to_char( "non hai le energie sufficienti per creare un nuovo accampamento ora.\n\r", ch );
		return;
	}

	if ( number_percent() > get_skill(ch, gsn_camp) )
	{
		send_to_char( "Fallisci nel creare un campo.\n\r", ch );
		check_improve( ch, gsn_camp, TRUE, 4 );
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	||	IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
	||	IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
	||	(	ch->in_room->sector_type != SECT_FIELD
		&&	ch->in_room->sector_type != SECT_FOREST
		&&	ch->in_room->sector_type != SECT_MOUNTAIN
		&&	ch->in_room->sector_type != SECT_HILLS) )
	{
		send_to_char( "Non vi e' abbastanza verde qui attorno per creare un campo.\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 150 )
	{
		send_to_char( "Non hai abbastanza energie per creare un campo.\n\r", ch );
		return;
	}

	check_improve( ch, gsn_camp, TRUE, 4 );
	ch->point[MANA] -= 150;

	WAIT_STATE( ch, skill_table[gsn_camp].beats );

	send_to_char( "Riesci a creare un accampamento ove bivaccare.\n\r", ch );
	act( "$n riesce a crare un accampamento ove bivaccare.", ch, NULL, NULL, TO_ROOM, POS_REST );

	af.where = TO_AFFECTS;
	af.type = gsn_camp;
	af.level = ch->level;
	af.duration = 12;
	af.bitvector = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	affect_to_char( ch, &af );

	af2.where = TO_ROOM_CONST;
	af2.type = gsn_camp;
	af2.level = ch->level;
	af2.duration = ch->level / 20;
	af2.bitvector = 0;
	af2.modifier = 2 * ch->level;
	af2.location = APPLY_ROOM_HEAL;
	affect_to_room( ch->in_room, &af2 );

	af2.modifier = ch->level;
	af2.location = APPLY_ROOM_MANA;
	affect_to_room( ch->in_room, &af2 );
}


void do_demand( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	int			chance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( IS_MOB(ch) )
		return;

	if ( ch->class != CLASS_ANTIPALADIN )
	{
		send_to_char( "Non puoi fare questo.\n\r", ch );
		return;
	}

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Pretendere cosa da chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg2)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) )
	{
		send_to_char( "Perche' non glielo chiedi direttamente?\n\r", ch );
		return;
	}

	WAIT_STATE( ch, PULSE_VIOLENCE );

	chance = IS_EVIL( victim ) ? 10 : IS_GOOD( victim ) ? -5 : 0;
	chance += ( get_curr_stat(ch, CHA) - 15 ) * 10;
	chance += ch->level - victim->level;

	if ( victim->level >= ch->level + 10 || victim->level >= ch->level * 2 )
		chance = 0;

	if ( number_percent() > chance )
	{
		do_say( victim, "Non ho intenzione di darti un bel niente!" );
		do_murder( victim, ch->name );
		return;
	}

	if (((obj = get_obj_carry(victim, arg1)) == NULL && (obj = get_obj_wear(victim, arg1)) == NULL)
	||	IS_SET(obj->extra_flags, ITEM_INVENTORY) )
	{
		do_say( victim, "Non ho questa cosa con me." );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE )
		unequip_char( victim, obj );

	if ( !can_drop_obj(ch, obj) )
	{
		do_say( victim, "L'oggetto e' maledetto, non posso lasciarlo." );
		return;
	}

	if ( ch->carry_number + get_obj_number(obj) > can_carry_n(ch) )
	{
		send_to_char( "Il tuo inventario e' pieno.\n\r", ch );
		return;
	}

	if ( ch->carry_weight + get_obj_weight(obj) > can_carry_w(ch) )
	{
		send_to_char( "Non puoi trasportare cosi' tanta roba.\n\r", ch );
		return;
	}

	if ( !can_see_obj(ch, obj) )
	{
		act( "Non puoi vederlo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	obj_from_char( obj );
	obj_to_char( obj, ch );
	act( "$n pretende $p da $N.", ch, obj, victim, TO_NOTVICT, POS_REST );
	act( "Pretendi $p da $N.", ch, obj, victim, TO_CHAR, POS_REST );
	act( "$n pretende $p da te.", ch, obj, victim, TO_VICT, POS_REST );

	if ( IS_SET(obj->progtypes, OPROG_GIVE) )
		( obj->pIndexData->oprogs->give_prog ) ( obj, ch, victim );

	if ( IS_SET(victim->progtypes, MPROG_GIVE) )
		( victim->pIndexData->mprogs->give_prog ) ( victim, ch, obj );
}


void do_control( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			chance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_control_animal, 1, 0, NULL) )
		return;

	if ( arg[0] == '\0' )
	{
		send_to_char( "Controllare chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e'.\n\r", ch );
		return;
	}

	if ( race_table[ORG_RACE(victim)].pc_race )
	{
		send_to_char( "Meglio che tu provi a farlo su altre creature.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( count_charmed(ch) )
		return;

	WAIT_STATE( ch, PULSE_VIOLENCE );

	chance = get_skill( ch, gsn_control_animal );

	chance += ( get_curr_stat(ch, CHA) - 20 ) * 5;
	chance += ( ch->level - victim->level ) * 3;
	chance += ( get_curr_stat(ch, INT) - get_curr_stat(victim, INT) ) * 5;

	if (IS_AFFECTED(victim, AFF_CHARM)
	||	IS_AFFECTED(ch, AFF_CHARM)
	||	number_percent() > chance
	||	ch->level < (victim->level + 2)
	||	IS_SET(victim->imm_flags, IMM_CHARM)
	||	(IS_MOB(victim) && victim->pIndexData->pShop != NULL) )
	{
		check_improve( ch, gsn_control_animal, FALSE, 2 );
		do_say( victim, "Non ho intenzione di seguirti!" );
		do_murder( victim, ch->name );
		return;
	}

	check_improve( ch, gsn_control_animal, TRUE, 2 );

	if ( victim->master )
		stop_follower( victim );
	SET_BIT( victim->affected_by, AFF_CHARM );
	victim->master = victim->leader = ch;

	act( "$n non e' adorabile?", ch, NULL, victim, TO_VICT, POS_REST );
	if ( ch != victim )
		act( "$N ti guarda con occhi adoranti.", ch, NULL, victim, TO_CHAR, POS_REST );
}


char *get_ac_alias( CHAR_DATA *ch )
{
	int		ac;

	ac = (GET_AC(ch, AC_EXOTIC) + GET_AC(ch, AC_BASH) + GET_AC(ch, AC_PIERCE) + GET_AC(ch, AC_SLASH)) / 4;

	if		( ac >=  101 )	return "Difesa nulla             ";
	else if ( ac >=   80 )	return "Difesa inefficace        ";
	else if ( ac >=   60 )	return "Lievissima protezione    ";
	else if ( ac >=   40 )	return "Leggera protezione       ";
	else if ( ac >=   20 )	return "Difesa leggera           ";
	else if ( ac >=    0 )	return "Protezione               ";
	else if ( ac >=  -20 )	return "Protezione sufficiente   ";
	else if ( ac >=  -40 )	return "Discreta protezione      ";
	else if ( ac >=  -60 )	return "Buona difesa             ";
	else if ( ac >=  -80 )	return "Protezione piu' che buona";
	else if ( ac >= -100 )	return "Ottima difesa            ";
	else					return "Superba protezione       ";
}


char *get_stat_alias( CHAR_DATA *ch, int where )
{
	char	*stat;
	int		istat;
	/*~~~~~~~~~~*/

	if ( where == STR )
	{
		istat = get_curr_stat( ch, STR );
		if		( istat > 22 )		stat = "Titanico";
		else if ( istat >= 20 )		stat = "Erculino";
		else if ( istat >= 18 )		stat = "Forte";
		else if ( istat >= 14 )		stat = "Medio";
		else if ( istat >= 10 )		stat = "Povero";
		else						stat = "Scarso";
		return( stat );
	}

	if ( where == WIS )
	{
		istat = get_curr_stat( ch, WIS );
		if		( istat > 22 )		stat = "Maestro";
		else if ( istat >= 20 )		stat = "Saggio";
		else if ( istat >= 18 )		stat = "Buono";
		else if ( istat >= 14 )		stat = "Medio";
		else if ( istat >= 10 )		stat = "Mediocre";
		else						stat = "Sciocco";
		return( stat );
	}

	if ( where == CON )
	{
		istat = get_curr_stat( ch, CON );
		if		( istat > 22 )		stat = "Roccia";
		else if ( istat >= 20 )		stat = "Vigoroso";
		else if ( istat >= 18 )		stat = "Robusto";
		else if ( istat >= 14 )		stat = "Medio";
		else if ( istat >= 10 )		stat = "Povero";
		else						stat = "Fragile";
		return( stat );
	}

	if ( where == INT )
	{
		istat = get_curr_stat( ch, INT );
		if		( istat > 22 )		stat = "Genio";
		else if ( istat >= 20 )		stat = "Capace";
		else if ( istat >= 18 )		stat = "Buono";
		else if ( istat >= 14 )		stat = "Medio";
		else if ( istat >= 10 )		stat = "Povero";
		else						stat = "Stupido";
		return( stat );
	}

	if ( where == DEX )
	{
		istat = get_curr_stat( ch, DEX );
		if		( istat > 22 )		stat = "Rapido";
		else if ( istat >= 20 )		stat = "Veloce";
		else if ( istat >= 18 )		stat = "Abile";
		else if ( istat >= 14 )		stat = "Normale";
		else if ( istat >= 10 )		stat = "Lento";
		else						stat = "Impedito";
		return( stat );
	}

	if ( where == CHA )
	{
		istat = get_curr_stat( ch, CHA );
		if		( istat > 22 )		stat = "Attraente";
		else if ( istat >= 20 )		stat = "Familiare";
		else if ( istat >= 18 )		stat = "Cordiale";
		else if ( istat >= 14 )		stat = "Medio";
		else if ( istat >= 10 )		stat = "Rozzo";
		else						stat = "Selvaggio";
		return( stat );
	}

	bug( "stat_alias: Bad stat number.", 0 );
	return( NULL );
}


void do_score( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	char	buf2[MIL];
	char	buf_stat[MIL];
	int		ekle = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	sprintf( buf, "\n\r   %s/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~\\\n\r",
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* Testa dello score */
	sprintf( buf, "  |  %sNome %s%-12s                       %sAnni %s%-3d  %sSesso %s%-9s  %s|%s_|%s\n\r",
		CLR(ch, WHITE_BOLD),
		CLR(ch, WHITE),
		ch->name,
		CLR(ch, WHITE_BOLD),
		CLR(ch, WHITE),
		get_age(ch),
		CLR(ch, WHITE_BOLD),
		CLR(ch, WHITE),
		ch->sex == 0 ? "Asessuato" : ch->sex == 1 ? "Maschio" : "Femmina",
		CLR(ch, GREEN_BOLD),
		CLR(ch, GREEN),
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	sprintf( buf, "  |%s----------------------+------------------------+---------------------%s|\n\r",
		CLR(ch, CYAN_BOLD),
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* Razza, Forza, Posizione */
	if ( IS_ADMIN(ch) )
		sprintf( buf_stat, " %s%2d (%2d) ", CLR(ch, WHITE_BOLD), ch->perm_stat[STR], get_curr_stat(ch, STR) );
	else
		sprintf( buf_stat, "%s%-9s", CLR(ch, WHITE_BOLD), get_stat_alias(ch, STR) );

	switch ( ch->position )
	{
	case POS_DEAD:		sprintf( buf2, "Sei mort$x!      " );		break;
	case POS_MORTAL:	sprintf( buf2, "Sei quasi mort$x " );		break;
	case POS_INCAP:		sprintf( buf2, "Sei incapacitat$x" );		break;
	case POS_STUN:		sprintf( buf2, "Sei tramortit$x  " );		break;
	case POS_SLEEP:		sprintf( buf2, "Stai dormendo   " );		break;
	case POS_REST:		sprintf( buf2, "Stai riposando  " );		break;
	case POS_SIT:		sprintf( buf2, "Sei sedut$x      " );		break;
	case POS_STAND:		sprintf( buf2, "Sei in piedi    " );		break;
	case POS_FIGHT:		sprintf( buf2, "Stai lottando   " );		break;
	default:			sprintf( buf2, ">sconosciuta<   " );		break;
	}

	sprintf( buf, "  | %sRazza  %s%-13s %s| %sForza        %s %s| %s%-16s    %s|",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		RACE_NAME_ORG(ch),
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		buf_stat,
		CLR(ch, CYAN_BOLD),
		CLR(ch, WHITE_BOLD),
		buf2,
		CLR(ch, GREEN_BOLD) );
	act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );

	/* Classe, Intelligenza, Religione */
	if ( IS_ADMIN(ch) )
		sprintf( buf_stat, " %s%2d (%2d) ", CLR(ch, WHITE_BOLD), ch->perm_stat[INT], get_curr_stat(ch, INT) );
	else
		sprintf( buf_stat, "%s%-9s", CLR(ch, WHITE_BOLD), get_stat_alias(ch, INT) );

	sprintf( buf, "  | %sClasse %s%-13s %s| %sIntelligenza %s %s| %sCredo %s%-13s %s|\n\r",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		IS_MOB(ch) ? "mobile"
			: (ch->sex == SEX_FEMALE) ? class_table[ch->class].fname : class_table[ch->class].name,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		buf_stat,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		(religion_table[ch->religion].leader == '\0') ? "Nessuno" : religion_table[ch->religion].leader,
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* Patria, Saggezza, Pratiche */
	if ( IS_ADMIN(ch) )
		sprintf( buf_stat, " %s%2d (%2d) ", CLR(ch, WHITE_BOLD), ch->perm_stat[WIS], get_curr_stat(ch, WIS) );
	else
		sprintf( buf_stat, "%s%-9s", CLR(ch, WHITE_BOLD), get_stat_alias(ch, WIS) );

	sprintf( buf,"  | %sPatria %s%-13s %s| %sSaggezza     %s %s| %sPratiche      %s%3d   %s|\n\r",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		IS_MOB(ch) ? "Midgaard" : hometown_table[ch->hometown].name,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		buf_stat,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->practice,
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* Etica, destrezza, allenamento */
	if ( IS_ADMIN(ch) )
		sprintf( buf_stat, " %s%2d (%2d) ", CLR(ch, WHITE_BOLD), ch->perm_stat[DEX], get_curr_stat(ch, DEX) );
	else
		sprintf( buf_stat, "%s%-9s", CLR(ch, WHITE_BOLD), get_stat_alias(ch, DEX) );

	sprintf( buf,
		"  | %sEtica  %s%-13s %s| %sDestrezza    %s %s| %sAllenamenti   %s%3d   %s|\n\r",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		IS_MOB(ch) ? "Mobile" : ch->ethos == 1 ? "Legale" : ch->ethos == 2 ? "Neutrale" : ch->ethos == 3 ? "Caotico" : "Nessuno",
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		buf_stat,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->train,
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* Allineamento, costituzione, punti quest */
	if ( IS_ADMIN(ch) )
		sprintf( buf_stat, " %s%2d (%2d) ", CLR(ch, WHITE_BOLD), ch->perm_stat[CON], get_curr_stat(ch, CON) );
	else
		sprintf( buf_stat, "%s%-9s", CLR(ch, WHITE_BOLD), get_stat_alias(ch, CON) );

	sprintf( buf,
		"  | %sAllin. %s%-13s %s| %sCostituzione %s %s| %sPunti quest %s%5d   %s|\n\r",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		IS_GOOD(ch) ? "Buono" : IS_EVIL(ch) ? "Malvagio" : "Neutrale",
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		buf_stat,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		IS_MOB(ch) ? 0 : ch->pcdata->questpoints,
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* (Livello), carisma, morti/fuga */
	if ( IS_ADMIN(ch) )
		sprintf( buf2, "%sLiv.   %s%-3d    ", CLR(ch, RED_BOLD), CLR(ch, WHITE_BOLD), ch->level );
	else
		sprintf( buf2, "              " );

	if ( IS_ADMIN(ch) )
		sprintf( buf_stat, " %s%2d (%2d) ", CLR(ch, WHITE_BOLD), ch->perm_stat[CHA], get_curr_stat(ch, CHA) );
	else
		sprintf( buf_stat, "%s%-9s", CLR(ch, WHITE_BOLD), get_stat_alias(ch, CHA) );

	sprintf( buf, "  | %s       %s| %sCarisma      %s %s| %s%s   %s%3d   %s|\n\r",
		buf2,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		buf_stat,
		CLR(ch, CYAN_BOLD),
		CLR(ch, RED_BOLD),
		ch->class == CLASS_SAMURAI ? "Morti      " : "Punti fuga ",
		CLR(ch, WHITE_BOLD),
		ch->class == CLASS_SAMURAI ? ch->pcdata->death : ch->wimpy,
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* messaggi vari */
	sprintf( buf, "  |%s----------------------+------------------------+---------------------%s|\n\r",
		CLR(ch, CYAN_BOLD),
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	if ( ch->guarding != NULL )
	{
		ekle = 1;
		sprintf( buf, "   | %sStai proteggendo %s%-12s                                       %s|\n\r",
			CLR(ch, WHITE),
			CLR(ch, WHITE_BOLD),
			ch->guarding->name,
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );
	}

	if ( ch->guarded_by != NULL )
	{
		ekle = 1;
		sprintf( buf, "  | %sSei protett$x da %s%-12s                                         %s|",
			CLR(ch, WHITE),
			CLR(ch, WHITE_BOLD),
			ch->guarded_by->name,
			CLR(ch, GREEN_BOLD) );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );
	}

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )
	{
		ekle = 1;
		sprintf( buf, "  | %sSei ubriac$x.                                                        %s|",
			CLR(ch, WHITE),
			CLR(ch, GREEN_BOLD) );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );
	}

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_THIRST] <= 0 )
	{
		ekle = 1;
		sprintf( buf, "  | %sSei assetat$x.                                                       %s|",
			CLR(ch, WHITE),
			CLR(ch, GREEN_BOLD) );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );
	}

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_HUNGER] <= 0 )
	{
		ekle = 1;
		sprintf( buf, "  | %sSei affamat$x.                                                       %s|",
			CLR(ch, WHITE),
			CLR(ch, GREEN_BOLD) );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );
	}

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_BLOODLUST] <= 0 )
	{
		ekle = 1;
		sprintf( buf, "  | Sei assetat$x di sangue.                                             |" );
		act( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );
	}

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_DESIRE] <= 0 )
	{
		ekle = 1;
		sprintf( buf, "  | %sHai nostalgia di casa.                                              %s|\n\r",
			CLR(ch, WHITE),
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );
	}

	if ( ch->last_fight_time != -1 && !IS_ADMIN(ch) && (current_time - ch->last_fight_time) < FIGHT_DELAY_TIME )
	{
		ekle = 1;
		sprintf( buf, "  | %sLa tua adrenalina e' a mille!                                       %s|\n\r",
			CLR(ch, WHITE),
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );
	}

	if ( ekle )
	{
		sprintf( buf, "  |%s---------------------------------------------------------------------%s|\n\r",
			CLR(ch, CYAN_BOLD),
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );
	}

	/* Oggetti trasportati, Argento, Oro */
	sprintf( buf, "  | %sOgg. trasportati    %s%3d/%-4d     %sArgento %s%7ld     %sOro  %s%7ld   %s|\n\r",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->carry_number,
		can_carry_n(ch),
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->silver,
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->gold,
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	/* Peso trasportato, Hitroll, Damroll */
	sprintf( buf, "  | %sPeso trasportato %s%6ld/%-8d %sHitroll     %s%3d     %sDamroll  %s%3d   %s|\n\r",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		get_carry_weight(ch),
		can_carry_w(ch),
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		GET_HITROLL(ch),
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		GET_DAMROLL(ch),
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	if ( IS_ADMIN(ch) )
	{
		sprintf( buf, "  | %sEsperienza          %s%-7d      %sEsperienza al prox liv %s%-7d     %s|\n\r",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			ch->exp,
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			IS_MOB(ch) ? 0 : exp_to_level(ch, ch->pcdata->points),
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );

		sprintf( buf, "  | %sLiv. di fiducia %s%3d   %sLiv. invisibilita' %s%3d   %sLiv. incognito %s%3d   %s|\n\r",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			get_trust(ch),
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			ch->invis_level,
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			ch->incog_level,
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );

		sprintf( buf, "  | %sHoly Light %s[%s]                  %sOre di gioco totali %s%-3d            %s|\n\r",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			IS_SET(ch->act,PLR_HOLYLIGHT) ? "SI" : "no",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			(ch->played + (int) (current_time - ch->logon)) / 3600,
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );

		sprintf( buf, "  | %sArmatura vs l'esotiche    %s%-4d   %sArmatura vs le spinte     %s%-4d     %s|\n\r",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			GET_AC(ch, AC_EXOTIC),
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			GET_AC(ch, AC_BASH),
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );

		sprintf( buf, "  | %sArmatura vs le pugnalate  %s%-4d   %sArmatura vs i fendenti    %s%-4d     %s|\n\r",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			GET_AC(ch, AC_PIERCE),
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			GET_AC(ch, AC_SLASH),
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );

		/* Tiro salvezza vs incantesimi */
		sprintf( buf, "  | %sTiro salvezza incantesimi %s%3d                                       %s|\n\r",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			ch->saving_throw,
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf2, "%s%s",
			CLR(ch, WHITE_BOLD),
			get_ac_alias(ch) );

		sprintf( buf, "  | %sTiro salvezza incantesimi %s%3d    %sArmatura %s %s|\n\r",
			CLR(ch, RED_BOLD),
			CLR(ch, WHITE_BOLD),
			ch->saving_throw,
			CLR(ch, RED_BOLD),
			buf2,
			CLR(ch, GREEN_BOLD) );
		send_to_char( buf, ch );
	}

	/* Linea vuota */
	sprintf( buf, "  |%69s|\n\r", " " );
	send_to_char( buf, ch );

	/* Vita, movimento, mana */
	sprintf( buf, "  | %sVita %s%5d / %-5d    %sMovimento %s%5d / %-5d    %sMana %s%5d / %-5d %s|\n\r",
		CLR(ch, RED_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->point[LIFE],
		ch->max_point[LIFE],
		CLR(ch, GREEN_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->point[MOVE],
		ch->max_point[MOVE],
		CLR(ch, CYAN_BOLD),
		CLR(ch, WHITE_BOLD),
		ch->point[MANA],
		ch->max_point[MANA],
		CLR(ch, GREEN_BOLD) );
	send_to_char( buf, ch );

	sprintf( buf, "/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/  |\n\r" );
	send_to_char( buf, ch );
	sprintf( buf, "\\____________________________________________________________________\\_/%s\n\r",
		CLR(ch, WHITE_BOLD) );
	send_to_char( buf, ch );

	if ( ch->affected != NULL && IS_SET(ch->comm, COMM_SHOW_AFFECTS) )
		do_affects( ch, "" );
}


void do_make_arrow( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*arrow;
	AFFECT_DATA tohit, todam, saf;
	int			count, color, mana, wait;
	char		buf[MSL];
	char		arg[MIL];
	char		*str;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( skill_failure_check(ch, gsn_make_arrow, 0, 0, "Non conosci il modo di creare delle frecce.") )
		return;

	if ( ch->in_room->sector_type != SECT_FIELD && ch->in_room->sector_type != SECT_FOREST && ch->in_room->sector_type != SECT_HILLS )
	{
		send_to_char( "Non trovi abbastanza legna.\n\r", ch );
		return;
	}

	mana = skill_table[gsn_make_arrow].min_mana;
	wait = skill_table[gsn_make_arrow].beats;

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
		color = 0;
	else if ( !str_prefix(arg, "verde") || !str_prefix(arg, "green") )
		color = gsn_green_arrow;
	else if ( !str_prefix(arg, "rossa") || !str_prefix(arg, "red") )
		color = gsn_red_arrow;
	else if ( !str_prefix(arg, "bianca") || !str_prefix(arg, "white") )
		color = gsn_white_arrow;
	else if ( !str_prefix(arg, "blu") || !str_prefix(arg, "blue") )
		color = gsn_blue_arrow;
	else
	{
		send_to_char( "Non sai come costruire questo tipo di freccia.\n\r", ch );
		return;
	}

	if ( color != 0 )
	{
		mana += skill_table[color].min_mana;
		wait += skill_table[color].beats;
	}

	if ( ch->point[MANA] < mana )
	{
		send_to_char( "Non possiedi abbastanza energie per questo tipo di freccia.\n\r", ch );
		return;
	}

	ch->point[MANA] -= mana;
	WAIT_STATE( ch, wait );

	send_to_char( "Cominci ad intagliare il legno per crearne delle frecce.\n\r", ch );
	act( "$n comincia ad intagliare del legno!", ch, NULL, NULL, TO_ROOM, POS_REST );
	for ( count = 0; count < (ch->level / 5); count++ )
	{
		if ( number_percent() > get_skill(ch, gsn_make_arrow) )
		{
			send_to_char( "Fallisci nell'intagliare una freccia e la rompi.\n\r", ch );
			check_improve( ch, gsn_make_arrow, FALSE, 3 );
			continue;
		}

		send_to_char( "Riesci con successo ad intagliare una freccia.\n\r", ch );
		check_improve( ch, gsn_make_arrow, TRUE, 3 );

		arrow = create_object( get_obj_index(OBJ_VNUM_RANGER_ARROW), ch->level );
		arrow->level = ch->level;
		arrow->value[1] = ch->level / 10;
		arrow->value[2] = ch->level / 10;

		tohit.where = TO_OBJECT;
		tohit.type = gsn_make_arrow;
		tohit.level = ch->level;
		tohit.duration = -1;
		tohit.location = APPLY_HITROLL;
		tohit.modifier = ch->level / 10;
		tohit.bitvector = 0;
		affect_to_obj( arrow, &tohit );

		todam.where = TO_OBJECT;
		todam.type = gsn_make_arrow;
		todam.level = ch->level;
		todam.duration = -1;
		todam.location = APPLY_DAMROLL;
		todam.modifier = ch->level / 10;
		todam.bitvector = 0;
		affect_to_obj( arrow, &todam );

		if ( color != 0 )
		{
			saf.where = TO_WEAPON;
			saf.type = color;
			saf.level = ch->level;
			saf.duration = -1;
			saf.location = 0;
			saf.modifier = 0;

			if ( color == gsn_green_arrow )
			{
				saf.bitvector = WEAPON_POISON;
				str = "verde";
			}
			else if ( color == gsn_red_arrow )
			{
				saf.bitvector = WEAPON_FLAMING;
				str = "rossa";
			}
			else if ( color == gsn_white_arrow )
			{
				saf.bitvector = WEAPON_FROST;
				str = "bianca";
			}
			else
			{
				saf.bitvector = WEAPON_SHOCKING;
				str = "blu";
			}
		}
		else
			str = "di legno";

		sprintf( buf, arrow->name, str );
		free_string( arrow->name );
		arrow->name = str_dup( buf );

		sprintf( buf, arrow->short_descr, str );
		free_string( arrow->short_descr );
		arrow->short_descr = str_dup( buf );

		sprintf( buf, arrow->description, str );
		free_string( arrow->description );
		arrow->description = str_dup( buf );

		if ( color != 0 )
			affect_to_obj( arrow, &saf );
		obj_to_char( arrow, ch );
		arrow = NULL;
	}
}


void do_make_bow( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*bow;
	AFFECT_DATA tohit, todam;
	int			mana, wait;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( skill_failure_check(ch, gsn_make_bow, 0, 0, "Non conosci il modo di costruire archi.") )
		return;

	if ( ch->in_room->sector_type != SECT_FIELD && ch->in_room->sector_type != SECT_FOREST && ch->in_room->sector_type != SECT_HILLS )
	{
		send_to_char( "Non riesci a trovare abbastanza legna.\n\r", ch );
		return;
	}

	mana = skill_table[gsn_make_bow].min_mana;
	wait = skill_table[gsn_make_bow].beats;

	if ( ch->point[MANA] < mana )
	{
		send_to_char( "Non possiedi abbastanza concentrazione per creare un arco.\n\r", ch );
		return;
	}

	ch->point[MANA] -= mana;
	WAIT_STATE( ch, wait );

	if ( number_percent() > get_skill(ch, gsn_make_bow) )
	{
		send_to_char( "Fallisci nel costruire l'arco, il legno si rompe.\n\r", ch );
		check_improve( ch, gsn_make_bow, FALSE, 1 );
		return;
	}

	send_to_char( "Riesci a costruire un maneggevole arco.\n\r", ch );
	check_improve( ch, gsn_make_bow, TRUE, 1 );

	bow = create_object( get_obj_index(OBJ_VNUM_RANGER_BOW), ch->level );
	bow->level = ch->level;
	bow->value[1] = 3 + ch->level / 12;
	bow->value[2] = 4 + ch->level / 12;

	tohit.where = TO_OBJECT;
	tohit.type = gsn_make_arrow;
	tohit.level = ch->level;
	tohit.duration = -1;
	tohit.location = APPLY_HITROLL;
	tohit.modifier = ch->level / 10;
	tohit.bitvector = 0;
	affect_to_obj( bow, &tohit );

	todam.where = TO_OBJECT;
	todam.type = gsn_make_arrow;
	todam.level = ch->level;
	todam.duration = -1;
	todam.location = APPLY_DAMROLL;
	todam.modifier = ch->level / 10;
	todam.bitvector = 0;
	affect_to_obj( bow, &todam );

	obj_to_char( bow, ch );
}


void do_make( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Puoi creare sia un arco oppure una freccia.\n\r", ch );
		return;
	}

	if ( !str_prefix(arg, "arrow") || !str_prefix(arg, "freccia") )
		do_make_arrow( ch, argument );
	else if ( !str_prefix(arg, "bow") || !str_prefix(arg, "arco") )
		do_make_bow( ch, argument );
	else
		do_make( ch, "" );
}


void do_nocancel( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_NOCANCEL) )
	{
		send_to_char( "Accetti che altre persone cancellino gli incantesimi su di te.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_NOCANCEL );
	}
	else
	{
		send_to_char( "Non accetti che altre persone cancellino gli incantesimi su di te.\n\r", ch );
		SET_BIT( ch->act, PLR_NOCANCEL );
	}
}
