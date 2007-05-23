/* Xandra Code 1.1 by Onirik and T2Team 2004 - 2006 */

/***************************************************************************
 *   ANATOLIA MUD is copyright 1996-2002 Serdar BULUT, Ibrahim CANPUNAR    *
 *   ANATOLIA has been brought to you by ANATOLIA consortium               *
 *     Serdar BULUT     {Chronos}  bulut@anatoliamud.org                   *
 *     Ibrahim Canpunar {Asena}    canpunar@anatoliamud.org                *
 *     Murat BICER      {KIO}      mbicer@anatoliamud.org                  *
 *     D.Baris ACAR     {Powerman} dbacar@anatoliamud.org                  *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ANATOLIA license, in the file xandra/doc/license.anatolia             *
 ***************************************************************************/

/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in xandra/doc/license.diku as well the  *
 *  Merc license in xandra/doc/license.merc.                               *
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
 *   ROM license, in the file Xandra/doc/license.rom                       *
 ***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xandra.h"
#include "magic.h"
#include "interp.h"


DECLARE_OPROG_FUN_WEAR	( wear_prog_excalibur );
DECLARE_OPROG_FUN_REMOVE( remove_prog_excalibur );
DECLARE_OPROG_FUN_DEATH	( death_prog_excalibur );
DECLARE_OPROG_FUN_SPEECH( speech_prog_excalibur );
DECLARE_OPROG_FUN_SAC	( sac_prog_excalibur );

DECLARE_OPROG_FUN_SAC	( sac_prog_cabal_item );
DECLARE_OPROG_FUN_GET	( get_prog_cabal_item );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_sub_weapon );
DECLARE_OPROG_FUN_SPEECH( speech_prog_kassandra );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_chaos_blade );
DECLARE_OPROG_FUN_DEATH	( death_prog_chaos_blade );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_apollon );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_zeus );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_siebele );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_ahrumazda );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_hephaestus );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_ehrumen );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_venus );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_deimos );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_odin );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_phobos );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_mars );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_athena );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_prometheus );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_goktengri );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_hera );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_ares );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_tattoo_eros );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_golden_weapon );
DECLARE_OPROG_FUN_DEATH	( death_prog_golden_weapon );

DECLARE_OPROG_FUN_GET	( get_prog_heart );

DECLARE_OPROG_FUN_WEAR	( wear_prog_bracer );
DECLARE_OPROG_FUN_REMOVE( remove_prog_bracer );

DECLARE_OPROG_FUN_WEAR	( wear_prog_ranger_staff );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_ranger_staff );
DECLARE_OPROG_FUN_DEATH	( death_prog_ranger_staff );

DECLARE_OPROG_FUN_WEAR	( wear_prog_coconut );
DECLARE_OPROG_FUN_ENTRY	( entry_prog_coconut );
DECLARE_OPROG_FUN_GREET	( greet_prog_coconut );
DECLARE_OPROG_FUN_GET	( get_prog_coconut );
DECLARE_OPROG_FUN_REMOVE( remove_prog_coconut );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_firegauntlets );
DECLARE_OPROG_FUN_WEAR	( wear_prog_firegauntlets );
DECLARE_OPROG_FUN_REMOVE( remove_prog_firegauntlets );

/* Ibrahim armbands */
DECLARE_OPROG_FUN_FIGHT	( fight_prog_armbands );
DECLARE_OPROG_FUN_WEAR	( wear_prog_armbands );
DECLARE_OPROG_FUN_REMOVE( remove_prog_armbands );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_demonfireshield );
DECLARE_OPROG_FUN_WEAR	( wear_prog_demonfireshield );
DECLARE_OPROG_FUN_REMOVE( remove_prog_demonfireshield );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_vorbalblade );
DECLARE_OPROG_FUN_GET	( get_prog_spec_weapon );
DECLARE_OPROG_FUN_GET	( get_prog_quest_obj );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_shockwave );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_snake );

/* New ones by Chronos */
DECLARE_OPROG_FUN_WEAR	( wear_prog_wind_boots );
DECLARE_OPROG_FUN_REMOVE( remove_prog_wind_boots );

DECLARE_OPROG_FUN_WEAR	( wear_prog_arm_hercules );
DECLARE_OPROG_FUN_REMOVE( remove_prog_arm_hercules );

DECLARE_OPROG_FUN_WEAR	( wear_prog_girdle_giant );
DECLARE_OPROG_FUN_REMOVE( remove_prog_girdle_giant );

DECLARE_OPROG_FUN_WEAR	( wear_prog_breastplate_strength );
DECLARE_OPROG_FUN_REMOVE( remove_prog_breastplate_strength );

DECLARE_OPROG_FUN_WEAR	( wear_prog_boots_flying );
DECLARE_OPROG_FUN_REMOVE( remove_prog_boots_flying );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_rose_shield );
DECLARE_OPROG_FUN_FIGHT	( fight_prog_lion_claw );

DECLARE_OPROG_FUN_SPEECH( speech_prog_ring_ra );
DECLARE_OPROG_FUN_WEAR	( wear_prog_eyed_sword );
DECLARE_OPROG_FUN_WEAR	( wear_prog_katana_sword );

DECLARE_OPROG_FUN_WEAR	( wear_prog_snake );
DECLARE_OPROG_FUN_REMOVE( remove_prog_snake );
DECLARE_OPROG_FUN_GET	( get_prog_snake );

DECLARE_OPROG_FUN_WEAR	( wear_prog_fire_shield );
DECLARE_OPROG_FUN_REMOVE( remove_prog_fire_shield );
DECLARE_OPROG_FUN_WEAR	( wear_prog_quest_weapon );
DECLARE_OPROG_FUN_GET	( get_prog_quest_reward );
DECLARE_OPROG_FUN_GET	( get_prog_quest_hreward );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_ancient_gloves );
DECLARE_OPROG_FUN_WEAR	( wear_prog_ancient_gloves );
DECLARE_OPROG_FUN_REMOVE( remove_prog_ancient_gloves );

DECLARE_OPROG_FUN_FIGHT	( fight_prog_ancient_shield );
DECLARE_OPROG_FUN_WEAR	( wear_prog_ancient_shield );
DECLARE_OPROG_FUN_REMOVE( remove_prog_ancient_shield );

DECLARE_OPROG_FUN_WEAR	( wear_prog_neckguard );
DECLARE_OPROG_FUN_REMOVE( remove_prog_neckguard );

DECLARE_OPROG_FUN_WEAR	( wear_prog_headguard );
DECLARE_OPROG_FUN_REMOVE( remove_prog_headguard );

DECLARE_OPROG_FUN_WEAR	( wear_prog_blackguard );
DECLARE_OPROG_FUN_REMOVE( remove_prog_blackguard );


