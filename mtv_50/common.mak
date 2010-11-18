
.SUFFIXES: .c .o .cpp

all: $(LIBDIR)/$(LIBNAME)

$(LIBDIR)/$(LIBNAME): $(OBJS)
	$(AR) $(LIBDIR)/$(LIBNAME) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -fno-rtti -o $@ -c $<
.S.o:
	$(CC) $(CFLAGS) -D_ASSEMBLER_ -D__ASSEMBLY__ -o $@ -c $<
clean:
	rm -f  *~ *.o $(LIBDIR)/$(LIBNAME) depend

dep:    depend
depend:
	$(CC) -MM $(CFLAGS) $(SOURCES) 1>.depend

filelist:
	$(OBJS) $(SOURCES)

