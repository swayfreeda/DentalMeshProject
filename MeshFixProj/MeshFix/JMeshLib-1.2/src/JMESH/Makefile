include ../../makeconf

INCPATH = -I../../include

CFLAGS = $(MOREFLAGS) $(WARNLEVEL) -pedantic $(INCPATH)

SOURCES.C    =  jmesh.cpp

OBJECTS.O    =  $(SOURCES.C:.cpp=.o)

../OBJECTS/jmesh.o: $(OBJECTS.O)
	$(LD) -r -o ../OBJECTS/jmesh.o $(OBJECTS.O)

depend:
	makedepend -- $(CFLAGS) -- $(SOURCES.C)
	$(RM) -f Makefile.bak

clean :
	$(RM) -f *.o ../OBJECTS/jmesh.o

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

# DO NOT DELETE

jmesh.o: ../../include/jmesh.h ../../include/j_mesh.h /usr/include/stdio.h
jmesh.o: /usr/include/features.h /usr/include/stdc-predef.h
jmesh.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
jmesh.o: /usr/include/math.h /usr/include/limits.h ../../include/list.h
jmesh.o: ../../include/point.h ../../include/vertex.h ../../include/edge.h
jmesh.o: ../../include/triangle.h ../../include/matrix.h ../../include/tin.h
jmesh.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/string.h
jmesh.o: /usr/include/xlocale.h
