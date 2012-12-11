#include <iostream>
#include "classloader.h"
#include "interpret.h"

using namespace std;

int main(int argc, char ** argv)
{
	const char * folder;
	const char * className;
	if(argc < 3)
	{
		cout <<  "try \"lessievm classname\"" << endl;
		return 1;
	}
	folder = argv[1];
	className = argv[2];
	ClassLoader cl(folder);
	Interpret interpret(&cl);
	try {
		
		return interpret.run(className, "main", argc - 3, argv + 3);
	} catch(const char * ex)
	{
		cout << "Program terminated: " << ex << endl;
	}
}