void oprog_set( OBJ_INDEX_DATA *objindex, const char *progtype, const char *name )
{
	if ( !str_cmp(progtype, "wear_prog") )
	{
		 /*
		 if (!str_cmp(name, "wear_prog_")) objindex->mprogs->wear_prog = wear_prog_;
		 else if (!str_cmp(name, "wear_prog_")) ...
		 */
		if		( !str_cmp(name, "wear_prog_excalibur") )			objindex->oprogs->wear_prog = wear_prog_excalibur;
		else if ( !str_cmp(name, "wear_prog_bracer") )				objindex->oprogs->wear_prog = wear_prog_bracer;
		else if ( !str_cmp(name, "wear_prog_coconut") )				objindex->oprogs->wear_prog = wear_prog_coconut;
		else if ( !str_cmp(name, "wear_prog_firegauntlets") )		objindex->oprogs->wear_prog = wear_prog_firegauntlets;
		else if ( !str_cmp(name, "wear_prog_armbands") )			objindex->oprogs->wear_prog = wear_prog_armbands;
		else if ( !str_cmp(name, "wear_prog_demonfireshield") )		objindex->oprogs->wear_prog = wear_prog_demonfireshield;
		else if ( !str_cmp(name, "wear_prog_ranger_staff") )		objindex->oprogs->wear_prog = wear_prog_ranger_staff;
		else if ( !str_cmp(name, "wear_prog_wind_boots") )			objindex->oprogs->wear_prog = wear_prog_wind_boots;
		else if ( !str_cmp(name, "wear_prog_boots_flying") )		objindex->oprogs->wear_prog = wear_prog_boots_flying;
		else if ( !str_cmp(name, "wear_prog_arm_hercules") )		objindex->oprogs->wear_prog = wear_prog_arm_hercules;
		else if ( !str_cmp(name, "wear_prog_girdle_giant") )		objindex->oprogs->wear_prog = wear_prog_girdle_giant;
		else if ( !str_cmp(name, "wear_prog_breastplate_strength")) objindex->oprogs->wear_prog = wear_prog_breastplate_strength;
		else if ( !str_cmp(name, "wear_prog_katana_sword") )		objindex->oprogs->wear_prog = wear_prog_katana_sword;
		else if ( !str_cmp(name, "wear_prog_eyed_sword") )			objindex->oprogs->wear_prog = wear_prog_eyed_sword;
		else if ( !str_cmp(name, "wear_prog_snake") )				objindex->oprogs->wear_prog = wear_prog_snake;
		else if ( !str_cmp(name, "wear_prog_fire_shield") )			objindex->oprogs->wear_prog = wear_prog_fire_shield;
		else if ( !str_cmp(name, "wear_prog_quest_weapon") )		objindex->oprogs->wear_prog = wear_prog_quest_weapon;
		else if ( !str_cmp(name, "wear_prog_ancient_gloves") )		objindex->oprogs->wear_prog = wear_prog_ancient_gloves;
		else if ( !str_cmp(name, "wear_prog_ancient_shield") )		objindex->oprogs->wear_prog = wear_prog_ancient_shield;
		else if ( !str_cmp(name, "wear_prog_neckguard") )			objindex->oprogs->wear_prog = wear_prog_neckguard;
		else if ( !str_cmp(name, "wear_prog_headguard") )			objindex->oprogs->wear_prog = wear_prog_headguard;
		else if ( !str_cmp(name, "wear_prog_blackguard") )			objindex->oprogs->wear_prog = wear_prog_blackguard;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_WEAR );
		return;
	}

	if ( !str_cmp(progtype, "remove_prog") )
	{
		if		( !str_cmp(name, "remove_prog_excalibur") )				objindex->oprogs->remove_prog = remove_prog_excalibur;
		else if ( !str_cmp(name, "remove_prog_bracer") )				objindex->oprogs->remove_prog = remove_prog_bracer;
		else if ( !str_cmp(name, "remove_prog_coconut") )				objindex->oprogs->remove_prog = remove_prog_coconut;
		else if ( !str_cmp(name, "remove_prog_firegauntlets") )			objindex->oprogs->remove_prog = remove_prog_firegauntlets;
		else if ( !str_cmp(name, "remove_prog_armbands") )				objindex->oprogs->remove_prog = remove_prog_armbands;
		else if ( !str_cmp(name, "remove_prog_demonfireshield") )		objindex->oprogs->remove_prog = remove_prog_demonfireshield;
		else if ( !str_cmp(name, "remove_prog_wind_boots") )			objindex->oprogs->remove_prog = remove_prog_wind_boots;
		else if ( !str_cmp(name, "remove_prog_boots_flying") )			objindex->oprogs->remove_prog = remove_prog_boots_flying;
		else if ( !str_cmp(name, "remove_prog_arm_hercules") )			objindex->oprogs->remove_prog = remove_prog_arm_hercules;
		else if ( !str_cmp(name, "remove_prog_girdle_giant") )			objindex->oprogs->remove_prog = remove_prog_girdle_giant;
		else if ( !str_cmp(name, "remove_prog_breastplate_strength") )	objindex->oprogs->remove_prog = remove_prog_breastplate_strength;
		else if ( !str_cmp(name, "remove_prog_snake") )					objindex->oprogs->remove_prog = remove_prog_snake;
		else if ( !str_cmp(name, "remove_prog_fire_shield") )			objindex->oprogs->remove_prog = remove_prog_fire_shield;
		else if ( !str_cmp(name, "remove_prog_ancient_gloves") )		objindex->oprogs->remove_prog = remove_prog_ancient_gloves;
		else if ( !str_cmp(name, "remove_prog_ancient_shield") )		objindex->oprogs->remove_prog = remove_prog_ancient_shield;
		else if ( !str_cmp(name, "remove_prog_neckguard") )				objindex->oprogs->remove_prog = remove_prog_neckguard;
		else if ( !str_cmp(name, "remove_prog_headguard") )				objindex->oprogs->remove_prog = remove_prog_headguard;
		else if ( !str_cmp(name, "remove_prog_blackguard") )			objindex->oprogs->remove_prog = remove_prog_blackguard;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_REMOVE );
		return;
	}

	if ( !str_cmp(progtype, "get_prog") )
	{
		if		( !str_cmp(name, "get_prog_cabal_item") )		objindex->oprogs->get_prog = get_prog_cabal_item;
		else if ( !str_cmp(name, "get_prog_heart") )			objindex->oprogs->get_prog = get_prog_heart;
		else if ( !str_cmp(name, "get_prog_coconut") )			objindex->oprogs->get_prog = get_prog_coconut;
		else if ( !str_cmp(name, "get_prog_spec_weapon") )		objindex->oprogs->get_prog = get_prog_spec_weapon;
		else if ( !str_cmp(name, "get_prog_quest_obj") )		objindex->oprogs->get_prog = get_prog_quest_obj;
		else if ( !str_cmp(name, "get_prog_snake") )			objindex->oprogs->get_prog = get_prog_snake;
		else if ( !str_cmp(name, "get_prog_quest_reward") )		objindex->oprogs->get_prog = get_prog_quest_reward;
		else if ( !str_cmp(name, "get_prog_quest_hreward") )	objindex->oprogs->get_prog = get_prog_quest_hreward;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_GET );
		return;
	}

	if ( !str_cmp(progtype, "drop_prog") )
	{
		/* if (!str_cmp(name, "drop_prog_")) objindex->oprogs->drop_prog = drop_prog_; else */
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_DROP );
		return;
	}

	/* returning TRUE prevents destruction, gives player gold */
	if ( !str_cmp(progtype, "sac_prog") )
	{
		if ( !str_cmp(name, "sac_prog_excalibur") )				objindex->oprogs->sac_prog = sac_prog_excalibur;
		else if ( !str_cmp(name, "sac_prog_cabal_item") )		objindex->oprogs->sac_prog = sac_prog_cabal_item;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_SAC );
		return;
	}

	if ( !str_cmp(progtype, "entry_prog") )
	{
		if ( !str_cmp(name, "entry_prog_coconut") )				objindex->oprogs->entry_prog = entry_prog_coconut;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_ENTRY );
		return;
	}

	if ( !str_cmp(progtype, "give_prog") )
	{
		/* if (!str_cmp(name, "give_prog_")) objindex->oprogs->give_prog = give_prog_; else */
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_GIVE );
		return;
	}

	if ( !str_cmp(progtype, "greet_prog") )
	{
		if ( !str_cmp(name, "greet_prog_coconut") )				objindex->oprogs->greet_prog = greet_prog_coconut;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_GREET );
		return;
	}

	if ( !str_cmp(progtype, "fight_prog") )
	{
		if		( !str_cmp(name, "fight_prog_ranger_staff") )		objindex->oprogs->fight_prog = fight_prog_ranger_staff;
		else if ( !str_cmp(name, "fight_prog_sub_weapon") )			objindex->oprogs->fight_prog = fight_prog_sub_weapon;
		else if ( !str_cmp(name, "fight_prog_chaos_blade") )		objindex->oprogs->fight_prog = fight_prog_chaos_blade;
		else if ( !str_cmp(name, "fight_prog_tattoo_apollon") )		objindex->oprogs->fight_prog = fight_prog_tattoo_apollon;
		else if ( !str_cmp(name, "fight_prog_tattoo_zeus") )		objindex->oprogs->fight_prog = fight_prog_tattoo_zeus;
		else if ( !str_cmp(name, "fight_prog_tattoo_siebele") )		objindex->oprogs->fight_prog = fight_prog_tattoo_siebele;
		else if ( !str_cmp(name, "fight_prog_tattoo_ahrumazda") )	objindex->oprogs->fight_prog = fight_prog_tattoo_ahrumazda;
		else if ( !str_cmp(name, "fight_prog_tattoo_hephaestus") )	objindex->oprogs->fight_prog = fight_prog_tattoo_hephaestus;
		else if ( !str_cmp(name, "fight_prog_tattoo_ehrumen") )		objindex->oprogs->fight_prog = fight_prog_tattoo_ehrumen;
		else if ( !str_cmp(name, "fight_prog_tattoo_venus") )		objindex->oprogs->fight_prog = fight_prog_tattoo_venus;
		else if ( !str_cmp(name, "fight_prog_tattoo_deimos") )		objindex->oprogs->fight_prog = fight_prog_tattoo_deimos;
		else if ( !str_cmp(name, "fight_prog_tattoo_odin") )		objindex->oprogs->fight_prog = fight_prog_tattoo_odin;
		else if ( !str_cmp(name, "fight_prog_tattoo_phobos") )		objindex->oprogs->fight_prog = fight_prog_tattoo_phobos;
		else if ( !str_cmp(name, "fight_prog_tattoo_mars") )		objindex->oprogs->fight_prog = fight_prog_tattoo_mars;
		else if ( !str_cmp(name, "fight_prog_tattoo_athena") )		objindex->oprogs->fight_prog = fight_prog_tattoo_athena;
		else if ( !str_cmp(name, "fight_prog_golden_weapon") )		objindex->oprogs->fight_prog = fight_prog_golden_weapon;
		else if ( !str_cmp(name, "fight_prog_snake") )				objindex->oprogs->fight_prog = fight_prog_snake;
		else if ( !str_cmp(name, "fight_prog_tattoo_prometheus") )	objindex->oprogs->fight_prog = fight_prog_tattoo_prometheus;
		else if ( !str_cmp(name, "fight_prog_tattoo_goktengri") )	objindex->oprogs->fight_prog = fight_prog_tattoo_goktengri;
		else if ( !str_cmp(name, "fight_prog_shockwave") )			objindex->oprogs->fight_prog = fight_prog_shockwave;
		else if ( !str_cmp(name, "fight_prog_firegauntlets") )		objindex->oprogs->fight_prog = fight_prog_firegauntlets;
		else if ( !str_cmp(name, "fight_prog_armbands") )			objindex->oprogs->fight_prog = fight_prog_armbands;
		else if ( !str_cmp(name, "fight_prog_demonfireshield") )	objindex->oprogs->fight_prog = fight_prog_demonfireshield;
		else if ( !str_cmp(name, "fight_prog_vorbalblade") )		objindex->oprogs->fight_prog = fight_prog_vorbalblade;
		else if ( !str_cmp(name, "fight_prog_rose_shield") )		objindex->oprogs->fight_prog = fight_prog_rose_shield;
		else if ( !str_cmp(name, "fight_prog_lion_claw") )			objindex->oprogs->fight_prog = fight_prog_lion_claw;
		else if ( !str_cmp(name, "fight_prog_tattoo_hera") )		objindex->oprogs->fight_prog = fight_prog_tattoo_hera;
		else if ( !str_cmp(name, "fight_prog_tattoo_ares") )		objindex->oprogs->fight_prog = fight_prog_tattoo_ares;
		else if ( !str_cmp(name, "fight_prog_tattoo_eros") )		objindex->oprogs->fight_prog = fight_prog_tattoo_eros;
		else if ( !str_cmp(name, "fight_prog_ancient_gloves") )		objindex->oprogs->fight_prog = fight_prog_ancient_gloves;
		else if ( !str_cmp(name, "fight_prog_ancient_shield") )		objindex->oprogs->fight_prog = fight_prog_ancient_shield;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_FIGHT );
		return;
	}

	if ( !str_cmp(progtype, "death_prog") ) /* returning TRUE prevents death */
	{
		if ( !str_cmp(name, "death_prog_excalibur") )			objindex->oprogs->death_prog = death_prog_excalibur;
		else if ( !str_cmp(name, "death_prog_ranger_staff") )	objindex->oprogs->death_prog = death_prog_ranger_staff;
		else if ( !str_cmp(name, "death_prog_chaos_blade") )	objindex->oprogs->death_prog = death_prog_chaos_blade;
		else if ( !str_cmp(name, "death_prog_golden_weapon") )	objindex->oprogs->death_prog = death_prog_golden_weapon;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_DEATH );
		return;
	}

	if ( !str_cmp(progtype, "speech_prog") )
	{
		if ( !str_cmp(name, "speech_prog_excalibur") )			objindex->oprogs->speech_prog = speech_prog_excalibur;
		else if ( !str_cmp(name, "speech_prog_kassandra") )		objindex->oprogs->speech_prog = speech_prog_kassandra;
		else if ( !str_cmp(name, "speech_prog_ring_ra") )		objindex->oprogs->speech_prog = speech_prog_ring_ra;
		else
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_SPEECH );
		return;
	}

	if ( !str_cmp(progtype, "area_prog") )
	{
		/* if (!str_cmp(name, "area_prog_")) objindex->oprogs->area_prog = area_prog_; else */
		{
			bug( "Load_oprogs: 'O': Function not found for vnum %d", objindex->vnum );
			exit( 1 );
		}

		SET_BIT( objindex->progtypes, OPROG_AREA );
		return;
	}

	bug( "Load_oprogs: 'O': invalid program type for vnum %d", objindex->vnum );
	exit( 1 );
}


