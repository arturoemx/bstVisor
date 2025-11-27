all: bstVisor-raylib

TOPDIR = ..
INCLUDEDIR = $(TOPDIR)/include

ejArbol: ejArbol.cpp $(INCLUDEDIR)/BST.h include/nodoT.h
	g++ -Wall -g -I $(INCLUDEDIR) -o ejArbol ejArbol.cpp

bstVisor-raylib: bstVisor-raylib.cpp $(INCLUDEDIR)/BST.h $(INCLUDEDIR)/nodoT.h
	g++ -Wall -g -o bstVisor-raylib -I $(INCLUDEDIR) bstVisor-raylib.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

clean:
	rm bstVisor-raylib
