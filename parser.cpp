//
// Created by quflax on 01.11.23.
//

#include "main.h"

//#include "list"

typedef std::vector<token>::const_iterator iter;

static std::vector<value> values;
static iter begin, end;
static bool nofunc = false;

#define check(X) (begin->type == X)
#define expect(X) \
if (!next(X)) {\
	fprintf(stderr, "Parser ERR: expected %s but got %s, %s, %zu(%i, %i)\n", token_types[X], token_types[begin->type], begin->string, begin->offset, begin->row, begin->col);\
	exit(1);\
}
//#define addstatement(X)         \
X.data[X.size++] = Statement(); \
if (X.size == X.uint)           \
    assert(X.data = (statement*)realloc(X.data, (X.uint *= 2) * sizeof(statement)), "realloc failed");                                           \

void assert(bool b, const char* err) {
    if (!b) {
        fprintf(stderr, "Parser ERR: %s\n", err);
        exit(1);
    }
}

void _skip(size_t line, const char* func) {
    //printf("skip %zu %s\n", line, func);
    assert(begin++ != end, "unexpected end of file");
}
#define skip() _skip(__LINE__, __FUNCTION__)

bool next(token_type type) {
    if (begin == end)
        return false;
    bool b = check(type);
    if (b)
        skip();
    return b;
}

void checkstatement(statement& s) {
    assert(s.uint != 0, "checkstatement");
    if (s.size == s.uint) {
        s.uint *= 2;
        statement* data = (statement*)realloc(s.data, s.uint * sizeof(statement));
        assert(data != nullptr, "realloc failed");
        s.data = data;
    }
}

#define checkvalue(X) for (auto& v : values) if (v.name == X) return new expresion[1]{ { E_VALUE, nullptr, nullptr, nullptr, v } };
#define addvalue(X) values.push_back(X); return new expresion[1]{ { E_VALUE, nullptr, nullptr, nullptr, values.back() } };

expresion* Expresion();
statement Statement();

expresion* valueex() {
    const char* id = begin->string;
    switch (begin->type) {
    case T_IDENTIFIER: {
        skip(); checkvalue(id);
        value v = { V_IDENTIFIER, id };
        addvalue(v);
    }
    case T_NUMBER: {
		skip(); checkvalue(id);
		value v = { V_NUMBER, id }; v.ivalue = atoll(id);
        addvalue(v);
    }
    case T_FLOAT: {
		skip(); // a
        checkvalue(id);
		value v = { V_FLOAT, id }; v.fvalue = atof(id);
        addvalue(v);
    }
    case T_STRING: case T_FSTRING: {
        skip(); checkvalue(id);
        value v = { V_STRING, id, id };
        addvalue(v);
    }
    case T_THIS: {
		skip();
		return new expresion[1]{ { E_VALUE, nullptr, nullptr, nullptr, { V_THIS } } };
    }
    case T_ORBRACKET: {
		skip(); // (
        while (next(T_NL));
		expresion* inside = Expresion();
		expect(T_CRBRACKET); // )
        if (!nofunc && check(T_OCBRACKET)) { // { function
			return new expresion[1]{ { E_FUNCTION, inside, nullptr, nullptr,
                { V_IDENTIFIER, "_" }, new statement[1]{Statement()}}};
		}
		return inside;
    }
    case T_OCBRACKET: { 
        skip(); // {
		while (next(T_NL));
		expresion* inside = Expresion();
		expect(T_CCBRACKET); // }
        return new expresion[1]{ { E_VALUE, inside, nullptr, nullptr, { V_OBJECT } } };
    }
    default:
        fprintf(stderr, "Parser ERR: unexpected %s, %s, %zu(%i, %i)\n", token_types[begin->type], begin->string, begin->offset, begin->row, begin->col);
        exit(1);
    }
}

