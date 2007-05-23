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


#include <sys/dir.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "xandra.h"
#include "db.h"
#include "recycle.h"
#include "file.h"


/*
 * Funzioni locali.
 */
void	load_limited_objects	( void );



#if !defined( OLD_RAND )
int			getpid( void );
/*time_t		time( time_t *tloc ); (TT) */
#endif


/*
 * Externals for counting purposes
 */
extern OBJ_DATA			*obj_free;
extern CHAR_DATA		*char_free;
extern DESCRIPTOR_DATA	*descriptor_free;
extern PC_DATA			*pcdata_free;
extern AFFECT_DATA		*affect_free;


/*
 * Globals.
 */
HELP_DATA				*help_first;
HELP_DATA				*help_last;

SHOP_DATA				*shop_first;
SHOP_DATA				*shop_last;

NOTE_DATA				*note_free;

char					bug_buf[2 * MIL];
CHAR_DATA				*char_list;
char					*help_greeting;
char					log_buf[2 * MIL];
KILL_DATA				kill_table[MAX_LEVEL];
NOTE_DATA				*note_list;
OBJ_DATA				*object_list;
TIME_INFO_DATA			time_info;
WEATHER_DATA			weather_info;

AUCTION_DATA			*auction;
ROOM_INDEX_DATA			*top_affected_room;

int						reboot_counter;
int						time_sync;
int						max_newbies;
int						max_oldies;
int						iNumPlayers;

struct social_type social_table[MAX_SOCIALS];
int social_count;


/* for limited objects */
long					total_levels;

sh_int					gsn_backstab;
sh_int					gsn_dodge;
sh_int					gsn_envenom;
sh_int					gsn_hide;
sh_int					gsn_peek;
sh_int					gsn_pick_lock;
sh_int					gsn_sneak;
sh_int					gsn_steal;

sh_int					gsn_disarm;
sh_int					gsn_enhanced_damage;
sh_int					gsn_kick;
sh_int					gsn_parry;
sh_int					gsn_rescue;
sh_int					gsn_second_attack;
sh_int					gsn_third_attack;

sh_int					gsn_blindness;
sh_int					gsn_charm_person;
sh_int					gsn_curse;
sh_int					gsn_remove_curse;
sh_int					gsn_invis;
sh_int					gsn_mass_invis;

sh_int					gsn_poison;
sh_int					gsn_plague;
sh_int					gsn_sleep;
sh_int					gsn_sanctuary;
sh_int					gsn_fly;
sh_int					gsn_death_protection;

sh_int					gsn_fourth_attack;
sh_int					gsn_dual_backstab;
sh_int					gsn_cleave;
sh_int					gsn_counter;
sh_int					gsn_ambush;
sh_int					gsn_camouflage;
sh_int					gsn_circle;
sh_int					gsn_nerve;
sh_int					gsn_endure;
sh_int					gsn_quiet_movement;
sh_int					gsn_herbs;
sh_int					gsn_tame;
sh_int					gsn_butcher;
sh_int					gsn_assassinate;
sh_int					gsn_wanted;
sh_int					gsn_caltraps;
sh_int					gsn_throw;
sh_int					gsn_strangle;
sh_int					gsn_blackjack;
sh_int					gsn_bloodthirst;
sh_int					gsn_spellbane;
sh_int					gsn_resistance;
sh_int					gsn_deathblow;
sh_int					gsn_doppelganger;
sh_int					gsn_fade;
sh_int					gsn_garble;
sh_int					gsn_confuse;
sh_int					gsn_mirror;
sh_int					gsn_track;
sh_int					gsn_vanish;
sh_int					gsn_chaos_blade;
sh_int					gsn_terangreal;
sh_int					gsn_dispel_evil;
sh_int					gsn_dispel_good;
sh_int					gsn_wrath;
sh_int					gsn_stalker;
sh_int					gsn_tesseract;
sh_int					gsn_randomizer;
sh_int					gsn_trophy;
sh_int					gsn_truesight;
sh_int					gsn_brew;
sh_int					gsn_flamestrike;
sh_int					gsn_shadowlife;
sh_int					gsn_ruler_badge;
sh_int					gsn_remove_badge;
sh_int					gsn_dragon_strength;
sh_int					gsn_dragon_breath;
sh_int					gsn_warcry;
sh_int					gsn_entangle;
sh_int					gsn_dragonsword;
sh_int					gsn_knight;
sh_int					gsn_guard;
sh_int					gsn_guard_call;
sh_int					gsn_love_potion;
sh_int					gsn_deafen;
sh_int					gsn_protective_shield;
sh_int					gsn_protection_heat_cold;
sh_int					gsn_trance;
sh_int					gsn_demon_summon;
sh_int					gsn_nightwalker;
sh_int					gsn_bear_call;
sh_int					gsn_squire;
sh_int					gsn_lightning_bolt;
sh_int					gsn_disperse;
sh_int					gsn_bless;
sh_int					gsn_weaken;
sh_int					gsn_haste;
sh_int					gsn_cure_critical;
sh_int					gsn_cure_serious;
sh_int					gsn_burning_hands;
sh_int					gsn_acid_blast;
sh_int					gsn_ray_of_truth;
sh_int					gsn_spell_craft;
sh_int					gsn_giant_strength;
sh_int					gsn_explode;
sh_int					gsn_acid_breath;
sh_int					gsn_fire_breath;
sh_int					gsn_frost_breath;
sh_int					gsn_gas_breath;
sh_int					gsn_lightning_breath;
sh_int					gsn_cure_light;
sh_int					gsn_magic_missile;
sh_int					gsn_demonfire;
sh_int					gsn_faerie_fire;
sh_int					gsn_shield;
sh_int					gsn_chill_touch;
sh_int					gsn_second_weapon;
sh_int					gsn_target;
sh_int					gsn_sand_storm;
sh_int					gsn_scream;
sh_int					gsn_enchant_sword;
sh_int					gsn_tiger_power;
sh_int					gsn_lion_call;
sh_int					gsn_hara_kiri;
sh_int					gsn_mouse;
sh_int					gsn_enhanced_armor;
sh_int					gsn_vampire;
sh_int					gsn_vampiric_bite;
sh_int					gsn_light_res;
sh_int					gsn_blink;
sh_int					gsn_hunt;
sh_int					gsn_path_find;
sh_int					gsn_critical;
sh_int					gsn_detect_sneak;
sh_int					gsn_mend;
sh_int					gsn_shielding;
sh_int					gsn_blind_fighting;
sh_int					gsn_riding;
sh_int					gsn_smithing;
sh_int					gsn_swimming;
sh_int					gsn_move_camf;
sh_int					gsn_protection_heat;
sh_int					gsn_protection_cold;
sh_int					gsn_teleport;
sh_int					gsn_witch_curse;
sh_int					gsn_kassandra;
sh_int					gsn_sebat;
sh_int					gsn_matandra;
sh_int					gsn_armor_use;
sh_int					gsn_world_find;
sh_int					gsn_vampiric_touch;
sh_int					gsn_cure_poison;
sh_int					gsn_fire_shield;
sh_int					gsn_fear;
sh_int					gsn_settraps;
sh_int					gsn_mental_attack;
sh_int					gsn_secondary_attack;
sh_int					gsn_mortal_strike;
sh_int					gsn_shield_cleave;
sh_int					gsn_weapon_cleave;
sh_int					gsn_slow;
sh_int					gsn_imp_invis;
sh_int					gsn_camp;
sh_int					gsn_push;
sh_int					gsn_tail;
sh_int					gsn_power_stun;
sh_int					gsn_cabal_recall;
sh_int					gsn_escape;
sh_int					gsn_lay_hands;
sh_int					gsn_grip;
sh_int					gsn_concentrate;
sh_int					gsn_master_sword;
sh_int					gsn_master_hand;
sh_int					gsn_fifth_attack;
sh_int					gsn_area_attack;
sh_int					gsn_reserved;
sh_int					gsn_bandage;
sh_int					gsn_web;
sh_int					gsn_bow;
sh_int					gsn_bash_door;
sh_int					gsn_katana;
sh_int					gsn_bluefire;
sh_int					gsn_crush;
sh_int					gsn_perception;
sh_int					gsn_control_animal;
sh_int					gsn_sanctify_lands;
sh_int					gsn_deadly_venom;
sh_int					gsn_cursed_lands;
sh_int					gsn_lethargic_mist;
sh_int					gsn_black_death;
sh_int					gsn_mysterious_dream;
sh_int					gsn_sense_life;
sh_int					gsn_arrow;
sh_int					gsn_lance;
sh_int					gsn_evil_spirit;
sh_int					gsn_make_arrow;
sh_int					gsn_green_arrow;
sh_int					gsn_red_arrow;
sh_int					gsn_white_arrow;
sh_int					gsn_blue_arrow;
sh_int					gsn_make_bow;
sh_int					gsn_blindness_dust;
sh_int					gsn_poison_smoke;
sh_int					gsn_mastering_spell;
sh_int					gsn_judge;
sh_int					gsn_hand_block;
sh_int					gsn_cross_block;
sh_int					gsn_ground_strike;
sh_int					gsn_neckguard;
sh_int					gsn_lash;
sh_int					gsn_claw;
sh_int					gsn_absorb;
sh_int					gsn_earthfade;
sh_int					gsn_headguard;
sh_int					gsn_blackguard;
sh_int					gsn_mental_knife;

/* new_gsns */
sh_int					gsn_axe;
sh_int					gsn_dagger;
sh_int					gsn_flail;
sh_int					gsn_mace;
sh_int					gsn_polearm;
sh_int					gsn_shield_block;
sh_int					gsn_spear;
sh_int					gsn_sword;
sh_int					gsn_whip;

sh_int					gsn_bash;
sh_int					gsn_berserk;
sh_int					gsn_dirt;
sh_int					gsn_hand_to_hand;
sh_int					gsn_trip;

sh_int					gsn_fast_healing;
sh_int					gsn_haggle;
sh_int					gsn_lore;
sh_int					gsn_meditation;

sh_int					gsn_scrolls;
sh_int					gsn_staves;
sh_int					gsn_wands;
sh_int					gsn_recall;
sh_int					gsn_detect_hidden;
sh_int					gsn_purify;
sh_int					gsn_sharpen;


/*
 * Locals.
 */
MOB_INDEX_DATA			*mob_index_hash[MAX_KEY_HASH];
OBJ_INDEX_DATA			*obj_index_hash[MAX_KEY_HASH];
ROOM_INDEX_DATA			*room_index_hash[MAX_KEY_HASH];
char					*string_hash[MAX_KEY_HASH];

AREA_DATA				*area_first;

AREA_DATA				*area_last;

char					*string_space;

char					*top_string;
char					str_empty[1];

int						top_affect;
int						top_area;
int						top_ed;
int						top_exit;
int						top_help;
int						top_mob_index;
int						top_obj_index;
int						top_reset;
int						top_room;
int						top_shop;
int						mobile_count = 0;
int						newmobs = 0;
int						newobjs = 0;


/*
 * Memory management.
 * Increase MAX_STRING if you have too. Tune the others only if you understand what you're doing.
 */
#define MAX_STRING		3500000
#define MAX_PERM_BLOCK	150000
#define MAX_MEM_LIST	11

void					*rgFreeList[MAX_MEM_LIST];
const int				rgSizeList[MAX_MEM_LIST] = { 16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768 - 64 };

int						nAllocString;
int						sAllocString;
int						nAllocPerm;
int						sAllocPerm;


/*
 * Semi-locals.
 */
bool					fBootDb;
FILE					*fpArea;
char					strArea[MIL];
AREA_DATA				*Serarea;	/* currently read area */


/*
 * Local booting procedures.
 */
void init_mm			( void );
void load_area			( FILE *fp );
void load_helps			( FILE *fp );
void load_omprogs		( FILE *fp );
void load_mobiles		( FILE *fp );
void load_objects		( FILE *fp );
void load_resets		( FILE *fp );
void load_rooms			( FILE *fp );
void load_shops			( FILE *fp );
void load_socials		( FILE *fp );
void load_specials		( FILE *fp );
void load_notes			( void );
void load_bans			( void );
void load_olimits		( FILE *fp );
void load_practicer		( FILE *fp );
void load_resetmsg		( FILE *fp );
void load_aflag			( FILE *fp );

void fix_exits			( void );

void reset_area			( AREA_DATA *pArea );


/*
 * Big mama top level function.
 */
