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
#include <time.h>
#include <unistd.h>

#include "xandra.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "file.h"


extern int	max_on;

extern bool xandra_down;
extern int  xandra_exit;


/*
 * Funzioni esterne
 */
bool	RACE_OK		( CHAR_DATA *ch, int skill );


void do_cabal_scan( CHAR_DATA *ch, char *argument )
{
	int			i;
	char		buf1[MIL];
	char		buf2[MIL];
	OBJ_DATA	*in_obj;
	int			show;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_ADMIN(ch) && (ch->cabal == CABAL_NONE || IS_MOB(ch)) )
	{
		send_to_char( "Non appartieni a nessuna cabala.\n\r", ch );
		return;
	}

	for ( i = 1; i < MAX_CABAL; i++ )
	{
		if ( IS_ADMIN(ch) || ch->cabal == i )
			show = 1;
		else
			show = 0;
		sprintf( buf1, " Cabala: %-11s, stanza %4d, oggetto %4d, ptr: %-20s ",
			cabal_table[i].short_name,
			cabal_table[i].room_vnum,
			cabal_table[i].obj_vnum,
			cabal_table[i].obj_ptr != NULL ? cabal_table[i].obj_ptr->short_descr : "(NULL)" );

		if ( cabal_table[i].obj_ptr != NULL )
		{
			for ( in_obj = cabal_table[i].obj_ptr; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
				;
			if ( in_obj->carried_by )
				sprintf( buf2, "\n\r\t\tcarried_by: %s\n\r", PERS(in_obj->carried_by, ch) );
			else
			{
				sprintf( buf2, "\n\r\t\t\t\t\tin_room: %s\n\r", in_obj->in_room != NULL ? in_obj->in_room->name : "BUG!!" );
				if ( in_obj->in_room != NULL && in_obj->in_room->vnum == cabal_table[ch->cabal].room_vnum )
					show = 1;
			}
		}

		if ( show )
		{
			send_to_char( buf1, ch );
			send_to_char( buf2, ch );
		}
	}
}


void do_objlist( CHAR_DATA *ch, char *argument )
{
	FILE		*fp;
	OBJ_DATA	*obj;
	AFFECT_DATA *paf;
	/*~~~~~~~~~~~~~*/

	if ( (fp = fopen("objlist.txt", "w+")) == NULL )
	{
		send_to_char( "File error.\n\r", ch );
		return;
	}

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( CAN_WEAR(obj, ITEM_WIELD) && (obj->level < 25 && obj->level > 15) )
		{
			fprintf( fp, "\n#Obj: %s (Vnum : %d) \n", obj->short_descr, obj->pIndexData->vnum );
			fprintf( fp, "'%s' , oggetto di tipo %s, extra flags %s.\nPeso %d, Valore %d, livello %d.\n",
				obj->name,
				item_type_name(obj),
				extra_bit_name(obj->extra_flags),
				obj->weight / 10,
				obj->cost,
				obj->level );

			switch ( obj->item_type )
			{
			case ITEM_SCROLL:
			case ITEM_POTION:
			case ITEM_PILL:
				fprintf( fp, "%d livello dell'incantesimo di:", obj->value[0] );

				if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
					fprintf( fp, " '%s'", skill_table[obj->value[1]].name );

				if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
					fprintf( fp, " '%s'", skill_table[obj->value[2]].name );

				if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
					fprintf( fp, " '%s'", skill_table[obj->value[3]].name );

				if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL )
					fprintf( fp, " '%s'", skill_table[obj->value[4]].name );

				fprintf( fp, ".\n" );
				break;

			case ITEM_WAND:
			case ITEM_STAFF:
				fprintf( fp, "%d cariche di livello %d", obj->value[2], obj->value[0] );

				if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
					fprintf( fp, " '%s'", skill_table[obj->value[3]].name );

				fprintf( fp, ".\n" );
				break;

			case ITEM_DRINK_CON:

				fprintf( fp, "Contiene una sostanza di colore %s somigliante a %s.\n",
					liq_table[obj->value[2]].liq_color, liq_table[obj->value[2]].liq_translate );
				break;

			case ITEM_CONTAINER:
				fprintf( fp, "Capacita': %d#  Peso massimo sostenibile: %d#  flags: %s\n",
					obj->value[0],
					obj->value[3],
					cont_bit_name(obj->value[1]) );
				if ( obj->value[4] != 100 )
					fprintf( fp, "Moltiplicatore del peso: %d%%\n", obj->value[4] );

				break;

			case ITEM_WEAPON:
				fprintf( fp, "Tipo dell'arma " );
				switch ( obj->value[0] )
				{
				case ( WEAPON_EXOTIC ):		fprintf( fp, "esotica.\n" );		 break;
				case ( WEAPON_SWORD ):		fprintf( fp, "spada.\n" );			 break;
				case ( WEAPON_DAGGER ):		fprintf( fp, "daga o pugnale.\n" );		 break;
				case ( WEAPON_SPEAR ):		fprintf( fp, "giavellotto.\n" );	 break;
				case ( WEAPON_MACE ):		fprintf( fp, "mazza o clava.\n" );	 break;
				case ( WEAPON_AXE ):		fprintf( fp, "ascia.\n" );			 break;
				case ( WEAPON_FLAIL ):		fprintf( fp, "correggia.\n" );		 break;
				case ( WEAPON_WHIP ):		fprintf( fp, "frusta.\n" );			 break;
				case ( WEAPON_POLEARM ):	fprintf( fp, "polearm.\n" );		 break;
				case ( WEAPON_BOW ):		fprintf( fp, "arco.\n" );			 break;
				case ( WEAPON_ARROW ):		fprintf( fp, "freccia.\n" );		 break;
				case ( WEAPON_LANCE ):		fprintf( fp, "lancia.\n" );			 break;
				default:					fprintf( fp, "sconosciuto.\n" );	 break;
				}

				if ( obj->pIndexData->new_format )
					fprintf( fp, "Danno %dd%d (media %d).\n", obj->value[1], obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2 );
				else
					fprintf( fp, "Danno da %d a %d (media %d).\n", obj->value[1], obj->value[2], (obj->value[1] + obj->value[2]) / 2 );

				if ( obj->value[4] )	/* weapon flags */
					fprintf( fp, "Flag dell'arma: %s\n", weapon_bit_name(obj->value[4]) );
				break;

			case ITEM_ARMOR:
				fprintf( fp, "Classe armatura %d pugnalate, %d colpi, %d fendenti, and %d vs. armi esotiche.\n",
					obj->value[0],
					obj->value[1],
					obj->value[2],
					obj->value[3] );
				break;
			}

			for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
			{
				if ( paf == NULL )
					continue;
				fprintf( fp, "  Aggiunge i seguenti effetti %s by %d.\n", affect_loc_name(paf->location), paf->modifier );
				if ( paf->bitvector )
				{
					switch ( paf->where )
					{
					case TO_AFFECTS:	fprintf( fp, "   Effetto %s.\n", affect_bit_name(paf->bitvector) );			break;
					case TO_OBJECT:		fprintf( fp, "   Effetto %s all'oggetto.\n", extra_bit_name(paf->bitvector) );		break;
					case TO_IMMUNE:		fprintf( fp, "   Immunita' a %s.\n", imm_bit_name(paf->bitvector) );		break;
					case TO_RESIST:		fprintf( fp, "   Resistenza a %s.\n\r", imm_bit_name(paf->bitvector) );	break;
					case TO_VULN:		fprintf( fp, "   Vulnerabilita' a %s.\n\r", imm_bit_name(paf->bitvector) ); break;
					case TO_DETECTS:	fprintf( fp, "   Permette la distinzione de %s.\n\r", detect_bit_name(paf->bitvector) );	break;
					default:			fprintf( fp, "   Sconosciuto %d: %d\n\r", paf->where, paf->bitvector );			break;
					}
				}

			}
		}
	}
	fclose( fp );
}


void do_limited( CHAR_DATA *ch, char *argument )
{
	extern int		top_obj_index;
	OBJ_DATA		*obj;
	OBJ_INDEX_DATA	*obj_index;
	char			buf[MSL];
	char			output[4 * MSL];
	int				lCount = 0;
	int				ingameCount;
	int				nMatch;
	int				vnum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] != '\0' )
	{
		obj_index = get_obj_index( atoi(argument) );
		if ( obj_index == NULL )
		{
			send_to_char( "Non trovato.\n\r", ch );
			return;
		}

		if ( obj_index->limit == -1 )
		{
			send_to_char( "Non e' un oggetto limitato.\n\r", ch );
			return;
		}

		nMatch = 0;
		sprintf( buf, "%-35s [%5d]  Limit: %3d  Current: %3d\n\r",
			obj_index->short_descr,
			obj_index->vnum,
			obj_index->limit,
			obj_index->count );
		buf[0] = UPPER( buf[0] );
		send_to_char( buf, ch );
		ingameCount = 0;
		for ( obj = object_list; obj != NULL; obj = obj->next )
		{
			if ( obj->pIndexData->vnum == obj_index->vnum )
			{
				ingameCount++;
				if ( obj->carried_by != NULL )
					sprintf( buf, "Carried by %-30s\n\r", obj->carried_by->name );
				if ( obj->in_room != NULL )
					sprintf( buf, "At %-20s [%d]\n\r", obj->in_room->name, obj->in_room->vnum );
				if ( obj->in_obj != NULL )
					sprintf( buf, "In %-20s [%d] \n\r", obj->in_obj->short_descr, obj->in_obj->pIndexData->vnum );
				send_to_char( buf, ch );
			}
		}

		sprintf( buf, "  %d found in game. %d should be in pFiles.\n\r", ingameCount, obj_index->count - ingameCount );
		send_to_char( buf, ch );
		return;
	}

	nMatch = 0;
	output[0] = '\0';
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
	{
		if ( (obj_index = get_obj_index(vnum)) != NULL )
		{
			nMatch++;
			if ( obj_index->limit != -1 )
			{
				lCount++;
				sprintf
				(
					buf,
					"%-37s [%5d]  Limit: %3d  Current: %3d\n\r",
					obj_index->short_descr,
					obj_index->vnum,
					obj_index->limit,
					obj_index->count
				);
				buf[0] = UPPER( buf[0] );
				strcat( output, buf );
			}
		}
	}

	sprintf( buf, "\n\r%d of %d objects are limited.\n\r", lCount, nMatch );
	strcat( output, buf );
	page_to_char( output, ch );
}


void do_wiznet( CHAR_DATA *ch, char *argument )
{
	int		flag;
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sintassi: wiznet on\n\r", ch );
		send_to_char( "          wiznet off\n\r", ch );
		send_to_char( "          wiznet show\n\r\n\r", ch );

		send_to_char( "Stai visualizzando i seguenti sottocanali wiznet:\n\r", ch );

		buf[0] = '\0';
		for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
		{
			if ( IS_SET(ch->wiznet, wiznet_table[flag].flag) )
			{
				strcat( buf, wiznet_table[flag].name );
				strcat( buf, " " );
			}
		}
		strcat( buf, "\n\r" );
		send_to_char( buf, ch );

		return;
	}

	if ( !str_prefix(argument, "on") )
	{
		send_to_char( "Welcome to Wiznet!\n\r", ch );
		SET_BIT( ch->wiznet, WIZ_ON );
		return;
	}

	if ( !str_prefix(argument, "off") )
	{
		send_to_char( "Signing off of Wiznet.\n\r", ch );
		REMOVE_BIT( ch->wiznet, WIZ_ON );
		return;
	}

	if ( !str_prefix(argument, "show") )
	/* list of all wiznet options */
	{
		buf[0] = '\0';

		for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
		{
			if ( wiznet_table[flag].level <= get_trust(ch) )
			{
				strcat( buf, wiznet_table[flag].name );
				strcat( buf, " " );
			}
		}

		strcat( buf, "\n\r" );

		send_to_char( "Wiznet options available to you are:\n\r", ch );
		send_to_char( buf, ch );
		return;
	}

	flag = wiznet_lookup( argument );

	if ( flag == -1 || get_trust(ch) < wiznet_table[flag].level )
	{
		send_to_char( "No such option.\n\r", ch );
		return;
	}

	if ( IS_SET(ch->wiznet, wiznet_table[flag].flag) )
	{
		sprintf( buf, "You will no longer see %s on wiznet.\n\r", wiznet_table[flag].name );
		send_to_char( buf, ch );
		REMOVE_BIT( ch->wiznet, wiznet_table[flag].flag );
		return;
	}
	else
	{
		sprintf( buf, "You will now see %s on wiznet.\n\r", wiznet_table[flag].name );
		send_to_char( buf, ch );
		SET_BIT( ch->wiznet, wiznet_table[flag].flag );
		return;
	}
}


void wiznet( char *string, CHAR_DATA *ch, OBJ_DATA *obj, long flag, int min_level )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if (d->connected == CON_PLAYING
		&&	IS_ADMIN(d->character)
		&&	IS_SET(d->character->wiznet, WIZ_ON)
		&&	(!flag || IS_SET(d->character->wiznet, flag))
		&&	get_trust(d->character) >= min_level
		&&	d->character != ch )
		{
			send_to_char( "--> ", d->character );
			act( string, d->character, obj, ch, TO_CHAR, POS_DEAD );
		}
	}
}


void do_tick( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "tick area : area update\n\r", ch );
		send_to_char( "tick char : char update\n\r", ch );
		send_to_char( "tick obj  : obj  update\n\r", ch );
		send_to_char( "tick room : room update\n\r", ch );
		send_to_char( "tick track: track update\n\r", ch );
		return;
	}

	if ( is_name(arg, "area") )
	{
		area_update();
		send_to_char( "Area updated.\n\r", ch );
		return;
	}

	if ( is_name(arg, "char player giocatori") )
	{
		char_update();
		send_to_char( "Players updated.\n\r", ch );
		return;
	}

	if ( is_name(arg, "obj object oggetti") )
	{
		obj_update();
		send_to_char( "Obj updated.\n\r", ch );
		return;
	}

	if ( is_name(arg, "room stanze") )
	{
		room_update();
		send_to_char( "Room updated.\n\r", ch );
		return;
	}

	if ( is_name(arg, "track tracce") )
	{
		track_update();
		send_to_char( "Track updated.\n\r", ch );
		return;
	}

	do_tick( ch, "" );
}


/*
 * Ritorna la lista dei titoli dei pg.
 */
