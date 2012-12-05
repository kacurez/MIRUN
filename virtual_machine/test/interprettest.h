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
	
private:
	Method * initMethod(const char * name, const char * code, int codeLength, int paramCount, int locals, int flag);
	Class * initClass(const char * name, ConstantPool * pool, int fields = 0);
	
};

#endif // INTERPRETTEST_H
