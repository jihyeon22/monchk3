
###############################################################################
# Path

DESTDIR		:= $(CURDIR)/out

PREFIX		:= /system
BINDIR		:= $(PREFIX)/sbin
INITRCDIR	:= /etc/init.d


###############################################################################
# Compile

CC	:= $(CROSS_COMPILE)gcc

CFLAGS	:= $(EXTRA_CFLAGS)
LDFLAGS	:= $(EXTRA_LDFLAGS)

###############################################################################
# Options

#AUTOSTART	:= y

###############################################################################
# Board

BOARD	:=
CFLAGS	+= -DBOARD_$(BOARD)

###############################################################################
# Target rules

CFLAGS	+= 
LDFLAGS	+= 

MONCHK_SRC	:= src/monchk.c
CHKPROG_SRC	:= src/chkprog3.c

MONCHK_OBJ = $(MONCHK_SRC:.c=.o)
CHKPROG_OBJ = $(CHKPROG_SRC:.c=.o)

APP1	:= monchk3
APP2	:= chkprog3

define checkver
ifeq ($$(VER),0)
$$(error VER is not found, please define VER)
endif
endef

DEBUG_MODE  ?= 0

WORK_PATH   := /system/sbin
EXT_LIBS = -lmdsapi

all: all-before	$(APP1) $(APP2)

.c.o:
	$(CC) $(CFLAGS) $(LIB_LIBS) $(LIB_INCS) -c $< -o $@
    
all-before:
	$(eval $(call checkboard))


$(APP1):	$(MONCHK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(EXT_LIBS) 

$(APP2):		$(CHKPROG_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(EXT_LIBS) 
	
install:	install-binary

install-binary:	$(APP1) $(APP2)
	$(Q)$(call check_install_dir, $(DESTDIR)$(WORK_PATH))
	$(Q)fakeroot cp -v $(APP1) $(DESTDIR)$(WORK_PATH)/$(APP1) 
	$(Q)fakeroot cp -v $(APP2) $(DESTDIR)$(WORK_PATH)/$(APP2)
ifeq ($(VER),)
else
	$(Q)$(call check_install_dir, $(DESTDIR)/system/$(VER))
	$(Q)fakeroot cp -v $(APP1) $(DESTDIR)/system/$(VER)/$(APP1)
	$(Q)fakeroot cp -v $(APP2) $(DESTDIR)/system/$(VER)/$(APP2)
endif

clean:
	$(Q)rm -vrf $(APP1) $(MONCHK_OBJ) 
	$(Q)rm -vrf $(APP2) $(CHKPROG_OBJ) 

uninstall:
	$(Q)rm -vrf $(DESTDIR)$(WORK_PATH)/$(APP1)
	$(Q)rm -vrf $(DESTDIR)$(WORK_PATH)/$(APP2)


###############################################################################
# Functions

define check_install_dir
	if [ ! -d "$1" ]; then mkdir -p $1; fi
endef

define checkboard
#ifeq ($(BOARD),)
#$$(error BOARD is not found, BOARD=NEO_W100/NEO_W200)
#endif
endef
