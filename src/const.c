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
#include <time.h>

#include "xandra.h"
#include "magic.h"


/* language staff */
const struct translation_type translation_table[]=
{
	{ "a",	"e" },
	{ "A",	"E" },
	{ "b",	"c" },
	{ "B",	"C" },
	{ "c",	"d" },
	{ "C",	"D" },
	{ "d",	"f" },
	{ "D",	"F" },
	{ "e",	"i" },
	{ "E",	"I" },
	{ "f",	"g" },
	{ "F",	"G" },
	{ "g",	"h" },
	{ "G",	"H" },
	{ "h",	"j" },
	{ "H",	"J" },
	{ "i",	"o" },
	{ "I",	"O" },
	{ "j",	"k" },
	{ "J",	"K" },
	{ "k",	"l" },
	{ "K",	"L" },
	{ "l",	"m" },
	{ "L",	"M" },
	{ "m",	"n" },
	{ "M",	"N" },
	{ "n",	"p" },
	{ "N",	"P" },
	{ "o",	"u" },
	{ "O",	"U" },
	{ "p",	"q" },
	{ "P",	"Q" },
	{ "q",	"r" },
	{ "Q",	"R" },
	{ "r",	"s" },
	{ "R",	"S" },
	{ "s",	"t" },
	{ "S",	"T" },
	{ "t",	"v" },
	{ "T",	"V" },
	{ "u",	"y" },
	{ "U",	"Y" },
	{ "v",	"w" },
	{ "V",	"W" },
	{ "w",	"x" },
	{ "W",	"X" },
	{ "x",	"z" },
	{ "X",	"Z" },
	{ "y",	"a" },
	{ "Y",	"A" },
	{ "z",	"b" },
	{ "Z",	"B" },
	{ "", 	""  }
};


const struct language_type language_table[ MAX_LANGUAGE ] =
{
	{ "comune",		LANG_COMMON   },
	{ "umano",		LANG_HUMAN    },
	{ "elfico",		LANG_ELVISH   },
	{ "nanico",		LANG_DWARVISH },
	{ "gnomese",	LANG_GNOMISH  },
	{ "titanico",	LANG_GIANT    },
	{ "trollese",	LANG_TROLLISH },
	{ "felino",		LANG_CAT      }
};

/* item type list */
const struct item_type item_table[] =
{
	{ ITEM_LIGHT,		"light",		"luce"					},
	{ ITEM_SCROLL,		"scroll",		"pergamena"				},
	{ ITEM_WAND,		"wand",			"bacchetta"				},
	{ ITEM_STAFF,		"staff",		"bastone"				},
	{ ITEM_WEAPON,		"weapon",		"arma"					},
	{ ITEM_TREASURE,	"treasure",		"tesoro"				},
	{ ITEM_ARMOR,		"armor",		"armatura"				},
	{ ITEM_POTION,		"potion",		"pozione"				},
	{ ITEM_CLOTHING,	"clothing",		"vestito"				},
	{ ITEM_FURNITURE,	"furniture",	"mobile"				},
	{ ITEM_TRASH,		"trash",		"spazzatura"			},
	{ ITEM_CONTAINER,	"container",	"contenitore"			},
	{ ITEM_DRINK_CON,	"drink",		"recipiente"			},
	{ ITEM_KEY,			"key",			"chiave"				},
	{ ITEM_FOOD,		"food",			"cibo"					},
	{ ITEM_MONEY,		"money",		"moneta"				},
	{ ITEM_BOAT,		"boat",			"barca"					},
	{ ITEM_CORPSE_NPC,	"npc_corpse",	"cadavere"				},
	{ ITEM_CORPSE_PC,	"pc_corpse",	"cadavere"				},
	{ ITEM_FOUNTAIN,	"fountain",		"fontana"				},
	{ ITEM_PILL,		"pill",			"pillola"				},
	{ ITEM_PROTECT,		"protect",		"prote"					},
	{ ITEM_MAP,			"map",			"mappa"					},
	{ ITEM_PORTAL,		"portal",		"portale"				},
	{ ITEM_WARP_STONE,	"warp_stone",	"pietra dimensionale"	},
	{ ITEM_ROOM_KEY,	"room_key",		"chiave"				},
	{ ITEM_GEM,			"gem",			"gemma"					},
	{ ITEM_JEWELRY,		"jewelry",		"gioiello"				},
	{ ITEM_JUKEBOX,		"jukebox",		"carillon"				},
	{ ITEM_TATTOO,		"tattoo",		"tatuaggio"				},
	{ 0,				NULL,			NULL					}
};


/* weapon selection table */
const struct weapon_type weapon_table[]  =
{
	{ "sword",		"spada",	 	OBJ_VNUM_SCHOOL_SWORD,		WEAPON_SWORD,	&gsn_sword	 },
	{ "mace",		"mazza",	 	OBJ_VNUM_SCHOOL_MACE,		WEAPON_MACE,	&gsn_mace	 },
	{ "dagger",		"pugnale", 		OBJ_VNUM_SCHOOL_DAGGER,		WEAPON_DAGGER,	&gsn_dagger	 },
	{ "axe",		"ascia",	 	OBJ_VNUM_SCHOOL_AXE,		WEAPON_AXE,		&gsn_axe	 },
	{ "staff",		"bastone",		OBJ_VNUM_SCHOOL_STAFF,		WEAPON_SPEAR,	&gsn_spear	 },
	{ "flail",		"flagello",		OBJ_VNUM_SCHOOL_FLAIL,		WEAPON_FLAIL,	&gsn_flail	 },
	{ "whip",		"frusta",	 	OBJ_VNUM_SCHOOL_WHIP,		WEAPON_WHIP,	&gsn_whip	 },
	{ "polearm",	"alabarda", 	OBJ_VNUM_SCHOOL_POLEARM,	WEAPON_POLEARM,	&gsn_polearm },
	{ "bow",		"arco",	 		OBJ_VNUM_SCHOOL_BOW,		WEAPON_BOW,		&gsn_bow	 },
	{ "arrow",		"freccia",		OBJ_VNUM_SCHOOL_POLEARM,	WEAPON_ARROW,	&gsn_arrow	 },
	{ "lance",		"lancia",	 	OBJ_VNUM_SCHOOL_LANCE,		WEAPON_LANCE,	&gsn_lance	 },
	{ NULL,		 	NULL,			0,							0,				NULL		 }
};


/*
 * wiznet table and prototype for future flag setting
 */
const struct wiznet_type wiznet_table[] =
{
	{ "ticks",			WIZ_TICKS,		LEVEL_ADMIN			},
	{ "logins",			WIZ_LOGINS,		LEVEL_ADMIN			},
	{ "links",			WIZ_LINKS,		LEVEL_MASTER		},
	{ "deaths",			WIZ_DEATHS,		LEVEL_ADMIN			},
	{ "resets",			WIZ_RESETS,		LEVEL_CREATOR		},
	{ "(mobdeaths)",	WIZ_MOBDEATHS,	LEVEL_CREATOR		},	/* (GG) */
	{ "penalties",		WIZ_PENALTIES,	LEVEL_MASTER		},
	{ "saccing",		WIZ_SACCING,	LEVEL_MASTER		},
	{ "levels",			WIZ_LEVELS,		LEVEL_ADMIN			},
	{ "secure",			WIZ_SECURE,		LEVEL_IMPLEMENTOR	},
	{ "switches",		WIZ_SWITCHES,	LEVEL_CREATOR		},
	{ "snoops",			WIZ_SNOOPS,		LEVEL_CREATOR		},
	{ "restore",		WIZ_RESTORE,	LEVEL_CREATOR		},
	{ "load",			WIZ_LOAD,		LEVEL_CREATOR		},
	{ "spam",			WIZ_SPAM,		LEVEL_MASTER		},
	{ "(limited)",		WIZ_LIMITED,	LEVEL_MASTER		},	/* (GG) */
	{ NULL,				0,				0					}
};


/* attack table */
/* f = femminile, m = maschile */
const struct attack_type attack_table[] =
{
	{ "none",		"colpo",				 'm',	-1			  },  /*  0 */
	{ "slice",		"taglio",				 'm',	DAM_SLASH	  },
	{ "stab",		"pugnalata alle spalle", 'f',	DAM_PIERCE	  },
	{ "slash",		"fendente",				 'm',	DAM_SLASH	  },
	{ "whip",		"frustata",				 'f',	DAM_SLASH	  },
	{ "claw",		"artigliata",			 'f',	DAM_SLASH	  },  /*  5 */
	{ "blast",		"esplosione",			 'f',	DAM_BASH	  },
	{ "pound",		"martellata",			 'f',	DAM_BASH	  },
	{ "crush",		"mazzata",				 'f',	DAM_BASH	  },
	{ "grep",		"presa",				 'f',	DAM_SLASH	  },
	{ "bite",		"morso",				 'm',	DAM_PIERCE	  },  /* 10 */
	{ "pierce",		"coltellata",			 'f',	DAM_PIERCE	  },
	{ "suction",	"risucchio",			 'm',	DAM_BASH	  },
	{ "beating",	"bastonata",			 'f',	DAM_BASH	  },
	{ "digestion",	"digestione",			 'm',	DAM_ACID	  },
	{ "charge",		"carica",				 'f',	DAM_BASH	  },  /* 15 */
	{ "slap",		"sberla",				 'f',	DAM_BASH	  },
	{ "punch",		"pugno",				 'm',	DAM_BASH	  },
	{ "wrath",		"collera",				 'f',	DAM_ENERGY	  },
	{ "magic",		"magia",				 'f',	DAM_ENERGY	  },
	{ "divine",		"potere divino",		 'm',	DAM_HOLY	  },  /* 20 */
	{ "cleave",		"spaccatura",			 'f',	DAM_SLASH	  },
	{ "scratch",	"graffio",				 'm',	DAM_PIERCE	  },
	{ "peck",		"beccata",				 'f',	DAM_PIERCE	  },
	{ "peckb",		"beccata",				 'f',	DAM_BASH	  },
	{ "chop",		"morso",				 'f',	DAM_SLASH	  },  /* 25 */
	{ "sting",		"puntura",				 'f',	DAM_PIERCE	  },
	{ "smash",		"sfondata",				 'f',	DAM_BASH	  },
	{ "shbite",		"scarica elettrica",	 'f',	DAM_LIGHTNING },
	{ "flbite",		"massa di fuoco",		 'f',	DAM_FIRE	  },
	{ "frbite",		"massa di ghiaccio",	 'f',	DAM_COLD	  },  /* 30 */
	{ "acbite",		"massa d'acido",		 'f',	DAM_ACID	  },
	{ "chomp",		"morso",				 'f',	DAM_PIERCE	  },
	{ "drain",		"risucchio di vita",	 'm',	DAM_NEGATIVE  },
	{ "thrust",		"spinta",				 'f',	DAM_PIERCE	  },
	{ "slime",		"melma",                 'f',	DAM_ACID	  },
	{ "shock",		"shock",				 'm',	DAM_LIGHTNING },
	{ "thwack",		"battuta",				 'f',	DAM_BASH	  },
	{ "flame",		"fiamma",				 'f',	DAM_FIRE	  },
	{ "chill",		"gelo",					 'm',	DAM_COLD	  },
	{ NULL,			NULL,					 '\0',	0			  }
};