void wear_prog_excalibur( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "$p si circonda di un'aura di fiamme bianche.", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$p si circonda di un'aura di fiamme bianche.", ch, obj, NULL, TO_ROOM, POS_REST );

	if		( ch->level > 20 && ch->level <= 30 )		obj->value[2] = 4;
	else if ( ch->level > 30 && ch->level <= 40 )		obj->value[2] = 5;
	else if ( ch->level > 40 && ch->level <= 50 )		obj->value[2] = 6;
	else if ( ch->level > 50 && ch->level <= 60 )		obj->value[2] = 7;
	else if ( ch->level > 60 && ch->level <= 70 )		obj->value[2] = 9;
	else if ( ch->level > 70 && ch->level <= 80 )		obj->value[2] = 11;
	else												obj->value[2] = 12;
}


void wear_prog_bracer( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_haste) )
	{
		send_to_char( "Nell'istante in cui li infili essi  avviluppano le braccia come una seconda pelle.\n\r", ch );
		send_to_char( "Percepisci una leggerezza incredibile nel muoverti.\n\r", ch );

		af.where = TO_AFFECTS;
		af.type = gsn_haste;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = AFF_HASTE;
		af.location = APPLY_DEX;
		af.modifier = 1 + ( ch->level >= 18 ) + ( ch->level >= 30 ) + ( ch->level >= 45 );
		affect_to_char( ch, &af );
	}
}


void remove_prog_bracer( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_haste) )
	{
		affect_strip( ch, gsn_haste );
		send_to_char( "Il diffuso senso di leggerezza che pervadeva il tuo corpo svanisce.\n\r", ch );
	}
}


void remove_prog_excalibur( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "$p riassorbe nella lama l'aura di fiamme bianche.", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "$p riassorbe nella lama l'aura di fiamme bianche.", ch, obj, NULL, TO_ROOM, POS_REST );
}


bool death_prog_excalibur( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "Una forte aura blu avvampa lungo la lama di $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
	act( "Una forte aura blu avvampa lungo la lama di $p.,", ch, obj, NULL, TO_ROOM, POS_REST );

	ch->point[LIFE] = ch->max_point[LIFE];
	send_to_char( "Dall'elsa di Excalibur si sprigiona un Sacro tepore che ti ristora.", ch );
	act( "$n sembra stare meglio.", ch, NULL, NULL, TO_ROOM, POS_REST );

	return( TRUE );
}


void speech_prog_excalibur( OBJ_DATA *obj, CHAR_DATA *ch, char *speech )
{
	if ( !str_cmp(speech, "In nomine Sacra!") && (ch->fighting) && is_wielded_char(ch, obj) )
	{
		send_to_char( "Un getto d'acido si sprigiona da Excalibur.\n\r", ch );
		act( "Un getto d'acido si sprigiona da Excalibur.", ch, NULL, NULL, TO_ROOM, POS_REST );
		obj_cast_spell( gsn_acid_blast, ch->level, ch, ch->fighting, NULL );
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	}
}


bool sac_prog_excalibur( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "Il cielo tuona fragorosamente!", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "Il cielo tuona fragorosamente!", ch, NULL, NULL, TO_ROOM, POS_REST );

	damage( ch, ch, (ch->point[LIFE] - 1) > 1000 ? 1000 : (ch->point[LIFE] - 1), TYPE_HIT, DAM_HOLY, TRUE );
	ch->gold = 0;

	return( TRUE );
}


void fight_prog_ranger_staff( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_wielded_char(ch, obj) && number_percent() < 10 )
	{
		send_to_char( "Avverti un pallido lucore lungo il tuo bastone da ranger.\n\r", ch );
		act( "$n bastone da ranger sembra brillare debolmente per un istante.", ch, NULL, NULL, TO_ROOM, POS_REST );

		obj_cast_spell( gsn_cure_critical, ch->level, ch, ch, obj );
	}
}


void fight_prog_sub_weapon( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_wielded_char(ch, obj) && number_percent() < 30 )
	{
		if ( ((float) ch->point[LIFE]) / ((float) ch->max_point[LIFE]) > 0.9 )
			send_to_char( "La tua arma ti sussurra, 'Stai combattendo bene!'\n\r", ch );
		else if ( ((float) ch->point[LIFE]) / ((float) ch->max_point[LIFE]) > 0.6 )
			send_to_char( "La tua arma ti sussurra, 'Avanti cosi'!'\n\r", ch );
		else if ( ((float) ch->point[LIFE]) / ((float) ch->max_point[LIFE]) > 0.4 )
			send_to_char( "La tua arma ti sussurra, 'Ce la puoi fare!'\n\r", ch );
		else
			send_to_char( "La tua arma urla, 'VIA! SCAPPIAMO VIA!'\n\r", ch );
	}
}


bool death_prog_ranger_staff( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Il tuo bastone si dissolve nel nulla.\n\r", ch );
	act( "Il bastone di $n si dissolve nel nulla.", ch, NULL, NULL, TO_ROOM, POS_REST );
	extract_obj( obj );
	return( FALSE );
}


