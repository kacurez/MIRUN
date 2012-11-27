#include "interpret.h"
#include <stack>

Interpret::Interpret(ClassLoader * classLoader): classLoader(classLoader), heap(new Memory())
{
	//ctor
}

Interpret::~Interpret()
{
	delete heap;
}

int Interpret::run(const char * className, const char * methodName)
{
	Class * cls =classLoader->getClass(className);
	Method * m = cls->getMethod(methodName);
	currentFrame = new StackFrame(m->getLocals(), m->getCode(), cls->getConstantPool());
	return run();
}

int Interpret::run()
{
	while(1)
	{
		INSTRUCTION i = (INSTRUCTION)currentFrame->nextByte();
		switch(i)
		{
			case ADD: 
			case SUB:
			case MUL:
			case DIV:
				doAritmetics(i);
				break;
			case CMP_EQ:
			case CMP_NE:
			case INC:
				addConst(1);
				break;
			case DEC:
				addConst(-1);
				break;
			case NEW:
				break;
			case NEW_ARRAY:
				break;
			case JMP:
				break;
			case CALL:
			{
				uint16_t classNamePtr = currentFrame->nextShort();
				uint8_t methodPtr = currentFrame->nextByte();
				Class * c;//TODO get class by name from const pool
				Method * m = c->getMethod(methodPtr);
				callMethod(c, m);
				break;
			}
			case CALL_DYNAMIC:
			{
				Class * c = fetchObject()->getType();
				uint16_t methodNamePtr = currentFrame->nextShort();
				const char * methodName;//TODO get method name from const pool
				Method * m = c->getMethod(methodName);
				callMethod(c, m);
				break;
			}
			case RET:
			{
				uint32_t retValue = currentFrame->pop();
				currentFrame->getParent()->push(retValue);
			}
			case RET_VOID:
			{
				StackFrame * s = currentFrame;
				currentFrame = currentFrame->getParent();
				delete s;
				break;
			}
			case IF_EQ:
			case IF_GT:
			case IF_LT:
			case IF_GE:
			case IF_LE:
			case IF_NE:
			case PUSH:
			case LOAD:
			case STORE:
			case LOAD_LOCAL:
			case STORE_LOCAL:
			case POP:
				currentFrame->pop();
				break;
			default: 
				throw "Invalid bytecode.";
		}
	}
}

Object * Interpret::fetchObject()
{
	uint32_t pointer = currentFrame->pop();
	Object * o = heap->getObject(pointer);
	return o;
}

void Interpret::doAritmetics(INSTRUCTION i)
{
	Object * op1 = fetchObject();
	Object * op2 = fetchObject();
	if(!(checkNumber(op1) && checkNumber(op2)))
	{
		throw "Not a number.";
	}
	if(op1->getType() == classLoader->getClass(REAL_CLASS) ||  op2->getType() == classLoader->getClass(REAL_CLASS))
	{
		doRealAritmetics(op1, op2, i);
	} else
	{
		doIntAritmetics(op1, op2, i);
	}
}

bool Interpret::checkNumber(Object * o) const
{
	return o->getType() == classLoader->getClass(INT_CLASS) || o->getType() == classLoader->getClass(REAL_CLASS);
}

void Interpret::doRealAritmetics(Object * op1, Object *  op2, INSTRUCTION i)
{
	double left, right, result;
	if(op1->getType() == classLoader->getClass(INT_CLASS))
	{
		left = op1->getValue(0);
	} else
	{
		left = ((DoubleObject *) op1)->getValue();
	}
	if(op2->getType() == classLoader->getClass(INT_CLASS))
	{
		right = op2->getValue(0);
	} else
	{
		right = ((DoubleObject *) op2)->getValue();
	}
	switch(i)
	{
		case ADD:
			result = left + right;
			break;
		case SUB:
			result = left - right;
			break;
		case MUL:
			result = left * right;
			break;
		case DIV:
			result = left / right;
			break;
		default:
			throw "unreachable.";
	}
	currentFrame->push(heap->allocateNumber(classLoader->getClass(REAL_CLASS), result));
}

void Interpret::doIntAritmetics(Object * op1, Object *  op2, INSTRUCTION i)
{
	int left = op1->getValue(0);
	int right = op2->getValue(0);
	int result;
	switch(i)
	{
		case ADD:
			result = left + right;
			break;
		case SUB:
			result = left - right;
			break;
		case MUL:
			result = left * right;
			break;
		case DIV:
			result = left / right;
			break;
		default:
			throw "unreachable.";
	}
	currentFrame->push(heap->allocateNumber(classLoader->getClass(REAL_CLASS), result));
}

void Interpret::addConst(int c)
{
	Object * o = fetchObject();
	if(o->getType() != classLoader->getClass(INT_CLASS))
	{
		throw "Not an integer.";
	}
	currentFrame->push(heap->allocateNumber(classLoader->getClass(INT_CLASS), (int)(o->getValue(0)) + c));
}

void Interpret::callMethod(Class * cls, Method * m)
{
	StackFrame * nextFrame = new StackFrame(m->getLocals(), m->getCode(), cls->getConstantPool(), currentFrame);
	for(int i = 0; i < m->getParamCount(); i ++)
	{
		nextFrame->setLocal(i, currentFrame->pop());
	}
	currentFrame = nextFrame;
}
