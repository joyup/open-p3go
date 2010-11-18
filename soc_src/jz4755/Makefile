RELEASE = 1
I2C = 1
################################################################

#CAMERA :
#	    0. No Camera
#           1. OV2640
#           2.
################################################################

CAMERATYPE = 2


include ./tools.mak
TOP     := ../..
LIBDIR = $(TOP)/soc_src/LIBS

SOCDIR = $(TOP)/soc_src/jz4755#
DRVDIR = $(SOCDIR)/drv
OSDIR = $(TOP)/src
TARGETS = target
LIBNAME :=
INCDIR = $(TOP)/soc_src/common/inc
CFLAGS += -I$(SOCDIR) -I$(OSDIR) -I$(INCDIR) -DRELEASE=$(RELEASE)\
					-I$(SOCDIR)/include -I$(TOP)/inc  -DPLATFORM_CETUS

SOURCES :=
VPATH = $(INCDIR) $(OSDIR) $(SOCDIR)

CAMERASRC :=
SOURCES :=
I2CDIR = $(DRVDIR)/i2c
include $(I2CDIR)/i2c.mk
CAMERADIR = $(DRVDIR)/camera
include $(CAMERADIR)/camera.mk
CAMERASRC := $(SOURCES)
CAMERALIB := cameralib.a 
CAMERAOBJS 	:= $(addsuffix .o , $(basename $(notdir $(CAMERASRC))))
CAMERATAGETS := $(addprefix $(TARGETS)/, $(CAMERAOBJS))
SOURCES :=

include libdrv.mak
all: $(TARGETS) $(CAMERALIB)
	-@test $(LIBDIR)/$(CAMERALIB)
	
$(CAMERALIB):$(CAMERAOBJS) $(LIBDIR)
	$(AR) $(LIBDIR)/$@ $(CAMERATAGETS)
cameraclean:
	rm -f  *~ $(CAMERATAGETS) $(LIBDIR)/cameralib.a depend
$(LIBDIR):
	@mkdir $(LIBDIR)
source:
	echo $(CAMERAOBJS)
clean:cameraclean
$(TARGETS):
	-@test -d $(TARGETS) || mkdir $(TARGETS)
