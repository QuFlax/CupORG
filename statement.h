//
// Created by quflax on 27.11.23.
//

#ifndef CUPMYLANG_STATEMENT_H
#define CUPMYLANG_STATEMENT_H

#include <vector>
#include "token.h"

enum value_type {
    V_IDENTIFIER,
    V_NUMBER,
    V_FLOAT,
    V_STRING,
    V_FSTRING,
    V_THIS,
    V_OBJECT
};
struct value {
    value_type type;
    const char* name;
    union {
        const char* svalue;
        size_t ivalue;
		double fvalue;
        void* pvalue;
	};
};
enum expr_type {
    E_COMMA,

    E_EQ,
    E_ADDEQ,
    E_SUBEQ,
    E_MULEQ,
    E_DIVEQ,
    E_MODEQ,
    E_ANDEQ,
    E_OREQ,
    E_XOREQ,

    E_TENARY,

    E_NOT,
    E_ASSERT,
    E_OROR,
    E_ANDAND,

    E_EQEQ,
    E_NOTEQ,
    E_LESS,
    E_LESSEQ,
    E_GREAT,
    E_GREATEQ,

    E_PRE_INC,
    E_PRE_DEC,
    E_POST_INC,
    E_POST_DEC,

    E_ADD,
    E_SUB,
    E_MUL,
    E_DIV,
    E_MOD,

    E_POW,
    E_SQRT,
    E_MODMOD,
    E_XAND,

    E_AND,
    E_OR,
    E_XOR,
    E_LSHIFT,
    E_RSHIFT,

    E_DOT,
    E_INDEX,
    E_VALUE,
    E_CALL,

    E_FUNCTION
};
static const char* expr_types[] = {
	"E_COMMA",

	"E_EQ",
	"E_ADDEQ",
	"E_SUBEQ",
	"E_MULEQ",
	"E_DIVEQ",
	"E_MODEQ",
	"E_ANDEQ",
	"E_OREQ",
	"E_XOREQ",

	"E_TENARY",

	"E_NOT",
	"E_ASSERT",
	"E_OROR",
	"E_ANDAND",

	"E_EQEQ",
	"E_NOTEQ",
	"E_LESS",
	"E_LESSEQ",
	"E_GREAT",
	"E_GREATEQ",

	"E_PRE_INC",
	"E_PRE_DEC",
	"E_POST_INC",
	"E_POST_DEC",

	"E_ADD",
	"E_SUB",
	"E_MUL",
	"E_DIV",
	"E_MOD",

	"E_POW",
	"E_SQRT",
	"E_MODMOD",
	"E_XAND",

	"E_AND",
	"E_OR",
	"E_XOR",
	"E_LSHIFT",
	"E_RSHIFT",

	"E_DOT",
	"E_INDEX",
	"E_VALUE",
	"E_CALL",

	"E_THIS",
	"E_FUNCTION"
};

struct statement;
struct expresion {
    expr_type type;
    expresion* left;
    expresion* right;
    expresion* telse;
    struct value value;
    statement* block;
};

enum statement_type {
    S_BLOCK,
    S_IF,
    S_WHILE,
    S_FOR,
    S_BREAK,
    S_CONTINUE,
    S_RETURN,
    S_EXPRESION,
    S_FUNCTION
};

static const char* statement_types[] = {
    "S_BLOCK",
    "S_IF",
    "S_WHILE",
    "S_FOR",
    "S_BREAK",
    "S_CONTINUE",
    "S_RETURN",
    "S_EXPRESION",
    "S_FUNCTION"
};

struct statement {
    statement_type type;
    expresion* exprs;
    unsigned int uint;
    const char* name;
    unsigned int size;
    statement* data;
};


#endif //CUPMYLANG_STATEMENT_H
