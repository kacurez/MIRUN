#include <iostream>
#include "classloader.h"
#include "test/interprettest.h"

using namespace std;

int main()
{
    ClassLoader cl("data");
    cl.getClass("MyClass");
    InterpretTest test;
    test.aritmeticTest();
}
