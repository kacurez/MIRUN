#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <inttypes.h>

class Method
{
public:
	Method(const char * name);
	virtual ~Method();
	std::string getName() const;
	const char * getCode() const;
	void setCode(const char * val, uint16_t size, uint32_t offset = 0);
	uint8_t getParamCount() const;
	void setParamCount(uint8_t val);
	void setFlag(uint8_t flag);
	void setLocals(uint8_t locals);
	uint8_t getLocals() const;
	std::string debugString() const;
	
private:
	std::string name;
	char * code;
	uint16_t codeSize;
	uint8_t paramCount;
	uint8_t flag;
	uint8_t locals;
};

#endif // FUNCTION_H
