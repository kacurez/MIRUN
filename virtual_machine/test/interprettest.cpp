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
	assert(30 == instance.run(cls->getName().c_str(), m->getName().c_str()));
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
	Class * cls = initClass("CallTest", pool);
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
		CALL, 0x01, 0x00, 0x02, 0x00, //ClassRef na pozici 1, MethodRef na pozici 2
		CALL, 0x01, 0x00, 0x03, 0x00, //ClassRef na pozici 1, MethodRef na pozici 3
		RET
	};
	Method * m = initMethod("callTest", code, sizeof(code), 0, 0, 0);
	Method * m1 = initMethod("returnVoid", code1, sizeof(code1), 0, 0, 0);
	Method * m2 = initMethod("returnInt", code2, sizeof(code2), 0, 0, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 1;
	pool->addItem(&i, INT_CONST);//0
	ClassRef cr;
	memset(cr.name, 0x00, IDENTIFIER_LENGTH);
	sprintf(cr.name, "%s", cls->getName().c_str());
	pool->addItem(&cr, CLASS_REF);//1
	MethodRef mr;
	sprintf(mr.name, "%s", m1->getName().c_str());
	mr.params = 0;
	pool->addItem(&mr, METHOD_REF);//2
	sprintf(mr.name, "%s", m2->getName().c_str());
	pool->addItem(&mr, METHOD_REF);//3
	cls->addMethod(m);
	cls->addMethod(m1);
	cls->addMethod(m2);
	cl.addClass(cls);
	assert(1 == instance.run(cls->getName().c_str(), m->getName().c_str()));
}

void InterpretTest::callDynamicTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("Counter", pool);
	cls->addField("counter");
	const char code1[] = 
	{
		LOAD_LOCAL, 0x00, // PUSH this
		LOAD, 0x06, 0x00, //PUSH this[0]
		LOAD_LOCAL, 0x01, // PUSH value
		ADD,  // this[0] + value
		LOAD_LOCAL, 0x00, // PUSH this
		STORE, 0x06, 0x00, // this[0] = this[0] + value
		RET_VOID
	};
	const char code2[] = 
	{
		LOAD_LOCAL, 0x00, // PUSH this
		LOAD, 0x06, 0x00, //PUSH this[0]
		RET
	};
	const char code3[] = 
	{
		PUSH, 0x04, 0x00, // PUSH 0
		LOAD_LOCAL, 0x00, // PUSH this
		STORE, 0x06, 0x00,  // this[0] = 0
		RET_VOID
	};
	const char code[] =
	{
		NEW, 0x00, 0x00, //new Counter
		DUP, STORE_LOCAL, 0x00, //a = new Counter
		CALL_DYNAMIC, 0x03, 0x00, //a.init
		LOAD_LOCAL, 0x00,
		PUSH, 0x05, 0x00, //PUSH 3
		CALL_DYNAMIC, 0x01, 0x00, //a.increase(3)
		LOAD_LOCAL, 0x00,
		CALL_DYNAMIC, 0x02, 0x00, //a.getValue
		RET
	};
	Method * m = initMethod("callTest", code, sizeof(code), 0, 1, 0);
	Method * m1 = initMethod("increase", code1, sizeof(code1), 2, 0, 1);
	Method * m2 = initMethod("getValue", code2, sizeof(code2), 1, 1, 1);
	Method * m3 = initMethod("init", code3, sizeof(code3), 1, 1, 1);
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
	FieldRef fr;
	memset(fr.name, 0x00, IDENTIFIER_LENGTH);
	sprintf(fr.name, "counter");
	pool->addItem(&fr, FIELD_REF); //6
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

