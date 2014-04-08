PREFIX=/usr/local

OPTIONS = -D__Macintosh__ -D__MacOSX__ -D__Types__
INC = -I../common/Headers -I../common/atomic  -I../lib -Iinclude 
CFLAGS := -O3 $(OPTIONS) $(INC)
LDFLAGS := -macosx_version_min 10.6
COMP = -compatibility_version 1 -current_version 1

LIBO = ../lib/msPrefs.o ../lib/profport.o
OBJ  = MidiShare.o macosx.o $(LIBO)
COM  = msCommon.o msAudio.o msTimer.o
PA_STATIC    = /usr/local/lib/libportaudio.a
PA_DYNAMIC   = -lportaudio
TARGET = libMidiShare
VERS   = 1.8.6

all : static dylib

static : common $(OBJ) $(COM)
	ld -r $(LDFLAGS) -o $(TARGET).o \
		$(OBJ) $(COM) $(PA_STATIC) 

dylib : common $(OBJ) $(COM) 
	libtool -dynamic $(LDFLAGS) -o $(TARGET).$(VERS).dylib \
		$(OBJ) $(COM) $(PA_DYNAMIC) \
		-framework CoreAudio -framework CoreServices -framework AudioToolbox -framework AudioUnit \
		-lSystem /usr/lib/dylib1.o 
#		-exported_symbols_list _MidiShare.exp

common :
	make -C ../common

install :
	cp $(TARGET).$(VERS).dylib "$(PREFIX)"/lib
	cp include/MidiShare.h "$(PREFIX)"/include
	cd "$(PREFIX)"/lib && ln -s $(TARGET).$(VERS).dylib $(TARGET).dylib

uninstall :
	rm -f "$(PREFIX)"/lib/(TARGET).$(VERS).dylib
	rm -f "$(PREFIX)"/lib/(TARGET).dylib
	rm "$(PREFIX)"/include/MidiShare.h

clean :
	make -i -C ../common clean
	rm -f $(OBJ) $(TARGET).*

test :
	make -C ../tools

dep :
	make -i -C ../common dep 
