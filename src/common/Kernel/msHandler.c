/*

  Copyright � Grame 1999

  This library is free software; you can redistribute it and modify it under 
  the terms of the GNU Library General Public License as published by the 
  Free Software Foundation version 2 of the License, or any later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License 
  for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Grame Research Laboratory, 9, rue du Garet 69001 Lyon - France
  grame@rd.grame.fr
  
  modifications history:
   [08-09-99] DF - adaptation to the new memory management
   [13-09-99] DF - removing direct fifo access in ClockHandler

*/

#include "msEvents.h"
#include "msKernel.h"
#include "msSorter.h"
#include "msExtern.h"


/*===========================================================================
  Internal functions prototypes
  =========================================================================== */
static void Accept			( TMSGlobalPtr g, TApplPtr appl, MidiEvPtr ev);
static void Propose		    ( TMSGlobalPtr g, TApplPtr appl, MidiEvPtr ev);
static void DispatchEvents	( TMSGlobalPtr g, MidiEvPtr ev);
static void RcvAlarmLoop	( TMSGlobalPtr g);


/*===========================================================================
  External functions implementation
  ===========================================================================*/ 
void ClockHandler (TMSGlobalPtr g)
{
	 THorlogePtr h; TDatedEvPtr e;
	 MidiEvPtr ready;

	 h= &g->currTime;
	 if( ++h->reenterCount) {
		 return;
	 }

	 do {
		 h->time++;
		 e = (TDatedEvPtr)fifoclear (SorterList(g));
		 ready = (MidiEvPtr)SorterClock(Sorter(g), e);
		 if( ready) {
		 	NextActiveAppl(g) = ActiveAppl(g);
		 	DispatchEvents(g, ready);
		 	RcvAlarmLoop(g);
		 }

	}while( h->reenterCount--);
}



/*===========================================================================
  Internal functions implementation
  ===========================================================================*/ 
static void Accept( TMSGlobalPtr g, TApplPtr appl, MidiEvPtr ev)

{
	if( appl->refNum ) {
		fifoput (&appl->rcv, (cell *)ev);
		if( !++appl->rcvFlag) *NextActiveAppl(g)++ = appl;
	}
	else { 
		/* refnum 0 is MidiShare refnum */
		/* event should be handled by the port manager */
		SendToDriver (Driver(g), ev);
		MSFreeEv( ev, FreeList(Memory(g)));
	}
}

/*__________________________________________________________________________________*/
static void Propose( TMSGlobalPtr g, TApplPtr appl, MidiEvPtr ev) 

{
	MidiEvPtr copy;
	MidiFilterPtr filter;
	Byte type, canal;

	filter = appl->filter;
	type = EvType(ev);
	canal = Canal(ev)&0x0f;
	if (!filter
		|| ((IsAcceptedBit( filter->port, Port(ev)) && IsAcceptedBit( filter->evType, type))
			&& ((type > typePitchWheel) || IsAcceptedBit( filter->channel, canal)))) {
			copy= MSCopyEv( ev, FreeList(Memory(g)));
			if( copy)
				Accept( g, appl, copy);
	}
}

/*__________________________________________________________________________________*/
static void ProcessCall( TApplPtr appl, MidiEvPtr ev)
{
	CallTaskCode (appl->context, ev);
}

/*__________________________________________________________________________________*/
static inline void AcceptTask (TApplPtr appl, MidiEvPtr ev)
{
	fifoput (&appl->dTasks, (cell *)ev);
}

/*__________________________________________________________________________________*/
/*	DispatchEvents								*/
/*__________________________________________________________________________________*/
static void DispatchEvents (TMSGlobalPtr g, MidiEvPtr ev)
{
	MSMemoryPtr mem = Memory(g);
	MidiEvPtr next;
	short refNum; Byte type;

	do {
		type= EvType(ev);
		next= Link(ev);
		if( type >= typeReserved) {			/* typeReserved : event is ignored  */
			MSFreeEv( ev, FreeList(mem));	/* free the event  					*/
			ev= next;						/* next event						*/
			continue;						/* and loop							*/
		}
		
		refNum= RefNum(ev) & 0x7F;              /* unflag the refnum number     */
		if( CheckRefNum(Clients(g), refNum)) {  /* check if refnum is valid     */
			TApplPtr appl = Appls(g)[refNum];   /* get the corresponding client */

			if( (type >= typeQuarterFrame) || (type < typePrivate)) {
				if( RefNum(ev) & 0x80)	{	        /* refnum is marked			*/
					Accept( g, appl, ev);		    /* event is for appl itself */
				}
				else {					        	/* standard event received  */
					TConnectionPtr cnx;
					cnx= appl->dstList;		        /* get the connections list */
					while( cnx) {
						Propose( g,cnx->itsDst,ev);	/* propose to each connected appl */
						cnx= cnx->nextDst;		    /* loop thru the connection list  */
					}
					MSFreeEv( ev, FreeList(mem));   /* and free the event */
				}
			}
			else if (type == typeProcess) {		/* event is a realtime task		*/
				ProcessCall( appl, ev);		    /* execute the task				*/
#ifdef __Linux__
				MSFreeEv( ev, FreeList(mem));
#endif
			}
			else if (type == typeDProcess) {    /* typeDProcess : defered task	*/
				AcceptTask(appl, ev);    		/* store in the appl dtasks fifo*/
			}
			else if( type >= typePrivate) {     /* it's a private event 	*/
				Accept( g, appl, ev);           /* event is for appl itself */
			}
			else MSFreeEv( ev, FreeList(mem));
		}
		else MSFreeEv( ev, FreeList(mem));  /* free the event       */
		ev= next;                 /* go to the next event */
	} while( ev);                 /* and loop             */
}

/*__________________________________________________________________________________*/
/*	- RcvAlarmLoop - go thru the applications list and when necessary               */
/*	call the rcvAlarm                                                               */
/*__________________________________________________________________________________*/
static void RcvAlarmLoop( TMSGlobalPtr g)
{
	TApplPtr FAR *applPtr, appl;
	RcvAlarmPtr alarm;

	*NextActiveAppl(g) = 0;               /* close the active applications table */
	applPtr= ActiveAppl(g);              /* starts at the table beginning       */
	while( *applPtr) {
		appl= *applPtr++;
		appl->rcvFlag= (uchar)kNoRcvFlag;	/* unmark the application    */
		alarm= appl->rcvAlarm;              /* get the application alarm */
		if( alarm) {                        /* when the alarm exists     */
			CallRcvAlarm (appl->context, alarm, appl->refNum);
		}
	}

}
