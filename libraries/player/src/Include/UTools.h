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
//	 UTools.h 			    
// ===========================================================================
//
//	Utilities functions
// 


#ifndef __UTools__
#define __UTools__
 

#include "TPlayerConstants.h"

//-----------------------------
// Class UTools 
//-----------------------------

class UTools {

	public :
	
		static void SendVal(ULONG val);
		static void  MidiPrintText ( char * s);
	
};

#endif
