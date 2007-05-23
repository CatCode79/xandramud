/*--------------------------------------------------------------------------
 *               .88b  d88. db    db d8888b.   .d888b. db   dD
 *               88'YbdP`88 88    88 88  `8D   VP  `8D 88 ,8P'
 *               88  88  88 88    88 88   88      odD' 88,8P
 *               88  88  88 88    88 88   88    .88'   88`8b
 *               88  88  88 88b  d88 88  .8D   j88.    88 `88.
 *               YP  YP  YP ~Y8888P' Y8888D'   888888D YP   YD
 * This material is copyrighted (c) 1999 - 2000 by Thomas J Whiting
 * (twhiting@hawmps.2y.net). Usage of this material  means that you have read
 * and agree to all of the licenses in the ../licenses directory. None of these
 * licenses may ever be removed.
 * ----------------------------------------------------------------------------
 * A LOT of time has gone into this code by a LOT of people. Not just on
 * this individual code, but on all of the codebases this even takes a piece
 * of. I hope that you find this code in some way useful and you decide to
 * contribute a small bit to it. There's still a lot of work yet to do.
 * ---------------------------------------------------------------------------
 */


#ifdef WEATHER

#include <stdio.h>
#include <string.h>

#include "xandra.h"


void	lightning		( void );
void	ice				( void );
void	hail			( void );
void	blizzard		( void );
void	fog				( void );
void	weather_update	( void );


extern char *target_name;

#define LI1		MAX_LEVEL/6
#define LI2		MAX_LEVEL/3
#define LI3		MAX_LEVEL


/*
 * Update the weather.
 */
