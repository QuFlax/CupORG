//
// Created by quflax on 01.11.23.
//

#ifndef CUPMYLANG_TOKEN_H
#define CUPMYLANG_TOKEN_H

enum token_type {
    T_EOF, T_NL,
    T_IDENTIFIER,

    T_RETURN,
    T_CONTINUE,
    T_BREAK,
    T_IF, T_ELIF, T_ELSE,
    T_FOR, T_WHILE,

    T_NUMBER,
    T_FLOAT,
    T_STRING,
    T_FSTRING,

    T_ADDEQ,	// +=
    T_SUBEQ,	// -=
    T_MULEQ,	// *=
    T_DIVEQ,	// /=
    T_MODEQ,	// %=
    T_ANDEQ,	// &=
    T_OREQ,		// |=
    T_XOREQ,	// ^=
    T_LESSEQ,	// <=
    T_GREATEQ,	// >=
    T_NOTEQ,	// !=

    T_ADDADD,	// ++
    T_SUBSUB,	// --
    T_MULMUL,	// ** pow
    T_DIVDIV,	// // sqrt
    T_MODMOD,	// %% mod then mod to 0-1 float
    T_ANDAND,	// &&
    T_OROR,		// ||
    T_LESSLESS,	// <<
    T_GREATGREAT,//>>
    T_XAND,		// ^^ xand
    T_NOTNOT,	// !! 33

    T_ADD,		// + 43
    T_SUB,		// - 45
    T_MUL,		// * 42
    T_DIV,		// / 47
    T_MOD,		// % 37
    T_AND,		// & 38
    T_OR,		// | 124
    T_LESS,		// < 60
    T_GREAT,	// > 62
    T_XOR,		// ^ 94
    T_NOT,		// ! 33

    T_EQEQ,		// ==
    T_EQ,		// = 61

    T_DOTDOT,	// .. range
    T_DOT,		// . 46
    T_COMMA,	// , 44
    T_COLON,	// : 58
    T_SCOLON,	// ; 59

    T_IMPORT,	// # 35
    T_TERNARY,	// ? 63
    T_AT,		// @ 64
    T_THIS,		// $ 36

    T_NEXTLINE, // / but left
    T_TILDE,	// ~ 126

    //miscellaneous operators
    T_ORBRACKET,	// ( 40
    T_CRBRACKET,	// ) 41
    T_OCBRACKET,	// { 123
    T_CCBRACKET,	// } 125
    T_OSBRACKET,	// [ 91
    T_CSBRACKET		// ] 93
};

static const char* token_types[] = {
        "T_EOF", "T_NL",
        "T_IDENTIFIER",

        "T_RETURN",
        "T_CONTINUE",
        "T_BREAK",
        "T_IF", "T_ELIF", "T_ELSE",
        "T_FOR", "T_WHILE",

        "T_NUMBER",
        "T_FLOAT",
        "T_STRING",
        "T_FSTRING",

        "T_ADDEQ",	// +=
        "T_SUBEQ",	// -=
        "T_MULEQ",	// *=
        "T_DIVEQ",	// /=
        "T_MODEQ",	// %=
        "T_ANDEQ",	// &=
        "T_OREQ",		// |=
        "T_XOREQ",	// ^=
        "T_LESSEQ",	// <=
        "T_GREATEQ",	// >=
        "T_NOTEQ",	// !=

        "T_ADDADD",	// ++
        "T_SUBSUB",	// --
        "T_MULMUL",	// ** pow
        "T_DIVDIV",	// // sqrt
        "T_MODMOD",	// %% mod then mod to 0-1 float
        "T_ANDAND",	// &&
        "T_OROR",		// ||
        "T_LESSLESS",	// <<
        "T_GREATGREAT",//>>
        "T_XAND",		// ^^ xand
        "T_NOTNOT",	// !! 33

        "T_ADD",		// + 43
        "T_SUB",		// - 45
        "T_MUL",		// * 42
        "T_DIV",		// / 47
        "T_MOD",		// % 37
        "T_AND",		// & 38
        "T_OR",		// | 124
        "T_LESS",		// < 60
        "T_GREAT",	// > 62
        "T_XOR",		// ^ 94
        "T_NOT",		// ! 33

        "T_EQEQ",		// ==
        "T_EQ",		// = 61

        "T_DOTDOT",	// .. range
        "T_DOT",		// . 46
        "T_COMMA",	// , 44
        "T_COLON",	// : 58
        "T_SCOLON",	// ; 59

        "T_IMPORT",	// # 35
        "T_TERNARY",	// ? 63
        "T_AT",		// @ 64
        "T_THIS",		// $ 36

        "T_NEXTLINE", // / but left
        "T_TILDE",	// ~ 126

        //miscellaneous operators
        "T_ORBRACKET",	// ( 40
        "T_CRBRACKET",	// ) 41
        "T_OCBRACKET",	// { 123
        "T_CCBRACKET",	// } 125
        "T_OSBRACKET",	// [ 91
        "T_CSBRACKET"		// ] 93
};

struct token {
    const token_type type;
    const char* string;
    size_t offset;
    unsigned int row;
    unsigned int col;
};

#endif //CUPMYLANG_TOKEN_H
