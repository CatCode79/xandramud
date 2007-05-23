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
#include "recycle.h"


/*
 * Funzioni locali.
 */
int		lookup_religion_leader	( const char *name );


DECLARE_MPROG_FUN_GREET( greet_prog_ruler_pre );
DECLARE_MPROG_FUN_GREET( greet_prog_ruler );
DECLARE_MPROG_FUN_GREET( greet_prog_yvaladrin );
DECLARE_MPROG_FUN_GREET( greet_prog_army );
DECLARE_MPROG_FUN_GREET( greet_prog_chaos );
DECLARE_MPROG_FUN_GREET( greet_prog_knight );
DECLARE_MPROG_FUN_GREET( greet_prog_invader );
DECLARE_MPROG_FUN_GREET( greet_prog_lions );
DECLARE_MPROG_FUN_GREET( greet_prog_hunter );
DECLARE_MPROG_FUN_SPEECH( speech_prog_hunter_cleric );

DECLARE_MPROG_FUN_BRIBE( bribe_prog_cityguard );

DECLARE_MPROG_FUN_SPEECH( speech_prog_keeper );
DECLARE_MPROG_FUN_GREET( greet_prog_fireflash );
DECLARE_MPROG_FUN_GREET( greet_prog_solamnia );
DECLARE_MPROG_FUN_GIVE( give_prog_solamnia );
DECLARE_MPROG_FUN_GIVE( give_prog_fireflash );
DECLARE_MPROG_FUN_GIVE( give_prog_maggiordomo );
DECLARE_MPROG_FUN_GIVE( give_prog_keeper );
DECLARE_MPROG_FUN_GIVE( give_prog_dressmaker );
DECLARE_MPROG_FUN_GREET( greet_prog_keeper );

DECLARE_MPROG_FUN_DEATH( death_prog_stalker );

DECLARE_MPROG_FUN_GREET( greet_prog_templeman );
DECLARE_MPROG_FUN_SPEECH( speech_prog_templeman );
DECLARE_MPROG_FUN_FIGHT( fight_prog_diana );
DECLARE_MPROG_FUN_FIGHT( fight_prog_ofcol_guard );
DECLARE_MPROG_FUN_SPEECH( speech_prog_wiseman );

DECLARE_MPROG_FUN_GREET( greet_prog_armourer );
DECLARE_MPROG_FUN_GREET( greet_prog_baker );
DECLARE_MPROG_FUN_GREET( greet_prog_beggar );
DECLARE_MPROG_FUN_GREET( greet_prog_drunk );
DECLARE_MPROG_FUN_GREET( greet_prog_grocer );

DECLARE_MPROG_FUN_BRIBE( bribe_prog_beggar );
DECLARE_MPROG_FUN_BRIBE( bribe_prog_drunk );

DECLARE_MPROG_FUN_FIGHT( fight_prog_beggar );

DECLARE_MPROG_FUN_DEATH( death_prog_beggar );
DECLARE_MPROG_FUN_DEATH( death_prog_vagabond );

DECLARE_MPROG_FUN_SPEECH( speech_prog_crier );

DECLARE_MPROG_FUN_AREA( area_prog_drunk );
DECLARE_MPROG_FUN_AREA( area_prog_janitor );
DECLARE_MPROG_FUN_AREA( area_prog_vagabond );
DECLARE_MPROG_FUN_AREA( area_prog_baker );
DECLARE_MPROG_FUN_AREA( area_prog_grocer );
DECLARE_MPROG_FUN_FIGHT( fight_prog_golem );
DECLARE_MPROG_FUN_AREA( area_prog_arpist );


#define GIVE_HELP_RELIGION	16
#define RELIG_CHOSEN		17