void InterpretTest::consoleTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("ConsoleTest", pool);
	const char code[] = 
	{
		PUSH, 0x00, 0x00,
		CALL, 0x01, 0x00, 0x02, 0x00, //console.print(20)
		CALL, 0x01, 0x00, 0x03, 0x00, //line = console.readLine()
		CALL, 0x01, 0x00, 0x02, 0x00, //console.print(line)
		CALL, 0x01, 0x00, 0x04, 0x00, //i = console.readInt()
		CALL, 0x01, 0x00, 0x02, 0x00, //console.print(i)
		CALL, 0x01, 0x00, 0x05, 0x00, //i = console.readReal()
		CALL, 0x01, 0x00, 0x02, 0x00, //console.print(i)
		RET_VOID
	};
	Method * m = initMethod("consoleTest", code, sizeof(code), 0, 2, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 20;
	pool->addItem(&i, INT_CONST);//0
	ClassRef cr;
	sprintf(cr.name, "%s", CONSOLE_CLASS);//1
	pool->addItem(&cr, CLASS_REF);
	MethodRef mr;
	sprintf(mr.name, "print");
	mr.params = 1;
	pool->addItem(&mr, METHOD_REF);//2
	sprintf(mr.name, "readLine");
	mr.params = 0;
	pool->addItem(&mr, METHOD_REF);//3
	sprintf(mr.name, "readInt");
	mr.params = 0;
	pool->addItem(&mr, METHOD_REF);//4
	sprintf(mr.name, "readReal");
	mr.params = 0;
	pool->addItem(&mr, METHOD_REF);//5
	cls->addMethod(m);
	cl.addClass(cls);
	instance.run(cls->getName().c_str(), m->getName().c_str());
}

void InterpretTest::gcTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("GCTest", pool);
	const char code[] = 
	{
		PUSH, 0x00, 0x00,
		STORE_LOCAL, 0x00,
		PUSH, 0x01, 0x00, //label1
		LOAD_LOCAL, 0x00,
		SUB,
		IF_GE, 0x0D,//if(i < 1000)
		PUSH, 0x01, 0x00,
		NEW_ARRAY,
		STORE_LOCAL, 0x01,
		LOAD_LOCAL, 0x00,
		INC,
		STORE_LOCAL, 0x00,
		JMP, -21,
		RET_VOID
	};
	Method * m = initMethod("gcTest", code, sizeof(code), 0, 2, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 0;
	pool->addItem(&i, INT_CONST);//0
	i.value = 1000;
	pool->addItem(&i, INT_CONST);//1
	cls->addMethod(m);
	cl.addClass(cls);
	instance.run(cls->getName().c_str(), m->getName().c_str());
}

void InterpretTest::arrayTest()
{
	ConstantPool * pool = new ConstantPool();
	Class * cls = initClass("ArrayTest", pool);
	const char code[] = 
	{
		PUSH, 0x01, 0x00,
		NEW_ARRAY, // a = array[10]
		STORE_LOCAL, 0x00,
		PUSH, 0x00, 0x00,
		STORE_LOCAL, 0x01, // i = 0
		LOAD_LOCAL, 0x01, //label1
		PUSH, 0x01, 0x00,
		SUB,
		IF_GE, 0x0C,//if(i < 10)
		LOAD_LOCAL, 0x01,
		DUP, DUP,
		LOAD_LOCAL, 0x00,
		STORE_ARRAY, //a[i] = i
		INC,
		STORE_LOCAL, 0x01,
		JMP, -20, //jmp label1
		PUSH, 0x00, 0x00,
		PUSH, 0x00, 0x00,
		STORE_LOCAL, 0x01,
		STORE_LOCAL, 0x02,
		LOAD_LOCAL, 0x01, //label2
		PUSH, 0x01, 0x00,
		SUB,
		IF_GE, 17,//if(i < 10)
		LOAD_LOCAL, 0x01,
		LOAD_LOCAL, 0x00,
		LOAD_ARRAY, 
		LOAD_LOCAL, 0x02,
		ADD,  
		STORE_LOCAL, 0x02, //sum += a[i]
		LOAD_LOCAL, 0x01,
		INC,
		STORE_LOCAL, 0x01,
		JMP, -25, //jmp label2
		LOAD_LOCAL, 0x02,
		RET
	};
	Method * m = initMethod("arrayTest", code, sizeof(code), 0, 3, 0);
	ClassLoader cl("");
	Interpret instance(&cl);
	IntConst i;
	i.value = 0;
	pool->addItem(&i, INT_CONST);//0
	i.value = 10;
	pool->addItem(&i, INT_CONST);//1
	cls->addMethod(m);
	cl.addClass(cls);
	assert(45 == instance.run(cls->getName().c_str(), m->getName().c_str()));
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

Class* InterpretTest::initClass(const char * name, ConstantPool * pool)
{
	Class * c = new Class(name);
	c->setConstPool(pool);
	return c;
}

