/*

  Copyright � Grame 1999-2002

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


/*****************************************************************************/
/* MidiAppl.c : interface to MidiShare 
/*
/* MidiShare home page 	: http://www.grame.fr/MidiShare/
/* MidiShare/Java home page : http://www.grame.fr/MidiShare/Develop/Java.html
/*
/* Bug & comments report : MidiShare@rd.grame.fr
/*	
/* History : 20/07/96 Suppression des fonction SetData et GetData : utilisation de MidiSetField
/*          19/03/02 Thread bloquant sur MacOS9
/*	    17/04/02 Appel direct du code Java dans la ReceiveAlarm sur MacOSX, Windows et Linux
/*		
/*****************************************************************************/

#ifdef __Macintosh__
	#ifdef __MacOS9__
            #include <MidiShare.h>
            static  ProcessSerialNumber gJavaProcess;
            UPPRcvAlarmPtr UPPJRcvAlarmPtr ;
            UPPApplAlarmPtr UPPJApplAlarmPtr ;
	#else
            #include <MidiShare.h>
            RcvAlarmPtr UPPJRcvAlarmPtr ;
            ApplAlarmPtr UPPJApplAlarmPtr ;
	#endif
#endif

#ifdef __Linux__
	#include "MidiShare.h"
	#define MSALARMAPI
        RcvAlarmPtr UPPJRcvAlarmPtr ;
        ApplAlarmPtr UPPJApplAlarmPtr ;
#endif

#ifdef WIN32
	#define true 1
	#define false 0
	#include <MidiShare.h>
	ApplAlarmPtr UPPJApplAlarmPtr ;
	RcvAlarmPtr UPPJRcvAlarmPtr ;
#endif

#include "MidiAppl.h"
#include <stdlib.h>

#define typeTask 19
#define typeAlarm 20

/*--------------------------------------------------------------------------*/
typedef struct ApplContext {
   JavaVM * fJvm;
   JNIEnv * fEnv;
   jmethodID fMid;
   jobject fObj;
   jclass fClass;
   int fAttached;
}ApplContext;


/*--------------------------------------------------------------------------*/
static void  MSALARMAPI ApplAlarm( short ref,long code)
{	
        MidiEvPtr e;
        
	if ((e = MidiNewEv(typeAlarm))) {
		MidiSetField(e,0,code);
		MidiSendIm (ref+128, e);
  	}
}


/*--------------------------------------------------------------------------*/
static int CheckThreadEnv(ApplContext* context)
{
    if (context->fAttached) {
         return true;
    }else if ((*context->fJvm)->AttachCurrentThread(context->fJvm, &context->fEnv, NULL) == 0) {
        context->fAttached = true;
        return true;
    }else{
        return false;
    }
}
         
/*--------------------------------------------------------------------------*/
static void MSALARMAPI JavaTask ( long date, short refNum, long a1,long a2,long a3 )
{
        ApplContext* context = MidiGetInfo(refNum);
        jclass class;
        jmethodID mid;
        jobject task;
        jobject appl;
        jfieldID taskptr;
     
        if (context && CheckThreadEnv(context)) {
            appl = (jobject)a1;
            task = (jobject)a2;
            class = (*context->fEnv)->GetObjectClass(context->fEnv, task);
            mid = (*context->fEnv)->GetMethodID(context->fEnv, class, "Execute", "(Lgrame/midishare/MidiAppl;I)V");
            taskptr = (*context->fEnv)->GetFieldID(context->fEnv, class, "taskptr",  "I");
            (*context->fEnv)->SetIntField(context->fEnv,task,taskptr,0); 
            (*context->fEnv)->CallVoidMethod(context->fEnv, task, mid, appl,date);
            (*context->fEnv)->DeleteGlobalRef(context->fEnv, appl);
            (*context->fEnv)->DeleteGlobalRef(context->fEnv, task);
        }else{
            printf("ReceiveAlarm error : cannot callback Java task\n");
        }
}
                            
