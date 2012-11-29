#ifndef BYTECODECONSTANTS_H
#define BYTECODECONSTANTS_H

#define IDENTIFIER_LENGTH 25
#define INT_CLASS "int"
#define REAL_CLASS "real"
#define STRING_CLASS "string"
#define ARRAY_CLASS "array"
#define FILE_CLASS "file"

enum ConstPoolTag
{
	METHOD_REF = 1,
	CLASS_REF = 2,
	STRING_CONST = 3,
	INT_CONST = 4,
	REAL_CONST = 5
};

#endif // BYTECODECONSTANTS_H