void boot_db( void )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Init some data space stuff. */
	{
		if ( (string_space = calloc(1, MAX_STRING)) == NULL )
		{
			bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
			exit( 1 );
		}

		top_string = string_space;
		fBootDb = TRUE;
	}

	/* Init random number generator. */
	{
		init_mm();
	}

	/* Set time and weather. */
	{
		long	lhour, lday, lmonth;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		lhour = ( current_time - 650336715 ) / ( PULSE_TICK / PULSE_PER_SCD );
		time_info.hour = lhour % 24;
		lday = lhour / 24;
		time_info.day = lday % 35;
		lmonth = lday / 35;
		time_info.month = lmonth % 17;
		time_info.year = lmonth / 17;

		if		( time_info.hour < 5 )		weather_info.sunlight = SUN_DARK;
		else if ( time_info.hour < 6 )		weather_info.sunlight = SUN_RISE;
		else if ( time_info.hour < 19 )		weather_info.sunlight = SUN_LIGHT;
		else if ( time_info.hour < 20 )		weather_info.sunlight = SUN_SET;
		else								weather_info.sunlight = SUN_DARK;

		weather_info.change = 0;
#ifdef WEATHER
		weather_info.sky = SKY_CLOUDLESS;
#else
		weather_info.mmhg = 960;
		if ( time_info.month >= 7 && time_info.month <= 12 )
			weather_info.mmhg += number_range( 1, 50 );
		else
			weather_info.mmhg += number_range( 1, 80 );

		if		( weather_info.mmhg <= 980 )	weather_info.sky = SKY_LIGHTNING;
		else if ( weather_info.mmhg <= 1000 )	weather_info.sky = SKY_RAINING;
		else if ( weather_info.mmhg <= 1020 )	weather_info.sky = SKY_CLOUDY;
		else									weather_info.sky = SKY_CLOUDLESS;
#endif
	}

	/* auction */
	auction = (AUCTION_DATA *) malloc( sizeof(AUCTION_DATA) );
	if ( auction == NULL )
	{
		bug( "malloc'ing AUCTION_DATA didn't give %d bytes", sizeof(AUCTION_DATA) );
		exit( 1 );
	}

	auction->item = NULL;	/* nothing is being sold */

	/* room_affect_data */
	top_affected_room = NULL;

	/* reboot counter */
	reboot_counter = -1;	/* no default reboot */
	time_sync = 0;			/* time_sync is not set */

	max_newbies = MAX_NEWBIES;
	max_oldies = MAX_OLDIES;
	iNumPlayers = 0;

	/* Assign gsn's for skills which have them. */
	{
		int sn;
		/*~~~*/

		for ( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if ( skill_table[sn].pgsn != NULL )

				*skill_table[sn].pgsn = sn;
		}
	}

	/* Read in all the area files. */
	{
		FILE	*fpList;
		/*~~~~~~~~~~~~*/

		if ( (fpList = fopen(AREA_LIST, "r")) == NULL )
		{
			perror( AREA_LIST );
			exit( 1 );
		}

		for ( ;; )
		{
			strcpy( strArea, fread_word(fpList) );
			if ( strArea[0] == '$' )
				break;

			if ( strArea[0] == '-' )
			{
				fpArea = stdin;
			}
			else
			{
				sprintf( buf, "%s%s", AREA_DIR, strArea );
				if ( (fpArea = fopen(buf, "r")) == NULL )
				{
					perror( strArea );
					exit( 1 );
				}
			}

			for ( ;; )
			{
				char	*word;
				/*~~~~~~~~~~*/

				if ( fread_letter(fpArea) != '#' )
				{
					bug( "Boot_db: # not found.", 0 );
					exit( 1 );
				}

				word = fread_word( fpArea );

				if ( word[0] == '$' )							break;
				else if ( !str_cmp(word, "AREA") )				load_area( fpArea );
				else if ( !str_cmp(word, "HELPS") )				load_helps( fpArea );
				else if ( !str_cmp(word, "MOBILES") )			load_mobiles( fpArea );
				else if ( !str_cmp(word, "OBJECTS") )			load_objects( fpArea );
				else if ( !str_cmp(word, "RESETS") )			load_resets( fpArea );
				else if ( !str_cmp(word, "ROOMS") )				load_rooms( fpArea );
				else if ( !str_cmp(word, "SHOPS") )				load_shops( fpArea );
				else if ( !str_cmp(word, "SOCIALS") )			load_socials( fpArea );
				else if ( !str_cmp(word, "OMPROGS") )			load_omprogs( fpArea );
				else if ( !str_cmp(word, "OLIMITS") )			load_olimits( fpArea );
				else if ( !str_cmp(word, "SPECIALS") )			load_specials( fpArea );
				else if ( !str_cmp(word, "PRACTICERS") )		load_practicer( fpArea );
				else if ( !str_cmp(word, "RESETMESSAGE") )		load_resetmsg( fpArea );
				else if ( !str_cmp(word, "FLAG") )				load_aflag( fpArea );
				else
				{
					bug( "Boot_db: bad section name.", 0 );
					exit( 1 );
				}
			}

			if ( fpArea != stdin )
				fclose( fpArea );
			fpArea = NULL;
		}

		fclose( fpList );
		Serarea = NULL;
	}

	/* Fix up exits. Declare db booting over. Reset all areas once. Load up the songs, notes and ban files. */
	{

		fix_exits();
		load_limited_objects();
		sprintf( buf, "Total non-immortal levels > 5: %li", total_levels );
		log_string( buf );

		fBootDb = FALSE;
		area_update();
		load_notes();
		load_bans();
	}
}


/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
	AREA_DATA	*pArea;
	/*~~~~~~~~~~~~~~~*/

	pArea = alloc_perm( sizeof(*pArea) );
	pArea->reset_first = NULL;
	pArea->reset_last = NULL;
	pArea->file_name = fread_string( fp );
	pArea->name = fread_string( fp );
	fread_letter( fp );
	pArea->low_range = fread_number( fp );
	pArea->high_range = fread_number( fp );
	fread_letter( fp );
	pArea->writer = str_dup( fread_word(fp) );
	pArea->credits = fread_string( fp );
	pArea->min_vnum = fread_number( fp );
	pArea->max_vnum = fread_number( fp );
	pArea->age = 15;
	pArea->nplayer = 0;
	pArea->empty = FALSE;
	pArea->count = 0;
	pArea->resetmsg = NULL;
	pArea->area_flag = 0;

	if ( area_first == NULL )
		area_first = pArea;
	if ( area_last != NULL )
		area_last->next = pArea;
	area_last = pArea;
	pArea->next = NULL;
	Serarea = pArea;

	top_area++;
}


/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
	HELP_DATA	*pHelp;
	/*~~~~~~~~~~~~~~~*/

	for ( ;; )
	{
		pHelp = alloc_perm( sizeof(*pHelp) );
		pHelp->level = fread_number( fp );
		pHelp->keyword = fread_string( fp );
		if ( pHelp->keyword[0] == '$' )
			break;
		pHelp->text = fread_string( fp );

		if ( !str_cmp(pHelp->keyword, "greeting") )
			help_greeting = pHelp->text;

		if ( help_first == NULL )
			help_first = pHelp;
		if ( help_last != NULL )
			help_last->next = pHelp;

		help_last = pHelp;
		pHelp->next = NULL;
		top_help++;
	}
}


/*
 * Snarf a mob section.
 */
void load_mobiles( FILE *fp )
{
	MOB_INDEX_DATA	*pMobIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( ;; )
	{
		sh_int	vnum;
		char	letter;
		int		iHash;
		/*~~~~~~~~~~~*/


		letter = fread_letter( fp );
		if ( letter != '#' )
		{
			bug( "Load_mobiles: # not found.", 0 );

			exit( 1 );
		}

		vnum = fread_number( fp );
		if ( vnum == 0 )
			break;

		fBootDb = FALSE;
		if ( get_mob_index(vnum) != NULL )
		{
			bug( "Load_mobiles: vnum %d duplicated.", vnum );
			exit( 1 );
		}

		fBootDb = TRUE;

		pMobIndex = alloc_perm( sizeof(*pMobIndex) );
		pMobIndex->vnum = vnum;
		pMobIndex->new_format = TRUE;
		newmobs++;
		pMobIndex->player_name = fread_string( fp );
		pMobIndex->short_descr = fread_string( fp );
		pMobIndex->long_descr = fread_string( fp );
		pMobIndex->description = fread_string( fp );
		pMobIndex->race = race_lookup( fread_string(fp) );

		/* Toglie \n\r in piu' nelle long_descr */
		if ( pMobIndex->long_descr[strlen(pMobIndex->long_descr) - 2] == '\n' )
			pMobIndex->long_descr[strlen( pMobIndex->long_descr ) - 2] = '\0';

		pMobIndex->long_descr[0] = UPPER( pMobIndex->long_descr[0] );
		pMobIndex->description[0] = UPPER( pMobIndex->description[0] );

		pMobIndex->act = fread_flag( fp ) | ACT_IS_MOB | race_table[pMobIndex->race].act;

		pMobIndex->affected_by = fread_flag( fp ) | race_table[pMobIndex->race].aff;
		pMobIndex->practicer = 0;
		pMobIndex->detection = race_table[pMobIndex->race].det;

		/* chronos smashed affection of ROM and created detection of ANATOLIA */
		if ( IS_AFFECTED(pMobIndex, C) )	SET_BIT( pMobIndex->detection, C );		/* detect evil */
		if ( IS_AFFECTED(pMobIndex, D) )	SET_BIT( pMobIndex->detection, D );		/* detect invis */
		if ( IS_AFFECTED(pMobIndex, E) )	SET_BIT( pMobIndex->detection, E );		/* detect magic */
		if ( IS_AFFECTED(pMobIndex, F) )	SET_BIT( pMobIndex->detection, F );		/* detect hidden */
		if ( IS_AFFECTED(pMobIndex, G) )	SET_BIT( pMobIndex->detection, G );		/* detect good */
		if ( IS_AFFECTED(pMobIndex, Z) )	SET_BIT( pMobIndex->detection, Z );		/* dark vision */
		if ( IS_AFFECTED(pMobIndex, ee) )	SET_BIT( pMobIndex->detection, ee );	/* acute vision */
		REMOVE_BIT( pMobIndex->affected_by, (C | D | E | F | G | Z | ee) );

		pMobIndex->pShop = NULL;
		pMobIndex->alignment = fread_number( fp );
		pMobIndex->group = fread_number( fp );

		pMobIndex->level = fread_number( fp );
		pMobIndex->hitroll = fread_number( fp );

		/* read hit dice */
		pMobIndex->hit[DICE_NUMBER] = fread_number( fp );

		/* 'd' */
		fread_letter( fp );
		pMobIndex->hit[DICE_TYPE] = fread_number( fp );

		/* '+' */
		fread_letter( fp );
		pMobIndex->hit[DICE_BONUS] = fread_number( fp );

		/* read mana dice */
		pMobIndex->mana[DICE_NUMBER] = fread_number( fp );
		fread_letter( fp );
		pMobIndex->mana[DICE_TYPE] = fread_number( fp );
		fread_letter( fp );
		pMobIndex->mana[DICE_BONUS] = fread_number( fp );

		/* read damage dice */
		pMobIndex->damage[DICE_NUMBER] = fread_number( fp );
		fread_letter( fp );
		pMobIndex->damage[DICE_TYPE] = fread_number( fp );
		fread_letter( fp );
		pMobIndex->damage[DICE_BONUS] = fread_number( fp );
		pMobIndex->dam_type = attack_lookup( fread_word(fp) );

		/* read armor class */
		pMobIndex->ac[AC_PIERCE] = fread_number( fp ) * 10;

		pMobIndex->ac[AC_BASH] = fread_number( fp ) * 10;
		pMobIndex->ac[AC_SLASH] = fread_number( fp ) * 10;
		pMobIndex->ac[AC_EXOTIC] = fread_number( fp ) * 10;

		/* read flags and add in data from the race table */
		pMobIndex->off_flags = fread_flag( fp ) | race_table[pMobIndex->race].off;
		pMobIndex->imm_flags = fread_flag( fp ) | race_table[pMobIndex->race].imm;
		pMobIndex->res_flags = fread_flag( fp ) | race_table[pMobIndex->race].res;
		pMobIndex->vuln_flags = fread_flag( fp ) | race_table[pMobIndex->race].vuln;

		/* vital statistics */
		pMobIndex->start_pos = position_lookup( fread_word(fp) );
		pMobIndex->default_pos = position_lookup( fread_word(fp) );
		pMobIndex->sex = sex_lookup( fread_word(fp) );

		pMobIndex->wealth = fread_number( fp );
		pMobIndex->wealth = number_range( pMobIndex->wealth / 3, 3 * pMobIndex->wealth / 2 );	/* modifica la wealth e la rende causale */

		pMobIndex->form = fread_flag( fp ) | race_table[pMobIndex->race].form;
		pMobIndex->parts = fread_flag( fp ) | race_table[pMobIndex->race].parts;

		/* size */
		pMobIndex->size = size_lookup( fread_word(fp) );

		pMobIndex->material = str_dup( fread_word(fp) );
		/* (TT) serve a controllare se nelle aree ci sono stringhe-materiale sconosciute */
/*		{
			int			x;
			bool		found = FALSE;
			char		buf[MIL];

			for ( x = 0; material_table[x].name != NULL; x++ )
			{
				if ( !str_cmp(pMobIndex->material, material_table[x].name) )
					found = TRUE;
			}

			if ( !found && !strcmp(pMobIndex->material, "") && strcmp(pMobIndex->material, "unknown") && strcmp(pMobIndex->material, "oldstyle") )
			{
				sprintf( buf, "Materiale non trovato %s, al mob numero %d",
					pMobIndex->material, pMobIndex->vnum );
				bug( buf, 0 );
			}
		}
*/

		pMobIndex->mprogs = NULL;
		pMobIndex->progtypes = 0;

		for ( ;; )
		{
			letter = fread_letter( fp );

			if ( letter == 'F' )
			{
				char	*word;
				long	vector;
				/*~~~~~~~~~~~*/

				word = fread_word( fp );
				vector = fread_flag( fp );

				if		( !str_prefix(word, "act") )	REMOVE_BIT( pMobIndex->act, vector );
				else if ( !str_prefix(word, "aff") )	REMOVE_BIT( pMobIndex->affected_by, vector );
				else if ( !str_prefix(word, "off") )	REMOVE_BIT( pMobIndex->affected_by, vector );
				else if ( !str_prefix(word, "imm") )	REMOVE_BIT( pMobIndex->imm_flags, vector );
				else if ( !str_prefix(word, "res") )	REMOVE_BIT( pMobIndex->res_flags, vector );
				else if ( !str_prefix(word, "vul") )	REMOVE_BIT( pMobIndex->vuln_flags, vector );
				else if ( !str_prefix(word, "for") )	REMOVE_BIT( pMobIndex->form, vector );
				else if ( !str_prefix(word, "par") )	REMOVE_BIT( pMobIndex->parts, vector );
				else
				{
					bug( "Flag remove: flag not found.", 0 );
					exit( 1 );
				}
			}
			else
			{
				ungetc( letter, fp );
				break;
			}
		}

		iHash = vnum % MAX_KEY_HASH;
		pMobIndex->next = mob_index_hash[iHash];
		mob_index_hash[iHash] = pMobIndex;
		top_mob_index++;
		kill_table[URANGE( 0, pMobIndex->level, MAX_LEVEL - 1 )].number++;
	}
}



/*
 * Snarf an obj section. new style
 */
