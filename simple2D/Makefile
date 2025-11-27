all: ejArbol bstVisor-simple2D

TOPDIR = ..
INCLUDEDIR = $(TOPDIR)/include

ejArbol: ejArbol.cpp $(INCLUDEDIR)/BST.h $(INCLUDEDIR)/nodoT.h
	g++ -Wall -g -I ./$(INCLUDEDIR) -o ejArbol ejArbol.cpp

bstVisor-simple2D: bstVisor-simple2D.cpp $(INCLUDEDIR)/BST.h $(INCLUDEDIR)/nodoT.h
	g++ -Wall -g -o bstVisor-simple2D -I $(INCLUDEDIR) bstVisor-simple2D.cpp `simple2d --libs`

clean:
	rm ejArbol bstVisor-simple2D