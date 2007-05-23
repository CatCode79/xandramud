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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
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

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */


#include <sys/time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>

#include "xandra.h"
#include "interp.h"
#include "recycle.h"
#include "file.h"


/*
 * Malloc debugging stuff.
 */
#if defined( MALLOC_DEBUG )
#	include <malloc.h>
extern int malloc_debug		( int );
extern int malloc_verify	( void );
#endif


/*
 * Socket and TCP/IP stuff.
 */
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include "telnet.h"

const unsigned char	echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const unsigned char	echo_on_str[]  = { IAC, WONT, TELOPT_ECHO, '\0' };
const unsigned char	go_ahead_str[] = { IAC, GA, '\0' };

#ifdef MXP
#	define	TELOPT_MXP	'\x5B'

const unsigned char will_mxp_str [] = { IAC, WILL, TELOPT_MXP, '\0' };
const unsigned char start_mxp_str[] = { IAC, SB, TELOPT_MXP, IAC, SE, '\0' };
const unsigned char do_mxp_str   [] = { IAC, DO, TELOPT_MXP, '\0' };
const unsigned char dont_mxp_str [] = { IAC, DONT, TELOPT_MXP, '\0' };
#endif


/*
 * Global variables.
 */
DESCRIPTOR_DATA				*descriptor_list;	/* All open descriptors */
DESCRIPTOR_DATA				*d_next;			/* Next descriptor in loop */
FILE						*fpReserve;			/* Reserved file handle */
bool						god;				/* All new chars are gods! */
bool		 				xandra_down;		/* Shutdown			*/
int							xandra_exit;		/* Exit Code */
bool						wizlock;			/* Game is wizlocked */
bool						newlock;			/* Game is newlocked */
time_t						boot_time;			/* time of boot */
time_t						limit_time;			/* time of limited saving calculation */
time_t						current_time;		/* time of this pulse */


/*
 * OS-dependent local functions.
 */
void	xandra_engine			( int control );
int		init_socket				( int port );
void	init_descriptor			( int control );
bool	read_from_descriptor	( DESCRIPTOR_DATA *d );
void	send_help_greeting		( DESCRIPTOR_DATA *d );
void	init_console_descriptor	( void );


/*
 * Other local functions (OS-independent).
 */
bool	check_reconnect			( DESCRIPTOR_DATA *d, bool fConn );
bool	check_playing			( DESCRIPTOR_DATA *d, char *name );
int		main					( int argc, char **argv );
void	nanny					( DESCRIPTOR_DATA *d, char *argument );
bool	process_output			( DESCRIPTOR_DATA *d, bool fPrompt );
void	read_from_buffer		( DESCRIPTOR_DATA *d );
void	stop_idling				( CHAR_DATA *ch );
void	bust_a_prompt			( CHAR_DATA *ch );
void	exit_function			( );
int		log_area_popularity		( void );
int		sex_ok					( void );
int		class_ok				( CHAR_DATA *ch, int class );


/*
 *
 */
int main( int argc, char **argv )
{
	struct timeval	now_time;
	int				port;
	int				control;
	char			buf[MSL];
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Don't leave save processes stranded */
	signal( SIGQUIT, exit_function );

	/*
	 * Memory debugging if needed.
	 */
#if defined( MALLOC_DEBUG )
	malloc_debug( 2 );
#endif

	/*
	 * Init time.
	 */
	gettimeofday( &now_time, NULL );
	limit_time = boot_time = current_time = ( time_t ) now_time.tv_sec;

	/*
	 * Reserve one channel for our use.
	 */
	if ( (fpReserve = fopen(NULL_FILE, "r")) == NULL )
	{
		perror( NULL_FILE );
		exit( 1 );
	}

	/*
	 * Get the port number.
	 */
    port	 = XANDRA_PORT;
	if ( argc > 1 )
	{
		port = atoi(argv[1]);

		if ( !is_number(argv[1]) )
		{
			fprintf( stderr, "Utilizzo: %s [porta #]\n", argv[0] );
			exit( 1 );
		}
		else if ( port  <= 1024 || port > 30000 )
		{
			fprintf( stderr, "La porta deve essere maggiore di 1024 o minore e uguale a 30000.\n" );
			exit( 1 );
		}
	}

	/*
	 * Run the game.
	 */

    /* Don't leave save processes stranded */
    signal( SIGQUIT, exit_function );

    if ( XANDRA_SINGLE_USER == TRUE )
    {
		control = 0;
		sprintf( buf, "Anatolia has launched in single user mode." );
	}
	else
	{
		control = init_socket( port );
		sprintf( log_buf, "Xandra e' stato lanciato alla porta %d.", port );
	}

	boot_db();
	log_string( log_buf );
	xandra_engine( control );

    /*
     * This will close the single user descriptor as well
     */
	close( control );

	log_area_popularity();

	/* That's all, folks. */
	log_string( "Normal termination of game." );

	exit( xandra_exit );
	return( xandra_exit );
}


/*
 *
 */
void shutdown_xandra( int sig )
{
	log_string( "Received signal SIG_TERM." );
	reboot_xandra( FALSE, XANDRA_SHUTDOWN );
}


/*
 *
 */
void crash_chronos( int sig )
{
	char			buf[MSL];
	DESCRIPTOR_DATA *d, *dnext;
	CHAR_DATA		*ch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	log_string( "Core dumped." );
	sprintf( buf, "The core with signal %d", sig );
	bug( buf, 0 );
	for ( d = descriptor_list; d != NULL; d = dnext )
	{
		dnext = d->next;
		ch = d->original ? d->original : d->character;
		if ( IS_MOB(ch) )
			continue;
		save_char_obj( ch );
		sprintf( buf, "%s is saved", ch->name );
		log_string( buf );
		write_to_descriptor( d->descriptor, "\007Reboot del Server!!\007\n\r", 0 );
		write_to_descriptor( d->descriptor, "Salvato.\n\r", 0 );
		sprintf( buf, "%s ultimo comando %s", ch->name, ch->desc->inlast );
		bug( buf, 0 );
	}

	sprintf( buf, "SUCCESSFUL HANDLING!" );
	bug( buf, 0 );
}


#ifdef MXP
/*
 * Set up MXP
 */
void turn_on_mxp( DESCRIPTOR_DATA *d )
{
	d->mxp = TRUE;  /* turn it on now */
	write_to_buffer( d, (char*) start_mxp_str, 0 );
	write_to_buffer( d, MXPMODE (6), 0 );   /* permanent secure mode */
/*	write_to_buffer( d, MXPTAG("!-- Set up MXP elements --"), 0 ); */

	/* Exit tag */
	write_to_buffer( d, MXPTAG("!EL Ex '<send>' FLAG=RoomExit"), 0 );

	/* Tag per la descrizione della stanza. */
	write_to_buffer( d, MXPTAG("!EL rdesc '<p>' FLAG=RoomDesc"), 0 );

	mxp_toggle_language( d );
} /* end of turn_on_mxp */


void mxp_toggle_language( DESCRIPTOR_DATA *d )
{
	/* Puo' capitare quando si sconnette prima di aver creato un pg o entrato in gioco */
	if ( !d->character )
		return;

	if ( IS_MOB(d->character) )
		return;

	/* Get tag, per oggetti a terra */
	if ( !IS_SET(d->character->act, PLR_ENGLISH) )
	{
		write_to_buffer( d, MXPTAG("!EL Get \"<send href='"
			"prendi &#39;&name;&#39;|"
			"esamina &#39;&name;&#39;|"
			"bevi &#39;&name;&#39;' "
			"hint='Click sinistro per raccogliere questo oggetto|"
			"Prendi &desc;|"
			"Esamina &desc;|"
			"Bevi da &desc;"
			"'>\" ATT='name desc'"), 0 );
	}
	else
	{
		write_to_buffer( d, MXPTAG("!EL Get '<send href=\""
			"get &#39;&name;&#39;|"
			"examine &#39;&name;&#39;|"
			"drink &#39;&name;&#39;\" "
			"hint=\"Click sinistro per raccogliere questo oggetto|"
			"Prendi &desc;|"
			"Esamina &desc;|"
			"Bevi da &desc;"
			"\">' ATT='name desc'"), 0 );
 	}

	/* Drop tag, per gli oggetti in inventario */
	if ( !IS_SET(d->character->act, PLR_ENGLISH) )
	{
		write_to_buffer( d, MXPTAG("!EL Drop \"<send href='"
			"abbandona &#39;&name;&#39;|"
			"esamina &#39;&name;&#39;|"
			"guarda in &#39;&name;&#39;|"
			"indossa &#39;&name;&#39;|"
			"mangia &#39;&name;&#39;|"
			"bevi &#39;&name;&#39;' "
			"hint='Click sinistro per abbandonare questo oggetto|"
			"Abbandona &desc;|"
			"Esamina &desc;|"
			"Guarda dentro &desc;|"
			"Indossa &desc;|"
			"Mangia &desc;|"
			"Bevi &desc;"
			"'>\" ATT='name desc'"), 0 );
	}
	else
	{
		write_to_buffer( d, MXPTAG("!EL Drop '<send href=\""
			"drop &#39;&name;&#39;|"
			"examine &#39;&name;&#39;|"
			"look in &#39;&name;&#39;|"
			"wear &#39;&name;&#39;|"
			"eat &#39;&name;&#39;|"
			"drink &#39;&name;&#39;\" "
			"hint=\"Click sinistro per abbandonare questo oggetto|"
			"Abbandona &desc;|"
			"Esamina &desc;|"
			"Guarda dentro &desc;|"
			"Indossa &desc;|"
			"Mangia &desc;|"
			"Bevi &desc;"
			"\">' ATT='name desc'"), 0 );
	}

	/* Lst tag, per le liste di oggetti nei negozi */
	if ( !IS_SET(d->character->act, PLR_ENGLISH) )
	{
		write_to_buffer( d, MXPTAG("!EL Lst \"<send href='"
			"compra &#39;&name;&#39;' "
			"hint='Compra &desc;"
			"'>\" ATT='name desc'"), 0 );
	}
	else
	{
		write_to_buffer( d, MXPTAG("!EL Lst '<send href=\""
			"buy &#39;&name;&#39;\" "
			"hint=\"Compra &desc;"
			"\">' ATT='name desc'"), 0 );
	}

	/* Pg tag, per il comando who e tell */
	if ( !IS_SET(d->character->act, PLR_ENGLISH) )
	{
		write_to_buffer( d, MXPTAG("!EL Pg \"<send href='"
			"comunica &#39;&name;&#39;' "
			"hint='Manda un messaggio a &name;'"
			"prompt>\" ATT='name'"), 0 );
	}
	else
	{
		write_to_buffer( d, MXPTAG("!EL Pg '<send href=\""
			"tell &#39;&name;&#39;\" "
			"hint=\"Manda un messaggio a &name;\""
			"prompt>' ATT='name'"), 0 );
	}
}
#endif


/*
 *
 */
int init_socket( int port )
{
	static struct sockaddr_in	sa_zero;
	struct sockaddr_in			sa;
	int							x = 1;
	int							fd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror( "Init_socket: socket" );
		exit( 1 );
	}

	if ( setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x)) < 0 )
	{
		perror( "Init_socket: SO_REUSEADDR" );
		close( fd );
		exit( 1 );
	}

#if defined( SO_DONTLINGER )
	{
		struct linger	ld;
		/*~~~~~~~~~~~~~~~*/

		ld.l_onoff = 1;
		ld.l_linger = 1000;

		if ( setsockopt(fd, SOL_SOCKET, SO_DONTLINGER, (char *) &ld, sizeof(ld)) < 0 )
		{
			perror( "Init_socket: SO_DONTLINGER" );
			close( fd );
			exit( 1 );
		}
	}
