/*
  MidiShare Project
  Copyright (C) Grame 2001
  Copyright (C) Mil Productions 2001

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr

*/

#if !macintosh
#include <stdlib.h>
#include <stdio.h>
#endif

#include "TRemoteMgr.h"
#include "TMidiClient.h"

//____________________________________________________________
// UDP Remote management
//____________________________________________________________
TRemoteMgr::TRemoteMgr ()
{
	UDPStreamInitMthTbl (fMethods.outMethods);
	UDPParseInitMthTbl  (fMethods.inMethods);
}

//____________________________________________________________
TMidiRemote * TRemoteMgr::AddRemote (UPDParamsPtr params, RemoteCtrlParams * rp, short drvRef)
{
	rp->mth = &fMethods;
	TMidiRemote * r = new TMidiRemote (params, rp, drvRef);
	if (r) {
		RemotePtr cl = Add (r);
		if (cl) {
			cl->timeOut = MidiGetTime () + kInitTTL;
			return r;
		}
		delete r;
	}
	return 0;
}

//____________________________________________________________
short TRemoteMgr::RemoteCount ()
{ 
	short count = 0;
	RemotePtr r = Head();
	while (r) {
		TMidiRemote * m = (TMidiRemote *)r->remote;
		if (m->Opened()) count++;
		r = r->next;
	}
	return count; 
};

//____________________________________________________________
void TRemoteMgr::RcvAlarmLoop (short refNum)
{
	RemotePtr r = Head();
	while (r) {
		TMidiRemote * m = (TMidiRemote *)r->remote;
		if (m->CallRcvAlarm()) {
			m->RcvAlarm (refNum);
		}
		r = r->next;
	}
}				

//____________________________________________________________
void TRemoteMgr::SetMode (short mode, short refNum)
{
	RemotePtr r = Head();
	while (r) {
		TMidiRemote * m = (TMidiRemote *)r->remote;
		m->SetMode (mode, refNum);
		r = r->next;
	}
}

//____________________________________________________________
void TRemoteMgr::CheckTimeOut (unsigned long date)
{
	RemotePtr r = Head();
	RemotePtr prev = 0, next;
	while (r) {
		next = r->next;
		if (r->timeOut < date) {
			Remove (r->remote->GetID());
		}
		else prev = r;
		r = next;
	}
}

//____________________________________________________________
bool TRemoteMgr::Refresh (IPNum id, PeerTimesPtr times)
{
	RemotePtr r = Head();
	while (r) {
		if (r->remote->GetID() == id) {
			TMidiRemote * m = (TMidiRemote *)r->remote;
			r->timeOut = times->local + kRemoteTTL;
			m->CheckLatency (times);
			return true;
		}
		r = r->next;
	}
	return false;
}

//____________________________________________________________
TMidiRemote * TRemoteMgr::FindOutSlot (short slotRef)
{
	RemotePtr r = Head();
	while (r) {
		TMidiRemote * m = (TMidiRemote *)r->remote;
		if (m->OutSlotRef() == slotRef) return m;
		r = r->next;
	}
	return 0;
}

//____________________________________________________________
// Generic Remote List management
//____________________________________________________________
TRemote * TRemoteList::Find (IPNum id)
{
	RemotePtr prev, r = Find (id, &prev);
	return r ? r->remote : 0;
}

//____________________________________________________________
void TRemoteList::Remove (IPNum id)
{
// this method is not completely safe: the final link exchange
// don't check that r->next is still a valid pointer
// however, it should not be a problem in the context of the
// method invocation: only the application Idle thread should
// remove remote clients
// however, conflicts with new clients should be checked
	RemotePtr prev, r, * adr;
	r = Find(id, &prev);
	if (r) {
		adr = prev ? &prev->next : (RemotePtr *)&fRemote.top;
		*adr = r->next;
		Delete (r);
	}
}

//____________________________________________________________
RemotePtr TRemoteList::Add (TRemote * remote)
{
	RemotePtr cl = Allocate ();
	if (cl) {
		cl->remote = remote;
		lfpush (&fRemote, (lifocell *)cl);
	}
	return cl;
}

//____________________________________________________________
void TRemoteList::Clear ()
{
	RemotePtr cl = (RemotePtr)lfpop (&fRemote);
	while (cl) {
		Delete (cl);
		cl = (RemotePtr)lfpop (&fRemote);
	}
}

//____________________________________________________________
void TRemoteList::Delete (RemotePtr r)
{
	delete r->remote;
	Free (r);
}

//____________________________________________________________
RemotePtr TRemoteList::Find (IPNum id, RemotePtr *previous)
{
	RemotePtr r = Head();
	*previous = 0;
	while (r) {
		if (r->remote->GetID() == id) return r;
		*previous = r;
		r = r->next;
	}
	return 0;
}

//____________________________________________________________
void TRemoteList::Free (RemotePtr r)	
{ 
#if macintosh
	DisposePtr ((Ptr)r);
#else
	free (r);
#endif 
}

//____________________________________________________________
RemotePtr TRemoteList::Allocate() 
{
#if macintosh
	return (RemotePtr)NewPtr (sizeof(RemoteList)); 
#else
	return (RemotePtr)malloc (sizeof(RemoteList)); 
#endif 
}
