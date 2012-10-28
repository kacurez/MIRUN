#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <inttypes.h>

class Function
{
public:
	Function(const char * name);
	virtual ~Function();
	std::string getName() const;
	const unsigned char * getCode() const;
	void setCode(const unsigned char * val, uint16_t size, uint32_t offset = 0);
	uint8_t getParamCount() const;
	void setParamCount(uint8_t val);
private:
	std::string name;
	unsigned char * code;
	uint16_t codeSize;
	uint8_t paramCount;
};

#endif // FUNCTION_H