#endif
	sa = sa_zero;
	sa.sin_family = AF_INET;
	sa.sin_port = htons( port );

	if ( bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0 )
	{
		perror( "Init socket: bind" );
		close( fd );
		exit( 1 );
	}

	if ( listen(fd, 3) < 0 )
	{
		perror( "Init socket: listen" );
		close( fd );
		exit( 1 );
	}

	return( fd );
}



/*
 *
 */
void send_help_greeting( DESCRIPTOR_DATA *d )
{
	extern char * help_greeting;

	write_to_buffer( d, "\033[2J\033[0;0H\033[0;37;40m\n\r", 0 );
	if ( help_greeting[0] == '.' )
		write_to_buffer( d, help_greeting+1, 0 );
	else
		write_to_buffer( d, help_greeting  , 0 );
}


/*
 *
 */
void xandra_engine( int control )
{
	static struct timeval	null_time;
	struct timeval			last_time;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* signal( SIGSEGV, crash_chronos ); */		/* Decommentare se non si vuole il file core */
	signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, shutdown_xandra );
    signal( SIGKILL, shutdown_xandra );

	gettimeofday( &last_time, NULL );
	current_time = ( time_t ) last_time.tv_sec;

	if ( XANDRA_SINGLE_USER == TRUE )
		init_console_descriptor( );

	/* Main loop */
	while ( !xandra_down )
	{
		fd_set			in_set;
		fd_set			out_set;
		fd_set			exc_set;
		DESCRIPTOR_DATA *d;
		int				maxdesc;
		/*~~~~~~~~~~~~~~~~~~~~*/

#if defined( MALLOC_DEBUG )
		if ( malloc_verify() != 1 )
			abort();
#endif

		/* Poll all active descriptors. */
		FD_ZERO( &in_set );
		FD_ZERO( &out_set );
		FD_ZERO( &exc_set );

		/* Single user mode uses a descriptor */
		if ( XANDRA_SINGLE_USER == FALSE )
		{
			FD_SET( control, &in_set );
			maxdesc = control;
		}
		else
		{
			maxdesc = 4;

			/* If our descriptor is disconnected, create a new one */
			if ( !descriptor_list )
				init_console_descriptor( );
		}

		for ( d = descriptor_list; d; d = d->next )
		{
			maxdesc = UMAX( maxdesc, d->descriptor );
			FD_SET( d->descriptor, &in_set );
			FD_SET( d->descriptor, &out_set );
			FD_SET( d->descriptor, &exc_set );
		}

		if ( select(maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0 )
		{
			perror( "Game_loop: select: poll" );
			exit( 1 );
		}

		/*
		 * For MultiUser mode: New connection?
		 */
		if ( XANDRA_SINGLE_USER == FALSE )
		{
			if ( FD_ISSET(control, &in_set) )
				init_descriptor( control );
		}

		/* Kick out the freaky folks. */
		for ( d = descriptor_list; d != NULL; d = d_next )
		{
			d_next = d->next;
			if ( FD_ISSET(d->descriptor, &exc_set) )
			{
				FD_CLR( d->descriptor, &in_set );
				FD_CLR( d->descriptor, &out_set );
				if ( d->character && d->character->level > 1 )
					save_char_obj( d->character );
				d->outtop = 0;
				close_socket( d );
			}
		}

		/* Process input. */
		for ( d = descriptor_list; d != NULL; d = d_next )
		{
			d_next = d->next;
			d->fcommand = FALSE;

			if ( FD_ISSET(d->descriptor, &in_set) )
			{
				if ( d->character != NULL )
					d->character->timer = 0;
				if ( !read_from_descriptor(d) )
				{
					FD_CLR( d->descriptor, &out_set );
					if ( d->character != NULL && d->character->level > 1 )
						save_char_obj( d->character );
					d->outtop = 0;
					close_socket( d );
					continue;
				}
			}

			if ( d->character != NULL && d->character->daze > 0 )
				--d->character->daze;

			if ( d->character != NULL && d->character->wait > 0 )
			{
				--d->character->wait;
				continue;
			}

			read_from_buffer( d );
			if ( d->incomm[0] != '\0' )
			{
				d->fcommand = TRUE;
				stop_idling( d->character );

				if ( d->showstr_point )
					show_string( d, d->incomm );
				else if ( d->connected == CON_PLAYING )
				{
					if ( d->character && IS_SET(d->character->act, PLR_AFK) )
					{
						REMOVE_BIT( d->character->act, PLR_AFK );
						send_to_char( "Ritorni alla tastiera.\n\r", d->character );
					}
				    substitute_alias( d, d->incomm );
				}
				else
					nanny( d, d->incomm );

				d->incomm[0] = '\0';
			}
		}

		/* Autonomous game motion. */
		update_handler();

		/* Output. */
		for ( d = descriptor_list; d != NULL; d = d_next )
		{
			d_next = d->next;

			if ( (d->fcommand || d->outtop > 0) && FD_ISSET(d->descriptor, &out_set) )
			{
				if ( !process_output(d, TRUE) )
				{
					if ( d->character != NULL && d->character->level > 1 )
						save_char_obj( d->character );
					d->outtop = 0;
					close_socket( d );
				}
			}
		}

		/* Synchronize to a clock. Sleep( last_time + 1/PULSE_PER_SCD - now ).
		 *	Careful here of signed versus unsigned arithmetic. */
		{
			struct timeval	now_time;
			long			secDelta;
			long			usecDelta;
			/*~~~~~~~~~~~~~~~~~~~~~~*/

			gettimeofday( &now_time, NULL );
			usecDelta = ( (int) last_time.tv_usec ) - ( (int) now_time.tv_usec ) + 1000000 / PULSE_PER_SCD;
			secDelta = ( (int) last_time.tv_sec ) - ( (int) now_time.tv_sec );
			while ( usecDelta < 0 )
			{
				usecDelta += 1000000;
				secDelta -= 1;
			}

			while ( usecDelta >= 1000000 )
			{
				usecDelta -= 1000000;
				secDelta += 1;
			}

			if ( secDelta > 0 || (secDelta == 0 && usecDelta > 0) )
			{
				struct timeval	stall_time;
				/*~~~~~~~~~~~~~~~~~~~~~~~*/

				stall_time.tv_usec = usecDelta;
				stall_time.tv_sec = secDelta;
				if ( select(0, NULL, NULL, NULL, &stall_time) < 0 )
				{
					perror( "Game_loop: select: stall" );
					exit( 1 );
				}
			}
		}

		gettimeofday( &last_time, NULL );
		current_time = ( time_t ) last_time.tv_sec;
	}
}


/*
 *
 */
void init_descriptor( int control )
{

	char				buf[MSL];
	DESCRIPTOR_DATA		*dnew;
	struct sockaddr_in	sock;
	struct hostent		*from;
	int					desc;
	socklen_t			size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	size = sizeof( sock );
	getsockname( control, (struct sockaddr *) &sock, &size );
	if ( (desc = accept(control, (struct sockaddr *) &sock, &size)) < 0 )
	{
		perror( "New_descriptor: accept" );
		return;
	}

#if !defined( FNDELAY )
#	define FNDELAY O_NDELAY
#endif
	if ( fcntl(desc, F_SETFL, FNDELAY) == -1 )
	{
		perror( "New_descriptor: fcntl: FNDELAY" );
		return;
	}

	/* Cons a new descriptor. */
	dnew = new_descriptor();

	dnew->descriptor = desc;
	dnew->connected = CON_GET_NAME;
	dnew->showstr_head = NULL;
	dnew->showstr_point = NULL;
	dnew->outsize = 2000;
	dnew->outbuf = alloc_mem( dnew->outsize );
#ifdef MXP
	dnew->mxp = FALSE;   /* NJG - initially MXP is off */
#endif

	size = sizeof( sock );
	if ( getpeername(desc, (struct sockaddr *) &sock, &size) < 0 )
	{
		perror( "New_descriptor: getpeername" );
		dnew->host = str_dup( "(unknown)" );
	}
	else
	{
		/* Would be nice to use inet_ntoa here but it takes a struct arg,
		 *	which ain't very compatible between gcc and system libraries. */
		int addr;
		/*~~~~~*/

		addr = ntohl( sock.sin_addr.s_addr );
		sprintf( buf, "%d.%d.%d.%d", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF );
		sprintf( log_buf, "Sock.sinaddr:  %s", buf );
		log_string( log_buf );
		if ( XANDRA_DNS_RESOLVE == TRUE )
			from = gethostbyaddr( (char *) &sock.sin_addr, sizeof(sock.sin_addr), AF_INET );
		else
			from = NULL;
		dnew->host = str_dup( from ? from->h_name : buf );
	}

	/* Swiftest: I added the following to ban sites. I don't endorse banning of sites,
	 *	but Copper has few descriptors now and some people from certain sites keep abusing
	 *	access by using automated 'autodialers' and leaving connections hanging.
	 * Furey: added suffix check by request of Nickel of HiddenWorlds. */
	if ( check_ban(dnew->host, BAN_ALL) )
	{
		write_to_descriptor( desc, "Il tuo indirizzo e' stato bannato da questo Mud.\n\r", 0 );
		close( desc );
		free_descriptor( dnew );
		return;
	}

	/* Init descriptor data. */
	dnew->next = descriptor_list;
	descriptor_list = dnew;

	/* Send the greeting. */
	send_help_greeting( dnew );
}


/*
 *
 */
void init_console_descriptor( )
{
	DESCRIPTOR_DATA *dnew;

	/* New_descriptor analogue. */
	dnew = new_descriptor( );

	dnew->descriptor	= 0;
	dnew->connected		= CON_GET_NAME;
	dnew->host			= str_dup( "localhost" );
	dnew->outsize		= 2000;
	dnew->outbuf		= alloc_mem( dnew->outsize );
	dnew->showstr_head	= NULL;
	dnew->showstr_point	= NULL;

	dnew->next			= descriptor_list;
	descriptor_list		= dnew;

	/* Send the greeting */
	send_help_greeting( dnew );
}


/*
 *
 */
void close_socket( DESCRIPTOR_DATA *dclose )
{
	CHAR_DATA	*ch;
	/*~~~~~~~~~~~~*/

	if ( dclose->outtop > 0 )
		process_output( dclose, FALSE );

	if ( dclose->snoop_by != NULL )
		write_to_buffer( dclose->snoop_by, "La tua vittima e' uscita dal gioco.\n\r", 0 );

	{
		DESCRIPTOR_DATA *d;
		/*~~~~~~~~~~~~~~~*/

		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->snoop_by == dclose )
				d->snoop_by = NULL;
		}
	}

	if ( (ch = dclose->character) != NULL )
	{
		sprintf( log_buf, "Chiusura del collegamento a %s.", ch->name );
		log_string( log_buf );

		if ( ch->pet && (ch->pet->in_room == NULL || ch->pet->in_room == get_room_index(ROOM_VNUM_LIMBO)) )
		{
			char_to_room( ch->pet, get_room_index(ROOM_VNUM_LIMBO) );
			extract_char( ch->pet, TRUE );
		}

		if ( dclose->connected == CON_PLAYING )
		{
			if ( !IS_ADMIN(ch) )
				act( "$n ha perso il collegamento.", ch, NULL, NULL, TO_ROOM, POS_REST );

			wiznet( "Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0 );
			ch->desc = NULL;
		}
		else
		{
			free_char( dclose->character );
		}
	}

	if ( d_next == dclose )
		d_next = d_next->next;

	if ( dclose == descriptor_list )
	{
		descriptor_list = descriptor_list->next;
	}
	else
	{
		DESCRIPTOR_DATA *d;
		/*~~~~~~~~~~~~~~~*/

		for ( d = descriptor_list; d && d->next != dclose; d = d->next )
			;
		if ( d != NULL )
			d->next = dclose->next;
		else
			bug( "Close_socket: dclose not found.", 0 );
	}

    /* Do NOT close the console descriptor: 0 */
    if ( dclose->descriptor )
		close( dclose->descriptor );

	free_descriptor( dclose );
}