expresion* member() {
    expresion* ex = valueex();
	if (next(T_DOT)) // a.b.c
		return new expresion[1]{ { E_DOT, ex, member() } };
	if (next(T_OSBRACKET)) { // a[...]
        expresion* inside = Expresion();
        expect(T_CSBRACKET);
        return new expresion[1]{ { E_INDEX, ex, inside } };
    }
	if (next(T_ORBRACKET)) { // a(...)
        while (next(T_NL));
        expresion* args = Expresion();
        expect(T_CRBRACKET);
        if (!nofunc && check(T_OCBRACKET)) { // function
            statement* body = new statement[1]{ Statement() };
            if (ex->type == E_VALUE)
                return new expresion[1]{ { E_FUNCTION, args, nullptr, nullptr, ex->value , body } };
            return new expresion[1]{ { E_FUNCTION, args, nullptr, nullptr, { V_IDENTIFIER, "_" }, body}};
        }
        return new expresion[1]{ { E_CALL, ex, args } };
    }
	return ex;
}

expresion* prepost() {
    if (next(T_ADDADD)) // ++a
		return new expresion[1]{ { E_PRE_INC, member() } };
    if (next(T_SUBSUB)) // --a
        return new expresion[1]{ { E_PRE_DEC, member() } };
    expresion* ex = member();
    if (next(T_ADDADD)) // a++
		return new expresion[1]{ { E_POST_INC, ex } };
	if (next(T_SUBSUB)) // a--
		return new expresion[1]{ { E_POST_DEC, ex } };
	return ex;
}

expresion* bitlogic() {
	expresion* ex = prepost();
	if (next(T_AND)) // a & b & ...
		return new expresion[1]{ { E_AND, ex, bitlogic() } };
	if (next(T_OR)) // a | b | ...
		return new expresion[1]{ { E_OR, ex, bitlogic() } };
	if (next(T_XOR)) // a ^ b ^ ...
		return new expresion[1]{ { E_XOR, ex, bitlogic() } };
	if (next(T_LESSLESS)) // a << b << ...
		return new expresion[1]{ { E_LSHIFT, ex, bitlogic() } };
	if (next(T_GREATGREAT)) // a >> b >> ...
		return new expresion[1]{ { E_RSHIFT, ex, bitlogic() } };
	return ex;
}

expresion* powsqrtmmdxand() {
    expresion* ex = bitlogic();
	if (next(T_MULMUL)) // a ** b ** ...
		return new expresion[1]{ { E_POW, ex, powsqrtmmdxand() } };
	if (next(T_DIVDIV)) // a sqrt b sqrt ...
		return new expresion[1]{ { E_SQRT, ex, powsqrtmmdxand() } };
	if (next(T_MODMOD)) // a modmod b modmod ...
		return new expresion[1]{ { E_MODMOD, ex, powsqrtmmdxand() } };
	if (next(T_XAND)) // a xand b xand ...
		return new expresion[1]{ { E_XAND, ex, powsqrtmmdxand() } };
	return ex;
}

expresion* muldivmod() {
    expresion* ex = powsqrtmmdxand();
	if (next(T_MUL)) // a * b * ...
		return new expresion[1]{ { E_MUL, ex, muldivmod() } };
	if (next(T_DIV)) // a / b / ...
		return new expresion[1]{ { E_DIV, ex, muldivmod() } };
	if (next(T_MOD)) // a % b % ...
		return new expresion[1]{ { E_MOD, ex, muldivmod() } };
	return ex;
}

expresion* addsub() {
    expresion* ex = muldivmod();
    if (next(T_ADD)) // a + b + ...
        return new expresion[1]{ { E_ADD, ex, addsub() } };
    if (next(T_SUB)) // a - b - ...
        return new expresion[1]{ { E_SUB, ex, addsub() } };
    return ex;

}