void do_atitle( CHAR_DATA *ch, char *argument )
{
	char		name[MSL];
	char		buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, name );

	if ( argument[0] == '\0' || name[0] == '\0' )
	{
		DESCRIPTOR_DATA *d;
		/*~~~~~~~~~~~~~~~*/

		send_to_char( "Sintassi: atitle\n\r", ch );
		send_to_char( "Sintassi: atitle <nome pg> <descr pers>\n\r", ch );
		send_to_char( "Sintassi: atitle <nome pg> cancella\n\r\n\r", ch );

		send_to_char( "Nome         Titolo\n\r", ch );
		send_to_char( "--------------------------------------------------------------------------------\n\r", ch );

		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			victim = ( d->original != NULL ) ? d->original : d->character;

			/* Controlla se è in gioco. */
			if ( d->connected != CON_PLAYING )
				continue;

			sprintf( buf, "%-12s %s\n\r",
				victim->name, (victim->short_descr[0] == '\0') ? "(vuota)" : victim->short_descr );
			send_to_char( buf, ch );
		}

		return;
	}

	victim = get_char_world( ch, name );
	if ( !victim )
	{
		send_to_char( "There is no such a person online.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "You cannot use rpgpers on NPCs.\n\r", ch );
		return;
	}

	if ( (victim != ch) && (get_trust(victim) >= get_trust(ch)) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( !victim->desc || (victim->desc->connected != CON_PLAYING) )
	{
		send_to_char( "This player has lost his link or is inside a pager or the like.\n\r", ch );
		return;
	}

	if ( strlen(argument) > 30 )
	{
		send_to_char( "Titolo troppo lungo. Massimo consentito e' di 20 caratteri\n\r", ch );
		return;
	}

	if ( !str_cmp(argument, "cancella") || !str_cmp(argument, "delete") )
	{
		free_string( victim->short_descr );
		victim->short_descr = str_dup( "" );
		send_to_char( "Titolo cancellato.\n\r", ch );
	}
	else
	{
		free_string( victim->short_descr );
		victim->short_descr = str_dup( argument );
		send_to_char( "Titolo modificato.\n\r", ch );
	}
}


/*
 * RT nochannels command, for those spammers
 */
void do_nochannels( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL], buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Nochannel whom?", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET(victim->comm, COMM_NOCHANNELS) )
	{
		REMOVE_BIT( victim->comm, COMM_NOCHANNELS );
		send_to_char( "Ora puoi riprendere a comunicare liberamente.\n\r", victim );
		send_to_char( "NOCHANNELS rimosso.\n\r", ch );
		sprintf( buf, "$N restores channels to %s", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOCHANNELS );
		send_to_char( "Un'ombra cala su di te.. non riesci piu' a esprimerti in nessun modo!\n\r", victim );
		send_to_char( "NOCHANNELS impostato.\n\r", ch );
		sprintf( buf, "$N revokes %s's channels.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
}


void do_deny( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL], buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Deny whom?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	SET_BIT( victim->act, PLR_DENY );
	send_to_char( "Ti e' stata revocata la possibilita' di accedere al mud!\n\r", victim );
	sprintf( buf, "$N denies access to %s", victim->name );
	wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	send_to_char( "OK.\n\r", ch );
	save_char_obj( victim );
	stop_fighting( victim, TRUE );
	do_quit( victim, "" );
}


void do_disconnect( CHAR_DATA *ch, char *argument )
{
	char			arg[MIL];
	DESCRIPTOR_DATA *d, *d_next;
	CHAR_DATA		*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Disconnect whom?\n\r", ch );
		return;
	}

	if ( is_number(arg) )
	{
		int desc;
		/*~~~~~*/

		desc = atoi( arg );
		for ( d = descriptor_list; d != NULL; d = d_next )
		{
			d_next = d->next;
			if ( d->descriptor == desc )
			{
				close_socket( d );
				send_to_char( "Ok.\n\r", ch );
				return;
			}
		}
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL )
	{
		act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	for ( d = descriptor_list; d != NULL; d = d_next )
	{
		d_next = d->next;
		if ( d == victim->desc )
		{
			close_socket( d );
			send_to_char( "Ok.\n\r", ch );
			return;
		}
	}

	bug( "Do_disconnect: desc not found.", 0 );
	send_to_char( "Descriptor not found!\n\r", ch );
}


void do_echo( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Global echo what?\n\r", ch );
		return;
	}

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( d->connected == CON_PLAYING )
		{
			if ( get_trust(d->character) >= get_trust(ch) )
				send_to_char( "global> ", d->character );
			send_to_char( argument, d->character );
			send_to_char( "\n\r", d->character );
		}
	}
}


void do_recho( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Local echo what?\n\r", ch );

		return;
	}

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( d->connected == CON_PLAYING && d->character->in_room == ch->in_room )
		{
			if ( get_trust(d->character) >= get_trust(ch) )
				send_to_char( "local> ", d->character );
			send_to_char( argument, d->character );
			send_to_char( "\n\r", d->character );
		}
	}
}


void do_zecho( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Zone echo what?\n\r", ch );
		return;
	}

	for ( d = descriptor_list; d; d = d->next )
	{
		if
		(
			d->connected == CON_PLAYING
		&&	d->character->in_room != NULL
		&&	ch->in_room != NULL
		&&	d->character->in_room->area == ch->in_room->area
		)
		{
			if ( get_trust(d->character) >= get_trust(ch) )
				send_to_char( "zone> ", d->character );
			send_to_char( argument, d->character );
			send_to_char( "\n\r", d->character );
		}
	}
}


void do_pecho( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( argument[0] == '\0' || arg[0] == '\0' )
	{
		send_to_char( "Personal echo what?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "Target not found.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) && get_trust(ch) != MAX_LEVEL )
		send_to_char( "personal> ", victim );

	send_to_char( argument, victim );
	send_to_char( "\n\r", victim );
	send_to_char( "personal> ", ch );
	send_to_char( argument, ch );
	send_to_char( "\n\r", ch );
}


ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~*/

	if ( is_number(arg) )
		return( get_room_index(atoi(arg)) );

	if ( (victim = get_char_world(ch, arg)) != NULL )
		return( victim->in_room );

	if ( (obj = get_obj_world(ch, arg)) != NULL )
		return( obj->in_room );

	return( NULL );
}


void do_transfer( CHAR_DATA *ch, char *argument )
{
	char			arg1[MIL];
	char			arg2[MIL];
	ROOM_INDEX_DATA *location;
	DESCRIPTOR_DATA *d, *d_next;

	CHAR_DATA		*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "Transfer whom (and where)?\n\r", ch );
		return;
	}

	if ( !str_cmp(arg1, "all") )
	{
		for ( d = descriptor_list; d != NULL; d = d_next )
		{
			d_next = d->next;
			if ( d->connected == CON_PLAYING && d->character != ch && d->character->in_room != NULL && can_see(ch, d->character) )
			{

				char	buf[MSL];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				sprintf( buf, "%s %s", d->character->name, arg2 );
				do_transfer( ch, buf );
			}
		}

		return;
	}

	/* Thanks to Grodyn for the optional location parameter. */
	if ( arg2[0] == '\0' )
	{
		location = ch->in_room;
	}
	else
	{
		if ( (location = find_location(ch, arg2)) == NULL )
		{
			send_to_char( "No such location.\n\r", ch );
			return;
		}

		/* if ( !is_room_owner(ch,location) && room_is_private( location ) */
		if ( room_is_private(location) && get_trust(ch) < MAX_LEVEL )
		{
			send_to_char( "That room is private right now.\n\r", ch );
			return;
		}
	}

	if ( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->in_room == NULL )
	{
		send_to_char( "They are in limbo.\n\r", ch );
		return;
	}

	if ( victim->fighting != NULL )
		stop_fighting( victim, TRUE );
	act( "$n scompare improvvisamente.", victim, NULL, NULL, TO_ROOM, POS_REST );
	char_from_room( victim );
	char_to_room( victim, location );
	act( "$n appare con un'esplosione di luce.", victim, NULL, NULL, TO_ROOM, POS_REST );
	if ( ch != victim )
		act( "$n ti ha teleportato.", ch, NULL, victim, TO_VICT, POS_REST );
	do_look( victim, "auto" );
	send_to_char( "Ok.\n\r", ch );
}


void do_at( CHAR_DATA *ch, char *argument )
{
	char			arg[MIL];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	OBJ_DATA		*on;
	CHAR_DATA		*wch, *wch_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "At where what?\n\r", ch );
		return;
	}

	if ( (location = find_location(ch, arg)) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	/* if (!is_room_owner(ch,location) && room_is_private( location ) */
	if ( room_is_private(location) && get_trust(ch) < MAX_LEVEL )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	original = ch->in_room;
	on = ch->on;
	char_from_room( ch );
	char_to_room( ch, location );
	interpret( ch, argument, FALSE );

	/* See if 'ch' still exists before continuing! Handles 'at XXXX quit' case. */
	for ( wch = char_list; wch != NULL; wch = wch_next )
	{
		wch_next = wch->next;
		if ( wch == ch )
		{
			char_from_room( ch );
			char_to_room( ch, original );
			ch->on = on;
			break;
		}
	}
}

void do_iget( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char("Non c'e' nessuno con quel nome qui.\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry( victim, arg2 ) ) == NULL )
	{
		if ( ( obj = get_obj_wear( victim, arg2 ) ) == NULL )
		{
			send_to_char( "Non ha quell' oggetto.\n\r", ch );
			return;
		}
	}

	obj_from_char( obj );
	obj_to_char( obj, ch );

	act( "Con il pensiero sposti $p dalle mani di $N alle tue.", ch, obj, victim, TO_CHAR, POS_DEAD );
}

void do_goto( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *start_loc;
	ROOM_INDEX_DATA *dest_loc;
	CHAR_DATA		*rch;
	char			buf[MSL];
	int				count = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Goto where?\n\r", ch );
		return;
	}

	if ( (dest_loc = find_location(ch, argument)) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	count = 0;
	for ( rch = dest_loc->people; rch != NULL; rch = rch->next_in_room )
		count++;

	/*
	 * if (!is_room_owner(ch,dest_loc) && room_is_private(dest_loc) && (count > 1 || get_trust(ch) < MAX_LEVEL)) {
	 * send_to_char( "That room is private right now.\n\r", ch ); return; }
	 */
	if ( ch->fighting != NULL )
		stop_fighting( ch, TRUE );

	sprintf( buf, "$n scompare andando verso %s (%d).", dest_loc->name, dest_loc->vnum );
	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( IS_ADMIN(rch) && get_trust(rch) >= ch->invis_level )
			act( buf, ch, NULL, rch, TO_VICT, POS_DEAD );
	}

	start_loc = ch->in_room;

	char_from_room( ch );
	char_to_room( ch, dest_loc );

	sprintf( buf, "$n appare venendo da %s (%d).", start_loc->name, start_loc->vnum );
	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( IS_ADMIN(rch) && get_trust(rch) >= ch->invis_level )
			act( buf, ch, NULL, rch, TO_VICT, POS_DEAD );
	}

	do_look( ch, "auto" );
}


void do_violate( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *start_loc;
	ROOM_INDEX_DATA *dest_loc;
	CHAR_DATA		*rch;
	char			buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Goto where?\n\r", ch );
		return;
	}

	if ( (dest_loc = find_location(ch, argument)) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	if ( !room_is_private(dest_loc) )
	{
		send_to_char( "That room isn't private, use goto.\n\r", ch );
		return;
	}

	if ( ch->fighting != NULL )
		stop_fighting( ch, TRUE );

	sprintf( buf, "$n scompare andando verso %s (%d).", dest_loc->name, dest_loc->vnum );
	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( IS_ADMIN(rch) && get_trust(rch) >= ch->invis_level )
			act( buf, ch, NULL, rch, TO_VICT, POS_DEAD );
	}

	start_loc = ch->in_room;

	char_from_room( ch );
	char_to_room( ch, dest_loc );

	sprintf( buf, "$n appare venendo da %s (%d).", start_loc->name, start_loc->vnum );
	sprintf( buf, "$n appare venendo da %s (%d).", ch->was_in_room->name, ch->was_in_room->vnum );
	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( IS_ADMIN(rch) && get_trust(rch) >= ch->invis_level )
			act( buf, ch, NULL, rch, TO_VICT, POS_DEAD );
	}

	do_look( ch, "auto" );
}


/*
 * RT to replace the 3 stat commands
 */