/*
 *
 */
bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
	int iStart;
	/*~~~~~~~*/

	/* Hold horses if pending command already. */
	if ( d->incomm[0] != '\0' )
		return( TRUE );

	/* Check for overflow. */
	iStart = strlen( d->inbuf );
	if ( iStart >= sizeof(d->inbuf) - 10 )
	{
		sprintf( log_buf, "%s input overflow!", d->host );
		log_string( log_buf );
		write_to_descriptor( d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
		return( FALSE );
	}

	/* Snarf input. */
	for ( ;; )
	{
		int nRead;
		/*~~~~~~*/

		nRead = read( d->descriptor, d->inbuf + iStart, sizeof(d->inbuf) - 10 - iStart );
		if ( nRead > 0 )
		{
			iStart += nRead;
			if ( d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r' )
				break;
		}
		else if ( nRead == 0 )
		{
			log_string( "EOF encountered on read." );
			return( FALSE );
		}
		else if ( errno == EWOULDBLOCK )
			break;
		else
		{
			perror( "Read_from_descriptor" );
			return( FALSE );
		}
	}

	d->inbuf[iStart] = '\0';
	return( TRUE );
}


/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
	int				i,
					j,
					k;
	char			buf[MSL];
#ifdef MXP
	unsigned char	*p;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Hold horses if pending command already. */
	if ( d->incomm[0] != '\0' )
		return;

#if MXP
	/* Look for incoming telnet negotiation */
	for ( p = (unsigned char*) d->inbuf; *p; p++ )
	{
		if ( *p == IAC )
		{
			if ( memcmp (p, do_mxp_str, strlen((char*)do_mxp_str)) == 0 )
			{
				turn_on_mxp( d );
				/* remove string from input buffer */
				memmove( p, &p [strlen((char*)do_mxp_str)], strlen((char*)&p [strlen((char*)do_mxp_str)]) + 1 );
				p--; /* adjust to allow for discarded bytes */
			} /* end of turning on MXP */
			else  if (memcmp (p, dont_mxp_str, strlen((char*)dont_mxp_str)) == 0)
			{
				d->mxp = FALSE;
				/* remove string from input buffer */
				memmove( p, &p [strlen((char*)dont_mxp_str)], strlen((char*)&p [strlen((char*)dont_mxp_str)]) + 1 );
				p--; /* adjust to allow for discarded bytes */
			} /* end of turning off MXP */
		} /* end of finding an IAC */
	}
#endif

	/* Look for at least one new line. */
	for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
	{
		if ( d->inbuf[i] == '\0' )
			return;
	}

	/* Canonical input processing. */
	for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
	{
		if ( k >= MIL - 2 )
		{
			write_to_descriptor( d->descriptor, "Linea troppo lunga.\n\r", 0 );

			/* skip the rest of the line */
			for ( ; d->inbuf[i] != '\0'; i++ )
			{
				if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
					break;
			}

			d->inbuf[i] = '\n';
			d->inbuf[i + 1] = '\0';
			break;
		}

		if ( d->inbuf[i] == '\b' && k > 0 )
			--k;
		else if ( isprint(d->inbuf[i]) )
			d->incomm[k++] = d->inbuf[i];
	}

	/* Finish off the line. */
	if ( k == 0 )
		d->incomm[k++] = ' ';
	d->incomm[k] = '\0';

	/* Deal with bozos with #repeat 1000 ... */
	if ( k > 1 || d->incomm[0] == '!' )
	{
		if ( d->incomm[0] != '!' && strcmp(d->incomm, d->inlast) )
		{
			d->repeat = 0;
		}
		else
		{
			if ( ++d->repeat >= 25 )	/* corrected by chronos */
			{
				sprintf( log_buf, "%s spam di input!", d->host );
				log_string( log_buf );
				if ( d->character != NULL )
				{
					sprintf( buf, "SPAM! %s spamma, lo si butta fuori.", d->character->name );
					wiznet( buf, d->character, NULL, WIZ_SPAM, get_trust(d->character) );

					sprintf( buf, "[%s]'s  Inlast:[%s] Incomm:[%s]!", d->character->name, d->inlast, d->incomm );
					wiznet( buf, d->character, NULL, WIZ_SPAM, get_trust(d->character) );

					d->repeat = 0;

					write_to_descriptor( d->descriptor, "\n\r*** DIAMOCI UN TAGLIO!!! ***\n\r", 0 );

					/* strcpy( d->incomm, "quit" ); */
					close_socket( d );
					return;
				}
			}
		}
	}

	/* Do '!' substitution. */
	if ( d->incomm[0] == '!' )
		strcpy( d->incomm, d->inlast );
	else
		strcpy( d->inlast, d->incomm );

	/* Shift the input buffer. */
	while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		i++;
	for ( j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++ )
		;
}


/* Low level output function. */

/*
 * Some specials added by KIO
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
	/* Bust a prompt. */
	if ( !xandra_down && d->showstr_point )
	{
		write_to_buffer( d, "\r[Premi Invio per continuare]\n\r", 0 );
	}
	else if ( fPrompt && !xandra_down && d->connected == CON_PLAYING )
	{
		CHAR_DATA	*ch;
		CHAR_DATA	*victim;
		/*~~~~~~~~~~~~~~~~*/


		ch = d->character;

		/* battle prompt */
		if ( (victim = ch->fighting) != NULL && can_see(ch, victim) )
		{
			char	buf[MSL];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			sprintf( buf, "%s %s\n\r", IS_MOB(victim) ? victim->short_descr : victim->name, get_alias_wound(victim) );
			buf[0] = UPPER( buf[0] );
			write_to_buffer( d, buf, 0 );
		}

		ch = d->original ? d->original : d->character;
		if ( !IS_SET(ch->comm, COMM_COMPACT) )
			write_to_buffer( d, "\n\r", 2 );

		if ( IS_SET(ch->comm, COMM_PROMPT) )
			bust_a_prompt( d->character );

		if ( IS_SET(ch->comm, COMM_TELNET_GA) )
			write_to_buffer( d, (char*) go_ahead_str, 0 );
	}

	/* Short-circuit if nothing to write. */
	if ( d->outtop == 0 )
		return( TRUE );

	/* Snoop-o-rama. */
	if ( d->snoop_by != NULL )
	{
		if ( d->character != NULL )
			write_to_buffer( d->snoop_by, d->character->name, 0 );
		write_to_buffer( d->snoop_by, "> ", 2 );
		write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
	}

	/* OS-dependent output. */
	if ( !write_to_descriptor(d->descriptor, d->outbuf, d->outtop) )
	{
		d->outtop = 0;
		return( FALSE );
	}
	else
	{
		d->outtop = 0;
		return( TRUE );
	}
}


/*
 * Bust a prompt (player settable prompt) coded by Morgenes for Aldara Mud bust
 */