expresion* fboolean() {
    expresion* ex = addsub();
    if (next(T_EQEQ)) // a == b == ...
        return new expresion[1]{ { E_EQEQ, ex, fboolean() } };
    if (next(T_NOTEQ)) // a != b != ...
        return new expresion[1]{ { E_NOTEQ, ex, fboolean() } };
    if (next(T_LESS)) // a < b < ...
        return new expresion[1]{ { E_LESS, ex, fboolean() } };
    if (next(T_GREAT)) // a > b > ...
        return new expresion[1]{ { E_GREAT, ex, fboolean() } };
    if (next(T_LESSEQ)) // a <= b <= ...
        return new expresion[1]{ { E_LESSEQ, ex, fboolean() } };
    if (next(T_GREATEQ)) // a >= b >= ...
        return new expresion[1]{ { E_GREATEQ, ex, fboolean() } };
    return ex;
}

expresion* logic() {
    if (next(T_NOT)) // ! ... a
        return new expresion[1]{ { E_NOT, logic() } };
    if (next(T_NOTNOT)) // !! ... a
		return new expresion[1]{ { E_ASSERT, logic() } };
    expresion* ex = fboolean();
    if (next(T_OROR)) // a || b || ...
        return new expresion[1]{ { E_OROR, ex, logic() } };
    if (next(T_ANDAND)) // a && b && ...
        return new expresion[1]{ { E_ANDAND, ex, logic() } };
    return ex;
}

expresion* ternary() {
	expresion* ex = logic();
    if (next(T_TERNARY)) { // a ? (b ? ... : ...) : (c ? ... : ...)
        expresion* trueex = ternary();
        expect(T_COLON);
        expresion* falseex = ternary();
        return new expresion[1]{ { E_TENARY, ex, trueex, falseex } };
	}
	return ex;
}

expresion* assign() {
    expresion* ex = ternary();
    if (next(T_EQ)) // a = b = ...
        return new expresion[1]{ { E_EQ, ex, assign() } };
    if (next(T_ADDEQ)) // a += b
        return new expresion[1]{ { E_ADDEQ, ex, ternary() } };
    if (next(T_SUBEQ)) // a -= b
        return new expresion[1]{ { E_SUBEQ, ex, ternary() } };
    if (next(T_MULEQ)) // a *= b
        return new expresion[1]{ { E_MULEQ, ex, ternary() } };
    if (next(T_DIVEQ)) // a /= b
        return new expresion[1]{ { E_DIVEQ, ex, ternary() } };
    if (next(T_MODEQ)) // a %= b
        return new expresion[1]{ { E_MODEQ, ex, ternary() } };
    if (next(T_ANDEQ)) // a &= b
        return new expresion[1]{ { E_ANDEQ, ex, ternary() } };
    if (next(T_OREQ)) // a |= b
        return new expresion[1]{ { E_OREQ, ex, ternary() } };
    if (next(T_XOREQ)) // a ^= b
        return new expresion[1]{ { E_XOREQ, ex, ternary() } };
    return ex;
}

expresion* Expresion() {
    if (check(T_CRBRACKET) || check(T_CCBRACKET) || check(T_CSBRACKET))
        return nullptr;
    if (next(T_NL)) {
        fprintf(stderr, "Parser ERR: unexpected %s, %s, %zu(%i, %i)\n", token_types[begin->type], begin->string, begin->offset, begin->row, begin->col);
        exit(1);
    }
	expresion* ex = assign();
    if (next(T_COMMA)) { // a, ...
        while (next(T_NL));
        expresion* inside = Expresion();
        return new expresion[1]{ { E_COMMA, ex, inside } };
    }
	return ex;
}

