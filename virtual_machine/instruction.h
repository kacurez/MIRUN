#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

enum INSTRUCTION  {
	ADD = 0,
	SUB,
	MUL,
	DIV,
	/**
	 * Compare references if equal.
	 */
	CMP_EQ,
	/**
	 * Compare references if not equal.
	 */
	CMP_NE,
	INC,
	DEC,
	NEW,
	NEW_ARRAY,
	JMP,
	CALL,
	RET,
	RET_VOID,
	IF_EQ,
	IF_GT,
	IF_LT,
	IF_GE,
	IF_LE,
	IF_NE,
	PUSH,
	LOAD,
	STORE,
	LOAD_LOCAL,
	STORE_LOCAL,
	POP,
	CALL_DYNAMIC,
	LOAD_ARRAY,
	STORE_ARRAY
};

const char * mnemonics[] = {
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"CMP_EQ",
	"CMP_NE",
	"INC",
	"DEC",
	"NEW",
	"NEW_ARRAY",
	"JMP",
	"CALL",
	"RET",
	"RET_VOID",
	"IF_EQ",
	"IF_GT",
	"IF_LT",
	"IF_GE",
	"IF_LE",	
	"IF_NE",
	"PUSH",
	"LOAD",
	"STORE",
	"LOAD_LOCAL",
	"STORE_LOCAL",
	"POP",
	"CALL_DYNAMIC",
	"LOAD_ARRAY",
	"STORE_ARRAY"
};
#endif