void do_stat( CHAR_DATA *ch, char *argument )
{
	char			arg[MIL];
	char			*string;
	OBJ_DATA		*obj;
	ROOM_INDEX_DATA *location;
	CHAR_DATA		*victim;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	string = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  stat <name>\n\r", ch );
		send_to_char( "  stat obj <name>\n\r", ch );
		send_to_char( "  stat mob <name>\n\r", ch );
		send_to_char( "  stat room <number>\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "room") )
	{
		do_rstat( ch, string );
		return;
	}

	if ( !str_cmp(arg, "obj") )
	{
		do_ostat( ch, string );
		return;
	}

	if ( !str_cmp(arg, "char") || !str_cmp(arg, "mob") )
	{
		do_mstat( ch, string );
		return;
	}

	/* do it the old way */
	obj = get_obj_world( ch, argument );
	if ( obj != NULL )
	{
		do_ostat( ch, argument );
		return;
	}

	victim = get_char_world( ch, argument );
	if ( victim != NULL )
	{
		do_mstat( ch, argument );
		return;
	}

	location = find_location( ch, argument );
	if ( location != NULL )
	{
		do_rstat( ch, argument );
		return;
	}

	send_to_char( "Nothing by that name found anywhere.\n\r", ch );
}


void do_rstat( CHAR_DATA *ch, char *argument )
{
	char				buf[MSL];
	char				arg[MIL];
	ROOM_INDEX_DATA		*location;
	ROOM_HISTORY_DATA	*rh;
	OBJ_DATA			*obj;
	CHAR_DATA			*rch;
	int					door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
	if ( location == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	/* if (!is_room_owner(ch,location) && ch->in_room != location */
	if ( ch->in_room != location && room_is_private(location) && !IS_TRUSTED(ch, LEVEL_IMPLEMENTOR) )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	if ( ch->in_room->affected_by )
	{
		sprintf( buf, "Affected by %s\n\r", raffect_bit_name(ch->in_room->affected_by) );
		send_to_char( buf, ch );
	}

	if ( ch->in_room->room_flags )
	{
		sprintf( buf, "Roomflags %s\n\r", flag_room_name(ch->in_room->room_flags) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "Name: '%s'\n\rArea: '%s'\n\rOwner: '%s'\n\r", location->name, location->area->name, location->owner );
	send_to_char( buf, ch );

	sprintf( buf, "Vnum: %d  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
		location->vnum,
		location->sector_type,
		location->light,
		location->heal_rate,
		location->mana_rate );
	send_to_char( buf, ch );

	sprintf( buf, "Room flags: %ld.\n\rDescription:\n\r%s", location->room_flags, location->description );
	send_to_char( buf, ch );

	if ( location->extra_descr != NULL )
	{
		EXTRA_DESCR_DATA	*ed;
		/*~~~~~~~~~~~~~~~~~~~~*/

		send_to_char( "Extra description keywords: '", ch );
		for ( ed = location->extra_descr; ed; ed = ed->next )
		{
			send_to_char( ed->keyword, ch );
			if ( ed->next != NULL )
				send_to_char( " ", ch );
		}

		send_to_char( "'.\n\r", ch );
	}

	send_to_char( "Characters:", ch );
	for ( rch = location->people; rch; rch = rch->next_in_room )
	{
		if ( can_see(ch, rch) )
		{
			send_to_char( " ", ch );
			one_argument( rch->name, buf );
			send_to_char( buf, ch );
		}
	}

	send_to_char( ".\n\rObjects:   ", ch );
	for ( obj = location->contents; obj; obj = obj->next_content )
	{
		send_to_char( " ", ch );
		one_argument( obj->name, buf );
		send_to_char( buf, ch );
	}

	send_to_char( ".\n\r", ch );

	for ( door = 0; door < MAX_DIR; door++ )
	{
		EXIT_DATA	*pexit;
		/*~~~~~~~~~~~~~~~*/

		if ( (pexit = location->exit[door]) != NULL )
		{
			sprintf( buf, "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",
				door,
				(pexit->to_room == NULL ? -1 : pexit->to_room->vnum),
				pexit->key,
				pexit->exit_info,
				pexit->keyword,
				pexit->description[0] != '\0' ? pexit->description : "(none).\n\r" );
			send_to_char( buf, ch );
		}
	}

	send_to_char( "Tracks:\n\r", ch );
	for ( rh = location->history; rh != NULL; rh = rh->next )
	{
		sprintf( buf, "%s took door %i.\n\r", rh->name, rh->went );
		send_to_char( buf, ch );
	}
}


void do_ostat( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MIL];
	AFFECT_DATA *paf;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Stat what?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_world(ch, argument)) == NULL )
	{
		send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
		return;
	}


	sprintf( buf, "Name(s): %s\n\r", obj->name );
	send_to_char( buf, ch );

	sprintf( buf, "Vnum: %d  Format: %s  Type: %s  Resets: %d\n\r",
		obj->pIndexData->vnum,
		obj->pIndexData->new_format ? "new" : "old",
		item_type_name(obj),
		obj->pIndexData->reset_num );
	send_to_char( buf, ch );

	sprintf( buf, "Short description: %s\n\rLong description: %s\n\r", obj->short_descr, obj->description );
	send_to_char( buf, ch );

	sprintf( buf, "Wear bits: %s\n\rExtra bits: %s\n\r", wear_bit_name(obj->wear_flags), extra_bit_name(obj->extra_flags) );
	send_to_char( buf, ch );

	sprintf( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
		1,
		get_obj_number(obj),
		obj->weight,
		get_obj_weight(obj),
		get_true_weight(obj) );
	send_to_char( buf, ch );

	sprintf( buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d Count: %d\n\r",
		obj->level,
		obj->cost,
		obj->condition,
		obj->timer,
		obj->pIndexData->count );
	send_to_char( buf, ch );

	sprintf( buf, "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
		obj->in_room == NULL ? 0 : obj->in_room->vnum,
		obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr,
		obj->carried_by == NULL ? "(none)" : can_see(ch, obj->carried_by) ? obj->carried_by->name : "someone",
		obj->wear_loc );
	send_to_char( buf, ch );

	if ( obj->material != NULL )
	{
		sprintf( buf, "Materiale: %s\n\r", material_name(obj->material) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "Values: %d %d %d %d %d\n\r", obj->value[0], obj->value[1], obj->value[2], obj->value[3], obj->value[4] );
	send_to_char( buf, ch );

	/* now give out vital statistics as per identify */
	switch ( obj->item_type )
	{
	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
		sprintf( buf, "Level %d spells of:", obj->value[0] );
		send_to_char( buf, ch );

		if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[obj->value[1]].name, ch );
			send_to_char( "'", ch );
		}

		if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[obj->value[2]].name, ch );
			send_to_char( "'", ch );
		}

		if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[obj->value[3]].name, ch );
			send_to_char( "'", ch );
		}

		if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[obj->value[4]].name, ch );
			send_to_char( "'", ch );
		}

		send_to_char( ".\n\r", ch );
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		sprintf( buf, "Has %d(%d) charges of level %d", obj->value[1], obj->value[2], obj->value[0] );
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
		sprintf( buf, "It holds %s-colored %s.\n\r", liq_table[obj->value[2]].liq_color, liq_table[obj->value[2]].liq_translate );
		send_to_char( buf, ch );
		break;

	case ITEM_WEAPON:
		send_to_char( "Weapon type is ", ch );
		switch ( obj->value[0] )
		{
		case ( WEAPON_EXOTIC ):		send_to_char( "esotica\n\r", ch ); break;
		case ( WEAPON_SWORD ):		send_to_char( "sword\n\r", ch ); break;
		case ( WEAPON_DAGGER ):		send_to_char( "dagger\n\r", ch ); break;
		case ( WEAPON_SPEAR ):		send_to_char( "giavellotto\n\r", ch ); break;
		case ( WEAPON_MACE ):		send_to_char( "mace/club\n\r", ch ); break;
		case ( WEAPON_AXE ):		send_to_char( "axe\n\r", ch ); break;
		case ( WEAPON_FLAIL ):		send_to_char( "flail\n\r", ch ); break;
		case ( WEAPON_WHIP ):		send_to_char( "whip\n\r", ch ); break;
		case ( WEAPON_POLEARM ):	send_to_char( "polearm\n\r", ch ); break;
		case ( WEAPON_BOW ):		send_to_char( "bow\n\r", ch ); break;
		case ( WEAPON_ARROW ):		send_to_char( "arrow\n\r", ch ); break;
		case ( WEAPON_LANCE ):		send_to_char( "lance\n\r", ch ); break;
		default:					send_to_char( "unknown\n\r", ch ); break;
		}

		if ( obj->pIndexData->new_format )
		{
			sprintf( buf, "Damage is %dd%d (average %d)\n\r", obj->value[1], obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2 );
		}
		else
		{
			sprintf( buf, "Damage is %d to %d (average %d)\n\r", obj->value[1], obj->value[2], (obj->value[1] + obj->value[2]) / 2 );
		}

		send_to_char( buf, ch );

		sprintf( buf, "Damage noun is %s.\n\r", attack_table[obj->value[3]].noun );
		send_to_char( buf, ch );

		if ( obj->value[4] )	/* weapon flags */
		{
			sprintf( buf, "Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]) );
			send_to_char( buf, ch );
		}
		break;

	case ITEM_ARMOR:
		sprintf
		(
			buf,
			"Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
			obj->value[0],
			obj->value[1],
			obj->value[2],
			obj->value[3]
		);
		send_to_char( buf, ch );
		break;

	case ITEM_CONTAINER:
		sprintf( buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r", obj->value[0], obj->value[3], cont_bit_name(obj->value[1]) );
		send_to_char( buf, ch );
		if ( obj->value[4] != 100 )
		{
			sprintf( buf, "Weight multiplier: %d%%\n\r", obj->value[4] );
			send_to_char( buf, ch );
		}
		break;
	}

	if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
	{
		EXTRA_DESCR_DATA	*ed;
		/*~~~~~~~~~~~~~~~~~~~~*/

		send_to_char( "Extra description keywords: '", ch );

		for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )

		{
			send_to_char( ed->keyword, ch );
			if ( ed->next != NULL )
				send_to_char( " ", ch );
		}

		for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
		{
			send_to_char( ed->keyword, ch );
			if ( ed->next != NULL )
				send_to_char( " ", ch );
		}

		send_to_char( "'\n\r", ch );
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		sprintf( buf, "Affects %s by %d, level %d", affect_loc_name(paf->location), paf->modifier, paf->level );
		send_to_char( buf, ch );
		if ( paf->duration > -1 )
			sprintf( buf, ", %d hours.\n\r", paf->duration );
		else
			sprintf( buf, ".\n\r" );
		send_to_char( buf, ch );
		if ( paf->bitvector )
		{
			switch ( paf->where )
			{
			case TO_AFFECTS:	sprintf( buf, "Adds %s affect.\n", affect_bit_name(paf->bitvector) ); break;
			case TO_WEAPON:		sprintf( buf, "Adds %s weapon flags.\n", weapon_bit_name(paf->bitvector) ); break;
			case TO_OBJECT:		sprintf( buf, "Adds %s object flag.\n", extra_bit_name(paf->bitvector) ); break;
			case TO_IMMUNE:		sprintf( buf, "Adds immunity to %s.\n", imm_bit_name(paf->bitvector) ); break;
			case TO_RESIST:		sprintf( buf, "Adds resistance to %s.\n\r", imm_bit_name(paf->bitvector) ); break;
			case TO_VULN:		sprintf( buf, "Adds vulnerability to %s.\n\r", imm_bit_name(paf->bitvector) ); break;
			case TO_DETECTS:	sprintf( buf, "Adds %s detection.\n\r", detect_bit_name(paf->bitvector) ); break;
			default:			sprintf( buf, "Unknown bit %d: %d\n\r", paf->where, paf->bitvector ); break;

			}

			send_to_char( buf, ch );
		}
	}

	if ( !obj->enchanted )
	{
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			sprintf( buf, "Affects %s by %d, level %d.\n\r", affect_loc_name(paf->location), paf->modifier, paf->level );
			send_to_char( buf, ch );
			if ( paf->bitvector )
			{
				switch ( paf->where )
				{
				case TO_AFFECTS:	sprintf( buf, "Adds %s affect.\n", affect_bit_name(paf->bitvector) ); break;
				case TO_OBJECT:		sprintf( buf, "Adds %s object flag.\n", extra_bit_name(paf->bitvector) ); break;
				case TO_IMMUNE:		sprintf( buf, "Adds immunity to %s.\n", imm_bit_name(paf->bitvector) ); break;
				case TO_RESIST:		sprintf( buf, "Adds resistance to %s.\n\r", imm_bit_name(paf->bitvector) ); break;
				case TO_VULN:		sprintf( buf, "Adds vulnerability to %s.\n\r", imm_bit_name(paf->bitvector) ); break;
				case TO_DETECTS:	sprintf( buf, "Adds %s detection.\n\r", detect_bit_name(paf->bitvector) ); break;
				default:			sprintf( buf, "Unknown bit %d: %d\n\r", paf->where, paf->bitvector ); break;
				}

				send_to_char( buf, ch );
			}
		}
	}

	sprintf( buf, "Object progs: " );
	if ( obj->pIndexData->progtypes != 0 )
	{
		if ( IS_SET(obj->progtypes, OPROG_GET) )		strcat( buf, "get " );
		if ( IS_SET(obj->progtypes, OPROG_DROP) )		strcat( buf, "drop " );
		if ( IS_SET(obj->progtypes, OPROG_SAC) )		strcat( buf, "sacrifice " );
		if ( IS_SET(obj->progtypes, OPROG_GIVE) )		strcat( buf, "give " );
		if ( IS_SET(obj->progtypes, OPROG_FIGHT) )		strcat( buf, "fight " );
		if ( IS_SET(obj->progtypes, OPROG_DEATH) )		strcat( buf, "death " );
		if ( IS_SET(obj->progtypes, OPROG_SPEECH) )		strcat( buf, "speech " );
		if ( IS_SET(obj->progtypes, OPROG_AREA) )		strcat( buf, "area " );
		if ( IS_SET(obj->progtypes, OPROG_WEAR ) )		strcat( buf, "wear " );
		if ( IS_SET(obj->progtypes, OPROG_REMOVE ) )	strcat( buf, "remove " );
		if ( IS_SET(obj->progtypes, OPROG_ENTRY ) )		strcat( buf, "entry " );
		if ( IS_SET(obj->progtypes, OPROG_GREET ) )		strcat( buf, "greet " );
	}

	strcat( buf, "\n\r" );
	send_to_char( buf, ch );
	sprintf( buf, "Damage condition : %d (%s) ", obj->condition, get_cond_alias(obj) );
	send_to_char( buf, ch );
	send_to_char( "\n\r", ch );
}


void do_mstat( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		buf2[MSL];
	char		arg[MIL];
	AFFECT_DATA *paf;
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Stat whom?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, argument)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	sprintf( buf, "Name: [%s] Reset Zone: [%s] Logon: %s\r",
		victim->name,
		(IS_MOB(victim) && victim->zone) ? victim->zone->name : "?",
		ctime(&ch->logon) );
	send_to_char( buf, ch );

	sprintf( buf, "Vnum: %d  Format: %s  Race: %s(%s)  Group: %d  Sex: %s  Room: %d\n\r",
		IS_MOB(victim) ? victim->pIndexData->vnum : 0,
		IS_MOB(victim) ? victim->pIndexData->new_format ? "new" : "old" : "pc",
		RACE_NAME(victim),
		RACE_NAME_ORG(victim),
		IS_MOB(victim) ? victim->group : 0,
		sex_table[victim->sex].name,
		victim->in_room == NULL ? 0 : victim->in_room->vnum );
	send_to_char( buf, ch );

	if ( IS_MOB(victim) )
	{
		sprintf( buf, "Count: %d  Killed: %d  ---  Status: %d  Cabal: %d\n\r",
			victim->pIndexData->count,
			victim->pIndexData->killed,
			victim->status,
			victim->cabal );
		send_to_char( buf, ch );
	}

	sprintf( buf, "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d) Cha: %d(%d)\n\r",
		victim->perm_stat[STR],
		get_curr_stat(victim, STR),
		victim->perm_stat[INT],
		get_curr_stat(victim, INT),
		victim->perm_stat[WIS],
		get_curr_stat(victim, WIS),
		victim->perm_stat[DEX],
		get_curr_stat(victim, DEX),
		victim->perm_stat[CON],
		get_curr_stat(victim, CON),
		victim->perm_stat[CHA],
		get_curr_stat(victim, CHA) );
	send_to_char( buf, ch );

	sprintf( buf, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Practices: %d\n\r",
		victim->point[LIFE],
		victim->max_point[LIFE],
		victim->point[MANA],
		victim->max_point[MANA],
		victim->point[MOVE],
		victim->max_point[MOVE],
		IS_MOB(ch) ? 0 : victim->practice );
	send_to_char( buf, ch );

	if ( IS_MOB(victim) )
		sprintf( buf2, "%d", victim->alignment );
	else
	{
		sprintf( buf2, "%s", victim->ethos == 1 ? "Law-" : victim->ethos == 2 ? "Neut-" : victim->ethos == 3 ? "Cha-" : "none-" );
		strcat( buf2, IS_GOOD(victim) ? "Good" : IS_NEUTRAL(victim) ? "Neut" : IS_EVIL(victim) ? "Evil" : "Other" );
	}

	sprintf( buf, "It belives the religion of %s.\n\r", IS_MOB(victim) ? "Chronos" : religion_table[victim->religion].leader );
	send_to_char( buf, ch );
	sprintf( buf, "Lv: %d  Class: %s  Align: %s  Gold: %ld  Silver: %ld  Exp: %d\n\r",
		victim->level,
		IS_MOB(victim) ? "mobile"
			: (victim->sex == SEX_FEMALE) ? class_table[victim->class].fname : class_table[victim->class].name,
		buf2,
		victim->gold,
		victim->silver,
		victim->exp );
	send_to_char( buf, ch );

	sprintf( buf, "Punti Esp: %d  Punti esperienza per il prossimo livello: %d\n\r",
		victim->exp,
		IS_MOB(victim) ? 0 : exp_to_level(victim, victim->pcdata->points) );
	send_to_char( buf, ch );

	sprintf( buf, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
		GET_AC(victim, AC_PIERCE),
		GET_AC(victim, AC_BASH),
		GET_AC(victim, AC_SLASH),
		GET_AC(victim, AC_EXOTIC) );
	send_to_char( buf, ch );

	sprintf( buf, "Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
		GET_HITROLL(victim),
		GET_DAMROLL(victim),
		victim->saving_throw,
		size_table[victim->size].name,
		position_table[victim->position].name,
		victim->wimpy );
	send_to_char( buf, ch );

	if ( IS_MOB(victim) && victim->pIndexData->new_format )
	{
		sprintf( buf, "Damage: %dd%d  Message:  %s\n\r",
			victim->damage[DICE_NUMBER],
			victim->damage[DICE_TYPE],
			attack_table[victim->dam_type].noun );
		send_to_char( buf, ch );
	}

	sprintf( buf, "Fighting: %s Death: %d Carry number: %d  Carry weight: %ld\n\r",
		victim->fighting ? victim->fighting->name : "(none)",
		IS_MOB(victim) ? 0 : victim->pcdata->death,
		victim->carry_number,
		get_carry_weight(victim) / 10 );
	send_to_char( buf, ch );

	if ( !IS_MOB(victim) )
	{
		sprintf( buf, "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d Bloodlust: %d Desire: %d\n\r",
			victim->pcdata->condition[COND_THIRST],
			victim->pcdata->condition[COND_HUNGER],
			victim->pcdata->condition[COND_FULL],
			victim->pcdata->condition[COND_DRUNK],
			victim->pcdata->condition[COND_BLOODLUST],
			victim->pcdata->condition[COND_DESIRE] );
		send_to_char( buf, ch );
	}

	if ( !IS_MOB(victim) )
	{
		sprintf( buf, "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
			get_age(victim),
			(int) (victim->played + current_time - victim->logon) / 3600,
			victim->pcdata->last_level,
			victim->timer );
		send_to_char( buf, ch );
	}

	sprintf( buf, "Act: %s\n\r", act_bit_name(victim->act) );
	send_to_char( buf, ch );

	if ( victim->comm )
	{
		sprintf( buf, "Comm: %s\n\r", comm_bit_name(victim->comm) );
		send_to_char( buf, ch );
	}

	if ( IS_MOB(victim) && victim->off_flags )
	{
		sprintf( buf, "Offense: %s\n\r", off_bit_name(victim->off_flags) );
		send_to_char( buf, ch );
	}

	if ( victim->imm_flags )
	{
		sprintf( buf, "Immune: %s\n\r", imm_bit_name(victim->imm_flags) );
		send_to_char( buf, ch );
	}

	if ( victim->res_flags )
	{
		sprintf( buf, "Resist: %s\n\r", imm_bit_name(victim->res_flags) );
		send_to_char( buf, ch );
	}

	if ( victim->vuln_flags )
	{
		sprintf( buf, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags) );
		send_to_char( buf, ch );
	}

	if ( victim->detection )
	{
		sprintf( buf, "Detection: %s\n\r", (victim->detection) ? detect_bit_name(victim->detection) : "(none)" );
		send_to_char( buf, ch );
	}

	sprintf( buf, "Form: %s\n\rParts: %s\n\r", form_bit_name(victim->form), part_bit_name(victim->parts) );
	send_to_char( buf, ch );

	if ( victim->affected_by )
	{
		sprintf( buf, "Affected by %s\n\r", affect_bit_name(victim->affected_by) );
		send_to_char( buf, ch );
	}

	sprintf( buf, "Master: %s  Leader: %s  Pet: %s\n\r",
		victim->master ? victim->master->name : "(none)",
		victim->leader ? victim->leader->name : "(none)",
		victim->pet ? victim->pet->name : "(none)" );
	send_to_char( buf, ch );

	sprintf( buf, "Short description: %s\n\rLong  description: %s",
		victim->short_descr,
		victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r" );
	send_to_char( buf, ch );

	if ( IS_MOB(victim) && victim->spec_fun != 0 )
	{
		sprintf( buf, "Mobile has special procedure %s.\n\r", spec_name(victim->spec_fun) );
		send_to_char( buf, ch );
	}

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
	{
		sprintf( buf, "Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",
			skill_table[(int) paf->type].name,
			affect_loc_name(paf->location),
			paf->modifier,
			paf->duration,
			affect_bit_name(paf->bitvector),
			paf->level );
		send_to_char( buf, ch );
	}

	if ( !(IS_MOB(victim)) )
	{
		if ( IS_SET(victim->act, PLR_QUESTOR) )
		{
			sprintf( buf, "Questgiver: %d QuestPnts: %d  Questnext: %d\n\r",
				victim->pcdata->questgiver,
				victim->pcdata->questpoints,
				victim->pcdata->nextquest );
			send_to_char( buf, ch );
			sprintf( buf, "QuestCntDown: %d  QuestObj: %d    Questmob: %d\n\r",
				victim->pcdata->countdown,
				victim->pcdata->questobj,
				victim->pcdata->questmob );
			send_to_char( buf, ch );
		}

		if ( !IS_SET(victim->act, PLR_QUESTOR) )
		{
			sprintf( buf, "QuestPnts: %d Questnext: %d    NOT QUESTING\n\r", victim->pcdata->questpoints, victim->pcdata->nextquest );
			send_to_char( buf, ch );
		}
	}

	if ( IS_MOB(victim) )
	{
		if ( victim->pIndexData->progtypes != 0 )
		{
			sprintf( buf, "Mobile progs: " );
			if ( IS_SET(victim->progtypes, MPROG_BRIBE) )		strcat( buf, "bribe " );
			if ( IS_SET(victim->progtypes, MPROG_SPEECH) )		strcat( buf, "speech " );
			if ( IS_SET(victim->progtypes, MPROG_GIVE) )		strcat( buf, "give " );
			if ( IS_SET(victim->progtypes, MPROG_DEATH) )		strcat( buf, "death " );
			if ( IS_SET(victim->progtypes, MPROG_GREET) )		strcat( buf, "greet " );
			if ( IS_SET(victim->progtypes, MPROG_ENTRY) )		strcat( buf, "entry " );
			if ( IS_SET(victim->progtypes, MPROG_FIGHT) )		strcat( buf, "fight " );
			if ( IS_SET(victim->progtypes, MPROG_AREA) )		strcat( buf, "area " );
			strcat( buf, "\n\r" );
			send_to_char( buf, ch );
		}
	}

	sprintf( buf, "Last fought: %10s  Last fight time: %s",
		victim->last_fought != NULL ? victim->last_fought->name : "none",
		ctime(&(victim->last_fight_time)) );
	send_to_char( buf, ch );
	sprintf( buf, "In_mind: [%s] Hunting: [%s]\n\r",
		victim->in_mind != NULL ? victim->in_mind : "none",
		victim->hunting != NULL ? victim->hunting->name : "none" );
	send_to_char( buf, ch );
}