void bust_a_prompt( CHAR_DATA *ch )
{
	char		buf[MSL];
	char		buf2[MSL];
	const char	*str;
	const char	*i;
	char		*point;
	char		doors[MIL];
	CHAR_DATA	*victim;
	EXIT_DATA	*pexit;
	bool		found;
	const char	*dir_name[] = { "N", "E", "S", "O", "A", "B", "NE", "SE", "SO", "NO" };
	int			door;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef MXP
	/* reset MXP to default operation */
	if ( ch->desc->mxp )
		write_to_buffer( ch->desc, ESC "[3z", 0 );
#endif

	point = buf;
	str = ch->prompt;
	if ( str == NULL || str[0] == '\0' )
	{
		sprintf( buf, "<%d vita %d mov %d mana> %s", ch->point[LIFE], ch->point[MOVE], ch->point[MANA], ch->prefix );
		send_to_char( buf, ch );
		return;
	}

	while ( *str != '\0' )
	{
		if ( *str != '%' )
		{
			*point++ = *str++;
			continue;
		}

		++str;
		switch ( *str )
		{
		default:
			i = " ";
			break;

		case 'e':
			found = FALSE;
			doors[0] = '\0';
			for ( door = 0; door < MAX_DIR; door++ )
			{
				if ((pexit = ch->in_room->exit[door]) != NULL
				&&	pexit->to_room != NULL
				&&	(can_see_room(ch, pexit->to_room) || (IS_AFFECTED(ch, AFF_INFRARED) && !IS_AFFECTED(ch, AFF_BLIND)))
				&&	!IS_SET(pexit->exit_info, EX_CLOSED) )
				{
					found = TRUE;
					strcat( doors, dir_name[door] );
				}
			}

			if ( !found )
				strcat( buf, "none" );
			sprintf( buf2, "%s", doors );
			i = buf2;
			break;

		case 'c':
			sprintf( buf2, "%s", "\n\r" );
			i = buf2;
			break;

		/* added from here by KIO * */
		case 'n':
			sprintf( buf2, "%s", ch->name );
			i = buf2;
			break;

		case 'S':
			sprintf( buf2, "%s", (ch->sex == SEX_MALE ? "Maschio" : (!ch->sex ? "Neutro" : "Femmina")) );
			i = buf2;
			break;

		case 'y':
			if ( ch->point[LIFE] >= 0 )
				sprintf( buf2, "%d%%", ((100 * ch->point[LIFE]) / UMAX(1, ch->max_point[LIFE])) );
			else
				sprintf( buf2, "MALE!!" );
			i = buf2;
			break;

		case 'o':
			if ( (victim = ch->fighting) != NULL )
			{
				if ( victim->point[LIFE] >= 0 )
					sprintf( buf2, "%d%%", ((100 * victim->point[LIFE]) / UMAX(1, victim->max_point[LIFE])) );
				else
					sprintf( buf2, "MALE!!" );
			}
			else
				sprintf( buf2, "nessuno" );
			i = buf2;
			break;

		/* Finished */
		/* Thanx to zihni: T for time */
		case 'T':
			sprintf( buf2, "%d %s", (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12, time_info.hour >= 12 ? "pm" : "am" );
			i = buf2;
			break;

		case 'h':
			sprintf( buf2, "%s%d%s", CLR(ch, RED_BOLD), ch->point[LIFE], CLR(ch, WHITE_BOLD) );
			i = buf2;
			break;

		case 'H':
			sprintf( buf2, "%s%d%s", CLR(ch, RED), ch->max_point[LIFE], CLR(ch, WHITE_BOLD) );
			i = buf2;
			break;

		case 'I':
			if ( IS_ADMIN(ch) && ch->in_room != NULL )
				sprintf( buf2, "%d", ch->invis_level );
			else
				sprintf( buf2, " " );
			i = buf2;
			break;

		case 'm':
			sprintf( buf2, "%s%d%s", CLR(ch, CYAN_BOLD), ch->point[MANA], CLR(ch, WHITE_BOLD) );
			i = buf2;
			break;

		case 'M':
			sprintf( buf2, "%s%d%s", CLR(ch, CYAN), ch->max_point[MANA], CLR(ch, WHITE_BOLD) );
			i = buf2;
			break;

		case 'v':
			sprintf( buf2, "%s%d%s", CLR(ch, GREEN_BOLD), ch->point[MOVE], CLR(ch, WHITE_BOLD) );
			i = buf2;
			break;

		case 'V':
			sprintf( buf2, "%s%d%s", CLR(ch, GREEN), ch->max_point[MOVE], CLR(ch, WHITE_BOLD) );
			i = buf2;
			break;

		case 'x':
			if ( IS_ADMIN(ch) )
				sprintf( buf2, "%d", ch->exp );
			else
				sprintf( buf2, " " );
			i = buf2;
			break;

		case 'X':
			if ( IS_ADMIN(ch) )
				sprintf( buf2, "%d", IS_MOB(ch) ? 0 : exp_to_level(ch, ch->pcdata->points) );
			else
				sprintf( buf2, " " );
			i = buf2;
			break;

		case 'g':
			sprintf( buf2, "%ld", ch->gold );
			i = buf2;
			break;

		case 's':
			sprintf( buf2, "%ld", ch->silver );
			i = buf2;
			break;

		case 'a':
			sprintf( buf2, "%s", IS_GOOD(ch) ? "buono" : IS_EVIL(ch) ? "malvagio" : "neutrale" );
			i = buf2;
			break;

		case 'r':
			if ( ch->in_room != NULL )
			{
				sprintf( buf2, "%s",
					((!IS_MOB(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
					|| (!IS_AFFECTED(ch, AFF_BLIND) && !room_is_dark(ch)) ) ?
						ch->in_room->name : "oscurita'" );
			}
			else
				sprintf( buf2, " " );
			i = buf2;
			break;

		case 'R':
			if ( IS_ADMIN(ch) && ch->in_room != NULL )
				sprintf( buf2, "%d", ch->in_room->vnum );
			else
				sprintf( buf2, " " );
			i = buf2;
			break;

		case 'z':
			if ( IS_ADMIN(ch) && ch->in_room != NULL )
				sprintf( buf2, "%s", ch->in_room->area->name );
			else
				sprintf( buf2, " " );
			i = buf2;
			break;

		case '%':
			sprintf( buf2, "%%" );
			i = buf2;
			break;
		}

		++str;

		while ( (*point = *i) != '\0' )
			++point, ++i;
	}

	write_to_buffer( ch->desc, buf, point - buf );

	if ( ch->prefix[0] != '\0' )
		write_to_buffer( ch->desc, ch->prefix, 0 );
}


#ifdef MXP
/*
 * Count number of mxp tags need converting
 *    ie. < becomes &lt;
 *        > becomes &gt;
 *        & becomes &amp;
 */
int count_mxp_tags( const int bMXP, const char *txt, int length )
{
	char		c;
	const char	*p;
	int			count;
	int			bInTag = FALSE;
	int			bInEntity = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for ( p = txt, count = 0; length > 0; p++, length-- )
	{
		c = *p;

		if ( bInTag )	/* in a tag, eg. <send> */
		{
			if ( !bMXP )
				count--;     /* not output if not MXP */
#ifdef MXP_ZMUD
			else
				count += 4;  /* allow for ESC [1z */
#endif

			if ( c == MXP_ENDc )
				bInTag = FALSE;
		} /* end of being inside a tag */
		else if (bInEntity)  /* in a tag, eg. <send> */
		{
			if ( !bMXP )
				count--;     /* not output if not MXP */
			if ( c == ';' )
				bInEntity = FALSE;
		} /* end of being inside a tag */
		else
		{
			switch ( c )
			{
			case MXP_BEGc:
				bInTag = TRUE;
				if ( !bMXP )
					count--;	/* not output if not MXP */
				break;

			case MXP_ENDc:		/* shouldn't get this case */
				if ( !bMXP )
					count--;	/* not output if not MXP */
				break;

			case MXP_AMPc:
				bInEntity = TRUE;
				if ( !bMXP )
					count--;	/* not output if not MXP */
				break;

			default:
				if ( bMXP )
				{
					switch ( c )
					{
					case '<':       /* < becomes &lt; */
					case '>':       /* > becomes &gt; */
						count += 3;
						break;

					case '&':
						count += 4;    /* & becomes &amp; */
						break;

					case '"':        /* " becomes &quot; */
						count += 5;
						break;
					} /* end of inner switch */
				}   /* end of MXP enabled */
			} /* end of switch on character */
		}
	}   /* end of counting special characters */

	return count;
} /* end of count_mxp_tags */


void convert_mxp_tags( const int bMXP, char * dest, const char *src, int length )
{
	char		c;
	const char	*ps;
	char		*pd;
	int			bInTag = FALSE;
	int			bInEntity = FALSE;

	for ( ps = src, pd = dest; length > 0; ps++, length-- )
	{
		c = *ps;
		if ( bInTag )	/* in a tag, eg. <send> */
		{
			if ( c == MXP_ENDc )
			{
				bInTag = FALSE;
				if ( bMXP )
					*pd++ = '>';
			}
			else if ( bMXP )
				*pd++ = c;	/* copy tag only in MXP mode */
		} /* end of being inside a tag */
		else if ( bInEntity )	/* in a tag, eg. <send> */

		{
			if ( bMXP )
				*pd++ = c;  /* copy tag only in MXP mode */
			if ( c == ';' )
				bInEntity = FALSE;
		} /* end of being inside a tag */
		else
		{
			switch ( c )
			{
			case MXP_BEGc:
				bInTag = TRUE;
				if ( bMXP )
				{
#ifdef MXP_ZMUD
					memcpy (pd, MXPMODE (1), 4);
					pd += 4;
#endif
					*pd++ = '<';
				}
				break;

			case MXP_ENDc:	/* shouldn't get this case */
				if ( bMXP )
					*pd++ = '>';
				break;

			case MXP_AMPc:
				bInEntity = TRUE;
				if ( bMXP )
					*pd++ = '&';
				break;

			default:
				if ( bMXP )
				{
					switch ( c )
					{
					case '<':
						memcpy( pd, "&lt;", 4 );
						pd += 4;
						break;

					case '>':
						memcpy( pd, "&gt;", 4 );
						pd += 4;
						break;

					case '&':
						memcpy( pd, "&amp;", 5 );
						pd += 5;
						break;

					case '"':
						memcpy( pd, "&quot;", 6 );
						pd += 6;
						break;

					default:
						*pd++ = c;
						break;  /* end of default */
					} /* end of inner switch */
				}
				else
					*pd++ = c;  /* not MXP - just copy character */
				break;

			} /* end of switch on character */
		}
	}   /* end of converting special characters */
} /* end of convert_mxp_tags */
#endif


/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
#ifdef MXP
	int origlength;
	/*~~~~~~~~~~~*/
#endif

	if ( d == NULL )
	{
		bug( "write_to_buffer: descrittore NULL.", 0 );
		return;
	}

	/* Find length in case caller didn't. */
	if ( length <= 0 )
		length = strlen( txt );

#ifdef MXP
	origlength = length;

	/* work out how much we need to expand/contract it */
	length += count_mxp_tags( d->mxp, txt, length );
#endif

	/* Initial \n\r if needed. */
	if ( d->outtop == 0 && !d->fcommand )
	{
		d->outbuf[0] = '\n';
		d->outbuf[1] = '\r';
		d->outtop = 2;
	}

	/* Expand the buffer as needed. */
	while ( d->outtop + length >= d->outsize )
	{
		char	*outbuf;
		/*~~~~~~~~~~~~*/

		if ( d->outsize >= 32000 )
		{
			char	buf[MSL];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if ( (d != NULL) && (d->character != NULL) && (d->character->name != NULL) )
				sprintf( buf, "Buffer overflow. Chiusura di: [%s]", d->character->name );
			else
				sprintf( buf, "Buffer overflow. Chiusura di.. [?]" );
			bug( buf, 0 );
			close_socket( d );
			return;
		}

		outbuf = alloc_mem( 2 * d->outsize );
		strncpy( outbuf, d->outbuf, d->outtop );
		free_mem( d->outbuf, d->outsize );
		d->outbuf = outbuf;
		d->outsize *= 2;
	}

	/* Copy. */
#ifdef MXP
	convert_mxp_tags (d->mxp, d->outbuf + d->outtop, txt, origlength );
#else
	strcpy( d->outbuf + d->outtop, txt );
#endif
	d->outtop += length;
}


/*
 *    Lowest level output function. Write a block of text to the file descriptor. If this gives errors on very long
 *    blocks (like 'ofind all'), try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
	int iStart;
	int nWrite;
	int nBlock;
	/*~~~~~~~*/

	if ( length <= 0 )
		length = strlen( txt );


	for ( iStart = 0; iStart < length; iStart += nWrite )
	{
		nBlock = UMIN( length - iStart, 4096 );
		if ( (nWrite = write(desc, txt + iStart, nBlock)) < 0 )
		{
			perror( "Write_to_descriptor" );
			return( FALSE );
		}
	}

	return( TRUE );
}


/*
 *
 */
int search_sockets( DESCRIPTOR_DATA *inp )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	if ( IS_ADMIN(inp->character) )
		return( 0 );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( !strcmp(inp->host, d->host) )
		{
			if ( d->character && inp->character )
			{
				if ( !strcmp(inp->character->name, d->character->name) )
					continue;
			}

			return( 1 );
		}
	}

	return( 0 );
}


/*
 *
 */
int check_name_connected( DESCRIPTOR_DATA *inp, char *argument )
{
	DESCRIPTOR_DATA *d;
	/*~~~~~~~~~~~~~~~*/

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( inp != d && d->character && inp->character )
		{
			if ( !strcmp(argument, d->character->name) )
				return( 1 );
		}
	}

	return( 0 );
}