void load_objects( FILE *fp )
{
	OBJ_INDEX_DATA	*pObjIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( ;; )
	{
		sh_int	vnum;
		char	letter;
		int		iHash;
		/*~~~~~~~~~~~*/

		letter = fread_letter( fp );
		if ( letter != '#' )
		{
			bug( "Load_objects: # not found.", 0 );
			exit( 1 );
		}

		vnum = fread_number( fp );
		if ( vnum == 0 )
			break;

		fBootDb = FALSE;
		if ( get_obj_index(vnum) != NULL )
		{
			bug( "Load_objects: vnum %d duplicated.", vnum );
			exit( 1 );
		}

		fBootDb = TRUE;

		pObjIndex = alloc_perm( sizeof(*pObjIndex) );
		pObjIndex->vnum = vnum;
		pObjIndex->new_format = TRUE;
		pObjIndex->reset_num = 0;
		newobjs++;
		pObjIndex->name = fread_string( fp );
		pObjIndex->short_descr = fread_string( fp );
		pObjIndex->description = fread_string( fp );
		pObjIndex->material = fread_string( fp );
		/* (TT) serve a controllare se nelle aree ci sono stringhe-materiale sconosciute */
/*		{
			int			x;
			bool		found = FALSE;
			char		buf[MIL];

			for ( x = 0; material_table[x].name != NULL; x++ )
			{
				if ( !strcmp(pObjIndex->material, material_table[x].name) )
					found = TRUE;
			}

			if ( !found && strcmp(pObjIndex->material, "") && strcmp(pObjIndex->material, "unknown") )
			{
				sprintf( buf, "Materiale non trovato %s, all'oggetto numero %d",
					pObjIndex->material, pObjIndex->vnum );
				bug( buf, 0 );
			}
		}
*/

		pObjIndex->item_type = item_lookup( fread_word(fp) );
		pObjIndex->extra_flags = fread_flag( fp );
		pObjIndex->wear_flags = fread_flag( fp );
		switch ( pObjIndex->item_type )
		{
		case ITEM_WEAPON:
			pObjIndex->value[0] = weapon_type( fread_word(fp) );
			pObjIndex->value[1] = fread_number( fp );
			pObjIndex->value[2] = fread_number( fp );
			pObjIndex->value[3] = attack_lookup( fread_word(fp) );
			pObjIndex->value[4] = fread_flag( fp );
			break;

		case ITEM_CONTAINER:
			pObjIndex->value[0] = fread_number( fp );
			pObjIndex->value[1] = fread_flag( fp );
			pObjIndex->value[2] = fread_number( fp );
			pObjIndex->value[3] = fread_number( fp );
			pObjIndex->value[4] = fread_number( fp );
			break;

		case ITEM_DRINK_CON:
		case ITEM_FOUNTAIN:
			pObjIndex->value[0] = fread_number( fp );
			pObjIndex->value[1] = fread_number( fp );
			pObjIndex->value[2] = liq_lookup( fread_word(fp) );
			pObjIndex->value[3] = fread_number( fp );
			pObjIndex->value[4] = fread_number( fp );
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			pObjIndex->value[0] = fread_number( fp );
			pObjIndex->value[1] = fread_number( fp );
			pObjIndex->value[2] = fread_number( fp );
			pObjIndex->value[3] = skill_lookup( fread_word(fp) );
			pObjIndex->value[4] = fread_number( fp );
			break;

		case ITEM_POTION:
		case ITEM_PILL:
		case ITEM_SCROLL:
			pObjIndex->value[0] = fread_number( fp );
			pObjIndex->value[1] = skill_lookup( fread_word(fp) );
			pObjIndex->value[2] = skill_lookup( fread_word(fp) );
			pObjIndex->value[3] = skill_lookup( fread_word(fp) );
			pObjIndex->value[4] = skill_lookup( fread_word(fp) );
			break;

		default:
			pObjIndex->value[0] = fread_flag( fp );
			pObjIndex->value[1] = fread_flag( fp );
			pObjIndex->value[2] = fread_flag( fp );
			pObjIndex->value[3] = fread_flag( fp );
			pObjIndex->value[4] = fread_flag( fp );
			break;
		}

		pObjIndex->level = fread_number( fp );
		pObjIndex->weight = fread_number( fp );
		pObjIndex->cost = fread_number( fp );
		pObjIndex->progtypes = 0;
		pObjIndex->oprogs = NULL;
		pObjIndex->limit = -1;

		/* condition */
		letter = fread_letter( fp );
		switch ( letter )
		{
		case ( 'P' ):	pObjIndex->condition = 100; break;
		case ( 'G' ):	pObjIndex->condition = 90; break;
		case ( 'A' ):	pObjIndex->condition = 75; break;
		case ( 'W' ):	pObjIndex->condition = 50; break;
		case ( 'D' ):	pObjIndex->condition = 25; break;
		case ( 'B' ):	pObjIndex->condition = 10; break;
		case ( 'R' ):	pObjIndex->condition = 0; break;
		default:		pObjIndex->condition = 100; break;
		}

		for ( ;; )
		{
			letter = fread_letter( fp );


			if ( letter == 'A' )
			{
				AFFECT_DATA *paf;
				/*~~~~~~~~~~~~~*/

				paf = alloc_perm( sizeof(*paf) );
				paf->where = TO_OBJECT;
				paf->type = -1;
				paf->level = pObjIndex->level;
				paf->duration = -1;
				paf->location = fread_number( fp );
				paf->modifier = fread_number( fp );
				paf->bitvector = 0;
				paf->next = pObjIndex->affected;
				pObjIndex->affected = paf;
				top_affect++;
			}
			else if ( letter == 'F' )
			{
				AFFECT_DATA *paf;
				/*~~~~~~~~~~~~~*/

				paf = alloc_perm( sizeof(*paf) );
				letter = fread_letter( fp );
				switch ( letter )
				{
				case 'A':	paf->where = TO_AFFECTS;	break;
				case 'I':	paf->where = TO_IMMUNE;		break;
				case 'R':	paf->where = TO_RESIST;		break;
				case 'V':	paf->where = TO_VULN;		break;
				case 'D':	paf->where = TO_DETECTS;	break;
				default:	bug( "Load_objects: Bad where on flag set.", 0 ); exit( 1 );
				}

				paf->type = -1;
				paf->level = pObjIndex->level;
				paf->duration = -1;
				paf->location = fread_number( fp );
				paf->modifier = fread_number( fp );
				paf->bitvector = fread_flag( fp );
				paf->next = pObjIndex->affected;
				pObjIndex->affected = paf;
				top_affect++;
			}
			else if ( letter == 'E' )
			{
				EXTRA_DESCR_DATA	*ed;
				/*~~~~~~~~~~~~~~~~~~~~*/

				ed = alloc_perm( sizeof(*ed) );
				ed->keyword = fread_string( fp );
				ed->description = fread_string( fp );
				ed->next = pObjIndex->extra_descr;
				pObjIndex->extra_descr = ed;
				top_ed++;
			}
			else
			{
				ungetc( letter, fp );
				break;
			}
		}

		iHash = vnum % MAX_KEY_HASH;
		pObjIndex->next = obj_index_hash[iHash];
		obj_index_hash[iHash] = pObjIndex;
		top_obj_index++;
	}
}


/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
	RESET_DATA	*pReset;
	/*~~~~~~~~~~~~~~~~*/

	if ( area_last == NULL )
	{
		bug( "Load_resets: no #AREA seen yet.", 0 );
		exit( 1 );
	}

	for ( ;; )
	{
		ROOM_INDEX_DATA *pRoomIndex;
		EXIT_DATA		*pexit;
		char			letter;
		OBJ_INDEX_DATA	*temp_index;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( (letter = fread_letter(fp)) == 'S' )
			break;

		if ( letter == '*' )
		{
			fread_to_eol( fp );
			continue;
		}

		pReset = alloc_perm( sizeof(*pReset) );
		pReset->command = letter;

		/* if_flag */
		fread_number( fp );
		pReset->arg1 = fread_number( fp );
		pReset->arg2 = fread_number( fp );
		pReset->arg3 = ( letter == 'G' || letter == 'R' ) ? 0 : fread_number( fp );
		pReset->arg4 = ( letter == 'P' || letter == 'M' ) ? fread_number( fp ) : 0;
		fread_to_eol( fp );

		/* Validate parameters. We're calling the index functions for the side effect. */
		switch ( letter )
		{
		default:
			bug( "Load_resets: bad command '%c'.", letter );
			exit( 1 );
			break;

		case 'M':
			get_mob_index( pReset->arg1 );
			get_room_index( pReset->arg3 );
			break;

		case 'O':
			temp_index = get_obj_index( pReset->arg1 );
			temp_index->reset_num++;
			get_room_index( pReset->arg3 );
			break;

		case 'P':
			temp_index = get_obj_index( pReset->arg1 );
			temp_index->reset_num++;
			get_obj_index( pReset->arg3 );
			break;

		case 'G':
		case 'E':
			temp_index = get_obj_index( pReset->arg1 );
			temp_index->reset_num++;
			break;

		case 'D':
			pRoomIndex = get_room_index( pReset->arg1 );

			if (pReset->arg2 < 0
			||	pReset->arg2 > 5
			||	(pexit = pRoomIndex->exit[pReset->arg2]) == NULL
			||	!IS_SET(pexit->exit_info, EX_ISDOOR) )
			{
				bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
				exit( 1 );
			}

			if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
			{
				bug( "Load_resets: 'D': bad 'locks': %d.", pReset->arg3 );
				exit( 1 );
			}
			break;

		case 'R':
			pRoomIndex = get_room_index( pReset->arg1 );

			if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
			{
				bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
				exit( 1 );
			}
			break;
		}

		if ( area_last->reset_first == NULL )
			area_last->reset_first = pReset;
		if ( area_last->reset_last != NULL )
			area_last->reset_last->next = pReset;

		area_last->reset_last = pReset;
		pReset->next = NULL;
		top_reset++;
	}
}


/*
 * Snarf a room section.
 */
void load_rooms( FILE *fp )
{
	ROOM_INDEX_DATA *pRoomIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( area_last == NULL )
	{
		bug( "Load_resets: no #AREA seen yet.", 0 );
		exit( 1 );
	}

	for ( ;; )
	{
		sh_int	vnum;
		char	letter;
		int		door;
		int		iHash;
		/*~~~~~~~~~~~*/

		letter = fread_letter( fp );
		if ( letter != '#' )
		{
			bug( "Load_rooms: # not found.", 0 );
			exit( 1 );
		}

		vnum = fread_number( fp );
		if ( vnum == 0 )
			break;

		fBootDb = FALSE;
		if ( get_room_index(vnum) != NULL )
		{
			bug( "Load_rooms: vnum %d duplicated.", vnum );
			exit( 1 );
		}

		fBootDb = TRUE;

		pRoomIndex = alloc_perm( sizeof(*pRoomIndex) );
		pRoomIndex->owner = str_dup( "" );
		pRoomIndex->people = NULL;
		pRoomIndex->contents = NULL;
		pRoomIndex->extra_descr = NULL;
		pRoomIndex->history = NULL;
		pRoomIndex->area = area_last;
		pRoomIndex->vnum = vnum;
		pRoomIndex->name = fread_string( fp );
		pRoomIndex->description = fread_string( fp );

		/* Area number */
		fread_number( fp );
		pRoomIndex->room_flags = fread_flag( fp );

		if ( 3000 <= vnum && vnum < 3400 )
			SET_BIT( pRoomIndex->room_flags, ROOM_LAW );

		pRoomIndex->sector_type = fread_number( fp );
		pRoomIndex->light = 0;
		for ( door = 0; door < MAX_DIR; door++ )
			pRoomIndex->exit[door] = NULL;

		/* defaults */
		pRoomIndex->heal_rate = 100;
		pRoomIndex->mana_rate = 100;
		pRoomIndex->affected = NULL;
		pRoomIndex->affected_by = 0;
		pRoomIndex->aff_next = NULL;

		for ( ;; )
		{
			letter = fread_letter( fp );

			if ( letter == 'S' )
				break;

			if ( letter == 'H' )		/* healing room */
				pRoomIndex->heal_rate = fread_number( fp );
			else if ( letter == 'M' )	/* mana room */
				pRoomIndex->mana_rate = fread_number( fp );
			else if ( letter == 'D' )
			{
				EXIT_DATA	*pexit;
				int			locks;
				/*~~~~~~~~~~~~~~~*/

				door = fread_number( fp );
				if ( door < 0 || door > MAX_DIR - 1 )
				{
					bug( "Fread_rooms: vnum %d has bad door number.", vnum );
					exit( 1 );
				}

				pexit = alloc_perm( sizeof(*pexit) );
				pexit->description = fread_string( fp );
				pexit->keyword = fread_string( fp );
				pexit->exit_info = 0;
				locks = fread_number( fp );
				pexit->key = fread_number( fp );
				pexit->vnum = fread_number( fp );

				switch ( locks )
				{
				case 1: pexit->exit_info = EX_ISDOOR;								break;
				case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;				break;
				case 3: pexit->exit_info = EX_ISDOOR | EX_NOPASS;					break;
				case 4: pexit->exit_info = EX_ISDOOR | EX_NOPASS | EX_PICKPROOF;	break;
				case 5: pexit->exit_info = EX_NOFLEE;								break;
				}

				pRoomIndex->exit[door] = pexit;
				pRoomIndex->old_exit[door] = pexit;
				top_exit++;
			}
			else if ( letter == 'E' )
			{
				EXTRA_DESCR_DATA	*ed;
				/*~~~~~~~~~~~~~~~~~~~~*/

				ed = alloc_perm( sizeof(*ed) );
				ed->keyword = fread_string( fp );
				ed->description = fread_string( fp );
				ed->next = pRoomIndex->extra_descr;
				pRoomIndex->extra_descr = ed;
				top_ed++;
			}
			else if ( letter == 'O' )
			{
				int idxowner;
				/*~~~~~~~~~*/

				if ( pRoomIndex->owner[0] != '\0' )
				{
					bug( "Load_rooms: duplicate owner.", 0 );
					exit( 1 );
				}

				pRoomIndex->owner = fread_string( fp );

				for ( idxowner = 0; (pRoomIndex->owner)[idxowner] != '\0'; idxowner++ )
					( pRoomIndex->owner )[idxowner] = LOWER( (pRoomIndex->owner)[idxowner] );
			}
			else
			{
				bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
				exit( 1 );
			}
		}

		iHash = vnum % MAX_KEY_HASH;
		pRoomIndex->next = room_index_hash[iHash];
		room_index_hash[iHash] = pRoomIndex;
		top_room++;
	}
}


/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
	SHOP_DATA	*pShop;
	/*~~~~~~~~~~~~~~~*/


	for ( ;; )
	{
		MOB_INDEX_DATA	*pMobIndex;
		int				iTrade;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pShop = alloc_perm( sizeof(*pShop) );
		pShop->keeper = fread_number( fp );
		if ( pShop->keeper == 0 )
			break;
		for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
			pShop->buy_type[iTrade] = fread_number( fp );
		pShop->profit_buy = fread_number( fp );
		pShop->profit_sell = fread_number( fp );

		pShop->open_hour = fread_number( fp );
		pShop->close_hour = fread_number( fp );
		fread_to_eol( fp );
		pMobIndex = get_mob_index( pShop->keeper );
		pMobIndex->pShop = pShop;

		if ( shop_first == NULL )
			shop_first = pShop;
		if ( shop_last != NULL )
			shop_last->next = pShop;

		shop_last = pShop;
		pShop->next = NULL;
		top_shop++;
	}
}


/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
	for ( ;; )
	{
		MOB_INDEX_DATA	*pMobIndex;
		char			letter;

		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		switch ( letter = fread_letter(fp) )
		{
		default:
			bug( "Load_specials: letter '%c' not *MS.", letter );
			exit( 1 );

		case 'S':
			return;

		case '*':
			break;

		case 'M':
			pMobIndex = get_mob_index( fread_number(fp) );
			pMobIndex->spec_fun = spec_lookup( fread_word(fp) );
			if ( pMobIndex->spec_fun == 0 )
			{
				bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
				exit( 1 );
			}
			break;
		}

		fread_to_eol( fp );
	}
}



/*
 * Translate all room exits from virtual to real. Has to be done after all rooms are read in. Check for bad reverse
 *    exits.
 */