void do_vnum( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	char	*string;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	string = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  vnum obj <name>\n\r", ch );
		send_to_char( "  vnum mob <name>\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "obj") )
	{
		do_ofind( ch, string );
		return;
	}

	if ( !str_cmp(arg, "mob") || !str_cmp(arg, "char") )
	{
		do_mfind( ch, string );
		return;
	}

	/* do both */
	send_to_char("Mob:\n\r", ch);
	do_mfind( ch, argument );
	send_to_char("\n\rOggetti:\n\r", ch);
	do_ofind( ch, argument );
}


/*
 * Lista dei Maestri
 */
void do_mpract( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	BUFFER		*buffer;
	CHAR_DATA	*victim;
	bool		found;
	int			count = 0;
	bool		pfound;
	int			pidx;

	found = FALSE;
	buffer = new_buf( );
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
		if ( (victim -> in_room != NULL)
		&&   (IS_SET(victim -> act, ACT_PRACTICE))
		&&   (can_see(ch, victim))
		&&   (can_see_room(ch, victim->in_room))
		&&   (IS_MOB(victim)) )
		{
			found = TRUE;
			count++;
			sprintf( buf, "%3d) Mob:   [%5d] %-28s \n\r     Stanza:  [%5d] %s",
				count,
				IS_MOB(victim) ? victim->pIndexData->vnum : 0,
				IS_MOB(victim) ? victim->short_descr : victim->name,
				victim->in_room->vnum,
				victim->in_room->name );

			strcat( buf, "\n\r     Gruppo: (" );
			if ( victim->pIndexData->practicer == 0 )
				strcat( buf, "none)\n\r" );
			else
			{
				pfound = FALSE;

				for ( pidx = 0; pidx <=30; pidx++ )
				{
					if ( (victim->pIndexData->practicer) & (1 << pidx) )
					{
						if ( !pfound )
							pfound = TRUE;
						else
							strcat(buf, ", ");
						strcat( buf, prac_table[pidx].sh_name );
					}
				}
				strcat( buf, ")\n\r" );
			}
			add_buf( buffer, buf );
		}
	}

	if ( !found )
		send_to_char( "Niente.\n\r", ch );
	else
		page_to_char( buf_string(buffer), ch );

	free_buf( buffer );
}


void do_mfind( CHAR_DATA *ch, char *argument )
{
	extern int		top_mob_index;
	char			buf[MSL];
	char			arg[MIL];
	MOB_INDEX_DATA	*pMobIndex;
	int				vnum;
	int				nMatch;
	bool			fAll;
	bool			found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Chi vuoi trovare?\n\r", ch );
		return;
	}


	fAll = FALSE;	/* !str_cmp( arg, "all" ); */
	found = FALSE;
	nMatch = 0;

	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops. Get_mob_index is fast, and I don't feel like threading
	 * another link. Do you? -- Furey
	 */
	for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	{
		if ( (pMobIndex = get_mob_index(vnum)) != NULL )
		{
			nMatch++;
			if ( fAll || is_name(argument, pMobIndex->player_name) )
			{
				found = TRUE;
				sprintf( buf, "[%5d] %s\n\r", pMobIndex->vnum, pMobIndex->short_descr );
				send_to_char( buf, ch );
			}
		}
	}

	if ( !found )
		send_to_char( "Non esiste nessuno con quel nome.\n\r", ch );
}


void do_ofind( CHAR_DATA *ch, char *argument )
{
	extern int		top_obj_index;
	char			buf[MSL];
	char			arg[MIL];
	OBJ_INDEX_DATA	*pObjIndex;
	int				vnum;
	int				nMatch;
	bool			fAll;
	bool			found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Find what?\n\r", ch );
		return;
	}

	fAll = FALSE;	/* !str_cmp( arg, "all" ); */
	found = FALSE;
	nMatch = 0;

	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops. Get_obj_index is fast, and I don't feel like threading
	 * another link. Do you? -- Furey
	 */
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
	{
		if ( (pObjIndex = get_obj_index(vnum)) != NULL )
		{
			nMatch++;
			if ( fAll || is_name(argument, pObjIndex->name) )
			{
				found = TRUE;
				sprintf( buf, "[%5d] %s%s\n\r",
					pObjIndex->vnum,
					pObjIndex->short_descr,
					(IS_OBJ_STAT(pObjIndex, ITEM_GLOW) && CAN_WEAR(pObjIndex, ITEM_WEAR_HEAD)) ? " (Glowing)" : "" );
				send_to_char( buf, ch );
			}
		}
	}

	if ( !found )
		send_to_char( "No objects by that name.\n\r", ch );
}


void do_owhere( CHAR_DATA *ch, char *argument )
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
	max_found = 200;

	buffer = new_buf();

	if ( argument[0] == '\0' )
	{
		send_to_char( "Find what?\n\r", ch );
		return;
	}

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( !can_see_obj(ch, obj) || !is_name(argument, obj->name) || ch->level < obj->level )
			continue;

		found = TRUE;
		number++;

		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
			;

		if ( in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by) && in_obj->carried_by->in_room != NULL )
		{
			sprintf( buf, "%3d) %s is carried by %s [Room %d]\n\r",
				number,
				obj->short_descr,
				PERS(in_obj->carried_by, ch),
				in_obj->carried_by->in_room->vnum );
		}
		else if ( in_obj->in_room != NULL && can_see_room(ch, in_obj->in_room) )
			sprintf( buf, "%3d) %s is in %s [Room %d]\n\r", number, obj->short_descr, in_obj->in_room->name, in_obj->in_room->vnum );
		else
			sprintf( buf, "%3d) %s is somewhere\n\r", number, obj->short_descr );


		buf[0] = UPPER( buf[0] );
		add_buf( buffer, buf );

		if ( number >= max_found )
			break;
	}

	if ( !found )
		send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	else
		page_to_char( buf_string(buffer), ch );

	free_buf( buffer );
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	BUFFER		*buffer;
	CHAR_DATA	*victim;
	bool		found;
	int			count = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		DESCRIPTOR_DATA *d;
		/*~~~~~~~~~~~~~~~*/

		/* show characters logged */
		buffer = new_buf();
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if (d->character != NULL
			&&	d->connected == CON_PLAYING
			&&	d->character->in_room != NULL
			&&	can_see(ch, d->character)
			&&	can_see_room(ch, d->character->in_room) )
			{
				victim = d->character;
				count++;
				if ( d->original != NULL )
				{
					sprintf( buf, "%3d) %s (in the body of %s) is in %s [%d]\n\r",
						count,
						d->original->name,
						victim->short_descr,
						victim->in_room->name,
						victim->in_room->vnum );
				}
				else
				{
					sprintf( buf, "%3d) %s is in %s [%d]\n\r",
						count,
						victim->name,
						victim->in_room->name,
						victim->in_room->vnum );
				}

				add_buf( buffer, buf );
			}
		}

		page_to_char( buf_string(buffer), ch );
		free_buf( buffer );
		return;
	}

	found = FALSE;
	buffer = new_buf();
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
		if ( victim->in_room != NULL && is_name(argument, victim->name) )
		{
			found = TRUE;
			count++;
			sprintf( buf, "%3d) [%5d] %-28s [%5d] %s\n\r",
				count,
				IS_MOB(victim) ? victim->pIndexData->vnum : 0,
				IS_MOB(victim) ? victim->short_descr : victim->name,
				victim->in_room->vnum,
				victim->in_room->name );
			add_buf( buffer, buf );
		}
	}

	if ( !found )
		act( "You didn't find any $T.", ch, NULL, argument, TO_CHAR, POS_REST );
	else
		page_to_char( buf_string(buffer), ch );

	free_buf( buffer );
}


void do_reboo( CHAR_DATA *ch, char *argument )
{
	send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
}


void do_shutdow( CHAR_DATA *ch, char *argument )
{
	send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
}


void do_shutdown( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->invis_level < LEVEL_HERO )
		sprintf( buf, "Shutdown by %s.", ch->name );

	strcat( buf, "\n\r" );

	if ( ch->invis_level < LEVEL_HERO )
		do_echo( ch, buf );

	reboot_xandra( FALSE, XANDRA_SHUTDOWN );
}


void do_protect( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Protect whom from snooping?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, argument)) == NULL )
	{
		send_to_char( "You can't find them.\n\r", ch );
		return;
	}

	if ( IS_SET(victim->comm, COMM_SNOOP_PROOF) )
	{
		act( "$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
				REMOVE_BIT( victim->comm, COMM_SNOOP_PROOF );
	}
	else
	{
		act( "$N is now snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
		SET_BIT( victim->comm, COMM_SNOOP_PROOF );
	}
}


void do_snoop( CHAR_DATA *ch, char *argument )
{
	char			arg[MIL];
	DESCRIPTOR_DATA *d;
	CHAR_DATA		*victim;
	char			buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Snoop whom?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL )
	{
		send_to_char( "No descriptor to snoop.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Cancelling all snoops.\n\r", ch );

		wiznet( "$N stops being such a snoop.", ch, NULL, WIZ_SNOOPS, get_trust(ch) );
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->snoop_by == ch->desc )
				d->snoop_by = NULL;
		}

		return;
	}

	if ( victim->desc->snoop_by != NULL )
	{
		send_to_char( "Busy already.\n\r", ch );
		return;
	}

	if(!is_room_owner(ch, victim->in_room)
	&&	ch->in_room != victim->in_room
	&&	room_is_private(victim->in_room)
	&&	!IS_TRUSTED(ch, LEVEL_IMPLEMENTOR) )
	{
		send_to_char( "That character is in a private room.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) || IS_SET(victim->comm, COMM_SNOOP_PROOF) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( ch->desc != NULL )
	{
		for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
		{
			if ( d->character == victim || d->original == victim )
			{
				send_to_char( "No snoop loops.\n\r", ch );
				return;
			}
		}
	}

	victim->desc->snoop_by = ch->desc;

	sprintf( buf, "$N starts snooping on %s", (IS_MOB(ch) ? victim->short_descr : victim->name) );
	wiznet( buf, ch, NULL, WIZ_SNOOPS, get_trust(ch) );
	send_to_char( "Ok.\n\r", ch );
}


void do_switch( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL], buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Quale corpo vuoi possedere?\n\r", ch );
		return;
	}


	if ( ch->desc == NULL )
		return;

	if ( ch->desc->original != NULL )
	{
		send_to_char( "Sei gia' all'interno di un corpo.\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "Non e' qui.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Ok.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) )
	{
		send_to_char( "Non puoi farlo su avventurieri.\n\r", ch );
		return;
	}

	if(!is_room_owner(ch, victim->in_room)
	&&	ch->in_room != victim->in_room
	&&	room_is_private(victim->in_room)
	&&	!IS_TRUSTED(ch, LEVEL_IMPLEMENTOR))
	{
		send_to_char( "That character is in a private room.\n\r", ch );
		return;
	}

	if ( victim->desc != NULL )
	{
		send_to_char( "Character in use.\n\r", ch );
		return;
	}

	sprintf( buf, "$N switches into %s", victim->short_descr );
	wiznet( buf, ch, NULL, WIZ_SWITCHES, get_trust(ch) );

	ch->desc->character = victim;
	ch->desc->original = ch;
	victim->desc = ch->desc;
	ch->desc = NULL;

	/* change communications to match */
	if ( ch->prompt != NULL )
		victim->prompt = str_dup( ch->prompt );
	victim->comm = ch->comm;
	victim->lines = ch->lines;
	send_to_char( "Ok.\n\r", victim );
}


void do_return( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->desc == NULL )
		return;

	if ( ch->desc->original == NULL )
	{
		send_to_char( "You aren't switched.\n\r", ch );
		return;
	}

	send_to_char( "You return to your original body. Type replay to see any missed tells.\n\r", ch );
	if ( ch->prompt != NULL )
	{
		free_string( ch->prompt );
		ch->prompt = NULL;
	}

	sprintf( buf, "$N returns from %s.", ch->short_descr );
	wiznet( buf, ch->desc->original, 0, WIZ_SWITCHES, get_trust(ch) );
	ch->desc->character = ch->desc->original;
	ch->desc->original = NULL;
	ch->desc->character->desc = ch->desc;
	ch->desc = NULL;
}


