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

/***************************************************************************
 *  This file is a combination of:                                         *
 *    (H)unt.c, (E)nter.c, (R)epair.c and (A)uction.c                      *
 *  Thus it is called ACT_HERA.C                                           *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xandra.h"
#include "interp.h"


/*
 * Random room generation procedure
 */
ROOM_INDEX_DATA *get_random_room( CHAR_DATA *ch )
{
	ROOM_INDEX_DATA *room;
	/*~~~~~~~~~~~~~~~~~~*/

	for ( ; ; )
	{
		room = get_room_index( number_range(0, 65535) );
		if ( room != NULL )
		{
			if ( can_see_room(ch, room)
			&&	!room_is_private(room)
			&&	!IS_SET(room->room_flags, ROOM_PRIVATE)
			&&	!IS_SET(room->room_flags, ROOM_SOLITARY)
			&&	!IS_SET(room->room_flags, ROOM_SAFE)
			&&	(IS_MOB(ch) || IS_SET(ch->act, ACT_AGGRESSIVE) || !IS_SET(room->room_flags, ROOM_LAW)) )
				break;
		}
	}

	return( room );
}


/*
 * RT Enter portals
 */
void do_enter( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *location;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->fighting != NULL )
		return;

	/* nifty portal stuff */
	if ( argument[0] != '\0' )
	{
		ROOM_INDEX_DATA *old_room;
		OBJ_DATA		*portal;
		CHAR_DATA		*fch, *fch_next, *mount;
		char			buf[MSL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		old_room = ch->in_room;

		portal = get_obj_list( ch, argument, ch->in_room->contents );

		if ( portal == NULL )
		{
			send_to_char( "Non vedi cio' qui.\n\r", ch );
			return;
		}

		if ( portal->item_type != ITEM_PORTAL || (IS_SET(portal->value[1], EX_CLOSED) && !IS_TRUSTED(ch, LEVEL_MASTER)) )
		{
			send_to_char( "Non riesci a trovare modo di entrarci, forse non e' un portale.\n\r", ch );
			return;
		}

		if ( !IS_TRUSTED(ch, LEVEL_MASTER)
		&&	 !IS_SET(portal->value[2], GATE_NOCURSE)
		&&	(IS_AFFECTED(ch, AFF_CURSE) || IS_SET(old_room->room_flags, ROOM_NO_RECALL) || IS_RAFFECTED(old_room, AFF_ROOM_CURSE)) )
		{
			send_to_char( "Qualcosa ti impedisce di andartene..\n\r", ch );
			return;
		}

		if ( IS_SET(portal->value[2], GATE_RANDOM) || portal->value[3] == -1 )
		{
			location = get_random_room( ch );
			portal->value[3] = location->vnum;			/* keeps record */
		}
		else if ( IS_SET(portal->value[2], GATE_BUGGY) && (number_percent() < 5) )
			location = get_random_room( ch );
		else
			location = get_room_index( portal->value[3] );

		if ( location == NULL
		||	 location == old_room
		||	!can_see_room(ch, location)
		||	(room_is_private(location) && !IS_TRUSTED(ch, LEVEL_IMPLEMENTOR)) )
		{
			act( "$p non sembra condurre in nessun luogo.", ch, portal, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( IS_MOB(ch) && IS_SET(ch->act, ACT_AGGRESSIVE) && IS_SET(location->room_flags, ROOM_LAW) )
		{
			send_to_char( "Qualcosa ti impedisce di andartene..\n\r", ch );
			return;
		}

		if ( MOUNTED(ch) )
			sprintf( buf, "$n entra in $p, cavalcando %s.", MOUNTED(ch)->short_descr );
		else
			sprintf( buf, "$n entra in $p." );
		act( buf, ch, portal, NULL, TO_ROOM, POS_REST );

		if ( IS_SET(portal->value[2], GATE_NORMAL_EXIT) )
			act( "Entri con un balzo in $p.", ch, portal, NULL, TO_CHAR, POS_REST );

		else
			act( "Cammini dentro $p e ti ritrovi da un'altra parte..", ch, portal, NULL, TO_CHAR, POS_REST );

		mount = MOUNTED( ch );
		char_from_room( ch );
		char_to_room( ch, location );

		if ( IS_SET(portal->value[2], GATE_GOWITH) )	/* take the gate along */
		{
			obj_from_room( portal );
			obj_to_room( portal, location );
		}

		if ( IS_SET(portal->value[2], GATE_NORMAL_EXIT) )
		{
			if ( mount )
				act( "$n appare cavalcando $N", ch, portal, mount, TO_ROOM, POS_REST );
			else

				act( "$n appare all'improvviso.", ch, portal, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			if ( mount )
				act( "$n appare attraverso $p, cavalcando $N.", ch, portal, mount, TO_ROOM, POS_REST );
			else
				act( "$n appare attraverso $p.", ch, portal, NULL, TO_ROOM, POS_REST );
		}

		do_look( ch, "auto" );

		if ( mount )
		{
			char_from_room( mount );
			char_to_room( mount, location );
			ch->riding = TRUE;
			mount->riding = TRUE;
		}

		/* charges */
		if ( portal->value[0] > 0 )
		{
			portal->value[0]--;
			if ( portal->value[0] == 0 )
				portal->value[0] = -1;
		}

		/* protect against circular follows */
		if ( old_room == location )
			return;

		for ( fch = old_room->people; fch != NULL; fch = fch_next )
		{
			fch_next = fch->next_in_room;

			if ( portal == NULL || portal->value[0] == -1 )
				/* no following through dead portals */
				continue;

			if ( fch->master == ch && IS_AFFECTED(fch, AFF_CHARM) && fch->position < POS_STAND )
				do_stand( fch, "" );

			if ( fch->master == ch && fch->position == POS_STAND )
			{
				if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) && (IS_MOB(fch) && IS_SET(fch->act, ACT_AGGRESSIVE)) )
				{
					act( "Non puoi portare con te $N in una citta'.", ch, NULL, fch, TO_CHAR, POS_REST );
					act( "Ti e' impedito l'accesso alla citta'.", fch, NULL, NULL, TO_CHAR, POS_REST );
					continue;
				}

				act( "Segui $N.", fch, NULL, ch, TO_CHAR, POS_REST );
				do_enter( fch, argument );
			}
		}

		if ( portal != NULL && portal->value[0] == -1 )
		{
			act( "$p svanisce nel nulla.", ch, portal, NULL, TO_CHAR, POS_REST );

			if ( ch->in_room == old_room )
				act( "$p svanisce nel nulla.", ch, portal, NULL, TO_ROOM, POS_REST );
			else if ( old_room->people != NULL )
			{
				act( "$p svanisci nel nulla.", old_room->people, portal, NULL, TO_CHAR, POS_REST );
				act( "$p svanisce nel nulla.", old_room->people, portal, NULL, TO_ROOM, POS_REST );
			}

			extract_obj( portal );
		}

		return;
	}

	send_to_char( "No, non puoi farlo.\n\r", ch );
}


void do_settraps( CHAR_DATA *ch, char *argument )
{
	if ( skill_failure_check(ch, gsn_settraps, 0, 0, "Non sai proprio come potresti piazzare delle trappole.") )
		return;

	if ( !ch->in_room )
		return;

	if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) )
	{
		send_to_char( "Un potere magico protegge il luogo.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_settraps].beats );

	if ( IS_MOB(ch) || number_percent() < (get_skill(ch, gsn_settraps) * 0.7) )
	{
		AFFECT_DATA af, af2;
		/*~~~~~~~~~~~~~~~~*/

		check_improve( ch, gsn_settraps, TRUE, 1 );

		if ( is_affected_room(ch->in_room, gsn_settraps) )
		{
			send_to_char( "Questa stanza possiede gia' una trappola.\n\r", ch );
			return;
		}

		if ( is_affected(ch, gsn_settraps) )
		{
			send_to_char( "Questa abilita' e' stata utilizzata troppo di recente.\n\r", ch );
			return;
		}

		af.where = TO_ROOM_AFFECTS;
		af.type = gsn_settraps;
		af.level = ch->level;
		af.duration = ch->level / 40;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_ROOM_THIEF_TRAP;
		affect_to_room( ch->in_room, &af );

		af2.where = TO_AFFECTS;
		af2.type = gsn_settraps;
		af2.level = ch->level;

		if ( ch->last_fight_time != -1 && !IS_ADMIN(ch) && (current_time - ch->last_fight_time) < FIGHT_DELAY_TIME )
			af2.duration = 1;
		else
			af2.duration = ch->level / 10;

		af2.modifier = 0;
		af2.location = APPLY_NONE;
		af2.bitvector = 0;
		affect_to_char( ch, &af2 );
		send_to_char( "Piazzi la tua trappola nel luogo.\n\r", ch );
		act( "$n piazza con abilita' una trappola nel luogo.", ch, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}
	else
		check_improve( ch, gsn_settraps, FALSE, 1 );
}


/* hunt.c */
/*
 * Original idea from SillyMUD v1.1b (C)1993. Modified to merc2.1 by Rip. Modified by Turtle for Merc22 (07-Nov-94).
 * Adopted to ANATOLIA by Chronos.
 */
extern const char	*dir_name[];

struct hash_link
{
	int					key;
	struct hash_link	*next;
	void				*data;
};

struct hash_header
{
	int					rec_size;
	int					table_size;
	int					*keylist, klistsize, klistlen;	/* this is really lame, AMAZINGLY lame */
	struct hash_link	**buckets;
};

#define WORLD_SIZE			32700
#define HASH_KEY( ht, key ) ( (((unsigned int) (key)) * 17) % (ht)->table_size )

struct hunting_data
{
	char				*name;
	struct char_data	**victim;
};

struct room_q
{
	int				room_nr;
	struct room_q	*next_q;
};

struct nodes
{
	int visited;
	int ancestor;
};

#define IS_DIR			( get_room_index(q_head->room_nr)->exit[i] )
#define GO_OK			( !IS_SET(IS_DIR->exit_info, EX_CLOSED) )
#define GO_OK_SMARTER	1


void init_hash_table( struct hash_header *ht, int rec_size, int table_size )
{
	ht->rec_size = rec_size;
	ht->table_size = table_size;
	ht->buckets = (void *) calloc( sizeof(struct hash_link **), table_size );
	ht->keylist = (void *) malloc( sizeof(ht->keylist) * (ht->klistsize = 128) );
	ht->klistlen = 0;
}


CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*ach;
	int			number;
	int			count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
		return( NULL );

	number = number_argument( argument, arg );

	/* if (arg[0] == NULL) return NULL; */
	if ( arg[0] == '\0' )
		return( NULL );
	count = 0;

	if ( (ach = get_char_room(ch, argument)) != NULL )
		return( ach );

	for ( ach = char_list; ach != NULL; ach = ach->next )
	{
		if ( ach->in_room && (ach->in_room->area != ch->in_room->area
		|| !can_see(ch, ach) || !is_name(arg, ach->name)) )
			continue;

		if ( ++count == number )
			return( ach );
	}

	return( NULL );
}


void destroy_hash_table( struct hash_header *ht, void (*gman) () )
{
	int					i;
	struct hash_link	*scan, *temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( i = 0; i < ht->table_size; i++ )
	{
		for ( scan = ht->buckets[i]; scan; )
		{
			temp = scan->next;
			( *gman ) ( scan->data );
			free( scan );
			scan = temp;
		}
	}

	free( ht->buckets );
	free( ht->keylist );
}


void _hash_enter( struct hash_header *ht, int key, void *data )
{
	/* precondition: there is no entry for <key> yet */
	struct hash_link	*temp;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	temp = (struct hash_link *) malloc( sizeof(struct hash_link) );
	temp->key = key;
	temp->next = ht->buckets[HASH_KEY( ht, key )];
	temp->data = data;
	ht->buckets[HASH_KEY( ht, key )] = temp;
	if ( ht->klistlen >= ht->klistsize )
	{
		ht->keylist = (void *) realloc( ht->keylist, sizeof(*ht->keylist) * (ht->klistsize *= 2) );
	}

	for ( i = ht->klistlen; i >= 0; i-- )
	{
		if ( ht->keylist[i - 1] < key )
		{
			ht->keylist[i] = key;
			break;
		}

		ht->keylist[i] = ht->keylist[i - 1];
	}

	ht->klistlen++;
}


ROOM_INDEX_DATA *room_find( ROOM_INDEX_DATA *room_db[], int key )
{
	return( (key < WORLD_SIZE && key > -1) ? room_db[key] : 0 );
}


void *hash_find( struct hash_header *ht, int key )
{
	struct hash_link	*scan;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	scan = ht->buckets[HASH_KEY( ht, key )];

	while ( scan && scan->key != key )
		scan = scan->next;

	return( scan ? scan->data : NULL );
}


int room_enter( ROOM_INDEX_DATA *rb[], int key, ROOM_INDEX_DATA *rm )
{
	ROOM_INDEX_DATA *temp;
	/*~~~~~~~~~~~~~~~~~~*/

	temp = room_find( rb, key );
	if ( temp )
		return( 0 );

	rb[key] = rm;
	return( 1 );
}


int hash_enter( struct hash_header *ht, int key, void *data )
{
	void	*temp;
	/*~~~~~~~~~~*/

	temp = hash_find( ht, key );
	if ( temp )
		return( 0 );

	_hash_enter( ht, key, data );
	return( 1 );
}


ROOM_INDEX_DATA *room_find_or_create( ROOM_INDEX_DATA *rb[], int key )
{
	ROOM_INDEX_DATA *rv;
	/*~~~~~~~~~~~~~~~~*/

	rv = room_find( rb, key );
	if ( rv )
		return( rv );

	rv = (ROOM_INDEX_DATA *) malloc( sizeof(ROOM_INDEX_DATA) );
	rb[key] = rv;

	return( rv );
}


void *hash_find_or_create( struct hash_header *ht, int key )
{
	void	*rval;
	/*~~~~~~~~~~*/

	rval = hash_find( ht, key );
	if ( rval )
		return( rval );

	rval = (void *) malloc( ht->rec_size );
	_hash_enter( ht, key, rval );

	return( rval );
}


int room_remove( ROOM_INDEX_DATA *rb[], int key )
{
	ROOM_INDEX_DATA *tmp;
	/*~~~~~~~~~~~~~~~~~*/

	tmp = room_find( rb, key );
	if ( tmp )
	{
		rb[key] = 0;
		free( tmp );
	}

	return( 0 );
}


int exit_ok( EXIT_DATA *pexit )
{
	ROOM_INDEX_DATA *to_room;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if ( (pexit == NULL) || (to_room = pexit->to_room) == NULL )
		return( 0 );

	return( 1 );
}


void donothing( void )
{
	return;
}


int find_path( int in_room_vnum, int out_room_vnum, CHAR_DATA *ch, int depth, int in_zone )
{
	struct room_q		*tmp_q, *q_head, *q_tail;
	struct hash_header	x_room;
	int					i, tmp_room, count = 0, thru_doors;
	ROOM_INDEX_DATA		*herep;
	ROOM_INDEX_DATA		*startp;
	EXIT_DATA			*exitp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( depth < 0 )
	{
		thru_doors = TRUE;
		depth = -depth;
	}

	else
		thru_doors = FALSE;

	startp = get_room_index( in_room_vnum );

	init_hash_table( &x_room, sizeof(int), 2048 );
	hash_enter( &x_room, in_room_vnum, (void *) -1 );

	/* initialize queue */
	q_head = (struct room_q *) malloc( sizeof(struct room_q) );
	q_tail = q_head;
	q_tail->room_nr = in_room_vnum;
	q_tail->next_q = 0;

	while ( q_head )
	{
		herep = get_room_index( q_head->room_nr );

		/* for each room test all directions */
		if ( herep == NULL )
			fprintf( stderr, "BUG:  Null herep in hunt.c, room #%d", q_head->room_nr );
		if ( herep && (herep->area == startp->area || !in_zone) )
		{
			/* only look in this zone... saves cpu time and makes world safer for players */
			for ( i = 0; i <= 5; i++ )
			{
				exitp = herep->exit[i];
				if ( exit_ok(exitp) && (thru_doors ? GO_OK_SMARTER : GO_OK) )
				{
					/* next room */
					tmp_room = herep->exit[i]->to_room->vnum;
					if ( tmp_room != out_room_vnum )
					{
						/* shall we add room to queue ? count determines total breadth and depth */
						if ( !hash_find(&x_room, tmp_room) && (count < depth) )
						/* && !IS_SET( RM_FLAGS(tmp_room), DEATH ) ) */
						{
							count++;

							/* mark room as visted and put on queue */
							tmp_q = (struct room_q *) malloc( sizeof(struct room_q) );
							tmp_q->room_nr = tmp_room;
							tmp_q->next_q = 0;
							q_tail->next_q = tmp_q;
							q_tail = tmp_q;

							/* ancestor for first layer is the direction */
							hash_enter( &x_room, tmp_room,
								((int) hash_find(&x_room, q_head->room_nr) == -1) ? (void *) (i + 1) :
								hash_find( &x_room, q_head->room_nr) );
						}
					}
					else
					{
						/* have reached our goal so free queue */
						tmp_room = q_head->room_nr;
						for ( ; q_head; q_head = tmp_q )
						{
							tmp_q = q_head->next_q;
							free( q_head );
						}

						/* return direction if first layer */
						if ( (int) hash_find(&x_room, tmp_room) == -1 )
						{
							if ( x_room.buckets )
							{
								/* junk left over from a previous track */
								destroy_hash_table( &x_room, donothing );
							}

							return( i );
						}
						else
						{
							/* else return the ancestor */
							int iroom;
							/*~~*/

							iroom = (int) hash_find( &x_room, tmp_room );
							if ( x_room.buckets )
							{
								/* junk left over from a previous track */
								destroy_hash_table( &x_room, donothing );
							}

							return( -1 + iroom );
						}
					}
				}
			}
		}

		/* free queue head and point to next entry */
		tmp_q = q_head->next_q;
		free( q_head );
		q_head = tmp_q;
	}

	/* couldn't find path */
	if ( x_room.buckets )
	{
		/* junk left over from a previous track */
		destroy_hash_table( &x_room, donothing );
	}

	return( -1 );
}


void do_hunt( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MSL];
	CHAR_DATA	*victim;
	int			direction, i;
	bool		fArea, ok;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_hunt, 0, 0, NULL) )
		return;

	one_argument( argument, arg );

	if ( !arg || arg[0] == '\0' )
	{
		send_to_char( "Chi stai cercando di cacciare?\n\r", ch );
		return;
	}

	/* fArea = ( get_trust(ch) < MAX_LEVEL ); */
	fArea = !( IS_ADMIN(ch) );

	if ( number_percent() < get_skill(ch, gsn_world_find) )
	{
		fArea = 0;
		check_improve( ch, gsn_world_find, TRUE, 1 );
	}
	else
		check_improve( ch, gsn_world_find, FALSE, 1 );

	if ( fArea )
		victim = get_char_area( ch, arg );
	else
		victim = get_char_world( ch, arg );

	if ( victim == NULL )
	{
		send_to_char( "Nessuno in giro con tale nome.\n\r", ch );
		return;
	}

	if ( ch->in_room == victim->in_room )
	{
		act( "$N si trova qui!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	act( "$n scruta concentrato il terreno.", ch, NULL, NULL, TO_ROOM, POS_REST );

	WAIT_STATE( ch, skill_table[gsn_hunt].beats );
	direction = find_path( ch->in_room->vnum, victim->in_room->vnum, ch, -40000, fArea );

	if ( direction == -1 )
	{
		act( "Non puoi trovare una pista per giungere a $N da qui.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( direction < 0 || direction > MAX_DIR-1 )
	{
		send_to_char( "Hmm.. Qualcosa sembra essere sbagliato.\n\r", ch );
		return;
	}

	/* Give a random direction if the player misses the die roll. */
	if ( IS_MOB(ch) && number_percent() > 75 )	/* NPC @ 25% */
	{
		log_string( "Do PC hunt" );
		ok = FALSE;
		for ( i = 0; i < 6; i++ )
		{
			if ( ch->in_room->exit[direction] != NULL )
			{
				ok = TRUE;
				break;
			}
		}

		if ( ok )
		{
			do
			{
				direction = number_door();
			} while ( (ch->in_room->exit[direction] == NULL) || (ch->in_room->exit[direction]->to_room == NULL) );
		}
		else
		{
			log_string( "do_hunt, nessuna uscita dalla stanza!" );
			ch->hunting = NULL;
			send_to_char( "Quest o luogo non ha uscita!!\n\r", ch );
			return;
		}

		/* Display the results of the search. */
	}

	sprintf( buf, "$N si trova a %s da qui.", dir_name[direction] );
	act( buf, ch, NULL, victim, TO_CHAR, POS_REST );
}


/*
 * Revised by Chronos.
 */
void hunt_victim( CHAR_DATA *ch )
{
	int			dir;
	bool		found;
	CHAR_DATA	*tmp;
	char		tBuf[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Make sure the victim still exists. */
	for ( found = 0, tmp = char_list; tmp && !found; tmp = tmp->next )
	{
		if ( ch->hunting == tmp )
			found = 1;
	}

	if ( !found || !can_see(ch, ch->hunting) )
	{
		if ( get_char_area(ch, ch->hunting->name) != NULL )
		{
			sprintf( tBuf, "portal %s", ch->hunting->name );
			log_string( "mob portal" );
			do_cast( ch, tBuf );
			log_string( "do_enter1" );
			do_enter( ch, "portal" );
			if ( ch->in_room == NULL || ch->hunting == NULL )
				return;
			if ( ch->in_room == ch->hunting->in_room )
			{
				act( "$n guarda con ferocia $N e sibila 'Devi MORIRE!'", ch, NULL, ch->hunting, TO_NOTVICT, POS_REST );
				act( "$n ti guarda con ferocia e sibila 'Devi MORIRE!'", ch, NULL, ch->hunting, TO_VICT, POS_REST );
				act( "Guardi con ferocia $N e sibili 'Devi MORIRE!", ch, NULL, ch->hunting, TO_CHAR, POS_REST );
				multi_hit( ch, ch->hunting, TYPE_UNDEFINED );
				ch->hunting = NULL; /* No more hunting, now tracking */
				return;
			}

			log_string( "done1" );
			return;
		}
		else
		{
			do_say( ch, "Ahhh! La mia preda e' sfuggita!" );
			ch->hunting = NULL;
			return;
		}
	}	/* end if !found or !can_see */

	dir = find_path( ch->in_room->vnum, ch->hunting->in_room->vnum, ch, -40000, TRUE );

	if ( dir < 0 || dir > MAX_DIR-1 )
	{
		/* 1 */
		if ( get_char_area(ch, ch->hunting->name) != NULL && ch->level > 35 )
		{
			sprintf( tBuf, "portal %s", ch->hunting->name );
			log_string( "mob portal" );
			do_cast( ch, tBuf );
			log_string( "do_enter2" );
			do_enter( ch, "portal" );
			if ( ch->in_room == NULL || ch->hunting == NULL )
				return;
			if ( ch->in_room == ch->hunting->in_room )
			{
				act( "$n guarda con ferocia $N e sibila 'Devi MORIRE!'", ch, NULL, ch->hunting, TO_NOTVICT, POS_REST );
				act( "$n ti guarda con ferocia e sibila 'Devi MORIRE!'", ch, NULL, ch->hunting, TO_VICT, POS_REST );
				act( "Guardi con ferocia $N e sibili 'Devi MORIRE!", ch, NULL, ch->hunting, TO_CHAR, POS_REST );
				multi_hit( ch, ch->hunting, TYPE_UNDEFINED );
				ch->hunting = NULL;
				return;
			}

			log_string( "done2" );
			return;
		}
		else
		{
			act( "$n borbotta 'Ho perso la mia preda!'", ch, NULL, ch->hunting, TO_ROOM, POS_REST );
			ch->hunting = NULL;
			return;
		}
	}	/* if dir < 0 or > MAX_DIR-1 */

	if ( ch->in_room->exit[dir] && IS_SET(ch->in_room->exit[dir]->exit_info, EX_CLOSED) )
	{
		do_open( ch, (char *) dir_name[dir] );
		return;
	}

	if ( !ch->in_room->exit[dir] )
	{
		log_string( "BUG:  hunt through null door" );
		ch->hunting = NULL;
		return;
	}

	move_char( ch, dir, FALSE );
	if ( ch->in_room == NULL || ch->hunting == NULL )
		return;
	if ( ch->in_room == ch->hunting->in_room )
	{
		act( "$n guarda con ferocia $N e sibila 'Devi MORIRE!'", ch, NULL, ch->hunting, TO_NOTVICT, POS_REST );
		act( "$n ti guarda con ferocia e sibila 'Devi MORIRE!'", ch, NULL, ch->hunting, TO_VICT, POS_REST );
		act( "Guardi con ferocia $N e sibili 'Devi MORIRE!", ch, NULL, ch->hunting, TO_CHAR, POS_REST );
		multi_hit( ch, ch->hunting, TYPE_UNDEFINED );
		ch->hunting = NULL;
		return;
	}
}


/* repair.c */
void damage_to_obj( CHAR_DATA *ch, OBJ_DATA *wield, OBJ_DATA *worn, int obj_damage );


void damage_to_obj( CHAR_DATA *ch, OBJ_DATA *wield, OBJ_DATA *worn, int obj_damage )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( obj_damage == 0 )
		return;
	worn->condition -= obj_damage;

	act( "$C$p danneggia $P.$c", ch, wield, worn, TO_ROOM, POS_REST, GREEN );

	if ( worn->condition < 1 )
	{
		act( "$C$P cade a pezzi.$c", ch, wield, worn, TO_ROOM, POS_REST, WHITE );
		extract_obj( worn );
		return;
	}

	if ( (IS_SET(wield->extra_flags, ITEM_ANTI_EVIL) && IS_SET(wield->extra_flags, ITEM_ANTI_NEUTRAL))
	&&	 (IS_SET(worn->extra_flags, ITEM_ANTI_EVIL) && IS_SET(worn->extra_flags, ITEM_ANTI_NEUTRAL)) )
	{
		sprintf( buf, "$C$p non vuole combattere contro $P.$c" );
		act( buf, ch, wield, worn, TO_ROOM, POS_REST, GREEN );
		sprintf( buf, "$C$p comincia a bruciare e sei costretto a toglierlo!$c." );
		act( buf, ch, wield, worn, TO_CHAR, POS_REST, GREEN );
		sprintf( buf, "$C$n rimuove da se' $p$c." );
		act( buf, ch, wield, worn, TO_ROOM, POS_REST, GREEN );
		unequip_char( ch, wield );
		return;
	}

	if ( IS_SET(wield->extra_flags, ITEM_ANTI_EVIL) && IS_SET(worn->extra_flags, ITEM_ANTI_EVIL) )
	{
		sprintf( buf, "$C$p e' dispiaciut$x per il danno inferto a $P.$c" );
		act( buf, ch, wield, worn, TO_ROOM, POS_REST, GREEN );
		return;
	}
}


void check_weapon_destroy( CHAR_DATA *ch, CHAR_DATA *victim, bool second )
{
	OBJ_DATA	*wield, *destroy;
	int			skill, chance = 0, sn, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) || number_percent() < 94 )
		return;

	if ( (wield = get_wield_char(ch, second)) == NULL )
		return;
	sn = get_weapon_sn( ch, second );
	skill = get_skill( ch, sn );

	if ( is_metal(wield) )
	{
		for ( i = 0; i < MAX_WEAR; i++ )

		{
			if ((destroy = get_eq_char(victim, i)) == NULL
			||	number_percent() > 95
			||	number_percent() > 94
			||	number_percent() > skill
			||	ch->level < (victim->level - 10)
			||	check_material(destroy, "platinum")
			||	destroy->pIndexData->limit != -1
			||	(i == WEAR_LEFT || i == WEAR_RIGHT || i == WEAR_BOTH || i == WEAR_TATTOO || i == WEAR_STUCK_IN) )
				continue;

			chance += 20;
			if ( check_material(wield, "platinium") || check_material(wield, "titanium") )
				chance += 5;


			if ( is_metal(destroy) )
				chance -= 20;
			else
				chance += 20;

			chance += ( (ch->level - victim->level) / 5 );

			chance += ( (wield->level - destroy->level) / 2 );

			/* sharpness */
			if ( IS_WEAPON_STAT(wield, WEAPON_SHARP) )
				chance += 10;

			if ( sn == gsn_axe )
				chance += 10;

			/* spell affects */
			if ( IS_OBJ_STAT(destroy, ITEM_BLESS) )
				chance -= 10;
			if ( IS_OBJ_STAT(destroy, ITEM_MAGIC) )
				chance -= 20;

			chance += skill - 85;
			chance += get_curr_stat( ch, STR );

			/* chance /= 2; */
			if ( number_percent() < chance && chance > 50 )
			{
				damage_to_obj( ch, wield, destroy, (chance / 5) );
				break;
			}
		}
	}
	else
	{
		for ( i = 0; i < MAX_WEAR; i++ )
		{
			if ( (destroy = get_eq_char(victim, i)) == NULL
			||	number_percent() > 95
			||	number_percent() > 94
			||	number_percent() < skill
			||	ch->level < (victim->level - 10)
			||	check_material(destroy, "platinum")
			||	destroy->pIndexData->limit != -1
			||	(i == WEAR_LEFT || i == WEAR_RIGHT || i == WEAR_BOTH || i == WEAR_TATTOO || i == WEAR_STUCK_IN) )
				continue;

			chance += 10;

			if ( is_metal(destroy) )
				chance -= 20;

			chance += ( ch->level - victim->level );

			chance += ( wield->level - destroy->level );

			/* sharpness */
			if ( IS_WEAPON_STAT(wield, WEAPON_SHARP) )
				chance += 10;

			if ( sn == gsn_axe )
				chance += 10;

			/* spell affects */
			if ( IS_OBJ_STAT(destroy, ITEM_BLESS) )
				chance -= 10;
			if ( IS_OBJ_STAT(destroy, ITEM_MAGIC) )
				chance -= 20;

			chance += skill - 85;
			chance += get_curr_stat( ch, STR );

			/* chance /= 2; */
			if ( number_percent() < chance && chance > 50 )
			{
				damage_to_obj( ch, wield, destroy, chance / 5 );
				break;
			}
		}
	}
}


void do_repair( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*mob;
	char		arg[MIL];
	char		buf[MIL];
	OBJ_DATA	*obj;
	int			cost;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
		if ( !IS_MOB(mob) )
			continue;
		if ( mob->spec_fun == spec_lookup("spec_repairman")
		||	 mob->spec_fun == spec_lookup("spec_repair_cloth") )
			break;
	}

	if ( mob == NULL )
	{
		send_to_char( "Non puoi chiedere di farlo a nessuno qui.\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		do_say( mob, "Posso riparare un'arma, ma ad un prezzo." );
		send_to_char( "digita stima <arma> per conoscere la cifra che ti chiedera'.\n\r", ch );
		return;

	}

	if ( (obj = get_obj_carry(ch, arg)) == NULL )
	{
		do_say( mob, "Non hai quell'oggetto." );
		return;
	}

	if ( mob->spec_fun == spec_lookup("spec_repairman")	&& !is_metal(obj) )
	{
		sprintf( buf, "Non posso riparare %s non e' metallo.\n\r", obj->short_descr );
		do_say( mob, buf );
		return;
	}

	if ( mob->spec_fun == spec_lookup("spec_repair_cloth") && is_metal(obj) )
	{
		sprintf( buf, "Non posso riparare %s e' metallico.\n\r", obj->short_descr );
		do_say( mob, buf );
		return;
	}

	if ( mob->spec_fun == spec_lookup("spec_repairman") && obj->pIndexData->vnum == OBJ_VNUM_HAMMER )
	{
		if ( mob->race == 6 ) /* dwarf */
			do_say( mob, "Eh eh eh.. Questo martello lo potrei riparare anche ad occhi chiusi." );
		else
		{
			do_say( mob, "Questo martello e' praticamente impossibile da riparare." );
			return;
		}
	}

	if ( obj->condition >= 100 )
	{
		do_say( mob, "Questo oggetto non e' danneggiato." );
		return;
	}

	if ( obj->cost == 0 )
	{
		sprintf( buf, "E' inutile riparare %s non vale nulla.\n\r", obj->short_descr );
		do_say( mob, buf );
		return;
	}

	cost = ( (obj->level * 10) + ((obj->cost * (100 - obj->condition)) / 100) );
	cost /= 100;

	if ( cost > ch->gold )
	{
		do_say( mob, "Non hai abbastanza denaro per il mio lavoro." );
		return;
	}

	WAIT_STATE( ch, PULSE_VIOLENCE );

	ch->gold -= cost;
	mob->gold += cost;
	sprintf( buf, "$N prende %s da $n, lo ripara con abilita', e glielo riconsegna.", obj->short_descr );
	act( buf, ch, NULL, mob, TO_ROOM, POS_REST );
	sprintf( buf, "%s prende %s, lo ripara con abilita', e te lo riconsegna.\n\r", mob->short_descr, obj->short_descr );
	send_to_char( buf, ch );
	obj->condition = 100;
}


void do_estimate( CHAR_DATA *ch, char *argument )
{
	char		buf[MIL];
	OBJ_DATA	*obj;
	CHAR_DATA	*mob;
	char		arg[MIL];
	int			cost;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
		if ( !IS_MOB(mob) )
			continue;
		if ( mob->spec_fun == spec_lookup("spec_repairman")
		||	 mob->spec_fun == spec_lookup("spec_repair_cloth") )
			break;
	}

	if ( mob == NULL )
	{
		send_to_char( "Non puoi chiedere di farlo a nessuno qui.\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		do_say( mob, "prova con: stima <oggetto>" );
		return;
	}

	if ( (obj = (get_obj_carry(ch, arg))) == NULL )
	{
		do_say( mob, "Non hai quell'oggetto con te." );
		return;
	}

	if ( mob->spec_fun == spec_lookup("spec_repairman") && !is_metal(obj) )
	{
		sprintf( buf, "Non sono capace di stimare %s, non e' metallo.\n\r", obj->short_descr );
		do_say( mob, buf );
		return;
	}

	if ( mob->spec_fun == spec_lookup("spec_repair_cloth") && is_metal(obj) )
	{
		sprintf( buf, "Non sono capace di stimare %s, e' metallico.\n\r", obj->short_descr );
		do_say( mob, buf );
		return;
	}

	if ( mob->spec_fun == spec_lookup("spec_repairman") && obj->pIndexData->vnum == OBJ_VNUM_HAMMER )
	{
		if ( mob->race == 6 )	/* dwarf */
			do_say( mob, "Eh eh eh.. Questo martello lo potrei riconoscere anche nella miniera piu' profonda." );
		else
		{
			do_say( mob, "Questo martello ha una fattura al di la' delle mie possibili stime." );
			return;
		}
	}

	if ( obj->condition >= 100 )
	{
		do_say( mob, "Ma questo oggetto e' rotto!" );
		return;
	}

	if ( obj->cost == 0 )
	{
		do_say( mob, "Mi e' impossibile riparare questo oggetto." );
		return;
	}

	cost = ( (obj->level * 10) + ((obj->cost * (100 - obj->condition)) / 100) );
	cost /= 100;

	sprintf( buf, "Ti costera' %d monet%c per metterlo in sesto, va bene?", cost, (cost == 1) ? 'a' : 'e' );
	do_say( mob, buf );
}


void do_restring( CHAR_DATA *ch, char *argument )
{
	char		buf[MIL];
	char		arg[MIL];
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*mob;
	OBJ_DATA	*obj;
	int			cost = 2000;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
		if ( IS_MOB(mob) && IS_SET(mob->act, ACT_IS_HEALER) )
			break;
	}

	if ( mob == NULL )
	{
		send_to_char( "Non puoi farlo qui.\n\r", ch );
		return;
	}

	smash_tilde( argument );
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg1 );
	strcpy( arg2, argument );

	if ( arg[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "\n\rSintassi:\n\r", ch );
		send_to_char( "  restring <ogg> <campo> <stringa>\n\r", ch );
		send_to_char( "  campo puo' essere: name short long\n\r", ch );
		return;
	}

	if ( (obj = (get_obj_carry(ch, arg))) == NULL )
	{
		send_to_char( "Stringer dice '`sNon hai questo oggetto``.'\n\r", ch );
		return;
	}

	cost += ( obj->level * 1500 );

	if ( cost > ch->gold )
	{
		act( "$N dice 'Non hai abbastanza moneta per i miei servigi.'", ch, NULL, mob, TO_CHAR, POS_REST );
		return;
	}

	if ( !str_prefix(arg1, "name") )
	{
		free_string( obj->name );
		obj->name = str_dup( arg2 );
	}
	else if ( !str_prefix(arg1, "short") )
	{
		free_string( obj->short_descr );
		obj->short_descr = str_dup( arg2 );
	}
	else if ( !str_prefix(arg1, "long") )
	{
		free_string( obj->description );
		obj->description = str_dup( arg2 );
	}
	else
	{
		send_to_char( "Non e' un campo valido.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, PULSE_VIOLENCE );

	ch->gold -= cost;
	mob->gold += cost;
	sprintf( buf, "$N prende l'oggetto di $n', lo lavora abilmente, e glielo' rida'." );
	act( buf, ch, NULL, mob, TO_ROOM, POS_REST );
	sprintf( buf, "%s prende %s, lo lavora abilmente, e te lo rida'.\n\r", mob->short_descr, obj->short_descr );
	send_to_char( buf, ch );
	send_to_char( "Ricordati, se troveremo che l'oggetto sia offensivo, non ne saremo contenti.\n\r", ch );
	send_to_char( " Questo e' UN SOLO E IL SOLO avvertimento.\n\r", ch );
}


void check_shield_destroyed( CHAR_DATA *ch, CHAR_DATA *victim, bool second )
{
	OBJ_DATA	*wield, *destroy;
	int			skill, chance = 0, sn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) || number_percent() < 94 )
		return;

	if ( (wield = get_wield_char(ch, second)) == NULL )
		return;
	sn = get_weapon_sn( ch, second );
	skill = get_skill( ch, sn );

	destroy = get_shield_char( victim );

	if ( destroy == NULL )
		return;

	if ( is_metal(wield) )
	{
		if ( number_percent() > 94
		||	 number_percent() > skill
		||	ch->level < (victim->level - 10)
		||	check_material(destroy, "platinum")
		||	destroy->pIndexData->limit != -1 )
			return;

		chance += 20;
		if ( check_material(wield, "platinium") || check_material(wield, "titanium") )
			chance += 5;

		if ( is_metal(destroy) )
			chance -= 20;
		else
			chance += 20;

		chance += ( (ch->level - victim->level) / 5 );

		chance += ( (wield->level - destroy->level) / 2 );

		/* sharpness */
		if ( IS_WEAPON_STAT(wield, WEAPON_SHARP) )
			chance += 10;

		if ( sn == gsn_axe )
			chance += 10;

		/* spell affects */
		if ( IS_OBJ_STAT(destroy, ITEM_BLESS) )
			chance -= 10;
		if ( IS_OBJ_STAT(destroy, ITEM_MAGIC) )
			chance -= 20;

		chance += skill - 85;
		chance += get_curr_stat( ch, STR );

		/* chance /= 2; */
		if ( number_percent() < chance && chance > 20 )
		{
			damage_to_obj( ch, wield, destroy, (chance / 4) );
			return;
		}
	}
	else
	{
		if ( number_percent() > 94
		||	 number_percent() < skill
		||	 ch->level < (victim->level - 10)
		||	 check_material(destroy, "platinum")
		||	 destroy->pIndexData->limit != -1 )
			return;

		chance += 10;

		if ( is_metal(destroy) )
			chance -= 20;

		chance += ( ch->level - victim->level );

		chance += ( wield->level - destroy->level );

		/* sharpness */
		if ( IS_WEAPON_STAT(wield, WEAPON_SHARP) )
			chance += 10;

		if ( sn == gsn_axe )
			chance += 10;

		/* spell affects */
		if ( IS_OBJ_STAT(destroy, ITEM_BLESS) )
			chance -= 10;
		if ( IS_OBJ_STAT(destroy, ITEM_MAGIC) )
			chance -= 20;

		chance += skill - 85;
		chance += get_curr_stat( ch, STR );

		/* chance /= 2; */
		if ( number_percent() < chance && chance > 20 )
		{
			damage_to_obj( ch, wield, destroy, (chance / 4) );
			return;
		}
	}
}