int		align_restrict	( CHAR_DATA *ch );
int		hometown_check	( CHAR_DATA *ch );
int		hometown_ok		( CHAR_DATA *ch, int home );
int		ethos_check		( CHAR_DATA *ch );


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
	DESCRIPTOR_DATA *d_old, *d_next;
	char			buf[MSL];
	char			buf1[MSL];
	char			arg[MIL];
	CHAR_DATA		*ch;
	char			*pwdnew;
	char			*p;
	int				iClass, race, i;
	bool			fOld;
	int				obj_count;
	int				obj_count2;
	OBJ_DATA		*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* OBJ_DATA *inobj; */
	while ( isspace(*argument) )
		argument++;

	ch = d->character;

	switch ( d->connected )
	{
	default:
		bug( "Nanny: bad d->connected %d.", d->connected );
		close_socket( d );
		return;

	case CON_GET_NAME:
		if ( argument[0] == '\0' )
		{
			close_socket( d );
			return;
		}

		argument[0] = UPPER( argument[0] );
		if ( !check_parse_name(argument) )
		{
			write_to_buffer( d, "\n\rNome non valido, riprova.\n\rCon che nome vuoi essere conosciuto su Xandra?: ", 0 );
			return;
		}

		fOld = load_char_obj( d, argument );
		ch = d->character;

		if ( get_trust(ch) < LEVEL_ADMIN )
		{
			if ( check_ban(d->host, BAN_PLAYER) )
			{
				write_to_buffer( d, "Il tuo indirizzo e' stato bannato da questo Mud.\n\r", 0 );
				close_socket( d );
				return;
			}

#ifdef NO_PLAYING_TWICE
			if ( search_sockets(d) )
			{
				write_to_buffer( d, "E' proibito giocare in questo Mud del multiplaying..\n\r", 0 );
				close_socket( d );
				return;
			}
#endif
		}

		if ( IS_SET(ch->act, PLR_DENY) )
		{
			sprintf( log_buf, "Accesso proibito a  %s@%s.", argument, d->host );
			log_string( log_buf );
			write_to_buffer( d, "Ti e' negato l'accesso.\n\r", 0 );
			close_socket( d );
			return;
		}

#ifdef MXP
		/* Telnet negotiation to see if they support MXP */
		write_to_buffer( d, (char*) will_mxp_str, 0 );
#endif

		if ( check_reconnect(d, FALSE) )
		{
			fOld = TRUE;
		}
		else
		{
			if ( wizlock && !IS_HERO(ch) )
			{
				write_to_buffer( d, "Il gioco e' aperto sono agli implementor.\n\r", 0 );
				close_socket( d );
				return;
			}

			if ( !IS_ADMIN(ch) && !IS_SET(ch->act, PLR_CANINDUCT) )
			{
				if ( iNumPlayers >= max_oldies && fOld )
				{
					sprintf( buf, "\n\rAttualmente vi sono %i giocatori, ma il massimo consentito e': %i.\n\rRiprova tra un po'.\n\r",
						iNumPlayers,
						max_oldies );
					write_to_buffer( d, buf, 0 );
					close_socket( d );
					return;
				}

				if ( iNumPlayers >= max_newbies && !fOld )
				{
					sprintf( buf, "\n\rAttualmente vi sono %i giocatori, ma il massimo consentito per i nuovi giocatori e' limitato a \n\r%i giocatori. Riprova tra un po' di tempo.\n\r",
						iNumPlayers,
						max_newbies );
					write_to_buffer( d, buf, 0 );
					close_socket( d );
					return;
				}
			}
		}

		if ( fOld )
		{
			/* Old player */
			write_to_buffer( d, "Password: ", 0 );
			write_to_buffer( d, (char *) echo_off_str, 0 );
			ch->temp_num = 0;
			d->connected = CON_GET_OLD_PASSWORD;
			return;
		}
		else
		{
			/* New player */
			if ( newlock )
			{
				write_to_buffer( d, "Non e' consentita la creazione di nuovi personaggi.\n\r", 0 );
				close_socket( d );
				return;
			}

			if ( check_ban(d->host, BAN_NEWBIES) )
			{
				write_to_buffer( d, "Nuovi personaggi non sono ammessi da questo indirizzo.\n\r", 0 );
				close_socket( d );
				return;
			}

			if ( check_name_connected(d, argument) )
			{
				write_to_buffer( d, "Questo giocatore sta gia' giocando, prova con un'altro.\n\rNome: ", 0 );
				free_char( d->character );
				d->character = NULL;
				d->connected = CON_GET_NAME;
				return;
			}

			do_help( ch, "NAME" );
			d->connected = CON_CONFIRM_NEW_NAME;
			return;
		}
		break;

	case CON_GET_OLD_PASSWORD:
		write_to_buffer( d, "\n\r", 2 );

		if ( strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd) )
		{
			write_to_buffer( d, "Password errata.\n\r", 0 );
			sprintf( buf, "Password errata da %s@%s", ch->name, d->host );
			log_string( buf );
			if ( ch->temp_num == 2 )
			{
				close_socket( d );
			}
			else
			{
				write_to_buffer( d, "Password: ", 0 );
				write_to_buffer( d, (char *) echo_off_str, 0 );
				d->connected = CON_GET_OLD_PASSWORD;
				ch->temp_num++;
			}

			return;
		}

		write_to_buffer( d, (char *) echo_on_str, 0 );

		if ( check_reconnect(d, TRUE) )
			return;

		if ( check_playing(d, ch->name) )
			return;

		/* Count objects in loaded player file */
		for ( obj = ch->carrying, obj_count = 0; obj != NULL; obj = obj->next_content )
			obj_count += get_obj_realnumber( obj );

		strcpy( buf, ch->name );

		free_char( ch );
		fOld = load_char_obj( d, buf );
		ch = d->character;

		if ( !fOld )
		{
			write_to_buffer( d, "Riprova per creare un nuovo personaggio.\n\r", 0 );
			close_socket( d );
			return;
		}

		/* Count objects in refreshed player file */
		for ( obj = ch->carrying, obj_count2 = 0; obj != NULL; obj = obj->next_content )
			obj_count2 += get_obj_realnumber( obj );

		sprintf( log_buf, "%s@%s si e' conness%c.", ch->name, d->host, ch->sex == 2 ? 'a' : 'o' );
		log_string( log_buf );

		if ( IS_HERO(ch) )
		{
			do_help( ch, "IMOTD" );
			d->connected = CON_READ_IMOTD;
		}
		else
		{
			do_help( ch, "MOTD" );
			d->connected = CON_READ_MOTD;
		}

		/* This player tried to use the clone cheat -- Log in once, connect a second time and enter only name,
		 *	drop all and quit with first character, finish login with second. This clones the player's inventory. */
		if ( obj_count != obj_count2 )
		{
			sprintf( log_buf, "%s@%s ha provato ad utilizzare il clone-cheat.", ch->name, d->host );
			log_string( log_buf );

			/* Removing the punishment for the sake of players */
/*			for ( obj = ch->carrying; obj != NULL; obj = inobj )
			{
				inobj = obj->next_content;
				extract_obj_nocount( obj );
			}
			for ( obj_count = 0; obj_count < MAX_STATS; obj_count++ )
				ch->perm_stat[obj_count]--;
			save_char_obj( ch );
*/
			send_to_char( "Gli Dei non approvano le tue azioni.\n\r", ch );
		}
		break;

	/* RT code for breaking link */
	case CON_BREAK_CONNECT:
		switch ( *argument )
		{
		case 's':
		case 'S':
			for ( d_old = descriptor_list; d_old != NULL; d_old = d_next )
			{
				d_next = d_old->next;
				if ( d_old == d || d_old->character == NULL )
					continue;

				if ( str_cmp(ch->name, d_old->character->name) )
					continue;

				close_socket( d_old );
			}

			if ( check_reconnect(d, TRUE) )
				return;
			write_to_buffer( d, "Riconnessione fallita.\n\rNome: ", 0 );
			if ( d->character != NULL )
			{
				free_char( d->character );
				d->character = NULL;
			}

			d->connected = CON_GET_NAME;
			break;

		case 'n':
		case 'N':
			write_to_buffer( d, "Nome: ", 0 );
			if ( d->character != NULL )
			{
				free_char( d->character );
				d->character = NULL;
			}

			d->connected = CON_GET_NAME;
			break;

		default:
			write_to_buffer( d, "Digita S o N: ", 0 );
			break;
		}
		break;

	case CON_CONFIRM_NEW_NAME:
		switch ( *argument )
		{
		case 'S':
		case 's':
			sprintf( buf, "\n\rNuovo personaggio.\n\rInserisci una password per %s: %s", ch->name, (char *) echo_off_str );
			write_to_buffer( d, buf, 0 );
			d->connected = CON_GET_NEW_PASSWORD;
			break;

		case 'N':
		case 'n':
			write_to_buffer( d, "Ok, allora chi sei ? ", 0 );
			free_char( d->character );
			d->character = NULL;
			d->connected = CON_GET_NAME;
			break;

		default:
			write_to_buffer( d, "Digita Si o No: ", 0 );
			break;
		}
		break;

	case CON_GET_NEW_PASSWORD:
		write_to_buffer( d, "\n\r", 2 );

		if ( strlen(argument) < 6 )
		{
			write_to_buffer( d, "La parola d'accesso deve essere lunga almeno 6 caratteri.\n\rPassword: ", 0 );
			return;
		}

		pwdnew = crypt( argument, ch->name );
		for ( p = pwdnew; *p != '\0'; p++ )
		{
			if ( *p == '~' )
			{
				write_to_buffer( d, "Questa parola d'accesso non e' accettabile, riprova.\n\rPassword: ", 0 );
				return;
			}
		}

		free_string( ch->pcdata->pwd );
		ch->pcdata->pwd = str_dup( pwdnew );
		write_to_buffer( d, "Digita nuovamente la password: ", 0 );
		d->connected = CON_CONFIRM_NEW_PASSWORD;
		break;

	case CON_CONFIRM_NEW_PASSWORD:
		write_to_buffer( d, "\n\r", 2 );

		if ( strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd) )
		{
			write_to_buffer( d, "Le password non corrispondono.\n\rRidigita la password: ", 0 );
			d->connected = CON_GET_NEW_PASSWORD;
			return;
		}

		write_to_buffer( d, (char *) echo_on_str, 0 );
		sprintf( buf, "\n\rQuesto mondo ospita %d diverse razze la cui breve descrizione e' riportata sotto:", MAX_PC_RACE - 1 );
		write_to_buffer( d, buf, 0 );
		write_to_buffer( d, "\n\r", 0 );

		write_to_buffer( d, "\n\rQuale e' il tuo sesso? (M/F)? ", 0 );
		d->connected = CON_GET_NEW_SEX;
		break;

	case CON_GET_NEW_SEX:
		switch ( argument[0] )
		{
		case 'm':
		case 'M':
			ch->sex = SEX_MALE;
			ch->pcdata->true_sex = SEX_MALE;
			break;

		case 'f':
		case 'F':
			ch->sex = SEX_FEMALE;
			ch->pcdata->true_sex = SEX_FEMALE;
			break;

		default:
			write_to_buffer( d, "Fai una decisione valida.\n\rQuale e' il tuo sesso? ", 0 );
			return;
		}

		do_help( ch, "RACETABLE" );
		write_to_buffer( d, "\n\rQuale e' la razza che vorresti interpretare? ", 0 );
		d->connected = CON_GET_NEW_RACE;

		break;

	case CON_GET_NEW_RACE:
		one_argument( argument, arg );

		if ( !str_cmp(arg, "help") || !str_cmp(arg, "aiuto") )
		{
			argument = one_argument( argument, arg );
			if ( argument[0] == '\0' )
			{
				sprintf( buf, "Questo mondo ospita %d diverse razze la cui breve descrizione e' riportata sotto:", MAX_PC_RACE - 1 );
				write_to_buffer( d, buf, 0 );
				write_to_buffer( d, "\n\r", 0 );
				do_help( ch, "RACETABLE" );
				break;
			}
			else
			{
				do_help( ch, argument );
				write_to_buffer( d, "Quale e' la razza che vorresti interpretare? (digita 'aiuto' per maggiori informazioni) ", 0 );
			}
			break;
		}

		race = race_lookup( argument );

		if ( race == 0 || !race_table[race].pc_race )
		{
			char temp[MIL];

			write_to_buffer( d, "Questa non e' una razza valida.\n\r", 0 );
			write_to_buffer( d, "Possono essere scelte le seguenti razze:\n\r", 0 );
			for ( race = 1; race_table[race].translate != NULL; race++ )
			{
				if ( !race_table[race].pc_race )
					continue;
				sprintf( temp, " %s", (ch->sex == SEX_FEMALE) ? race_table[race].ftranslate : race_table[race].translate );
				write_to_buffer( d, temp, 0 );
			}

			write_to_buffer( d, "\n\r", 0 );
			write_to_buffer( d, "Qual e' la tua razza? (aiuto 'razza' per maggiori informazioni) ", 0 );
			break;
		}

		/* Corregge il numero di razza gigante */
		if ( race == 52 )
			race = 11;

		ch->pcdata->race = race;
		ch->race = race;
		for ( i = 0; i < MAX_STATS; i++ )
			ch->mod_stat[i] = 0;

		/* (FF) Add race stat modifiers */
