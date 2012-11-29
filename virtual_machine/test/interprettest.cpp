#include "interprettest.h"
#include "../class.h"
#include "../instruction.h"
#include "../interpret.h"
#include <assert.h>

InterpretTest::InterpretTest()
{
	//ctor
}

InterpretTest::~InterpretTest()
{
	//dtor
}

void InterpretTest::aritmeticTest()
{
	Class cls = Class("AritmeticTest");
	Method * m = new Method("aritmeticTest");
	ConstantPool * pool = new ConstantPool();
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 20;
	pool->addItem(&i, INT_CONST);
	i.value = 30;
	pool->addItem(&i, INT_CONST);
	const char code[] = 
	{
		PUSH, 0x00, 0x00,
		PUSH, 0x01, 0x00,
		ADD,
		PUSH, 0x00, 0x00,
		SUB,
		PUSH, 0x00, 0x00,
		MUL,
		PUSH, 0x00, 0x00,
		DIV,
		INC,
		DEC,
		INC,
		RET
	};
	m->setCode(code, sizeof(code));
	m->setLocals(0);
	m->setParamCount(0);
	m->setFlag(0);
	cls.setConstPool(pool);
	cls.addMethod(m);
	assert(1 == instance.run(&cls, "aritmeticTest"));
}
