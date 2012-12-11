/**
 * @author Jakub Valenta
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <queue>
#include <map>
#include "object.h"
#include "stackframe.h"
#include <stdio.h>

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
	uint32_t createFile(const char * fileName, const char * mask);
	FILE * getFileStream(uint32_t file);
	void gc(StackFrame * stack);

private:
	char * used;
	char * empty;
	uint32_t size;
	uint32_t maxSize;
	uint32_t objectSize;
	std::map<int, uint32_t> ints;
	std::queue<uint32_t> moved;
	
	uint32_t moveObject(uint32_t ptr);
	void moveField(Object * o, uint32_t field, uint32_t fieldSize);
	bool isSpace(uint32_t requested = 1) const;
	
};

#endif // MEMORY_H