void fix_exits( void )
{
	extern const sh_int rev_dir[];
	char				buf[MSL];
	ROOM_INDEX_DATA		*pRoomIndex;
	ROOM_INDEX_DATA		*to_room;
	EXIT_DATA			*pexit;
	EXIT_DATA			*pexit_rev;
	int					iHash;
	int					door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
		for ( pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL; pRoomIndex = pRoomIndex->next )
		{
			bool	fexit;
			/*~~~~~~~~~~*/

			fexit = FALSE;
			for ( door = 0; door < MAX_DIR; door++ )
			{
				if ( (pexit = pRoomIndex->exit[door]) != NULL )
				{
					if ( pexit->vnum <= 0 || get_room_index(pexit->vnum) == NULL )
						pexit->to_room = NULL;
					else
					{
						fexit = TRUE;
						pexit->to_room = get_room_index( pexit->vnum );
					}
				}
			}

			if ( !fexit )
				SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
		}
	}

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
		for ( pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL; pRoomIndex = pRoomIndex->next )
		{
			for ( door = 0; door < MAX_DIR; door++ )
			{
				if ((pexit = pRoomIndex->exit[door]) != NULL
				&&	(to_room = pexit->to_room) != NULL
				&&	(pexit_rev = to_room->exit[rev_dir[door]]) != NULL
				&&	pexit_rev->to_room != pRoomIndex
				&&	(pRoomIndex->vnum < 1200 || pRoomIndex->vnum > 1299) )
				{
					sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
						pRoomIndex->vnum,
						door,
						to_room->vnum,
						rev_dir[door],
						(pexit_rev->to_room == NULL) ? 0 : pexit_rev->to_room->vnum );
					bug( buf, 0 );
				}
			}
		}
	}
}


/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
	AREA_DATA		*pArea;
	DESCRIPTOR_DATA *d;
	char			buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( pArea = area_first;  pArea != NULL;  pArea = pArea->next )
	{
		if ( ++pArea->age < 3 )
			continue;

		/* Check age and reset. Note: Mud School resets every 3 minutes (not 15). */
		if ( (!pArea->empty && (pArea->nplayer == 0 || pArea->age >= 15)) || pArea->age >= 31 )
		{
			ROOM_INDEX_DATA *pRoomIndex;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			reset_area( pArea );
			sprintf( buf, "L'area %s e' appena stata resettata.", pArea->name );
			wiznet( buf, NULL, NULL, WIZ_RESETS, 0 );

			if ( number_range(0, 4) == 0 )
			{
				if ( pArea->resetmsg )
					sprintf( buf, "%s\n\r", pArea->resetmsg );
				else
				{
					switch ( number_range(0, 3) )
					{
					case 0:
						sprintf( buf, "Senti un abbaiare insistente..\n\r" );
						break;
					case 1:
						sprintf( buf, "Una lieve brezza ti accarezza la pelle.\n\r" );
						break;
					case 2:
						sprintf( buf, "Il rintocco di una campana si sente in lontananza.\n\r" );
						break;
					case 3:
						sprintf( buf, "Senti la terra tremare.\n\r" );
						break;
					}
				}

				for ( d = descriptor_list; d != NULL; d = d->next )
				{
					if ( d->connected == CON_PLAYING && IS_AWAKE(d->character) && d->character->in_room )
					{
						if ( d->character->in_room->area == pArea )
							send_to_char( buf, d->character );
					}
				}
			}

			pArea->age = number_range( 0, 3 );
			pRoomIndex = get_room_index( 200 );
			if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
				pArea->age = 15 - 2;
			pRoomIndex = get_room_index( 210 );
			if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
				pArea->age = 15 - 2;
			pRoomIndex = get_room_index( 220 );
			if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
				pArea->age = 15 - 2;
			pRoomIndex = get_room_index( 230 );
			if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
				pArea->age = 15 - 2;
			pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
			if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
				pArea->age = 15 - 2;
			else if ( pArea->nplayer == 0 )
				pArea->empty = TRUE;
		}
	}
}


/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
	RESET_DATA		*pReset;
	CHAR_DATA		*mob;
	bool			last;
	int				level;
	int				i;
	ROOM_INDEX_DATA *room;
	DESCRIPTOR_DATA *d;
	CHAR_DATA		*ch;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if ( weather_info.sky == SKY_RAINING )
	{
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected != CON_PLAYING )
				continue;
			ch = ( d->original != NULL ) ? d->original : d->character;
			if ( (ch->in_room->area == pArea)
			&& (get_skill(ch, gsn_track) > 60)
			&& (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) )
				send_to_char( "La pioggia cancella le tracce sul terreno.\n\r", ch );
		}

		for ( i = pArea->min_vnum; i < pArea->max_vnum; i++ )
		{
			room = get_room_index( i );
			if ( room == NULL )
				continue;
			if ( IS_SET(room->room_flags, ROOM_INDOORS) )
				continue;
			room_record( "erased", room, -1 );
			if ( number_percent() < 50 )
				room_record( "erased", room, -1 );
		}
	}

	mob = NULL;
	last = TRUE;
	level = 0;
	for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
	{
		ROOM_INDEX_DATA *pRoomIndex;
		MOB_INDEX_DATA	*pMobIndex;
		OBJ_INDEX_DATA	*pObjIndex;
		OBJ_INDEX_DATA	*pObjToIndex;
		OBJ_INDEX_DATA	*cabal_item;

		EXIT_DATA		*pexit;
		OBJ_DATA		*obj;
		OBJ_DATA		*obj_to;
		int				count, limit, ci_vnum = 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		switch ( pReset->command )
		{
		default:
			bug( "Reset_area: bad command %c.", pReset->command );
			break;

		case 'M':
			if ( (pMobIndex = get_mob_index(pReset->arg1)) == NULL )
			{
				bug( "Reset_area: 'M': bad vnum %d.", pReset->arg1 );
				continue;
			}

			if ( (pRoomIndex = get_room_index(pReset->arg3)) == NULL )
			{
				bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
				continue;
			}

			if ( pMobIndex->count >= pReset->arg2 )
			{
				last = FALSE;
				break;
			}

			count = 0;
			for ( mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room )
				if ( mob->pIndexData == pMobIndex )
				{
					count++;
					if ( count >= pReset->arg4 )
					{
						last = FALSE;
						break;
					}
				}

			if ( count >= pReset->arg4 )
				break;

			mob = create_mobile( pMobIndex );

			/* Check for pet shop. */
			{
				ROOM_INDEX_DATA *pRoomIndexPrev;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
				if ( pRoomIndexPrev != NULL && IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP) )
					SET_BIT( mob->act, ACT_PET );
			}

			/* set area */
			mob->zone = pRoomIndex->area;

			char_to_room( mob, pRoomIndex );
			level = URANGE( 0, mob->level - 2, LEVEL_HERO - 1 );
			last = TRUE;
			break;


		case 'O':
			if ( (pObjIndex = get_obj_index(pReset->arg1)) == NULL )
			{
				bug( "Reset_area: 'O': bad vnum %d.", pReset->arg1 );
				continue;
			}

			if ( (pRoomIndex = get_room_index(pReset->arg3)) == NULL )
			{
				bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
				continue;
			}

			if ( pArea->nplayer > 0 || count_obj_list(pObjIndex, pRoomIndex->contents) > 0 )
			{
				last = FALSE;
				break;
			}

			switch ( pObjIndex->vnum )
			{
			case OBJ_VNUM_RULER_STAND:		ci_vnum = cabal_table[CABAL_RULER].obj_vnum;		break;
			case OBJ_VNUM_INVADER_SKULL:	ci_vnum = cabal_table[CABAL_INVADER].obj_vnum;		break;
			case OBJ_VNUM_YVALADRIN_ALTAR:	ci_vnum = cabal_table[CABAL_YVALADRIN].obj_vnum;	break;
			case OBJ_VNUM_CHAOS_ALTAR:		ci_vnum = cabal_table[CABAL_CHAOS].obj_vnum;		break;
			case OBJ_VNUM_KNIGHT_ALTAR:		ci_vnum = cabal_table[CABAL_KNIGHT].obj_vnum;		break;
			case OBJ_VNUM_LIONS_ALTAR:		ci_vnum = cabal_table[CABAL_LIONS].obj_vnum;		break;
			case OBJ_VNUM_ARMY_THRONE:		ci_vnum = cabal_table[CABAL_ARMY].obj_vnum;			break;
			case OBJ_VNUM_HUNTER_ALTAR:		ci_vnum = cabal_table[CABAL_HUNTER].obj_vnum;		break;
			}

			cabal_item = get_obj_index( ci_vnum );
			if ( ci_vnum && cabal_item->count > 0 )
			{
				last = FALSE;
				break;
			}

			if ( (pObjIndex->limit != -1) && (pObjIndex->count >= pObjIndex->limit) )
			{
				last = FALSE;
				break;
			}

			obj = create_object( pObjIndex, UMIN(number_fuzzy(level), LEVEL_HERO - 1) );
			obj->cost = 0;

			obj_to_room( obj, pRoomIndex );
			last = TRUE;
			break;

		case 'P':
			if ( (pObjIndex = get_obj_index(pReset->arg1)) == NULL )
			{
				bug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
				continue;
			}

			if ( (pObjToIndex = get_obj_index(pReset->arg3)) == NULL )
			{
				bug( "Reset_area: 'P': bad vnum %d.", pReset->arg3 );
				continue;
			}

			if ( pReset->arg2 > 50 )		/* old format */
				limit = 6;
			else if ( pReset->arg2 == -1 )	/* no limit */
				limit = 999;
			else
				limit = pReset->arg2;

			if (pArea->nplayer > 0
			||	(obj_to = get_obj_type(pObjToIndex)) == NULL
			||	(obj_to->in_room == NULL && !last)
			||	(pObjIndex->count >= limit && number_range(0, 4) != 0)
			||	(count = count_obj_list(pObjIndex, obj_to->contains)) > pReset->arg4 )
			{
				last = FALSE;
				break;
			}

			if ( (pObjIndex->limit != -1) && (pObjIndex->count >= pObjIndex->limit) )
			{
				last = FALSE;
				dump_to_scr( "Reseting area: [P] OBJ limit reached\n\r" );
				break;
			}

			while ( count < pReset->arg4 )
			{
				obj = create_object( pObjIndex, number_fuzzy(obj_to->level) );
				obj_to_obj( obj, obj_to );
				count++;
				if ( pObjIndex->count >= limit )
					break;
			}

			/* fix object lock state! */
			obj_to->value[1] = obj_to->pIndexData->value[1];
			last = TRUE;
			break;

		case 'G':
		case 'E':
			if ( (pObjIndex = get_obj_index(pReset->arg1)) == NULL )
			{
				bug( "Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
				continue;
			}

			if ( !last )
				break;

			if ( mob == NULL )
			{
				bug( "Reset_area: 'E' or 'G': null mob for vnum %d.", pReset->arg1 );
				last = FALSE;
				break;
			}

			if ( mob->pIndexData->pShop != NULL )
			{
				int olevel = 0, val, class;
				/*~~~~~~~~~~~~~~~~~*/

				if ( !pObjIndex->new_format )
				{
					switch ( pObjIndex->item_type )
					{
					case ITEM_PILL:
					case ITEM_POTION:
					case ITEM_SCROLL:
						olevel = MAX_LEVEL - 7;
						for ( val = 1; val < 5; val++ )
						{
							if ( pObjIndex->value[val] > 0 )
							{
								for ( class = 0; class < MAX_CLASS; class++ )
									olevel = UMIN( olevel, skill_table[pObjIndex->value[val]].skill_level[class] );
							}
						}

						olevel = UMAX( 0, (olevel * 3 / 4) - 2 );
						break;

					case ITEM_WAND:
						olevel = number_range( 10, 20 );
						break;

					case ITEM_STAFF:
						olevel = number_range( 15, 25 );
						break;

					case ITEM_ARMOR:
						olevel = number_range( 5, 15 );
						break;

					case ITEM_WEAPON:
						olevel = number_range( 5, 15 );
						break;

					case ITEM_TREASURE:
						olevel = number_range( 10, 20 );
						break;
					}
				}

				obj = create_object( pObjIndex, olevel );
				SET_BIT( obj->extra_flags, ITEM_INVENTORY );
			}
			else
			{
				if ( (pObjIndex->limit == -1) || (pObjIndex->count < pObjIndex->limit) )
					obj = create_object( pObjIndex, UMIN(number_fuzzy(level), LEVEL_HERO - 1) );
				else
					break;
			}

			obj_to_char( obj, mob );
			if ( pReset->command == 'E' )
			{
				int iWear = wear_convert( pReset->arg3 );
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if ( iWear != WEAR_NONE )
					equip_char( mob, obj, iWear );
			}

			last = TRUE;
			break;

		case 'D':
			if ( (pRoomIndex = get_room_index(pReset->arg1)) == NULL )
			{
				bug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
				continue;
			}

			if ( (pexit = pRoomIndex->exit[pReset->arg2]) == NULL )
				break;

			switch ( pReset->arg3 )
			{
			case 0: REMOVE_BIT( pexit->exit_info, EX_CLOSED );	REMOVE_BIT( pexit->exit_info, EX_LOCKED );	break;
			case 1: SET_BIT( pexit->exit_info, EX_CLOSED );		REMOVE_BIT( pexit->exit_info, EX_LOCKED );	break;
			case 2: SET_BIT( pexit->exit_info, EX_CLOSED );		SET_BIT( pexit->exit_info, EX_LOCKED );		break;
			}

			last = TRUE;
			break;

		case 'R':
			if ( (pRoomIndex = get_room_index(pReset->arg1)) == NULL )
			{
				bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
				continue;
			}
			{
				int d0;
				int d1;
				/*~~~*/

				for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
				{
					d1 = number_range( d0, pReset->arg2 - 1 );
					pexit = pRoomIndex->exit[d0];
					pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
					pRoomIndex->exit[d1] = pexit;
				}
			}
			break;
		}
	}
}


