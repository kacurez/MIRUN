#ifndef INTERPRET_H
#define INTERPRET_H

#include "instruction.h"
#include "stackframe.h"

class Interpret
{
public:
	Interpret();
	~Interpret();
	
private:
	StackFrame * currentFrame;
};

#endif // INTERPRET_H
