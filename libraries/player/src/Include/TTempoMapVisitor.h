// ===========================================================================
// The Player Library is Copyright (c) Grame, Computer Music Research Laboratory 
// 1996-1999, and is distributed as Open Source software under the Artistic License;
// see the file "Artistic" that is included in the distribution for details.
//
// Grame : Computer Music Research Laboratory
// Web : http://www.grame.fr/Research
// E-mail : MidiShare@rd.grame.fr
// ===========================================================================


// ===========================================================================
//	TTempoMapVisitor.h		    
// ===========================================================================
//
// TTempoMapVisitor: an object used to maintain the current Tempo and TimeSign state
// and provide convertion functions 
//

#ifndef __TTempoMapVisitor__
#define __TTempoMapVisitor__

#include "TEvent.h"
#include "TScoreVisitorInterface.h"
#include "TTempoMap.h"
#include "TEvent.h"


//-----------------------
// Class TTempoMapVisitor 
//-----------------------

class TTempoMapVisitor :public TScoreVisitorInterface, public TTempoMap {

	public :
 
 		TTempoMapVisitor(ULONG tpq) :TTempoMap(tpq){}
 		virtual ~TTempoMapVisitor (){}
 		
 		void Visite (TTempoPtr ev,Boolean forward);
		void Visite (TTimeSignPtr ev,Boolean forward); 
		
			
};

typedef TTempoMapVisitor FAR * TTempoMapVisitorPtr;


#endif