void check_weapon_destroyed( CHAR_DATA *ch, CHAR_DATA *victim, bool second )
{
	OBJ_DATA	*wield, *destroy;
	int			skill, chance = 0, sn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(victim) || number_percent() < 94 )
		return;

	if ( (wield = get_wield_char(ch, second)) == NULL )
		return;
	sn = get_weapon_sn( ch, second );
	skill = get_skill( ch, sn );

	destroy = get_wield_char( victim, FALSE );
	if ( destroy == NULL )
		return;

	if ( is_metal(wield) )
	{
		if ( number_percent() > 94
		||	 number_percent() > skill
		||	 ch->level < (victim->level - 10)
		||	 check_material(destroy, "platinum")
		||	 destroy->pIndexData->limit != -1 )
			return;

		chance += 20;
		if ( check_material(wield, "platinium") || check_material(wield, "titanium") )
			chance += 5;

		if ( is_metal(destroy) )
			chance -= 20;
		else
			chance += 20;

		chance += ( (ch->level - victim->level) / 5 );

		chance += ( (wield->level - destroy->level) / 2 );

		/* sharpness */
		if ( IS_WEAPON_STAT(wield, WEAPON_SHARP) )
			chance += 10;

		if ( sn == gsn_axe )
			chance += 10;

		/* spell affects */
		if ( IS_OBJ_STAT(destroy, ITEM_BLESS) )
			chance -= 10;
		if ( IS_OBJ_STAT(destroy, ITEM_MAGIC) )
			chance -= 20;

		chance += skill - 85;
		chance += get_curr_stat( ch, STR );

		/* chance /= 2; */
		if ( number_percent() < (chance / 2) && chance > 20 )
		{
			damage_to_obj( ch, wield, destroy, (chance / 4) );
			return;
		}
	}
	else
	{
		if ( number_percent() > 94
		||	 number_percent() < skill
		||	 ch->level < (victim->level - 10)
		||	 check_material(destroy, "platinum")
		||	 destroy->pIndexData->limit != -1 )
			return;

		chance += 10;

		if ( is_metal(destroy) )
			chance -= 20;

		chance += ( ch->level - victim->level );

		chance += ( wield->level - destroy->level );

		/* sharpness */
		if ( IS_WEAPON_STAT(wield, WEAPON_SHARP) )
			chance += 10;

		if ( sn == gsn_axe )
			chance += 10;

		/* spell affects */
		if ( IS_OBJ_STAT(destroy, ITEM_BLESS) )
			chance -= 10;
		if ( IS_OBJ_STAT(destroy, ITEM_MAGIC) )
			chance -= 20;

		chance += skill - 85;
		chance += get_curr_stat( ch, STR );

		/* chance /= 2; */
		if ( number_percent() < (chance / 2) && chance > 20 )
		{
			damage_to_obj( ch, wield, destroy, chance / 4 );
			return;
		}
	}
}


