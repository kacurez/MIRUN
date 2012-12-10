program=lessie
compiler = lessie
machine = lessievm
testdir=test/
CC = g++    
CFLAGS = -Wno-write-strings -Wall -pedantic
COMPILEROBJECTS = ./virtual_machine/classgenerator.cpp ./virtual_machine/class.cpp ./virtual_machine/method.cpp  ./virtual_machine/constantpool.cpp
MACHINEOBJECTS = ./virtual_machine/class.cpp  ./virtual_machine/classgenerator.cpp  ./virtual_machine/classloader.cpp  ./virtual_machine/constantpool.cpp  ./virtual_machine/interpret.cpp  ./virtual_machine/memory.cpp  ./virtual_machine/method.cpp  ./virtual_machine/nativemethods.cpp  ./virtual_machine/object.cpp  ./virtual_machine/stackframe.cpp  ./virtual_machine/main.cpp 
# For debug activate -DDEBUG in CFLAGS


compiler: bison flex  $(COMPILEROBJECTS)
	$(CC) $(COMPILEROBJECTS) lex.yy.c lessie.tab.c -I./virtual_machine $(CFLAGS) -o $(compiler) 

machine: $(MACHINEOBJECTS)
	$(CC) $(MACHINEOBJECTS)  -I./virtual_machine $(CFLAGS) -o $(machine)

batoh: compiler
	$(compiler) item.c problem.c solution.c  
	   

all: compiler machine batoh
 

flex:
	flex lessie.l

bison:
	bison --verbose -t -d lessie.y

clean:
	rm -f lex.yy.c
	rm -f lessie.tab.c
	rm -f lessie.tab.h
	rm -f *.o
	rm -f *.lessie
	rm -f $(program)
	rm -f $(compiler)
	rm -f $(machine)
	rm -f $(program).exe
	rm -f $(compiler).exe
	rm -f $(machine).exe  
test: all
	python $(testdir)shouldcompile.py $(program)  $(testdir)/ExamplesPoliformat

  
