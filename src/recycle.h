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


/*
 * Externs
 */
extern char str_empty[1];
extern int	mobile_count;


/*
 * Stuff for providing a crash-proof buffer
 */
#define MAX_BUF			16384
#define MAX_BUF_LIST	10
#define BASE_BUF		1024


/*
 * Valid states
 */
#define BUFFER_SAFE		0
#define BUFFER_OVERFLOW 1
#define BUFFER_FREED	2


/* Note recycling */
NOTE_DATA	*new_note	( void );
void		free_note	( NOTE_DATA *note );

/* Ban data recycling */
BAN_DATA	*new_ban	( void );
void		free_ban	( BAN_DATA *ban );

/* Descriptor recycling */
DESCRIPTOR_DATA *new_descriptor	( void );
void			free_descriptor	( DESCRIPTOR_DATA *d );

/* Extra descr recycling */
EXTRA_DESCR_DATA	*new_extra_descr	( void );
void				free_extra_descr	( EXTRA_DESCR_DATA *ed );

/* Affect recycling */
AFFECT_DATA	*new_affect	( void );
void		free_affect	( AFFECT_DATA *af );

/* Object recycling */
OBJ_DATA	*new_obj	( void );
void		free_obj	( OBJ_DATA *obj );

/* Character recyling */
CHAR_DATA	*new_char	( void );
void		free_char	( CHAR_DATA *ch );
PC_DATA		*new_pcdata ( void );
void		free_pcdata	( PC_DATA *pcdata );


/* Mob id */
long		get_pc_id		( void );
long		get_mob_id		( void );

/* Buffer procedures */
BUFFER	*new_buf		( void );
void	free_buf		( BUFFER *buffer );
bool	add_buf			( BUFFER *buffer, char *string );
void	clear_buf		( BUFFER *buffer );
char	*buf_string		( BUFFER *buffer );
