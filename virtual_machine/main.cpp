#include <iostream>
#include "classloader.h"
#include "test/interprettest.h"

using namespace std;

int main()
{
	try {
		ClassLoader cl("data");
		cl.getClass("MyClass");
		InterpretTest test;
		test.aritmeticTest();
		test.jumpTest();
		test.callTest();
		test.callDynamicTest();
	} catch(const char * ex)
	{
		cout << "Program terminated: " << ex << endl;
	}
}
