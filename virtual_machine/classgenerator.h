/**
 * @author Jakub Valenta
 */
#ifndef CLASSGENERATOR_H
#define CLASSGENERATOR_H

#include "method.h"
#include "class.h"
#include "instruction.h"
#include <map>
#include <fstream>


class ClassGenerator
{
public:
	ClassGenerator();
	virtual ~ClassGenerator();
	void setClassName(const char * name);
	void addField(const char * name);
	void createMethod(const char * name, int paramCount);
	void finalizeMethod();
	void allocLocal(const char * name);
	void emit(INSTRUCTION ins);
	void emitNew(const char * className);
  void emitNewArray(int size);
  void storeArray(const char* name,int index)
  {
    emitPush(index);
    loadLocal(name);
    emit(STORE_ARRAY);
  
  }
	/**
	 * Emituje skok (jmp, if_*) na adresu.
	 * @return ukazatel na pozici addr
	 */
	int emitJump(INSTRUCTION ins, int addr = 0);
	/**
	 * Upravi hodnotu na pozici add, aby skocila na adresu pristi instrukce.
	 */
	void completeAddress(int addr);
	void emitCall(INSTRUCTION ins, const char * methodName, int paramCount, const char * className = 0);
	void emitPush(int value);
	void emitPush(double value);
	void emitPush(const char * value);
	void loadMember(const char * local, const char * field);
	void storeMember(const char * local, const char * field);
	void loadLocal(const char * local);
	void storeLocal(const char * local);
	void finalizeClass();
	bool localExist(const char* local);
	/**
	 * Adresa pristi instrukce.
	 */
	int nextInsAddress;

private:
	void emit2Byte(uint16_t value);
	void checkCodeSize(int toWrite = 1);
	void writeIdent(std::fstream & file, const char * ident);
	void writeConstantPool(std::fstream & file);
	void writeMethod(std::fstream & file, Method * m);
	
	char * code;
	int maxCode;
	Class * cls;
	Method * method;
	ConstantPool * pool;
	std::map<std::string, uint16_t> classNames;
	std::map<std::string, uint16_t> methodNames;
	std::map<std::string, uint16_t> fieldNames;
	std::map<std::string, uint16_t> locals;
	std::map<int, uint16_t> intConst;
	std::map<double, uint16_t> doubleConst;
	std::map<std::string, uint16_t> stringConst;
};

#endif // CLASSGENERATOR_H
