SOURCEOBJS := $(addsuffix .o ,$(basename $(notdir $(SOURCES))))
TARGEOBJS := $(SOURCEOBJS) $(OBJS) 
HEADO	:= $(addsuffix .o ,$(basename $(notdir $(HEADS))))
HEADTAGET = $(addprefix $(TARGETS)/,$(notdir $(HEADO)))
OBJSTAGET = $(addprefix $(TARGETS)/,$(notdir $(TARGEOBJS)))
APP	:= $(TARGETS)/$(PROGRAM).elf
VPATH += $(TARGETS) $(VPATH)

APPDEP = $(HEADO) $(TARGEOBJS) $(RELDEP) $(LDSFLAG) $(MAINLDS) makefile $(EXTRAOBJ) $(MAINAPP) $(SUBAPP) $(LIBS) $(OSLIBS)
all:	$(TARGETS) $(APP)
	$(OBJCOPY) -O binary $(APP) $(TARGETS)/$(PROGRAM).bin

$(APP):	$(APPDEP)
	echo $(SUBAPP)
	$(LD) -r -o $@.obj $(HEADTAGET) $(OBJSTAGET) $(DRVOBJ) $(EXTRAOBJ) $(MAINAPP) $(LIBS) $(GUILIB) $(OSLIBS)
ifeq ($(MODULE_GENLDS),1)
	$(GENLDS)  $@.obj  $(LDSFILES) > mobile_tv.lds
endif
	$(CC) --static -nostdlib -T $(LDSFLAG) -o $@ $@.obj $(STDCLIBS) $(LDFLAGS)


.c.o:
	$(CC) $(CFLAGS) -o $(TARGETS)/$@ -c $<
.cpp.o:
	$(CXX) $(CXXFLAGS) -fno-rtti -o $(TARGETS)/$@ -c $<
.S.o:
	$(CC) $(CFLAGS) -D_ASSEMBLER_ -D__ASSEMBLY__ -o  $(TARGETS)/$@ -c $<	

dump: $(TARGETS) $(APP)
	$(OBJCOPY) -O binary $(APP) $(TARGETS)/$(PROGRAM).bin
	$(OBJDUMP) -d $(APP) > $(TARGETS)/$(PROGRAM).dump
	$(NM) $(APP) | sort > $(TARGETS)/$(PROGRAM).sym 
	$(OBJDUMP) -h $(APP) > $(TARGETS)/$(PROGRAM).map
clean: $(SUBCLEAN)
	rm -fr $(TARGETS)
$(TARGETS): 
	-@test -d $(TARGETS) || mkdir $(TARGETS)