void mprog_set( MOB_INDEX_DATA *mobindex, const char *progtype, const char *name )
{
	if ( !str_cmp(progtype, "bribe_prog") )
	{
		/*
		if (!str_cmp(name, "bribe_prog")) mobindex->mprogs->bribe_prog = bribe_prog_;
		else if (!str_cmp(name,"bribe_prog")) ...
		 */
		if		( !str_cmp(name, "bribe_prog_cityguard") )	mobindex->mprogs->bribe_prog = bribe_prog_cityguard;
		else if ( !str_cmp(name, "bribe_prog_beggar") )		mobindex->mprogs->bribe_prog = bribe_prog_beggar;
		else if ( !str_cmp(name, "bribe_prog_drunk") )		mobindex->mprogs->bribe_prog = bribe_prog_drunk;
		else
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_BRIBE );
		return;
	}

	if ( !str_cmp(progtype, "entry_prog") )
	{
		/* if (!str_cmp(name, "entry_prog_")) mobindex->mprogs->entry_prog = entry_prog_; else */
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_ENTRY );
		return;
	}

	if ( !str_cmp(progtype, "greet_prog") )
	{
		if		( !str_cmp(name, "greet_prog_yvaladrin") )	mobindex->mprogs->greet_prog = greet_prog_yvaladrin;
		else if ( !str_cmp(name, "greet_prog_knight") )		mobindex->mprogs->greet_prog = greet_prog_knight;
		else if ( !str_cmp(name, "greet_prog_lions") )		mobindex->mprogs->greet_prog = greet_prog_lions;
		else if ( !str_cmp(name, "greet_prog_hunter") )		mobindex->mprogs->greet_prog = greet_prog_hunter;
		else if ( !str_cmp(name, "greet_prog_invader") )	mobindex->mprogs->greet_prog = greet_prog_invader;
		else if ( !str_cmp(name, "greet_prog_ruler") )		mobindex->mprogs->greet_prog = greet_prog_ruler;
		else if ( !str_cmp(name, "greet_prog_ruler_pre") )	mobindex->mprogs->greet_prog = greet_prog_ruler_pre;
		else if ( !str_cmp(name, "greet_prog_army") )		mobindex->mprogs->greet_prog = greet_prog_army;
		else if ( !str_cmp(name, "greet_prog_chaos") )		mobindex->mprogs->greet_prog = greet_prog_chaos;
		else if ( !str_cmp(name, "greet_prog_fireflash") )	mobindex->mprogs->greet_prog = greet_prog_fireflash;
		else if ( !str_cmp(name, "greet_prog_solamnia") )	mobindex->mprogs->greet_prog = greet_prog_solamnia;
		else if ( !str_cmp(name, "greet_prog_keeper") )		mobindex->mprogs->greet_prog = greet_prog_keeper;
		else if ( !str_cmp(name, "greet_prog_templeman") )	mobindex->mprogs->greet_prog = greet_prog_templeman;
		else if ( !str_cmp(name, "greet_prog_armourer") )	mobindex->mprogs->greet_prog = greet_prog_armourer;
		else if ( !str_cmp(name, "greet_prog_baker") )		mobindex->mprogs->greet_prog = greet_prog_baker;
		else if ( !str_cmp(name, "greet_prog_beggar") )		mobindex->mprogs->greet_prog = greet_prog_beggar;
		else if ( !str_cmp(name, "greet_prog_drunk") )		mobindex->mprogs->greet_prog = greet_prog_drunk;
		else if ( !str_cmp(name, "greet_prog_grocer") )		mobindex->mprogs->greet_prog = greet_prog_grocer;
		else
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_GREET );
		return;
	}

	if ( !str_cmp(progtype, "fight_prog") )
	{
		if		( !str_cmp(name, "fight_prog_diana") )			mobindex->mprogs->fight_prog = fight_prog_diana;
		else if ( !str_cmp(name, "fight_prog_ofcol_guard") )	mobindex->mprogs->fight_prog = fight_prog_ofcol_guard;
		else if ( !str_cmp(name, "fight_prog_beggar") )			mobindex->mprogs->fight_prog = fight_prog_beggar;
		else if ( !str_cmp(name, "fight_prog_golem") )			mobindex->mprogs->fight_prog = fight_prog_golem;
		else
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_FIGHT );
		return;
	}

	if ( !str_cmp(progtype, "death_prog") ) /* returning TRUE prevents death */
	{
		if		( !str_cmp(name, "death_prog_stalker") )	mobindex->mprogs->death_prog = death_prog_stalker;
		else if ( !str_cmp(name, "death_prog_beggar") )		mobindex->mprogs->death_prog = death_prog_beggar;
		else if ( !str_cmp(name, "death_prog_vagabond") )	mobindex->mprogs->death_prog = death_prog_vagabond;
		else
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_DEATH );
		return;
	}

	if ( !str_cmp(progtype, "area_prog") )
	{
		if		( !str_cmp(name, "area_prog_drunk") )		mobindex->mprogs->area_prog = area_prog_drunk;
		else if ( !str_cmp(name, "area_prog_janitor") )		mobindex->mprogs->area_prog = area_prog_janitor;
		else if ( !str_cmp(name, "area_prog_vagabond") )	mobindex->mprogs->area_prog = area_prog_vagabond;
		else if ( !str_cmp(name, "area_prog_baker") )		mobindex->mprogs->area_prog = area_prog_baker;
		else if ( !str_cmp(name, "area_prog_grocer") )		mobindex->mprogs->area_prog = area_prog_grocer;
		else if (!str_cmp(name, "area_prog_arpist"))		mobindex->mprogs->area_prog = area_prog_arpist;
		else
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_AREA );
		return;
	}

	if ( !str_cmp(progtype, "speech_prog") )
	{
		if		( !str_cmp(name, "speech_prog_keeper") )		mobindex->mprogs->speech_prog = speech_prog_keeper;
		else if ( !str_cmp(name, "speech_prog_templeman") )		mobindex->mprogs->speech_prog = speech_prog_templeman;
		else if ( !str_cmp(name, "speech_prog_wiseman") )		mobindex->mprogs->speech_prog = speech_prog_wiseman;
		else if ( !str_cmp(name, "speech_prog_crier") )			mobindex->mprogs->speech_prog = speech_prog_crier;
		else if ( !str_cmp(name, "speech_prog_hunter_cleric") )	mobindex->mprogs->speech_prog = speech_prog_hunter_cleric;
		else
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_SPEECH );
		return;
	}

	if ( !str_cmp(progtype, "give_prog") )
	{
		if		( !str_cmp(name, "give_prog_fireflash") )	mobindex->mprogs->give_prog = give_prog_fireflash;
		else if ( !str_cmp(name, "give_prog_maggiordomo") )	mobindex->mprogs->give_prog = give_prog_maggiordomo;
		else if ( !str_cmp(name, "give_prog_solamnia") )	mobindex->mprogs->give_prog = give_prog_solamnia;
		else if ( !str_cmp(name, "give_prog_keeper") )		mobindex->mprogs->give_prog = give_prog_keeper;
		else if ( !str_cmp(name, "give_prog_dressmaker") )	mobindex->mprogs->give_prog = give_prog_dressmaker;
		else
		{
			bug( "Load_mprogs: 'M': Function not found for vnum %d", mobindex->vnum );
			exit( 1 );
		}

		SET_BIT( mobindex->progtypes, MPROG_GIVE );
		return;
	}

	bug( "Load_mprogs: 'M': invalid program type for vnum %d", mobindex->vnum );
	exit( 1 );
}


