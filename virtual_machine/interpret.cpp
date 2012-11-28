#include "interpret.h"
#include <assert.h>

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
			{
				int32_t res = currentFrame->pop() - currentFrame->pop();
				currentFrame->push(heap->allocateNumber(classLoader->getClass(INT_CLASS), res));
				break;
			}
			case CMP_NE:
			{
				if(currentFrame->pop() != currentFrame->pop())
				{
					currentFrame->push(heap->allocateNumber(classLoader->getClass(INT_CLASS), 0));
				} else
				{
					currentFrame->push(heap->allocateNumber(classLoader->getClass(INT_CLASS), 1));
				}
				break;
			}
			case INC:
				addConst(1);
				break;
			case DEC:
				addConst(-1);
				break;
			case NEW:
			{
				uint16_t classNamePtr = currentFrame->nextShort();
				ConstPoolItem * i = currentFrame->constants->getItem(classNamePtr);
				assert(i->getType() == CLASS_REF);
				currentFrame->push(heap->allocate(classLoader->getClass(i->getStringValue())));
				break;
			}	
			case NEW_ARRAY:
				break;
			case JMP:
				doJump();
				break;
			case CALL:
			{
				uint16_t classNamePtr = currentFrame->nextShort();
				uint8_t methodPtr = currentFrame->nextByte();
				ConstPoolItem * i = currentFrame->constants->getItem(classNamePtr);
				assert(i->getType() == CLASS_REF);
				Class * c = classLoader->getClass(i->getStringValue());
				Method * m = c->getMethod(methodPtr);
				callMethod(c, m);
				break;
			}
			case CALL_DYNAMIC:
			{
				Class * c = fetchObject()->getType();
				uint16_t methodNamePtr = currentFrame->nextShort();
				ConstPoolItem * i = currentFrame->constants->getItem(methodNamePtr);
				assert(i->getType() == METHOD_REF);
				const char * methodName = i->getStringValue();
				Method * m = c->getMethod(methodName);
				if(m && m->getParamCount() == i->getParam())
				{
					callMethod(c, m);
				} else
				{
					throw "No such method.";
				}
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
				if(fetchInteger() == 0)
				{
					doJump();
				}
				break;
			case IF_GT:
				if(fetchInteger() > 0)
				{
					doJump();
				}
				break;
			case IF_LT:
				if(fetchInteger() < 0)
				{
					doJump();
				}
				break;
			case IF_GE:
				if(fetchInteger() >= 0)
				{
					doJump();
				}
				break;
			case IF_LE:
				if(fetchInteger() <= 0)
				{
					doJump();
				}
				break;
			case IF_NE:
				if(fetchInteger() != 0)
				{
					doJump();
				}
				break;
			case PUSH:
			{
				uint16_t index = currentFrame->nextShort();
				ConstPoolItem * item = currentFrame->constants->getItem(index);
				switch(item->getType())
				{
					case INT_CONST:
						currentFrame->push(heap->allocateNumber(classLoader->getClass(INT_CLASS), item->getIntValue()));
						break;
					case REAL_CONST:
						currentFrame->push(heap->allocateNumber(classLoader->getClass(REAL_CLASS), item->getRealValue()));
						break;
					case STRING_CONST:
						currentFrame->push(heap->allocateString(classLoader->getClass(STRING_CLASS), item->getStringValue()));
						break;
					default:
						throw "Not a constant.";
				}
				break;
			}
			case LOAD:
			{
				Object * o = fetchObject();
				uint8_t field = currentFrame->nextByte();
				currentFrame->push(o->getValue(field));
				break;
			}
			case STORE:
			{
				Object * o = fetchObject();
				uint8_t field = currentFrame->nextByte();
				o->setValue(field, currentFrame->pop());
				break;
			}
			case LOAD_LOCAL:
			{
				currentFrame->pushLocal(currentFrame->nextByte());
				break;
			}
			case STORE_LOCAL:
			{
				currentFrame->storeLocal(currentFrame->nextByte());
				break;
			}
			case POP:
				currentFrame->pop();
				break;
			case LOAD_ARRAY:
			{
				Object * o = fetchObject();
				if(o->getType() != classLoader->getClass(ARRAY_CLASS))
				{
					throw "Not an array.";
				}
				uint32_t index = fetchInteger();
				currentFrame->push(((ArrayObject *) o)->getValue(index));
				break;
			}
			case STORE_ARRAY:
			{
				Object * o = fetchObject();
				if(o->getType() != classLoader->getClass(ARRAY_CLASS))
				{
					throw "Not an array.";
				}
				uint32_t index = fetchInteger();
				((ArrayObject *) o)->setValue(index, currentFrame->pop());
				break;
			}
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
	checkNumber(op1);
	checkNumber(op2);
	if(op1->getType() == classLoader->getClass(REAL_CLASS) ||  op2->getType() == classLoader->getClass(REAL_CLASS))
	{
		doRealAritmetics(op1, op2, i);
	} else
	{
		doIntAritmetics(op1, op2, i);
	}
}

void Interpret::checkNumber(Object * o) const
{
	if(!(o->getType() == classLoader->getClass(INT_CLASS) || o->getType() == classLoader->getClass(REAL_CLASS)))
	{
		throw "Not a number.";
	}
}

void Interpret::checkInteger(Object * o) const
{
	if(o->getType() != classLoader->getClass(INT_CLASS))
	{
		throw "Not an integer.";
	}
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
	int32_t left = op1->getValue(0);
	int32_t right = op2->getValue(0);
	int32_t result;
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
	checkInteger(o);
	currentFrame->push(heap->allocateNumber(classLoader->getClass(INT_CLASS), (int32_t)(o->getValue(0)) + c));
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

void Interpret::doJump()
{
	int8_t offset = currentFrame->nextShort();
	currentFrame->programCounter += offset;
}

int32_t Interpret::fetchInteger()
{
	Object * o = fetchObject();
	checkInteger(o);
	return o->getValue(0);
}
