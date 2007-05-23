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


/*
 * Funzioni locali.
 */
int			mount_success		( CHAR_DATA *ch, CHAR_DATA *mount, int canattack );


char *const dir_name[] =
{
	"nord",
	"est",
	"sud",
	"ovest",
	"alto",
	"basso",
	"nordest",
	"sudest",
	"sudovest",
	"nordovest"
};

char *const dir_name_en[] =
{
	"north",
	"east",
	"south",
	"west",
	"up",
	"down",
	"northeast",
	"southeast",
	"southwest",
	"northwest"
};


char *const dir_name_from[] =
{
	"da nord",
	"da est",
	"da sud",
	"da ovest",
	"dall'alto",
	"dal basso",
	"da nordest",
	"da sudest",
	"da sudovest",
	"da nordovest"
};

char *const	dir_name_to[] =
{
	"verso nord",
	"verso est",
	"verso sud",
	"verso ovest",
	"verso l'alto",
	"verso il basso",
	"verso nordest",
	"verso sudest",
	"verso sudovest",
	"verso nordovest"
};


const sh_int rev_dir[] = { 2, 3, 0, 1, 5, 4, 6, 7, 8, 9 };


const sh_int movement_loss[SECT_MAX] =
{
	1,	/* inside */
	2,	/* city */
	3,	/* field */
	4,	/* forest */
	5,	/* hills */
	7,	/* mountain */
	5,	/* water_swim */
	2,	/* water_noswim */
	7,	/* unused */
	11, /* air */
	7,	/* desert */
};


/*
 * Local functions.
 */
bool	has_key		( CHAR_DATA * ch, int key );