/*
 * trust levels for load and clone
 */
bool obj_check( CHAR_DATA *ch, OBJ_DATA *obj )
{
	if ( IS_TRUSTED(ch, LEVEL_CREATOR) || (IS_TRUSTED(ch, LEVEL_MASTER) && obj->level <= 30 && obj->cost <= 1000) )
		return( TRUE );
	else
		return( FALSE );
}


/*
 * for clone, to insure that cloning goes many levels deep
 */
void recursive_clone( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone )
{
	OBJ_DATA	*c_obj, *t_obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content )
	{
		if ( obj_check(ch, c_obj) )
		{
			t_obj = create_object( c_obj->pIndexData, 0 );
			clone_object( c_obj, t_obj );
			obj_to_obj( t_obj, clone );
			recursive_clone( ch, c_obj, t_obj );
		}
	}
}


/*
 * command that is similar to load
 */
void do_clone( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		*rest;
	CHAR_DATA	*mob;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	rest = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Clone what?\n\r", ch );
		return;
	}

	if ( !str_prefix(arg, "object") )
	{
		mob = NULL;
		obj = get_obj_here( ch, rest );
		if ( obj == NULL )
		{
			send_to_char( "You don't see that here.\n\r", ch );
			return;
		}
	}
	else if ( !str_prefix(arg, "mobile") || !str_prefix(arg, "character") )
	{
		obj = NULL;
		mob = get_char_room( ch, rest );
		if ( mob == NULL )
		{
			send_to_char( "You don't see that here.\n\r", ch );
			return;
		}
	}
	else	/* find both */
	{
		mob = get_char_room( ch, argument );
		obj = get_obj_here( ch, argument );
		if ( mob == NULL && obj == NULL )
		{
			send_to_char( "You don't see that here.\n\r", ch );
			return;
		}
	}

	/* clone an object */
	if ( obj != NULL )
	{
		OBJ_DATA	*clone;
		/*~~~~~~~~~~~~~~~*/

		if ( !obj_check(ch, obj) )
		{
			send_to_char( "Your powers are not great enough for such a task.\n\r", ch );
			return;
		}

		clone = create_object( obj->pIndexData, 0 );
		clone_object( obj, clone );
		if ( obj->carried_by != NULL )
			obj_to_char( clone, ch );
		else
			obj_to_room( clone, ch->in_room );
		recursive_clone( ch, obj, clone );

		act( "Riproduci fedelmente $p.", ch, clone, NULL, TO_CHAR, POS_REST );
		wiznet( "$N clones $p.", ch, clone, WIZ_LOAD, get_trust(ch) );
		return;
	}
	else if ( mob != NULL )
	{
		CHAR_DATA	*clone;
		OBJ_DATA	*new_object;
		char		buf[MSL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


		if ( !IS_MOB(mob) )
		{
			send_to_char( "You can only clone mobiles.\n\r", ch );
			return;
		}

		if ( mob->level > 20 && !IS_TRUSTED(ch, LEVEL_MASTER) )
		{
			send_to_char( "Your powers are not great enough for such a task.\n\r", ch );
			return;
		}

		clone = create_mobile( mob->pIndexData );
		clone_mobile( mob, clone );

		for ( obj = mob->carrying; obj != NULL; obj = obj->next_content )
		{
			if ( obj_check(ch, obj) )
			{
				new_object = create_object( obj->pIndexData, 0 );
				clone_object( obj, new_object );
				recursive_clone( ch, obj, new_object );
				obj_to_char( new_object, clone );
				new_object->wear_loc = obj->wear_loc;
			}
		}

		char_to_room( clone, ch->in_room );
		act( "Riproduci $N.", ch, NULL, clone, TO_CHAR, POS_REST );
		sprintf( buf, "$N clones %s.", clone->short_descr );
		wiznet( buf, ch, NULL, WIZ_LOAD, get_trust(ch) );
		return;
	}
}


/*
 * RT to replace the two load commands
 */
void do_load( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  load mob <vnum>\n\r", ch );
		send_to_char( "  load obj <vnum> <level>\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "mob") || !str_cmp(arg, "char") )
	{
		do_mload( ch, argument );
		return;
	}

	if ( !str_cmp(arg, "obj") )
	{
		do_oload( ch, argument );
		return;
	}

	/* echo syntax */
	do_load( ch, "" );
}


void do_mload( CHAR_DATA *ch, char *argument )
{
	char			arg[MIL];
	MOB_INDEX_DATA	*pMobIndex;
	CHAR_DATA		*victim;
	char			buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' || !is_number(arg) )
	{
		send_to_char( "Syntax: load mob <vnum>.\n\r", ch );
		return;
	}

	if ( (pMobIndex = get_mob_index(atoi(arg))) == NULL )
	{
		send_to_char( "No mob has that vnum.\n\r", ch );
		return;
	}

	victim = create_mobile( pMobIndex );
	char_to_room( victim, ch->in_room );
	act( "$n evoca dal nulla $N!", ch, NULL, victim, TO_ROOM, POS_REST );
	sprintf( buf, "$N loads %s.", victim->short_descr );
	wiznet( buf, ch, NULL, WIZ_LOAD, get_trust(ch) );
	send_to_char( "Ok.\n\r", ch );
}


void do_oload( CHAR_DATA *ch, char *argument )
{
	char			arg1[MIL], arg2[MIL];
	OBJ_INDEX_DATA	*pObjIndex;
	OBJ_DATA		*obj;
	int				level;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || !is_number(arg1) )
	{
		send_to_char( "Syntax: load obj <vnum> <level>.\n\r", ch );
		return;
	}

	level = get_trust( ch );	/* default */

	if ( arg2[0] != '\0' )		/* load with a level */
	{
		if ( !is_number(arg2) )
		{
			send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
			return;
		}

		level = atoi( arg2 );
		if ( level < 0 || level > get_trust(ch) )
		{
			send_to_char( "Level must be be between 0 and your level.\n\r", ch );
			return;
		}
	}

	if ( (pObjIndex = get_obj_index(atoi(arg1))) == NULL )
	{
		send_to_char( "No object has that vnum.\n\r", ch );
		return;
	}

	obj = create_object( pObjIndex, level );
	if ( CAN_WEAR(obj, ITEM_TAKE) )
		obj_to_char( obj, ch );
	else
		obj_to_room( obj, ch->in_room );
	act( "$n crea dal nulla $p!", ch, obj, NULL, TO_ROOM, POS_REST );
	wiznet( "$N loads $p.", ch, obj, WIZ_LOAD, get_trust(ch) );
	send_to_char( "Ok.\n\r", ch );
}


void do_purge( CHAR_DATA *ch, char *argument )
{
	char			arg[MIL];
	char			buf[100];
	CHAR_DATA		*victim;
	OBJ_DATA		*obj;
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		/* 'purge' */
		CHAR_DATA	*vnext;
		OBJ_DATA	*obj_next;
		/*~~~~~~~~~~~~~~~~~~*/

		for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
		{
			vnext = victim->next_in_room;
			if ( IS_MOB(victim) && !IS_SET(victim->act, ACT_NOPURGE) && victim != ch /* safety precaution */ )
				extract_char( victim, TRUE );
		}

		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( !IS_OBJ_STAT(obj, ITEM_NOPURGE) )
				extract_obj( obj );
		}

		act( "$n batte le palpebre e un bagliore improvviso invade la stanza!", ch, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Ok.\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) )
	{
		if ( ch == victim )
		{
			send_to_char( "Ho ho ho.\n\r", ch );
			return;
		}

		if ( get_trust(ch) <= get_trust(victim) )
		{
			send_to_char( "Maybe that wasn't a good idea..\n\r", ch );
			sprintf( buf, "%s tried to purge you!\n\r", ch->name );
			send_to_char( buf, victim );
			return;
		}

		act( "$n disintegra $N.", ch, 0, victim, TO_NOTVICT, POS_REST );


		if ( victim->level > 1 )
			save_char_obj( victim );
		d = victim->desc;
		extract_char( victim, TRUE );
		if ( d != NULL )
			close_socket( d );

		return;
	}

	act( "$n disintegra $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
	extract_char( victim, TRUE );
}


void do_trust( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	int			level;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
	{
		send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char( "That player is not here.\n\r", ch );
		return;
	}

	if ( (level = atoi(arg2)) < 0 || level > 100 )
	{
		send_to_char( "Level must be 0 (reset) or 1 to 100.\n\r", ch );
		return;
	}

	if ( level > get_trust(ch) )
	{
		send_to_char( "Limited to your trust.\n\r", ch );
		return;
	}

	victim->trust = level;
}


void restore_set( CHAR_DATA *vch )
{
	affect_strip( vch, gsn_blindness );
	affect_strip( vch, gsn_curse );
	affect_strip( vch,gsn_fire_breath );
	affect_strip( vch, gsn_plague );
	affect_strip( vch, gsn_poison );
	affect_strip( vch,gsn_sand_storm );
	affect_strip( vch, gsn_sleep );
	affect_strip( vch,gsn_weaken );

	if ( !IS_MOB(vch) )
	{
		vch->pcdata->condition[COND_DRUNK]		= 0;
		vch->pcdata->condition[COND_FULL]		= 100;
		vch->pcdata->condition[COND_THIRST]		= 100;
		vch->pcdata->condition[COND_HUNGER]		= 100;
		vch->pcdata->condition[COND_BLOODLUST]	= 100;
		vch->pcdata->condition[COND_DESIRE]		= 100;
	}

	vch->point[LIFE] = vch->max_point[LIFE];
	vch->point[MANA] = vch->max_point[MANA];
	vch->point[MOVE] = vch->max_point[MOVE];

	update_pos( vch );
}


