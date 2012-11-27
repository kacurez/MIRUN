#include "stackframe.h"

StackFrame::StackFrame(uint32_t localCount, const char * code, ConstantPool * constants, StackFrame * parent): code(code), constants(constants), programCounter(0), localCount(localCount), stackPointer(0), parent(parent)
{
	locals = new uint32_t[localCount];
}

StackFrame::~StackFrame()
{
	delete [] locals;
}

uint32_t StackFrame::pop()
{
	if(stackPointer == 0)
	{
		throw 1;
	}
	return stack[-- stackPointer];
}

uint32_t StackFrame::top() const
{
	if(stackPointer == 0)
	{
		throw 1;
	}
	return stack[stackPointer - 1];
}

void StackFrame::push(uint32_t item)
{
	if(stackPointer == MAX_STACK - 1)
	{
		throw 1;
	}
	stack[stackPointer ++] = item;
}

void StackFrame::storeLocal(uint32_t index)
{
	if(index < localCount)
	{
		locals[index] = pop();
	} else 
	{
		throw 1;
	}
}

void StackFrame::pushLocal(uint32_t index)
{
	if(index < localCount)
	{
		push(locals[index]);
	} else
	{
		throw 1;
	}
}

StackFrame * StackFrame::getParent() const
{
	return parent;
}

void StackFrame::setLocal(uint32_t index, uint32_t item)
{
	if(index > localCount)
	{
		throw "No such local.";
	}
	locals[index] = item;
}

uint16_t StackFrame::nextShort()
{
	uint16_t value = (uint16_t)(* (code + programCounter));
	programCounter += 2;
	return value;
}

uint8_t StackFrame::nextByte()
{
	return code[programCounter ++];
}