/*		for (i = 0; i < MAX_STATS; i++)
			ch->mod_stat[i] += pc_race_table[race].stats[i];
*/
		/* Add race modifiers */
		ch->max_point[LIFE] += pc_race_table[race].point[LIFE];
		ch->point[LIFE] = ch->max_point[LIFE];
		ch->max_point[MANA] += pc_race_table[race].point[MANA];
		ch->point[MANA] = ch->max_point[MANA];
		ch->max_point[MOVE] += pc_race_table[race].point[MOVE];	/* Xandra */
		ch->point[MOVE] = ch->max_point[MOVE];					/* Xandra */
		ch->practice += pc_race_table[race].prac_bonus;

		ch->detection = ch->affected_by | race_table[race].det;
		ch->affected_by = ch->affected_by | race_table[race].aff;
		ch->imm_flags = ch->imm_flags | race_table[race].imm;
		ch->res_flags = ch->res_flags | race_table[race].res;
		ch->vuln_flags = ch->vuln_flags | race_table[race].vuln;
		ch->form = race_table[race].form;
		ch->parts = race_table[race].parts;

		/* add skills */
		for ( i = 0; i < 5; i++ )
		{
			if ( pc_race_table[race].skills[i] == NULL )
				break;
			ch->pcdata->learned[skill_lookup( pc_race_table[race].skills[i] )] = 100;
		}

		/* add cost */
		ch->pcdata->points = pc_race_table[race].points;

		ch->size = pc_race_table[race].size;

		do_help( ch, "CLASSTABLE" );

		strcpy( buf, "\n\rScegli una classe tra:\n\r " );
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
			if ( class_ok(ch, iClass) )
			{
				if ( ch->sex == SEX_FEMALE )
					strcat( buf, class_table[iClass].name );
				else
					strcat( buf, class_table[iClass].fname );
				strcat( buf, " " );
			}
		}

		strcat( buf, ":\n\r " );
		write_to_buffer( d, buf, 0 );
		write_to_buffer( d, "Qual e' la tua classe? (aiuto per maggiori informazioni) ", 0 );
		d->connected = CON_GET_NEW_CLASS;
		break;

	case CON_GET_NEW_CLASS:
		iClass = class_lookup( argument );
		argument = one_argument( argument, arg );

		if ( !str_cmp(arg, "help") || !str_cmp(arg, "aiuto") )
		{
			if ( argument[0] == '\0' )
				do_help( ch, "CLASSTABLE" );
			else
				do_help( ch, argument );
			write_to_buffer( d, "Qual e' la tua classe? (aiuto per maggiori informazioni) ", 0 );
			return;
		}

		if ( iClass == -1 )
		{
			write_to_buffer( d, "Questa non e' una classe valida.\n\rQuale e' la tua classe? ", 0 );
			return;
		}

		if ( !class_ok(ch, iClass) )
		{
			write_to_buffer( d, "Questa classe non e' disponibile per la tua razza o per il tuo sesso.\n\rScegline un'altra: ", 0 );
			return;
		}

		ch->class = iClass;

		ch->pcdata->points += class_table[iClass].points;
		sprintf( buf, "Ora tu sei %s.\n\r", ch->sex == SEX_FEMALE ? class_table[iClass].fname : class_table[iClass].name );
		write_to_buffer( d, buf, 0 );

		for ( i = 0; i < MAX_STATS; i++ )
		{
			ch->perm_stat[i] = number_range( 10, (20 + pc_race_table[ORG_RACE(ch)].stat[i] + class_table[ch->class].stat[i]) );
			ch->perm_stat[i] = UMIN( 25, ch->perm_stat[i] );
		}

		for ( i = 0; i < MAX_STATS; i++ )
			ch->mod_stat[i] = 0;
		write_to_buffer( d, "\n\r", 2 );
		if ( !align_restrict(ch) )

		{
			write_to_buffer( d, "Puoi essere buono, neutrale o malvagio.\n\r", 0 );
			write_to_buffer( d, "Quale allineamento scegli (B/N/M)? ", 0 );
			d->connected = CON_GET_ALIGNMENT;
		}
		else
		{
			ch->temp_num = 100;
			d->connected = CON_PICK_HOMETOWN;
		}
		break;

	case CON_GET_ALIGNMENT:
		switch ( argument[0] )
		{
		case 'B':
		case 'b':
			ch->alignment = 1000;
			write_to_buffer( d, "Ora il tuo personaggio e' buono.\n\r", 0 );
			break;

		case 'N':
		case 'n':
			ch->alignment = 0;
			write_to_buffer( d, "Ora il tuo personaggio e' neutrale.\n\r", 0 );
			break;

		case 'M':
		case 'm':
			ch->alignment = -1000;
			write_to_buffer( d, "Ora il tuo personaggio e' malvagio.\n\r", 0 );
			break;

		default:
			write_to_buffer( d, "\n\rAllineamento non valido.\n\r", 0 );
			write_to_buffer( d, "\n\rPuoi essere Buono, Neutrale o Malvagio (B/N/M): ", 0 );
			return;
		}
		write_to_buffer( d, "[Premi Invio per continuare]\n\r", 0 );
		ch->temp_num = 100;
		d->connected = CON_PICK_HOMETOWN;
		break;

	case CON_PICK_HOMETOWN:
		sprintf( buf1, ", [O]fcol" );
		sprintf( buf, "[M]idgaard, [N]ew Thalos%s?", IS_NEUTRAL(ch) ? buf1 : "" );
		if ( ch->temp_num )
		{
			ch->temp_num = 0;
			if ( !hometown_check(ch) )
			{
				do_help( ch, "HOMETOWN" );
				write_to_buffer( d, buf, 0 );
				d->connected = CON_PICK_HOMETOWN;
				return;
			}
			else
			{
				ch->temp_num = 100;
				d->connected = CON_GET_ETHOS;
			}
			break;
		}

		switch ( argument[0] )
		{
		case 'H':
		case 'h':
		case 'A':
		case 'a':
		case '?':
			do_help( ch, "HOMETOWN" );
			write_to_buffer( d, buf, 0 );
			return;

		case 'M':
		case 'm':
			if ( hometown_ok(ch, 0) )
			{
				ch->hometown = 0;
				write_to_buffer( d, "La tua patria e' Midgaard.\n\r", 0 );
				break;
			}

		case 'N':
		case 'n':
			if ( hometown_ok(ch, 1) )
			{
				ch->hometown = 1;
				write_to_buffer( d, "La tua patria e' Nuova Thalos.\n\r", 0 );
				break;
			}

		case 'O':
		case 'o':
			if ( hometown_ok(ch, 3) )
			{
				ch->hometown = 3;
				write_to_buffer( d, "La tua patria e' Ofcol.\n\r", 0 );
				break;
			}

		default:
			write_to_buffer( d, "\n\rQuesta non e' una valida patria ove tu possa vivere.\n\r", 0 );
			write_to_buffer( d, "Quale patria vorresti? <digita aiuto per maggiori informazioni>? ", 0 );
			return;
		}
		ch->temp_num = 100;
		d->connected = CON_GET_ETHOS;
		break;

	case CON_GET_ETHOS:
		if ( !ch->temp_num )
		{
			switch ( argument[0] )
			{
			case 'H':
			case 'h':
			case '?':
				do_help( ch, "ALIGNMENT" );
				return;
				break;

			case 'L':
			case 'l':
				sprintf( buf, "\n\rOra sei legale-%s.\n\r", IS_GOOD(ch) ? "buono" : IS_EVIL(ch) ? "malvagio" : "neutrale" );
				write_to_buffer( d, buf, 0 );
				ch->ethos = 1;
				break;

			case 'N':
			case 'n':
				sprintf( buf, "\n\rOra sei neutrale-%s.\n\r", IS_GOOD(ch) ? "buono" : IS_EVIL(ch) ? "malvagio" : "neutrale" );
				write_to_buffer( d, buf, 0 );
				ch->ethos = 2;
				break;

			case 'C':
			case 'c':
				sprintf( buf, "\n\rOra sei caotico-%s.\n\r", IS_GOOD(ch) ? "buono" : IS_EVIL(ch) ? "malvagio" : "neutrale" );
				write_to_buffer( d, buf, 0 );
				ch->ethos = 3;
				break;

			default:
				write_to_buffer( d, "\n\rQuesta non e' una scelta etica valida.\n\r", 0 );
				write_to_buffer( d, "Quale etica vorresti? (L/N/C) <digita aiuto per maggiori informazioni> ", 0 );
				return;
			}
		}
		else
		{
			ch->temp_num = 0;
			if ( !ethos_check(ch) )
			{
				write_to_buffer( d, "Quale etica vorresti? (L/N/C) <digita aiuto per maggiori informazioni> ?", 0 );
				d->connected = CON_GET_ETHOS;
				return;
			}
		}

		write_to_buffer( d, "[Premi Invio per continuare]\n\r", 0 );
		write_to_buffer( d, "\n\rVuoi i comandi, per interagire col gioco, in inglese o in italiano? ", 0 );
		d->connected = CON_GET_ENGLISH;
		break;

	case CON_GET_ENGLISH:
		switch ( argument[1] )
		{
		case 'T':
		case 't':
			REMOVE_BIT( ch->act, PLR_ENGLISH );
			write_to_buffer( d, "\n\rOra invierai i comandi con la lingua italiana.\n\r", 0 );
			break;

		case 'N':
		case 'n':
			SET_BIT( ch->act, PLR_ENGLISH );
			write_to_buffer( d, "\n\rOra invierai i comandi con la lingua inglese.\n\r", 0 );
			break;

		default:
			write_to_buffer( d, "\n\rScelta errata, digita ita o ing. ", 0 );
			return;
		}
		write_to_buffer( d, "[Premi Invio per continuare]\n\r", 0 );
		d->connected = CON_CREATE_DONE;		/* Fix di Corben: Mancava per uscire dalla domanda */
		break;

	case CON_CREATE_DONE:
		sprintf( log_buf, "%s@%s nuovo giocatore.", ch->name, d->host );
		log_string( log_buf );
		group_add( ch );
		ch->pcdata->learned[gsn_recall] = 75;
		do_help( ch, "GENERAL" );
		write_to_buffer( d, "[Premi Invio per continuare]\n\r", 0 );
		d->connected = CON_READ_NEWBIE;
		break;

	case CON_READ_NEWBIE:
		write_to_buffer( d, "\n\r", 2 );
		do_help( ch, "MOTD" );
		d->connected = CON_READ_MOTD;
		break;

	case CON_READ_IMOTD:
		write_to_buffer( d, "\n\r", 2 );
		do_help( ch, "MOTD" );
		d->connected = CON_READ_MOTD;
		break;

	case CON_READ_MOTD:
		write_to_buffer( d, "\n\rBenvenuto in questo mondo! Divertiti!..\n\r\n\r", 0 );
		ch->next = char_list;
		char_list = ch;
		d->connected = CON_PLAYING;
		{
			int			count;
			extern int	max_on;
			/*~~~~~~~~~~~~~~~*/

			count = 0;
			for ( d = descriptor_list; d != NULL; d = d->next )
			{
				if ( d->connected == CON_PLAYING )
					count++;
			}

			max_on = UMAX( count, max_on );
		}

		iNumPlayers++;

		if ( ch->level == 0 )
		{
			int l;
			/*~~*/

			ch->level = 1;
			ch->exp = base_exp( ch, ch->pcdata->points );
			ch->point[LIFE] = ch->max_point[LIFE];
			ch->point[MANA] = ch->max_point[MANA];
			ch->point[MOVE] = ch->max_point[MOVE];
			ch->pcdata->perm_point[LIFE] = ch->max_point[LIFE];
			ch->pcdata->perm_point[MANA] = ch->max_point[MANA];
			ch->pcdata->perm_point[MOVE] = ch->max_point[MOVE];
			ch->train += 3;
			ch->practice += 5;
			ch->pcdata->death = 0;

			obj_to_char( create_object(get_obj_index(OBJ_VNUM_MAP), 0), ch );
			obj_to_char( create_object(get_obj_index(OBJ_VNUM_NMAP1), 0), ch );
			obj_to_char( create_object(get_obj_index(OBJ_VNUM_NMAP2), 0), ch );

			if ( ch->hometown == 0 && IS_EVIL(ch) )	obj_to_char( create_object(get_obj_index(OBJ_VNUM_MAP_SM), 0), ch );
			if ( ch->hometown == 1 )				obj_to_char( create_object(get_obj_index(OBJ_VNUM_MAP_NT), 0), ch );
			if ( ch->hometown == 3 )				obj_to_char( create_object(get_obj_index(OBJ_VNUM_MAP_OFCOL), 0), ch );
			if ( ch->hometown == 2 )				obj_to_char( create_object(get_obj_index(OBJ_VNUM_MAP_TITAN), 0), ch );
			if ( ch->hometown == 4 )				obj_to_char( create_object(get_obj_index(OBJ_VNUM_MAP_OLD), 0), ch );

			/* Dona la pg nuovo un po' di viveri */
			obj_to_char( create_object(get_obj_index(20), 0), ch );	/* mela magica in limbo */
			obj_to_char( create_object(get_obj_index(9413), 3), ch );	/* panini all'olio del palazzone */

			ch->pcdata->learned[get_weapon_sn( ch, FALSE )] = 40;

			/* Setto di default le opzioni piu' utilizzate */
			SET_BIT( ch->act, PLR_COLOR );

			char_to_room( ch, get_room_index(ROOM_VNUM_SCHOOL) );
			send_to_char( "\n\r", ch );
			do_help( ch, "NEWBIE INFO" );
			send_to_char( "\n\r", ch );

			/* give some bonus time */
			for ( l = 0; l < MAX_TIME_LOG; l++ )
				ch->pcdata->log_time[l] = 60;

			do_outfit( ch, "" );
		}
		else if ( ch->in_room != NULL )
		{
			if ( cabal_area_check(ch) )
			{
				int all;
				/*~~*/

				if ( IS_GOOD(ch) )
					all = 0;
				if ( IS_EVIL(ch) )
					all = 2;
				else
					all = 1;
				char_to_room( ch, get_room_index(hometown_table[ch->hometown].altar[all]) );
			}
			else
				char_to_room( ch, ch->in_room );

			/* Rimuove il bit sull'afk */
			REMOVE_BIT( ch->act, PLR_AFK );
		}
		else if ( IS_ADMIN(ch) )
			char_to_room( ch, get_room_index(ROOM_VNUM_CHAT) );
		else
			char_to_room( ch, get_room_index(ROOM_VNUM_TEMPLE) );

		reset_char( ch );
		if ( !IS_ADMIN(ch) )
			act( "$n sbuca da un abbagliante portale.", ch, NULL, NULL, TO_ROOM, POS_REST );
		wiznet( "$N entra nel Mud.", ch, NULL, WIZ_LOGINS, 0 );

		if ( ch->exp < (exp_per_level(ch, ch->pcdata->points) * ch->level) )
			ch->exp = ( ch->level ) * ( exp_per_level(ch, ch->pcdata->points) );
		else if ( ch->exp > (exp_per_level(ch, ch->pcdata->points) * (ch->level + 1)) )
		{
			ch->exp = ( ch->level + 1 ) * ( exp_per_level(ch, ch->pcdata->points) );
			ch->exp -= 10;
		}

		if ( IS_SET(ch->act, PLR_QUESTOR) && ch->pcdata->questmob == 0 )
		{
			ch->pcdata->nextquest = ch->pcdata->countdown;
			ch->pcdata->questobj = 0;
			REMOVE_BIT( ch->act, PLR_QUESTOR );
		}

		if ( IS_SET(ch->act, PLR_NO_EXP) )
			REMOVE_BIT( ch->act, PLR_NO_EXP );
		if ( IS_SET(ch->act, PLR_CHANGED_AFF) )
			REMOVE_BIT( ch->act, PLR_CHANGED_AFF );

		for ( i = 0; i < MAX_STATS; i++ )
		{
			if ( ch->perm_stat[i] > (20 + pc_race_table[ORG_RACE(ch)].stat[i] + class_table[ch->class].stat[i]) )
			{
				ch->train += ( ch->perm_stat[i] - (20 + pc_race_table[ORG_RACE(ch)].stat[i] + class_table[ch->class].stat[i]) );
				ch->perm_stat[i] = 20 + pc_race_table[ORG_RACE( ch )].stat[i] + class_table[ch->class].stat[i];
			}
		}

		do_look( ch, "auto" );

		if ( !IS_ADMIN(ch) && ch->gold > 5000 && ch->gold <= 10000 )
		{
			long	tax;

			tax = ( number_range(1, 50) / 4 ) * 100;
			sprintf( buf, "Vieni tassat$x di %ld monet%c d'oro per il bar Mayor.\n\r",
				tax, (tax == 1) ? 'a' : 'e' );
			act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
			ch->gold -= tax;
		}

		if ( !IS_ADMIN(ch) && ch->gold > 10000 )
		{
			long	tax;

			tax = ( number_range(100, ch->gold/100) / 2 ) * 100;
			sprintf( buf, "Vieni tassat$x di %ld monet%c d'oro per la Guardia Cittadina.\n\r",
				tax, (tax == 1) ? 'a' : 'e' );
			act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
			ch->gold -= tax;
		}

		if ( !IS_ADMIN(ch) && ch->pcdata->bank_g > 450000 && (number_range(0, 9) == 0) )
		{
			long	tax;

			tax = number_range(500, 2500) * 100;
			sprintf( buf, "Vieni tassat$x di %ld monet%c d'oro da pagare per i costi militari del Sultano.\n\r",
				tax, (tax == 1) ? 'a' : 'e' );
			act( buf, ch, NULL, NULL, TO_CHAR, POS_REST );
			ch->pcdata->bank_g -= tax;
		}

		if ( ch->pet != NULL )
		{
			char_to_room( ch->pet, ch->in_room );
			act( "$n e' entrat$x in gioco.", ch->pet, NULL, NULL, TO_ROOM, POS_REST );
		}

		if ( ch->pcdata->confirm_delete )
			ch->pcdata->confirm_delete = FALSE;

		do_unread( ch, "login" );
		break;
	}
}



