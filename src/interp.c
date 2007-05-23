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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "xandra.h"
#include "interp.h"
#include "file.h"


/*
 * Funzioni locali
 */
bool	check_social	( CHAR_DATA * ch, char *command, char *argument );


/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2


/*
 * Log-all switch.
 */
bool	fLogAll = FALSE;


/*
 * Immortals log file
 */
#undef IMMORTALS_LOGS
#ifdef IMMORTALS_LOGS
FILE	*imm_log;
#endif


/*
 * Command table.
 */
const struct cmd_type cmd_table[] =
{
	/*
	 * Common movement commands.
	 */
	{ "nord",			 "north",		do_north,			POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "est",			 "east",		do_east,			POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sud",			 "south",		do_south,			POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "ovest",			 "west",		do_west,			POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "alto",			 "up",			do_up,				POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "basso",			 "down",		do_down,			POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "nordest",		 "northeast",	do_northeast,		POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sudest",			 "southeast",	do_southeast,		POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sudovest",		 "southwest",	do_southwest,		POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "nordovest",		 "northwest",	do_northwest,		POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "corri",			 "run",			do_run,				POS_STAND,	IMP, LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},

	/*
	 * Common other commands.
	 * Placed here so one and two letter abbreviations work.
	 */
	{ "formula",		 "cast",		do_cast,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "supplica",		 "supplication",do_supplication,	POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "di",				 "say",			do_say,				POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "dilania",		 "claw",		do_claw,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "crecall",		 "crecall",		do_crecall,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "asta",			 "auction",		do_auction,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "ldt",			 "afk",			do_afk,				POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "compra",			 "buy",			do_buy,				POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "canali",			 "channels",	do_channels,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "uccidi",			 "kill",		do_kill,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "uscite",			 "exits",		do_exits,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "stima",			 "estimate",	do_estimate,		POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "prendi",			 "get",			do_get,				POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "guarda",			 "look",		do_look,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "goto",			 "goto",		do_goto,			POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "glista",			 "glist",		do_glist,			POS_DEAD,	0,	 LOG_NEVER,	 1, 0						},
	{ "gruppo",			 "group",		do_group,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "inventario",		 "inventory",	do_inventory,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "pratica",		 "practice",	do_practice,		POS_SLEEP,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "riposa",			 "rest",		do_rest,			POS_SLEEP,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "ripara",			 "repair",		do_repair,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "siediti",		 "sit",			do_sit,				POS_SLEEP,	0,	 LOG_NORMAL, 1, 0						},
	{ "forgia",			 "smithing",	do_smithing,		POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "socket",			 "sockets",		do_sockets,			POS_DEAD,	CRE, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "alzati",			 "stand",		do_stand,			POS_SLEEP,	0,	 LOG_NORMAL, 1, 0						},
	{ "comunica",		 "tell",		do_tell,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sblocca",		 "unlock",		do_unlock,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "set",			 "set",			do_set,				POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "stat",			 "stat",		do_stat,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "impugna",		 "wield",		do_wear,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "sussurra",		 "whisper",		do_whisper,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "mormora",		 "murmur",		do_murmur,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "wizhelp",		 "wizhelp",		do_wizhelp,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},

	/*
	 * Informational commands.
	 */
	{ "affetto",		 "affects",		do_affects,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "help",			 "aiuto",		do_help,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "aree",			 "areas",		do_areas,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "bilancio",		 "balance",		do_balance,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "bug",			 "bug",			do_bug,				POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "changes",		 "changes",		do_changes,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "comandi",		 "commands",	do_commands,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "confronta",		 "compare",		do_compare,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "considera",		 "consider",	do_consider,		POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "concentrati",	 "concentrate",	do_concentrate,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "crediti",		 "credits",		do_credits,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "deposita",		 "deposit",		do_deposit,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "equipaggiamento", "equipment",	do_equipment,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "scappa",			 "escape",		do_escape,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "esamina",		 "examine",		do_examine,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "aiuto",			 "help",		do_help,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "idea",			 "idea",		do_idea,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "motd",			 "motd",		do_motd,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "notizie",		 "news",		do_news,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "raffetto",		 "raffects",	do_raffects,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "leggi",			 "read",		do_read,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "regole",			 "rules",		do_rules,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "scruta",			 "scan",		do_scan,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "stato",			 "score",		do_score,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "abilita",		 "skills",		do_skills,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "parla",			 "speak",		do_speak,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sociali",		 "socials",		do_socials,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "mostra",			 "show",		do_show,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "incantesimi",	 "spells",		do_spells,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "ordina",			 "order",		do_order,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "data",			 "date",		do_date,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "ora",			 "time",		do_date,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "typo",			 "typo",		do_typo,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "tempo",			 "weather",		do_weather,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "chi",			 "who",			do_who,				POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "preleva",		 "withdraw",	do_withdraw,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "wizlista",		 "wizlist",		do_wizlist,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},

	/*
	 * Configuration commands.
	 */
	{ "alia",			 "alia",		do_alia,			POS_DEAD,	0,	 LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST },
	{ "alias",			 "alias",		do_alias,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST },
	{ "pulisci",		 "clear",		do_clear,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "cls",			 "cls",			do_clear,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "autolista",		 "autolist",	do_autolist,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "colori",			 "color",		do_color,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "autoassisti",	 "autoassist",	do_autoassist,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "autouscite",		 "autoexit",	do_autoexit,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
#ifdef WEATHER
	{ "autotempo",		 "autoweather",	do_autoweather,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
#endif
	{ "automonete",		 "autogold",	do_autogold,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "autosaccheggia",	 "autoloot",	do_autoloot,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "autosacrifica",	 "autosac",		do_autosac,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "autodividi",		 "autosplit",	do_autosplit,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "breve",			 "brief",		do_brief,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "combina",		 "combine",		do_combine,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "compatto",		 "compact",		do_compact,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "descrizione",	 "description",	do_description,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "titolo",			 "title",		do_title,			POS_DEAD,	0,	 LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "inglese",		 "italiano",	do_english,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "cancell",		 "delet",		do_delet,			POS_DEAD,	0,	 LOG_ALWAYS, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "cancella",		 "delete",		do_delete,			POS_DEAD,	0,	 LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "identifica",		 "identify",	do_identify,		POS_STAND,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "nocancella",		 "nocancel",	do_nocancel,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "noseguito",		 "nofollow",	do_nofollow,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "noevocato",		 "nosummon",	do_nosummon,		POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "occorrente",		 "outfit",		do_outfit,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "password",		 "password",	do_password,		POS_DEAD,	0,	 LOG_NEVER,	 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "prompt",			 "prompt",		do_prompt,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "ricerca",		 "quest",		do_quest,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "righe",			 "scroll",		do_scroll,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "unalias",		 "unalias",		do_unalias,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST },
	{ "fuga",			 "wimpy",		do_wimpy,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},


	/*
	 * Communication commands.
	 */
	{ "chiamaorsi",		 "bearcall",	do_bear_call,		POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "sordo",			 "deaf",		do_deaf,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "esprimi",		 "emote",		do_emote,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "gcomunica",		 "gtell",		do_gtell,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "gc2",			 "gt2",			do_gt2,				POS_SLEEP,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "nota",			 "note",		do_note,			POS_SLEEP,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "posa",			 "pose",		do_pose,			POS_STAND,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "prega",			 "pray",		do_pray,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "quieto",			 "quiet",		do_quiet,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "replica",		 "reply",		do_reply,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "ripeti",			 "replay",		do_replay,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "urla",			 "yell",		do_yell,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "grida",			 "shout",		do_shout,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "gridoguerra",	 "warcry",		do_warcry,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "nonletto",		 "unread",		do_unread,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},

	/*
	 * Object manipulation commands.
	 */
	{ "brandisci",		 "brandish",	do_brandish,		POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "seppellisci",	 "bury",		do_bury,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "macella",		 "butcher",		do_butcher,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "chiudi",			 "close",		do_close,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "indossa",		 "wear",		do_wear,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "individua",		 "detect",		do_detect_hidden,	POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "trascina",		 "drag",		do_drag,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "bevi",			 "drink",		do_drink,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "abbandona",		 "drop",		do_drop,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "mangia",			 "eat",			do_eat,				POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "incanta",		 "enchant",		do_enchant,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "avvelena",		 "envenom",		do_envenom,			POS_STAND,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "riempi",			 "fill",		do_fill,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "vola",			 "fly",			do_fly,				POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "dai",			 "give",		do_give,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "cura",			 "heal",		do_heal,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "tieni",			 "hold",		do_wear,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "imponimani",		 "layhands",	do_layhands,		POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "lista",			 "list",		do_list,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "blocca",			 "lock",		do_lock,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "tradizione",		 "lore",		do_lore,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "apri",			 "open",		do_open,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "scassina",		 "pick",		do_pick,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "svuota",			 "pour",		do_pour,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "metti",			 "put",			do_put,				POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "tracanna",		 "quaff",		do_quaff,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "recita",			 "recite",		do_recite,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "rimuovi",		 "remove",		do_remove,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "richiedi",		 "request",		do_request,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "vendi",			 "sell",		do_sell,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "sacrifica",		 "sacrifice",	do_sacrifice,		POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "butta",			 "junk",		do_junk,			POS_REST,	0,	 LOG_NORMAL, 0, 0						},
	{ "trofeo",			 "trophy",		do_trophy,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "valuta",			 "value",		do_value,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "punta",			 "zap",			do_zap,				POS_REST,	0,	 LOG_NORMAL, 1, 0						},

	/*
	 * Combat commands.
	 */
	{ "agguato",		 "ambush",		do_ambush,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "assassina",		 "assassinate",	do_assassinate,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "pugnala",		 "backstab",	do_backstab,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "sbilancia",		 "bash",		do_bash,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "sfondaporta",	 "bashdoor",	do_bash_door,		POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "bs",				 "bs",			do_backstab,		POS_STAND,	0,	 LOG_NORMAL, 0, 0						},
	{ "mordi",			 "bite",		do_vbite,			POS_STAND,	0,	 LOG_NORMAL, 0, 0						},
	{ "acceca",			 "blindness",	do_blindness_dust,	POS_FIGHT,	0,	 LOG_ALWAYS, 1, 0						},
	{ "tocca",			 "touch",		do_vtouch,			POS_STAND,	0,	 LOG_NORMAL, 0, 0						},
	{ "berserk",		 "berserk",		do_berserk,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "setesangue",		 "bloodthirst",	do_bloodthirst,		POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "manganello",		 "blackjack",	do_blackjack,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "spuntoni",		 "caltraps",	do_caltraps,		POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "esplodi",		 "explode",		do_explode,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "camuffati",		 "camouflage",	do_camouflage,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "circonda",		 "circle",		do_circle,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "fendi",			 "cleave",		do_cleave,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},

	{ "polvere",		 "dirt",		do_dirt,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "disarma",		 "disarm",		do_disarm,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "disonora",		 "dishonor",	do_dishonor,		POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "scendi",			 "dismount",	do_dismount,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "fuggi",			 "flee",		do_flee,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "proteggi",		 "guard",		do_guard,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},

	{ "calcia",			 "kick",		do_kick,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "sferza",			 "lash",		do_lash,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "chiamaleoni",	 "lioncall",	do_lion_call,		POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "crea",			 "make",		do_make,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "cavalca",		 "mount",		do_mount,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "assassin",		 "murde",		do_murde,			POS_FIGHT,	0,	 LOG_NORMAL, 0, 0						},
	{ "assassina",		 "murder",		do_murder,			POS_FIGHT,	0,	 LOG_ALWAYS, 1, 0						},
	{ "nervo",			 "nerve",		do_nerve,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "veleno",			 "poison",		do_poison_smoke,	POS_FIGHT,	0,	 LOG_ALWAYS, 1, 0						},
	{ "soccorri",		 "rescue",		do_rescue,			POS_FIGHT,	0,	 LOG_NORMAL, 0, 0						},
	{ "resistenza",		 "resistance",	do_resistance,		POS_FIGHT,	0,	 LOG_NORMAL, 0, 0						},
	{ "scudo",			 "shield",		do_shield,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "antimagia",		 "spellbane",	do_spellbane,		POS_FIGHT,	0,	 LOG_NORMAL, 0, 0						},
	{ "strangola",		 "strangle",	do_strangle,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "ammansisci",		 "tame",		do_tame,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "lancia",			 "throw",		do_throw,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "tigre",			 "tiger",		do_tiger,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "sgambetta",		 "trip",		do_trip,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "bersaglio",		 "target",		do_target,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "vampiro",		 "vampire",		do_vampire,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "svanisci",		 "vanish",		do_vanish,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "spaccaarma",		 "weapon",		do_weapon,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "illusione",		 "blink",		do_blink,			POS_FIGHT,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},

	/*
	 * Miscellaneous commands.
	 */
	{ "pensa",			 "think",		do_think,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sopporta",		 "endure",		do_endure,			POS_STAND,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "entra",			 "enter",		do_enter,			POS_STAND,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "fin",			 "fin",			do_qui,				POS_DEAD,	0,	 LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "fine",			 "fine",		do_quit,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "esc",			 "qui",			do_qui,				POS_DEAD,	0,	 LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "esci",			 "quit",		do_quit,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "segui",			 "follow",		do_follow,			POS_STAND,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "guadagna",		 "gain",		do_gain,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "svanisci",		 "fade",		do_fade,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "erbe",			 "herbs",		do_herbs,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "hara",			 "hara",		do_hara,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "induct",			 "induct",		do_induct,			POS_DEAD,	0,	 LOG_ALWAYS, 1, 0						},
	{ "nasconditi",		 "hide",		do_hide,			POS_REST,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "umano",			 "human",		do_human,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "caccia",			 "hunt",		do_hunt,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "lascia",			 "leave",		do_enter,			POS_STAND,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "ritorna",		 "recall",		do_recall,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "salva",			 "save",		do_save,			POS_DEAD,	0,	 LOG_NORMAL, 1, CMD_GHOST				},
	{ "dormi",			 "sleep",		do_sleep,			POS_SLEEP,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "furtivo",		 "sneak",		do_sneak,			POS_STAND,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "dividi",			 "split",		do_split,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "ruba",			 "steal",		do_steal,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "allena",			 "train",		do_train,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "visibile",		 "visible",		do_visible,			POS_SLEEP,	0,	 LOG_NORMAL, 1, 0						},
	{ "svegliati",		 "wake",		do_wake,			POS_SLEEP,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE			},
	{ "traccia",		 "track",		do_track,			POS_STAND,	0,	 LOG_NORMAL, 0, 0						},
	{ "ricercato",		 "wanted",		do_wanted,			POS_REST,	0,	 LOG_ALWAYS, 1, 0						},
	{ "cabal_scan",		 "cabal_scan",	do_cabal_scan,		POS_STAND,	0,	 LOG_NEVER,	 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "trappola",		 "settraps",	do_settraps,		POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "categoria",		 "slook",		do_slook,			POS_SLEEP,	0,	 LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "impara",			 "learn",		do_learn,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "insegna",		 "teach",		do_teach,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "campo",			 "camp",		do_camp,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "scava",			 "dig",			do_dig,				POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "coda",			 "tail",		do_tail,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "spingi",			 "push",		do_push,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "purifica",		 "purify",		do_purify,			POS_REST,	0,	 LOG_NORMAL, 0, 0						},
	{ "pretendi",		 "demand",		do_demand,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "fascia",			 "bandage",		do_bandage,			POS_FIGHT,	0,	 LOG_NORMAL, 1, 0						},
	{ "tira",			 "shoot",		do_shoot,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "katana",			 "katana",		do_katana,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "controlla",		 "control",		do_control,			POS_STAND,	0,	 LOG_NORMAL, 1, 0						},
	{ "sensi",			 "sense",		do_sense,			POS_REST,	0,	 LOG_NORMAL, 1, 0						},
	{ "giudica",		 "judge",		do_judge,			POS_REST,	0,	 LOG_ALWAYS, 1, CMD_KEEP_HIDE			},
	{ "vistavera",		 "truesight",	do_truesight,		POS_FIGHT,	0,	 LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},


	/*
	 * Immortal commands.
	 */
	{ "slist",			 "slist",		do_slist,			POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "at",				 "at",			do_at,				POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "atitle",			 "atitle",		do_atitle,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "advance",		 "advance",		do_advance,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "dump",			 "dump",		do_dump,			POS_DEAD,	IMP, LOG_ALWAYS, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "rename",			 "rename",		do_rename,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "violate",		 "violate",		do_violate,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "trust",			 "trust",		do_trust,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "tvalue",			 "tvalue",		do_tvalue,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "allow",			 "allow",		do_allow,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "ban",			 "ban",			do_ban,				POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "deny",			 "deny",		do_deny,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "disconnect",		 "disconnect",	do_disconnect,		POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "flag",			 "flag",		do_flag,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "freeze",			 "freeze",		do_freeze,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "gwho",			 "gwho",		do_gwho,			POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "permban",		 "permban",		do_permban,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "protect",		 "protect",		do_protect,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "reboo",			 "reboo",		do_reboo,			POS_DEAD,	IMP, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "reboot",			 "reboot",		do_reboot,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "grant",			 "grant",		do_grant,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "smit",			 "smit",		do_smit,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "smite",			 "smite",		do_smite,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "limited",		 "limited",		do_limited,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "lookup",			 "lookup",		do_slookup,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "popularity",		 "popularity",	do_popularity,		POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "shutdow",		 "shutdow",		do_shutdow,			POS_DEAD,	IMP, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "shutdown",		 "shutdown",	do_shutdown,		POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sockets",		 "sockets",		do_sockets,			POS_DEAD,	CRE, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "wizlock",		 "wizlock",		do_wizlock,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "affrooms",		 "affrooms",	do_affrooms,		POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "force",			 "force",		do_force,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "forcerent",		"forcerent",	do_forcerent,		POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "load",			 "load",		do_load,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "newlock",		 "newlock",		do_newlock,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "noaffect",		 "noaffect",	do_noaffect,		POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "nochanells",		 "nochannels",	do_nochannels,		POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "noemote",		 "noemote",		do_noemote,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "noyell",			 "noyell",		do_noyell,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "notell",			 "notell",		do_notell,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "pecho",			 "pecho",		do_pecho,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "purge",			 "purge",		do_purge,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "restore",		 "restore",		do_restore,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sla",			 "sla",			do_sla,				POS_DEAD,	CRE, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "slay",			 "slay",		do_slay,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "teleport",		 "teleport",	do_transfer,		POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "tick",			 "tick",		do_tick,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "transfer",		 "transfer",	do_transfer,		POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "gecho",			 "gecho",		do_echo,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "holylight",		 "holylight",	do_holylight,		POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "incognito",		 "incognito",	do_incognito,		POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "invis",			 "invis",		do_invis,			POS_DEAD,	ADM, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "iget",			 "iget",		do_iget,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST },
	{ "log",			 "log",			do_log,				POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "memory",			 "memory",		do_memory,			POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "mwhere",			 "mwhere",		do_mwhere,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "owhere",			 "owhere",		do_owhere,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "peace",			 "peace",		do_peace,			POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "penalty",		 "penalty",		do_penalty,			POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "echo",			 "echo",		do_recho,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "return",			 "return",		do_return,			POS_DEAD,	DEI, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "snoop",			 "snoop",		do_snoop,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "string",			 "string",		do_string,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "switch",			 "switch",		do_switch,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "wizinvis",		 "wizinvis",	do_invis,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "vnum",			 "vnum",		do_vnum,			POS_DEAD,	CRE, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "zecho",			 "zecho",		do_zecho,			POS_DEAD,	CRE, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "where",			 "where",		do_where,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "clone",			 "clone",		do_clone,			POS_DEAD,	DEI, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "wiznet",			 "wiznet",		do_wiznet,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "admtalk",		 "admtalk",		do_admtalk,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "imotd",			 "imotd",		do_imotd,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "prefi",			 "prefi",		do_prefi,			POS_DEAD,	ADM, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "prefix",			 "prefix",		do_prefix,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "objlist",		 "objlist",		do_objlist,			POS_DEAD,	IMP, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "maximum",		 "maximum",		do_maximum,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "mpract",			 "mpract",		do_mpract,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST },
	{ "find",			 "find",		do_find,			POS_DEAD,	IMP, LOG_ALWAYS, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "worth",			 "worth",		do_worth,			POS_DEAD,	ADM, LOG_NORMAL, 1, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "sposa",			 "marry",		do_marry,			POS_DEAD,	DEI, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "divorzia",		 "divorce",		do_divorce,			POS_DEAD,	DEI, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},
	{ "anelli",			 "rings",		do_rings,			POS_DEAD,	DEI, LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	},

	/*
	 * End of list.
	 */
	{ "",				 "",			0,					POS_DEAD,	0,	 LOG_NORMAL, 0, CMD_KEEP_HIDE|CMD_GHOST	}
};


/*
 * The main entry point for executing commands. Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument, bool is_order )
{
	char	command[MIL];
	char	logline[MIL];
#ifdef IMMORTALS_LOGS
	char	buf[MIL];
	char	*strtime;
#endif
	int		cmd;
	int		trust;
	bool	found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Strip leading spaces. */
	smash_tilde( argument );
	while ( isspace(*argument) )
		argument++;
	if ( argument[0] == '\0' )
		return;

	/* Implement freeze command. */
	if ( !IS_MOB(ch) && IS_SET(ch->act, PLR_FREEZE) )
	{
		send_to_char( "Sei totalmente ghiacciato!\n\r", ch );
		return;
	}

	/* Grab the command word. Special parsing so ' can be a command, also no spaces needed after punctuation. */
	strcpy( logline, argument );

#ifdef IMMORTALS_LOGS
	if ( IS_ADMIN(ch) )
	{
		if ( (imm_log = fopen(IMM_LOG_FILE, "a+")) == NULL )
			bug( "interpret: cannot open imm_log_file", 0 );
		else
		{
			strtime = (char *) malloc( 100 );
			strtime = ctime( &current_time );
			strtime[strlen( strtime ) - 1] = '\0';	/* (FF) Xandra prompt problema dei mob */
			sprintf( buf, "%s :[%s]:%s\n", strtime, ch->name, logline );
			fprintf( imm_log, buf );
			fclose( imm_log );
			free( strtime );
		}
	}
#endif

	if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
	{
		command[0] = argument[0];
		command[1] = '\0';
		argument++;
		while ( isspace(*argument) )
			argument++;
	}
	else
		argument = one_argument( argument, command );

	/* Look for command in command table. */
	found = FALSE;
	trust = get_trust( ch );
	for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	{
		if ( ((!IS_SET(ch->act, PLR_ENGLISH)
			&& command[0] == cmd_table[cmd].name[0]
			&& !str_prefix(command, cmd_table[cmd].name))
		|| (IS_SET(ch->act, PLR_ENGLISH)
			&& command[0] == cmd_table[cmd].english[0]
			&& !str_prefix(command, cmd_table[cmd].english)))
		&& cmd_table[cmd].level <= trust )
		{
			/* Implement charmed mobs commands. */
			if ( !is_order && IS_AFFECTED(ch, AFF_CHARM) )
			{
				send_to_char( "Prima chiedi al tuo amato maestro!\n\r", ch );
				return;
			}

			if ( IS_AFFECTED(ch, AFF_STUN) && !(cmd_table[cmd].extra & CMD_KEEP_HIDE) )
			{
				send_to_char( "Sei troppo stordito per fare cio'.\n\r", ch );
				return;
			}

			/* Come out of hiding for most commands */
			if ( IS_AFFECTED(ch, AFF_HIDE) && !IS_MOB(ch) && !(cmd_table[cmd].extra & CMD_KEEP_HIDE) )
			{
				REMOVE_BIT( ch->affected_by, AFF_HIDE );
				send_to_char( "Esci dall'ombra.\n\r", ch );
				act( "$n esce dall'ombra.", ch, NULL, NULL, TO_ROOM, POS_REST );
			}

			if ( IS_AFFECTED(ch, AFF_FADE) && !IS_MOB(ch) && !(cmd_table[cmd].extra & CMD_KEEP_HIDE) )
			{
				REMOVE_BIT( ch->affected_by, AFF_FADE );
				send_to_char( "Esci dall'ombra.\n\r", ch );
				act( "$n esce dall'ombra.", ch, NULL, NULL, TO_ROOM, POS_REST );
			}

			if ( IS_AFFECTED(ch, AFF_IMP_INVIS) && !IS_MOB(ch) && (cmd_table[cmd].position == POS_FIGHT) )
			{
				affect_strip( ch, gsn_imp_invis );
				REMOVE_BIT( ch->affected_by, AFF_IMP_INVIS );
				send_to_char( "Appari di nuovo.\n\r", ch );
				act( "$n appare lentamente.", ch, NULL, NULL, TO_ROOM, POS_REST );
			}

			if ( CAN_DETECT(ch, ADET_EARTHFADE) && !IS_MOB(ch) && (cmd_table[cmd].position == POS_FIGHT) )
			{
				affect_strip( ch, gsn_earthfade );
				REMOVE_BIT( ch->detection, ADET_EARTHFADE );
				WAIT_STATE( ch, (PULSE_VIOLENCE / 2) );
				send_to_char( "Lentamente ritorni aella tua forma neutrale.\n\r", ch );
				act( "La terra plasma $n davanti ai tuoi occhi.", ch, NULL, NULL, TO_ROOM, POS_REST );
			}

			/* prevent ghosts from doing a bunch of commands */
			if ( IS_SET(ch->act, PLR_GHOST) && !IS_MOB(ch) && !(cmd_table[cmd].extra & CMD_GHOST) )
				continue;

			found = TRUE;
			break;
		}
	}

	/* Log and snoop. */
	if ( cmd_table[cmd].log == LOG_NEVER )
		strcpy( logline, "" );

	if ( ((!IS_MOB(ch) && IS_SET(ch->act, PLR_LOG)) || fLogAll || cmd_table[cmd].log == LOG_ALWAYS)
	&&	logline[0] != '\0'
	&&	logline[0] != '\n' )
	{
		sprintf( log_buf, "Log %s: %s", ch->name, logline );
		wiznet( log_buf, ch, NULL, WIZ_SECURE, get_trust(ch) );
		log_string( log_buf );
	}

	if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
	{
		write_to_buffer( ch->desc->snoop_by, "# ", 2 );
		write_to_buffer( ch->desc->snoop_by, logline, 0 );
		write_to_buffer( ch->desc->snoop_by, "\n\r", 2 );
	}

	if ( !found )
	{
		/* Look for command in socials table. */
		if ( !check_social(ch, command, argument) )
		{
			send_to_char( "Come?\n\r", ch );
			return;
		}
		else
			return;
	}

	/* Character not in position for command? */
	if ( ch->position < cmd_table[cmd].position )
	{
		switch ( ch->position )
		{
		case POS_DEAD:
			act( "Sei mort$x!!", ch, NULL, NULL, TO_CHAR, POS_DEAD );
			break;

		case POS_MORTAL:
		case POS_INCAP:
			act( "Sei troppo ferit$x per poterlo fare.", ch, NULL, NULL, TO_CHAR, POS_MORTAL );
			break;

		case POS_STUN:
			act( "Sei troppo stordit$x e non riesci a fare molto.", ch, NULL, NULL, TO_CHAR, POS_STUN );
			break;

		case POS_SLEEP:
			act( "Nei tuoi sogni?", ch, NULL, NULL, TO_CHAR, POS_SLEEP );
			break;

		case POS_REST:
			act( "Nah.. Ti senti troppo rilassat$x..", ch, NULL, NULL, TO_CHAR, POS_REST );
			break;

		case POS_SIT:
			act( "Meglio alzarsi in piedi prima di tutto.", ch, NULL, NULL, TO_CHAR, POS_REST );
			break;

		case POS_FIGHT:
			act( "Non puoi! Stai combattendo!", ch, NULL, NULL, TO_CHAR, POS_REST );
			break;
		}

		return;
	}

	/* Dispatch the command. */
	( *cmd_table[cmd].do_fun ) ( ch, argument );

	tail_chain( );
}


char *translate_command( char *argument, CHAR_DATA *ch, CHAR_DATA *victim )
{
	int cmd;

	if ( !ch )
	{
		bug( "translate_command: ch e' NULL.", 0 );
		return "";
	}

	if ( !victim )
	{
		bug( "translate_command: victim e' NULL.", 0 );
		return "";
	}

	if ( IS_SET(ch->act, PLR_ENGLISH) && !IS_SET(victim->act, PLR_ENGLISH) )
	{
		for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
		{
			if ( argument[0] != cmd_table[cmd].english[0] )
				continue;
			if ( !str_prefix(argument, cmd_table[cmd].english) )
				return cmd_table[cmd].name;
		}
	}
	else if ( !IS_SET(ch->act, PLR_ENGLISH) && IS_SET(victim->act, PLR_ENGLISH) )
	{
		for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
		{
			if ( argument[0] != cmd_table[cmd].name[0] )
				continue;
			if ( !str_prefix(argument, cmd_table[cmd].name) )
				return cmd_table[cmd].english;
		}
	}

	return argument;
}


bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
	char		arg[MIL];
	CHAR_DATA	*victim;
	int			cmd;
	bool		found;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	found = FALSE;
	for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
	{
		if ( command[0] == social_table[cmd].name[0] && !str_prefix(command, social_table[cmd].name) )
		{
			found = TRUE;
			break;
		}
	}

	if ( !found )
		return( FALSE );

	if ( !IS_MOB(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
	{
		send_to_char( "Non ti e' permesso esprimerti socialmente!\n\r", ch );
		return( TRUE );
	}

	switch ( ch->position )
	{
	case POS_DEAD:
		act( "Sei mort$x!!.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return( TRUE );

	case POS_INCAP:
	case POS_MORTAL:
		act( "Sei troppo ferit$x per poterlo fare.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return( TRUE );

	case POS_STUN:
		act( "Sei troppo stordit$x e non riesci a fare molto.", ch, NULL, NULL, TO_CHAR, POS_REST );
		return( TRUE );

	case POS_SLEEP:
		/* I just know this is the path to a 12" 'if' statement. :( But two players asked for it already! -- Furey */
		if ( !str_cmp(social_table[cmd].name, "snore") )
			break;
		send_to_char( "Nei tuoi sogni?\n\r", ch );
		return( TRUE );
	}

	if ( IS_AFFECTED(ch, AFF_HIDE) )
	{
		REMOVE_BIT( ch->affected_by, AFF_HIDE );
		send_to_char( "Esci dall'ombra.\n\r", ch );
		act( "$n esce dall'ombra.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_AFFECTED(ch, AFF_FADE) )
	{
		REMOVE_BIT( ch->affected_by, AFF_FADE );
		send_to_char( "Esci dall'ombra.\n\r", ch );
		act( "$n esce dall'ombra.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( IS_AFFECTED(ch, AFF_IMP_INVIS) && !IS_MOB(ch) && (cmd_table[cmd].position == POS_FIGHT) )
	{
		affect_strip( ch, gsn_imp_invis );
		REMOVE_BIT( ch->affected_by, AFF_IMP_INVIS );
		send_to_char( "Appari di nuovo.\n\r", ch );
		act( "$n appare lentamente.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	if ( CAN_DETECT(ch, ADET_EARTHFADE) && !IS_MOB(ch) && (cmd_table[cmd].position == POS_FIGHT) )
	{
		affect_strip( ch, gsn_earthfade );
		REMOVE_BIT( ch->detection, ADET_EARTHFADE );
		WAIT_STATE( ch, (PULSE_VIOLENCE / 2) );
		send_to_char( "Lentamente ritorni nella tua forma neutrale.\n\r", ch );
		act( "La terra forma $n davanti ai tuoi occhi.", ch, NULL, NULL, TO_ROOM, POS_REST );
	}

	one_argument( argument, arg );
	victim = NULL;
	if ( arg[0] == '\0' )
	{
		act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM, POS_REST );
		act( social_table[cmd].char_no_arg, ch, NULL, victim, TO_CHAR, POS_REST );
	}
	else if ( (victim = get_char_room(ch, arg)) == NULL )
	{
		send_to_char( "Non c'e' qui.\n\r", ch );
	}
	else if ( victim == ch )
	{
		act( social_table[cmd].others_auto, ch, NULL, victim, TO_ROOM, POS_REST );
		act( social_table[cmd].char_auto, ch, NULL, victim, TO_CHAR, POS_REST );
	}
	else
	{
		act( social_table[cmd].others_found, ch, NULL, victim, TO_NOTVICT, POS_REST );
		act( social_table[cmd].char_found, ch, NULL, victim, TO_CHAR, POS_REST );
		act( social_table[cmd].vict_found, ch, NULL, victim, TO_VICT, POS_REST );

		if ( !IS_MOB(ch) && IS_MOB(victim) && !IS_AFFECTED(victim, AFF_CHARM) && IS_AWAKE(victim) && victim->desc == NULL )
		{
			switch ( number_bits(4) )
			{
			case 0:
			case 1:
			case 2:
				break;

			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				act( social_table[cmd].others_found, victim, NULL, ch, TO_NOTVICT, POS_REST );
				act( social_table[cmd].char_found, victim, NULL, ch, TO_CHAR, POS_REST );
				act( social_table[cmd].vict_found, victim, NULL, ch, TO_VICT, POS_REST );
				break;

			case 9:
			case 10:
			case 11:
			case 12:
				act( "$n guarda con perplessita' $N.", victim, NULL, ch, TO_NOTVICT, POS_REST );
				act( "Guardi con perplessita' $N.", victim, NULL, ch, TO_CHAR, POS_REST );
				act( "$n ti guarda con perplessita'.", victim, NULL, ch, TO_VICT, POS_REST );
				break;

			}
		}
	}

	return( TRUE );
}


/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
	if ( *arg == '\0' )
		return( FALSE );

	if ( *arg == '+' || *arg == '-' )
		arg++;

	for ( ; *arg != '\0'; arg++ )
	{
		if ( !isdigit(*arg) )
			return( FALSE );
	}

	return( TRUE );
}


/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
	char	*pdot;
	int		number;
	/*~~~~~~~~~~~*/

	for ( pdot = argument; *pdot != '\0'; pdot++ )
	{
		if ( *pdot == '.' )
		{
			*pdot = '\0';
			number = atoi( argument );
			*pdot = '.';
			strcpy( arg, pdot + 1 );
			return( number );
		}
	}

	strcpy( arg, argument );
	return( 1 );
}


/*
 * Given a string like 14*foo, return 14 and 'foo'
 */
int mult_argument( char *argument, char *arg )
{
	char	*pdot;
	int		number;
	/*~~~~~~~~~~~*/

	for ( pdot = argument; *pdot != '\0'; pdot++ )
	{
		if ( *pdot == '*' )
		{
			*pdot = '\0';
			number = atoi( argument );
			*pdot = '*';
			strcpy( arg, pdot + 1 );
			return( number );
		}
	}

	strcpy( arg, argument );
	return( 1 );
}


/*
 * Pick off one argument from a string and return the rest. Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
	char	cEnd;
	/*~~~~~~~~~*/

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

		*arg_first = LOWER( *argument );
		arg_first++;
		argument++;
	}

	*arg_first = '\0';

	while ( isspace(*argument) )
		argument++;

	return( argument );
}


/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
	char	buf[MSL];
	char	output[4 * MSL];
	int		letter;
	int		cmd;
	int		col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	col = 0;
	output[0] = '\0';

	for ( letter = 'a'; letter <= 'z'; letter++ )
	{
		if ( IS_SET(ch->act, PLR_ENGLISH) )
		{
			for ( cmd = 0; cmd_table[cmd].english[0] != '\0'; cmd++ )
			{
				if (cmd_table[cmd].english[0] == letter
				&&	cmd_table[cmd].level < LEVEL_HERO
				&&	cmd_table[cmd].level <= get_trust(ch)
				&&	cmd_table[cmd].show )
				{
					sprintf( buf, "%-16s", cmd_table[cmd].english );
					strcat( output, buf );
					if ( ++col % 5 == 0 )
						strcat( output, "\n\r" );
				}
			}
		}
		else
		{
			for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
			{
				if (cmd_table[cmd].name[0] == letter
				&&	cmd_table[cmd].level < LEVEL_HERO
				&&	cmd_table[cmd].level <= get_trust(ch)
				&&	cmd_table[cmd].show )
				{
					sprintf( buf, "%-16s", cmd_table[cmd].name );
					strcat( output, buf );
					if ( ++col % 5 == 0 )
						strcat( output, "\n\r" );
				}
			}
		}
	}

	if ( col % 6 != 0 )
		strcat( output, "\n\r" );

	page_to_char( output, ch );
}


void do_wizhelp( CHAR_DATA *ch, char *argument )
{
	char	output[MSL*4];
	int		lev;
	int		cmd;
	int		col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( lev = LEVEL_ADMIN; lev <= get_trust(ch); lev++ )
	{
		char	letter;
		col = 0;
		output[0] = '\0';
		/*~~~~~~~~~~~~~*/

		if ( lev != LEVEL_ADMIN )
			strcat( output, "\n\r\n\r" );
		strcat( output, CLR(ch, WHITE) );
		switch ( lev )
		{
		case ADM:	strcat( output, "Staff (97)" );		break;
		case DEI:	strcat( output, "Master (98)" );	break;
		case CRE:	strcat( output, "Builder (99)" );	break;
		case IMP:	strcat( output, "Imple (100)" );	break;
		default :	strcat( output, "Sconosciuta" );	break;
		}
		strcat( output, CLR(ch, WHITE_BOLD) );
		strcat( output, "\n\r" );

		for ( letter = 'a'; letter <= 'z'; letter++ )
		{
			for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
			{
				char	buf[MSL];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if ( cmd_table[cmd].name[0] != letter )
					continue;
				if ( cmd_table[cmd].level != lev )
					continue;
				if ( cmd_table[cmd].show )
				{
					sprintf( buf, "%-12s", cmd_table[cmd].name );
					strcat( output, buf );
					if ( ++col % 6 == 0 )
						strcat( output, "\n\r" );
				}
			}
		}
		if ( col % 6 != 0 )
			strcat( output, "\n\r" );

		page_to_char( output, ch );
	}
}


/*********** alias.c **************/

/*
 *  Alias e altro
 */
void substitute_alias( DESCRIPTOR_DATA *d, char *argument )
{
	CHAR_DATA	*ch;
	char		buf[MSL],
				prefix[MIL],
				name[MIL];
	char		*point;
	int			alias;

	ch = d->original ? d->original : d->character;

	/* controlla il prefisso */
	if ( ch->prefix[0] != '\0' && str_prefix("prefix",argument) )
	{
		if ( strlen(ch->prefix) + strlen(argument) > MIL )
			send_to_char( "Errore, linea troppo lunga, inserirne una piu' breve.\r\n", ch );
		else
		{
			sprintf( prefix, "%s %s", ch->prefix, argument );
			argument = prefix;
		}
	}

	if ( IS_MOB(ch) || ch->pcdata->alias[0] == NULL
	||	!str_prefix("alias",argument) || !str_prefix("una",argument)
	||	!str_prefix("prefix",argument) )
	{
		interpret(d->character,argument, FALSE);
		return;
	}

	strcpy(buf,argument);

	for ( alias = 0; alias < MAX_ALIAS; alias++ )	/* go through the aliases */
	{
		if ( ch->pcdata->alias[alias] == NULL )
			break;

		if ( !str_prefix(ch->pcdata->alias[alias],argument) )
		{
			point = one_argument( argument, name );
			if ( !strcmp(ch->pcdata->alias[alias],name) )
			{
				buf[0] = '\0';
				strcat( buf, ch->pcdata->alias_sub[alias] );
				strcat( buf, " " );
				strcat( buf, point );
				break;
			}

			if ( strlen(buf) > MIL )
			{
				send_to_char("Alias sostitutivo troppo lungo, troncato.\r\n",ch);
				buf[MIL -1] = '\0';
			}
		}
	}

	interpret( d->character, buf, FALSE );
}


void do_alia(CHAR_DATA *ch, char *argument)
{
	send_to_char( "Digita alias per esteso, grazie.\n\r", ch );
}

void do_alias(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA	*rch;
	char		arg[MIL],
				buf[MSL];
	int			pos;

	smash_tilde( argument );

	if ( ch->desc == NULL )
		rch = ch;
	else
		rch = ch->desc->original ? ch->desc->original : ch;

	if ( IS_MOB(rch) )
		return;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		if ( rch->pcdata->alias[0] == NULL )
		{
			send_to_char( "Non hai definito alcun alias per ora.\n\r", ch );
			return;
		}
		send_to_char( "I tuoi alias attualmente definiti sono:\n\r", ch );

		for ( pos = 0; pos < MAX_ALIAS; pos++ )
		{
			if ( rch->pcdata->alias[pos] == NULL
			||	 rch->pcdata->alias_sub[pos] == NULL )
				break;

			sprintf( buf,"    %s:  %s\n\r",rch->pcdata->alias[pos],
				rch->pcdata->alias_sub[pos] );
			send_to_char(buf,ch);
		}
		return;
	}

	if ( !str_prefix("una",arg) || !str_cmp("alias",arg) )
	{
		send_to_char("Spiacente, quella parola e' riservata ad altre funzioni primarie.\n\r",ch);
		return;
	}

	if ( argument[0] == '\0' )
	{
		for ( pos = 0; pos < MAX_ALIAS; pos++ )
		{
			if ( rch->pcdata->alias[pos] == NULL
			||	 rch->pcdata->alias_sub[pos] == NULL )
				break;

			if ( !str_cmp(arg,rch->pcdata->alias[pos]) )
			{
				sprintf( buf, "%s diventa alias per '%s'.\n\r", rch->pcdata->alias[pos],
					rch->pcdata->alias_sub[pos] );
				send_to_char( buf, ch );
				return;
			}
		}

		send_to_char( "Questo alias non e' definito.\n\r", ch );
		return;
	}

	if ( !str_prefix(argument,"delete") || !str_prefix(argument,"prefix") )
	{
		send_to_char( "Attenzione, e' una mossa pericolosa!\n\r", ch );
		return;
	}

	for ( pos = 0; pos < MAX_ALIAS; pos++ )
	{
		if ( rch->pcdata->alias[pos] == NULL )
			break;

		if ( !str_cmp(arg,rch->pcdata->alias[pos]) ) /* ridefinisce un alias */
		{
			free_string( rch->pcdata->alias_sub[pos] );
			rch->pcdata->alias_sub[pos] = str_dup( argument );
			sprintf( buf, "%s viene ora riassegnato a '%s'.\n\r", arg, argument );
			send_to_char( buf, ch );
			return;
		}
	}

	if ( pos >= MAX_ALIAS )
	{
		send_to_char( "Spiacente, non hai piu' alias a disposizione.\n\r", ch );
		return;
	}

	/*definizione di un alias */
	rch->pcdata->alias[pos]		= str_dup( arg );
	rch->pcdata->alias_sub[pos]	= str_dup( argument );
	sprintf( buf, "%s e' ora alias per '%s'.\n\r", arg, argument );
	send_to_char( buf, ch );
}


void do_unalias( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA	*rch;
	char		arg[MIL];
	int			pos;
	bool		found = FALSE;

	if ( ch->desc == NULL )
		rch = ch;
	else
		rch = ch->desc->original ? ch->desc->original : ch;

	if ( IS_MOB(rch) )
		return;

	argument = one_argument( argument, arg );

	if ( arg == '\0' )
	{
		send_to_char( "Oltre al comando devi anche inserire la parola il cui alias va eliminato.\n\r", ch );
		return;
	}

	for ( pos = 0; pos < MAX_ALIAS; pos++ )
	{
		if ( rch->pcdata->alias[pos] == NULL )
			break;

		if ( found )
		{
			rch->pcdata->alias[pos-1]		= rch->pcdata->alias[pos];
			rch->pcdata->alias_sub[pos-1]	= rch->pcdata->alias_sub[pos];
			rch->pcdata->alias[pos]			= NULL;
			rch->pcdata->alias_sub[pos]		= NULL;
			continue;
		}

		if ( !strcmp(arg,rch->pcdata->alias[pos]) )
		{
			send_to_char( "Alias rimosso.\n\r", ch );
			free_string( rch->pcdata->alias[pos] );
			free_string( rch->pcdata->alias_sub[pos] );
			rch->pcdata->alias[pos] = NULL;
			rch->pcdata->alias_sub[pos] = NULL;
			found = TRUE;
		}
	}

	if ( !found )
		send_to_char( "Non hai alias definiti con quel termine.\n\r", ch );
}