/* race table */
const struct race_type race_table[] =
{
/*
	{
		"name",       "translate",		"translate femminile" pc_race,
		act bits,   aff_by bits,    off bits,
		imm,        res,        vuln,
		form,       parts
	},
*/
	{
		"unique",     "unica",	"", FALSE, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0
	},

	{ /* 1 */
		"human",    "umano",		"umana", TRUE,       0,
		0,      0,      0,
		0,      0,      0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 2 */
		"elf",      "elfo",	"elfa", TRUE,       0,
		0,      AFF_INFRARED|AFF_SNEAK, 0,
		0,      RES_CHARM,  VULN_IRON,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 3 */
		"half-elf",     "mezzelfo",	"mezzelfa", TRUE,       0,
		0,              AFF_INFRARED,   0,
		0,              RES_CHARM,      VULN_IRON,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 4 */
		"drow",     	"drow",		"drow", TRUE,       0,
		0,              AFF_INFRARED|AFF_SNEAK, 0,
		0,              RES_CHARM,      VULN_IRON,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 5 */
	    "rockseer",     "rockseer",	"rockseer", TRUE,       0,
	    0,              AFF_INFRARED|AFF_SNEAK|AFF_PASS_DOOR, 0,
	    0,              RES_CHARM,      VULN_WOOD,
	    A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 6 */
		"dwarf",    "nano",			"nana", TRUE,       0,
		0,      AFF_INFRARED,   0,
		0,      RES_POISON|RES_DISEASE, VULN_DROWNING,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 7 */
		"svirfnebli",     "svirfnebli",	"svirfnebli", TRUE,       0,
		0,                AFF_INFRARED,   0,
		0,                RES_MAGIC|RES_POISON|RES_DISEASE, VULN_BASH,
		A|H|M|V,          A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 8 */
		"duergar",        "duergar",		"duergar", TRUE,       0,
		0,                AFF_INFRARED,   0,
		0,                RES_MAGIC|RES_POISON|RES_DISEASE, VULN_DROWNING,
		A|H|M|V,          A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 9 */
		"arial",        "arial",		"arial", TRUE,       0,
		0,              AFF_FLYING,             0,
		0,              0,              0,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 10 */
		"gnome",        "gnomo",		"gnomo", TRUE,       0,
		0,              AFF_INFRARED,   0,
		0,              RES_POISON,     VULN_BASH,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 11 */
		"storm giant",  "gigante",	"gigantessa", FALSE,       0,
		0,      AFF_FLYING,     0,
		0,      RES_LIGHTNING,  VULN_MENTAL|VULN_COLD|VULN_FIRE,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 12 */
		"cloud giant",  "gigante",	"gigantessa", FALSE,       0,
		0,      AFF_FLYING,     0,
		0,      RES_WEAPON, VULN_MENTAL|VULN_LIGHTNING,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 13 */
		"fire giant",   "gigante",	"gigantessa", FALSE,       0,
		0,      0,      0,
		0,      RES_FIRE,   VULN_MENTAL|VULN_LIGHTNING|VULN_COLD,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 14 */
		"frost giant",  "gigante",	"gigantessa", FALSE,       0,
		0,      0,      0,
		0,      RES_COLD,   VULN_MENTAL|VULN_LIGHTNING|VULN_FIRE,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 15 */
		"felar",        "felar",	"felar", TRUE,       0,
		0,              AFF_INFRARED,   OFF_TAIL,
		0,              RES_LIGHT|RES_COLD,     VULN_FIRE|VULN_DROWNING,
		A|H|M|V,        A|C|D|E|F|H|J|K|Q|U|V
	},

	{ /* 16 */
		"githyanki",    "githyanki",		"githyanki", TRUE,       0,
		0,      AFF_INFRARED,   0,
		0,      0,      0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 17 */
		"satyr",    "satiro",		"satiro", TRUE,       0,
		0,      AFF_INFRARED,   0,
		0,      RES_WOOD|RES_DISEASE,   0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 18 */
		"troll",    "troll",		"troll", TRUE,       0,/*DETECT_HIDDEN*/
		0,      AFF_REGENERATION|AFF_INFRARED,  OFF_BERSERK,
		0,  RES_CHARM|RES_BASH, VULN_FIRE|VULN_ACID,
		A|B|H|M|V,      A|B|C|D|E|F|G|H|I|J|K|U|V
	},

	{ /* 19 */
		"black dragon", "dragone nero",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_ACID,    VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 20 */
		"blue dragon",  "dragone blu",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_LIGHTNING,
		VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 21 */
		"green dragon", "dragone verde",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_POISON,  VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 22 */
		"red dragon",   "dragone rosso",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_FIRE,    VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 23 */
		"white dragon", "dragone bianco",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_COLD,    VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 24 */
		"brass dragon", "dragone di ottone",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_FIRE,    VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 25 */
		"gold dragon",  "dragone d'oro",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_FIRE|RES_POISON,
		VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 26 */
		"silver dragon",	"dragone d'argento",	"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_COLD,    VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 27 */
		"bronze dragon",	"dragone di bronzo",	"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_LIGHTNING,
		VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 28 */
		"copper dragon",	"dragone di rame",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_BASH|RES_CHARM|RES_ACID,    VULN_PIERCE,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 29 */
		"bat",      "pipisterello",		"", FALSE,      0,
		0,      AFF_FLYING/*|AFF_DARK_VISION*/, OFF_DODGE|OFF_FAST,
		0,      0,      VULN_LIGHT,
		A|G|V,      A|C|D|E|F|H|J|K|P
	},

	{ /* 30 */
		"bear",     "orso",		"", FALSE,      0,
		0,      0,      OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
		0,      RES_BASH|RES_COLD,  0,
		A|G|V,      A|B|C|D|E|F|H|J|K|U|V
	},

	{  /* 31 */
		"cat",      "gatto",		"", FALSE,      0,
		0,      /*AFF_DARK_VISION*/0,   OFF_FAST|OFF_DODGE,
		0,      0,      0,
		A|G|V,      A|C|D|E|F|H|J|K|Q|U|V
	},

	{  /* 32 */
		"centipede",    "millepiedi",	"", FALSE,      0,
		0,      /*AFF_DARK_VISION*/0,   0,
		0,      RES_PIERCE|RES_COLD,    VULN_BASH,
		A|B|G|O,        A|C|K
	},

	{  /* 33 */
		"dog",      "cane",		"", FALSE,      0,
		0,      0,      OFF_FAST,
		0,      0,      0,
		A|G|V,      A|C|D|E|F|H|J|K|U|V
	},

	{ /* 34 */
		"doll",     "bambola",		"", FALSE,      0,
		0,      0,      0,
		IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE
		|IMM_DROWNING,  RES_BASH|RES_LIGHT,
		VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
		E|J|M|cc,   A|B|C|G|H|K
	},

	{ /* 35 */
		"fido",     "cane",		"", FALSE,      0,
		0,      0,      OFF_DODGE|ASSIST_RACE,
		0,      0,          VULN_MAGIC,
		A|B|G|V,    A|C|D|E|F|H|J|K|Q|V
	},

	{ /* 36 */
		"fox",      "volpe",	"", FALSE,      0,
		0,      /*AFF_DARK_VISION*/0,   OFF_FAST|OFF_DODGE,
		0,      0,      0,
		A|G|V,      A|C|D|E|F|H|J|K|Q|V
	},

	{ /* 37 */
		"goblin",   "goblin",	"", FALSE,      0,
		0,      AFF_INFRARED,   0,
		0,      RES_DISEASE,    VULN_MAGIC,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 38 */
		"hobgoblin",    "hobgoblin",	"", FALSE,      0,
		0,      AFF_INFRARED,   0,
		0,      RES_DISEASE|RES_POISON, 0,
		A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y
	},

	{  /* 39 */
		"kobold",   "koboldo",	"", FALSE,      0,
		0,      AFF_INFRARED,   0,
		0,      RES_POISON, VULN_MAGIC,
		A|B|H|M|V,  A|B|C|D|E|F|G|H|I|J|K|Q
	},

	{  /* 40 */
		"lizard",   "lucertola",	"", FALSE,      0,
		0,      0,      0,
		0,      RES_POISON, VULN_COLD,
		A|G|X|cc,   A|C|D|E|F|H|K|Q|V
	},

	{  /* 41 */
		"modron",   "modron",	"", FALSE,      0,
		0,      AFF_INFRARED,   ASSIST_RACE|ASSIST_ALIGN,
		IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
		        RES_FIRE|RES_COLD|RES_ACID, 0,
		H,      A|B|C|G|H|J|K
	},

	{  /* 42 */
		"orc",      "orco",		"", FALSE,      0,
		0,      AFF_INFRARED,   0,
		0,      RES_DISEASE,    VULN_LIGHT,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 43 */
		"pig",      "suino",		"", FALSE,      0,
		0,      0,      0,
		0,      0,      0,
		A|G|V,      A|C|D|E|F|H|J|K
	},

	{ /* 44 */
		"rabbit",   "coniglio",		"", FALSE,      0,
		0,      0,      OFF_DODGE|OFF_FAST,
		0,      0,      0,
		A|G|V,      A|C|D|E|F|H|J|K
	},

	{ /* 45 */
		"school monster",   "mostro",	"", FALSE,      0,
		ACT_NOALIGN,        0,      0,
		IMM_CHARM|IMM_SUMMON,   0,      VULN_MAGIC,
		A|M|V,      A|B|C|D|E|F|H|J|K|Q|U
	},

	{  /* 46 */
		"snake",    "serpente",		"", FALSE,      0,
		0,      0,      0,
		0,      RES_POISON, VULN_COLD,
		A|G|X|Y|cc, A|D|E|F|K|L|Q|V|X
	},

	{  /* 47 */
		"song bird",    "uccello canterino",		"", FALSE,      0,
		0,      AFF_FLYING,     OFF_FAST|OFF_DODGE,
		0,      0,      0,
		A|G|W,      A|C|D|E|F|H|K|P
	},

	{  /* 48 */
		"water fowl",   "uccello marino",		"", FALSE,      0,
		0,      AFF_SWIM|AFF_FLYING,    0,
		0,      RES_DROWNING,       0,
		A|G|W,      A|C|D|E|F|H|K|P
	},

	{  /* 49 */
		"wolf",     "lupo",		"", FALSE,      0,
		0,      /*AFF_DARK_VISION*/0,   OFF_FAST|OFF_DODGE,
		0,      0,      0,
		A|G|V,      A|C|D|E|F|J|K|Q|V
	},

	{  /* 50 */
		"wyvern",   "viverna",		"", FALSE,      DETECT_INVIS|DETECT_HIDDEN,
		0,      AFF_FLYING, OFF_BASH|OFF_FAST|OFF_DODGE,
		IMM_POISON, 0,  VULN_LIGHT,
		A|B|G|Z,        A|C|D|E|F|H|J|K|Q|V|X
	},

	{ /* 51 */
		"dragon",   "dragone",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		0,      RES_FIRE|RES_BASH|RES_CHARM,    VULN_PIERCE|VULN_COLD,
		A|H|Z,      A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X
	},

	{ /* 52 */
		"giant",    "gigante",		"gigantessa", TRUE,      0,
		0,      0,      0,
		0,      RES_FIRE|RES_COLD,  VULN_MENTAL|VULN_LIGHTNING,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 53 */
		"golem",    "golem",		"", FALSE,      0,
		0,      0,      0,
		IMM_POISON|IMM_DISEASE,     RES_WEAPON, VULN_MENTAL|VULN_MAGIC,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 54 */
		"undead",   "non-morto",		"", FALSE,
		DETECT_INVIS|DETECT_HIDDEN|DETECT_GOOD|DETECT_EVIL,
		0,      AFF_PASS_DOOR|AFF_INFRARED, 0,
		IMM_DROWNING|IMM_DISEASE|IMM_POISON,    RES_NEGATIVE|RES_ENERGY,
		VULN_SILVER|VULN_HOLY|VULN_LIGHT,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 55 */
		"dark-elf",     "elfoscuro",		"", FALSE,       0,
		0,      AFF_INFRARED,   0,
		0,      RES_CHARM,      VULN_IRON,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 56 */
		"draconian",    "draconiano",		"", FALSE,      0,
		0,      0,      0,
		0,      RES_SLASH|RES_POISON,   VULN_COLD,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 57 */
		"sprite",   "spiritello",		"", FALSE,      DETECT_EVIL,
		0,      AFF_FLYING,     G,
		0,      0,          VULN_COLD|VULN_WEAPON,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 58 */
		"aarakocra",    "aarakocra",		"", FALSE,      0,
		ACT_SCAVENGER,  AFF_FLYING, OFF_FAST,
		0,      0,      0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K|P
	},

	{ /*  59 */
		"beholder", "beholder",		"", FALSE,      0,
		0,      AFF_REGENERATION,       0,
		0,      RES_MAGIC,      0,
		A|H|M|V,    A|E|K
	},

	{  /* 60 */
		"tiger",    "tigre",		"", FALSE,      0,
		0,  /*AFF_DARK_VISION*/0, OFF_FAST|OFF_DODGE,
		0,      RES_WEAPON,     0,
		A|G|V,      A|C|D|E|F|H|J|K|Q|U|V
	},

	{  /* 61 */
		"lion",     "leone",		"", FALSE,      0,
		ACT_SENTINEL,   /*AFF_DARK_VISION*/0,   OFF_FAST|OFF_DODGE,
		0,      RES_WEAPON,     0,
		A|G|V,      A|C|D|E|F|H|J|K|Q|U|V
	},

	{  /* 62 */
		"centaur",  "centauro",		"", FALSE,      0,
		0,      0, OFF_BASH|OFF_KICK|OFF_KICK_DIRT|OFF_DODGE,
		0,      RES_WEAPON,     0,
		A|G|N|V,        A|B|C|D|E|F|G|H|I|J|K|Q
	},

	{  /* 63 */
		"death knight", "cavaliere spettrale",		"", FALSE,      0,
		ACT_UNDEAD, 0,  OFF_PARRY|OFF_DISARM,
		IMM_DISEASE|IMM_POISON|IMM_DROWNING,    RES_FIRE|RES_NEGATIVE,
		VULN_HOLY,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{ /* 65 */
		"dracolich",    "drago non-morto",		"", FALSE,      0,
		0,      AFF_INFRARED|AFF_FLYING,    0,
		IMM_CHARM|IMM_LIGHTNING|IMM_COLD,   RES_FIRE|RES_BASH,
		VULN_PIERCE|VULN_COLD,
		A|H|Z,      A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X
	},

	{  /* 66 */
		"air elemental",	"elementale dell'aria",		"", FALSE,      0,
		0,      0,      OFF_FADE,
		0,      RES_WEAPON|RES_SUMMON,      0,
		A|H|Z,      A|D
	},

	{  /* 67 */
		"earth elemental",  "elementale della terra",		"", FALSE,      0,
		0,      0,  OFF_FADE,
		0,      RES_WEAPON|RES_SUMMON,      0,
		A|H|Z,      A|D
	},

	{  /* 68 */
		"fire elemental",   "elementale del fuoco",		"", FALSE,      0,
		0,      0,  OFF_FADE,
		IMM_FIRE,       RES_WEAPON,     VULN_COLD,
		A|H|Z,      A|D
	},

	{  /* 69 */
		"water elemental",  "elementale dell'acqua",		"", FALSE,      0,
		0,      0,  OFF_FADE,
		0,      RES_WEAPON|RES_FIRE|RES_COLD,   0,
		A|H|Z,      A|D
	},

	{  /* 70 */
		"fish",     "pesce",		"", FALSE,      0,
		0,      AFF_SWIM,   0,
		0,      0,      VULN_COLD|VULN_FIRE,
		A|H|Z,      A|D|E|F|J|K|O|X
	},

	{  /* 71 */
		"gargoyle", "gargoyle",		"", FALSE,      0,
		ACT_AGGRESSIVE, AFF_FLYING, OFF_FAST,
		0,      RES_WEAPON, VULN_COLD|VULN_FIRE,
		A|G|W,      A|C|D|E|F|H|K|P
	},

	{  /* 72 */
		"ghost",    "fantasma",		"", FALSE,      0,
		ACT_UNDEAD, AFF_FLYING, OFF_FADE,
		IMM_DROWNING|IMM_NEGATIVE|IMM_DISEASE|IMM_POISON,
		RES_WEAPON, VULN_HOLY,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 73 */
		"ghoul",    "ghoul",		"", FALSE,      0,
		0,      0,      0,
		IMM_DISEASE,    RES_CHARM,  VULN_MAGIC,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 74 */
		"gnoll",    "gnoll",		"", FALSE,      DETECT_INVIS|DETECT_HIDDEN,
		0,      0,      OFF_DISARM,
		0,      0,      0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 75 */
		"harpy",    "arpia",		"", FALSE,      DETECT_INVIS|DETECT_HIDDEN,
		0,      AFF_FLYING,     OFF_DISARM,
		0,      RES_CHARM,      0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K|P
	},

	{  /* 76 */
		"lamia",    "lamia",		"", FALSE,      0,
		ACT_AGGRESSIVE, 0,      OFF_FAST|OFF_DODGE,
		0,      RES_CHARM,      0,
		A|G|V,      A|B|C|D|E|F|G|H|I|J|K|Q|U
	},

	{  /* 77 */
		"lich",     "lich",		"", FALSE,      0,
		0,      0,      OFF_FAST|OFF_DODGE,
		IMM_POISON|IMM_DISEASE|IMM_CHARM|IMM_LIGHTNING|IMM_COLD,
		RES_WEAPON|RES_FIRE,    VULN_HOLY,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 77 */
		"minotaur", "minotauro",		"", FALSE,      0,
		0,      0,      OFF_CRUSH|OFF_FAST|OFF_BASH,
		0,      0,      0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K|W
	},

	{  /* 78 */
		"ogre",     "ogre",		"", FALSE,      0,
		0,      AFF_INFRARED,   0,
		0,      RES_DISEASE,    0,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 79 */
		"zombie",   "zombie",		"", FALSE,      0,
		0,      AFF_SLOW,   0,
		IMM_MAGIC|IMM_POISON|IMM_CHARM, RES_COLD, VULN_WEAPON,
		A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
	},

	{  /* 80 */
		"horse",    "cavallo",		"", FALSE,      0,
		0,      0,      OFF_FAST|OFF_DODGE,
		0,      0,      0,
		A|G|V,      A|C|D|E|F|J|K|Q
	},

	{
		NULL, NULL, NULL, FALSE, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0
	}
};


const struct pc_race_type pc_race_table[MAX_PC_RACE] =
{
/*
	{
		"race name", "nome femminile", points, { class multipliers },
		{ bonus skills },
		{ stat mods: str, int, wis, dex, con, cha }, { max stats:  str, int, wis, dex, con, cha },
		{ bonus point: life, mana, mov }, { max race point: life, mana, point },
		size, practice_bonus, align
	},
*/

	{
		"razza nulla", "razza nulla", 0, { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
		{ "" },
		{ 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 },
		{ 0, 0, 0 }, { 1500, 1500, 400 },
		0, 0, 0, 0
	},

	{
		"Umano", "Umana", 0, { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
		{ "" },
		{ 0, 0, 0, 0, 0 ,0 }, { 20, 20, 20, 20, 20, 25 },
		{ 0, 0, 5 }, { 2000, 1800, 450 },
		SIZE_MEDIUM, 0, CR_ALL, LANG_HUMAN
	},

	{
		"Elfo", "Elfa", 500, { 100, 100, 100, 100, 100, -1, -1, 100, 100, -1, -1, -1, 100 },
		{ "" },
		{ -2, 5, 0, 2, -2, 0 }, { 18, 25, 21, 23, 16, 25 },
		{ -10, 30, -10 }, { 1800, 2500, 400 },
		SIZE_MEDIUM, 0, CR_GOOD, LANG_ELVISH
	},

	{
		"Mezzelfo", "Mezzelfa", 200, { 100, 100, 100, 100, 100, 100, 100, 100, 100, -1, -1, 100, 100 },
		{ "" },
		{ -1, 2, 0, 1, -1, 0 }, { 19, 21, 21, 21, 19 ,25 },
		{ -10, 10, -5 }, { 1900, 1900, 450 },
		SIZE_MEDIUM, 0, CR_ALL, LANG_ELVISH
	},

	{
		"Drow", "Drow", 500, { 100, 100, 100, 100, -1, 100, 100, -1, 100, -1, -1, 100, 100 },
		{ "" },
		{ -2, 4, 0, 2, -2 ,0 }, { 18, 24, 21, 24, 16, 25 },
		{ -10, 20, -10 }, { 1800, 2500, 400 },
		SIZE_MEDIUM, 0, CR_EVIL, LANG_ELVISH
	},

	{
		"Rockseer", "Rockseer", 500, { 100, 100, 100, 100, -1, -1, 100, 100, 100, -1, -1, -1, 100 },
		{ "wand" },
		{ -1, 2, 0, 4, -2 ,0 }, { 18, 24, 21, 24, 16, 25 },
		{ -10, 20, -15 }, { 1800, 2500, 400 },
		SIZE_MEDIUM, 0, CR_NEUTRAL, LANG_ELVISH
	},

	{
		"Nano", "Nana", 250, { 100, 100, -1, 100, -1, -1, -1, -1, 100, -1, -1, -1, 100 },
		{ "berserk" },
		{ 0, 0, 1, -4, 5,0 }, { 21, 19, 19, 18, 25, 25 },
		{ 30, 0, 10 }, { 2200, 1100, 500 },
		SIZE_MEDIUM, 0, CR_ALL, LANG_DWARVISH
	},

	{
		"Svirfnebli", "Svirfnebli", 250, { 100, 100, 100, 100, -1, -1, -1, -1, 100, -1, -1, -1, 100 },
		{ "lore", "identify" },
		{ 0, -1, 3, 0, 0, 0 }, { 20, 19, 23, 18, 19 , 25 },
		{ 10, 0, -20 }, { 1500, 2200, 300 },
		SIZE_SMALL, 2, CR_NEUTRAL, LANG_GNOMISH
	},

	{
		"Duergar", "Duergar", 250, { -1, 100, 100, 100, -1, 100, -1, -1, -1, -1, -1, 100, -1 },
		{ "berserk", "pick lock" },
		{ 2, -2, -1, 1, 1 ,0 }, { 22, 16, 18, 20, 21, 25 },
		{ 25, -10, 5 }, { 2100, 1200, 500 },
		SIZE_MEDIUM, 0, CR_EVIL, LANG_DWARVISH
	},

	{
		"Arial", "Arial", 250, { 100, 100, 100, 100, -1, 100, 100, -1, 100, -1, -1, 100, 100 },
		{ "" },
		{ -2, 3, 0, 5, -2 ,0 }, { 19, 23, 20, 25, 18, 25 },
		{ -10, 50, -15 }, { 1900, 2200, 300 },
		SIZE_MEDIUM, 0, CR_ALL, LANG_HUMAN
	},

	{
		"Gnomo", "Gnoma", 300, { 100, 100, 100, 100, -1, -1, -1, -1, 100, -1, -1, -1, 100 },
		{ "identify" },
		{ -2, 3, 5, 0, -2 ,0 }, { 18, 23, 25, 20, 18, 25 },
		{ 0, 10, -20 }, { 1500, 2200, 300 },
		SIZE_SMALL, 5, CR_NEUTRAL, LANG_GNOMISH
	},

	{
		"Gigante", "Gigantessa", 500, { -1, 100, -1, 100, -1, 100, -1, 100, -1, -1, -1, -1, 100 },
		{ "bash", "enhanced damage" },
		{ 2, -4, 0, -4, 2 ,0 }, { 25, 15, 17, 16, 23, 25 },
		{ 20, -20, 30 }, { 2500, 1000, 500 },
		SIZE_HUGE, 0, CR_ALL, LANG_GIANT
	},

	{
		"Gigante", "Gigantessa", 500, { -1, 100, -1, 100, -1, 100, -1, 100, -1, -1, -1, -1, 100 },	/* Si', lo so, e' una pezza, e tra le peggiori, rende abb stabile la conversione tra razze giocanti e no, visto che son state tolte le 4 razze di giganti. */
		{ "bash", "enhanced damage" },
		{ 2, -4, 0, -4, 2 ,0 }, { 25, 15, 17, 16, 23, 25 },
		{ 20, -20, 30 }, { 2500, 1000, 500 },
		SIZE_HUGE, 0, CR_ALL, LANG_GIANT
	},

	{
		"Gigante", "Gigantessa",500, { -1, 100, -1, 100, -1, 100, -1, 100, -1, -1, -1, -1, 100 },
		{ "bash", "enhanced damage" },
		{ 2, -4, 0, -4, 2 ,0 }, { 25, 15, 17, 16, 23, 25 },
		{ 20, -20, 30 }, { 2500, 1000, 500 },
		SIZE_HUGE, 0, CR_ALL, LANG_GIANT
	},

	{
		"Gigante", "Gigantessa",500, { -1, 100, -1, 100, -1, 100, -1, 100, -1, -1, -1, -1, 100 },
		{ "bash", "enhanced damage" },
		{ 2, -4, 0, -4, 2 ,0 }, { 25, 15, 17, 16, 23, 25 },
		{ 20, -20, 30 }, { 2500, 1000, 500 },
		SIZE_HUGE, 0, CR_ALL, LANG_GIANT
	},

	{
		"Felar", "Felar", 250, { -1, -1, 100, 100, -1, 100, 100, 100, 100, -1, -1, -1, -1 },
		{ "hand to hand" },
		{ -1, -1, 0, 2, 2, 0 }, { 17, 17, 20, 23, 23, 25 },
		{ 10, -10, 15 }, { 2000, 1000, 450 },
		SIZE_MEDIUM, 0, CR_ALL, LANG_CAT
	},

	{
		"Githyanki", "Githyanki", 250, { 100, -1, -1, 100, -1, -1, 100, 100, 100, -1, -1, 100, 100 },
		{ "sword", "dagger" },
		{ 0, 2, -2, 3, -1, 0 }, { 19, 23, 20, 25, 18, 25 },
		{ 0, 0, 0 }, { 1900, 2000, 400 },
		SIZE_MEDIUM, 0, CR_EVIL, LANG_HUMAN
	},

	{
		"Satiro", "Satira", 250, { -1, -1, 100, 100, -1, -1, -1, 100, -1 , -1, -1, -1, -1 },
		{ "camouflage", "ambush", "steal" },
		{ -1, 0, 0, 3, 0, 0 }, { 19, 23, 20, 25, 18, 25 },
		{ 0, 0, 20 }, { 2100, 1300, 500 },
		SIZE_MEDIUM, 0, CR_NEUTRAL, LANG_HUMAN
	},

	{
		"Troll", "Troll", 400, { -1, -1, -1, 100, -1, 100, -1, 100, -1, -1, -1, -1, -1 },
		{ "fast healing", "trance" },
		{ 2, -2, -1, -2, 3, 0 }, { 19, 23, 20, 25, 18, 25 },
		 { 0, 0, 15 }, { 2300, 900, 500 },
		SIZE_LARGE, 0, CR_EVIL, LANG_TROLLISH
	}
};


/*
 * Class table.
 */
const struct class_type class_table[MAX_CLASS] =
{
	{
		"evocatore", "evocatrice", INT, OBJ_VNUM_SCHOOL_DAGGER,
		{ 3018, 0, 0, 0, 0, 0 }, 60, 18, 6, 60, 100, TRUE,
		0, {0,3,0,0,0,5}, CR_ALL
	},

	{
		"chierico", "chierica", WIS,  OBJ_VNUM_SCHOOL_MACE,
		{ 3003, 9619, 9619, 9619, 9619, 9619},  60,  18, 2,  70, 80, TRUE,
		0,{0,0,3,0,0,5},CR_ALL
	},

	{
		"ladro", "ladra", DEX,  OBJ_VNUM_SCHOOL_DAGGER,
		{ 3028, 9639, 9639, 9639, 9639, 9639 },  60,  18,  -4, 100, 50, FALSE,
		0,{0,0,0,3,0,5},CR_ALL
	},

	{
		"guerriero", "guerriera", STR,  OBJ_VNUM_SCHOOL_SWORD,
		{ 3022, 9633, 9633, 9633, 9633, 9633 },  60,  18,  -10, 100,50,FALSE,
		0,{3,0,0,0,1,5},CR_ALL
	},

	{
		"paladino", "paladina", STR,  OBJ_VNUM_SCHOOL_SWORD,
		{ 3063, 0, 0, 0, 0, 0 },  60,  18,  -6,  80, 70, TRUE,
		400,{2,0,1,0,2,5},CR_GOOD
	},

	{
		"antipaladino", "antipaladina", STR,  OBJ_VNUM_SCHOOL_SWORD,
		{ 3061, 0, 0, 0, 0, 0 },  60,  18,  -6,  80, 70, TRUE,
		300,{2,1,0,1,1,5},CR_EVIL
	},

	{
		"ninja", "ninja", DEX,  OBJ_VNUM_SCHOOL_DAGGER,
		{ 3062, 0, 0, 0, 0, 0 },  60,  18,  -10,  90, 60, FALSE,
		300,{1,0,0,2,0,5},CR_ALL
	},

	{
		"ranger", "ranger", STR,  OBJ_VNUM_SCHOOL_SWORD,
		{ 6156, 0, 0, 0, 0, 0 },  60,  18,  -8,  90, 60, FALSE,
		200,{1,0,0,1,1,5},CR_ALL
	},

	{
		"trasmutatore", "trasmutratrice", INT,  OBJ_VNUM_SCHOOL_DAGGER,
		{ 3073, 0, 0, 0, 0, 0 },  60,  18, 2,  60,  100, TRUE,
		0,{0,3,0,0,0,5},CR_ALL
	},

	{
		"samurai", "samurai", STR,  OBJ_VNUM_SCHOOL_SWORD,
		{ 3075, 9633, 0, 0, 0, 0 },  60,  18,  -10,  100, 60,FALSE,
		500,{2,1,1,1,1,5},CR_ALL
	},

	{
		"vampiro", "vampira", INT,  OBJ_VNUM_SCHOOL_DAGGER,
		{ 5395, 5396, 0, 0, 0, 0 },  60,  18, -6,  80,  70, TRUE,
		300,{1,2,0,0,0,5}, CR_EVIL
	},

	{
		"necromante", "necromante", INT,  OBJ_VNUM_SCHOOL_DAGGER,
		{ 5393, 5394, 0, 0, 0, 0 },  60,  18, 6,  60,  100, TRUE,
		200,{0,2,1,0,0,5},CR_EVIL
	},

	{
		"elementalista", "elementalista", INT,  OBJ_VNUM_SCHOOL_DAGGER,
		{ 9618, 0, 0, 0, 0, 0 },  60,  18, 6,  60,  100, TRUE,
		0,{0,3,0,0,0,5},CR_ALL
	}
};


const struct color_type color_table[] =
{
	{ "Nero",		BLACK	},
	{ "Verde",		GREEN	},
	{ "Marrone",	BROWN	},
	{ "Giallo",		YELLOW	},
	{ "Rosso",		RED		},
	{ "Blu",		BLUE	},
	{ "Magenta",	MAGENTA	},
	{ "Ciano",		CYAN	},
	{ "Bianco",		WHITE	},
	{ NULL,			NULL	}	/*  Last entry should be NULL   */
};


/*
 * God's Name, name of religion, tattoo vnum
 */
const struct religion_type religion_table[] =
{
	{ "",			"None",				0 },
	{ "Atum-Ra",	"Lawful Good",		OBJ_VNUM_TATTOO_APOLLON		},
	{ "Zeus",		"Neutral Good",		OBJ_VNUM_TATTOO_ZEUS		},
	{ "Siebele",	"True Neutral",		OBJ_VNUM_TATTOO_SIEBELE		},
	{ "Shamash",	"God of Justice",	OBJ_VNUM_TATTOO_HEPHAESTUS	},
	{ "Ahuramazda",	"Chaotic Good",		OBJ_VNUM_TATTOO_EHRUMEN		},
	{ "Ehrumen",	"Chaotic Evil",		OBJ_VNUM_TATTOO_AHRUMAZDA	},
	{ "Deimos",		"Lawful Evil",		OBJ_VNUM_TATTOO_DEIMOS		},
	{ "Phobos",		"Neutral Evil",		OBJ_VNUM_TATTOO_PHOBOS		},
	{ "Odin",		"Lawful Neutral",	OBJ_VNUM_TATTOO_ODIN		},
	{ "Teshub",		"Chaotic Neutral",	OBJ_VNUM_TATTOO_MARS		},
	{ "Ares",		"God of War",		OBJ_VNUM_TATTOO_ATHENA		},
	{ "Goktengri",	"God of Honor",		OBJ_VNUM_TATTOO_GOKTENGRI	},
	{ "Hera",		"God of Hate",		OBJ_VNUM_TATTOO_HERA		},
	{ "Venus",		"God of beauty",	OBJ_VNUM_TATTOO_VENUS		},
	{ "Seth",		"God of Anger",		OBJ_VNUM_TATTOO_ARES		},
	{ "Enki",		"God of Knowledge",	OBJ_VNUM_TATTOO_PROMETHEUS	},
	{ "Eros",		"God of Love",		OBJ_VNUM_TATTOO_EROS		}
};


const struct ethos_type ethos_table[] =
{
	{ "Nulla"	 },
	{ "Legale"	 },
	{ "Neutrale" },
	{ "Caotico"	 }
};


/*
 * Tabella delle costanti sulle patrie.
 */
const struct hometown_type hometown_table[] =
{
/*					{altar: good, neut, evil}, {recall: good, neut, evil}, {pit: good, neut, evil} */
	{ "Midgaard",		{ 3070,  3054,  3072}, { 3068,  3001,  3071}, { 3069,  3054,  3072} },
	{ "Nuova Thalos",	{ 9605,  9605,  9605}, { 9609,  9609,  9609}, { 9609,  9609,  9609} },
	{ "Valle Titania",	{18127, 18127, 18127}, {18126, 18126, 18126}, {18127, 18127, 18127} },
	{ "Nuova Ofcol",	{  669,   669,   669}, {  698,   698,   698}, {  669,   669,   669} },
	{ "Midgaard Vec.",	{ 5386,  5386,  5386}, { 5379,  5379,  5379}, { 5386,  5386,  5386} },
};


/*
 * Attribute bonus tables.
 */
const struct str_app_type str_app[26] =
{
	{ -5, -4,   0,  0 },  /* 0  */
	{ -5, -4,   3,  1 },  /* 1  */
	{ -3, -2,   3,  2 },
	{ -3, -1,  10,  3 },  /* 3  */
	{ -2, -1,  25,  4 },
	{ -2, -1,  55,  5 },  /* 5  */
	{ -1,  0,  80,  6 },
	{ -1,  0,  90,  7 },
	{  0,  0, 100,  8 },
	{  0,  0, 100,  9 },
	{  0,  0, 115, 10 }, /* 10  */
	{  0,  0, 115, 11 },
	{  0,  0, 130, 12 },
	{  0,  0, 130, 13 }, /* 13  */
	{  0,  1, 140, 14 },
	{  1,  1, 150, 15 }, /* 15  */
	{  1,  2, 165, 16 },
	{  2,  3, 180, 22 },
	{  2,  3, 200, 25 }, /* 18  */
	{  3,  4, 225, 30 },
	{  3,  5, 250, 35 }, /* 20  */
	{  4,  6, 300, 40 },
	{  4,  6, 350, 45 },
	{  5,  7, 400, 50 },
	{  5,  8, 450, 55 },
	{  6,  9, 500, 60 }  /* 25   */
};



const struct int_app_type int_app[26] =
{
	{  2 }, /*  0 */
	{  3 }, /*  1 */
	{  4 },
	{  5 }, /*  3 */
	{  6 },
	{  7 }, /*  5 */
	{  8 },
	{  9 },
	{ 10 },
	{ 12 },
	{ 14 }, /* 10 */
	{ 16 },
	{ 19 },
	{ 21 },
	{ 24 },
	{ 27 }, /* 15 */
	{ 30 },
	{ 33 },
	{ 36 }, /* 18 */
	{ 40 },
	{ 45 }, /* 20 */
	{ 51 },
	{ 56 },
	{ 66 },
	{ 76 },
	{ 81 }  /* 25 */
};



const struct wis_app_type wis_app[26] =
{
	{ 0 },  /*  0 */
	{ 0 },  /*  1 */
	{ 0 },
	{ 0 },  /*  3 */
	{ 0 },
	{ 1 },  /*  5 */
	{ 1 },
	{ 1 },
	{ 1 },
	{ 1 },
	{ 1 },  /* 10 */
	{ 1 },
	{ 1 },
	{ 1 },
	{ 1 },
	{ 2 },  /* 15 */
	{ 2 },
	{ 2 },
	{ 3 },  /* 18 */
	{ 3 },
	{ 3 },  /* 20 */
	{ 3 },
	{ 4 },
	{ 4 },
	{ 4 },
	{ 5 }   /* 25 */
};



const struct dex_app_type dex_app[26] =
{
	{   60 },   /* 0 */
	{   50 },   /* 1 */
	{   50 },
	{   40 },
	{   30 },
	{   20 },   /* 5 */
	{   10 },
	{    0 },
	{    0 },
	{    0 },
	{    0 },   /* 10 */
	{    0 },
	{    0 },
	{    0 },
	{    0 },
	{ - 10 },   /* 15 */
	{ - 15 },
	{ - 20 },
	{ - 30 },
	{ - 40 },
	{ - 50 },   /* 20 */
	{ - 60 },
	{ - 75 },
	{ - 90 },
	{ -105 },
	{ -120 }    /* 25 */
};


const struct con_app_type con_app[26] =
{
	{  0, 20 },   /*  0 */
	{  1, 25 },   /*  1 */
	{  1, 30 },
	{  2, 35 },   /*  3 */
	{  3, 40 },
	{  4, 45 },   /*  5 */
	{  5, 50 },
	{  6, 55 },
	{  7, 60 },
	{  8, 65 },
	{  9, 70 },   /* 10 */
	{ 10, 75 },
	{ 11, 80 },
	{ 12, 85 },
	{ 13, 88 },
	{ 14, 90 },   /* 15 */
	{ 15, 95 },
	{ 16, 97 },
	{ 17, 99 },   /* 18 */
	{ 18, 99 },
	{ 19, 99 },   /* 20 */
	{ 20, 99 },
	{ 21, 99 },
	{ 22, 99 },
	{ 23, 99 },
	{ 24, 99 }    /* 25 */
};


/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct liq_type liq_table[] =
{
/*    name					tranlate				color		proof, full, thirst, food, ssize */
	{ "water",				"acqua",				"chiaro",		{   0, 1, 10, 0, 16 } },
	{ "beer",				"birra",				"ambra",		{  12, 1,  8, 1, 12 } },
	{ "red wine",			"vino rosso",			"borgogna",		{  30, 1,  8, 1,  5 } },
	{ "ale",				"birra chiara",			"bruno",		{  15, 1,  8, 1, 12 } },
	{ "dark ale",			"birra scura",			"scuro",		{  16, 1,  8, 1, 12 } },

	{ "whisky",				"whisky",				"oro",			{ 120, 1,  5, 0,  2 } },
	{ "lemonade",			"limonata",				"rosa",			{   0, 1,  9, 2, 12 } },
	{ "firebreather",		"torcibudella",			"bollente",		{ 190, 0,  4, 0,  2 } },
	{ "local specialty",	"specialita' locale",	"chiaro",		{ 151, 1,  3, 0,  2 } },
	{ "slime mold juice",	"birra ammuffita",		"verde",		{   0, 2, -8, 1,  2 } },

	{ "milk",				"latte",				"bianco",		{   0, 2,  9, 3, 12 } },
	{ "tea",				"te'",					"abbronzato",	{   0, 1,  8, 0,  6 } },
	{ "coffee",				"caffe'",				"nero",			{   0, 1,  8, 0,  6 } },
	{ "blood",				"sangue",				"rosso",		{   0, 2, -1, 2,  6 } },
	{ "salt water",			"acqua salata",			"chiaro",		{   0, 1, -2, 0,  1 } },

	{ "coke",				"locacaco",				"marrone",		{   0, 2,  9, 2, 12 } },
	{ "root beer",			"birra del nano",		"marrone",		{   0, 2,  9, 2, 12 } },
	{ "elvish wine",		"vino elfico",			"verde",		{  35, 2,  8, 1,  5 } },
	{ "white wine",			"vino bianco",			"oro",			{  28, 1,  8, 1,  5 } },
	{ "champagne",			"champagne",			"oro",			{  32, 1,  8, 1,  5 } },

	{ "mead",				"idromele",				"miele", 		{  34, 2,  8, 2, 12 } },
	{ "rose wine",			"rose'",				"rosa",			{  26, 1,  8, 1,  5 } },
	{ "benedictine wine",	"vino benedettino",		"borgogna",		{  40, 1,  8, 1,  5 } },
	{ "vodka",				"vodka",				"chiaro",		{ 130, 1,  5, 0,  2 } },
	{ "cranberry juice",	"succo di mirtilli",	"rosso",		{   0, 1,  9, 2, 12 } },

	{ "orange juice",		"sciroppo arancio",		"arancio",		{   0, 2,  9, 3, 12 } },
	{ "absinthe",			"absinthe",				"verde",		{ 200, 1,  4, 0,  2 } },
	{ "brandy",				"brandy",				"oro",			{  80, 1,  5, 0,  4 } },
	{ "aquavit",			"acquavite",			"chiaro",		{ 140, 1,  5, 0,  2 } },
	{ "schnapps",			"superalcolico",		"chiaro",		{  90, 1,  5, 0,  2 } },

	{ "icewine",			"vino dei giganti",		"porpora",		{  50, 2,  6, 1,  5 } },
	{ "amontillado",		"amotillado",			"borgogna",		{  35, 2,  8, 1,  5 } },
	{ "sherry",				"sherry",				"rosso",		{  38, 2,  7, 1,  5 } },
	{ "framboise",			"framboise",			"rosso",		{  50, 1,  7, 1,  5 } },
	{ "rum",				"rum",					"ambra",		{ 151, 1,  4, 0,  2 } },

	{ "cordial",			"cordiale",				"chiaro",		{ 100, 1,  5, 0,  2 } },
	{ NULL,					"",						NULL,			{   0, 0,  0, 0,  0 } }
};


/*
 * Tabella dei materiali.
 */
const struct material_type material_table[] =
{
	{ "adamantite",		"adamantio",		MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "aliminum",		"alluminio",		MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "steel",			"acciaio",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "silver",			"argento",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "gold",			"oro",				MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "copper",			"rame",				MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "corundum",		"corindone",		MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "iron",			"ferro",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "bronze",			"bronzo",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "brass",			"ottone",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "mithril",		"mithril",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "lead",			"piombo",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "platinum",		"platino",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "titanium",		"titanio",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "metal",			"metallo",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "can",			"latta",			MATERIAL_METAL | MATERIAL_CANTFLOAT },
	{ "tin",			"stagno",			MATERIAL_METAL | MATERIAL_CANTFLOAT },

	{ "ivory",			"avorio",			MATERIAL_CANTFLOAT },
	{ "diamond",		"diamante",			MATERIAL_CANTFLOAT },
	{ "pearl",			"perla",			MATERIAL_CANTFLOAT },
	{ "gem",			"gemma",			MATERIAL_CANTFLOAT },
	{ "ruby",			"rubino",			MATERIAL_CANTFLOAT },
	{ "emerald",		"smeraldo",			MATERIAL_CANTFLOAT },
	{ "sapphire",		"zaffiro",			MATERIAL_CANTFLOAT },
	{ "amethyst",		"ametista",			MATERIAL_CANTFLOAT },
	{ "obsidian",		"ossidiana",		MATERIAL_CANTFLOAT },
	{ "aquamarine",		"acquamarina",		MATERIAL_CANTFLOAT },
	{ "coral",			"corallo",			MATERIAL_CANTFLOAT },
	{ "filigree",		"filigrana",		MATERIAL_CANTFLOAT },
	{ "crystal",		"cristallo",		MATERIAL_CANTFLOAT },
	{ "porcelain",		"porcellana",		MATERIAL_CANTFLOAT },
	{ "marble",			"marmo",			MATERIAL_CANTFLOAT },
	{ "quartz",			"quarzo",			MATERIAL_CANTFLOAT },
	{ "onyx",			"onice",			MATERIAL_CANTFLOAT },
	{ "opal",			"opale",			MATERIAL_CANTFLOAT },

	{ "wood",			"legno",			MATERIAL_FLOAT		},
	{ "softwood",		"legno morbido",	MATERIAL_FLOAT		},
	{ "hardwood",		"legno duro",		MATERIAL_FLOAT		},
	{ "ebony",			"ebano",			MATERIAL_FLOAT		},
	{ "bamboo",			"bamboo",			MATERIAL_FLOAT		},
	{ "straw",			"paglia",			MATERIAL_FLOAT		},
	{ "shavings",		"trucioli",			MATERIAL_FLOAT		},
	{ "oak",			"quercia",			MATERIAL_FLOAT		},
	{ "cork",			"sughero",			MATERIAL_FLOAT		},
	{ "flower",			"fiore",			MATERIAL_FLOAT		},
	{ "plant",			"pianta",			MATERIAL_FLOAT		},

	{ "silk",			"seta",				MATERIAL_FLOAT		},
	{ "wool",			"lana",				MATERIAL_FLOAT		},
	{ "cloth",			"stoffa",			MATERIAL_FLOAT		},
	{ "fur",			"pelliccia",		MATERIAL_FLOAT		},
	{ "leather",		"cuoio",			MATERIAL_FLOAT		},
	{ "softleather",	"cuoio morbido",	MATERIAL_FLOAT		},
	{ "hardleather",	"cuoio duro",		MATERIAL_CANTFLOAT	},
	{ "reptile",		"rettile",			MATERIAL_FLOAT		},
	{ "skin",			"pelle",			MATERIAL_FLOAT		},
	{ "snakeskin",		"pelle di serpente",MATERIAL_FLOAT		},
	{ "gut",			"budella",			MATERIAL_FLOAT		},
	{ "lace",			"merletto",			MATERIAL_FLOAT		},
	{ "laundry",		"biancheria",		MATERIAL_FLOAT		},
	{ "canvas",			"tela",				MATERIAL_FLOAT		},
	{ "velvet",			"velluto",			MATERIAL_FLOAT		},
	{ "felt",			"feltro",			MATERIAL_FLOAT		},
	{ "hemp",			"canapa",			MATERIAL_FLOAT		},
	{ "weaving",		"tessitura",		MATERIAL_FLOAT		},
	{ "feathers",		"piume",			MATERIAL_FLOAT		},
	{ "veil",			"velo",				MATERIAL_FLOAT		},
	{ "cotton",			"cotone",			MATERIAL_FLOAT		},
	{ "linen",			"lino",				MATERIAL_FLOAT		},
	{ "brocade",		"broccato",			MATERIAL_FLOAT		},
	{ "satin",			"raso",				MATERIAL_FLOAT		},
	{ "fiber",			"fibra",			MATERIAL_FLOAT		},
	{ "hair",			"capelli",			MATERIAL_FLOAT		},
	{ "bearskin",		"pelo d'orso",		MATERIAL_FLOAT		},
	{ "cashmire",		"cashmire",			MATERIAL_FLOAT		},

	{ "food",			"cibo",				MATERIAL_CANTFLOAT	},
	{ "meat",			"carne",			MATERIAL_CANTFLOAT	},
	{ "bread",			"pane",				MATERIAL_FLOAT		},
	{ "flour",			"farina",			MATERIAL_MELTED		},
	{ "cream",			"crema",			MATERIAL_MELTED		},
	{ "oil",			"olio",				MATERIAL_FLOAT		},
	{ "fruit",			"frutta",			MATERIAL_CANTFLOAT	},
	{ "vegetable",		"verdura",			MATERIAL_FLOAT		},
	{ "roots",			"radici",			MATERIAL_FLOAT		},
	{ "fish",			"pesce",			MATERIAL_FLOAT		},
	{ "eggs",			"uova",				MATERIAL_CANTFLOAT	},
	{ "milk",			"latte",			MATERIAL_MELTED		},
	{ "honey",			"miele",			MATERIAL_MELTED		},
	{ "wheat",			"grano",			MATERIAL_CANTFLOAT	},
	{ "chocolate",		"cioccolato",		MATERIAL_CANTFLOAT	},
	{ "blood",			"sangue",			MATERIAL_MELTED		},
	{ "human flesh",	"carne umana",		MATERIAL_CANTFLOAT	},
	{ "human femur",	"femore umano",		MATERIAL_CANTFLOAT	},

	{ "water",			"acqua",			0					},
	{ "air",			"aria",				0					},
	{ "heart",			"terra",			MATERIAL_CANTFLOAT	},
	{ "fire",			"fuoco",			0					},

	{ "ice",			"ghiaccio",			MATERIAL_FLOAT		},
	{ "energy",			"energia",			0					},
	{ "flesh",			"carne",			MATERIAL_CANTFLOAT	},
	{ "shadow",			"ombra",			0					},
	{ "wax",			"cera",				MATERIAL_CANTFLOAT	},
	{ "rubber",			"gomma",			MATERIAL_FLOAT		},
	{ "plastic",		"plastica",			MATERIAL_FLOAT		},
	{ "paraffin",		"paraffina",		MATERIAL_MELTED		},
	{ "gelatine",		"gelatina",			MATERIAL_MELTED		},
	{ "plasma",			"plasma",			MATERIAL_MELTED		},
	{ "mud",			"melma",			MATERIAL_MELTED		},
	{ "balm",			"balsamo",			MATERIAL_MELTED		},
	{ "bowel",			"intestino",		MATERIAL_CANTFLOAT	},
	{ "horn",			"corno",			MATERIAL_CANTFLOAT	},
	{ "light",			"luce",				0					},
	{ "sponge",			"spugna",			MATERIAL_FLOAT		},
	{ "elastic",		"elastico",			MATERIAL_FLOAT		},
	{ "paper",			"carta",			MATERIAL_FLOAT		},
	{ "parchment",		"pergamena",		MATERIAL_FLOAT		},
	{ "vellum",			"pergamena",		MATERIAL_FLOAT		},
	{ "china",			"inchiostro",		MATERIAL_MELTED		},
	{ "stone",			"pietra",			MATERIAL_CANTFLOAT	},
	{ "electrum",		"electrum",			0					},
	{ "cartboard",		"cartone",			MATERIAL_FLOAT		},
	{ "dark",			"buio",				0					},
	{ "granite",		"granito",			MATERIAL_CANTFLOAT	},
	{ "flint",			"selce",			MATERIAL_CANTFLOAT	},
	{ "magnet",			"magnete",			MATERIAL_CANTFLOAT	},
	{ "enamel",			"smalto",			MATERIAL_CANTFLOAT	},
	{ "glass",			"vetro",			MATERIAL_CANTFLOAT	},
	{ "ceramics",		"ceramica",			MATERIAL_CANTFLOAT	},
	{ "bone",			"osso",				MATERIAL_CANTFLOAT	},
	{ "shell",			"conchiglia",		MATERIAL_CANTFLOAT	},
	{ "limestone",		"calcare",			MATERIAL_CANTFLOAT	},
	{ "slate",			"ardesia",			MATERIAL_CANTFLOAT	},
	{ "acid",			"acido",			MATERIAL_MELTED		},
	{ "coal",			"carbone",			MATERIAL_CANTFLOAT	},
	{ "sand",			"sabbia",			MATERIAL_MELTED		},
	{ "sandstone",		"blocco di sabbia",	MATERIAL_CANTFLOAT	},
	{ "clay",			"argilla",			MATERIAL_MELTED		},
	{ "ash",			"cenere",			MATERIAL_MELTED		},
	{ "ethereal",		"etereo",			0					},
	{ "scale",			"squama",			MATERIAL_CANTFLOAT	},
	{ "nothing",		"nulla",			0					},
	{ "dragonskin",		"pelle di drago",	MATERIAL_CANTFLOAT	},
	{ "dragonscale",	"squama di drago",	MATERIAL_CANTFLOAT	},
	{ "blackmoor",		"blackmoor",		MATERIAL_CANTFLOAT	},
	{ "sulfur",			"zolfo",			MATERIAL_MELTED		},
	{ NULL,				NULL,				0					}
};


/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n) n

const struct skill_type skill_table[MAX_SKILL] =
{

/*
 * Magic spells.
 */
	{
		"reserved",	{100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,         TAR_IGNORE,     POS_STAND,
		&gsn_reserved,          SLOT( 0),    0,  0,
		'\0',    "",         "",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"absorb",   { 67, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_absorb,       TAR_CHAR_SELF,      POS_STAND,
		&gsn_absorb,        SLOT(707),  100,    12,
		'\0',   "",     "Il campo d'energia che ti proteggeva svanisce!",
		"Il campo energetico di $p svanisce.",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"acetum primus",    { 34, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_acetum_primus,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(654),  20, 12,
		'm', "prisma acido",    "!acetum primus!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"acid arrow",       { 48, 97, 97, 97, 97, 97, 97, 97,48, 97, 97, 48, 48},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_acid_arrow,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(644),  20, 12,
		'f', "freccia acida",       "!Acid Arrow!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"acid blast",       { 63, 97, 97, 97, 97, 97, 97, 97, 63, 97, 97, 63, 63},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		spell_acid_blast,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_acid_blast,    SLOT(70),   40, 12,
		'm'  ,  "getto acido",       "!Acid Blast!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"acute vision",         { 97, 97, 97, 97, 97, 97, 97, 7, 97, 97, 97, 97, 97},
		{ 3,  2,  1,  1, 1, 1, 2, 1, 1, 1,1,1,1},
		spell_acute_vision,        TAR_CHAR_SELF,          POS_STAND,
		NULL,                    SLOT(514),        10,       0,
		'\0',  "",         "Your vision seems duller.", "",    CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"aid",      { 97, 53, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		    spell_aid,        TAR_CHAR_DEFENSIVE,          POS_FIGHT,
		NULL,       SLOT(680),  100,        12,
		'\0',  "", "", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"amnesia",         {  97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_amnesia,      TAR_CHAR_OFFENSIVE,       POS_STAND,
		NULL,       SLOT(538),        100,       12,
		'\0',  "", "!amnesia!", "",    CABAL_NONE , RACE_NONE,ALIGN_NONE, GROUP_NONE
	},

	{
		"animate dead",     { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,42,38,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_animate_dead, TAR_OBJ_CHAR_OFF,   POS_STAND,
		NULL,       SLOT(581),  50, 12,
		'\0',   "", "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"animate object",   { 70, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_animate_object,   TAR_OBJ_CHAR_OFF,   POS_STAND,
		NULL,       SLOT(709),  50, 12,
		'\0',   "",     "", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"armor",        {  13,1, 97, 97,  12, 97,13,97 , 97, 10,13,13,13},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_armor,        TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT( 1),    5, 12,
		'\0',   "",         "L'armatura magica attorno al tuo corpo svanisce.",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"assist",      {  97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 43,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_assist,        TAR_CHAR_DEFENSIVE,          POS_FIGHT,
		NULL,             SLOT(670),        100,       12,
		'\0',   "", "", "",    CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"astral walk",      {31, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_astral_walk,  TAR_IGNORE, POS_FIGHT,
		NULL,           SLOT(622),  80, 12,
		'\0',    "",         "!Astral Walk!",    "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"attract other",    { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,30, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_attract_other,    TAR_CHAR_OFFENSIVE, POS_STAND,
		NULL,       SLOT(580),  5,  12,
		'\0',    "", "Riprendi padronanza di te.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BEGUILING
	},

	{
		"bark skin",         {  97, 97, 97, 97, 97, 97, 97, 22 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_bark_skin,        TAR_CHAR_SELF,          POS_STAND,
		NULL,             SLOT(515),        40,       0,
		'\0',    "", "La tua pelle torna normale.", "",    CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"black death",    { 97, 97, 97, 97, 97,  64, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_black_death,  TAR_IGNORE, POS_STAND,
		&gsn_black_death,   SLOT(677),  200,    24,
		'\0',    "",     "!black death!",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"blade barrier",    {97,60, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_blade_barrier,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(679),  40, 12,
		'm',    "cerchio di lame",    "!Blade Barrier!",  "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"bless",        { 97,  14, 97, 97,14, 97, 97, 97, 97, 12, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_bless,        TAR_OBJ_CHAR_DEF,   POS_STAND,
		&gsn_bless,     SLOT( 3),    5, 12,
		'\0',    "",         "La benedizione ti abbandona.",
		"L'aura che circonda $p sfuma.", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"bless weapon", {  97, 97, 97, 97,55, 97, 97, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_bless_weapon, TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(637),  100,    24,
		'\0',    "",         "!Bless Weapon!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENCHANTMENT
	},

	{
		"blindness",    {  97,  14, 97, 97, 97, 14,16, 97, 20, 97,20,19,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_blindness,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_blindness,     SLOT( 4),    5, 12,
		'\0',    "",         "Torni a vedere distintamente.",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"bluefire",     {  97,37, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_bluefire, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_bluefire,          SLOT(660),  20, 12,
		'm',    "vortice di fiamme nere",     "!Bluefire!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"burning hands",    {  14, 97, 97, 97, 97, 97, 97, 97 ,14, 97,14,14,14},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_burning_hands,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_burning_hands, SLOT( 5),   15, 12,
		'm',    "tocco bruciante",    "!Burning Hands!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"call lightning",   { 97, 44, 97, 97, 41, 97,50, 97 , 97, 97, 97, 97, 33},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_call_lightning,   TAR_IGNORE,     POS_FIGHT,
		NULL,           SLOT( 6),   15, 12,
		'f',    "scarica di fulmini",   "!Call Lightning!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEATHER
	},

	{
		"calm",         { 97, 26, 97, 97, 63, 97, 97, 97 , 97,60, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_calm,     TAR_IGNORE,     POS_FIGHT,
		NULL,           SLOT(509),  30, 12,
		'\0',    "",         "La calma che frenava il tuo animo ti abbandona.",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"cancellation",     { 24, 32, 97, 97, 97, 97, 97, 97 , 28, 97, 97, 28,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cancellation, TAR_CHAR_DEFENSIVE, POS_FIGHT,
		NULL,           SLOT(507),  20, 12,
		'\0',    "",         "!cancellation!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"cause critical",   {  97,  15, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1,  1,  1,  1, 1, 1, 1, 1,1, 1,1,1,1},
		spell_cause_critical,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(63),   20, 12,
		'\0',    "incantesimo",        "!Cause Critical!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HARMFUL
	},

	{
		"cause light",      { 97,  2, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cause_light,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(62),   15, 12,
		'\0',    "incantesimo",        "!Cause Light!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HARMFUL
	},

	{
		"cause serious",    {  97,  8, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cause_serious,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(64),   17, 12,
		'\0',    "incantesimo",        "!Cause Serious!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HARMFUL
	},

	{
		"caustic font", { 97, 97, 97, 97, 97, 97, 97, 97, 41, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_caustic_font, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(653),  20, 12,
		'm',    "prisma caustico", "!caustic font!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"chain lightning",  { 33, 97, 97, 97, 97, 97, 97, 97, 33 , 97,33,33,33},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_chain_lightning,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(500),  25, 12,
		'm',    "raggio fulminante",        "!Chain Lightning!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"charm person",     { 97, 97, 97, 97, 97, 38, 97, 97,  97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_charm_person, TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_charm_person,  SLOT( 7),    5, 12,
		'\0',    "",     "Riprendi padronanza di te.",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_BEGUILING
	},

	{
		"chromatic orb",    { 97, 97, 97, 97, 97, 97, 97, 97, 63, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_chromatic_orb,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(714),  50, 12,
		'f',    "sfera cromatica",        "!Chromatic Orb!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"control undead",      {  97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 57,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_control_undead,   TAR_CHAR_OFFENSIVE,          POS_STAND,
		NULL,             SLOT(669),        20,       12,
		'\0',    "", "Riprendi padronanza di te.", "",    CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"chill touch",      {  5, 97, 97, 97, 97, 6, 97,97 ,5, 97, 97, 7,8 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_chill_touch,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_chill_touch,       SLOT( 8),   15, 12,
		'm',    "tocco raggelante",   "Senti meno freddo ora.",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"colour spray",     {  22, 97, 97, 97, 97, 97, 97, 97 , 22, 97,25, 97,22},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_colour_spray, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(10),   15, 12,
		'm',    "spruzzo colorato",     "!Colour Spray!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"continual light",  {  11,  5, 97, 97, 97, 97, 97, 97 , 11, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_continual_light,  TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(57),    7, 12,
		'\0',    "",         "!Continual Light!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CREATION
	},

	{
		"control weather",  { 97,21, 97, 97,  97, 97, 11,33, 30, 97,24,24,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_control_weather,  TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(11),   25, 12,
		'\0',    "",         "!Control Weather!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEATHER
	},

	{
		"corruption",      {  97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,63,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_corruption,    TAR_CHAR_OFFENSIVE,    POS_FIGHT,
		NULL,             SLOT(671),        20,       12,
		'm',    "tocco debilitante", "Ti senti piu' in forze ora.", "",CABAL_NONE,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"create food",      { 15, 8, 97, 97,3, 97, 97, 97 , 97, 97, 97,12,13},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_create_food,  TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(12),    5, 12,
		'\0',    "",         "!Create Food!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CREATION
	},

	{
		"create rose",      { 26, 97, 97, 97, 20, 97, 97, 97, 97, 97,26,26,26},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_create_rose,  TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(511),  30,     12,
		'\0',    "",         "!Create Rose!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CREATION
	},

	{
		"create spring",    { 24, 27, 97, 97, 97, 97, 97, 97 , 97, 97, 97,31,31},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_create_spring,    TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(80),   20, 12,
		'\0',    "",         "!Create Spring!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CREATION
	},

	{
		"create water",     { 8,  3, 97, 97,4, 97, 97, 97 , 97, 97, 97,11,8},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_create_water, TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(13),    5, 12,
		'\0',    "",         "!Create Water!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CREATION
	},

	{
		"cure blindness",   { 97,11, 97, 97,  7, 97,9,13, 97,20, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cure_blindness,   TAR_CHAR_DEFENSIVE, POS_FIGHT,
		NULL,           SLOT(14),    5, 12,
		'\0',    "",         "!Cure Blindness!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CURATIVE
	},

	{
		"cure critical",    {97,  18, 97, 97,18, 97,25, 97, 97, 25, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cure_critical,    TAR_CHAR_DEFENSIVE, POS_FIGHT,
		&gsn_cure_critical, SLOT(15),   20, 12,
		'\0',    "",         "!Cure Critical!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"cure disease",     {  97, 19, 97, 97,33, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cure_disease, TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(501),  20, 12,
		'\0',    "",         "!Cure Disease!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CURATIVE
	},

	{
		"cure light",       { 97,  4, 97, 97,5,11,9, 97, 97, 10, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cure_light,   TAR_CHAR_DEFENSIVE, POS_FIGHT,
		&gsn_cure_light,    SLOT(16),   10, 12,
		'\0',    "",         "!Cure Light!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"cure poison",      { 97,  23, 97, 97, 33, 97, 97, 97, 97, 35, 97, 97, 23 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cure_poison,  TAR_CHAR_DEFENSIVE, POS_STAND,
		&gsn_cure_poison,       SLOT(43),    5, 12,
		'\0',    "",         "!Cure Poison!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CURATIVE
	},

	{
		"cure serious",     { 97,  10, 97, 97,10, 97,18, 97, 97, 18, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cure_serious, TAR_CHAR_DEFENSIVE, POS_FIGHT,
		&gsn_cure_serious,  SLOT(61),   15, 12,
		'\0',    "",         "!Cure Serious!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"curse",        { 97, 33, 97, 97, 97,18, 97, 97, 31, 97,44,34,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_curse,        TAR_OBJ_CHAR_OFF,   POS_FIGHT,
		&gsn_curse,     SLOT(17),   20, 12,
		'f'   ,"curse",        "La maledizione svanisce.",
		"", CABAL_NONE , RACE_NONE,
		ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"cursed lands",    { 97, 41, 97, 97, 97,  97, 97, 97 , 97, 97, 97,64,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_cursed_lands, TAR_IGNORE, POS_STAND,
		&gsn_cursed_lands,  SLOT(675),  200,    24,
		'\0',    "",     "!cursed lands!",   "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"deadly venom",    { 97, 97, 97, 97, 97,  58, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_deadly_venom, TAR_IGNORE, POS_STAND,
		&gsn_deadly_venom,      SLOT(674),  200,    24,
		'\0',    "",     "!deadly venom!",   "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"deafen",         { 97, 97, 97, 97, 97, 42, 97,97 , 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_deafen,      TAR_CHAR_OFFENSIVE,   POS_FIGHT,
		&gsn_deafen,       SLOT(570),        40,       12,
		'm',    "attacco stordente", "L'urlo stordente finalmente lascia la tua mente.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"demonfire",        {  97, 37, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_demonfire,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_demonfire,         SLOT(505),  20, 12,
		'm'  ,    "vortice fiammeggiante",     "!Demonfire!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"desert fist",      { 97, 58, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 55 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_desert_fist,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(681),  50, 12,
		'm',    "pugno di sabbia",  "!desert fist!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE,GROUP_NONE
	},

	{
		"detect evil",      { 12,  11, 97, 97,3, 97, 97, 97 ,12, 97, 97, 97,13},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_detect_evil,  TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(18),    5, 12,
		'\0',    "",         "",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"detect good",          { 12,  11, 97, 97,3, 97, 97, 97 ,12, 97,13,13,13},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_detect_good,      TAR_CHAR_SELF,          POS_STAND,
		NULL,                   SLOT(513),        5,     12,
		'\0',    "",                     "",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"detect hide",  { 97, 97, 10, 97, 97, 97, 10, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_SELF,      POS_STAND,
		&gsn_detect_hidden, SLOT(0),     5, 12,
		'\0',    "",         "",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"detect invis",     {  5,  17, 97, 97,23,13, 97,97 ,10, 97,23,13,8},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_detect_invis, TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(19),    5, 12,
		'\0',    "",         "",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"detect magic",     {  8,  12, 97, 97, 97, 97, 97, 97,6,30, 97,15,12},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_detect_magic, TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(20),    5, 12,
		'\0',    "",         "",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"detect poison",    { 35,  39, 97, 97, 97, 97, 97, 97,35 ,39,31,21,7},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_detect_poison,    TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(21),    5, 12,
		'\0',    "",         "!Detect Poison!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"detect undead",    { 97, 97, 97,  97, 97, 97, 97, 97 , 97, 97, 97,13,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_detect_undead,    TAR_CHAR_SELF,      POS_STAND,
		NULL,       SLOT(594),  5,  12,
		'\0',    "",     "", "",
		CABAL_NONE,     RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"disenchant armor", { 27, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_disenchant_armor,     TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(705),  50, 24,
		'\0',    "",     "!disenchant armor!",   "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"disenchant weapon",    { 34, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_disenchant_weapon,    TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(706),  50, 24,
		'\0',    "",     "!disenchant weapon!",  "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"disintegrate",         {  75, 97, 97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_disintegrate,      TAR_CHAR_OFFENSIVE,   POS_STAND,
		NULL,                    SLOT(574),    100,       18,
		'm',    "raggio disgregante", "!disintegrate!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},
/*
	{
		"dismantle",    { 97, 97, 97,  97, 97, 97, 97, 97 , 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		NULL,       SLOT(621),  200,    24,
		'\0',    "", "!621!",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},
*/
	{
		"dispel evil",      { 97, 27, 97, 97,35, 97, 97, 97,55 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_dispel_evil,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(22),   15, 12,
		'\0',    "dispel evil",      "!Dispel Evil!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"dispel good",          {   97,  27,  97,  97, 97, 30, 97, 97, 57, 97,32,32,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		NULL,                   SLOT(512),      15,     12,
		'\0',    "dispel good",          "!Dispel Good!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"dispel magic",     { 36, 43, 97, 97, 97, 97, 97, 97 ,41, 97,24,24,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_dispel_magic, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(59),   15, 12,
		'\0',    "",         "!Dispel Magic!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"disruption",   { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,40,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_disruption,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(648),  20, 12,
		'\0',    "disruption",   "!disruption!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"dragon breath", {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{1,1,1,1,1,1,1,1,1, 1,1,1,1},
		spell_dragon_breath,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_dragon_breath,  SLOT(563),   75,   12,
		'm',    "soffio infuocato", "!dragon breath!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"dragon skin",  { 10, 10,10, 10,10,10,10,10 ,10,10,10,10,10},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_dragon_skin,  TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(612),  50, 24,
		'\0',    "", "La tua pelle torna normale.",    "",
		CABAL_NONE , RACE_TOP, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"dragon strength", {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{1,1,1,1,1,1,1,1, 1, 1,1,1,1},
		spell_dragon_strength, TAR_CHAR_SELF, POS_FIGHT,
		&gsn_dragon_strength, SLOT(562),    75,    12,
		'\0',    "", "La tua forza torna nella norma.", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"dragons breath",   { 61, 97, 97, 97, 97, 97, 97, 97 ,61, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_dragons_breath,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(576),  200,    24,
		'm',    "respiro draconico",    "Ti senti di nuovo in forze.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"drain",        { 31, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_drain,        TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(704),  5,  12,
		'\0',    "",     "!drain!",  "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"earthfade",        { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,73},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_earthfade,    TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		&gsn_earthfade,         SLOT(702),  100,    12,
		'\0',    "",     "Torni alla tua forma naturale.",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"earthmaw",     { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,67},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_earthmaw,     TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		NULL,           SLOT(703),  30, 12,
		'm',    "scossone tellurico",     "!earthmaw!",   "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"earthquake",       { 97,  19, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,19 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_earthquake,   TAR_IGNORE,     POS_FIGHT,
		NULL,           SLOT(23),   15, 12,
		'm',    "terremoto",       "!Earthquake!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"elemental sphere", { 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97, 97, 29},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_elemental_sphere, TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(719),  75, 12,
		'\0',    "", "La sfera degli elementi che ti proteggeva si dissolve.",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"enchant armor",    {  26, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_enchant_armor,    TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(510),  100,    24,
		'\0',    "",         "!Enchant Armor!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENCHANTMENT
	},

	{
		"enchant weapon",   {  33, 97, 97, 97, 97, 97, 97, 97,17, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_enchant_weapon,   TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(24),   100,    24,
		'\0',    "",         "!Enchant Weapon!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENCHANTMENT
	},

	{
		"energy drain",     { 38,42, 97, 97, 97,26, 97, 97, 97, 97, 97,45,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_energy_drain, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(25),   35, 12,
		'm',    "risucchio vitale",     "!Energy Drain!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"enhanced armor",   { 48, 33, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1, 1},
		spell_enhanced_armor,   TAR_CHAR_DEFENSIVE, POS_FIGHT,
		&gsn_enhanced_armor,    SLOT(583),  20, 12,
		'\0',    "", "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"enlarge",      {  97, 97, 97, 97,  97, 97, 97, 97,  38, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_enlarge,      TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(711),   50,    12,
		'\0',    "", "Il tuo corpo torna alle dimensioni originarie.", "", CABAL_NONE,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"etheral fist",     { 97, 97, 97, 97,  97, 50, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_etheral_fist, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(645),  20, 12,
		'm',    "pugno etereo",     "!Etheral Fist!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"faerie fire",      { 11,  6, 97, 97, 97, 97,8,97 ,15 , 97, 97, 97,11},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_faerie_fire,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_faerie_fire,       SLOT(72),    5, 12,
		'm',    "fuoco fatuo",      "La fastidiosa aura rosa attorno al tuo corpo svanisce.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_WEATHER
	},

	{
		"faerie fog",       { 24, 31, 97, 97, 97, 97, 97, 97 ,27, 97, 97, 97,24},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_faerie_fog,   TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(73),   12, 12,
		'f',    "polvere fatata",       "!Faerie Fog!",     "", CABAL_NONE ,
		RACE_NONE,ALIGN_NONE, GROUP_WEATHER
	},

	{
		"farsight",     {  22, 97, 97, 97, 97, 97, 97, 97,22, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_farsight,     TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(521),  20, 12,
		'\0',    "farsight",     "!Farsight!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"fear",     {   97, 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 73,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fear, TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_fear,  SLOT(598),  50, 12,
		'\0',    "",     "La paura ti abbandona.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ILLUSION
	},

	{
		"fire and ice",     { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,65},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fire_and_ice, TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		NULL,           SLOT(699),  40, 12,
		'm',    "fuoco glaciale",     "!fire and ice!",   "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"fireball",     {55, 97, 97, 97, 97, 56, 97, 97, 56, 97, 97, 97,55},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fireball,     TAR_IGNORE, POS_FIGHT,
		NULL,           SLOT(26),   25, 12,
		'f',    "palla di fuoco",     "!Fireball!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"fireproof",        { 25, 16, 97, 97, 20, 97, 97, 97,21, 97, 97, 42,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fireproof,    TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(523),  10, 12,
		'\0',    "",     "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENCHANTMENT
	},

	{
		"firestream",       { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,35},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_firestream,   TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		NULL,           SLOT(692),  20, 12,
		'\0',    "",     "", "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"fire shield",      {   97, 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97,68},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fire_shield,  TAR_IGNORE, POS_STAND,
		&gsn_fire_shield,       SLOT(601),  200,    24,
		'\0',    "",         "!fire shield!",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"flamestrike",      {97,42, 97, 97, 32, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_flamestrike,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(65),   20, 12,
		'f',    "sferza infuocata",      "!Flamestrike!",        "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"fly",          { 19, 30, 97, 97, 97, 97, 97, 97, 19, 97,43,23,21},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fly,      TAR_CHAR_DEFENSIVE, POS_STAND,
		&gsn_fly,           SLOT(56),   10, 18,
		'\0',   "",     "Lentamente torni a terra.",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"floating disc",    { 4, 10, 97, 97, 97, 97, 97, 97 ,4, 97, 97, 97,4},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_floating_disc,    TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(522),  40, 24,
		'\0',   "",         "!Floating disc!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"forcecage",        { 29, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_forcecage,    TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(717),  75, 12,
		'\0',   "", "La gabbia di forza attorno a te svanisce.",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"frenzy",               {  97, 34, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STAND,
		NULL,                   SLOT(504),      30,     24,
		'\0',   "",                     "La tua rabbia si placa.",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"frostbolt",        { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,42},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_frostbolt,    TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		NULL,           SLOT(695),  20, 12,
		'f',   "scarica glaciale",        "!frostbolt!",  "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"fumble",       { 97, 97, 97, 97,  97, 97, 97,97 ,36, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fumble,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(712),  25, 18,
		'\0',   "",     "I tuoi muscoli tornano recettivi!",   "",
		CABAL_NONE, RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"galvanic whip",    { 28, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_galvanic_whip,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(655),  20, 12,
		'f',   "sferzata ionica",    "!galvanic whip!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"gate",         {97,63, 97, 97, 97, 97, 97, 97, 27, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_gate,     TAR_IGNORE,     POS_FIGHT,
		NULL,           SLOT(83),   80, 12,
		'\0',   "",         "!Gate!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"giant strength",   {  20, 39, 97, 97, 97, 97, 97, 97, 30, 97, 97, 28,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_giant_strength,   TAR_CHAR_DEFENSIVE, POS_STAND,
		&gsn_giant_strength,        SLOT(39),   20, 12,
		'\0',   "",         "La tua forza torna ai valori originali.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENHANCEMENT
	},

	{
		"grounding",        { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,53},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_grounding,    TAR_CHAR_SELF,      POS_FIGHT,
		NULL,           SLOT(700),  50, 12,
		'\0',   "",     "",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"group defense",    {   97, 35, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_group_defense,        TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(586),  100,    36,
		'\0',   "",         "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"group heal",       {  97,65, 97, 97, 75, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_group_healing,    TAR_CHAR_DEFENSIVE, POS_FIGHT,
		NULL,           SLOT(642),  500,    24,
		'\0',   "",         "!Group Heal!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"hallucination",    {   97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_hallucination,    TAR_CHAR_SELF,  POS_STAND,
		NULL,       SLOT(606),  200,    12,
		'\0',   "",         "La tua protezione dagli incantesimi torna nella norma.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_ILLUSION
	},

	{
		"hand of undead",   { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,44,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_hand_of_undead,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(647),  20, 24,
		'f',   "stretta della morte",   "!hand of undead!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"harm",         {  97, 26, 97, 97, 97, 29, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_harm,     TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(27),   35, 12,
		'm',   "incantesimo",       "!Harm!",   "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HARMFUL
	},

	{
		"haste",        { 24, 97, 97, 97, 97, 97, 97, 97, 24, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_haste,        TAR_CHAR_DEFENSIVE, POS_FIGHT,
		&gsn_haste,     SLOT(502),  30, 12,
		'\0',   "",         "La tua velocita' torna nella norma.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENHANCEMENT
	},

	{
		"heal",         {  97, 28, 97, 97,29, 97, 97, 97 ,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_heal,     TAR_CHAR_DEFENSIVE, POS_FIGHT,
		NULL,           SLOT(28),   50, 12,
		'\0',   "",         "!Heal!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"healing light",    { 97, 41, 97, 97, 97,  97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_healing_light,    TAR_IGNORE, POS_STAND,
		NULL,           SLOT(613),  200,    24,
		'\0',   "",         "",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"heat metal",       { 97,26, 97, 97,  97, 97, 97,97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_heat_metal,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(516),  25, 18,
		'm',   "plasma bollente",        "!Heat Metal!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"helical flow",     {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 43},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_helical_flow,     TAR_IGNORE, POS_FIGHT,
		NULL,           SLOT(661),  80, 12,
		'\0',   "",         "!Helical Flow!",   "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"hellfire",         {  97, 97, 97, 97, 97, 97, 45, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_hellfire,      TAR_CHAR_OFFENSIVE,          POS_FIGHT,
		NULL,       SLOT(520),        20,       12,
		'm',   "inferno di fiamme", "!hellfire!", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"holy aura",    { 97, 97, 97, 97,62, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_holy_aura,    TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,       SLOT(617),  75, 12,
		'\0',   "", "La tua aura sacra svanisce.", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"holy fury",    { 97, 97, 97, 97,59, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_holy_fury,    TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,       SLOT(682),  50, 24,
		'\0',   "", "Il tuo furore scema lentamente.",   "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"holy word",        { 97,48, 97, 97, 38, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_holy_word,    TAR_IGNORE, POS_FIGHT,
		NULL,           SLOT(506),  200,    24,
		'f',   "parola divina",     "!Holy Word!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"hurricane",    {  97, 97, 97, 97, 97,53, 97, 97, 97, 97, 97,65,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_hurricane,    TAR_IGNORE, POS_FIGHT,
		NULL,       SLOT(672),  200,    24,
		'm',   "uragano", "!Hurricane!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"hydroblast",   {  97, 97, 97, 97,  97, 97, 97,46, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_hydroblast,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(684),  50, 12,
		'f',   "scarica d'acqua",   "!Hydroblast!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"iceball",         {  97, 97, 97, 97, 97,58, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_iceball,     TAR_IGNORE,     POS_FIGHT,
		NULL,                   SLOT(513),        25,       12,
		'f',   "sfera di ghiaccio",         "!Iceball!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"identify",     { 18, 36, 97, 97, 97, 97, 97, 97 ,25, 97, 97, 97,18},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_identify,     TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(53),   12, 24,
		'\0',   "",         "!Identify!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"improved detect",  { 40, 97, 97, 97, 97, 97, 97, 97 ,40, 97, 97, 40,40},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_improved_detection,   TAR_CHAR_SELF,      POS_STAND,
		NULL,       SLOT(626),   20,    12,
		'\0',   "",         "",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"improved invis",   {  43, 97, 97, 97, 97, 97, 97, 97,43, 97, 97, 97,43 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_improved_invis,       TAR_CHAR_DEFENSIVE, POS_STAND,
		&gsn_imp_invis,     SLOT(627),   20,    12,
		'\0',   "",         "",
		"$p torna visibile.", CABAL_NONE , RACE_NONE,ALIGN_NONE, GROUP_ILLUSION
	},

	{
		"infravision",      {  17,  25, 97, 97, 97, 97,5, 97,18, 97, 97, 21,17},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_infravision,  TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(77),    5, 18,
		'\0',   "",         "La tua infravisione svanisce.",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENHANCEMENT
	},

	{
		"insanity", { 97, 97, 97,  97, 97, 97, 97, 97 , 97, 97, 97,59,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_insanity,     TAR_CHAR_OFFENSIVE, POS_STAND,
		NULL,       SLOT(616),  100,    24,
		'\0',   "", "Riacquisti coscienza di te, la tua pazzia svanisce.", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"inspire",      {   97, 49, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_inspire,      TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(587),  75, 24,
		'\0',   "",         "L'ispirazione divina ti abbandona.",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"invisibility",     {  10, 97, 97, 97, 97, 9, 97, 97, 13, 97, 97, 97,10 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_invis,        TAR_OBJ_CHAR_DEF,   POS_STAND,
		&gsn_invis,     SLOT(29),    5, 12,
		'\0',   "",         "",
		"$p torna visibile.", CABAL_NONE , RACE_NONE,ALIGN_NONE, GROUP_ILLUSION
	},

	{
		"iron body",        { 97, 97, 97, 97, 97, 97, 97,97 ,29, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_iron_body,    TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(718),  75, 12,
		'\0',   "", "La pelle riacquista la normale consistenza.",   "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"knock",        {   56, 97, 97, 97, 97, 97, 97, 97,64, 97, 97, 62,46},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_knock,    TAR_IGNORE, POS_STAND,
		NULL,       SLOT(603),  20, 24,
		'\0',   "",         "!knock!",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"know alignment",   {  22,  13, 97, 97, 1,5, 97, 97, 29, 97, 97, 22,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_know_alignment,   TAR_CHAR_DEFENSIVE, POS_FIGHT,
		NULL,           SLOT(58),    9, 12,
		'\0',   "",         "!Know Alignment!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"lethargic mist",    { 52, 97, 97, 97, 97,  62, 97, 97, 56, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_lethargic_mist,   TAR_IGNORE, POS_STAND,
		&gsn_lethargic_mist,    SLOT(676),  200,    24,
		'\0',   "",     "!lethargic mist!", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"light arrow",  {  97, 97, 97, 97,64, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_light_arrow,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(683),  40, 12,
		'f',   "freccia di luce",  "!light arrow!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"lightning bolt",   {  23, 97, 97, 97, 97, 13, 97,97 ,23, 97, 97, 23,23},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_lightning_bolt,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_lightning_bolt,        SLOT(30),   15, 12,
		'f',   "scarica di fulmini",   "!Lightning Bolt!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"lightning shield", { 41, 97, 97,  97, 97, 97, 97, 97 , 97, 97, 97,41,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_lightning_shield, TAR_IGNORE, POS_STAND,
		NULL,       SLOT(614),  150,    24,
		'f',   "protezione elettrifica", "",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"link",     {   31, 97, 97, 97, 97, 97, 97, 97,33 , 97, 97, 33,33},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_link,     TAR_CHAR_DEFENSIVE,     POS_STAND,
		NULL,       SLOT(588),  125,    18,
		'\0',   "",         "!link!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MEDITATION
	},

	{
		"lion help",        {   97, 97, 97, 97, 97, 97, 97, 63,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_lion_help,    TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(595),  100,    12,
		'\0',   "", "",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"locate object",    {   30, 30, 97, 97,19, 97,14, 97,29, 97, 97, 97,31},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_locate_object,    TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(31),   20, 18,
		'\0',   "",         "!Locate Object!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"love potion",         { 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_love_potion, TAR_CHAR_SELF,       POS_STAND,
		&gsn_love_potion,  SLOT(666),        10,       0,
		'\0',   "", "Riacquisti possesso delle tue facolta' mentali.", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"magic jar",        {   97, 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 68,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_magic_jar,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(596),  20, 12,
		'\0',   "",         "!magic jar!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BEGUILING
	},

	{
		"magic missile",    { 1, 97, 97, 97, 97, 1, 97,97 ,1, 97, 97, 2,1},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_magic_missile,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_magic_missile,     SLOT(32),   15, 12,
		'm',   "dardo incantato",    "!Magic Missile!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"magic resistance", {   68, 97, 97, 97, 97, 97, 97, 97,72 , 97, 97, 69,71},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_magic_resistance, TAR_CHAR_SELF,  POS_STAND,
		NULL,       SLOT(605),  200,    24,
		'\0',   "",         "",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"magnetic trust",   { 97, 97, 97, 97, 97, 97, 97, 97,34, 97, 97, 97,34},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_magnetic_trust,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(656),  20, 12,
		'm',   "turbine magnetico",   "!magnetic trust!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"mass healing",     { 97, 54, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mass_healing, TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(508),  100,    36,
		'\0',   "",         "!Mass Healing!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"mass invis",       { 43, 47, 97, 97, 97, 97, 97, 97 ,42, 97, 97, 97,46},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mass_invis,   TAR_IGNORE,     POS_STAND,
		&gsn_mass_invis,    SLOT(69),   20, 24,
		'\0',   "",         "",     "", CABAL_NONE ,
		RACE_NONE,ALIGN_NONE, GROUP_ILLUSION
	},

	{
		"mass sanctuary",   {   97, 62, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mass_sanctuary,   TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(589),  200,    24,
		'\0',   "",         "La bianca aura protettiva attorno al tuo corpo svanisce.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"master healing",   {  97,58, 97, 97, 70, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_master_heal,  TAR_CHAR_DEFENSIVE,POS_FIGHT,
		NULL,           SLOT(641),  300,    12,
		'\0',   "",         "!Master Heal!",    "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"meld into stone",  { 20, 20,20, 20,20,20,20,20 ,20,20,20,20,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_meld_into_stone,  TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(584),  12, 18,
		'\0',   "", "La tua corazza di pietra sulla pelle svanisce nel nulla.",   "",
		CABAL_NONE , RACE_ROCKSEER, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"mend",     {   50, 97, 97, 97, 97, 97, 97, 97,50 , 97, 97, 97,50},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mend,     TAR_OBJ_INV,        POS_STAND,
		&gsn_mend,      SLOT(590),  150,    24,
		'\0',   "",         "!mend!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENCHANTMENT
	},

	{
		"mind light",   { 97, 83, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mind_light,   TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(82),   200,    24,
		'\0',   "", "",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"mind wrack",   { 97, 97, 97, 97, 97, 27, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mind_wrack,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(650),  20, 12,
		'f',   "morsa mentale",   "!mind wrack!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"mind wrench",  { 97, 97, 97, 97, 97, 40, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mind_wrench,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(651),  20, 12,
		'f',   "esplosione celebrale",  "!mind wrench!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"mist walk",    {  97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 47,47,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mist_walk,    TAR_IGNORE, POS_FIGHT,
		NULL,           SLOT(658),  80, 12,
		'\0',   "",         "!Mist Walk!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"mummify",      { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 44,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mummify,  TAR_OBJ_CHAR_OFF,   POS_STAND,
		NULL,       SLOT(715),  50, 12,
		'\0',   "", "",     "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"mysterious dream",     { 97, 97, 97, 97, 97, 35, 97, 97, 49, 97, 97, 27,97},
		    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mysterious_dream, TAR_IGNORE, POS_STAND,
		&gsn_mysterious_dream,  SLOT(678),  200,    24,
		'\0',   "",     "!mysterous dream!",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_BEGUILING
	},

	{
		"nexus",                { 97, 97, 97, 97, 97, 97, 97,97 ,60, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_nexus,            TAR_IGNORE,             POS_STAND,
		NULL,                   SLOT(520),       150,   36,
		'\0',   "",                     "!Nexus!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"pass door",        {  24, 32, 97, 97, 97, 97, 97, 97 ,24,24, 97,27,24},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_pass_door,    TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(74),   20, 12,
		'\0',   "",         "You feel solid again.",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"plague",       {  97, 97, 97, 97, 97, 46, 97, 97, 33, 97, 97, 36,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_plague,       TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_plague,        SLOT(503),  20, 12,
		'f',   "pestilenza",     "Le piaghe finalmente ti danno pace.",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"poison",       {  97,  22, 97, 97, 97,15, 97, 97, 27, 97, 97, 23,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_poison,       TAR_OBJ_CHAR_OFF,   POS_FIGHT,
		&gsn_poison,        SLOT(33),   10, 12,
		'm',   "veleno",       "Il veleno finalmente ti abbandona.",
		"", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"polymorph",    { 97, 97, 97, 97, 97,  97, 97, 97,73, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_polymorph,    TAR_IGNORE, POS_STAND,
		NULL,           SLOT(639),  250,    24,
		'\0',   "",         "Ritorni alla tua razza originaria.","",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"portal",               { 54, 97, 97, 97, 97, 97, 97, 97, 40, 97, 97, 43,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_portal,           TAR_IGNORE,             POS_STAND,
		NULL,                   SLOT(519),       100,     24,
		'\0',   "",                     "!Portal!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"protection cold",{   97, 44, 97, 97, 97, 97, 97,25, 97, 97, 97,50,44},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_protection_cold,  TAR_CHAR_SELF,  POS_STAND,
		&gsn_protection_cold,       SLOT(600),  5,  12,
		'\0',   "",         "La tua aura di fiamme svanisce.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"protection evil",  { 97,  9, 97, 97,11, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_protection_evil,  TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(34),   5,  12,
		'\0',   "",         "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"protection good",  { 97,9, 97, 97, 97,13, 97, 97, 97, 97, 27,17,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_protection_good,  TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(666),  5,  12,
		'\0',   "",         "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"protection heat",  { 97, 39, 97, 97, 97, 97, 97, 97, 97, 97, 97,48,39},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_protection_heat,  TAR_CHAR_SELF,  POS_STAND,
		&gsn_protection_heat,       SLOT(599),  5,  12,
		'\0',   "",         "",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"protection negative",{   97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 15,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_protection_negative,  TAR_CHAR_SELF,  POS_STAND,
		NULL,       SLOT(636),  20, 12,
		'\0',   "",     "",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"protective shield",        { 26,38, 97, 97,  97, 97, 97,97 ,26, 97, 97, 18,26},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_protective_shield,      TAR_CHAR_SELF,   POS_STAND,
		&gsn_protective_shield,       SLOT(572),        70,       12,
		'\0',   "", "La tua aura protettiva svanisce in un soffio.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"power word kill",  {   97, 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 78,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_power_kill,   TAR_CHAR_OFFENSIVE, POS_STAND,
		NULL,       SLOT(604),  200,    18,
		'm',   "vortice mortale",        "", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"power word stun", { 97, 97, 97,  97, 97,  62, 97,97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_power_stun,   TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_power_stun,        SLOT(625),  200,    24,
		'\0',   "", "Ora riesci a muoverti.",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"purify", {97, 13, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,   TAR_IGNORE,   POS_REST,
		&gsn_purify,   SLOT(0),   0,   36,
		'\0', "",    "!Purify!", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"quantum spike",    { 41, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,41},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_quantum_spike,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(657),  20, 12,
		'f',   "scossa molecolare",    "!quantum spike!", "",
		CABAL_NONE, RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"ranger staff",         {  97, 97, 97, 97, 97, 97, 97, 32 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_ranger_staff,      TAR_IGNORE,          POS_FIGHT,
		NULL,             SLOT(519),        75,       0,
		'\0',   "", "!ranger staff!", "", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"ray of truth",         { 97, 37, 97, 97, 45, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		&gsn_ray_of_truth,      SLOT(518),      20,     12,
		'm',   "raggio Celeste",         "!Ray of Truth!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"recharge",     { 29, 97, 97, 97, 97, 97, 97, 97, 29, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_recharge,     TAR_OBJ_INV,        POS_STAND,
		NULL,           SLOT(517),  60, 24,
		'\0',   "",         "!Recharge!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ENCHANTMENT
	},

	{
		"refresh",      {  97,7, 97, 97,  97, 97, 97, 97,  97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_refresh,      TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(81),   12, 18,
		'\0',   "refresh",      "!Refresh!",        "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},
/*
	{
		"reincarnation",      {  97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_IGNORE,          POS_STAND,
		NULL,             SLOT(668),        0,       0,
		'\0',   "", "!!", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},
*/
	{
		"remove curse",     {  97, 32, 97, 97,13, 97, 97, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_remove_curse, TAR_OBJ_CHAR_DEF,   POS_STAND,
		&gsn_remove_curse,  SLOT(35),    5, 12,
		'\0',   "",         "!Remove Curse!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_CURATIVE
	},

	{
		"remove fear",      { 97, 34, 97, 97, 41, 97, 97, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_remove_fear,  TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,       SLOT(582),  5,  12,
		'\0',   "", "!Remove Fear!", "", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"remove tattoo",         { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_remove_tattoo, TAR_CHAR_DEFENSIVE,       POS_STAND,
		NULL,       SLOT(552),        10,       0,
		'\0',   "",     "!remove tattoo!",  "",
		    CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"resilience",   {  97, 97,  97, 97,56, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_resilience,   TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT( 638),  50,    12,
		'\0',   "", "", "",
		CABAL_NONE ,RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"restoring light",    { 97, 71, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_restoring_light,  TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,           SLOT(643),  50, 24,
		'\0',   "",         "!restoring light!","",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"sanctuary",        { 97, 29, 97, 97,27, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sanctuary,    TAR_CHAR_DEFENSIVE, POS_STAND,
		&gsn_sanctuary,     SLOT(36),   75, 12,
		'\0',   "",         "La bianca aura che ti proteggeva svanisce.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"sanctify lands",    { 97, 41, 97, 97,53, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sanctify_lands,   TAR_IGNORE, POS_STAND,
		NULL,           SLOT(673),  200,    24,
		'\0',   "",     "!sanctify lands!", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"sand storm",       { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,51 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sand_storm,   TAR_IGNORE, POS_FIGHT,
		&gsn_sand_storm,        SLOT(577),  200,    24,
		'f',   "tempesta di sabbia",    "La sabbia ti acceca.","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"scream",       { 52, 97, 97, 97, 97, 97, 97, 97, 48, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_scream,   TAR_IGNORE, POS_FIGHT,
		&gsn_scream,        SLOT(578),  200,    24,
		'm',   "urlo",   "Torni a udire distintamente.",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"severity force",   { 97, 45, 97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 41},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_severity_force,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(622),  20, 12,
		'f',   "scossa nella terra",       "!severity force!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"sharpen",   { 97, 97, 97, 50, 97, 97, 97, 97, 97, 97 , 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,   TAR_OBJ_INV, POS_STAND,
		&gsn_sharpen,       SLOT(0),  0, 12,
		'\0',   "",       "!Sharpen!", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"shield",       { 20, 35, 97, 97, 97, 97, 19, 97, 97, 97, 20, 20, 20 },
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_shield,       TAR_CHAR_DEFENSIVE, POS_STAND,
		&gsn_shield,        SLOT(67),   12, 18,
		'\0',   "", "Il tuo scudo di forza splende per un attimo prima di svanire.",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"shielding",    {   97, 97, 97, 97, 97, 97, 97, 97,53 , 97, 97, 53,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_shielding,    TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_shielding,     SLOT(591),  250,    12,
		'\0',   "",         "Senti allontanarsi la tua fonte vitale..",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"shocking grasp",   { 8, 97, 97, 97, 97, 97, 97, 97 ,9, 97, 97, 97,8},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_shocking_grasp,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(53),   15, 12,
		'f',   "morsa elettrizzante",   "!Shocking Grasp!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"shocking trap",    { 37, 97, 97,  97, 97, 97, 97, 97 ,40, 97, 97, 37,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_shocking_trap,    TAR_IGNORE, POS_STAND,
		NULL,       SLOT(615),  150,    24,
		'f',   "trappola",    "","",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"sleep",        { 97, 97, 97, 97, 97, 25, 97, 97, 39, 97, 97, 39, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sleep,        TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_sleep,     SLOT(38),   15, 12,
		'\0',   "",         "Riacquisti le forze.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BEGUILING
	},

	{
		"slow",                 { 28,38, 97, 97,  97, 28, 97, 97 ,23, 97,28,29,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_slow,             TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		&gsn_slow,                   SLOT(515),      30,     12,
		'\0',   "",                     "L'effetto rilassante sui tuoi muscoli cessa.",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"soften",       { 97, 97, 97, 97, 97, 97, 97, 97 ,58, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_soften,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(713),   75,    12,
		'\0',   "soften",   "L'effetto di morbidezza sulla tua pelle cessa.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_WEATHER
	},

	{
		"solar flight",     {97,35, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_solar_flight, TAR_IGNORE, POS_FIGHT,
		NULL,           SLOT(659),  80, 12,
		'\0',   "",         "!Solar Flight!",   "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"sonic resonance",  { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 28,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sonic_resonance,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(649),  20, 12,
		'f',   "tempesta sonica",  "!sonic resonance!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"soul bind",        { 97, 97, 97, 97, 97, 39, 97, 97,  97, 97, 97,54,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_soul_bind,    TAR_CHAR_OFFENSIVE, POS_STAND,
		NULL,       SLOT(716),   5, 12,
		'\0',   "",     "Riacquisti le tue facolta' mentali.",    "",
		CABAL_NONE, RACE_NONE, ALIGN_NONE, GROUP_BEGUILING
	},

	{
		"spectral furor",   { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 35,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_spectral_furor,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(646),  20, 12,
		'f',   "furia spettrale",   "!spectral furor!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"stone skin",       { 25, 36, 97, 97, 97, 97, 35, 97, 97, 97,60,30,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_stone_skin,   TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(66),   12, 18,
		'\0',   "",         "La tua pelle perde la rigidita'..",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"suffocate",      {  97, 97, 97, 97,  97, 97, 97, 97,  48, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_suffocate,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,             SLOT(714),        50,       12,
		'm',   "soffocamento",     "Torni a respirare.", "",CABAL_NONE,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"sulfurus spray",   { 97, 97, 97, 97, 97, 97, 97, 97, 28, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sulfurus_spray,   TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(652),  20, 12,
		'm',   "spray solforoso",   "!sulfurus spray!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"summon",       {  23, 22, 97, 97,51,50, 97, 97, 24, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon,       TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(40),   50, 12,
		'\0',   "",         "!Summon!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"summon zombie",     {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,25,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_zombie, TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(662),  200,        12,
		'\0',   "", "","",
		CABAL_NONE ,    RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon skeleton",      {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,39,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_skeleton,  TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(663),  300,        18,
		'\0',   "", "","",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon spectre",       {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,52,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_spectre,   TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(664),  400,        24,
		'\0',   "", "","",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon banshee",{97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,71,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_banshee, TAR_IGNORE, POS_STAND,
		NULL,       SLOT(665),  500,        30,
		'\0',   "", "","",
		CABAL_NONE ,    RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon air elemental", { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,42},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_air_elm,   TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(696),  50, 12,
		'\0',   "", "",
		"",     CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon earth elemental",{ 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,38},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_earth_elm, TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(693),  50, 12,
		'\0',   "", "",
		"",     CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon fire elemental",{ 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,58},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_fire_elm,  TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(697),  50, 12,
		'\0',   "", "",
		"",     CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon lightning elemental",{ 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,68},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_light_elm, TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(710),  50, 12,
		'\0',   "", "",
		"",     CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon water elemental",{ 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,48},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_water_elm, TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(698),  50, 12,
		'\0',   "", "",
		"",     CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"summon shadow",    { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 50,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_summon_shadow,    TAR_CHAR_SELF,      POS_STAND,
		NULL,       SLOT(620),  200,    24,
		'\0',   "", "",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"superior heal",    {  97, 38, 97, 97, 43, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_super_heal,   TAR_CHAR_DEFENSIVE,POS_FIGHT,
		NULL,           SLOT(640),  100,    12,
		'\0',   "",         "!Super Heal!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_HEALING
	},

	{
		"tattoo",    { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{1,1,1,1,1,1,1,1,1, 1,1,1,1},
		spell_tattoo, TAR_CHAR_DEFENSIVE,       POS_STAND,
		NULL,       SLOT(551),        10,       0,
		'\0',   "", "!tattoo!", "",CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"teleport",     {  22, 32, 97, 97, 97, 7, 97,97 ,23, 97, 97, 16,16},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_teleport,     TAR_CHAR_SELF,      POS_FIGHT,
		&gsn_teleport,          SLOT( 2),   35, 12,
		'\0',   "",         "!Teleport!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

	{
		"transfer object",  { 62, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_transfer_object,  TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(708),  40, 12,
		'\0',   "",     "!transfer object!", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"tsunami",      { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,62},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_tsunami,      TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,           SLOT(701),  50, 12,
		'f',   "onda devastante",    "!tsunami!",    "",     CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"turn",     {   97, 50, 97, 97,60, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_turn, TAR_IGNORE, POS_FIGHT,
		NULL,       SLOT(597),  50, 12,
		'm',   "comando divino", "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"vampiric blast",   { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97 ,38, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_vam_blast,    TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(611),  20, 12,
		'm',   "scoppio acido",   "!Vampiric Blast!", "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"ventriloquate",    {   1, 97, 97, 97, 97, 97, 97, 97, 1, 97, 97, 97,3},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_ventriloquate,    TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(41),    5, 12,
		'\0',   "",         "!Ventriloquate!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_ILLUSION
	},

	{
		"web",      {   97, 97, 97, 97, 97, 97, 97, 97,69 , 97, 97, 58,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_web,      TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_web,       SLOT(592),  50, 12,
		'\0',   "",     "I filamenti di ragnatela che ti imprigionavano cedono.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_ATTACK
	},

	{
		"windwall",     { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,47},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_windwall,     TAR_IGNORE,     POS_FIGHT,
		NULL,           SLOT(694),  20, 12,
		'f',   "tempesta ventosa",        "I tuoi occhi stanno meglio ora.",   "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_COMBAT
	},

	{
		"witch curse",      {   97, 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_witch_curse,  TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_witch_curse,       SLOT(602),  150,    24,
		'\0',   "",         "Ti senti meglio.. la maledizione delle streghe ti ha abbandonato.",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"wrath",         { 97, 97, 97, 97, 34, 97, 97, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_wrath, TAR_CHAR_OFFENSIVE,       POS_STAND,
		&gsn_wrath,       SLOT(553),        20,       12,
		'm',   "furore celeste", "La maledizione svanisce.", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BENEDICTIONS
	},

	{
		"weaken",   {   97, 24, 97, 97, 97, 97, 97, 97 ,19, 97,16,29,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_weaken,       TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_weaken,        SLOT(68),   20, 12,
		'\0',   "attacco indebolente",        "Recuperi le tue forze.",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MALADICTIONS
	},

	{
		"word of recall",   { 32, 28, 97, 97,32, 97,29,97 ,32,38,10,10,32},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_word_of_recall,   TAR_CHAR_SELF,      POS_REST,
		NULL,           SLOT(42),    5, 12,
		'\0',   "",         "!Word of Recall!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_TRANSPORTATION
	},

/*
 * Dragon breath
 */
	{
		"acid breath",      {  67, 67, 67, 67,67,67,67,67 ,67,67,67,67,67},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_acid_breath,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_acid_breath,       SLOT(200),  100,    24,
		'm',   "soffio acido",    "!Acid Breath!",    "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"desert heat",      { 69, 69, 69, 69,69,69,69,69,69,69,69,69,69 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(629),  200,    24,
		'm',   "soffio di sabbia cocente", "Rimuovi la sabbia dai tuoi occhi.",    "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"fire breath",      { 69, 69, 69, 69,69,69,69,69,69,69,69,69,69 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_fire_breath,  TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_fire_breath,       SLOT(201),  200,    24,
		'm',   "soffio infuocato",   "Il fumo smette di farti lacrimare gli occhi.",  "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"frost breath",     { 61,61,61,61,61,61,61,61,61,61,61,61,61  },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_frost_breath, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_frost_breath,      SLOT(202),  125,    24,
		'm',   "soffio di ghiaccio",   "!Frost Breath!",   "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"gas breath",       { 70,70,70,70,70,70,70,70,70,70,70,70,70  },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_gas_breath,   TAR_IGNORE,     POS_FIGHT,
		&gsn_gas_breath,    SLOT(203),  175,    24,
		'm',   "soffio gassoso",     "!Gas Breath!",     "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"lightning breath", { 64,64,64,64,64,64,64,64,64,64,64,64,64  },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_lightning_breath, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		&gsn_lightning_breath,      SLOT(204),  150,    24,
		'm',   "riverbero fulminante",   "!Lightning Breath!",   "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"lightning stroke",     { 69, 69, 69, 69,69,69,69,69,69,69,69,69,69 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(632),  200,    24,
		'f',   "tempesta fulminante",  "!lightning stroke!",   "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"luck bonus",   { 67, 67, 67, 67,67,67,67,67,67,67,67,67,67 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_DEFENSIVE, POS_STAND,
		NULL,       SLOT( 630),  20,    12,
		'\0',   "",     "", "", CABAL_NONE,
		RACE_TOP, ALIGN_NONE, GROUP_PROTECTIVE
	},

	{
		"paralyzation",     { 69, 69, 69, 69,69,69,69,69,69,69,69,69,69 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_IGNORE, POS_FIGHT,
		NULL,       SLOT(631),  200,    24,
		'm',   "gas paralizzante",  "A stento capisci che i muscoli hanno ripreso a risponderti.", "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"repulsion",        { 65, 65, 65, 65,65,65,65,65,65,65,65,65,65 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(633),  200,    24,
		'm',   "attacco respingente",    "!repulsion!",  "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"sleep gas",    { 67, 67, 67, 67,67,67,67,67,67,67,67,67,67 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_IGNORE, POS_FIGHT,
		NULL,       SLOT(628),  200,    24,
		'm',   "gas soporifero",    "Ti senti svenire.",    "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},

	{
		"slow gas", { 69, 69, 69, 69,69,69,69,69,69,69,69,69,69 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(634),  200,    24,
		'm',   "gas ammorbante", "Torni a muoverti normalmente.", "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_DRACONIAN
	},


/*
 * Skills for mobiles. (general purpose and high explosive from
 * Glop/Erkenbrand
 */
	{
		"crush",    {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_crush,             SLOT( 0),       0,      18,
		'm',   "tentativo di schiacciamento",                 "!crush!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"general purpose",      { 97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		NULL,                   SLOT(401),      0,      12,
		'm',   "colpo esplosivo", "!General Purpose Ammo!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"high explosive",       { 97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		NULL,                   SLOT(402),      0,      12,
		'm',   "botto esplosivo",  "!High Explosive Ammo!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"tail",         {  53, 53,53, 53,53,53,53,53,53 ,53,53,53,53},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_tail,              SLOT( 0),       0,      18,
		'm',   "colpo di coda",                 "!Tail!",       "", CABAL_NONE ,
		RACE_TOP, ALIGN_NONE, GROUP_FIGHTMASTER
	},


/*
 * Combat and weapons skills
 */
	{
		"arrow",    { 97, 97, 97,  97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_IGNORE, POS_STAND,
		&gsn_arrow, SLOT(0),    0,  0,
		'f',   "freccia",    "!arrow!",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"axe",          {  97, 97, 97,  1, 97,1, 97,1 ,97 , 97, 97, 97,97},
		{ 2, 2, 1, 1,1,1,1,1,1, 1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_axe,               SLOT( 0),       0,      0,
		'f'  ,      "",                     "!Axe!",        "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"bow",  { 97, 97, 97,  44, 97, 97, 97,35 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_IGNORE, POS_STAND,
		&gsn_bow,       SLOT(0),    0,  12,
		'm',   "arco",      "!bow!",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"dagger",               { 1,  97,  1,  1,1,1,1,1 ,1 ,1,1,1,1},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_dagger,            SLOT( 0),       0,      0,
		'f',   "",                     "!Dagger!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"flail",        { 97,  1, 97,  1, 97,1, 97,97 , 97, 97, 97, 97, 1},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_flail,             SLOT( 0),       0,      0,
		'm',   "",                     "!Flail!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"lance",    { 33,33,33,33,33,33,33,33,33,33,33,33,33},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_IGNORE, POS_STAND,
		&gsn_lance, SLOT(0),    0,  0,
		'f',   "lancia",    "!lance!",  "",
		CABAL_KNIGHT , RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"mace",         {  97,  1,  1,  1,1,1, 97,97 ,97 ,1, 97, 97, 97},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_mace,              SLOT( 0),       0,      0,
		'f',   "",                     "!Mace!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"polearm",      { 97, 97, 97,  1,1,1, 97, 97, 97 , 97, 97, 97,97},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_polearm,           SLOT( 0),       0,      0,
		'm',   "",                     "!Polearm!",        "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"shield block",     { 97,17,7,1,1,1,12,10,97 ,10, 97, 97, 97},
		{ 2, 1, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_shield_block,  SLOT(0),    0,  0,
		'\0',   "",         "!Shield!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DEFENSIVE
	},

	{
		"spear",        {  97,  97,  1,  1, 97,1,1,1 , 97,1,8, 97,97 },
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_spear,             SLOT( 0),       0,      12,
		'm',   "arpione",                     "!Spear!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"sword",        {  97, 97,  1,  1,1,1,1,1,97 ,1,1, 97,97},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_sword,             SLOT( 0),       0,      0,
		'f',   "",                     "!sword!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"whip",         {  1, 1,  97,  1, 97,1, 97,1 ,1, 97,1,1,1},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_whip,              SLOT( 0),       0,      0,
		'f',   "",                     "!Whip!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"second weapon",    {  97, 97, 97,23,48,48, 97,23, 97,24, 97, 97, 97},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_second_weapon,             SLOT( 0),       0,      0,
		'\0',   "", "!second weapon!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"ambush",         {  97, 97, 97, 97, 97, 97, 97, 17,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_CHAR_OFFENSIVE,          POS_STAND,
		&gsn_ambush,                    SLOT(0),        0,       12,
		'f',   "imboscata", "!Ambush!", "",  CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"area attack",        {97, 97, 97, 73, 97, 97, 97, 97, 97,69, 97, 97, 97},
		{1,1,1,1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_area_attack,     SLOT(0),        0,      0,
		'\0',   "",         "!Area Attack!",    "", CABAL_NONE,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"assassinate",         {  97, 97, 97, 97, 97, 97, 65, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,          POS_STAND,
		&gsn_assassinate,       SLOT(0),        0,       18,
		'm',   "soffocamento", "!assassinate!", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"backstab",             {  97, 97,  11, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97 },
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_STAND,
		&gsn_backstab,          SLOT( 0),        0,     12,
		'f',   "pugnalata alle spalle",             "!Backstab!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"bash",         {  97, 97, 97,1, 6,1,1,10 ,97 , 97,11, 97,97},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_bash,              SLOT( 0),       0,      18,
		'f',   "spallata",                 "!Bash!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"bash door",    {  97, 97, 97,  1,6,1,1,97 , 97, 97, 11, 97,97},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_bash_door,         SLOT( 0),       0,      18,
		'f',   "spallata",                 "!Bash Door!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"bear call",         {  97, 97, 97, 97, 97, 97, 97, 31, 97, 97, 97, 97, 97 },
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,          POS_FIGHT,
		&gsn_bear_call,     SLOT(518),  50,       0,
		'\0',   "", "", "",  CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"berserk",      { 97, 97, 97, 18, 97, 97, 97,97 , 97,20, 97, 97, 97 },
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_berserk,           SLOT( 0),       0,      24,
		'\0',   "",                     "",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"blackguard",   {  97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_OFFENSIVE,   POS_STAND,
		&gsn_blackguard,        SLOT(0),    0,       12,
		'\0',   "Sesto senso.",       "Il senso di allerta ti abbandona.","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"blackjack",        {  97, 97, 30, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_OFFENSIVE,   POS_STAND,
		&gsn_blackjack,     SLOT(0),    0,       8,
		'f',   "randellata",        "Il dolore sulla nuca ti abbandona.","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"blind fighting",   {   97, 97, 97, 13,32, 97,11,43,97 ,23, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_blind_fighting,        SLOT(0),    0,  0,
		'\0',   "",         "!blind fighting!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"blindness dust",         {  97, 97, 97, 97, 97, 97, 28, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,         TAR_IGNORE,             POS_FIGHT,
		&gsn_blindness_dust,    SLOT(0),        20,       18,
		'\0',   "",                     "!blindness dust!", "",  CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"blink",                {  1, 97,  97, 97, 97, 97, 97, 97 ,1, 97, 97, 1,1},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_blink,             SLOT( 0),        0,     0,
		'\0',   "",                     "!Blink!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"butcher",         {  97, 97, 97, 97, 97, 97, 97, 13 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_IGNORE,          POS_STAND,
		&gsn_butcher,             SLOT(0),        0,       0,
		'\0',   "", "!butcher!", "",    CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"caltraps",         {  97, 97, 97, 97, 97, 97, 18, 97 , 97,36, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,          POS_FIGHT,
		&gsn_caltraps,       SLOT(0),        0,       18,
		'm',   "shaken", "", "",    CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"camouflage",         {  97, 97, 97, 97, 97, 97, 97, 4 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_IGNORE,           POS_STAND,
		&gsn_camouflage,         SLOT(0),        0,       24,
		'\0',   "",         "!Camouflage!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"camouflage move",  { 97, 97, 97, 97, 97, 97, 97,23, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_move_camf,     SLOT(0),    0,  12,
		'\0',   "",     "!move camouflaged!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"camp",         {  97, 97, 97,  97, 97, 97, 97, 48 ,97 , 97, 97, 97,97},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_STAND,
		&gsn_camp,              SLOT( 0),       0,      24,
		'm',   "accampamento",          "","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"circle",         {  97, 97, 7, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_CHAR_OFFENSIVE,          POS_FIGHT,
		&gsn_circle,             SLOT(0),        0,       18,
		'f',   "pugnalata aggirante", "!Circle!", "",      CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"control animal",   { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,43, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_control_animal,    SLOT( 0),    5, 12,
		'\0',   "",         "Riacquisti il controllo di te.",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_BEGUILING
	},

	{
		"cleave",         {  97, 97, 97, 97, 97, 55, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_OFFENSIVE,     POS_STAND,
		&gsn_cleave,         SLOT(0),        0,       24,
		'm',   "colpo improvviso",         "!Cleave!", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"concentrate",         { 97, 97, 97, 59, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_concentrate,        SLOT( 0),        0,     18,
		'\0',   "",      "",  "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"counter",         {  97, 97, 97, 28, 35, 97, 97, 97 , 97,28, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_IGNORE,           POS_STAND,
		&gsn_counter,         SLOT(0),        0,       0,
		'\0',   "",         "!Counter!", "",        CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"critical strike",  {   97, 97, 97, 97, 97, 97,58, 97,97 ,62, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_critical,          SLOT(0),    0,  18,
		'\0',   "",         "!critical strike!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"cross block",          {  97, 97, 97,31,56,56, 97,31, 97,32, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_cross_block,       SLOT( 0),        0,     0,
		'\0',   "",                     "!cross block!",    "", CABAL_NONE ,
		RACE_NONE,ALIGN_NONE, GROUP_DEFENSIVE
	},

	{
		"detect hidden",    { 97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_detect_hidden,    TAR_CHAR_SELF,      POS_STAND,
		NULL,       SLOT(44),    5, 12,
		'\0',   "",         "",
		"", CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"detect sneak",     {   97, 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_SELF,      POS_STAND,
		&gsn_detect_sneak,      SLOT(0),    20, 18,
		'\0',   "",         "!detect sneak!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DETECTION
	},

	{
		"dirt kicking",     { 97, 97,  3,  8, 97,3,3,5,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_dirt,      SLOT( 0),   0,  12,
		'm',   "calcio sporco",      "Riesci a togliere la terra dagli occhi.",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"disarm",               {  97, 97, 28, 20,20,21,23,21, 97,21, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_disarm,            SLOT( 0),        0,     18,
		'\0',   "",                     "!Disarm!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"dodge",                {  33, 97,1,13, 21,21,1,9, 97,15,8, 97,27},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_dodge,             SLOT( 0),        0,     0,
		'\0',   "",                     "!Dodge!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DEFENSIVE
	},

	{
		"dual backstab",         {  97, 97, 38, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_CHAR_OFFENSIVE,           POS_STAND,
		&gsn_dual_backstab,       SLOT(0),        0,       0,
		'f',   "pugnalata alle spalle",      "!dual backstab!", "",      CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"enchant sword",    { 97, 97, 97, 97, 97, 97, 97, 97, 97, 42, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_OBJ_INV,        POS_STAND,
		&gsn_enchant_sword, SLOT(0),    100,    24,
		'\0',   "", "!Enchant sword!",  "", CABAL_NONE , RACE_NONE,
		ALIGN_NONE, GROUP_NONE
	},

	{
		"endure",         {  97, 97, 97, 97, 97, 97, 20, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_CHAR_SELF,          POS_FIGHT,
		&gsn_endure,             SLOT(0),        0,       24,
		'\0',   "", "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"enhanced damage",      {  97, 33, 25,  3,6,3,16,15 , 97,8,14, 97,97},
		{ 1,  2,  1,  1, 1, 1, 1, 1, 1, 1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_enhanced_damage,   SLOT( 0),        0,     0,
		'\0',   "",                     "!Enhanced Damage!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"entangle",         {  97, 97, 97, 97, 97, 97, 97,26 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_entangle,      TAR_CHAR_OFFENSIVE,   POS_FIGHT,
		&gsn_entangle,       SLOT(568),        40,       12,
		'm',   "intrappolamento", "Riesci finalmente a muoverti normalmente.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"envenom",      { 97, 97, 15, 97, 97, 97, 97, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_REST,
		&gsn_envenom,       SLOT(0),    0,  12,
		'\0',   "",         "!Envenom!",        "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"escape",       {97, 97, 45, 97, 97, 97,55, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_escape,        SLOT( 0),   0,  24,
		'\0',   "", "!escape!","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"explode",         {  97, 97, 97, 97, 97, 97, 97, 97 , 97,46, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_OFFENSIVE,          POS_FIGHT,
		&gsn_explode,       SLOT(0),        100,       24,
		'm',   "attacco bruciante", "Torni a vederci.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"ground strike",    { 97, 97, 97, 97,  97, 97, 97,52, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_ground_strike, SLOT(0),    0,  18,
		'\0',   "",         "!ground strike!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"hand block",                { 97, 97, 97, 97, 97, 97, 22, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_hand_block,             SLOT( 0),        0,     0,
		'\0',   "",                     "!hand block!",     "", CABAL_NONE ,
		RACE_NONE,ALIGN_NONE, GROUP_DEFENSIVE
	},

	{
		"hand to hand",     { 45,  30, 18, 9,11,18,3,24,45,12,19,45,35 },
		{ 2,  1,  1,  1, 1, 1, 1, 1, 1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_hand_to_hand,  SLOT( 0),   0,  0,
		'\0',   "",         "!Hand to Hand!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"hara kiri",        { 97, 97, 97, 97, 97, 97, 97, 97, 97, 25, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE, POS_FIGHT,
		&gsn_hara_kiri,     SLOT(0),    50, 12,
		'\0',   "", "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"headguard",    {  97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_OFFENSIVE,   POS_STAND,
		&gsn_headguard,     SLOT(0),    0,       12,
		'm',   "Sesto senso.",        "Ti senti meno all'erta.","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"herbs",         {  97, 97, 97, 97, 97, 97, 97, 18 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_CHAR_DEFENSIVE,          POS_STAND,
		&gsn_herbs,             SLOT(0),        0,       30,
		'\0',   "", "", "",  CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"kick",                 {  97, 12, 14,  5, 97,9,1,8 , 97,2,4, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		&gsn_kick,              SLOT( 0),        0,     12,
		'm',   "calcio",                 "!Kick!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"lash",         { 48,48, 97, 97,  48, 97,48, 97, 48, 97,48,48,48},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_lash,              SLOT( 0),       0,      4,
		'f',   "frusta",                 "!Lash!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"light resistance", {  97,  97, 97, 97, 97, 97, 97, 97, 97, 97,50, 97,97},
		{ 1,  1,  2,  2, 1, 1, 1, 1,1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_SLEEP,
		&gsn_light_res,     SLOT( 0),   0,  0,
		'\0',   "",         "Light Resistance","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"lion call",        { 97, 97, 97, 97, 97, 97, 97, 55, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_DEFENSIVE, POS_FIGHT,
		&gsn_lion_call,     SLOT(0),    50, 12,
		'\0',   "",     "!lion call!",      "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"make arrow",       { 97, 97, 97, 97, 97, 97, 97,37, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_make_arrow,    SLOT(0),     50,    24,
		'\0',   "",     "!make arrow!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"make bow",     { 97, 97, 97, 97, 97, 97, 97,36, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_make_bow,      SLOT(0),     200,   24,
		'\0',   "",     "!make bow!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"mental attack",    { 97, 97, 97,  97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		&gsn_mental_attack,     SLOT(0),    200,    24,
		'\0',   "", "!mental attack!",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"neckguard",    {  97, 97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_CHAR_OFFENSIVE,   POS_STAND,
		&gsn_neckguard,     SLOT(0),    0,       12,
		'm',   "sesto senso",        "Ti senti meno all'erta.","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"nerve",         {  97, 97, 97, 97, 97, 97, 29, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_CHAR_OFFENSIVE,          POS_FIGHT,
		&gsn_nerve,             SLOT(0),        0,       18,
		'\0',   "", "Senti che i tuoi muscoli tornano attivi come prima.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"parry",                { 97,25,19,1, 3,5, 97,3, 23,1,22, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_parry,             SLOT( 0),        0,     0,
		'\0',   "",                     "!Parry!",      "", CABAL_NONE ,
		RACE_NONE,ALIGN_NONE, GROUP_DEFENSIVE
	},

	{
		"perception",      {  97, 97, 23, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_IGNORE,          POS_STAND,
		&gsn_perception,             SLOT(0),        0,       0,
		'\0',   "", "!perception!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"push",         {  97, 97, 53,  97, 97, 97, 97, 97 ,97 , 97, 97, 97,97},
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_STAND,
		&gsn_push,              SLOT( 0),       0,      18,
		'f',   "spintarella",          "!push!","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"rescue",               {  97, 97, 97,  1,1, 97, 97, 97, 97, 23, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_rescue,            SLOT( 0),        0,     12,
		'\0',   "",                     "!Rescue!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_DEFENSIVE
	},

	{
		"sense life",   { 97, 97, 97,  97, 97, 97, 97, 97 , 97, 97, 13, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		&gsn_sense_life,    SLOT(623),  20, 12,
		'\0',   "", "",    "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"settraps", { 97, 97, 59, 97, 97, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		&gsn_settraps,      SLOT(0),    200,    24,
		'f',   "trappola", "",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"shield cleave",{ 97, 97, 33, 26, 29, 29, 31, 35, 97, 26, 39, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		&gsn_shield_cleave,     SLOT(0),    200,    24,
		'\0',   "", "!shield cleave!",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"smithing", {   97, 97, 97, 50, 97, 97, 97, 97, 97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_smithing,          SLOT(0),    10, 18,
		'\0',   "",         "!smithing!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"spell craft",      { 10,30, 97, 97,  97, 97, 97, 97,  10, 97, 97, 10,10 },
		{ 1,  1,  1,  1, 1, 1, 2, 2, 1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_spell_craft,   SLOT(0),    0,       0,
		'f',   "comprensione della pronuncia arcana",      "!spell craft!","", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"strangle",         {  97,  97,  97,  97, 97, 97,26,97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_OFFENSIVE,          POS_STAND,
		&gsn_strangle,       SLOT(0),        0,       8,
		'm',   "strangolamento", "Il tuo collo ti fa un po' meno male ora.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"swimming", {   97, 97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_swimming,      SLOT(0),    0,  0,
		'\0',   "",         "!swimming!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"target",                 {  97, 97, 97,  97, 97, 97, 97, 97 , 97,40, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHT,
		&gsn_target,              SLOT( 0),        0,     12,
		'm',   "bersaglio",                 "!Kick!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"throw",         {  97, 97, 97, 97, 97, 97, 25, 97 , 97,37, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_OFFENSIVE,       POS_FIGHT,
		&gsn_throw,       SLOT(0),        0,       18,
		'f',   "proiezione", "!throw!", "", CABAL_NONE , RACE_NONE,ALIGN_NONE, GROUP_NONE
	},

	{
		"tiger power",      { 97, 97, 97, 97,  97, 97, 97,61, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE, POS_FIGHT,
		&gsn_tiger_power,   SLOT(0),    0,  12,
		'\0',   "", "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"track",         {  97, 97, 97, 97, 97, 97, 97,39, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_STAND,
		&gsn_track,       SLOT(0),        0,       12,
		'\0',   "", "!track!", "",CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"trip",         {  97, 97,  3, 25, 97,10,8,97 , 97,20, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_trip,      SLOT( 0),   0,  12,
		'm',   "sgambetto",         "!Trip!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"vampire",      { 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,10, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_IGNORE,     POS_STAND,
		&gsn_vampire,       SLOT(0),    100,    12,
		'\0',   "", "Torni al tuo aspetto non vampirico.",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"vampiric bite",         {  97, 97,  97, 97, 97, 97, 97, 97 , 97, 97, 10, 97,97 },
		{ 1,  1,  1,  1, 1, 1, 1, 1, 1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_STAND,
		&gsn_vampiric_bite,          SLOT( 0),        0,     12,
		'm',   "morso",             "!vampiric bite!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"vampiric touch",         {  97,  97,  97,  97, 97, 97, 97, 97 , 97, 97, 15, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_OFFENSIVE,          POS_STAND,
		&gsn_vampiric_touch,       SLOT(0),        0,       12,
		'm',   "tocco", "Ti risvegli da un sonno agitato.", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"vanish",         {  97, 97, 97, 97, 97, 97, 18, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_SELF,          POS_FIGHT,
		&gsn_vanish,       SLOT(521),        25,       18,
		'\0',   "", "!vanish!", "",     CABAL_NONE , RACE_NONE,ALIGN_NONE, GROUP_NONE
	},

	{
		"warcry",         {  97, 97, 97,31, 97, 97, 97,35,97 ,30, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_FIGHT,
		&gsn_warcry,       SLOT(0),        30,       12,
		'\0',   "", "", "",    CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"weapon cleave", { 97, 97, 37, 29, 34, 34, 35, 38, 97, 29, 44, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		&gsn_weapon_cleave, SLOT(0),    200,    24,
		'\0',   "", "!weapon cleave!",  "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"second attack",        { 34, 28, 18, 10, 13, 14, 15, 13, 25, 10, 12, 34, 34 },
		{ 2,  1,  1,  1, 1, 1, 1, 1, 1, 1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_second_attack,     SLOT( 0),        0,     0,
		'\0',   "",                     "!Second Attack!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"secondary attack", {97, 97, 97,52, 61,61, 97,58 , 97,55, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		&gsn_secondary_attack,  SLOT(0),    200,    24,
		'\0',   "", "!secondary attack!",   "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"third attack",         { 97, 97,36,19,27,26,26,45, 97,20,32, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_third_attack,      SLOT( 0),        0,     0,
		'\0',   "",                     "!Third Attack!",   "", CABAL_NONE ,
		RACE_NONE,ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"fourth attack",        {97, 97, 97,30, 97, 97, 48, 97, 97, 97, 97, 97, 97},
		{1,1,1,1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_fourth_attack,     SLOT(0),        0,      0,
		'\0',   "",                     "!Fourth Attack!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"fifth attack",        {97, 97, 97, 52, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{1,1,1,1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_fifth_attack,     SLOT(0),        0,      0,
		'\0',   "",          "!Fifth Attack!",  "", CABAL_NONE,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},


/*
 * Non-combat skills
 */
	{
		"blue arrow",       { 97, 97, 97, 97, 97, 97, 97,51, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_blue_arrow,    SLOT(0),     50,    12,
		'\0',   "",     "!blue arrow!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"fast healing",     {  15, 9, 16,  6,7,7,9,6 ,15 ,3,9,9,9},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_SLEEP,
		&gsn_fast_healing,  SLOT( 0),   0,  0,
		'\0',   "",         "!Fast Healing!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MEDITATION
	},

	{
		"green arrow",      { 97, 97, 97, 97, 97, 97, 97,40, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_green_arrow,   SLOT(0),     50,    12,
		'\0',   "",     "!green arrow!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"grip",           { 97, 97, 97,43, 97, 97, 97, 97, 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_grip,            SLOT( 0),        0,     18,
		'\0',   "",                     "!Grip!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"haggle",       {  7, 18,  1, 14,12,8,17,15 ,7 ,19,5,5,5},
		{ 2,  2,  1,  2, 2, 2, 1, 2, 1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_REST,
		&gsn_haggle,        SLOT( 0),   0,  0,
		'\0',   "",         "!Haggle!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"hide",         {  97, 97,  1,  97, 97, 97,10,97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_REST,
		&gsn_hide,      SLOT( 0),    0, 18,
		'\0',   "",         "!Hide!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"katana",   { 97, 97, 97, 97, 97, 97, 97, 97, 97, 72, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_OBJ_INV,        POS_STAND,
		&gsn_katana,        SLOT(0),    100,    24,
		'\0',   "",     "", "",
		CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"lay hands",        {97, 97, 97, 97, 63, 97, 97, 97, 97, 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_lay_hands,     SLOT( 0),   0,  12,
		'\0',   "", "", "",CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"lore",         {  97, 97, 13,21,19,20,19,18, 97,22,20, 97,97},
		{ 2,  2,  2,  2, 2, 2, 2, 2, 2, 2,2,2,2},
		spell_null,     TAR_IGNORE,     POS_REST,
		&gsn_lore,      SLOT( 0),   0,  36,
		'\0',   "",         "!Lore!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"mastering pound",  { 97, 97, 97, 97, 97, 97, 61, 97, 97, 97, 97, 97,97},
		{ 2,  1,  1,  1, 1, 1, 1, 1, 1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_master_hand,   SLOT( 0),   0,  0,
		'\0',   "",         "!Master Hand!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_FIGHTMASTER
	},

	{
		"mastering sword",  { 97, 97, 97, 97, 97, 97, 97, 97, 97,61, 97, 97, 97},
		{ 2, 2, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_FIGHT,
		&gsn_master_sword,           SLOT( 0),       0,      0,
		'\0',   "",                     "!master sword!",   "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WEAPONSMASTER
	},

	{
		"meditation",       {  6,  6, 15, 15,14,15,14,16,6,16,6,6,6},
		{ 1,  1,  2,  2, 1, 1, 1, 1,1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_SLEEP,
		&gsn_meditation,    SLOT( 0),   0,  0,
		'\0',   "",         "Meditation",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_MEDITATION
	},

	{
		"peek",         {  97, 97,  1, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_peek,      SLOT( 0),    0,  0,
		'\0',   "",         "!Peek!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"pick lock",        { 97, 97,  17, 97, 97, 97,34, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_pick_lock,     SLOT( 0),    0, 12,
		'\0',   "",         "!Pick!",       "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"poison smoke",         {  97, 97, 97, 97, 97, 97, 30, 97, 97, 97, 97, 97,97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,         TAR_IGNORE,            POS_FIGHT,
		&gsn_poison_smoke,      SLOT(0),        20,     18,
		'\0',   "",                     "!poison smoke!", "",   CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"quiet movement",         {  97, 97, 97, 97, 97, 97, 97, 15 , 97, 97, 10, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_IGNORE,          POS_STAND,
		&gsn_quiet_movement,             SLOT(0),        0,       0,
		'\0',   "", "!quiet movement!", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"recall",       {  1,  1,  1,  1,1,1,1,1 ,1,1,1,1,1},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_recall,        SLOT( 0),   0,  24,
		'\0',   "",         "!Recall!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"red arrow",        { 97, 97, 97, 97, 97, 97, 97,44, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_red_arrow, SLOT(0),     50,    12,
		'\0',   "",     "!red arrow!",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"sneak",        {  97, 97, 4, 97, 97, 97, 18, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_sneak,     SLOT( 0),    0, 12,
		'\0',   "",         "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"steal",        {  97, 97, 12, 97, 97, 97, 97, 97,97 , 97, 97, 97,97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_steal,     SLOT( 0),    0, 12,
		'\0',   "",         "!Steal!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"scrolls",      {  1,  1,  1,  97, 97, 1, 97,1 , 1,6,1,1,1},
		{ 1,  1,  2,  2, 2, 2, 2, 2, 2, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_scrolls,       SLOT( 0),   0,  12,
		'\0',   "",         "!Scrolls!",        "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WIZARD
	},

	{
		"staves",       {  1,  1,  97,  97, 97, 97,1,1 ,1,1,1,1,1},
		{ 1,  1,  2,  2, 2, 2, 2, 2, 2, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_staves,        SLOT( 0),   0,  12,
		'\0',   "",         "!Staves!",     "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WIZARD
	},

	{
		"tame",         {  97, 97, 97, 97, 97, 97, 97, 39 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,        TAR_CHAR_DEFENSIVE,          POS_FIGHT,
		&gsn_tame,             SLOT(0),        0,       24,
		'\0',   "", "!tame!", "",CABAL_NONE , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"trance",         {  14,15, 97, 97, 97, 97, 97, 97, 14 , 97,21,21,21},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,   POS_SLEEP,
		&gsn_trance,       SLOT(0),        0,       0,
		'\0',   "", "", "", CABAL_NONE , RACE_NONE,ALIGN_NONE, GROUP_MEDITATION
	},

	{
		"white arrow",      { 97, 97, 97, 97, 97, 97, 97,49, 97, 97, 97, 97, 97 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_white_arrow,   SLOT(0),     50,    12,
		'\0',   "",     "!white arrow!",    "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"wands",        {  1,  1,  97,  97, 97, 1, 97,97 ,1,5,1,1,1},
		{ 1,  1,  2,  2, 2, 2, 2, 2, 1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_wands,     SLOT( 0),   0,  12,
		'\0',   "",         "!Wands!",      "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_WIZARD
	},


/*
 * Cabals
 */
	{
		"mortal strike",    { 63,63,63,63,63,63,63,63,63,63,63,63,63},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null, TAR_CHAR_SELF,      POS_STAND,
		&gsn_mortal_strike,     SLOT(0),    200,    24,
		'm',   "colpo devastante",    "!mortal strike!",  "",
		CABAL_ARMY , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"disgrace", {  33, 33, 33, 33, 33,33,33,33,33,33,33,33,33},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_disgrace, TAR_CHAR_OFFENSIVE,     POS_STAND,
		NULL,       SLOT(619),  200,    12,
		'\0',   "", "",   "",
		CABAL_CHAOS , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"evil spirit",  {  33, 33, 33, 33, 33,33,33,33,33,33,33,33,33},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_evil_spirit,  TAR_IGNORE,     POS_STAND,
		&gsn_evil_spirit,       SLOT(618),  800,    36,
		'm',   "spirito nero",  "Riacquisti completamente il tuo spirito.",   "",
		CABAL_INVADER , RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"ruler aura",      {  33, 33, 33, 33, 33,33,33,33,33,33,33,33,33},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_ruler_aura,    TAR_CHAR_SELF,          POS_STAND,
		NULL,             SLOT(667),        20,       12,
		'\0',   "", "",   "", CABAL_RULER ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"sword of justice", {48,48,48,48, 48,48,48,48,48,48,48,48,48},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sword_of_justice, TAR_CHAR_OFFENSIVE, POS_FIGHT,
		NULL,       SLOT(686),  50, 12,
		'f',   "spada della giustizia", "!sword of justice!", "", CABAL_RULER ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"bandage",        {23, 23, 23, 23,23,23,23,23,23,23,23,23,23},
		{1,1,1,1,1,1,1,1,1,1,1,1,1},
		spell_null,             TAR_IGNORE,             POS_STAND,
		&gsn_bandage,     SLOT(0),        0,      0,
		'\0',   "",        "",    "", CABAL_ARMY,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"cabal recall",     { 10,10,10,10,10,10,10,10,10,10,10,10,10},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_cabal_recall,      SLOT( 0),   0,  24,
		'\0',   "", "","", CABAL_ARMY ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"wanted",               {  1,  1,  1,  1, 1, 1, 1, 1 ,1,1,1,1,1},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_DEAD,
		&gsn_wanted,                   SLOT(0),        0,       0,
		'\0',   "",                     "!Wanted!", "",     CABAL_RULER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"judge",               {  1,  1,  1,  1, 1, 1, 1, 1 ,1,1,1,1,1},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,             TAR_IGNORE,             POS_DEAD,
		&gsn_judge,              SLOT(0),        0,       0,
		'\0',   "",                     "!Judge!", "",      CABAL_RULER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"bloodthirst",         {  15, 15, 15, 15, 15, 15, 15, 15 ,15,15,15,15,15},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_SELF,       POS_FIGHT,
		&gsn_bloodthirst,       SLOT(0),        0,       12,
		'\0',   "", "La tua sete di sangue e' momentaneamente sopita..", "", CABAL_ARMY,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"spellbane",         { 10,10,10,10,10,10,10,10,10,10,10,10,10 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_SELF,       POS_STAND,
		&gsn_spellbane,       SLOT(0),        0,       12,
		'm',   "contrattacco", "", "",   CABAL_ARMY,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"resistance",         {  12, 12, 12,12,12,12,12,12 ,12,12,12,12,12},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_CHAR_SELF,       POS_FIGHT,
		&gsn_resistance,       SLOT(0),        0,       24,
		'\0',   "", "", "", CABAL_ARMY,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"deathblow",         {  20,20,20,20,20,20,20,20 ,20,20,20,20,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_STAND,
		&gsn_deathblow,       SLOT(0),        0,       0,
		'\0',   "", "!deathblow!", "",  CABAL_ARMY,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"transform",         {  10, 10, 10,10,10,10,10,10 ,10,10,10,10,10},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_transform,      TAR_CHAR_SELF,       POS_STAND,
		NULL,       SLOT(522),        100,       24,
		'\0',   "", "", "",   CABAL_YVALADRIN,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"mental knife",         {  15, 15, 15, 97,15,15,15,15,15,15,15,15,15 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mental_knife,      TAR_CHAR_OFFENSIVE,       POS_FIGHT,
		&gsn_mental_knife,       SLOT(524),        35,       12,
		'f',   "lama mentale", "La fitta nella tua testa svanisce.", "",CABAL_YVALADRIN,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"demon summon",         {  25, 25, 25, 97,25,25,25,25 ,25,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_demon_summon,      TAR_CHAR_SELF,       POS_FIGHT,
		&gsn_demon_summon,       SLOT(525),        100,       12,
		'\0',   "", "", "", CABAL_YVALADRIN,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"scourge",         {  30, 30, 30, 97,30,30,30,30,30,30,30,30,30 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_scourge,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(526),        50,       18,
		'f',   "maledizione del raggio viola", "!scourge!", "",CABAL_YVALADRIN,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"manacles",         {  20, 20, 20,20,20,20,20,20 ,20,20,20,20,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_manacles,      TAR_CHAR_OFFENSIVE,       POS_FIGHT,
		NULL,       SLOT(528),        75,       12,
		'\0',   "", "Le manette si dissolvono.", "",  CABAL_RULER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"shield of ruler",         {  10, 10, 10,10,10,10,10,10 ,10,10,10,10,10},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_shield_ruler,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(529),        100,       12,
		'\0',   "", "!shield!", "", CABAL_RULER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"guard",         {  22,22,22,22,22,22,22,22 ,22,22,22,22,22},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_STAND,
		&gsn_guard,       SLOT(0),        0,       12,
		'\0',   "", "", "", CABAL_KNIGHT,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"guard call",         {  15, 15, 15,15,15,15,15,15 ,15,15,15,15,15},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_guard_call,      TAR_IGNORE,       POS_FIGHT,
		&gsn_guard_call,       SLOT(530),        75,       12,
		'\0',   "",     "",    "", CABAL_RULER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"nightwalker",         {  25, 25, 25,25,25,25,25,25 ,25,25,25,25,25},
		{ 1,			  1,  1,  1, 1, 1, 1, 1, 1, 1,1,1,1},
		spell_nightwalker,      TAR_IGNORE,       POS_FIGHT,
		&gsn_nightwalker,       SLOT(531),        75,       12,
		'\0',   "", "", "",CABAL_INVADER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"eyes of intrigue",         {  13, 13, 13,13,13,13,13,13,13,13,13,13,13 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_eyes,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(532),        75,       12,
		'\0',   "", "!eyes of intrigue!", "",   CABAL_INVADER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"fade",         {  20, 20, 20,20,20,20,20,20 ,20,20,20,20,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_STAND,
		&gsn_fade,       SLOT(0),        0,       24,
		'\0',   "", "!fade!", "",   CABAL_INVADER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"shadow cloak",         {  10, 10, 10,10,10,10,10,10,10,10,10,10,10 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_shadow_cloak,      TAR_CHAR_DEFENSIVE,       POS_STAND,
		NULL,       SLOT(533),        10,       12,
		'\0',   "", "Le ombre smettono di proteggerti.", "",   CABAL_INVADER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"nightfall",         {  16, 16, 16,16,16,16,16,16,16,16,16,16,16 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_nightfall,      TAR_IGNORE,       POS_STAND,
		NULL,           SLOT(534),        50,       12,
		'\0',   "", "", "",  CABAL_INVADER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"aura of chaos",      {  16, 16, 16,16,16,16,16,16,16,16,16,16,16 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_aura_of_chaos,      TAR_CHAR_DEFENSIVE,       POS_STAND,
		NULL,           SLOT(720),        20,       12,
		'\0',   "", "La protezione della tua aura caotica svanisce.",     "",
		CABAL_CHAOS, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"garble",         {  10, 10, 10,10,10,10,10,10 ,10,10,10,10,10},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_garble,      TAR_CHAR_DEFENSIVE,       POS_FIGHT,
		&gsn_garble,       SLOT(535),        30,       12,
		'\0',   "", "Riacquisti padronanza sul tuo parlare.", "",    CABAL_CHAOS,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"mirror",         { 15, 15, 15,15,15,15,15,15,15,15,15,15,15 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_mirror,      TAR_CHAR_DEFENSIVE,       POS_STAND,
		&gsn_mirror,       SLOT(536),        40,       12,
		'\0',   "", "Svanisci.", "",   CABAL_CHAOS,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"confuse",         {  20, 20, 20,20,20,20,20,20 ,20,20,20,20,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_confuse,      TAR_CHAR_OFFENSIVE,       POS_FIGHT,
		&gsn_confuse,       SLOT(537),        20,       12,
		'\0',   "", "La confusione finalmente ti abbandona.", "",  CABAL_CHAOS,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"doppelganger",         {  25, 25, 25,25,25,25,25,25 ,25,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_doppelganger,      TAR_CHAR_DEFENSIVE,       POS_STAND,
		&gsn_doppelganger,       SLOT(527),        75,       12,
		'\0',   "", "Torni alle tue sembianze originali.", "",  CABAL_CHAOS,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"chaos blade",         {  23, 23, 23,23,23,23,23,23,23,23,23,23,23 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_chaos_blade,      TAR_IGNORE,       POS_STAND,
		&gsn_chaos_blade,       SLOT(550),        60,       12,
		'\0',   "", "!chaos blade!", "",    CABAL_CHAOS,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"stalker",         {  97, 97, 97, 97, 97, 97, 97,97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_stalker,      TAR_IGNORE,       POS_STAND,
		&gsn_stalker,       SLOT(554),        100,       12,
		'\0',   "", "", "",
		CABAL_RULER,    RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"randomizer",         {  28, 28, 28,28,28,28,28,28 ,28,28,28,28,28},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_randomizer,      TAR_IGNORE,       POS_STAND,
		&gsn_randomizer,       SLOT(555),        200,       24,
		'\0',   "", "", "", CABAL_CHAOS,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"tesseract",         {  29, 29, 29, 97,29,29,29,29 ,29,29,29,29,29},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_tesseract,      TAR_IGNORE,       POS_STAND,
		&gsn_tesseract,       SLOT(556),        150,       12,
		'\0',   "", "!tesseract!", "",  CABAL_YVALADRIN,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"trophy",         {  23, 23, 23,23,23,23,23,23 ,23,23,23,23,23},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_STAND,
		&gsn_trophy,       SLOT(0),        30,       12,
		'\0',   "", "", "",    CABAL_ARMY,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"truesight",         {  25, 25, 25,25,25,25,25,25 ,25,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_STAND,
		&gsn_truesight,       SLOT(0),        50,       12,
		'\0',   "", "", "",    CABAL_ARMY,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"brew",         {  23, 23, 23,23,23,23,23,23,23 ,23,23,23,23},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_brew,      TAR_OBJ_INV,       POS_STAND,
		&gsn_brew,       SLOT(557),        25,       12,
		'\0',   "", "", "",
		CABAL_YVALADRIN,  RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"shadowlife",         {  30, 30, 30,30,30,30,30,30 ,30,30,30,30,30},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_shadowlife,      TAR_CHAR_OFFENSIVE,       POS_STAND,
		&gsn_shadowlife,       SLOT(558),        80,       12,
		'\0',   "", "Senti le ombre posarsi sulle tue spalle.", "",  CABAL_INVADER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"ruler badge",         {  25, 25, 25,25,25,25,25,25 ,25,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_ruler_badge,      TAR_CHAR_SELF,   POS_STAND,
		&gsn_ruler_badge,       SLOT(560),        50,       12,
		'\0',   "", "!ruler badge!", "",    CABAL_RULER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"remove badge",         {  28, 28, 28,28,28,28,28,28,28,28,28,28,28 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_remove_badge,      TAR_CHAR_SELF,   POS_STAND,
		&gsn_remove_badge,       SLOT(561),        100,       12,
		'\0',   "", "!remove badge!", "",   CABAL_RULER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"golden aura",         {  10,10,10,10,10,10,10,10,10,10,10,10,10 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_golden_aura,      TAR_IGNORE,   POS_STAND,
		NULL,       SLOT(564),        25,       12,
		'\0',   "", "L'aura dorata svanisce.", "",  CABAL_KNIGHT,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"dragonplate",         {  15,15,15,15,15,15,15,15 ,15,15,15,15,15},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_dragonplate,      TAR_IGNORE,   POS_STAND,
		NULL,       SLOT(565),        60,       12,
		'\0',   "", "", "", CABAL_KNIGHT,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"squire",         {  25, 25, 25,25,25,25,25,25 ,25,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_squire,      TAR_IGNORE,   POS_STAND,
		&gsn_squire,       SLOT(566),        100,       12,
		'\0',   "", "", "",  CABAL_KNIGHT,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"dragonsword",         {  25,25,25,25,25,25,25,25 ,25,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_dragonsword,      TAR_IGNORE,   POS_STAND,
		&gsn_dragonsword,       SLOT(567),        70,       12,
		'\0',   "", "", "", CABAL_KNIGHT,   RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"holy armor",         {  10,10,10,10,10,10,10,10,10 ,10,10,10,10},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_holy_armor,      TAR_CHAR_SELF,   POS_REST,
		&gsn_knight,       SLOT(569),        20,       12,
		'\0',   "", "La protezione sacra si esaurisce.", "",    CABAL_KNIGHT,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},


	{
		"disperse",         {  20,20,20,20,20,20,20,20,20,20,20,20,20 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_disperse,      TAR_IGNORE,    POS_FIGHT,
		&gsn_disperse,       SLOT(573),        100,       24,
		'\0',   "", "Riacquisti il potere di dispersione.", "",CABAL_CHAOS,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"hunt",         {  13, 13, 13,13,13,13,13,13 ,13,13,13,13,13},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_STAND,
		&gsn_hunt,       SLOT(0),        0,       6,
		'\0',   "",     "!hunt!",   "", CABAL_HUNTER,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"find object",  {   25, 25, 25, 25,25,25,25,25,25 ,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_find_object,  TAR_IGNORE,     POS_STAND,
		NULL,           SLOT(585),  20, 18,
		'\0',   "",         "!Find Object!",    "", CABAL_HUNTER ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"path find",        {  13,13 ,13,13,13,13,13,13 ,13 ,13,13,13,13},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_SLEEP,
		&gsn_path_find,     SLOT( 0),   0,  0,
		'\0',   "",         "!endur!",  "", CABAL_HUNTER ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"riding",       {   30, 30, 30, 30,30,30,30,30,30 ,30,30,30,30},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_STAND,
		&gsn_riding,            SLOT(0),    0,  6,
		'\0',   "",         "!riding!", "", CABAL_KNIGHT ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"wolf",     {   20, 20,20, 20,20,20,20,20,20 ,20,20,20,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_wolf, TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(593),  100,    12,
		'\0',   "", "",  "", CABAL_HUNTER ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"wolf spirit",  {   28, 28, 28, 28,28,28,28,28,28 ,28,28,28,28},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_wolf_spirit,  TAR_CHAR_SELF,      POS_STAND,
		NULL,           SLOT(685),  50, 12,
		'\0',   "", "Il sangue del lupo ti abbandona.", "",
		CABAL_HUNTER , RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"armor use",        { 17,17,17,17,17,17,17,17,17 ,17,17,17,17},
		{ 2, 1, 1, 1,1,1,1,1,1,1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_armor_use, SLOT(0),    0,  0,
		'\0',   "",         "!Armor Use!",      "", CABAL_HUNTER ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"world find",   {  53,53 ,53,53,53,53,53,53 ,53 ,53,53,53,53},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,     TAR_IGNORE,     POS_SLEEP,
		&gsn_world_find,    SLOT( 0),   0,  0,
		'\0',   "",         "!world find!", "", CABAL_HUNTER ,
		RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"take revenge", {  53,53 ,53,53,53,53,53,53 ,53 ,53,53,53,53},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_take_revenge, TAR_IGNORE,     POS_STAND,
		NULL,       SLOT(624),  50,     12,
		'\0',   "", "!take revenge!",   "",
		CABAL_HUNTER , RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"mastering spell",  {37,37,37,37,37,37,37,37,37,37,37,37,37},
		{ 1,  1,  1,  1, 1, 1, 2, 2, 1, 1,1,1,1},
		spell_null,     TAR_IGNORE,     POS_FIGHT,
		&gsn_mastering_spell,   SLOT(0),    0,       0,
		'f',   "conoscenza arcana",      "!mastering spell!","",
		CABAL_YVALADRIN, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"guard dogs",         {  15, 15, 15,15,15,15,15,15 ,15,15,15,15,15},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_guard_dogs,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(687),        100,       12,
		'\0',   "",     "",     "",
		CABAL_LIONS, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"eyes of tiger",         {  25, 25, 25,25,25,25,25,25 ,25,25,25,25,25},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_eyes_of_tiger,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(688),        20,       12,
		'\0',   "",     "",     "",
		CABAL_LIONS, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"lion shield",         {  24, 24, 24,24,24,24,24,24 ,24,24,24,24,24},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_lion_shield,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(689),        200,       12,
		'\0',   "",     "",     "",
		CABAL_LIONS, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"evolve lion",         {  18, 18, 18,18,18,18,18,18 ,18,18,18,18,18},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_evolve_lion,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(690),        50,       12,
		'\0',   "",     "",     "",
		CABAL_LIONS, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"claw",         {  20, 20, 20,20,20,20,20,20 ,20,20,20,20,20},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_null,      TAR_IGNORE,       POS_FIGHT,
		&gsn_claw,       SLOT(0),        50,       24,
		'\0',   "claw",     "",     "",
		CABAL_LIONS, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},

	{
		"prevent",      {  35, 35, 35,35,35,35,35,35 ,35,35,35,35,35},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_prevent,      TAR_IGNORE,       POS_FIGHT,
		NULL,       SLOT(691),        75,       12,
		'\0',   "",     "",     "",
		CABAL_LIONS, RACE_NONE, ALIGN_NONE, GROUP_CABAL
	},


/*
 * Object spells
 */
	{
		"terangreal",       { 97,  97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_terangreal,   TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_terangreal,    SLOT(607),   5, 12,
		'm',   "terangreal",       "Ti svegli.", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"kassandra",        { 97,  97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_kassandra,    TAR_CHAR_SELF,      POS_STAND,
		&gsn_kassandra, SLOT(608),   5, 12,
		'\0',   "",     "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"sebat",        { 97,  97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_sebat,        TAR_CHAR_SELF,      POS_STAND,
		&gsn_sebat,     SLOT(609),   5, 12,
		'\0',   "",         "",  "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	},

	{
		"matandra",     { 97,  97, 97, 97, 97, 97, 97, 97 , 97, 97, 97, 97, 97},
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		spell_matandra, TAR_CHAR_OFFENSIVE, POS_STAND,
		&gsn_matandra,      SLOT(610),   5, 12,
		'f',   "parola sacra",    "", "", CABAL_NONE ,
		RACE_NONE, ALIGN_NONE, GROUP_NONE
	}
};
