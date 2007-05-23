/*                           Marriage Code v1.3
 *                                By Cid
 *                          eos.execnet.com:1234
 *
 * Unlike my other codes this one does require the addition of another pcdata
 * field.  Like spouse.  If you add it spouse is a string not an integer.
 * So thats how it works.  Plus you'll have to modify db.c save.c to load up
 * the spouse string so this will work.   This snippet also includes a rings
 * command.  It'll create the rings for the married couple.  Which you'll have
 * to define in merc.h OBJ_VNUM_DIAMOND_RING and OBJ_VNUM_WEDDING_BAND to vnums
 * that you might have (ofcourse I created 2 rings for this then added in the
 * marriage code).  The rings command would be the easiest to add in cause of
 * just putting in 2 lines in merc.h... even though its a lot easier then the
 * marriage, and divorce code... but it needs the spouse field(checks to see if
 * they are married).   Well enjoy... btw dont forget to put somewhere in your
 * code to give me credits or if you just pop in this file... Keep the this header
 * intact.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xandra.h"
#include "recycle.h"


void do_marry( CHAR_DATA *ch, char *argument)
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	CHAR_DATA	*victim2;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !arg1 || arg1[0] == '\0' || !arg2 || arg2[0] == '\0' )
	{
		send_to_char( "Sintassi: marry <pg1> <pg2>\n\r", ch );
		return;
	}

	if ( ((victim  = get_char_world(ch,arg1)) == NULL)
	||	 ((victim2 = get_char_world(ch,arg2)) == NULL) )
	{
		send_to_char( "Entrambi i personaggi devono essere in gioco!\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) || IS_MOB(victim2))
	{
		send_to_char( "I mobile non possono maritarsi!\n\r", ch );
		return;
	}

	if ( victim->pcdata->spouse[0] == '\0' && victim2->pcdata->spouse[0] == '\0' )
	{
		send_to_char( "You pronounce them man and wife!\n\r", ch );

		send_to_char( "You say the big 'I do.'\n\r", victim );
		send_to_char( "You say the big 'I do.'\n\r", victim2 );

		act( "$n and $N are now declared married!\n\r", victim, NULL, victim2, TO_ROOM, POS_REST );

		victim->pcdata->spouse  = str_dup( victim2->name );
		victim2->pcdata->spouse = str_dup( victim->name );
	}
	else
		send_to_char( "Sono gia' maritati!\n\r", ch );
}


void do_divorce( CHAR_DATA *ch, char *argument)
{
	char		arg1[MIL];
	char		arg2[MIL];
	CHAR_DATA	*victim;
	CHAR_DATA	*victim2;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Sintassi: divorce <pg1> <pg2>\n\r",ch );
		return;
	}

	if ( ((victim = get_char_world(ch,arg1)) == NULL)
	||	 ((victim2 = get_char_world(ch,arg2)) == NULL) )
	{
		send_to_char( "Entrambi i personaggi devono essere in gioco!\n\r", ch );
		return;
	}

	if ( IS_MOB(victim) || IS_MOB(victim2) )
	{
		send_to_char( "Non penso che i mobile possano essere maritati!\n\r", ch );
		return;
	}

	if (victim->pcdata->spouse != victim2->name || victim2->pcdata->spouse != victim->name)
	{
		send_to_char( "Non sono maritati tra di loro!!\n\r", ch );
		return;
	}

	if ( !str_cmp(victim->pcdata->spouse, victim2->name) && !str_cmp(victim2->pcdata->spouse, victim->name) )
	{
		send_to_char( "You hand them their papers.\n\r", ch );
		send_to_char( "Your divorce is final.\n\r", victim );
		send_to_char( "Your divorce is final.\n\r", victim2 );

		act( "$n and $N swap divorce papers, they are no-longer married.",
			victim, NULL, victim2, TO_NOTVICT, POS_REST );

		free_string( victim->pcdata->spouse );
		free_string( victim2->pcdata->spouse );
		victim->pcdata->spouse = str_dup( "" );
		victim2->pcdata->spouse = str_dup( "" );
	}
	else
		send_to_char( "Non sono maritati!", ch );
}


void do_rings( CHAR_DATA *ch, char *argument )
{
	char			 arg1[MIL];
	char			 arg2[MIL];
	CHAR_DATA		 *victim;
	CHAR_DATA		 *victim2;
	char			 buf  [ MSL ];
	OBJ_DATA		 *ring;
	EXTRA_DESCR_DATA *ed;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1 == '\0' || arg2 == '\0' )
	{
		send_to_char( "Sintassi: rings <pg1> <pg2>", ch );
		return;
	}

	if ( ((victim  = get_char_world(ch, arg1)) == NULL)
	||	 ((victim2 = get_char_world(ch, arg2)) == NULL) )
	{
		send_to_char( "Entrambi i personaggi devono essere in gioco!\n\r", ch );
		return;
	}

	if ( (victim->pcdata->spouse != victim2->name) || (victim2->pcdata->spouse != victim->name) )
	{
		send_to_char( "Non sono maritati tra di loro!!\n\r", ch );
		return;
	}

	switch ( victim2->sex )
	{
	  case SEX_FEMALE:
	  {
		ring = create_object( get_obj_index(OBJ_VNUM_DIAMOND_RING), 0 );

		switch ( victim->sex )
		{
		  case SEX_MALE:
		  case SEX_NEUTRAL:
		  default:
		  {
			sprintf( buf, "This is the beautiful diamond ring given to you by your handsome\n\rhusband %s at your wedding. It signifies your eternal love for eachother.\n\r", victim->name );
			ring->description = str_dup( buf );
			break;
		  }
		  case SEX_FEMALE:
		  {
			sprintf( buf, "This is the beautiful diamond ring given to you by your lovely\n\rwife %s at your wedding. It signifies your eternal love for eachother.\n\r", victim->name );
			ring->description = str_dup( buf );
			break;
		  }
		}

		ed = new_extra_descr();

		ed->keyword = str_dup( "inscription" );
		sprintf( buf, "The inscription reads:\n\rTo my lovely wife, yours forever, %s\n\r", victim->name );
		ed->description = str_dup( buf );
		ed->next = ring->extra_descr;
		ring->extra_descr = ed;
		break;
	  }	/* chiude il case SEX_FEMALE */
	  case SEX_MALE:
	  case SEX_NEUTRAL:
	  default:
	  {
		ring = create_object( get_obj_index(OBJ_VNUM_WEDDING_BAND), 0 );

		switch( victim->sex )
		{
		  case SEX_MALE:
		  case SEX_NEUTRAL:
		  default:
		  {
			sprintf( buf, "This is the ring given to you by your handsome husband %s\n\rat your wedding. It signifies your eternal love for eachother.\n\r", victim->name );
			ring->description = str_dup( buf );
			break;
		  }
		  case SEX_FEMALE:
		  {
			sprintf( buf, "This is the ring given to you by your beautifull wife %s\n\rat your wedding. It signifies your eternal love for eachother.\n\r", victim->name );
			ring->description = str_dup( buf );
			break;
		  }
		}

		ed = new_extra_descr();

		ed->keyword = str_dup( "inscription" );
		ed->next = ring->extra_descr;
		ring->extra_descr = ed;


		switch( victim->sex )
		{
		  default:
		  case SEX_MALE:
		  {
			sprintf( buf, "The inscription reads:\n\rTo my handsome husband.. Forever yours, %s\n\r", victim->name );
			ed->description = str_dup( buf );
			break;
		  }
		  case SEX_NEUTRAL:
		  {
			sprintf( buf,"The inscription reads:\n\rForever love, %s\n\r", victim->name );
			ed->description = str_dup( buf );
			break;
		  }
		}
	  } /* chiude il case default */
	} /* chiude lo switch */

	obj_to_char ( ring, victim );
}
