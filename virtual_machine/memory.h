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
	uint32_t allocateNumber(Class * cls, int value);
	uint32_t allocateNumber(Class * cls, double value);

private:
	std::vector<Object *> memory;
	std::map<int, uint32_t> ints;
	std::map<double, uint32_t> doubles;
};

#endif // MEMORY_H