void weather_update( void )
{
	char buf[MSL];
	DESCRIPTOR_DATA *d;

	buf[0] = '\0';

	switch ( ++time_info.hour )
	{
	  case  5:
		weather_info.sunlight = SUN_LIGHT;
		strcat( buf, "Un nuovo giorno e' appena nato..\n\r" );
		break;

	  case  6:
		weather_info.sunlight = SUN_RISE;
		strcat( buf, "Lentamente, sorge il sole, rischiarando l'est.\n\r" );
		break;

	  case 19:
		weather_info.sunlight = SUN_SET;
		strcat( buf, "Lentamente, tramonta il sole, ad occidente.\n\r" );
		break;

	  case 20:
		weather_info.sunlight = SUN_DARK;
		strcat( buf, "Scende la notte con il suo nero manto..\n\r" );
		break;

	  case 24:
		time_info.hour = 0;
		time_info.day++;
		break;
	}

	if ( time_info.day   >= 35 )
	{
		time_info.day = 0;
		time_info.month++;
	}

	if ( time_info.month >= 12 )
	{
		time_info.month = 0;
		time_info.year++;
	}

	/*
	 * Weather change.
	 */
	switch ( weather_info.sky )
	{
	  default:
		weather_info.sky = SKY_CLOUDLESS;
		break;

	  case SKY_CLOUDLESS:
		if		( (number_chance(10)) && (time_info.hour <= 6) )	weather_info.sky = SKY_FOGGY;
		else if (  number_chance(15) )								weather_info.sky = SKY_CLOUDY;
		else if (  number_chance(30) )								weather_info.sky = SKY_RAINING;
		else if (  number_chance(45) )								weather_info.sky = SKY_CLOUDLESS;
		break;

	  case SKY_CLOUDY:
		if		( number_chance(15) )		weather_info.sky = SKY_SNOWING;
		else if ( number_chance(15) )		weather_info.sky = SKY_HAILSTORM;
		else if ( number_chance(15) )		weather_info.sky = SKY_THUNDERSTORM;
		else if ( number_chance(15) )		weather_info.sky = SKY_ICESTORM;
		else if ( number_chance(15) )		weather_info.sky = SKY_CLOUDLESS;
		else if ( number_chance(25) )		weather_info.sky = SKY_CLOUDY;

	  case SKY_RAINING:
		if ( number_chance(15) )
		{
			weather_info.sky = SKY_LIGHTNING;
			lightning( );
		}
		else if ( number_chance(10) )
		{
			weather_info.sky = SKY_HAILSTORM;
			hail( );
		}
		else if ( number_chance(10) )		weather_info.sky = SKY_THUNDERSTORM;
		else if ( number_chance(10) )		weather_info.sky = SKY_CLOUDY;
		else if ( number_chance(55) )		weather_info.sky = SKY_RAINING;
		break;

	  case SKY_SNOWING:
		if		( number_chance(15) )		weather_info.sky = SKY_BLIZZARD;
		else if ( number_chance(15) )		weather_info.sky = SKY_CLOUDY;
		else if ( number_chance(15) )		weather_info.sky = SKY_RAINING;
		else if ( number_chance(55) )		weather_info.sky = SKY_SNOWING;
		break;

	  case SKY_LIGHTNING:
		if		( number_chance(15) )		weather_info.sky = SKY_THUNDERSTORM;
		else if ( number_chance(15) )		weather_info.sky = SKY_RAINING;
		else if ( number_chance(15) )		weather_info.sky = SKY_CLOUDY;
		else if ( number_chance(15) )		weather_info.sky = SKY_HAILSTORM;
		else if ( number_chance(40) )		weather_info.sky = SKY_LIGHTNING;
		break;

	  case SKY_FOGGY:
		if		( number_chance(45) )		weather_info.sky = SKY_CLOUDY;
		else if ( number_chance(55) )	{	weather_info.sky = SKY_FOGGY;			fog();			}
		break;

	  case SKY_THUNDERSTORM:
		if		( number_chance(15) )		weather_info.sky = SKY_RAINING;
		else if ( number_chance(15) )		weather_info.sky = SKY_CLOUDY;
		else if ( number_chance(15) )		weather_info.sky = SKY_LIGHTNING;
		else if ( number_chance(15) )	{	weather_info.sky = SKY_HAILSTORM;		hail( );		}
		else if ( number_chance(40) )		weather_info.sky = SKY_THUNDERSTORM;
		break;

	  case SKY_HAILSTORM:
		if		( number_chance(15) )		weather_info.sky = SKY_CLOUDY;
		else if ( number_chance(30) )		weather_info.sky = SKY_RAINING;
		else if ( number_chance(55) )	{	weather_info.sky = SKY_HAILSTORM;		hail( );		}
		break;

	  case SKY_ICESTORM:
		if		( number_chance(15) )		weather_info.sky = SKY_CLOUDY;
		else if ( number_chance(15) )		weather_info.sky = SKY_BLIZZARD;
		else if ( number_chance(15) )		weather_info.sky = SKY_SNOWING;
		else if ( number_chance(55) )	{	weather_info.sky = SKY_ICESTORM;		ice( );			}
		break;

	  case SKY_BLIZZARD:
		if		( number_chance(15) )		weather_info.sky = SKY_SNOWING;
		else if ( number_chance(15) )	{	weather_info.sky = SKY_ICESTORM;		blizzard( );	ice( );		}
		else if ( number_chance(15) )	{	weather_info.sky = SKY_CLOUDY;			blizzard( );	}
		else if ( number_chance(55) )	{	weather_info.sky = SKY_BLIZZARD;		blizzard( );	}
		break;
	}

	if ( buf && buf[0] != '\0' )
	{
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected == CON_PLAYING
			  && IS_OUTSIDE(d->character)
			  && IS_AWAKE(d->character) )
				send_to_char( buf, d->character );
		}
	}
}


/*
 * New weather command. Reads the new weather stats as well as  tells you
 * what time of day it is (morning, noon, night)
 * Added so that players don't HAVE to have autoweather enabled
 */
