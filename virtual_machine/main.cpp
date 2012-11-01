#include <iostream>
#include "classloader.h"

using namespace std;

int main()
{
    ClassLoader cl("data");
    delete cl.loadClass("MyClass");
}
