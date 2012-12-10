#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
#include <map>
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
	Method * getMethod(const char * name, uint8_t paramCount) const;
	bool hasMethod(const char * name) const;
	int getFieldCount() const;
	void setConstPool(ConstantPool * pool);
	ConstantPool * getConstantPool() const;
	uint8_t addField(const char * name);
	uint8_t getFieldIndex(const char * name);
	std::string getFieldName(uint8_t field) const;
	uint8_t getMethodCount() const;
	
private:
	std::string name;
	std::vector<Method *> methods;
	ConstantPool * pool;
	std::map<std::string, uint8_t> fields;  
};

#endif // CLASS_H
