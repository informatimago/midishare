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
//	UScoreManager.h		    
// ===========================================================================
//
// Utility functions for score management
// 

#ifndef __UScoreManager__
#define __UScoreManager__

#include "TScore.h"
#include "TScoreVisitorInterface.h"
#include "TScoreIterator.h"

class UScoreManager {

	public:
	
		static void Apply (TScorePtr score, TScoreVisitorInterfacePtr fun) ;
		static void Apply (TScorePtr score, TScoreVisitorInterfacePtr fun, ULONG begin, ULONG end) ;
};

#endif