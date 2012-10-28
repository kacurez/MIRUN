#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
#include <inttypes.h>
#include "function.h"

class Class
{
public:
	Class(const char * name);
	~Class();
	std::string getName() const;
	void addMethod(Function * f);
	const Function * getMethod(uint8_t i) const;
	int getFieldCount() const;
	void setFieldCount(uint8_t fieldCount);
	
private:
	std::string name;
	std::vector<Function *> methods;
	uint8_t fieldCount;
};

#endif // CLASS_H
