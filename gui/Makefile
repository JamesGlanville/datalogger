OPENGL_LIBS = -lglut -lGL -lGLU

CXX = $(shell wx-config --cxx)

SRC = datalogger.cpp

G_OBJECTS = datalogger.o

# implementation

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) -c `wx-config --cxxflags` -g -o $@ $<

all:    datalogger

datalogger: $(G_OBJECTS)
	 $(CXX) -o datalogger $(G_OBJECTS) `wx-config --libs --gl_libs` $(OPENGL_LIBS)

clean: 
	rm -f *.o

depend:
	makedepend $(SRC)

# DO NOT DELETE


