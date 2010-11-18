.SUFFIXES: .c .o .cpp
.c.o:
	$(CC) $(CFLAGS) -o $(TARGETS)/$@ -c $<
.cpp.o:
	$(CXX) $(CXXFLAGS) -fno-rtti -fvtable-gc -o $(TARGETS)/$@ -c $<
.S.o:
	$(CC) $(CFLAGS) -D_ASSEMBLER_ -D__ASSEMBLY__ -o $(TARGETS)/$@ -c $<
