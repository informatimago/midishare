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

*/

#ifdef __Macintosh__
#include <MacMemory.h>
#endif

#ifdef __Windows__
#include <windows.h>
#endif

#ifdef __Linux__

# ifdef MODVERSIONS
# include <linux/modversions.h>
# endif


#ifdef MODULE
# include <linux/malloc.h>
# define malloc(size)	kmalloc(size, GFP_KERNEL)
# define free(mem)	kfree(mem)
#else
# include <malloc.h>
#endif

#endif /* __Linux__ */

#include "msMem.h"

/*_________________________________________________________________________*/
/* memory allocation implementation                                        */
/*_________________________________________________________________________*/
FarPtr(void) AllocateMemory (MemoryType type, unsigned long size)
{
#if defined (__Macintosh__)
	return NewPtrSys (size);
#elif defined (__Linux__)
	return (void*)malloc(size);
#elif defined (__Windows__)
	HLOCAL h = LocalAlloc (LMEM_FIXED, size + sizeof(HLOCAL));
	if (h) {
		HLOCAL * ptr = (HLOCAL *)LocalLock (h);
		if (ptr) {
			*ptr++ = h;
			return ptr;
		}
	}
	return 0;
#endif
}

/*_________________________________________________________________________*/
void DisposeMemory  (FarPtr(void) memPtr)
{
#if defined (__Macintosh__)
	if (memPtr) DisposePtr ((Ptr)memPtr);
#elif defined (__Linux__)
	if (memPtr) free(memPtr);
#elif defined (__Windows__)
	HLOCAL * ptr = (HLOCAL *)memPtr;
	if (ptr) {
		HLOCAL h = *--ptr;
		if (h) {
			LocalUnlock (h);
			LocalFree (h);
		}
	}
#endif
}