/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
	CHAR_DATA	*mob;
	int			i;
	AFFECT_DATA af;
	/*~~~~~~~~~~~~~*/

	mobile_count++;

	if ( pMobIndex == NULL )
	{
		bug( "Create_mobile: NULL pMobIndex.", 0 );
		exit( 1 );
	}

	mob = new_char();

	mob->pIndexData = pMobIndex;

	mob->name = pMobIndex->player_name;
	mob->id = get_mob_id();
	mob->short_descr = pMobIndex->short_descr;
	mob->long_descr = pMobIndex->long_descr;
	mob->description = pMobIndex->description;
	mob->spec_fun = pMobIndex->spec_fun;
	mob->prompt = NULL;
	mob->progtypes = pMobIndex->progtypes;
	mob->riding = FALSE;
	mob->mount = NULL;
	mob->hunting = NULL;
	mob->in_mind = NULL;
	mob->cabal = CABAL_NONE;
	mob->class = CLASS_CLERIC;

	if ( pMobIndex->wealth == 0 )
	{
		mob->silver = 0;
		mob->gold = 0;
	}
	else
	{
		long	wealth;
		/*~~~~~~~~~~~*/

		wealth = number_range( pMobIndex->wealth / 3, 3 * pMobIndex->wealth / 2 );
		mob->gold = number_range( wealth / 200, wealth / 100 );
		mob->silver = wealth - ( mob->gold * 100 );
	}

	if ( pMobIndex->new_format )
	/* load in new style */
	{
		/* read from prototype */
		mob->group = pMobIndex->group;
		mob->act = pMobIndex->act | ACT_IS_MOB;
		mob->comm = COMM_NOCHANNELS | COMM_NOYELL | COMM_NOTELL;
		mob->affected_by = pMobIndex->affected_by;
		mob->detection = pMobIndex->detection;
		mob->alignment = pMobIndex->alignment;
		mob->level = pMobIndex->level;
		mob->hitroll = ( mob->level / 2 ) + pMobIndex->hitroll;
		mob->damroll = pMobIndex->damage[DICE_BONUS];
		mob->max_point[LIFE] = dice( pMobIndex->hit[DICE_NUMBER], pMobIndex->hit[DICE_TYPE] ) + pMobIndex->hit[DICE_BONUS];
		mob->point[LIFE] = mob->max_point[LIFE];
		mob->max_point[MANA] = dice( pMobIndex->mana[DICE_NUMBER], pMobIndex->mana[DICE_TYPE] ) + pMobIndex->mana[DICE_BONUS];
		mob->point[MANA] = mob->max_point[MANA];
		mob->damage[DICE_NUMBER] = pMobIndex->damage[DICE_NUMBER];
		mob->damage[DICE_TYPE] = pMobIndex->damage[DICE_TYPE];
		mob->dam_type = pMobIndex->dam_type;

		mob->status = 0;
		if ( mob->dam_type == 0 )
		{
			switch ( number_range(1, 3) )
			{
			case ( 1 ): mob->dam_type = 3; break;	/* slash */
			case ( 2 ): mob->dam_type = 7; break;	/* pound */
			case ( 3 ): mob->dam_type = 11; break;	/* pierce */
			}
		}

		for ( i = 0; i < 4; i++ )
			mob->armor[i] = pMobIndex->ac[i];
		mob->off_flags = pMobIndex->off_flags;
		mob->imm_flags = pMobIndex->imm_flags;
		mob->res_flags = pMobIndex->res_flags;
		mob->vuln_flags = pMobIndex->vuln_flags;
		mob->start_pos = pMobIndex->start_pos;
		mob->default_pos = pMobIndex->default_pos;
		mob->sex = pMobIndex->sex;
		if ( mob->sex == 3 )						/* random sex */
			mob->sex = number_range( 1, 2 );
		mob->race = pMobIndex->race;
		mob->form = pMobIndex->form;
		mob->parts = pMobIndex->parts;
		mob->size = pMobIndex->size;
		mob->material = str_dup( pMobIndex->material );
		mob->progtypes = pMobIndex->progtypes;
		mob->extracted = FALSE;

		/* computed on the spot */
		for ( i = 0; i < MAX_STATS; i++ )
			mob->perm_stat[i] = UMIN( 25, 11 + mob->level / 4 );

		if ( IS_SET(mob->act, ACT_WARRIOR) )
		{
			mob->perm_stat[STR] += 3;
			mob->perm_stat[INT] -= 1;
			mob->perm_stat[CON] += 2;
		}

		if ( IS_SET(mob->act, ACT_THIEF) )
		{
			mob->perm_stat[DEX] += 3;
			mob->perm_stat[INT] += 1;
			mob->perm_stat[WIS] -= 1;
		}

		if ( IS_SET(mob->act, ACT_CLERIC) )
		{
			mob->perm_stat[WIS] += 3;
			mob->perm_stat[DEX] -= 1;
			mob->perm_stat[STR] += 1;
		}

		if ( IS_SET(mob->act, ACT_MAGE) )
		{
			mob->perm_stat[INT] += 3;
			mob->perm_stat[STR] -= 1;
			mob->perm_stat[DEX] += 1;
		}

		if ( IS_SET(mob->off_flags, OFF_FAST) )
			mob->perm_stat[DEX] += 2;

		mob->perm_stat[STR] += mob->size - SIZE_MEDIUM;
		mob->perm_stat[CON] += ( mob->size - SIZE_MEDIUM ) / 2;

		/* let's get some spell action */
		if ( IS_AFFECTED(mob, AFF_SANCTUARY) )
		{
			af.where = TO_AFFECTS;
			af.type = skill_lookup( "sanctuary" );
			af.level = mob->level;
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_SANCTUARY;
			affect_to_char( mob, &af );
		}

		if ( IS_AFFECTED(mob, AFF_HASTE) )
		{
			af.where = TO_AFFECTS;
			af.type = skill_lookup( "haste" );
			af.level = mob->level;
			af.duration = -1;
			af.location = APPLY_DEX;
			af.modifier = 1 + ( mob->level >= 18 ) + ( mob->level >= 25 ) + ( mob->level >= 32 );
			af.bitvector = AFF_HASTE;
			affect_to_char( mob, &af );
		}

		if ( IS_AFFECTED(mob, AFF_PROTECT_EVIL) )
		{
			af.where = TO_AFFECTS;
			af.type = skill_lookup( "protection evil" );
			af.level = mob->level;
			af.duration = -1;
			af.location = APPLY_SAVES;
			af.modifier = -1;
			af.bitvector = AFF_PROTECT_EVIL;
			affect_to_char( mob, &af );
		}

		if ( IS_AFFECTED(mob, AFF_PROTECT_GOOD) )
		{
			af.where = TO_AFFECTS;
			af.type = skill_lookup( "protection good" );
			af.level = mob->level;
			af.duration = -1;
			af.location = APPLY_SAVES;
			af.modifier = -1;
			af.bitvector = AFF_PROTECT_GOOD;
			affect_to_char( mob, &af );
		}
	}
	else	/* read in old format and convert */
	{
		mob->act = pMobIndex->act;
		mob->affected_by = pMobIndex->affected_by;
		mob->detection = pMobIndex->detection;
		mob->alignment = pMobIndex->alignment;
		mob->level = pMobIndex->level;
		mob->hitroll = UMAX( pMobIndex->hitroll, pMobIndex->level / 4 );
		mob->damroll = pMobIndex->level / 2;
		if ( mob->level < 30 )
			mob->max_point[LIFE] = mob->level * 20 + number_range( mob->level, mob->level * 5 );
		else if ( mob->level < 60 )
			mob->max_point[LIFE] = mob->level * 50 + number_range( mob->level * 10, mob->level * 50 );
		else
			mob->max_point[LIFE] = mob->level * 100 + number_range( mob->level * 20, mob->level * 100 );
		if ( IS_SET(mob->act, ACT_MAGE | ACT_CLERIC) )
			mob->max_point[LIFE] *= 0.9;
		mob->point[LIFE] = mob->max_point[LIFE];
		mob->max_point[MANA] = 100 + dice( mob->level, 10 );
		mob->point[MANA] = mob->max_point[MANA];
		switch ( number_range(1, 3) )
		{
		case ( 1 ): mob->dam_type = 3; break;	/* slash */
		case ( 2 ): mob->dam_type = 7; break;	/* pound */
		case ( 3 ): mob->dam_type = 11; break;	/* pierce */
		}

		for ( i = 0; i < 3; i++ )
			mob->armor[i] = interpolate( mob->level, 100, -100 );
		mob->armor[3] = interpolate( mob->level, 100, 0 );
		mob->race = pMobIndex->race;
		mob->off_flags = pMobIndex->off_flags;
		mob->imm_flags = pMobIndex->imm_flags;
		mob->res_flags = pMobIndex->res_flags;
		mob->vuln_flags = pMobIndex->vuln_flags;
		mob->start_pos = pMobIndex->start_pos;
		mob->default_pos = pMobIndex->default_pos;
		mob->sex = pMobIndex->sex;
		mob->form = pMobIndex->form;
		mob->parts = pMobIndex->parts;
		mob->size = SIZE_MEDIUM;
		mob->material = "";
		mob->extracted = FALSE;

		/* for (i = 0; i < MAX_STATS; i ++) mob->perm_stat[i] = 11 + mob->level/4; computed on the spot */
		for ( i = 0; i < MAX_STATS; i++ )
			mob->perm_stat[i] = UMIN( 25, 11 + mob->level / 4 );

		if ( IS_SET(mob->act, ACT_WARRIOR) )
		{
			mob->perm_stat[STR] += 3;
			mob->perm_stat[INT] -= 1;
			mob->perm_stat[CON] += 2;
		}

		if ( IS_SET(mob->act, ACT_THIEF) )
		{
			mob->perm_stat[DEX] += 3;
			mob->perm_stat[INT] += 1;
			mob->perm_stat[WIS] -= 1;
		}

		if ( IS_SET(mob->act, ACT_CLERIC) )
		{
			mob->perm_stat[WIS] += 3;
			mob->perm_stat[DEX] -= 1;
			mob->perm_stat[STR] += 1;
		}

		if ( IS_SET(mob->act, ACT_MAGE) )
		{
			mob->perm_stat[INT] += 3;
			mob->perm_stat[STR] -= 1;
			mob->perm_stat[DEX] += 1;
		}

		if ( IS_SET(mob->off_flags, OFF_FAST) )
			mob->perm_stat[DEX] += 2;
	}

	mob->position = mob->start_pos;

	if ( mob->gold > mob->level )
		mob->gold = dice( 6, mob->level );

	/* link the mob to the world list */
	mob->next = char_list;
	char_list = mob;
	pMobIndex->count++;
	return( mob );
}


/*
 * duplicate a mobile exactly -- except inventory
 */
void clone_mobile( CHAR_DATA *parent, CHAR_DATA *clone )
{
	int			i;
	AFFECT_DATA *paf;
	/*~~~~~~~~~~~~~*/

	if ( parent == NULL || clone == NULL || !IS_MOB(parent) )
		return;

	/* start fixing values */
	clone->name = str_dup( parent->name );
	clone->version = parent->version;
	clone->short_descr = str_dup( parent->short_descr );
	clone->long_descr = str_dup( parent->long_descr );
	clone->description = str_dup( parent->description );
	clone->group = parent->group;
	clone->sex = parent->sex;
	clone->class = parent->class;
	clone->race = parent->race;
	clone->level = parent->level;
	clone->trust = 0;
	clone->timer = parent->timer;
	clone->wait = parent->wait;
	clone->point[LIFE] = parent->point[LIFE];
	clone->max_point[LIFE] = parent->max_point[LIFE];
	clone->point[MANA] = parent->point[MANA];
	clone->max_point[MANA] = parent->max_point[MANA];
	clone->point[MOVE] = parent->point[MOVE];
	clone->max_point[MOVE] = parent->max_point[MOVE];
	clone->gold = parent->gold;
	clone->silver = parent->silver;
	clone->exp = parent->exp;
	clone->act = parent->act;
	clone->comm = parent->comm;
	clone->imm_flags = parent->imm_flags;
	clone->res_flags = parent->res_flags;
	clone->vuln_flags = parent->vuln_flags;
	clone->invis_level = parent->invis_level;
	clone->affected_by = parent->affected_by;
	clone->detection = parent->detection;
	clone->position = parent->position;
	clone->practice = parent->practice;
	clone->train = parent->train;
	clone->saving_throw = parent->saving_throw;
	clone->alignment = parent->alignment;
	clone->hitroll = parent->hitroll;
	clone->damroll = parent->damroll;
	clone->wimpy = parent->wimpy;
	clone->form = parent->form;
	clone->parts = parent->parts;
	clone->size = parent->size;
	clone->material = str_dup( parent->material );
	clone->extracted = parent->extracted;

	clone->off_flags = parent->off_flags;
	clone->dam_type = parent->dam_type;
	clone->start_pos = parent->start_pos;
	clone->default_pos = parent->default_pos;
	clone->spec_fun = parent->spec_fun;
	clone->progtypes = parent->progtypes;
	clone->status = parent->status;
	clone->hunting = NULL;
	clone->in_mind = NULL;
	clone->cabal = CABAL_NONE;

	for ( i = 0; i < 4; i++ )
		clone->armor[i] = parent->armor[i];

	for ( i = 0; i < MAX_STATS; i++ )
	{
		clone->perm_stat[i] = parent->perm_stat[i];
		clone->mod_stat[i] = parent->mod_stat[i];
	}

	for ( i = 0; i < 3; i++ )
		clone->damage[i] = parent->damage[i];

	/* now add the affects */
	for ( paf = parent->affected; paf != NULL; paf = paf->next )
		affect_to_char( clone, paf );
}


/*
 * Create an object with modifying the count
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
	return( create_object_org(pObjIndex, level, TRUE) );
}


/*
 * For player load/quit Create an object and do not modify the count
 */
OBJ_DATA *create_object_nocount( OBJ_INDEX_DATA *pObjIndex, int level )
{
	return( create_object_org(pObjIndex, level, FALSE) );
}


/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object_org( OBJ_INDEX_DATA *pObjIndex, int level, bool Count )
{
	AFFECT_DATA *paf;
	OBJ_DATA	*obj;
	int			i;
	/*~~~~~~~~~~~~~*/

	if ( pObjIndex == NULL )
	{
		bug( "Create_object: NULL pObjIndex.", 0 );
		exit( 1 );
	}

	obj = new_obj();

	obj->pIndexData = pObjIndex;
	obj->in_room = NULL;
	obj->enchanted = FALSE;


	for ( i = 1; i < MAX_CABAL; i++ )
	{
		if ( pObjIndex->vnum == cabal_table[i].obj_vnum )
		{
			/* if ( count_obj_list( pObjIndex, object_list) > 0 ) return(NULL); */
			cabal_table[i].obj_ptr = obj;
			break;
		}
	}

	if ( (obj->pIndexData->limit != -1) && (obj->pIndexData->count >= obj->pIndexData->limit) )
		if ( pObjIndex->new_format == 1 )
			dump_to_scr( "" );

	if ( pObjIndex->new_format == 1 )
		obj->level = pObjIndex->level;
	else
		obj->level = UMAX( 0, level );
	obj->wear_loc = -1;

	obj->name = pObjIndex->name;
	obj->short_descr = pObjIndex->short_descr;
	obj->description = pObjIndex->description;
	obj->material = str_dup( pObjIndex->material );
	obj->item_type = pObjIndex->item_type;
	obj->extra_flags = pObjIndex->extra_flags;
	obj->wear_flags = pObjIndex->wear_flags;
	obj->value[0] = pObjIndex->value[0];
	obj->value[1] = pObjIndex->value[1];
	obj->value[2] = pObjIndex->value[2];
	obj->value[3] = pObjIndex->value[3];
	obj->value[4] = pObjIndex->value[4];
	obj->weight = pObjIndex->weight;
	obj->extracted = FALSE;
	obj->progtypes = pObjIndex->progtypes;
	obj->from = str_dup( "" );		/* used with body parts */
	obj->pit = OBJ_VNUM_PIT;		/* default for corpse decaying */
	obj->altar = ROOM_VNUM_ALTAR;	/* default for corpses */
	obj->condition = pObjIndex->condition;

	if ( level == -1 || pObjIndex->new_format )
		obj->cost = pObjIndex->cost;
	else
		obj->cost = number_fuzzy( 10 ) * number_fuzzy( level ) * number_fuzzy( level );

	/* Mess with object properties. */
	switch ( obj->item_type )
	{
	default:
		bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
		break;

	case ITEM_LIGHT:
		if ( obj->value[2] == 999 )
			obj->value[2] = -1;
		break;

	case ITEM_FURNITURE:
	case ITEM_TRASH:
	case ITEM_CONTAINER:
	case ITEM_DRINK_CON:
	case ITEM_KEY:
	case ITEM_FOOD:
	case ITEM_BOAT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_FOUNTAIN:
	case ITEM_MAP:
	case ITEM_CLOTHING:
	case ITEM_PORTAL:
		if ( !pObjIndex->new_format )
			obj->cost /= 5;
		break;

	case ITEM_TREASURE:
	case ITEM_WARP_STONE:
	case ITEM_ROOM_KEY:
	case ITEM_GEM:
	case ITEM_JEWELRY:
	case ITEM_TATTOO:
		break;

	case ITEM_JUKEBOX:
		for ( i = 0; i < 5; i++ )
			obj->value[i] = -1;
		break;

	case ITEM_SCROLL:
		if ( level != -1 && !pObjIndex->new_format )
			obj->value[0] = number_fuzzy( obj->value[0] );
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		if ( level != -1 && !pObjIndex->new_format )
		{
			obj->value[0] = number_fuzzy( obj->value[0] );
			obj->value[1] = number_fuzzy( obj->value[1] );
			obj->value[2] = obj->value[1];
		}

		if ( !pObjIndex->new_format )
			obj->cost *= 2;
		break;

	case ITEM_WEAPON:
		if ( level != -1 && !pObjIndex->new_format )
		{
			obj->value[1] = number_fuzzy( number_fuzzy(1 * level / 4 + 2) );
			obj->value[2] = number_fuzzy( number_fuzzy(3 * level / 4 + 6) );
		}
		break;

	case ITEM_ARMOR:
		if ( level != -1 && !pObjIndex->new_format )
		{
			obj->value[0] = number_fuzzy( level / 5 + 3 );
			obj->value[1] = number_fuzzy( level / 5 + 3 );
			obj->value[2] = number_fuzzy( level / 5 + 3 );
		}
		break;

	case ITEM_POTION:
	case ITEM_PILL:
		if ( level != -1 && !pObjIndex->new_format )
			obj->value[0] = number_fuzzy( number_fuzzy(obj->value[0]) );
		break;

	case ITEM_MONEY:
		if ( !pObjIndex->new_format )
			obj->value[0] = obj->cost;
		break;
	}

	for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )

	{
		if ( paf->location == APPLY_SPELL_AFFECT )
			affect_to_obj( obj, paf );
	}

	obj->next = object_list;
	object_list = obj;
	if ( Count )
		pObjIndex->count++;
	return( obj );
}