void bribe_prog_cityguard( CHAR_DATA *mob, CHAR_DATA *ch, int amount )
{
	if ( amount < 100 )
	{
		do_say( mob, "Cos'e' questa miseria?!" );
		do_murder( mob, ch->name );
		return;
	}
	else if ( amount >= 5000 )
	{
		interpret( mob, "smile", FALSE );
		do_sleep( mob, "" );
		return;
	}
	else
	{
		do_say( mob, "Cerchi di corrompermi? Ti costera' piu' di qualche moneta!" );
		return;
	}
}


void greet_prog_yvaladrin( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_YVALADRIN;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_YVALADRIN )
	{
		do_say( mob, "Questa e' la dimora del Sapere." );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spirito tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void greet_prog_invader( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_INVADER;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_INVADER )
	{
		do_say( mob, "Questa e' la dimora delle Ombre." );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spirito tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void greet_prog_ruler_pre( CHAR_DATA *mob, CHAR_DATA *ch )
{
	char	buf[100];
	/*~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( ch->cabal == CABAL_RULER )
	{
		sprintf( buf, "bow %s", ch->name );
		interpret( mob, buf, FALSE );
		return;
	}

	do_say( mob, "Dovreste allontanarvi." );
	do_say( mob, "Gli estranei non hanno il permesso di entrare." );
}


void greet_prog_ruler( CHAR_DATA *mob, CHAR_DATA *ch )
{
	char	buf[100];
	/*~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_RULER;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_RULER )
	{
		sprintf( buf, "bow %s", ch->name );
		interpret( mob, buf, FALSE );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spiriti tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void greet_prog_chaos( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_CHAOS;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_CHAOS )
	{
		do_say( mob, "Questa e' la dimora del Caos." );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spiriti tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void greet_prog_army( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_ARMY;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_ARMY )
	{
		do_say( mob, "Questa e' la dimora della Guerra." );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spiriti tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void give_prog_keeper( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
	OBJ_DATA	*rug;
	char		buf[100];
	/*~~~~~~~~~~~~~~~~~*/

	if ( obj->pIndexData->vnum == 90 )
	{
		do_say( mob, "Oh finalmente il mio abito!" );
		act( "$n mette da parte l'abito.", mob, NULL, NULL, TO_ROOM, POS_REST );
		obj_from_char( obj );
		extract_obj( obj );
		if ( (rug = get_obj_carry(ch, "rug")) != NULL )
		{
			do_say( mob, "Suppongo che vogliate vedere Fireflash ora." );
			do_say( mob, "Fate attenzione, non sembra essere di buon umore." );
			do_unlock( mob, "door" );	/* (RR) qui e sotto, traduzione e attenzione all'area */
			do_open( mob, "door" );
		}
		else
		{
			do_say( mob, "Desiderate qualcosa?" );
			do_say( mob, "Come? Ah no.. non avete nulla con voi." );
		}
	}
	else
	{
		sprintf( buf, "%s %s", obj->name, ch->name );
		do_give( mob, buf );
		do_say( mob, "E che cosa sarebbe?" );
	}
}


void speech_prog_keeper( CHAR_DATA *mob, CHAR_DATA *ch, char *speech )
{
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~*/

	if ( !str_cmp(speech, "custode") && !IS_MOB(ch) )
	{
		obj = create_object( get_obj_index(90), 0 );
		free_string( obj->name );
		obj->name = str_dup( "abito custode" );
		act( "$n ricava uno splendido abito dalla seta.", mob, NULL, NULL, TO_ROOM, POS_REST );
		act( "Appronti l'abito.", mob, NULL, NULL, TO_CHAR, POS_REST );
		do_say( mob, "Ecco l'abito!" );
		obj_to_char( obj, ch );
	}
}


void greet_prog_fireflash( CHAR_DATA *mob, CHAR_DATA *ch )
{
	char		buf[100];
	OBJ_DATA	*obj;
	/*~~~~~~~~~~~~~~~~~*/

	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;

	if ( (obj = get_obj_carry(ch, "rug")) == NULL )
	{
		do_say( mob, "Argh! Ancora quel tappeto! Non lo voglio piu' vedere in vita mia!" );
		do_say( mob, "Ma perche' non lo portate a quella Suora che.." );
		do_unlock( mob, "box" );
		do_open( mob, "box" );
		do_get( mob, "papers box" );
		do_say( mob, "Ecco, tenete queste, vi serviranno!" );
		act( "$n sogghigna verso di te.", mob, NULL, ch, TO_VICT, POS_REST );
		act( "Sogghigni verso $N.", mob, NULL, ch, TO_CHAR, POS_REST );
		act( "$n sogghigna verso $N.", mob, NULL, ch, TO_NOTVICT, POS_REST );
		sprintf( buf, "papers %s", ch->name );
		do_give( mob, buf );
		do_close( mob, "box" );
		do_lock( mob, "box" );
	}
}


void give_prog_fireflash( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
	char	buf[100];
	/*~~~~~~~~~~~~~*/

	if ( !can_see(mob, ch) )
		do_say( mob, "Ehi, chi c'e'?" );
	else if ( IS_MOB(ch) )
		do_say( mob, "Oh grazie mille." );
	else if ( obj->pIndexData->vnum != 91 )
	{
		do_say( mob, "Interessante.. ma che me ne faccio?" );
		interpret( mob, "giggle", FALSE );
		sprintf( buf, "%s %s", obj->name, ch->name );
		do_give( mob, buf );
	}
	else
	{
		do_say( mob, "Che splendido tappeto! Dove lo posso mettere.." );
		act( "$n comincia a girare intorno alla stanza con aria meditabonda.", mob, NULL, NULL, TO_ROOM, POS_REST );
		act( "$n si mette le mani nelle tasche.", mob, NULL, NULL, TO_ROOM, POS_REST );
		do_load( mob, "obj 2438" );
		do_say( mob, "Oh.. una chiave?! Vi serve? Ecco, e' vostra." );
		sprintf( buf, "xxx %s", ch->name );
		do_give( mob, buf );
		act( "$n mette il tappeto sotto la sedia.", mob, NULL, NULL, TO_ROOM, POS_REST );
		obj_from_char( obj );
		extract_obj( obj );
	}
}


void give_prog_maggiordomo( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
	char		buf[100];
	OBJ_DATA	*key;
	/*~~~~~~~~~~~~~~~~~~~*/


	if ( obj->pIndexData->vnum == 9480 )
	{
		do_say( mob, "Ecco la key della vostra stanza." );
		key = create_object( get_obj_index(9420), 0 );
	}

	if ( obj->pIndexData->vnum == 9481 )
	{
		do_say( mob, "Ecco la key della vostra stanza." );
		key = create_object( get_obj_index(9422), 0 );
	}

	if ( obj->pIndexData->vnum == 9482 )
	{
		do_say( mob, "Ecco la key della vostra stanza." );
		key = create_object( get_obj_index(9421), 0 );
	}

	if ( obj->pIndexData->vnum == 9483 )
	{
		do_say( mob, "Ecco la key della vostra stanza." );
		key = create_object( get_obj_index(9423), 0 );

	}

	key->timer = 500;
	obj_to_char( key, ch );
	sprintf( buf, "chiave" );
	do_give( mob, buf );
	do_say( mob, "Buona permanenza." );
	obj_from_char( obj );
	extract_obj( obj );
}





void greet_prog_solamnia( CHAR_DATA *mob, CHAR_DATA *ch )
{
	OBJ_DATA	*obj;
	char		arg[100];
	/*~~~~~~~~~~~~~~~~~*/

	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;

	sprintf( arg, "xxx" );

	if ( (obj = get_obj_carry(ch, arg)) != NULL )
	{
		do_say( mob, "Oh.. vedo che avete qualcosa per me.." );
		interpret( mob, "smile", FALSE );
	}
}


void give_prog_solamnia( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
	char		buf[100];
	OBJ_DATA	*kassandra;
	/*~~~~~~~~~~~~~~~~~~~*/

	if ( obj->pIndexData->vnum == 2438 )
	{
		do_say( mob, "Ecco, questo e' per voi!" );
		kassandra = create_object( get_obj_index(89), 0 );
		kassandra->timer = 500;
		obj_to_char( kassandra, mob );
		sprintf( buf, "kassandra %s", ch->name );
		do_give( mob, buf );
		do_say( mob, "Non e' una semplice pietra.. lo capirete." );
		obj_from_char( obj );
		extract_obj( obj );
	}
}


bool death_prog_stalker( CHAR_DATA *mob )
{
	char	buf[100];
	/*~~~~~~~~~~~~~*/

	mob->cabal = CABAL_RULER;
	sprintf( buf, "Ho fallito nell'uccidere %s, mi ritiro lontano da qui..", mob->last_fought->name );
	do_yell( mob, buf );
	return( FALSE );
}


void greet_prog_knight( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_KNIGHT;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_KNIGHT )
	{
		do_say( mob, "L'Onore e' dei giusti!" );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spiriti tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void give_prog_dressmaker( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
	if ( IS_MOB(ch) )
		return;

	if ( !can_see(mob, ch) )
	{
		do_say( mob, "E da dove vien fuori questo?" );
		return;
	}

	if ( obj->pIndexData->vnum != 2436 )
	{
		do_say( mob, "Ma no.. mi serve della seta!" );
		do_drop( mob, obj->name );
		return;
	}
	else
	{
		do_say( mob, "Per chi devo fare l'abito?" );
		obj_from_char( obj );
		extract_obj( obj );
	}
}


void greet_prog_keeper( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	if ( !can_see(mob, ch) )
		return;

	do_say( mob, "Che ci fate qui? E' l'abito che ho ordinato?" );
}


void speech_prog_templeman( CHAR_DATA *mob, CHAR_DATA *ch, char *speech )
{
	char	buf[160];
	int		chosen = 0, correct = 1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !str_cmp(speech, "religione") )
		mob->status = GIVE_HELP_RELIGION;
	else if ( (chosen = lookup_religion_leader(speech)) != 0 )
		mob->status = RELIG_CHOSEN;
	else
		return;

	if ( mob->status == RELIG_CHOSEN )
	{
		if ( (ch->religion > 0) && (ch->religion < MAX_RELIGION) )
		{
			sprintf( buf, "Sei gia' sul sentiero di %s", religion_table[ch->religion].leader );
			do_say( mob, buf );
			return;
		}

		switch ( chosen )
		{
		case RELIGION_APOLLON:		if ( !IS_GOOD(ch) && ch->ethos != 1 )	 correct = 0;	break;
		case RELIGION_ZEUS:			if ( !IS_GOOD(ch) && ch->ethos != 2 )	 correct = 0;	break;
		case RELIGION_SIEBELE:		if ( !IS_NEUTRAL(ch) && ch->ethos != 2 ) correct = 0;	break;
		case RELIGION_EHRUMEN:		if ( !IS_GOOD(ch) && ch->ethos != 3 )	 correct = 0;	break;
		case RELIGION_AHRUMAZDA:	if ( !IS_EVIL(ch) && ch->ethos != 3 )	 correct = 0;	break;
		case RELIGION_DEIMOS:		if ( !IS_EVIL(ch) && ch->ethos != 1 )	 correct = 0;	break;
		case RELIGION_PHOBOS:		if ( !IS_EVIL(ch) && ch->ethos != 2 )	 correct = 0;	break;
		case RELIGION_ODIN:			if ( !IS_NEUTRAL(ch) && ch->ethos != 1 ) correct = 0;	break;
		case RELIGION_MARS:			if ( !IS_NEUTRAL(ch) && ch->ethos != 3 ) correct = 0;	break;
		}

		if ( !correct )
		{
			do_say( mob, "Il tuo animo non riuscira' mai a seguire questa divinita'." );
			return;
		}

		ch->religion = chosen;
		sprintf( buf, "Ora e sempre la guida del tuo animo sara' %s", religion_table[ch->religion].leader );
		do_say( mob, buf );
		return;
	}

	do_say( mob, "Oh Religione.." );
	do_say( mob, "Vuoi parlarne con me? Bene." );
	do_say( mob, "Ricorda che puoi cambiare religione in accordo con la tua disposizione d'animo " );
	do_say( mob, "ma dovrai prima completare qualche missione per gli Dei tramite Questor." );
}


void greet_prog_templeman( CHAR_DATA *mob, CHAR_DATA *ch )
{
	char	arg[100];
	/*~~~~~~~~~~~~~*/

	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;

	sprintf( arg, "smile %s", ch->name );
	interpret( mob, arg, FALSE );
}


int lookup_religion_leader( const char *name )
{
	int value;
	/*~~~~~~*/

	for ( value = 0; value < MAX_RELIGION; value++ )
	{
		if ( LOWER(name[0]) == LOWER(religion_table[value].leader[0]) && !str_prefix(name, religion_table[value].leader) )
			return( value );
	}

	return( 0 );
}


void greet_prog_lions( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_LIONS;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_LIONS )
	{
		do_say( mob, "La natura e' la nostra casa." );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spiriti tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void greet_prog_hunter_old( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_HUNTER;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_HUNTER )
	{
		do_say( mob, "Le foreste sono il nostro respiro." );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spiriti tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void greet_prog_hunter( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
		return;

	mob->cabal = CABAL_HUNTER;
	SET_BIT( mob->off_flags, OFF_AREA_ATTACK );

	if ( ch->cabal == CABAL_HUNTER )
	{
		char		buf[MSL];
		OBJ_DATA	*eyed;
		int			i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		do_say( mob, "Le foreste sono il nostro respiro." );
		if ( IS_SET(ch->quest, QUEST_EYE) )
			return;

		SET_BIT( ch->quest, QUEST_EYE );

		if ( IS_GOOD(ch) )
			i = 0;
		else if ( IS_EVIL(ch) )
			i = 2;
		else
			i = 1;

		eyed = create_object( get_obj_index(OBJ_VNUM_EYED_SWORD), 0 );
		eyed->owner = str_dup( ch->name );
		eyed->from = str_dup( ch->name );
		eyed->altar = hometown_table[ch->hometown].altar[i];
		eyed->pit = hometown_table[ch->hometown].pit[i];
		eyed->level = ch->level;

		sprintf( buf, eyed->short_descr, ch->name );
		free_string( eyed->short_descr );
		eyed->short_descr = str_dup( buf );

		sprintf( buf, eyed->pIndexData->extra_descr->description, ch->name );
		eyed->extra_descr = new_extra_descr();
		eyed->extra_descr->keyword = str_dup( eyed->pIndexData->extra_descr->keyword );
		eyed->extra_descr->description = str_dup( buf );
		eyed->extra_descr->next = NULL;

		eyed->value[2] = ( ch->level / 10 ) + 3;
		eyed->level = ch->level;
		eyed->cost = 0;
		obj_to_char( eyed, mob );
		interpret( mob, "emote crea una splendida spada.", FALSE );
		do_say( mob, "Ecco la tua spada." );
		sprintf( buf, "give eyed %s", ch->name );
		interpret( mob, buf, FALSE );
		do_say( mob, "Ricorda che se la perdi potrai richiederla al curatore della gilda!" );
		do_say( mob, "Dovrai pronunciare in sua presenza la parola 'flaerin'" );
		return;
	}

	if ( ch->last_death_time != -1 && current_time - ch->last_death_time < 600 )
	{
		do_say( mob, "Gli spiriti tornino nei loro mondi!" );
		do_slay( mob, ch->name );
		return;
	}

	if ( IS_ADMIN(ch) )
		return;

	do_yell( mob, "Intrusi! Intrusi!" );
	do_say( mob, "Come osi violare questo luogo?" );
}


void fight_prog_diana( CHAR_DATA *mob, CHAR_DATA *ch )
{
	CHAR_DATA	*ach, *ach_next;
	int			door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !mob->in_room || number_percent() < 25 )
		return;

	if ( mob->in_room->area != mob->zone )
		return;

	do_yell( mob, "Guardie!." );
	for ( ach = char_list; ach != NULL; ach = ach_next )
	{
		ach_next = ach->next;
		if ( !ach->in_room || ach->in_room->area != ch->in_room->area || !IS_MOB(ach) )
			continue;
		if ( ach->pIndexData->vnum == 600 || ach->pIndexData->vnum == 603 )
		{
			if ( ach->fighting || ach->last_fought )
				continue;
			if ( mob->in_room == ach->in_room )
			{
				int i;
				/*~~*/

				act( "$C$n richiama l'aiuto degli Dei.$c",
					ach, NULL, NULL, TO_ROOM, POS_SLEEP, BLUE );
				act( "$CGli dei favoriscono $n per aiutare Diana.$c",
					ach, NULL, NULL, TO_ROOM, POS_SLEEP, GREEN );
				ach->max_point[LIFE] = 6000;
				ach->point[LIFE] = 6000;
				ach->level = 60;
				ach->timer = 0;
				ach->damage[DICE_NUMBER] = number_range( 3, 5 );
				ach->damage[DICE_TYPE] = number_range( 12, 22 );
				ach->damage[DICE_BONUS] = number_range( 6, 8 );
				for ( i = 0; i < MAX_STATS; i++ )
					ach->perm_stat[i] = 23;
				do_say( ach, "Arrivo Diana!" );
				do_murder( ach, ch->name );
				continue;
			}

			door = find_path( ach->in_room->vnum, mob->in_room->vnum, ach, -40, 1 );
			if ( door == -1 )
				bug( "Couldn't find a path with -40", 0 );
			else
			{
				if ( number_percent() < 20 )
					do_yell( ach, " Resisti Diana!" );
				else
					do_say( ach, "Devo correre da Diana!" );
				move_char( ach, door, FALSE );
			}
		}
	}
}


void fight_prog_ofcol_guard( CHAR_DATA *mob, CHAR_DATA *ch )
{
	char		buf[MSL];
	CHAR_DATA	*ach, *ach_next;
	int			door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( number_percent() < 20 )
		return;
	sprintf( buf, "Aiuto Guardie! %s mi attacca!", ch->name );
	do_yell( mob, buf );
	for ( ach = char_list; ach != NULL; ach = ach_next )
	{
		ach_next = ach->next;
		if ( ach->in_room->area != ch->in_room->area || !IS_MOB(ach) )
			continue;
		if ( ach->pIndexData->vnum == 600 )
		{
			if ( ach->fighting )
				continue;
			if ( mob->in_room == ach->in_room )
			{
				sprintf( buf, "%s ,ora pagherai la tua sfrontatezza!", ch->name );
				do_say( ach, buf );
				do_murder( ach, ch->name );
				continue;
			}

			door = find_path( ach->in_room->vnum, mob->in_room->vnum, ach, -40, 1 );
			if ( door == -1 )
				bug( "Couldn't find a path with -40", 0 );
			else
			{
				if ( number_percent() < 20 )
					do_yell( ach, " Resisti! Arrivo!" );
				else
					do_say( ach, "Devo correre!" );
				move_char( ach, door, FALSE );
			}
		}
	}
}


void speech_prog_wiseman( CHAR_DATA *mob, CHAR_DATA *ch, char *speech )
{
	char	arg[512];
	/*~~~~~~~~~~~~~*/

	one_argument( speech, arg );
	if ( arg[0] == '\0' )
		return;

	if ( !str_cmp(speech, "aiutami saggio") )
		heal_army( mob, ch );
	else
		return;
}


void greet_prog_armourer( CHAR_DATA *mob, CHAR_DATA *ch )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;
	interpret( mob, "smile", FALSE );
	sprintf
	(
		buf,
		"Questa e' la mia armeria, %s",
		str_cmp(mob->in_room->area->name, hometown_table[ch->hometown].name) ? "viaggiatore" : ch->name
	);
	do_say( mob, buf );
	do_say( mob, "Vi interessa qualcosa?" );
	do_say( mob, "Presto avremo molta piu' merce." );
	interpret( mob, "emote sorride orgogliosamente.", FALSE );
}


void greet_prog_baker( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;
	interpret( mob, "smile", FALSE );
	do_say( mob, "Desiderate qualcosa?" );
}


void greet_prog_beggar( CHAR_DATA *mob, CHAR_DATA *ch )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;
	sprintf( buf, "Perdonami %s",
		str_cmp(mob->in_room->area->name, hometown_table[ch->hometown].name) ? "viandante" : ch->name );
	do_say( mob, buf );
	do_say( mob, "Fate la carita'.." );
}


void greet_prog_drunk( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;
	if ( number_percent() < 3 )
	{
		do_yell( mob, "Un dra..drago! Ti uccid.. hic!" );
		do_murder( mob, ch->name );
	}
}


void greet_prog_grocer( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( !can_see(mob, ch) || IS_MOB(ch) || IS_ADMIN(ch) )
		return;

	do_say( mob, "Posso esservi utile in qualcosa?" );
}


void bribe_prog_beggar( CHAR_DATA *mob, CHAR_DATA *ch, int amount )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( amount < 10 )
	{
		sprintf( buf, "Grazie %s",
			str_cmp(mob->in_room->area->name, hometown_table[ch->hometown].name) ? "viandante" : ch->name );
		interpret( mob, buf, FALSE );
		return;
	}
	else if ( amount < 100 )
	{
		do_say( mob, "Vi ringrazio.." );
		return;
	}
	else if ( amount < 500 )
	{
		do_say( mob, "Oh.. grazie!" );
		sprintf( buf, "french %s", ch->name );
		interpret( mob, buf, FALSE );
		return;
	}
	else
	{
		sprintf( buf, "dance %s", ch->name );
		interpret( mob, buf, FALSE );
		sprintf( buf, "french %s", ch->name );
		interpret( mob, buf, FALSE );
		return;
	}
}


