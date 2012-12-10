#include <iostream>
#include "classloader.h"
#include "interpret.h"
#include "test/interprettest.h"

using namespace std;

void runTests()
{
	ClassLoader cl("data");
	cl.getClass("MyClass");
	InterpretTest test;
	test.aritmeticTest();
	test.jumpTest();
	test.callTest();
	test.callDynamicTest();
	test.localsTest();
	test.consoleTest();
	test.arrayTest();
	test.gcTest();
}

int main(int argc, char ** argv)
{
	const char * folder;
	const char * className;
	if(argc < 2)
	{
		cout <<  "try \"lessievm classname\" or \"lessievm classpath classname\"" << endl;
		return 1;
	} else if(argc == 2)
	{
		folder = "";
		className = argv[1];
	} else
	{
		folder = argv[1];
		className = argv[2];
	}
	ClassLoader cl(folder);
	Interpret interpret(&cl);
	try {
		
		return interpret.run(className, "main");
	} catch(const char * ex)
	{
		cout << "Program terminated: " << ex << endl;
	}
}