void get_prog_spec_weapon( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( strstr(obj->extra_descr->description, ch->name) != NULL )
	{
		if ( IS_AFFECTED(ch, AFF_POISON) && (dice(1, 5) == 1) )
		{
			send_to_char( "Un lucore verdastro illumina la tua arma.", ch );
			act( "Un sinistro lucore illumina l'arma di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
			spell_cure_poison( gsn_cure_poison, 30, ch, ch, TARGET_CHAR );
			return;
		}

		if ( IS_AFFECTED(ch, AFF_CURSE) && (dice(1, 5) == 1) )
		{
			send_to_char( "Una sinistra luce rossa aleggia sulla tua arma.", ch );
			act( "Una sinistra aura rossa sorvola l'arma di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
			spell_remove_curse( gsn_remove_curse, 30, ch, ch, TARGET_CHAR );
			return;
		}

		send_to_char( "La tua arma sembra tornare al suo normale stato.\n\r", ch );
		return;
	}

	act( "Cerchi di impugnare $p ma ti respinge!", ch, obj, NULL, TO_CHAR, POS_REST );

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );

	switch ( dice(1, 10) )
	{
	case 1: spell_curse( gsn_curse, ch->level < 10 ? 1 : ch->level - 9, ch, ch, TARGET_CHAR ); break;
	case 2: spell_poison( gsn_poison, ch->level < 10 ? 1 : ch->level - 9, ch, ch, TARGET_CHAR ); break;
	}
}


void get_prog_quest_hreward( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( strstr(obj->extra_descr->description, ch->name) != NULL )
	{
		act( "$C$p comincia a risplendere.\n\r$c",
			ch, obj, NULL, TO_CHAR, POS_SLEEP, BLUE );
		return;
	}

	act( "Cerchi di prendere $p ma ti respinge!", ch, obj, NULL, TO_CHAR, POS_REST );

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
}


void get_prog_quest_obj( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( strstr(obj->extra_descr->description, ch->name) != NULL )
	{
		if ( IS_AFFECTED(ch, AFF_POISON) && (dice(1, 5) == 1) )
		{
			send_to_char( "Un lucore verdastro illumina la tua arma.", ch );
			act( "Un sinistro lucore illumina l'arma di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
			spell_cure_poison( gsn_cure_poison, 30, ch, ch, TARGET_CHAR );
			return;
		}

		if ( IS_AFFECTED(ch, AFF_CURSE) && (dice(1, 5) == 1) )
		{
			send_to_char( "Una sinistra luce rossa aleggia sulla tua arma.", ch );
			act( "Una sinistra aura rossa sorvola l'arma di $n.", ch, NULL, NULL, TO_ROOM, POS_REST );
			spell_remove_curse( gsn_remove_curse, 30, ch, ch, TARGET_CHAR );
			return;
		}

		send_to_char( "L'oggetto aspetta di ritornare al suo posto..\n\r", ch );
		return;
	}

	act( "Cerchi di prendere $p ma ti respinge!", ch, obj, NULL, TO_CHAR, POS_REST );

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );

	switch ( dice(1, 10) )
	{
	case 1: spell_curse( gsn_curse, ch->level < 10 ? 1 : ch->level - 9, ch, ch, TARGET_CHAR ); break;
	case 2: spell_poison( gsn_poison, ch->level < 10 ? 1 : ch->level - 9, ch, ch, TARGET_CHAR ); break;
	}
}


void get_prog_cabal_item( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( IS_MOB(ch) )
	{
		act( "Una forza misteriosa trascina $p per terra.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "Una forza misteriosa trascina $p per terra.", ch, obj, NULL, TO_ROOM, POS_REST );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		return;
	}

	if ( obj->timer < 1 )
	{
		obj->timer = 30;
		act( "$p diventa trasparente.", ch, obj, NULL, TO_CHAR, POS_REST );
		act( "$p diventa trasparente.", ch, obj, NULL, TO_ROOM, POS_REST );
	}
}


bool sac_prog_cabal_item( OBJ_DATA *obj, CHAR_DATA *ch )
{
	OBJ_DATA	*container;
	char		buf[160];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~*/

	act( "Nel cielo rimbombano fragorosi tuoni!", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "Nel cielo rimbombano fragorosi tuoni!", ch, NULL, NULL, TO_ROOM, POS_REST );
	damage( ch, ch, (int) (ch->point[LIFE] / 10), TYPE_HIT, DAM_HOLY, TRUE );
	ch->gold = 0;

	obj_from_room( obj );
	for ( i = 0; i < MAX_CABAL; i++ )
		if ( cabal_table[i].obj_ptr == obj )
			break;

	if ( i < MAX_CABAL )
	{
		if ( obj->pIndexData->vnum == cabal_table[CABAL_RULER].obj_vnum )
			container = create_object( get_obj_index(OBJ_VNUM_RULER_STAND), 100 );
		else if ( obj->pIndexData->vnum == cabal_table[CABAL_INVADER].obj_vnum )
			container = create_object( get_obj_index(OBJ_VNUM_INVADER_SKULL), 100 );
		else if ( obj->pIndexData->vnum == cabal_table[CABAL_ARMY].obj_vnum )
			container = create_object( get_obj_index(OBJ_VNUM_ARMY_THRONE), 100 );
		else if ( obj->pIndexData->vnum == cabal_table[CABAL_KNIGHT].obj_vnum )
			container = create_object( get_obj_index(OBJ_VNUM_KNIGHT_ALTAR), 100 );
		else if ( obj->pIndexData->vnum == cabal_table[CABAL_CHAOS].obj_vnum )
			container = create_object( get_obj_index(OBJ_VNUM_CHAOS_ALTAR), 100 );
		else if ( obj->pIndexData->vnum == cabal_table[CABAL_LIONS].obj_vnum )
			container = create_object( get_obj_index(OBJ_VNUM_LIONS_ALTAR), 100 );
		else if ( obj->pIndexData->vnum == cabal_table[CABAL_HUNTER].obj_vnum )
			container = create_object( get_obj_index(OBJ_VNUM_HUNTER_ALTAR), 100 );
		else
			container = create_object( get_obj_index(OBJ_VNUM_YVALADRIN_ALTAR), 100 );

		obj_to_obj( obj, container );
		obj_to_room( container, get_room_index(cabal_table[i].room_vnum) );
		sprintf( buf, "Vedi %s riprendere forma lentamente.\n\r", container->short_descr );
		if ( get_room_index(cabal_table[i].room_vnum)->people != NULL )
		{
			act( buf, get_room_index(cabal_table[i].room_vnum)->people, NULL, NULL, TO_CHAR, POS_REST );
			act( buf, get_room_index(cabal_table[i].room_vnum)->people, NULL, NULL, TO_ROOM, POS_REST );
		}

		return( TRUE );
	}
	else
	{
		extract_obj( obj );
		bug( "oprog: Sac_cabal_item: Was not the cabal's item.", 0 );
	}

	return( FALSE );
}


void speech_prog_kassandra( OBJ_DATA *obj, CHAR_DATA *ch, char *speech )
{
	if ( !str_cmp(speech, "kassandra") && (get_hold_char(ch) == obj) && !IS_MOB(ch) )
		obj_cast_spell( gsn_kassandra, ch->level, ch, ch, NULL );
	else if ( !str_cmp(speech, "sebat") && (get_hold_char(ch) == obj) && !IS_MOB(ch) )
		obj_cast_spell( gsn_sebat, ch->level, ch, ch, NULL );
	else if ( !str_cmp(speech, "matandra") && (get_hold_char(ch) == obj) && (ch->fighting) && !IS_MOB(ch) )
	{
		act( "Una scarica di energia schizza dalle tue mani su $N!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Una scarica di energia schizza dalle mani di $n su di te!", ch, NULL, ch->fighting, TO_VICT, POS_REST );
		act( "Una scarica di energia schizza dalle mani di $n su $N!", ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
		obj_cast_spell( gsn_matandra, ch->level, ch, ch->fighting, NULL );
	}
}


void fight_prog_chaos_blade( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_wielded_char(ch, obj) )
	{
		switch ( number_bits(7) )
		{
		case 0:
			act( "Avverti uno strano ronzio..", ch, NULL, NULL, TO_ROOM, POS_REST );
			send_to_char( "La tua lama del caos vibra vistosamente!\n\r", ch );
			obj_cast_spell( gsn_mirror, ch->level, ch, ch, obj );
			WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
			break;

		case 1:
			act( "Odi un distinto tintinnio.", ch, NULL, NULL, TO_ROOM, POS_REST );
			send_to_char( "La tua spada del caos freme per un istante!\n\r", ch );
			obj_cast_spell( gsn_garble, ch->level, ch, ch->fighting, obj );
			WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
			break;

		case 2:
			act( "Confusi sprazzi di luce schizzano per la stanza!", ch, NULL, NULL, TO_ROOM, POS_REST );
			send_to_char( "La tua lama del caos sprizza luce vibrando senza controllo!\n\r", ch );
			obj_cast_spell( gsn_confuse, ch->level, ch, ch->fighting, obj );
			WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
			break;
		}
	}
}


bool death_prog_chaos_blade( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "La tua spada del caos svanisce.\n\r", ch );
	act( "La spada di $n svanisce.", ch, NULL, NULL, TO_ROOM, POS_REST );
	extract_obj( obj );
	return( FALSE );
}


void fight_prog_tattoo_apollon( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int sn;
	/*~~~*/

	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
		case 1:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			break;

		case 2:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			do_yell( ch, "Ayel dneai leena.." );
			sn = skill_lookup( "holy word" );
			spell_holy_word( sn, ch->level, ch, NULL, TARGET_CHAR );
			break;
		}
	}
}


