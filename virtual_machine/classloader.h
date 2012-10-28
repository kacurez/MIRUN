#ifndef CLASSLOADER_H
#define CLASSLOADER_H

#include <string>
#include <fstream>
#include <vector>
#include "class.h"

class ClassLoader
{
public:
	ClassLoader();
	~ClassLoader();
	void load(const char * filename);

private:
	void loadClass();

	std::fstream file;
	std::vector<Class *> classTable;

};

#endif // CLASSLOADER_H
