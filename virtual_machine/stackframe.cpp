#include "stackframe.h"

StackFrame::StackFrame(uint32_t localCount, StackFrame * parent): localCount(localCount), stackPointer(0), parent(parent)
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