/*
 * duplicate an object exactly -- except contents
 */
void clone_object( OBJ_DATA *parent, OBJ_DATA *clone )
{
	int					i;
	AFFECT_DATA			*paf;
	EXTRA_DESCR_DATA	*ed, *ed_new;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( parent == NULL || clone == NULL )
		return;

	/* start fixing the object */
	clone->name = str_dup( parent->name );
	clone->short_descr = str_dup( parent->short_descr );
	clone->description = str_dup( parent->description );
	clone->item_type = parent->item_type;
	clone->extra_flags = parent->extra_flags;
	clone->wear_flags = parent->wear_flags;
	clone->weight = parent->weight;
	clone->cost = parent->cost;
	clone->level = parent->level;
	clone->condition = parent->condition;
	clone->material = str_dup( parent->material );
	clone->timer = parent->timer;
	clone->from = parent->from;
	clone->extracted = parent->extracted;

	clone->pit = parent->pit;
	clone->altar = parent->altar;

	for ( i = 0; i < 5; i++ )
		clone->value[i] = parent->value[i];

	/* affects */
	clone->enchanted = parent->enchanted;

	for ( paf = parent->affected; paf != NULL; paf = paf->next )
		affect_to_obj( clone, paf );

	/* extended desc */
	for ( ed = parent->extra_descr; ed != NULL; ed = ed->next )
	{
		ed_new = new_extra_descr();
		ed_new->keyword = str_dup( ed->keyword );
		ed_new->description = str_dup( ed->description );
		ed_new->next = clone->extra_descr;
		clone->extra_descr = ed_new;
	}
}


/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
	static CHAR_DATA	ch_zero;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	*ch = ch_zero;
	ch->name = &str_empty[0];
	ch->short_descr = &str_empty[0];
	ch->long_descr = &str_empty[0];
	ch->description = &str_empty[0];
	ch->prompt = &str_empty[0];
	ch->logon = current_time;
	ch->lines = PAGELEN;
	for ( i = 0; i < 4; i++ )
		ch->armor[i] = 100;
	ch->position = POS_STAND;
	ch->point[LIFE]		= 20;
	ch->max_point[LIFE] = 20;
	ch->point[MANA]		= 100;
	ch->max_point[MANA] = 100;
	ch->point[MOVE]		= 100;
	ch->max_point[MOVE] = 100;
	ch->last_fought = NULL;
	ch->last_fight_time = -1;
	ch->last_death_time = -1;
	ch->on = NULL;
	for ( i = 0; i < MAX_STATS; i++ )
	{
		ch->perm_stat[i] = 13;
		ch->mod_stat[i] = 0;
	}
}


/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
	for ( ; ed != NULL; ed = ed->next )
	{
		if ( is_name((char *) name, ed->keyword) )
			return( ed->description );
	}

	return( NULL );
}


/*
 * Translates mob virtual number to its mob index struct. Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
	MOB_INDEX_DATA	*pMobIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( pMobIndex = mob_index_hash[vnum % MAX_KEY_HASH]; pMobIndex != NULL; pMobIndex = pMobIndex->next )
	{
		if ( pMobIndex->vnum == vnum )
			return( pMobIndex );
	}

	if ( fBootDb )
	{
		bug( "Get_mob_index: bad vnum %d.", vnum );
		exit( 1 );
	}

	return( NULL );
}


/*
 * Translates mob virtual number to its obj index struct. Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
	OBJ_INDEX_DATA	*pObjIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( pObjIndex = obj_index_hash[vnum % MAX_KEY_HASH]; pObjIndex != NULL; pObjIndex = pObjIndex->next )
	{
		if ( pObjIndex->vnum == vnum )
			return( pObjIndex );
	}

	if ( fBootDb )
	{
		bug( "Get_obj_index: bad vnum %d.", vnum );
		exit( 1 );
	}

	return( NULL );
}


/*
 * Translates mob virtual number to its room index struct. Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
	ROOM_INDEX_DATA *pRoomIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( pRoomIndex = room_index_hash[vnum % MAX_KEY_HASH]; pRoomIndex != NULL; pRoomIndex = pRoomIndex->next )
	{
		if ( pRoomIndex->vnum == vnum )
			return( pRoomIndex );
	}

	if ( fBootDb )
	{
		bug( "Get_room_index: bad vnum %d.", vnum );
		exit( 1 );
	}

	return( NULL );
}


bool controlla_vocale( const char letter )
{
	if (letter == 'A'
	||	letter == 'a'
	||	letter == 'E'
	||	letter == 'e'
	||	letter == 'I'
	||	letter == 'i'
	||	letter == 'O'
	||	letter == 'o'
	||	letter == 'U'
	||	letter == 'u')
		return( TRUE );
	else
		return( FALSE );
}


/*
 * Snippet di Onirik, la funzione scegli_articolo, l'originale si trova al sito http://utenti.lycos.it/RPGfantasy/
 */
char *scegli_articolo( const char *parola, int tipo )
{
    if ( parola == NULL )
    {
        bug( "scegli_articolo: la variabile 'parola' e' NULL.", 0 );
        return "il ";       /* giusto per ritornare qualcosa :P */
    }

    if ( !IS_SET(tipo, ART_FEMMINILE) )
    {
        if ( (LOWER(parola[0]) == 's' && !controlla_vocale(parola[1]))  /* Se e' esse impura                 */
          || LOWER(parola[0]) == 'z'                            /* Se inizia con 'z'                */
          || LOWER(parola[0]) == 'x'                            /*  o con 'x'                       */
          || !str_prefix("pn", parola )                         /* Se inizia con il gruppo "pn"     */
          || !str_prefix("ps", parola )                         /*  o "ps"                          */
          || !str_prefix("gn", parola )                         /* Se inizia con il diagramma "gn"  */
/*        || !str_prefix(parola, "sc")                              o "sc" (inutile visto che ci pensa il controllo sulla esse impura, e' qui giusto per una precisazione linguistica */
          || (LOWER(parola[0]) == 'i' && (controlla_vocale(parola[1]) && parola[1] != parola[0])) ) /* Se inizia per vocale semiconsontantica i/j/ */
        {
            if ( !IS_SET(tipo, ART_PLURALE) )
            {
                if ( !IS_SET(tipo, ART_INDETERMINATO) )
                    return "lo ";
                else
                    return "uno ";
            }
            else
            {
                if ( !IS_SET(tipo, ART_INDETERMINATO) )
                    return "gli ";
                else
                    return "degli ";
            }
        }
        else
        {
            /* Se la parola non e' al plurale */
            if ( !IS_SET(tipo, ART_PLURALE) )
            {
                if ( controlla_vocale(parola[0]) )
                {
                    if ( !IS_SET(tipo, ART_INDETERMINATO) )
                        return "l'";
                    else
                        return "un ";
                }
                else
                {
                    if ( !IS_SET(tipo, ART_INDETERMINATO) )
                        return "il ";   /* Ci sarebbe da effettuare un controllo sulle 'h' aspirate e non per l'apostrofo.. ma la cosa non e' fattibile senza, per esempio, una funzione che controlli un elenco di sostantivi scelti per essere apostrofati, poiche' vi possono essere nomi fantasy del contesto background del mud che richiedono questo stile.. fate vobis :P */
                    else
                        return "un ";
                }
            }
            else
            {
                if ( controlla_vocale(parola[0]) )
                {
                    if ( !IS_SET(tipo, ART_INDETERMINATO) )
                        return "gli ";
                    else
                        return "degli ";
                }
                else
                {
                    if ( !IS_SET(tipo, ART_INDETERMINATO) )
                        return "i ";
                    else
                        return "dei ";
                }
            }
        }
    }
    else
    {
        if ( !IS_SET(tipo, ART_PLURALE) )
        {
            /* Se la prima lettera e' una vocale e non e' una semiconsonante allora elide
             *  l'articolo con l'apostrofo */
            if ( controlla_vocale(parola[0]) && !(LOWER(parola[0]) == 'i' && (controlla_vocale(parola[1]) && parola[1] != parola[0])) ) /* Se non semiconsonante */
            {
                if ( !IS_SET(tipo, ART_INDETERMINATO) )
                    return "l'";
                else
                    return "un'";
            }
            else
            {
                if ( !IS_SET(tipo, ART_INDETERMINATO) )
                    return "la ";   /* davanti a 'h', al contrario del maschile, non c'e' la forma con l'apostrofo, un problema in meno :P */
                else
                    return "una ";
            }
        }
        else
        {
            if ( !IS_SET(tipo, ART_INDETERMINATO) )
                return "le ";       /* raramente anche "l'" davanti a vocale ma solo in testi poetici. */
            else
                return "delle ";
        }
    }
}


/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
	char	c;
	/*~~~~~~*/

	do
	{
		c = getc( fp );
	} while ( isspace(c) );

	return( c );
}


/*
 * Read a number from a file.
 */
int fread_number( FILE *fp )
{
	int		number;
	bool	sign;
	char	c;

	/*~~~~~~~~~~~*/

	do
	{
		c = getc( fp );
	} while ( isspace(c) );

	number = 0;

	sign = FALSE;
	if ( c == '+' )
	{
		c = getc( fp );
	}
	else if ( c == '-' )
	{
		sign = TRUE;
		c = getc( fp );
	}

	if ( !isdigit(c) )
	{
		bug( "Fread_number: bad format.", 0 );
		exit( 1 );
	}

	while ( isdigit(c) )
	{
		number = number * 10 + c - '0';
		c = getc( fp );
	}

	if ( sign )
		number = 0 - number;

	if ( c == '|' )
		number += fread_number( fp );
	else if ( c != ' ' )
		ungetc( c, fp );

	return( number );
}


long fread_flag( FILE *fp )
{
	int		number;
	char	c;
	bool	negative = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	do
	{
		c = getc( fp );
	} while ( isspace(c) );

	if ( c == '-' )
	{
		negative = TRUE;
		c = getc( fp );
	}

	number = 0;

	if ( !isdigit(c) )
	{
		while ( ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') )
		{
			number += flag_convert( c );
			c = getc( fp );
		}
	}

	while ( isdigit(c) )
	{
		number = number * 10 + c - '0';
		c = getc( fp );
	}

	if ( c == '|' )
		number += fread_flag( fp );
	else if ( c != ' ' )
		ungetc( c, fp );

	if ( negative )
		return( -1 * number );

	return( number );
}


long flag_convert( char letter )
{
	long	bitsum = 0;
	char	i;
	/*~~~~~~~~~~~~~~~*/

	if ( 'A' <= letter && letter <= 'Z' )
	{
		bitsum = 1;
		for ( i = letter; i > 'A'; i-- )
			bitsum *= 2;
	}
	else if ( 'a' <= letter && letter <= 'z' )
	{
		bitsum = 67108864;	/* 2^26 */
		for ( i = letter; i > 'a'; i-- )
			bitsum *= 2;
	}

	return( bitsum );
}


/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 *   this function takes 40% to 50% of boot-up time.
 */
char *fread_string( FILE *fp )
{
	char	*plast;
	char	c;
	/*~~~~~~~~~~~*/

	plast = top_string + sizeof( char * );
	if ( plast > &string_space[MAX_STRING - MSL] )
	{
		bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
		exit( 1 );
	}

	/* Skip blanks. Read first char. */
	do
	{
		c = getc( fp );
	} while ( isspace(c) );

	if ( (*plast++ = c) == '~' )
		return( &str_empty[0] );

	for ( ;; )
	{
		/* Back off the char type lookup, it was too dirty for portability. -- Furey */
		switch ( *plast = getc(fp) )
		{
		default:
			plast++;
			break;

		case EOF:
			/* temp fix */
			bug( "Fread_string: EOF", 0 );
			return( NULL );

			/* exit( 1 ); */
			break;

		case '\n':
			plast++;
			*plast++ = '\r';
			break;

		case '\r':
			break;

		case '~':
			plast++;
			{
				union
				{
					char	*pc;
					char	rgc[sizeof( char * )];
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				} u1;

				int		ic;
				int		iHash;
				char	*pHash;
				char	*pHashPrev;
				char	*pString;
				/*~~~~~~~~~~~~~~~*/

				plast[-1] = '\0';
				iHash = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
				for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
				{
					for ( ic = 0; ic < sizeof(char *); ic++ )
						u1.rgc[ic] = pHash[ic];
					pHashPrev = u1.pc;
					pHash += sizeof( char * );

					if ( top_string[sizeof(char *)] == pHash[0] && !strcmp(top_string + sizeof(char *) + 1, pHash + 1) )
						return( pHash );
				}

				if ( fBootDb )
				{
					pString = top_string;
					top_string = plast;
					u1.pc = string_hash[iHash];
					for ( ic = 0; ic < sizeof(char *); ic++ )
						pString[ic] = u1.rgc[ic];
					string_hash[iHash] = pString;

					nAllocString += 1;
					sAllocString += top_string - pString;
					return( pString + sizeof(char *) );
				}
				else
					return( str_dup(top_string + sizeof(char *)) );
			}
		}
	}
}


char *fread_string_eol( FILE *fp )
{
	static bool char_special[256 - EOF];
	char		*plast;
	char		c;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( char_special[EOF - EOF] != TRUE )
	{
		char_special[EOF - EOF] = TRUE;
		char_special['\n' - EOF] = TRUE;
		char_special['\r' - EOF] = TRUE;
	}

	plast = top_string + sizeof( char * );
	if ( plast > &string_space[MAX_STRING - MSL] )
	{
		bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
		exit( 1 );
	}

	/* Skip blanks. Read first char. */
	do
	{
		c = getc( fp );
	} while ( isspace(c) );

	if ( (*plast++ = c) == '\n' )
		return( &str_empty[0] );

	for ( ;; )
	{
		if ( !char_special[(*plast++ = getc(fp)) - EOF] )
			continue;

		switch ( plast[-1] )
		{
		default:
			break;

		case EOF:
			bug( "Fread_string_eol  EOF", 0 );
			exit( 1 );
			break;

		case '\n':
		case '\r':
			{
				union
				{
					char	*pc;
					char	rgc[sizeof( char * )];
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				} u1;

				int		ic;
				int		iHash;
				char	*pHash;
				char	*pHashPrev;
				char	*pString;
				/*~~~~~~~~~~~~~~~*/

				plast[-1] = '\0';
				iHash = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
				for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
				{
					for ( ic = 0; ic < sizeof(char *); ic++ )
						u1.rgc[ic] = pHash[ic];
					pHashPrev = u1.pc;
					pHash += sizeof( char * );

					if ( top_string[sizeof(char *)] == pHash[0] && !strcmp(top_string + sizeof(char *) + 1, pHash + 1) )
						return( pHash );
				}

				if ( fBootDb )
				{
					pString = top_string;
					top_string = plast;
					u1.pc = string_hash[iHash];
					for ( ic = 0; ic < sizeof(char *); ic++ )
						pString[ic] = u1.rgc[ic];
					string_hash[iHash] = pString;

					nAllocString += 1;
					sAllocString += top_string - pString;
					return( pString + sizeof(char *) );
				}
				else
					return( str_dup(top_string + sizeof(char *)) );
			}
		}
	}
}


/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
	char	c;
	/*~~~~~~*/

	do
	{
		c = getc( fp );
	} while ( c != '\n' && c != '\r' );

	do
	{
		c = getc( fp );
	} while ( c == '\n' || c == '\r' );

	ungetc( c, fp );
}


