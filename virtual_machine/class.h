#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
#include <inttypes.h>
#include "method.h"
#include "constantpool.h"

class Class
{
public:
	Class(const char * name);
	~Class();
	std::string getName() const;
	void addMethod(Method * f);
	Method * getMethod(uint8_t i) const;
	Method * getMethod(const char * name) const;
	int getFieldCount() const;
	void setFieldCount(uint8_t fieldCount);
	void setConstPool(ConstantPool * pool);
	ConstantPool * getConstantPool() const;
	
private:
	std::string name;
	std::vector<Method *> methods;
	uint8_t fieldCount;
	ConstantPool * pool;
};

#endif // CLASS_H
