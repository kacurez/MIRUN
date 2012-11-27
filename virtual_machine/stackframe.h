#ifndef STACKFRAME_H
#define STACKFRAME_H

#include <inttypes.h>
#include "constantpool.h"

#define MAX_STACK 256

class StackFrame
{
public:
	StackFrame(uint32_t localCount, const char * code, ConstantPool * constants, StackFrame * parent = 0);
	~StackFrame();
	void push(uint32_t item);
	void pushLocal(uint32_t index);
	void storeLocal(uint32_t index);
	uint32_t pop();
	uint32_t top() const;
	StackFrame * getParent() const;
	void setLocal(uint32_t index, uint32_t item);
	uint8_t nextByte();
	uint16_t nextShort();
	
	const char * code;
	
private:
	uint32_t programCounter;
	uint32_t localCount;
	uint32_t * locals;
	uint32_t stack[MAX_STACK];
	uint32_t stackPointer;
	StackFrame * parent;
	ConstantPool * constants;
};

#endif // STACKFRAME_H
