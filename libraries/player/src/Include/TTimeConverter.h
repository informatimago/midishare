// ===========================================================================
// The Player Library is Copyright (c) Grame, Computer Music Research Laboratory 
// 1996-2003, and is distributed as Open Source software under the Artistic License;
// see the file "Artistic" that is included in the distribution for details.
//
// Grame : Computer Music Research Laboratory
// Web : http://www.grame.fr/Research
// E-mail : MidiShare@rd.grame.fr
// ===========================================================================


// ===========================================================================
//	TTimeConverter.h		    
// ===========================================================================

#ifndef __TTimeConverter__
#define __TTimeConverter__

 
#include "GenericMidiShare.h"
#include "TTime.h"


//----------------------
// Class TTimeConverter 
//----------------------
/*!
	\brief Ticks to Bar Beat Unit conversion functions (and the contrary)
*/


class TTimeConverter  {

	private:

		/*! Date of the last timesign change (in ticks) */
		ULONG 	fLast_timesign ;    
		/*! Position of the last timesign change */
		TPos   	fLast_pos;         
		short   fNumerator;         
		short 	fDenominator;
		short  	fNClocks;
		short 	fN32nd;
		TTime 	fInt;     			// Time Signature 
		
		ULONG 	CalcBeat (ULONG val, short r) { return (r < 0) ? val<<(- r) : val>>r;}
		
	public:
			
		TTimeConverter():fInt() {Init (kDefaultTpq);} 
		virtual ~TTimeConverter() {}
		
		void Init(ULONG tpq);  // Initialize with a new TPQ value
		void Init(); 		   // Initialize keeping the current TPQ
		
		TPos ConvertTickToBBU  (ULONG date); 	
		ULONG ConvertBBUToTick  (const TPos& pos);	

		void Update (MidiEvPtr ts) ;
		void Update (ULONG date, short num, short denom, short clock , short N32nd);
		
		short GetNum() 		{return fNumerator;}
		short GetDenom() 	{return fDenominator;}
		short GetNClocks() 	{return fNClocks;}
		short GetN32nd() 	{return fN32nd;}
		
		Boolean Inf (const TPos& p1, const TPos& p2) {return fInt.Inf(p1,p2);}
		Boolean Sup (const TPos& p1, const TPos& p2) {return fInt.Sup(p1,p2);}
		Boolean InfEq (const TPos& p1, const TPos& p2) {return fInt.InfEq(p1,p2);}
		Boolean SupEq (const TPos& p1, const TPos& p2) {return fInt.SupEq(p1,p2);}

};

typedef TTimeConverter FAR * TTimeConverterPtr;

#endif
