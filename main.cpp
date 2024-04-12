//
// Created by quflax on 01.11.23.
//

#include "main.h"

struct Option {
    bool usage;
    bool version;
    bool position;
    bool nowarnings;
    bool x32;
    bool debug;
};

void printUsage() {
    printf("Usage:\tcup [options] <file>\nOptions:\n"
        "-? / -h\t\tShow this help\n"
        "-v\t\tShow the version\n"
        "-n\t\tNo warnings\n"
        "-x\t\tCompile x32\n"
        "-d\t\tAdd DebugInfo\n"
        "-p\t\t0:1 -> 1:2\n");
}

#define tochar(X)  {\
    char buffer[10];\
    char* ptr = buffer;\
    cryuint(ptr, X);\
    WriteFile(hFile, buffer, ptr - buffer, NULL, NULL);\
}

void inline fixpos(std::vector<token>& tokens) {
    for (auto& t : tokens) { t.col++; t.row++; }
}

bool readS(SFile& file) {
    HANDLE hFile = CreateFileA(file.name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("%i\n", GetLastError());
        return false;
    }
    file.size = GetFileSize(hFile, NULL);
    file.data = new char[file.size];
    bool bRead = ReadFile(hFile, file.data, file.size, NULL, NULL);
    CloseHandle(hFile);
    return bRead;
    //std::ifstream ifs(file.name);
    //if (!ifs.is_open()) return false;
    //ifs.seekg(0, std::ios::end);
    //file.size = ifs.tellg();
    //ifs.seekg(0, std::ios::beg);
    //file.data = new char[file.size];
    //ifs.read(file.data, file.size);
    //ifs.close();
    //return true;
}

void cryuint(char*& ptr, unsigned int number) { // 10 digits no terminator
    unsigned int divisor = 1000000000;
    while (divisor > 0) {
        unsigned int digit = number / divisor;
        number %= divisor;
        if (divisor == 1 || digit != 0)
            *ptr++ = digit + '0';
        divisor /= 10;
    }
}

void cry(char*& ptr, const char* src) {
    while (*src) *ptr++ = *src++;
}

void printstatement(std::ofstream& ofs, statement st);

void printexpresion(std::ofstream& ofs, expresion ex) {
	//ofs << expr_types[ex.type] << "\n";
    switch (ex.type) {
        case E_VALUE: {
			struct value val = ex.value;
            switch (val.type) {
                case V_STRING: case V_FSTRING:
                    ofs << "\"" << val.svalue << "\"";
					break;
                case V_IDENTIFIER: ofs << val.name; break;
                case V_NUMBER: ofs << val.ivalue; break;
                case V_OBJECT: ofs << "{}"; break;
                case V_THIS: ofs << "this"; break;
			}
			break;
		}
        case E_FUNCTION: {
            if (ex.value.name != nullptr)
                ofs << ex.value.name;
            ofs << " ( ";
            if (ex.left != nullptr)
				printexpresion(ofs, *ex.left);
            ofs << " ) ";
            printstatement(ofs, *ex.block);
            break;
        }
        case E_EQ: {
			printexpresion(ofs, *ex.left);
            ofs << " = ";
            //ofs << expr_types[ex.right->type];
            printexpresion(ofs, *ex.right);
			break;
        }
        case E_ADD: {
			printexpresion(ofs, *ex.left);
			ofs << " + ";
			printexpresion(ofs, *ex.right);
            break;
        }
        case E_ADDEQ: {
			printexpresion(ofs, *ex.left);
            ofs << " += ";
            printexpresion(ofs, *ex.right);
            break;
        }
        case E_OREQ: {
			printexpresion(ofs, *ex.left);
			ofs << " |= ";
			printexpresion(ofs, *ex.right);
			break;
        }
        case E_EQEQ: {
            printexpresion(ofs, *ex.left);
			ofs << " == ";
            printexpresion(ofs, *ex.right);
            break;
        }
        case E_INDEX: {
			printexpresion(ofs, *ex.left);
            ofs << " [ ";
            if (ex.right) printexpresion(ofs, *ex.right);
            ofs << " ] ";
			break;
		}
        case E_DOT: {
            printexpresion(ofs, *ex.left);
            if (ex.right == nullptr) {
                fprintf(stderr, "ERR DOT\n");
                exit(1);
            }
            ofs << " . ";
            printexpresion(ofs, *ex.right);
            break;
        }
        case E_CALL: {
            printexpresion(ofs, *ex.left);
            ofs << " ( ";
            if (ex.right) printexpresion(ofs, *ex.right);
            ofs << " ) ";
            break;
        }
        case E_COMMA: {
            //ofs << expr_types[ex.left->type];
			printexpresion(ofs, *ex.left);
			ofs << " , ";
            if (ex.right) printexpresion(ofs, *ex.right);
            break;
		}
	}
}

