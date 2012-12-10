#include "classgenerator.h"
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#define INIT_CODE_SIZE 1024

using namespace std;

ClassGenerator::ClassGenerator(): code(new char[INIT_CODE_SIZE]), maxCode(INIT_CODE_SIZE)
{
  std::cout << "ctor" << std::endl;
	//ctor
}

ClassGenerator::~ClassGenerator()
{
	delete [] code;
	delete cls;
}



void ClassGenerator::emitPush(double value)
{
	emit(PUSH);
	if(doubleConst.find(value) == doubleConst.end())
	{
		RealConst r;
		r.value = value;
		doubleConst[value] = pool->addItem(&r, REAL_CONST);
	}
	emit2Byte(doubleConst[value]);
}

void ClassGenerator::emitPush(int value)
{
	emit(PUSH);
	if(intConst.find(value) == intConst.end())
	{
		IntConst i;
		i.value = value;
		intConst[value] = pool->addItem(&i, INT_CONST);
	}
	emit2Byte(intConst[value]);
}

void ClassGenerator::emitPush(const char * value)
{
	emit(PUSH);
	cout << "push(\"" << value << "\")";
	if(stringConst.find(value) == stringConst.end())
	{
		StringConst s;
		s.length = strlen(value);
		s.value = new char[s.length];
		memcpy(s.value, value, s.length);
		stringConst[value] = pool->addItem(&s, STRING_CONST);
		delete s.value;
	}
	emit2Byte(stringConst[value]);
}

void ClassGenerator::allocLocal(const char * name)
{
//self has to be allocated at first ie at 0 position
  if(locals.size() > 0 && strcmp(name,"self")==0)
    throw "(Re)Allocating self on position that is greater than zero";
	
	uint8_t local = locals.size();
	locals[name] = local;
}

void ClassGenerator::finalizeMethod()
{
	cout << "finalizeMethod(): codeSize = " << nextInsAddress << ", locals = " << locals.size() << endl; 
	if(code[nextInsAddress -1] != RET && code[nextInsAddress -1] != RET_VOID)
	{
		code[nextInsAddress ++] = RET_VOID;
	}
	method->setCode(code, nextInsAddress);
	method->setLocals(locals.size());
	method->setFlag(0);
}
void ClassGenerator::setParamsCountForCurrentMethod(int count)
{
	cout << "setParamsForCurrentMethod(" << count << ")\n";
  method->setParamCount(count);
}
void ClassGenerator::createMethod(const char * name, int paramCount)
{
	cout << "createMethod(" << name << ")\n";
	method = new Method(name);
	method->setParamCount(paramCount);
	cls->addMethod(method);
	nextInsAddress = 0;
	locals.clear();
}

void ClassGenerator::addField(const char * name)
{
	cout << "setting field: " <<  name << " = " << (int) (cls->addField(name));
}

void ClassGenerator::setClassName(const char * name)
{
	cout << "setClassName(" << name << ")\n";
	cls = new Class(name);
	pool = new ConstantPool();
	cls->setConstPool(pool);
}

void ClassGenerator::emit(INSTRUCTION ins)
{
	checkCodeSize();
	code[nextInsAddress ++] = ins;
}

void ClassGenerator::emitNew(const char * className)
{
	emit(NEW);
	if(classNames.find(className) == classNames.end())
	{
		ClassRef cr;
		memset(cr.name, 0, IDENTIFIER_LENGTH);
		sprintf(cr.name, "%s", className);
		classNames[className] = pool->addItem(&cr, CLASS_REF);
	}
	emit2Byte(classNames[className]);
}

void ClassGenerator::completeAddress(int addr)
{
	code[addr] = nextInsAddress - addr - 1;
}

void ClassGenerator::emitSimpleJump(INSTRUCTION ins, int addr)
{
 emit(ins);
 code[nextInsAddress ++] = addr;
}
int ClassGenerator::emitNonCompleteJump(INSTRUCTION ins)
{
  emit(ins);
	checkCodeSize();
  return nextInsAddress++;
}

void ClassGenerator::emitCall(INSTRUCTION ins, const char * methodName, int paramCount, const char * className)
{
	emit(ins);
	cout << "emitCall: ";
	if(ins == CALL)
	{
		if(classNames.find(className) == classNames.end())
		{
			ClassRef cr;
			memset(cr.name, 0, IDENTIFIER_LENGTH);
			sprintf(cr.name, "%s", className);
			classNames[className] = pool->addItem(&cr, CLASS_REF);
		}
		cout << "static classs: " << className << "[" << classNames[className] << "]::";
		emit2Byte(classNames[className]);
		
	}
	std::stringstream ss;
	ss << methodName << "(" << paramCount << ")";
	if(methodNames.find(ss.str()) == methodNames.end())
	{
		MethodRef mr;
		mr.params = paramCount;
		memset(mr.name, 0, IDENTIFIER_LENGTH);
		sprintf(mr.name, "%s", methodName);
		methodNames[ss.str()] = pool->addItem(&mr, METHOD_REF);
	}
	cout << ss.str() << "[" << methodNames[ss.str()] << "]" << endl;
	emit2Byte(methodNames[ss.str()]);
}