statement Statement() {
    if (next(T_OCBRACKET)) { // {
        statement block = { S_BLOCK, nullptr, 1, nullptr, 0, new statement[1] };
        while (next(T_NL));
        while (begin->type != T_CCBRACKET) {
            block.data[block.size++] = Statement();
            checkstatement(block);
            while (next(T_NL));
        }
		skip(); // }
		return block;
    }
    if (check(T_IF) || check(T_WHILE)) { // if while
        statement block = { check(T_IF) ? S_IF : S_WHILE, nullptr, 2, nullptr, 0, new statement[2] };
        skip(); // if or while

        nofunc = true;
        block.exprs = Expresion(); // condition
        if (block.exprs->type != E_FUNCTION) {
            while (next(T_NL));
            block.data[block.size++] = Statement(); // block
        } else {
            //return new expresion[1]{ { E_FUNCTION, args, nullptr, nullptr, { V_IDENTIFIER, "_" }, body } };
            //block.exprs = block.exprs->block;
            //block.data[block.size++] = *block.exprs->block; // block
            block.size++;
        }
        nofunc = false;

        iter backup = begin;
        while (true) {
            while (next(T_NL));
            if (next(T_ELIF)) {
                statement st = { S_IF, nullptr, 1, nullptr, 0, new statement[1] };
                st.exprs = Expresion(); // condition
                st.data[st.size++] = Statement(); // block

                block.data[block.size++] = Statement(); // else block
                checkstatement(block);
            } else
				break;
        }
        if (next(T_ELSE)) {
            while (next(T_NL));
            block.data[block.size++] = Statement(); // else block
            //checkstatement(block);
        }
        if (block.size == 1)
            begin = backup;
        return block;
    }
    if (next(T_FOR)) {
        statement st = { S_FOR };
        // todo
        return st;
    }
    if (next(T_BREAK)) {
        statement st = { S_BREAK };
        // todo
        return st;
    }
    if (next(T_CONTINUE)) {
        statement st = { S_CONTINUE };
		// todo
		return st;
	}
    if (next(T_RETURN)) {
        expresion* ex = Expresion();
		return { S_RETURN, ex };
	}
#if 0
    if (check(T_IDENTIFIER)) {
        iter backup = begin;
        skip(); // identifier
		if (next(T_ORBRACKET)) { // (
            statement st = { S_FUNCTION };
			st.name = backup->string;
            st.exprs = Expresion();
            expect(T_CRBRACKET);
			if (check(T_OCBRACKET)) { // function
                st.data = new statement[] { Statement() };
				return st;
			}
		}
        // not function
        begin = backup;
    }
#endif // 0
    return { S_EXPRESION, Expresion() };
    // Declarations https://en.cppreference.com/w/cpp/language/declarations
    // Initialization https://en.cppreference.com/w/cpp/language/initialization
    // expr https://en.cppreference.com/w/cpp/language/expressions
}

bool checkdependency(SParser& parser, const char* dll) {
    for (auto e : parser.dependencies)
        if (e.first == dll && e.second == begin->string)
            return true;
	return false;
}

void parse(SLexer& lexer, SParser& parser) {
    begin = lexer.tokens.cbegin();
    end = lexer.tokens.cend();

    parser.result = { S_BLOCK, nullptr, 1, nullptr, 0, new statement[1] };

	while (begin != end) {
        while (next(T_NL));
		if (next(T_IMPORT)) {
            do {
                assert(check(T_STRING) || check(T_FSTRING) || check(T_IDENTIFIER), "no import");
                parser.imports.push_back(begin->string);
                skip();
            } while (next(T_COMMA));
		} else if (next(T_TILDE)) {
			assert(check(T_STRING) || check(T_FSTRING), "no dll");
            const char* dll = begin->string;
            skip();
			expect(T_COMMA);
			do {
				while (next(T_NL));
				assert(begin->type == T_IDENTIFIER, "need _dllfuncs");
                if (!checkdependency(parser, dll))
                    parser.dependencies.push_back({ dll, begin->string });
				skip();
			} while (next(T_COMMA));
		} else {
            parser.result.data[parser.result.size++] = Statement();
            checkstatement(parser.result);
		}
        if (begin != end) {
            expect(T_NL);
            while (next(T_NL));
        }
	}
    parser.values = values;
}