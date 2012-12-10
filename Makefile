program=lessie
testdir=test/
CC = g++    
CFLAGS = -Wno-write-strings 
COMPILEROBJECTS = ./virtual_machine/DebugMsg.cpp ./virtual_machine/classgenerator.cpp ./virtual_machine/class.cpp ./virtual_machine/method.cpp  ./virtual_machine/constantpool.cpp
# For debug activate -DDEBUG in CFLAGS

all: ./virtual_machine/DebugMsg.cpp bison flex	
	$(CC) $(COMPILEROBJECTS) lex.yy.c lessie.tab.c -I./virtual_machine $(CFLAGS) -o $(program) 

flex:
	flex lessie.l

bison:
	bison --verbose -t -d lessie.y

clean:
	rm -f lex.yy.c
	rm -f lessie.tab.c
	rm -f lessie.tab.h
	rm -f *.o
	rm -f $(program)
test: all
	python $(testdir)shouldcompile.py $(program)  $(testdir)/ExamplesPoliformat

  
