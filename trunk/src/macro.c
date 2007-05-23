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


/* Convertite alcune macro in funzioni per rendere più leggero l'eseguibile */

#include "stdio.h"
#include "xandra.h"


bool IS_VAMPIRE( CHAR_DATA *ch )
{
	if ( !IS_MOB(ch) && IS_SET(ch->act, PLR_VAMPIRE) )
		return TRUE;

	return FALSE;
}


bool RIGHT_HANDER( CHAR_DATA *ch )
{
	if ( IS_MOB(ch) || !IS_SET(ch->act, PLR_LEFTHAND) )
		return TRUE;

	return FALSE;
}


bool LEFT_HANDER( CHAR_DATA *ch )
{
	if ( !IS_MOB(ch) && IS_SET(ch->act, PLR_LEFTHAND) )
		return TRUE;

	return FALSE;
}


char LOWER( char c )
{
	if ( c >= 'A' && c <= 'Z' )
		return (c + 'a' - 'A');
	else
		return c;
}


char UPPER( char c )
{
	if ( c >= 'a' && c <= 'z' )
		return (c + 'A' - 'a');
	else
		return c;
}


bool IS_WATER( ROOM_INDEX_DATA *var )
{
	if ( var->sector_type == SECT_WATER_SWIM || var->sector_type == SECT_WATER_NOSWIM )
		return TRUE;

	return FALSE;
}


/*
 * Funzioni riguardanti i pg.
 */
bool IS_MOB( CHAR_DATA *ch )
{
	if ( IS_SET(ch->act, ACT_IS_MOB) )
		return TRUE;

	return FALSE;
}


bool IS_ADMIN( CHAR_DATA *ch )
{
	if ( get_trust(ch) >= LEVEL_ADMIN )
		return TRUE;

	return FALSE;
}


bool IS_HERO( CHAR_DATA *ch )
{
	if ( get_trust(ch) >= LEVEL_HERO )
		return TRUE;

	return FALSE;
}


bool IS_TRUSTED( CHAR_DATA *ch, int level )
{
	if ( get_trust(ch) >= level )
		return TRUE;

	return FALSE;
}


bool CAN_DETECT( CHAR_DATA *ch, int sn )
{
	if ( IS_SET(ch->detection, sn) )
		return TRUE;

	return FALSE;
}


int ORG_RACE( CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return ch->pIndexData->race;
	else
		return ch->pcdata->race;
}


char *RACE_NAME( CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
	{
		if ( ch->sex == SEX_FEMALE )
			return race_table[ch->race].ftranslate;
		else
			return race_table[ch->race].translate;
	}
	else
	{
		if ( ch->sex == SEX_FEMALE )
			return pc_race_table[ch->race].fname;
		else
			return pc_race_table[ch->race].name;
	}
}


char *RACE_NAME_ORG( CHAR_DATA *ch )
{
	int orace;

	orace = ORG_RACE( ch );

	if ( IS_MOB(ch) )
	{
		if ( ch->sex == SEX_FEMALE )
			return race_table[orace].ftranslate;
		else
			return race_table[orace].translate;
	}
	else
	{
		if ( ch->sex == SEX_FEMALE )
			return pc_race_table[orace].fname;
		else
			return pc_race_table[orace].name;
	}
}


bool IS_AWAKE( CHAR_DATA *ch )
{
	if ( ch->position > POS_SLEEP )
		return TRUE;

	return FALSE;
}


int GET_AC( CHAR_DATA *ch, int type )
{
	if ( IS_AWAKE(ch) )
		return ( ch->armor[type] + dex_app[get_curr_stat(ch, DEX)].defensive );
	else
		return ( ch->armor[type] );
}


int GET_HITROLL( CHAR_DATA *ch )
{
	return ( ch->hitroll + str_app[get_curr_stat(ch, STR)].tohit );
}


int GET_DAMROLL( CHAR_DATA *ch )
{
	return ( ch->damroll + str_app[get_curr_stat(ch, STR)].todam );
}


bool IS_OUTSIDE( CHAR_DATA *ch )
{
	if( !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)
	&& ch->in_room->sector_type != SECT_INSIDE )
		return TRUE;
	else
		return FALSE;
}


void WAIT_STATE( CHAR_DATA *ch, int npulse )
{
	if ( IS_ADMIN(ch) )
		ch->wait = 1;
	else
		ch->wait = UMAX( ch->wait, (npulse) );
}


/*
 * Funzioni riguardanti le stanze
 */
bool IS_ROOM_AFFECTED( ROOM_INDEX_DATA *room, int sn )
{
	if ( IS_SET(room->affected_by, sn) )
		return TRUE;

	return FALSE;
}


bool IS_RAFFECTED( ROOM_INDEX_DATA *room, int sn )
{
	if ( IS_SET(room->affected_by, sn) )
		return TRUE;

	return FALSE;
}


CHAR_DATA *MOUNTED( CHAR_DATA *ch )
{
	if ( !IS_MOB(ch) && ch->mount && ch->riding )
		return ch->mount;

	return NULL;
}


CHAR_DATA *RIDDEN( CHAR_DATA *ch )
{
	if ( IS_MOB(ch) && ch->mount && ch->riding )
		return ch->mount;

	return NULL;
}


bool IS_WEAPON_STAT( OBJ_DATA *obj, int stat )
{
	if ( IS_SET(obj->value[4], stat) )
		return TRUE;

	return FALSE;
}


/*
 * Funzioni riguardanti la descrizione del pg.
 */
char *PERS( CHAR_DATA *ch, CHAR_DATA *looker )
{
	if ( can_see(looker, ch) )
	{
		if ( IS_MOB(ch) || (!IS_MOB(ch) && ch->short_descr[0] != '\0') )
			return ch->short_descr;
		else
		{
			if ( IS_VAMPIRE(ch) && !IS_ADMIN(looker) )
				return "Una sgradevole creatura";
			else
				return ch->name;
		}
	}

	return "qualcuno";
}
