#ifndef OBJECT_H
#define OBJECT_H

#include "class.h"

class Object
{
public:
	Object(Class * type);
	virtual ~Object();
	Class * getType() const;
	uint32_t getValue(uint8_t field) const;
	void setValue(uint8_t field, uint32_t value);

protected:
	Class * type;
	uint32_t * fields;
};

class DoubleObject: public Object
{
public:
	DoubleObject(Class * type);
	virtual ~DoubleObject();
	void setValue(double value);
	double getValue() const;
	
private:
	double value;
};

class StringObject: public Object
{
public:
	StringObject(Class * type);
	~StringObject();
	void setValue(const char * value);
	const char * getValue() const;
	
private:
	char * value;
};

class ArrayObject: public Object
{
public:
	ArrayObject(Class * type, uint32_t length);
	uint32_t getValue(uint32_t field) const;
	void setValue(uint32_t field, uint32_t value);
	
private:
	uint32_t length;
};

#endif // OBJECT_H
