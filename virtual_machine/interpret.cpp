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
	uint32_t returnValue = VM_NULL;
	while(currentFrame)
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
				int res = currentFrame->pop() - currentFrame->pop();
				currentFrame->push(allocateNumber(res));
				break;
			}
			case CMP_NE:
			{
				if(currentFrame->pop() != currentFrame->pop())
				{
					currentFrame->push(allocateNumber(0));
				} else
				{
					currentFrame->push(allocateNumber(1));
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
				Class * cls = classLoader->getClass(i->getStringValue());
				uint32_t ref = allocate(cls);
				currentFrame->push(ref);
				break;
			}	
			case NEW_ARRAY:
			{
				uint32_t length = fetchInteger();
				currentFrame->push(allocateArray(length));
				break;
			}
			case JMP:
				doJump();
				break;
			case CALL:
			{
				uint16_t classNamePtr = currentFrame->nextShort();
				uint16_t methodPtr = currentFrame->nextShort();
				ConstPoolItem * i = currentFrame->constants->getItem(classNamePtr);
				ConstPoolItem * methodName = currentFrame->constants->getItem(methodPtr);
				assert(i->getType() == CLASS_REF);
				assert(methodName->getType() == METHOD_REF);
				Class * c = classLoader->getClass(i->getStringValue());
				Method * m = c->getMethod(methodName->getStringValue(), methodName->getParam());
				callMethod(c, m);
				break;
			}
			case CALL_DYNAMIC:
			{
				uint16_t methodNamePtr = currentFrame->nextShort();
				ConstPoolItem * i = currentFrame->constants->getItem(methodNamePtr);
				assert(i->getType() == METHOD_REF);
				Class * c = heap->getObject(currentFrame->top(i->getParam()))->getType();
				const char * methodName = i->getStringValue();
				Method * m = c->getMethod(methodName, i->getParam());
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
				if(currentFrame->getParent())
				{
					currentFrame->getParent()->push(retValue);
				} else
				{
					returnValue = retValue;
				}
			}
			case RET_VOID:
			{
				StackFrame * s = currentFrame;
				currentFrame = currentFrame->getParent();
				delete s;
				break;
			}
			case IF_EQ:
				doJump(fetchInteger() == 0);
				break;
			case IF_GT:
				doJump(fetchInteger() > 0);
				break;
			case IF_LT:
				doJump(fetchInteger() < 0);
				break;
			case IF_GE:
				doJump(fetchInteger() >= 0);
				break;
			case IF_LE:
				doJump(fetchInteger() <= 0);
				break;
			case IF_NE:
				doJump(fetchInteger() != 0);
				break;
			case PUSH:
			{
				uint16_t index = currentFrame->nextShort();
				ConstPoolItem * item = currentFrame->constants->getItem(index);
				switch(item->getType())
				{
					case INT_CONST:
						currentFrame->push(allocateNumber(item->getIntValue()));
						break;
					case REAL_CONST:
						currentFrame->push(allocateNumber(item->getRealValue()));
						break;
					case STRING_CONST:
						currentFrame->push(allocateString(item->getStringValue()));
						break;
					default:
						throw "Not a constant.";
				}
				break;
			}
			case LOAD:
			{
				Object * o = fetchObject();
				uint16_t field = currentFrame->nextShort();
				ConstPoolItem * i = currentFrame->constants->getItem(field);
				assert(i->getType() == FIELD_REF);
				currentFrame->push(o->getValueByName(i->getStringValue()));
				break;
			}
			case STORE:
			{
				Object * o = fetchObject();
				uint16_t field = currentFrame->nextShort();
				uint32_t value = currentFrame->pop();
				ConstPoolItem * i = currentFrame->constants->getItem(field);
				assert(i->getType() == FIELD_REF);
				o->setValueByName(i->getStringValue(), value);
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
				uint32_t index = fetchInteger();
				if(o->getType() != classLoader->getClass(ARRAY_CLASS))
				{
					throw "Not an array.";
				}
				currentFrame->push(o->getValue(index));
				break;
			}
			case STORE_ARRAY:
			{
				Object * o = fetchObject();
				uint32_t value = currentFrame->pop();
				uint32_t index = fetchInteger();
				if(o->getType() != classLoader->getClass(ARRAY_CLASS))
				{
					throw "Not an array.";
				}
				o->setValue(index, value);
				break;
			}
			case DUP:
				currentFrame->push(currentFrame->top());
				break;
			default: 
				throw "Invalid bytecode.";
		}
	}
	if(returnValue != VM_NULL)
	{
		Object * o = heap->getObject(returnValue);
		if(o->getType() == classLoader->getClass(INT_CLASS))
		{
			return o->getValue(0);
		}
	}
	return 0;
}

