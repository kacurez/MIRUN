program=lessie
testdir=test/
CC = g++    
CFLAGS = -Wno-write-strings 

# For debug activate -DDEBUG in CFLAGS

all: ./include/DebugMsg.cpp bison flex	
	$(CC) ./include/DebugMsg.cpp lex.yy.c lessie.tab.c -I./include $(CFLAGS) -o $(program) 

flex:
	flex lessie.l

bison:
	bison -t -d lessie.y

clean:
	rm -f lex.yy.c
	rm -f lessie.tab.c
	rm -f lessie.tab.h
	rm -f *.o
	rm -f $(program)
test: all
	python $(testdir)shouldcompile.py $(program)  $(testdir)/ExamplesPoliformat

  