/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp )
{
	char		buf[MSL];
	static char word[MIL];
	char		*pword;
	char		cEnd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	do
	{
		cEnd = getc( fp );
	} while ( isspace(cEnd) );

	if ( cEnd == '\'' || cEnd == '"' )
	{
		pword = word;
	}
	else
	{
		word[0] = cEnd;
		pword = word + 1;
		cEnd = ' ';
	}

	for ( ; pword < word + MIL; pword++ )
	{
		*pword = getc( fp );
		if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
		{
			if ( cEnd == ' ' )
				ungetc( *pword, fp );
			*pword = '\0';
			return( word );
		}
	}

	sprintf( buf, "Fread_word: parola troppo lunga: %s", word );
	bug( buf, 0 );
	exit( 1 );
	return( NULL );
}


/*
 * Allocate some ordinary memory, with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
	char	*pMem;
	int		*magic;
	int		iList;
	/*~~~~~~~~~~~*/

	sMem += sizeof( *magic );


	for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
	{
		if ( sMem <= rgSizeList[iList] )
			break;
	}

	if ( iList == MAX_MEM_LIST )
	{
		bug( "Alloc_mem: size %d too large.", sMem );
		exit( 1 );
	}

	if ( rgFreeList[iList] == NULL )
	{
		pMem = (char *) alloc_perm( rgSizeList[iList] );
	}
	else
	{
		pMem = (char *) rgFreeList[iList];
		rgFreeList[iList] = *( (void **) rgFreeList[iList] );
	}

	magic = (int *) pMem;
	*magic = MAGIC_NUM;
	pMem += sizeof( *magic );

	return (void *) pMem;
}


/*
 * Free some memory. Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
	int		iList;
	int		*magic;
	char	*char_pMem = (char *) pMem;
	/*~~~~~~~*/

	char_pMem -= sizeof( *magic );
	magic = (int *) char_pMem;

	if ( *magic != MAGIC_NUM )
	{
		bug( "Attempt to recyle invalid memory of size %d.", sMem );
		bug( (char *) char_pMem + sizeof(*magic), 0 );
		return;
	}

	*magic = 0;
	sMem += sizeof( *magic );

	for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
	{
		if ( sMem <= rgSizeList[iList] )
			break;
	}

	if ( iList == MAX_MEM_LIST )
	{
		bug( "Free_mem: size %d too large.", sMem );
		exit( 1 );
	}

	*( (void **) char_pMem ) = rgFreeList[iList];
	rgFreeList[iList] = char_pMem;
}


/*
 * Allocate some permanent memory. Permanent memory is never freed, pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
	static char *pMemPerm;
	static int	iMemPerm;
	void		*pMem;
	/*~~~~~~~~~~~~~~~~~~*/

	while ( sMem % sizeof(long) != 0 )
		sMem++;
	if ( sMem > MAX_PERM_BLOCK )
	{
		bug( "Alloc_perm: %d too large.", sMem );
		exit( 1 );
	}

	if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
	{
		iMemPerm = 0;
		if ( (pMemPerm = calloc(1, MAX_PERM_BLOCK)) == NULL )
		{
			perror( "Alloc_perm" );
			exit( 1 );
		}
	}

	pMem = pMemPerm + iMemPerm;
	iMemPerm += sMem;
	nAllocPerm += 1;
	sAllocPerm += sMem;
	return( pMem );
}


/*
 * Duplicate a string into dynamic memory. Fread_strings are read-only and shared.
 */
char *str_dup( const char *str )
{
	char	*str_new;
	/*~~~~~~~~~~~~~*/


	if ( str[0] == '\0' )
		return( &str_empty[0] );

	if ( str >= string_space && str < top_string )
		return (char *) str;

	str_new = alloc_mem( strlen(str) + 1 );
	strcpy( str_new, str );
	return( str_new );
}


/*
 * Free a string. Null is legal here to simplify callers. Read-only shared strings are not touched.
 */
void free_string( char *pstr )
{
	if ( pstr == NULL || pstr == &str_empty[0] || (pstr >= string_space && pstr < top_string) )

		return;

	free_mem( pstr, strlen(pstr) + 1 );
}


void do_areas( CHAR_DATA *ch, char *argument )
{
	char		buf[MSL*5];
	char		buf_vnum[MSL];
	AREA_DATA	*pArea;
	int			iArea;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Aree correnti nel Mud: \n\r" );

	buf_vnum[0] = '\0';
	pArea = area_first;

	for ( iArea = 0; iArea < top_area; iArea++ )
	{
		if ( IS_ADMIN(ch) )
		{
			sprintf( buf_vnum, "[#%s%-5d%s - #%s%-5d%s] ",
				CLR(ch, CYAN),
				pArea->min_vnum,
				CLR(ch, WHITE_BOLD),
				CLR(ch, CYAN),
				pArea->max_vnum,
				CLR(ch, WHITE_BOLD) );
		}

		sprintf( buf + strlen(buf), "{%s%3d%s - %s%3d%s} %s %s%-12s%s  %s%s%s\n\r",
			CLR(ch, CYAN),
			pArea->low_range,
			CLR(ch, WHITE_BOLD),
			CLR(ch, CYAN),
			pArea->high_range,
			CLR(ch, WHITE_BOLD),

			buf_vnum,

			CLR(ch, WHITE),
			pArea->writer,
			CLR(ch, WHITE_BOLD),

			CLR(ch, GREEN),
			pArea->credits,
			CLR(ch, WHITE_BOLD) );

		pArea = pArea->next;
	}

	strcat( buf, "\n\r" );
	page_to_char( buf, ch );
}


void do_memory( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf( buf, "Affects  %5d\n\r", top_affect );									send_to_char( buf, ch );
	sprintf( buf, "Aree     %5d\n\r", top_area );									send_to_char( buf, ch );
	sprintf( buf, "ExDes    %5d\n\r", top_ed );										send_to_char( buf, ch );
	sprintf( buf, "Uscite   %5d\n\r", top_exit );									send_to_char( buf, ch );
	sprintf( buf, "Help     %5d\n\r", top_help );									send_to_char( buf, ch );
	sprintf( buf, "Social   %5d\n\r", social_count );								send_to_char( buf, ch );
	sprintf( buf, "Mob      %5d(%d nuovo formato)\n\r", top_mob_index, newmobs );	send_to_char( buf, ch );
	sprintf( buf, "        (%5d in uso)\n\r", mobile_count );						send_to_char( buf, ch );
	sprintf( buf, "Oggetti  %5d(%d nuovo formato)\n\r", top_obj_index, newobjs );	send_to_char( buf, ch );
	sprintf( buf, "Reset    %5d\n\r", top_reset );									send_to_char( buf, ch );
	sprintf( buf, "Stanze   %5d\n\r", top_room );									send_to_char( buf, ch );
	sprintf( buf, "Negozi   %5d\n\r", top_shop );									send_to_char( buf, ch );

	sprintf( buf, "Stringhe %5d stringhe di %7d bytes (max %d).\n\r", nAllocString, sAllocString, MAX_STRING );
	send_to_char( buf, ch );

	sprintf( buf, "Perms   %5d blocchi di %7d bytes.\n\r", nAllocPerm, sAllocPerm );
	send_to_char( buf, ch );
}



void do_dump( CHAR_DATA *ch, char *argument )
{
	int				count, count2, num_pcs, aff_count;
	CHAR_DATA		*fch;
	MOB_INDEX_DATA	*pMobIndex;
	PC_DATA			*pc;
	OBJ_DATA		*obj;
	OBJ_INDEX_DATA	*pObjIndex;
	ROOM_INDEX_DATA *room;
	EXIT_DATA		*pexit;
	DESCRIPTOR_DATA *d;
	AFFECT_DATA		*af;
	FILE			*fp;
	int				vnum, nMatch = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* open file */
	fclose( fpReserve );
	fp = fopen( "mem.dmp", "w" );

	/* report use of data structures */
	num_pcs = 0;
	aff_count = 0;

	/* mobile prototypes */
	fprintf( fp, "MobProt %4d (%8d bytes)\n",
		top_mob_index, top_mob_index * (sizeof(*pMobIndex)) );

	/* mobs */
	count = 0;
	count2 = 0;
	for ( fch = char_list; fch != NULL; fch = fch->next )
	{
		count++;
		if ( fch->pcdata != NULL )
			num_pcs++;
		for ( af = fch->affected; af != NULL; af = af->next )
			aff_count++;
	}

	for ( fch = char_free; fch != NULL; fch = fch->next )
		count2++;

	fprintf( fp, "Mobs    %4d (%8d bytes), %2d free (%d bytes)\n",
		count, count * (sizeof(*fch)), count2, count2 * (sizeof(*fch)) );

	/* pcdata */
	count = 0;
	for ( pc = pcdata_free; pc != NULL; pc = pc->next )
		count++;

	fprintf( fp, "Pcdata  %4d (%8d bytes), %2d free (%d bytes)\n",
		num_pcs, num_pcs * (sizeof(*pc)), count, count * (sizeof(*pc)) );

	/* descriptors */
	count = 0;
	count2 = 0;
	for ( d = descriptor_list; d != NULL; d = d->next )
		count++;
	for ( d = descriptor_free; d != NULL; d = d->next )
		count2++;

	fprintf( fp, "Descs  %4d (%8d bytes), %2d free (%d bytes)\n",
		count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)) );

	/* object prototypes */
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
	{
		if ( (pObjIndex = get_obj_index(vnum)) != NULL )
		{
			for ( af = pObjIndex->affected; af != NULL; af = af->next )
				aff_count++;
			nMatch++;
		}
	}

	fprintf( fp, "ObjProt %4d (%8d bytes)\n",
		top_obj_index, top_obj_index * (sizeof(*pObjIndex)) );

	/* objects */
	count = 0;
	count2 = 0;
	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		count++;
		for ( af = obj->affected; af != NULL; af = af->next )
			aff_count++;
	}

	for ( obj = obj_free; obj != NULL; obj = obj->next )
		count2++;

	fprintf( fp, "Objs    %4d (%8d bytes), %2d free (%d bytes)\n",
		count, count * (sizeof(*obj)), count2, count2 * (sizeof(*obj)) );

	/* affects */
	count = 0;
	for ( af = affect_free; af != NULL; af = af->next )
		count++;

	fprintf( fp, "Affects %4d (%8d bytes), %2d free (%d bytes)\n",
		aff_count, aff_count * (sizeof(*af)), count, count * (sizeof(*af)) );

	/* rooms */
	fprintf( fp, "Rooms   %4d (%8d bytes)\n", top_room, top_room * (sizeof(*room)) );

	/* exits */
	fprintf( fp, "Exits   %4d (%8d bytes)\n", top_exit, top_exit * (sizeof(*pexit)) );

	fclose( fp );

	/* start printing out mobile data */
	fp = fopen( "mob.dmp", "w" );

	fprintf( fp, "\nMobile Analysis\n" );
	fprintf( fp, "---------------\n" );
	nMatch = 0;
	for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	{
		if ( (pMobIndex = get_mob_index(vnum)) != NULL )
		{

			nMatch++;
			fprintf( fp, "#%-4d %3d active %3d killed     %s\n",
				pMobIndex->vnum,
				pMobIndex->count,
				pMobIndex->killed,
				pMobIndex->short_descr );
		}
	}

	fclose( fp );

	/* start printing out object data */
	fp = fopen( "obj.dmp", "w" );

	fprintf( fp, "\nObject Analysis\n" );
	fprintf( fp, "---------------\n" );
	nMatch = 0;
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
	{
		if ( (pObjIndex = get_obj_index(vnum)) != NULL )
		{
			nMatch++;
			fprintf( fp, "#%-4d %3d active %3d reset      %s\n",
				pObjIndex->vnum,
				pObjIndex->count,
				pObjIndex->reset_num,
				pObjIndex->short_descr );
		}
	}

	/* close file */
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
}


/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
	switch ( number_bits(2) )
	{
	case 0: number -= 1; break;
	case 3: number += 1; break;
	}

	return( UMAX(1, number) );
}


/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
	int power;
	int number;
	/*~~~~~~~*/

	if ( from == 0 && to == 0 )
		return( 0 );

	if ( (to = to - from + 1) <= 1 )
		return( from );

	for ( power = 2; power < to; power <<= 1 )
		;

	while ( (number = number_mm() & (power - 1)) >= to )
		;

	return( from + number );
}


