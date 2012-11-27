#ifndef INTERPRET_H
#define INTERPRET_H

#include "instruction.h"
#include "stackframe.h"
#include "classloader.h"
#include "memory.h"

class Interpret
{
public:
	Interpret(ClassLoader * classLoader);
	~Interpret();
	int run(const char * className, const char * methodName);
	
private:
	int run();
	void doAritmetics(INSTRUCTION i);
	void doRealAritmetics(Object * op1, Object * op2, INSTRUCTION i);
	void doIntAritmetics(Object * op1, Object * op2, INSTRUCTION i);
	void addConst(int c);
	void callMethod(Class * cls, Method * m);
	Object * fetchObject();
	bool checkNumber(Object * o) const;

	StackFrame * currentFrame;
	ClassLoader * classLoader;
	Memory * heap;
	Class * currentClass;
	
};

#endif // INTERPRET_H
