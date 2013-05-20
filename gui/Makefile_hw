OPENGL_LIBS = -lglut -lGL -lGLU

CXX = $(shell wx-config --cxx)

SRC = Hello_World.cpp

G_OBJECTS = Hello_World.o

# implementation

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) -c `wx-config --cxxflags` -g -o $@ $<

all:    Hello_World

Hello_World: $(G_OBJECTS)
	 $(CXX) -o Hello_World $(G_OBJECTS) `wx-config --libs --gl_libs` $(OPENGL_LIBS)

clean: 
	rm -f *.o logsim guitest

depend:
	makedepend $(SRC)

# DO NOT DELETE

