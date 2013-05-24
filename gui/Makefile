OPENGL_LIBS = -lglut -lGL -lGLU

CXX = $(shell wx-config --cxx)

SRC = datalogger.cpp comms.cpp rs232.cpp

OBJECTS = datalogger.o comms.o rs232.o

# implementation

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) -c `wx-config --cxxflags` -g -o $@ $<

all:    datalogger

datalogger: $(OBJECTS)
	    $(CXX) -o datalogger $(OBJECTS) `wx-config --libs --gl_libs` $(OPENGL_LIBS)

clean: 
	rm -f *.o datalogger

depend:
	makedepend $(SRC)

# DO NOT DELETE

datalogger.o: datalogger.h comms.h rs232.h
comms.o: comms.h rs232.h
rs232.o: rs232.h