/*--------------------------------------------------------------------------*/
static void MSALARMAPI ReceiveAlarm( short ref)
{
        #if defined (__Macintosh__) && defined(__MacOS9__)
            UniversalProcPtr proc = MidiGetInfo(ref);
            
            if (proc) {
                CallUniversalProc(proc, 0);
                WakeUpProcess(&gJavaProcess);
            }
        #else
            ApplContext* context = MidiGetInfo(ref);
             
            if (context && CheckThreadEnv(context)) {   
                (*context->fEnv)->CallVoidMethod(context->fEnv, context->fObj, context->fMid);
            }else{
                printf("ReceiveAlarm error : cannot callback Java MidiEventLoop\n");
            }
        #endif
}


/*--------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_grame_midishare_MidiAppl_ApplOpen
  (JNIEnv * env , jobject obj, jint ref) {
  
        ApplContext* context;
        jclass cls;
        jsize size;
        int res;
    
        context = (ApplContext*) malloc(sizeof(ApplContext));
        if (!context) goto error;        
        
        res = JNI_GetCreatedJavaVMs(&context->fJvm, 1, &size);
        if (res < 0) goto error;
        
        cls = (*env)->GetObjectClass(env, obj);
        context->fClass = (*env)->NewGlobalRef(env,cls);
        context->fMid = (*env)->GetMethodID(env, context->fClass, "MidiEventLoop", "()V");
        context->fObj = (*env)->NewGlobalRef(env,obj);
        context->fAttached = false;
        context->fEnv = 0;
         
        MidiSetInfo(ref,context);
        
    #if defined( __Macintosh__) && defined( __MacOS9__)
        UPPJApplAlarmPtr = NewApplAlarmPtr(ApplAlarm);
        UPPJRcvAlarmPtr = NewRcvAlarmPtr(ReceiveAlarm);
        GetCurrentProcess(&gJavaProcess);
    #else
     	UPPJApplAlarmPtr =  ApplAlarm;
        UPPJRcvAlarmPtr =  ReceiveAlarm ;
    #endif 

        MidiSetApplAlarm(ref,UPPJApplAlarmPtr);
        MidiSetRcvAlarm(ref,UPPJRcvAlarmPtr);
 
        return 1;
       
  error :
       if (context) free (context);
       return 0;         
   	
}

/*--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_grame_midishare_MidiAppl_ApplClose
  (JNIEnv * env, jobject obj, jint ref) 
{
        ApplContext* context = MidiGetInfo(ref);

        if (context) {
            (*context->fEnv)->DeleteGlobalRef(context->fEnv, context->fClass);
            (*context->fEnv)->DeleteGlobalRef(context->fEnv, context->fObj);
            free(context);
        }
            
        #if defined( __Macintosh__) && defined( __MacOS9__)
            if (MidiGetApplAlarm (ref)) DisposeRoutineDescriptor (MidiGetApplAlarm (ref));
            if (MidiGetRcvAlarm (ref)) DisposeRoutineDescriptor (MidiGetRcvAlarm (ref));
        #endif
  }
 
/*--------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_grame_midishare_MidiAppl_ScheduleTask
	(JNIEnv * env, jobject appl, jobject task, jint date, jint ref)
{
        jclass cls;
        jfieldID taskptr;
        MidiEvPtr taskev;
        
        #if defined (__Macintosh__) && defined(__MacOS9__)
            taskev = MidiDTask(JavaTask, date, ref, (long)((*env)->NewGlobalRef(env,appl)), (long)((*env)->NewGlobalRef(env,task)), 0);
        #else
            taskev = MidiTask(JavaTask, date, ref, (long)((*env)->NewGlobalRef(env,appl)), (long)((*env)->NewGlobalRef(env,task)), 0);
        #endif
        
        cls = (*env)->GetObjectClass(env, task);
        taskptr = (*env)->GetFieldID(env, cls, "taskptr",  "I");
        (*env)->SetIntField(env,task,taskptr,(jint)taskev); 
          
        return (jint)taskev;
}


