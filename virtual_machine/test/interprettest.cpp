#include "interprettest.h"
#include "../class.h"
#include "../instruction.h"
#include "../interpret.h"
#include <assert.h>
#include <string.h>

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
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("AritmeticTest", pool);
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
	Method * m = initMethod("aritmeticTest", code, sizeof(code), 0, 0, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 20;
	pool->addItem(&i, INT_CONST);
	i.value = 30;
	pool->addItem(&i, INT_CONST);
	cls->addMethod(m);
	cl.addClass(cls);
	assert(1 == instance.run(cls->getName().c_str(), m->getName().c_str()));
}

void InterpretTest::jumpTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("JumpTest", pool);
	const char code[] = 
	{
		JMP, 0x01,
		RET_VOID, //skipped
		PUSH, 0x01, 0x00,
		IF_EQ, 0x01,
		RET_VOID, //skipped
		PUSH, 0x00, 0x00,
		IF_NE, 0x01,
		RET_VOID, //skipped
		PUSH, 0x02, 0x00,
		IF_LT, 0x01,
		RET_VOID, //skipped
		PUSH, 0x00, 0x00,
		IF_GT, 0x01,
		RET_VOID, //skipped
		PUSH, 0x00, 0x00,
		PUSH, 0x00, 0x00,
		IF_LE, 0x02,
		IF_GE, 0x01,
		RET_VOID, //skipped
		PUSH, 0x00, 0x00,
		INC,
		PUSH, 0x00, 0x00,
		INC,
		CMP_EQ,
		IF_EQ, 0x01,
		RET_VOID,
		PUSH, 0x00, 0x00,
		PUSH, 0x0, 0x00,
		CMP_NE,
		IF_NE, 0x01,
		RET_VOID,
		PUSH, 0x00, 0x00,
		RET
	};
	Method * m = initMethod("jumpTest", code, sizeof(code), 0, 0, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 1;
	pool->addItem(&i, INT_CONST);
	i.value = 0;
	pool->addItem(&i, INT_CONST);
	i.value = -1;
	pool->addItem(&i, INT_CONST);
	cls->addMethod(m);
	cl.addClass(cls);
	assert(1 == instance.run(cls->getName().c_str(), m->getName().c_str()));
}

void InterpretTest::callTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("CallTest", pool, 1);
	const char code1[] = 
	{
		RET_VOID
	};
	const char code2[] = 
	{
		PUSH, 0x00, 0x00,
		RET
	};
	const char code[] =
	{
		CALL, 0x01, 0x00, 0x01, //ClassRef na pozici 1, ukazatel na funkci 1
		CALL, 0x01, 0x00, 0x02, //ClassRef na pozici 1, ukazatel na funkci 2
		RET
	};
	Method * m = initMethod("callTest", code, sizeof(code), 0, 0, 0);
	Method * m1 = initMethod("returnVoid", code1, sizeof(code1), 0, 0, 0);
	Method * m2 = initMethod("returnInt", code2, sizeof(code2), 0, 0, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 1;
	pool->addItem(&i, INT_CONST);
	ClassRef cr;
	memset(cr.name, 0x00, IDENTIFIER_LENGTH);
	sprintf(cr.name, "%s", cls->getName().c_str());
	pool->addItem(&cr, CLASS_REF);
	cls->addMethod(m);
	cls->addMethod(m1);
	cls->addMethod(m2);
	cl.addClass(cls);
	assert(1 == instance.run(cls->getName().c_str(), m->getName().c_str()));
}