void do_weather( CHAR_DATA *ch, char *argument )
{
	char	 buf [MSL];

	if ( !IS_OUTSIDE(ch) )
	{
		send_to_char( "Non puoi mica vedere il tempo in un luogo chiuso..\n\r", ch );
		return;
	}
	send_to_char( "\n\r", ch );

	if ( weather_info.sky == SKY_RAINING )
	{
		int num = number_range( 0, 100 );

		if		( num <= 30 )	send_to_char( "Ticchetta al suolo una fitta pioggia fredda..", ch );
		else if ( num <= 60 )	send_to_char( "Comincia a scendere una fitta pioggia tiepida..", ch );
		else if ( num <= 100 )	send_to_char( "Un vento pungente porta gocce tiepide nel suo vortice..\n\r", ch );

		send_to_char( "\n\r", ch );
	}
	else if ( weather_info.sky == SKY_CLOUDY )
	{
		sprintf( buf, "%s.\n\r",
			weather_info.change >= 0 ?
			"S'alza una leggera brezza tiepida.." :
			"Soffia un vento freddo." );
			send_to_char( buf, ch );
	}
	else if ( weather_info.sky == SKY_CLOUDLESS )		send_to_char( "Il cielo e' terso: non una nuvola ad oscurarlo.\n\r", ch );
	else if ( weather_info.sky == SKY_THUNDERSTORM )	send_to_char( "Il cielo s'adombra all'improvviso: si prepara una tempesta.\n\r", ch );
	else if ( weather_info.sky == SKY_ICESTORM )		send_to_char( "Grossi chicchi di ghiaccio precipitano violentemente dal cielo.\n\r", ch );
	else if ( weather_info.sky == SKY_HAILSTORM )		send_to_char( "Una fitta grandine comincia a venir giu' dal cielo.\n\r", ch );
	else if ( weather_info.sky == SKY_SNOWING )			send_to_char( "Cade silente dal cielo una morbida neve..\n\r", ch );
	else if ( weather_info.sky == SKY_BLIZZARD )		send_to_char( "La neve incalza: e' bufera..\n\r", ch );
	else if ( weather_info.sky == SKY_FOGGY )			send_to_char( "Una densa foschia copre l'orizzonte.\n\r", ch );
	else if ( weather_info.sky == SKY_LIGHTNING )		send_to_char( "Lampi squarciano il cielo: s'avvicina un temporale.\n\r", ch );
	else												send_to_char( "BUG! Non esiste descrizione per questo tipo di cielo.\n\r", ch );
}