void ClassGenerator::checkCodeSize(int toWrite)
{
	if(nextInsAddress + toWrite > maxCode)
	{
		char * newCode = new char[maxCode * 2];
		memcpy(newCode, code, nextInsAddress);
		delete [] code;
		code = newCode;
		maxCode *= 2;
	}
}

void ClassGenerator::emit2Byte(uint16_t value)
{
	checkCodeSize(2);
	code[nextInsAddress ++] = value & 0xff;
	code[nextInsAddress ++] = value >> 8;
}

void ClassGenerator::loadMember(const char * local, const char * field)
{
	loadLocal(local);
	emit(LOAD);
	if(fieldNames.find(field) == fieldNames.end())
	{
		FieldRef fr;
		memset(fr.name, 0, IDENTIFIER_LENGTH);
		sprintf(fr.name, "%s", field);
		fieldNames[field] = pool->addItem(&fr, FIELD_REF);
	}
	emit2Byte(fieldNames[field]);
}

void ClassGenerator::storeArray(const char* name, int index)
{
  emitPush(index);
  loadLocal(name);
  emit(STORE_ARRAY);  
}

void ClassGenerator::storeMember(const char * local, const char * field)
{
	loadLocal(local);
	emit(STORE);
	if(fieldNames.find(field) == fieldNames.end())
	{
		FieldRef fr;
		memset(fr.name, 0, IDENTIFIER_LENGTH);
		sprintf(fr.name, "%s", field);
		fieldNames[field] = pool->addItem(&fr, FIELD_REF);
	}
	emit2Byte(fieldNames[field]);
}

bool ClassGenerator::localExist(const char* local)
{
   return locals.find(local) != locals.end();

}

void ClassGenerator::loadLocal(const char * local)
{
	emit(LOAD_LOCAL);
	cout << "loadLoacal(" << local << ")" << endl;
  if(locals.find(local) == locals.end())
    throw (std::string("Uknown local variable:") + local).c_str();
	code[nextInsAddress ++] = locals[local];
}

void ClassGenerator::emitNewArray(int size)
{
  if(size <= 0)
    throw (std::string("Can not allocate an array with size <=0!!!")).c_str();   
    
  emitPush(size);
  emit(NEW_ARRAY);
  
  
}
void ClassGenerator::storeLocal(const char * local)
{
	emit(STORE_LOCAL);
	code[nextInsAddress ++] = locals[local];
}

void ClassGenerator::finalizeClass()
{
	std::string filename = cls->getName() + ".lessie";
	std::fstream file;
	file.open(filename.c_str(),  std::ios::out | std::ios::binary);
	writeIdent(file, cls->getName().c_str());
	writeConstantPool(file);
	uint8_t fieldCount = cls->getFieldCount();
	file.write((const char *)&fieldCount, sizeof(uint8_t));
	for(unsigned int i = 0; i < fieldCount; i ++)
	{
		writeIdent(file, cls->getFieldName(i).c_str());
	}
	uint8_t methodCount = cls->getMethodCount();
	file.write((const char *)&methodCount, sizeof(uint8_t));
	for(unsigned int i = 0; i < methodCount; i ++)
	{
		writeMethod(file, cls->getMethod(i));
	}
}

void ClassGenerator::writeIdent(std::fstream & file, const char * ident)
{
	char name[IDENTIFIER_LENGTH];
	memset(name, 0, IDENTIFIER_LENGTH);
	strcpy(name, ident);
	file.write(name, IDENTIFIER_LENGTH);
}

void ClassGenerator::writeConstantPool(std::fstream & file)
{
	uint16_t poolSize = pool->size();
	file.write((char *)&poolSize, sizeof(uint16_t));
	for(unsigned int i = 0; i < poolSize; i ++)
	{
		ConstPoolItem * item = pool->getItem(i);
		uint8_t tag = item->getType();
		file.write((char *)&tag, sizeof(uint8_t));
		switch(item->getType())
		{
			case METHOD_REF:
			{
				writeIdent(file, item->getStringValue());
				uint8_t paramCount = item->getParam();
				file.write((char *)&paramCount, sizeof(uint8_t));
				break;
			}
			case CLASS_REF:
			{
				writeIdent(file, item->getStringValue());
				break;
			}
			case STRING_CONST:
			{
				uint16_t len = item->getIntValue();
				file.write((char *)&len, sizeof(uint16_t));
				file.write(item->getStringValue(), len);
				break;
			}
			case INT_CONST:
			{
				int32_t value = item->getIntValue();
				file.write((char *)&value, sizeof(int32_t));
				break;
			}
			case REAL_CONST:
			{
				double value = item->getRealValue();
				file.write((char *)&value, sizeof(double));
				break;
			}
			case FIELD_REF:
			{
				writeIdent(file, item->getStringValue());
				break;
			}
		}
	}
}

void ClassGenerator::writeMethod(std::fstream & file, Method * m)
{
	uint8_t flag = m->getFlag();
	uint8_t paramCount = m->getParamCount();
	uint8_t localCount = m->getLocals();
	uint16_t codeSize = m->getCodeSize();
	writeIdent(file, m->getName().c_str());
	file.write((char *)&flag, sizeof(uint8_t));
	file.write((char *)&paramCount, sizeof(uint8_t));
	file.write((char *)&localCount, sizeof(uint8_t));
	file.write((char *)&codeSize, sizeof(uint16_t));
	file.write(m->getCode(), codeSize);
}