/*
 *    Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
	/* Reserved words. */
	if (	is_name(name, "all auto immortale stesso qualcuno qualcosa il lo la gli le io tu egli noi voi essi demise bilancio balance circle loner honor onore cazzo culo figa") )
		return( FALSE );

	/* Length restrictions. */
	if ( strlen(name) < 2 )
		return( FALSE );

	if ( strlen(name) > 12 )
		return( FALSE );

	/* Alphanumerics only. Lock out IllIll twits. */
	{
		char	*pc;
		bool	fIll, adjcaps = FALSE, cleancaps = FALSE;
		int		total_caps = 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


		fIll = TRUE;
		for ( pc = name; *pc != '\0'; pc++ )
		{
			if ( !isalpha(*pc) )
				return( FALSE );

			if ( isupper(*pc) ) /* ugly anti-caps hack */
			{
				if ( adjcaps )
					cleancaps = TRUE;
				total_caps++;
				adjcaps = TRUE;
			}
			else
				adjcaps = FALSE;

			if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
				fIll = FALSE;
		}

		if ( fIll )
			return( FALSE );

		if ( (total_caps > (strlen(name)) / 2 && strlen(name) < 3) )
			return( FALSE );
	}

	/* Prevent players from naming themselves after mobs. */
	{
		extern MOB_INDEX_DATA	*mob_index_hash[MAX_KEY_HASH];
		MOB_INDEX_DATA			*pMobIndex;
		int						iHash;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
		{
			for ( pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL; pMobIndex = pMobIndex->next )
			{
				if ( is_name(name, pMobIndex->player_name) )
					return( FALSE );
			}
		}
	}

	return( TRUE );
}


/*
 *    Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, bool fConn )
{
	CHAR_DATA	*ch;
	/*~~~~~~~~~~~~*/

	for ( ch = char_list; ch != NULL; ch = ch->next )
	{
		if ( !IS_MOB(ch) && (!fConn || ch->desc == NULL) && !str_cmp(d->character->name, ch->name) )
		{
			if ( fConn == FALSE )
			{
				free_string( d->character->pcdata->pwd );
				d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
			}
			else
			{
				OBJ_DATA	*obj;
				/*~~~~~~~~~~~~~*/

				free_char( d->character );
				d->character = ch;
				ch->desc = d;
				ch->timer = 0;
				if ( !IS_SET(ch->act, PLR_ENGLISH) )
					send_to_char( "Riconnessione. Digita ripeti per leggere i messaggi tell non sentiti.\n\r", ch );
				else
					send_to_char( "Riconnessione. Digita replay per leggere i messaggi tell non sentiti.\n\r", ch );
				if ( !IS_ADMIN(ch) )
					act( "$n si e' riconnesso.", ch, NULL, NULL, TO_ROOM, POS_REST );
				if ( (obj = get_light_char(ch)) != NULL )
					--ch->in_room->light;

				sprintf( log_buf, "%s@%s si e' riconnesso.", ch->name, d->host );
				log_string( log_buf );

				wiznet( "$N e' riuscito a ristabilire la connessione.", ch, NULL, WIZ_LINKS, 0 );
				d->connected = CON_PLAYING;
			}

			return( TRUE );
		}
	}

	return( FALSE );
}


/*
 *    Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
	DESCRIPTOR_DATA *dold;
	/*~~~~~~~~~~~~~~~~~~*/

	for ( dold = descriptor_list; dold; dold = dold->next )
	{
		if (dold != d
		&&	dold->character != NULL
		&&	dold->connected != CON_GET_NAME
		&&	dold->connected != CON_GET_OLD_PASSWORD
		&&	!str_cmp(name, dold->original ? dold->original->name : dold->character->name) )
		{
			write_to_buffer( d, "Questo personaggio sta gia' giocando.\n\r", 0 );
			write_to_buffer( d, "Vuoi entrare ugualmente (S/N)?", 0 );
			d->connected = CON_BREAK_CONNECT;
			return( TRUE );
		}
	}

	return( FALSE );
}


/*
 *
 */
void stop_idling( CHAR_DATA *ch )
{
	if( ch == NULL
	||	ch->desc == NULL
	||	ch->desc->connected != CON_PLAYING
	||	ch->was_in_room == NULL
	||	ch->in_room != get_room_index(ROOM_VNUM_LIMBO) )
		return;

	ch->timer = 0;
	char_from_room( ch );
	char_to_room( ch, ch->was_in_room );
	ch->was_in_room = NULL;

	act( "Sei tornato dal Vuoto.", ch, NULL, NULL, TO_CHAR, POS_REST );
	act( "$n e' tornato dal Vuoto.", ch, NULL, NULL, TO_ROOM, POS_REST );
}