void InterpretTest::callDynamicTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("Counter", pool, 1);
	const char code1[] = 
	{
		LOAD_LOCAL, 0x00, // PUSH this
		LOAD, 0x00, //PUSH this[0]
		LOAD_LOCAL, 0x01, // PUSH value
		ADD,  // this[0] + value
		LOAD_LOCAL, 0x00, // PUSH this
		STORE, 0x00, // this[0] = this[0] + value
		RET_VOID
	};
	const char code2[] = 
	{
		LOAD_LOCAL, 0x00, // PUSH this
		LOAD, 0x00, //PUSH this[0]
		RET
	};
	const char code3[] = 
	{
		PUSH, 0x04, 0x00, // PUSH 0
		LOAD_LOCAL, 0x00, // PUSH this
		STORE, 0x00,  // this[0] = 0
		RET_VOID
	};
	const char code[] =
	{
		NEW, 0x00, 0x00, //new Counter
		DUP, STORE_LOCAL, 0x00, //a = new Counter
		CALL_DYNAMIC, 0x03, 0x00, //a.init
		PUSH, 0x05, 0x00, //PUSH 3
		LOAD_LOCAL, 0x00,
		CALL_DYNAMIC, 0x01, 0x00, //a.increase(3)
		LOAD_LOCAL, 0x00,
		CALL_DYNAMIC, 0x02, 0x00, //a.getValue
		RET
	};
	Method * m = initMethod("callTest", code, sizeof(code), 0, 1, 0);
	Method * m1 = initMethod("increase", code1, sizeof(code1), 2, 0, 1);
	Method * m2 = initMethod("getValue", code2, sizeof(code2), 1, 0, 1);
	Method * m3 = initMethod("init", code3, sizeof(code3), 1, 0, 1);
	ClassLoader cl("");
	Interpret instance(&cl);
	ClassRef cr;
	memset(cr.name, 0x00, IDENTIFIER_LENGTH);
	sprintf(cr.name, "%s", cls->getName().c_str());
	pool->addItem(&cr, CLASS_REF); //0
	MethodRef mr;
	mr.params = m1->getParamCount();
	sprintf(mr.name, "%s", m1->getName().c_str());
	pool->addItem(&mr, METHOD_REF); //1
	mr.params = m2->getParamCount();
	sprintf(mr.name, "%s", m2->getName().c_str());
	pool->addItem(&mr, METHOD_REF); //2
	mr.params = m3->getParamCount();
	sprintf(mr.name, "%s", m3->getName().c_str());
	pool->addItem(&mr, METHOD_REF); //3
	IntConst i;
	i.value = 0;
	pool->addItem(&i, INT_CONST); //4
	i.value = 3;
	pool->addItem(&i, INT_CONST); //5
	cls->addMethod(m);
	cls->addMethod(m1);
	cls->addMethod(m2);
	cls->addMethod(m3);
	cl.addClass(cls);
	assert(3 == instance.run(cls->getName().c_str(), m->getName().c_str()));
}

void InterpretTest::localsTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("LocalsTest", pool);
	const char code[] = 
	{
		PUSH, 0x00, 0x00,
		STORE_LOCAL, 0x00,
		PUSH, 0x01, 0x00,
		STORE_LOCAL, 0x01,
		LOAD_LOCAL, 0x00,
		LOAD_LOCAL, 0x01,
		ADD,
		RET
	};
	Method * m = initMethod("localsTest", code, sizeof(code), 0, 2, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 20;
	pool->addItem(&i, INT_CONST);
	i.value = 30;
	pool->addItem(&i, INT_CONST);
	cls->addMethod(m);
	cl.addClass(cls);
	assert(50 == instance.run(cls->getName().c_str(), m->getName().c_str()));
}

Method * InterpretTest::initMethod(const char * name, const char * code, int codeLength, int paramCount, int locals, int flag)
{
	Method * m = new Method(name);
	m->setCode(code, codeLength);
	m->setLocals(locals);
	m->setParamCount(paramCount);
	m->setFlag(flag);
	return m;
}

Class* InterpretTest::initClass(const char * name, ConstantPool * pool, int fields)
{
	Class * c = new Class(name);
	c->setConstPool(pool);
	c->setFieldCount(fields);
	return c;
}