void bribe_prog_drunk( CHAR_DATA *mob, CHAR_DATA *ch, int amount )
{
	do_say( mob, "Spiriti! Sempre piu' spirit.. hic!" );
	interpret( mob, "sing", FALSE );
}


void fight_prog_beggar( CHAR_DATA *mob, CHAR_DATA *ch )
{
	if ( mob->point[LIFE] < (mob->max_point[LIFE] * 0.45) && mob->point[LIFE] > (mob->max_point[LIFE] * 0.55) )
		do_say( mob, "Verso la morte.." );
}


bool death_prog_beggar( CHAR_DATA *mob )
{
	if ( number_percent() < 50 )
		do_say( mob, "Un miglior posto mi aspetta." );
	else
		do_say( mob, "Perdonatemi oh Dei.." );
	return( FALSE );
}


bool death_prog_vagabond( CHAR_DATA *mob )
{
	interpret( mob, "emote ride sguaiatamente mostrando la dentatura.. orribile!", FALSE );
	return( FALSE );
}


void speech_prog_crier( CHAR_DATA *mob, CHAR_DATA *ch, char *speech )
{
	char	arg[512];
	/*~~~~~~~~~~~~~*/

	speech = one_argument( speech, arg );
	if ( is_name(arg, "successo") )
		do_say( mob, "La mia fidanzata mi ha lasciato.." );
}


