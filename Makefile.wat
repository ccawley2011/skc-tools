system = nt
# system = linux
# system = dos

CC = wcc386
CXX = wpp386

CFLAGS = -zq -obmilers -d0 -w8
CXXFLAGS = -zq -obmilers -d0 -w8
RCFLAGS =  -q -r

EXEEXT = .exe

!ifeq system nt
CFLAGS += -bt=$(system) -I"$(%WATCOM)/h" -I"$(%WATCOM)/h/nt"
CXXFLAGS += -bt=$(system) -I"$(%WATCOM)/h" -I"$(%WATCOM)/h/nt"
RCFLAGS += -bt=$(system) -I"$(%WATCOM)/h/nt"

!else ifeq system linux
CFLAGS += -bt=$(system) -I"$(%WATCOM)/lh"
!undef EXEEXT

!else ifeq system dos
CFLAGS += -bt=$(system) -I"$(%WATCOM)/h"
CC = wcc

!else
CFLAGS += -bt=$(system) -I"$(%WATCOM)/h"
!endif

SMC2SMF_EXE = skc2smf$(EXEEXT)
SMC2SMF_LNK = skc2smf.lnk
SMC2SMF_OBJS = skc2smf.obj Convert.obj

PLAYER_EXE = skc-music-player$(EXEEXT)
PLAYER_LNK = skc-music-player.lnk
PLAYER_OBJS = main.obj Convert.obj MidiDLL.obj WinMsg.obj
PLAYER_RCOBJS = resource.res


!ifeq system nt

all: $(PLAYER_EXE)

$(PLAYER_EXE) : $(PLAYER_LNK) $(PLAYER_OBJS) $(PLAYER_RCOBJS)
    wlink @$[@
    wrc -q $(PLAYER_RCOBJS) $^@

$(PLAYER_LNK) : Makefile.wat
    %create $@
    @%append $@ debug all
    @%append $@ name skc-music-player
    @%append $@ op map, quiet
    @%append $@ system nt_win
    @for %i in ($(PLAYER_OBJS)) do @%append $@ file %i

!endif

all: .symbolic $(SMC2SMF_EXE)

$(SMC2SMF_EXE) : $(SMC2SMF_LNK) $(SMC2SMF_OBJS)
    wlink @$[@

$(SMC2SMF_LNK) : Makefile.wat
    %create $@
    @%append $@ debug all
    @%append $@ name skc2smf
    @%append $@ op map, quiet
    @%append $@ system $(system)
!ifndef EXEEXT
    @%append $@ op noext
!endif
    @for %i in ($(SMC2SMF_OBJS)) do @%append $@ file %i

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj .res
.EXTENSIONS: .c .cpp .rc

.c.obj :
    $(CC) $(CFLAGS) -fo=$@ $[*

.cpp.obj :
    $(CXX) $(CXXFLAGS) -fo=$@ $[*

.rc.res :
    wrc $(RCFLAGS) -fo=$@ $[*

package: .symbolic all
    mkdir -p package/img/
    copy img/player-screenshot.png package/img/
    copy README.md package/
    copy $(SMC2SMF_EXE) package/
!ifeq system nt
    copy $(PLAYER_EXE) package/
!endif

clean: .symbolic
    rm -rf package/
    rm -f $(PLAYER_EXE) $(SMC2SMF_EXE)
    rm -f $(PLAYER_LNK) $(SMC2SMF_LNK)
    rm -f $(PLAYER_OBJS) $(SMC2SMF_OBJS)
    rm -f $(PLAYER_RCOBJS)
    rm -f *.map *.err
