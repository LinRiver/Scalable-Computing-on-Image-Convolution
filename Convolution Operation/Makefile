CXX=g++
CXXFLAGS=-c -fopenmp
LDFLAGS=-fopenmp

OBJECTS=main.o image.o

stencil: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o stencil $(OBJECTS) -lpng

all:	stencil

run:	all
	./stencil test-image.png