void fight_prog_tattoo_zeus( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
		case 1:
		case 2:
			act( "$CIl tuo tatuaggio diventa rosso fuoco.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_critical, ch->level, ch, ch, obj );
			break;

		case 3:
			act( "$CIl tuo tatuaggio diventa nero..$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			if ( IS_AFFECTED(ch, AFF_PLAGUE) )
				spell_cure_disease( 25, 100, ch, ch, TARGET_CHAR );
			if ( IS_AFFECTED(ch, AFF_POISON) )
				spell_cure_poison( 27, 100, ch, ch, TARGET_CHAR );
			break;
		}
	}
}


void fight_prog_tattoo_siebele( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
			act( "$CIl tuo tatuaggio diventa viola.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			break;

		case 1:
			act( "$CIl tuo tatuaggio diventa bianco.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			spell_bluefire( gsn_dispel_good, ch->level, ch, ch->fighting, TARGET_CHAR );
			break;
		}
	}
}


void fight_prog_tattoo_ahrumazda( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
			act( "$CIl tuo tatuaggio diventa nero.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, BLUE );
			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			break;

		case 1:
			act( "$CIl tuo tatuaggio diventa rossastro.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_demonfire, ch->level, ch, ch->fighting, obj );
			break;
		}
	}
}


void fight_prog_tattoo_hephaestus( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
		case 1:
			act( "$CIl tuo tatuaggio diventa giallo.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			break;

		case 2:
			act( "$CIl tuo tatuaggio diventa bianco avorio.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			do_yell( ch, "In nome della giustizia!!.." );
			spell_scream( gsn_scream, ch->level, ch, ch->fighting, TARGET_CHAR );
			break;
		}
	}
}


void fight_prog_tattoo_ehrumen( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
			act( "$CIl tuo tatuaggio diventa color ghiaccio.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_cure_light, ch->level, ch, ch->fighting, obj );
			break;

		case 1:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			break;

		case 2:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			spell_dispel_evil( gsn_dispel_evil, ch->level, ch, ch->fighting, TARGET_CHAR );
			break;
		}
	}
}


void fight_prog_tattoo_venus( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(7) )
		{
		case 0:
		case 1:
		case 2:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_light, ch->level, ch, ch, obj );
			break;

		case 3:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_plague, ch->level, ch, ch->fighting, obj );
			break;

		case 4:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_bless, ch->level, ch, ch, obj );
			break;
		}
	}
}


void fight_prog_tattoo_ares( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(5) )
		{
		case 0:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_dragon_strength, ch->level, ch, ch, obj );
			break;

		case 1:
			act( "$CIl tuo tatuaggio risplende di un rosso intenso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_dragon_breath, ch->level, ch, ch->fighting, obj );
			break;
		}
	}
}


void fight_prog_tattoo_odin( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(5) )
		{
		case 0:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_critical, ch->level, ch, ch, obj );
			break;

		case 1:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_faerie_fire, ch->level, ch, ch->fighting, obj );
			break;
		}
	}
}


void fight_prog_tattoo_phobos( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int sn;
	/*~~~*/

	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			break;

		case 1:
			if ( (sn = skill_lookup("colour spray")) < 0 )
				break;
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( sn, ch->level, ch, ch->fighting, obj );
			break;
		}
	}
}


void fight_prog_tattoo_mars( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(7) )
		{
		case 0:
			obj_cast_spell( gsn_blindness, ch->level, ch, ch->fighting, obj );
			send_to_char( "Dal tuo tatuaggio si sprigiona una strana nuvola di vapore!\n\r", ch );
			break;

		case 1:
			obj_cast_spell( gsn_poison, ch->level, ch, ch->fighting, obj );
			send_to_char( "Una stilla luminosa verdastra sprizza dal tuo tatuaggio al nemico.\n\r", ch );
			break;

		case 2:
			obj_cast_spell( gsn_haste, ch->level, ch, ch, obj );
			send_to_char( "Il tuo tatuaggio comincia a bruciare dandoti nuove energie!\n\r", ch );
			break;

		case 3:
			obj_cast_spell( gsn_shield, ch->level, ch, ch, obj );
			send_to_char( "Il tuo tatuaggio proietta una sinistra ombra sull'avversario!\n\r", ch );
			break;
		}
	}
}


void fight_prog_tattoo_athena( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		if ( number_percent() < 50 )
		{
			switch ( number_bits(4) )
			{
			case 0:
				if ( IS_AFFECTED(ch, AFF_BERSERK) || is_affected(ch, gsn_berserk) || is_affected(ch, skill_lookup("frenzy")) )
				{
					send_to_char( "Senti la furia dentro di te aumentare.\n\r", ch );
					return;
				}

				af.where = TO_AFFECTS;
				af.type = gsn_berserk;
				af.level = ch->level;
				af.duration = ch->level / 3;
				af.modifier = ch->level / 5;
				af.bitvector = AFF_BERSERK;

				af.location = APPLY_HITROLL;
				affect_to_char( ch, &af );

				af.location = APPLY_DAMROLL;
				affect_to_char( ch, &af );

				af.modifier = 10 * ( ch->level / 10 );
				af.location = APPLY_AC;
				affect_to_char( ch, &af );

				ch->point[LIFE] += ch->level * 2;
				ch->point[LIFE] = UMIN( ch->point[LIFE], ch->max_point[LIFE] );

				send_to_char( "Il tuo corpo e' una cosa sola con la tua ira!\n\r", ch );
				act( "$n ha gli occhi indemoniati!", ch, NULL, NULL, TO_ROOM, POS_REST );

				break;
			}
		}
		else
		{
			switch ( number_bits(4) )
			{
			case 0: do_yell( ch, "Non esiste gloria senza sangue!" ); break;
			case 1: do_yell( ch, "Nessuna pieta'!" ); break;
			case 2: do_yell( ch, "La morte e' giunta e porta il mio nome!" ); break;
			case 3: do_yell( ch, "Alla battaglia e alla vittoria!" ); break;
			case 4: do_yell( ch, "Il mio sentiero e' un tappeto di cadaveri!" ); break;
			}
		}
	}
}


void fight_prog_tattoo_hera( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(5) )
		{
		case 0:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_plague, ch->level, ch, ch->fighting, obj );
			break;

		case 1:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_poison, ch->level, ch, ch->fighting, obj );

		case 2:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_weaken, ch->level, ch, ch->fighting, obj );

		case 3:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			obj_cast_spell( gsn_slow, ch->level, ch, ch->fighting, obj );
			break;
		}
	}
}


void fight_prog_tattoo_deimos( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int sn;
	/*~~~*/

	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(6) )
		{
		case 0:
		case 1:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			break;

		case 2:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			sn = skill_lookup( "web" );
			spell_web( sn, ch->level, ch, ch->fighting, TARGET_CHAR );
			break;
		}
	}
}


void fight_prog_tattoo_eros( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int sn;
	/*~~~*/

	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(5) )
		{
		case 0:
		case 1:
			if ( (sn = skill_lookup("heal")) < 0 )
				break;
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( sn, ch->level, ch, ch, obj );
			break;

		case 2:
			if ( (sn = skill_lookup("mass healing")) < 0 )
				break;
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( sn, ch->level, ch, ch, obj );
			break;
		}
	}
}


bool death_prog_golden_weapon( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "La tua arma dorata svanisce.\n\r", ch );
	act( "L'arma dorata di $n svanisce.", ch, NULL, NULL, TO_ROOM, POS_REST );
	extract_obj( obj );
	ch->point[LIFE] = 1;
	while ( ch->affected )
		affect_remove( ch, ch->affected );
	ch->last_fight_time = -1;
	ch->last_death_time = current_time;
	if ( cabal_area_check(ch) )
	{
		act( "$n svanisce.", ch, NULL, NULL, TO_ROOM, POS_REST );
		char_from_room( ch );
		char_to_room( ch, get_room_index(cabal_table[CABAL_KNIGHT].room_vnum) );
		act( "$n appare improvvisamente.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	return( TRUE );
}


void fight_prog_golden_weapon( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_wielded_char(ch, obj) )
	{
		if ( number_percent() < 4 )
		{
			act( "La tua $p risplende di blu!", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "La $p di $n risplende di blu!", ch, obj, NULL, TO_ROOM, POS_REST );

			obj_cast_spell( gsn_cure_critical, ch->level, ch, ch, obj );
			return;
		}
		else if ( number_percent() > 92 )
		{
			act( "La tua $p risplende di blu!", ch, obj, NULL, TO_CHAR, POS_REST );
			act( "La $p di $n risplende di blu!", ch, obj, NULL, TO_ROOM, POS_REST );

			obj_cast_spell( gsn_cure_serious, ch->level, ch, ch, obj );
			return;
		}
	}
}


