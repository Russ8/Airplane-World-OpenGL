
PLATFORM := $(shell uname)

# Use pkg-config to get the right libraries for your platform
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -lGL
EXT = 
DEFS = `pkg-config --cflags glfw3`

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
    DEFS =-DWIN32
endif

LINK += stb_image.o 
LINK += tiny_obj_loader.o 
LINK += QuadTree.o 
LINK += Particle.o 
LINK += Model.o 

.PHONY:  clean

all : run$(EXT)

run$(EXT) : main.o $(LINK)
	g++ $(DEFS) -o run main.o $(LINK) $(GL_LIBS)

main.o : main.cpp $(LINK)

stb_image.o : stb_image.cpp external/stb_image.h
	g++ $(DEFS) -c stb_image.cpp

tiny_obj_loader.o : tiny_obj_loader.cc external/tiny_obj_loader.h
	g++ $(DEFS) -c tiny_obj_loader.cc

QuadTree.o : QuadTree.cpp QuadTree.h
	g++ $(DEFS) -c QuadTree.cpp

Particle.o : Particle.cpp Particle.h
	g++ $(DEFS) -c Particle.cpp

Model.o : Model.cpp Model.h
	g++ $(DEFS) -c Model.cpp


clean:
	rm -f *.o assign3_part2$(EXT)
