#ifndef LCC_NG_LCC_H
#define LCC_NG_LCC_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <zconf.h>
#include "ADT.h"

#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

static char *regular_reg[][4] = {
    {
        "al",
        "ax",
        "eax",
        "rax",
    },
    {
        "bl",
        "bx",
        "ebx",
        "rbx",
    },
    {
        "cl",
        "cx",
        "ecx",
        "rcx",

    }
};

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

typedef enum Attribute {
    type_specifier,             // int
    identifier,                 // a
    parameter_declaration,      // int a
    parameter_list,             // int a, int b
    func_declarator,            // foo(int a, int b)
    function_defination,        // int foo(int a, int b) { return a; }
    function_declaration,       // int foo(int a, int b)

    new_scope,                  // { ... }
    statement,                  // ...

    local_var,                  // int a = 6, b = 5
    temporary,                  // temporary variable
    init_declarator_list,       // a = 6, b = 5

    expression,                 // a + b
} Attribute;

typedef enum Size_type {
    BYTE,
    WORD,
    LONG_WORD,
    QUAD_WORD,
} Size_type;

typedef enum Op_type {
    IMUL,
    IDIV,
    ADD,
    SUB,
    ASSIGN,
} Op_type;

typedef struct Assembly {
    List_node *beg, *end;
} Assembly;

typedef struct Symbol {
    Attribute attr;
    String *name;
    Size_type basic_type;
    int offset, upper_bound;    // upper_bound stores position of adjacency object
    int rsp;
    Assembly *code;
    struct Symbol *parent;
    Vector *param, *init_list;
} Symbol;

typedef enum Status {
    Used,
    Unused,
} Status;

typedef struct Expr_stack {
    Vector *content;
    int eax;
} Expr_stack;

void expr_stack_init();

int expr_stack_size();

static void expr_stack_clear();

void expr_stack_push(Symbol *expr, struct Symbol *);

void expr_stack_pop(Symbol *expr, int idx);

Symbol *expr_stack_top();

Symbol *end_statement(Symbol *assembly);

void signal_extend(Assembly *code, int idx, Size_type original, Size_type new);

void extend(Assembly *code, char *reg[][4], int idx, Size_type original, Size_type new);

Symbol *find_symbol(Symbol *name);

static Symbol *make_symbol();

static void free_symbol(Symbol *sym);

Symbol *make_identifier(char *);

void make_new_scope();

void destroy_scope();

Symbol *make_data_type(Size_type type);

Symbol *make_parameter_declaration(Symbol *type, Symbol *name);

Symbol *make_parameter_list(Symbol *decl);

Symbol *parameter_list_push_back(Symbol *list, Symbol *decl);

Symbol *make_func_declarator(Symbol *name, Symbol *param_list);

Symbol *make_func_def_step2(Symbol *signature, Symbol *stat);

Symbol *make_fun_def_step1(Symbol *signature);

Symbol *make_func_declaration(Symbol *type, Symbol *signature);

Symbol *make_declaration(Symbol *type, Symbol *list);

Symbol *make_expression();

static Symbol *make_expression_with_assembly(Symbol *p1, Symbol *p2);

Symbol *make_op_expression(Symbol *p1, enum Op_type op, Symbol *p2);

static void assign_op(Symbol *expr, Symbol *target);

static void additive_op(Symbol *expr, char *op_prefix);

static void multiplicative_op(Symbol *expr, char *op_prefix);

static void divisional_op(Symbol *expr, char *op_prefix);

Symbol *make_init_list();

Symbol *init_list_push_back(Symbol *list, Symbol *);

Symbol *block_item_cat(Symbol *list, Symbol *block);

Symbol *make_empty_expression_stat();

Assembly *make_assembly();

Assembly *assembly_cat(Assembly *c1, Assembly *c2);

void assembly_to_file(Symbol *code);

void assembly_push_back(Assembly *code, String *piece);

void allocate_stack(Symbol *s);

Symbol *get_cur_func();

void unset_cur_func();

void set_cur_func(Symbol *func);

void symtab_append(Symbol *sym);

void info(const char *fmt, ...);

void yyerror(const char *fmt, ...);

#define YYSTYPE Symbol *

#endif //LCC_NG_LCC_H