void get_prog_heart( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( obj->timer == 0 )
		obj->timer = 24;
}


void fight_prog_snake( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_wielded_char(ch, obj) )
	{
		switch ( number_bits(7) )
		{
		case 0:
			act( "Una delle teste di serpente della tua frusta si anima e morde $N!",
				ch, NULL, ch->fighting, TO_CHAR, POS_REST );
			act( "Una delle teste di serpente della frusta di $n si anima e ti morde!",
				ch, NULL, ch->fighting, TO_VICT, POS_REST );
			act( "Una delle teste di serpente della frusta di $n si anima e morde $N!",
				ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
			obj_cast_spell( gsn_poison, ch->level, ch, ch->fighting, obj );
			break;

		case 1:
			act( "Una delle teste di serpente della tua frusta si anima e morde $N!",
				ch, NULL, ch->fighting, TO_CHAR, POS_REST );
			act( "Una delle teste di serpente della frusta di $n si anima e ti morde!",
				ch, NULL, ch->fighting, TO_VICT, POS_REST );
			act( "Una delle teste di serpente della frusta di $n si anima e morde $N!",
				ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
			obj_cast_spell( gsn_weaken, ch->level, ch, ch->fighting, obj );
			break;
		}
	}
}


void fight_prog_tattoo_prometheus( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(5) )
		{
		case 0:
			act( "$CIl tuo tatuaggio risplende di blu.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, CYAN );
			obj_cast_spell( gsn_cure_critical, ch->level, ch, ch, obj );
			break;

		case 1:
		case 2:
			act( "$CIl tuo tatuaggio risplende di rosso.$c", ch, NULL, NULL, TO_CHAR, POS_DEAD, RED );
			if ( IS_EVIL(ch->fighting) )
				spell_dispel_evil( gsn_dispel_evil, 1.2 * ch->level, ch, ch->fighting, TARGET_CHAR );
			else if ( IS_GOOD(ch->fighting) )
				spell_dispel_good( gsn_dispel_good, 1.2 * ch->level, ch, ch->fighting, TARGET_CHAR );
			else
				spell_lightning_bolt( 64, 1.2 * ch->level, ch, ch->fighting, TARGET_CHAR );
			break;
		}
	}
}


void fight_prog_shockwave( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_wielded_char(ch, obj) )
	{
		switch ( number_bits(5) )
		{
		case 0:
			act( "Sprigioni un violento arco di energia che colpisce in pieno $N!",
				ch, NULL, ch->fighting, TO_CHAR, POS_REST );
			act( "$n sprigiona un violento arco di energia che ti colpisce in pieno!",
				ch, NULL, ch->fighting, TO_VICT, POS_REST );
			act( "$n sprigiona un violento arco di energia che colpisce in pieno $N!",
				ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
			obj_cast_spell( gsn_lightning_bolt, ch->level, ch, ch->fighting, NULL );
			break;
		}
	}
}


void wear_prog_ranger_staff( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( ch->class != CLASS_RANGER )
	{
		send_to_char( "Non hai la piu' pallida idea di come utilizzare questa roba..\n\r", ch );
		unequip_char( ch, obj );
		send_to_char( "Il bastone ti scivola via dalle mani.\n\r", ch );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		return;
	}
}


void wear_prog_coconut( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "Fai rintuzzare le noci di cocco l'una contro l'altra.", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "Avverti il rumore di una cavalcata..", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "Avverti il rumore di una cavalcata..", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void entry_prog_coconut( OBJ_DATA *obj )
{
	if ( obj->carried_by != NULL )
	{
		if ( get_hold_char(obj->carried_by) == obj )
		{
			act( "Avverti il rumore di una cavalcata..",
				obj->carried_by, NULL, NULL, TO_ROOM, POS_REST );
		}
	}
}


void greet_prog_coconut( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( obj->carried_by != NULL )
	{
		if ( get_hold_char(obj->carried_by) == obj && obj->carried_by != ch )
			act( "Avverti il rumore di una cavalcata..", ch, NULL, NULL, TO_CHAR, POS_REST );
	}
	else
		send_to_char( "Avverti il rumore di una cavalcata..\n\r", ch );
}


void get_prog_coconut( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Avverti il rumore di una cavalcata..\n\r"
		"CLOP CLOP CLOP.\n\r", ch );
	act( "$n avvicina il guscio di cocco all'orecchio.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void remove_prog_coconut( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Il rumore della cavalcata svanisce..\n\r", ch );
	act( "Il rumore della cavalcata svanisce..", ch, NULL, NULL, TO_ROOM, POS_REST );
}


void fight_prog_firegauntlets( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int dam;
	/*~~~~*/

	if ( get_wield_char(ch, FALSE) != NULL )
		return;

	if ( get_eq_char(ch, WEAR_HANDS) != obj )
		return;

	if ( number_percent() < 50 )
	{
		dam = dice( ch->level, 8 ) + number_percent() / 2;
		act( "Una vampata di calore schizza dai tuoi guanti sul volto di $N!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Una vampata di calore schizza dai guanti di $n sul volto di $N!", ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
		act( "Una terribile vampata schizza dai guanti di $N sul tuo viso! Brucia!", ch->fighting, NULL, ch, TO_CHAR, POS_REST );
		damage( ch, ch->fighting, dam / 2, gsn_burning_hands, DAM_FIRE, TRUE );
		if ( ch == NULL || ch->fighting == NULL )
			return;
		fire_effect( ch->fighting, obj->level / 2, dam / 2, TARGET_CHAR );
	}
}


void wear_prog_firegauntlets( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Un brivido caldo ti avvolge mentre infili i guanti.\n\r", ch );
}


void remove_prog_firegauntlets( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Appena rimuovi i guanti una carezza gelida ti accarezza.\n\r", ch );
}


void fight_prog_armbands( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int dam;
	/*~~~~*/

	if ( get_eq_char(ch, WEAR_ARMS) != obj )
		return;

	if ( IS_MOB(ch) )
		return;

	if ( number_percent() < 20 )
	{
		dam = number_percent() / 2 + 30 + 5 * ch->level;
		act( "Un'improvvisa fiammata scaturisce dai tuoi bracciali bruciando $N!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Un'improvvisa fiammata scaturisce dai bracciali di $n bruciando $N!", ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
		act( "Un'improvvisa fiammata scaturisce dai bracciali di $n bruciandoti!", ch->fighting, NULL, ch, TO_CHAR, POS_REST );
		damage( ch, ch->fighting, dam, gsn_burning_hands, DAM_FIRE, TRUE );
		if ( ch == NULL || ch->fighting == NULL )
			return;
		fire_effect( ch->fighting, obj->level / 2, dam, TARGET_CHAR );
	}
}


void wear_prog_armbands( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Una stretta di calore avvampa le tue braccia.\n\r", ch );
}


void remove_prog_armbands( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "La morsa di calore abbandona le tue braccia.\n\r", ch );
}


void fight_prog_demonfireshield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int dam;
	/*~~~~*/

	if ( get_shield_char(ch) != obj )
		return;

	if ( number_percent() < 15 )
	{
		dam = number_percent() / 2 + 5 * ch->level;
		act( "Un vortice nero appare nel tuo scudo!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Una fiammata fuoriuscita dal vortice avvolge $N!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Una fiammata fuoriuscita dallo scudo di $n vortice avvolge $N!", ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
		act( "Una fiammata fuoriuscita dallo scudo di $n ti avvolge!", ch->fighting, NULL, ch, TO_CHAR, POS_REST );
		fire_effect( ch->fighting, obj->level, dam, TARGET_CHAR );
		damage( ch, ch->fighting, dam, gsn_demonfire, DAM_FIRE, TRUE );
	}
}


void wear_prog_demonfireshield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Un diffuso senso di tepore ti avvolge mentre stringi lo scudo.\n\r", ch );
}


void remove_prog_demonfireshield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "L'abbraccio di calore dello scudo ti abbandona.\n\r", ch );
}


void fight_prog_vorbalblade( OBJ_DATA *obj, CHAR_DATA *ch )
{
	CHAR_DATA	*victim;
	/*~~~~~~~~~~~~~~~~*/

	if ( IS_MOB(ch) )
		return;

	if ( !is_wielded_char(ch, obj) )
		return;

	victim = ch->fighting;

	if ( !IS_EVIL(victim) )
		return;

	if ( number_percent() < 10 )
	{
		send_to_char( "La tua arma si muove da sola puntando il tuo avversario!\n\r", ch );
		if ( number_percent() < 20 )
		{
			act( "La lama descrive un arco nell'aria e DECAPITA in un sol colpo $N!", ch, NULL, victim, TO_CHAR, POS_REST );
			act( "$N ti decapita con un sol colpo!", ch, NULL, victim, TO_NOTVICT, POS_REST );
			act( "$n decapita $N con un colpo straordinario!", ch, NULL, victim, TO_ROOM, POS_REST );
			act( "$n e' mort$x!!", victim, NULL, NULL, TO_ROOM, POS_REST );
			raw_kill( victim, 3 );
			act( "Sei stat$x uccis$x!!", victim, NULL, NULL, TO_CHAR, POS_REST );
			return;
		}
	}
}


