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
#ifdef MXP
#	include <ctype.h>
#endif

#include "xandra.h"
#include "interp.h"
#include "magic.h"


/*
 * Local functions.
 */
bool		remove_obj_loc	( CHAR_DATA *ch, int iWear, bool fReplace );
CHAR_DATA	*find_keeper 	( CHAR_DATA *ch );
int			get_cost		( CHAR_DATA *keeper, OBJ_DATA * obj, bool fBuy );
void		obj_to_keeper	( OBJ_DATA *obj, CHAR_DATA * ch );
OBJ_DATA	*get_obj_keeper ( CHAR_DATA *ch, CHAR_DATA * keeper, char *argument );
void		hold_a_light	( CHAR_DATA *ch, OBJ_DATA * obj, int iWear );
void		hold_a_shield	( CHAR_DATA *ch, OBJ_DATA * obj, int iWear );
void		hold_a_thing	( CHAR_DATA *ch, OBJ_DATA * obj, int iWear );
void		wear_multi		( CHAR_DATA *ch, OBJ_DATA * obj, int iWear, bool fReplace );
void		wear_a_wield	( CHAR_DATA *ch, OBJ_DATA * obj, bool fReplace );


extern char *const dir_name[];


/*
 * Funzioni esterne
 */
int			WEIGHT_MULT		( OBJ_DATA *obj );	/* handler.c */


/*
 *RT parte di codice riguardante il loot del corpo
 */
bool can_loot( CHAR_DATA *ch, OBJ_DATA *obj )
{
	CHAR_DATA	*owner, *wch;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	return( TRUE );
	if ( IS_ADMIN(ch) )
		return( TRUE );

	if ( !obj->owner || obj->owner == NULL )
		return( TRUE );

	owner = NULL;
	for ( wch = char_list; wch != NULL; wch = wch->next )
		if ( !str_cmp(wch->name, obj->owner) )
			owner = wch;

	if ( owner == NULL )
		return( TRUE );

	if ( !str_cmp(ch->name, owner->name) )
		return( TRUE );

	if ( is_same_group(ch, owner) )
		return( TRUE );

	return( FALSE );
}


void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    /* variabili dell'AUTOSPLIT */
	CHAR_DATA	*gch;
	int			members;
	char		buffer[100];
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( !CAN_WEAR(obj, ITEM_TAKE) )
	{
    	send_to_char( "Non e' raccoglibile.\n\r", ch );
		return;
	}

	if ( (IS_EVIL(ch) && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))
	||	 (IS_GOOD(ch) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))
	||	 (IS_NEUTRAL(ch) && IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL)) )
	{
		ch->point[LIFE] -= ch->level;
		act( "L'aura di $p avvampa sulla tua mano costringendoti a liberartene!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n geme di dolore mentre butta a terra  $p.",  ch, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	/* Controllo per le slay */
	if ( (IS_EVIL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYEVIL))
	||	 (IS_GOOD(ch) && IS_OBJ_STAT(obj, ITEM_SLAYGOOD))
	||	 (IS_NEUTRAL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYNEUTRAL)) )
	{
		ch->point[LIFE] -= ch->level*2;
		act( "L'aura di $p avvampa sulla tua mano costringendoti a liberartene!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n geme di dolore mentre butta a terra  $p.",  ch, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( ch->carry_number + get_obj_number(obj) > can_carry_n(ch) )
	{
		act( "$d: non puoi portare altri oggetti.", ch, NULL, obj->name, TO_CHAR, POS_REST );
		return;
	}

	if ( get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch) )
	{
		act( "$d: sarebbe un peso troppo grande per la tua forza.", ch, NULL, obj->name, TO_CHAR, POS_REST );
		return;
	}

	if ( obj->in_room != NULL )
	{
		for ( gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room )
		{
			if ( gch->on == obj )
			{
				act( "$N sembra che stia utilizzando $p.", ch, obj, gch, TO_CHAR, POS_REST );
				return;
			}
		}
	}

	if ( container != NULL )
	{
		if (container->pIndexData->vnum == OBJ_VNUM_INVADER_SKULL
		||	container->pIndexData->vnum == OBJ_VNUM_RULER_STAND
		||	container->pIndexData->vnum == OBJ_VNUM_ARMY_THRONE
		||	container->pIndexData->vnum == OBJ_VNUM_CHAOS_ALTAR
		||	container->pIndexData->vnum == OBJ_VNUM_YVALADRIN_ALTAR
		||	container->pIndexData->vnum == OBJ_VNUM_KNIGHT_ALTAR
		||	container->pIndexData->vnum == OBJ_VNUM_LIONS_ALTAR
		||	container->pIndexData->vnum == OBJ_VNUM_HUNTER_ALTAR )
		{
			DESCRIPTOR_DATA *d;
			/*~~~~~~~~~~~~~~~*/

			act( "Prendi $p da $P.", ch, obj, container, TO_CHAR, POS_REST );
			if ( !IS_AFFECTED(ch, AFF_SNEAK) )
			act( "$n prende $p da $P.", ch, obj, container, TO_ROOM, POS_REST );
			obj_from_obj( obj );
			act( "$p si oscura per un attimo quindi sparisce nel nulla!", ch,container,NULL,TO_ROOM, POS_REST );
			act( "$p si oscura per un attimo quindi sparisce nel nulla!", ch,container,NULL,TO_CHAR, POS_REST );
			extract_obj( container );
			obj_to_char( obj, ch );

			for ( d = descriptor_list; d; d = d->next )
			{
				if ( d->connected == CON_PLAYING && d->character != NULL
				&& cabal_table[d->character->cabal].obj_ptr == obj )
				{
					act( "$CAvverti un tremito nel cuore.. Senti una parte della tua forza svanire!$c",
						d->character, NULL, NULL, TO_CHAR, POS_DEAD, GREEN );
				}
			}

			if ( IS_SET(obj->progtypes, OPROG_GET) )
				( obj->pIndexData->oprogs->get_prog ) ( obj, ch );
			return;
		}

		if ( container->pIndexData->vnum == OBJ_VNUM_PIT
		&& !CAN_WEAR(container, ITEM_TAKE)
		&& !IS_OBJ_STAT(obj, ITEM_HAD_TIMER) )
			obj->timer = 0;
		act( "Prendi $p da $P.", ch, obj, container, TO_CHAR, POS_REST );
		if ( !IS_AFFECTED(ch, AFF_SNEAK) )
			act( "$n prende $p da $P.", ch, obj, container, TO_ROOM, POS_REST );
		REMOVE_BIT( obj->extra_flags, ITEM_HAD_TIMER );
		obj_from_obj( obj );
	}
	else
	{
		act( "Prendi $p.", ch, obj, container, TO_CHAR, POS_REST );
		if ( !IS_AFFECTED(ch, AFF_SNEAK) )
			act( "$n prende $p.", ch, obj, container, TO_ROOM, POS_REST );
		obj_from_room( obj );
	}

	if ( obj->item_type == ITEM_MONEY )
	{
		ch->silver += obj->value[0];
		ch->gold += obj->value[1];
		if ( IS_SET(ch->act, PLR_AUTOSPLIT) )
		{	/* AUTOSPLIT code */
			members = 0;
			for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
			{
				if ( !IS_AFFECTED(gch, AFF_CHARM) && is_same_group(gch, ch) )
					members++;

			}

			if ( members > 1 && (obj->value[0] > 1 || obj->value[1]) )
			{
				sprintf( buffer, "%d %d", obj->value[0], obj->value[1] );
				do_split( ch, buffer );
			}
		}

		extract_obj( obj );
	}
	else
	{
		obj_to_char( obj, ch );
		if ( IS_SET(obj->progtypes, OPROG_GET) )
			( obj->pIndexData->oprogs->get_prog ) ( obj, ch );
	}
}


void do_get( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	OBJ_DATA	*container;
	bool		found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !str_cmp(arg2,"da") || !str_cmp(arg2, "from") )
		argument = one_argument( argument, arg2 );

	/* Get type. */
	if ( arg1[0] == '\0' )
	{
    send_to_char( "Cosa vuoi prendere?\n\r", ch );
		return;
	}

	if ( is_number(arg1) )
	{
		int amount, weight, gold = 0, silver = 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		amount = atoi( arg1 );
		if ( amount <= 0
		|| (str_cmp(arg2, "coins") && str_cmp(arg2, "monete")
			&& str_cmp(arg2, "coin") && str_cmp(arg2, "moneta")
			&& str_cmp(arg2, "gold") && str_cmp(arg2, "oro")
			&& str_cmp(arg2, "silver") && str_cmp(arg2, "argento")) )
		{
			send_to_char( "Uso: <get> <numero> <argento|oro|coin|coins>\n\r", ch );
			return;
		}

		if ( !str_cmp(arg2, "gold") || !str_cmp(arg2, "oro"))
			weight = amount * 2 / 5;
		else
			weight = amount / 10;

		if ( get_carry_weight(ch) + weight > can_carry_w(ch) )
		{
			act( "Quelle monete pesano decisamente troppo per te.", ch, NULL, NULL, TO_CHAR, POS_REST );
			return;
		}

		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;

			switch ( obj->pIndexData->vnum )
			{
			case OBJ_VNUM_SILVER_ONE:	silver	+= 1; break;
			case OBJ_VNUM_GOLD_ONE:		gold	+= 1; break;
			case OBJ_VNUM_SILVER_SOME:	silver	+= obj->value[0]; break;
			case OBJ_VNUM_GOLD_SOME:	gold	+= obj->value[1]; break;
			case OBJ_VNUM_COINS:		silver	+= obj->value[0]; gold += obj->value[1]; break;
			}
		}

		if ( ((!str_cmp(arg2, "oro") || !str_cmp(arg2, "gold")) && amount > gold)
		|| ((str_cmp(arg2, "argento") || str_cmp(arg2, "silver")) && amount > silver) )
		{
			send_to_char( "Non ci sono cosi' tante monete qui.\n\r", ch );
			return;
		}

		if ( !str_cmp(arg2, "oro") || !str_cmp(arg2, "gold") )
		{
			gold = amount;
			silver = 0;
		}
		else
		{
			silver = amount;
			gold = 0;
		}

		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;

			switch ( obj->pIndexData->vnum )
			{
			case OBJ_VNUM_SILVER_ONE:
				if ( silver )
				{
					silver -= 1;
					extract_obj( obj );
				}
				break;

			case OBJ_VNUM_GOLD_ONE:
				if ( gold )
				{
					gold -= 1;
					extract_obj( obj );
				}
				break;

			case OBJ_VNUM_SILVER_SOME:
				if ( silver )
				{
					if ( silver >= obj->value[0] )
					{
						silver -= obj->value[0];
						extract_obj( obj );
					}
					else
					{
						obj->value[0] -= silver;
						silver = 0;
					}
				}
				break;

			case OBJ_VNUM_GOLD_SOME:
				if ( gold )
				{
					if ( gold >= obj->value[1] )
					{
						gold -= obj->value[1];
						extract_obj( obj );
					}
					else
					{
						obj->value[1] -= gold;
						gold = 0;
					}
				}
				break;

			case OBJ_VNUM_COINS:
				if ( silver )
				{
					if ( silver >= obj->value[0] )
					{
						silver -= obj->value[0];
						gold = obj->value[1];
						extract_obj( obj );
						obj = create_money( gold, 0 );
						obj_to_room( obj, ch->in_room );
						gold = 0;
					}
					else
					{
						obj->value[0] -= silver;
						silver = 0;
					}
				}

				if ( gold )
				{
					if ( gold >= obj->value[1] )
					{
						gold -= obj->value[1];
						silver = obj->value[0];
						extract_obj( obj );
						obj = create_money( 0, silver );
						obj_to_room( obj, ch->in_room );
						silver = 0;
					}
					else
					{
						obj->value[1] -= gold;
						gold = 0;
					}
				}
				break;
			}

			if ( !silver && !gold )
				break;
		}

		/* restore the amount */
		if ( !str_cmp(arg2, "oro") || !str_cmp(arg2, "gold") )
		{
			gold = amount;
			silver = 0;
		}
		else
		{
			silver = amount;
			gold = 0;
		}

		if ( silver )
			ch->silver += amount;
		else
			ch->gold += amount;

		act( "Prendi qualche moneta da terra.", ch, NULL, NULL, TO_CHAR, POS_REST );
		if ( !IS_AFFECTED(ch, AFF_SNEAK) )
			act( "$n prende qualche moneta da terra.", ch, NULL, NULL, TO_ROOM, POS_REST );

		if ( IS_SET(ch->act, PLR_AUTOSPLIT) )
		{	/* AUTOSPLIT code */
			int			members = 0;
			CHAR_DATA	*gch;
			char		buffer[MIL];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
			{
				if ( !IS_AFFECTED(gch, AFF_CHARM) && is_same_group(gch, ch) )
					members++;
			}

			if ( members > 1 && (amount > 1) )
			{
				sprintf( buffer, "%d %d", silver, gold );
				do_split( ch, buffer );
			}
		}
		return;
	}

	if ( arg2[0] == '\0' )
	{
		if ( str_cmp("all", arg1) && str_cmp("tutti", arg1)
		&&	str_prefix("all.", arg1) && str_prefix("tutti.", arg1) )
		{
			/* 'get obj' */
			obj = get_obj_list( ch, arg1, ch->in_room->contents );
			if ( obj == NULL )
			{
				act( "Non vedo $T qui.", ch, NULL, arg1, TO_CHAR, POS_REST );
				return;
			}

			get_obj( ch, obj, NULL );
		}
		else
		{
			/* 'get all' or 'get all.obj' */
			found = FALSE;
			for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
			{
				obj_next = obj->next_content;
				if ( (arg1[3] == '\0' || is_name(&arg1[4], obj->name)) && can_see_obj(ch, obj) )
				{
					found = TRUE;
					get_obj( ch, obj, NULL );
				}
			}

			if ( !found )
			{
				if ( arg1[3] == '\0' )
					send_to_char( "Non vedo nulla di simile qui.\n\r", ch );
				else
					act( "Non vedo $T qui.", ch, NULL, &arg1[4], TO_CHAR, POS_REST );
			}
		}
	}
	else
	{
		/* 'get .. container' */
		if ( !str_cmp("all", arg2) || !str_cmp("tutti", arg2)
		||	!str_prefix("all.", arg2) || !str_prefix("tutti.", arg2) )
		{
			send_to_char( "Non puoi farlo.\n\r", ch );
			return;
		}

		if ( (container = get_obj_here(ch, arg2)) == NULL )
		{
			act( "Non vedo $T qui.", ch, NULL, arg2, TO_CHAR, POS_REST );
			return;
		}

		switch ( container->item_type )
		{
		default:
			send_to_char( "Non e' un contenitore valido.\n\r", ch );
			return;

		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
			break;

		case ITEM_CORPSE_PC:
			{
				if ( !can_loot(ch, container) )
				{
					send_to_char( "Non puoi farlo.\n\r", ch );
					return;
				}
			}
		}

		if ( IS_SET(container->value[1], CONT_CLOSED) )
		{
			act( "$d e' chiuso.", ch, NULL, container->name, TO_CHAR, POS_REST );
			return;
		}

		if ( str_cmp("all", arg1) && str_cmp("tutti", arg1)
		&&	str_prefix("all.", arg1) && str_prefix("tutti.", arg1) )
		{
			/* 'get obj container' */
			obj = get_obj_list( ch, arg1, container->contains );
			if ( obj == NULL )
			{
				act( "Non vedo nulla del genere nel $T.", ch, NULL, arg2, TO_CHAR, POS_REST );
				return;
			}

			get_obj( ch, obj, container );
		}
		else
		{
			/* 'get all container' or 'get all.obj container' */
			found = FALSE;
			for ( obj = container->contains; obj != NULL; obj = obj_next )
			{
				obj_next = obj->next_content;
				if ( (arg1[3] == '\0' || is_name(&arg1[4], obj->name)) && can_see_obj(ch, obj) )
				{
					found = TRUE;
					if ( container->pIndexData->vnum == OBJ_VNUM_PIT && !IS_ADMIN(ch) )
					{
						send_to_char( "L'avarizia e' un peccato capitale sai?!\n\r", ch );
						return;
					}

					get_obj( ch, obj, container );
				}
			}

			if ( !found )
			{
				if ( arg1[3] == '\0' )
					act( "Non trovi nulla in $T.", ch, NULL, arg2, TO_CHAR, POS_REST );
				else
					act( "Non trovi nulla del genere in $T.", ch, NULL, arg2, TO_CHAR, POS_REST );
			}
		}
	}
}


