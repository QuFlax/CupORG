//
// Created by quflax on 01.11.23.
//

#include "main.h"

void push(SLexer& lexer, token_type type, size_t offset, const char* start, const unsigned int size) {
    if (size == 0) {
        lexer.tokens.push_back({ type, "", offset, lexer.row, lexer.col});
        return;
    }
    for (auto e : lexer.strings) {
        if (strncmp(e, start, size) == 0) {
            lexer.tokens.push_back({ type, e, offset, lexer.row, lexer.col });
            return;
        }
    }
    char* pointer = new char[size + 1];
    memcpy(pointer, start, size);
    pointer[size] = '\0';
    lexer.strings.push_back(pointer);
    lexer.tokens.push_back({ type, pointer, offset, lexer.row, lexer.col });
}

void tokenize(SFile& file, SLexer& lexer) {
    const token_type TYPES[93] = { T_NOT, T_EOF, T_IMPORT, T_THIS, T_MOD, T_AND, T_EOF, T_ORBRACKET,
                                T_CRBRACKET, T_MUL, T_ADD, T_COMMA, T_SUB, T_DOT, T_DIV,
                                T_OCBRACKET, T_OR, T_CCBRACKET, T_TILDE, T_EOF, T_EOF, T_EOF, T_EOF, T_EOF, T_EOF,
                                T_COLON, T_SCOLON, T_LESS, T_EQ, T_GREAT, T_TERNARY, T_AT,
                                T_ADDEQ, T_SUBEQ, T_MULEQ, T_DIVEQ, T_MODEQ, T_ANDEQ, T_OREQ, T_XOREQ, T_LESSEQ, T_GREATEQ, T_NOTEQ, T_EQEQ,
                                T_ADDADD, T_SUBSUB, T_MULMUL, T_DIVDIV, T_MODMOD, T_ANDAND, T_OROR, T_XAND, T_LESSLESS, T_GREATGREAT, T_NOTNOT, T_DOTDOT,
                                T_EOF, T_EOF, T_OSBRACKET, T_NEXTLINE, T_CSBRACKET, T_XOR
    };
    const char* TYPES_STR1 = "+-*/%&|^<>!=";
    const char* TYPES_STR2 = "+-*/%&|^<>!.";
    const char* end = file.data + file.size;
    for (const char* point = file.data; point < end; point++) {
		const char cur = *point;
		if (cur == '\r') continue;
		unsigned int size = 0;
		size_t offset = point - file.data;

        switch (cur) {
			case ' ': case '\t':
				lexer.col++;
				break;
            case '\n': {
                lexer.tokens.push_back({ T_NL, nullptr, offset, lexer.row++, lexer.col });
                lexer.col = 0;
				break;
			}
            case '"': {
				const char* start = ++point;
				bool unicode = false;
                while (point < end && *point != '"' && *point != '\r' && *point != '\n') {
                    if (*point >= 32) {
						size++;
						if (unicode) size++;
					} unicode = *(point++) < 0;
				}
				bool error = cur != *point;
				if (error)
					printf("ERROR STRING : missing end of string from offset = %llu\n", offset);
                push(lexer, T_STRING, offset, start, size);
                lexer.col++;
                lexer.col += size;
				if (!error) lexer.col++;
				break;
			}
            case '\'': {
				const char *start = ++point;
				bool unicode = false;
				bool value = false;
                while (point < end && *point != '\'' && *point != '\r' && *point != '\n') {
					if (*point == '}') { value = false; printf("\n"); }
                    if (value) {
						printf("%c",*point);
					}
					if (*point == '{') { value = true; printf("fstr: "); }
                    if (*point >= 32) {
						size++;
						if (unicode) size++;
					} unicode = *(point++) < 0;
				}
				bool error = cur != *point;
				if (error)
					printf("ERROR FORMAT STRING : missing end of string from offset = %llu\n", offset);
                push(lexer, T_FSTRING, offset, start, size);
                lexer.col++;
                lexer.col += size;
				if (!error) lexer.col++;
				break;
			}
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                const char* start = point++;
                const char next = *point;
                bool float_point = false;

                switch (next) {
                case 'b': case 'B':
                    point++;
                    while (point < end && (*point == '_' || *point == '0' || *point == '1')) point++;
                    break;
                case 'o': case 'O':
                    point++;
                    while (point < end && (*point == '_' || (*point >= '0' && *point <= '7'))) point++;
                    break;
                case 'x': case 'X':
                    point++;
                    while (point < end && (*point == '_' || isdigit(*point) || isxdigit(*point))) point++;
                    break;
                default:
                    while (point < end && (*point == '_' || *point == '.' || (*point >= '0' && *point <= '9'))) {
                        if (*point == '.') {
                            if (float_point) {
                                printf("ERROR FLOAT : multiple float points from offset = %llu\n", offset);
                                break;
                            }
                            float_point = true;
                        }
                        point++;
                    }
                }
                size = (point - start);
                //lexer.tokens.push_back({ T_NUMBER, offset, row, col, substring(size, start, lexer.strings) });
                push(lexer, T_NUMBER, offset, start, size);
                lexer.col += size;
                point--;
                break;
            }
            case '+': case '-': case '*': case '/': case '%': case '&':
            case '|': case '<': case '>': case '^': case '!': case '=': {
                if (*(point + 1) == '=') { // 12
                    for (const char* char_ptr = TYPES_STR1; *char_ptr != '\0'; char_ptr++)
                        if (*char_ptr == cur) {
                            lexer.tokens.push_back({ TYPES[char_ptr - TYPES_STR1 + 32], nullptr, offset, lexer.row, lexer.col++});
                            lexer.col++;
                            point++;
                            break;
                        }
                    break;
                }
                [[fallthrough]]; // fallthrough is explicit
            }
            case '.': { // 12
                if (*(point + 1) == cur) {
                    for (const char* char_ptr = TYPES_STR2; *char_ptr != '\0'; char_ptr++)
                        if (*char_ptr == cur) {
                            lexer.tokens.push_back({ TYPES[char_ptr - TYPES_STR2 + 44], nullptr, offset, lexer.row, lexer.col++});
                            lexer.col++;
                            point++;
                            break;
                        }
                    break;
                }
                [[fallthrough]]; // fallthrough is explicit
            }
            case ',':
            case ':':
            case ';':
            case '#':
            case '?':
            case '@':
            case '$':
            case '~':
            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case '\\': {
                unsigned char sd = cur - '!';
                if (sd > 64) sd -= 75;
                lexer.tokens.push_back({ TYPES[sd], nullptr, offset, lexer.row, lexer.col++});
                break;
            }
            default: {
                const char* start = point;
                bool unicode = false;
                while (point < end && (isdigit(*point) || isalpha(*point) || *point == '_' || *point == '`' || *point < 0)) {
                    if (*point >= 32) {
                        size++;
                        if (unicode) size++;
                    } unicode = *(point++) < 0;
                }
                {
                    if (strncmp("if", start, size) == 0)
                        lexer.tokens.push_back({ T_IF, nullptr, offset, lexer.row, lexer.col });
                    else if (strncmp("for", start, size) == 0)
                        lexer.tokens.push_back({ T_FOR, nullptr, offset, lexer.row, lexer.col });
                    else if (strncmp("elif", start, size) == 0)
                        lexer.tokens.push_back({ T_ELIF, nullptr, offset, lexer.row, lexer.col });
                    else if (strncmp("else", start, size) == 0)
                        lexer.tokens.push_back({ T_ELSE, nullptr, offset, lexer.row, lexer.col });
                    else if (strncmp("break", start, size) == 0)
                        lexer.tokens.push_back({ T_BREAK, nullptr, offset, lexer.row, lexer.col });
                    else if (strncmp("while", start, size) == 0)
                        lexer.tokens.push_back({ T_WHILE, nullptr, offset, lexer.row, lexer.col });
                    else if (strncmp("return", start, size) == 0)
                        lexer.tokens.push_back({ T_RETURN, nullptr, offset, lexer.row, lexer.col });
                    else if (strncmp("continue", start, size) == 0)
                        lexer.tokens.push_back({ T_CONTINUE, nullptr, offset, lexer.row, lexer.col });
                    else
                        push(lexer, T_IDENTIFIER, offset, start, size);
                }
                point--;
                lexer.col += size;
                break;
            }
        }
    }
}