void move_char( CHAR_DATA *ch, int door, bool follow )
{
	CHAR_DATA		*fch;
	CHAR_DATA		*fch_next;
	CHAR_DATA		*mount;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA		*pexit;
	bool			room_has_pc;
	OBJ_DATA		*obj;
	char			buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( RIDDEN(ch) && !IS_MOB(ch->mount) )
	{
		move_char( ch->mount, door, follow );
		return;
	}

	if ( CAN_DETECT(ch, ADET_WEB) || (MOUNTED(ch) && CAN_DETECT(ch->mount, ADET_WEB)) )
	{
		WAIT_STATE( ch, PULSE_VIOLENCE );
		if ( number_percent() < str_app[IS_MOB(ch) ? 20 : get_curr_stat(ch, STR)].tohit * 5 )
		{
			affect_strip( ch, gsn_web );
			REMOVE_BIT( ch->detection, ADET_WEB );
			send_to_char( "Tenti d'andartene dal luogo e rompi le ragnatele ed esse ti imprigionavano le caviglie.\n\r", ch );
			act( "$n rompe le ragnatele che l$x imprigionavano.", ch, NULL, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			send_to_char( "Tenti d'andartene dal luogo, ma le ragnatele che ti impriginano le caviglie sono robuste..\n\r", ch );
			act( "$n tenta di rompere le ragnatele che l$x imprigionano.", ch, NULL, NULL, TO_ROOM, POS_REST );
			return;
		}
	}

	if ( door < 0 || door > MAX_DIR - 1 )
	{
		bug( "Do_move: bad door %d.", door );
		return;
	}

	if ( (IS_AFFECTED(ch, AFF_HIDE) && !IS_AFFECTED(ch, AFF_SNEAK)) || (IS_AFFECTED(ch, AFF_FADE) && !IS_AFFECTED(ch, AFF_SNEAK)) )
	{
		REMOVE_BIT( ch->affected_by, AFF_HIDE );
		send_to_char( "Esci dalle ombre.\n\r", ch );
		act( "$n esce dalle ombre.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_AFFECTED(ch, AFF_CAMOUFLAGE) )
	{
		if ( number_percent() < get_skill(ch,gsn_move_camf) )
			check_improve(ch,gsn_move_camf,TRUE,5);
		else
		{
			REMOVE_BIT( ch->affected_by, AFF_CAMOUFLAGE );
			send_to_char( "Esci dal tuo nascondiglio.\n\r", ch );
			act( "$n esce dal suo nascondiglio.", ch, NULL, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_move_camf, FALSE, 5 );
		}
	}

	if ( CAN_DETECT(ch, ADET_EARTHFADE) )
	{
		send_to_char( "Torni lentamente nella tua forma neutrale.\n\r", ch );
		act( "LA terra plasma $n davanti a te.", ch, NULL, NULL, TO_ROOM, POS_REST );
		affect_strip( ch, gsn_earthfade );
		WAIT_STATE( ch, (PULSE_VIOLENCE / 2) );
		REMOVE_BIT( ch->detection, ADET_EARTHFADE );
	}

	in_room = ch->in_room;
	if ( (pexit = in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL || !can_see_room(ch, pexit->to_room) )
	{
		send_to_char( "Ahime', non puoi andare da quella parte.\n\r", ch );
		return;
	}

	if ( IS_ROOM_AFFECTED(in_room, AFF_ROOM_RANDOMIZER) )
	{
		int d0;
		int i;
		/*~~~*/

		for ( i = 0; i < 1000; i++ )
		{
			d0 = number_range( 0, 5 );
			if ( (pexit = in_room->exit[d0]) == NULL || (to_room = pexit->to_room) == NULL || !can_see_room(ch, pexit->to_room) )
				continue;
			door = d0;
			break;
		}
	}

	if ( IS_SET(pexit->exit_info, EX_CLOSED)
	&&	 (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info, EX_NOPASS))
	&&	 !IS_TRUSTED(ch, LEVEL_MASTER) )
	{
		if ( IS_AFFECTED(ch, AFF_PASS_DOOR) && IS_SET(pexit->exit_info, EX_NOPASS) )
		{
			act( "Non riesci a passare attraverso $d.", ch, NULL, pexit->keyword, TO_CHAR, POS_REST );
			act( "$n tenta di passare attraverso $d, ma fallisce.", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );
		}
		else
			act( "L'uscita e' chiusa.", ch, NULL, pexit->keyword, TO_CHAR, POS_REST );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL && in_room == ch->master->in_room )
	{
		send_to_char( "Come? E lasciare il tuo adorato maestro?\n\r", ch );
		return;
	}

	/* if ( !is_room_owner(ch,to_room) && room_is_private( to_room ) ) */
	if ( room_is_private(to_room) )
	{
		send_to_char( "Quella stanza e' privata.\n\r", ch );
		return;
	}

	if ( MOUNTED(ch) )
	{
		if ( MOUNTED(ch)->position < POS_FIGHT )
		{
			send_to_char( "La tua cavalcatura deve essere in piedi.\n\r", ch );
			return;
		}

		if ( !mount_success(ch, MOUNTED(ch), FALSE) )
		{
			send_to_char( "La tua cavalcatura si rifiuta di proseguire.\n\r", ch );
			return;
		}
	}

	if ( !IS_MOB(ch) )
	{
		int iClass, iGuild;
		int move;
		/*~~~~~~~~~~~~~~~*/

		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
			for ( iGuild = 0; iGuild < MAX_GUILD; iGuild++ )
			{
				if ( to_room->vnum == class_table[iClass].guild[iGuild] && !IS_ADMIN(ch) )
				{
					if ( iClass != ch->class )
					{
						send_to_char( "Non ti e' permesso di andare laggiu'.\n\r", ch );
						return;
					}

					if ( ch->last_fight_time != -1 && current_time - ch->last_fight_time < FIGHT_DELAY_TIME )
					{
						act( "Sei tropp$x assetat$x di sangue per andare laggiu'", ch, NULL, NULL, TO_CHAR, POS_REST );
						return;
					}
				}
			}
		}

		if ( in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR )
		{
			if ( MOUNTED(ch) )
			{
				if ( !IS_AFFECTED(MOUNTED(ch), AFF_FLYING) )
				{
					send_to_char( "La tua cavalcatura non puo' volare.\n\r", ch );
					return;
				}
			}
			else if ( !IS_AFFECTED(ch, AFF_FLYING) && !IS_ADMIN(ch) )
			{
				send_to_char( "Non puoi volare.\n\r", ch );
				return;
			}
		}

		if ((in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
		&&	(MOUNTED(ch) && !IS_AFFECTED(MOUNTED(ch), AFF_FLYING)) )
		{
			send_to_char( "Non puoi portare la tua cavalcatura con te laggiu'.\n\r", ch );
			return;
		}

		if ((in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
		&&	(!MOUNTED(ch) && !IS_AFFECTED(ch, AFF_FLYING)) )
		{
			bool		found;
			/*~~~~~~~~~~~~~~*/

			/* Look for a boat. */
			found = FALSE;

			if ( IS_ADMIN(ch) )
				found = TRUE;

			for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
			{
				if ( obj->item_type == ITEM_BOAT )
				{
					found = TRUE;
					break;
				}
			}

			if ( !found )
			{
				send_to_char( "Hai bisogno di una barca per andare laggiu'.\n\r", ch );
				return;
			}
		}

#ifdef WEATHER

#	if 0
	/* bonus per questi casi: */
	&&	 ((!IS_AFFECTED(ch, AFF_FLYING))
	&&	 (!IS_AFFECTED(ch,AFF_HASTE))
	&&	 (!IS_AFFECTED(ch,AFF_INFRARED))

	e ranger


#	endif

	/*
	 * Major code upgrade here. Added a few spells to allow people to move
	 *	around during storms, and a few random effects.
	 */

	/* Da settore chiuso a settore aperto */
	if ( ((in_room->sector_type == SECT_INSIDE) || (in_room->room_flags == ROOM_INDOORS))
	&&	 ((to_room->sector_type != SECT_INSIDE) && (to_room->room_flags != ROOM_INDOORS)) )
	{
		if		( weather_info.sky == SKY_BLIZZARD )
			send_to_char( "Una bufera di neve imperversa all'esterno..\n\r", ch );
		else if ( weather_info.sky == SKY_HAILSTORM )
			send_to_char( "Una fitta pioggerella vien giu' dal cielo..\n\r", ch );
		else if ( weather_info.sky == SKY_ICESTORM )
			send_to_char( "Fuori imperversa una tempesta di ghiaccio..\n\r", ch );
		else if ( weather_info.sky == SKY_FOGGY )
			send_to_char( "Una fitta nebbiolina avvolge completamente il paesaggio..\n\r", ch );
	}

	/* Da settore aperto a settore chiuso */
	if ( ((in_room->sector_type != SECT_INSIDE) && (in_room->room_flags != ROOM_INDOORS))
	&&	 ((to_room->sector_type == SECT_INSIDE) || (to_room->room_flags == ROOM_INDOORS)) )
	{
		if        ( weather_info.sky == SKY_BLIZZARD )
			send_to_char( "Un posto al riparo dalla bufera.. proprio quello che ci voleva.\n\r", ch );
		else if ( weather_info.sky == SKY_HAILSTORM )
			send_to_char( "Intravedi finalmente un rifugio dalla grandine che ti gela..\n\r", ch );
		else if ( weather_info.sky == SKY_ICESTORM )
			send_to_char( "Un sicuro rifugio dalla tempesta di ghiaccio che frusta il tuo corpo qui all'esterno.\n\r", ch );
		else if ( weather_info.sky == SKY_FOGGY )
			send_to_char( "Al riparo di un tetto non rischierai di perderti.. questa nebbia e' davvero insidiosa.\n\r", ch );
	}

	if ( ((in_room->sector_type != SECT_INSIDE) && (in_room->room_flags != ROOM_INDOORS))
	&&	 ((to_room->sector_type != SECT_INSIDE) && (to_room->room_flags != ROOM_INDOORS)) )
	{
		if ( weather_info.sky == SKY_BLIZZARD )
		{
			/* I hate these damned nested if statements, but it's the nicest way I
		 	*	know of to do this one */
			if ( IS_AFFECTED(ch, AFF_FLYING) && (IS_SET(ch->parts, PART_WINGS)) )
			{
				/* we've established we're a flying race. Now what size wings do we have?? */
				if ( (ch->race == race_lookup("pixie"))
				||	 (ch->race == race_lookup("avian")) )		/* small */
				{
					if ( number_range(0, 100) >= 88 )	/* 15 % chance for these poor souls */
					{
						send_to_char( "La neve fiacca le tue ali rendendole ali sono stanche e logore..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.2;
					}
				}

				if ( (ch->race == race_lookup("kenku"))
				||	 (ch->race == race_lookup("gryphon"))
				||	 (ch->race == race_lookup("gryfalcon"))
				||	 (ch->race == race_lookup("phoenix")) )		/* medium */
				{
					if ( number_range(0, 100) >= 93 )	/* 10 % chance for these poor souls */
					{
						send_to_char( "Hai volato per molto tempo..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.3;
					}
				}

				if ( (ch->race == race_lookup("draconian"))
				||	 (ch->race == race_lookup("crystal-dragon"))
				||	 (ch->race == race_lookup("silver-dragon"))
				||	 (ch->race == race_lookup("valkyrie")) )	/* rather large */
				{
					if ( number_range(0, 100) >= 98 )	/* 5 % chance for these poor souls */
					{
						send_to_char( "Non hai piu' forza per muovere le ali.\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.4;
					}
				}
			}
			else if ( IS_AFFECTED(ch, AFF_FLYING) )
			{
				if ( number_range(0, 100) >= 78 )	/* 25 % chance for these poor souls */
				{
					send_to_char( "Le tue ali sono indolenzite.\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}

			if ( IS_AFFECTED(ch, AFF_INFRARED) )
			{
				if ( number_range(0, 100) >= 78 )	/* 25 % chance of sight loss */
				{
					send_to_char( "Gli occhi ti bruciano un po' per la stanchezza.\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}
		}
		else if ( weather_info.sky == SKY_HAILSTORM )
		{
			/* I hate these damned nested if statements, but it's the nicest way I
			 *	know of to do this one */
			if ( IS_AFFECTED(ch, AFF_FLYING) && IS_SET(ch->parts,PART_WINGS) )
			{
				/* we've established we're a flying race. Now what size wings do we have?? */
				if ( (ch->race == race_lookup("pixie"))
				||	 (ch->race == race_lookup("avian")) )		/* small */
				{
					if ( number_range(0, 100) >= 88 )	/* 15 % chance for these poor souls */
					{
						send_to_char( "Le tue ali vengono bagnate e appesantite dalla pioggia..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.2;
					}
				}

				if ( (ch->race == race_lookup("kenku"))
				||	 (ch->race == race_lookup("gryphon"))
				||	 (ch->race == race_lookup("gryfalcon"))
				||	 (ch->race == race_lookup("phoenix")) )		/* medium */
				{
					if ( number_range(0, 100) >= 93 ) /* 10 % chance for these poor souls */
					{
						send_to_char( "Le tue ali sono zuppe di pioggia..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.3;
					}
				}

				if ( (ch->race == race_lookup("draconian"))
				||	 (ch->race == race_lookup("crystal-dragon"))
				||	 (ch->race == race_lookup("silver-dragon"))
				||	 (ch->race == race_lookup("valkyrie")) )		/* rather large */
				{
					if ( number_range(0, 100) >= 98 ) /* 5 % chance for these poor souls */
					{
						send_to_char( "Le tue ali sono pesantissime a causa dell'acqua..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.4;
					}
				}
			}
			else if ( IS_AFFECTED(ch, AFF_FLYING) )
			{
				if ( number_range(0, 100) >= 78 )		/* 25 % chance for these poor souls */
				{
					send_to_char( "L'acqua impregna le tue stanche ali..\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}

			if ( IS_AFFECTED(ch, AFF_INFRARED) )
			{
				if ( number_range(0, 100) >= 78 )		/* 25 % chance of sight loss */
				{
					send_to_char( "La tua vista viene sfocata dalla pioggia che copre ogni cosa..\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}
		}
		else if ( weather_info.sky == SKY_ICESTORM )
		{
			/* I hate these damned nested if statements, but it's the nicest way I
			 *	know of to do this one */
			if ( IS_AFFECTED(ch, AFF_FLYING) && (IS_SET(ch->parts,PART_WINGS)) )
			{
				/* we've established we're a flying race. Now what size wings do we have?? */
				if ( (ch->race == race_lookup("pixie"))
				||	 (ch->race == race_lookup("avian")) )		/* small */
				{
					if ( number_range(0, 100) >= 88 ) /* 15 % chance for these poor souls */
					{
						send_to_char( "Le schegge di grandine indolenziscono le tue ali..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.2;
					}
				}

				if ( (ch->race == race_lookup("kenku"))
				||	 (ch->race == race_lookup("gryphon"))
				||	 (ch->race == race_lookup("gryfalcon"))
				||	 (ch->race == race_lookup("phoenix")) )	/* medium */
				{
					if ( number_range(0, 100) >= 93 ) /* 10 % chance for these poor souls */
					{
						send_to_char( "Le tue ali cominciano a dolere per la grandine..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.3;
					}
				}

				if ( (ch->race == race_lookup("draconian"))
				||	 (ch->race == race_lookup("crystal-dragon"))
				||	 (ch->race == race_lookup("silver-dragon"))
				||	 (ch->race == race_lookup("valkyrie")) )	/* rather large */
				{
					if ( number_range(0, 100) >= 98 ) /* 5 % chance for these poor souls */
					{
						send_to_char( "In queste condizioni le ali non ti dureranno a lungo.\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.4;
					}
				}
			}
			else if ( IS_AFFECTED(ch, AFF_FLYING) )
			{
				if ( number_range(0, 100) >= 78 ) /* 25 % chance for these poor souls */
				{
					send_to_char( "Le frustate della grandine hanno indolenzito le tue ali intorpidendole..\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}

			if ( IS_AFFECTED(ch, AFF_INFRARED) )
			{
				if ( number_range(0, 100) >= 78 ) /* 25 % chance of sight loss */
				{
					send_to_char( "Gli occhi di bruciano un po' mentre cerchi di guardare oltre la tempesta..\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}
		}
		else if ( weather_info.sky == SKY_FOGGY )
		{
			if ( IS_AFFECTED(ch, AFF_FLYING) && (IS_SET(ch->parts,PART_WINGS)) )
			{
				if ( (ch->race == race_lookup("pixie"))
				||	 (ch->race == race_lookup("avian")) )		/* small */
				{
					if ( number_range(0, 100) >= 88 ) /* 15 % chance for these poor souls */
					{
						send_to_char( "Le tue ali non ce la fanno piu'.\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.2;
					}
				}

				if ( (ch->race == race_lookup("kenku"))
				||	 (ch->race == race_lookup("gryphon"))
				||	 (ch->race == race_lookup("gryfalcon"))
				||	 (ch->race == race_lookup("phoenix")) )		/* medium */
				{
					if ( number_range(0, 100) >= 93 ) /* 10 % chance for these poor souls */
					{
						send_to_char( "Le tue ali cominciano ad essere troppo stanche..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.3;
					}
				}

				if ( (ch->race == race_lookup("draconian"))
				||	 (ch->race == race_lookup("crystal-dragon"))
				||	 (ch->race == race_lookup("silver-dragon"))
				||	 (ch->race == race_lookup("valkyrie")) )		/* rather large */
				{
					if ( number_range(0, 100) >= 98 ) /* 5 % chance for these poor souls */
					{
						send_to_char( "La nebbia rende il volo troppo faticoso..\n\r", ch );
						ch->point[MOVE] = ch->point[MOVE] / 1.4;
					}
				}
			}
			else if ( IS_AFFECTED(ch, AFF_FLYING) )
			{
				if ( number_range(0, 100) >= 78 ) /* 25 % chance for these poor souls */
				{
					send_to_char( "La nebbia pesa sulle tue ali che a fatica la fendono..\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}

			if ( IS_AFFECTED(ch, AFF_INFRARED) )
			{
				if ( number_range(0, 100) >= 78 ) /* 25 % chance of sight loss */
				{
					send_to_char( "Non e' cosi' facile penetrare la nebbia con lo sguardo..\n\r", ch );
					ch->point[MOVE] = ch->point[MOVE] / 1.2;
				}
			}
		}
	}	/* fine if da stanza all'aperto ad un'altra all'aperto */
#endif

		move = movement_loss[UMIN( SECT_MAX - 1, in_room->sector_type )] + movement_loss[UMIN( SECT_MAX - 1, to_room->sector_type )];

		/* Xandra: i giocatori un po' newbie vengono facilitati nel movimento */
		if ( ch->level <= LEVEL_NEWBIE+10 )
		{
			move /= 3;
		}
		else
			move /= 2;	/* i.e. the average */

		/* Xandra: Se il pg e' ranger e si trova nella foresta allora il suo passo
		 * e' piu' esperto */
		if ( ch->class == CLASS_RANGER && in_room->sector_type == SECT_FOREST )
			move--;

		/* conditional effects */
		if ( IS_AFFECTED(ch, AFF_FLYING) || IS_AFFECTED(ch, AFF_HASTE) )
			move /= 2;

		if ( IS_AFFECTED(ch, AFF_SLOW) )
			move *= 2;

		if ( !MOUNTED(ch) && ch->point[MOVE] < move )
		{
			act( "Sei troppo stanc$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
			return;
		}

		if ( !MOUNTED(ch)
		&&	(ch->in_room->sector_type == SECT_DESERT
			|| IS_WATER(ch->in_room)
			|| ch->in_room->sector_type == SECT_MOUNTAIN) )	/* Xandra */
			WAIT_STATE( ch, 4 );
		else
			WAIT_STATE( ch, 2 );

		if ( !MOUNTED(ch) || !IS_ADMIN(ch) )
			ch->point[MOVE] -= move;
	}

	if ( !IS_AFFECTED(ch, AFF_SNEAK) && !IS_AFFECTED(ch, AFF_CAMOUFLAGE) && ch->invis_level < LEVEL_HERO )
	{
		if (!IS_MOB(ch)
		&&	ch->in_room->sector_type != SECT_INSIDE
		&&	ch->in_room->sector_type != SECT_CITY
		&&	number_percent() < get_skill(ch, gsn_quiet_movement) )
		{
			if ( MOUNTED(ch) )
				sprintf( buf, "$n se ne va, cavalcando %s.", MOUNTED(ch)->short_descr );
			else
				sprintf( buf, "$n se ne va." );
			check_improve( ch, gsn_quiet_movement, TRUE, 1 );
		}
		else
		{
			if ( MOUNTED(ch) )
				sprintf( buf, "$n se ne va $T, cavalcando %s.", MOUNTED(ch)->short_descr );
			else
				sprintf( buf, "$n se ne va $T." );
		}

		act( buf, ch, NULL, dir_name_to[door], TO_ROOM, POS_REST );
	}

	if (IS_AFFECTED(ch, AFF_CAMOUFLAGE)
	&&	to_room->sector_type != SECT_FIELD
	&&	to_room->sector_type != SECT_FOREST
	&&	to_room->sector_type != SECT_MOUNTAIN
	&&	to_room->sector_type != SECT_HILLS )
	{
		REMOVE_BIT( ch->affected_by, AFF_CAMOUFLAGE );
		send_to_char( "Esci dal tuo nascondiglio.\n\r", ch );
		act( "$n esce dal suo nascondiglio.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( (IS_AFFECTED(ch, AFF_HIDE))
	&& (to_room->sector_type == SECT_FOREST
	||  to_room->sector_type == SECT_FIELD) )
	{
		REMOVE_BIT( ch->affected_by, AFF_HIDE );
		send_to_char( "Esci dall'ombra.\n\r", ch );
		act( "$n esce dall'ombra.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	mount = MOUNTED( ch );
	char_from_room( ch );
	char_to_room( ch, to_room );

	if ( ch->in_room != to_room )
	{
		bug( "Move_char: ch e' morto!", 0 );
		return;
	}

	/* room record for tracking */
	if ( !IS_MOB(ch) && ch->in_room )
		room_record( ch->name, in_room, door );

	if ( !IS_AFFECTED(ch, AFF_SNEAK) && ch->invis_level < LEVEL_HERO )
	{
		if ( mount )
			act( "$n e' arrivat$x, cavalcando $N.", ch, NULL, mount, TO_ROOM, POS_REST );
		else
			act( "$n e' arrivat$x.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	do_look( ch, "auto" );

	if ( mount )
	{
		char_from_room( mount );
		char_to_room( mount, to_room );
		ch->riding = TRUE;
		mount->riding = TRUE;
	}

	if ( in_room == to_room )	/* no circular follows */
		return;

	for ( fch = to_room->people, room_has_pc = FALSE; fch != NULL; fch = fch_next )
	{
		fch_next = fch->next_in_room;
		if ( !IS_MOB(fch) )
			room_has_pc = TRUE;
	}

	for ( fch = to_room->people; fch != NULL; fch = fch_next )
	{
		fch_next = fch->next_in_room;

		/* greet progs for items carried by people in room */
		for ( obj = fch->carrying; room_has_pc && obj != NULL; obj = obj->next_content )
		{
			if ( IS_SET(obj->progtypes, OPROG_GREET) )
				( obj->pIndexData->oprogs->greet_prog ) ( obj, ch );
		}

		/* greet programs for npcs */
		if ( room_has_pc && IS_SET(fch->progtypes, MPROG_GREET) )
			( fch->pIndexData->mprogs->greet_prog ) ( fch, ch );
	}

	/* entry programs for items */
	for ( obj = ch->carrying; room_has_pc && obj != NULL; obj = obj->next_content )
	{
		if ( IS_SET(obj->progtypes, OPROG_ENTRY) )
			( obj->pIndexData->oprogs->entry_prog ) ( obj );
	}

	for ( fch = in_room->people; fch != NULL; fch = fch_next )
	{
		fch_next = fch->next_in_room;

		if ( fch->master == ch && IS_AFFECTED(fch, AFF_CHARM) && fch->position < POS_STAND )
			do_stand( fch, "" );

		if ( fch->master == ch && fch->position == POS_STAND && can_see_room(fch, to_room) )
		{
			if ( IS_SET(ch->in_room->room_flags, ROOM_LAW) && (IS_MOB(fch) && IS_SET(fch->act, ACT_AGGRESSIVE)) )
			{
				act( "Non puoi portare con te $N nella citta'.", ch, NULL, fch, TO_CHAR, POS_REST );
				act( "Non puoi entrare nella citta'.", fch, NULL, NULL, TO_CHAR, POS_REST );
				continue;
			}

			act( "Segui $N.", fch, NULL, ch, TO_CHAR, POS_REST );
			move_char( fch, door, TRUE );
		}
	}

	for ( obj = ch->in_room->contents; room_has_pc && obj != NULL; obj = obj->next_content )
	{
		if ( IS_SET(obj->progtypes, OPROG_GREET) )
			( obj->pIndexData->oprogs->greet_prog ) ( obj, ch );
	}

	if ( IS_SET(ch->progtypes, MPROG_ENTRY) )
		( ch->pIndexData->mprogs->entry_prog ) ( ch );
}


void do_north( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_NORTH, FALSE );
}

void do_east( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_EAST, FALSE );
}

void do_south( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_SOUTH, FALSE );
}

void do_west( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_WEST, FALSE );
}

void do_up( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_UP, FALSE );
}

void do_down( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_DOWN, FALSE );
}

void do_northeast( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_NORTHEAST, FALSE );
}

void do_southeast( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_SOUTHEAST, FALSE );
}

void do_southwest( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_SOUTHWEST, FALSE );
}

void do_northwest( CHAR_DATA *ch, char *argument )
{
	move_char( ch, DIR_NORTHWEST, FALSE );
}


void do_run( CHAR_DATA *ch, char *argument )
{
	char	arg1[MIL];
	char	arg2[MIL];
	int		door, count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "corri <direzione> <passi>\n\r", ch );
		return;
	}

	if ( (door = find_exit(ch, arg1, TRUE)) < -1 )
		return;


	if ( (count = atoi(arg2)) < 1 )
	{
		send_to_char( "Devi fare almeno un passo per poter correre.\n\r", ch );
		return;
	}
}


char *get_exit_string( char *arg )
{
	/* inglese */
	if ( !str_prefix(arg, "north") )								return( "north" );
	if ( !str_prefix(arg, "east") )									return( "east" );
	if ( !str_prefix(arg, "south") )								return( "south" );
	if ( !str_prefix(arg, "west") )									return( "west" );
	if ( !str_prefix(arg, "up") )									return( "up" );
	if ( !str_prefix(arg, "down") )									return( "down" );
	if ( !str_cmp(arg, "ne") || !str_prefix(arg, "northeast") )		return( "northeast" );
	if ( !str_cmp(arg, "se") || !str_prefix(arg, "southeast") )		return( "southeast" );
	if ( !str_cmp(arg, "sw") || !str_prefix(arg, "southwest") )		return( "southwest" );
	if ( !str_cmp(arg, "nw") || !str_prefix(arg, "northwest") )		return( "northwest" );

	/* inglese */
	if ( !str_prefix(arg, "nord") )									return( "nord" );
	if ( !str_prefix(arg, "est") )									return( "est" );
	if ( !str_prefix(arg, "sud") )									return( "sud" );
	if ( !str_prefix(arg, "ovest") )								return( "ovest" );
	if ( !str_prefix(arg, "alto") )									return( "alto" );
	if ( !str_prefix(arg, "basso") )								return( "basso" );
	if ( !str_cmp(arg, "ne") || !str_prefix(arg, "nordest") )		return( "nordest" );
	if ( !str_cmp(arg, "se") || !str_prefix(arg, "sudest") )		return( "sudest" );
	if ( !str_cmp(arg, "so") || !str_prefix(arg, "sudovest") )		return( "sudovest" );
	if ( !str_cmp(arg, "no") || !str_prefix(arg, "nordovest") )		return( "nordovest" );

	return( NULL );
}


int find_exit( CHAR_DATA *ch, char *arg, bool view_msg )
{
	/* inglese */
	if ( !str_prefix(arg, "north") )								return( 0 );
	if ( !str_prefix(arg, "east") )									return( 1 );
	if ( !str_prefix(arg, "south") )								return( 2 );
	if ( !str_prefix(arg, "west") )									return( 3 );
	if ( !str_prefix(arg, "up") )									return( 4 );
	if ( !str_prefix(arg, "down") )									return( 5 );
	if ( !str_cmp(arg, "ne") || !str_prefix(arg, "northeast") )		return( 6 );
	if ( !str_cmp(arg, "se") || !str_prefix(arg, "southeast") )		return( 7 );
	if ( !str_cmp(arg, "sw") || !str_prefix(arg, "southwest") )		return( 8 );
	if ( !str_cmp(arg, "nw") || !str_prefix(arg, "northwest") )		return( 9 );

	/* inglese */
	if ( !str_prefix(arg, "nord") )									return( 0 );
	if ( !str_prefix(arg, "est") )									return( 1 );
	if ( !str_prefix(arg, "sud") )									return( 2 );
	if ( !str_prefix(arg, "ovest") )								return( 3 );
	if ( !str_prefix(arg, "alto") )									return( 4 );
	if ( !str_prefix(arg, "basso") )								return( 5 );
	if ( !str_cmp(arg, "ne") || !str_prefix(arg, "nordest") )		return( 6 );
	if ( !str_cmp(arg, "se") || !str_prefix(arg, "sudest") )		return( 7 );
	if ( !str_cmp(arg, "so") || !str_prefix(arg, "sudovest") )		return( 8 );
	if ( !str_cmp(arg, "no") || !str_prefix(arg, "nordovest") )		return( 9 );

	if ( view_msg == TRUE )
		act( "Non vedi uscite a $T.", ch, NULL, arg, TO_CHAR, POS_REST );

	return( -1 );
}


int find_door( CHAR_DATA *ch, char *arg )
{
	EXIT_DATA	*pexit;
	int			door = -1;
	/*~~~~~~~~~~~~~~~~~~*/

	door = find_exit( ch, arg, TRUE );
	if ( door == -1 )
	{
		for ( door = 0; door < MAX_DIR; door++ )
		{
			if ((pexit = ch->in_room->exit[door]) != NULL
			&&	IS_SET(pexit->exit_info, EX_ISDOOR)
			&&	pexit->keyword != NULL
			&&	is_name(arg, pexit->keyword) )
				return( door );
		}

		act( "Non vedi nessun $T qui.", ch, NULL, arg, TO_CHAR, POS_REST );
		return( -1 );
	}

	if ( (pexit = ch->in_room->exit[door]) == NULL )
	{
		act( "Non trovi nessuna uscita $T qui.", ch, NULL, arg, TO_CHAR, POS_REST );
		return( -1 );
	}

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
		send_to_char( "Non puoi fare cio'.\n\r", ch );
		return( -1 );
	}

	return( door );
}

/* scan.c */
char *const		distance[4] = { "giusto qua.", "vicino a %s.", "non lontano da %s.", "distante da %s." };

void scan_list	( ROOM_INDEX_DATA * scan_room, CHAR_DATA * ch, sh_int depth, sh_int door );
void scan_char	( CHAR_DATA * victim, CHAR_DATA * ch, sh_int depth, sh_int door );


void do_scan2( CHAR_DATA *ch, char *argument )
{
	EXIT_DATA	*pExit;
	sh_int		door;
	/*~~~~~~~~~~~~~~~*/

	send_to_char( "Guardandoti attorno vedi:\n\r", ch );
	scan_list( ch->in_room, ch, 0, -1 );
	for ( door = 0; door < MAX_DIR; door++ )
	{
		if ( (pExit = ch->in_room->exit[door]) == NULL
		|| pExit->to_room == NULL
		|| IS_SET(pExit->exit_info, EX_CLOSED) )
			continue;

		scan_list( pExit->to_room, ch, 1, door );
	}
}


void scan_list( ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch, sh_int depth, sh_int door )
{
	CHAR_DATA	*rch;
	/*~~~~~~~~~~~~~*/

	if ( scan_room == NULL )
		return;
	for ( rch = scan_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch == ch )
			continue;
		if ( !IS_MOB(rch) && rch->invis_level > get_trust(ch) )
			continue;
		if ( can_see(ch, rch) )
			scan_char( rch, ch, depth, door );
	}
}


void scan_char( CHAR_DATA *victim, CHAR_DATA *ch, sh_int depth, sh_int door )
{
	char				buf[MIL], buf2[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	buf[0] = '\0';

	strcat( buf, (is_affected(victim, gsn_doppelganger) && !IS_SET(ch->act, PLR_HOLYLIGHT)) ?
		PERS(victim->doppel, ch) : PERS(victim, ch) );
	strcat( buf, ", " );
	sprintf( buf2, distance[depth], dir_name[door] );
	strcat( buf, buf2 );
	strcat( buf, "\n\r" );

	send_to_char( buf, ch );
}


void do_open( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	int			door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Aprire cosa?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_here(ch, arg)) != NULL )
	{
		/* open portal */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET(obj->value[1], EX_ISDOOR) )
			{
				send_to_char( "Non puoi fare cio'.\n\r", ch );
				return;
			}

			if ( !IS_SET(obj->value[1], EX_CLOSED) )
			{
				send_to_char( "E' gia' aperto.\n\r", ch );
				return;
			}

			if ( IS_SET(obj->value[1], EX_LOCKED) )
			{
				send_to_char( "E' chiuso a chiave.\n\r", ch );
				return;
			}

			REMOVE_BIT( obj->value[1], EX_CLOSED );
			act( "Apri $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n apre $p.", ch, obj, NULL, TO_ROOM, POS_REST );
			return;
		}

		/* 'open object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Non e' un contenitore.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_CLOSED) )
		{
			send_to_char( "E' gia' aperto.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
		{
			send_to_char( "Non puoi fare cio'.\n\r", ch );
			return;
		}

		if ( IS_SET(obj->value[1], CONT_LOCKED) )
		{
			send_to_char( "E' chiuso a chiave.\n\r", ch );
			return;
		}

		REMOVE_BIT( obj->value[1], CONT_CLOSED );
		act( "Apri $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n apre un $p.", ch, obj, NULL, TO_ROOM, POS_REST );	/* (RR) get_article */
		return;
	}

	if ( (door = find_door(ch, arg)) >= 0 )
	{
		/* 'open door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA		*pexit;
		EXIT_DATA		*pexit_rev;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pexit = ch->in_room->exit[door];
		if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			send_to_char( "E' gia' aperta.\n\r", ch );
			return;
		}

		if ( IS_SET(pexit->exit_info, EX_LOCKED) )
		{
			send_to_char( "E' chiusa a chiave.\n\r", ch );
			return;
		}

		REMOVE_BIT( pexit->exit_info, EX_CLOSED );
		act( "$n apre un $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );	/* (RR) get_article */
		send_to_char( "Ok.\n\r", ch );

		/* open the other side */
		if ((to_room = pexit->to_room) != NULL
		&&	(pexit_rev = to_room->exit[rev_dir[door]]) != NULL
		&&	pexit_rev->to_room == ch->in_room )
		{
			CHAR_DATA	*rch;
			/*~~~~~~~~~~~~~*/

			REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
				act( "$d si apre.", rch, NULL, pexit_rev->keyword, TO_CHAR, POS_REST );
		}
		return;
	}
}


void do_close( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	int			door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Chiudere cosa?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_here(ch, arg)) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET(obj->value[1], EX_ISDOOR) || IS_SET(obj->value[1], EX_NOCLOSE) )
			{
				send_to_char( "Non puoi fare cio'.\n\r", ch );
				return;
			}


			if ( IS_SET(obj->value[1], EX_CLOSED) )
			{
				send_to_char( "E' gia' chiusa.\n\r", ch );
				return;
			}

			SET_BIT( obj->value[1], EX_CLOSED );
			act( "Chiudi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n chiude $p.", ch, obj, NULL, TO_ROOM, POS_REST );
			return;
		}

		/* 'close object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Questo oggetto non e' un contenitore.\n\r", ch );
			return;
		}

		if ( IS_SET(obj->value[1], CONT_CLOSED) )
		{

			send_to_char( "E' gia' chiuso.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
		{
			send_to_char( "Non puoi fare cio'.\n\r", ch );
			return;
		}

		SET_BIT( obj->value[1], CONT_CLOSED );
		act( "Chiudi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n chiude $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( (door = find_door(ch, arg)) >= 0 )
	{
		/* 'close door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA		*pexit;
		EXIT_DATA		*pexit_rev;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pexit = ch->in_room->exit[door];
		if ( IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			send_to_char( "E' gia' chiusa.\n\r", ch );
			return;
		}

		SET_BIT( pexit->exit_info, EX_CLOSED );
		act( "$n chiude $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );	/* Xandra */
		send_to_char( "Ok.\n\r", ch );

		/* close the other side */
		if ((to_room = pexit->to_room) != NULL
		&&	(pexit_rev = to_room->exit[rev_dir[door]]) != 0
		&&	pexit_rev->to_room == ch->in_room )
		{
			CHAR_DATA	*rch;
			/*~~~~~~~~~~~~~*/

			SET_BIT( pexit_rev->exit_info, EX_CLOSED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
				act( "$d si chiude.", rch, NULL, pexit_rev->keyword, TO_CHAR, POS_REST );
		}
		return;
	}
}


/*
 * Added can_see check. Kio.
 */
bool has_key( CHAR_DATA *ch, int key )
{
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~*/

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == key )
			if ( can_see_obj(ch, obj) )
				return( TRUE );
	}

	return( FALSE );
}


bool has_key_ground( CHAR_DATA *ch, int key )
{
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~*/

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == key )
		{
			if ( can_see_obj(ch, obj) )
				return( TRUE );
		}
	}

	return( FALSE );
}


void do_lock( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	int			door;
	CHAR_DATA	*rch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Bloccare cosa?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_here(ch, arg)) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET(obj->value[1], EX_ISDOOR) || IS_SET(obj->value[1], EX_NOCLOSE) )
			{
				send_to_char( "Non puoi fare cio'.\n\r", ch );
				return;
			}

			if ( !IS_SET(obj->value[1], EX_CLOSED) )
			{
				send_to_char( "Non e' chiusa.\n\r", ch );
				return;
			}

			if ( obj->value[4] < 0 || IS_SET(obj->value[1], EX_NOLOCK) )
			{
				send_to_char( "Non puo' essere bloccata.\n\r", ch );
				return;
			}

			if ( !has_key(ch, obj->value[4]) )
			{
				send_to_char( "Ti manca la chiave.\n\r", ch );
				return;
			}

			if ( IS_SET(obj->value[1], EX_LOCKED) )
			{
				send_to_char( "E' gia' chiusa a chiave.\n\r", ch );
				return;
			}

			SET_BIT( obj->value[1], EX_LOCKED );
			act( "Blocchi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n blocca $p.", ch, obj, NULL, TO_ROOM, POS_REST );
			return;
		}

		/* 'lock object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Questo non e' un contenitore.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_CLOSED) )
		{
			send_to_char( "Non e' chiuso.\n\r", ch );
			return;
		}

		if ( obj->value[2] < 0 )
		{
			send_to_char( "Non puo' essere bloccato.\n\r", ch );
			return;
		}

		if ( !has_key(ch, obj->value[2]) )
		{
			send_to_char( "Ti manca la chiave.\n\r", ch );
			return;
		}

		if ( IS_SET(obj->value[1], CONT_LOCKED) )
		{
			send_to_char( "E' gia' chiuso a chiave.\n\r", ch );
			return;
		}

		SET_BIT( obj->value[1], CONT_LOCKED );
		act( "Blocchi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n blocca $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( (door = find_door(ch, arg)) >= 0 )
	{
		/* 'lock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA		*pexit;
		EXIT_DATA		*pexit_rev;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pexit = ch->in_room->exit[door];
		if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			send_to_char( "Non e' chiuso.\n\r", ch );
			return;
		}

		if ( pexit->key < 0 )
		{
			send_to_char( "Non puo' essere bloccata.\n\r", ch );
			return;
		}

		if ( !has_key(ch, pexit->key) && !has_key_ground(ch, pexit->key) )
		{
			send_to_char( "Ti manca la chiave.\n\r", ch );
			return;
		}

		if ( IS_SET(pexit->exit_info, EX_LOCKED) )
		{
			send_to_char( "E' gia' chiusa a chiave.\n\r", ch );
			return;
		}

		SET_BIT( pexit->exit_info, EX_LOCKED );
		send_to_char( "*Click*\n\r", ch );
		act( "$n chiude $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );

		/* lock the other side */
		if ((to_room = pexit->to_room) != NULL
		&&	(pexit_rev = to_room->exit[rev_dir[door]]) != 0
		&&	pexit_rev->to_room == ch->in_room )
		{
			SET_BIT( pexit_rev->exit_info, EX_LOCKED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
				act( "$d scatta.", rch, NULL, pexit_rev->keyword, TO_CHAR, POS_REST );
		}

		return;
	}
}


void do_unlock( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	OBJ_DATA	*obj;
	int			door;
	CHAR_DATA	*rch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Sbloccare cosa?\n\r", ch );
		return;
	}

	if ( (obj = get_obj_here(ch, arg)) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( IS_SET(obj->value[1], EX_ISDOOR) )
			{
				send_to_char( "Non puoi fare cio'.\n\r", ch );
				return;
			}

			if ( !IS_SET(obj->value[1], EX_CLOSED) )
			{
				send_to_char( "Non e' chiuso.\n\r", ch );
				return;
			}

			if ( obj->value[4] < 0 )
			{
				send_to_char( "Non puo' essere bloccato.\n\r", ch );
				return;
			}

			if ( !has_key(ch, obj->value[4]) )
			{
				send_to_char( "Ti manca la chiave.\n\r", ch );
				return;
			}

			if ( !IS_SET(obj->value[1], EX_LOCKED) )
			{
				send_to_char( "E' gia' bloccata.\n\r", ch );
				return;
			}

			REMOVE_BIT( obj->value[1], EX_LOCKED );
			act( "Sblocchi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n sblocca $p.", ch, obj, NULL, TO_ROOM, POS_REST );
			return;
		}

		/* 'unlock object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Questo non e' un contenitore.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_CLOSED) )
		{
			send_to_char( "Non e' chiuso.\n\r", ch );
			return;
		}

		if ( obj->value[2] < 0 )
		{
			send_to_char( "Non puo' essere sbloccato.\n\r", ch );
			return;
		}

		if ( !has_key(ch, obj->value[2]) )
		{
			send_to_char( "Ti manca la chiave.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_LOCKED) )
		{
			send_to_char( "E' gia' chiuso a chiave.\n\r", ch );
			return;
		}

		REMOVE_BIT( obj->value[1], CONT_LOCKED );
		act( "Sblocchi $p.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n sblocca $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		return;
	}

	if ( (door = find_door(ch, arg)) >= 0 )
	{
		/* 'unlock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA		*pexit;
		EXIT_DATA		*pexit_rev;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pexit = ch->in_room->exit[door];
		if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			send_to_char( "Non e' chiusa.\n\r", ch );
			return;

		}

		if ( pexit->key < 0 )
		{
			send_to_char( "Non puo' essere sbloccata.\n\r", ch );
			return;
		}

		if ( !has_key(ch, pexit->key) && !has_key_ground(ch, pexit->key) )
		{
			send_to_char( "Ti manca la chiave.\n\r", ch );
			return;
		}

		if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
		{
			send_to_char( "E' gia' sbloccata.\n\r", ch );
			return;
		}

		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		send_to_char( "*Click*\n\r", ch );
		act( "$n sblocca $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );

		/* unlock the other side */
		if ((to_room = pexit->to_room) != NULL
		&&	(pexit_rev = to_room->exit[rev_dir[door]]) != NULL
		&&	pexit_rev->to_room == ch->in_room )
		{
			REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
				act( "$d scatta.", rch, NULL, pexit_rev->keyword, TO_CHAR, POS_REST );
		}
		return;
	}
}


void do_pick( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*gch;
	OBJ_DATA	*obj;
	int			door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Scassinare cosa?\n\r", ch );
		return;
	}

	if ( MOUNTED(ch) )
	{
		send_to_char( "Non puoi scassinare standotene sulla tua cavalcatura.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

	/* look for guards */
	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
		if ( IS_MOB(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
		{
			act( "$N e' troppo vicin$X alla serratura.", ch, NULL, gch, TO_CHAR, POS_REST );
			return;
		}
	}

	if ( !IS_MOB(ch) && number_percent() > get_skill(ch, gsn_pick_lock) )
	{
		send_to_char( "Hai fallito.\n\r", ch );
		check_improve( ch, gsn_pick_lock, FALSE, 2 );
		return;
	}

	if ( (obj = get_obj_here(ch, arg)) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET(obj->value[1], EX_ISDOOR) )
			{
				send_to_char( "Non puoi fare cio'.\n\r", ch );
				return;
			}

			if ( !IS_SET(obj->value[1], EX_CLOSED) )
			{
				send_to_char( "Non e' chiuso.\n\r", ch );
				return;
			}

			if ( obj->value[4] < 0 )
			{
				send_to_char( "Non puo' essere sbloccato.\n\r", ch );
				return;
			}

			if ( IS_SET(obj->value[1], EX_PICKPROOF) )
			{
				send_to_char( "Fallisci, e' troppo difficile per te.\n\r", ch );
				return;
			}

			REMOVE_BIT( obj->value[1], EX_LOCKED );
			act( "Scassini la serratura.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n scassina una serratura.", ch, obj, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_pick_lock, TRUE, 2 );
			return;
		}

		/* 'pick object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Questo non e' un contenitore.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_CLOSED) )
		{
			send_to_char( "Non e' chiuso.\n\r", ch );
			return;
		}

		if ( obj->value[2] < 0 )
		{
			send_to_char( "Non puo' essere sbloccato.\n\r", ch );
			return;
		}

		if ( !IS_SET(obj->value[1], CONT_LOCKED) )
		{
			send_to_char( "E' gia' sbloccato.\n\r", ch );
			return;
		}

		if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
		{
			send_to_char( "Fallisci, e' troppo difficile per te.\n\r", ch );
			return;
		}

		REMOVE_BIT( obj->value[1], CONT_LOCKED );
		act( "Scassini la serratura.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$n scassina una serratura.", ch, obj, NULL, TO_ROOM, POS_REST );
		check_improve( ch, gsn_pick_lock, TRUE, 2 );
		return;
	}

	if ( (door = find_door(ch, arg)) >= 0 )
	{
		/* 'pick door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA		*pexit;
		EXIT_DATA		*pexit_rev;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pexit = ch->in_room->exit[door];
		if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_ADMIN(ch) )
		{
			send_to_char( "Non e' chiusa.\n\r", ch );
			return;
		}

		if ( pexit->key < 0 && !IS_ADMIN(ch) )
		{
			send_to_char( "Non puo' essere sbloccata.\n\r", ch );
			return;
		}

		if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
		{
			send_to_char( "E' gia' sbloccata.\n\r", ch );
			return;
		}

		if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_ADMIN(ch) )
		{
			send_to_char( "Fallisci, e' troppo difficile per te.\n\r", ch );
			return;
		}

		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		send_to_char( "*Click*\n\r", ch );
		act( "$n scassina $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );
		check_improve( ch, gsn_pick_lock, TRUE, 2 );

		/* pick the other side */
		if ((to_room = pexit->to_room) != NULL
		&&	(pexit_rev = to_room->exit[rev_dir[door]]) != NULL
		&&	pexit_rev->to_room == ch->in_room )
		{
			REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
		}
	}
}


void do_stand( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj = NULL;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( argument[0] != '\0' )
	{
		if ( ch->position == POS_FIGHT )
		{
			send_to_char( "Forse e' meglio che tu finisca di combattere?\n\r", ch );
			return;
		}

		obj = get_obj_list( ch, argument, ch->in_room->contents );
		if ( obj == NULL )
		{
			send_to_char( "Non lo vedi qui.\n\r", ch );
			return;
		}

		if (obj->item_type != ITEM_FURNITURE
		||	(!IS_SET(obj->value[2], STAND_AT)
			&& !IS_SET(obj->value[2], STAND_ON)
			&& !IS_SET(obj->value[2], STAND_IN)) )
		{
			send_to_char( "Non trovi nessun posto dove fermarti.\n\r", ch );
			return;
		}

		if ( ch->on != obj && count_users(obj) >= obj->value[0] )
		{
			act( "Non c'e' posto per fermarsi su $p.", ch, obj, NULL, TO_ROOM, POS_DEAD );
			return;
		}
	}

	switch ( ch->position )
	{
	case POS_SLEEP:
		if ( IS_AFFECTED(ch, AFF_SLEEP) )
		{
			send_to_char( "Non riesci a svegliarti!\n\r", ch );
			return;
		}

		if ( obj == NULL )
		{
			send_to_char( "Ti svegli e ti alzi in piedi.\n\r", ch );
			act( "$n si sveglia e si alza in piedi.", ch, NULL, NULL, TO_ROOM, POS_REST );
			dream_kill( ch );
			ch->on = NULL;
		}
		else if ( IS_SET(obj->value[2], STAND_AT) )
		{
			act( "Ti svegli e ti alzi in piedi.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			act( "$n si sveglia e si alza in piedi.", ch, obj, NULL, TO_ROOM, POS_REST );
			dream_kill( ch );
		}
		else if ( IS_SET(obj->value[2], STAND_ON) )
		{
			act( "Ti svegli e ti alzi sul $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			act( "$n si sveglia e si alza sul $p.", ch, obj, NULL, TO_ROOM, POS_REST );
			dream_kill( ch );
		}
		else
		{
			act( "Ti svegli e ti alzi nel $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			act( "$n si sveglia e si alza nel $p.", ch, obj, NULL, TO_ROOM, POS_REST );
			dream_kill( ch );
		}

		if ( IS_SET(ch->act, PLR_HARA_KIRI) )
		{
			send_to_char( "Senti il sangue scaldare il tuo corpo.\n\r", ch );
			REMOVE_BIT( ch->act, PLR_HARA_KIRI );
		}

		ch->position = POS_STAND;
		do_look( ch, "auto" );
		break;

	case POS_REST:
	case POS_SIT:
		if ( obj == NULL )
		{
			send_to_char( "Ti alzi.\n\r", ch );
			act( "$n si alza.", ch, NULL, NULL, TO_ROOM, POS_REST );
			ch->on = NULL;
		}
		else if ( IS_SET(obj->value[2], STAND_AT) )
		{
			act( "Ti alzi.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si alza.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], STAND_ON) )
		{
			act( "Ti alzi sul $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si alza sul $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			act( "Ti alzi nel $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si alza nel $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}

		ch->position = POS_STAND;
		break;

	case POS_STAND:
		send_to_char( "Sei gia' in piedi.\n\r", ch );
		break;

	case POS_FIGHT:
		send_to_char( "Stai gia' combattendo!\n\r", ch );
		break;
	}
}


void do_rest( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj = NULL;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->position == POS_FIGHT )
	{
		send_to_char( "Stai gia' combattendo!\n\r", ch );
		return;
	}

	if ( MOUNTED(ch) )
	{
		send_to_char( "Non puoi riposarti mentre stai cavalcando.\n\r", ch );
		return;
	}

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi riposarti mentre stai cavalcando.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	{
		send_to_char( "Ma stai gia' dormendo.\n\r", ch );
		return;
	}

	/* okay, now that we know we can rest, find an object to rest on */
	if ( argument[0] != '\0' )
	{
		obj = get_obj_list( ch, argument, ch->in_room->contents );
		if ( obj == NULL )
		{
			send_to_char( "Non vedi nulla qui.\n\r", ch );
			return;
		}
	}
	else
		obj = ch->on;

	if ( obj != NULL )
	{
		if (!IS_SET(obj->item_type, ITEM_FURNITURE)
		||	(!IS_SET(obj->value[2], REST_ON)
			&& !IS_SET(obj->value[2], REST_IN)
			&& !IS_SET(obj->value[2], REST_AT)) )
		{
			send_to_char( "Non puoi riposarti qui.\n\r", ch );
			return;
		}

		if ( obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0] )
		{
			act( "Non c'e' spazio per riposarsi su $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		ch->on = obj;
	}

	switch ( ch->position )
	{
	case POS_SLEEP:
		if ( obj == NULL )
		{
			act( "Ti desti e ti stropicci gli occhi.", ch, NULL, NULL, TO_CHAR, POS_SLEEP );
			act( "$n si desta e si stropiccia gli occhi.", ch, NULL, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], REST_AT) )
		{
			act( "Ti desti e stropicci gli occhi.", ch, obj, NULL, TO_CHAR, POS_SLEEP );
			act( "$n si desta e stropiccia gli occhi.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], REST_ON) )
		{
			act( "Ti desti e stropicci gli occhi sul $p.", ch, obj, NULL, TO_CHAR, POS_SLEEP );
			act( "$n si desta e stropiccia gli occhi sul $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			act( "Ti desti e stropicci gli occhi nel $p.", ch, obj, NULL, TO_CHAR, POS_SLEEP );
			act( "$n si desta e stropiccia gli occhi nel $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}

		ch->position = POS_REST;
		break;

	case POS_REST:
		send_to_char( "Ti stai gia' riposando.\n\r", ch );
		break;

	case POS_STAND:
		if ( obj == NULL )
		{
			send_to_char( "Ti riposi.\n\r", ch );
			act( "$n si siede e riposa.", ch, NULL, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], REST_AT) )
		{
			act( "Ti siedi $p e riposi.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n siede $p e riposa.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], REST_ON) )

		{
			act( "Ti siedi su $p e riposi.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si siede su $p e riposa.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			act( "Ti riposi dentro $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si riposa dentro $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}

		ch->position = POS_REST;
		break;

	case POS_SIT:
		if ( obj == NULL )
		{
			send_to_char( "Ti riposi.\n\r", ch );
			act( "$n riposa.", ch, NULL, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], REST_AT) )
		{
			act( "Ti riposi.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si riposa.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], REST_ON) )
		{
			act( "Ti riposi su $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si riposa su $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			act( "Ti riposi dentro $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si riposa dentro $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}

		ch->position = POS_REST;

		if ( IS_SET(ch->act, PLR_HARA_KIRI) )
		{
			send_to_char( "Senti il sangue scaldare il tuo corpo.\n\r", ch );
			REMOVE_BIT( ch->act, PLR_HARA_KIRI );
		}
		break;
	}
}


void do_sit( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj = NULL;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->position == POS_FIGHT )
	{
		send_to_char( "Prima finisci di battagliare.\n\r", ch );
		return;
	}

	if ( MOUNTED(ch) )
	{
		send_to_char( "Non puoi sederti da nessuna parte mentre cavalchi.\n\r", ch );
		return;
	}

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi sederti mentre stai cavalcando.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	{
		send_to_char( "Stai dormendo.\n\r", ch );
		return;
	}

	/* okay, now that we know we can sit, find an object to sit on */
	if ( argument[0] != '\0' )
	{
		obj = get_obj_list( ch, argument, ch->in_room->contents );
		if ( obj == NULL )
		{
			send_to_char( "Non lo vedi qui.\n\r", ch );
			return;
		}
	}
	else
		obj = ch->on;

	if ( obj != NULL )
	{
		if (!IS_SET(obj->item_type, ITEM_FURNITURE)
		||	(!IS_SET(obj->value[2], SIT_ON)
			&& !IS_SET(obj->value[2], SIT_IN)
			&& !IS_SET(obj->value[2], SIT_AT)) )
		{
			send_to_char( "Non puoi sedere laggiu'.\n\r", ch );
			return;
		}

		if ( obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0] )
		{
			act( "Non vi e' spazio su $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		ch->on = obj;
	}

	switch ( ch->position )
	{
	case POS_SLEEP:
		if ( obj == NULL )
		{
			send_to_char( "Ti svegli e ti siedi.\n\r", ch );
			act( "$n si sveglia e si siede.", ch, NULL, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], SIT_AT) )
		{
			act( "Ti svegli e ti siedi.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			act( "$n si sveglia e si siede.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], SIT_ON) )
		{
			act( "Ti svegli e ti siedi su $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			act( "$n si sveglia e si siede su $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			act( "Ti svegli e ti siedi dentro $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			act( "$n si sveglia e si siede dentro $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}

		ch->position = POS_SIT;
		break;

	case POS_REST:
		if ( obj == NULL )
			send_to_char( "Smetti di riposarti.\n\r", ch );
		else if ( IS_SET(obj->value[2], SIT_AT) )
		{
			act( "Ti siedi su $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si siede su $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], SIT_ON) )
		{
			act( "Ti siedi dentro $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si siede dentro $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}

		ch->position = POS_SIT;
		break;

	case POS_SIT:
		act( "Sei gia' sedut$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
		break;

	case POS_STAND:
		if ( obj == NULL )
		{
			send_to_char( "Ti siedi per terra.\n\r", ch );
			act( "$n si siede per terra.", ch, NULL, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], SIT_AT) )
		{
			act( "Ti siedi su $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si siede su $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else if ( IS_SET(obj->value[2], SIT_ON) )
		{
			act( "Ti siedi su $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si siede su $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}
		else
		{
			act( "Ti siedi dentro $p.", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "$n si siede dentro $p.", ch, obj, NULL, TO_ROOM, POS_REST );
		}

		ch->position = POS_SIT;
		break;
	}

	if ( IS_SET(ch->act, PLR_HARA_KIRI) )
	{
		send_to_char( "Senti il sangue scaldare il tuo corpo.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_HARA_KIRI );
	}
}


void do_sleep( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj = NULL;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( MOUNTED(ch) )
	{
		send_to_char( "Non puoi addormentarti mentre sei sopra una cavalcatura.\n\r", ch );
		return;
	}

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi addormentarti mentre cavalchi.\n\r", ch );
		return;
	}

	switch ( ch->position )
	{
	case POS_SLEEP:
		send_to_char( "Stai gia' dormendo.\n\r", ch );
		break;

	case POS_REST:
	case POS_SIT:
	case POS_STAND:
		if ( argument[0] == '\0' && ch->on == NULL )
		{
			send_to_char( "Ti addormenti.\n\r", ch );
			act( "$n inizia a dormire.", ch, NULL, NULL, TO_ROOM, POS_REST );
			ch->position = POS_SLEEP;
		}
		else	/* find an object and sleep on it */
		{
			if ( argument[0] == '\0' )
				obj = ch->on;
			else
				obj = get_obj_list( ch, argument, ch->in_room->contents );

			if ( obj == NULL )
			{
				send_to_char( "Non vedi cio' qui.\n\r", ch );
				return;
			}

			if (obj->item_type != ITEM_FURNITURE
			||	(!IS_SET(obj->value[2], SLEEP_ON)
				&& !IS_SET(obj->value[2], SLEEP_IN)
				&& !IS_SET(obj->value[2], SLEEP_AT)) )
			{
				send_to_char( "Difficile addormentarti su una cosa simile!\n\r", ch );
				return;
			}

			if ( ch->on != obj && count_users(obj) >= obj->value[0] )
			{
				act( "Non c'e' spazio su $p per distendere le stanche membra.", ch, obj, NULL, TO_CHAR, POS_DEAD );
				return;
			}

			ch->on = obj;
			if ( IS_SET(obj->value[2], SLEEP_AT) )
			{
				act( "Ti addormenti su $p.", ch, obj, NULL, TO_CHAR, POS_REST );
				act( "$n si addormenta su $p.", ch, obj, NULL, TO_ROOM, POS_REST );
				dream_reset( ch );
			}
			else if ( IS_SET(obj->value[2], SLEEP_ON) )
			{
				act( "Ti addormenti su $p.", ch, obj, NULL, TO_CHAR, POS_REST );
				act( "$n si addormenta su $p.", ch, obj, NULL, TO_ROOM, POS_REST );
				dream_reset( ch );
			}
			else
			{
				act( "Ti addormenti dentro $p.", ch, obj, NULL, TO_CHAR, POS_REST );
				act( "$n si addormenta dentro $p.", ch, obj, NULL, TO_ROOM, POS_REST );
				dream_reset( ch );
			}

			ch->position = POS_SLEEP;
		}
		break;

	case POS_FIGHT:
		send_to_char( "Stai combattendo!\n\r", ch );
		break;
	}
}


void do_wake( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		do_stand( ch, argument );
		return;
	}

	if ( !IS_AWAKE(ch) )
	{
		send_to_char( "Ti svegli di soprassalto.\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( IS_AWAKE(victim) )
	{
		act( "$N e' gia' svegli$X.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{
		act( "Non riesci a svegliar$M!", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	act( "$n ti sveglia.", ch, NULL, victim, TO_VICT, POS_SLEEP );
	do_stand( victim, "" );
}


void do_sneak( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( MOUNTED(ch) )
	{
		send_to_char( "Montando una cavalcatura e' difficile non fare rumore.\n\r", ch );
		return;
	}

    if ( RIDDEN(ch) )
    {
        send_to_char( "Non puoi nasconderti se sei cavalcato.\n\r", ch );
        return;
    }

	send_to_char( "Cerchi di muoverti silenziosamente.\n\r", ch );
	affect_strip( ch, gsn_sneak );

	if ( IS_AFFECTED(ch, AFF_SNEAK) )
		return;

	if ( number_percent() < get_skill(ch, gsn_sneak) )
	{
		check_improve( ch, gsn_sneak, TRUE, 3 );
		af.where = TO_AFFECTS;
		af.type = gsn_sneak;
		af.level = ch->level;
		af.duration = ch->level;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_SNEAK;
		affect_to_char( ch, &af );
	}
	else
		check_improve( ch, gsn_sneak, FALSE, 3 );
}


void do_hide( CHAR_DATA *ch, char *argument )
{
	int forest;
	/*~~~~~~~*/

	if ( MOUNTED(ch) )
	{
		send_to_char( "Non puoi nasconderti su di una cavalcatura.\n\r", ch );
		return;
	}

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi nasconderti nell'ombra mentre stai cavalcando.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_FAERIE_FIRE) )
	{
		send_to_char( "non puoi nasconderti se della luce ti avvolge.\n\r", ch );
		return;
	}


	forest = 0;
	forest += ch->in_room->sector_type == SECT_FOREST ? 60 : 0;
	forest += ch->in_room->sector_type == SECT_FIELD ? 60 : 0;

	send_to_char( "Provi a nasconderti tra le ombre.\n\r", ch );

	if ( number_percent() < get_skill(ch, gsn_hide) - forest )
	{
		SET_BIT( ch->affected_by, AFF_HIDE );
		check_improve( ch, gsn_hide, TRUE, 3 );
	}
	else
	{
		if ( IS_AFFECTED(ch, AFF_HIDE) )
			REMOVE_BIT( ch->affected_by, AFF_HIDE );
		check_improve( ch, gsn_hide, FALSE, 3 );
	}

	WAIT_STATE( ch, skill_table[gsn_hide].beats );
}


void do_camouflage( CHAR_DATA *ch, char *argument )
{
    if ( skill_failure_check(ch, gsn_camouflage, 0, 0, "Non sapresti proprio come fare a camuffarti..") )

	if ( MOUNTED(ch) )
	{
		send_to_char( "Non puoi camuffarti sopra una cavalcatura.\n\r", ch );
		return;
	}

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi camuffarti se stai cavalcando.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_FAERIE_FIRE) )
	{
		send_to_char( "Non puoi camuffarti se una luce ti avvolge.\n\r", ch );
		return;
	}

	if ( ch->in_room->sector_type != SECT_FOREST && ch->in_room->sector_type != SECT_HILLS && ch->in_room->sector_type != SECT_MOUNTAIN )
	{
		send_to_char( "Non c'e' rifugio qui.\n\r", ch );
		act( "$n prova a camuffarsi dietro all'unica foglia per terra..", ch, NULL, NULL, TO_ROOM, POS_REST );
		return;
	}

	send_to_char( "Provi a camuffarti.\n\r", ch );
	WAIT_STATE( ch, skill_table[gsn_camouflage].beats );

	if ( IS_AFFECTED(ch, AFF_CAMOUFLAGE) )
		REMOVE_BIT( ch->affected_by, AFF_CAMOUFLAGE );

	if ( IS_MOB(ch) || number_percent() < get_skill(ch, gsn_camouflage) )
	{
		SET_BIT( ch->affected_by, AFF_CAMOUFLAGE );
		check_improve( ch, gsn_camouflage, TRUE, 1 );
	}
	else
		check_improve( ch, gsn_camouflage, FALSE, 1 );
}


/*
 * Contributed by Alander
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->affected_by, AFF_HIDE) )
	{
		send_to_char( "Esci dall'ombra.\n\r", ch );
		REMOVE_BIT( ch->affected_by, AFF_HIDE );
		act( "$n esce dall'ombra.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_SET(ch->affected_by, AFF_FADE) )
	{
		send_to_char( "Esci dall'ombra.\n\r", ch );
		REMOVE_BIT( ch->affected_by, AFF_FADE );
		act( "$n esce dall'ombra.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_SET(ch->affected_by, AFF_CAMOUFLAGE) )
	{
		send_to_char( "Esci dal tuo nascondiglio.\n\r", ch );
		REMOVE_BIT( ch->affected_by, AFF_CAMOUFLAGE );
		act( "$n esce dal suo nascondiglio.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_SET(ch->affected_by, AFF_INVISIBLE) )
	{
		send_to_char( "Ritorni visibile.\n\r", ch );
		affect_strip( ch, gsn_invis );
		affect_strip( ch, gsn_mass_invis );
		REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
		act( "$n ritorna visibile.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_SET(ch->affected_by, AFF_IMP_INVIS) )
	{
		send_to_char( "Ritorni visibile.\n\r", ch );
		affect_strip( ch, gsn_imp_invis );
		REMOVE_BIT( ch->affected_by, AFF_IMP_INVIS );
		act( "$n ritorna visibile.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_SET(ch->affected_by, AFF_SNEAK) && !IS_MOB(ch) && !IS_SET(race_table[ch->race].aff, AFF_SNEAK) )
	{
		send_to_char( "I tuoi passi tornano pesanti.. ricominci a muoverti rumorosamente.\n\r", ch );
		affect_strip( ch, gsn_sneak );
		REMOVE_BIT( ch->affected_by, AFF_SNEAK );
	}

	affect_strip( ch, gsn_mass_invis );

	if ( CAN_DETECT(ch, ADET_EARTHFADE) )
	{
		send_to_char( "Ritorni nella tua forma neutrale.\n\r", ch );
		act( "La terra plasma $n davanti a te.", ch, NULL, NULL, TO_ROOM, POS_REST );
		affect_strip( ch, gsn_earthfade );
		WAIT_STATE( ch, (PULSE_VIOLENCE / 2) );
		REMOVE_BIT( ch->detection, ADET_EARTHFADE );
	}
}


/*
 * Sceglie piu' o meno casualmente un oggetto potente da togliere al pg.
 */
OBJ_DATA *object_for_tribute( CHAR_DATA *ch )
{
	OBJ_DATA	*obj;
	sh_int		i;
	/*~~~~~~~~~~~~~*/

	if ( ch == NULL )
		return( NULL );

	/* guarda nell'equipaggiamento */
	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		/* non gli oggetti "limbo" */
		if ( obj->pIndexData->vnum < 100 )
			continue;

		/* non gli oggetti cabala */
		for ( i = 1; i < MAX_CABAL; i++ )
		{
			if ( obj->pIndexData->vnum == cabal_table[i].obj_vnum )
				continue;
		}

		/* Se e' un oggetto limitato allora lo sceglie */
		if ( obj->pIndexData->limit != -1 )
			return( obj );

		/* altrimenti potrebbe scegliere un altro oggetto di alot livello, da implementare forse */
	}

	return( NULL );
}


void do_recall( CHAR_DATA *ch, char *argument )
{
	char			buf[MSL];
	CHAR_DATA		*victim;
	ROOM_INDEX_DATA *location;
	int				point;
	int				skill;
	bool			dt; /* controlla se il pg si trovi in una dt oppure no */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_GOOD(ch) )
		point = hometown_table[ch->hometown].recall[0];
	else if ( IS_EVIL(ch) )
		point = hometown_table[ch->hometown].recall[2];
	else
		point = hometown_table[ch->hometown].recall[1];

	if ( IS_MOB(ch) && !IS_SET(ch->act, ACT_PET) )
	{
		send_to_char( "Solo i giocatori possono richiamare.\n\r", ch );
		return;
	}

	dt = room_is_dt( ch );

	if ( ch->level >= LEVEL_NEWBIE+5 && !IS_ADMIN(ch) && dt == FALSE )
	{
		sprintf( buf, "Il richiamo e' per avventurieri di bassa esperienza.\n\r" );
		send_to_char( buf, ch );
		return;
	}

	if ( ch->desc != NULL && current_time - ch->last_fight_time < FIGHT_DELAY_TIME
	&& !IS_ADMIN(ch) && ch->level >= LEVEL_NEWBIE+5 )
	{
		send_to_char( "Hai troppa adrenalina in circolo per poter pregare ora.\n\r", ch );
		return;
	}

	if ( ch->desc == NULL && !IS_MOB(ch) )
		point = hometown_table[number_range( 0, 4 )].recall[number_range( 0, 2 )];

	act( "$n prega per essere trasportat$x!", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( (location = get_room_index(point)) == NULL )
	{
		send_to_char( "Ti accorgi d'aver smarrito ogni punto di riferimento.. e ora?\n\r", ch );
		return;
	}

	if ( ch->in_room == location )
		return;

	if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || IS_AFFECTED(ch, AFF_CURSE) || IS_RAFFECTED(ch->in_room, AFF_ROOM_CURSE) )
	{
		act( "Gli dei ti hanno abbandonat$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( (victim = ch->fighting) != NULL )
	{
		send_to_char( "Stai combattendo!\n\r", ch );

		if ( IS_MOB(ch) )
			skill = 40 + ch->level;
		else
			skill = get_skill( ch, gsn_recall );

		if ( number_percent() < 80 * skill / 100 )
		{
			check_improve( ch, gsn_recall, FALSE, 6 );
			WAIT_STATE( ch, 4 );
			sprintf( buf, "Fallisci!.\n\r" );
			send_to_char( buf, ch );
			return;
		}

		gain_exp( ch, -(ch->level*2) );
		check_improve( ch, gsn_recall, TRUE, 4 );
		send_to_char( "Riesci a sfuggire al combattimento!\n\r", ch );
		stop_fighting( ch, TRUE );
	}

	/*
	 * Xandra: se il pg si trovava in una dt e non era newbie o immortale per uscirne gli dei prendono come pegno un
	 * oggetto tosto, raro se c'e', e tutte le monete d'oro
	 */
	if ( !IS_MOB(ch) && dt == TRUE && ch->level >= LEVEL_NEWBIE && !IS_ADMIN(ch) )
	{
		OBJ_DATA	*tribute;
		/*~~~~~~~~~~~~~~~~~*/

		tribute = object_for_tribute( ch );

		/* Se non trova un oggetto adatto, allora toglie un po' di premi quest */
		if ( tribute == NULL )
		{
			/* Sono buono, se ne ha meno di 500 non toglie nulla */
			if ( ch->pcdata->questpoints > 500 )
				ch->pcdata->questpoints -= number_range( 5, 50 );
		}
		else
			extract_obj( tribute );

		ch->gold = 0;
		send_to_char( "Gli dei per farti uscire da questo luogo sperduto esigono un loro tributo in oggetti e oro.", ch );
	}

	ch->point[MOVE] /= 4;
	act( "$n sparisce.", ch, NULL, NULL, TO_ROOM, POS_REST );
	char_from_room( ch );
	char_to_room( ch, location );
	act( "$n appare nella stanza.", ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );

	if ( ch->pet != NULL )
	{
		char_from_room( ch->pet );
		char_to_room( ch->pet, location );
		do_look( ch->pet, "auto" );
	}
}


void do_train( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	CHAR_DATA	*mob;
	sh_int		stat = -1;
	char		*pOutput = NULL;
	int			cost;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	/* Check for trainer. */
	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
		if ( IS_MOB(mob) && (IS_SET(mob->act, ACT_PRACTICE) || IS_SET(mob->act, ACT_TRAIN) || IS_SET(mob->act, ACT_GAIN)) )
			break;
	}

	if ( mob == NULL )
	{
		send_to_char( "Non puoi farlo qui.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		sprintf( buf, "Possiedi %d sessioni di allenamento.\n\r", ch->train );
		send_to_char( buf, ch );

		strcpy( buf, "Puoi allenare:" );
		if ( ch->perm_stat[STR] < get_max_train2(ch, STR) )		strcat( buf, " forza," );
		if ( ch->perm_stat[INT] < get_max_train2(ch, INT) )		strcat( buf, " intelligenza," );
		if ( ch->perm_stat[WIS] < get_max_train2(ch, WIS) )		strcat( buf, " saggezza," );
		if ( ch->perm_stat[DEX] < get_max_train2(ch, DEX) )		strcat( buf, " destrezza," );
		if ( ch->perm_stat[CON] < get_max_train2(ch, CON) )		strcat( buf, " costituzione," );
		if ( ch->perm_stat[CHA] < get_max_train2(ch, CHA) )		strcat( buf, " carisma," );
		if ( ch->pcdata->perm_point[LIFE] < pc_race_table[ch->race].max_point[LIFE] )
			strcat( buf, " vita," );
		if ( ch->pcdata->perm_point[MANA] < pc_race_table[ch->race].max_point[MANA] )
			strcat( buf, " mana," );
		if ( ch->pcdata->perm_point[MOVE] < pc_race_table[ch->race].max_point[MOVE] )
			strcat( buf, " movimento," );

		if ( buf[strlen(buf) - 1] != ':' )
		{
			strcat( buf, ".\n\r" );
			send_to_char( buf, ch );
		}
		else
			send_to_char( "Tu, non puoi allenarti ancora!\n\r", ch );

		return;
	}

	cost = 1;

	if ( !str_prefix(argument, "forza") )
	{
		if ( class_table[ch->class].attr_prime == STR )
			cost = 1;
		stat = STR;
		pOutput = "forza";
	}
	else if ( !str_prefix(argument, "intelligenza") )
	{
		if ( class_table[ch->class].attr_prime == INT )
			cost = 1;
		stat = INT;
		pOutput = "intelligenza";
	}
	else if ( !str_prefix(argument, "saggezza") )
	{
		if ( class_table[ch->class].attr_prime == WIS )
			cost = 1;
		stat = WIS;
		pOutput = "saggezza";
	}
	else if ( !str_prefix(argument, "destrezza") )
	{
		if ( class_table[ch->class].attr_prime == DEX )
			cost = 1;
		stat = DEX;
		pOutput = "destrezza";
	}
	else if ( !str_prefix(argument, "costituzione") )
	{
		if ( class_table[ch->class].attr_prime == CON )
			cost = 1;
		stat = CON;
		pOutput = "costituzione";
	}
	else if ( !str_prefix(argument, "carisma") )
	{
		if ( class_table[ch->class].attr_prime == CHA )
			cost = 1;
		stat = CHA;
		pOutput = "carisma";

		/*
		 * sprintf( buf, "Non puoi aumentare il carisma. E' fuori dalle tue possibilita'.\n\r"); send_to_char( buf,
		 * ch ); return;
		 */
	}
	else if ( !str_prefix(argument, "vita") )
	{
		cost = 1;
	}
	else if ( !str_prefix(argument, "mana") )
	{
		cost = 1;
	}
	else if ( !str_prefix(argument, "movimento") )
	{
		cost = 1;
	}
	else
	{
		send_to_char( "Non puoi allenare nulla del genere.\n\r", ch );
		return;
	}

	if ( !str_prefix(argument, "vita") )
	{
		if ( cost > ch->train )
		{
			send_to_char( "Non possiedi abbastanza allenamenti.\n\r", ch );
			return;
		}

		/* Xandra */
		if ( ch->pcdata->perm_point[LIFE] >= pc_race_table[ch->race].max_point[LIFE] )
		{
			send_to_char( "Non puoi allenare la tua resistenza piu' di cosi'.\n\r", ch );
			return;
		}

		ch->train -= cost;
		ch->pcdata->perm_point[LIFE] += 10;
		ch->max_point[LIFE] += 10;
		ch->point[LIFE] += 10;
		act( "Aumenti la tua resistenza!", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( !str_prefix(argument, "mana") )
	{

		if ( cost > ch->train )
		{
			send_to_char( "Non possiedi abbastanza allenamenti.\n\r", ch );
			return;
		}

		/* Xandra */
		if ( ch->pcdata->perm_point[MANA] >= pc_race_table[ch->race].max_point[MANA] )
		{
			send_to_char( "Non puoi allenare la tua energia piu' di cosi'.\n\r", ch );
			return;
		}

		ch->train -= cost;
		ch->pcdata->perm_point[MANA] += 10;
		ch->max_point[MANA] += 10;
		ch->point[MANA] += 10;
		act( "Il tuo potere aumenta!", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( !str_prefix(argument, "movimento") )
	{
		if ( cost > ch->train )
		{
			send_to_char( "Non possiedi abbastanza allenamenti.\n\r", ch );
			return;
		}

		/* Xandra */
		if ( ch->pcdata->perm_point[MOVE] >= pc_race_table[ch->race].max_point[MOVE] )
		{
			send_to_char( "Non puoi allenare i tuoi movimenti piu' di cosi'.\n\r", ch );
			return;
		}

		ch->train -= cost;
		ch->pcdata->perm_point[MOVE] += 10;
		ch->max_point[MOVE] += 10;
		ch->point[MOVE] += 10;
		act( "Il tuo movimento aumenta!", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( ch->perm_stat[stat] >= get_max_train2(ch, stat) )
	{
		act( "La tua $t e' gia' al massimo.", ch, pOutput, NULL, TO_CHAR, POS_REST );
		return;
	}

	if ( cost > ch->train )
	{
		send_to_char( "Non possiedi abbastanza allenamenti.\n\r", ch );
		return;
	}
	else
	{
		ch->train -= cost;
		ch->perm_stat[stat] += 1;

		/* (RR) inserire la cerca_articolo */
		act( "Aumenti $t!", ch, pOutput, NULL, TO_CHAR, POS_REST );
	}
}


void do_track( CHAR_DATA *ch, char *argument )
{
	ROOM_HISTORY_DATA	*rh;
	EXIT_DATA			*pexit;
	char				buf[MSL];
	static char			*door[] = { "nord", "est", "sud", "ovest", "alto", "basso", "nordest", "sudest", "sudovest", "nordovest", "that way" };	/* (RR) Controllare 'sto that way */
	int					d;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( get_char_world(ch, argument) == NULL )
	{
		send_to_char( "Non c'e' nessuna traccia qui.\n\r", ch );
		return;
	}

	if ( skill_failure_check(ch, gsn_track, 0, 0, "Non c'e' nessuna traccia qui.") )
		return;

	WAIT_STATE( ch, skill_table[gsn_track].beats );
	act( "$n scruta il terreno in cerca di tracce.", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( IS_MOB(ch) || number_percent() < get_skill(ch, gsn_track) )
	{
		if ( IS_MOB(ch) )
		{
			ch->status = 0;
			if ( ch->last_fought != NULL && !IS_SET(ch->act, ACT_NOTRACK) )
				add_mind( ch, ch->last_fought->name );
		}

		for ( rh = ch->in_room->history; rh != NULL; rh = rh->next )
		{
			if ( is_name(argument, rh->name) )
			{
				check_improve( ch, gsn_track, TRUE, 1 );
				if ( (d = rh->went) == -1 )
					continue;
				sprintf( buf, "Le tracce di %s portano a %s.\n\r", capitalize(rh->name), door[d] );
				send_to_char( buf, ch );
				if ( (pexit = ch->in_room->exit[d]) != NULL && IS_SET(pexit->exit_info, EX_ISDOOR) && pexit->keyword != NULL )
				{
					sprintf( buf, "%s", door[d] );
					do_open( ch, buf );
				}

				move_char( ch, rh->went, FALSE );
				return;
			}
		}
	}

	send_to_char( "Non vedi nessuna traccia.\n\r", ch );
	if ( IS_MOB(ch) )
		ch->status = 5;		/* for stalker */
	check_improve( ch, gsn_track, FALSE, 1 );
}


void do_vampire( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	int			level, duration;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_vampire, 0, 0, "Provi a rivelare la tua vera forma, ma qualcosa non va'.") )
		return;

	if ( is_affected(ch, gsn_vampire) )
	{
		send_to_char( "Non puoi divenire piu' vampiro di cosi'!\n\r", ch );
		return;
	}

	if ( get_skill(ch, gsn_vampire) < 50 )
	{
		send_to_char( "Vai a chiedere aiuto al Maestro delle missioni.\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_vampire) )
	{
		send_to_char( "Se vuoi divenire maggiormente vampiro devi uccidere un giocatore..\n\r", ch );
		return;
	}

	if ( weather_info.sunlight == SUN_LIGHT || weather_info.sunlight == SUN_RISE )
		send_to_char( "Avresti dovuto che il sole tramontasse per trasformarti in vampiro.\n\r", ch );

	level = ch->level;
	duration = level / 10;
	duration += 5;

	/* haste */
	af.where = TO_AFFECTS;
	af.type = gsn_vampire;
	af.level = level;
	af.duration = duration;
	af.location = APPLY_DEX;
	af.modifier = 1 + ( level / 20 );
	af.bitvector = AFF_HASTE;
	affect_to_char( ch, &af );

	/* giant strength + infrared */
	af.where = TO_AFFECTS;
	af.type = gsn_vampire;
	af.level = level;
	af.duration = duration;
	af.location = APPLY_STR;
	af.modifier = 1 + ( level / 20 );
	af.bitvector = AFF_INFRARED;
	affect_to_char( ch, &af );

	/* size */
	af.where = TO_AFFECTS;
	af.type = gsn_vampire;
	af.level = level;
	af.duration = duration;
	af.location = APPLY_SIZE;
	af.modifier = 1 + ( level / 50 );
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );

	/* damroll */
	af.where = TO_AFFECTS;
	af.type = gsn_vampire;
	af.level = level;
	af.duration = duration;
	af.location = APPLY_DAMROLL;
	af.modifier = ch->damroll;
	af.bitvector = AFF_BERSERK;
	affect_to_char( ch, &af );

	/* negative immunity */
	af.where = TO_IMMUNE;
	af.type = gsn_vampire;
	af.duration = duration;
	af.level = level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = IMM_NEGATIVE;
	affect_to_char( ch, &af );

	/* vampire flag */
	af.where = TO_ACT_FLAG;
	af.type = gsn_vampire;
	af.level = level;
	af.duration = duration;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = PLR_VAMPIRE;
	affect_to_char( ch, &af );

	send_to_char( "Senti che il potere a te familiare fuisce nel tuo corpo, mentre il tuo aspetto muta e diviene terribile.\n\r", ch );
	act( "Qualcosa di orribile capita a $n. Ora una creatura uscita dagli incubi si trova davanti a te.",
		ch, NULL, NULL, TO_ROOM, POS_REST );
}


void do_vbite( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

    if ( skill_failure_check(ch, gsn_vampiric_bite, 0, 0, "Non sai di certo mordere altre creature.") )
		return;

	if ( !IS_VAMPIRE(ch) )
	{
		send_to_char( "Devi trasformarti in vampiro prima di poter mordere.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Mordere chi?\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( victim->position != POS_SLEEP )
	{
		send_to_char( "Deve essere addormentato.\n\r", ch );
		return;
	}

	if ( (IS_MOB(ch)) && (!(IS_MOB(victim))) )
		return;

	if ( victim == ch )
	{
		send_to_char( "Non ci riesci proprio, non sei un contorsionista!\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( victim->fighting != NULL )
	{
		send_to_char( "Non rieschi a mordere chi e' impegnato in lotta.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_vampiric_bite].beats );

	if ( victim->point[LIFE] < (0.8 * victim->max_point[LIFE]) && (IS_AWAKE(victim)) )
	{
		act( "$N e' ferit$X e sospettos$X.. non e' il caso, ritenta poi.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( current_time - victim->last_fight_time < 300 && IS_AWAKE(victim) )
	{
		act( "$N e' sospettos$X.. non e' proprio il caso di tentare di morderlo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if (!IS_AWAKE(victim)
	&&	(IS_MOB(ch) || number_percent() < ((get_skill(ch, gsn_vampiric_bite) * 0.7) + (2 * (ch->level - victim->level)))) )
	{
		check_improve( ch, gsn_vampiric_bite, TRUE, 1 );
		one_hit( ch, victim, gsn_vampiric_bite, FALSE );
	}
	else
	{
		check_improve( ch, gsn_vampiric_bite, FALSE, 1 );
		damage( ch, victim, 0, gsn_vampiric_bite, DAM_NONE, TRUE );
	}

	/* Mob shouts if he doesn't die */
	if ( IS_MOB(victim) && victim->position == POS_FIGHT )
		do_yell( victim, "Aiuto! Mi sta attaccando!" );
}



void do_bash_door( CHAR_DATA *ch, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*gch;
	int			chance = 0;
	int			damage_bash, door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( argument, arg );

    if ( skill_failure_check(ch, gsn_bash_door, 0, OFF_BASH, "Urtare contro quale cosa?") )
		return;

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi sfondare le porte finche' tai cavalcando.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Sfondare quale porta in quale direzione?\n\r", ch );
		return;
	}

	if ( ch->fighting )
	{
		send_to_char( "Attendi fino a che il combattimento finisca.\n\r", ch );
		return;
	}

	/* look for guards */
	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
		if ( IS_MOB(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
		{
			act( "$N e' troppo vicin$X all'uscita.", ch, NULL, gch, TO_CHAR, POS_REST );
			return;
		}
	}

	if ( (door = find_door(ch, arg)) >= 0 )
	{
		/* 'bash door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA		*pexit;
		EXIT_DATA		*pexit_rev;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pexit = ch->in_room->exit[door];
		if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
		{
			send_to_char( "E' gia' aperta.\n\r", ch );
			return;
		}

		if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
		{
			send_to_char( "Bisognerebbe provare ad aprirla.\n\r", ch );
			return;
		}

		if ( IS_SET(pexit->exit_info, EX_NOPASS) )
		{
			send_to_char( "Uno scudo mistico protegge l'uscita.\n\r", ch );
			return;
		}

		/* modifiers */
		/* size and weight */
		chance += get_carry_weight( ch ) / 100;

		chance += ( ch->size - 2 ) * 20;

		/* stats */
		chance += get_curr_stat( ch, STR );

		if ( IS_AFFECTED(ch, AFF_FLYING) )
			chance -= 10;

		/* level chance += ch->level / 10; */
		chance += ( get_skill(ch, gsn_bash_door) - 90 );

		act( "Urti con forza contro $d, e provi a rompere $d!", ch, NULL, pexit->keyword, TO_CHAR, POS_REST );
		act( "Urti con forza $d, e provi a rompere $d!", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );

		if ( room_dark(ch->in_room) )
			chance /= 2;

		/* now the attack */
		if ( number_percent() < chance )
		{
			check_improve( ch, gsn_bash_door, TRUE, 1 );

			REMOVE_BIT( pexit->exit_info, EX_LOCKED );
			REMOVE_BIT( pexit->exit_info, EX_CLOSED );
			act( "$n urta violentemente contro $d e rompe la serratura.", ch, NULL, pexit->keyword, TO_ROOM, POS_REST );
			act( "Urti violentemente contro $d e rompi la serratura!", ch, NULL, NULL, TO_CHAR, POS_REST );


			/* open the other side */
			if ((to_room = pexit->to_room) != NULL
			&&	(pexit_rev = to_room->exit[rev_dir[door]]) != NULL
			&&	pexit_rev->to_room == ch->in_room )
			{
				CHAR_DATA	*rch;
				/*~~~~~~~~~~~~~*/

				REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
				REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
				for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
					act( "$d si apre.", rch, NULL, pexit_rev->keyword, TO_CHAR, POS_REST );
			}

			if ( number_percent() < chance )
			{
				move_char( ch, door, FALSE );
				ch->position = POS_REST;
			}

			WAIT_STATE( ch, skill_table[gsn_bash_door].beats );
		}
		else
		{
			act( "Perdi l'equilibrio e cadi rovinosamente!", ch, NULL, NULL, TO_CHAR, POS_REST );
			act( "$n perde l'equilibrio e cade rovinosamente.", ch, NULL, NULL, TO_ROOM, POS_REST );
			check_improve( ch, gsn_bash_door, FALSE, 1 );
			ch->position = POS_REST;
			WAIT_STATE( ch, skill_table[gsn_bash].beats * 3 / 2 );
			damage_bash = ch->damroll + number_range( 4, 4 + 4 * ch->size + chance / 5 );
			damage( ch, ch, damage_bash, gsn_bash, DAM_BASH, TRUE );
		}
		return;
	}
}


void do_blink( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

    if ( skill_failure_check(ch, gsn_blink, 0, 0, NULL) )
    	return;

	if ( arg[0] == '\0' )
	{
		sprintf( buf, "Il tuo stato blink corrente: %s.\n\r",
			IS_SET(ch->act, PLR_BLINK_ON) ? "attivo" : "disattivo" );
		send_to_char( buf, ch );
		return;
	}

	if ( !str_cmp(arg, "on") || !str_cmp(arg, "si") )
	{
		SET_BIT( ch->act, PLR_BLINK_ON );
		send_to_char( "Ora il tuo stato blink e'attivo.\n\r", ch );
		return;
	}

	if ( !str_cmp(arg, "off") || !str_cmp(arg, "no") )
	{
		REMOVE_BIT( ch->act, PLR_BLINK_ON );
		send_to_char( "Ora, il tuo stato blink e' disattivo.\n\r", ch );
		return;
	}

	send_to_char( "Penso che tu ti stia sbagliando.\n\r", ch );

}


void do_vanish( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *pRoomIndex;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_vanish, 0, 0, NULL) )
		return;

	if ( ch->point[MANA] < 25 )
	{
		send_to_char( "Non ha abbastanza energia magica.\n\r", ch );
		return;
	}

	ch->point[MANA] -= 25;

	WAIT_STATE( ch, skill_table[gsn_vanish].beats );

	if ( number_percent() > get_skill(ch, gsn_vanish) )
	{
		send_to_char( "Hai fallito.\n\r", ch );
		check_improve( ch, gsn_vanish, FALSE, 1 );
		return;
	}

	if ( ch->in_room == NULL || IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || cabal_area_check(ch) )

	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	for ( i = 0; i < 65535; i++ )
	{
		if ( (pRoomIndex = get_room_index(number_range(0, 65535))) == NULL )
			continue;
		if ( can_see_room(ch, pRoomIndex) && !room_is_private(pRoomIndex) && ch->in_room->area == pRoomIndex->area )
			break;
	}

	if ( pRoomIndex == NULL )
	{
		send_to_char( "Hai fallito.\n\r", ch );
		return;
	}

	act( "$n lancia per terra un piccolo globo.", ch, NULL, NULL, TO_ROOM, POS_REST );

	check_improve( ch, gsn_vanish, TRUE, 1 );

	if ( !IS_MOB(ch) && ch->fighting != NULL && number_bits(1) == 1 )
	{
		send_to_char( "Hai fallito.\n\r", ch );
		return;
	}

	act( "$n e' svanit$x!", ch, NULL, NULL, TO_ROOM, POS_REST );

	char_from_room( ch );
	char_to_room( ch, pRoomIndex );
	act( "$n appare dal nulla.", ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );
	stop_fighting( ch, TRUE );
}


void do_detect_sneak( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_detect_sneak, 0, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_detect_sneak) )
	{
		send_to_char( "Gia' percepisci chi si muove silenziosamente.\n\r", ch );
		return;
	}

	af.where = TO_DETECTS;
	af.type = gsn_detect_sneak;
	af.level = ch->level;
	af.duration = ch->level / 10;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = DETECT_SNEAK;
	affect_to_char( ch, &af );
	send_to_char( "Potrai percepire chi si muove silenziosamente.\n\r", ch );
}



void do_vtouch( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~*/

    if ( skill_failure_check(ch, gsn_vampiric_touch, 0, 0, "Ti manca l'abilita' per farlo.") )
		return;

	if ( !IS_VAMPIRE(ch) )
	{
		send_to_char( "Provaci..\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, argument)) == NULL )
	{
		send_to_char( "Non vedi questa persona qui.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Mossa molto intelligente..\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && victim == ch->leader )
	{
		send_to_char( "Non vorrai colpire il tuo adorabile maestro??\n\r", ch );
		return;
	}

	if ( IS_AFFECTED(victim, AFF_CHARM) )
	{
		send_to_char( "La tua vittima sta gia' dormendo.\n\r", ch );
		return;
	}

	if ( is_affected(victim, gsn_blackguard) )
	{
		act( "$N non ti permette di avvicinarti troppo.", ch, NULL, victim, TO_CHAR, POS_REST );
		return;
	}

	if ( is_affected(victim, gsn_vampiric_touch) )
	{
		send_to_char( "La sua vitalita' e' gia' stata lesa.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
	{
		send_to_char( "La tua vittima si trova in un luogo protetto.\n\r", ch );
		return;
	}

	victim->last_fight_time = current_time;
	ch->last_fight_time = current_time;

	WAIT_STATE( ch, skill_table[gsn_vampiric_touch].beats );

	if ( IS_MOB(ch) || number_percent() < 0.85 * get_skill(ch, gsn_vampiric_touch) )
	{
		act( "Tocchi il collo di $N che si addormenta.", ch, NULL, victim, TO_CHAR, POS_REST );
		act( "$n ti tocca il collo e ti addormenti, avvolto da incubi.", ch, NULL, victim, TO_VICT, POS_REST );
		act( "$n tocca il collo di $N che si addormenta.", ch, NULL, victim, TO_NOTVICT, POS_REST );
		check_improve( ch, gsn_vampiric_touch, TRUE, 1 );

		af.type = gsn_vampiric_touch;
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
		damage( ch, victim, 0, gsn_vampiric_touch, DAM_NONE, TRUE );
		check_improve( ch, gsn_vampiric_touch, FALSE, 1 );

		if ( IS_MOB(victim) && victim->position > POS_REST )
		{
			do_yell( victim, "Aiuto! Mi sta attaccando!" );
		}

		af.type = gsn_blackguard;
		af.where = TO_AFFECTS;
		af.level = victim->level;
		af.duration = 2 + victim->level / 50;
		af.modifier = 0;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		affect_join( victim, &af );
	}
}


void do_fly( CHAR_DATA *ch, char *argument )
{
	char	arg[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	argument = one_argument( argument, arg );
	if ( !str_cmp(arg, "up") || !str_cmp(arg, "alto") )
	{
		if ( IS_AFFECTED(ch, AFF_FLYING) )
		{
			send_to_char( "Stai gia' volando.\n\r", ch );
			return;
		}

		if ( is_affected(ch, gsn_fly) || (race_table[ch->race].aff & AFF_FLYING) || affect_check_obj(ch, AFF_FLYING) )
		{
			SET_BIT( ch->affected_by, AFF_FLYING );
			REMOVE_BIT( ch->act, PLR_CHANGED_AFF );
			send_to_char( "Inizi a volare.\n\r", ch );
		}
		else
			send_to_char( "Per volare dovresti trovare una pozione o delle ali.\n\r", ch );
	}
	else if ( !str_cmp(arg, "down") || !str_cmp(arg, "basso") )
	{
		if ( IS_AFFECTED(ch, AFF_FLYING) )
		{
			REMOVE_BIT( ch->affected_by, AFF_FLYING );
			SET_BIT( ch->act, PLR_CHANGED_AFF );
			send_to_char( "Lentamente tocchi il terreno.\n\r", ch );
		}
		else
		{
			send_to_char( "Sei gia' coi piedi per terra!\n\r", ch );
			return;
		}
	}
	else
	{
		send_to_char( "Volare sopra o in basso?\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_fly].beats );
}


void do_push( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL];
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	int			percent;
	int			door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( skill_failure_check(ch, gsn_push, 0, 0, NULL) )
		return;

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Chi vuoi spingere e in quale direzione?\n\r", ch );
		return;
	}

	if ( IS_MOB(ch) && IS_SET(ch->affected_by, AFF_CHARM) && (ch->master != NULL) )
	{
		send_to_char( "Sei troppo intontito per spingere qualcuno.\n\r", ch );
		return;
	}

	if ( (victim = get_char_room(ch, arg1)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) && victim->desc == NULL )
	{
		send_to_char( "Non puoi fare questo.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "E' inutile.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
		return;

	if ( victim->position == POS_FIGHT )
	{
		send_to_char( "Attendi che la lotta finisca.\n\r", ch );
		return;
	}

	if ( (door = find_exit(ch, arg2, TRUE)) >= 0 )
	{
		/* 'push' */
		EXIT_DATA	*pexit;
		/*~~~~~~~~~~~~~~~*/

		if ( (pexit = ch->in_room->exit[door]) != NULL )
		{
			if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
			{
				if ( IS_SET(pexit->exit_info, EX_CLOSED) )
					send_to_char( "Quella direzione e' chiusa.\n\r", ch );
				else if ( IS_SET(pexit->exit_info, EX_LOCKED) )
					send_to_char( "Quella direzione e' chiusa a chiave.\n\r", ch );
				return;
			}
		}

		if ( CAN_DETECT(ch, ADET_WEB) )
		{
			send_to_char( "Le tue caviglie sono imprigionate da delle ragnatele, non riesci a muoverti!\n\r", ch );
			act( "$n tenta di spingere $N anche se e' imprigionato dalle ragnatele.", ch, NULL, victim, TO_ROOM, POS_REST );
			return;
		}

		if ( CAN_DETECT(victim, ADET_WEB) )
		{
			act( "Provi a spingere $N, ma le ragnatele l$X imprigionano troppo forte.", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$n prova a spingere $n, ma le ragnatelel che l$X imprigionano sono troppo forti.", ch, NULL, victim, TO_ROOM, POS_REST );
			return;
		}

		ch->last_death_time = -1;

		WAIT_STATE( ch, skill_table[gsn_push].beats );
		percent = number_percent() + ( IS_AWAKE(victim) ? 10 : -50 );
		percent += can_see( victim, ch ) ? -10 : 0;

		if ( /* ch->level + 5 < victim->level || */ victim->position == POS_FIGHT || (!IS_MOB(ch) && percent > get_skill(ch, gsn_push)) )
		{
			/* Failure. */
			send_to_char( "Oops.\n\r", ch );
			if ( !IS_AFFECTED(victim, AFF_SLEEP) )
			{
				victim->position = victim->position == POS_SLEEP ? POS_STAND : victim->position;
				act( "$n prova a spingerti.", ch, NULL, victim, TO_VICT, POS_REST );
			}

			act( "$n prova a spingere $N.", ch, NULL, victim, TO_NOTVICT, POS_REST );

			if ( IS_MOB(victim) && IS_AWAKE(victim) )
			{
				sprintf( buf, "Togli le tue manacce di dosso, %s!", ch->name );
				do_yell( victim, buf );
			}

			if ( !IS_MOB(ch) )
			{
				if ( IS_MOB(victim) )
				{
					check_improve( ch, gsn_push, FALSE, 2 );
					multi_hit( victim, ch, TYPE_UNDEFINED );
				}
			}

			return;
		}

		sprintf( buf, "$CSpingi $N verso %s.$c", dir_name[door] );
		act( buf, ch, NULL, victim, TO_CHAR, POS_SLEEP, YELLOW );
		sprintf( buf, "$C$n ti spinge verso %s.$c", dir_name[door] );
		act( buf, ch, NULL, victim, TO_VICT, POS_SLEEP, YELLOW );
		sprintf( buf, "$C$n spinge $N verso %s.$c", dir_name[door] );
		act( buf, ch, NULL, victim, TO_NOTVICT, POS_SLEEP, YELLOW );
		move_char( victim, door, FALSE );

		check_improve( ch, gsn_push, TRUE, 1 );
	}
}


void do_crecall( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA		*victim;
	ROOM_INDEX_DATA *location;
	int				point = ROOM_VNUM_ARMY;
	AFFECT_DATA		af;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_cabal_recall, 0, 0, NULL) )
		return;

	if ( is_affected(ch, gsn_cabal_recall) )
	{
		send_to_char( "Non puoi pregare ora.\n\r", ch );
	}

/*	if (ch->desc != NULL && current_time - ch->last_fight_time < FIGHT_DELAY_TIME)
	{
		send_to_char("Sei sotto pressione per una ricerca spirituale.\n\r",ch);
		return;
	}
*/

	if ( ch->desc == NULL && !IS_MOB(ch) )
		point = ROOM_VNUM_ARMY;

	act( "$n prega per il Sangue e la Gloria e viene trasportato via!", ch, NULL, NULL, TO_ROOM, POS_REST );

	if ( (location = get_room_index(point)) == NULL )
	{
		send_to_char( "Ti sei completamente perso.\n\r", ch );
		return;
	}

	if ( ch->in_room == location )
		return;

	if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || IS_AFFECTED(ch, AFF_CURSE) || IS_RAFFECTED(ch->in_room, AFF_ROOM_CURSE) )
	{
		send_to_char( "Gli Dei ti hanno abbandonato.\n\r", ch );
		return;
	}

	if ( (victim = ch->fighting) != NULL )
	{
		send_to_char( "Ma stai combattendo!.\n\r", ch );
		return;
	}

	if ( ch->point[MANA] < (ch->max_point[MANA] * 0.3) )
	{
		send_to_char( "Non hai abbastanza energie interiori per pregare ora.\n\r", ch );
		return;
	}

	ch->point[MOVE] /= 4;
	ch->point[MANA] /= 10;
	act( "$n scompare.", ch, NULL, NULL, TO_ROOM, POS_REST );
	char_from_room( ch );
	char_to_room( ch, location );
	act( "$n appare nella stranza.", ch, NULL, NULL, TO_ROOM, POS_REST );
	do_look( ch, "auto" );

	if ( ch->pet != NULL )
	{
		char_from_room( ch->pet );
		char_to_room( ch->pet, location );
		do_look( ch->pet, "auto" );
	}

	af.type = gsn_cabal_recall;
	af.level = ch->level;
	af.duration = ch->level / 6 + 15;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = 0;

	affect_to_char( ch, &af );
}


void do_escape( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *was_in;
	ROOM_INDEX_DATA *now_in;
	CHAR_DATA		*victim;
	char			arg[MIL];
	int				door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( skill_failure_check(ch, gsn_escape, 0, 0, "Forse e' il momento di darsela a gambe?") )
		return;

	if ( (victim = ch->fighting) == NULL )
	{
		if ( ch->position == POS_FIGHT )
			ch->position = POS_STAND;
		send_to_char( "Non stai combattendo contro nessuno.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Fuggire in quale direzione?\n\r", ch );
		return;
	}

	if ( RIDDEN(ch) )
	{
		send_to_char( "Non puoi scappare se stai cavalcando.\n\r", ch );
		return;
	}

	was_in = ch->in_room;
	if ( (door = find_exit(ch, arg, TRUE)) >= 0 )
	{
		EXIT_DATA	*pexit;
		/*~~~~~~~~~~~~~~~*/

		if ((pexit = was_in->exit[door]) == 0
		||	pexit->to_room == NULL
		||	(IS_SET(pexit->exit_info, EX_CLOSED)
			&&	(!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info, EX_NOPASS))
			&&	!IS_TRUSTED(ch, LEVEL_MASTER))
		||	(IS_SET(pexit->exit_info, EX_NOFLEE))
		||	(IS_MOB(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)) )
		{
			send_to_char( "Qualcosa ti impedisce di fuggire in quella direzione.\n\r", ch );
			return;
		}

		if ( number_percent() > get_skill(ch, gsn_escape) )
		{
			send_to_char( "PANICO! non riesci a fuggire!\n\r", ch );
			check_improve( ch, gsn_escape, FALSE, 1 );
			return;
		}

		check_improve( ch, gsn_escape, TRUE, 1 );
		move_char( ch, door, FALSE );
		if ( (now_in = ch->in_room) == was_in )
		{
			send_to_char( "Non riuscirai a raggiungere quella direzione, prova con un altra.\n\r", ch );
			return;
		}

		ch->in_room = was_in;
		act( "$n e' fuggit$x!", ch, NULL, NULL, TO_ROOM, POS_REST );
		ch->in_room = now_in;

		if ( !IS_MOB(ch) )
		{
			send_to_char( "Fuggi dal combattimento!.\n\r", ch );
			gain_exp( ch, -(ch->level*2) );
		}
		else
			ch->last_fought = NULL; /* Once fled, the mob will not go after */

		stop_fighting( ch, TRUE );
	}
	else
		send_to_char( "Hai scelto la direzione sbagliata.\n\r", ch );
}


void do_layhands( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~~~~*/

    if ( skill_failure_check(ch, gsn_lay_hands, 0, 0, "Non possiedi le capacita' per curare altre persone imponendo le mani.") )
		return;

	if ( (victim = get_char_room(ch, argument)) == NULL )
	{
		send_to_char( "Non vedi questa persona qui.\n\r", ch );
		return;
	}

	if ( is_affected(ch, gsn_lay_hands) )
	{
		send_to_char( "Non riesci a concentrarti abbastanza.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_lay_hands].beats );

	af.type = gsn_lay_hands;
	af.where = TO_AFFECTS;
	af.level = ch->level;
	af.duration = 2;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );

	victim->point[LIFE] = UMIN( victim->point[LIFE] + ch->level * 2, victim->max_point[LIFE] );
	update_pos( victim );
	send_to_char( "Una avvolgente calore ti riempie il corpo curandoti.\n\r", victim );
	if ( ch != victim )
		send_to_char( "Imponi le tue mani e lasci fluire il potere curativo.\n\r", ch );
	check_improve( ch, gsn_lay_hands, TRUE, 1 );
}


int mount_success( CHAR_DATA *ch, CHAR_DATA *mount, int canattack )
{
	int percent;
	int success;
	/*~~~~~~~~*/

	percent = number_percent() + ( ch->level < mount->level ? (mount->level - ch->level) * 3 : (mount->level - ch->level) * 2 );

	if ( !ch->fighting )
		percent -= 25;

	if ( !IS_MOB(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )	/* ex-macro IS_DRUNK */
	{
		percent += get_skill( ch, gsn_riding ) / 2;
		send_to_char( "Ti e' un po' difficile guidare la tua cavalcatura cosi' conciato..\n\r", ch );
	}

	success = percent - get_skill( ch, gsn_riding );
	if ( success <= 0 )
	{	/* Success */
		check_improve( ch, gsn_riding, TRUE, 1 );
		return( 1 );
	}
	else
	{
		check_improve( ch, gsn_riding, FALSE, 1 );
		if ( success >= 10 && MOUNTED(ch) == mount )
		{
			act( "Perdi il controllo e cadi da $N.", ch, NULL, mount, TO_CHAR, POS_REST );
			act( "$n perde il controllo e cade da $N.", ch, NULL, mount, TO_ROOM, POS_REST );
			act( "$n perde il controllo e cade dalla tua groppa.", ch, NULL, mount, TO_VICT, POS_REST );

			ch->riding = FALSE;
			mount->riding = FALSE;
			if ( ch->position > POS_STUN )
				ch->position = POS_SIT;

			ch->point[LIFE] -= number_range( 1, 10 );
			update_pos( ch );
		}

		if ( success >= 40 && canattack )
		{
			act( "$N non e' content$x di come $M stai trattando.", ch, NULL, mount, TO_CHAR, POS_REST );
			act( "$N non e' content$x di come $n $M sta trattando.", ch, NULL, mount, TO_ROOM, POS_REST );
			act( "Non sei content$x di come $n ti sta trattando.", ch, NULL, mount, TO_VICT, POS_REST );

			act( "$N brontola e ti attacca!", ch, NULL, mount, TO_CHAR, POS_REST );
			act( "$N brontola e attacca $n!", ch, NULL, mount, TO_ROOM, POS_REST );
			act( "Brontoli e attacchi $n!", ch, NULL, mount, TO_VICT, POS_REST );

			damage( mount, ch, number_range(1, mount->level), gsn_kick, DAM_BASH, TRUE );
		}
	}

	return( 0 );
}


/*
 * It is not finished yet to implement all.
 */
void do_mount( CHAR_DATA *ch, char *argument )
{
	char				arg[MIL];
	char				buf[MSL];
	struct char_data	*mount;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	argument = one_argument( argument, arg );

	if ( skill_failure_check(ch, gsn_riding, 1, 0, "Non sapresti proprio come cavalcare!" ) )
		return;

	if ( arg[0] == '\0' && ch->mount && ch->mount->in_room == ch->in_room )
	{
		mount = ch->mount;
	}
	else if ( arg[0] == '\0' )
	{
		send_to_char( "Su quale cavalcatura?\n\r", ch );
		return;
	}

	if ( !(mount = get_char_room(ch, arg)) )
	{
		send_to_char( "Non vedi qui nessuna cavalcatura.\n\r", ch );
		return;
	}

	if ( !IS_MOB(mount) || !IS_SET(mount->act, ACT_RIDEABLE) || IS_SET(mount->act, ACT_NOTRACK) )
	{
		send_to_char( "Non puoi cavalcare cio'.\n\r", ch );
		return;
	}

	if ( mount->level - 5 > ch->level )
	{
		send_to_char( "Questa cavalcatura pare troppo per te.", ch );
		return;
	}

	if ( (mount->mount) && (!mount->riding) && (mount->mount != ch) )
	{
		sprintf( buf, "%s appartiene a %s.\n\r", mount->short_descr, mount->mount->name );
		send_to_char( buf, ch );
		return;
	}

	if ( mount->position < POS_STAND )
	{
		send_to_char( "Convinci la tua cavalcatura a tornare in piedi.\n\r", ch );
		return;
	}

	if ( RIDDEN(mount) )
	{
		send_to_char( "Questo animale e' gia' cavalcato da qualcuno.\n\r", ch );
		return;
	}
	else if ( MOUNTED(ch) )
	{
		send_to_char( "Stai gia' cavalcando.\n\r", ch );
		return;
	}

	if ( !mount_success(ch, mount, TRUE) )
	{
		send_to_char( "Fallisci a montare sull'animale.\n\r", ch );
		if ( number_range(1, 20) == 1 )
		{
			send_to_char( "Ahi! E ti fai pure male cadendo!", ch );
			ch->point[LIFE] -= number_range( 1, 10 );
		}

		return;
	}

	act( "Salti sulla schiena di $N.", ch, NULL, mount, TO_CHAR, POS_REST );
	act( "$n salta sulla schiena di $N.", ch, NULL, mount, TO_NOTVICT, POS_REST );
	act( "$n ti salta in groppa!", ch, NULL, mount, TO_VICT, POS_REST );

	ch->mount = mount;
	ch->riding = TRUE;
	mount->mount = ch;
	mount->riding = TRUE;

	/* No sneaky people on mounts */
	affect_strip( ch, gsn_sneak );
	REMOVE_BIT( ch->affected_by, AFF_SNEAK );
	affect_strip( ch, gsn_hide );
	REMOVE_BIT( ch->affected_by, AFF_HIDE );
	affect_strip( ch, gsn_fade );
	REMOVE_BIT( ch->affected_by, AFF_FADE );
	affect_strip( ch, gsn_imp_invis );
	REMOVE_BIT( ch->affected_by, AFF_IMP_INVIS );
}


void do_dismount( CHAR_DATA *ch, char *argument )
{
	struct char_data	*mount;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( MOUNTED(ch) )
	{
		mount = MOUNTED( ch );

		act( "Smonti da $N.", ch, NULL, mount, TO_CHAR, POS_REST );
		act( "$n smonta da $N.", ch, NULL, mount, TO_NOTVICT, POS_REST );
		act( "$n smonta da te.", ch, NULL, mount, TO_VICT, POS_REST );

		ch->riding = FALSE;
		mount->riding = FALSE;
	}
	else
	{
		send_to_char( "Non stai cavalcando nessun animale.\n\r", ch );
		return;
	}
}


int send_arrow( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *arrow, int door, int chance, int bonus )
{
	EXIT_DATA		*pExit;
	ROOM_INDEX_DATA *dest_room;
	char			buf[512];
	AFFECT_DATA		*paf;
	int				damroll = 0, hitroll = 0, sn;
	AFFECT_DATA		af;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( arrow->value[0] == WEAPON_SPEAR )
		sn = gsn_spear;
	else
		sn = gsn_arrow;

	for ( paf = arrow->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->location == APPLY_DAMROLL )
			damroll += paf->modifier;
		if ( paf->location == APPLY_HITROLL )
			hitroll += paf->modifier;
	}

	dest_room = ch->in_room;
	chance += ( hitroll + str_app[get_curr_stat(ch, STR)].tohit + (get_curr_stat(ch, DEX) - 18) ) * 2;
	damroll *= 10;
	for ( i = 0; i < 1000; i++ )
	{
		chance -= 10;
		if ( victim->in_room == dest_room )
		{

			if ( number_percent() < chance )
			{
				if ( check_obj_dodge(ch, victim, arrow, chance) )
					return( 0 );
				act( "$p ti colpisce!", victim, arrow, NULL, TO_CHAR, POS_REST );
				act( "La tua $p colpisce $N!", ch, arrow, victim, TO_CHAR, POS_REST );
				if ( ch->in_room == victim->in_room )
					act( "La $p di $n colpisce $N!", ch, arrow, victim, TO_NOTVICT, POS_REST );
				else
				{
					act( "La $p di $n colpisce $N!", ch, arrow, victim, TO_ROOM, POS_REST );
					act( "$p colpisce $n!", victim, arrow, NULL, TO_ROOM, POS_REST );
				}

				if ( is_safe(ch, victim) || (IS_MOB(victim) && IS_SET(victim->act, ACT_NOTRACK)) )
				{
					act( "$p cade da $n senza arrecare danno visibile..", victim, arrow, NULL, TO_ALL, POS_REST );
					act( "$p cade da $n senza arrecare danno visibile..", ch, arrow, NULL, TO_CHAR, POS_REST );
					obj_to_room( arrow, victim->in_room );
				}
				else
				{
					int dam;
					/*~~~~*/

					dam = dice( arrow->value[1], arrow->value[2] );
					dam = number_range( dam, (3 * dam) );
					dam += damroll + bonus + ( 10 * str_app[get_curr_stat(ch, STR)].todam );

					if ( IS_WEAPON_STAT(arrow, WEAPON_POISON) )
					{
						int			level;
						AFFECT_DATA *poison;
						/*~~~~~~~~~~~~~~~~~~~~*/

						if ( (poison = affect_find(arrow->affected, gsn_poison)) == NULL )
							level = arrow->level;
						else
							level = poison->level;
						if ( !saves_spell(level, victim, DAM_POISON) )
						{
							send_to_char( "Senti il veleno scorrerti nelle vene.", victim );
							act( "$n sembra molto pallid$x e trema.", victim, arrow, NULL, TO_ROOM, POS_REST );

							af.where = TO_AFFECTS;
							af.type = gsn_poison;
							af.level = level * 3 / 4;
							af.duration = level / 2;
							af.location = APPLY_STR;
							af.modifier = -1;
							af.bitvector = AFF_POISON;
							affect_join( victim, &af );
						}
					}

					if ( IS_WEAPON_STAT(arrow, WEAPON_FLAMING) )
					{
						act( "$n e' bruciat$x da una $p.", victim, arrow, NULL, TO_ROOM, POS_REST );
						act( "$p brucia le tue carni.", victim, arrow, NULL, TO_CHAR, POS_REST );
						fire_effect( (void *) victim, arrow->level, dam, TARGET_CHAR );
					}

					if ( IS_WEAPON_STAT(arrow, WEAPON_FROST) )
					{
						act( "$p congela $n.", victim, arrow, NULL, TO_ROOM, POS_REST );
						act( "Il tocco gelido di una $p ti ricopre di ghiaccio.", victim, arrow, NULL, TO_CHAR, POS_REST );
						cold_effect( victim, arrow->level, dam, TARGET_CHAR );
					}

					if ( IS_WEAPON_STAT(arrow, WEAPON_SHOCKING) )
					{
						act( "$n viene fulminat$x da una $p.", victim, arrow, NULL, TO_ROOM, POS_REST );
						act( "Vieni fulminat$x da una $p.", victim, arrow, NULL, TO_CHAR, POS_REST );
						shock_effect( victim, arrow->level, dam, TARGET_CHAR );
					}

					if ( dam > victim->max_point[LIFE] / 10 && number_percent() < 50 )
					{
						af.where = TO_AFFECTS;
						af.type = sn;
						af.level = ch->level;
						af.duration = -1;
						af.location = APPLY_HITROLL;
						af.modifier = -( dam / 20 );
						if ( IS_MOB(victim) )
							af.bitvector = 0;
						else
							af.bitvector = AFF_CORRUPTION;

						affect_join( victim, &af );

						obj_to_char( arrow, victim );
						equip_char( victim, arrow, WEAR_STUCK_IN );
					}
					else
						obj_to_room( arrow, victim->in_room );

					damage( ch, victim, dam, sn, DAM_PIERCE, TRUE );
					path_to_track( ch, victim, door );
				}
				return( 1 );
			}
			else
			{
				obj_to_room( arrow, victim->in_room );
				act( "$p si conficca nel terreno vicino ai tuoi piedi!", victim, arrow, NULL, TO_ALL, POS_REST );
				return( 0 );
			}
		}

		pExit = dest_room->exit[door];
		if ( !pExit )
			break;
		else
		{
			dest_room = pExit->to_room;
			if ( dest_room->people )
			{
				sprintf( buf, "$p arriva nel luogo %s!", dir_name_from[rev_dir[door]] );
				act( buf, dest_room->people, arrow, NULL, TO_ALL, POS_REST );
			}
		}
	}

	return( 0 );
}


void do_shoot( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*wield;
	OBJ_DATA	*arrow;
	char		arg1[512], arg2[512], buf[512];
	bool		success;
	int			chance, direction;
	int			range = ( ch->level / 10 ) + 1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( skill_failure_check(ch, gsn_bow, 0, 0, "Non sai lanciare le frecce.") )
		return;

	argument = one_argument( argument, arg1 );
	one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Lanciare in quale direzione e contro chi?\n\r", ch );
		return;
	}

	if ( ch->fighting )
	{
		send_to_char( "Sei impegnato in una lotta, ti e' impossibile farlo.\n\r", ch );
		return;
	}

	direction = find_exit( ch, arg1, TRUE );

	if ( direction < 0 || direction > MAX_DIR )
	{
		send_to_char( "Verso quale direzione?\n\r", ch );
		return;
	}

	if ( (victim = find_char(ch, arg2, direction, range)) == NULL )
	{
		send_to_char( "Non vedi la tua vittima in quella direzione.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) && victim->desc == NULL )
	{
		send_to_char( "non puoi fare cio'.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "E' insensato.\n\r", ch );
		return;
	}

	if ( is_at_cabal_area(ch) || is_at_cabal_area(victim) )
	{
		send_to_char( "Una forza magica ti impedisce di alzare il braccio per tendere l'arco.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
	{
		sprintf( buf, "Gli Dei proteggono %s.\n\r", victim->name );
		send_to_char( buf, ch );
		return;
	}

	if ( (wield = get_weapon_char(ch, WEAPON_BOW)) == NULL )
	{
		send_to_char( "Probabilmente hai bisogno di un arco..\n\r", ch );
		return;
	}

	if ( (arrow = get_weapon_char(ch, WEAPON_ARROW)) == NULL )
	{
		send_to_char( "Devi impugnare una freccia da usare come proiettile!\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_bow].beats );

	chance = ( get_skill(ch, gsn_bow) - 50 ) * 2;
	if ( ch->position == POS_SLEEP )
		chance += 40;
	if ( ch->position == POS_REST )
		chance += 10;
	if ( victim->position == POS_FIGHT )
		chance -= 40;
	chance += GET_HITROLL( ch );

	sprintf( buf, "Lanci $p %s.", dir_name_to[direction] );
	act( buf, ch, arrow, NULL, TO_CHAR, POS_REST );
	sprintf( buf, "$n lancia $p %s.", dir_name_to[direction] );
	act( buf, ch, arrow, NULL, TO_ROOM, POS_REST );

	obj_from_char( arrow );
	success = send_arrow( ch, victim, arrow, direction, chance, dice(wield->value[1], wield->value[2]) );
	check_improve( ch, gsn_bow, TRUE, 1 );
}


char *find_way( CHAR_DATA *ch, ROOM_INDEX_DATA *rstart, ROOM_INDEX_DATA *rend )
{
	int			direction;
	static char buf[1024];
	EXIT_DATA	*pExit;
	char		buf2[2];
	int			i;
	/*~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Trova: " );
	for ( i = 0; i < 65535; i++ )
	{
		if ( (rend == rstart) )
			return( buf );
		if ( (direction = find_path(rstart->vnum, rend->vnum, ch, -40000, 0)) == -1 )
		{
			strcat( buf, " BUGGY" );
			return( buf );
		}

		if ( direction < 0 || direction > MAX_DIR )
		{
			strcat( buf, " VERY BUGGY" );
			return( buf );
		}

		if ( IS_SET(ch->act, PLR_ENGLISH) )
			buf2[0] = dir_name_en[direction][0];
		else
			buf2[0] = dir_name[direction][0];

		buf2[1] = '\0';
		strcat( buf, buf2 );

		/* find target room */
		pExit = rstart->exit[direction];
		if ( !pExit )
		{
			strcat( buf, " VERY VERY BUGGY" );
			return( buf );
		}
		else
			rstart = pExit->to_room;
	}

	return( buf );
}


void do_human( CHAR_DATA *ch, char *argument )
{
	if ( ch->class != CLASS_VAMPIRE )
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if ( !IS_VAMPIRE(ch) )
	{
		act( "Sei gia' uman$x.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return;
	}

	affect_strip( ch, gsn_vampire );
	REMOVE_BIT( ch->act, PLR_VAMPIRE );
	send_to_char( "Ritorni nella tua forma orginale.\n\r", ch );
}


void do_throw_spear( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*victim;
	OBJ_DATA	*spear;
	char		arg1[512], arg2[512], buf[512];
	bool		success;
	int			chance, direction;
	int			range = ( ch->level / 10 ) + 1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( skill_failure_check(ch, gsn_spear, 1, 0, "Non sapresti proprio come tirare una lancia." ) )
		return;

	argument = one_argument( argument, arg1 );
	one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Tirare una lancia un quale direzione e su chi?\n\r", ch );
		return;
	}

	if ( ch->fighting )
	{
		send_to_char( "Non puoi se sei impegnato in un combattimento.\n\r", ch );
		return;
	}

	direction = find_exit( ch, arg1, TRUE );

	if ( direction < 0 || direction > MAX_DIR )
	{
		send_to_char( "Non mi pare una direzione?\n\r", ch );
		return;
	}

	if ( (victim = find_char(ch, arg2, direction, range)) == NULL )
	{
		send_to_char( "Non trovi nessuno contro cui lanciarla.\n\r", ch );
		return;
	}

	if ( !IS_MOB(victim) && victim->desc == NULL )
	{
		send_to_char( "Non puoi fare cio'.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "E' insensato.\n\r", ch );
		return;
	}

	if ( is_at_cabal_area(ch) || is_at_cabal_area(victim) )
	{
		send_to_char( "Qualche strano potere ti paralizza il braccio.\n\r", ch );
		return;
	}

	if ( is_safe(ch, victim) )
	{
		sprintf( buf, "Gli Dei proteggono %s.\n\r", victim->name );
		send_to_char( buf, ch );
		return;
	}

	if ( (spear = get_weapon_char(ch, WEAPON_SPEAR)) == NULL )
	{
		send_to_char( "Hai bisogno di una lancia per caso?\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_spear].beats );

	chance = ( get_skill(ch, gsn_spear) - 50 ) * 2;
	if ( ch->position == POS_SLEEP )
		chance += 40;
	if ( ch->position == POS_REST )
		chance += 10;
	if ( victim->position == POS_FIGHT )
		chance -= 40;
	chance += GET_HITROLL( ch );

	sprintf( buf, "Tiri $p %s.", dir_name_to[direction] );
	act( buf, ch, spear, NULL, TO_CHAR, POS_REST );
	sprintf( buf, "$n tira $p %s.", dir_name_to[direction] );
	act( buf, ch, spear, NULL, TO_ROOM, POS_REST );

	obj_from_char( spear );
	success = send_arrow( ch, victim, spear, direction, chance, dice(spear->value[1], spear->value[2]) );
	check_improve( ch, gsn_spear, TRUE, 1 );
}
