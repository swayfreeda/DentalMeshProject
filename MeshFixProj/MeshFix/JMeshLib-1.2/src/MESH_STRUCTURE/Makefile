include ../../makeconf

INCPATH = -I../../include
CFLAGS = $(MOREFLAGS) $(WARNLEVEL) -pedantic $(INCPATH) -I../RANGECODER

SOURCES.C    =  point.cpp\
		vertex.cpp\
		edge.cpp\
		triangle.cpp\
		tin.cpp\
		io.cpp\
		checkAndRepair.cpp
		
OBJECTS.O    =  $(SOURCES.C:.cpp=.o)

../OBJECTS/mesh_structure.o: $(OBJECTS.O)
	$(LD) -r -o ../OBJECTS/mesh_structure.o $(OBJECTS.O)

depend:
	makedepend -- $(CFLAGS) -- $(SOURCES.C)
	$(RM) -f Makefile.bak

clean :
	$(RM) -f *.o ../OBJECTS/mesh_structure.o

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

# DO NOT DELETE

point.o: ../../include/point.h ../../include/j_mesh.h /usr/include/stdio.h
point.o: /usr/include/features.h /usr/include/stdc-predef.h
point.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
point.o: /usr/include/math.h /usr/include/limits.h /usr/include/stdlib.h
point.o: /usr/include/alloca.h /usr/include/errno.h
vertex.o: ../../include/vertex.h ../../include/j_mesh.h /usr/include/stdio.h
vertex.o: /usr/include/features.h /usr/include/stdc-predef.h
vertex.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
vertex.o: /usr/include/math.h /usr/include/limits.h ../../include/list.h
vertex.o: ../../include/point.h ../../include/edge.h ../../include/triangle.h
vertex.o: ../../include/jmesh.h ../../include/matrix.h ../../include/tin.h
vertex.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/errno.h
edge.o: ../../include/edge.h ../../include/j_mesh.h /usr/include/stdio.h
edge.o: /usr/include/features.h /usr/include/stdc-predef.h
edge.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
edge.o: /usr/include/math.h /usr/include/limits.h ../../include/vertex.h
edge.o: ../../include/list.h ../../include/point.h ../../include/triangle.h
edge.o: ../../include/jmesh.h ../../include/matrix.h ../../include/tin.h
triangle.o: ../../include/triangle.h ../../include/jmesh.h
triangle.o: ../../include/j_mesh.h /usr/include/stdio.h
triangle.o: /usr/include/features.h /usr/include/stdc-predef.h
triangle.o: /usr/include/libio.h /usr/include/_G_config.h
triangle.o: /usr/include/wchar.h /usr/include/math.h /usr/include/limits.h
triangle.o: ../../include/list.h ../../include/point.h ../../include/vertex.h
triangle.o: ../../include/edge.h ../../include/matrix.h ../../include/tin.h
triangle.o: /usr/include/stdlib.h /usr/include/alloca.h
tin.o: ../../include/tin.h ../../include/jmesh.h ../../include/j_mesh.h
tin.o: /usr/include/stdio.h /usr/include/features.h
tin.o: /usr/include/stdc-predef.h /usr/include/libio.h
tin.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/math.h
tin.o: /usr/include/limits.h ../../include/list.h ../../include/point.h
tin.o: ../../include/vertex.h ../../include/edge.h ../../include/triangle.h
tin.o: ../../include/matrix.h /usr/include/stdlib.h /usr/include/alloca.h
tin.o: /usr/include/string.h
io.o: ../../include/jmesh.h ../../include/j_mesh.h /usr/include/stdio.h
io.o: /usr/include/features.h /usr/include/stdc-predef.h /usr/include/libio.h
io.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/math.h
io.o: /usr/include/limits.h ../../include/list.h ../../include/point.h
io.o: ../../include/vertex.h ../../include/edge.h ../../include/triangle.h
io.o: ../../include/matrix.h ../../include/tin.h /usr/include/stdlib.h
io.o: /usr/include/alloca.h /usr/include/string.h /usr/include/ctype.h
io.o: /usr/include/endian.h
checkAndRepair.o: ../../include/jmesh.h ../../include/j_mesh.h
checkAndRepair.o: /usr/include/stdio.h /usr/include/features.h
checkAndRepair.o: /usr/include/stdc-predef.h /usr/include/libio.h
checkAndRepair.o: /usr/include/_G_config.h /usr/include/wchar.h
checkAndRepair.o: /usr/include/math.h /usr/include/limits.h
checkAndRepair.o: ../../include/list.h ../../include/point.h
checkAndRepair.o: ../../include/vertex.h ../../include/edge.h
checkAndRepair.o: ../../include/triangle.h ../../include/matrix.h
checkAndRepair.o: ../../include/tin.h ../../include/jqsort.h
checkAndRepair.o: /usr/include/stdlib.h /usr/include/alloca.h
checkAndRepair.o: /usr/include/string.h
