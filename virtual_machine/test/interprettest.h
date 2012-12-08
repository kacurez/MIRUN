#ifndef INTERPRETTEST_H
#define INTERPRETTEST_H

#include "../class.h"

class InterpretTest
{
public:
	InterpretTest();
	virtual ~InterpretTest();
	void aritmeticTest();
	void jumpTest();
	void callTest();
	void callDynamicTest();
	void localsTest();
	void consoleTest();
	void arrayTest();
	void gcTest();
	
private:
	Method * initMethod(const char * name, const char * code, int codeLength, int paramCount, int locals, int flag);
	Class * initClass(const char * name, ConstantPool * pool);
	
};

#endif // INTERPRETTEST_H
