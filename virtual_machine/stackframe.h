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
	uint32_t size() const;
	uint32_t getValueAt(uint32_t index) const;
	void setValueAt(uint32_t index, uint32_t value);
	uint32_t getLocal(uint32_t local) const;
	uint32_t getLocalCount() const;
	
	const char * code;
	ConstantPool * constants;
	uint32_t programCounter;
	
private:
	uint32_t localCount;
	uint32_t * locals;
	uint32_t stack[MAX_STACK];
	uint32_t stackPointer;
	StackFrame * parent;
};

#endif // STACKFRAME_H
