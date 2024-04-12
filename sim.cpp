#include "main.h"

static std::vector<void*> vars;
static std::vector<void*> scope;

const char* evalue(SParser& parser, expresion ex) {
    switch (ex.type) {
    case E_VALUE: {
        switch (ex.value.type) {
        case V_IDENTIFIER: {
            return ex.value.name;
        }
        }
    }
    }
}

void simstatement(SParser& parser, statement st);

void simexpresion(SParser& parser, expresion ex) {
    switch (ex.type) {
    case E_FUNCTION: {
        const char* name = ex.value.name;
        for (size_t i = 0; i < parser.values.size(); i++) {
            if (parser.values[i].name == name) {
                printf("define function %s %zu\n", name, i);
                vars[i] = ex.block;
                return;
            }
        }
        fprintf(stderr, "ERROR: function %s\n", name);
        exit(1);
        return;
    }
    case E_EQ: {
        const char* name = evalue(parser, *ex.left);
        /*auto it = std::find_if(vars.begin(), vars.end(), [name](std::pair<const char*, void*> var) {
            return strcmp(var.first, name) == 0;
        });
        if (it->second == nullptr) {
            it->second = (void*)value(*ex.right);
        }
        else {
            printf("ERROR: Variable %s already defined\n", name);
            it->second = ex.right;
        }*/
        return;
    }
    case E_CALL: {
		const char* name = evalue(parser, *ex.left);
        printf("call %s\n", name);
        for (size_t i = 0; i < parser.values.size(); i++) {
            if (parser.values[i].name == name) {
                printf("call function %s %zu\n", name, i);
                return;
            }
        }
		fprintf(stderr, "ERROR: function %s\n", name);
        exit(1);
    }
    }
}

void simstatement(SParser& parser, statement st) {
    switch (st.type) {
    case S_EXPRESION: {
        simexpresion(parser, *st.exprs);
        break;
    }
    case S_BLOCK: {
        for (size_t i = 0; i < st.size; i++)
            simstatement(parser, st.data[i]);
        break;
    }
    }
}

void sim(SParser& parser) {
	for (size_t i = 0; i < parser.values.size(); i++)
        vars.push_back(nullptr);

    printf("size %zu uint %zu\n", parser.result.size, parser.result.uint);
    // add call main
    expresion mainvalue = { E_VALUE, nullptr, nullptr, nullptr, { V_IDENTIFIER, "main" } };
    expresion callmainexpr = { E_CALL, &mainvalue, nullptr };
    statement callmain = { S_EXPRESION, &callmainexpr };

    parser.result.data[parser.result.size++] = callmain;
    printf("size %zu uint %zu\n", parser.result.size, parser.result.uint);

    simstatement(parser, parser.result);
    //for (auto st : parser.result) \
        simstatement(parser, st);
}