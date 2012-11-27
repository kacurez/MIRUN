/**
 * @author Jakub Valenta
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <map>
#include "object.h"

class Memory
{
public:
	Memory();
	virtual ~Memory();
	Object * getObject(uint32_t pointer) const;
	uint32_t allocate(Class * cls);
	uint32_t allocateNumber(Class * cls, int32_t value);
	uint32_t allocateNumber(Class * cls, double value);
	uint32_t allocateString(Class * cls, const char * value);
	uint32_t allocateArray(Class * cls, uint32_t length);

private:
	std::vector<Object *> memory;
	std::map<int, uint32_t> ints;
	std::map<double, uint32_t> doubles;
};

#endif // MEMORY_H