void area_prog_drunk( CHAR_DATA *mob )
{
	if ( number_percent() < 3 )
		interpret( mob, "dance", FALSE );
	else if ( number_percent() < 7 )
		interpret( mob, "sing", FALSE );
}


void area_prog_janitor( CHAR_DATA *mob )
{
	if ( number_percent() < 10 )
	{
		interpret( mob, "grumble", FALSE );
		do_say( mob, "Incivili" );
		if ( number_percent() < 15 )
		{
			do_say( mob, "Devo sempre ripulire cio' che gli altri sporcano." );
			if ( number_percent() < 15 )
				do_say( mob, "Non mi pagano abbastanza per questo." );
			else if ( number_percent() < 15 )
			{
				do_say( mob, "La mia povera schiena.." );
				if ( number_percent() < 5 )
					do_say( mob, "Ahh le mie braccia.." );
			}
		}
	}
}


void area_prog_vagabond( CHAR_DATA *mob )
{
	if ( number_percent() < 5 )
		do_say( mob, "Sangue! Sangue! Sangue!" );
}


void area_prog_baker( CHAR_DATA *mob )
{
	if ( number_percent() < 3 )
		do_say( mob, "Volete provare una deliziosa torta?" );
}


void area_prog_grocer( CHAR_DATA *mob )
{
	if ( number_percent() < 3 )
		do_say( mob, "Avete bisogno di una luce?" );
}