void printstatement(std::ofstream& ofs, statement st) {
	//ofs << "\n" << statement_types[st.type] << "\n";
    switch (st.type) {
    case S_EXPRESION: {
        printexpresion(ofs, *st.exprs);
        break;
    }
    case S_RETURN: {
        ofs << "RETURN ";
        printexpresion(ofs, *st.exprs);
        break;
    }
    case S_IF: case S_WHILE: {
		ofs << "IF SIZE " << st.size << "\n";
		printexpresion(ofs, *st.exprs);
		ofs << "\n";
        if (st.size == 1) {
            printstatement(ofs, st.data[0]);
        }
        else {
            printstatement(ofs, st.data[0]);
            ofs << "\nELSE\n";
            printstatement(ofs, st.data[1]);
        }
        break;
    }
    case S_BLOCK: {
        //ofs << "\nSIZE " << st.size << "\n";
        switch (st.size) {
        case 0: {
            ofs << "{ ... }";
            return;
        }
        case 1: {
            printstatement(ofs, st.data[0]);
            return;
        }
        default: {
            ofs << "{\n";
            for (size_t i = 0; i < st.size; i++) {
                if (i > 0) ofs << "\n";
                printstatement(ofs, st.data[i]);
            }
            ofs << "\n}";
            return;
        }
        }
    }
    }
}

bool writeS(const char* fname, SParser parser) {
    std::ofstream ofs(fname);
    if (!ofs.is_open()) return false;
    for (auto import : parser.imports) {
		ofs << "IMPORT " << import << "\n";
	}
    for (auto dep : parser.dependencies) {
		ofs << "DEPENDENCY " << dep.first << " " << dep.second << "\n";
	}
    printstatement(ofs, parser.result);
    //for (auto st : parser.result) { \
        ofs << "\n"; \
        printstatement(ofs, st); \
	}
    for (auto value : parser.values) {
        ofs << "\nVALUE ";
        switch (value.type) {
            case V_STRING: case V_FSTRING:
                ofs << "\"" << value.svalue << "\"";
                break;
            case V_IDENTIFIER: ofs << "ID " << value.name; break;
            case V_NUMBER: ofs << value.ivalue; break;
            case V_OBJECT: ofs << "{}"; break;
        }
    }
    ofs.close();
    return true;
}

bool writeS(const char* fname, SLexer& lexer) {
    HANDLE hFile = CreateFileA(fname, GENERIC_WRITE, 1, NULL, CREATE_ALWAYS, NULL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    for (auto str : lexer.strings) {
        WriteFile(hFile, "strings: ", 9, NULL, NULL);
        WriteFile(hFile, str, strlen(str), NULL, NULL);
        WriteFile(hFile, "\n", 1, NULL, NULL);
        //ofs << "strings: " << str.len << " " << str.pointer << "\n";
    }
    for (token& token : lexer.tokens) {
        if (token.string) {
            WriteFile(hFile, "{", 1, NULL, NULL);
            WriteFile(hFile, token_types[token.type], strlen(token_types[token.type]), NULL, NULL);
            WriteFile(hFile, ", ", 2, NULL, NULL);
            WriteFile(hFile, token.string, strlen(token.string), NULL, NULL);
            WriteFile(hFile, ", ", 2, NULL, NULL);
            tochar(token.offset);
            WriteFile(hFile, "(", 1, NULL, NULL);
            tochar(token.row);
            WriteFile(hFile, ", ", 2, NULL, NULL);
            tochar(token.col);
            WriteFile(hFile, ")}\n", 3, NULL, NULL);
            //ofs << "{" << token_types[token.type] << ", " << token.string << ", " <<
                //token.offset << "(" << token.row << ", " << token.col << ")}\n";
        }
        else {
            char buffer[10];
            char* ptr = buffer;
            WriteFile(hFile, "{", 1, NULL, NULL);
            WriteFile(hFile, token_types[token.type], strlen(token_types[token.type]), NULL, NULL);
            WriteFile(hFile, ", ", 2, NULL, NULL);
            tochar(token.offset);
            WriteFile(hFile, "(", 1, NULL, NULL);
            tochar(token.row);
            WriteFile(hFile, ", ", 2, NULL, NULL);
            tochar(token.col);
            WriteFile(hFile, ")}\n", 3, NULL, NULL);
            //ofs << "{" << token_types[token.type] << ", " <<
                //token.offset << "(" << token.row << ", " << token.col << ")}\n";
        }
    }
    CloseHandle(hFile);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printUsage();
        return 1;
    }

    Option options = { 0 };
    SFile file = { 0 };
    //std::vector<CupFile> files;

    for (size_t i = 1; i < argc; i++) {
        const char* arg = argv[i];
        if (arg[0] == '-') {
            switch (arg[1]) {
                case 'v': case 'V': options.version = true; break;
                case 'n': case 'N': options.nowarnings = true; break;
                case 'x': case 'X': options.x32 = true; break;
                case 'd': case 'D': options.debug = true; break;
                case 'p': case 'P': options.position = true; break;
                case '?': case 'H': case 'h': default:
                    options.usage = true; break;
            }
        } else {
            //SFile file = { .name = arg };
            file.name = arg;
            if (!readS(file)) {
                fprintf(stderr, "ERROR: Not Found %s\n", arg);
                return 1;
            }
            //if (readCup(file))
            //    files.emplace_back(file);
        }
    }

    if (options.version)    printf("Cup 1.0\n");
    if (options.usage)      printUsage();
    if (file.data == nullptr) {
        fprintf(stderr, "ERROR: No file provided\n");
		return 1;
    }
    SLexer lexer{};
    tokenize(file, lexer);
    if (options.position)
        fixpos(lexer.tokens);

    delete[] file.data;
    file.data = nullptr;

    const int len = strlen(file.name);
    char* tks_filename = new char[len + 5];
    strncpy(tks_filename, file.name, len);
    strncpy(tks_filename + len, ".tks", 4);
    tks_filename[len + 4] = '\0';

    SParser parser{};
    parse(lexer, parser);

    writeS(tks_filename, parser);
    sim(parser);

    return 0;
}