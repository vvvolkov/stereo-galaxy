#############
# VARS
#############

CC      = g++
CFLAGS = -Wall -Wextra
OFLAGS = -DNDEBUG -O3
SRCFILE = StereoGalaxy.cpp
EXEFILE = stereo-galaxy


#############
# SFML 2.0:
#############

SFPATH = ~vvvolkov/Downloads/SFML-2.0-alpha
IFLAGS = -I$(SFPATH)/include
LFLAGS = -L$(SFPATH)/build/lib
SFLIBS = -lsfml-window-s -lsfml-system-s

LINKLIBS = $(SFLIBS) -lX11 -lICE -lSM -lXrandr -lGL -lGLU -lrt -pthread -Wl,-rpath,$(SFPATH)


#############
# TARGETS
#############


all:
	$(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) -o $(EXEFILE) $(SRCFILE) $(LINKLIBS)

opt:
	$(CC) $(CFLAGS) $(OFLAGS) $(IFLAGS) $(LFLAGS) -o $(EXEFILE) $(SRCFILE) $(LINKLIBS)

run:
	./$(EXEFILE)

clean:
	rm -f $(EXEFILE)
