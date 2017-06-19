#ifndef LCC_NG_LCC_H
#define LCC_NG_LCC_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <zconf.h>
#include "ADT.h"


typedef enum Attribute {
    data_type,
    identifier,
    parameter,
    parameter_list,
    func_declarator,
    func_signature,
    function_definition,
    function_declaration,
    new_scope,
    statment,
} Attribute;

typedef enum Data_type {
    int_type,
} Data_type;

typedef struct Assembly {
    List_node *beg, *end;
} Assembly;

typedef struct Symbol {
    Attribute attr;
    String *name;
    Data_type basic_type;
    Assembly *code;
    struct Symbol *parent;
    Vector *param;
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

Symbol *make_declaration(Symbol *type, Symbol *declarator);

Symbol *make_func_declaration(Symbol *type, Symbol *signature);

Symbol *make_empty_expression_stat();

Assembly *make_assembly();

Assembly *assembly_cat(Assembly *c1, Assembly *c2);

void assembly_to_file(Symbol *code);

void assembly_push_back(Assembly *code, String *piece);

void info(const char *fmt, ...);

void yyerror(const char *fmt, ...);

#define YYSTYPE Symbol *

#endif //LCC_NG_LCC_H
