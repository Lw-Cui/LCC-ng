#ifndef LCC_NG_LCC_H
#define LCC_NG_LCC_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <zconf.h>
#include "ADT.h"

static char *arguments_reg[][4] = {
        {
                "dil",
                "di",
                "edi",
                "rdi",
        },
        {

                "sil",
                "si",
                "esi",
                "rsi",
        },
        {
                "dl",
                "dx",
                "edx",
                "rdx",
        },
        {
                "cl",
                "cx",
                "ecx",
                "rcx",
        }
};


static char op_suffix[] = {
        'b', 'w', 'l', 'q',
};

static int actual_size[] = {
        1, 2, 4, 8,
};

typedef enum Size_type {
    BYTE = 0,
    WORD,
    LONG_WORD,
    QUAD_WORD,
} Size_type;

typedef enum Attribute {
    data_type,                  // int
    identifier,                 // a
    parameter,                  // int a
    parameter_list,             // int a, int b
    func_declarator,            // foo(int a, int b)
    function_definition,        // int foo(int a, int b) { return a; }
    function_declaration,       // int foo(int a, int b)

    new_scope,                  // { ... }
    statement,                  // ...

    local_var,                  // int a = 6, b = 5
    init_list,                  // a = 6, b = 5
} Attribute;

typedef enum Data_type {
    char_type = BYTE,
    int_type = LONG_WORD,
} Data_type;

typedef struct Assembly {
    List_node *beg, *end;
} Assembly;

typedef struct Symbol {
    Attribute attr;
    String *name;
    Data_type basic_type;
    int offset, rsp;
    Assembly *code;
    struct Symbol *parent;
    Vector *param, *init_list;
} Symbol;

Symbol *make_symbol();

void free_symbol(Symbol *sym);

Symbol *make_identifier(char *);

void make_new_scope();

void destroy_scope();

Symbol *make_data_type(Data_type type);

Symbol *make_parameter_declaration(Symbol *type, Symbol *name);

Symbol *make_parameter_list(Symbol *decl);

Symbol *parameter_list_push_back(Symbol *list, Symbol *decl);

Symbol *make_func_declarator(Symbol *name, Symbol *param_list);

Symbol *make_func_definition(Symbol *signature, Symbol *stat);

Symbol *make_func_signature(Symbol *signature);

Symbol *make_func_declaration(Symbol *type, Symbol *signature);

Symbol *make_declaration(Symbol *type, Symbol *list);

Symbol *make_init_list();

Symbol *init_list_push_back(Symbol *list, Symbol *);

Symbol *block_item_cat(Symbol *list, Symbol *block);

Symbol *make_empty_expression_stat();

Assembly *make_assembly();

Assembly *assembly_cat(Assembly *c1, Assembly *c2);

void assembly_to_file(Symbol *code);

void assembly_push_back(Assembly *code, String *piece);

int allocate_stack(Data_type type);

Symbol *get_cur_func();

void unset_cur_func();

void set_cur_func(Symbol *func);

void symtab_append(Symbol *sym);

void info(const char *fmt, ...);

void yyerror(const char *fmt, ...);

#define YYSTYPE Symbol *

#endif //LCC_NG_LCC_H