Object * Interpret::fetchObject()
{
	uint32_t pointer = currentFrame->pop();
	Object * o = heap->getObject(pointer);
	return o;
}

void Interpret::doAritmetics(INSTRUCTION i)
{
	Object * right = fetchObject();
	Object * left = fetchObject();
	checkNumber(left);
	checkNumber(right);
	if(left->getType() == classLoader->getClass(REAL_CLASS) ||  right->getType() == classLoader->getClass(REAL_CLASS))
	{
		doRealAritmetics(left, right, i);
	} else
	{
		doIntAritmetics(left, right, i);
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
		left = heap->getDoubleValue(op1->getValue(0));
	}
	if(op2->getType() == classLoader->getClass(INT_CLASS))
	{
		right = op2->getValue(0);
	} else
	{
		right = heap->getDoubleValue(op2->getValue(0));
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
	currentFrame->push(allocateNumber(result));
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
	currentFrame->push(allocateNumber(result));
}

void Interpret::addConst(int c)
{
	Object * o = fetchObject();
	checkInteger(o);
	currentFrame->push(allocateNumber((int)(o->getValue(0)) + c));
}

void Interpret::callMethod(Class * cls, Method * m)
{
	if(m->isNative())
	{
		NativeMethod * nm = dynamic_cast<NativeMethod *>(m);
		for(int i = m->getParamCount() - 1; i >= 0 ; i --)
		{
			nm->setParam(i, currentFrame->pop());
		}
		uint32_t ret = nm->run(heap);
		if(ret != VM_NULL)
		{
			currentFrame->push(ret);
		}
	} else
	{
		StackFrame * nextFrame = new StackFrame(m->getLocals(), m->getCode(), cls->getConstantPool(), currentFrame);
		for(int i = m->getParamCount() - 1; i >= 0 ; i --)
		{
			nextFrame->setLocal(i, currentFrame->pop());
		}
		currentFrame = nextFrame;
	}
}

void Interpret::doJump(bool cond)
{
	int8_t offset = currentFrame->nextByte();
	if(cond)
	{
		currentFrame->programCounter += offset;
	}
}

int32_t Interpret::fetchInteger()
{
	Object * o = fetchObject();
	checkInteger(o);
	return o->getValue(0);
}

uint32_t Interpret::allocateString(const char * value)
{
	uint32_t ptr = heap->allocateString(classLoader->getClass(STRING_CLASS), value);
	if(ptr == VM_NULL)
	{
		heap->gc(currentFrame);
		ptr = heap->allocateString(classLoader->getClass(STRING_CLASS), value);
		if(ptr == VM_NULL)
		{
			throw "Out of memory.";
		}
	}
	return ptr;
}

uint32_t Interpret::allocate(Class * cls)
{
	uint32_t ptr = heap->allocate(cls);
	if(ptr == VM_NULL)
	{
		heap->gc(currentFrame);
		ptr = heap->allocate(cls);
		if(ptr == VM_NULL)
		{
			throw "Out of memory.";
		}
	}
	return ptr;
}

uint32_t Interpret::allocateNumber(int value)
{
	uint32_t ptr = heap->allocateNumber(classLoader->getClass(INT_CLASS), value);
	if(ptr == VM_NULL)
	{
		heap->gc(currentFrame);
		ptr = heap->allocateNumber(classLoader->getClass(INT_CLASS), value);
		if(ptr == VM_NULL)
		{
			throw "Out of memory.";
		}
	}
	return ptr;
}

uint32_t Interpret::allocateNumber(double value)
{
	uint32_t ptr = heap->allocateNumber(classLoader->getClass(REAL_CLASS), value);
	if(ptr == VM_NULL)
	{
		heap->gc(currentFrame);
		ptr = heap->allocateNumber(classLoader->getClass(REAL_CLASS), value);
		if(ptr == VM_NULL)
		{
			throw "Out of memory.";
		}
	}
	return ptr;
}

uint32_t Interpret::allocateArray(uint32_t length)
{
	uint32_t ptr = heap->allocateArray(classLoader->getClass(ARRAY_CLASS), length);
	if(ptr == VM_NULL)
	{
		heap->gc(currentFrame);
		ptr = heap->allocateArray(classLoader->getClass(ARRAY_CLASS), length);
		if(ptr == VM_NULL)
		{
			throw "Out of memory.";
		}
	}
	return ptr;
}