void do_smithing( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		buf[MIL];
	OBJ_DATA	*obj;
	OBJ_DATA	*hammer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_smithing, 0, 0, NULL) )
		return;

	if ( ch->fighting )
	{
		send_to_char( "Attendi che il combattimento finisca.\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Quale oggetto vorresti riparare?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_carry(ch, arg)) == NULL )
	{
		send_to_char( "Non porti nessuno oggetto di tal genere mi pare.\n\r", ch );
		return;
	}

	if ( obj->condition >= 100 )
	{
		send_to_char( "Ma questo oggetto non e' danneggiato.\n\r", ch );
		return;
	}

	if ( (hammer = get_hold_char(ch)) == NULL )
	{
		send_to_char( "Non stai impugnando nessun oggetto.\n\r", ch );
		return;
	}

	if ( hammer->pIndexData->vnum != OBJ_VNUM_HAMMER )
	{
		send_to_char( "Non e' il corretto martello da utilizzare.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	if ( number_percent() > get_skill(ch, gsn_smithing) )
	{
		check_improve( ch, gsn_smithing, FALSE, 8 );
		sprintf( buf, "$n prova a riparare %s con il martello. Ma fallisce.", obj->short_descr );
		act( buf, ch, NULL, obj, TO_ROOM, POS_REST );
		sprintf( buf, "Fallisci a riparare %s\n\r", obj->short_descr );
		send_to_char( buf, ch );
		hammer->condition -= 25;
	}
	else
	{
		check_improve( ch, gsn_smithing, TRUE, 4 );
		sprintf( buf, "$n ripara %s con il martello.", obj->short_descr );
		act( buf, ch, NULL, NULL, TO_ROOM, POS_REST );
		sprintf( buf, "Ripari %s\n\r", obj->short_descr );
		send_to_char( buf, ch );
		obj->condition = UMAX( 100, obj->condition + (get_skill(ch, gsn_smithing) / 2) );
		hammer->condition -= 15;
	}

	if ( hammer->condition < 1 )
		extract_obj( hammer );
}


/* auction.c */
/*
 * This snippet was orginally written by Erwin S. Andreasen. erwin@pip.dknet.dk, http://pip.dknet.dk/~pip1773/
 * Adopted to Anatolia MUD by chronos.
 */
DECLARE_SPELL_FUN( spell_identify );


void talk_auction( const char *argument, CHAR_DATA *ch, const char *color1, const char *color2 )
{
	DESCRIPTOR_DATA *d;
	char			buf[MSL];
	CHAR_DATA		*original;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Asta: %s%s%s",
		IS_SET(ch->act,PLR_COLOR) ? color1 : "",
		argument,
		IS_SET(ch->act,PLR_COLOR) ? color2 : "" );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		original = d->original ? d->original : d->character;	/* if switched */

		if ( d->connected == CON_PLAYING
		&&	 !IS_SET(d->character->comm, COMM_NOAUCTION)
		&&	 d->character->in_room != NULL
		&&	 d->character->in_room->area == ch->in_room->area
		&&	 !is_affected(d->character, gsn_deafen) )
			act( buf, original, NULL, NULL, TO_CHAR, POS_REST );
	}
}


void auction_update( void )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( auction->item != NULL )
	{
		if ( --auction->pulse <= 0 )						/* decrease pulse */
		{
			auction->pulse = PULSE_AUCTION;
			switch ( ++auction->going )						/* increase the going state */
			{
			case 1:											/* prima volta */
			case 2:											/* seconda volta */
			case 3:											/* terza volta */
				if ( auction->bet > 0 )
				{
					sprintf( buf, "%s: sta per essere venduto per %d, e %s.",
						auction->item->short_descr, auction->bet,
						(auction->going == 1) ? "uno!" : (auction->going == 2) ? "due!!" : "tre!!!" );
				}
				else
				{
					sprintf( buf, "%s: non ha ricevuto offerte, e %s.",
						auction->item->short_descr, (auction->going == 1) ? "uno!" : (auction->going == 2) ? "due!!" : "tre!!!" );
				}

				talk_auction( buf, auction->seller, CYAN, WHITE_BOLD );
				break;

			case 4:											/* VENDUTO! */
				if ( auction->bet > 0 )
				{
					sprintf( buf, "%s venduto a %s per %d.",
						auction->item->short_descr,
						IS_MOB(auction->buyer) ? auction->buyer->short_descr : auction->buyer->name,
						auction->bet );

					talk_auction( buf, auction->seller, CYAN, WHITE_BOLD );
					obj_to_char( auction->item, auction->buyer );
					act( "Un corriere addetto, cavalcando un pegaso, giunge a te prendendoti $p.",
						auction->buyer, auction->item, NULL, TO_CHAR, POS_REST );
					act( "Un corriere addetto, cavalcando un pegaso, giunge a te dandoti $p.",
						auction->buyer, auction->item, NULL, TO_ROOM, POS_REST );

					auction->seller->gold += auction->bet;	/* give him the money */

					auction->item = NULL;					/* reset item */
				}
				else						/* not sold */
				{
					sprintf( buf, "Nessuna offerta ricevuta per %s.", auction->item->short_descr );
					talk_auction( buf, auction->seller, CYAN, WHITE_BOLD );
					sprintf( buf, "Oggetto non venduto. L'asta e' chiusa." );
					talk_auction( buf, auction->seller, RED, WHITE_BOLD );
					obj_to_char( auction->item, auction->seller );
					auction->item = NULL;	/* clear auction */
				}	/* else */
			}		/* switch */
		}			/* if */
	}
}					/* func */


