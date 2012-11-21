#include "interpret.h"

Interpret::Interpret(ClassLoader * classLoader): classLoader(classLoader)
{
	//ctor
}

Interpret::~Interpret()
{
	//dtor
}

int Interpret::run(const char * className, const char * methodName)
{
	Class * cls =classLoader->getClass(className);
	Method * m = cls->getMethod(methodName);
	currentFrame = new StackFrame(m->getLocals(), m->getCode());
	return run();
}

int Interpret::run()
{
	
}