void do_autoweather( CHAR_DATA *ch, char *argument )
{
	if ( IS_MOB(ch) )
		return;

	if ( IS_SET(ch->act, PLR_AUTOWEATHER) )
	{
		send_to_char( "Non visualizzerai piu' le informazioni metereologiche.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_AUTOWEATHER );
	}
	else
	{
		send_to_char( "Ora potrai visualizzare le informazioni metereologiche nelle locazioni all'aperto.\n\r", ch );
		SET_BIT( ch->act, PLR_AUTOWEATHER );
	}
}


void show_weather( CHAR_DATA *ch )
{
	char buf[MSL];

	/*for weather stuff -- tjw*/
	if ( weather_info.sky == SKY_RAINING )
	{
		int num = number_range( 0, 100 );

		/* (bb) questi msg non sono coordinati con il comandoi do_weather, e quindi possono mandarne di differenti */
		if		( num <= 30  )	send_to_char( "Precipita dal cielo una fitta pioggia fredda.\n\r", ch );
		else if ( num <= 60  )	send_to_char( "Scende dal cielo una pioggia tiepida..\n\r", ch );
		else if ( num <= 100 )	send_to_char( "Lentamente comincia una leggera pioggia calda..\n\r", ch );

		send_to_char( "\n\r", ch );
	}
	else if ( weather_info.sky == SKY_CLOUDY )
	{
		sprintf( buf, "%s.\n\r",
			weather_info.change >= 0 ?
			"Una tiepida brezza avvolge l'aria.." :
			"Una fredda brezza frusta l'aria.." );
		send_to_char( buf, ch );
	}
	else if ( weather_info.sky == SKY_CLOUDLESS )		send_to_char( "Il cielo e' terso: non una nuvola ad oscurarlo.\n\r", ch );
	else if ( weather_info.sky == SKY_THUNDERSTORM )	send_to_char( "Il cielo s'adombra all'improvviso: si prepara una tempesta.\n\r", ch );
	else if ( weather_info.sky == SKY_ICESTORM )		send_to_char( "Grossi chicchi di ghiaccio precipitano violentemente dal cielo.\n\r", ch );
	else if ( weather_info.sky == SKY_HAILSTORM )		send_to_char( "Una fitta grandine comincia a venir giu' dal cielo.\n\r", ch );
	else if ( weather_info.sky == SKY_SNOWING )			send_to_char( "Cade silente dal cielo una morbida neve..\n\r", ch );
	else if ( weather_info.sky == SKY_BLIZZARD )		send_to_char( "La neve incalza: e' bufera..\n\r", ch );
	else if ( weather_info.sky == SKY_FOGGY )			send_to_char( "Una densa foschia copre l'orizzonte.\n\r", ch );
	else if ( weather_info.sky == SKY_LIGHTNING )		send_to_char( "Lampi squarciano il cielo: s'avvicina un temporale.\n\r", ch );
	else												send_to_char( "BUG! Non esiste descrizione per questo tipo di cielo.\n\r", ch );
}


void lightning( void )
{
	DESCRIPTOR_DATA *d;
	for ( d = descriptor_list;  d != NULL;  d = d->next )
	{
		if ( d->connected == CON_PLAYING
		  && IS_OUTSIDE( d->character )
		  && IS_AWAKE  ( d->character )
		  && number_chance(10)
		  && !IS_ADMIN(d->character)
		  && d->character->level > 17	/* (RR) farlo con i livelli di hardore? */
		  && weather_info.sky == SKY_LIGHTNING )
		{
			send_to_char( "Un fascio di luce accecante squarcia il cielo per un istante!\n\r", d->character );
			act( "$n e' stato colpito da un lampo!", d->character, NULL, NULL, TO_ROOM, POS_REST );
			if ( check_immune(d->character, DAM_LIGHTNING) != IS_IMMUNE )
			{
				if ( d->character->fighting )
					stop_fighting( d->character, TRUE );
				if ( check_immune(d->character, DAM_LIGHTNING) != IS_RESISTANT )
				{
					if ( d->character->level < LI1 );
						d->character->point[LIFE] -= d->character->point[LIFE]/25;
				}
				if ( d->character->level < LI2 );
					d->character->point[LIFE] -= d->character->point[LIFE]/20;
				if ( d->character->level < LI3 );
					d->character->point[LIFE] -= d->character->point[LIFE]/15;

				WAIT_STATE( d->character, 40 );
			}
			else
			{
				if ( check_immune(d->character,DAM_LIGHTNING) == IS_VULNERABLE )
				{
					d->character->point[LIFE] -= d->character->point[LIFE]/10;
					WAIT_STATE( d->character, 40 );
				}
			}
		}
	}
}


void blizzard( void )
{
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING
		  && IS_OUTSIDE(d->character)
		  && weather_info.sky == SKY_BLIZZARD )
		{
			if ( number_range(0, 2000) >= 1500 )
				send_to_char( "Il cielo e' coperto di grossi nuvoloni gonfi.. Sara' meglio cercare un riparo..\n\r", d->character );
			else if ( number_range(0, 2000) >= 1250 )
			{
				if ( d->character->in_room->sector_type == SECT_FIELD
				||	 d->character->in_room->sector_type == SECT_FOREST
				||	 d->character->in_room->sector_type == SECT_HILLS
				||	 d->character->in_room->sector_type == SECT_MOUNTAIN )
				{
					send_to_char( "Inciampi e caschi in una buca! Maledetta tormenta!\n\r", d->character );
					act( "$n inciampa e casca in una buca.", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				else
				{
					send_to_char( "Scivoli sulla neve cascando a terra!\n\r", d->character );
					act( "$n scivola sulla neve cascando per terra.", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				d->character->point[LIFE] -= d->character->level / 2;
			}
			else if ( number_range(0, 2000) >= 1000 )
			{
				if ( d->character->in_room->sector_type == SECT_FOREST )
				{
					send_to_char( "Sei finito nel cavo di un tronco d'albero.. Speriamo almeno non ti sia fratturato nulla!\n\r", d->character );
					act( "$n si e' piantato a capofitto in un albero!", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				else
				{
					send_to_char( "Inciampa nella spessa neve cascando a terra!\n\r", d->character );
					act( "$n inciampa nella spessa neve cascando per terra.", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				d->character->point[LIFE] -= d->character->level;
			}
			else if ( number_range(0, 2000) >= 250 )
				send_to_char( "Il cielo e' coperto di grossi nuvoloni gonfi.. Sara' meglio cercare un riparo..\n\r", d->character );
			else if ( number_range(0, 2000) >= 50 )
			{
				send_to_char( "Il tuo corpo e' scosso dai brividi e le tue membra cominciano lentamente a gelare..\n\r", d->character );
				act( "Un velo di ghiaccio ricopre lentamente il corpo di $n..", d->character, NULL, NULL, TO_ROOM, POS_REST );
				d->character->point[LIFE] -= d->character->level * 2;
			}
		}
	}
}


void ice( void )
{
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING
		  && IS_OUTSIDE(d->character)
		  && weather_info.sky == SKY_ICESTORM )
		{
			if ( number_range(0, 2000) >= 1500 )
				send_to_char( "Comincia a grandinare. Meglio mettersi al riparo.\n\r", d->character );
			else if ( number_range(0, 2000) >= 1250 )
			{
				send_to_char( "Scivoli sul ghiaccio ammassato in terra.. E caschi!\n\r", d->character );
				act( "$n scivola sul ghiaccio ammassato in terra.. E casca!", d->character, NULL, NULL, TO_ROOM, POS_REST );
				d->character->point[LIFE] -= d->character->level/2;
			}
			else if ( number_range(0, 2000) >= 1000 )
			{
				if ( d->character->in_room->sector_type == SECT_FOREST )
				{
					send_to_char( "Sei finito nel cavo di un tronco d'albero.. Speriamo almeno non ti sia fratturato nulla!\n\r", d->character );
					act( "$n si e' piantato a capofitto in un albero!", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				else
				{
					send_to_char( "Scivoli maldestramente sul ghiaccio ammassato in terra.. E caschi!\n\r", d->character );
					act( "$n scivola maldestramente sul ghiaccio ammassato in terra.. E casca!", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				d->character->point[LIFE] -= d->character->level;
			}
			else if ( number_range(0, 2000) >= 250 )
				send_to_char( "Il cielo e' coperto di grossi nuvoloni gonfi.. Sara' meglio cercare un riparo..\n\r", d->character );
			else if ( number_range(0, 2000) >= 50 )
			{
				send_to_char( "Il tuo corpo e' scosso dai brividi e le tue membra cominciano lentamente a gelare..\n\r", d->character );
				act( "Un velo di ghiaccio ricopre lentamente il corpo di $n..", d->character, NULL, NULL, TO_ROOM, POS_REST );
				d->character->point[LIFE] -= d->character->level * 2;
			}
		}
	}
}


void hail( void )
{
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING
		  && IS_OUTSIDE(d->character)
		  && weather_info.sky == SKY_HAILSTORM )
		{
			if		( number_range(0, 2000) >= 1500 )
				send_to_char( "Perle di ghiaccio turbinano nell'aria.. Trovati un riparo.\n\r", d->character );
			else if ( number_range(0, 2000) >= 1250 )
			{
				send_to_char( "Accidenti! Un grosso chicco di grandine ti ha colpito in viso!\n\r", d->character );
				act( "$n e' stato appena colpito in viso da un grosso chicco di grandine!", d->character, NULL, NULL, TO_ROOM, POS_REST );
				d->character->point[LIFE] -= d->character->level / 2;
			}
/* (bb) qui per stare in linea con gli altri dovrebbe avere un number_range( 0, 2000 ) >= 1000 */
			else if ( number_range(0, 2000) >= 250 )
				send_to_char( "Il cielo e' coperto di grossi nuvoloni gonfi.. Sara' meglio cercare un riparo..\n\r", d->character );
			else if ( number_range(0, 2000) >= 50 )
			{
				send_to_char( "Il tuo corpo e' scosso dai brividi e le tue membra cominciano lentamente a gelare..\n\r", d->character );
				act( "Un velo di ghiaccio ricopre lentamente il corpo di $n..", d->character, NULL, NULL, TO_ROOM, POS_REST );
				d->character->point[LIFE] -= d->character->level * 2;
			}
		}
	}
}


void fog( void )
{
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( d->connected == CON_PLAYING
		  && IS_OUTSIDE(d->character)
		  && weather_info.sky == SKY_FOGGY )
		{
			if ( number_range(0, 2000) >= 1500 )
				send_to_char( "La nebbia mattutina e' densa e pesante..\n\r", d->character );
			else if ( number_range(0, 2000) >= 1250 )
			{
				if ( d->character->in_room->sector_type == SECT_FIELD
				||	 d->character->in_room->sector_type == SECT_FOREST
				||	 d->character->in_room->sector_type == SECT_HILLS
				||	 d->character->in_room->sector_type == SECT_MOUNTAIN )
				{
					send_to_char( "Inciampi.. E sei cascato in un buco! Che nebbione!\n\r",d->character );
					act( "$n inciampa e casca in un buco.", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				else
				{
					send_to_char( "Inciampi.. E cadi! Che nebbione!\n\r",d->character );
					act( "$n inciampa e casca per terra.", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				d->character->point[LIFE] -= d->character->level / 2;
			}
			else if ( number_range(0, 2000) >= 1000 )
			{
				if ( d->character->in_room->sector_type == SECT_FOREST )
				{
					send_to_char( "Sei finito nel cavo di un tronco d'albero.. Dannata nebbia!\n\r", d->character );
					act( "$n si e' piantato a capofitto in un albero!", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				else
				{
					send_to_char( "Inciampi.. E cadi! Dannata nebbia!\n\r",d->character );
					act( "$n inciampa e casca per terra.", d->character, NULL, NULL, TO_ROOM, POS_REST );
				}
				d->character->point[LIFE] -= d->character->level;
			}
			else if ( number_range(0, 2000) >= 250 )
				send_to_char( "La nebbia mattutina e' densa e pesante..\n\r", d->character );
			else if ( number_range(0, 2000) >= 50 )
			{
				send_to_char( "Scivoli maldestramente e.. Ops! Finisci in acqua!\n\r", d->character );
				act( "$n e' finito in acqua!", d->character, NULL, NULL, TO_ROOM, POS_REST );
				d->character->point[LIFE] -= d->character->level * 2;
			}
		}
	}
}


bool number_chance( int num )
{
	if ( number_range(1, 100) <= num )
		return TRUE;
	else
		return FALSE;
}



void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( !str_cmp(target_name, "better") || !str_cmp(target_name, "migliore") )
	{
		if ( weather_info.sky == SKY_CLOUDLESS )
			send_to_char( "Ma fuori e' gia' bel tempo!\n\r", ch );
		else if ( weather_info.sky == SKY_CLOUDY )
		{
			send_to_char( "Reciti un antico incantamento e le nubi lentamente si dipartono, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e le nubi lentamente si dipartono, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_CLOUDLESS;
		}
		else if ( weather_info.sky == SKY_RAINING )
		{
			send_to_char( "Reciti un antico incantamento e la pioggia lentamente scema, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e la pioggia lentamente scema, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_CLOUDY;
		}
		else if ( weather_info.sky == SKY_LIGHTNING )
		{
			send_to_char( "Reciti un antico incantamento e i lampi rapidamente s'arrestano, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e i lampi rapidamente s'arrestano, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_THUNDERSTORM;
		}
		else if ( weather_info.sky == SKY_THUNDERSTORM )
		{
			send_to_char( "Reciti un antico incantamento e rapidamente cessa la tempesta, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e rapidamente cessa la tempesta, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_RAINING;
		}
		else if ( weather_info.sky == SKY_SNOWING )
		{
			send_to_char( "Reciti un antico incantamento e la neve lentamente svanisce, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e la neve lentamente svanisce, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_CLOUDY;
		}
		else if ( weather_info.sky == SKY_BLIZZARD )
		{
			send_to_char( "Reciti un antico incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_SNOWING;
		}
		else if ( weather_info.sky == SKY_ICESTORM )
		{
			send_to_char( "Reciti un antico incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_RAINING;
		}
		else if ( weather_info.sky == SKY_HAILSTORM )
		{
			send_to_char( "Reciti un antico incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_RAINING;
		}
		else if ( weather_info.sky == SKY_FOGGY )
		{
			send_to_char( "Reciti un antico incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e l'orizzonte lentamente si rischiara, in ossequiosa obbedienza..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_CLOUDY;
		}
		else
		{
			send_to_char( "spell_control_weather: bug, da notificare agli amministratori.\n\r", ch );
			return;
		}
	}
	else if ( !str_cmp(target_name, "worse") || !str_cmp(target_name, "peggiore") )
	{
		if ( weather_info.sky == SKY_CLOUDLESS )
		{
			send_to_char( "Reciti un antico incantamento e rapide nuvole s'addensano, al tuo comando..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e rapide le nuvole s'addensano, al suo comando..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_CLOUDY;
		}
		else if ( weather_info.sky == SKY_CLOUDY )
		{
			send_to_char( "Reciti un antico incantamento e rapida scende la pioggia, al tuo comando..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e rapida scende la pioggia, al suo comando..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_RAINING;
		}
		else if ( weather_info.sky == SKY_RAINING )
		{
			send_to_char( "Reciti un antico incantamento e lentamente la pioggia muta in grandine, al tuo comando..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e lentamente la pioggia muta in grandine, al suo comando..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_HAILSTORM;
		}
		else if ( weather_info.sky == SKY_LIGHTNING )
		{
			send_to_char( "Reciti un antico incantamento e rapida scende la grandine, al tuo comando..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e rapida scende la grandine, al suo comando..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_ICESTORM;
		}
		else if ( weather_info.sky == SKY_THUNDERSTORM )
		{
			send_to_char( "Reciti un antico incantamento e le nuvole esplodono in fragorosi tuoni, al tuo comando..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e le nuvole esplodono in fragorosi tuoni, al suo comando..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_LIGHTNING;
		}
		else if ( weather_info.sky == SKY_SNOWING )
		{
			send_to_char( "Reciti un antico incantamento e rapida la neve incalza, al tuo comando..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e rapida la neve incalza, al suo comando..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_BLIZZARD;
		}
		else if ( weather_info.sky == SKY_BLIZZARD )
			send_to_char( "Ma e' gia' cattivo tempo..\n\r", ch );
		else if ( weather_info.sky == SKY_ICESTORM )
			send_to_char( "Ma e' gia' cattivo tempo..\n\r", ch );
		else if ( weather_info.sky == SKY_HAILSTORM )
			send_to_char( "Ma e' gia' cattivo tempo..\n\r", ch );
		else if ( weather_info.sky == SKY_FOGGY )
		{
			send_to_char( "Reciti un antico incantamento e l'orizzonte lentamente si rischiara, al tuo comando..\n\r", ch );
			act( "$n gesticola bisbigliando un arcano incantamento e l'orizzonte lentamente si rischiara, al suo comando..", ch, NULL, NULL, TO_ROOM, POS_REST );
			weather_info.sky = SKY_CLOUDY;
		}
		else
		{
			send_to_char( "spell_control_weather: bug, da notificare agli amministratori.\n\r", ch );
			return;
		}
	}
	else
	{
		send_to_char( "Vuoi che il tempo sia migliore o peggiore?\n\r", ch );
		return;
	}

	send_to_char( "Ok.\n\r", ch );
}


void show_syntax_wset( CHAR_DATA *ch )
{
	send_to_char( "Syntax:\n\r", ch );
	send_to_char( "  set weather <condizione> \n\r", ch );
	send_to_char( "  Dove condizione puo' essere :\n\r", ch );
	send_to_char( "  hail, fog, icestorm, blizzard, snowing, rain\n\r", ch );
	send_to_char( "  lightning, thunderstorm, cloudless, cloudy\n\r", ch );
}


void do_wset( CHAR_DATA *ch, char *argument )
{
	char arg1 [MIL];
	argument = one_argument( argument, arg1 );

	if ( arg1[0] == '\0' )
		show_syntax_wset( ch );

	if ( !str_cmp(arg1,"cloudless") )
	{
		send_to_char( "Batti le mani e, al tuo comando, le nuvole cominciano a dipanarsi.. \n\r", ch );
		weather_info.sky = SKY_CLOUDLESS;
		act( "$n batte le mani e, al suo comando, le nuvole cominciano a dipanarsi..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if( !str_cmp(arg1,"fog") )
	{
		send_to_char( "Batti le mani e, al tuo comando, una nebbia densa avvolge l'orizzonte..\n\r", ch);
		weather_info.sky = SKY_FOGGY;
		act( "$n batte le mani e, al suo comando, una nebbia densa avvolge l'orizzonte..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if( !str_cmp(arg1,"hail") )
	{
		send_to_char( "Batti le mani e, al tuo comando, comincia a grandinare notevolmente..\n\r", ch );
		weather_info.sky = SKY_HAILSTORM;
		act( "$n batte le mani e, al suo comando, comincia a grandinare notevolmente..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if (!str_cmp(arg1,"icestorm") )
	{
		send_to_char( "Batti le mani e, al tuo comando, dal cielo precipita una pioggia di ghiaccio..\n\r", ch );
		weather_info.sky = SKY_ICESTORM;
		act( "$n batte le mani e, al suo comando, dal cielo precipita una pioggia di ghiaccio..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if ( !str_cmp(arg1,"blizzard") )
	{
		send_to_char( "Batti le mani e, al tuo comando, vortica rapida una bufera di neve..\n\r", ch );
		weather_info.sky = SKY_BLIZZARD;
		act( "$n batti le mani e, al suo comando, vortica rapida una bufera di neve..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if ( !str_cmp(arg1,"snowing") )
	{
		send_to_char( "Batti le mani e, al tuo comando, una soffice neve comincia a cadere dal cielo..\n\r", ch );
		weather_info.sky = SKY_SNOWING;
		act( "$n batte le mani e, al suo comando, una soffice neve comincia a cadere dal cielo..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if ( !str_cmp(arg1,"rain") )
	{
		send_to_char( "Batti le mani e, al tuo comando, scende rapida una fitta pioggia tiepida..\n\r", ch );
		weather_info.sky = SKY_RAINING;
		act( "$n batte le mani e, al suo comando, scende rapida una fitta pioggia tiepida..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if ( !str_cmp(arg1,"lightning") )
	{
		send_to_char( "Batti le mani e, al tuo comando, il cielo si rischiara di fulgidi lampi..\n\r", ch );
		weather_info.sky = SKY_LIGHTNING;
		act( "$n batte le mani e, al suo comando, il cielo si rischiara di fulgidi lampi..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if ( !str_cmp(arg1,"thunderstorm") )
	{
		send_to_char( "Batti le mani e, al tuo comando, le nuvole esplodono in fragorosi tuoni..\n\r", ch );
		weather_info.sky = SKY_THUNDERSTORM;
		act( "$n batte le mani e, al suo comando, le nuvole esplodono in fragorosi tuoni..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	else if ( !str_cmp(arg1,"cloudy") )
	{
		send_to_char( "Batti le mani e, al tuo comando, nuvolone gonfie di pioggia coprono l'orizzonte..\n\r", ch);
		weather_info.sky = SKY_CLOUDY;
		act( "$n batte le mani e, al suo comando, nuvole gonfie di pioggia coprono l'orizzonte..", ch, NULL, NULL, TO_ROOM, POS_REST );
	}
	/* okay, we haven't met any conditions so far. Did they forget? */
	else
		show_syntax_wset( ch );
}

#endif
