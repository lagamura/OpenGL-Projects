CC = gcc
ifeq ($(shell sw_vers 2>/dev/null | grep mac | awk '{ print $$2}'),macOS)
CFLAGS = -g -Wno-deprecated-declarations
INCFLAGS = -I/usr/X11/include
LDFLAGS = -framework OpenGL -framework GLUT -framework Cocoa 
#CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED -DOSX -Wno-deprecated-register -Wno-deprecated-declarations -Wno-shift-op-parentheses -Wno-parentheses-equality
#INCFLAGS = -I/usr/X11/include
#LDFLAGS = -framework GLUT -framework OpenGL \
#		-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
#		-lGL -lGLU -lm -lstdc++ 
else
CFLAGS = -g #-DGL_GLEXT_PROTOTYPES 
INCFLAGS = -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LDFLAGS = -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib \
		-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11 -lm -lstdc++
endif
RM = /bin/rm -f
all: bresenham polygon_yx

bresenham: bresenham.o 
	$(CC) $(CFLAGS) -o bresenham bresenham.o $(INCFLAGS) $(LDFLAGS)
bresenham.o: bresenham.cpp 
	$(CC) $(CFLAGS) $(INCFLAGS) -c bresenham.cpp
	
polygon_yx: polygon_yx.o 
	$(CC) $(CFLAGS) -o polygon_yx polygon_yx.o $(INCFLAGS) $(LDFLAGS)
polygon_yx.o: polygon_yx.cpp 
	$(CC) $(CFLAGS) $(INCFLAGS) -c polygon_yx.cpp


clean: 
	$(RM) *.o my_triangles my_white
 
