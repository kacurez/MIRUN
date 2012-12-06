/**
 * @author Jakub Valenta
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include "object.h"
#include "stackframe.h"
#include <stdio.h>

template<class T> class Page
{
public:
	Page(T used, T empty, uint32_t maxSize): used(used), empty(empty), size(0), maxSize(maxSize) {}
	
	~Page()
	{
		delete [] used;
		delete [] empty;
	}
	
	bool isSpace(uint32_t requested = 1) const
	{
		return requested + size < maxSize;
	}
	
	void swap()
	{
		std::swap(used, empty);
	}
	
	T used;
	T empty;
	uint32_t size;
	uint32_t maxSize;
};

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
	FILE * getFileStream(uint32_t file);
	void gc(StackFrame * stack);

private:
	Page<Object *> memory;
	Page<uint32_t *> fields;
	std::map<int, uint32_t> ints;
	Page<double *> doubles;
	Page<char *> strings;
	Page<FILE **> files;
	
	uint32_t moveObject(uint32_t ptr);
	
};

#endif // MEMORY_H