void do_put( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	OBJ_DATA	*container;
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	OBJ_DATA	*objc;
	int			pcount;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !str_cmp(arg2, "in") || !str_cmp(arg2, "dentro") )
		argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Mettere cosa dentro cosa?\n\r", ch );
		return;
	}

	if ( !str_cmp(arg2, "all") || !str_cmp(arg2, "tutto")
	||	 !str_prefix("all.", arg2) || !str_prefix("tutto.", arg2) )
	{
		send_to_char( "Non puoi fare questo.\n\r", ch );
		return;
	}

	if ( (container = get_obj_here(ch, arg2)) == NULL )
	{
		act( "Non trovi nessun $T qua.", ch, NULL, arg2, TO_CHAR, POS_REST );
		return;
	}

	if ( container->item_type != ITEM_CONTAINER )
	{
		send_to_char( "Non e' un contenitore.\n\r", ch );
		return;
	}

	if ( IS_SET(container->value[1], CONT_CLOSED) )
	{
		act( "$d e' chiuso.", ch, NULL, container->name, TO_CHAR, POS_REST );
		return;
	}

	if ( str_cmp("all", arg1) && str_cmp("tutti", arg1)
	&& str_prefix("all.", arg1) && str_prefix("all.", arg1) )
	{
		/* 'put obj container' */
		if ( (obj = get_obj_carry(ch, arg1)) == NULL )
		{
			send_to_char( "Non hai questo oggetto.\n\r", ch );
			return;
		}

		if ( obj == container )
		{
			send_to_char( "Non puoi metterlo dentro se' stesso.\n\r", ch );
			return;
		}

		if ( !can_drop_obj(ch, obj) )
		{
			send_to_char( "Non riesci a liberartene!\n\r", ch );
			return;
		}

		if ( WEIGHT_MULT(obj) != 100 )
		{
			send_to_char("Ti viene come l'idea che non sia una buona idea..\n\r",ch);
			return;
		}

		if ( obj->pIndexData->limit != -1 /*&& !IS_SET(container->value[1], CONT_ST_LIMITED)*/ )
		{
			act( "Questo contenitore senza valore non potrebbe mai reggere il potere di $p.",
				ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( IS_SET(container->value[1], CONT_FOR_ARROW) && (obj->item_type != ITEM_WEAPON || obj->value[0] != WEAPON_ARROW) )
		{
			act( "Puoi riporre solo delle frecce in $p.", ch, container, NULL, TO_CHAR, POS_REST );
			return;
		}

		if (get_obj_weight(obj) + get_true_weight(container) > (container->value[0] * 10)
		||	get_obj_weight(obj) > (container->value[3] * 10) )
		{
			send_to_char( "Non ci sta.\n\r", ch );
			return;
		}

		if ( obj->item_type == ITEM_POTION && IS_SET(container->wear_flags, ITEM_TAKE) )
		{
			pcount = 0;

			for ( objc = container->contains; objc != NULL; objc = objc->next_content )
			{
				if ( objc->item_type == ITEM_POTION )
					pcount++;
			}

			if ( pcount > 10 )
			{
				act( "Non e' prudente mettere troppe pozioni assieme, si potrebbero rompere..", ch, container, NULL, TO_CHAR, POS_REST );
				return;
			}
		}

		pcount = 0;
		for ( objc = container->contains; objc != NULL; objc = objc->next_content )
			pcount++;

		if ( pcount > container->value[0] )
		{
			act( "Non e' consigliabile mettere cosi' tanti oggetti di questo tipo dentro $p.",
				ch, container, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( container->pIndexData->vnum == OBJ_VNUM_PIT && !CAN_WEAR(container, ITEM_TAKE) )
		{
			if ( obj->timer )
				SET_BIT( obj->extra_flags, ITEM_HAD_TIMER );
			else
				obj->timer = number_range( 100, 200 );
		}

		obj_from_char( obj );
		obj_to_obj( obj, container );

		if ( IS_SET(container->value[1], CONT_PUT_ON) )
		{
			act("$n mette $p dentro $P.",ch,obj,container, TO_ROOM, POS_REST);
			act("Metti $p dentro $P.",ch,obj,container, TO_CHAR, POS_REST);
		}
		else
		{
			act( "$n mette $p dentro $P.", ch, obj, container, TO_ROOM, POS_REST );
			act( "Metti $p dentro $P.", ch, obj, container, TO_CHAR, POS_REST );
		}
	}
	else
	{
		pcount = 0;
		for ( objc = container->contains; objc != NULL; objc = objc->next_content )
			pcount++;

		/* 'put all container' or 'put all.obj container' */
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;

			if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
			&&	can_see_obj(ch, obj)
			&&	WEIGHT_MULT(obj) == 100
			&&	obj->wear_loc == WEAR_NONE
			&&	obj != container
			&&	can_drop_obj(ch, obj)
			&&	get_obj_weight(obj) + get_true_weight(container) <= (container->value[0] * 10)
			&&	get_obj_weight(obj) < (container->value[3] * 10) )
			{
				if ( container->pIndexData->vnum == OBJ_VNUM_PIT && !CAN_WEAR(obj, ITEM_TAKE) )
				{
					if ( obj->timer )
						SET_BIT( obj->extra_flags, ITEM_HAD_TIMER );
					else
						obj->timer = number_range( 100, 200 );
				}

				if ( obj->pIndexData->limit != -1  /*&& !IS_SET(container->value[1], CONT_ST_LIMITED)*/ )
				{
					act( "Questo contenitore senza valore non e' degno di $p.", ch, obj, NULL, TO_CHAR, POS_REST);
					continue;
				}

				if ( obj->item_type == ITEM_POTION && IS_SET(container->wear_flags, ITEM_TAKE) )
				{
					pcount = 0;
					for ( objc = container->contains; objc != NULL; objc = objc->next_content )
					{
						if ( objc->item_type == ITEM_POTION )
							pcount++;
					}

					if ( pcount > 10 )
					{
						act( "Non e' prudente mettere troppe pozioni assieme, si potrebbero rompere..",
							ch, container, NULL, TO_CHAR, POS_REST );
						continue;
					}
				}

				pcount++;
				if ( pcount > container->value[0] )
				{
					act( "Non e' consigliabile mettere tanti oggetti di questo tipo dentro $p.",
						ch, container, NULL, TO_CHAR, POS_REST );
					return;
				}

				obj_from_char( obj );
				obj_to_obj( obj, container );

				if ( IS_SET(container->value[1], CONT_PUT_ON) )
				{
					act("$n mette $p dentro $P.",ch,obj,container, TO_ROOM, POS_REST);
					act("Metti $p dentro $P.",ch,obj,container, TO_CHAR, POS_REST);
				}
				else
				{
					act( "$n mette $p dentro $P.", ch, obj, container, TO_ROOM, POS_REST );
					act( "Metti $p dentro $P.", ch, obj, container, TO_CHAR, POS_REST );
				}
			}
		}
	}
}


void do_drop( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	bool		found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Cosa vuoi posare per terra?\n\r", ch );
		return;
	}

	if ( is_number(arg) )
	{
		/* 'drop NNNN coins' */
		int amount, gold = 0, silver = 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		amount = atoi( arg );
		argument = one_argument( argument, arg );
		if ( amount <= 0
		|| (str_cmp(arg, "coins") && str_cmp(arg, "monete")
			&& str_cmp(arg, "coin") && str_cmp(arg, "moneta")
			&& str_cmp(arg, "gold") && str_cmp(arg, "oro")
			&& str_cmp(arg, "silver") && str_cmp(arg, "argento")) )
		{
			send_to_char( "Spiacente, non puoi farlo.\n\r", ch );
			return;
		}

		if ( !str_cmp(arg, "coins") || !str_cmp(arg, "monete")
		|| !str_cmp(arg, "coin") || !str_cmp(arg, "moneta")
		|| !str_cmp(arg, "silver") || !str_cmp(arg, "argento") )
		{
			if ( ch->silver < amount )
			{
				send_to_char( "Non hai cosi' tante monete d'argento.\n\r", ch );
				return;
			}

			ch->silver -= amount;
			silver = amount;
		}
		else
		{
			if ( ch->gold < amount )
			{
				send_to_char( "Non hai cosi' tante monete d'oro.\n\r", ch );
				return;
			}

			ch->gold -= amount;
			gold = amount;
		}

		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;

			switch ( obj->pIndexData->vnum )
			{
			case OBJ_VNUM_SILVER_ONE:	silver += 1; extract_obj( obj ); break;
			case OBJ_VNUM_GOLD_ONE:		gold += 1; extract_obj( obj ); break;
			case OBJ_VNUM_SILVER_SOME:	silver += obj->value[0]; extract_obj( obj ); break;
			case OBJ_VNUM_GOLD_SOME:	gold += obj->value[1]; extract_obj( obj ); break;
			case OBJ_VNUM_COINS:		silver += obj->value[0]; gold += obj->value[1]; extract_obj( obj ); break;
			}
		}

		obj = create_money( gold, silver );
		obj_to_room( obj, ch->in_room );
		if ( !IS_AFFECTED(ch, AFF_SNEAK) )
		act( "$n posa qualche moneta.", ch, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "OK.\n\r", ch );
		if ( IS_WATER(ch->in_room) )
		{
			extract_obj( obj );
			if ( !IS_AFFECTED(ch, AFF_SNEAK) )
				act( "Le monete s'immergono nell'acqua.", ch, NULL, NULL, TO_ROOM, POS_REST );
			act( "Le monete s'immergono nell'acqua.", ch, NULL, NULL, TO_CHAR, POS_REST );
		}

		return;
	}

	if ( str_cmp("all", arg) && str_cmp("tutti", arg)
	&&	str_prefix("all.", arg) && str_prefix("tutti.", arg) )
	{
		/* 'drop obj' */
		if ( (obj = get_obj_carry(ch, arg)) == NULL )
		{
			send_to_char( "Non hai quest'oggetto.\n\r", ch );
			return;
		}

		if ( !can_drop_obj(ch, obj) )
		{
			send_to_char( "Non riesci a liberartene.\n\r", ch );
			return;
		}

		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		if ( !IS_AFFECTED(ch, AFF_SNEAK) )
			act( "$n posa $p.", ch, obj, NULL, TO_ROOM, POS_REST);
		act( "Posi a terra $p.", ch, obj, NULL, TO_CHAR , POS_REST);
		if ( obj->pIndexData->vnum == OBJ_VNUM_POTION_VIAL && number_percent() < 40 )
		{
			if (!IS_SET(ch->in_room->sector_type, SECT_FOREST)
			&&	!IS_SET(ch->in_room->sector_type, SECT_DESERT)
			&&	!IS_SET(ch->in_room->sector_type, SECT_AIR)
			&&	!IS_WATER(ch->in_room) )
			{
				act( "$p si frantuma in mille pezzi.", ch, obj, NULL,TO_ROOM , POS_REST);
				act( "$p si frantuma in mille pezzi.", ch, obj, NULL,TO_CHAR , POS_REST);
				extract_obj( obj );
				return;
			}
		}

		if ( IS_SET(obj->progtypes, OPROG_DROP) )
			( obj->pIndexData->oprogs->drop_prog ) ( obj, ch );

		if ( !may_float(obj) && cant_float(obj) && IS_WATER(ch->in_room) )
		{
			if ( !IS_AFFECTED(ch, AFF_SNEAK) )
				act( "$p affonda nell'acqua.", ch, obj, NULL, TO_ROOM, POS_REST);
			act( "$p affonda nell'acqua.", ch, obj, NULL, TO_CHAR, POS_REST);
			extract_obj( obj );
		}
		else if ( is_melted(obj) && IS_WATER(ch->in_room) )
		{
			if ( !IS_AFFECTED(ch, AFF_SNEAK) )
				act( "$p si scioglie nell'acqua.", ch, obj, NULL, TO_ROOM, POS_REST);
			act( "$p si scioglie nell'acqua.", ch, obj, NULL, TO_CHAR, POS_REST);
			extract_obj( obj );
		}
		else if ( IS_OBJ_STAT(obj, ITEM_MELT_DROP) )
		{
			if ( !IS_AFFECTED(ch, AFF_SNEAK) )
				act( "$p si dissolve in una nuvoletta di polvere.", ch, obj, NULL, TO_ROOM, POS_REST );
			act( "$p si dissolve in una nuvoletta di polvere.", ch, obj, NULL, TO_CHAR, POS_REST );
			extract_obj( obj );
		}
	}
	else
	{
		/* 'drop all' or 'drop all.obj' */
		found = FALSE;
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;

			if ((arg[3] == '\0' || is_name(&arg[4], obj->name))
			&&	can_see_obj(ch, obj)
			&&	obj->wear_loc == WEAR_NONE
			&&	can_drop_obj(ch, obj) )
			{
				found = TRUE;
				obj_from_char( obj );
				obj_to_room( obj, ch->in_room );
				if ( !IS_AFFECTED(ch, AFF_SNEAK) )
					act( "$n posa $p.", ch, obj, NULL, TO_ROOM , POS_REST );
				act( "Posi $p per terra.", ch, obj, NULL, TO_CHAR , POS_REST );
				if ( obj->pIndexData->vnum == OBJ_VNUM_POTION_VIAL && number_percent() < 70 )
				{
					if (!IS_SET(ch->in_room->sector_type, SECT_FOREST)
					&&	!IS_SET(ch->in_room->sector_type, SECT_DESERT)
					&&	!IS_SET(ch->in_room->sector_type, SECT_AIR)
					&&	!IS_WATER(ch->in_room) )
					{
						if ( !IS_AFFECTED(ch, AFF_SNEAK) )
							act( "$p si frantuma in mille pezzi.", ch, obj, NULL,TO_ROOM , POS_REST);
						act( "$p si frantuma in mille pezzi.", ch, obj, NULL,TO_CHAR , POS_REST);
						extract_obj( obj );
						continue;
					}
				}

				if ( IS_SET(obj->progtypes, OPROG_DROP) )
					( obj->pIndexData->oprogs->drop_prog ) ( obj, ch );

				if ( !may_float(obj) && cant_float(obj) && IS_WATER(ch->in_room) )
				{
					if ( !IS_AFFECTED(ch, AFF_SNEAK) )
						act( "$p affonda nell'acqua.", ch, obj, NULL, TO_ROOM, POS_REST);
					act( "$p affonda nell'acqua.", ch, obj, NULL, TO_CHAR, POS_REST);
					extract_obj( obj );
				}
				else if ( IS_OBJ_STAT(obj, ITEM_MELT_DROP) )
				{
					if ( !IS_AFFECTED(ch, AFF_SNEAK) )
						act( "$p si dissolve in una nuvoletta di polvere.", ch, obj, NULL, TO_ROOM, POS_REST );
					act( "$p si dissolve in una nuvoletta di polvere.", ch, obj, NULL, TO_CHAR, POS_REST );
					extract_obj( obj );
				}
			}
		}

		if ( !found )
		{
			if ( arg[3] == '\0' )
				act( "Non stai trasportando nulla.", ch, NULL, arg, TO_CHAR , POS_REST );
			else
				act( "Non stai trasportando nessun $T.", ch, NULL, &arg[4], TO_CHAR, POS_REST );
		}
	}
}