void do_auction( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	char		arg1[MIL];
	char		buf[MSL];
	char		bufc[MSL];
	char		betbuf[MSL];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );

	if ( IS_MOB(ch) )						/* NPC extracted can't auction! */
		return;

	if ( IS_SET(ch->comm, COMM_NOAUCTION) )
	{
		if ( !str_cmp(arg1, "on") || !str_cmp(arg1, "si") )
		{
			send_to_char( "Il canale delle aste e' ora attivo.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_NOAUCTION );
			return;
		}
		else
		{
			send_to_char( "Il canale delle aste e' disattivo.\n\r", ch );
			send_to_char( "Devi prima attivare il canale delle aste.\n\r", ch );
			return;
		}
	}

	if ( arg1[0] == '\0' )
	{
		if ( auction->item != NULL )
		{
			/* show item data here */
			if ( auction->bet > 0 )
				sprintf( buf, "L'offerta corrente per questo oggetto ammonta ad un valore di %d d'oro.\n\r", auction->bet );
			else
				sprintf( buf, "Nessuna offerta ricevuta per questo oggetto.\n\r" );

			sprintf( bufc, "%s%s%s", CLR(ch, GREEN), buf, CLR(ch, WHITE_BOLD) );
			send_to_char( bufc, ch );
			spell_identify( 0, 0, ch, auction->item, 0 );
			return;
		}
		else
		{
			sprintf( bufc, "%sAstare cosa?%s\n\r", CLR(ch, RED), CLR(ch, WHITE_BOLD) );
			send_to_char( bufc, ch );
			return;
		}
	}

	if ( !str_cmp(arg1, "off") || !str_cmp(arg1, "no") )
	{
		send_to_char( "Il canale delle aste e' ora disattivo.\n\r", ch );
		SET_BIT( ch->comm, COMM_NOAUCTION );
		return;
	}

	if ( IS_ADMIN(ch) && (!str_cmp(arg1, "stop") || !str_cmp(arg1, "ferma")) )
	{
		if ( auction->item == NULL )
		{
			send_to_char( "Non vi sono aste da fermare.\n\r", ch );
			return;
		}
		else
		{									/* stop the auction */
			sprintf( buf, "La vendita di %s e' stata fermata dagli dei e l'oggetto confiscato.", auction->item->short_descr );
			talk_auction( buf, auction->seller, WHITE, WHITE_BOLD );
			obj_to_char( auction->item, auction->seller );
			auction->item = NULL;
			if ( auction->buyer != NULL )
			{								/* return money to the buyer */
				auction->buyer->gold += auction->bet;
				send_to_char( "Le tue monete ti vengono restituite.\n\r", auction->buyer );
			}

			return;
		}
	}

	if ( !str_cmp(arg1, "bet") || !str_cmp(arg1, "offri") )
	{
		if ( auction->item != NULL )
		{
			int newbet;
			/*~~~~~~~*/

			if ( ch == auction->seller )
			{
				send_to_char( "Non puoi comperare cio' che e' tuo..\n\r", ch );
				return;
			}

			/* make - perhaps - a bet now */
			if ( argument[0] == '\0' )
			{
				send_to_char( "Quante monete d'oro?\n\r", ch );
				return;
			}

			newbet = (int) atoi( argument );
			sprintf( betbuf, "Offerta: %d\n\r", newbet );

			if ( newbet < (auction->bet + 1) )
			{
				send_to_char( "Devi offrire almeno una moneta d'oro in piu' della offerta corrente.\n\r", ch );
				return;
			}

			if ( newbet > ch->gold )
			{
				send_to_char( "Non hai con te cosi' tanta moneta!\n\r", ch );
				return;
			}

			/* the actual bet is OK! */
			/* return the gold to the last buyer, if one exists */
			if ( auction->buyer != NULL )
				auction->buyer->gold += auction->bet;

			ch->gold -= newbet;				/* substract the gold - important :) */
			auction->buyer = ch;
			auction->bet = newbet;
			auction->going = 0;
			auction->pulse = PULSE_AUCTION; /* start the auction over again */

			sprintf( buf, "Offerta su %s del valore totale di %d.\n\r", auction->item->short_descr, newbet );
			talk_auction( buf, auction->seller, MAGENTA, WHITE_BOLD );
			return;
		}
		else
		{
			send_to_char( "Non c'e' nulla da comprare ora.\n\r", ch );
			return;
		}
	}

	/* finally... */
	obj = get_obj_carry( ch, arg1 );		/* does char have the item ? */

	if ( obj == NULL )
	{
		send_to_char( "Non lo hai.\n\r", ch );
		return;
	}

	if ( obj->pIndexData->vnum < 100 )
	{
		send_to_char( "Non puoi astare questo oggetto.\n\r", ch );
		return;
	}

	for ( i = 1; i < MAX_CABAL; i++ )
	{
		if ( obj->pIndexData->vnum == cabal_table[i].obj_vnum )
		{
			send_to_char( "Gli Dei si infurierebbero se questo oggetto fosse messo all'asta.\n\r", ch );
			return;
		}
	}

	if ( auction->item == NULL )
	{
		switch ( obj->item_type )
		{
		default:
			act( "$CNon puoi mettere all'asta $T.$c",
				ch, NULL, item_type_name(obj), TO_CHAR, POS_SLEEP, RED );
			return;

		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_STAFF:
		case ITEM_WAND:
		case ITEM_SCROLL:
			obj_from_char( obj );
			auction->item = obj;
			auction->bet = 0;				/* obj->cost / 100 */
			auction->buyer = NULL;
			auction->seller = ch;
			auction->pulse = PULSE_AUCTION;
			auction->going = 0;

			sprintf( buf, "Un nuovo oggetto messo all'asta: %s.", obj->short_descr );
			talk_auction( buf, auction->seller, CLR(ch, YELLOW), CLR(ch, WHITE_BOLD) );

			return;
		}	/* switch */
	}
	else
	{
		act( "Riprova piu' tardi - $p e' all'asta ora!", ch, auction->item, NULL, TO_CHAR, POS_REST );
		return;
	}
}
