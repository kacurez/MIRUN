#ifndef INTERPRET_H
#define INTERPRET_H

#include "instruction.h"
#include "stackframe.h"
#include "classloader.h"

class Interpret
{
public:
	Interpret(ClassLoader * classLoader);
	~Interpret();
	int run(const char * className, const char * methodName);
	
private:
	int run();

	StackFrame * currentFrame;
	ClassLoader * classLoader;
	
};

#endif // INTERPRET_H
