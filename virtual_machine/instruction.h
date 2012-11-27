#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

enum INSTRUCTION  {
	ADD = 0,
	SUB,
	MUL,
	DIV,
	CMP_EQ,
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
	CALL_DYNAMIC
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
	"POP"
};
#endif