void speech_prog_hunter_cleric( CHAR_DATA *mob, CHAR_DATA *ch, char *speech )
{
	char		buf[MSL];
	OBJ_DATA	*obj, *in_obj;
	int			matched, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( str_cmp(speech, "problema") )
		return;

	if ( ch->cabal != CABAL_HUNTER )
	{
		do_say( mob, "Cosa vorreste da me?" );
		return;
	}

	if ( !IS_SET(ch->quest, QUEST_EYE) )
	{
		do_say( mob, "Cosa intendi?" );
		return;
	}

	matched = 0;
	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( obj->pIndexData->vnum != OBJ_VNUM_EYED_SWORD || strstr(obj->short_descr, ch->name) == NULL )
			continue;

		matched = 1;
		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
			;

		if ( in_obj->carried_by != NULL )
		{
			if ( in_obj->carried_by == ch )
			{
				do_say( mob, "Controlla meglio tra le tue cose!" );
				do_smite( mob, ch->name );
				return;
			}

			sprintf( buf, "La tua spada e' in possesso di %s!", PERS(in_obj->carried_by, ch) );
			do_say( mob, buf );
			if ( in_obj->carried_by->in_room )
			{
				sprintf
				(
					buf,
					"%s e' nella zona  %s presso %s!",
					PERS(in_obj->carried_by, ch),
					in_obj->carried_by->in_room->area->name,
					in_obj->carried_by->in_room->name
				);
				do_say( mob, buf );
				return;
			}
			else
			{
				extract_obj( obj );
				do_say( mob, "Te ne daro' un'altra." );
			}
		}
		else
		{
			if ( in_obj->in_room != NULL )
			{
				sprintf( buf, "La tua Spada si trova nella zona %s presso %s!", in_obj->in_room->area->name, in_obj->in_room->name );
				do_say( mob, buf );
				return;
			}
			else
			{
				extract_obj( obj );
				do_say( mob, "Te ne daro' un'altra." );
			}
		}
		break;
	}

	if ( !matched )
		do_say( mob, "La tua spada e' perduta!" );

	if ( IS_GOOD(ch) )
		i = 0;
	else if ( IS_EVIL(ch) )
		i = 2;
	else
		i = 1;

	obj = create_object( get_obj_index(OBJ_VNUM_EYED_SWORD), 0 );
	obj->owner = str_dup( ch->name );
	obj->from = str_dup( ch->name );
	obj->altar = hometown_table[ch->hometown].altar[i];
	obj->pit = hometown_table[ch->hometown].pit[i];
	obj->level = ch->level;

	sprintf( buf, obj->short_descr, ch->name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->pIndexData->extra_descr->description, ch->name );
	obj->extra_descr = new_extra_descr();
	obj->extra_descr->keyword = str_dup( obj->pIndexData->extra_descr->keyword );
	obj->extra_descr->description = str_dup( buf );
	obj->extra_descr->next = NULL;

	obj->value[2] = ( ch->level / 10 ) + 3;
	obj->level = ch->level;
	obj->cost = 0;
	interpret( mob, "emote forgia una splendida spada.", FALSE );
	do_say( mob, "Ecco, questa e' per te." );
	act( "$N da $p a $n.", ch, obj, mob, TO_ROOM, POS_REST );
	act( "$N ti da $p.", ch, obj, mob, TO_CHAR, POS_REST );
	obj_to_char( obj, ch );
	do_say( mob, "Non perderla ancora!" );
}


