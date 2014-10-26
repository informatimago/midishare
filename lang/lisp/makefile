MIDISHARE_FRAMEWORK ?= ../../src/macosx-mono/build/Default/MidiShare.framework
PLAYER_FRAMEWORK    ?= ../../src/macosx-mono/build/Default/Player.framework

INSTDIR := $(MIDISHARE_FRAMEWORK)/Interfaces-ccl

FFIGEN_DIR=/usr/local/libexec/ffigen
GEN=$(FFIGEN_DIR)/bin/ffigen
CFLAGS="-m64 -isystem ${FFIGEN_DIR}/include -quiet -fffigen "


all:$(INSTDIR) $(INSTDIR)/Midishare.ffi $(INSTDIR)/MidiShare.lisp

$(INSTDIR):
	-mkdir -p $(INSTDIR) 

$(INSTDIR)/Midishare.ffi:$(MIDISHARE_FRAMEWORK)/Headers/MidiShare.h
	cd $(INSTDIR) ; $(GEN) $(CFLAGS) -D__Types__ ../Headers/MidiShare.h -o MidiShare.ffi

$(INSTDIR)/MidiShare.lisp:interface/openmcl/MidiShare-Interface.lisp
	cp -v interface/openmcl/MidiShare-Interface.lisp  $(INSTDIR)/MidiShare.lisp

