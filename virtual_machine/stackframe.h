#ifndef STACKFRAME_H
#define STACKFRAME_H

#include <inttypes.h>

#define MAX_STACK 256

class StackFrame
{
public:
	StackFrame(uint32_t localCount, const char * code, StackFrame * parent = 0);
	~StackFrame();
	void push(uint32_t item);
	void pushLocal(uint32_t index);
	void storeLocal(uint32_t index);
	uint32_t pop();
	StackFrame * getParent() const;
	
	const char * code;
	uint32_t programCounter;
	
private:
	uint32_t localCount;
	uint32_t * locals;
	uint32_t stack[MAX_STACK];
	uint32_t stackPointer;
	StackFrame * parent;
};

#endif // STACKFRAME_H