void fight_prog_golem( CHAR_DATA *mob, CHAR_DATA *ch )
{
	CHAR_DATA	*master;
	CHAR_DATA	*m_next;
	char		buf[MIL];
	char		*spell;
	int			sn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( master = mob->in_room->people; master != NULL; master = m_next )
	{
		m_next = master->next_in_room;
		if ( !IS_MOB(master) && mob->master == master && master->class == CLASS_NECROMANCER )
			break;
	}

	if ( master == NULL )
		return;

	if ( !master->fighting )
		return;

	if ( master->fighting->fighting == master )
	{
		sprintf( buf, "%s", master->name );
		do_rescue( mob, buf );
	}

	switch ( number_bits(4) )
	{
	case 0:		spell = "curse";		break;
	case 1:		spell = "weaken";		break;
	case 2:		spell = "chill touch";	break;
	case 3:		spell = "blindness";	break;
	case 4:		spell = "poison";		break;
	case 5:		spell = "energy drain";	break;
	case 6:		spell = "harm";			break;
	case 7:		spell = "teleport";		break;
	case 8:		spell = "plague";		break;
	default:	spell = NULL;			break;
	}

	if ( spell == NULL || (sn = skill_lookup(spell)) < 0 )
		return;

	m_next = ( mob->fighting ) ? mob->fighting : master->fighting;
	if ( m_next )
	{
		say_spell( mob, sn );
		( *skill_table[sn].spell_fun ) ( sn, mob->level, mob, m_next, TARGET_CHAR );
	}
}


void area_prog_arpist( CHAR_DATA *mob )
{
	if ( number_percent() < 10 )
	{
		interpret( mob, "emote sorride, socchiude gli occhi cantando.. Tiral.. Nir van estal..", FALSE );
		do_say( mob, "Uran in sion de pr'an.." );
		if ( number_percent() < 15 )
		{
			do_say( mob, "Eden qurrel lorn.. Firnir in sion de pr'an" );
			if ( number_percent() < 15 )
				do_say( mob, "Loec in sion de pr'an" );
			else if ( number_percent() < 15 )
			{
				do_say( mob, "Es yewm van estal.." );
				if ( number_percent() < 7 )
					do_yell( mob, "Es yewm van estal..Es yewm van estal!!!" );
			}
		}
	}
}
