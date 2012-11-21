#ifndef CLASSLOADER_H
#define CLASSLOADER_H

#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "class.h"
#include "method.h"
#include "constantpool.h"

class ClassLoader
{
public:
	ClassLoader(const char * folder);
	~ClassLoader();
	Class * getClass(const char * className);

private:
	Class * loadClass(const char * className);
	Method * loadMethod();
	ConstantPool * loadConstantPool();
	std::string getFileName(const char * className);

	std::string folder;
	std::fstream file;
	std::map<std::string, Class *> classTable;

};

#endif // CLASSLOADER_H