void do_drag( CHAR_DATA *ch, char *argument )
{
	char			arg1[MIL];
	char			arg2[MIL];
	char			buf[MSL];
	CHAR_DATA		*gch;
	OBJ_DATA		*obj;
	EXIT_DATA		*pexit;
	ROOM_INDEX_DATA *was_in_room;
	int				direction;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	/* Get type. */
	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Cosa vuoi trasportare? E in che direzione?\n\r", ch );
		return;
	}

	if ( str_cmp("all", arg1) || str_cmp("tutti", arg1)
	||	!str_prefix("all.", arg1) || !str_prefix("tutti.", arg1) )
	{
		send_to_char( "Non puoi farlo.\n\r", ch );
		return;
	}

	obj = get_obj_list( ch, arg1, ch->in_room->contents );
	if ( obj == NULL )
	{
		act( "Non vedo $T qui.", ch, NULL, arg1, TO_CHAR , POS_REST);
		return;
	}

	if ( !CAN_WEAR(obj, ITEM_TAKE) )
	{
		send_to_char( "Non e' raccoglibile.\n\r", ch );
		return;
	}

	if ( (IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch))
	||	 (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch))
	||	 (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)) )
	{
		ch->point[LIFE] -= ch->level;
		act( "L'aura di $p avvampa sulla tua mano costringendoti a liberartene!", ch, obj, NULL, TO_CHAR , POS_REST);
		act( "$n geme di dolore mentre butta a terra  $p.",  ch, obj, NULL, TO_ROOM , POS_REST);
		return;
	}

	/* Controllo per le slay */
	if ( (IS_EVIL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYEVIL))
	||	 (IS_GOOD(ch) && IS_OBJ_STAT(obj, ITEM_SLAYGOOD))
	||	 (IS_NEUTRAL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYNEUTRAL)) )
	{
		ch->point[LIFE] -= ch->level*2;
		act( "L'aura di $p avvampa sulla tua mano costringendoti a liberartene!", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n geme di dolore mentre butta a terra  $p.",  ch, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( obj->in_room != NULL )
	{
		for ( gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room )
			if ( gch->on == obj )
			{
				act( "$N sembra che stia usando $p.", ch, obj, gch, TO_CHAR, POS_REST );
				return;
			}
	}

	if ( (get_carry_weight(ch) + get_obj_weight(obj)) > (2 * can_carry_w(ch)) )
	{
		act( "$d: Non puoi trascinare cosi' tanto peso.", ch, NULL, obj->name, TO_CHAR, POS_REST );
		return;
	}

	if ( get_eq_char(ch, WEAR_LEFT) != NULL || get_eq_char(ch, WEAR_RIGHT) != NULL || get_eq_char(ch, WEAR_BOTH) != NULL )
	{
		send_to_char( "Hai bisogno di avere entrambe le mani libere.\n\r",ch);
		return;
	}

	if ( (direction = find_exit(ch, arg2, TRUE)) < 0 )
		return;

	if ( (pexit = ch->in_room->exit[direction]) == NULL || pexit->to_room == NULL || !can_see_room(ch, pexit->to_room) )
	{
		send_to_char( "Non puoi andare in quella direzione.\n\r", ch );
		return;
	}

	sprintf( buf,"Trascini $p verso %s.", dir_name[direction] );
	act( buf, ch, obj, NULL, TO_CHAR, POS_REST );
	if ( !IS_AFFECTED(ch, AFF_SNEAK) )
	{
		sprintf( buf,"$n trascina $p verso %s.", dir_name[direction] );
		act( buf, ch, obj, NULL, TO_ROOM, POS_REST );
	}

	obj_from_room( obj );
	obj_to_char( obj, ch );

	if ( IS_SET(obj->progtypes, OPROG_GET) )
		( obj->pIndexData->oprogs->get_prog ) ( obj, ch );

	if ( obj->carried_by != ch )
		return;

	was_in_room = ch->in_room;
	move_char( ch, direction, FALSE );

	if ( was_in_room == ch->in_room )
		send_to_char( "Non puoi trascinare nulla in quella direzione.\n\r", ch );
	else
	{
		if ( !IS_AFFECTED(ch, AFF_SNEAK) )
			act( "$n posa per terra $p.", ch, obj, NULL, TO_ROOM , POS_REST);
		act( "Posi per terra $p.", ch, obj, NULL, TO_CHAR , POS_REST);
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	}

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
}


void do_give( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	char		buf[MSL];
	char		buf2[MSL];
	CHAR_DATA	*victim;
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	sprintf( buf2, "%s", argument );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Cosa vuoi dare e a chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg2)) == ch )
	{
		send_to_char( "Cosa ti vorresti dare?\n\r", ch );
		return;
	}

	if ( is_number(arg1) )
	{
		/* 'give NNNN coins victim' */
		int		amount;
		bool	silver;
		int		weight;
		/*~~~~~~~~~~~*/

		amount = atoi( arg1 );
		if (amount <= 0	||
			(	str_cmp(arg2, "coins")
			&&	str_cmp(arg2, "coin")
			&&	str_cmp(arg2, "gold")
			&&	str_cmp(arg2, "silver")
			&&	str_cmp(arg2, "monete")
			&&	str_cmp(arg2, "moneta")
			&&	str_cmp(arg2, "oro")
			&&	str_cmp(arg2, "argento")) )
		{
			send_to_char( "Spiacente, non puoi farlo.\n\r", ch );
			return;
		}

		silver = ( str_cmp(arg2, "gold") && str_cmp(arg2, "oro") );

		argument = one_argument( argument, arg2 );
		if ( arg2[0] == '\0' )
		{
			send_to_char( "Dare cosa a chi?\n\r", ch );
			return;
		}

		if ( (victim = get_char_room(ch, arg2)) == NULL )
		{
			send_to_char( "Non trovi nessuno qui con tale nome.\n\r", ch );
			return;
		}

		if ( (!silver && ch->gold < amount) || (silver && ch->silver < amount) )
		{
			send_to_char( "Non hai cosi' tante monete.\n\r", ch );
			return;
		}

		if ( IS_MOB(victim) && IS_SET(victim->act, ACT_IS_CHANGER) && !can_see(victim, ch) )
		{
			act( "$n ti dice 'Mi dispiace ma non ti vedo.'", victim, NULL, ch, TO_VICT, POS_REST );
			return;
		}

		if ( !silver )
			weight = amount * 2 / 5;
		else
			weight = amount / 10;

		if ( !IS_MOB(victim) && get_carry_weight(victim) + weight > can_carry_w(victim) )
		{
			act( "$N non puo' sopportare cosi' tanto peso.", ch, NULL, victim, TO_CHAR, POS_REST );
			send_to_char( "Non puoi trasportare cosi' tanto peso", victim );
			return;
		}

		if ( silver )
		{
			ch->silver -= amount;
			victim->silver += amount;
		}
		else
		{
			ch->gold -= amount;
			victim->gold += amount;
		}

		sprintf( buf,"$n ti da' %d monet%c %s.", amount, (amount == 1) ? 'a' : 'e', (silver) ? "d'argento" : "d'oro" );
		act( buf, ch, NULL, victim, TO_VICT, POS_REST );

		if ( amount == 1 )
			act( "$n da' a $N una moneta.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		else
			act( "$n da' a $N qualche moneta.", ch, NULL, victim, TO_NOTVICT, POS_REST );

		sprintf( buf, "Dai a $N %d monet%c %s.", amount, (amount == 1) ? 'a' : 'e', (silver) ? "d'argento" : "d'oro" );
		act( buf, ch, NULL, victim, TO_CHAR, POS_REST );

		if ( IS_SET(victim->progtypes, MPROG_BRIBE) )
			( victim->pIndexData->mprogs->bribe_prog ) ( victim, ch, amount );

		if ( IS_MOB(victim) && IS_SET(victim->act, ACT_IS_CHANGER) )
		{
			int change;
			/*~~~~~~~*/

			change = ( silver ? 95 * amount / 100 / 100 : 95 * amount );

			if ( silver )
				weight = change * 2 / 5;
			else
				weight = change / 10;

			if ( !silver )
				weight -= amount * 2 / 5;
			else
				weight -= amount / 10;

			if ( !IS_MOB(ch) && get_carry_weight(ch) + weight > can_carry_w(ch) )
			{
				act( "Non puoi sopportare cosi' tanto peso.", ch, NULL, NULL, TO_CHAR, POS_REST );
				return;
			}

			if ( !silver && change > victim->silver )
				victim->silver += change;

			if ( silver && change > victim->gold )
				victim->gold += change;

			if ( change < 1 && can_see(victim, ch) )
			{
				act( "$n ti dice 'Scusate ma non mia vete dato abbastanza per fare il cambio.'", victim, NULL, ch, TO_VICT, POS_REST );
				ch->reply = victim;
				sprintf( buf, "%d %s %s", amount, silver ? "silver" : "gold", ch->name );
				do_give( victim, buf );
			}
			else if ( can_see(victim, ch) )
			{
				sprintf( buf, "%d %s %s", change, silver ? "gold" : "silver", ch->name );
				do_give( victim, buf );
				if ( silver )
				{
					sprintf( buf, "%d silver %s", (95 * amount / 100 - change * 100), ch->name );
					do_give( victim, buf );
				}
				act( "$n ecco a voi, tornate presto.'", victim, NULL, ch, TO_VICT, POS_REST );
				ch->reply = victim;
			}
		}

		return;
	}

	if ( (obj = get_obj_carry(ch, arg1)) == NULL )
	{
		send_to_char( "Non hai nessun oggetto con questo nome.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE )
	{
		send_to_char( "Prima smetti di usarlo.\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg2)) == NULL )
	{
		send_to_char( "Non sono qui.\n\r", ch );
		return;
	}

	if ( (IS_MOB(victim) && victim->pIndexData->pShop != NULL) && !IS_SET(victim->progtypes, MPROG_GIVE) )
	{
		act( "$N ti dice 'Mi spiace, dovrete venderlo.'", ch, NULL, victim, TO_CHAR, POS_REST );
		ch->reply = victim;
		return;
	}

	if ( !can_drop_obj(ch, obj) )
	{
		send_to_char( "Non riesci a liberartene.\n\r", ch );
		return;
	}

	if ( victim->carry_number + get_obj_number(obj) > can_carry_n(victim) )
	{
		act( "$N ha le mani piene.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( get_carry_weight(victim) + get_obj_weight(obj) > can_carry_w(victim) )
	{
		act( "$N non puo' sopportare altro peso.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n non riesce a darti $p.", ch, obj, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( !can_see_obj(victim, obj) )
	{
		act( "$N non puo' vederlo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(victim))
	||	(IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(victim))
	||	(IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(victim)) )
	{
		ch->point[LIFE] -= ch->level;
		send_to_char( "L'oggetto rifiuta di avvicinarsi al destinatario.", ch );
		return;
	}

	/* Controllo per le slay */
	if ( (IS_EVIL(victim) && IS_OBJ_STAT(obj, ITEM_SLAYEVIL))
	||	 (IS_GOOD(victim) && IS_OBJ_STAT(obj, ITEM_SLAYGOOD))
	||	 (IS_NEUTRAL(victim) && IS_OBJ_STAT(obj, ITEM_SLAYNEUTRAL)) )
	{
		victim->point[LIFE] -= victim->level*2;
		act( "L'aura di $p avvampa sulla tua mano costringendoti a liberartene!", victim, obj, NULL, TO_CHAR, POS_REST );
		act( "$n geme di dolore mentre butta a terra  $p.", victim, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	obj_from_char( obj );
	obj_to_char( obj, victim );
	act( "$n da' $p a $N.", ch, obj, victim, TO_NOTVICT, POS_REST );
	act( "$n ti da' $p.",   ch, obj, victim, TO_VICT, POS_REST );
	act( "Dai $p a $N.", ch, obj, victim, TO_CHAR, POS_REST );

	if ( IS_SET(obj->progtypes, OPROG_GIVE) )
		( obj->pIndexData->oprogs->give_prog ) ( obj, ch, victim );

	if ( IS_SET(obj->progtypes, OPROG_GET) )
		( obj->pIndexData->oprogs->get_prog ) ( obj, victim );

	if ( IS_SET(victim->progtypes, MPROG_GIVE) )
		( victim->pIndexData->mprogs->give_prog ) ( victim, ch, obj );
}


void do_bury( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		buf[MIL];
	char		*bufp;
	OBJ_DATA	*obj, *shovel, *stone;
	int			move;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( !IS_ADMIN(ch) && ch->class != CLASS_CLERIC )
	{
		send_to_char( "Non sei mica un chierico!\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Quale corpo vuoi seppellire?\n\r", ch );
		return;
	}

	if ( (shovel = get_weapon_char(ch, WEAPON_MACE)) == NULL
	|| !is_name("shovel vanga pala piccone piccozza zappa badile", shovel->name) )
	{
		send_to_char( "Ti serve un attrezzo per scavare!\n\r", ch );
		return;
	}

	obj = get_obj_list( ch, arg, ch->in_room->contents );
	if ( obj == NULL )
	{
		act( "Non vedo nessun $T qui.", ch, NULL, arg, TO_CHAR, POS_REST );
		return;
	}

	if ( obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC )
	{
		send_to_char( "Perche' vorresti seppellirlo?\n\r", ch );
		return;
	}

	switch ( ch->in_room->sector_type )
	{
	case SECT_CITY:
	case SECT_INSIDE:
		send_to_char( "Il pavimento e' troppo duro per scavarci.\n\r", ch );
		return;

	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
		send_to_char( "Non puoi seppellire nessuno qui.\n\r", ch );
		return;

	case SECT_AIR:
		send_to_char( "In aria? Va bene avere la testa tra le nuvole, ma..\n\r", ch );
		return;
	}

	move = (int) ( (obj->weight * 5) / get_curr_stat(ch, STR) );
	move = URANGE( 2, move, 1000 );
	if ( move > ch->point[MOVE] )
	{
		send_to_char( "E dove le trovi le forze per scavare un giaciglio per QUELLO?\n\r", ch );
		return;
	}

	ch->point[MOVE] -= move;

	act( "Seppellisci solennemente $p..", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$n seppellisce solennemente $p..", ch, obj, NULL, TO_ROOM, POS_REST );

	SET_BIT( obj->extra_flags, ITEM_BURIED );
	WAIT_STATE( ch, 4 * PULSE_VIOLENCE );

	obj->timer = -1;

	buf[0] = '\0';
	bufp = obj->short_descr;
	while ( bufp[0] != '\0' )
	{
		bufp = one_argument( bufp, arg );
		if ( !(!str_cmp(arg, "The") || !str_cmp(arg, "undead") || !str_cmp(arg, "body") || !str_cmp(arg, "corpse") || !str_cmp(arg, "of"))
		|| !(!str_cmp(arg,"Il") || !str_cmp(arg,"cadavere") || !str_cmp(arg,"rianimato") || !str_cmp(arg,"corpo") || !str_cmp(arg,"di") ))
		{
			if ( buf[0] == '\0' )
				strcat( buf, arg );
			else
			{
				strcat( buf, " " );
				strcat( buf, arg );
			}
		}
	}

	sprintf( arg, "%s", buf );

	stone = create_object( get_obj_index(OBJ_VNUM_GRAVE_STONE), ch->level );

	sprintf( buf, stone->description, arg );
	free_string( stone->description );
	stone->description = str_dup( buf );

	sprintf( buf, stone->short_descr, arg );
	free_string( stone->short_descr );
	stone->short_descr = str_dup( buf );

	obj_to_room( stone, ch->in_room );

	/*
	 * a little trick here.. :) although grave stone is not a container.. protects corpse from area affect
	 * attacks. but what about earthquake
	 */
	obj_from_room( obj );
	obj_to_obj( obj, stone );
}


void do_dig( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj, *shovel, *corpse;
	int			move;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Scavare in quale tomba??\n\r", ch );
		return;
	}

	if ( (shovel = get_weapon_char(ch, WEAPON_MACE)) == NULL
	|| !is_name("shovel vanga pala piccone piccozza zappa badile", shovel->name) )
	{
		send_to_char( "Ti serve il badile per farlo!\n\r", ch );
		return;
	}

	obj = get_obj_list( ch, arg, ch->in_room->contents );
	if ( obj == NULL )
	{
		act( "Non vedo $T qui.", ch, NULL, arg, TO_CHAR, POS_REST );
		return;
	}

	if ( obj->pIndexData->vnum != OBJ_VNUM_GRAVE_STONE )
	{
		send_to_char( "E sarebbe una tomba.. ?\n\r", ch );
		return;
	}

	move = (int) ( (obj->weight * 5) / get_curr_stat(ch, STR) );
	move = URANGE( 2, move, 1000 );
	if ( move > ch->point[MOVE] )
	{
		send_to_char( "Decisamente ti servirebbero un po' di energie in piu'..\n\r", ch );
		return;
	}

	ch->point[MOVE] -= move;

	act( "Cominci a scavare $p..", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$n comincia a scavare $p..", ch, obj, NULL, TO_ROOM, POS_REST );

	WAIT_STATE( ch, 4 * PULSE_VIOLENCE );

	if ( (corpse = obj->contains) == NULL )
	{
		act( "Il tuo scavare non rivela nulla.", ch, NULL, NULL, TO_ALL, POS_REST );
		return;
	}

	REMOVE_BIT( corpse->extra_flags, ITEM_BURIED );
	obj_from_obj( corpse );
	obj_to_room( corpse, ch->in_room );
	extract_obj( obj );
	corpse->timer = number_range( 25, 40 );
	act( "Il tuo scavare rivela la presenza di $p.", ch, corpse, NULL, TO_ALL, POS_REST );
}


/*
 * For poisoning weapons and food/drink
 */
void do_envenom( CHAR_DATA *ch, char *argument )
{

	OBJ_DATA	*obj;
	AFFECT_DATA af;
	int			percent, skill;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* find out what */
	if ( argument == '\0' )
	{
		send_to_char( "Cosa vuoi avvelenare?\n\r", ch );
		return;
	}

	obj = get_obj_list( ch, argument, ch->carrying );

	if ( obj == NULL )
	{
		send_to_char( "Non hai nessun oggetto con questo nome.\n\r", ch );
		return;
	}

	if ( (skill = get_skill(ch, gsn_envenom)) < 1 )
	{
		send_to_char( "Ti vuoi avvelenare?!\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON )
	{
		if ( IS_OBJ_STAT(obj, ITEM_BLESS) || IS_OBJ_STAT(obj, ITEM_BURN_PROOF) )
		{
			act( "Non riesci nel tuo intento di avvelenamento $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( number_percent() < skill ) /* success! */
		{
			act( "$n contagia $p con del veleno.", ch, obj, NULL, TO_ROOM, POS_REST );
			act( "Contagi $p con del veleno.", ch, obj, NULL, TO_CHAR, POS_REST );
			if ( !obj->value[3] )
			{
				obj->value[3] = 1;
				check_improve( ch, gsn_envenom, TRUE, 4 );
			}

			WAIT_STATE( ch, skill_table[gsn_envenom].beats );
			return;
		}

		act( "Non riesci ad avvelenare $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		if ( !obj->value[3] )
			check_improve( ch, gsn_envenom, FALSE, 4 );
		WAIT_STATE( ch, skill_table[gsn_envenom].beats );
		return;
	}

	if ( obj->item_type == ITEM_WEAPON )
	{
		if (IS_WEAPON_STAT(obj, WEAPON_FLAMING)
		||	IS_WEAPON_STAT(obj, WEAPON_FROST)
		||	IS_WEAPON_STAT(obj, WEAPON_VAMPIRIC)
		||	IS_WEAPON_STAT(obj, WEAPON_SHARP)
		||	IS_WEAPON_STAT(obj, WEAPON_VORPAL)
		||	IS_WEAPON_STAT(obj, WEAPON_SHOCKING)
		||	IS_WEAPON_STAT(obj, WEAPON_HOLY)
		||	IS_OBJ_STAT(obj, ITEM_BLESS)
		||	IS_OBJ_STAT(obj, ITEM_BURN_PROOF) )
		{
			act( "Il tuo tentativo di avvelenare $p non sembra essere riuscito.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( obj->value[3] < 0 || attack_table[obj->value[3]].damage == DAM_BASH )
		{
			send_to_char( "Puoi avvelenare solo armi appuntite.\n\r", ch );
			return;
		}

		if ( IS_WEAPON_STAT(obj, WEAPON_POISON) )
		{
			act( "$p ha gia' del veleno in se'.", ch, obj, NULL, TO_CHAR, POS_REST );
			return;
		}

		percent = number_percent();
		if ( percent < skill )
		{
			af.where = TO_WEAPON;
			af.type = gsn_poison;
			af.level = ch->level * percent / 100;
			af.duration = ch->level * percent / 100;
			af.location = 0;
			af.modifier = 0;
			af.bitvector = WEAPON_POISON;
			affect_to_obj( obj, &af );

			if ( !IS_AFFECTED(ch, AFF_SNEAK) )
				act( "$n copre $p con del veleno.", ch, obj, NULL, TO_ROOM, POS_REST );
			act( "Copri $p con del veleno.", ch, obj, NULL, TO_CHAR, POS_REST );
			check_improve( ch, gsn_envenom, TRUE, 3 );
			WAIT_STATE( ch, skill_table[gsn_envenom].beats );
			return;
		}
		else
		{
			act( "Non riesci ad avvelenare $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			check_improve( ch, gsn_envenom, FALSE, 3 );
			WAIT_STATE( ch, skill_table[gsn_envenom].beats );
			return;
		}
	}


	act( "Non puoi avvelenare $p.", ch, obj, NULL, TO_CHAR, POS_REST );
}


void do_fill( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		buf[MSL];
	OBJ_DATA	*obj;
	OBJ_DATA	*fountain;
	bool		found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Cosa vuoi riempire?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_carry(ch, arg)) == NULL )
	{
		send_to_char( "Non hai nessun oggetto con questo nome.\n\r", ch );
		return;
	}

	found = FALSE;
	for ( fountain = ch->in_room->contents; fountain != NULL; fountain = fountain->next_content )
	{
		if ( fountain->item_type == ITEM_FOUNTAIN )
		{
			found = TRUE;
			break;
		}
	}

	if ( !found )
	{
		send_to_char( "Non vedo fontane qui..\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		send_to_char( "Devi usare un contenitore adatto!\n\r", ch );
		return;
	}

	if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
	{
		send_to_char( "C'e' gia' un altro liquido all'interno.\n\r", ch );
		return;
	}

	if ( obj->value[1] >= obj->value[0] )
	{
		send_to_char( "il contenitore e' gia' pieno.\n\r", ch );
		return;
	}

	sprintf( buf,"Riempi $p di %s da $P.", liq_table[fountain->value[2]].liq_translate );
	act( buf, ch, obj, fountain, TO_CHAR, POS_REST );
	sprintf( buf,"$n riempie $p di %s da $P.", liq_table[fountain->value[2]].liq_translate );
	act( buf, ch, obj, fountain, TO_ROOM, POS_REST );

	obj->value[2] = fountain->value[2];
	obj->value[1] = obj->value[0];
}


void do_pour( CHAR_DATA *ch, char *argument )
{
	char		arg[MSL], buf[MSL];
	OBJ_DATA	*out, *in;
	CHAR_DATA	*vch = NULL;
	int			amount;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "Cosa vuoi svuotare e dove?\n\r", ch );
		return;
	}

	if ( (out = get_obj_carry(ch, arg)) == NULL )
	{
		send_to_char( "Non hai nessun oggetto con questo nome.\n\r", ch );
		return;
	}

	if ( out->item_type != ITEM_DRINK_CON )
	{
		send_to_char( "Non e' un oggetto da cui poi bere.\n\r", ch );
		return;
	}

	if ( !str_cmp(argument, "out") || !str_cmp(argument, "fuori") )
	{
		if ( out->value[1] == 0 )
		{
			send_to_char( "Il contenitore e' vuoto.\n\r", ch );
			return;
		}

		out->value[1] = 0;
		out->value[3] = 0;
		if ( !IS_WATER(ch->in_room) )
		{
			sprintf( buf, "Svuoti $p riversando %s per terra.", liq_table[out->value[2]].liq_translate );
			act( buf, ch, out, NULL, TO_CHAR, POS_REST );

			sprintf( buf,"$n svuota $p riversando %s per terra.", liq_table[out->value[2]].liq_translate );
			act( buf, ch, out, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			sprintf( buf,"Svuoti $p versando %s nell'acqua.", liq_table[out->value[2]].liq_translate );
			act( buf, ch, out, NULL, TO_CHAR, POS_REST );

			sprintf( buf,"$n svuota $p versando %s nell'acqua.", liq_table[out->value[2]].liq_translate );
			act( buf, ch, out, NULL, TO_ROOM, POS_REST );
		}

		return;
	}

	if ( (in = get_obj_here(ch, argument)) == NULL )
	{
		vch = get_char_room( ch, argument );

		if ( vch == NULL )
		{
        send_to_char( "Dove vuoi svuotare?\n\r", ch );
			return;
		}

		in = get_hold_char( vch );

		if ( in == NULL )
		{
        send_to_char( "Non ha il tipo di contenitore adatto.", ch );
			return;
		}
	}

	if ( in->item_type != ITEM_DRINK_CON )
	{
    send_to_char( "Puoi riempire solo altri contenitori di liquido.\n\r", ch );
		return;
	}

	if ( in == out )
	{
    send_to_char( "Cambiamo le leggi della fisica?!\n\r", ch );
		return;
	}

	if ( in->value[1] != 0 && in->value[2] != out->value[2] )
	{
    send_to_char( "Contengono liquidi differenti.\n\r", ch );
		return;
	}

	if ( out->value[1] == 0 )
	{
    act( "Non c'e' nulla da svuotare..", ch, out, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( in->value[1] >= in->value[0] )
	{

    act( "$p e' gia' al massimo della capacita'.", ch, in, NULL, TO_CHAR, POS_REST );
		return;
	}

	amount = UMIN( out->value[1], in->value[0] - in->value[1] );

	in->value[1] += amount;
	out->value[1] -= amount;
	in->value[2] = out->value[2];

	if ( vch == NULL )
	{
		sprintf( buf,"Riempi $P di %s da $p .", liq_table[out->value[2]].liq_translate );
		act( buf, ch, out, in, TO_CHAR, POS_REST );
		sprintf( buf,"$n riempie di %s $P da $p.", liq_table[out->value[2]].liq_translate );
		act( buf, ch, out, in, TO_ROOM, POS_REST );
	}
	else
	{
		sprintf( buf,"Versi un po' di %s per $N.", liq_table[out->value[2]].liq_translate );
		act( buf, ch, NULL, vch, TO_CHAR, POS_REST );
		sprintf( buf,"$n versa un po' di %s per te.", liq_table[out->value[2]].liq_translate );
		act( buf, ch, NULL, vch, TO_VICT, POS_REST );
		sprintf( buf,"$n versa un po' di %s per $N.", liq_table[out->value[2]].liq_translate );
		act( buf, ch, NULL, vch, TO_NOTVICT, POS_REST );
	}
}


void do_drink( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	int			amount;
	int			liquid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
		{
			if ( obj->item_type == ITEM_FOUNTAIN )
				break;
		}

		if ( obj == NULL )
		{
			send_to_char( "Cosa vuoi bere?\n\r", ch );
			return;
		}
	}
	else
	{
		if ( (obj = get_obj_here(ch, arg)) == NULL )
		{
			send_to_char( "Non hai niente del genere.\n\r", ch );
			return;
		}
	}

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )
	{
		send_to_char( "Ups..non riesci a centrare la tua bocca..  *Hic*\n\r", ch );
		return;
	}

	switch ( obj->item_type )
	{
	default:
		send_to_char( "Non puoi mica bere da li'..\n\r", ch );
		return;

	case ITEM_FOUNTAIN:
		if ( (liquid = obj->value[2]) < 0 )
		{
			bug( "Do_drink: numero di liquido errato %d.", liquid );
			liquid = obj->value[2] = 0;
		}

		amount = liq_table[liquid].liq_affect[4] * 3;
		break;

	case ITEM_DRINK_CON:
		if ( obj->value[1] <= 0 )
		{
			send_to_char( "Il contenitore e' vuoto.\n\r", ch );
			return;
		}

		if ( (liquid = obj->value[2]) < 0 )
		{
			bug( "Do_drink: bad liquid number %d.", liquid );
			liquid = obj->value[2] = 0;
		}

		amount = liq_table[liquid].liq_affect[4];
		amount = UMIN( amount, obj->value[1] );
		break;
	}

	if ( !IS_MOB(ch) && !IS_ADMIN(ch) && ch->pcdata->condition[COND_FULL] > 80
	  && (ch->pcdata->condition[COND_THIRST] > 0) )
	{
		send_to_char( "Bere? Ancora?!.\n\r", ch );
		return;
	}

	act( "$n beve $T da $p.", ch, obj, liq_table[liquid].liq_translate, TO_ROOM, POS_REST );
	act( "Bevi $T da $p.", ch, obj, liq_table[liquid].liq_translate, TO_CHAR, POS_REST );

	if ( ch->fighting != NULL )
		WAIT_STATE( ch, 3*PULSE_VIOLENCE );

	gain_condition( ch, COND_DRUNK,  amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36 );
	gain_condition( ch, COND_FULL,   amount * liq_table[liquid].liq_affect[COND_FULL] / 2 );
	gain_condition( ch, COND_THIRST, amount * liq_table[liquid].liq_affect[COND_THIRST] / 5 );
	gain_condition( ch, COND_HUNGER, amount * liq_table[liquid].liq_affect[COND_HUNGER] / 1 );

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )
		act( "Ti senti un po' ubriac$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_FULL] > 60 )
		act( "Sei pien$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_THIRST] > 60 )
		act( "La tua sete si e' placata.", ch, NULL, NULL, TO_CHAR, POS_REST );

	if ( obj->value[3] != 0 )
	{
		/* The drink was poisoned ! */
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		act( "$n soffoca violentemente.", ch, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Ti senti soffocare!\n\r", ch );
		af.where = TO_AFFECTS;
		af.type = gsn_poison;
		af.level = number_fuzzy( amount );
		af.duration = 3 * amount;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_POISON;
		affect_join( ch, &af );
	}

	if ( obj->value[0] > 0 )
		obj->value[1] -= amount;
}


void do_eat( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Cosa vuoi mangiare?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_carry(ch, arg)) == NULL )
	{
		send_to_char( "Non hai niente con questo nome.\n\r", ch );
		return;
	}

	if ( !IS_ADMIN(ch) )
	{
		if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
		{
			send_to_char( "Non e' commestibile.\n\r", ch );
			return;
		}

		if ( !IS_MOB(ch) && ch->pcdata->condition[COND_FULL] > 80
		  && (ch->pcdata->condition[COND_HUNGER] > 0) )
		{
			send_to_char( "Hai lo stomaco troppo pieno per mangiare altro.\n\r", ch );
			return;
		}
	}

	act( "$n mangia $p.",  ch, obj, NULL, TO_ROOM, POS_REST );
	act( "Mangi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
	if ( ch->fighting != NULL )
		WAIT_STATE( ch, 3 * PULSE_VIOLENCE );

	switch ( obj->item_type )
	{
	case ITEM_FOOD:
		if ( !IS_MOB(ch) )
		{
			int condition;
			/*~~~~~~~~~~*/

			condition = ch->pcdata->condition[COND_HUNGER];
			gain_condition( ch, COND_FULL, obj->value[0] * 2 );
			gain_condition( ch, COND_HUNGER, obj->value[1] * 2 );
			if ( condition == 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
				send_to_char( "Non hai piu' fame.\n\r", ch );
			else if ( ch->pcdata->condition[COND_FULL] > 80 )
				act( "Sei pien$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
		}

		if ( obj->value[3] != 0 )
		{
			/* The food was poisoned! */
			AFFECT_DATA af;
			/*~~~~~~~~~~~*/

			act( "$n soffoca violentemente.", ch, 0, 0, TO_ROOM, POS_REST );
			send_to_char( "Ti senti soffocare!\n\r", ch );

			af.where = TO_AFFECTS;
			af.type = gsn_poison;
			af.level = number_fuzzy( obj->value[0] );
			af.duration = 2 * obj->value[0];
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_POISON;
			affect_join( ch, &af );
		}
		break;

	case ITEM_PILL:
		obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
		obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
		obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
		break;
	}

	extract_obj( obj );
}


/*
 * Remove an object. Only for non-multi-wear locations
 */
bool remove_obj_loc( CHAR_DATA *ch, int iWear, bool fReplace )
{
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~*/

	if ( (obj = get_eq_char(ch, iWear)) == NULL )
		return( TRUE );

	if ( !fReplace )
		return( FALSE );

	if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
	{
		act( "Non riesci a lasciare $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		return( FALSE );
	}

	if ( (obj->item_type == ITEM_TATTOO) && (!IS_ADMIN(ch)) )
	{
		act( "Come vorresti toglierti  $p? Amputandoti l'arto?", ch, obj, NULL, TO_CHAR, POS_REST );
		return( FALSE );
	}

	if ( iWear == WEAR_STUCK_IN )
	{
		unequip_char( ch, obj );

		if ( get_eq_char(ch, WEAR_STUCK_IN) == NULL )
		{
			if ( is_affected(ch, gsn_arrow) )
				affect_strip( ch, gsn_arrow );
			if ( is_affected(ch, gsn_spear) )
				affect_strip( ch, gsn_spear );
		}

		act( "In una fitta di dolore rimuovi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n si libera di $p con una smorfia di dolore.", ch, obj, NULL, TO_ROOM, POS_REST );
		WAIT_STATE( ch, 4 );
		return( TRUE );
	}

	unequip_char( ch, obj );
	act( "$n smette di usare $p.", ch, obj, NULL, TO_ROOM, POS_REST );
	act( "Smette di usare $p.", ch, obj, NULL, TO_CHAR, POS_REST );

	return( TRUE );
}


/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
	if ( obj == NULL )
		return( TRUE );

	if ( !fReplace )
		return( FALSE );

	if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
	{
		act( "Non riesci a liberarti di $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		return( FALSE );
	}

	if ( (obj->item_type == ITEM_TATTOO) && (!IS_ADMIN(ch)) )
	{
		act( "Non puoi liberarti cosi' facilmente del simbolo della tua fede.", ch, obj, NULL, TO_CHAR, POS_REST );
		return( FALSE );
	}

	if ( obj->wear_loc == WEAR_STUCK_IN )
	{
		unequip_char( ch, obj );

		if ( get_eq_char(ch, WEAR_STUCK_IN) == NULL )
		{
			if ( is_affected(ch, gsn_arrow) )
				affect_strip( ch, gsn_arrow );
			if ( is_affected(ch, gsn_spear) )
				affect_strip( ch, gsn_spear );
		}

		act( "In una fitta di dolore ti liberi di $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n con una smorfia di dolore si libera di $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		WAIT_STATE( ch, 4 );
		return( TRUE );
	}

	unequip_char( ch, obj );
	act( "$n smette di usare $p.", ch, obj, NULL, TO_ROOM, POS_REST );
	act( "Smetti di usare $p.", ch, obj, NULL, TO_CHAR, POS_REST );

	return( TRUE );
}


/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
	int		wear_level;
	/*~~~~~~~~~~~~~~~*/

	wear_level = ch->level;

	if ((class_table[ch->class].fMana && obj->item_type == ITEM_ARMOR)
	||	(!class_table[ch->class].fMana && obj->item_type == ITEM_WEAPON) )
		wear_level += 3;

	if ( wear_level < obj->level )
	{
		send_to_char( "Il livello di potere di questo artefatto e' troppo superiore al tuo.\n\r", ch );
		act( "$n cerca di utilizzare $p, ma ancora gli manca esperienza.", ch, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( obj->item_type == ITEM_LIGHT )
	{
		if ( get_eq_char(ch, WEAR_BOTH) != NULL )
		{
			if ( !remove_obj_loc(ch, WEAR_BOTH, fReplace) )
				return;
			hold_a_light( ch, obj, WEAR_LEFT );
		}
		else if ( get_eq_char(ch, WEAR_LEFT) == NULL )
			hold_a_light( ch, obj, WEAR_LEFT );
		else if ( get_eq_char(ch, WEAR_RIGHT) == NULL )
			hold_a_light( ch, obj, WEAR_RIGHT );
		else if ( remove_obj_loc(ch, WEAR_LEFT, fReplace) )
			hold_a_light( ch, obj, WEAR_LEFT );
		else if ( remove_obj_loc(ch, WEAR_RIGHT, fReplace) )
			hold_a_light( ch, obj, WEAR_RIGHT );
        else
			send_to_char( "Non puoi mantenere un lume, ora.\n\r", ch );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_FINGER) )
	{
		wear_multi( ch, obj, WEAR_FINGER, fReplace );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_NECK) )
	{
		wear_multi( ch, obj, WEAR_NECK, fReplace );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_BODY) )
	{
		if ( !remove_obj_loc(ch, WEAR_BODY, fReplace) )
			return;
		act( "$n indossa $p sul su$x torso.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Indossi $p sul torso.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_BODY );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_HEAD) )
	{
		if ( !remove_obj_loc(ch, WEAR_HEAD, fReplace) )
			return;
		act( "$n si mette $p sul capo.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Metti $p sul capo.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_HEAD );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_LEGS) )
	{
		if ( !remove_obj_loc(ch, WEAR_LEGS, fReplace) )
			return;
		act( "$n infila $p alle gambe.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Infili $p alle gambe.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_LEGS );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_FEET) )
	{
		if ( !remove_obj_loc(ch, WEAR_FEET, fReplace) )
			return;
		act( "$n infila $p ai piedi.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "indossi $p ai piedi.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_FEET );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_HANDS) )
	{
		if ( !remove_obj_loc(ch, WEAR_HANDS, fReplace) )
			return;
		act( "$n indossa $p sulle mani.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Indossi $p sulle mani.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_HANDS );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_ARMS) )
	{
		if ( !remove_obj_loc(ch, WEAR_ARMS, fReplace) )
			return;
		act( "$n mette $p sulle braccia.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Metti $p sulle braccia.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_ARMS );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_ABOUT) )
	{
		if ( !remove_obj_loc(ch, WEAR_ABOUT, fReplace) )
			return;
		act( "$n indossa $p alle spalle.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Indossi $p alle spalle.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_ABOUT );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_WAIST) )
	{
		if ( !remove_obj_loc(ch, WEAR_WAIST, fReplace) )
			return;
		act( "$n allaccia $p alla cintola.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Allacci $p attorno alla cintola.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_WAIST );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_WRIST) )
	{
		wear_multi( ch, obj, WEAR_WRIST, fReplace );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_SHIELD) )
	{
		if ( get_eq_char(ch, WEAR_BOTH) != NULL )
		{
			if ( !remove_obj_loc(ch, WEAR_BOTH, fReplace) )
				return;
			hold_a_shield( ch, obj, WEAR_LEFT );
		}
		else if ( get_eq_char(ch, WEAR_LEFT) == NULL )
			hold_a_shield( ch, obj, WEAR_LEFT );
		else if ( get_eq_char(ch, WEAR_RIGHT) == NULL )
			hold_a_shield( ch, obj, WEAR_RIGHT );
		else if ( remove_obj_loc(ch, WEAR_LEFT, fReplace) )
			hold_a_shield( ch, obj, WEAR_LEFT );
		else if ( remove_obj_loc(ch, WEAR_RIGHT, fReplace) )
			hold_a_shield( ch, obj, WEAR_RIGHT );
		else
			send_to_char( "Non hai la possibilita' di reggere uno scudo in questo momento.\n\r", ch );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WIELD) )
	{
		wear_a_wield( ch, obj, fReplace );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_HOLD) )
	{
		if ( get_eq_char(ch, WEAR_BOTH) != NULL )
		{
			if ( !remove_obj_loc(ch, WEAR_BOTH, fReplace) )
				return;
			hold_a_thing( ch, obj, WEAR_LEFT );
		}
		else if ( get_eq_char(ch, WEAR_LEFT) == NULL )
			hold_a_thing( ch, obj, WEAR_LEFT );
		else if ( get_eq_char(ch, WEAR_RIGHT) == NULL )
			hold_a_thing( ch, obj, WEAR_RIGHT );
		else if ( remove_obj_loc(ch, WEAR_LEFT, fReplace) )
			hold_a_thing( ch, obj, WEAR_LEFT );
		else if ( remove_obj_loc(ch, WEAR_RIGHT, fReplace) )
			hold_a_thing( ch, obj, WEAR_RIGHT );
        else
			send_to_char( "Non puoi tenere in mano nulla in questo momento.\n\r", ch );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_FLOAT) )
	{
		if ( !remove_obj_loc(ch, WEAR_FLOAT, fReplace) )
			return;
		act( "$n lascia fluttuare $p accanto a se'.", ch, obj, NULL, TO_ROOM, POS_REST );
		act( "Lasci fluttuare $p accanto a te.", ch, obj, NULL, TO_CHAR, POS_REST );
		equip_char( ch, obj, WEAR_FLOAT );
		return;
	}

	if ( CAN_WEAR(obj, ITEM_WEAR_TATTOO) && IS_ADMIN(ch) )
	{
		wear_multi( ch, obj, WEAR_TATTOO, fReplace );
		return;
	}

	if ( fReplace )
		send_to_char( "Non puoi indossarlo o tenerlo in mano..\n\r", ch );
}


void do_wear( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Che cosa vuoi usare?\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "all") || !str_cmp(arg, "tutti") )
	{
		OBJ_DATA	*obj_next;
		/*~~~~~~~~~~~~~~~~~~*/

		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) )
				wear_obj( ch, obj, FALSE );
		}
	}
	else
	{
		if ( (obj = get_obj_carry(ch, arg)) == NULL )
		{
			send_to_char( "Non hai nessun oggetto con questo nome.\n\r", ch );
			return;
		}

		wear_obj( ch, obj, TRUE );
	}
}


void do_remove( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Cosa vuoi smettere di usare?\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "all") || !str_cmp(arg, "tutti") )
	{
		OBJ_DATA	*obj_next;
		/*~~~~~~~~~~~~~~~~~~*/

		if ( ch->position == POS_FIGHT )
		{
			send_to_char( "Non puoi svestirti mentre stai combattendo.\n\r", ch );
			return;
		}

		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( obj->wear_loc != WEAR_NONE )
				remove_obj( ch, obj, TRUE );
		}

		return;
	}

	if ( (obj = get_obj_wear(ch, arg)) == NULL )
	{
		send_to_char( "Non hai nessun oggetto con questo nome.\n\r", ch );
		return;
	}

	remove_obj( ch, obj, TRUE );
}


void do_sacrifice( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		buf[MSL];
	char		buf2[MSL];

	OBJ_DATA	*obj;
	OBJ_DATA	*obj_content;
	OBJ_DATA	*obj_next;
	OBJ_DATA	*two_objs[2];
	int			silver;
	int			iScatter;
	bool		fScatter;
	sh_int		i;
	/* variables for AUTOSPLIT */
	CHAR_DATA	*gch;
	int			members;
	char		buffer[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Che cosa vorresti sacrificare?", ch );
		return;
	}

	if ( !str_cmp(arg, ch->name) )
	{
		act( "$n cerca di sacrificarsi agli Dei..", ch, NULL, NULL, TO_ROOM, POS_REST );
		send_to_char( "Gli Dei apprezzano la tua generosita' e ritireranno la tua anima quanto prima..\n\r", ch );
		return;
	}

	/* Sacrifica solo se l'oggetto e' per terra */
	obj = get_obj_list( ch, arg, ch->in_room->contents );
	if ( obj == NULL )
	{
		obj = get_obj_carry( ch, arg );
		if ( obj == NULL )
			send_to_char( "Non trovi nessun oggetto di questo tipo.", ch );
		else
			send_to_char( "Per compiere il rituale di sacrificio devi lasciare che l'oggetto sia libero, depositalo in terra.\n\r", ch );

		return;
	}

	if ( obj->item_type == ITEM_CORPSE_PC && ch->level < LEVEL_ADMIN )
	{
		send_to_char( "Gli Dei non te lo permettono.\n\r", ch );
		return;
	}

	if ( !CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC) )
	{
		act( "$p non e' un oggetto valido da sacrificare.", ch, obj, 0, TO_CHAR, POS_REST );
		return;
	}

	for ( i = 1; i < MAX_CABAL; i++ )
	{
		if ( obj->pIndexData->vnum == cabal_table[i].obj_vnum )
		{
			send_to_char( "Gli Dei si infurierebbero se questo oggetto fosse sacrificato.\n\r", ch );
			return;
		}
	}

	silver = UMAX( 1, number_fuzzy(obj->level) );

	if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC )
		silver = UMIN( silver, obj->cost );

	sprintf( buf,"Gli Dei ti danno %d monet%c d'argento per il sacrificio.\n\r",
		silver, (silver == 1) ? 'a' : 'e' );
	send_to_char( buf, ch );

	ch->silver += silver;

	if ( IS_SET(ch->act, PLR_AUTOSPLIT) )
	{	/* AUTOSPLIT code */
		members = 0;
		for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
		{
			if ( is_same_group(gch, ch) )
				members++;
		}

		if ( members > 1 && silver > 1 )
		{
			sprintf( buffer, "%d", silver );
			do_split( ch, buffer );
		}
	}

	act( "$n sacrifica $p agli Dei.", ch, obj, NULL, TO_ROOM, POS_REST );

	if ( IS_SET(obj->progtypes, OPROG_SAC) )
	{
		if ( (obj->pIndexData->oprogs->sac_prog) (obj, ch) )
			return;
	}

	wiznet( "$N sacrifica $p.", ch, obj, WIZ_SACCING, 0 );
	fScatter = TRUE;
	if ( (obj->item_type == ITEM_CORPSE_NPC)
	||	 (obj->item_type == ITEM_CORPSE_PC)
	||	  obj->item_type == ITEM_CONTAINER )	/* Prima gli ogg nei contenitori.. puff */
	{
		iScatter = 0;
		for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
		{
			obj_next = obj_content->next_content;
			two_objs[iScatter < 1 ? 0 : 1] = obj_content;
			obj_from_obj( obj_content );
			obj_to_room( obj_content, ch->in_room );
			iScatter++;
		}

		if ( iScatter == 1 )
		{
			act( "Il tuo sacrificio rivela la presenza di $p.", ch, two_objs[0], NULL, TO_CHAR, POS_REST);
			act( "Il sacrificio di $n rivela la presenza di $p.", ch, two_objs[0], NULL, TO_ROOM, POS_REST);
		}

		if ( iScatter == 2 )
		{
			act( "Il tuo sacrificio rivela la presenza di $p e $P.", ch, two_objs[0], two_objs[1], TO_CHAR, POS_REST );
			act( "Il sacrificio di $n rivela la presenza di $p e $P.", ch, two_objs[0], two_objs[1], TO_ROOM, POS_REST );
		}

		sprintf( buf, "All'atto del sacrificio del corpo, " );
		sprintf( buf2, "All'atto del sacrificio del corpo da parte di $n, ");

		if ( iScatter < 3 )
			fScatter = FALSE;
		else if ( iScatter < 5 )
		{
			strcat( buf, "alcuni oggetti " );
			strcat( buf2, "alcuni oggetti " );
		}
		else if ( iScatter < 9 )
		{
			strcat( buf, "una moltitudine di oggetti " );
			strcat( buf2, "una moltitudine di oggetti " );
		}
		else if ( iScatter < 15 )
		{
			strcat( buf, "tantissimi oggetti " );
			strcat( buf2, "tantissimi oggetti " );
		}
		else
		{
			strcat( buf, "degli oggetti " );
			strcat( buf2, "degli oggetti " );
		}
		strcat( buf, "su questo, " );
		strcat( buf2, "su questo, " );

		switch ( ch->in_room->sector_type )
		{
		case SECT_FIELD:		strcat( buf, "si sparpagliano intorno." );			strcat( buf2, "si sparpagliano intorno." );				break;
		case SECT_FOREST:		strcat( buf, "si sparpagliano intorno." );			strcat( buf2, "si sparpagliano intorno." );				break;
		case SECT_WATER_SWIM:	strcat( buf, "si sparpagliano sul velo d'acqua." );	strcat( buf2, "si sparpagliano sul velo d'acqua." );	break;
		case SECT_WATER_NOSWIM:	strcat( buf, "si sparpagliano sul velo d'acqua." );	strcat( buf2, "si sparpagliano sul velo d'acqua." );	break;
		default:				strcat( buf, "si sparpagliano intorno." );			strcat( buf2, "si sparpagliano intorno." );				break;
		}

		if ( fScatter )
		{
			act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
			act( buf2, ch, NULL, NULL, TO_ROOM, POS_REST );
		}
	}

	extract_obj( obj );
}


void do_junk( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	char		arg2[MIL];
	OBJ_DATA	*obj;
	sh_int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );
	one_argument( argument, arg2 );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Quale oggetto vorresti buttare?\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, ch->name) )
	{
		send_to_char( "Ti senti proprio cosi' da buttare?\n\r", ch );
		return;
	}

	obj = get_obj_carry( ch, arg );

	/* Cerca nell'inventario */
	if ( obj == NULL )
	{
		/* Cerca nella stanza */
		obj = get_obj_list( ch, arg, ch->in_room->contents );
		if ( obj == NULL )
			send_to_char( "Non trovi nessun oggetto da buttare qui in giro", ch );
		else
			send_to_char( "Non hai nell'inventario nessun oggetto di quel tipo da buttare.\n\r", ch );

		return;
	}

	if ( (obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC)
	  && ch->level < LEVEL_ADMIN )
	{
		send_to_char( "Piu' buttato di cosi', si muore..\n\r", ch );
		return;
	}

	/* Non puo' farlo se e' un contenitore pieno e se non ha confermato */
	if ( obj->item_type == ITEM_CONTAINER && str_cmp(arg2, "si") )
	{
		send_to_char( "L'oggetto e' un contenitore, devi confermare con un 'si' se vuoi buttarlo ed eventualmente tutto cio' che v'e' al suo interno.", ch );
		return;
	}

	if ( !CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC) )
	{
		act( "$p non ti e' permesso buttare via questo oggetto.", ch, obj, NULL, TO_CHAR, POS_REST );
		return;
	}

	for ( i = 1; i < MAX_CABAL; i++ )
	{
		if ( obj->pIndexData->vnum == cabal_table[i].obj_vnum )
		{
			send_to_char( "Gli Dei si infurierebbero se questo oggetto fosse buttato via.\n\r", ch );
			return;
		}
	}

	act( "Butti via $p", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$n butta via $p.", ch, obj, NULL, TO_ROOM, POS_REST );

	/* Dovrebbe scattare con il sac.. ma teniamolo anche per il junk va' */
	if ( IS_SET(obj->progtypes, OPROG_SAC) )
	{
		if ( (obj->pIndexData->oprogs->sac_prog) (obj, ch) )
			return;
	}

	extract_obj( obj );
}


void do_quaff( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( ch->cabal == CABAL_ARMY && !IS_ADMIN(ch) )
	{
		send_to_char( "Sei un impavido guerriero, cosa te ne fai di quella roba?!\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Quale pozione vuoi bere?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_carry(ch, arg)) == NULL )
	{
		send_to_char( "Non hai pozioni simili.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_POTION )
	{
		send_to_char( "Puoi trangugiare solo pozioni.\n\r", ch );
		return;
	}

	if ( ch->level < obj->level )
	{
		send_to_char( "Non hai abbastanza potere per trangugiare questa pozione.\n\r", ch );
		return;
	}

	act( "$n beve $p.", ch, obj, NULL, TO_ROOM , POS_REST );
	act( "Tracanni $p.", ch, obj, NULL ,TO_CHAR , POS_REST );

	obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );

	if ( (ch->last_fight_time != -1 && (current_time - ch->last_fight_time) < FIGHT_DELAY_TIME) || (ch->fighting != NULL) )
	{
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	}

	extract_obj( obj );
	obj_to_char( create_object(get_obj_index(OBJ_VNUM_POTION_VIAL), 0), ch );

	if ( IS_MOB(ch) )
		do_drop( ch, "fiala" );

}


void do_recite( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*scroll;
	OBJ_DATA	*obj;
	sh_int		malus;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->cabal == CABAL_ARMY )
	{
		send_to_char( "Sei un GUERRIERO, non hai bisogno di alcuna poesiola!\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( (scroll = get_obj_carry(ch, arg1)) == NULL )
	{
		send_to_char( "Non hai questo incantesimo.\n\r", ch );
		return;
	}

	if ( scroll->item_type != ITEM_SCROLL )
	{
		send_to_char( "Puoi recitare solo incantesimi.\n\r", ch );
		return;
	}

	if ( ch->level < scroll->level )
	{
		send_to_char( "L'incantesimo e' troppo complesso per il tuo livello di potere.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg2[0] == '\0' )
	{
		victim = ch;
	}
	else
	{
		if ( (victim = get_char_room(ch, arg2)) == NULL && (obj = get_obj_here(ch, arg2)) == NULL )
		{
			send_to_char( "Non riesci a trovare nulla di simile.\n\r", ch );
			return;
		}
	}

	act( "$n recita $p.", ch, scroll, NULL, TO_ROOM, POS_REST );
	act( "Reciti solennemente $p.", ch, scroll, NULL, TO_CHAR , POS_REST );

	if ((ch->class != CLASS_INVOKER)
	||	(ch->class != CLASS_CLERIC)
	||	(ch->class != CLASS_TRANSMUTER)
	||	(ch->class != CLASS_VAMPIRE)
	||	(ch->class != CLASS_NECROMANCER)
	||	(ch->class != CLASS_ELEMENTALIST) )
		malus = -20;
	else
		malus = 0;


	if ( number_percent() >= malus + get_skill(ch, gsn_scrolls) * 4 / 5 )
	{
		send_to_char( "Incespichi su qualche sillaba.\n\r", ch );
		check_improve( ch, gsn_scrolls, FALSE, 2 );
	}
	else
	{
		obj_cast_spell( scroll->value[1], scroll->value[0], ch, victim, obj );
		obj_cast_spell( scroll->value[2], scroll->value[0], ch, victim, obj );
		obj_cast_spell( scroll->value[3], scroll->value[0], ch, victim, obj );
		check_improve( ch, gsn_scrolls, TRUE, 2 );

		if ( (ch->last_fight_time != -1 && (current_time - ch->last_fight_time) < FIGHT_DELAY_TIME)
		|| (ch->fighting != NULL) )
		{
			WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		}
	}

	extract_obj( scroll );
}


void do_brandish( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*vch;
	CHAR_DATA	*vch_next;
	OBJ_DATA	*staff;
	int			sn;
	sh_int		malus;
	/*~~~~~~~~~~~~~~~~~~*/

	if ( ch->cabal == CABAL_ARMY )
	{
		send_to_char( "Non sei un inutile mago!\n\r", ch );
		return;
	}

	if ( (staff = get_hold_char(ch)) == NULL )
	{
		send_to_char( "Non hai nulla da brandire.\n\r", ch );
		return;
	}

	if ( staff->item_type != ITEM_STAFF )
	{
		send_to_char( "Puoi brandire solo staffe.\n\r", ch );
		return;
	}

	if ( (sn = staff->value[3]) < 0 || sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
	{
		bug( "Do_brandish: bad sn %d.", sn );
		return;
	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	if ( staff->value[2] > 0 )
	{
		act( "$n brandisce $p.", ch, staff, NULL, TO_ROOM , POS_REST );
		act( "Brandisci $p.", ch, staff, NULL, TO_CHAR , POS_REST );

		if ((ch->class != CLASS_INVOKER)
		||	(ch->class != CLASS_CLERIC)
		||	(ch->class != CLASS_TRANSMUTER)
		||	(ch->class != CLASS_VAMPIRE)
		||	(ch->class != CLASS_NECROMANCER)
		||	(ch->class != CLASS_ELEMENTALIST) )
			malus = -10;
		else
			malus = 10;

		if ( ch->level + 3 < staff->level || number_percent() >= malus + get_skill(ch, gsn_staves) * 4 / 5 )
		{
			act( "Non riesci a invocare il potere da $p.", ch, staff, NULL, TO_CHAR, POS_REST );
			act( ".. e non succede nulla.", ch, NULL, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_staves, FALSE, 2 );
		}
		else
		{
			for ( vch = ch->in_room->people; vch; vch = vch_next )
			{
				vch_next = vch->next_in_room;

				switch ( skill_table[sn].target )
				{
				default:					bug( "Do_brandish: bad target for sn %d.", sn ); return;
				case TAR_IGNORE:			if ( vch != ch ) continue; break;
				case TAR_CHAR_OFFENSIVE:	if ( IS_MOB(ch) ? IS_MOB(vch) : !IS_MOB(vch) ) continue; break;
				case TAR_CHAR_DEFENSIVE:	if ( IS_MOB(ch) ? !IS_MOB(vch) : IS_MOB(vch) ) continue; break;
				case TAR_CHAR_SELF:			if ( vch != ch ) continue; break;
				}

				obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
				check_improve( ch, gsn_staves, TRUE, 2 );
			}
		}
	}

	if ( --staff->value[2] <= 0 )
	{
		act( "$p brilla per un istante nelle mani di $n per poi esplodere in mille frammenti.", ch, staff, NULL, TO_ROOM , POS_REST);
		act( "$p brilla per un istante prima di esplodere in mille frammenti.", ch, staff, NULL, TO_CHAR , POS_REST);
		extract_obj( staff );
	}
}


void do_zap( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	OBJ_DATA	*wand;
	OBJ_DATA	*obj;
	sh_int		malus;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->cabal == CABAL_ARMY )
	{
		send_to_char( "Sei un Combattente, non un maghetto da quattro soldi!\n\r", ch );
		return;
	}

	one_argument( argument, arg );
	if ( arg[0] == '\0' && ch->fighting == NULL )
	{
		send_to_char( "Chi o cosa?\n\r", ch );
		return;
	}

	if ( (wand = get_hold_char(ch)) == NULL )
	{
		send_to_char( "Non hai nulla in mano!\n\r", ch );
		return;
	}

	if ( wand->item_type != ITEM_WAND )
	{
		send_to_char( "Hai bisogno di una bacchetta magica per farlo.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg[0] == '\0' )
	{
		if ( ch->fighting != NULL )
			victim = ch->fighting;
		else
		{
			send_to_char( "Chi o cosa?\n\r", ch );
			return;
		}
	}
	else
	{
		if ( (victim = get_char_room(ch, arg)) == NULL && (obj = get_obj_here(ch, arg)) == NULL )
		{
			send_to_char( "Non vedi nulla di simile.\n\r", ch );
			return;
		}
	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	if ( wand->value[2] > 0 )
	{
		if ( victim != NULL )
		{

			act( "$n evoca il potere di $p su $N.", ch, wand, victim, TO_ROOM , POS_REST);
			act( "Evochi il potere di $p su $N $p.", ch, wand, victim, TO_CHAR , POS_REST);
		}
		else
		{
			act( "$n colpisce $P con $p.", ch, wand, obj, TO_ROOM , POS_REST);
			act( "Colpisci $P con $p.", ch, wand, obj, TO_CHAR , POS_REST);
		}

		if ((ch->class != CLASS_INVOKER)
		||	(ch->class != CLASS_CLERIC)
		||	(ch->class != CLASS_TRANSMUTER)
		||	(ch->class != CLASS_VAMPIRE)
		||	(ch->class != CLASS_NECROMANCER)
		||	(ch->class != CLASS_ELEMENTALIST) )
			malus = 0;
		else
			malus = 20;

		if ( ch->level + 5 < wand->level || number_percent() >= malus + get_skill(ch, gsn_wands) * 4 / 5 )
		{
			act( "$p emette scintille e fumo.. nulla di fatto.", ch,wand,NULL,TO_CHAR, POS_REST);
			act( "$n punta $p ma ottiene solo fumo e scintille..", ch,wand,NULL,TO_ROOM, POS_REST);
			check_improve( ch, gsn_wands, FALSE, 2 );
		}
		else
		{
			obj_cast_spell( wand->value[3], wand->value[0], ch, victim, obj );
			check_improve( ch, gsn_wands, TRUE, 2 );
		}
	}

	if ( --wand->value[2] <= 0 )
	{
		act( "$p si frammenta nelle mani di $n.", ch, wand, NULL, TO_ROOM , POS_REST);
		act( "$p si frammenta nelle tue mani.", ch, wand, NULL, TO_CHAR , POS_REST);
		extract_obj( wand );
	}
}


void do_steal( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	CHAR_DATA	*tmp_ch;
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_inve;

	int			percent, number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Rubare cosa a chi?\n\r", ch );
		return;
	}

	if ( IS_MOB(ch) && IS_SET(ch->affected_by, AFF_CHARM) && (ch->master != NULL) )
	{
		act( "Sei troppo intontit$x per rubare qualcosa a chiunque.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( (victim = get_char_room(ch, arg2)) == NULL )
	{
		send_to_char( "Non e' qui.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) && victim->desc == NULL )
	{
		send_to_char("Non puoi farlo.\n\r", ch);
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Grande idea.. \n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( victim->position == POS_FIGHT )
	{
		send_to_char("Non durante un combattimento.. si muove troppo.\n\r",ch);
		return;
	}

	ch->last_death_time = -1;

	tmp_ch = ( is_affected(ch, gsn_doppelganger) && !IS_ADMIN(victim) ) ? ch->doppel : ch;

	WAIT_STATE( ch, skill_table[gsn_steal].beats );
	percent = get_skill( ch, gsn_steal ) + ( IS_AWAKE(victim) ? -10 : 30 );
	percent -= can_see( victim, ch ) ? 10 : 0;
	percent -= ( IS_MOB(victim) && victim->pIndexData->pShop != NULL ) ? 25 : 0;
	percent -= ( victim->level > ch->level ) ? ( (victim->level - ch->level) * 2 ) : 0;

	obj = NULL;
	if (str_cmp(arg1, "coin")
	&&	str_cmp(arg1, "coins")
	&&	str_cmp(arg1, "moneta")
	&&	str_cmp(arg1, "monete")
	&&	str_cmp(arg1, "silver")
	&&	str_cmp(arg1, "gold")
	&&	str_cmp(arg1, "argento")
	&&	str_cmp(arg1, "oro") )
	{
		if ( (obj = get_obj_carry(victim, arg1)) == NULL )
		{
			send_to_char( "Non trovi nulla di simile.\n\r", ch );
			return;

		}
	}

	if ( obj != NULL )
	{
		if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch))
		||	(IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch))
		||	(IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)) )
		{
			ch->point[LIFE] -= ch->level;
			act( "Vieni colpit$x da $p.", ch, obj, NULL, TO_CHAR , POS_REST);
			act( "$n viene colpit$x da $p.",  ch, obj, NULL, TO_ROOM , POS_REST);
			percent = 0;
		}

		/* Controllo per le slay */
		if ( (IS_EVIL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYEVIL))
		||	 (IS_GOOD(ch) && IS_OBJ_STAT(obj, ITEM_SLAYGOOD))
		||	 (IS_NEUTRAL(ch) && IS_OBJ_STAT(obj, ITEM_SLAYNEUTRAL)) )
		{
			ch->point[LIFE] -= ch->level*2;
			act( "L'aura di $p avvampa sulla tua mano costringendoti a liberartene!", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n geme di dolore mentre butta a terra  $p.",  ch, obj, NULL, TO_ROOM, POS_REST );
			percent = 0;
		}

		if ( obj->pIndexData->limit != -1 && obj->pIndexData->limit < obj->pIndexData->count )
		{
			ch->point[LIFE] -= ch->level*3;
			act( "Gli dei non permettono che si rubi $p.", ch, obj, NULL, TO_CHAR , POS_REST);
			act( "Un'ombra gelida cala su $n.",  ch, obj, NULL, TO_ROOM , POS_REST);
			percent = 0;
		}
	}

	number = ( obj != NULL ) ? get_obj_number( obj ) : 0;

	if ( ch->carry_number + number > can_carry_n(ch) )
	{
		send_to_char( "Non puoi portare cosi' tanti oggetti.\n\r", ch );
		return;
	}

	if ( victim->position == POS_FIGHT || number_percent() > percent )
	{
		/* Failure. */
		send_to_char( "Oops.\n\r", ch );
		if ( !IS_AFFECTED(victim, AFF_SLEEP) )
		{
			victim->position = victim->position == POS_SLEEP ? POS_STAND : victim->position;
			act( "$n ha cercato di derubarti!", ch, NULL, victim, TO_VICT , POS_REST );
		}
		act( "$n ha cercato di derubare $N.",  ch, NULL, victim, TO_NOTVICT, POS_REST );

		switch ( number_range(0, 3) )
		{
		case 0:	sprintf( buf, "%s ha cercato di derubarmi!", tmp_ch->name );	break;
		case 1:	sprintf( buf, "Al ladro!Al ladro!" );							break;
		case 2: sprintf( buf,"%s cerca di derubarmi!",tmp_ch->name );			break;
		case 3:	sprintf(buf,"Tieni le mani lontane da me, %s!",tmp_ch->name);	break;
		}

		if ( IS_AWAKE(victim) )
			do_yell( victim, buf );

		if ( !IS_MOB(ch) )
		{
			if ( IS_MOB(victim) )
			{
				check_improve( ch, gsn_steal, FALSE, 2 );
				multi_hit( victim, ch, TYPE_UNDEFINED );
			}
		}

		return;
	}

	if ( !str_cmp(arg1, "moneta") || !str_cmp(arg1, "monete") || !str_cmp(arg1, "argento") || !str_cmp(arg1, "oro")
	||	 !str_cmp(arg1, "coin") || !str_cmp(arg1, "coins") || !str_cmp(arg1, "silver") || !str_cmp(arg1, "gold") )
	{
		int amount_s = 0;
		int amount_g = 0;
		/*~~~~~~~~~~~~~*/

		if ( !str_cmp(arg1, "argento") || !str_cmp(arg1, "moneta") || !str_cmp(arg1, "monete")
		||	 !str_cmp(arg1, "silver") || !str_cmp(arg1, "coin") || !str_cmp(arg1, "coins") )
			amount_s = victim->silver * number_range( 1, 20 ) / 100;
		else if ( !str_cmp(arg1, "gold") || !str_cmp(arg1, "oro") )
			amount_g = victim->gold * number_range( 1, 7 ) / 100;

		if ( amount_s <= 0 && amount_g <= 0 )
		{
			send_to_char( "Non puoi prendere altre monete.\n\r", ch );
			return;
		}

		ch->gold += amount_g;
		victim->gold -= amount_g;
		ch->silver += amount_s;
		victim->silver -= amount_s;
		sprintf( buf, "Soldi facili! Hai preso %d monet%c d'%s.\n\r",
			(amount_s != 0) ? amount_s : amount_g,
			(amount_s != 0) ?  (amount_s == 1) ? 'a' : 'e'  :  (amount_g == 1) ? 'a' : 'e',
			(amount_s != 0) ? "argento" : "oro" );
		send_to_char( buf, ch );

		check_improve( ch, gsn_steal, TRUE, 2 );
		return;
	}

	if ( (obj = get_obj_carry(victim, arg1)) == NULL )
	{
		send_to_char( "Non ti pare di trovare nulla di quel genere.\n\r", ch );
		return;
	}

	if ( !can_drop_obj(ch, obj) /* || IS_SET(obj->extra_flags, ITEM_INVENTORY) */ /* || obj->level > ch->level */ )
	{
		send_to_char( "Non riesci a liberartene.\n\r", ch );
		return;
	}

	if ( ch->carry_number + get_obj_number(obj) > can_carry_n(ch) )
	{
		send_to_char( "Hai le mani piene.\n\r", ch );
		return;
	}

	if ( ch->carry_weight + get_obj_weight(obj) > can_carry_w(ch) )
	{
		send_to_char( "Non puoi sopportare altro peso.\n\r", ch );
		return;
	}

	if ( !IS_SET(obj->extra_flags, ITEM_INVENTORY) )
	{
		obj_from_char( obj );
		obj_to_char( obj, ch );
		send_to_char( "Ce l'hai fatta!\n\r", ch );
		check_improve( ch, gsn_steal, TRUE, 2 );
		if ( IS_SET(obj->progtypes, OPROG_GET) )
			( obj->pIndexData->oprogs->get_prog ) ( obj, ch );
	}
	else
	{
		obj_inve = NULL;
		obj_inve = create_object( obj->pIndexData, 0 );
		clone_object( obj, obj_inve );
		REMOVE_BIT( obj_inve->extra_flags, ITEM_INVENTORY );
		obj_to_char( obj_inve, ch );
		send_to_char( "Uno.. !\n\r", ch );
		check_improve( ch, gsn_steal, TRUE, 1 );
		if ( IS_SET(obj_inve->progtypes, OPROG_GET) )
			( obj_inve->pIndexData->oprogs->get_prog ) ( obj_inve, ch );
	}
}


/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
	char		buf[MSL];
	CHAR_DATA	*keeper;
	SHOP_DATA	*pShop;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pShop = NULL;
	for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
	{
		if ( IS_MOB(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
			break;
	}

	if ( pShop == NULL )
	{
		send_to_char( "Non puoi farlo qui.\n\r", ch );
		return( NULL );
	}

	if ( IS_SET(keeper->in_room->area->area_flag, AREA_HOMETOWN) && !IS_MOB(ch) && IS_SET(ch->act, PLR_WANTED) )
	{
		do_say( keeper, "I criminali non sono i benvenuti qui!" );
		sprintf( buf, "%s e' qui! Accorrete!\n\r", ch->name );
		do_yell( keeper, buf );
		return( NULL );
	}

	/* Shop hours. */
	if ( time_info.hour < pShop->open_hour )
	{
		do_say( keeper, "Al momento il negozio e' chiuso, ripassate piu' tardi." );
		return( NULL );
	}


	if ( time_info.hour > pShop->close_hour )
	{
		do_say( keeper, "Al momento il negozio e' chiuso, ripassate domani." );
		return( NULL );
	}

	/* Invisible or hidden people. */
	if ( !can_see(keeper, ch) && !IS_ADMIN(ch) )
	{
		do_say( keeper, "Fatti vedere, dopo parlero' con te." );
		return( NULL );
	}

	return( keeper );
}


/*
 * insert an object at the right spot for the keeper
 */
void obj_to_keeper( OBJ_DATA *obj, CHAR_DATA *ch )
{
	OBJ_DATA	*t_obj, *t_obj_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* see if any duplicates are found */
	for ( t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next )
	{
		t_obj_next = t_obj->next_content;

		if ( obj->pIndexData == t_obj->pIndexData && !str_cmp(obj->short_descr, t_obj->short_descr) )
		{
			if ( IS_OBJ_STAT(t_obj, ITEM_INVENTORY) )
			{
				extract_obj( obj );
				return;
			}

			obj->cost = t_obj->cost;	/* keep it standard */
			break;
		}
	}

	if ( t_obj == NULL )
	{
		obj->next_content = ch->carrying;
		ch->carrying = obj;
	}
	else
	{
		obj->next_content = t_obj->next_content;
		t_obj->next_content = obj;
	}

	obj->carried_by = ch;
	obj->in_room = NULL;
	obj->in_obj = NULL;
	ch->carry_number += get_obj_number( obj );
	ch->carry_weight += get_obj_weight( obj );
}


/*
 * get an object from a shopkeeper's list
 */
OBJ_DATA *get_obj_keeper( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	int			number;
	int			count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	number = number_argument( argument, arg );
	count = 0;
	for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE && can_see_obj(keeper, obj) && can_see_obj(ch, obj) && is_name(arg, obj->name) )
		{
			if ( ++count == number )
				return( obj );

			/* skip other objects of the same name */
			while ( obj->next_content != NULL
			&&	obj->pIndexData == obj->next_content->pIndexData
			&&	!str_cmp(obj->short_descr, obj->next_content->short_descr) )
			{
				obj = obj->next_content;
			}
		}
	}

	return( NULL );
}


int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
	SHOP_DATA	*pShop;
	int			cost;
	/*~~~~~~~~~~~~~~~*/

	if ( obj == NULL || (pShop = keeper->pIndexData->pShop) == NULL )
		return( 0 );

	if ( IS_OBJ_STAT(obj, ITEM_NOSELL) )
		return( 0 );

	if ( fBuy )
	{
		cost = obj->cost * pShop->profit_buy / 100;
	}
	else
	{
		OBJ_DATA	*obj2;
		int			itype;
		/*~~~~~~~~~~~~~~*/

		cost = 0;
		for ( itype = 0; itype < MAX_TRADE; itype++ )
		{
			if ( obj->item_type == pShop->buy_type[itype] )
			{
				cost = obj->cost * pShop->profit_sell / 100;
				break;
			}
		}

		if ( !IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT) )
		{

			for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
			{
				if ( obj->pIndexData == obj2->pIndexData && !str_cmp(obj->short_descr, obj2->short_descr) )
					return( 0 );

				/* if (IS_OBJ_STAT(obj2,ITEM_INVENTORY)) cost /= 2; else cost = cost * 3 / 4; */
			}
		}
	}

	if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
	{
		if ( obj->value[1] == 0 )
			cost /= 4;
		else
			cost = cost * obj->value[2] / obj->value[1];
	}

	return( cost );
}


void do_buy( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	int		cost, roll;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] == '\0' )
	{
		send_to_char( "Cosa vuoi comprare?\n\r", ch );
		return;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
	{
		char			arg[MIL];
		CHAR_DATA		*pet;
		ROOM_INDEX_DATA *pRoomIndexNext;
		ROOM_INDEX_DATA *in_room;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* added by kio */
		smash_tilde( argument );

		if ( IS_MOB(ch) )
			return;

		argument = one_argument( argument, arg );

		/* hack to make new thalos pets work */
		if ( ch->in_room->vnum == 9621 )
			pRoomIndexNext = get_room_index( 9706 );
		else
			pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
		if ( pRoomIndexNext == NULL )
		{
			bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
			send_to_char( "Spiacente.. non puoi comprare nulla di simile qui.\n\r", ch );
			return;
		}

		in_room = ch->in_room;
		ch->in_room = pRoomIndexNext;
		pet = get_char_room( ch, arg );
		ch->in_room = in_room;

		if ( pet == NULL || !IS_SET(pet->act, ACT_PET) || !IS_MOB(pet) )
		{
			send_to_char( "Spiacente, non puoi comprare nulla di simile qui.\n\r", ch );
			return;
		}

		if ( IS_SET(pet->act, ACT_RIDEABLE) && ch->cabal == CABAL_KNIGHT && !MOUNTED(ch) )
		{
			cost = 10 * pet->level * pet->level;

			if ( (ch->silver + 100 * ch->gold) < cost )
			{
				send_to_char( "Non hai abbastanza denaro.\n\r", ch );
				return;
			}

			if ( ch->level < pet->level + 5 )
			{
				send_to_char( "Non hai abbastanza esperienza per trattare con un simile 'cucciolo'.\n\r", ch );
				return;
			}

			deduct_cost( ch, cost );
			pet = create_mobile( pet->pIndexData );
			pet->comm = COMM_NOTELL | COMM_NOYELL | COMM_NOCHANNELS;

			char_to_room( pet, ch->in_room );
			do_mount( ch, pet->name );
			send_to_char( "Ecco pronta la tua cavalcatura.\n\r", ch );
			act( "$n compra $N.", ch, NULL, pet, TO_ROOM, POS_REST );
			return;
		}

		if ( ch->pet != NULL )
		{
			send_to_char( "Hai gia' un cucciolo da accudire.\n\r", ch );
			return;
		}

		cost = 10 * pet->level * pet->level;

		if ( (ch->silver + 100 * ch->gold) < cost )
		{
			send_to_char( "Non hai abbastanza denaro.\n\r", ch );
			return;
		}

		if ( ch->level < pet->level )
		{
			send_to_char( "Non hai abbastanza esperienza per una montatura simile.\n\r", ch );
			return;
		}

		/* haggle */
		roll = number_percent();
		if ( roll < get_skill(ch, gsn_haggle) )
		{
			cost -= cost / 2 * roll / 100;
			sprintf( buf,"Riesci a contrattare il prezzo di %d monet%c.\n\r",
				cost, (cost == 1) ? 'a' : 'e' );
			send_to_char( buf, ch );
			check_improve( ch, gsn_haggle, TRUE, 4 );
		}

		deduct_cost( ch, cost );
		pet = create_mobile( pet->pIndexData );
		SET_BIT( pet->act, ACT_PET );
		SET_BIT( pet->affected_by, AFF_CHARM );
		pet->comm = COMM_NOTELL | COMM_NOYELL | COMM_NOCHANNELS;

		argument = one_argument( argument, arg );
		if ( arg[0] != '\0' )
		{
			sprintf( buf, "%s %s", pet->name, arg );
			free_string( pet->name );
			pet->name = str_dup( buf );
		}

		sprintf( buf, "%s ha una targhetta che recita 'Appartengo a %s'.\n\r", pet->description, ch->name );
		free_string( pet->description );
		pet->description = str_dup( buf );

		char_to_room( pet, ch->in_room );
		add_follower( pet, ch );
		pet->leader = ch;
		ch->pet = pet;
		send_to_char( "Ecco il tuo cucciolo.\n\r", ch );
		act( "$n compra $N.", ch, NULL, pet, TO_ROOM, POS_REST );
		return;
	}
	else
	{
		CHAR_DATA	*keeper;
		OBJ_DATA	*obj, *t_obj;
		char		arg[MIL];
		int			number, count = 1;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( (keeper = find_keeper(ch)) == NULL )
			return;

		number = mult_argument( argument, arg );
		if ( number < -1 || number > 100 )
		{
			act( "$n ti dice 'Ma fai sul serio?'", keeper, NULL, ch, TO_VICT, POS_REST );
			ch->reply = keeper;
			return;
		}

		obj = get_obj_keeper( ch, keeper, arg );
		cost = get_cost( keeper, obj, TRUE );

		if ( cost <= 0 || !can_see_obj(ch, obj) )
		{
			act( "$n ti dice 'Non vendo nulla di simile.'.", keeper, NULL, ch, TO_VICT, POS_REST );
			ch->reply = keeper;
			return;
		}

		if ( !IS_OBJ_STAT(obj, ITEM_INVENTORY) )
		{
			for ( t_obj = obj->next_content; count < number && t_obj != NULL; t_obj = t_obj->next_content )
			{
				if ( t_obj->pIndexData == obj->pIndexData && !str_cmp(t_obj->short_descr, obj->short_descr) )
					count++;
				else
					break;
			}

			if ( count < number )
			{
				act( "$n ti dice  'Non ne ho cosi' in abbondanza!'.", keeper, NULL, ch, TO_VICT, POS_REST );
				ch->reply = keeper;
				return;
			}
		}
		else if ( obj->pIndexData->limit != -1 )
		{
			count = 1 + obj->pIndexData->limit - obj->pIndexData->count;
			if ( count < 1 )
			{
				act("$n ti dice 'Eh eh.. si, ti piacerebbe..'", keeper,NULL,ch,TO_VICT, POS_REST);
				ch->reply = keeper;
				return;
			}

			if ( count < number )
			{
				act( "$n ti dice 'Non ne ho cosi' in abbondanza!", keeper, NULL, ch, TO_VICT, POS_REST );
				ch->reply = keeper;
				return;
			}
		}

		if ( (ch->silver + ch->gold * 100) < cost * number )
		{
			if ( number > 1 )
			{
				act( "$n ti dice 'Non hai abbastanza moneta per un simile quantitativo.",
					keeper, obj, ch, TO_VICT, POS_REST );
			}
			else
				act( "$n ti dice 'Non hai abbastanza moneta!'.", keeper, obj, ch, TO_VICT, POS_REST);

			ch->reply = keeper;
			return;
		}

		if ( obj->level > ch->level )
		{
			act( "$n ti dice 'Non hai abbastanza esperienza per usare $p'.", keeper, obj, ch, TO_VICT, POS_REST );
			ch->reply = keeper;
			return;
		}

		if ( ch->carry_number + number * get_obj_number(obj) > can_carry_n(ch) )
		{
			send_to_char( "Non puoi trasportare cosi' tanti oggetti!\n\r", ch );
			return;
		}

		if ( ch->carry_weight + number * get_obj_weight(obj) > can_carry_w(ch) )
		{
			send_to_char( "Non puoi portare cosi' tanto peso!\n\r", ch );
			return;
		}

		/* haggle */
		roll = number_percent();
		if ( !IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT) && roll < get_skill(ch, gsn_haggle) )
		{
			cost -= obj->cost / 2 * roll / 100;
			act( "Contratti il prezzo con $N.", ch, NULL, keeper, TO_CHAR, POS_REST );
			check_improve( ch, gsn_haggle, TRUE, 4 );
		}

		if ( number > 1 )
		{
			sprintf( buf, "$n compra $p[%d].", number );
			act( buf, ch, obj, NULL, TO_ROOM, POS_REST );
			sprintf( buf, "Compri $p[%d] per %d argento.", number, cost*number );
			act( buf, ch, obj, NULL, TO_CHAR, POS_REST );
		}
		else
		{
			act( "$n compra $p.", ch, obj, NULL, TO_ROOM , POS_REST );
			sprintf( buf, "Compri $p per %d monet%c d'argento.", cost, (cost == 1) ? 'a' : 'e' );
			act( buf, ch, obj, NULL, TO_CHAR, POS_REST );
		}

		deduct_cost( ch, cost * number );
		keeper->gold += cost * number / 100;
		keeper->silver += cost * number - ( cost * number / 100 ) * 100;

		for ( count = 0; count < number; count++ )
		{
			if ( IS_SET(obj->extra_flags, ITEM_INVENTORY) )
				t_obj = create_object( obj->pIndexData, obj->level );
			else
			{
				t_obj = obj;
				obj = obj->next_content;
				obj_from_char( t_obj );
			}

			if ( t_obj->timer > 0 && !IS_OBJ_STAT(t_obj, ITEM_HAD_TIMER) )
				t_obj->timer = 0;
			REMOVE_BIT( t_obj->extra_flags, ITEM_HAD_TIMER );
			obj_to_char( t_obj, ch );
			if ( cost < t_obj->cost )
				t_obj->cost = cost;
		}
	}
}


void do_list( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
	{
		ROOM_INDEX_DATA *pRoomIndexNext;
		CHAR_DATA		*pet;
		bool			found;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* hack to make new thalos pets work */
		if ( ch->in_room->vnum == 9621 )
			pRoomIndexNext = get_room_index( 9706 );
		else
			pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );

		if ( pRoomIndexNext == NULL )
		{
			bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
			send_to_char( "Non puoi farlo qui.\n\r", ch );
			return;
		}

		found = FALSE;
		for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
		{
			if ( !IS_MOB(pet) )
				continue;	/* :) */

			if ( IS_SET(pet->act, ACT_PET) )
			{
				if ( !found )
				{
					found = TRUE;
					send_to_char( "In vendita:\n\r", ch );
				}

				sprintf( buf, "%8d - %s\n\r", 10 * pet->level * pet->level, pet->short_descr );
				send_to_char( buf, ch );
			}
		}

		if ( !found )
			send_to_char( "Spiacente.. non ne abbiamo in questo momento.\n\r", ch );
		return;
	}
	else
	{
		CHAR_DATA	*keeper;
		OBJ_DATA	*obj;
		int			cost, count;
		bool		found;
		char		arg[MIL];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( (keeper = find_keeper(ch)) == NULL )
			return;
		one_argument( argument, arg );

		found = FALSE;
		for ( obj = keeper->carrying; obj; obj = obj->next_content )
		{
			if (obj->wear_loc == WEAR_NONE
			&&	can_see_obj(ch, obj)
			&&	(cost = get_cost(keeper, obj, TRUE)) > 0
			&&	(arg[0] == '\0' || is_name(arg, obj->name)) )
			{
				if ( !found )
				{
					found = TRUE;
					send_to_char( "[Prezzo Qta] Oggetto\n\r", ch );
				}

				if ( IS_OBJ_STAT(obj, ITEM_INVENTORY) )
				{
#ifdef MXP
					sprintf( buf,"[%5d  -- ] "
						MXPTAG("Lst '%s' '%s'")
						"%s"
						MXPTAG("/Lst")
						"%s\n\r",
						cost,
						obj->name, obj->short_descr,
						obj->short_descr,
						(obj->pIndexData->limit != -1) ?
							(obj->pIndexData->count > obj->pIndexData->limit) ? " (NOT AVAILABLE NOW)" : " (AVAILABLE)" : "" );
#else
					sprintf( buf, "[%5d  -- ] %s%s\n\r",
						cost,
						obj->short_descr,
						(obj->pIndexData->limit != -1) ?
							(obj->pIndexData->count > obj->pIndexData->limit) ? " (NOT AVAILABLE NOW)" : " (AVAILABLE)" : "" );
#endif
				}
				else
				{
					count = 1;

					while(	obj->next_content != NULL
					&&		obj->pIndexData == obj->next_content->pIndexData
					&&		!str_cmp(obj->short_descr, obj->next_content->short_descr) )
					{
						obj = obj->next_content;
						count++;
					}

/*
#ifdef MXP
					sprintf( buf,"[%5d  %2d ] "
						MXPTAG ("Lst")
						"%s"
						MXPTAG ("/Lst")
						"\n\r",
						cost, count, obj->short_descr );
#else
*/
					sprintf( buf, "[%5d  %2d ] %s\n\r", cost, count, obj->short_descr );
/*#endif*/
				}

				send_to_char( buf, ch );
			}
		}

		if ( !found )
			send_to_char( "Non puoi comprare nulla qui.\n\r", ch );
	}
}


void do_sell( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		buf2[MSL];
	char		arg[MIL];
	CHAR_DATA	*keeper;
	OBJ_DATA	*obj;
	int			cost, roll;
	int			gold, silver;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Cosa vorresti vendere?\n\r", ch );
		return;
	}

	if ( (keeper = find_keeper(ch)) == NULL )
		return;

	if ( (obj = get_obj_carry(ch, arg)) == NULL )
	{
		act( "$n ti dice 'Non hai nulla di simile!", keeper, NULL, ch, TO_VICT, POS_REST );
		ch->reply = keeper;
		return;
	}

	if ( !can_drop_obj(ch, obj) )
	{
		send_to_char( "Non riesci a liberartene!\n\r", ch );
		return;
	}

	if ( !can_see_obj(keeper, obj) )
	{
		act("$n non riesce a vedere l'oggetto che cerchi di vendere.",keeper,NULL,ch,TO_VICT, POS_REST);
		return;
	}

	if ( (cost = get_cost(keeper, obj, FALSE)) <= 0 )
	{
		act( "$n non sembra affatto interessat$x a  $p.", keeper, obj, ch, TO_VICT , POS_REST);
		return;
	}

	if ( cost > (keeper->silver + 100 * keeper->gold) )
	{
		act("$n ti dice 'Eh, magari mi potessi permettere $p.", keeper,obj,ch,TO_VICT, POS_REST);
		return;
	}

	act( "$n vende $p.", ch, obj, NULL, TO_ROOM, POS_REST );

	/* haggle */
	roll = number_percent();
	if ( !IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT) && roll < get_skill(ch, gsn_haggle) )
	{
		roll = get_skill( ch, gsn_haggle ) + number_range( 1, 20 ) - 10;
		send_to_char( "Contratti il prezzo con il negoziante.\n\r", ch );
		cost += obj->cost / 2 * roll / 100;
		cost = UMIN( cost, 95 * get_cost(keeper, obj, TRUE) / 100 );
		cost = UMIN( cost, (keeper->silver + 100 * keeper->gold) );
		check_improve( ch, gsn_haggle, TRUE, 4 );
	}

	silver = cost - ( cost / 100 ) * 100;
	gold = cost / 100;

	sprintf( buf2, "Vendi $p per %s %s%spiece%s.",
		silver != 0 ? "%d silver" : "", /* silvers */
		(silver != 0 && gold != 0) ? "and " : "",	/* and */
		gold != 0 ? "%d gold " : "",				/* golds */
		silver + gold > 1 ? "s" : "" );	/* piece(s) */
	sprintf( buf, buf2, silver, gold );

	act( buf, ch, obj, NULL, TO_CHAR, POS_REST );
	ch->gold += gold;
	ch->silver += silver;
	deduct_cost( keeper, cost );

	if ( keeper->gold < 0 )
		keeper->gold = 0;
	if ( keeper->silver < 0 )
		keeper->silver = 0;

	if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT) )
		extract_obj( obj );
	else
	{
		obj_from_char( obj );
		if ( obj->timer )
			SET_BIT( obj->extra_flags, ITEM_HAD_TIMER );
		else
			obj->timer = number_range( 50, 100 );
		obj_to_keeper( obj, keeper );
	}
}


void do_value( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg[MIL];
	CHAR_DATA	*keeper;
	OBJ_DATA	*obj;
	int			cost;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );


	if ( arg[0] == '\0' )
	{
		send_to_char( "Cosa vuoi valutare?\n\r", ch );
		return;
	}

	if ( (keeper = find_keeper(ch)) == NULL )
		return;

	if ( (obj = get_obj_carry(ch, arg)) == NULL )
	{
		act( "$n ti dice 'Non hai nessun oggetto con quel nome'.", keeper, NULL, ch, TO_VICT, POS_REST );
		ch->reply = keeper;
		return;
	}

	if ( !can_see_obj(keeper, obj) )
	{
		act( "$n non puo' vedere cio' che stai cercando di vendere.", keeper, NULL, ch, TO_VICT, POS_REST);
		return;
	}

	if ( !can_drop_obj(ch, obj) )
	{
		send_to_char( "Non riesci a liberartene!\n\r", ch );
		return;
	}

	if ( (cost = get_cost(keeper, obj, FALSE)) <= 0 )
	{
		act( "$n non sembra affatto interessat$x a  $p.", keeper, obj, ch, TO_VICT , POS_REST);
		return;
	}

	sprintf( buf, "$n ti dice 'Ti daro' %d monete d'argento e %d d'oro per $p'.",
		cost - (cost/100)*100,
		cost/100 );
	act( buf, keeper, obj, ch, TO_VICT, POS_REST );
	ch->reply = keeper;
}


void do_herbs( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	char		arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_herbs, 0, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_herbs) )
	{
		send_to_char( "Non trovi piu' erbe qui.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
		victim = ch;
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non e' qui.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_herbs].beats );

	if (ch->in_room->sector_type != SECT_INSIDE
	&&	ch->in_room->sector_type != SECT_CITY
	&&	(IS_MOB(ch) || number_percent() < get_skill(ch, gsn_herbs)) )
	{
		AFFECT_DATA af;
		/*~~~~~~~~~~~*/

		af.where = TO_AFFECTS;
		af.type = gsn_herbs;
		af.level = ch->level;
		af.duration = 5;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = 0;

		affect_to_char( ch, &af );

		send_to_char( "Riesci a trovare un po' di erbe curative.\n\r", ch );
		act( "$n racimola un po' di erbe.", ch, NULL, NULL, TO_ROOM, POS_REST );

		if ( ch != victim )
		{
			act( "$n ti da' qualche filo d'erba balsamico.", ch, NULL, victim, TO_VICT, POS_REST );
			act( "Dai un po' di erba balsamica a $N.", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n gives the herbs to $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		}

		if ( victim->point[LIFE] < victim->max_point[LIFE] )
		{
			send_to_char( "Ti senti meglio.\n\r", victim );
			act( "$n sembra sentirsi un po' meglio.", victim, NULL, NULL, TO_ROOM, POS_REST );
		}

		victim->point[LIFE] = UMIN( victim->max_point[LIFE], victim->point[LIFE] + 5 * ch->level );
		check_improve( ch, gsn_herbs, TRUE, 1 );
		if ( is_affected(victim, gsn_plague) )
		{
			if ( check_dispel(ch->level, victim, gsn_plague) )
			{
				send_to_char( "La maledizione che gravava su di te sparisce.\n\r", victim );
				act( "L'ombra grigia che sembrava gravare su $n svanisce.", victim, NULL, NULL,TO_ROOM, POS_REST );
			}
		}
	}
	else
	{
		send_to_char("Non sembrano esserci erbe qui..\n\r", ch );
		act( "$n cerca delle erbe in giro.", ch, NULL, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_herbs, FALSE, 1 );
	}
}


void do_lore( CHAR_DATA *ch, char *argument )
{
	char		arg1[MIL];
	OBJ_DATA	*obj;
	char		buf[MSL];
	AFFECT_DATA *paf;
	int			chance;
	int			value0, value1, value2, value3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );

	if ( skill_failure_check(ch, gsn_lore, 1, 0, "Cerchi di esaminare l'oggetto.. ma non riesci a capirci nulla.") )
		return;

	if ( (obj = get_obj_carry(ch, arg1)) == NULL )
	{
		send_to_char( "Non hai niente con questo nome.\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < 30 )
	{
		send_to_char( "Non hai energie sufficienti per farlo.\n\r", ch );
		return;
	}

	/* a random lore */
	chance = number_percent();

	if ( get_skill(ch, gsn_lore) < 20 )
	{
      sprintf( buf, "Oggetto '%s'.\n\r", obj->name );
		send_to_char( buf, ch );
		ch->point[MANA] -= 30;
		check_improve( ch, gsn_lore, TRUE, 8 );
		return;
	}
	else if ( get_skill(ch, gsn_lore) < 40 )
	{
		sprintf( buf, "Oggetto '%s'.  Peso %d, valore %d.\n\r",
			obj->name,
			chance < 60 ? obj->weight : number_range(1, 2 * obj->weight),
			chance < 60 ? number_range(1, 2 * obj->cost) : obj->cost );
		send_to_char( buf, ch );
		if ( str_cmp(obj->material, "oldstyle") )
		{
			sprintf( buf, "Materiale %s.\n\r", material_name(obj->material) );
			send_to_char( buf, ch );
		}

		ch->point[MANA] -= 30;
		check_improve( ch, gsn_lore, TRUE, 7 );
		return;
	}
	else if ( get_skill(ch, gsn_lore) < 60 )
	{
		sprintf( buf, "Oggetto '%s' peso %d.\n\rValore is %d.\n\rMateriale %s.\n\r",
			obj->name,
			obj->weight,
			chance < 60 ? number_range(1, 2 * obj->cost) : obj->cost,
			str_cmp(obj->material, "oldstyle") ? material_name(obj->material) : "sconosciuto" );
		send_to_char( buf, ch );
		ch->point[MANA] -= 30;
		check_improve( ch, gsn_lore, TRUE, 6 );
		return;
	}
	else if ( get_skill(ch, gsn_lore) < 80 )
	{
		sprintf( buf, "Oggetto '%s' di tipo %s, extra flags %s.\n\rPeso %d, valore %d.\n\rMateriale %s.\n\r",
			obj->name,
			item_type_name(obj),
			extra_bit_name(obj->extra_flags),
			obj->weight,
			chance < 60 ? number_range(1, 2 * obj->cost) : obj->cost,
			str_cmp(obj->material, "oldstyle") ? material_name(obj->material) : "sconosciuto" );
		send_to_char( buf, ch );
		ch->point[MANA] -= 30;
		check_improve( ch, gsn_lore, TRUE, 5 );
		return;
	}
	else if ( get_skill(ch, gsn_lore) < 85 )
	{
		sprintf( buf, "Oggetto '%s' di tipo %s, extra flags %s.\n\rPeso %d, valore %d.\n\rMateriale %s.\n\r",
			obj->name,
			item_type_name(obj),
			extra_bit_name(obj->extra_flags),
			obj->weight,
			obj->cost,
			str_cmp(obj->material, "oldstyle") ? material_name(obj->material) : "sconosciuto" );
		send_to_char( buf, ch );
	}
	else
	{
		sprintf( buf, "Oggetto '%s' di tipo %s, extra flags %s.\n\rPeso %d, valore %d.\n\rMateriale %s.\n\r",
			obj->name,
			item_type_name(obj),
			extra_bit_name(obj->extra_flags),
			obj->weight,
			obj->cost,
			str_cmp(obj->material, "oldstyle") ? material_name(obj->material) : "sconosciuto" );
		send_to_char( buf, ch );
	}

	ch->point[MANA] -= 30;

	value0 = obj->value[0];
	value1 = obj->value[1];
	value2 = obj->value[2];
	value3 = obj->value[3];

	switch ( obj->item_type )
	{
	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
		if ( get_skill(ch, gsn_lore) < 85 )
		{
			value0 = number_range( 1, 60 );
			if ( chance > 40 )
			{
				value1 = number_range( 1, (MAX_SKILL - 1) );
				if ( chance > 60 )
				{
					value2 = number_range( 1, (MAX_SKILL - 1) );
					if ( chance > 80 )
						value3 = number_range( 1, (MAX_SKILL - 1) );
				}
			}
		}
		else
		{
			if ( chance > 60 )
			{
				value1 = number_range( 1, (MAX_SKILL - 1) );
				if ( chance > 80 )
				{
					value2 = number_range( 1, (MAX_SKILL - 1) );
					if ( chance > 95 )
						value3 = number_range( 1, (MAX_SKILL - 1) );
				}
			}
		}

		sprintf( buf, "Livello %d incantesimo di:", obj->value[0] );
		send_to_char( buf, ch );

		if ( value1 >= 0 && value1 < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[value1].name, ch );
			send_to_char( "'", ch );
		}

		if ( value2 >= 0 && value2 < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[value2].name, ch );
			send_to_char( "'", ch );
		}

		if ( value3 >= 0 && value3 < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[value3].name, ch );
			send_to_char( "'", ch );
		}

		send_to_char( ".\n\r", ch );
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		if ( get_skill(ch, gsn_lore) < 85 )
		{
			value0 = number_range( 1, 60 );
			if ( chance > 40 )
			{
				value3 = number_range( 1, (MAX_SKILL - 1) );
				if ( chance > 60 )
				{
					value2 = number_range( 0, 2 * obj->value[2] );
					if ( chance > 80 )
						value1 = number_range( 0, value2 );
				}
			}
		}
		else
		{
			if ( chance > 60 )
			{
				value3 = number_range( 1, (MAX_SKILL - 1) );
				if ( chance > 80 )
				{
					value2 = number_range( 0, 2 * obj->value[2] );
					if ( chance > 95 )
						value1 = number_range( 0, value2 );
				}
			}
		}

		sprintf( buf, "Ha %d(%d) cariche di livello %d", value1, value2, value0 );
		send_to_char( buf, ch );

		if ( value3 >= 0 && value3 < MAX_SKILL )
		{
			send_to_char( " '", ch );
			send_to_char( skill_table[value3].name, ch );
			send_to_char( "'", ch );
		}
		send_to_char( ".\n\r", ch );
		break;

	case ITEM_WEAPON:
		send_to_char( "L'arma e' di tipo ", ch );
		if ( get_skill(ch, gsn_lore) < 85 )
		{
			value0 = number_range( 0, 8 );
			if ( chance > 33 )
			{
				value1 = number_range( 1, 2 * obj->value[1] );
				if ( chance > 66 )
					value2 = number_range( 1, 2 * obj->value[2] );
			}
		}
		else
		{
			if ( chance > 50 )
			{
				value1 = number_range( 1, 2 * obj->value[1] );
				if ( chance > 75 )
					value2 = number_range( 1, 2 * obj->value[2] );
			}
		}

		switch ( value0 )
		{
		case WEAPON_EXOTIC	:	send_to_char( "esotica.\n\r", ch );			break;
		case WEAPON_SWORD	:	send_to_char( "spada.\n\r", ch );			break;
		case WEAPON_DAGGER	:	send_to_char( "daga.\n\r", ch );			break;
		case WEAPON_SPEAR	:	send_to_char( "tridente/staffa.\n\r", ch );	break;
		case WEAPON_MACE	:	send_to_char( "mazza/clava.\n\r", ch );		break;
		case WEAPON_AXE		:	send_to_char( "ascia.\n\r", ch );			break;
		case WEAPON_FLAIL	:	send_to_char( "correggia.\n\r", ch );		break;
		case WEAPON_WHIP	:	send_to_char( "frusta.\n\r", ch );			break;
		case WEAPON_POLEARM	:	send_to_char( "alabarda.\n\r", ch );		break;
		case WEAPON_BOW		:	send_to_char( "arco.\n\r", ch );			break;
		case WEAPON_ARROW	:	send_to_char( "freccia.\n\r", ch );			break;
		case WEAPON_LANCE	:	send_to_char( "lancia.\n\r", ch );			break;
		default				:	send_to_char( "sconosciuto.\n\r", ch );		break;
		}

		if ( obj->pIndexData->new_format )
			sprintf( buf, "Danno %dd%d (media %d).\n\r", value1, value2, (1 + value2) * value1 / 2 );
		else
			sprintf( buf, "Danno da %d a %d (media %d).\n\r", value1, value2, (value1 + value2) / 2 );
		send_to_char( buf, ch );
		break;


	case ITEM_ARMOR:
		if ( get_skill(ch, gsn_lore) < 85 )
		{
			if ( chance > 25 )
			{
				value2 = number_range( 0, 2 * obj->value[2] );
				if ( chance > 45 )
				{
					value0 = number_range( 0, 2 * obj->value[0] );
					if ( chance > 65 )
					{
						value3 = number_range( 0, 2 * obj->value[3] );
						if ( chance > 85 )
							value1 = number_range( 0, 2 * obj->value[1] );
					}
				}
			}
		}
		else
		{
			if ( chance > 45 )
			{
				value2 = number_range( 0, 2 * obj->value[2] );
				if ( chance > 65 )
				{
					value0 = number_range( 0, 2 * obj->value[0] );
					if ( chance > 85 )
					{
						value3 = number_range( 0, 2 * obj->value[3] );
						if ( chance > 95 )
							value1 = number_range( 0, 2 * obj->value[1] );
					}
				}
			}
		}

		sprintf( buf, "Classe armatura fornita %d pugnalate, %d colpi, %d fendenti, e %d contro armi esotiche.\n\r", value0, value1, value2, value3 );
		send_to_char( buf, ch );
		break;
	}

	if ( get_skill(ch, gsn_lore) < 90 )
		return;

	if ( !obj->enchanted )
	{
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location != APPLY_NONE && paf->modifier != 0 )
			{
				sprintf( buf, "Attivi sull'oggetto %s  %d.\n\r",
					affect_loc_name(paf->location), paf->modifier );
				send_to_char( buf, ch );
			}
		}
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->location != APPLY_NONE && paf->modifier != 0 )
		{
			sprintf( buf, "Affetto %s da %d.\n\r",
				affect_loc_name(paf->location), paf->modifier );
			send_to_char( buf, ch );
		}
	}

	check_improve( ch, gsn_lore, TRUE, 5 );
}


void do_butcher( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	char		buf[MSL];
	char		arg[MSL];
	OBJ_DATA	*tmp_obj;
	OBJ_DATA	*tmp_next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_butcher, 1, 0, "Ti servira' piu' precisione per farlo.") )
		return;

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Da cosa vuoi ricavare il tuo pasto?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_here(ch, arg)) == NULL )
	{
		send_to_char( "Non vedi nulla di simile qui.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC )
	{
		send_to_char( "Non puoi farlo su quel corpo.\n\r", ch );
		return;
	}

	if ( obj->carried_by != NULL )
	{
		send_to_char("Mettilo giu' prima.\n\r",ch);
		return;
	}

	obj_from_room( obj );

	for ( tmp_obj = obj->contains; tmp_obj != NULL; tmp_obj = tmp_next )
	{
		tmp_next = tmp_obj->next_content;
		obj_from_obj( tmp_obj );
		obj_to_room( tmp_obj, ch->in_room );
	}

	if ( IS_MOB(ch) || number_percent() < get_skill(ch, gsn_butcher) )
	{
		int			numsteaks;
		int			i;
		OBJ_DATA	*steak;
		/*~~~~~~~~~~~~~~~~~~*/

		numsteaks = number_bits( 2 ) + 1;

		if ( numsteaks > 1 )
		{
			sprintf( buf, "$n ricava da $p delle deliziose fette di carne." );
			act( buf, ch, obj, NULL, TO_ROOM, POS_REST );

			sprintf( buf, "Ricavi da $p delle saporite costate." );
			act( buf, ch, obj, NULL, TO_CHAR, POS_REST );
		}
		else
		{
			act( "$n macella $p ottenendo della carne.", ch, obj, NULL, TO_ROOM, POS_REST );
			act( "Macelli $p ottenendo un pasto.", ch, obj, NULL, TO_CHAR, POS_REST );
		}

		check_improve( ch, gsn_butcher, TRUE, 1 );

		for ( i = 0; i < numsteaks; i++ )
		{
			steak = create_object( get_obj_index(OBJ_VNUM_STEAK), 0 );
			sprintf( buf, steak->short_descr, obj->short_descr );
			free_string( steak->short_descr );
			steak->short_descr = str_dup( buf );

			sprintf( buf, steak->description, obj->short_descr );
			free_string( steak->description );
			steak->description = str_dup( buf );

			obj_to_room( steak, ch->in_room );
		}
	}
	else
	{
		act( "Fallisci distruggendo $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n cerca di macellare $p ma non ne ottiene nulla.", ch, obj, NULL, TO_ROOM, POS_REST );

		check_improve( ch, gsn_butcher, FALSE, 1 );
	}

	extract_obj( obj );
}


void do_balance( CHAR_DATA *ch, char *argument )
{
	char	buf[160];
	char	buf2[100];
	long	bank_g;
	long	bank_s;
	/*~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
	{
		send_to_char( "Non hai un conto in banca.\n\r", ch );
		return;
	}

	if ( !IS_SET(ch->in_room->room_flags, ROOM_BANK) )
	{
		send_to_char( "Non sei in una banca.\n\r", ch );
		return;
	}

	if ( ch->pcdata->bank_s + ch->pcdata->bank_g == 0 )
	{
		send_to_char( "Il tuo conto in banca e' vuoto.\n\r", ch );
		return;
	}

	bank_g = ch->pcdata->bank_g;
	bank_s = ch->pcdata->bank_s;
	sprintf( buf, "Hai %s%s%s monet%c in banca.\n\r",
		bank_g != 0 ? "%ld d'oro" : "",
		(bank_g != 0) && (bank_s != 0) ? " e " : "",
		bank_s != 0 ? "%ld d'argento" : "",
		bank_s + bank_g > 1 ? 'e' : 'a' );
	if ( bank_g == 0 )
		sprintf( buf2, buf, bank_s );
	else
		sprintf( buf2, buf, bank_g, bank_s );

	send_to_char( buf2, ch );
}


void do_withdraw( CHAR_DATA *ch, char *argument )
{
	long	amount_s;
	long	amount_g;
	char	arg[MIL];
	char	buf[100];
	int		weight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
	{
		send_to_char( "Non hai un conto in banca.\n\r", ch );
		return;
	}

	if ( !IS_SET(ch->in_room->room_flags, ROOM_BANK) )
	{
	send_to_char( "Ti sembra di essere in una banca?\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Quanto vuoi ritirare?\n\r", ch );
		return;
	}

	amount_s = labs( atol(arg) );

	if ( amount_s <= 0 )
	{
		send_to_char( "Non e' un valore accettabile, quanto vorresti prelevare?\n\r", ch );
		return;
	}

	if ( !str_cmp(argument, "silver") || !str_cmp(argument, "argento") || argument[0] == '\0' )
		amount_g = 0;
	else if ( !str_cmp(argument, "gold") || !str_cmp(argument, "oro") )
	{
		amount_g = amount_s;
		amount_s = 0;
	}
	else
	{
		send_to_char( "Scegli se ritirare monete d'argento o d'oro.", ch );
		return;
	}

	if ( amount_g > ch->pcdata->bank_g )
	{
		send_to_char( "Spiacente, niente prestiti.\n\r", ch );
		return;
	}

	if ( amount_s > ch->pcdata->bank_s )
	{
		send_to_char( "Spiacente, niente prestiti.\n\r", ch );
		return;
	}

	weight = amount_g * 2 / 5;
	weight += amount_s / 10;

	if ( get_carry_weight(ch) + weight > can_carry_w(ch) )
	{
		act( "Un po' di meno.. non ne puoi reggere cosi' tante.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	ch->pcdata->bank_g -= amount_g;
	ch->pcdata->bank_s -= amount_s;
	ch->gold += 0.98 * amount_g;
	ch->silver += 0.90 * amount_s;
	if ( amount_s > 0 && amount_s < 10 )
	{
		if ( amount_s == 1 )
			sprintf( buf, "Una moneta!? Crepi l'avarizia eh?!\n\r" );
		else
			sprintf( buf, "%ld monete?! Crepi l'avarizia eh?!\n\r", amount_s );
	}
	else
	{
		sprintf( buf, "Ecco a voi %ld %s monete, meno %ld come tributo.\n\r",
			amount_s != 0 ? amount_s : amount_g,
			amount_s != 0 ? "silver" : "gold",
			amount_s != 0 ? (long) UMAX(1, (0.10 * amount_s)) : (long) UMAX(1, (0.02 * amount_g)) );
	}

	send_to_char( buf, ch );
	act( "$n si avvicina all'impiegato.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void do_deposit( CHAR_DATA *ch, char *argument )
{
	long	amount_s;
	long	amount_g;
	char	buf[100];
	char	arg[200];
	/*~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
	{
		send_to_char( "Non hai un conto in banca.\n\r", ch );
		return;
	}

	if ( !IS_SET(ch->in_room->room_flags, ROOM_BANK) )
	{
		send_to_char( "Ti sembra di essere in una banca?\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Quanto vuoi depositare?\n\r", ch );
		return;
	}

	amount_s = labs( atol(arg) );

	if ( amount_s <= 0 )
	{
		send_to_char( "Non e' un valore accettabile, quanto vorresti depositare?\n\r", ch );
		return;
	}

	if ( !str_cmp(argument, "silver") || !str_cmp(argument, "argento") || argument[0] == '\0' )
		amount_g = 0;
	else if ( !str_cmp(argument, "gold") || !str_cmp(argument, "oro") )
	{
		amount_g = amount_s;
		amount_s = 0;
	}
	else
	{
		send_to_char( "Scegli se depositare monete d'oro o d'argento.", ch );
		return;
	}

	if ( amount_g > ch->gold )
	{
		send_to_char( "Non ne hai cosi' tante.. fai i conti in tasca.\n\r", ch );
		return;
	}

	if ( amount_s > ch->silver )
	{
		send_to_char( "Non ne hai cosi' tante.. fai i conti in tasca.\n\r", ch );
		return;
	}

	if ( (amount_s + ch->pcdata->bank_s) > 500000 )
	{
		send_to_char( "La banca non puo' accettare di depositare cifre superiori a 500.000 monete d'argento.\n\r", ch );
		return;
	}

	if ( (amount_g + ch->pcdata->bank_g) > 400000 )
	{
		send_to_char( "La banca non puo' accettare di depositare cifre superiori a 500.000 monete d'oro.\n\r", ch );
		return;
	}

	ch->pcdata->bank_s += amount_s;
	ch->pcdata->bank_g += amount_g;
	ch->gold -= amount_g;
	ch->silver -= amount_s;

	if ( amount_s == 1 )
		sprintf( buf, "Oh boy! One gold coin!\n\r" );
	else
	{
		sprintf( buf, "%ld %s coins deposited. Come again soon!\n\r",
			amount_s != 0 ? amount_s : amount_g,
			amount_s != 0 ? "silver" : "gold" );
	}

	send_to_char( buf, ch );
	act( "$n steps up to the teller window.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void do_enchant( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	int			wear_level;
	/*~~~~~~~~~~~~~~~~~~~*/

    if ( skill_failure_check(ch, gsn_enchant_sword, 0, 0, NULL) )
    	return;

	if ( argument[0] == '\0' )	/* empty */
	{
		send_to_char( "Quale arma vuoi incantare?\n\r", ch );
		return;
	}

	obj = get_obj_carry( ch, argument );

	if ( obj == NULL )
	{
		send_to_char( "Non hai nessun oggetto con questo nome.\n\r", ch );
		return;
	}

	wear_level = ch->level;

	if ((class_table[ch->class].fMana && obj->item_type == ITEM_ARMOR)
	||	(!class_table[ch->class].fMana && obj->item_type == ITEM_WEAPON) )
		wear_level += 3;

	if ( wear_level < obj->level )
	{
		send_to_char( "Sei troppo inesperto per riuscire nell'incantamento.\n\r", ch );
		act( "$n cerca di incantare $p, ma non ha abbastanza potere.", ch, obj, NULL, TO_ROOM , POS_REST);
		return;
	}

	if ( ch->point[MANA] < 100 )
	{
		send_to_char( "Non hai abbastanza mana per farlo.\n\r", ch );
		return;
	}

	if ( number_percent() > get_skill(ch, gsn_enchant_sword) )
	{
		send_to_char( "Perdi la concentrazione sul piu' bello.\n\r", ch );
		act( "$n cerca di incantare $p, ma perde la concentrazione..", ch, obj, NULL, TO_ROOM, POS_REST );
		WAIT_STATE( ch, skill_table[gsn_enchant_sword].beats );
		check_improve( ch, gsn_enchant_sword, FALSE, 6 );
		ch->point[MANA] -= 50;
		return;
	}

	ch->point[MANA] -= 100;
	spell_enchant_weapon( 24, ch->level, ch, obj, TARGET_OBJ );
	check_improve( ch, gsn_enchant_sword, TRUE, 2 );
	WAIT_STATE( ch, skill_table[gsn_enchant_sword].beats );
}


void hold_a_light( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
	act( "$n usa $p come luce.", ch, obj, NULL, TO_ROOM, POS_REST);
	act( "Utilizzi $p come luce.",  ch, obj, NULL, TO_CHAR, POS_REST);
	equip_char( ch, obj, iWear );
}


void hold_a_shield( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
	act( "$n prende $p come scudo.", ch, obj, NULL, TO_ROOM , POS_REST);
	act( "Reggi $p come scudo.", ch, obj, NULL, TO_CHAR , POS_REST);
	equip_char( ch, obj, iWear );
}


void hold_a_thing( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
	act( "$n tiene $p in mano.",   ch, obj, NULL, TO_ROOM , POS_REST);
	act( "Tieni $p in mano.", ch, obj, NULL, TO_CHAR , POS_REST);
	equip_char( ch, obj, iWear );
}


/*
 * wear object as a secondary weapon
 */
void hold_a_wield( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
	int sn, skill;
	/*~~~~~~~~~~*/

	if ( obj == NULL )
	{
		bug( "Hold_a_wield: Obj NULL", 0 );
		return;
	}

	if ( obj->item_type != ITEM_WEAPON )
	{
		hold_a_thing( ch, obj, iWear );
		return;
	}

	act( "$n impugna $p.", ch, obj, NULL, TO_ROOM , POS_REST);
	act( "Impugni $p.", ch, obj, NULL, TO_CHAR , POS_REST);
	equip_char( ch, obj, iWear );
	if ( get_wield_char(ch, TRUE) == obj )
		sn = get_weapon_sn( ch, TRUE );
	else
		sn = get_weapon_sn( ch, FALSE );

	if ( sn )
	{
		skill = get_weapon_skill( ch, sn );

		if		( skill >= 100 )	act( "$p diventa parte di te!", ch, obj, NULL, TO_CHAR, POS_REST );
		else if ( skill >   85 )	act( "Senti che $p non ti tradira'.", ch, obj, NULL, TO_CHAR, POS_REST );
		else if ( skill >   70 )	act( "Senti d'avere una buona confidenza con $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		else if ( skill >   50 )	act( "Non sei perfettamente addestrat$x all'uso di $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		else if ( skill >   25 )	act( "Impugni in maniera incerta $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		else if ( skill >    1 )	act( "Impugni a tentoni $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		else						act( "Vuoi usare $p? Ma sai come si impugna?", ch, obj, NULL, TO_CHAR, POS_REST );
	}
}


void wear_a_wield( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
	if ( !IS_MOB(ch) && get_obj_weight(obj) > str_app[get_curr_stat(ch, STR)].carry )
	{
		send_to_char( "E' troppo pesante per te.\n\r", ch );
		return;
	}

	if ( IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS) && (!IS_MOB(ch) && ch->size < SIZE_LARGE) )
	{
		if ( get_eq_char(ch, WEAR_BOTH) != NULL )
		{
			if ( !remove_obj_loc(ch, WEAR_BOTH, fReplace) )
				return;
			hold_a_wield( ch, obj, WEAR_BOTH );
		}
		else
		{
			if ( get_eq_char(ch, WEAR_RIGHT) )
				if ( !remove_obj_loc(ch, WEAR_RIGHT, fReplace) )
					return;
			if ( get_eq_char(ch, WEAR_LEFT) )
				if ( !remove_obj_loc(ch, WEAR_LEFT, fReplace) )
					return;
			hold_a_wield( ch, obj, WEAR_BOTH );
		}
	}
	else
	{
		if ( get_eq_char(ch, WEAR_BOTH) != NULL )
		{
			if ( !remove_obj_loc(ch, WEAR_BOTH, fReplace) )
				return;
			hold_a_wield( ch, obj, WEAR_RIGHT );
		}
		else if ( get_eq_char(ch, WEAR_RIGHT) == NULL )
			hold_a_wield( ch, obj, WEAR_RIGHT );
		else if ( get_eq_char(ch, WEAR_LEFT) == NULL )
			hold_a_wield( ch, obj, WEAR_LEFT );
		else if ( remove_obj_loc(ch, WEAR_RIGHT, fReplace) )
			hold_a_wield( ch, obj, WEAR_RIGHT );
		else if ( remove_obj_loc(ch, WEAR_LEFT, fReplace) )
			hold_a_wield( ch, obj, WEAR_LEFT );
		else
			send_to_char("Hai le mani occupate.\n\r",ch);
	}
}


void wear_multi( CHAR_DATA *ch, OBJ_DATA *obj, int iWear, bool fReplace )
{
	if ( count_worn(ch, iWear) < max_can_wear(ch, iWear) )
	{
		switch ( iWear )
		{
		case WEAR_FINGER:
			act( "$n infila $p al dito.", ch, obj, NULL, TO_ROOM , POS_REST);
			act( "Infili $p al dito.", ch, obj, NULL, TO_CHAR , POS_REST);
			break;

		case WEAR_NECK:
			act( "$n mette $p attorno al collo.",   ch, obj, NULL, TO_ROOM , POS_REST);
			act( "Metti $p attorno al collo.", ch, obj, NULL, TO_CHAR , POS_REST);
			break;

		case WEAR_WRIST:
			act( "$n mette $p attorno al polso.", ch, obj, NULL, TO_ROOM, POS_REST );
			act( "Metti $p attorno al polso.",ch, obj, NULL, TO_CHAR, POS_REST );
			break;

		case WEAR_TATTOO:
			act( "$n ha ora il tatuaggio della sua religione.", ch, obj, NULL, TO_ROOM, POS_REST );
			act( "Ora hai il tatuaggio della tua religione.", ch, obj, NULL, TO_CHAR, POS_REST );
			break;

		default:
			act( "$n indossa $p.", ch, obj, NULL, TO_ROOM , POS_REST );
			act( "Indossi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			break;
		}

		equip_char( ch, obj, iWear );
	}
	else if ( fReplace )
	{
		OBJ_DATA	*w;
		int			not_worn = 1;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		for ( w = ch->carrying; w != NULL; w = w->next_content )
		{
			if ( w->wear_loc == iWear && !IS_SET(w->extra_flags, ITEM_NOREMOVE) && (w->item_type != ITEM_TATTOO || IS_ADMIN(ch)) )
			{
				unequip_char( ch, w );
				act( "$n smette di usare $p.", ch, w, NULL, TO_ROOM, POS_REST );
				act( "Smetti di usare $p.", ch, w, NULL, TO_CHAR, POS_REST );
				wear_multi( ch, obj, iWear, fReplace );
				not_worn = 0;
				break;
			}
		}

		if ( not_worn )
			act( "Non puoi rimuovere niente da rimpiazzare con $p.", ch, obj, NULL, TO_CHAR, POS_REST );
	}
}


/*
 * Equips a character
 */
void do_outfit( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	int			sn, vnum;
	/*~~~~~~~~~~~~~~~~~*/

	if ( (ch->level > LEVEL_NEWBIE || IS_MOB(ch)) && !IS_ADMIN(ch) )
	{
		act( "Hai abbastanza esperienza da trovartelo da sol$x!", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( ch->carry_number + 1 > can_carry_n(ch) )
	{
		send_to_char( "Non puoi trasportare cosi' tanti oggetti.\n\r", ch );
		return;
	}

	if ( (obj = get_light_char(ch)) == NULL )
	{
		obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
		obj->cost = 0;
		obj->condition = 100;
		obj_to_char( obj, ch );
	}

	if ( ch->carry_number + 1 > can_carry_n(ch) )
	{
		send_to_char( "Non puoi trasportare cosi' tanti oggetti.\n\r", ch );
		return;
	}

	if ( (obj = get_eq_char(ch, WEAR_BODY)) == NULL )
	{
		obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
		obj->cost = 0;
		obj->condition = 100;
		obj_to_char( obj, ch );
	}

	if ( ch->carry_number + 1 > can_carry_n(ch) )
	{
		send_to_char( "Non puoi trasportare cosi' tanti oggetti.\n\r", ch );
		return;
	}

	/* do the weapon thing */
	if ( (obj = get_wield_char(ch, FALSE)) == NULL )
	{
		sn = 0;
		vnum = OBJ_VNUM_SCHOOL_SWORD;	/* just in case! */
		vnum = class_table[ch->class].weapon;
		obj = create_object( get_obj_index(vnum), 0 );
		obj->condition = 100;
		obj_to_char( obj, ch );
	}

	obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
	obj->cost = 0;
	obj->condition = 100;
	obj_to_char( obj, ch );

	send_to_char( "Ti e' stato donato dell'equipaggiamento dagli Dei.\n\r", ch );
	send_to_char( "Scrivi 'invent' per vedere la lista degli oggetti che trasporti.\n\r", ch );
	send_to_char( "Scrivi 'indossa <nome oggetto>' per indossare un oggetto che trasporti.\n\r", ch );
}


/*
 * Creodin for Acasaid 2001
 * Purifica oggetti dal veleno.
 */
void do_purify( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	int skill;

	/* find out what */
	if ( argument[0] == '\0' )
	{
		send_to_char( "Purificare quale cibo o bevanda?\n\r", ch );
		return;
	}

	obj = get_obj_list( ch, argument, ch->carrying );

	if ( obj == NULL )
	{
		send_to_char( "Non possiedi questo oggetto.\n\r", ch );
		return;
	}

	if ( (skill = get_skill(ch, gsn_purify)) < 1 )
	{
		send_to_char( "La tua divinita' non vuole aiutarti in questo momento.\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON )
	{
		if ( number_percent () < skill )
		{
			act( "$n prega gli Dei affinche' purifichino $p.", ch, obj, NULL, TO_ROOM, POS_REST );
			act( "Preghi gli Dei per purificare $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			obj->value[3] = 0;
			check_improve( ch, gsn_purify, TRUE, 4 );
			WAIT_STATE( ch, skill_table[gsn_purify].beats );
			ch->point[MANA] -= 10;
		}
		else
		{
			act( "Fallisci nel purificare $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			check_improve( ch, gsn_purify, FALSE, 3 );
			WAIT_STATE( ch, skill_table[gsn_purify].beats );
			ch->point[MANA] -= 10;
		}
	}
}