void do_restore( CHAR_DATA *ch, char *argument )
{
	char			arg[MIL],
					buf[MSL];
	CHAR_DATA		*victim;
	CHAR_DATA		*vch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' || !str_cmp(arg, "room") )
	{
		/* cure room */
		for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
		{
			restore_set( vch );
			act( "$n ti ha ristorat$X.", ch, NULL, vch, TO_VICT, POS_REST );
		}

		sprintf( buf, "$N restored room %d.", ch->in_room->vnum );
		wiznet( buf, ch, NULL, WIZ_RESTORE, get_trust(ch) );


		send_to_char( "Room restored.\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	restore_set( victim );
	act( "$n ti ha ristorat$X.", ch, NULL, victim, TO_VICT, POS_REST );
	sprintf( buf, "$N restored %s", IS_MOB(victim) ? victim->short_descr : victim->name );
	wiznet( buf, ch, NULL, WIZ_RESTORE, get_trust(ch) );
	send_to_char( "Ok.\n\r", ch );
}


/*
 *
 */
void do_forcerent (CHAR_DATA *ch, char *argument)
{

	CHAR_DATA		*vch;
	char			buffer[MSL];

	if ( argument[0] == '\0' )
	{
		send_to_char( "A chi vuoi chiudere il collegamento?\r\n", ch );
		return;
	}

	argument[0] = UPPER( argument[0] );
	if ( !strcmp(ch->name, argument) )
	{
		send_to_char( "Non mi pare il caso.\r\n", ch );
		return;
	}

	for ( vch = char_list;  vch != NULL;  vch = vch->next )
	{
		if ( !strcmp(vch->name, argument) )
		{
			sprintf( buffer,"Chiude il collegamento a %s.\r\n", vch->name );
			send_to_char( buffer, ch );

			if ( vch->desc != NULL )
				close_socket( vch->desc );

			save_char_obj( vch );
			extract_char( vch, TRUE );

			return;
		}
	}
	send_to_char( "Non esiste nessuno che si chiami cosi'.\r\n", ch );
}


void do_freeze( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL], buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Freeze whom?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET(victim->act, PLR_FREEZE) )
	{
		REMOVE_BIT( victim->act, PLR_FREEZE );
		send_to_char( "Le placche di ghiaccio che ti imprigionavano si sciolgono.\n\r", victim );
		send_to_char( "FREEZE removed.\n\r", ch );
		sprintf( buf, "$N thaws %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
	else
	{
		SET_BIT( victim->act, PLR_FREEZE );
		send_to_char( "Il tuo corpo viene bloccato da imponenti lastre ti ghiaccio!\n\r", victim );
		send_to_char( "FREEZE set.\n\r", ch );
		sprintf( buf, "$N puts %s in the deep freeze.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}

	save_char_obj( victim );
}


void do_log( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Log whom?\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "all") )
	{
		if ( fLogAll )
		{
			fLogAll = FALSE;
			send_to_char( "Log ALL off.\n\r", ch );
		}
		else
		{
			fLogAll = TRUE;
			send_to_char( "Log ALL on.\n\r", ch );
		}

		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	/* No level check, gods can log anyone. */
	if ( IS_SET(victim->act, PLR_LOG) )
	{
		REMOVE_BIT( victim->act, PLR_LOG );
		send_to_char( "LOG removed.\n\r", ch );
	}
	else
	{
		SET_BIT( victim->act, PLR_LOG );
		send_to_char( "LOG set.\n\r", ch );
	}
}


void do_noemote( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL], buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Noemote whom?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET(victim->comm, COMM_NOEMOTE) )
	{
		REMOVE_BIT( victim->comm, COMM_NOEMOTE );
		send_to_char( "I tuoi muscoli tornano in condizione di esprimere emozioni.\n\r", victim );
		send_to_char( "NOEMOTE removed.\n\r", ch );
		sprintf( buf, "$N restores emotes to %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOEMOTE );
		send_to_char( "Senti i tuoi muscoli facciali irrigidirsi!\n\r", victim );
		send_to_char( "NOEMOTE set.\n\r", ch );
		sprintf( buf, "$N revokes %s's emotes.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
}


void do_noyell( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL], buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Noyell whom?\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET(victim->comm, COMM_NOYELL) )
	{
		REMOVE_BIT( victim->comm, COMM_NOYELL );
		send_to_char( "La morsa nera che ti stringeva il collo finalmente svanisce.\n\r", victim );
		send_to_char( "NOYELL removed.\n\r", ch );

		sprintf( buf, "$N restores yells to %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOYELL );
		send_to_char( "Una morsa nera ti avvinghia il collo, non riesci piu' ad urlare!\n\r", victim );
		send_to_char( "NOYELL set.\n\r", ch );
		sprintf( buf, "$N revokes %s's yells.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
}


void do_notell( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL], buf[MSL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Notell whom?", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( get_trust(victim) >= get_trust(ch) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET(victim->comm, COMM_NOTELL) )
	{
		REMOVE_BIT( victim->comm, COMM_NOTELL );
		send_to_char( "Ora puoi tornare a bisbigliare.\n\r", victim );
		send_to_char( "NOTELL removed.\n\r", ch );
		sprintf( buf, "$N restores tells to %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOTELL );
		send_to_char( "Improvvisamente ti accorgi di non riuscire a parlare in segreto a nessuno!\n\r", victim );
		send_to_char( "NOTELL set.\n\r", ch );
		sprintf( buf, "$N revokes %s's tells.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, 0 );
	}
}


void do_peace( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*rch;
	/*~~~~~~~~~~~~~*/

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch->fighting != NULL )
			stop_fighting( rch, TRUE );
		if ( IS_MOB(rch) && IS_SET(rch->act, ACT_AGGRESSIVE) )
			REMOVE_BIT( rch->act, ACT_AGGRESSIVE );
	}

	send_to_char( "Ok.\n\r", ch );
}


void do_wizlock( CHAR_DATA *ch, char *argument )
{
	extern bool wizlock;
	/*~~~~~~~~~~~~~~~~*/

	wizlock = !wizlock;

	if ( wizlock )
	{
		wiznet( "$N has wizlocked the game.", ch, NULL, 0, 0 );
		send_to_char( "Game wizlocked.\n\r", ch );
	}
	else
	{
		wiznet( "$N removes wizlock.", ch, NULL, 0, 0 );
		send_to_char( "Game un-wizlocked.\n\r", ch );
	}
}


/*
 * RT anti-newbie code
 */
void do_newlock( CHAR_DATA *ch, char *argument )
{
	extern bool newlock;
	/*~~~~~~~~~~~~~~~~*/

	newlock = !newlock;

	if ( newlock )
	{
		wiznet( "$N locks out new characters.", ch, NULL, 0, 0 );
		send_to_char( "New characters have been locked out.\n\r", ch );
	}
	else
	{
		wiznet( "$N allows new characters back in.", ch, NULL, 0, 0 );
		send_to_char( "Newlock removed.\n\r", ch );
	}
}


void do_slookup( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	char	arg[MIL];
	int		sn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Quale incantesimo o abilita'?\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "all") )
	{
		for ( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if ( skill_table[sn].name == NULL )
				break;
			sprintf( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r", sn, skill_table[sn].slot, skill_table[sn].name );
			send_to_char( buf, ch );
		}
	}
	else
	{
		if ( (sn = skill_lookup(arg)) < 0 )
		{
			send_to_char( "Nessun incantesimo o abilita' con quel nome.\n\r", ch );
			return;
		}

		sprintf( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r", sn, skill_table[sn].slot, skill_table[sn].name );
		send_to_char( buf, ch );
	}
}


void do_tvalue( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	char		arg1[MIL],
				arg2[MIL],
				buf[MSL];
	int			quantity;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	if ( arg1[0] == '\0' || arg2 == '\0' )
	{
		send_to_char( "Sintassi: tvalue <avventuriero> argento <quantita'>\n\r", ch );
		send_to_char( "Sintassi: tvalue <avventuriero> oro <quantita'>\n\r", ch );
		send_to_char( "Sintassi: tvalue <avventuriero> puntiquest <quantita'>\n\r\n\r", ch );

		send_to_char( "Per togliere una certa' quantità inserire un numero negativo\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char( "Non c'e' nessuno con quel nome.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Quanto vorresti aggiungere o togliere?\n\r", ch );
		return;
	}

	quantity = atoi( argument );
	if ( quantity < -10000 || quantity > 10000 )
	{
		send_to_char( "Hei! Vacci piano.. Un po' tantini.\n\r", ch );
		return;
	}

	if		( !str_prefix(arg2, "argento") || !str_prefix(arg2, "silver") )
	{
		sprintf( buf, "%d d'argento a %s.\n\r", quantity, victim->name );
		send_to_char( buf, ch );

		victim->silver += quantity;
		if ( victim->silver < 0 )
			victim->silver = 0;
	}
	else if ( !str_prefix(arg2, "oro") || !str_prefix(arg2, "gold") )
	{
		sprintf( buf, "%d d'oro a %s.\n\r", quantity, victim->name );
		send_to_char( buf, ch );

		victim->gold += quantity;
		if ( victim->gold < 0 )
			victim->gold = 0;
	}
	else if ( !str_prefix(arg2, "puntiquest") || !str_prefix(arg2, "questpoints") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Non puoi darli a qualcuno che non e' un avventuriero!\n\r", ch );
			return;
		}

		sprintf( buf, "%d punti quest a %s.\n\r", quantity, victim->name );
		send_to_char( buf, ch );

		victim->pcdata->questpoints += quantity;
		if ( victim->pcdata->questpoints < 0 )
			victim->pcdata->questpoints = 0;
	}
	else
		send_to_char( "Tipo di danaro o punti da passare al pg non corretto <argento|oro|puntiquest>\n\r", ch );
}


/*
 * RT set replaces sset, mset, oset, and rset
 */
void do_set( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set mob   <name> <field> <value>\n\r", ch );
		send_to_char( "  set obj   <name> <field> <value>\n\r", ch );
		send_to_char( "  set room  <room> <field> <value>\n\r", ch );
		send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch );
#ifdef WEATHER
		send_to_char( "  set weather <value>\n\r", ch );
#endif
		return;
	}

	if		( !str_prefix(arg, "mobile")  || !str_prefix(arg, "pg") )		do_mset( ch, argument );
	else if ( !str_prefix(arg, "skill")	  || !str_prefix(arg, "spell") )	do_sset( ch, argument );
#ifdef WEATHER
    else if ( !str_prefix(arg, "weather") || !str_prefix(arg, "tempo") )	do_wset( ch, argument );
#endif
	else if ( !str_prefix(arg, "object")  || !str_prefix(arg, "oggetti") )	do_oset( ch, argument );
	else if ( !str_prefix(arg, "room")	  || !str_prefix(arg, "stanza") )	do_rset( ch, argument );
	else																	do_set( ch, "" );	/* echo syntax */
}


void do_sset( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	char		arg3[MIL];
	CHAR_DATA	*victim;
	int			value;
	int			sn;
	bool		fAll;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch );
		send_to_char( "  set skill <pg name> all <value>\n\r", ch );
		send_to_char( "   (use the name of the skill, not the number)\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	fAll = !str_cmp( arg2, "all" );
	sn = 0;
	if ( !fAll && (sn = skill_lookup(arg2)) < 0 )
	{
		send_to_char( "No such skill or spell.\n\r", ch );
		return;
	}

	/* Snarf the value. */
	if ( !is_number(arg3) )
	{
		send_to_char( "Value must be numeric.\n\r", ch );
		return;
	}

	value = atoi( arg3 );
	if ( value < 0 || value > 100 )
	{
		send_to_char( "Value range is 0 to 100.\n\r", ch );
		return;
	}

	if ( fAll )
	{
		for ( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if((skill_table[sn].name != NULL)
			&&	((victim->cabal == skill_table[sn].cabal) || (skill_table[sn].cabal == CABAL_NONE))
			&&	(RACE_OK(victim, sn)) )
				victim->pcdata->learned[sn] = value;
		}
	}
	else
		victim->pcdata->learned[sn] = value;

	send_to_char( "Ok.\n\r", ch );
}


void do_string( CHAR_DATA *ch, char *argument )
{
	char		type[MIL];
	char		arg1[MIL];
	char		arg2[MIL];
	char		arg3[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	smash_tilde( argument );
	argument = one_argument( argument, type );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

	if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  string char <name> <field> <string>\n\r", ch );
		send_to_char( "    fields: name short long desc spec\n\r", ch );
		send_to_char( "  string obj  <name> <field> <string>\n\r", ch );
		send_to_char( "    fields: name short long extended\n\r", ch );
		return;
	}

	if ( !str_prefix(type, "character") || !str_prefix(type, "mobile") )
	{
		if ( (victim = get_char_world(ch, arg1)) == NULL )
		{
			send_to_char( "They aren't here.\n\r", ch );
			return;
		}

		/* clear zone for mobs */
		victim->zone = NULL;

		/* string something */
		if ( !str_prefix(arg2, "name") )
		{
			if ( !IS_MOB(victim) )
			{
				send_to_char( "Not on PC's.\n\r", ch );
				return;
			}

			free_string( victim->name );
			victim->name = str_dup( arg3 );
		}
		else if ( !str_prefix(arg2, "description") )
		{
			free_string( victim->description );
			victim->description = str_dup( arg3 );
		}
		else if ( !str_prefix(arg2, "short") )
		{
			free_string( victim->short_descr );
			victim->short_descr = str_dup( arg3 );
		}
		else if ( !str_prefix(arg2, "long") )
		{
			free_string( victim->long_descr );
			strcat( arg3, "\n\r" );
			victim->long_descr = str_dup( arg3 );
		}
		else if ( !str_prefix(arg2, "spec") )
		{
			if ( !IS_MOB(victim) )
			{
				send_to_char( "Not on PC's.\n\r", ch );
				return;
			}

			if ( (victim->spec_fun = spec_lookup(arg3)) == 0 )
			{
				send_to_char( "No such spec fun.\n\r", ch );
				return;
			}
		}

		send_to_char( "Ok.", ch );
	}
	else if ( !str_prefix(type, "object") )
	{
		/* string an obj */
		if ( (obj = get_obj_world(ch, arg1)) == NULL )
		{
			send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
			return;
		}

		if ( !str_prefix(arg2, "name") )
		{
			free_string( obj->name );
			obj->name = str_dup( arg3 );
		}
		else if ( !str_prefix(arg2, "short") )
		{
			free_string( obj->short_descr );
			obj->short_descr = str_dup( arg3 );	/* (bb) nel tempio non si riesce a restringare la short O_o */
		}
		else if ( !str_prefix(arg2, "long") )
		{
			free_string( obj->description );
			obj->description = str_dup( arg3 );
		}
		else if ( !str_prefix(arg2, "ed") || !str_prefix(arg2, "extended") )
		{
			EXTRA_DESCR_DATA	*ed;
			/*~~~~~~~~~~~~~~~~~~~~*/

			argument = one_argument( argument, arg3 );
			if ( argument == NULL )
			{
				send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r", ch );
				return;
			}

			strcat( argument, "\n\r" );

			ed = new_extra_descr();

			ed->keyword = str_dup( arg3 );
			ed->description = str_dup( argument );
			ed->next = obj->extra_descr;
			obj->extra_descr = ed;
		}

		send_to_char( "Ok.", ch );
	}
	else
		do_string( ch, "" );	/* echo bad use message */
}


void do_oset( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	char		arg3[MIL];
	OBJ_DATA	*obj;
	int			value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set obj <object> <field> <value>\n\r", ch );
		send_to_char( "  Field being one of:\n\r", ch );
		send_to_char( "    value0 value1 value2 value3 value4 (v1-v4)\n\r", ch );
		send_to_char( "    extra wear level weight cost timer\n\r", ch );
		return;
	}

	if ( (obj = get_obj_world(ch, arg1)) == NULL )
	{
		send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
		return;
	}

	/* Snarf the value (which need not be numeric). */
	value = atoi( arg3 );

	/* Set something. */
	if ( !str_cmp(arg2, "value0") || !str_cmp(arg2, "v0") )
		obj->value[0] = UMIN( 50, value );
	else if ( !str_cmp(arg2, "value1") || !str_cmp(arg2, "v1") )
		obj->value[1] = value;
	else if ( !str_cmp(arg2, "value2") || !str_cmp(arg2, "v2") )
		obj->value[2] = value;
	else if ( !str_cmp(arg2, "value3") || !str_cmp(arg2, "v3") )
		obj->value[3] = value;
	else if ( !str_cmp(arg2, "value4") || !str_cmp(arg2, "v4") )
		obj->value[4] = value;
	else if ( !str_prefix(arg2, "extra") )
		obj->extra_flags = value;
	else if ( !str_prefix(arg2, "wear") )
		obj->wear_flags = value;
	else if ( !str_prefix(arg2, "level") )
		obj->level = value;
	else if ( !str_prefix(arg2, "weight") )
		obj->weight = value;
	else if ( !str_prefix(arg2, "cost") )
		obj->cost = value;
	else if ( !str_prefix(arg2, "timer") )
		obj->timer = value;
	else
		do_oset( ch, "" );	/* Generate usage message. */

	send_to_char( "Ok.", ch );
}


void do_rset( CHAR_DATA *ch, char *argument )
{
	char			arg1[MIL];
	char			arg2[MIL];
	char			arg3[MIL];
	ROOM_INDEX_DATA *location;
	int				value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set room <location> <field> <value>\n\r", ch );
		send_to_char( "  Field being one of:\n\r", ch );
		send_to_char( "    flags sector\n\r", ch );
		return;
	}

	if ( (location = find_location(ch, arg1)) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	/* if (!is_room_owner(ch,location) && ch->in_room != location */
	if ( ch->in_room != location && room_is_private(location) && !IS_TRUSTED(ch, LEVEL_IMPLEMENTOR) )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	/* Snarf the value. */
	if ( !is_number(arg3) )
	{
		send_to_char( "Value must be numeric.\n\r", ch );
		return;
	}

	value = atoi( arg3 );

	/* Set something. */
	if ( !str_prefix(arg2, "flags") )
		location->room_flags = value;
	else if ( !str_prefix(arg2, "sector") )
		location->sector_type = value;
	else
		do_rset( ch, "" );	/* Generate usage message. */

	send_to_char( "Ok.", ch );
}


void do_sockets( CHAR_DATA *ch, char *argument )
{
	char			buf[2 * MSL];
	char			buf2[MSL];
	char			arg[MIL];
	DESCRIPTOR_DATA *d;
	int				count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	count = 0;
	buf[0] = '\0';

	one_argument( argument, arg );
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if( d->character != NULL
		&&	can_see(ch, d->character)
		&&	(arg[0] == '\0' || is_name(arg, d->character->name) || (d->original && is_name(arg, d->original->name))) )
		{
			count++;
			sprintf( buf + strlen(buf), "[%3d %2d] %s@%s\n\r",
				d->descriptor,
				d->connected,
				d->original ? d->original->name : d->character ? d->character->name : "(none)",
				d->host );
		}
	}

	if ( count == 0 )
	{
		send_to_char( "No one by that name is connected.\n\r", ch );
		return;
	}

	sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
	strcat( buf, buf2 );
	page_to_char( buf, ch );
}


/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	char	name[MIL];
	char	arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, name );

	if ( name[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "Force whom to do what?\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( !str_cmp(arg, "delete") )
	{
		send_to_char( "That will NOT be done.\n\r", ch );
		return;
	}

	sprintf( buf, "$n ti costringe mentalmente a '%s %s'.", arg, argument );

	if ( !str_cmp(name, "all") )
	{
		CHAR_DATA	*vch;
		CHAR_DATA	*vch_next;
		/*~~~~~~~~~~~~~~~~~~*/

		if ( get_trust(ch) <= LEVEL_MASTER )
		{
			send_to_char( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;

			if ( !IS_MOB(vch) && get_trust(vch) < get_trust(ch) )
			{
				char tcmd[MIL];

				act( buf, ch, NULL, vch, TO_VICT, POS_REST );

				sprintf( tcmd, "%s %s", translate_command(arg, ch, vch), argument );
				interpret( vch, tcmd, TRUE );
			}
		}
	}
	else if ( !str_cmp(name, "players") )
	{
		CHAR_DATA	*vch;
		CHAR_DATA	*vch_next;
		/*~~~~~~~~~~~~~~~~~~*/

		if ( get_trust(ch) < MAX_LEVEL - 2 )
		{
			send_to_char( "HAHAHA!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;

			if ( !IS_MOB(vch) && get_trust(vch) < get_trust(ch) && vch->level < LEVEL_HERO )
			{
				char tcmd[MIL];

				act( buf, ch, NULL, vch, TO_VICT, POS_REST );

				sprintf( tcmd, "%s %s", translate_command(arg, ch, vch), argument );
				interpret( vch, tcmd, FALSE );
			}
		}
	}
	else if ( !str_cmp(name, "gods") )
	{
		CHAR_DATA	*vch;
		CHAR_DATA	*vch_next;
		/*~~~~~~~~~~~~~~~~~~*/

		if ( get_trust(ch) < MAX_LEVEL - 2 )
		{
			send_to_char( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;

			if ( !IS_MOB(vch) && get_trust(vch) < get_trust(ch) && vch->level >= LEVEL_HERO )
			{
				char tcmd[MIL];

				act( buf, ch, NULL, vch, TO_VICT, POS_REST );

				sprintf( tcmd, "%s %s", translate_command(arg, ch, vch), argument );
				interpret( vch, tcmd, FALSE );
			}
		}
	}
	else
	{
		CHAR_DATA	*victim;
		char tcmd[MIL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( (victim = get_char_world(ch, name)) == NULL )
		{
			send_to_char( "They aren't here.\n\r", ch );
			return;
		}

		if ( victim == ch )
		{
			send_to_char( "Aye aye, right away!\n\r", ch );
			return;
		}

		if (!is_room_owner(ch, victim->in_room)
		&&	ch->in_room != victim->in_room
		&&	room_is_private(victim->in_room)
		&&	!IS_TRUSTED(ch, LEVEL_IMPLEMENTOR) )
		{
			send_to_char( "That character is in a private room.\n\r", ch );
			return;
		}

		if ( get_trust(victim) >= get_trust(ch) )
		{
			send_to_char( "HAHAHA!\n\r", ch );
			return;
		}

		if ( !IS_MOB(victim) && get_trust(ch) < MAX_LEVEL - 3 )
		{
			send_to_char( "Not at your level!\n\r", ch );
			return;
		}

		act( buf, ch, NULL, victim, TO_VICT, POS_REST );

		sprintf( tcmd, "%s %s", translate_command(arg, ch, victim), argument );
		interpret( victim, tcmd, FALSE );
	}

	send_to_char( "Ok.\n\r", ch );
}


/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
	int		level;
	char	arg[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* RT code for taking a level argument */
	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{	/* take the default path */
		if ( ch->invis_level )
		{
			ch->invis_level = 0;
			send_to_char( "Il tuo corpo ridiventa visibile.\n\r", ch );
		}
		else
		{
			ch->invis_level = get_trust( ch );
			send_to_char( "Il tuo corpo svanisce nell'aria.\n\r", ch );
		}
	}
	else
	/* do the level thing */
	{
		level = atoi( arg );
		if ( level < 1 || level > get_trust(ch) )
		{
			send_to_char( "Invis level must be between 1 and your level.\n\r", ch );
			return;
		}
		else
		{
			ch->reply = NULL;
			ch->invis_level = level;
			send_to_char( "Il tuo corpo svanisce nell'aria.\n\r", ch );
		}
	}
}


void do_incognito( CHAR_DATA *ch, char *argument )
{
	int		level;
	char	arg[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* RT code for taking a level argument */
	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{	/* take the default path */
		if ( ch->incog_level )
		{
			ch->incog_level = 0;
			send_to_char( "You are no longer cloaked.\n\r", ch );
		}
		else
		{
			ch->incog_level = get_trust( ch );
			send_to_char( "You cloak your presence.\n\r", ch );
		}
	}
	else
	/* do the level thing */
	{
		level = atoi( arg );
		if ( level < 1 || level > get_trust(ch) )
		{
			send_to_char( "Incog level must be between 1 and your level.\n\r", ch );
			return;
		}
		else
		{
			ch->reply = NULL;
			ch->incog_level = level;
			send_to_char( "You cloak your presence.\n\r", ch );
		}
	}
}


void do_holylight( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
	{
		REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
		send_to_char( "Holy light mode off.\n\r", ch );
	}
	else
	{
		SET_BIT( ch->act, PLR_HOLYLIGHT );
		send_to_char( "Holy light mode on.\n\r", ch );
	}
}


/*
 * prefix command: it will put the string typed on each line typed
 */
void do_prefi( CHAR_DATA *ch, char *argument )
{
	send_to_char( "You cannot abbreviate the prefix command.\r\n", ch );
}


void do_prefix( CHAR_DATA *ch, char *argument )
{
	char	buf[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		if ( ch->prefix[0] == '\0' )
		{
			send_to_char( "You have no prefix to clear.\r\n", ch );
			return;
		}

		send_to_char( "Prefix removed.\r\n", ch );
		free_string( ch->prefix );
		ch->prefix = str_dup( "" );
		return;
	}

	if ( ch->prefix[0] != '\0' )
	{
		sprintf( buf, "Prefix changed to %s.\r\n", argument );
		free_string( ch->prefix );
	}
	else
	{
		sprintf( buf, "Prefix set to %s.\r\n", argument );
	}

	ch->prefix = str_dup( argument );
}


/*
 * RT nochannels command, for those spammers
 */
void do_grant( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Grant whom induct privileges?", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_SET(victim->act, PLR_CANINDUCT) )
	{
		REMOVE_BIT( victim->act, PLR_CANINDUCT );
		send_to_char( "Non sei piu' a capo della tua Gilda.\n\r", victim );
		send_to_char( "INDUCT powers removed.\n\r", ch );
	}

	else
	{
		SET_BIT( victim->act, PLR_CANINDUCT );
		send_to_char( "Ora sei a capo della tua gilda!\n\r", victim );
		send_to_char( "INDUCT powers given.\n\r", ch );
	}
}


void do_cecho( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	char			buf[MIL];
	char			color[MIL];
	bool			bFound = FALSE;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Color echo what?\n\r", ch );
		return;
	}

	argument = one_argument( argument, color );

	for ( i = 0; color_table[i].name != NULL; i++ )
	{
		if ( !str_cmp(color, color_table[i].name) )
		{
			bFound = TRUE;
			break;
		}
	}

	if ( !bFound )
	{
		sprintf( buf, "Usage: cecho <color> <message>.\n\rChoose from one of these colors:\n\r" );

		for ( i = 0; color_table[i].name != NULL; i++ )
		{
			strcat( buf, color_table[i].name );
			strcat( buf, " " );
		}

		send_to_char( buf, ch );
		return;
	}

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( d->connected == CON_PLAYING )

		{
			if ( get_trust(d->character) >= get_trust(ch) )
				send_to_char( "global> ", d->character );
			act( "$C$t$c", d->character, argument, NULL, TO_CHAR, POS_REST, color_table[i].code );
		}
	}
}


void do_advance( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	int			level;
	int			iLevel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );


	if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
	{
		send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg1)) == NULL )
	{
		send_to_char( "That player is not here.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( (level = atoi(arg2)) < 1 || level > MAX_LEVEL )
	{
		send_to_char( "Level must be 1 to 100.\n\r", ch );
		return;
	}

	if ( level > get_trust(ch) )
	{
		send_to_char( "Limited to your trust level.\n\r", ch );
		return;
	}

	/* Level counting */
	if ( ch->level <= 5 || ch->level > LEVEL_HERO )
	{
		if ( 5 < level && level <= LEVEL_HERO )
			total_levels += level - 5;
	}
	else
	{
		if ( 5 < level && level <= LEVEL_HERO )
			total_levels += level - ch->level;
		else
			total_levels -= ( ch->level - 5 );
	}

	/* Lower level: Reset to level 1. Then raise again. Currently, an imp can lower another imp. -- Swiftest */
	if ( level <= victim->level )
	{
		int temp_prac;
		/*~~~~~~~~~~*/

		send_to_char( "Lowering a player's level!\n\r", ch );
		temp_prac = victim->practice;
		victim->level = 1;
		victim->exp = exp_to_level( victim, victim->pcdata->points );
		victim->max_point[LIFE] = 20;
		victim->max_point[MANA] = 100;
		victim->max_point[MOVE] = 100;
		victim->practice = 0;
		victim->point[LIFE] = victim->max_point[LIFE];
		victim->point[MANA] = victim->max_point[MANA];
		victim->point[MOVE] = victim->max_point[MOVE];
		advance_level( victim );
		victim->practice = temp_prac;
	}
	else
		send_to_char( "Huzzah!\n\r", ch );

	for ( iLevel = victim->level; iLevel < level; iLevel++ )
	{
		victim->exp += exp_to_level( victim, victim->pcdata->points );
		victim->level += 1;
		advance_level( victim );
	}

	victim->trust = 0;
	save_char_obj( victim );
}


void do_mset( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	char		arg3[MIL];
	char		buf[MSL];
	CHAR_DATA	*victim;
	int			value, sn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set char <name> <field> <value>\n\r", ch );
		send_to_char( "  Field being one of:\n\r", ch );
		send_to_char( "    str int wis dex con cha sex class level\n\r", ch );
		send_to_char( "    race gold hp mana move practice align\n\r", ch );
		send_to_char( "    train thirst drunk full hometown ethos\n\r", ch );

		/* Added By KIO */
		send_to_char( "    questp questt relig bloodlust desire\n\r", ch );

		/* Added By KIO */
		return;
	}

	if ( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	/* Snarf the value (which need not be numeric). */
	value = is_number( arg3 ) ? atoi( arg3 ) : -1;

	/* Set something. */
	if ( !str_cmp(arg2, "str") )
	{
		if ( value < 3 || value > get_max_train(victim, STR) )
		{
			sprintf( buf, "Strength range is 3 to %d\n\r.", get_max_train(victim, STR) );
			send_to_char( buf, ch );
			return;
		}

		victim->perm_stat[STR] = value;
	}
	else if ( !str_cmp(arg2, "int") )
	{
		if ( value < 3 || value > get_max_train(victim, INT) )
		{
			sprintf( buf, "Intelligence range is 3 to %d.\n\r", get_max_train(victim, INT) );
			send_to_char( buf, ch );
			return;
		}

		victim->perm_stat[INT] = value;
	}
	else if ( !str_cmp(arg2, "wis") )
	{
		if ( value < 3 || value > get_max_train(victim, WIS) )
		{
			sprintf( buf, "Wisdom range is 3 to %d.\n\r", get_max_train(victim, WIS) );
			send_to_char( buf, ch );
			return;
		}

		victim->perm_stat[WIS] = value;
	}
	else if ( !str_cmp(arg2, "questp") )
	{
		if ( value == -1 )
			value = 0;
		if ( !IS_MOB(victim) )
			victim->pcdata->questpoints = value;
	}
	else if ( !str_cmp(arg2, "questt") )
	{
		if ( value == -1 )
			value = 30;
		if ( !IS_MOB(victim) )
			victim->pcdata->nextquest = value;
	}
	else if ( !str_cmp(arg2, "relig") )
	{
		if ( value == -1 )
			value = 0;
		victim->religion = value;
	}
	else if ( !str_cmp(arg2, "dex") )
	{
		if ( value < 3 || value > get_max_train(victim, DEX) )
		{
			sprintf( buf, "Dexterity ranges is 3 to %d.\n\r", get_max_train(victim, DEX) );
			send_to_char( buf, ch );
			return;
		}

		victim->perm_stat[DEX] = value;
	}
	else if ( !str_cmp(arg2, "con") )
	{
		if ( value < 3 || value > get_max_train(victim, CON) )
		{
			sprintf( buf, "Constitution range is 3 to %d.\n\r", get_max_train(victim, CON) );
			send_to_char( buf, ch );
			return;
		}

		victim->perm_stat[CON] = value;
	}
	else if ( !str_cmp(arg2, "cha") )
	{

		if ( value < 3 || value > get_max_train(victim, CHA) )
		{
			sprintf( buf, "Constitution range is 3 to %d.\n\r", get_max_train(victim, CHA) );
			send_to_char( buf, ch );
			return;
		}

		victim->perm_stat[CHA] = value;
	}
	else if ( !str_prefix(arg2, "sex") )
	{
		if ( value < 0 || value > 2 )
		{
			send_to_char( "Sex range is 0 to 2.\n\r", ch );
			return;
		}

		if ( (victim->class == CLASS_INVOKER) || (victim->class == CLASS_TRANSMUTER) )
		{
			send_to_char( "You can't change their sex.\n\r", ch );
			return;
		}

		victim->sex = value;
		if ( !IS_MOB(victim) )
			victim->pcdata->true_sex = value;
	}
	else if ( !str_prefix(arg2, "class") )
	{
		int class;
		/*~~~~~~*/

		if ( IS_MOB(victim) )
		{
			send_to_char( "Mobiles have no class.\n\r", ch );
			return;
		}

		class = class_lookup( arg3 );
		if ( class == -1 )
		{
			sprintf( buf, "le classi possibili sono: " );
			for ( class = 0; class < MAX_CLASS; class++ )
			{
				if ( class > 0 )
					strcat( buf, " " );
				strcat( buf, class_table[class].name );
			}

			strcat( buf, ".\n\r" );
			send_to_char( buf, ch );
			return;
		}

		victim->class = class;
		victim->exp = victim->level * exp_per_level( victim, 0 );
	}
	else if ( !str_prefix(arg2, "level") )
	{
		if ( !IS_MOB(victim) )
		{
			send_to_char( "Not on PC's.\n\r", ch );
			return;
		}

		if ( value < 0 || value > 100 )
		{
			send_to_char( "Level range is 0 to 100.\n\r", ch );
			return;
		}

		victim->level = value;
	}
	else if ( !str_prefix(arg2, "gold") )
	{
		victim->gold = value;
	}
	else if ( !str_prefix(arg2, "hp") )
	{
		if ( !IS_MOB(victim) && (victim->max_point[LIFE] + value) >= pc_race_table[victim->race].max_point[LIFE] )
		{
			sprintf( buf, "Non puoi superare %d per la razza %s.\n\r",
				pc_race_table[victim->race].max_point[LIFE], RACE_NAME(ch) );
			send_to_char( buf, ch );
			return;
		}

		if ( value < -10 || value > 30000 )
		{
			send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
			return;
		}

		victim->max_point[LIFE] = value;
		if ( !IS_MOB(victim) )
			victim->pcdata->perm_point[LIFE] = value;
	}
	else if ( !str_prefix(arg2, "mana") )
	{
		if ( !IS_MOB(victim) && (victim->max_point[MANA] + value) >= pc_race_table[victim->race].max_point[MANA] )
		{
			sprintf( buf, "Non puoi superare %d per la razza %s.\n\r",
				pc_race_table[victim->race].max_point[LIFE], RACE_NAME(ch) );
			send_to_char( buf, ch );
			return;
		}

		if ( value < 0 || value > 60000 )
		{
			send_to_char( "Mana range is 0 to 60,000 mana points.\n\r", ch );
			return;
		}

		victim->max_point[MANA] = value;
		if ( !IS_MOB(victim) )
			victim->pcdata->perm_point[MANA] = value;
	}
	else if ( !str_prefix(arg2, "move") )
	{
		if ( !IS_MOB(victim) && (victim->max_point[MOVE] + value) >= pc_race_table[victim->race].max_point[MOVE] )
		{
			sprintf( buf, "Non puoi superare %d per la razza %s.\n\r",
				pc_race_table[victim->race].max_point[LIFE], RACE_NAME(ch) );
			send_to_char( buf, ch );
			return;
		}

		if ( value < 0 || value > 60000 )
		{
			send_to_char( "Move range is 0 to 60,000 move points.\n\r", ch );
			return;
		}

		victim->max_point[MOVE] = value;
		if ( !IS_MOB(victim) )
			victim->pcdata->perm_point[MOVE] = value;
	}
	else if ( !str_prefix(arg2, "practice") )
	{
		if ( value < 0 || value > 250 )
		{
			send_to_char( "Practice range is 0 to 250 sessions.\n\r", ch );
			return;
		}

		victim->practice = value;
	}
	else if ( !str_prefix(arg2, "train") )
	{
		if ( value < 0 || value > 50 )
		{
			send_to_char( "Training session range is 0 to 50 sessions.\n\r", ch );
			return;
		}

		victim->train = value;
	}
	else if ( !str_prefix(arg2, "align") )
	{
		if ( value < -1000 || value > 1000 )
		{
			send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
			return;
		}

		victim->alignment = value;
		send_to_char( "Remember to check their hometown.\n\r", ch );
	}
	else if ( !str_prefix(arg2, "ethos") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Mobiles don't have an ethos.\n\r", ch );
			return;
		}

		if ( value < 0 || value > 3 )
		{
			send_to_char( "The values are Lawful - 1, Neutral - 2, Chaotic - 3\n\r", ch );
			return;
		}

		victim->ethos = value;
	}
	else if ( !str_prefix(arg2, "hometown") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Mobiles don't have hometowns.\n\r", ch );
			return;
		}

		if ( value < 0 || value > 4 )
		{
			send_to_char( "Please choose one of the following :.\n\r", ch );
			send_to_char( "Town        Alignment       Value\n\r", ch );
			send_to_char( "----        ---------       -----\n\r", ch );
			send_to_char( "Midgaard     Any              0\n\r", ch );
			send_to_char( "New Thalos   Any              1\n\r", ch );
			send_to_char( "Titan        Any              2\n\r", ch );
			send_to_char( "Ofcol        Neutral          3\n\r", ch );
			send_to_char( "Old Midgaard Evil             4\n\r", ch );
			return;
		}

		if ( (value == 2 && !IS_GOOD(victim)) || (value == 3 && !IS_NEUTRAL(victim)) || (value == 4 && !IS_EVIL(victim)) )
		{
			send_to_char( "The hometown doesn't match this character's alignment.\n\r", ch );
			return;
		}

		victim->hometown = value;
	}
	else if ( !str_prefix(arg2, "thirst") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 )
		{
			send_to_char( "Thirst range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_THIRST] = value;
	}
	else if ( !str_prefix(arg2, "drunk") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 )
		{
			send_to_char( "Drunk range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_DRUNK] = value;
	}
	else if ( !str_prefix(arg2, "hunger") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 )
		{
			send_to_char( "Hunger range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_HUNGER] = value;
	}
	else if ( !str_prefix(arg2, "full") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 )
		{
			send_to_char( "Full range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_FULL] = value;
	}
	else if ( !str_prefix(arg2, "bloodlust") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 )
		{
			send_to_char( "Full range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_BLOODLUST] = value;
	}
	else if ( !str_prefix(arg2, "desire") )
	{
		if ( IS_MOB(victim) )
		{
			send_to_char( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 )
		{
			send_to_char( "Full range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_DESIRE] = value;
	}
	else if ( !str_prefix(arg2, "race") )
	{
		int race;
		/*~~~~~*/

		race = race_lookup( arg3 );

		if ( race == 0 )
		{
			send_to_char( "That is not a valid race.\n\r", ch );
			return;
		}

		if ( !IS_MOB(victim) && !race_table[race].pc_race )
		{
			send_to_char( "That is not a valid player race.\n\r", ch );
			return;
		}

		if ( !IS_MOB(victim) )
		{
			for ( sn = 0; sn < MAX_SKILL; sn++ )
			{
				if ( (skill_table[sn].name != NULL) && !RACE_OK(victim, sn) )
					victim->pcdata->learned[sn] = 0;

				if ( (skill_table[sn].name != NULL) && (ORG_RACE(victim) == skill_table[sn].race) )
					victim->pcdata->learned[sn] = 70;
			}
		}

		if ( ORG_RACE(victim) == victim->race )
			victim->race = race;

		if ( IS_MOB(victim) )
			victim->pIndexData->race = race;
		else
			victim->pcdata->race = race;

		victim->exp = victim->level * exp_per_level( victim, 0 );
	}
	else
		do_mset( ch, "" );/* Generate usage message. */

	send_to_char( "Ok.", ch );
}


void do_induct( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		*arg2;
	char		buf[MSL];
	char		*cabal;
	CHAR_DATA	*victim;
	int			sn;
	int			i, prev_cabal = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	cabal = NULL;
	argument = one_argument( argument, arg1 );
	arg2 = argument;

	if ( arg1[0] == '\0' || *arg2 == '\0' )
	{
		send_to_char( "Utilizzo: induct <avventuriero> <gilda>\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char( "L'avventuriero dev'essere davanti a te!\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		act( "No, non $N.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( (i = cabal_lookup(arg2)) == -1 )
	{
		send_to_char( "E che cabala sarebbe?\n\r", ch );
		return;
	}

	if ( victim->class == CLASS_WARRIOR && i == CABAL_YVALADRIN )
	{
		act( "$N!? Ma e' un guerriero!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( i == CABAL_RULER && get_curr_stat(victim, INT) < 20 )
	{
		act( "$N non e' abbastanza intelligente!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if(IS_TRUSTED(ch, LEVEL_ADMIN)
	|| (IS_SET(ch->act, PLR_CANINDUCT)
	&& ((i == CABAL_NONE && (ch->cabal == victim->cabal))
	|| (i != CABAL_NONE && ch->cabal == i && victim->cabal == CABAL_NONE))) )
	{
		prev_cabal = victim->cabal;
		victim->cabal = i;
		REMOVE_BIT( victim->act, PLR_CANINDUCT );
		cabal = cabal_table[i].long_name;
	}
	else
	{
		send_to_char( "Non sei a capo di nessuna gilda.\n\r", ch );
		return;
	}

	/* set cabal skills to 60, remove other cabal skills */
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( (victim->cabal) && (skill_table[sn].cabal == victim->cabal) )
			victim->pcdata->learned[sn] = 60;
		else if ( skill_table[sn].cabal != CABAL_NONE && victim->cabal != skill_table[sn].cabal )
			victim->pcdata->learned[sn] = 0;
	}

	sprintf( buf, "$n e' entrato a far parte della gilda %s.", cabal );
	act( buf, victim, NULL, NULL, TO_NOTVICT, POS_REST );
	sprintf( buf, "Da questo momento sei un membro della gilda %s.", cabal );
	act( buf, victim, NULL, NULL, TO_CHAR, POS_REST );
	if ( ch->in_room != victim->in_room )
	{
		sprintf( buf, "%s has been inducted into %s.\n\r", IS_MOB(victim) ? victim->short_descr : victim->name, cabal );
		send_to_char( buf, ch );
	}
}


void do_desocket( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d, *d_next;
	int				socket;
	char			arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( !is_number(arg) )
	{
		send_to_char( "The argument must be a number.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Disconnect which socket?\n\r", ch );
		return;
	}
	else
	{
		socket = atoi( arg );
		for ( d = descriptor_list; d != NULL; d = d_next )
		{
			d_next = d->next;
			if ( d->descriptor == socket )
			{
				if ( d->character == ch )
				{
					send_to_char( "It would be foolish to disconnect yourself.\n\r", ch );
					return;
				}

				if ( d->connected == CON_PLAYING )
				{
					send_to_char( "Why don't you just use disconnect?\n\r", ch );
					return;
				}

				write_to_descriptor( d->descriptor, "Disconnessione da parte di un immortale.", 0 );
				close_socket( d );
				send_to_char( "Done.\n\r", ch );
				return;
			}
		}

		send_to_char( "No such socket is connected.\n\r", ch );
		return;
	}
}


void do_smit( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Se vuoi fulminare qualcuno, scrivilo per esteso.", ch );
}


void do_smite( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "You are so frustrated you smite yourself!  OWW!\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, argument)) == NULL )
	{
		send_to_char( "You'll have to smite them some other day.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "That poor mob never did anything to you.\n\r", ch );
		return;
	}

	if ( victim->trust > ch->trust )
	{
		send_to_char( "How dare you!\n\r", ch );
		return;
	}

	if ( victim->position < POS_SLEEP )
	{
		send_to_char( "Take pity on the poor thing.\n\r", ch );
		return;
	}

	act( "Un fulmine cade dal cielo direttamente su di te!!!", victim, NULL, ch, TO_CHAR, POS_REST );
	act( "Invochi un fulmine contro $n!", victim, NULL, ch, TO_VICT, POS_REST );
	act( "Un fulmine cade dal cielo direttamente su $n!", victim, NULL, ch, TO_NOTVICT, POS_REST );
	victim->point[LIFE] = victim->point[LIFE] / 2;
}


void do_popularity( CHAR_DATA *ch, char *argument )
{
	char				buf[4 * MSL];
	char				buf2[MSL];
	AREA_DATA			*area;
	extern AREA_DATA	*area_first;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Area popularity statistics (in char * ticks)\n\r" );

	for ( area = area_first, i = 0; area != NULL; area = area->next, i++ )
	{
		if ( area->count >= 5000000 )
			sprintf( buf2, "%-20s overflow       ", area->name );
		else
			sprintf( buf2, "%-20s %-8lu       ", area->name, area->count );

		if ( i % 2 == 0 )
			strcat( buf, "\n\r" );
		strcat( buf, buf2 );
	}

	strcat( buf, "\n\r\n\r" );
	page_to_char( buf, ch );
}


void do_rename( CHAR_DATA *ch, char *argument )
{
	char		old_name[MIL],
				new_name[MIL],
				strsave[MIL];
	CHAR_DATA	*victim;
	FILE		*file;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, old_name );
	one_argument( argument, new_name );

	if ( !old_name[0] )
	{
		send_to_char( "Rename who?\n\r", ch );
		return;
	}

	victim = get_char_world( ch, old_name );

	if ( !victim )
	{
		send_to_char( "There is no such a person online.\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) )
	{
		send_to_char( "You cannot use Rename on NPCs.\n\r", ch );
		return;
	}

	if ( (victim != ch) && (get_trust(victim) >= get_trust(ch)) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( !victim->desc || (victim->desc->connected != CON_PLAYING) )
	{
		send_to_char( "This player has lost his link or is inside a pager or the like.\n\r", ch );
		return;
	}

	if ( !new_name[0] )
	{
		send_to_char( "Rename to what new name?\n\r", ch );
		return;
	}

	/*
	 * if (victim->cabal) { send_to_char ("This player is member of a cabal, remove him from there first.\n\r",ch);
	 * return; }
	 */
	new_name[0] = UPPER( new_name[0] );
	if ( !check_parse_name(new_name) )
	{
		send_to_char( "The new name is illegal.\n\r", ch );
		return;
	}

	if ( get_char_world(ch, new_name) )
	{
		send_to_char( "A player with the name you specified already exists!\n\r", ch );
		return;
	}

	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(new_name) );

	fclose( fpReserve );
	file = fopen( strsave, "r" );
	if ( file )
	{
		send_to_char( "A player with that name already exists!\n\r", ch );
		fclose( file );
		fpReserve = fopen( NULL_FILE, "r" );
		return;
	}

	fpReserve = fopen( NULL_FILE, "r" );

	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(victim->name) );

	/* NOTE: Players who are level 1 do NOT get saved under a new name */
	free_string( victim->name );
	victim->name = str_dup( capitalize(new_name) );

	save_char_obj( victim );

	unlink( strsave );
	send_to_char( "Character renamed.\n\r", ch );
	victim->position = POS_STAND;
	act( "$n ti ha rinominato in $N!", ch, NULL, victim, TO_VICT, POS_REST );
}


void do_noaffect( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA *paf, *paf_next;
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !IS_ADMIN(ch) )
		return;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Usage:\n\r  noaffect <player>\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg)) == NULL )
	{
		send_to_char( "He is not currently playing.\n\r", ch );
		return;
	}

	for ( paf = victim->affected; paf != NULL; paf = paf_next )
	{
		paf_next = paf->next;
		if ( paf->duration >= 0 )
		{
			if ( paf->type > 0 && skill_table[paf->type].msg_off )
			{
				send_to_char( skill_table[paf->type].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

			affect_remove( victim, paf );
		}
	}
}


void do_affrooms( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *room;
	ROOM_INDEX_DATA *room_next;
	char			buf[MSL];
	int				count = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !top_affected_room )
	{
		send_to_char( "No affected room.\n\r", ch );
	}

	for ( room = top_affected_room; room; room = room_next )
	{
		room_next = room->aff_next;
		count++;
		sprintf( buf, "%d) [Vnum : %5d] %s\n\r", count, room->vnum, room->name );
		send_to_char( buf, ch );
	}
}


void do_find( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *location;
	char			buf[MSL];
	char			lbuf[MSL];
	char			arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Ok. But what I should find?\n\r", ch );
		return;
	}

	if ( (location = find_location(ch, arg)) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	sprintf( buf, "%s.\n\r", find_way(ch, ch->in_room, location) );
	send_to_char( buf, ch );
	sprintf( lbuf, "From %d to %d: %s", ch->in_room->vnum, location->vnum, buf );
	log_string( lbuf );
}


void do_reboot( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Usage: reboot now\n\r", ch );
		send_to_char( "Usage: reboot <ticks to reboot>\n\r", ch );
		send_to_char( "Usage: reboot cancel\n\r", ch );
		send_to_char( "Usage: reboot status\n\r", ch );
		return;
	}

	if ( is_name(arg, "cancel cancella") )
	{
		if ( time_sync )
		{
			send_to_char( "Time synchronization is activated, you cannot cancel the reboot.\n\r", ch );
			return;
		}

		reboot_counter = -1;
		send_to_char( "Reboot canceled.\n\r", ch );
		return;
	}

	if ( is_name(arg, "now ora") )
	{
		reboot_xandra( TRUE, XANDRA_REBOOT );
		return;
	}

	if ( is_name(arg, "status stato") )
	{
		if ( time_sync )
		{
			send_to_char( "Time synchronization is activated.\n\r", ch );
			return;
		}

		if ( reboot_counter == -1 )
			sprintf( buf, "Only time synchronization reboot is activated.\n\r" );
		else
			sprintf( buf, "Nuova Alba entro %i minuti.\n\r", reboot_counter );
		send_to_char( buf, ch );
		return;
	}

	if ( is_number(arg) )
	{
		if ( time_sync )
		{
			send_to_char( "Time synchronization is activated, you cannot change the reboot.\n\r", ch );
			return;
		}

		reboot_counter = atoi( arg );
		sprintf( buf, "Nuova Alba entro %i minuti.\n\r", reboot_counter );
		send_to_char( buf, ch );
		return;
	}

	do_reboot( ch, "" );
}


void reboot_xandra( bool fMessage, int fType )
{
	DESCRIPTOR_DATA *d, *d_next;
	char buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( fType == XANDRA_REBOOT )
	{
		sprintf( log_buf, "Rebooting di Xandra." );
		sprintf( buf, "Xandra is going down for reboot NOW!\n\r" );
	}
	else
	{
		sprintf( log_buf, "Shutting down Xandra.");
		sprintf( buf, "Xandra is going down for halt NOW!\n\r" );
	}

	log_string( log_buf );

	xandra_down = TRUE;
	xandra_exit = fType;

	for ( d = descriptor_list; d != NULL; d = d_next )
	{
		d_next = d->next;

		if ( fMessage )
			write_to_buffer( d, buf, 0 );

		if ( d->character != NULL )
			save_char_obj( d->character );

		close_socket( d );
	}
}


void do_maximum( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Sintassi: maximum reset\n\r", ch );
		send_to_char( "Sintassi: maximum newbies <number of newbies>\n\r", ch );
		send_to_char( "Sintassi: maximum oldies <number of oldies>\n\r\n\r", ch );

		sprintf( buf, "Maximum oldies allowed: %d.\n\r", max_oldies );
		send_to_char( buf, ch );
		sprintf( buf, "Maximum newbies allowed: %d.\n\r", max_newbies );
		send_to_char( buf, ch );
		sprintf( buf, "Current number of players: %d.\n\r", iNumPlayers );
		send_to_char( buf, ch );

		return;
	}

	if ( is_name(arg, "reset resetta") )
	{
		max_newbies = MAX_NEWBIES;
		max_oldies = MAX_OLDIES;
		sprintf( buf, "Maximum newbies and oldies have been reset.\n\r" );
		send_to_char( buf, ch );
		do_maximum( ch, "status" );
		return;
	}

	if ( is_name(arg, "newbies newbie") )
	{
		argument = one_argument( argument, arg );
		if ( !is_number(arg) )
		{
			do_maximum( ch, "" );
			return;
		}

		max_newbies = atoi( arg );
		if ( max_newbies < 0 )
			sprintf( buf, "No newbies are allowed!!!\n\r" );
		else
			sprintf( buf, "Now maximum newbies allowed: %d.\n\r", max_newbies );
		send_to_char( buf, ch );
		return;
	}

	if ( is_name(arg, "oldies oldie") )
	{
		argument = one_argument( argument, arg );
		if ( !is_number(arg) )
		{
			do_maximum( ch, "" );
			return;
		}

		max_oldies = atoi( arg );
		if ( max_oldies < 0 )
			sprintf( buf, "No oldies are allowed!!!\n\r" );
		else
			sprintf( buf, "Now maximum oldies allowed: %d.\n\r", max_oldies );
		send_to_char( buf, ch );
		return;
	}

	do_maximum( ch, "" );
}