void wear_prog_wind_boots( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_fly) )
	{
		send_to_char( "Ti senti sollevare appena infili ai piedi gli stivali.\n\r", ch );
		send_to_char( "Inizi a fluttuare.\n\r", ch );

		af.where = TO_AFFECTS;
		af.type = gsn_fly;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = AFF_FLYING;
		af.location = 0;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
}


void remove_prog_wind_boots( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_fly) )
	{
		affect_strip( ch, gsn_fly );
		send_to_char( "Ritorni a terra.\n\r", ch );
		send_to_char( "Lentamente..\n\r", ch );
	}
}


void wear_prog_boots_flying( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_fly) )
	{
		send_to_char( "Ti senti sollevare appena infili ai piedi gli stivali.\n\r", ch );
		send_to_char( "Inizi a fluttuare.\n\r", ch );

		af.where = TO_AFFECTS;
		af.type = gsn_fly;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = AFF_FLYING;
		af.location = 0;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
}


void remove_prog_boots_flying( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_fly) )
	{
		affect_strip( ch, gsn_fly );
			send_to_char( "Ritorni a terra.\n\r", ch );
		send_to_char( "Lentamente..\n\r", ch );
	}
}


void wear_prog_arm_hercules( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_giant_strength) )
	{
		send_to_char( "Appena li indossi senti la tua muscolatura diventare piu' possente.\n\r", ch );
		send_to_char( "Ti senti veramente forte!\n\r", ch );

		af.where = TO_AFFECTS;
		af.type = gsn_giant_strength;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = 0;
		af.location = APPLY_STR;
		af.modifier = 1 + ( ch->level >= 18 ) + ( ch->level >= 30 ) + ( ch->level >= 45 );
		affect_to_char( ch, &af );
	}
}


void remove_prog_arm_hercules( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_giant_strength) )
	{
		affect_strip( ch, gsn_giant_strength );
		send_to_char( "La tua muscolatura ritorna alla normalita'.\n\r", ch );
	}
}


void wear_prog_girdle_giant( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_giant_strength) )
	{
		send_to_char( "Appena la indossi senti la tua muscolatura diventare piu' possente.\n\r", ch );
		send_to_char( "Ti senti veramente forte!\n\r", ch );

		af.where = TO_AFFECTS;
		af.type = gsn_giant_strength;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = 0;
		af.location = APPLY_STR;
		af.modifier = 1 + ( ch->level >= 18 ) + ( ch->level >= 30 ) + ( ch->level >= 45 );
		affect_to_char( ch, &af );
	}
}


void remove_prog_girdle_giant( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_giant_strength) )
	{
		affect_strip( ch, gsn_giant_strength );
		send_to_char( "La tua muscolatura torna alla normalita'.\n\r", ch );
	}
}


void wear_prog_breastplate_strength( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_giant_strength) )
	{
		send_to_char( "Appena la indossi senti la tua muscolatura diventare piu' possente.\n\r", ch );
		send_to_char( "Ti senti veramente forte!\n\r", ch );

		af.where = TO_AFFECTS;
		af.type = gsn_giant_strength;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = 0;
		af.location = APPLY_STR;
		af.modifier = 1 + ( ch->level >= 18 ) + ( ch->level >= 30 ) + ( ch->level >= 45 );
		affect_to_char( ch, &af );
	}
}


void remove_prog_breastplate_strength( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_giant_strength) )
	{
		affect_strip( ch, gsn_giant_strength );
		send_to_char( "La tua muscolatura torna alla normalita'.\n\r", ch );
	}
}


void fight_prog_rose_shield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if (!((ch->in_room->sector_type != SECT_FIELD)
		||	(ch->in_room->sector_type != SECT_FOREST)
		||	(ch->in_room->sector_type != SECT_MOUNTAIN)
		||	(ch->in_room->sector_type != SECT_HILLS)) )
		return;

	if ( get_shield_char(ch) != obj )
		return;

	if ( number_percent() < 90 )
		return;

	send_to_char( "Un turbinio di foglie si solleva dal tuo scudo!\n\r", ch );
	send_to_char( "Le foglie formano un vortice intorno a te!\n\r", ch->fighting );
	act( "Un turbinio di foglie si solleva attorno a $n proteggendol$x!", ch, NULL, NULL, TO_ROOM, POS_REST );
	obj_cast_spell( gsn_slow, ch->level, ch, ch->fighting, NULL );
}


void fight_prog_lion_claw( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( number_percent() < 90 )
		return;

	if ( is_wielded_char(ch, obj) )
	{
		send_to_char( "Dalla tua arma spuntano improvvisamente degli artigli!\n\r", ch );
		act( "Dall'arma di $n spuntano fuori degli artigli!", ch, NULL, NULL, TO_ROOM, POS_DEAD, WHITE );
		one_hit( ch, ch->fighting, TYPE_HIT, FALSE );
		one_hit( ch, ch->fighting, TYPE_HIT, FALSE );
		one_hit( ch, ch->fighting, TYPE_HIT, FALSE );
		one_hit( ch, ch->fighting, TYPE_HIT, FALSE );
		send_to_char( "Gli artigli si ritraggono.\n\r", ch );
		act( "Gli artigli sull'arma di $n si ritraggono.", ch, NULL, NULL, TO_ROOM, POS_DEAD, WHITE );
		return;
	}
}


void speech_prog_ring_ra( OBJ_DATA *obj, CHAR_DATA *ch, char *speech )
{
	if ( !str_cmp(speech, "furia caeli") && (ch->fighting) && is_equiped_char(ch, obj, WEAR_FINGER) )
	{
		send_to_char( "Un terribile arco di energia si sprigiona dall'anello!\n\r", ch );
		act( "Un terribile arco di energia si sprigiona dall'anello!", ch, NULL, NULL, TO_ROOM, POS_REST );
		obj_cast_spell( gsn_lightning_breath, ch->level, ch, ch->fighting, NULL );
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	}
}


void wear_prog_eyed_sword( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "Un occhio magico si apre su $p.", ch, obj, NULL, TO_CHAR, POS_REST );
	act( "Un occhio magico si apre su $p.", ch, obj, NULL, TO_ROOM, POS_REST );

	if		( ch->level <= 10 )						obj->value[2] = 2;
	else if ( ch->level > 10 && ch->level <= 20 )	obj->value[2] = 3;
	else if ( ch->level > 20 && ch->level <= 30 )	obj->value[2] = 4;
	else if ( ch->level > 30 && ch->level <= 40 )	obj->value[2] = 5;
	else if ( ch->level > 40 && ch->level <= 50 )	obj->value[2] = 6;
	else if ( ch->level > 50 && ch->level <= 60 )	obj->value[2] = 7;
	else if ( ch->level > 60 && ch->level <= 70 )	obj->value[2] = 9;
	else if ( ch->level > 70 && ch->level <= 80 )	obj->value[2] = 11;
	else											obj->value[2] = 12;

	obj->level = ch->level;
}


void wear_prog_katana_sword( OBJ_DATA *obj, CHAR_DATA *ch )
{
	obj->value[2] = 2;
	if ( obj->item_type == ITEM_WEAPON && IS_WEAPON_STAT(obj, WEAPON_KATANA)
	&&	strstr(obj->extra_descr->description, ch->name) != NULL )
	{
		if		( ch->level <= 10 )						obj->value[2] = 2;
		else if ( ch->level > 10 && ch->level <= 20 )	obj->value[2] = 3;
		else if ( ch->level > 20 && ch->level <= 30 )	obj->value[2] = 4;
		else if ( ch->level > 30 && ch->level <= 40 )	obj->value[2] = 5;
		else if ( ch->level > 40 && ch->level <= 50 )	obj->value[2] = 6;
		else if ( ch->level > 50 && ch->level <= 60 )	obj->value[2] = 7;
		else if ( ch->level > 60 && ch->level <= 70 )	obj->value[2] = 9;
		else if ( ch->level > 70 && ch->level <= 80 )	obj->value[2] = 11;
		else											obj->value[2] = 12;

		obj->level = ch->level;
		send_to_char( "La tua katana e' una cosa sola con il tuo corpo!\n\r", ch );
	}
}


void fight_prog_tattoo_goktengri( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( get_eq_char(ch, WEAR_TATTOO) == obj )
	{
		switch ( number_bits(4) )
		{
		case 0:
		case 1:
			act( "$CIl tuo tatuaggio risplende di bianco.$c",
				ch, NULL, NULL, TO_CHAR, POS_DEAD, WHITE );
			do_say( ch, "Non c'e' vita senza Onore!" );
			one_hit( ch, ch->fighting, TYPE_UNDEFINED, FALSE );
			break;
		}
	}
}


