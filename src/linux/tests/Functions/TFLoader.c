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
  
  [04-04-05] YO - Removed modversions dependencies 

*/

/*
 * --------------------------------------------------------------------------
 *	Includes for kernel code
 * --------------------------------------------------------------------------
*/

#ifdef MODULE
#include <linux/module.h>
#endif

#include <linux/malloc.h>
#include <linux/kernel.h>
#include <linux/kmod.h>

#include "MidiShareKernel.h"
#include "TFonctions.h"
#include "TFClient.h"

MODULE_SUPPORTED_DEVICE("MidiShare Functions Test");

#define kMSTFunMajor  154
#define kMSTFunName   "msFun-tests"

/*__________________________________________________________________________________*/
static int myopen(struct inode *inode, struct file * f)
{
	MOD_INC_USE_COUNT;
	return 0; 		/* 0 = OK, tout va bien */
}

/*__________________________________________________________________________________*/
static int myclose(struct inode *inode, struct file * f)
{
	MOD_DEC_USE_COUNT;
	return 0;
}

/*__________________________________________________________________________________*/
static ssize_t myread(struct file * f, char * s, size_t n, loff_t * i)
{
	return 0;
}

/*__________________________________________________________________________________*/
static ssize_t mywrite(struct file * f, const char * s, size_t n, loff_t * i)
{
	return -EINVAL;
}

/*__________________________________________________________________________________*/
/* ioctl dispatch */ 
static int myioctl(struct inode *inode, struct file *f, unsigned int msg, unsigned long param)
{ 
	switch (msg) {
		case kEnvironment:	Environment();
			break;
		case kOpenClose:	OpenClose();
			break;
		case kApplConfiguration: ApplConfiguration();
			break;
		case kConnections:	Connections(param);
			break;
		case kEvents:		Events(param);
			break;
		case kSequences:	Sequences(param);
			break;
		case kTime:		Time();
			break;
		case kSending:		Sending();
			break;
		case kMail:		Mail();
			break;	
		case kTasks:		Tasks(param);
			break;
		case kSynchro:		Synchro();
			break;
		case kTolerance:	Tolerance();
			break;
		case kNoMem:		NoMem();
			break;
		case kClose:		Close();
			break;
	}
	return 0;
}

struct file_operations myops = {
	read : myread,
	write : mywrite,
	ioctl : myioctl,
	open : myopen,
	release : myclose,
};

/*__________________________________________________________________________________*/
int init_module()
{
	int r = register_chrdev(kMSTFunMajor, kMSTFunName, &myops);
	if (r < 0) {
		printk ("Error when opening MidiShare Functions test module");
		return 1; 
	}
	return 0;
}

/*__________________________________________________________________________________*/
void cleanup_module()
{
	int r;
	r = unregister_chrdev(kMSTFunMajor, kMSTFunName);
	if (r < 0) printk ("Error when closing MidiShare Functions test module");
}

