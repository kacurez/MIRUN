/**
 * @author Jakub Valenta
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <map>
#include <fstream>
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
	double getDoubleValue(uint32_t ref) const;
	const char * getStringValue(uint32_t ref) const;
	uint32_t createFile(const char * fileName);
	std::fstream * getFileStream(uint32_t file);

private:
	std::vector<Object *> memory;
	std::map<int, uint32_t> ints;
	std::vector<double> doubles;
	std::vector<char *> strings;
	std::vector<std::fstream *> files;
};

#endif // MEMORY_H