void wear_prog_snake( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "$CI serpenti della frusta sprigionano vapori velenosi.$c",
		ch, obj, NULL, TO_CHAR, POS_DEAD, GREEN );
	act( "$CI serpenti della frusta sprigionano vapori velenosi.$c",
		ch, obj, NULL, TO_ROOM, POS_DEAD, GREEN );

	if		( ch->level > 20 && ch->level <= 30 )	obj->value[2] = 4;
	else if ( ch->level > 30 && ch->level <= 40 )	obj->value[2] = 5;
	else if ( ch->level > 40 && ch->level <= 50 )	obj->value[2] = 6;
	else if ( ch->level > 50 && ch->level <= 60 )	obj->value[2] = 7;
	else if ( ch->level > 60 && ch->level <= 70 )	obj->value[2] = 9;
	else if ( ch->level > 70 && ch->level <= 80 )	obj->value[2] = 11;
	else											obj->value[2] = 12;
}


void remove_prog_snake( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "$CI serpenti sulla frusta diventano di pietra.$c",
		ch, obj, NULL, TO_CHAR, POS_DEAD, RED );
	act( "$CI serpenti sulla frusta diventano di pietra.$c",
		ch, obj, NULL, TO_ROOM, POS_DEAD, RED );
}


void get_prog_snake( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "Noti che uno dei serpenti della frusta si e' mosso.", ch, obj, NULL, TO_CHAR, POS_REST );
}


void wear_prog_fire_shield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( strstr(obj->extra_descr->description, "cold") != NULL )
	{
		if ( !is_affected(ch, gsn_fire_shield) )
		{
			send_to_char( "Una calda aura si sprigiona dallo scudo avvolgendoti.\n\r", ch );

			af.where = TO_RESIST;
			af.type = gsn_fire_shield;
			af.duration = -2;
			af.level = ch->level;
			af.bitvector = RES_COLD;
			af.location = 0;
			af.modifier = 0;
			affect_to_char( ch, &af );
		}
	}
	else
	{
		if ( !is_affected(ch, gsn_fire_shield) )
		{
			send_to_char( "Una fredda aura glaciale si sprigiona dallo scudo avvolgendoti.\n\r", ch );

			af.where = TO_RESIST;
			af.type = gsn_fire_shield;
			af.duration = -2;
			af.level = ch->level;
			af.bitvector = RES_FIRE;
			af.location = 0;
			af.modifier = 0;
			affect_to_char( ch, &af );
		}
	}
}


void remove_prog_fire_shield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_fire_shield) )
	{
		affect_strip( ch, gsn_fire_shield );
		if ( strstr(obj->extra_descr->description, "cold") != NULL )
			send_to_char( "La calda aura dello scudo ti abbandona.\n\r", ch );
		else
			send_to_char( "La fredda aura dello scudo ti abbandona.\n\r", ch );
	}
}


void wear_prog_quest_weapon( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( strstr(obj->short_descr, ch->name) != NULL )
	{
		act( "$CUn bagliore illumina la tua arma.$c",
			ch, NULL, NULL, TO_CHAR, POS_REST, BLUE );

		if		( ch->level > 20 && ch->level <= 30 )	obj->value[2] = 4;
		else if ( ch->level > 30 && ch->level <= 40 )	obj->value[2] = 5;
		else if ( ch->level > 40 && ch->level <= 50 )	obj->value[2] = 6;
		else if ( ch->level > 50 && ch->level <= 60 )	obj->value[2] = 7;
		else if ( ch->level > 60 && ch->level <= 70 )	obj->value[2] = 9;
		else if ( ch->level > 70 && ch->level <= 80 )	obj->value[2] = 11;
		else											obj->value[2] = 12;

		obj->level = ch->level;
		return;
	}

	act( "Cerchi di raccogliere $p ma ti rifiuta!", ch, obj, NULL, TO_CHAR, POS_REST );

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
}


void get_prog_quest_reward( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( strstr(obj->short_descr, ch->name) != NULL )
	{
		act( "$C$p si illumina.\n\r$c", ch, obj, NULL, TO_CHAR, POS_SLEEP, BLUE );
		return;
	}

	act( "Cerchi di raccogliere $p ma ti rifiuta!", ch, obj, NULL, TO_CHAR, POS_REST );

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
}


void fight_prog_ancient_gloves( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int dam;
	/*~~~~*/

	if ( get_eq_char(ch, WEAR_HANDS) != obj || get_wield_char(ch, FALSE) != NULL )
		return;

	if ( number_percent() < 20 )
	{
		dam = number_percent() + dice( ch->level, 14 );
		act( "Appena sfiori $N le fiamme sui tuoi guanti ardono piu' intensamente bruciandol$X!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Appena $n sfiora $N i suoi guanti ardono piu' intensamente bruciandol$X!", ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
		act( "Appena $n ti sfiora i suoi guanti ardono piu' intensamente bruciandoti!", ch->fighting, NULL, ch, TO_CHAR, POS_REST );
		fire_effect( ch->fighting, obj->level, dam, TARGET_CHAR );
		damage( ch, ch->fighting, dam, gsn_burning_hands, DAM_FIRE, TRUE );
	}
}


void remove_prog_ancient_gloves( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Le fiamme che avvolgono i tuoi guanti si diradano.\n\r", ch );
}


void wear_prog_ancient_gloves( OBJ_DATA *obj, CHAR_DATA *ch )
{
	send_to_char( "Violente fiamme avvolgono i tuoi guanti!\n\r", ch );
}


void fight_prog_ancient_shield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	int chance;
	int dam;
	/*~~~~~~~*/

	if ( get_shield_char(ch) != obj )
		return;

	if ( (chance = number_percent()) < 5 )
	{
		dam = dice( ch->level, 20 );
		act( "Un delicato bagliore attraversa il tuo scudo!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Uno strano bagliore attraversa lo scudo di $n!", ch, NULL, ch->fighting, TO_VICT, POS_REST );
		act( "Uno strano bagliore attraversa lo scudo di $n!", ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
		fire_effect( ch->fighting, obj->level / 2, dam, TARGET_CHAR );
		damage( ch, ch->fighting, dam, gsn_fire_breath, DAM_FIRE, TRUE );
	}
	else if ( chance < 10 )
	{
		act( "Un forte bagliore scarlatto attraversa il tuo scudo!", ch, NULL, ch->fighting, TO_CHAR, POS_REST );
		act( "Un forte bagliore scarlatto attraversa lo scudo di $n!", ch, NULL, ch->fighting, TO_VICT, POS_REST );
		act( "Un forte bagliore scarlatto attraversa lo scudo di $n!", ch, NULL, ch->fighting, TO_NOTVICT, POS_REST );
		obj_cast_spell( gsn_blindness, ch->level + 5, ch, ch->fighting, obj );
		obj_cast_spell( gsn_slow, ch->level + 5, ch, ch->fighting, obj );
	}
}


void remove_prog_ancient_shield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "$CIl tuo scudo riacquista la forma originaria.$c", ch, obj, NULL, TO_CHAR, POS_DEAD, RED );
	act( "$CIl tuo scudo riacquista la forma originaria.$c", ch, obj, NULL, TO_ROOM, POS_DEAD, RED );
	return;
}


void wear_prog_ancient_shield( OBJ_DATA *obj, CHAR_DATA *ch )
{
	act( "$CLa superficie del tuo scudo si ricopre di scaglie di argentate.\n\r"
		"La testa di un drago compare al centro dello scudo spalancando le fauci!$c",
		ch, obj, NULL, TO_CHAR, POS_DEAD, RED );
	act( "$CLo scudo di $n si ricopre di squame argentate!\n\r"
		"Al centro dello scudo compare la testa di un drago che spalanca minaccioso le fauci!$c",
		ch, obj, NULL, TO_ROOM, POS_DEAD, RED );
}


void wear_prog_neckguard( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_neckguard) )
	{
		af.where = TO_AFFECTS;
		af.type = gsn_neckguard;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
}


void remove_prog_neckguard( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_neckguard) )
		affect_strip( ch, gsn_neckguard );
}


void wear_prog_headguard( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_headguard) )
	{
		af.where = TO_AFFECTS;
		af.type = gsn_headguard;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
}


void remove_prog_headguard( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_headguard) )
		affect_strip( ch, gsn_headguard );
}


void wear_prog_blackguard( OBJ_DATA *obj, CHAR_DATA *ch )
{
	AFFECT_DATA af;
	/*~~~~~~~~~~~*/

	if ( !is_affected(ch, gsn_blackguard) )
	{
		af.where = TO_AFFECTS;
		af.type = gsn_blackguard;
		af.duration = -2;
		af.level = ch->level;
		af.bitvector = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		affect_to_char( ch, &af );
	}
}


void remove_prog_blackguard( OBJ_DATA *obj, CHAR_DATA *ch )
{
	if ( is_affected(ch, gsn_blackguard) )
		affect_strip( ch, gsn_blackguard );
}