/*
 * Write to one char.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
	if ( txt != NULL && ch->desc != NULL )
		write_to_buffer( ch->desc, txt, strlen(txt) );
}


/*
 * Send a page to one char.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
	if ( txt == NULL || ch->desc == NULL )
		return; /* ben yazdim ibrahim */

	if ( ch->lines == 0 )
	{
		send_to_char( txt, ch );
		return;
	}

	ch->desc->showstr_head = alloc_mem( strlen(txt) + 1 );
	strcpy( ch->desc->showstr_head, txt );
	ch->desc->showstr_point = ch->desc->showstr_head;
	show_string( ch->desc, "" );
}


/*
 * String pager
 */
void show_string( struct descriptor_data *d, char *input )
{
	char			buffer[4 * MSL];
	char			buf[MIL];
	register char	*scan, *chk;
	int				lines = 0, toggle = 1;
	int				show_lines;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	one_argument( input, buf );
	if ( buf[0] != '\0' )
	{
		if ( d->showstr_head )
		{
			free_string( d->showstr_head );
			d->showstr_head = 0;
		}

		d->showstr_point = 0;
		return;
	}

	if ( d->character )
		show_lines = d->character->lines;
	else
		show_lines = 0;

	for ( scan = buffer; ; scan++, d->showstr_point++ )
	{
		if ( ((*scan = *d->showstr_point) == '\n' || *scan == '\r') && (toggle = -toggle) < 0 )
			lines++;
		else if ( !*scan || (show_lines > 0 && lines >= show_lines) )
		{
			*scan = '\0';
			write_to_buffer( d, buffer, strlen(buffer) );
			for ( chk = d->showstr_point; isspace(*chk); chk++ )
				;

			if ( !*chk )
			{
				if ( d->showstr_head )
				{
					free_string( d->showstr_head );
					d->showstr_head = 0;
				}
				d->showstr_point = 0;
			}
			return;
		}
	}
}


/*
 * Quick sex fixer
 */
void fix_sex( CHAR_DATA *ch )
{
	if ( ch->sex < 0 || ch->sex > 2 )
		ch->sex = IS_MOB( ch ) ? 0 : ch->pcdata->true_sex;
}


int PERS_SEX( CHAR_DATA *ch, CHAR_DATA *looker )
{
	if ( can_see(looker, ch) )
	{
		if ( IS_MOB(ch) )
			return ch->sex;
		else
		{
			if ( IS_VAMPIRE(ch) && !IS_ADMIN(looker) )
				return 1;
			else
				return ch->sex;
		}
	}
	else
		return 1;
}


void act( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int min_pos, ... )
{
	static char *const	he_she[] = { "lui", "lui", "lei" };
	static char *const	him_her[] = { "lo", "lo", "la" };
	static char *const	char_sex[] = { "o", "o", "a" };
	char				buf[MSL];
	char				fname[MIL];
	CHAR_DATA			*to;
	CHAR_DATA			*vch = (CHAR_DATA *) arg2;
	OBJ_DATA			*obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA			*obj2 = (OBJ_DATA *) arg2;
	const char			*str;
	const char			*i;
	char				*point;
	int					n;
	va_list				colors;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Discard null and zero-length messages. */
	if ( format == NULL || format[0] == '\0' )
		return;

	/* discard null rooms and chars */
	if ( ch == NULL || ch->in_room == NULL )
		return;

	to = ch->in_room->people;
	if ( type == TO_VICT )
	{
		if ( vch == NULL )
		{
			bug( "Act: vch Null con TO_VICT.", 0 );
			return;
		}

		if ( vch->in_room == NULL )
			return;

		to = vch->in_room->people;
	}

	for ( ; to != NULL; to = to->next_in_room )
	{
		va_start( colors, min_pos );

		if ( to->desc == NULL || to->position < min_pos )
			continue;

		if ( type == TO_CHAR && to != ch )						continue;
		if ( type == TO_VICT && (to != vch || to == ch) )		continue;
		if ( type == TO_ROOM && to == ch )						continue;
		if ( type == TO_NOTVICT && (to == ch || to == vch) )	continue;

		point = buf;
		str = format;
		while ( *str != '\0' )
		{
			if ( *str != '$' )
			{
				*point++ = *str++;
				continue;
			}

			++str;

			switch ( *str )
			{
			/* Thx alex for 't' idea */
			case 't':
				i = (char *) arg1;
				break;

			case 'T':
				i = (char *) arg2;
				break;

			case 'n':
				i = ( is_affected(ch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
					PERS( ch->doppel, to ) : PERS( ch, to );
				break;

			case 'N':
				i = ( is_affected(vch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
					PERS( vch->doppel, to ) : PERS( vch, to );
				break;

			case 'x':
				if ( type == TO_CHAR )
				{
					i = ( is_affected(ch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
						char_sex[URANGE(0, ch->doppel->sex, 2)] : char_sex[URANGE(0, ch->sex, 2)];
				}
				else
				{
					i = ( is_affected(ch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
						char_sex[PERS_SEX(ch->doppel, to)] : char_sex[PERS_SEX(ch, to)];
				}
				break;

			case 'X':
				if ( type == TO_CHAR )
				{
					i = ( is_affected(vch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
						char_sex[URANGE(0, vch->doppel->sex, 2)] : char_sex[URANGE(0, vch->sex, 2)];
				}
				else
				{
					i = ( is_affected(vch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
						char_sex[PERS_SEX(vch->doppel, to)] : char_sex[PERS_SEX(vch, to)];
				}
				break;

			case 'e':
				i = ( is_affected(ch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
					he_she[URANGE( 0, ch->doppel->sex, 2 )] : he_she[URANGE( 0, ch->sex, 2 )];
				break;

			case 'E':
				i = ( is_affected(vch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) )?
					he_she[URANGE( 0, vch->doppel->sex, 2 )] : he_she[URANGE( 0, vch->sex, 2 )];
				break;

			case 'm':
				i = ( is_affected(ch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
					him_her[URANGE( 0, ch->doppel->sex, 2 )] : him_her[URANGE( 0, ch->sex, 2 )];
				break;

			case 'M':
				i = ( is_affected(vch, gsn_doppelganger) && !IS_SET(to->act, PLR_HOLYLIGHT) ) ?
					him_her[URANGE( 0, vch->doppel->sex, 2 )] : him_her[URANGE( 0, vch->sex, 2 )];
				break;

			case 'C':
				if ( IS_SET(to->act, PLR_COLOR) )
					i = va_arg( colors, char * );
				else
					i = "";
				break;

			case 'c':
				if ( IS_SET(to->act, PLR_COLOR) )
					i = NORMAL;
				else
					i = "";
				break;

			case 'p':
				i = can_see_obj( to, obj1 ) ? obj1->short_descr : "qualcosa";
				break;

			case 'P':
				i = can_see_obj( to, obj2 ) ? obj2->short_descr : "qualcosa";
				break;

			case 'd':
				if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
				{
					i = "porta";
				}
				else
				{
					one_argument( (char *) arg2, fname );
					i = fname;
				}
				break;
			default:
				bug( "Act: bad code %d.", *str );
				i = " <@@@> ";
				break;
			}	/* chiude lo switch */

			++str;
			while ( (*point = *i) != '\0' )
				++point, ++i;
		}		/* chiude il while */

		*point++ = '\n';
		*point++ = '\r';

		/* fix for color prefix and capitalization */
		if ( buf[0] == '' )
		{
			for ( n = 1; buf[n] != 'm'; n++ )
				;
			buf[n + 1] = UPPER( buf[n + 1] );
		}
		else
			buf[0] = UPPER( buf[0] );

		write_to_buffer( to->desc, buf, point - buf );

		va_end( colors );
	}			/* chiude il for */
}


/*
 *    writes bug directly to user screen.
 */
void dump_to_scr( char *text )

{
	int a;
	/*~~*/

	a = strlen( text );
	write( 1, text, a );
}


/*
 *
 */
int log_area_popularity( void )
{
	FILE				*fp;
	AREA_DATA			*area;
	extern AREA_DATA	*area_first;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	system( "rm -f area_stat.txt" );
	fp = fopen( AREASTAT_FILE, "a" );
	fprintf( fp, "\nBoot: %sStatistiche di popolazione delle aree (in char * ticks)\n",
		(char *) ctime(&boot_time) );

	for ( area = area_first; area != NULL; area = area->next )
	{
		if ( area->count >= 5000000 )
			fprintf( fp, "%-60s overflow\n", area->name );
		else
			fprintf( fp, "%-60s %lu\n", area->name, area->count );
	}

	fclose( fp );

	return( 1 );
}


/*
 *    Function for save processes.
 */
void exit_function( void )
{
	dump_to_scr( "Exiting from the player saver.\n\r" );
	wait( NULL );
}


/*
 *
 */
int sex_ok( void )
{
	return 1;
}


/*
 *
 */
int class_ok( CHAR_DATA *ch, int class )
{
	if ( pc_race_table[ORG_RACE(ch)].class_mult[class] == -1 )
		return( 0 );

	return( 1 );
}


/*
 *
 */
int align_restrict( CHAR_DATA *ch )
{
	DESCRIPTOR_DATA *d = ch->desc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( IS_SET(pc_race_table[ORG_RACE(ch)].align, CR_GOOD) || IS_SET(class_table[ch->class].align, CR_GOOD) )
	{
		write_to_buffer( d, "Il tuo personaggio e' Buono.\n\r", 0 );
		ch->alignment = 1000;
		return( N_ALIGN_GOOD );
	}

	if ( IS_SET(pc_race_table[ORG_RACE(ch)].align, CR_NEUTRAL) || IS_SET(class_table[ch->class].align, CR_NEUTRAL) )
	{
		write_to_buffer( d, "Il tuo personaggio e' Neutrale.\n\r", 0 );
		ch->alignment = 0;
		return( N_ALIGN_NEUTRAL );
	}

	if ( IS_SET(pc_race_table[ORG_RACE(ch)].align, CR_EVIL) || IS_SET(class_table[ch->class].align, CR_EVIL) )
	{
		write_to_buffer( d, "Il tuo personaggio e' Malvagio.\n\r", 0 );
		ch->alignment = -1000;
		return( N_ALIGN_EVIL );
	}

	return( N_ALIGN_ALL );
}


/*
 *
 */
int hometown_check( CHAR_DATA *ch )
{
	DESCRIPTOR_DATA *d = ch->desc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->class == CLASS_VAMPIRE || ch->class == CLASS_NECROMANCER )
	{
		write_to_buffer( d, "\n\r", 0 );
		write_to_buffer( d, "La tua patria e' Old Midgaard, permanentemente.\n\r", 0 );
		ch->hometown = 4;
		write_to_buffer( d, "\n\r", 0 );
		return( 1 );
	}

	if ( ORG_RACE(ch) == 11 )
	{
		write_to_buffer( d, "\n\r", 0 );
		write_to_buffer( d, "La tua patria e' la Valle dei Titani, permanentemente.\n\r", 0 );
		ch->hometown = 2;
		write_to_buffer( d, "\n\r", 0 );
		return( 1 );
	}

	return( 0 );
}


/*
 *
 */
int hometown_ok( CHAR_DATA *ch, int home )
{
	if ( !IS_NEUTRAL(ch) && home == 3 )
		return( 0 );

	return( 1 );
}


/*
 *
 */
int ethos_check( CHAR_DATA *ch )
{
	DESCRIPTOR_DATA *d = ch->desc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( ch->class == CLASS_PALADIN )
	{
		ch->ethos = 1;
		write_to_buffer( d, "Sei legale.\n\r", 0 );
		return( 1 );
	}

	return( 0 );
}