/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
	int percent;
	/*~~~~~~~~*/

	while ( (percent = number_mm() & (128 - 1)) > 99 )
		;

	return( 1 + percent );
}


/*
 * Generate a random door.
 */
int number_door( void )
{
	int door;
	/*~~~~~*/

	while ( (door = number_mm() & (8 - 1)) > MAX_DIR - 1 )
		;

	return( door );
}


int number_bits( int width )
{
	return( number_mm() & ((1 << width) - 1) );
}


/*
 * I've gotten too many bad reports on OS-supplied random number generators. This is the Mitchell-Moore algorithm
 * from Knuth Volume II. Best to leave the constants alone unless you've read Knuth. -- Furey
 */
/*
 * I noticed streaking with this random number generator, so I switched back to the system srandom call. If this
 * doesn't work for you, define OLD_RAND to use the old system -- Alander
 */
#if defined( OLD_RAND )
static int	rgiState[2 + 55];
#endif


void init_mm( void )
{
#if defined( OLD_RAND )
	int *piState;
	int iState;
	/*~~~~~~~~~*/

	piState = &rgiState[2];

	piState[-2] = 55 - 55;
	piState[-1] = 55 - 24;

	piState[0] = ( (int) current_time ) & ( (1 << 30) - 1 );
	piState[1] = 1;
	for ( iState = 2; iState < 55; iState++ )
	{
		piState[iState] = ( piState[iState - 1] + piState[iState - 2] ) & ( (1 << 30) - 1 );
	}

#else
	srandom( time(NULL) ^ getpid() );
#endif
}


long number_mm( void )
{
#if defined( OLD_RAND )
	int *piState;
	int iState1;
	int iState2;
	int iRand;
	/*~~~~~~~~~*/

	piState = &rgiState[2];
	iState1 = piState[-2];
	iState2 = piState[-1];
	iRand = ( piState[iState1] + piState[iState2] ) & ( (1 << 30) - 1 );
	piState[iState1] = iRand;
	if ( ++iState1 == 55 )
		iState1 = 0;
	if ( ++iState2 == 55 )
		iState2 = 0;
	piState[-2] = iState1;
	piState[-1] = iState2;
	return( iRand >> 6 );
#else
	return( random() >> 6 );
#endif
}


/*
 * Roll some dice.
 */
int dice( int number, int size )
{
	int idice;
	int sum;
	/*~~~~~~*/

	switch ( size )
	{
	case 0: return( 0 );
	case 1: return( number );
	}

	for ( idice = 0, sum = 0; idice < number; idice++ )
		sum += number_range( 1, size );

	return( sum );
}


/*
 * Simple linear interpolation.
 */

int interpolate( int level, int value_00, int value_32 )
{
	return( value_00 + level * (value_32 - value_00) / 32 );
}


/*
 * Removes the tildes from a string. Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
	for ( ; *str != '\0'; str++ )
	{
		if ( *str == '~' )
			*str = '-';
	}
}


/*
 * Compare strings, case insensitive. Return TRUE if different (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
	if ( astr == NULL )
	{
		bug( "Str_cmp: null astr.", 0 );
		return( TRUE );
	}

	if ( bstr == NULL )
	{
		bug( "Str_cmp: null bstr.", 0 );
		return( TRUE );
	}

	for ( ; *astr || *bstr; astr++, bstr++ )
	{
		if ( LOWER(*astr) != LOWER(*bstr) )
			return( TRUE );
	}

	return( FALSE );
}


/*
 * Compare strings, case insensitive, for prefix matching. Return TRUE if astr not a prefix of bstr (compatibility

 *    with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
	if ( astr == NULL )
	{
		bug( "Strn_cmp: null astr.", 0 );
		return( TRUE );
	}

	if ( bstr == NULL )
	{
		bug( "Strn_cmp: null bstr.", 0 );
		return( TRUE );
	}

	for ( ; *astr; astr++, bstr++ )
	{
		if ( LOWER(*astr) != LOWER(*bstr) )
			return( TRUE );
	}

	return( FALSE );
}


/*
 * Compare strings, case insensitive, for match anywhere. Returns TRUE is astr not part of bstr. (compatibility with
 *    historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
	int		sstr1;
	int		sstr2;
	int		ichar;
	char	c0;
	/*~~~~~~~~~~*/

	if ( (c0 = LOWER(astr[0])) == '\0' )
		return( FALSE );

	sstr1 = strlen( astr );
	sstr2 = strlen( bstr );

	for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
	{
		if ( c0 == LOWER(bstr[ichar]) && !str_prefix(astr, bstr + ichar) )
			return( FALSE );
	}

	return( TRUE );
}


/*
 * Compare strings, case insensitive, for suffix matching. Return TRUE if astr not a suffix of bstr (compatibility
 *    with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
	int sstr1;
	int sstr2;
	/*~~~~~~*/

	sstr1 = strlen( astr );
	sstr2 = strlen( bstr );
	if ( sstr1 <= sstr2 && !str_cmp(astr, bstr + sstr2 - sstr1) )
		return( FALSE );
	else
		return( TRUE );
}


/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
	static char strcap[MSL];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( i = 0; str[i] != '\0'; i++ )
		strcap[i] = LOWER( str[i] );
	strcap[i] = '\0';
	strcap[0] = UPPER( strcap[0] );
	return( strcap );
}


/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
	FILE	*fp;
	/*~~~~~~~~*/

	if ( IS_MOB(ch) || str[0] == '\0' )
		return;

	fclose( fpReserve );
	if ( (fp = fopen(file, "a")) == NULL )
	{
		perror( file );
		send_to_char( "Could not open the file!\n\r", ch );
	}
	else
	{
		fprintf( fp, "[%5d] %s: %s\n", ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
		fclose( fp );
	}

	fpReserve = fopen( NULL_FILE, "r" );
}


/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
	char	buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( fpArea != NULL )
	{
		int iLine;
		int iChar;
		/*~~~~~~*/

		if ( fpArea == stdin )
		{
			iLine = 0;
		}
		else
		{
			iChar = ftell( fpArea );
			fseek( fpArea, 0, 0 );
			for ( iLine = 0; ftell(fpArea) < iChar; iLine++ )
			{
				while ( getc(fpArea) != '\n' )
					;
			}

			fseek( fpArea, iChar, 0 );
		}

		sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
		log_string( buf );

/*		RT removed because we don't want bugs shutting the mud
    	if ( (fp = fopen("shutdown.txt", "a")) != NULL )
		{
			fprintf( fp, "[*****] %s\n", buf );
			fclose( fp );
		}
*/
	}

	strcpy( buf, "[*****] BUG: " );
	sprintf( buf + strlen(buf), str, param );
	log_string( buf );

/*	RT removed due to bug-file spamming
	fclose( fpReserve );
	if ( (fp = fopen(BUG_FILE, "a")) != NULL )
	{
		fprintf( fp, "%s\n", buf );
		fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );
*/
}


/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
	char	*strtime;
	/*~~~~~~~~~~~~~*/

	strtime = ctime( &current_time );
	strtime[strlen( strtime ) - 1] = '\0';
	fprintf( stderr, "%s :: %s\n", strtime, str );
}


/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *  -- Furey
 */
void tail_chain( void )
{
	return;
}


void load_olimits( FILE *fp )
{
	int				vnum;
	int				limit;
	char			ch;
	OBJ_INDEX_DATA	*pIndex;
	/*~~~~~~~~~~~~~~~~~~~~*/

	for ( ch = fread_letter(fp); ch != 'S'; ch = fread_letter(fp) )
	{
		switch ( ch )
		{
		case 'O':
			vnum = fread_number( fp );
			limit = fread_number( fp );
			if ( (pIndex = get_obj_index(vnum)) == NULL )
			{
				bug( "Load_olimits: bad vnum %d", vnum );
				exit( 1 );
			}
			else
				pIndex->limit = limit;
			break;

		case '*':
			fread_to_eol( fp );
			break;

		default:
			bug( "Load_olimits: bad command '%c'", ch );
			exit( 1 );
		}
	}
}


/*
 * Add the objects in players not logged on to object count
 */
void load_limited_objects( void )
{
	struct dirent	*dp;
	DIR				*dirp;
	FILE			*pfile;
	char			letter;
	char			*word;
	char			buf[MIL];
	bool			fReadLevel;
	int				vnum;
	int				tplayed;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	total_levels = 0;

	if ( (dirp = opendir(PLAYER_DIR)) == NULL )
	{
		bug( "Load_limited_objects: unable to open player directory.", 0 );
		exit( 1 );
	}

	for ( dp = readdir(dirp); dp != NULL; dp = readdir(dirp) )
	{
		if ( strlen(dp->d_name) < 3 )
			continue;

		sprintf( buf, "%s/", PLAYER_DIR );
		strcat( buf, dp->d_name );
		fReadLevel = FALSE;
		tplayed = 0;

		if ( (pfile = fopen(buf, "r")) == NULL )
		{
			sprintf(log_buf, "Load_limited_objects: Can't open player file: %s", buf);
			bug( log_buf, 0 );
			continue;
		}

		for ( letter = fread_letter(pfile); letter != EOF; letter = fread_letter(pfile) )
		{
			if ( letter == 'L' )
			{
				if ( !fReadLevel )
				{
					word = fread_word( pfile );

					if ( !str_cmp(word, "evel") )
					{
						i = fread_number( pfile );
						fReadLevel = TRUE;
						total_levels += UMAX( 0, i - 5 );
						sprintf( log_buf,"[%s] file +: %d", dp->d_name, UMAX(0,i-5) );
						log_string( log_buf );
						continue;
					}
				}
			}
			else if ( letter == 'P' )

			{
				word = fread_word( pfile );

				if ( !str_cmp(word, "layLog") )
				{
					int d, t, d_start, d_stop;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					fread_number( pfile );	/* read the version */
					while ( 1 )
					{
						if ( (d = fread_number(pfile)) < 0 )
							break;
						t = fread_number( pfile );

						d_start = get_played_day( MAX_TIME_LOG - 1 );
						d_stop  = get_played_day( 0 );

						if ( d >= d_start && d <= d_stop )
							tplayed += t;
					}
				}
			}
			else if ( letter == '#' )
			{
				word = fread_word( pfile );
				if ( !str_cmp(word, "O") || !str_cmp(word, "OBJECT") )
				{
					if ( tplayed < MIN_TIME_LIMIT )
					{
						sprintf( log_buf, "Discarding the player %s's limited equipments.", dp->d_name );
						log_string( log_buf );
						break;
					}

					fread_word( pfile );
					fBootDb = FALSE;
					vnum = fread_number( pfile );
					if ( get_obj_index(vnum) != NULL )
						get_obj_index( vnum )->count++;
					fBootDb = TRUE;
				}
			}
			else
				fread_to_eol( pfile );
		}
		fclose( pfile );
	}

	closedir( dirp );
}


/*
 * Given a name, return the appropriate prac fun.
 */
long prac_lookup( const char *name )
{
	int i;
	/*~~*/

	for ( i = 0; prac_table[i].name != NULL; i++ )
	{
		if ( LOWER(name[0]) == LOWER(prac_table[i].name[0]) && !str_prefix(name, prac_table[i].name) )
			return( 1 << prac_table[i].number );
	}

	return( 0 );
}


/*
 * Snarf can prac declarations.
 */
void load_practicer( FILE *fp )
{
	for ( ;; )
	{
		MOB_INDEX_DATA	*pMobIndex;
		char			letter;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		switch ( letter = fread_letter(fp) )
		{
		default:
			bug( "Load_specials: letter '%c' not *MS.", letter );
			exit( 1 );

		case 'S':
			return;

		case '*':
			break;

		case 'M':
			pMobIndex = get_mob_index( fread_number(fp) );
			SET_BIT( pMobIndex->practicer, prac_lookup(fread_word(fp)) );
			if ( pMobIndex->practicer == 0 )
			{
				bug( "Load_practicers: 'M': vnum %d.", pMobIndex->vnum );
				exit( 1 );
			}
			break;
		}

		fread_to_eol( fp );
	}
}


void load_resetmsg( FILE *fp )
{
	Serarea->resetmsg = fread_string( fp );
}


void load_aflag( FILE *fp )
{
	Serarea->area_flag = fread_flag( fp );
}


/*
 * Snarf a socials file
 */
void load_socials( FILE *fp )
{
	for ( ;; )
	{
		struct social_type	social;
		char				*temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* clear social */
		social.char_no_arg = NULL;
		social.others_no_arg = NULL;
		social.char_found = NULL;
		social.others_found = NULL;
		social.vict_found = NULL;
		social.char_not_found = NULL;
		social.char_auto = NULL;
		social.others_auto = NULL;

		temp = fread_word( fp );
		if ( !strcmp(temp, "#0") )
			return; /* done */
#if DEBUG_SOCIAL
		else
			fprintf( stderr, "%s\n\r", temp );
#endif
		strcpy( social.name, temp );
		fread_to_eol( fp );

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.char_no_arg = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.char_no_arg = temp;

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.others_no_arg = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.others_no_arg = temp;

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.char_found = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.char_found = temp;

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.others_found = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.others_found = temp;

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.vict_found = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.vict_found = temp;

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.char_not_found = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.char_not_found = temp;

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.char_auto = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.char_auto = temp;

		temp = fread_string_eol( fp );
		if ( !strcmp(temp, "$") )
			social.others_auto = NULL;
		else if ( !strcmp(temp, "#") )
		{
			social_table[social_count] = social;
			social_count++;
			continue;
		}
		else
			social.others_auto = temp;

		social_table[social_count] = social;
		social_count++;
	}
}


/*
 * Snarf a mprog section
 */
void load_omprogs( FILE *fp )
{
	char	progtype[MIL];
	char	progname[MIL];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( ;; )
	{
		MOB_INDEX_DATA	*pMobIndex;
		OBJ_INDEX_DATA	*pObjIndex;
		char			letter;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		switch ( letter = fread_letter(fp) )
		{
		default:
			bug( "Load_omprogs: letter '%c' not *IMS.", letter );
			exit( 1 );

		case 'S':
			return;

		case '*':
			break;

		case 'O':
			pObjIndex = get_obj_index( fread_number(fp) );
			if ( pObjIndex->oprogs == NULL )
				pObjIndex->oprogs = alloc_perm( sizeof(OPROG_DATA) );


			strcpy( progtype, fread_word(fp) );
			strcpy( progname, fread_word(fp) );
			oprog_set( pObjIndex, progtype, progname );
			break;

		case 'M':
			pMobIndex = get_mob_index( fread_number(fp) );
			if ( pMobIndex->mprogs == NULL )
				pMobIndex->mprogs = alloc_perm( sizeof(MPROG_DATA) );

			strcpy( progtype, fread_word(fp) );
			strcpy( progname, fread_word(fp) );
			mprog_set( pMobIndex, progtype, progname );
			break;
		}

		fread_to_eol( fp );
	}
}
